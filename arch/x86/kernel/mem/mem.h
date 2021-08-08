#ifndef __MEM_H__
#define __MEM_H__

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

struct global_mem_descriptor {
	struct E820 e820[32];
	unsigned long e820_length;
};

void init_memory();

extern struct global_mem_descriptor global_mem_struct;
#endif
