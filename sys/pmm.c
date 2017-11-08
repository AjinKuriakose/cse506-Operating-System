#include <sys/defs.h>
#include <sys/pmm.h>
#include <sys/kprintf.h>

phys_block_t *free_list = NULL;
phys_block_t *used_list = NULL;
phys_block_t *free_list_tail = NULL; /* Connect e820 returned phys mem chunks */

void print_list(phys_block_t *list) {
  int i = 0;
  while(list) {
    i++;
    list = list->next;
  }
  kprintf("i = %d\n", i);
}

uint64_t pmm_alloc_block() {

  uint32_t block_index; 

  phys_block_t *free_block = free_list;
  if (!free_block) {
    return -1;
  }

  free_list = free_list->next;
  free_block->next = used_list;
  free_block->used = 1;
  used_list = free_block;

  /* Getting the index of free block using array base address */
  block_index = free_block - phys_blocks;
  return (uint64_t)(block_index * PHYS_BLOCK_SIZE);
}

void pmm_dealloc_block(uint64_t phys_addr) {

  uint32_t index = phys_addr / PHYS_BLOCK_SIZE;
  
  if (used_list == &phys_blocks[index]) {
    used_list = used_list->next;
    phys_blocks[index].next = free_list;
    phys_blocks[index].used = 0;
    free_list = &phys_blocks[index];

  } else {

    phys_block_t *tmp = used_list;
    while (tmp->next != &phys_blocks[index]) {
      tmp = tmp->next;
    }

    tmp->next = phys_blocks[index].next;
    phys_blocks[index].next = free_list;
    phys_blocks[index].used = 0;
    free_list = &phys_blocks[index];
  }
}

/*
 * update phys_blocks[] using start_addr to end_addr.
 * update freelist for memory chunks reported e820
 * http://wiki.osdev.org/Page_Frame_Allocation Hybrid scheme 2
 */
void update_phys_blocks(uint64_t start_addr, uint64_t end_addr) {

  uint32_t i = 0;
  uint32_t start_index = start_addr / PHYS_BLOCK_SIZE;
  uint32_t end_index = end_addr / PHYS_BLOCK_SIZE;
  if (end_addr && !(end_addr % PHYS_BLOCK_SIZE))
    end_index--;

  /* while connecting chunks, tail is updated to the first block 
   * Not applicable for the very first chunk
   */
  if (free_list_tail) {
    free_list_tail->next = &phys_blocks[start_index];
  }

  i = start_index;
  while (i < end_index) {

    phys_blocks[i].next = &phys_blocks[i + 1];

    /* set head pointer */
    if (!free_list) {
      free_list = &phys_blocks[i];
    }
    i++;
  }
  
  free_list_tail = &phys_blocks[i];

}

/*
 * marking blocks from physbase to physfree as used since 
 * kernel & related data reside there
 */
void mark_kernel_blocks(void *physbase, void *physfree) {
  
  uint32_t i = 0;
  uint32_t start_index = (uint64_t)physbase / PHYS_BLOCK_SIZE;
  uint32_t end_index = (uint64_t)physfree / PHYS_BLOCK_SIZE;
  if ((uint64_t)physfree && !((uint64_t)physfree % PHYS_BLOCK_SIZE))
    end_index--;

  while (phys_blocks[i].next != &phys_blocks[start_index]) {
    i++;
  }

  phys_blocks[i].next = phys_blocks[end_index].next;
  phys_blocks[end_index].next = NULL;
  used_list = &phys_blocks[start_index];
}

/*
 * initialize physical memory manager.
 */
void init_pmm(uint32_t *modulep, void *physbase, void *physfree) {

  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;

  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  int i =0;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
		  i++;	
      kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
      update_phys_blocks(smap->base, smap->base + smap->length);
    }
  }

  mark_kernel_blocks(physbase, physfree); 
}

