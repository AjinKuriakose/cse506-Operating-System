#include<stdlib.h>

ssize_t read(int fd, void *c, size_t size) {
  return sys_call(__NR_read, fd, c, size);
}
