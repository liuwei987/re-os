#include "gate.h"

void set_intr_gate(unsigned int n, unsigned char ist, void *addr)
{
	_set_gate(IDT_Table + n, 0x8E, ist, addr); //P,DPL=0,TYPE=E, interrupt gate
}

void set_trap_gate(unsigned int n, unsigned char ist, void *addr)
{
	_set_gate(IDT_Table + n, 0x8f, ist, addr); //P,DPL=0,TYPE=E, trap gate
}

void set_system_gate(unsigned int n, unsigned char ist, void *addr)
{
	_set_gate(IDT_Table + n, 0xef, ist, addr); //P,DPL=3,TYPE=f, system gate(trap of user privilege leve)
}


void set_tss64(unsigned long rsp0, unsigned long rsp1, unsigned long rsp2, unsigned long ist1, unsigned long ist2, unsigned long ist3, unsigned long ist4, unsigned long ist5, unsigned long ist6, unsigned long ist7)
{

	*(unsigned long *)(TSS64_Table + 1) = rsp0;
	*(unsigned long *)(TSS64_Table + 3) = rsp1;
	*(unsigned long *)(TSS64_Table + 5) = rsp2;

	*(unsigned long *)(TSS64_Table + 9) =  ist1;
	*(unsigned long *)(TSS64_Table + 11) = ist2;
	*(unsigned long *)(TSS64_Table + 13) = ist3;
	*(unsigned long *)(TSS64_Table + 15) = ist4;
	*(unsigned long *)(TSS64_Table + 17) = ist5;
	*(unsigned long *)(TSS64_Table + 19) = ist6;
	*(unsigned long *)(TSS64_Table + 21) = ist7;
}
