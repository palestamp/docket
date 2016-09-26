#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/queue.h>

#include "config.h"
#include "options.h"
#include "abspath.h"

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
    const char *source_path = real_path(argv[0]);

    if(source_path == NULL) {
        fprintf(stderr, "ERROR: No such file: %s\n", argv[0]);
        return 0;
    }

    if(config_exists()) {
        c = config_load();
        if(config_has(c, DCT_CONFIG_SOURCES_TRIE_PATH, source_path)) {
            config_free(c);
        } else {
            config_add(c, DCT_CONFIG_SOURCES_TRIE_PATH, source_path);
            config_sync(c);
            config_free(c);
        }
    } else {
        c = config_create();
        config_add(c, DCT_CONFIG_SOURCES_TRIE_PATH, source_path);
        config_sync(c);
        config_free(c);

    }
    return 1;
}
