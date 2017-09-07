#include<stdlib.h>

int open(const char *pathname, int flags) {
  return sys_call(__NR_open, pathname, flags);
}

