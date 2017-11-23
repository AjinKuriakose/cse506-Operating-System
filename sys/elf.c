#include <sys/elf64.h>
#include <sys/task.h>
#include <sys/tarfs.h>
#include <sys/kprintf.h>

int load_binary(task_struct_t *task, char *bin_filename) {

  Elf64_Ehdr *elf_header = get_elf_header(bin_filename);
  //Elf64_Phdr *prog_header = (Elf64_Phdr *)((char *)elf_header + elf_header->e_phoff);

  /* TODO : Remove these prints after development */
#if 0
  if (elf_header) {
    kprintf("ELF.e_type = %d\n", elf_header->e_type);
    kprintf("ELF.e_machine = %d\n", elf_header->e_machine);
    kprintf("ELF.e_version = %d\n", elf_header->e_version);
    kprintf("ELF.e_entry = %d\n", elf_header->e_entry);
    kprintf("ELF.e_phoff = %d\n", elf_header->e_phoff);
    kprintf("ELF.e_shoff = %d\n", elf_header->e_shoff);
    kprintf("ELF.e_flags = %d\n", elf_header->e_flags);
    kprintf("ELF.e_ehsize = %d\n", elf_header->e_ehsize);
    kprintf("ELF.e_phentsize = %d\n", elf_header->e_phentsize);
    kprintf("ELF.e_phnum = %d\n", elf_header->e_phnum);
    kprintf("ELF.e_shentsize = %d\n", elf_header->e_shentsize);
    kprintf("ELF.e_shnum = %d\n", elf_header->e_shnum);
    kprintf("ELF.e_shstrndx = %d\n", elf_header->e_shstrndx);
  }
#endif

  mm_struct_t *mm = task->mm;
  mm->mmap = NULL;

  uint16_t e_phnum = elf_header->e_phnum;
  while (e_phnum) {



    e_phnum--;
  }


  task->rip = elf_header->e_entry;

  return 0;
}
