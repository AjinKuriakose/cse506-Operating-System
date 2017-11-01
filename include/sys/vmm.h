#ifndef _VMM_H_
#define _VMM_H_


void init_paging(uint64_t physbase, uint64_t physfree);

void vmm_alloc_page(uint64_t entry);
void vmm_dealloc_page(uint64_t entry);






#endif

