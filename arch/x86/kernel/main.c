#include "printk.h"
#include "cpu.h"

void start_kernel(void)
{

	int eax, ebx, ecx, edx, leaf, subleaf;

	/* TODO: init_print should be initaionazed before kernel start */
	init_printk();
	color_printk(WHITE, BLACK, "Hello world!\n");

	leaf = CPUID_EAX_CPU;
	subleaf = 0;
	get_cpuid(&eax, &ebx, &ecx, &edx, leaf, subleaf);
	color_printk(GREEN, BLACK, "CPU NAME: %s%s%s\n", &ebx, &ecx, &edx);
	while (1);
}
