#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/queue.h>

#include "config.h"
#include "kv.h"
#include "docket.h"
#include "options.h"
#include "abspath.h"
#include "report.h"
#include "strbuf.h"

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

    struct kvsrc *kv = NULL;

    char *source_accessor = build_path(DCT_CONFIG_SOURCES_TRIE_PATH, "*", "source");
    char *name_accessor = build_path(DCT_CONFIG_SOURCES_TRIE_PATH, "*", "name");

    config_path = (char *)get_config_path(config_path);
    if(kv_exists(config_path)) {
        kv = kv_load(config_path);
        if (kv_has(kv, name_accessor, name)) {
            die_error("Duplicate name");
        } else if (kv_has(kv, source_accessor, source_path)) {
            die_error("Duplicate source");
        } else {
            int max = 0;
            int found = 0;
            char max_str[24] = "";

            struct word_trie *root = kv_get(kv, DCT_CONFIG_SOURCES_TRIE_PATH);
            struct word_trie *swap_trie  = NULL;
            while((swap_trie = trie_loop_children(swap_trie, root))) {
                if((found = atoi(swap_trie->word)) > max) {
                    max = found;
                }
            }

            sprintf(max_str, "%d", max + 1);

            char *new_source_accessor = build_path(DCT_CONFIG_SOURCES_TRIE_PATH, max_str, "source");
            char *new_name_accessor = build_path(DCT_CONFIG_SOURCES_TRIE_PATH, max_str, "name");

            kv_add(kv, new_name_accessor, name);
            kv_add(kv, new_source_accessor, source_path);

            kv_sync(kv);
        }
    } else {
        kv = malloc(sizeof(struct kvsrc));
        kv->trie = trie_new();
        kv->origin = fopen(config_path, "w");

        char *new_source_accessor = build_path(DCT_CONFIG_SOURCES_TRIE_PATH, "0", "source");
        char *new_name_accessor = build_path(DCT_CONFIG_SOURCES_TRIE_PATH, "0", "name");

        kv_add(kv, new_name_accessor, name);
        kv_add(kv, new_source_accessor, source_path);

        kv_sync(kv);
    }

    return 1;
}
