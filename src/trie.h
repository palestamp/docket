#ifndef _H_DOCKET__TRIE
#define _H_DOCKET__TRIE

struct word_trie {
    int cap;
    int len;
    char *word;
    struct word_trie **edges;
};

struct word_trie * trie_get(struct word_trie *t, char *word) ;
struct word_trie * trie_new(void) ;
struct word_trie * trie_add(struct word_trie *t, char *word) ;
void trie_print(struct word_trie *t, int l) ;
#endif
