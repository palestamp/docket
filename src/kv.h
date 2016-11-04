#ifndef DOCKET_KV_H
#define DOCKET_KV_H

#include <stdio.h>
#include "trie.h"

struct kvsrc {
    struct word_trie *trie;
    FILE *origin;
};

int kv_exists(const char *cpath);
struct kvsrc *kv_load(const char *kvsrc_path);
void kv_add(struct kvsrc *kv, const char * accessor_string, const char *value);
struct word_trie *kv_get(struct kvsrc *kv, const char * accessor_string);
int kv_has(struct kvsrc *kv, const char *filter, const char *value);
int kv_flush(struct kvsrc *kv, char **buf);
int kv_sync(struct kvsrc *kv);
#endif
