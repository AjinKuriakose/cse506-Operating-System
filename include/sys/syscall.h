#ifndef __SYSCALL_H__
#define __SYSCALL_H__
#include <sys/defs.h> 
 
void init_syscall();

void sys_ps();
void sys_getpid();
void sys_getppid();

#endif /* __TASK_H__ */
