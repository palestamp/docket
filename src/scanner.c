#include <string.h>
#include "scanner.h"


void
scanner_own_cfdm(struct scanner *s, struct cfdmap *m) {
    s->pos = 0;
    s->line = SCANNER_LINE_START;
    s->lpos = SCANNER_POS_START;
    s->buf = m->map;
    s->len = strlen(m->map);
}

int
scanner_get_title(struct scanner *s, int *len) {
    *len = 0;

    char *cursor = SCANNER_CURSOR(s);
    if(*cursor == '@' && *(cursor + 1) == '@') {
        while(*cursor != '\n') {
            cursor++;
            (*len)++;
            SCANNER_ADVANCE(s);
        }
        return 1;
    }
    return 0;
}

int
scanner_get_body(struct scanner *s, int *len) {
    *len = 0;

    char *cursor = SCANNER_CURSOR(s);
    while(!SCANNER_END(s)) {
        // if we have header-like string - return
        if(*cursor == '[' && ((s->pos == 0) || (*(cursor - 1) == '\n'))) {
            return 1;
        }
        SCANNER_ADVANCE(s);
        cursor++;
        (*len)++;
    }

    if(*len > 0) {
        return 1;
    } else {
        return 0;
    }
}

int
scanner_get_header(struct scanner *s, int *len, int *sc) {
    *len = 0;
    *sc = 0;

    char *cursor = SCANNER_CURSOR(s);
    while(*cursor) {
        // If we in the beginning of buf or '[' - first elem in line
        if(*cursor == '[' && ((s->pos == 0) || (*(cursor - 1) == '\n'))) {
            char *subcursor = cursor;
            while(1) {
                subcursor++;
                (*len)++;
                SCANNER_ADVANCE(s);
                switch (*subcursor) {
                    case ':':
                        (*sc)++;
                        break;
                    case ']':
                        // each path has at least one section
                        (*sc)++;
                        (*len)++;
                        SCANNER_ADVANCE(s);
                        return 1;
                    case '\n':
                        fprintf(stderr, "HEADER_ERROR");
                        return 0;
                }

            }

            SCANNER_ADVANCE(s);
            return 1;
        }

        SCANNER_ADVANCE(s);
        cursor++;
    }
    return 0;
}

void
scanner_next_line(struct scanner *s) {
    while(SCANNER_CURSOR(s)) {
        if(SCANNER_CURRENT_CHAR(s) == '\n') {
            SCANNER_ADVANCE(s);
            return;
        }
        SCANNER_ADVANCE(s);
    }
}
