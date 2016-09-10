#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct word_trie {
    int cap;
    int len;
    char *word;
    struct word_trie **edges;
};

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

//int main(void)
//{
//    struct word_trie *root = trie_new();
//    struct word_trie *trie =NULL;
//    trie = trie_add(root, "aaa");
//    trie = trie_add(root, "bbb");
//    trie_add(trie, "ccc");
//    trie_add(trie, "ccc");
//    trie_add(trie, "ccc");
//    trie_add(trie, "ccc");
//    trie = trie_add(trie, "ccc");
//    trie_print(root, 0);
//    return 0;
//}
