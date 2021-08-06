#ifndef __MEM_H__
#define __MEM_H__

struct memory_e820 {
	unsigned int addr1;
	unsigned int addr2;
	unsigned int length1;
	unsigned int length2;
	unsigned int type;
};

void init_memory();

#endif
