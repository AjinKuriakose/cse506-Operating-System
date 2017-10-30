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

#define INITIAL_STACK_SIZE 4096

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

void start(uint32_t *modulep, void *physbase, void *physfree)
{
  
  init_pmm(modulep, physbase, physfree);

#if 0 
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
    }
  }
#endif

  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("physbase %p\n", (uint64_t)physbase);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
  
  init_idt();
  pic_offset_init(0x20,0x28);
  __asm__ volatile (
    "cli;"
    "sti;"
  );

  tcltest();
  checkAllBuses();  

  while(1) __asm__ volatile ("hlt");
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp1, *temp2;

  for(temp1=(char *)0xb8000, temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2, temp1 += 2) {
	*temp2 = 7 /* white */;
	*temp1 =' ';
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
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1) __asm__ volatile ("hlt");
}

