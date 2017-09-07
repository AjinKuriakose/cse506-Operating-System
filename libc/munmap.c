#include<stdlib.h>

int munmap(void *addr, size_t length) {
  return sys_call(__NR_munmap, addr, length);
}

