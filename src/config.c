#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "config.h"
#include "strbuf.h"
#include "scanner.h"
#include "docket.h"


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


int
config_exists(const char *cpath) {
    const char *config_path = get_config_path(cpath);
    if(access(config_path, R_OK | W_OK) == 0) {
        struct stat sb;
        if(stat(config_path, &sb) == 0 && sb.st_size > 0) {
            free((char *)config_path);
            return 1;
        }
    }

    free((char *)config_path);
    // XXX check errno in caller
    return 0;
}


int
create_config_file(const char *cpath) {
    FILE *config = config_open(cpath, "a+");
    fclose(config);
    return 1;
}


int
config_sync(struct config *c) {
    char *buf = NULL;
    config_flush(c, &buf);
    fwrite(buf, 1, strlen(buf), c->cf);
    free(buf);
    return 1;
}


struct config *
config_create(const char *cpath) {
    struct config *c = malloc(sizeof(struct config));
    c->trie = trie_new();
    c->cf = config_open(cpath, "w");
    return c;
}


FILE *
config_open(const char *cpath, const char *flags) {
    const char *config_path = get_config_path(cpath);
    FILE *cf =  fopen(config_path, flags);
    free((void *)config_path);
    return cf;
}


struct config *
config_load(const char *cpath) {
    struct config *c = malloc(sizeof(struct config));
    c->trie = trie_new();
    c->cf = config_open(cpath, "r+");

    struct cfdmap *m = malloc(sizeof(struct cfdmap));
    struct scanner *s = malloc(sizeof(struct scanner));

    const char *config_path = get_config_path(cpath);

    map(config_path, m);
    free((void *)config_path);

    scanner_own_cfdm(s, m);

    char *cursor = (char *)SCANNER_CURSOR(s);

    while(*cursor) {
        if((s->pos == 0) || (*(cursor - 1) == '\n')) {
            /* if line starts with any non-alphabetic character - skip this line */
            if(!isalpha((int)(*cursor))) {
                continue;
            }

            char *ptr_delim = strchr(cursor, '=');
            char *alloced_path = copy_slice(cursor, ptr_delim - cursor);
            cursor = ptr_delim + 1;

            ptr_delim = strchr(cursor, '\n');
            char *alloced_data = copy_slice(cursor, ptr_delim - cursor);

            trie_insert_by_path(c->trie, alloced_path, (void *)alloced_data);
            free(alloced_path);
        }
        SCANNER_ADVANCE(s);
        cursor++;
    }

    return c;
}


int
config_add(struct config *c, const char * accessor_string, const char *value) {
    trie_insert_by_path(c->trie, accessor_string, (void *)strdup(value));
    return 1;
}




int config_has(struct config *c, const char *filter, const char *value) {
    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    struct path_filter *pf = compile_filter_from_s(filter);
    TRIE_BRANCH_LOOP_INIT(&loop, pf);
    while((loop_ptr = trie_filter_branch(c->trie, loop_ptr))) {
        struct word_trie *t = LOOP_ONSTACK_TRIE(loop_ptr);
        if (trie_has_data(t, cmp_str, (void *)value)) {
            return 1;
        }
    }
    return 0;
}


int
config_has_source(struct config *c, const char * accessor_string, const char *value) {
    struct word_trie *t = NULL;
    // get common sources node
    if((t = trie_get_path(c->trie, accessor_string)) && t->len > 0) {

        // iterate over all numbered childeren
        struct word_trie *loop_trie = NULL;
        while((loop_trie = trie_loop_children(loop_trie, t))) {

            // search for source-name pairs
            struct word_trie *source_node = NULL;
            struct word_trie *name_node = NULL;
            if((source_node = trie_get_path(loop_trie, "source")) &&
                    (name_node = trie_get_path(loop_trie, "name"))) {

                // search for targeted source
                struct leaf_list *lfls = NULL;
                TAILQ_FOREACH(lfls, &source_node->leafs, leaf) {
                    if(!cmp_str((void *)value, lfls->data)) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}



int
config_flush(struct config *c, char **buf) {
    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    int len = 0;
    int pos = 0;
    TRIE_BRANCH_LOOP_INIT(&loop, NULL);
    while(1) {
        // WARN be careful with trie_filter_branch_end
        loop_ptr = trie_loop_branch(c->trie, loop_ptr) ;
        if (loop_ptr == NULL) break;
        bufcat(buf,  &len, &pos, "%s", loop_stack_sprint_kv(loop_ptr));
    }
    return 1;
}


void
config_free(struct config *c) {
    return ;
}

