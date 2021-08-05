#ifndef __PRINTK__
#define __PRINTK__

#include <stdarg.h>

struct position {
	int x_resolution;
	int y_resolution;

	int x_position;
	int y_position;

	int x_char_size;
	int y_char_size;

	unsigned int *fb_addr;
	unsigned long fb_len;
};

struct std_fmt {
	int flags;
	int filed_width;
	int precision;
	int qualifier;
};

#define WHITE  0x00ffffff
#define BLACK  0x00000000
#define RED    0x00ff0000
#define ORANGE 0x00ff8000
#define YELLOW 0x00ffff00
#define GREEN  0x0000ff00
#define BLUE   0x000000ff
#define INDIGO 0x0000ffff
#define RED    0x00ff0000
#define PURPLE 0x008000ff

#define ZEROPAD 1  /* pad with zero */
#define SIGN    2  /* unsigned/signed long */
#define PLUS    4  /* show plus */
#define SPACE   8  /* space if plus */
#define LEFT    16 /* left justified */
#define SPECIAL 32 /* 0x */
#define SMALL   64 /* use 'abcdef' instead of 'ABCDEF' */

void init_printk(void);
int color_printk(unsigned int fr_color, unsigned int bk_color, const char *fmt, ...);
void
putchar(unsigned int *fb, int x_size, int x, int y, unsigned int fr_color, unsigned int bk_color, unsigned char font);
int 
vsprintf(char *buf, const char *fmt, va_list args);
#endif
