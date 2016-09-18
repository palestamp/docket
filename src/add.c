#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "options.h"

/*
 * Register new docket file in config
 */
int 
cmd_add(int argc, const char **argv) {
    const struct option add_options[] = {
        {0},
    };
    struct config *c = NULL;
    if(config_exists()) {
        c = config_load();
        if(config_has(c, "sources", argv[0])) {
            config_free(c);
        } else {
            printf("%s\n", argv[0]);
            config_add(c, "sources", argv[0]);
        }
            /*
            config_flush(c);
            config_free(c);
        }
    } else {
        c = config_create();
//        config_add(c, "sources", argv[0]);
 //       config_flush(c);
  //      config_free(c);
  */
    }
    return 1;
}
