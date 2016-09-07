#include "cfdm.h"
#include <stdlib.h>
#include <stdio.h>

#define SCANNER_LINE_START 0
#define SCANNER_POS_START  0

struct scanner {
    size_t pos;
    size_t line;
    size_t lpos;
    char *buf;
    struct vmparser **vmt;  
};

typedef int(*advancer)(struct scanner *s);
typedef int(*parser)(char ch);
struct vmparser {
    advancer advancefn;
    parser    parsefn;
};

int
lbracket_adv(struct scanner *s) {
    printf("LBRACKET\n");
}


void
scanner_bind_vmt(struct scanner *s, struct vmparser vmp[]) {
    s->vmt = &vmp;
}

void
scanner_own_cfdm(struct scanner *s, struct cfdmap *m) {
    s->pos = 0;
    s->line = SCANNER_LINE_START;
    s->lpos = SCANNER_POS_START;
    s->buf = m->map;
}

static struct vmparser dvmt[] = {
    {NULL, NULL}, // illegal
    {NULL, NULL}, // eof
    {NULL, NULL}, // cr
    {NULL, NULL}, // lf
    {NULL, NULL}, // ws

    {NULL, NULL}, // dash
    {&lbracket_adv, NULL}, // lbracket
    {NULL, NULL}, // rbracket
    {NULL, NULL}, // semicolon
    {NULL, NULL}, // atsign
};

enum token {
    /* Special */
    illegal = 0,
    eof ,
    cr ,
    lf ,
    ws ,

    /* Grammar */
    // Comment
    dash ,
    // Possible path start
    lbracket ,
    // Possible path end
    rbracket ,
    // Possible path delimiter
    semicolon ,
    // Possible title start
    atsign ,
};
static const char *token_lut[] = {
    "ILLEGAL",
    "EOF",
    "CL",
    "LF",
    "WS",
    "DASH",
    "LBRACKET"
    "RBRACKET" ,
    "SEMICOLON" ,
    "ATSIGN" ,
};

int 
iswhitespace(char ch) {
    return ch == ' ' || ch == '\t';   
}

int parse(struct scanner *s) {
    char *cursor = s->buf;
    while (*cursor++) {
        switch (*cursor) {
            case '[': s->vmt[lbracket]->advancefn(s); break;
        }
    }
    return 0;
}

int main(int argc, const char *argv[])
{
    struct cfdmap *m = malloc(sizeof(m));
    map_file("../local.docket", m);
    struct scanner *s = malloc(sizeof(s));
    scanner_bind_vmt(s, dvmt);
    scanner_own_cfdm(s, m);
    parse(s);
    return 0;
}
