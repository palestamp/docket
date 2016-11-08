#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"
#include "strbuf.h"

static void trie_printX(struct word_trie *t, int level);

struct word_trie *
trie_new(void) {
    struct word_trie *n = NULL;
    n = malloc(sizeof(struct word_trie));
    n->len = 0;
    n->cap = 0;
    n->pos = 0;
    n->edges = NULL;
    n->word = NULL;
    TAILQ_INIT(&n->leafs);
    return n;
}


/*
 ********************** Inner trie operations **************************************
 */

static struct word_trie *
trie_get(struct word_trie *t, const char *word) {
    if (t == NULL) {
        return NULL;
    }
    int i;
    struct word_trie *n = NULL;
    for (i = 0; i < t->len; i++) {
        n = *(t->edges + i);
        if(n != NULL && (strcmp(n->word, word) == 0)) {
            return n;
        }
    }
    return NULL;
}


static void
trie_grow(struct word_trie *t) {
    // last pointer should be NULL
    if (t->cap <= t->len + 1) {
        // 1.125 growth rate (python implementation)
        int newalloc = (t->cap >> 3) + (t->cap < 9 ? 3 : 6);
        struct word_trie **temp = NULL;
        temp = realloc(t->edges, sizeof(struct word_trie *) * (t->cap + newalloc));

        int i;
        for(i = t->cap ; i < t->cap + newalloc; i++) {
            *(temp + i) = NULL;
        }

        t->cap += newalloc;
        t->edges = temp;
    }
}


static struct word_trie *
trie_add(struct word_trie *t, const char *word) {
    struct word_trie *n = trie_get(t, word);
    if (n == NULL) {
        n = trie_new();
        n->word = strdup(word);
        trie_grow(t);
        // save child's position
        n->pos = t->len;

        t->edges[n->pos] = n;
        t->len++;
    }
    return n;
}


/*
 ************************ Interface functions **************************************
 */

struct word_trie *
trie_append_child(struct word_trie *host, struct word_trie *child) {
    struct word_trie *n = trie_get(host, child->word);
    if (n == NULL) {
        trie_grow(host);

        // save child's position
        child->pos = host->len;
        host->edges[child->pos] = child;
        host->len++;
    }
    return child;
}


#ifndef MAX_PATH_CHUNK_LEN
#define MAX_PATH_CHUNK_LEN 64
#endif


struct word_trie *
trie_over_path_apply(trie_path_iter_cb cb, struct word_trie *t, const char *path) {
    struct word_trie *trie = t;
    char *cursor = (char *)path;

    char localbuffer[MAX_PATH_CHUNK_LEN] = {0};
    int l = 0;
    int r = 0;
    while(1) {
        if(*(cursor + r) == ':' || *(cursor + r) == '\0') {
            char *off_left = cursor + l;
            int loclen = r - l ;
            memcpy(localbuffer, off_left, loclen);
            localbuffer[loclen] = '\0';
            l = r + 1;
            trie = cb(trie, localbuffer);

            // for get operations only
            if (trie == NULL) {
                return NULL;
            }

            memset(localbuffer, 0, MAX_PATH_CHUNK_LEN);
            if(*(cursor + r) == '\0') {
                break;
            }
        }
        r++;
    }
    return trie;
}


struct word_trie *
trie_insert_path(struct word_trie *t, const char *path) {
    return trie_over_path_apply(trie_add, t, path);
}


void trie_insert_by_path(struct word_trie *trie, const char *path, void *data) {
    struct word_trie *target_trie = trie_insert_path(trie, path);
    struct leaf_list *list = malloc(sizeof(struct leaf_list));
    list->data = data;
    TAILQ_INSERT_TAIL(&target_trie->leafs, list, leaf);
}


struct word_trie *
trie_get_path(struct word_trie *t, const char *path) {
    return trie_over_path_apply(trie_get, t, path);
}


int
trie_has_value(struct word_trie *trie, cmpfn cmpf, void *value) {
    struct leaf_list *lfls = NULL;
    TAILQ_FOREACH(lfls, &trie->leafs, leaf) {
        if(!cmpf(value, lfls->data)) {
            return 1;
        }
    }
    return 0;
}


