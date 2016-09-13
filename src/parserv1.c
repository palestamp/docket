#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "trie.h"
#include "cfdm.h"

/*
 * Top-level lexing
 *
 * Stream tokens:
 *      LAYOUT
 *      HEADER
 *      TITLE
 *
 * TODO
 *      1. Find all headers +
 */


#define SCANNER_LINE_START 0
#define SCANNER_POS_START  0

struct scanner {
    unsigned char *buf;
    size_t pos;
    size_t line;
    size_t lpos;
};


void
scanner_own_cfdm(struct scanner *s, struct cfdmap *m) {
    s->pos = 0;
    s->line = SCANNER_LINE_START;
    s->lpos = SCANNER_POS_START;
    s->buf = m->map;
}


unsigned char *
get_header(struct scanner *s, int *len, int *sc) {
    unsigned char *cursor = (s->buf + s->pos);
    while(*cursor) {
        // If we in the beginning of buf or '[' - first elem in line
        if(*cursor == '[' && ((s->pos == 0) || (*(cursor - 1) == '\n'))) {
            unsigned char *subcursor = cursor;
            while(1) {
                subcursor++;
                s->pos++;
                (*len)++;
                switch (*subcursor) {
                    case ':':
                        (*sc)++;
                        break;
                    case ']':
                        // each path has at least one section
                        (*sc)++;
                        (*len)++;
                        s->pos++;
                        return cursor;
                    case '\n':
                        printf("ERROR");
                        return NULL;
                }
            }

            s->pos++;
            return cursor;
        }

        s->pos++;
        cursor++;
    }
    return NULL;
}

int main(int argc, const char *argv[])
{
    struct cfdmap *m = malloc(sizeof(struct cfdmap));
    map("/Users/staskazhavets/Dropbox/owl.docket", m);
    struct scanner *s = malloc(sizeof(struct scanner));
    scanner_own_cfdm(s, m);

    unsigned char *header = NULL;
    int len = 0;
    int sections = 0;
    unsigned char pbuf[1024] = {0};
    struct word_trie *root = trie_new();
    struct word_trie *swap_node = NULL;
    while((header = get_header(s, &len, &sections)) != NULL) {
        // strip left and right bracket
        pbuf[len - 2] = '\0';
        memcpy(pbuf, header + 1, len - 2);
        swap_node = root;

        // addind path to  trie
        unsigned char localbuffer[64] = {0};
        int l = 0;
        int r = 0;
        while(1) {
            if (pbuf[r] == ':' || pbuf[r] == '\0') {
                unsigned char *off_left = pbuf + l;
                int loclen = r - l ;
                memcpy(localbuffer, off_left, loclen);
                localbuffer[loclen] = '\0';
                l = r + 1;

                swap_node = trie_add(swap_node, strdup((const char *)localbuffer));
                memset(localbuffer, 0, 64);
                if (pbuf[r] == '\0') {
                    break;
                }
            }
            r++;
        }

        len = 0;
        sections = 0;
    }

    trie_sort(root, trie_sort_path_label_asc);
    trie_print(root, 0);
    return 0;
}
