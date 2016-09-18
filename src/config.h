#ifndef _H_CONFIG
#define _H_CONFIG

#include <sys/queue.h>

#include "trie.h"
#include "list.h"


struct config {
    struct word_trie *trie;
};

/*
 * Create config file in HOME directory
 */
int create_config_file(void);

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

int config_flush(struct config *);


#endif
