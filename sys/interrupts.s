.global _isr_kb
_isr_kb:
  call key_handler
  call send_EOI
  iretq

.global _isr_timer1
_isr_timer1:
  call send_EOI
  pushq %rax
  pushq %rcx
  pushq %rdx
  pushq %rbx
  pushq %rbp
  pushq %rsi
  pushq %rdi
  pushq %r8
  pushq %r9
  pushq %r10
  call print_timer
  call send_EOI
  popq %r10
  popq %r9
  popq %r8
  popq %rdi
  popq %rsi
  popq %rbp
  popq %rbx
  popq %rdx
  popq %rcx
  popq %rax
  iretq

.global _isr_sys_def0
_isr_sys_def0:
  call send_EOI
  iretq

.global _isr_sys_def1
_isr_sys_def1:
  call send_EOI
  iretq

.global _isr_sys_def2
_isr_sys_def2:
  call send_EOI
  iretq

.global _isr_sys_def3
_isr_sys_def3:
  call send_EOI
  iretq

.global _isr_sys_def4
_isr_sys_def4:
  call send_EOI
  iretq

.global _isr_sys_def5
_isr_sys_def5:
  call send_EOI
  iretq

.global _isr_sys_def6
_isr_sys_def6:
  call send_EOI
  iretq

.global _isr_sys_def7
_isr_sys_def7:
  call send_EOI
  iretq

.global _isr_sys_def8
_isr_sys_def8:
  call send_EOI
  iretq

.global _isr_sys_def9
_isr_sys_def9:
  call send_EOI
  iretq

.global _isr_sys_def10
_isr_sys_def10:
  call send_EOI
  iretq

.global _isr_sys_def11
_isr_sys_def11:
  call send_EOI
  iretq

.global _isr_sys_def12
_isr_sys_def12:
  call send_EOI
  iretq

.global _isr_sys_def13
_isr_sys_def13:
  call send_EOI
  iretq

.global _isr_sys_def14
_isr_sys_def14:
  call send_EOI
  iretq

.global _isr_sys_def15
_isr_sys_def15:
  call send_EOI
  iretq

.global _isr_sys_def16
_isr_sys_def16:
  call send_EOI
  iretq

.global _isr_sys_def17
_isr_sys_def17:
  call send_EOI
  iretq

.global _isr_sys_def18
_isr_sys_def18:
  call send_EOI
  iretq

.global _isr_sys_def19
_isr_sys_def19:
  call send_EOI
  iretq

.global _isr_sys_def20
_isr_sys_def20:
  call send_EOI
  iretq

.global _isr_sys_def21
_isr_sys_def21:
  call send_EOI
  iretq

.global _isr_sys_def22
_isr_sys_def22:
  call send_EOI
  iretq

.global _isr_sys_def23
_isr_sys_def23:
  call send_EOI
  iretq

.global _isr_sys_def24
_isr_sys_def24:
  call send_EOI
  iretq

.global _isr_sys_def25
_isr_sys_def25:
  call send_EOI
  iretq

.global _isr_sys_def26
_isr_sys_def26:
  call send_EOI
  iretq

.global _isr_sys_def27
_isr_sys_def27:
  call send_EOI
  iretq

.global _isr_sys_def28
_isr_sys_def28:
  call send_EOI
  iretq

.global _isr_sys_def29
_isr_sys_def29:
  call send_EOI
  iretq

.global _isr_sys_def30
_isr_sys_def30:
  call send_EOI
  iretq

.global _isr_sys_def31
_isr_sys_def31:
  call send_EOI
  iretq

