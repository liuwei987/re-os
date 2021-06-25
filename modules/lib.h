#ifndef __LIB_H__
#define __LIB_H__

#define NULL 0

int strlen(char *str)
{
	register int __res;
	__asm__ __volatile__ ( "cld \n\t"
				"repne \n\t"
				"scasb \n\t"
				"notl %0 \n\t"
				"decl %0 \n\t"
				:"=c"(__res)
				:"D"(str), "a"(0), "0"(0xffffffff)
			);

	return __res;
}


#endif
