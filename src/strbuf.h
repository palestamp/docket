#ifndef _H_STRBUF
#define _H_STRBUF

#include <stdarg.h>
#include <string.h>

int bufcat(char **buf, int *buf_len, int *buf_pos, const char *fmt, ...);
char *copy_slice(const char *start_ptr, size_t len);
char *_build_path(int dummy, ...);
int startswith(const char *prefix, const char *str);

#define build_path(...) _build_path(0, __VA_ARGS__, NULL)

#endif
