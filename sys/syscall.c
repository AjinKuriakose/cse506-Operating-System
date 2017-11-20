#include <sys/task.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/vmm.h>
#include <sys/gdt.h>

 
#define MSR_LSTAR   0xc0000082 
#define MSR_STAR    0xc0000081

#define __NR_syscall_max     50 
#define __NR_read            0
#define __NR_write           1

typedef void (*sys_call_ptr_t) (void);
sys_call_ptr_t sys_call_table[__NR_syscall_max];



/*
 * TODO: not saving rsp now.
 * save if required later.
 * saving rcx register. When sysretq is invoked, rip would be 
 * loaded with rcx value.
 */
void syscall_handler() {

  uint64_t rcx; 
  uint64_t __NR_syscall;

  /* get the syscall number from rax register */
  /*reading the value of rax first. else the next statement will overwrite it. */
  __asm__ volatile("" :"=a"(__NR_syscall));
  __asm__ volatile("mov %%rcx, %0" :"=a"(rcx));

  (*sys_call_table[__NR_syscall])();
  
  /* restoring rcx register value, rip <-- rcx upon sysretq */
  __asm__ volatile("mov %0, %%rcx" ::"a"(rcx));
  __asm__ volatile("add $0x8, %rsp"); 
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
  kprintf("\nsys_write dummy funtion. Ring 0 \n");
  char *ptr;
  __asm__ volatile("mov %%rsi, %0" :"=a"(ptr));
//  kprintf("hellon %c\n", *ptr);
}
void sys_read() {
  kprintf("\nsys_read dummy funtion. Ring 0\n");
}

/*
 * setting up syscall table init 
 */
void setup_sys_call_table() {

  sys_call_table[__NR_read] = sys_read;  
  sys_call_table[__NR_write] = sys_write;  
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
