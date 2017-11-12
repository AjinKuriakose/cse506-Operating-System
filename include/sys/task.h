#ifndef __TASK_H__
#define __TASK_H__
#include <sys/defs.h> 
 
extern void initTasking();
/*
 * not using these members except rsp as we are pushing registers.
 * TODO: not using these register members now.
 */ 
typedef struct {
      uint64_t rsp;
      uint64_t r15;
      uint64_t r14;
      uint64_t r13;
      uint64_t r12;
      uint64_t rbx;
      uint64_t rbp;
} context;

/*
 * following is the PCB.
 * should add requied parameters like pid etc.
 *
 */
typedef struct Task {
      context ctx;
      char kstack[4096];
      struct Task *next;
} Task;
 
extern void initTasking();
//extern void createTask(Task*, void(*)(), uint64_t, uint64_t*);
extern void createTaskNw(Task*, void(*)());
 
extern void yield(); 
extern void switchTask(Task *old, Task *new); 
//void switchTask(Task *from, Task *to); 
void doIt();
#endif /* __TASK_H__ */
