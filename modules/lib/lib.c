#include "lib.h"
#if 1
void * memset(void *address, unsigned char c, long cnt)
{

	char *tmp = (char *)address;

	while (cnt--) {
		*tmp++ = c;
	}
}
#endif
