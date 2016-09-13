#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Originaly written by Pelle Johansson 
 */
int
bufcat(char **buf, int *buf_len, int *buf_pos, const char *fmt, ...) {
	int res;
	int remaining = *buf_len - *buf_pos;
	va_list ap, apc;

	if (!*buf) {
		*buf = malloc(128);
		*buf_len = 128;
		*buf_pos = 0;
		remaining = 128;
	}

	va_start(ap, fmt);
	va_copy(apc, ap);
	res = vsnprintf(*buf + *buf_pos, remaining, fmt, apc);
	va_end(apc);
	if (res >= remaining) {
		/* Data truncated, increase buffer */
		char *tmp = realloc(*buf, *buf_len + (res + 1) * 2);

		*buf = tmp;
		*buf_len += (res + 1) * 2;
		remaining += (res + 1) * 2;
		va_copy(apc, ap);
		res = vsnprintf(*buf + *buf_pos, remaining, fmt, apc);
		va_end(apc);
	}
	*buf_pos += res;

	va_end(ap);
	return res;
}
