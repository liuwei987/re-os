
#define X_DOT (1440*20)

void start_kernel(void)
{

	int *frame_buf = (int *)0xffff800000a00000;
	int i;
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
	while (1);
}
