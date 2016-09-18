#ifndef _H_LIST
#define _H_LIST

#include "cmpfn.h"

struct lnode {
    void *data;
    struct lnode *next;
};

void list_insert(struct lnode *head, void *data);
int list_has(struct lnode *head, const void *data, cmpfn comporator);
struct lnode * next(struct lnode *curr);
#endif
