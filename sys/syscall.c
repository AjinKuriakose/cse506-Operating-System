#include <sys/task.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/vmm.h>
#include <sys/gdt.h>
 

#define MSR_LSTAR   0xc0000082 /* long mode SYSCALL target */
#define MSR_STAR    0xc0000081 /* legacy mode SYSCALL target */

/*
 * TODO: not saving rsp now.
 * save if required later.
 */
void kernel_testfun() {
  uint64_t rcx; 
  __asm__ volatile("mov %%rcx, %0" :"=a"(rcx));

  kprintf("Hi from ring 0\n");
  __asm__ volatile("mov %0, %%rcx" ::"a"(rcx));
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

  __wrmsr(MSR_LSTAR, get_low_dword((uint64_t)kernel_testfun), get_high_dword((uint64_t)kernel_testfun)); 
  __wrmsr(MSR_STAR, get_low_dword(star_reg_value), get_high_dword(star_reg_value)); 

	enable_syscall_instr();
}
