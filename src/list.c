#include <stdlib.h>
#include "list.h"


void list_insert(struct lnode *head, void *data)  {
    struct lnode *n = malloc(sizeof(struct lnode));
    n->data = head->data;
    n->next = head->next;
    head->data = data;
    head->next = n;
}

struct lnode *
next(struct lnode *curr) {
    return curr->next;
}

int
list_has(struct lnode *head, const void *data, cmpfn cmp) {
    struct lnode *swap = head;
    while(swap) {
        if(cmp) {
            if(cmp(data, swap->data) == 0) {
                return 1;
            }
        } else {
            if(data == (const void *)(swap->data)) {
                return 1;
            }
        }
        swap = swap->next;
    }
    return 0;
}
