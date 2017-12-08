#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[]) {
  char buff[128] = {0};
  getcwd(buff, 127);

  buff[strlen(buff)] = '\n';
  write(1, buff, strlen(buff));

	exit(0);
}

