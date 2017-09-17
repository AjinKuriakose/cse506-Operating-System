#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>

#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;

void start(uint32_t *modulep, void *physbase, void *physfree)
{
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
  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
}

void testfn(){
 __asm__(
    "int $0x20"
  );
}


void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp1, *temp2;

  for(temp1 = (char*)0xb8000, temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2, temp1 += 2) {
    *temp2 = 7 /* white */;
    *temp1 = ' ';
  }
/*
  __asm__(
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
*/
  init_gdt();
  init_idt();

  kprintf("interrupt!");
  testfn();

/*
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
*/

  /*
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  */
/*
  int i = 1;
  int a = 2;
  int b = 2;
  int c = 2;
  kprintf("a = %d, b = %d, c = %d\n", a, b, c);
  kprintf("addr a = %p\n", &i);
  while (i < 27) {
    kprintf("This is line %d\n", i);
    i++;
  }
	
  kprintf("Name : AMD\n");
  kprintf("Name : AMD\n");
  kprintf("This is a very long line.This is a very long line.This is a very long line.67890123\n");
  kprintf("This is a very long line.This is a very long line.This is a very long line.67890123");
  kprintf("%x\n", i);
  kprintf("%x\n", i);
  kprintf("%s\n", "arjun");
  kprintf("%p\n", &i);
  kprintf("%c, %c, %c\n", 65, 66, 67);
  kprintf("%s, %s, %s\n", "abc", "def", "ghi");
  kprintf("%d, %d, %d\n", 7, 8, 9);
  kprintf("%x, %x, %x\n", 25, 26, 27);
*/
  while(1);
}
