#include <sys/defs.h>
#include <sys/vmm.h>
#include <sys/pmm.h>
#include <sys/kprintf.h>

#define PML4_SIZE    512
#define PDP_SIZE     512
#define PD_SIZE      512
#define PT_SIZE      512

#define PAGE_SIZE    4096

#define PAGE_PML4_INDEX(x)  ((x >> 39) & 0x1FF)
#define PAGE_PDP_INDEX(x)   ((x >> 30) & 0x1FF)
#define PAGE_PD_INDEX(x)    ((x >> 21) & 0x1FF)
#define PAGE_PT_INDEX(x)    ((x >> 12) & 0x1FF)
#define PAGE_OFFSET(x)      (x & 0xFFF)

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

void enable_paging(pml4_t *pml4) {

  __asm__ volatile("movq %0, %%cr3":: "r"(pml4));
  uint64_t cr0;
  __asm__ volatile("mov %%cr0, %0": "=r"(cr0));
  cr0 |= 0x80000000; /* Enable paging */
  __asm__ volatile("mov %0, %%cr0":: "r"(cr0));
}

void init_paging(uint64_t physbase, uint64_t physfree) {

  pml4_t  *pml4; 
  pdp_t   *pdp; 
  pd_t    *pd; 
  pt_t    *pt; 

  uint64_t v_addr = (uint64_t)&kernmem;
  uint64_t p_free = physfree;
  uint64_t p_base = physbase;

  uint64_t pml4_index = PAGE_PML4_INDEX(v_addr);
  uint64_t pdp_index  = PAGE_PDP_INDEX(v_addr);
  uint64_t pd_index   = PAGE_PD_INDEX(v_addr);

  /* Allocate memory for pml4 table */
  pml4 = (pml4_t *)alloc_block();

  /* Allocate and insert pdp table entry in pml4 table */
  pdp = (pdp_t *)alloc_block();
  pdp = (pdp_t *)((uint64_t)pdp | (PTE_PRESENT | PTE_WRITABLE | PTE_USER));
  pml4->pml4_entries[pml4_index] = (uint64_t)pdp;
  
  /* Allocate and insert page directory entry in pdp table */
  pd = (pd_t *)alloc_block();
  pd = (pd_t *)((uint64_t)pd | (PTE_PRESENT | PTE_WRITABLE | PTE_USER));
  pdp->pdp_entries[pdp_index] = (uint64_t)pd;

  /* Allocate and insert page table entry in page directory */
  pt = (pt_t *)alloc_block();
  pt = (pt_t *)((uint64_t)pt | (PTE_PRESENT | PTE_WRITABLE | PTE_USER));
  pd->pd_entries[pd_index] = (uint64_t)pt;

  for (; p_base < p_free; p_base += 4096, v_addr += 4096) {

    uint64_t pt_index = PAGE_PT_INDEX(v_addr);
    uint64_t entry = p_base;
    entry |= (PTE_PRESENT | PTE_WRITABLE | PTE_USER);
    pt->pt_entries[pt_index] = entry;
  }

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
