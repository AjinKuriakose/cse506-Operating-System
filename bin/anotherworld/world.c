#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {

#if 1
  char *argvv[] = {0};
  int ret = fork();
  if(ret == 0) {
#endif

    write(1, "CLD", 3);
#if 1
    execvpe("bin/ps", &argvv[0], 0);
  }
  else {
    write(1, "PAR1", 4);
    int st = 0;
    wait(&st);
    write(1, "PAR2", 4);
  }
#endif

  printf("RET = %d\n", ret);
  exit(0);
}

