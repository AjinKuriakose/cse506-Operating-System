#ifndef __TASK_H__
#define __TASK_H__

#include <sys/defs.h> 

#define MAX_NUM_PROCESSES   50

#define TASK_KSTACK_SIZE    4096
#define TASK_STATE_READY    1
#define TASK_STATE_RUNNING  2
#define INVALID_PID         0xFF

#define VMA_TYPE_STACK	    4
#define VMA_TYPE_HEAP	    3
#define VMA_TYPE_DATA	    2
#define VMA_TYPE_TEXT	    1

extern void init_tasking();
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
  
} task_struct_t;

extern void init_tasking();
extern void create_task(task_struct_t *, void(*)());
 
extern void yield(); 
extern void switch_task(task_struct_t *old, task_struct_t *new); 
extern void switchring3(void *, uint64_t, uint64_t, uint64_t); 
void doIt();
void start_init_process();
void start_sbush_process(char *bin_filename);

task_struct_t *get_current_running_task();
int sys_fork();

#endif /* __TASK_H__ */
