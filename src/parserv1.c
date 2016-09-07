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

int
cl_adv(struct scanner *s) {
    printf("CL\n");
}
int
lf_adv(struct scanner *s) {
    printf("LF\n");
}
int
ws_adv(struct scanner *s) {
    printf("WS\n");
}
int 
rbracket_adv(struct scanner *s) {
    printf("RBRACKET\n");
}
int 
atsign_adv(struct scanner *s) {
    printf("ATSIGN\n");
}

int 
semicolon_adv(struct scanner *s) {
    printf("SEMICOLON\n");
}

void
scanner_own_cfdm(struct scanner *s, struct cfdmap *m) {
    s->pos = 0;
    s->line = SCANNER_LINE_START;
    s->lpos = SCANNER_POS_START;
    s->buf = (char *)m->map;
}

static struct vmparser dvmt[] = {
    {NULL, NULL}, // illegal
    {NULL, NULL}, // eof
    {cl_adv, NULL}, // cr
    {lf_adv, NULL}, // lf
    {ws_adv, NULL}, // ws

    {NULL, NULL}, // dash
    {lbracket_adv, NULL}, // lbracket
    {rbracket_adv, NULL}, // rbracket
    {semicolon_adv, NULL}, // semicolon
    {atsign_adv, NULL}, // atsign
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
    "LBRACKET",
    "RBRACKET" ,
    "SEMICOLON" ,
    "ATSIGN" ,
};

int 
iswhitespace(char ch) {
    return ch == ' ' || ch == '\t';   
}

int parse(struct scanner *s, struct vmparser vmt[]) {
    char *cursor = s->buf;
    char tok = 0;
    while ((tok = *cursor)) {
        switch(tok) {
            case '\n': vmt[lf].advancefn(s); break;
            case '\r': vmt[cr].advancefn(s); break;
            case '[': vmt[lbracket].advancefn(s); break;
            case ']': vmt[rbracket].advancefn(s); break;
            case ':': vmt[semicolon].advancefn(s); break;
            case '@': vmt[atsign].advancefn(s); break;
            case '\t':
            case ' ': vmt[ws].advancefn(s); break;
        }


        cursor++;
    }
    return 0;
}

int main(int argc, const char *argv[])
{
    struct cfdmap *m = malloc(sizeof(m));
    map_file("../local.docket", m);
    struct scanner *s = malloc(sizeof(s));
    scanner_own_cfdm(s, m);
    parse(s, dvmt);
    return 0;
}
