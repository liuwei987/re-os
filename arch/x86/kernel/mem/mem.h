#ifndef __MEM_H__
#define __MEM_H__

#include "lib.h"

#define PTRS_PER_PAGE 512UL
#define LINEAR_OFFSET ((unsigned long)0xffff800000000000)

//#define PAGE_GDT_SHIFT 39
#define PAGE_4K_SHIFT		12UL
#define PAGE_2M_SHIFT		21UL
#define PAGE_1G_SHIFT		30UL

#define PAGE_4K			(1UL << PAGE_4K_SHIFT)
#define PAGE_2M			(1UL << PAGE_2M_SHIFT)
#define PAGE_4K_MASK		(~(PAGE_4K - 1UL))
#define PAGE_2M_MASK		(~(PAGE_2M - 1UL))
#define PAGE_4K_ALIGN(x)	((x + PAGE_4K - 1UL) & PAGE_4K_MASK)
#define PAGE_2M_ALIGN(x)	((x + PAGE_2M - 1UL) & PAGE_2M_MASK)

#define virt_to_phy(addr)	((unsigned long)addr - LINEAR_OFFSET)
#define phy_to_virt(addr)	((unsigned long)addr + LINEAR_OFFSET)

#define flash_tlp()					\
do {							\
	unsigned long *tmpreg;				\
	__asm__ __volatile__("movq %%cr3, %0\n\t"	\
				"movq %0, %%cr3\n\t"	\
				:"=r"(tmpreg)		\
				::"memory");		\
} while(0)

inline unsigned long *get_gdt()
{	
	unsigned long *gdt;
	__asm__ __volatile__("movq %%cr3, %0\n\t"
				:"=r"(gdt)
				::"memory");
}

struct memory_e820_fmt {
	unsigned int addr1;
	unsigned int addr2;
	unsigned int length1;
	unsigned int length2;
	unsigned int type;
};

struct E820 {
	unsigned long address;
	unsigned long length;
	unsigned int type;
}__attribute__((packed));

struct Zone;
struct Page;

struct global_mem_descriptor {
	struct E820 e820[32];
	unsigned long e820_length;

	unsigned long *bits_map;
	unsigned long bits_size;
	unsigned long bits_length;

	struct Page *page_struct;
	unsigned long pages_size;
	unsigned long pages_length;

	struct Zone *zone_struct;
	unsigned long zone_size;
	unsigned long zone_length;

	unsigned long start_code, end_code, end_data, end_brk;
	unsigned long end_of_struct;
};

struct Page {
	struct Zone *zone;
	unsigned long phy_addr;
	unsigned long attribute;
	unsigned long ref_count;
	unsigned long age;
};

struct Zone {
	struct Page *pages_group;
	unsigned long pages_length;

	unsigned long zone_start_addr;
	unsigned long zone_end_addr;
	unsigned long zone_length;
	unsigned long attribute;

	struct global_mem_descriptor *GMD_struct;

	unsigned long page_using_count;
	unsigned long page_free_count;
	unsigned long total_page_link;
};

void init_memory();

extern struct global_mem_descriptor global_mem_struct;
unsigned int ZONE_NORMAL_INDEX = ZONE_DMA_INDEX ZONE_UNMAPED_INDEX = 0;
#endif
