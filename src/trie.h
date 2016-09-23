#ifndef _H_DOCKET__TRIE
#define _H_DOCKET__TRIE

#include "cmpfn.h"
#include <sys/queue.h>

struct leaf_list {
    TAILQ_ENTRY(leaf_list) leaf;
    void *data;
};


struct word_trie {
    int cap;
    int len;
    int pos;
    const char *word;
    struct word_trie **edges;
    
    TAILQ_HEAD(,leaf_list) leafs;
    TAILQ_ENTRY(word_trie) tries;
};


struct trie_loop {
    int edge_offset;
    int pass;
    TAILQ_HEAD(loop_head ,word_trie) stack;
};

struct word_trie * trie_new(void);

/*
 * Callback function for trie_over_path_apply
 */
typedef struct word_trie *(*trie_path_iter_cb)(struct word_trie *t, const char *path);

/*
 * Apply function over all chunks in path
 */
struct word_trie * trie_over_path_apply(trie_path_iter_cb cb, struct word_trie *t, const char *path);

struct word_trie * trie_get(struct word_trie *t, const char *word);
struct word_trie * trie_get_path(struct word_trie *, const char * path);

struct word_trie * trie_add(struct word_trie *t, const char *word);
struct word_trie * trie_insert_path(struct word_trie *, const char * path);

void trie_sort(struct word_trie *t, cmpfn comporator);
void trie_print(struct word_trie *t);

/*
 * Iterating
 */
typedef int(*loop_guard)(struct word_trie *wt);
struct trie_loop * trie_loop_branch(struct word_trie *t, struct trie_loop *loop, loop_guard guard_fn);
void loop_stack_print(struct trie_loop *loop);

/* utility functions */
int trie_sort_path_label_asc(const void *a, const void *b);
int trie_sort_path_label_desc(const void *a, const void *b);
#endif
