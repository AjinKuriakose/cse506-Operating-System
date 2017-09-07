#include <stdlib.h>
#if 0
void _start(void) {
  // call main() and exit() here
  __asm__(
    "xorl %ebp, %ebp;"

     "popq %rdi;"
     "popq %rdi;"
	//keeping the same value in argv & env.
	//would need to do env[argc+1] inside main
	//replace rdx later with proper value
      "movq %rsp, %rsi ;"
      "movq %rsp, %rdx;"
//    "add %rdx,%rdi;"


    "call main;"

    /*exit*/

    "movl $1, %eax;"
    "xorl %ebx, %ebx;"
    "int $0x80"
  );
}
#endif
void _start(void) {
  // call main() and exit() here
  __asm__(
    "xorl %ebp, %ebp;"
//    "movq %rdx, %r9;"
    //"popq %rsi;"
    "popq %rdi;"
    "movq %rsp, %rsi;"
    "movq %rsp, %rdx;"

//    "andq $~15, %rsp;"

    "pushq %rax;"
    "pushq %rsp;"
//    "movq (main), %rdi;"
    "call main;"

    "movl $1, %eax;"
    "xorl %ebx, %ebx;"
    "int $0x80"
  );
}

