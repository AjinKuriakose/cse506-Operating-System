#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/pci.h>
#include <tcl/tcl.h>
#include <sys/pmm.h>
#include <sys/vmm.h>
#include <sys/task.h>
#include <sys/utils.h>
#include <sys/terminal.h>

#define INITIAL_STACK_SIZE 4096
#define ASCII_TO_NUM(num) (num - 48)

uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
phys_block_t phys_blocks[MAX_NUM_PHYS_BLOCKS];

uint32_t* loader_stack;
extern char kernmem, physbase;

void tcltest() {

  Tcl_Interp *myinterp;
  char *cmd = "puts \"Hello World from Tcl!\"";

  kprintf ("Invoking Tcl interpreter ... \n");
  myinterp = Tcl_CreateInterp();
  Tcl_Eval(myinterp, cmd, 0, NULL);
}

/* TODO : This is for POC purpose. Need to move out and/or change later */
uint64_t get_size_tar_octal(char *data, size_t size) {
  char *curr = (char *)data + size;
  char *ptr = curr;
  uint64_t sum = 0;
  uint64_t multiply = 1;

  while (ptr >= (char *) data) {
    if ((*ptr) == 0 || (*ptr) == ' ') {
      curr = ptr - 1;
    }
    ptr--;
  }

  while (curr >= (char *) data) {
    sum += ASCII_TO_NUM(*curr) * multiply;
    multiply *= 8;
    curr--;
  }

  return sum;
}

/* TODO : This is for POC purpose. Need to move out and/or change later */
void browse_tarfs() {
  uint64_t align_512  = 0;
  uint64_t total_size = 0;
  struct posix_header_ustar *hdr = (struct posix_header_ustar *)&_binary_tarfs_start;

  while ((char *)hdr < &_binary_tarfs_end) {
    uint64_t file_size = get_size_tar_octal(hdr->size, 12);
    uint64_t pad_size  = get_size_tar_octal(hdr->pad, 12);

    total_size = sizeof(*hdr) + file_size + pad_size;

    /* Skipping the entries with empty names. TODO: find out why they are present (Piazza 429) */
    if (strlen(hdr->name))
      kprintf("tarfs content : name = %s, size = %d\n", hdr->name, file_size);

    if (total_size % 512)
      align_512 = 512 - (total_size % 512);
    else
      align_512 = 0;
      
    hdr = (struct posix_header_ustar *)((char *)hdr + total_size + align_512);
  }
}

void start(uint32_t *modulep, void *physbase, void *physfree)
{
  init_pmm(modulep, physbase, physfree);
  init_paging(0, (uint64_t)physfree);

  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("physbase %p\n", (uint64_t)physbase);
  kprintf("tarfs in [%p - %p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
  
  browse_tarfs();

  init_idt();
  pic_offset_init(0x20,0x28);
  __asm__ volatile (
    "cli;"
    "sti;"
  );

#if 0
  tcltest();
  checkAllBuses();  
#endif

  init_terminal();

  /* TODO : context switching.. needs renaming */
  initTasking();
	doIt();

  while(1) __asm__ volatile ("hlt");
}

void boot(void)
{
  /* Note: function changes rsp, local stack variables can't be practically used */
  register char *temp1, *temp2;

  for(temp1=(char *)VIDEO_VIRT_MEM_BEGIN, temp2 = (char*)(VIDEO_VIRT_MEM_BEGIN + 1); temp2 < (char*)VIDEO_VIRT_MEM_BEGIN+160*25; temp2 += 2, temp1 += 2) {
	*temp2 = 7 /* white */;
	*temp1 =' ';
  }

  /* Seperation Indication */ 
  for(temp1=(char *)VIDEO_VIRT_MEM_BEGIN+160*17; temp1 < (char*)VIDEO_VIRT_MEM_BEGIN+160*18; temp1 += 2) {
	*temp1 ='=';
  }

  __asm__ volatile (
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  init_gdt();
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)VIDEO_VIRT_MEM_BEGIN;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1) __asm__ volatile ("hlt");
}

