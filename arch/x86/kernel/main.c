#include "printk.h"

void start_kernel(void)
{

	int i;
#if 0
#define X_DOT (1440*20)
	int *frame_buf = (int *)0xffff800000a00000;
	/* red line */
	for (i = 0; i < X_DOT; i ++) {
		*((char *)frame_buf + 0) = (char)0x00;
		*((char *)frame_buf + 1) = (char)0x00;
		*((char *)frame_buf + 2) = (char)0xff;
		*((char *)frame_buf + 3) = (char)0x00;
		frame_buf++;
	}
	/* green line */
	for (i = 0; i < X_DOT; i ++) {
		*((char *)frame_buf + 0) = (char)0x00;
		*((char *)frame_buf + 1) = (char)0xff;
		*((char *)frame_buf + 2) = (char)0x00;
		*((char *)frame_buf + 3) = (char)0x00;
		frame_buf++;
	}
	/* blue line */
	for (i = 0; i < X_DOT; i ++) {
		*((char *)frame_buf + 0) = (char)0xff;
		*((char *)frame_buf + 1) = (char)0x00;
		*((char *)frame_buf + 2) = (char)0x00;
		*((char *)frame_buf + 3) = (char)0x00;
		frame_buf++;
	}
	/* light line */
	for (i = 0; i < X_DOT; i ++) {
		*((char *)frame_buf + 0) = (char)0xff;
		*((char *)frame_buf + 1) = (char)0xff;
		*((char *)frame_buf + 2) = (char)0xff;
		*((char *)frame_buf + 3) = (char)0x00;
		frame_buf++;
	}
#else
	pos.x_resolution = 1440;
	pos.y_resolution = 990;

	pos.x_position = 0;
	pos.y_position = 0;

	pos.x_char_size = 8;
	pos.y_char_size = 16;

	pos.fb_addr = (int *)0xffff800000a00000;
	pos.fb_len = (pos.x_resolution * pos.x_char_size * 4);

	color_printk(WHITE, BLACK, "Hello world!\n");
#endif
	while (1);
}
