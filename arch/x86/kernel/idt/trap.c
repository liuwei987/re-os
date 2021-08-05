#include "trap.h"
#include "gate.h"

void do_divide_error(unsigned long rsp, unsigned long error_code)
{

	unsigned long *p = NULL;
	p = (unsigned long *)(rsp + 0x98);
	color_printk(RED, BLACK, "do_divide_error(0), ERROR_CODE:%#018lx, RSP:%#018lx, RIP:%#018lx\n", error_code, rsp, *p);
	while(1);
}

void do_nmi(unsigned long rsp, unsigned long error_code)
{
	unsigned long *p = NULL;
	p = (unsigned long *)(rsp + 0x98);
	color_printk(RED, BLACK, "do_nmi(2), ERROR_CODE:%#018lx, RSP:%#018lx, RIP:%#018lx\n", error_code, rsp, *p);
	while(1);
}

void do_invalid_TSS(unsigned long rsp, unsigned long error_code)
{
	unsigned long *p = NULL;
	p = (unsigned long *)(rsp + 0x98);
	color_printk(RED, BLACK, "do_invalid_TSS(10), ERROR_CODE:%#018lx, RSP:%#018lx, RIP:%#018lx\n", error_code, rsp, *p);

	if (error_code & 0x01) {
		color_printk(RED, BLACK, "The exception occurred during delivery of an event external to the program, such as an interrupt or an ealier exception.\n");
	}

	if (error_code & 0x02) {
		color_printk(RED, BLACK, "Refers to a gate descriptor in the IDT;\n");
	} else {
		//color_printk(RED, BLACK, "Refers to a descriptor in the GDT or the current LDT;\n");
		if (error_code & 0x04) {
			color_printk(RED, BLACK, "Refers to a segment or gate descriptor in the LDT;\n");
		} else {
			color_printk(RED, BLACK, "Refers to descriptor in the current GDT;\n");
		}
	}

	color_printk(RED, BLACK, "Segment Selector Index:%#010x\n", error_code & 0xfff8);

	while(1);
}

void do_page_fault(unsigned long rsp, unsigned long error_code)
{
	unsigned long *p = NULL;
	unsigned long cr2 = 0;
	p = (unsigned long *)(rsp + 0x98);
	__asm__ __volatile__("movq %%cr2, %0":"=r"(cr2)::"memory");
	color_printk(RED, BLACK, "do_page_fault(14), ERROR_CODE:%#018lx, RSP:%#018lx, RIP:%#018lx\n", error_code, rsp, *p);

	if (error_code & 0x01) {
		color_printk(RED, BLACK, "Page proteced,\t");
	} else {
		color_printk(RED, BLACK, "Page Not-Present,\t");
	}

	if (error_code & 0x02) {
		color_printk(RED, BLACK, "Write cause Fault,\t");
	} else {
		color_printk(RED, BLACK, "Read cause Fault,\t");
	}

	if (error_code & 0x04) {
		color_printk(RED, BLACK, "Fault in User(3),\t");
	} else {
		color_printk(RED, BLACK, "Fault in Supervisor(0,1,2),\t");
	}

	if (error_code & 0x08) {
		color_printk(RED, BLACK, "Reserved bit cause Fault,\t");
	} else {
		color_printk(RED, BLACK, "Not-Reserved bit cause Fault,\t");
	}

	if (error_code & 0x10) {
		color_printk(RED, BLACK, "Instructioon fetch cause Fault\n");
	} else {
		color_printk(RED, BLACK, "Not-Instructioon fetch cause Fault\n");
	}

	color_printk(RED, BLACK, "CR2:%#018lx\n", cr2);
	while(1);
}

void sys_vector_init()
{

	set_trap_gate(0, 1, divide_error);
	//set_trap_gate(1, 1, debug);
	set_intr_gate(2, 1, nmi);
	//set_system_gate(3, 1, int3);
	//set_system_gate(4, 1, overflow);
	//set_system_gate(5, 1, bounds);

	//set_trap_gate(6, 1, undefine_opcode);
	//set_trap_gate(7, 1, dev_not_available);
	//set_trap_gate(8, 1, double_fault);
	//set_trap_gate(9, 1, coprocessor_segment_overrun);
	set_trap_gate(10, 1, invalid_TSS);
	//set_trap_gate(11, 1, segment_not_present);
	//set_trap_gate(12, 1, stack_segment_fault);
	//set_trap_gate(13, 1, general_protection);
	set_trap_gate(14, 1, page_fault);
	// 15 Intel use
	//set_trap_gate(16, 1, x87_FPU_error);
	//set_trap_gate(17, 1, alignment_check);
	//set_trap_gate(18, 1, machine_check);
	//set_trap_gate(19, 1, SIMD_exception);
	//set_trap_gate(20, 1, virtualization_exception);

	//set_system_gate(SYSTEM_CALL_VECTOR, 7 system_call);
}
