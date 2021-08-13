#include "mem.h"
#include "printk.h"

unsigned long *global_cr3 = NULL;
unsigned int ZONE_NORMAL_INDEX = 0, ZONE_DMA_INDEX = 0, ZONE_UNMAPED_INDEX = 0;
struct global_mem_descriptor global_mem_struct = {{0}, 0};

static unsigned long page_init(struct Page *page, unsigned long flags)
{

	if (!page->attribute) {
		*(global_mem_struct.bits_map + ((page->phy_addr >> PAGE_2M_SHIFT) >> 6)) |= 1UL << (page->phy_addr >> PAGE_2M_SHIFT) % 64;
		page->attribute = flags;
		page->ref_count ++;
		page->zone->page_using_count++;
		page->zone->page_free_count--;
		page->zone->total_page_link++;

	} else if ((page->attribute & PG_Referenced) || (page->attribute & PG_K_Share_To_U) || (flags & PG_Referenced) || (flags & PG_K_Share_To_U)) {
		page->attribute |= flags;
		page->ref_count++;
		page->zone->total_page_link++;
	} else {
		*(global_mem_struct.bits_map + ((page->phy_addr >> PAGE_2M_SHIFT) >> 6)) |= 1UL << (page->phy_addr >> PAGE_2M_SHIFT) % 64;
		page->attribute |= flags;
	}

	return 0;
}

