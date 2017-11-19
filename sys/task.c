#include <sys/task.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/vmm.h>
#include <sys/gdt.h>
 
//TODO: all syscall details should be moved to syscall.c
//including init_syscall. This file should be handling only
//task related stuff.

#define MSR_LSTAR   0xc0000082 /* long mode SYSCALL target */
#define MSR_STAR    0xc0000081 /* legacy mode SYSCALL target */


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
/*
 * TODO: not saving rsp now.
 * save if required later.
 */

void kernel_testfun() {
  uint64_t rcx; 
  __asm__ volatile("mov %%rcx, %0" :"=a"(rcx));

  kprintf("hai\n");

  __asm__ volatile("mov %0, %%rcx" ::"a"(rcx));
  __asm__ volatile("sysretq"); 

  while(1);
}

void ring3func() {

  kprintf("Inside user land function Dummy....\n");
  __asm__ volatile("syscall");
  kprintf("Dummy\n");

  while(1);
}

uint32_t get_high_dword(uint64_t qword) {
  return (uint32_t)(qword >> 32);
}

uint32_t get_low_dword(uint64_t qword) {
  return (uint32_t)qword;
}

/*
 * has to be saved seperately high & low part.
 * references: 
 * https://github.com/torvalds/linux/blob/ead751507de86d90fa250431e9990a8b881f713c/arch/x86/include/asm/msr.h
 */
static inline void __wrmsr(unsigned int msr, uint32_t low, uint32_t high) {
 __asm__ __volatile__("wrmsr" :: "a"(low), "d"(high), "c"(msr));

}

// setting the 0th(SCE) bit of IA32_EFER to enable syscall instruction.
static inline void enable_syscall_instr() {
	__asm__ __volatile__("xor %rcx, %rcx; \
											 mov $0xC0000080, %rcx; \
												rdmsr; \
											 or $0x1, %rax; \
											wrmsr");
}

/*
 * setting lstar with the syscall_handler address.
 * setting star with required values.
 *
 * http://wiki.osdev.org/Sysenter#AMD:_SYSCALL.2FSYSRET
 * http://www.felixcloutier.com/x86/SYSCALL.html
 * https://xem.github.io/minix86/manual/intel-x86-and-64-manual-vol3/o_fe12b1e2a880e0ce-174.html
 *
 */
void init_syscall1() {

  uint64_t star_reg_value = ((uint64_t)0x1b << 48) | ((uint64_t)0x8 << 32);

  __wrmsr(MSR_LSTAR, get_low_dword((uint64_t)kernel_testfun), get_high_dword((uint64_t)kernel_testfun)); 
  __wrmsr(MSR_STAR, get_low_dword(star_reg_value), get_high_dword(star_reg_value)); 

	enable_syscall_instr();
}

void switch_to_user_mode() {
  uint64_t cs = get_user_cs() | 0x3;
  uint64_t ds = get_user_ds() | 0x3;

  init_syscall1();
  switchring3(ring3func, cs, ds);
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
        kprintf("dwInside Dummy....\n");
				yield();
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

