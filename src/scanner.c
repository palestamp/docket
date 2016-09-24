#include "scanner.h"


void
scanner_own_cfdm(struct scanner *s, struct cfdmap *m) {
    s->pos = 0;
    s->line = SCANNER_LINE_START;
    s->lpos = SCANNER_POS_START;
    s->buf = m->map;
}


