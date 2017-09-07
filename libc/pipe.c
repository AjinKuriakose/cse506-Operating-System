#include<stdlib.h>

int pipe(int pipefd[2]) {
  return sys_call(__NR_pipe, pipefd);
}

