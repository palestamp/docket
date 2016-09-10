#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

struct word_trie *
trie_get(struct word_trie *t, char *word) {
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
    struct word_trie **temp = NULL;
    temp = realloc(t->edges, sizeof(struct word_trie *) * (t->cap + 10));
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
    return n;
}


struct word_trie *
trie_add(struct word_trie *t, char *word) {
    struct word_trie *n = trie_get(t, word);
    if (n == NULL) {
        n = trie_new();
        n->word = word;
        trie_grow(t);

        t->edges[t->len] = n;
        t->len++;
    }
    return n;
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
trie_print(struct word_trie *t, int l) {
    int i;
    if(t != NULL && t->word != NULL) {
        printf("%*s%s\n", l, "",  t->word);
        l++;
    }
    for(i= 0; i < t->len; i++) {
        trie_print(*(t->edges + i), l);
    }
}

int
trie_sort_path_label_asc(const void *a, const void *b) {
    if(a == NULL && b == NULL) {
        return 0;
    }

    // since we get pointer to poiner enforced by qsort we need to
    // dereference ptrptr and back one step from correct pointer type
    char *aw = (*(struct word_trie **)a)->word;
    char *bw = (*(struct word_trie **)b)->word;

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
    char *aw = (*(struct word_trie **)a)->word;
    char *bw = (*(struct word_trie **)b)->word;

    if(aw == NULL) {
        return 1;
    } else if (bw == NULL) {
        return -1;
    }
    return -strcmp(aw, bw);
}

