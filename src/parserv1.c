
struct parser {
    size_t pos;
    size_t line;
    size_t lpos;
    char *buf;
    
};

static inline void
unread(struct parser *p) {
    if(p->pos > 0) {
        p->pos--;
        if()
    }
}

static inline char
read(struct parser *p) {
    return *(p->buf + p->pos);
}

enum token {
    /* Special */
    illegal,
    eof,
    cr,
    lf,
    ws,
    
    /* Grammar */
    // Comment
    dash,
    // Possible path start
    lbracket,
    // Possible path end
    rbracket,
    // Possible path delimiter
    semicolon,
    // Possible title start
    atsign,
};

int 
iswhitespace(ch char) {
    return ch == ' ' || ch == '\t';   
}

int parse()

int parser_advance(struct parser *p) {
	char ch = read(p);
	
}

