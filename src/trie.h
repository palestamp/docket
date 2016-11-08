#ifndef DOCKET_TRIE_H
#define DOCKET_TRIE_H

#include "cmpfn.h"
#include "filter.h"
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

    // XXX should be in outer struct
    TAILQ_ENTRY(word_trie) tries;
};

struct trie_loop;
typedef int(*loop_guard)(const struct word_trie *wt);
struct trie_loop {
    int edge_offset;
    int pass;
    int depth;
    loop_guard guard_fn;
    struct path_filter *filter;
    TAILQ_HEAD(loop_head ,word_trie) stack;
};

struct word_trie * trie_new(void);

/*
 * Callback function for trie_over_path_apply
 */
typedef struct word_trie *(*trie_path_iter_cb)(struct word_trie *t, const char *word);

/*
 * Apply function over all chunks in path
 */
struct word_trie * trie_over_path_apply(trie_path_iter_cb cb, struct word_trie *t, const char *path);

struct word_trie * trie_get_path(struct word_trie *, const char * path);

struct word_trie * trie_insert_path(struct word_trie *, const char * path);
struct word_trie * trie_append_child(struct word_trie *host, struct word_trie *child);
void trie_insert_by_path(struct word_trie *trie, const char *path, void *data);


void trie_sort(struct word_trie *t, cmpfn comporator);
void trie_print(struct word_trie *t);

/*
 * Iterating
 */
#define TRIE_LOOP_INIT(_var, _guard, _filter) do {     \
    (_var)->edge_offset = 0;          \
    (_var)->depth = -1;               \
    (_var)->guard_fn = (_guard);      \
    (_var)->filter = (_filter);       \
    TAILQ_INIT(&(_var)->stack);       \
} while(0)

#define TRIE_BRANCH_LOOP_INIT(_var, _filter) do {                   \
    TRIE_LOOP_INIT((_var), trie_filter_branch_end, (_filter));  \
} while(0)

#define TRIE_DATA_LOOP_INIT(_var, _filter) do {                   \
    TRIE_LOOP_INIT((_var), trie_filter_has_data, (_filter));  \
} while(0)

#define LOOP_ONSTACK_TRIE(_loop_ptr) TAILQ_LAST(&(_loop_ptr)->stack, loop_head)

struct trie_loop * trie_loop_branch(struct word_trie *t, struct trie_loop *loop);
struct trie_loop *trie_filter_branch(struct word_trie *t, struct trie_loop *loop);

struct word_trie *trie_loop_children(struct word_trie *cell, struct word_trie *host);

// filter_fn
// accept if node has node child nodes
int trie_filter_branch_end(const struct word_trie *trie);
// accept if node has data
int trie_filter_has_data(const struct word_trie *trie);

char *loop_stack_sprint(struct trie_loop *loop);
void loop_stack_print(struct trie_loop *loop);
char *loop_stack_sprint_kv(struct trie_loop *loop);

/* utility functions */
int trie_sort_path_label_asc(const void *a, const void *b);
int trie_sort_path_label_desc(const void *a, const void *b);
#endif
