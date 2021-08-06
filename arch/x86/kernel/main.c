#include "printk.h"
#include "cpu.h"
#include "trap.h"
#include "gate.h"
#include "mem.h"

void start_kernel(void)
{

	int eax, ebx, ecx, edx, leaf, subleaf;

	/* TODO: init_print should be initaionazed before kernel start */
	init_printk();
	color_printk(WHITE, BLACK, "Hello world!\n");

	/* Get CPU name from CPUID */
	leaf = CPUID_EAX_CPU;
	subleaf = 0;
	get_cpuid(&eax, &ebx, &ecx, &edx, leaf, subleaf);
	color_printk(GREEN, BLACK, "CPU NAME: %s%s%s\n", &ebx, &ecx, &edx);
	/* Load to task register */
	load_TR(8);
	set_tss64(0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00);

	/* init vector from system */
	sys_vector_init();

	/* init memory(show e820 table) */
	init_memory();

	/* Triger the divide error */
	//int m = 1 / 0;  // traped in DIVIDE ERROR and show up the divide fault message
	while (1);
}
