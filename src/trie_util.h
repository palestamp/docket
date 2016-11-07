#ifndef DOCKET_TRIE_UTIL_H
#define DOCKET_TRIE_UTIL_H
#include "trie.h"

int trie_get_max_int_child(struct word_trie *host);
struct word_trie *trie_get_max_int_child_node(struct word_trie *host);
#endif