/*
 ********************** Sorting ****************************************************
 */
void
trie_sort(struct word_trie *t, cmpfn cmpf) {
    qsort((void *)(t->edges), t->len, sizeof(struct word_trie *), cmpf);
    int i;
    for(i = 0; i < t->len; i++) {
        trie_sort(*(t->edges + i), cmpf);
    }
}


void
trie_print(struct word_trie *t) {
    trie_printX(t, 0);
}


static void
trie_printX(struct word_trie *t, int l) {
    int i;
    if(t != NULL && t->word != NULL) {
        fprintf(stderr, "%*s%s\n", l, "",  t->word);
        l++;
    }
    for(i= 0; i < t->len; i++) {
        trie_printX(*(t->edges + i), l);
    }
}


char *
loop_stack_sprint(struct trie_loop *loop) {
    char *buf = NULL;
    int len = 0;
    int pos = 0;
    struct word_trie *l = NULL;
    char *delimiter = "";
    TAILQ_FOREACH(l , &loop->stack, tries) {
        if (l->word) {
            bufcat(&buf, &len, &pos, "%s%s", delimiter,  l->word);
            delimiter = ":";
        }
    }
    return buf;
}


void
loop_stack_print(struct trie_loop *loop) {
    char *buf = loop_stack_sprint(loop);
    printf("%s\n", buf);
    free(buf);
}


char *
loop_stack_sprint_kv(struct trie_loop *loop) {
    struct word_trie *trie = LOOP_ONSTACK_TRIE(loop);
    char *buf = NULL;
    int len = 0;
    int pos = 0;
    if (trie) {
        char *path_buf = loop_stack_sprint(loop);
        struct leaf_list *entry = NULL;
        TAILQ_FOREACH(entry, &trie->leafs, leaf) {
            bufcat(&buf, &len, &pos, "%s=%s\n", path_buf, (char *)entry->data);
        }
    }
    return buf;
}


// XXX Sure that guard_fn works with one-node branch
static struct trie_loop *
trie_loopX2(struct word_trie *trie, struct trie_loop *loop) {
    // this is always root node which basicaly can not store any information
    // therefore we can skip guard_fn on it still it non-informative
    if(TAILQ_EMPTY(&loop->stack)) {
        if(loop->pass != 0) {
            return NULL;
        }

        loop->pass = 1;
        TAILQ_INSERT_TAIL(&loop->stack, trie, tries);
        return trie_loopX2(trie, loop);
    } else {
        // get trie node to process
        struct word_trie *last = TAILQ_LAST(&loop->stack, loop_head);


        // if latest node have right sibling - iterate over them
        // else - go to the upper level
        if(last->len > 0 && *(last->edges + loop->edge_offset)) {
            struct word_trie *newt = *(last->edges + loop->edge_offset);

            loop->edge_offset = 0;
            loop->depth += 1;

            TAILQ_INSERT_TAIL(&loop->stack, newt, tries);
            if (loop->guard_fn(newt)) {
                return loop;
            } else {
                return trie_loopX2(newt, loop);
            }
        } else {
            // we in leaf node
            // get position of current node in parent edges
            int sibling_pos = last->pos;

            // remove current node
            TAILQ_REMOVE(&loop->stack, last, tries);

            // grab parent of current node
            last = TAILQ_LAST(&loop->stack, loop_head);

            loop->edge_offset = sibling_pos + 1;
            loop->depth -= 1;
            // recursing case we already have this nodes
            return trie_loopX2(last, loop);
        }
    }
};



