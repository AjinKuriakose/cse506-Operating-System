#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void handle_cat(int num, char *files[]) {
  int i = 0;
  while (i < num) {
    int file = open(files[i], O_RDONLY);
    char code[1024] = {0};
    size_t n = 0;
    char c;

    if (file == -1)
      return;

    while (read(file, &c, 1) > 0)
    {
      code[n++] = (char) c;
      if (c == '\n') {
        code[n - 1] = '\0';
        puts(code);
        n = 0;
      }
    }

    close(file);
    i++;
  }
}

int main(int argc, char *argv[], char *envp[]) {

  if (argc > 1)
    handle_cat(argc - 1, &argv[1]);

  return 0;
}

