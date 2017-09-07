#include<stdlib.h>

pid_t fork() {
  return sys_call(__NR_fork);
}
