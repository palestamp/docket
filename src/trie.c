#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strbuf.h"
#include "trie.h"

static void trie_printX(struct word_trie *t, int level);

struct word_trie *
trie_get(struct word_trie *t, const char *word) {
    if (t == NULL) {
        return NULL;
    }
    int i;
    struct word_trie *n = NULL;
    for (i = 0; i < t->len; i++) {
        n = *(t->edges + i);
        if(n != NULL && (strcmp(n->word, word) == 0)) {
            return n;
        }
    }
    return NULL;
}


void
trie_grow(struct word_trie *t) {
    // last pointer should be NULL
    if (t->cap <= t->len + 1) {
        struct word_trie **temp = NULL;
        temp = realloc(t->edges, sizeof(struct word_trie *) * (t->cap + 10));
        int i;
        for(i = t->cap ; i < t->cap + 10; i++) {
            *(temp + i) = NULL;
        }
        t->cap += 10;
        (t->edges) = temp;
    }
}


struct word_trie *
trie_new(void) {
    struct word_trie *n = NULL;
    n = malloc(sizeof(struct word_trie));
    n->len = 0;
    n->cap = 0;
    n->pos = 0;
    n->edges = NULL;
    n->word = NULL;
    TAILQ_INIT(&n->leafs);
    return n;
}


struct word_trie *
trie_add(struct word_trie *t, const char *word) {
    struct word_trie *n = trie_get(t, word);
    if (n == NULL) {
        n = trie_new();
        n->word = strdup(word);
        trie_grow(t);
        // save child's position
        n->pos = t->len;

        t->edges[n->pos] = n;
        t->len++;
    }
    return n;
}

struct word_trie *
trie_over_path_apply(trie_path_iter_cb cb, struct word_trie *t, const char *path) {
    struct word_trie *trie = t;
    char *cursor = (char *)path;

    char localbuffer[64] = {0};
    int l = 0;
    int r = 0;
    while(1) {
        if(*(cursor + r) == ':' || *(cursor + r) == '\0') {
            char *off_left = cursor + l;
            int loclen = r - l ;
            memcpy(localbuffer, off_left, loclen);
            localbuffer[loclen] = '\0';
            l = r + 1;
            trie = cb(trie, localbuffer);

            // for get operations only
            if (trie == NULL) {
                return NULL;
            }

            memset(localbuffer, 0, 64);
            if(*(cursor + r) == '\0') {
                break;
            }
        }
        r++;
    }
    return trie;
}


struct word_trie *
trie_insert_path(struct word_trie *t, const char *path) {
    return trie_over_path_apply(trie_add, t, path);
}

void trie_insert_by_path(struct word_trie *trie, const char *path, void *data) {
    struct word_trie *target_trie = trie_insert_path(trie, path);
    struct leaf_list *list = malloc(sizeof(struct leaf_list));
    list->data = data;
    TAILQ_INSERT_TAIL(&target_trie->leafs, list, leaf); 
}


struct word_trie *
trie_get_path(struct word_trie *t, const char *path) {
    return trie_over_path_apply(trie_get, t, path);
}


void
trie_sort(struct word_trie *t, int(*cmpfn)(const void *, const void *)) {
    qsort((void *)(t->edges), t->len, sizeof(struct word_trie *), cmpfn);
    int i;
    for(i = 0; i < t->len; i++) {
        trie_sort(*(t->edges + i), cmpfn);
    }
}


void 
trie_print(struct word_trie *t) {
    trie_printX(t, 0);
}


static void
trie_printX(struct word_trie *t, int l) {
    int i;
    if(t != NULL && t->word != NULL) {
        printf("%*s%s\n", l, "",  t->word);
        l++;
    }
    for(i= 0; i < t->len; i++) {
        trie_printX(*(t->edges + i), l);
    }
}



void
loop_stack_print(struct trie_loop *loop) {
    char *buf = NULL;
    int len = 0;
    int pos = 0;
    struct word_trie *l = NULL;
    char *format = "%s";
    TAILQ_FOREACH(l , &loop->stack, tries) {
        if (l->word) { 
            bufcat(&buf, &len, &pos, format, l->word);
            format= ":%s";
        }
    }
    printf("%s\n", buf);
    free(buf);
}


static struct trie_loop *
trie_loopX2(struct word_trie *trie, struct trie_loop *loop, loop_guard guard_fn) {
    if(TAILQ_EMPTY(&loop->stack)) {
        if(loop->pass != 0) {
            return NULL;
        }

        loop->pass = 1;
        TAILQ_INSERT_TAIL(&loop->stack, trie, tries);
        return trie_loopX2(trie, loop, guard_fn);
    } else {
        // get trie node to process
        struct word_trie *last = TAILQ_LAST(&loop->stack, loop_head); 


        // if latest node have right sibling - iterate over them
        // else - go to the upper level
        if(last->len > 0 && *(last->edges + loop->edge_offset)) {
            struct word_trie *newt = *(last->edges + loop->edge_offset);
            TAILQ_INSERT_TAIL(&loop->stack, newt, tries);
            loop->edge_offset = 0;
            if (guard_fn(newt)) {
                return loop;
            } else {
                return trie_loopX2(newt, loop, guard_fn);
            }
        } else {
            // we in leaf node
            // get position of current node in parent edges 
            int sibling_pos = last->pos;

            // remove current node
            TAILQ_REMOVE(&loop->stack, last, tries);

            // grab parent of current node
            last = TAILQ_LAST(&loop->stack, loop_head); 
            loop->edge_offset = sibling_pos + 1;
            // recursing case we already have this nodes
            return trie_loopX2(last, loop, guard_fn); 
        }
    }
}


struct trie_loop * 
trie_loop_branch(struct word_trie *t, struct trie_loop *loop, loop_guard guard_fn) {
    return trie_loopX2(t, loop, guard_fn);
}

int
trie_sort_path_label_asc(const void *a, const void *b) {
    if(a == NULL && b == NULL) {
        return 0;
    }

    // since we get pointer to poiner enforced by qsort we need to
    // dereference ptrptr and back one step from correct pointer type
    const char *aw = (*(struct word_trie **)a)->word;
    const char *bw = (*(struct word_trie **)b)->word;

    if(aw == NULL) {
        return -1;
    } else if (bw == NULL) {
        return 1;
    }
    return strcmp(aw, bw);
}


int
trie_sort_path_label_desc(const void *a, const void *b) {
    if(a == NULL && b == NULL) {
        return 0;
    }

    // since we get pointer to poiner enforced by qsort we need to
    // dereference ptrptr and back one step from correct pointer type
    const char *aw = (*(struct word_trie **)a)->word;
    const char *bw = (*(struct word_trie **)b)->word;

    if(aw == NULL) {
        return 1;
    } else if (bw == NULL) {
        return -1;
    }
    return -strcmp(aw, bw);
}



