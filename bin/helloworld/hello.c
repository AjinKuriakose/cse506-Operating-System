int global_var1[100000];
int global_var2 = 2;

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

int main() {

  char *buff = "manu";
  int a[10];
  a[0] = 4;

  write(1, buff, a[0]);

  return 0;
}
