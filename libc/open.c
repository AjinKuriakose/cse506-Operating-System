#include<stdlib.h>

int open(const char *pathname, int flags) {
  return syscall(__NR_open, pathname, flags);
}

