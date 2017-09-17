.global _isr_sys_def
_isr_sys_def:
  cli
  sti
  iret 


.global _isr_timer
_isr_timer:
  cli
  sti
  iret 
