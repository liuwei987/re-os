#include "cpu.h"
#include "printk.h"

#define SUPPORT_CPUID_FLAG (1 << 21)

static int is_support_cpuid(void)
{
	long ret = 0;
	/* sbbl -> dest - (src + cf) */
	__asm__ volatile ("mov $0, %%rax\n" \
			"not %%rax\n" \
			"add $1, %%rax\n" \
			"clc\n" \
			"pushfq \n" \
			"pop %%rax\n" \
			"bts  %1, %%eax\n" \
			"sbb %0, %0\n" \
			:"=r"(ret) \
			:"r"(SUPPORT_CPUID_FLAG) \
			:"cc", "memory");
	/* return true if bit offset in EFLAGS was clear */
	return (ret == 0);
}

int get_cpuid(int *eax, int *ebx, int *ecx, int *edx, int leaf, int subleaf)
{

	/* not support CPUID, printk and return */
	if (!is_support_cpuid()) {
		color_printk(WHITE, BLACK, "The processor does not supporte CPUID instruction!\n");
		return -1;
	}

	__asm__ volatile (\
			"cpuid \n\r"\
			:"=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)\
			:"a" (leaf), "c" (subleaf)\
			:"memory");
}
