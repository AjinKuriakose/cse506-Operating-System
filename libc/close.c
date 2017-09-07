#include<stdlib.h>

int close(int fd) {
  return sys_call(__NR_close, fd);
}

