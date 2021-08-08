#include "mem.h"
#include "printk.h"


struct global_mem_descriptor global_mem_struct = {{0}, 0};

void init_memory()
{

	unsigned long total_mem = 0;
	struct E820 *p;

	unsigned int i, e820_entrys;
	e820_entrys = sizeof(global_mem_struct.e820) / sizeof(global_mem_struct.e820[0]);

	p = (struct E820 *)0xffff800000007e00;

	/* walk over the e820 table */
	//color_printk(ORANGE, BLACK, "Display Physic Address MAP,Type(1:RAM,2:ROM or Reserved,3:ACPI Reclaim Memory,4:ACPI NVS Memory,Others:Undefine)\n");
	for (i = 0; i < e820_entrys; i ++) {
		//color_printk(ORANGE, BLACK, "Address:%#010x,%08x\tLength:%#010x,%08x\tType:%#010x\n", p->addr2, p->addr1, p->length2, p->length1, p->type);
		color_printk(ORANGE, BLACK, "Address:%#010x\tLength:%#010x\tType:%#010x\n", p->address, p->length, p->type);

		if (p->type == 1) {
			total_mem += p->length;
		}

		global_mem_struct.e820[i].address = p->address;
		global_mem_struct.e820[i].length  = p->length;
		global_mem_struct.e820[i].type    = p->type;
		global_mem_struct.e820_length = i;

		if (p->type > 4) {
			break;
		}

		p++;
	}

	color_printk(ORANGE, BLACK, "OS can use total RAM:%#018lx\n", total_mem);

	total_mem = 0;
	for (i = 0; i < global_mem_struct.e820_length; i ++) {
		unsigned long start, end;

		if (global_mem_struct.e820[i].type != 1)
			continue;

		start = PAGE_2M_ALIGN(global_mem_struct.e820[i].address);
		//end = PAGE_2M_ALIGN(global_mem_struct.e820[i].address + global_mem_struct.e820[i].length);  don't use
		end = (global_mem_struct.e820[i].address + global_mem_struct.e820[i].length) & PAGE_2M_MASK;
		//end = ((global_mem_struct.e820[i].address + global_mem_struct.e820[i].length) >> PAGE_2M_SHIFT) << PAGE_2M_SHIFT;
		if (end < start)
			continue;

		total_mem += (end - start) >> PAGE_2M_SHIFT;
	}

	color_printk(ORANGE, BLACK, "OS can use 2M pages:%#010x=%010d\n", total_mem, total_mem);
}
