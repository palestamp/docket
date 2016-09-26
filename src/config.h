#ifndef _H_CONFIG
#define _H_CONFIG

#include <stdio.h>

#include "trie.h"


#define DCT_CONFIG_SOURCES_TRIE_PATH "docket:settings:sources"

struct config {
    struct word_trie *trie;
    FILE *cf;
};

/*
 * Create config file in HOME directory
 */
int create_config_file(void);

FILE * config_open(const char *flags);
/*
 * Check if $HOME/.docket exists
 */
int config_exists(void);

/*
 * Build '$HOME/.docket'
 */
const char *get_config_path(void);

/*
 * Load configuration file into config struct
 */
struct config *config_load(void);

struct config *config_create(void);

/*
 * Create empty config struct
 */
void config_free(struct config *);

/*
 * Check if config has value on accessor_string path
 */
int config_has(struct config *, const char *accessor_string, const char *value);

int config_add(struct config *, const char *accessor_string, const char *value);

int config_flush(struct config *, char **);
int config_sync(struct config *c);


#endif
