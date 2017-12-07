#ifndef __TASK_H__
#define __TASK_H__

#include <sys/defs.h> 

#define MAX_NUM_PROCESSES   50
#define MAX_NUM_FDS         50

#define TASK_KSTACK_SIZE    4096

#define TASK_STATE_UNKNOWN  0
#define TASK_STATE_READY    1
#define TASK_STATE_RUNNING  2
#define TASK_STATE_WAITING  3
#define TASK_STATE_STOPPED  4 

#define INVALID_PID         0xFF
#define INVALID_FD          0xFFFF

#define VMA_TYPE_STACK	    4
#define VMA_TYPE_HEAP	      3
#define VMA_TYPE_DATA	      2
#define VMA_TYPE_TEXT	      1

#define CWD_LEN             64

extern void init_tasking();

typedef struct sycall_args_t {

  uint64_t rdi;
  uint64_t rsi;
  uint64_t rdx;
  uint64_t r10;
  uint64_t r8;
  uint64_t r9;
  uint64_t rcx;
  uint64_t __NR_syscall;

} syscall_args_t;

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

typedef struct vma_struct_t {
  uint64_t vma_start;
  uint64_t vma_end;
  struct vma_struct_t *vma_next;
  struct mm_struct_t *vma_mm;
  uint64_t flags;
  uint64_t vma_type;
} vma_struct_t;

typedef struct mm_struct_t {
  uint64_t code_start;
  uint64_t code_end;
  uint64_t data_start;
  uint64_t data_end;
  uint64_t brk_start;
  uint64_t brk;
  uint64_t stack_start;
  vma_struct_t *mmap;
} mm_struct_t;

/*
 * Following is the PCB.
 * TODO : should add requied parameters like pid etc.
 */
typedef struct task_struct_t {
  uint64_t    rsp;
  uint64_t    kstack;
  uint64_t    kstack_top;
  struct task_struct_t *next;
  struct task_struct_t *parent_task;
  uint64_t    pid;
  uint64_t    ppid;
  uint64_t    rip;
  uint64_t    cr3;
  uint64_t    retV;
  uint64_t    ursp; //to save user stack address
  char        name[32];
  mm_struct_t *mm;
  uint8_t     task_state;
  uint16_t    num_children;
  uint16_t    fd_list[MAX_NUM_FDS];
  syscall_args_t syscall_args;
  char        cwd[CWD_LEN];
} task_struct_t;

extern task_struct_t *running_task;
char task_state_str[10][32];

extern void init_tasking();
extern void create_task(task_struct_t *, void(*)());
 
extern void yield(); 
extern void switch_task(task_struct_t *old, task_struct_t *new); 
extern void switchring3(void *, uint64_t, uint64_t, uint64_t); 
void doIt();
void start_init_process();
void start_sbush_process(char *bin_filename);

task_struct_t *get_current_running_task();
void sys_fork();
uint16_t get_free_fd(task_struct_t *task);
void free_fd(task_struct_t *task, uint16_t fd);

void execve_handler(char *filename);
void task_cleanup(task_struct_t *task);
void set_task_state(uint8_t);
uint8_t get_task_state();

#endif /* __TASK_H__ */
