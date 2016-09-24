#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "options.h"
#include <sys/queue.h>
/*
 * Register new docket file in config
 */
int 
cmd_add(int argc, const char **argv) {
    const struct option add_options[] = {
        {0},
    };
    
    

    argc--;
    if (argc < 1) {
        return 0;
    }

    argv++;
    struct config *c = NULL;
    if(config_exists()) {
    printf("CASE 1");
        c = config_load();
        if(config_has(c, "docket:settings:sources", argv[0])) {
    printf("CASE 1.1");
            config_free(c);
        } else {
    printf("CASE 1.2");
            config_add(c, "docket:settings:sources", argv[0]);
            config_sync(c);
            config_free(c);
        }
    } else {
    printf("CASE 2");
        c = config_create();
        config_add(c, "docket:settings:sources", argv[0]);
        config_sync(c);
        config_free(c);

    }
    return 1;
}
