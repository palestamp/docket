#include <stdlib.h>
#include <stdio.h>

#include "options.h"

/*
 * Create config file in HOME directory
 */
int create_config_file() {
    const char *home = getenv("HOME");
    printf("%s\n", home);
    return 1;
}

/*
 * Register new docket file in config
 */
int 
cmd_add(int argc, const char **argv) {
    const struct option add_options[] = {
        {0},
    };

    return create_config_file();
}
