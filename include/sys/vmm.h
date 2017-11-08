#ifndef _VMM_H_
#define _VMM_H_

#define VIRT_ADDR_BASE      0xFFFFFFFF00000000
#define VIRT_PAGE_SIZE      4096

void init_paging(uint64_t physbase, uint64_t physfree);

uint64_t vmm_alloc_page();
void vmm_dealloc_page(uint64_t v_addr);






#endif

