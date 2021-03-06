#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void
vreportf(const char *prefix, const char *err, va_list params) {
	FILE *fh = stderr;
	fflush(fh);

	fputs(prefix, fh);
	vfprintf(fh, err, params);
	fputc('\n', fh);
}

void
die_error(const char *err, ...) {
	va_list params;
	va_start(params, err);
	vreportf("error: ", err, params);
	va_end(params);
    exit(128);
}

void
die_fatal(const char *err, ...) {
	va_list params;
	va_start(params, err);
	vreportf("fatal: ", err, params);
	va_end(params);
    exit(128);
}

void
usage_and_die(const char *err, ...) {
	va_list params;
	va_start(params, err);
	vreportf("usage: ", err, params);
	va_end(params);
    exit(128);
}
