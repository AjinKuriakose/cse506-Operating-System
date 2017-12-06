#include<unistd.h>
long syscall(int syscall_number, ...) {
  long ret;
  __asm__(
  "mov    %%rdi,%%rax;"
  "mov    %%rsi,%%rdi;"
  "mov    %%rdx,%%rsi;"
  "mov    %%rcx,%%rdx;"
  "mov    %%r8,%%r10;"
  "mov    %%r9,%%r8;"
  "mov    0x8(%%rsp),%%r9;"
  "syscall;"
  "cmp    $0xfffffffffffff001,%%rax;"
  :"=r"(ret)
  );

  return ret;
}


int exit() {
  return syscall(60);
}
int main() {

  char ch[10] = "helloworld";
  char *a = "m";
   write(1, ch, 10);

   int ret = fork();
   if(ret == 0) {
//	execve("bin/anotherworld", 0,0);	
    }
   else {
   write(1, ch, 10);
   }

   write(1, a, 1);
   write(1, a, 1);
   write(1, a, 1);
   write(1, a, 1);
   write(1, a, 1);
   write(1, a, 1);
    while(1);
	
  //exit();
  return 0;
}
