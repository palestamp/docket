#ifndef DOCKET_KV_H
#define DOCKET_KV_H

#include <stdio.h>

#include "trie_util.h"
#include "scanner.h"

struct kvsrc {
    struct word_trie *trie;
    FILE *origin;
};

int kv_exists(const char *cpath);
void kv_parse(struct kvsrc *kv, struct scanner *s);
struct kvsrc *kv_load(const char *kvsrc_path, void (*parse)(struct kvsrc *kv, struct scanner *s));
void kv_add(struct kvsrc *kv, const char * accessor_string, const char *value);
struct word_trie *kv_get(struct kvsrc *kv, const char * accessor_string);
int kv_has(struct kvsrc *kv, const char *filter, const char *value);
int kv_flush(struct kvsrc *kv, char **buf);
int kv_sync(struct kvsrc *kv);
#endif
