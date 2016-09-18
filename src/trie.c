#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

static void trie_printX(struct word_trie *t, int level);

struct word_trie *
trie_get(struct word_trie *t, const char *word) {
    int i;
    struct word_trie *n = NULL;
    for (i = 0; i < t->len; i++) {
        n = *(t->edges + i);
        if(n != NULL && (strcmp(n->word, word) == 0)) {
            printf("IN TRIE_GET %s\n", n->word);
            return n;
        }
    }
    return NULL;
}


void
trie_grow(struct word_trie *t) {
    struct word_trie **temp = NULL;
    temp = realloc(t->edges, sizeof(struct word_trie *) * (t->cap + 10));
    int i;
    for(i = 0; i < t->cap + 10; i++) {
        *(temp + i) = NULL;
    }
    (t->edges) = temp;
}


struct word_trie *
trie_new(void) {
    struct word_trie *n = NULL;
    n = malloc(sizeof(struct word_trie));
    n->len = 0;
    n->cap = 0;
    n->edges = NULL;
    n->word = NULL;
    n->data = NULL;
    return n;
}


struct word_trie *
trie_add(struct word_trie *t, const char *word) {
    struct word_trie *n = trie_get(t, word);
    if (n == NULL) {
        n = trie_new();
        n->word = strdup(word);
        trie_grow(t);

        t->edges[t->len] = n;
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

