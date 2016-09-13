#include <limits.h> /* PATH_MAX */
#include <stdlib.h>
#include <string.h>


/*
 * Get real path
 * Simple unifying wrapper
 */
const char *
real_path(const char *path) {
    char buf[PATH_MAX + 1] = {0};
    char *res = realpath(path, buf);
    if (res) {
        return strdup(res);
    }
	return NULL;
}
