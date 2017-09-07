#include<stdlib.h>

char *getcwd(char *buf, size_t size) {
  return (char *)sys_call(__NR_getcwd, buf, size);
}