static struct trie_loop *
trie_loopX3(struct word_trie *trie, struct trie_loop *loop) {
    // this is always root node which basicaly can not store any information
    // therefore we can skip guard_fn on it still it non-informative
    if(TAILQ_EMPTY(&loop->stack)) {
        if(loop->pass != 0) {
            return NULL;
        }

        loop->pass = 1;
        TAILQ_INSERT_TAIL(&loop->stack, trie, tries);
        return trie_loopX3(trie, loop);
    } else {
        // get trie node to process
        struct word_trie *last = TAILQ_LAST(&loop->stack, loop_head);
        // if latest node have right sibling - iterate over them
        if(last->len > 0 && *(last->edges + loop->edge_offset)) {
            // else - go to the upper level
            struct word_trie *newt = *(last->edges + loop->edge_offset);

            loop->depth += 1;
            if (match_filter_chunk(loop->filter, newt->word, loop->depth) == 0) {
                loop->edge_offset += 1;
                loop->depth -= 1;
                return trie_loopX3(last, loop);
            }
            loop->edge_offset = 0;

            TAILQ_INSERT_TAIL(&loop->stack, newt, tries);
            // check that filter len is lesser or equal to path len
            // this caused by 'tilda' nodes where filter can be  shorter than path
            // Strictly saying, left part is responsible for filtering on minimum
            // path length and right part for node params
            if (loop->filter->len - 1 <= loop->depth && loop->guard_fn(newt)) {
                return loop;
            } else {
                return trie_loopX3(newt, loop);
            }
        } else {
            // we in leaf node
            // get position of current node in parent edges
            int self_pos_rel_to_parent = last->pos;

            // remove current node
            TAILQ_REMOVE(&loop->stack, last, tries);

            // grab parent of current node
            last = TAILQ_LAST(&loop->stack, loop_head);

            loop->edge_offset = self_pos_rel_to_parent + 1;
            loop->depth -= 1;
            // recursing cause we already have this nodes
            return trie_loopX3(last, loop);
        }
    }
}


/*
 ************************* Loop functions ******************************************
 */

/** Use trie_loopX2 for reducing number of if statements
*/
struct trie_loop *
trie_loop_branch(struct word_trie *t, struct trie_loop *loop) {
    return trie_loopX2(t, loop);
}


struct trie_loop *
trie_filter_branch(struct word_trie *t, struct trie_loop *loop) {
    // XXX TODO maybe put tilda filter as default
    if (loop->filter == NULL) {
        return NULL;
    }
    return trie_loopX3(t, loop);
}


struct word_trie *
trie_loop_children(struct word_trie *cell, struct word_trie *host) {
    // first iteration
    if (host == NULL) {
        return NULL;
    } else if (cell == NULL) {
        cell = host->edges[0];
    } else if (cell->pos < host->len){
        cell = host->edges[cell->pos + 1];
    }
    return cell;
}


/*
 **************************** Comporators ******************************************
 */

int
trie_sort_path_label_asc(const void *a, const void *b) {
    if(a == NULL && b == NULL) {
        return 0;
    }

    // since we get pointer to poiner enforced by qsort we need to
    // dereference ptrptr and back one step from correct pointer type
    const char *aw = (*(struct word_trie **)a)->word;
    const char *bw = (*(struct word_trie **)b)->word;

    if(aw == NULL) {
        return -1;
    } else if (bw == NULL) {
        return 1;
    }
    return strcmp(aw, bw);
}


int
trie_sort_path_label_desc(const void *a, const void *b) {
    if(a == NULL && b == NULL) {
        return 0;
    }

    // since we get pointer to poiner enforced by qsort we need to
    // dereference ptrptr and back one step from correct pointer type
    const char *aw = (*(struct word_trie **)a)->word;
    const char *bw = (*(struct word_trie **)b)->word;

    if(aw == NULL) {
        return 1;
    } else if (bw == NULL) {
        return -1;
    }
    return -strcmp(aw, bw);
}


/*
 ************************** Filter guards ******************************************
 */

/** Filter only branches with no children
*/
int
trie_filter_branch_end(const struct word_trie *trie) {
    if(trie->len) {
        return 0;
    } else {
        return 1;
    }
}


/** Filter branches with data leafs
*/
int
trie_filter_has_data(const struct word_trie *trie) {
    if(TAILQ_EMPTY(&trie->leafs)) {
        return 0;
    } else {
        return 1;
    }
}



