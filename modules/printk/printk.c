#include "printk.h"
#include "lib.h"
#include "font.h"
#include "mem.h"

#define PRINT_BUF 4096
#define is_digit(c) ((c) >= '0' && (c) <= '9')
#define do_div(n, base) ({ \
			int __res; \
			__asm__ ("divq %%rcx" \
				:"=a"(n), "=d"(__res) \
				:"0"(n), "1"(0), "c"(base)); \
			__res; \
			})

struct position pos;

static int skip_atoi(const char **s)
{
	int i = 0;
	while (is_digit(**s))
		i = i * 10 + *((*s)++) - '0';

	return i;
}

static char
*number(char *str, long num, int base, struct std_fmt opt)
{
	char c, sign = 0, tmp[50];
	const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i = 0;

	if (opt.flags & SMALL)
		digits = "0123456789abcdefghijklmnopqrstuvwxyz";

	if (opt.flags & LEFT)
		opt.flags &= ~ZEROPAD;

	if (base < 2 || base > 36)
		return 0;

	c = (opt.flags & ZEROPAD) ? '0': ' ';
	if (opt.flags & SIGN && num < 0) {
		sign = '-';
		num = -num;
	} else {

		sign = (opt.flags & PLUS) ? '+': ((opt.flags & SPACE)? ' ': 0);
	}

	if (sign)
		opt.filed_width--;
	if (opt.flags & SPECIAL) {
		if (base = 16)
			opt.flags -= 2;
		else if (base == 8)
			opt.flags--;
	}

	if (num == 0)
		tmp[i++] = '0';
	else
		while (num != 0)
			tmp[i++] = digits[do_div(num, base)];

	if (i > opt.precision)
		opt.precision = i;
	opt.filed_width -= opt.precision;

	if (!(opt.flags & (ZEROPAD + LEFT))) {
		while (opt.filed_width-- > 0)
		*str++ = ' ';
	}

	if (sign)
		*str++ = sign;

	if (opt.flags & SPECIAL) {
		if (base == 8) {
			*str++ = '0';
		} else if (base == 16) {
			*str++ = '0';
			*str++ = digits[33];
		}
	}

	if (!(opt.flags & LEFT)) {
		while (opt.filed_width-- > 0) {
			*str++ = c;
		}
	}

	while (i < opt.precision--)
		*str++ = '0';

	while (i-- > 0)
		*str++ = tmp[i];

	while (opt.filed_width-- > 0)
		*str++ = ' ';

	return str;
}

int color_printk(unsigned int fr_color, unsigned int bk_color, const char *fmt, ...)
{
	int i = 0, count =0, line = 0;
	va_list aq;
	char buf[PRINT_BUF];
	va_start(aq, fmt);
	i = vsprintf(buf, fmt, aq);
	va_end(aq);

	for (count = 0; count < i || line; count ++) {
		if (line > 0) {
			count--;
			goto tab_space;
		}

		if ((unsigned char)*(buf + count) == '\n') {
			pos.x_position = 0;
			pos.y_position++;
		} else if ((unsigned char)*(buf + count) == '\b') {
			pos.x_position--;
			if (pos.x_position < 0) {
				pos.y_position--;
				pos.x_position = (pos.x_resolution / pos.x_char_size - 1) * pos.x_char_size;
				if (pos.y_position < 0) {
					pos.y_position = (pos.y_resolution / pos.y_char_size - 1) * pos.y_char_size;
				}
			}
			putchar(pos.fb_addr, pos.x_resolution, pos.x_position * pos.x_char_size, pos.y_position * pos.y_char_size, fr_color, bk_color, ' ');
		} else if ((unsigned char)*(buf + count) == '\t') {

			line = ((pos.x_position + 8) & ~(8 - 1)) - pos.x_position;
		tab_space:
			line--;
			putchar(pos.fb_addr, pos.x_resolution, pos.x_position * pos.x_char_size, pos.y_position * pos.y_char_size, fr_color, bk_color, ' ');
		} else {

			putchar(pos.fb_addr, pos.x_resolution, pos.x_position * pos.x_char_size, pos.y_position * pos.y_char_size, fr_color, bk_color, (unsigned char)*(buf + count));
			pos.x_position++;
		}

		if (pos.x_position >= pos.x_resolution / pos.x_char_size) {
			pos.x_position = 0;
			pos.y_position++;
		}
		if (pos.y_position >= pos.y_resolution / pos.y_char_size) {
			pos.y_position = 0;
		}
	}
}

void
putchar(unsigned int *fb, int x_size, int x, int y, unsigned int fr_color, unsigned int bk_color, unsigned char font)
{
	int i = 0, j = 0;
	unsigned int *addr = NULL;
	unsigned char *fontp = NULL;
	int testval = 0;
	fontp = font_ascii[font];

	for (i = 0; i < 16; i ++) {
		testval = 0x100;
		addr = fb + x_size * (y + i) + x;
		for (j = 0; j < 8; j ++) {
			testval = testval >> 1;
			if (*fontp & testval) {
				*addr = fr_color;
			} else {
				*addr = bk_color;
			}
			addr++;
		}
		fontp++;
	}
}

