#include <stdint.h>
#include <stdarg.h>

static int do_sprintf (char *str, const char *format, va_list argp)
{
	char c, *s = str;
	char hex[] = "0123456789ABCDEF";

	if (!format)
		return -1;

	while ((c = *format++) != 0)
	{
		switch (c)
		{
		case '%':
			switch (*format++) {
			case 0:
				break;

			case '%':
				*s++ = '%';
				break;

			case 'c':
				*s++ = va_arg(argp, int);
				break;

			case 'd': {
				uint32_t div;
				int32_t d = va_arg(argp, int32_t);
				if (d < 0) {
					*s++ = '-';
					d = -d;
				}
				if (d == 0) {
					*s++ = '0';
					break;
				}
				for (div = 1; div * 10 <= (uint32_t) d; div *= 10)
					continue;

				while (div) {
					uint8_t digit = d / div;
					d -= div * digit;
					div /= 10;
					*s++ = '0' + digit;
				}
				break;
			}

			case 's': {
				const char *p = va_arg(argp, char *);
				while (*p)
					*s++ = *p;
				break;
			}

			case 'x': {
				uint32_t div, x = va_arg(argp, uint32_t);
				if (x == 0) {
					*s++ = '0';
					break;
				}
				for (div = 1; div * 16 <= x; div *= 16)
					continue;

				while (div) {
					uint8_t digit = x / div;
					x -= div * digit;
					div /= 16;
					*s++ = hex[digit];
				}
				break;
			}
			}
			break;

		default:
			*s++ = c;
		}
	}

	// Null-terminate string:
	*s = '\0';

	// Return printed characters minus null terminator:
	return s - str - 1;
}

int sprintf (char *str, const char *format, ...)
{
	va_list argp;

	va_start(argp, format);
	int ret = do_sprintf(str, format, argp);
	va_end(argp);

	return ret;
}
