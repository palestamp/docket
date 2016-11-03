
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "docket.h"
#include "options.h"
#include "trie.h"


int
cmd_list(int argc, const char **argv) {
    const struct option list_options[] = {
        {0},
    };

    struct config *c = NULL;
    if(!config_exists(NULL)) {
        return 0;//DCT_NO_CONFIG_FOUND;
    }
    c = config_load(NULL);
    struct word_trie *source_root = trie_get_path(c->trie, DCT_CONFIG_SOURCES_TRIE_PATH);

    struct word_trie *loop_trie = NULL;
    while((loop_trie = trie_loop_children(loop_trie, source_root))) {
        struct word_trie *source_node = trie_get_path(loop_trie, "source");
        struct word_trie *name_node = trie_get_path(loop_trie, "name");

        if (source_node == NULL || name_node == NULL) {
            if (source_node != name_node) {
                die_error("Corrupted configuration data");
            }
            return 0;//DCT_NO_SOURCES_FOUND;
        }

        const char *source = TAILQ_FIRST(&source_node->leafs)->data;
        const char *name = TAILQ_FIRST(&name_node->leafs)->data;

        if(access(source, R_OK) != 0) {
            return 0; // DCT_NO_ACCESS
        }

        struct docket_shelve *ds = docket_shelve_load_file(source);
        printf("Source found: %s\n", ds->source_path);

        struct trie_loop loop = {0};
        struct trie_loop *loop_ptr = &loop;
        TRIE_BRANCH_LOOP_INIT(&loop, NULL);

        while((loop_ptr = trie_loop_branch(ds->trie, loop_ptr))) {
            struct leaf_list *leaf_inner = NULL;
            struct word_trie *leafed_trie = LOOP_ONSTACK_TRIE(loop_ptr);

            TAILQ_FOREACH(leaf_inner, &leafed_trie->leafs, leaf) {
                struct docket *dnode = ((struct docket *)leaf_inner->data);
                char *title = docket_fix_title(dnode->title);
                fprintf(stdout, "%s %s\n", dnode->head, docket_fix_title(dnode->title));
                free(title);
            }
        }
    }
    return 1;
}
