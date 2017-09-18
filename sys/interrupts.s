.global _isr_timer
_isr_timer:
  cli
  call print_timer
  sti
  call send_EOI
  iretq

.global _isr_timer_new
_isr_timer_new:
    iretq
    push %rax
    push %rcx
    push %rdx
    push %rbx
    push %rbp
    push %rsi
    push %rdi
    #call print_timer
    #call send_EOI
    pop %rdi    
    pop %rsi    
    pop %rbp    
    pop %rbx    
    pop %rdx    
    pop %rcx
    pop %rax
    iretq

.global _isr_kb
_isr_kb:
  iretq
  cli
  call key_handler 
  sti
  call send_EOI
  iretq

.global send_EOI
.global print_timer

.global _isr_timer1
_isr_timer1:
    push %rax
    push %rcx
    push %rdx
    push %rbx
    push %rbp
    push %rsi
    push %rdi
    call print_timer
    call send_EOI 
    pop %rdi
    pop %rsi
    pop %rbp
    pop %rbx
    pop %rdx
    pop %rcx
    pop %rax
    iretq

.global _isr_sys_def0
_isr_sys_def0:
 iretq

.global _isr_sys_def1
_isr_sys_def1:
 iretq

.global _isr_sys_def2
_isr_sys_def2:
 iretq

.global _isr_sys_def3
_isr_sys_def3:
 iretq

.global _isr_sys_def4
_isr_sys_def4:
 iretq

.global _isr_sys_def5
_isr_sys_def5:
 iretq

.global _isr_sys_def6
_isr_sys_def6:
 iretq

.global _isr_sys_def7
_isr_sys_def7:
 iretq

.global _isr_sys_def8
_isr_sys_def8:
 iretq

.global _isr_sys_def9
_isr_sys_def9:
 iretq

.global _isr_sys_def10
_isr_sys_def10:
 iretq

.global _isr_sys_def11
_isr_sys_def11:
 iretq

.global _isr_sys_def12
_isr_sys_def12:
 iretq

.global _isr_sys_def13
_isr_sys_def13:
 iretq

.global _isr_sys_def14
_isr_sys_def14:
 iretq

.global _isr_sys_def15
_isr_sys_def15:
 iretq

.global _isr_sys_def16
_isr_sys_def16:
 iretq

.global _isr_sys_def17
_isr_sys_def17:
 iretq

.global _isr_sys_def18
_isr_sys_def18:
 iretq

.global _isr_sys_def19
_isr_sys_def19:
 iretq

.global _isr_sys_def20
_isr_sys_def20:
 iretq

.global _isr_sys_def21
_isr_sys_def21:
 iretq

.global _isr_sys_def22
_isr_sys_def22:
 iretq

.global _isr_sys_def23
_isr_sys_def23:
 iretq

.global _isr_sys_def24
_isr_sys_def24:
 iretq

.global _isr_sys_def25
_isr_sys_def25:
 iretq

.global _isr_sys_def26
_isr_sys_def26:
 iretq

.global _isr_sys_def27
_isr_sys_def27:
 iretq

.global _isr_sys_def28
_isr_sys_def28:
 iretq

.global _isr_sys_def29
_isr_sys_def29:
 iretq

.global _isr_sys_def30
_isr_sys_def30:
 iretq

.global _isr_sys_def31
_isr_sys_def31:
 iretq

