#ifndef __SYSCALL_H__
#define __SYSCALL_H__
#include <sys/defs.h> 
 
void init_syscall();

void sys_ps();
uint64_t sys_getpid();
uint64_t sys_getppid();

#endif /* __TASK_H__ */
