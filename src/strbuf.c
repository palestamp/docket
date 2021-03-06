#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define SLICE_MAX_BUF 1024

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


char *
copy_slice(const char *start_ptr, size_t len) {
    char *buffer = malloc(len + 1);
    memcpy(buffer, start_ptr, len);
    buffer[len] = '\0';
    return buffer;
}


char *
_build_path(int dummy, ...) {
    va_list ch;
    char *key = NULL;

    char *out = NULL;
    int pos = 0;
    int len = 0;

    char *delimiter = "";
    va_start(ch, dummy);
    while((key = va_arg(ch, char *)) != NULL) {
        bufcat(&out, &len, &pos, "%s%s", delimiter, key);
        delimiter = ":";
    }
    va_end(ch);
    return out;
}


int
startswith(const char *prefix, const char *str) {
    return strncmp(prefix, str, strlen(prefix)) == 0;
}
