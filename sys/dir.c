#include <sys/dirent.h>
#include <sys/syscall.h>
#include <sys/task.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/vmm.h>
#include <sys/gdt.h>
#include <sys/utils.h>
#include <sys/terminal.h>

DIR *opendir(const char *name) {

  DIR *dir = NULL;

  return dir;
}

struct dirent *readdir(DIR *dirp) {

  dirent *d_ent = NULL;

  return d_ent;
}

int closedir(DIR *dirp) {

  return 0;
}

