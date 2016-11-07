#include <stdlib.h>
#include <stdio.h>
#include "trie.h"
#include "strbuf.h"
#include "filter.h"
/**
 * Grammar includes only '*' char that acts like glob
 */

struct path_filter *
compile_filter_from_s(const char *pattern) {
    // at least one chunck
    int sc = 1;
    char *cursor = (char *)pattern;
    while(*cursor) {
        if(*cursor == ':') {
            sc++;
        }
        cursor++;
    }
    struct path_filter *pf = malloc(sizeof(struct path_filter));
    pf->len = sc;
    pf->patterns = malloc(pf->len * sizeof(char *));

    int path_idx = 0;
    int i = 0;
    cursor = (char *)pattern;
    while(1) {
        if(*cursor == ':' || *cursor == '\0') {
            *(pf->patterns + path_idx) = copy_slice(cursor - i , i);
            if(*cursor == '\0') {
                break;
            }
            i = 0;
            path_idx++;
        } else {
            i++;
        }
        cursor++;
    }
    return pf;
}

int
match_filter_chunk(struct path_filter *pf, const char *ch, int depth) {
    if(depth < pf->len) {
        if (pf->patterns[depth][0] == '*' || pf->patterns[depth][0] == '~' ||
                (strcmp(pf->patterns[depth], ch) == 0)) {
            return 1;
        }
    } else {
        if (pf->patterns[pf->len - 1][0] == '~') {
            return 1;
        }
    }
    return 0;
}
