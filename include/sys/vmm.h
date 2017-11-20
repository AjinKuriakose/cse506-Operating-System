#ifndef _VMM_H_
#define _VMM_H_

#define VIRT_ADDR_BASE      0xFFFFFFFF00000000
#define VIRT_PAGE_SIZE      4096

#define PML4_SIZE    512
#define PDP_SIZE     512
#define PD_SIZE      512
#define PT_SIZE      512


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

void init_paging(uint64_t physbase, uint64_t physfree);
void setup_four_level_paging(uint64_t physbase, uint64_t physfree);
void remap_kernel(pt_t *pt, uint64_t p_base, uint64_t p_free);

uint64_t vmm_alloc_page();
void vmm_dealloc_page(uint64_t v_addr);
void identity_map(uint64_t);
#endif

