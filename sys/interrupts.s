.global _isr_sys_def
_isr_sys_def:
  cli
  call testprint
  sti
  call send_EOI
  iretq
 
.global _isr_timer
_isr_timer:
  cli
  call print_timer
  sti
  call send_EOI
  iretq

.global _isr_kb
_isr_kb:
  cli
  call key_handler 
  sti
  call send_EOI
  iretq
