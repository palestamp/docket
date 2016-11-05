#ifndef _H_SCANNER
#define _H_SCANNER

#include "cfdm.h"


struct scanner {
    char *buf;
    size_t pos;
    size_t len;
};


#define SCANNER_CURSOR(_scanner) \
    ((_scanner)->buf + (_scanner)->pos)


#define SCANNER_ADVANCE(_scanner) do { \
    (_scanner)->pos++;                 \
} while(0)


#define SCANNER_RETREAT(_scanner, _len) do { \
    (_scanner)->pos -= (_len);               \
} while(0)


#define SCANNER_END(_scanner)                \
    ((_scanner)->pos == (_scanner)->len)


#define SCANNER_CURRENT_CHAR(_scanner)  \
    *(SCANNER_CURSOR(_scanner))


void scanner_own_cfdm(struct scanner *s, struct cfdmap *m);
int scanner_get_title(struct scanner *s, int *len);
int scanner_get_body(struct scanner *s, int *len);
int scanner_get_header(struct scanner *s, int *len, int *sc);
void scanner_next_line(struct scanner *s);
#endif
