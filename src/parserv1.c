#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "trie.h"
#include "cfdm.h"
#include "scanner.h"

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


int
get_title(struct scanner *s, int *len) {
    *len = 0;

    char *cursor = SCANNER_CURSOR(s);
    if(*cursor == '@' && *(cursor + 1) == '@') {
        while(*cursor != '\n') {
            (*len)++;
        }
        return 1;
    }
    return 0;
}

int
get_body(struct scanner *s, int *len) {
    int *len = 0;

    int hlen = 0;
    int sc = 0;
    char *cursor = SCANNER_CURSOR(s);
    while(1) {
        if(*cursor == '[') {
            if(get_header(s, &hlen, &sc) == 1) {
                SCANNER_RETREAT(s, hlen);
                return 1;
            }
        } else {
            (*len)++;
        }
    }
}

int
get_header(struct scanner *s, int *len, int *sc) {
    *len = 0;
    *sc = 0;

    char *cursor = SCANNER_CURSOR(s);
    while(*cursor) {
        // If we in the beginning of buf or '[' - first elem in line
        if(*cursor == '[' && ((s->pos == 0) || (*(cursor - 1) == '\n'))) {
            while(1) {
                char *subcursor = cursor;
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
                        return 0;
                }
            }

            s->pos++;
            return 1;
        }

        s->pos++;
        cursor++;
    }
    return 0;
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
