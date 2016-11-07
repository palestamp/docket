
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "strbuf.h"
#include "scanner.h"
#include "kv.h"
#include "docket.h"

int
kv_exists(const char *file_path) {
    if(access(file_path, R_OK | W_OK) == 0) {
        return 1;
    }
    return 0;
}


struct kvsrc *
kv_load(const char *file_path, void(*parse)(struct kvsrc *kv, struct scanner *s)) {
    struct kvsrc *kv = malloc(sizeof(struct kvsrc));
    kv->trie = trie_new();

    const char *mode = "r+";
    if (!kv_exists(file_path)) {
        mode = "w";
    }
    kv->origin = fopen(file_path, mode);

    struct stat sb;
    // if file has content - parse it
    if(stat(file_path, &sb) == 0 && sb.st_size > 0) {
        struct cfdmap *m = malloc(sizeof(struct cfdmap));
        struct scanner *s = malloc(sizeof(struct scanner));

        map(file_path, m);

        scanner_own_cfdm(s, m);

        while(!SCANNER_END(s)) {
            parse(kv, s);
        }
    }

    return kv;
}


void
kv_parse(struct kvsrc *kv, struct scanner *s) {
    char *cursor = SCANNER_CURSOR(s);
    if((s->pos == 0) || (*(cursor - 1) == '\n')) {
        /* if line starts with any non-alphabetic character - skip this line */
        if(!isalpha((int)(*cursor))) {
            SCANNER_ADVANCE(s);
            return;
        }

        char *ptr_delim = strchr(cursor, '=');
        char *alloced_path = copy_slice(cursor, ptr_delim - cursor);
        cursor = ptr_delim + 1;

        ptr_delim = strchr(cursor, '\n');
        char *alloced_data = copy_slice(cursor, ptr_delim - cursor);

        kv_add(kv, alloced_path, (void *)alloced_data);
        free(alloced_path);
        free(alloced_data);
    }
    SCANNER_ADVANCE(s);
}


void
kv_add(struct kvsrc *kv, const char * accessor_string, const char *value) {
    // XXX strdup here and in  kv_parse
    trie_insert_by_path(kv->trie, accessor_string, (void *)strdup(value));
}


struct word_trie *
kv_get(struct kvsrc *kv, const char * accessor_string) {
    return trie_get_path(kv->trie, accessor_string);
}


int
kv_has(struct kvsrc *kv, const char *filter, const char *value) {
    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    struct path_filter *pf = compile_filter_from_s(filter);
    TRIE_BRANCH_LOOP_INIT(&loop, pf);

    while((loop_ptr = trie_filter_branch(kv->trie, loop_ptr))) {
        struct word_trie *trie = LOOP_ONSTACK_TRIE(loop_ptr);
        if (trie_has_value(trie, cmp_str, (void *)value)) {
            return 1;
        }
    }
    return 0;
}


int
kv_flush(struct kvsrc *kv, char **buf) {
    int len = 0;
    int pos = 0;

    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    TRIE_BRANCH_LOOP_INIT(&loop, NULL);

    while((loop_ptr = trie_loop_branch(kv->trie, loop_ptr))) {
        bufcat(buf,  &len, &pos, "%s", loop_stack_sprint_kv(loop_ptr));
    }
    return 1;
}


int
kv_sync(struct kvsrc *kv) {
    char *buf = NULL;
    kv_flush(kv, &buf);
    fwrite(buf, 1, strlen(buf), kv->origin);
    free(buf);

    return 1;
}
