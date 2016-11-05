#ifndef _H_DOCKET
#define _H_DOCKET

#include "trie.h"
#include "cfdm.h"
#include "scanner.h"
#include "kv.h"

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

void docket_parse(struct kvsrc *kv, struct scanner *s);
// utility functions

char * docket_fix_title(const char *title);
size_t docket_get_body_len(struct docket *d);
int cmp_str(const void *a, const void *b);

#endif
