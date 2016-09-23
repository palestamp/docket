#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "config.h"

const char *
get_config_path(void) {
    char pathbuf[PATH_MAX] = {0};     
    sprintf(pathbuf, "%s/.docket", getenv("HOME"));
    return (const char *)strdup(pathbuf);
}

int
config_exists(void) {
    const char *config_path = get_config_path();
    if(access(config_path, R_OK | W_OK) == 0) {
        free((char *)config_path);
        return 1;
    }
    free((char *)config_path);
    // XXX check errno in caller
    return 0;
}

int 
create_config_file(void) {
    const char *config_path = get_config_path();
    FILE *config = fopen(config_path, "a+");
    fclose(config);
    free((char *)config_path);
    return 1;
}

struct config *
config_create(void) {
    struct config *c = malloc(sizeof(struct config));
    c->trie = trie_new();
    return c;
}

struct config *
config_load(void) {
    return config_create();
}

int
config_add(struct config *c, const char * accessor_string, const char *value) {
    struct word_trie *t = trie_get_path(c->trie, accessor_string);
    if(t == NULL) {
        t = trie_insert_path(c->trie, accessor_string);
    }
   
    if(t->data == NULL) {
        t->data = calloc(1, sizeof(struct lnode));
    }
    list_insert((struct lnode *)t->data,  (void *)(strdup(value)));
    return 1;

}

static int
cmp_str(const void *a, const void *b) {
    if(!(a && b)) {
        return -1;
    }

    const char *ad = (const char *)a;
    const char *bd = (const char *)b;
    return strcmp(ad, bd);
}

int
config_has(struct config *c, const char * accessor_string, const char *value) {
    struct word_trie *t = NULL;
    if((t = trie_get_path(c->trie, accessor_string)) && t->data) {    
        struct lnode *l = (struct lnode *)t->data;
        return list_has(l, (void *)value, cmp_str);
    }
    return 0;
}

int
config_flush(struct config *c) {
    char linebuf[1024] = 0;

    trie_print(c->trie);
    return 1;
}

void 
config_free(struct config *c) {
    return ;
}