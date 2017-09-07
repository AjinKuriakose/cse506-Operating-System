#include<stdlib.h>

int chdir(const char *path) {
  return sys_call(__NR_chdir, path);
}
