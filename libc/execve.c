#include<stdlib.h>
#include<unistd.h>

//int execve(const char *filename, char* argv[], char *const envp[]) {
int execve(const char *filename, char (*argv)[64], char *const envp[]) {
  return syscall(__NR_execve, filename, argv, envp);
}

