#include<stdlib.h>

int waitpid(int pid, int *st_ptr, int options) {
  return sys_call(__NR_wait4, pid, st_ptr, options, NULL);
}

