#ifndef _IDT_H
#define _IDT_H

void init_idt();
void _isr_timer();
void _isr_kb();
void _isr_sys_def();

#endif
