#ifndef _H_DOCKET
#define _H_DOCKET

#include "trie.h"
#include "cfdm.h"

/*
 * Main docket struct
 */
struct docket {
    const char *head;
    const char *title;
    const char *body;
};

struct docket* docket_new(void);
const char* docket_serialize(struct docket *d);


struct docket_shelve {
    const char *source_path;
    struct word_trie *trie;
};

#define DOCKET_SHELVE_INIT(_ds) do {  \
    (_ds)->source_path = NULL;        \
    (_ds)->trie = trie_new();         \
} while(0)

struct docket_shelve *docket_shelve_load_file(const char *source_name);

// utility functions

char * docket_fix_title(const char *title);
size_t docket_get_body_len(struct docket *d);
int cmp_str(const void *a, const void *b);

#endif
