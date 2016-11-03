#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/queue.h>

#include "config.h"
#include "options.h"
#include "abspath.h"
#include "report.h"

/*
 * Register new docket file in config
 */
int
cmd_add(int argc, const char **argv) {
    char *config_path = NULL;
    char *name = NULL;

    struct option add_options[] = {
        {"c", "use-config", {.required = 0, .has_args = 1}, &config_path},
        {"n", "name",       {.required = 1, .has_args = 1}, &name},
        {0},
    };

    char err[1024] = "";
    int rv = options_populate(err, &argc, &argv, add_options);
    if (rv != 0) {
        die_error("%s", err);
    }

    if (argc < 1) {
        die_error("Not enough arguments");
    }

    const char *source_path = real_path(argv[0]);

    if(source_path == NULL) {
        die_error("No such file: %s", argv[0]);
    }

    struct config *c = NULL;
    if(config_exists(config_path)) {
        c = config_load(config_path);
        if(config_has_source(c, DCT_CONFIG_SOURCES_TRIE_PATH, source_path)) {
            config_free(c);
        } else {
            config_add_source(c, DCT_CONFIG_SOURCES_TRIE_PATH, source_path);
            config_sync(c);
            config_free(c);
        }
    } else {
        c = config_create(config_path);
        config_add_source(c, DCT_CONFIG_SOURCES_TRIE_PATH, source_path);
        config_sync(c);
        config_free(c);
    }

    return 1;
}
