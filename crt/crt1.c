#include <stdlib.h>

void _start(void) {
  // call main() and exit() here
  __asm__(
    "xorl %ebp, %ebp;"
    "movq %rdx, %r9;"
    "popq %rsi;"
    "movq %rsp, %rdx;"
    "andq $~15, %rsp;"
    "pushq %rax;"
    "pushq %rsp;"
    "movq (main), %rdi;"
    "call main;"
    "movl $1, %eax;"
    "xorl %ebx, %ebx;"
    "int $0x80"
  );
}
