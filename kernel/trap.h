#ifndef _TRAP_H_
#define _TRAP_H_

extern void initTss();
extern void initIdt();


extern void idt_ent_divide_error();
extern void idt_ent_debug_exception();
extern void idt_ent_nmi();
extern void idt_ent_breakpoint();
extern void idt_ent_overflow();
extern void idt_ent_bound();
extern void idt_ent_invalid_opcode();
extern void idt_ent_device_not_available();
extern void idt_ent_double_fault();
extern void idt_ent_coprocessor_segment_overrun();
extern void idt_ent_invalid_tss();
extern void idt_ent_segment_not_present();
extern void idt_ent_stack_segment_fault();
extern void idt_ent_general_protection();
extern void idt_ent_page_fault();
extern void idt_ent_x87_fpu_floating_point_error();
extern void idt_ent_alignment_check();
extern void idt_ent_machine_check();
extern void idt_ent_simd_floating_point_exception();
extern void idt_ent_virtualization_exception();
extern void idt_ent_control_protection_exception();
extern void ignore_int_and_halt();


#endif