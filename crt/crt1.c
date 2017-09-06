#include <stdlib.h>

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
