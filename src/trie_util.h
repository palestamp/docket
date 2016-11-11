#ifndef DOCKET_TRIE_UTIL_H
#define DOCKET_TRIE_UTIL_H
#include "trie.h"

int trie_get_max_int_child(struct word_trie *host);
struct word_trie *trie_get_max_int_child_node(struct word_trie *host);

// check for requested value in trie's leaf array
int trie_has_value(struct word_trie *trie, cmpfn fn, void *value);
struct data_leaf *trie_get_value_node(struct word_trie *trie, int index);
void *trie_get_value(struct word_trie *trie, int index);
#endif
