#include "printk.h"

void start_kernel(void)
{

	/* TODO: init_print should be initaionazed before kernel start */
	init_printk();
	color_printk(WHITE, BLACK, "Hello world!\n");

	while (1);
}
