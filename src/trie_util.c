#include <stdlib.h>
#include "trie_util.h"


int
trie_get_max_int_child(struct word_trie *host) {
    if (host == NULL || host->len < 1) {
        return 0;
    }
    int max = 0;
    int found = 0;

    struct word_trie *swap_trie  = NULL;
    while((swap_trie = trie_loop_children(swap_trie, host))) {
        if((found = atoi(swap_trie->word)) > max) {
            max = found;
        }
    }
    return max;
}


struct word_trie *
trie_get_max_int_child_node(struct word_trie *host) {
    if (host == NULL || host->len < 1) {
        return 0;
    }

    int max = 0;
    int found = 0;
    struct word_trie *ret_node = NULL;

    struct word_trie *swap_trie  = NULL;
    while((swap_trie = trie_loop_children(swap_trie, host))) {
        if((found = atoi(swap_trie->word)) > max) {
            max = found;
            ret_node = swap_trie;
        }
    }
    return ret_node;
}


struct leaf_list *
trie_get_value_node(struct word_trie *trie, int index) {
    if (trie == NULL) {
        return NULL;
    }

    int i = 0;
    struct leaf_list *lfls = NULL;
    TAILQ_FOREACH(lfls, &trie->leafs, leaf) {
        if(i == index) {
            return lfls;
        }
    }
    return NULL;
}


void *
trie_get_value(struct word_trie *trie, int index) {
    if (trie == NULL) {
        return NULL;
    }

    int i = 0;
    struct leaf_list *lfls = NULL;
    TAILQ_FOREACH(lfls, &trie->leafs, leaf) {
        if(i == index) {
            return lfls->data;
        }
    }
    return NULL;
}
