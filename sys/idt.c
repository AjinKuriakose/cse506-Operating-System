#include <sys/defs.h>
#include <sys/idt.h>

#define MAX_IDT	256 


struct idt_elem_t {
   uint16_t off_1; 
   uint16_t selector;  
   uint8_t pad_1;      
   uint8_t type_attr; 
   uint16_t off_2; 
   uint32_t off_3; 
   uint32_t pad_2; 
}__attribute__((packed));


struct idtr_t {
  uint16_t limit;
  uint64_t base;
}__attribute__((packed));

void _x86_64_asm_lidt(struct idtr_t *idtr);

static struct idt_elem_t idt[MAX_IDT];

static struct idtr_t idtr = { sizeof(idt), (uint64_t)idt };

static void init_idt_entry(unsigned char irq, uint64_t base, uint16_t sel, uint8_t type) {

   idt[irq].type_attr = type;
   idt[irq].selector = sel;

   idt[irq].pad_1 = 0;

   idt[irq].off_1 = base & 0xFFFF;
   idt[irq].off_2 = (base >> 16) & 0xFFFF;
   idt[irq].off_3 = (base >> 32) & 0xFFFFFFFF;

   idt[irq].pad_2 = 0;
}

void init_idt() {

  init_idt_entry(0, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(1, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(2, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(3, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(4, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(5, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(6, (uint64_t)_isr_sys_def, 0x08, 0x8E); //==
  init_idt_entry(7, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(8, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(9, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(10, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(11, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(12, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(13, (uint64_t)_isr_sys_def, 0x08, 0x8E); //==
  init_idt_entry(14, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(15, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(16, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(17, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(18, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(19, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(20, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(21, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(22, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(23, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(24, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(25, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(26, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(27, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(28, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(29, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(30, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(31, (uint64_t)_isr_sys_def, 0x08, 0x8E);
  init_idt_entry(40, (uint64_t)_isr_timer, 0x08, 0x8E);

  _x86_64_asm_lidt(&idtr);
}