/*
 *
 * @pre *fmt: %[flags][width][.perc] [F|N|h|l]type
 * flags: none,'-','+','#'0n(n=0,1,2...)
 */
int vsprintf(char *buf, const char *fmt, va_list args)
{
	char *str, *s;
	int len, i;
	struct std_fmt opt;

	for (str = buf; *fmt; fmt++) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}

		opt.flags = 0;
		repeat:
			fmt++;
			switch (*fmt) {
				case '-': opt.flags |= LEFT;
				goto repeat;
				case '+': opt.flags |= PLUS;
				goto repeat;
				case ' ': opt.flags |= SPACE;
				goto repeat;
				case '#': opt.flags |= SPECIAL;
				goto repeat;
				case '0': opt.flags |= ZEROPAD;
				goto repeat;
			}

		opt.filed_width = -1;
		if (is_digit(*fmt)) {
			opt.filed_width = skip_atoi(&fmt);
		} else if (*fmt == '*'){
			fmt++;
			opt.filed_width = va_arg(args, int);
			if (opt.filed_width < 0) {
				opt.filed_width = -opt.filed_width;
				opt.flags |= LEFT;
			}
		}

		opt.precision = -1;
		if (*fmt == '.') {
			fmt++;
			if (is_digit(*fmt)) {
				opt.precision = skip_atoi(&fmt);
			} else if (*fmt == '*') {
				fmt++;
				opt.precision = va_arg(args, int);
			}
			if (opt.precision < 0) {
				opt.precision = 0;
			}
		}

		opt.qualifier = -1;
		if (*fmt == 'l' || *fmt == 'h' || *fmt == 'L' || *fmt == 'Z') {
			opt.qualifier = *fmt;
			fmt++;
		}

		switch (*fmt) {
			case 'c':
				if (!(opt.flags & LEFT)) {
					while (--opt.filed_width > 0) {
						*str++ = ' ';
					}
				}
				*str++ = (unsigned char)va_arg(args, int);
				while (--opt.filed_width > 0) {
					*str++ = ' ';
				}
				break;
			case 's':
				s = va_arg(args, char *);
				if (!s)
					s = '\0';
				len = strlen(s);
				if (opt.precision < 0) {
					opt.precision = len;
				} else if (len > opt.precision) {
					len = opt.precision;
				}

				if (!(opt.flags & LEFT)) {
					while (len < opt.filed_width--) {
						*str++ = ' ';
					}
				}

				for (i = 0; i < len; i++) {
					*str++ = *s++;
				}
				while (len < opt.filed_width--) {
					*str++ = ' ';
				}
				break;
			case 'o':
				if (opt.qualifier == 'l')
					str = number(str, va_arg(args, unsigned long), 8, opt);
				else
					str = number(str, va_arg(args, unsigned int), 8, opt);
				break;
			case 'p':
				if (opt.filed_width == -1) {
					opt.filed_width = 2 * sizeof(void *);
					opt.flags |= ZEROPAD;
				}
				str = number(str, (unsigned long)va_arg(args, void *), 16, opt);
				break;
			case 'x':
				opt.flags |= SMALL;
			case 'X':
				if (opt.qualifier == 'l') {
					str = number(str, va_arg(args, unsigned long), 16, opt);
				} else {
					str = number(str, va_arg(args, unsigned int), 16, opt);
				}
				break;
			case 'd':
			case 'i':
				opt.flags |= SIGN;
			case 'u':
				if (opt.qualifier == 'l')
					str = number(str, va_arg(args, unsigned long), 10, opt);
				else
					str = number(str, va_arg(args, unsigned int), 10, opt);
				break;
			case 'n':
				if (opt.qualifier == 'l') {
					long *ip = va_arg(args, long *);
					*ip = (str - buf);
				} else {
					int *ip = va_arg(args, int *);
					*ip = (str - buf);
				}
				break;
			case '%':
				*str++ = '%';
				break;
			default:
				*str++ = '%';
				if (*fmt) {
					*str++ = *fmt;
				} else {
					fmt--;
				}
				break;
		}
	}

	*str = '\0';
	return str -buf;
}

void init_printk(void)
{
#if 1
	pos.x_resolution = 1440;
	pos.y_resolution = 990;

	pos.x_position = 0;
	pos.y_position = 0;

	pos.x_char_size = 8;
	pos.y_char_size = 16;

	pos.fb_addr = (int *)0xffff800000a00000;
	pos.fb_len = PAGE_4K_ALIGN(pos.x_resolution * pos.x_char_size * 4);
#else
#define X_DOT (1440*20)
	int i;
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
#endif
}
