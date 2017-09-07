#include<stdlib.h>

void exit(int status) {
    sys_call(__NR_exit, status);
}
