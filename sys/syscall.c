#include <sys/syscall.h>
#include <sys/task.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/vmm.h>
#include <sys/gdt.h>
#include <sys/utils.h>
#include <sys/terminal.h>
#include <sys/dirent.h>
 
#define MSR_LSTAR   0xc0000082 
#define MSR_STAR    0xc0000081

#define __NR_syscall_max     100 
#define __NR_read            0
#define __NR_write           1
#define __NR_exit            60 
#define __NR_fork            57 
#define __NR_execve          59 
#define __NR_wait4           61
#define __NR_getcwd          79
#define __NR_ps              90
#define __NR_getpid          91
#define __NR_getppid         92

typedef void (*sys_call_ptr_t) (void);
sys_call_ptr_t sys_call_table[__NR_syscall_max];

static syscall_args_t syscall_args;

void copy_syscall_args_to_task_struct(task_struct_t *task) {

  (task->syscall_args).rdi = syscall_args.rdi;
  (task->syscall_args).rsi = syscall_args.rsi;
  (task->syscall_args).rdx = syscall_args.rdx;
  (task->syscall_args).r10 = syscall_args.r10;
  (task->syscall_args).r8  = syscall_args.r8;
  (task->syscall_args).r9  = syscall_args.r9;
  (task->syscall_args).rcx = syscall_args.rcx;
  (task->syscall_args).__NR_syscall = syscall_args.__NR_syscall;

}

void get_syscall_args() {

  __asm__ __volatile__(
      "pushq %r9;"
      "pushq %r8;"
      "pushq %r10;"
      "pushq %rdx;"
      "pushq %rsi;"
      "pushq %rdi;"
      "pushq %rcx;"
      );

  __asm__ __volatile__("popq %%rax;movq %%rax, %0;":"=a"(syscall_args.rcx));
  __asm__ __volatile__("popq %%rax;movq %%rax, %0;":"=a"(syscall_args.rdi));
  __asm__ __volatile__("popq %%rax;movq %%rax, %0;":"=a"(syscall_args.rsi));
  __asm__ __volatile__("popq %%rax;movq %%rax, %0;":"=a"(syscall_args.rdx));
  __asm__ __volatile__("popq %%rax;movq %%rax, %0;":"=a"(syscall_args.r10));
  __asm__ __volatile__("popq %%rax;movq %%rax, %0;":"=a"(syscall_args.r8));
  __asm__ __volatile__("popq %%rax;movq %%rax, %0;":"=a"(syscall_args.r9));

}

/*
 * TODO: not saving rsp now.
 * save if required later.
 * saving rcx register. When sysretq is invoked, rip would be 
 * loaded with rcx value.
 */
