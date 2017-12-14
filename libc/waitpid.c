#include <stdlib.h>
#include <unistd.h>

int waitpid(int pid, int *status) {
  return syscall(__NR_wait4, pid, status);
}

