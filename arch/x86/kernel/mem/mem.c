#include "mem.h"
#include "printk.h"

void init_memory()
{

	unsigned long total_mem = 0;
	struct memory_e820 *p;
	unsigned int e820_len = 32, i;

	p = (struct memory_e820 *)0xffff800000007e00;

	/* walk over the e820 table */
	color_printk(ORANGE, BLACK, "Display Physic Address MAP,Type(1:RAM,2:ROM or Reserved,3:ACPI Reclaim Memory,4:ACPI NVS Memory,Others:Undefine)\n");
	for (i = 0; i < e820_len; i ++) {
		color_printk(ORANGE, BLACK, "Address:%#010x,%08x\tLength:%#010x,%08x\tType:%#010x\n", p->addr2, p->addr1, p->length2, p->length1, p->type);

		if (p->type == 1) {
			unsigned long tmp = 0;
			tmp = p->length2;
			total_mem += p->length1;
			total_mem += tmp << 32;
		}

		if (p->type > 4) {
			break;
		}

		p++;
	}

	color_printk(ORANGE, BLACK, "OS can use total RAM:%#018lx\n", total_mem);

}
