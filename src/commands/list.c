
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
    struct word_trie *root = trie_get_path(c->trie, DCT_CONFIG_SOURCES_TRIE_PATH);

    struct word_trie *loop_trie = NULL;
    while((loop_trie = trie_loop_children(loop_trie, root))) {
        struct word_trie *trie = trie_get_path(loop_trie, "source");

        if (trie == NULL) {
            return 0;//DCT_NO_SOURCES_FOUND;
        }

        struct leaf_list *leaf = NULL;
        TAILQ_FOREACH(leaf, &trie->leafs, leaf) {
            if(access((const char *)leaf->data, R_OK) != 0) {
                return 0; // DCT_NO_ACCESS
            }

            struct docket_shelve *ds = docket_shelve_load_file((const char *)leaf->data);
            printf("Source found: %s\n", ds->source_path);

            struct trie_loop loop = {0};
            struct trie_loop *loop_ptr = &loop;
            TRIE_BRANCH_LOOP_INIT(&loop, NULL);

            while(1) {
                loop_ptr = trie_loop_branch(ds->trie, loop_ptr) ;
                if (loop_ptr == NULL) break;

                struct leaf_list *leaf_inner = NULL;
                struct word_trie *leafed_trie = TAILQ_LAST(&loop_ptr->stack, loop_head);

                TAILQ_FOREACH(leaf_inner, &leafed_trie->leafs, leaf) {
                    struct docket *ddd = ((struct docket *)leaf_inner->data);
                    char *title = docket_fix_title(ddd->title);
                    fprintf(stderr, "%s %s\n", ddd->head, docket_fix_title(ddd->title));
                    free(title);
                }
            }
        }
    }
    return 1;
}
