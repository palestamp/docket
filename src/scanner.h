#ifndef _H_SCANNER
#define _H_SCANNER

#include "cfdm.h"

#define SCANNER_LINE_START 0
#define SCANNER_POS_START  0

struct scanner {
    unsigned char *buf;
    size_t pos;
    size_t line;
    size_t lpos;
};

#define SCANNER_CURSOR(_scanner) \
    ((_scanner)->buf + (_scanner)->pos)

#define SCANNER_ADVANCE(_scanner) do { \
    (_scanner)->pos++;                 \
} while(0)                          

void scanner_own_cfdm(struct scanner *s, struct cfdmap *m);
#endif
