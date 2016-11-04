#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "config.h"


#ifndef DOCKET_CONFIG_NAME
#define DOCKET_CONFIG_NAME ".docket"
#endif

const char *
get_config_path(const char *override) {
    char pathbuf[PATH_MAX] = {0};
    if (override) {
        sprintf(pathbuf, "%s", override);
    } else {
        sprintf(pathbuf, "%s/%s", getenv("HOME"), DOCKET_CONFIG_NAME);
    }
    return (const char *)strdup(pathbuf);
}