void init_memory()
{

	unsigned long total_mem = 0;
	struct E820 *p = NULL;

	unsigned int i, e820_entrys;
	e820_entrys = sizeof(global_mem_struct.e820) / sizeof(global_mem_struct.e820[0]);

	p = (struct E820 *)0xffff800000007e00;

	/* walk over the e820 table */
	for (i = 0; i < e820_entrys; i ++) {
		color_printk(ORANGE, BLACK, "Address:%#010x\tLength:%#010x\tType:%#010x\n", p->address, p->length, p->type);

		// method of caculate type 1(RAM) total memory
		if (p->type == 1) {
			total_mem += p->length;
		}

		global_mem_struct.e820[i].address += p->address;
		global_mem_struct.e820[i].length  += p->length;
		global_mem_struct.e820[i].type    = p->type;
		global_mem_struct.e820_length     = i;

		p++;
		if(p->type > 4 || p->length == 0 || p->type < 1) {
			break;
		}
	}
	color_printk(ORANGE, BLACK, "OS can use total RAM:%#018lx\n", total_mem);

	total_mem = 0;
	for (i = 0; i < global_mem_struct.e820_length; i ++) {
		unsigned long start, end;
		if (global_mem_struct.e820[i].type != 1) {
			continue;
		}
		start = PAGE_2M_ALIGN(global_mem_struct.e820[i].address);
		end = (global_mem_struct.e820[i].address + global_mem_struct.e820[i].length) & PAGE_2M_MASK;
		if (end <= start)
			continue;

		total_mem += (end - start) >> PAGE_2M_SHIFT;

	}
	color_printk(ORANGE, BLACK, "OS can use RAM for 2M pages:%#010x=%010d\n", total_mem, total_mem);

	total_mem = global_mem_struct.e820[global_mem_struct.e820_length].address + global_mem_struct.e820[global_mem_struct.e820_length].length;
	/* text, data, available memory adress for global memory struct */
	global_mem_struct.start_code = (unsigned long)&_text;
	global_mem_struct.end_code   = (unsigned long)&_etext;
	global_mem_struct.end_data   = (unsigned long)&_edata;
	global_mem_struct.end_brk    = (unsigned long)&_end;

	/* init bit map for global memory struct */
	global_mem_struct.bits_map   = (unsigned long *)PAGE_4K_ALIGN(global_mem_struct.end_brk);
	global_mem_struct.bits_size  = total_mem >> PAGE_2M_SHIFT;
	global_mem_struct.bits_length = (((unsigned long)(total_mem >> PAGE_2M_SHIFT) + sizeof(long) * 8 - 1) / 8) & (~(sizeof(long) - 1));
	memset(global_mem_struct.bits_map, 0xff, global_mem_struct.bits_length);

	/* page_struct init for global memory struct */
	global_mem_struct.page_struct  = (struct Page *)(PAGE_4K_ALIGN((unsigned long)global_mem_struct.bits_map + global_mem_struct.bits_length));
	global_mem_struct.pages_size   = total_mem >> PAGE_2M_SHIFT;
	global_mem_struct.pages_length = ((total_mem >> PAGE_2M_SHIFT) * sizeof(struct Page) + sizeof(long) - 1) & (~(sizeof(long) - 1));
	memset(global_mem_struct.page_struct, 0x00, global_mem_struct.pages_length);

	/* zone_struct init for global memory struct */
	global_mem_struct.zone_struct = (struct Zone *)(PAGE_4K_ALIGN((unsigned long)global_mem_struct.page_struct + global_mem_struct.pages_length));
	global_mem_struct.zone_size = 0;
	global_mem_struct.zone_length = (5 * sizeof(struct Zone) + sizeof(long) - 1) & (~(sizeof(long) - 1));
	memset(global_mem_struct.zone_struct, 0x00, global_mem_struct.zone_length);

	for (i = 0; i < global_mem_struct.e820_length; i ++) {
		unsigned long start, end;
		struct Zone *z;
		struct Page *p;

		if (global_mem_struct.e820[i].type != 1)
			continue;

		start = PAGE_2M_ALIGN(global_mem_struct.e820[i].address);
		//end = PAGE_2M_ALIGN(global_mem_struct.e820[i].address + global_mem_struct.e820[i].length); FAULT: don't use 
		end = (global_mem_struct.e820[i].address + global_mem_struct.e820[i].length) & PAGE_2M_MASK;
		//end = ((global_mem_struct.e820[i].address + global_mem_struct.e820[i].length) >> PAGE_2M_SHIFT) << PAGE_2M_SHIFT;
		if (end <= start)
			continue;

		/* init struct of Zone */
		z = global_mem_struct.zone_struct + global_mem_struct.zone_size;
		global_mem_struct.zone_size ++;
		z->zone_start_addr  = start;
		z->zone_end_addr    = end;
		z->zone_length      = end - start;
		z->attribute        = 0;

		z->page_using_count = 0;
		z->page_free_count  = (end - start) >> PAGE_2M_SHIFT;
		z->total_page_link  = 0;

		z->GMD_struct       = &global_mem_struct;

		z->pages_length     = (end - start) >> PAGE_2M_SHIFT;
		z->pages_group      = (struct Page *)(global_mem_struct.page_struct + (start >> PAGE_2M_SHIFT));

		p = z->pages_group;
		/* init struct of Page */
		for (int j = 0; j < z->pages_length; j++, p++) {
			p->zone     = z;
			p->phy_addr  = start + PAGE_2M * j;
			p->attribute = 0;
			p->ref_count = 0;
			p->age       = 0;

			*(global_mem_struct.bits_map + ((p->phy_addr >> PAGE_2M_SHIFT) >> 6)) ^= 1UL << (p->phy_addr >> PAGE_2M_SHIFT) % 64;

		}

	}

	global_mem_struct.page_struct->zone = global_mem_struct.zone_struct;
	global_mem_struct.page_struct->phy_addr  = 0;
	global_mem_struct.page_struct->attribute = 0;
	global_mem_struct.page_struct->ref_count = 0;
	global_mem_struct.page_struct->age       = 0;

	global_mem_struct.zone_length = ((global_mem_struct.zone_size * sizeof(struct Zone) + sizeof(long) -1)) & (~(sizeof(long) - 1));

	color_printk(ORANGE, BLACK, "bits_map:%#018lx, bits_size:%#018lx, bits_length:%#018lx\n", global_mem_struct.bits_map, global_mem_struct.bits_size, global_mem_struct.bits_length);
	color_printk(ORANGE, BLACK, "page_struct:%#018lx, page_size:%#018lx, page_length:%#018lx\n", global_mem_struct.page_struct, global_mem_struct.pages_size, global_mem_struct.pages_length);
	color_printk(ORANGE, BLACK, "zone_struct:%#018lx, zone_size:%#018lx, zone_length:%#018lx\n", global_mem_struct.zone_struct, global_mem_struct.zone_size, global_mem_struct.zone_length);

	ZONE_DMA_INDEX = 0;	//need rewrite in the future
	ZONE_NORMAL_INDEX = 0;	//need rewrite in the future

	for (i = 0; i < global_mem_struct.zone_size; i++) {
		struct Zone *z = global_mem_struct.zone_struct + i;
		color_printk(ORANGE, BLACK, "zone_start_addr:%#018lx, zone_end_addr:%#018lx, zone_length:%#018lx, pages_group:%#018lx\n", z->zone_start_addr, z->zone_end_addr, z->zone_length, z->pages_group);

		if (z->zone_start_addr == 0x100000000)
			ZONE_UNMAPED_INDEX = 1;
	}

	global_mem_struct.end_of_struct = (unsigned long)((unsigned long)(global_mem_struct.zone_struct + global_mem_struct.zone_length + sizeof(long) * 32)) & (~(sizeof(long) - 1));

	color_printk(ORANGE, BLACK, "start_code:%#018lx, end_code:%#018lx, end_data:%#018lx, end_brk:%#018lx, end_of_struct:%#018lx\n", global_mem_struct.start_code,
	global_mem_struct.end_code, global_mem_struct.end_brk, global_mem_struct.end_of_struct);

	i = virt_to_phy(global_mem_struct.end_of_struct) >> PAGE_2M_SHIFT;

	for (int j = 0; j <= i; j++) {
		page_init(global_mem_struct.page_struct + j, PG_PTable_Mapped | PG_Kernel_Init | PG_Active | PG_Kernel);

	}

	global_cr3 = get_gdt();
	color_printk(ORANGE, BLACK, "Global_cr3\t:%#018lx\n", global_cr3);
	color_printk(ORANGE, BLACK, "*Global_cr3\t:%#018lx\n", *(unsigned long *)(phy_to_virt(global_cr3)) & (~0xff));
	color_printk(ORANGE, BLACK, "**Global_cr3\t:%#018lx\n", *(unsigned long *)phy_to_virt(*(unsigned long *)phy_to_virt(global_cr3) & (~0xff)) & (~0xff));

	for (i = 0; i < 10; i++) {
		*(unsigned long *)(phy_to_virt(global_cr3) + i) = 0UL;
	}

	flush_tlb();

}
