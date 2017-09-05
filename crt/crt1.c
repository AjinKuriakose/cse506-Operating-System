#include <stdlib.h>

void _start(void) {
  main(1, 0,0 );
  // call main() and exit() here
/*asm("movl $1,%eax;"
        "xorl %ebx,%ebx;"
        "int  $0x80"
    );*/
//exit (ret);

}
