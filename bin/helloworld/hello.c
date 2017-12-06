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

int write(int fd, const void *c, int size) {
  return syscall(1, fd, c, size);
}

int exit() {
  return syscall(60);
}
int main() {

  char ch[10] = "helloworld";
  char ab[7] = "manu999";
  //int i = 0;  
  //while(i<5) {
   write(1, ch, 10);
	int i, b;
	i = 7;
	b =7;
   write(1, ab, i+b-b);
//	while(1);
//    write(1, ch, 10);
 //   exit();
    while(1);
	
   // i++;
 // }
  //exit();
  return 0;
}
