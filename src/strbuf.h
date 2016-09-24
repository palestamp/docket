#ifndef _H_STRBUF
#define _H_STRBUF


#include <stdarg.h>

int bufcat(char **buf, int *buf_len, int *buf_pos, const char *fmt, ...);
char * copy_slice(const char *start_ptr, size_t len);

#endif
