
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "kv.h"
#include "docket.h"
#include "options.h"
#include "trie.h"
#include "report.h"



struct tree_padding {
    int offcursor;
    int offsets[64];
    char buf[256];
};


static void print_offsets(struct tree_padding *padding);
static void trie_offset_advance(struct tree_padding *to, int len);
static void trie_offset_return(struct tree_padding *to);
static void trie_offset_invert_last(struct tree_padding *to);
static void print_head(const struct word_trie *trie_host, struct tree_padding *padding);
static void print_title(const struct word_trie *trie_host, struct tree_padding *padding);
static void print_docket_node(const struct word_trie *trie_host, struct tree_padding *padding);


/** Tree command
 *  Output branch structure as linux tree command do
 */
int
cmd_tree(int argc, const char **argv) {
    char *config_path = NULL;
    char *name = NULL;

    struct option tree_options[] = {
        {"c", "use-config", {.required = 0, .has_args = 1}, &config_path},
        {"n", "name",       {.required = 0, .has_args = 1}, &name},
        {0},
    };

    char err[1024] = "";
    int rv = options_populate(err, &argc, &argv, tree_options);
    if (rv != 0) {
        die_error("%s", err);
    }

    config_path = (char *)get_config_path(config_path);
    if(!kv_exists(config_path)) {
        die_error("No configuration found");
    }

    struct word_trie *host = trie_new();

    struct kvsrc *kv = kv_load(config_path);

    struct word_trie *root = kv_get(kv, DCT_CONFIG_SOURCES_TRIE_PATH);
    struct word_trie *loop_trie = NULL;
    while((loop_trie = trie_loop_children(loop_trie, root))) {
        struct word_trie *source_node = trie_get_path(loop_trie, "source");
        struct word_trie *name_node = trie_get_path(loop_trie, "name");

        if (source_node == NULL || name_node == NULL) {
            if (source_node != name_node) {
                die_fatal("Corrupted configuration data");
            }
            die_error("No source files found");
        }

        // filter sources by alias name
        if (name && (trie_has_value(name_node, cmp_str, name) == 0)) {
            continue;
        }

        const char *source = TAILQ_FIRST(&source_node->leafs)->data;
        const char *name = TAILQ_FIRST(&name_node->leafs)->data;

        if(access(source, R_OK) != 0) {
            die_error("No access to '%s'", (const char *)source);
        }

        struct docket_shelve *ds = docket_shelve_load_file(source);

        ds->trie->word = strdup(name);
        trie_append_child(host, ds->trie);
    }

    struct tree_padding padding = {.offsets = {0}, .buf = "", .offcursor = 1};
    print_docket_node(host, &padding);
    return 1;
}

static void
print_offsets(struct tree_padding *padding) {
    int i;
    fprintf(stdout, "[");
    for(i = 0; i < padding->offcursor; i++) {
        if(i > 0) {
            fprintf(stdout, ", ");
        }
        fprintf(stdout, "%d", padding->offsets[i]);
    }
    fprintf(stdout, "]\n");
}


static void
trie_offset_advance(struct tree_padding *to, int len) {
    to->offsets[to->offcursor] = len;
    to->offcursor++;
}


static void
trie_offset_return(struct tree_padding *to) {
    to->offsets[to->offcursor] = 0;
    to->offcursor--;
}


static void
trie_offset_invert_last(struct tree_padding *to) {
    if(to->offcursor > 0) {
        to->offsets[to->offcursor - 1] *= -1;
    }
}


static void
print_head(const struct word_trie *trie_host, struct tree_padding *padding) {
    char buf[264] = "";
    if(padding->offcursor > 1) {
        int i,j;
        int full = padding->offcursor;
        int buf_pos = 0;

        for(i = 0; i < (full - 1); i++) {
            int absoffset = abs(padding->offsets[i]);
            for(j = buf_pos; j < buf_pos + absoffset; j++) {
                buf[j] = ' ';
            }
            if (padding->offsets[i] > 0) {
                buf[buf_pos] = '|';
            }
            buf_pos += absoffset;
        }

        buf[buf_pos] = '+';
        memset(buf + buf_pos + 1, '-', abs(padding->offsets[i]) - 1);
    }
    fprintf(stdout, "%s%s\n", buf, trie_host->word);
}


static void
print_title(const struct word_trie *trie_host, struct tree_padding *padding) {
    char buf[264] = "";
    if(padding->offcursor > 1) {
        int i,j;
        int full = padding->offcursor;
        int buf_pos = 0;

        for(i = 0; i < full; i++) {
            int absoffset = abs(padding->offsets[i]);
            for(j = buf_pos; j < buf_pos + absoffset; j++) {
                buf[j] = ' ';
            }
            if (padding->offsets[i] > 0) {
                buf[buf_pos] = '|';
            }
            buf_pos += absoffset;
        }
    }

    struct leaf_list *leaf_inner = NULL;
    TAILQ_FOREACH(leaf_inner, &trie_host->leafs, leaf) {
        struct docket *ddd = ((struct docket *)leaf_inner->data);
        char *title = docket_fix_title(ddd->title);
        fprintf(stdout, "%s* %s (%lu)\n", buf, title, docket_get_body_len(ddd));
    }
    if (!TAILQ_EMPTY(&trie_host->leafs)) {
        fprintf(stdout, "%s\n", buf);
    }
}


static void
print_docket_node(const struct word_trie *trie_host, struct tree_padding *padding) {
    if (trie_host->word) {
        print_head(trie_host, padding);
        print_title(trie_host, padding);
        trie_offset_advance(padding, strlen(trie_host->word));
    }

    if(trie_host->len > 0) {
        struct word_trie *trie_child = NULL;
        int i;
        // recurse into each branch except last
        for (i = 0; i < (trie_host->len - 1); i++) {
            trie_child = *(trie_host->edges + i);
            print_docket_node(trie_child, padding);
            trie_offset_return(padding);
        }

        // recurse into last branch
        trie_child = *(trie_host->edges + i);
        trie_offset_invert_last(padding);
        print_docket_node(trie_child, padding);

        trie_offset_return(padding);
    }
}
