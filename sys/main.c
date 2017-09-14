#include <sys/defs.h>
#include <sys/gdt.h>
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

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp1, *temp2;

  for(temp1 = (char*)0xb8000, temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2, temp1 += 2) {
    *temp2 = 7 /* white */;
    *temp1 = ' ';
  }
  __asm__(
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

  /*
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  */

  //kprintf("this is a line printed by amd\nthis is another line\nanother one");
  kprintf("this is a line printed by amd\n"
          "this is a line printed by amd\n");
  //kprintf("this is a line printed by amd,this is a line printed by amd,this is a line printed by amd\n");
  //kprintf("\nthis is a line printed by amd,this is a line printed by amd,this is a line printed by amd");

  /*
  char *a = "Arjun Mathew Dan";
  char *s = a;
  while (*s) {
    kprintf("The char is : %c\n", s);
    s++;
  }
  */

  char c = 'A';
  kprintf("The char is : %c\n", c); 
  c = 'M';
  kprintf("The char is : %c\n", c); 
  kprintf("The char is : %c\n", 'D');

  char *st = "This is Awesome!";
  kprintf("Line : %s\n\n\n", st);
  kprintf("%d\n", 25);
  kprintf("%p\n", 25);
  //kprintf("%p\n", 0x7fffd54becf4);
  int gg = 2;
  kprintf("Addr : %p\n", &gg);
  kprintf("%x\n", 6753429);
 // kprintf("%c", 'F');
  //kprintf("%d", 1234);
  while(1);
}
