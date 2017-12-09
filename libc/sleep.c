#include <stdlib.h>
#include <unistd.h>

int sleep(int time) {
  return syscall(__NR_sleep, time);
}

