#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[]) {
        write(1, argv[0], strlen(argv[0]));
	ps();
	exit(0);
}

