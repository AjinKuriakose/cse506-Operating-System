#include <sys/task.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/vmm.h>
#include <sys/gdt.h>
 
static Task *runningTask;
static Task mainTask;

static Task task1;
static Task task2;
/*
 * just for testing 
 * TODO:not required. just to verify switching print statement
 */
void Sleep() {
    volatile int spin = 0;
      while (spin < 50000000) {
            spin++;
              }
}

void dummy() {
  kprintf("Inside Dummy....\n");

  //__asm__ volatile("pushq %rbx");   // OK (Normal operation)
  //__asm__ volatile("sti");          // NOK - Reboot (Privileged operation)
  //__asm__ volatile("hlt");          // NOK - Reboot (Privileged operation)
  while(1);
}
 
void switch_to_user_mode() {

  uint64_t cs = get_user_cs() | 0x3;
  uint64_t ds = get_user_ds() | 0x3;

  kprintf("cs : [%p]\n", cs);
  kprintf("ds : [%p]\n", ds);
  kprintf("get_user_cs : [%p], get_user_ds : [%p]\n", get_user_cs(), get_user_ds());
  switchring3(dummy, cs, ds);
}

void task1Main() {
    while(1) {
        kprintf("Thread ####1\n");
        Sleep();
        set_tss_rsp((void *)task1.ctx.rsp);
				yield();
    }
}
 
void task2Main() {
    while(1) {
        kprintf("Thread ####2\n");
        Sleep();
        set_tss_rsp((void *)task1.ctx.rsp);
        switch_to_user_mode();
//        while(1);
//				yield();
    }
}
/*
 * init tasking.. creating two tasks. task1 & task2
 * initializing the tasks as follows.
 * maintask ---> task1 <------> task2
 * task1 & task2 are in a cycle.
 * TODO: not pusing cr3 & EFLAGS now. may need later.
 */
void initTasking() {
    // Get EFLAGS and CR3
    //__asm__ volatile("mov %%cr3, %%eax; mov %%eax, %0;":"=m"(mainTask.ctx.cr3)::"%eax");
    // __asm__ volatile("mov %%cr3, %0": "=r"(mainTask.ctx.cr3));
    //__asm__ volatile("pushfl; movq (%%esp), %%eax; movq %%eax, %0; popfl;":"=m"(mainTask.ctx.eflags)::"%eax");
 
    //createTask(&task1, task1Main, mainTask.ctx.eflags, (uint64_t*)mainTask.ctx.cr3);
    createTaskNw(&task1, task1Main);
    createTaskNw(&task2, task2Main);

    mainTask.next = &task1;
    task1.next = &task2;
    task2.next = &task1;
 
    runningTask = &mainTask;
}
 
void createTaskNw(Task *task, void (*main)()) {
    task->ctx.rsp = (uint64_t) &(task->kstack[4016]);;
    /* placing main's address, func pointer in the stack
     * towards the end. kstack is a char array, in order to 
     * save a 64 bit address in the stack, first creating a
     * uint64_t pointer to kstack. Then saving main's address.
     */
    uint64_t *tmp_ptr = (uint64_t *)&(task->kstack[4088]);
    *tmp_ptr = (uint64_t) main;
}
 
void yield() {
    Task *last = runningTask;
    runningTask = runningTask->next;
    switchTask(last, runningTask);
}
/*
 * call from main.c comes here in doIt
 * This can be moved to main.c itself
 * TODO: should it be in main???
 */
void doIt() {
    yield();
}