void syscall_handler() {

  __asm__ __volatile__("pushq %rax;");
  __asm__ __volatile__("popq %%rax;movq %%rax, %0;":"=a"(syscall_args.__NR_syscall));
  
  get_syscall_args();

  copy_syscall_args_to_task_struct(get_current_running_task());

  /* not sure if doing right..above two operations would happen in userstack */
  __asm__ __volatile__("movq %%rsp, %0"  : "=a"(get_current_running_task()->ursp));
  __asm__ __volatile__("movq %0, %%rsp" :: "a"(get_current_running_task()->rsp));
  
  /* register bound variable to save the return value 
   * normal variables would be allocated in the stack. And we
   * are switching stack down the line. so "ret" value won't be
   * available to return.. so we should use register variables */
  //register int64_t ret __asm__("r15") = 0;
  /*ret =*/ (*sys_call_table[syscall_args.__NR_syscall])();
//  if(syscall_args.__NR_syscall != __NR_execve)
	 yield();

  
  __asm__ __volatile__("movq %%rsp, %0" : "=a"(get_current_running_task()->rsp));

 /* stack change from rsp to ursp */ 
  __asm__ __volatile__("movq %0, %%rsp" :: "a"(get_current_running_task()->ursp));
  
  /* restoring rcx register value, rip <-- rcx upon sysretq */
  __asm__ volatile("mov %0, %%rcx" ::"a"((get_current_running_task()->syscall_args).rcx));
  __asm__ volatile("add $0x8, %rsp"); 

  /* return value of syscall*/
  //__asm__ volatile("movq %0, %%rax"::"a"(ret));
  __asm__ volatile("movq %0, %%rax"::"a"(get_current_running_task()->retV));

  __asm__ volatile("sysretq"); 

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

/*TODO: move syscall functions somewhere else
 * as of now adding here itself
 *
 */
void sys_write() {

  //uint64_t fd;
  void *ptr;
  uint64_t size;
  char buff[512];
  ptr = (void*)syscall_args.rsi;
  //fd = syscall_args.rdi;
  size = syscall_args.rdx;

  memcpy(buff, ptr, size);
  write_to_terminal(buff, size);  
  
  get_current_running_task()->retV = 1;

  //return 1;
}

void sys_read() {
  uint64_t fd;
  void *ptr;
  uint64_t size;
  int ret = -1;

  ptr = (void*)(syscall_args.rsi);
  fd = syscall_args.rdi;
  size = syscall_args.rdx;

  while (ret == -1) {
    if (fd == STDIN) {
      ret = read_from_terminal(ptr, size);
    }
  }

  get_current_running_task()->retV = ret;
}

void sys_exit() {
  //kprintf("Done from exit()!\n");
  get_current_running_task()->parent_task->task_state = TASK_STATE_RUNNING;
  set_task_state(TASK_STATE_STOPPED);
}

void sys_getcwd() {
  void *ptr;
  uint64_t size;

  ptr = (void *)(syscall_args.rdi);
  size = syscall_args.rsi;

  if (size >= strlen(get_current_running_task()->cwd)) {
    strcpy(ptr, get_current_running_task()->cwd);
    get_current_running_task()->retV = 1;

  } else {
    get_current_running_task()->retV = 0;
  }
}

void sys_ps() {
  task_struct_t *tmp = running_task;
  kprintf("\nPID | PPID | NAME | STATE\n");
  while (tmp && tmp->next != running_task) {
    kprintf("%d | %d | %s | %s\n", tmp->pid, tmp->ppid, tmp->name, task_state_str[tmp->task_state]);
    tmp = tmp->next;
  }

  if (tmp && tmp != running_task) {
    kprintf("%d | %d | %s | %s\n", tmp->pid, tmp->ppid, tmp->name, task_state_str[tmp->task_state]);
  }
}

void sys_getpid() {

}

void sys_getppid() {

}

void sys_wait() {
  get_current_running_task()->task_state = TASK_STATE_WAITING;
}

void sys_execve() {

 // char *filename = (char *)syscall_args.rdi;
  char *filename = (char *)(get_current_running_task()->syscall_args.rdi);
  //char *const argv[]; rsi
  //char *const envp[]; rdx

//  kprintf("filename is.. %s\n", filename);
  execve_handler(filename);

  (get_current_running_task()->syscall_args).rcx= get_current_running_task()->rip;
}

/*
 * setting up syscall table init 
 */
void setup_sys_call_table() {

  sys_call_table[__NR_read]     = sys_read;  
  sys_call_table[__NR_write]    = sys_write;  
  sys_call_table[__NR_exit]     = sys_exit;  
  sys_call_table[__NR_fork]     = sys_fork;  
  sys_call_table[__NR_execve]   = sys_execve;  
  sys_call_table[__NR_getcwd]   = sys_getcwd;  
  sys_call_table[__NR_ps]       = sys_ps;  
  sys_call_table[__NR_getpid]   = sys_getpid;  
  sys_call_table[__NR_getppid]  = sys_getppid;  
  sys_call_table[__NR_wait4]    = sys_wait;  
  /* add remaining syscalls here..*/

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
void init_syscall() {

  uint64_t star_reg_value = ((uint64_t)0x1b << 48) | ((uint64_t)0x8 << 32);

  __wrmsr(MSR_LSTAR, get_low_dword((uint64_t)syscall_handler), get_high_dword((uint64_t)syscall_handler)); 

  __wrmsr(MSR_STAR, get_low_dword(star_reg_value), get_high_dword(star_reg_value)); 

	enable_syscall_instr();
  
  setup_sys_call_table();

}
