#include <sys/defs.h>
#include <sys/vmm.h>
#include <sys/pmm.h>
#include <sys/kprintf.h>

#define PML4_SIZE    512
#define PDP_SIZE     512
#define PD_SIZE      512
#define PT_SIZE      512

#define PAGE_SIZE    4096

#define PAGE_PML4_INDEX(x)            ((x >> 39) & 0x1FF)
#define PAGE_PDP_INDEX(x)             ((x >> 30) & 0x1FF)
#define PAGE_PD_INDEX(x)              ((x >> 21) & 0x1FF)
#define PAGE_PT_INDEX(x)              ((x >> 12) & 0x1FF)
#define PAGE_OFFSET(x)                ( x &  0xFFF)
#define PAGE_GET_PHYSICAL_ADDRESS(x)  (*x & ~0xFFF)

#define PTE_PRESENT   0x1
#define PTE_WRITABLE  0x2
#define PTE_USER      0x4

extern char kernmem;

typedef struct pml4_t {
    uint64_t pml4_entries[PML4_SIZE];
} pml4_t;

typedef struct pdp_t {
    uint64_t pdp_entries[PDP_SIZE];
} pdp_t;

typedef struct pd_t {
    uint64_t pd_entries[PD_SIZE];
} pd_t;

typedef struct pt_t {
    uint64_t pt_entries[PT_SIZE];
} pt_t;

uint64_t current_cr3;
pml4_t  *pml4; 

void enable_paging(pml4_t *pml4) {

  __asm__ volatile("movq %0, %%cr3":: "r"(pml4));
  uint64_t cr0;
  __asm__ volatile("mov %%cr0, %0": "=r"(cr0));
  cr0 |= 0x80000000; /* Enable paging */
  __asm__ volatile("mov %0, %%cr0":: "r"(cr0));
}

void page_fault_handler() {

  kprintf("Inside page_fault_handler\n");
}

void virt_phys_map(uint64_t v_addr, uint64_t p_addr) {

  pdp_t   *pdp; 
  pd_t    *pd; 
  pt_t    *pt; 

  uint64_t  pdp_addr; 
  uint64_t  pd_addr; 
  uint64_t  pt_addr; 
  uint64_t  set_flags_PWU = (PTE_PRESENT | PTE_WRITABLE | PTE_USER);

  /* Get pml4 entry using virtual address. If not present, create */
  pdp_addr = pml4->pml4_entries[PAGE_PML4_INDEX(v_addr)];
  if (pdp_addr & PTE_PRESENT)
    pdp = (pdp_t *)PAGE_GET_PHYSICAL_ADDRESS(&pdp_addr);
  else
    pdp = (pdp_t *)alloc_block();
    pml4->pml4_entries[PAGE_PML4_INDEX(v_addr)] = ((uint64_t)pdp | set_flags_PWU);

  /* Get pdp entry using virtual address. If not present, create */
  pd_addr = pdp->pdp_entries[PAGE_PDP_INDEX(v_addr)];
  if (pd_addr & PTE_PRESENT)
    pd = (pd_t *)PAGE_GET_PHYSICAL_ADDRESS(&pd_addr);
  else
    pd = (pd_t *)alloc_block();
    pdp->pdp_entries[PAGE_PDP_INDEX(v_addr)] = ((uint64_t)pd | set_flags_PWU);
   
  /* Get pd entry using virtual address. If not present, create */
  pt_addr = pd->pd_entries[PAGE_PD_INDEX(v_addr)];
  if (pt_addr & PTE_PRESENT)
    pt = (pt_t *)PAGE_GET_PHYSICAL_ADDRESS(&pt_addr);
  else
    pt = (pt_t *)alloc_block();
    pd->pd_entries[PAGE_PD_INDEX(v_addr)] = ((uint64_t)pt | set_flags_PWU);

  /* Update pt entry with the provided physical address */
  pt->pt_entries[PAGE_PT_INDEX(v_addr)] = p_addr | set_flags_PWU; 
}

void init_paging(uint64_t physbase, uint64_t physfree) {

  pdp_t   *pdp; 
  pd_t    *pd; 
  pt_t    *pt; 

  uint64_t  p_base = physbase;
  uint64_t  p_free = physfree;
  uint64_t  v_addr = (uint64_t)&kernmem;
  uint64_t  set_flags_PWU = (PTE_PRESENT | PTE_WRITABLE | PTE_USER);

  /* Allocate memory for pml4 table */
  pml4 = (pml4_t *)alloc_block();

  /* Allocate and insert pdp table entry in pml4 table */
  pdp = (pdp_t *)alloc_block();
  pml4->pml4_entries[PAGE_PML4_INDEX(v_addr)] = ((uint64_t)pdp | set_flags_PWU);
  
  /* Allocate and insert page directory entry in pdp table */
  pd = (pd_t *)alloc_block();
  pdp->pdp_entries[PAGE_PDP_INDEX(v_addr)] = ((uint64_t)pd | set_flags_PWU);

  /* Allocate and insert page table entry in page directory */
  pt = (pt_t *)alloc_block();
  pd->pd_entries[PAGE_PD_INDEX(v_addr)] = ((uint64_t)pt | set_flags_PWU);

  while (p_base < p_free) {
    pt->pt_entries[PAGE_PT_INDEX(v_addr)] = p_base | set_flags_PWU;
    p_base += 4096;
    v_addr += 4096;
  }

  virt_phys_map(VIDEO_VIRT_MEM_BEGIN, VIDEO_PHYS_MEM_BEGIN);

  enable_paging(pml4);
}

#if 0
void vmm_alloc_page(uint64_t pt_entry) {

  /* allocate a physical memory block */
  uint64_t block = alloc_block();
  //if(!block)

  /* map this block to a page */


}

void vmm_dealloc_page(uint64_t entry) {

  dealloc_block();

}
#endif
