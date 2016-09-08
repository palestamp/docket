#include "cfdm.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SCANNER_LINE_START 0
#define SCANNER_POS_START  0
#define MAX_MORPHEME_STACK 64

#define _SCANNER_STACK_DUMP(_s) do {        \
    int __i;                                \
    for(__i = 0; __i < 64; __i++) {         \
        if(_s->tokens[__i] != 0) {                \
            printf("%s ", token_lut[_s->tokens[__i]]);     \
        }                                      \
    }                                       \
    printf("\n");                           \
} while (0)

#define SCANNER_REWIND(_s) do {    \
    _s->last_morpheme_start = -1;  \
    _s->next_slot = 0;             \
} while (0)                     

#define SCANNER_PUT_TOKEN(_s, _token) do {  \
    if(_s->next_slot == 0) {\
        _s->last_morpheme_start = _s->pos;      \
    }\
    _s->tokens[_s->next_slot] = _token;     \
    _s->next_slot++;                        \
} while(0)

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


struct scanner {
    size_t pos;
    size_t line;
    size_t lpos;
    char *buf;
    int last_morpheme_start;
    int next_slot;
    int tokens[MAX_MORPHEME_STACK];
};

typedef int(*advancer)(struct scanner *s);
typedef int(*parser)(char ch);
struct vmparser {
    advancer advancefn;
    parser    parsefn;
};

int
lbracket_adv(struct scanner *s) {
    if (s->next_slot == 1 && s->tokens[0] == lf) {
        SCANNER_PUT_TOKEN(s, lbracket);
    }
    printf("LBRACKET\n");
}

int
cl_adv(struct scanner *s) {
    printf("CL\n");
}
int
lf_adv(struct scanner *s) {
    if(s->next_slot > 1 && s->tokens[0] == lf && s->tokens[1] == lbracket) {
        printf("ERROR\n");
        abort();
    }
    SCANNER_REWIND(s);
    SCANNER_PUT_TOKEN(s, lf);
    printf("LF\n");
}
int
ws_adv(struct scanner *s) {
    printf("WS\n");
}
int 
rbracket_adv(struct scanner *s) {

    printf("RBRACKET\n");
    if(s->next_slot > 1 && s->tokens[0] == lf && s->tokens[1] == lbracket) {
        printf("HEADER FOUND ");
        // shifting all range to one char to the right
        printf("%.*s\n", (s->pos - s->last_morpheme_start), (s->buf + s->last_morpheme_start + 1));
        SCANNER_REWIND(s);
    }
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
    memset((void *)s->tokens, 0, MAX_MORPHEME_STACK * sizeof(int));
    s->next_slot = 0;
    s->last_morpheme_start = 0;
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

//        _SCANNER_STACK_DUMP(s);
        cursor++;
        s->pos++;
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
