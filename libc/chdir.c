#include<stdlib.h>

int chdir(const char *path) {
  return syscall(__NR_chdir, path);
}
