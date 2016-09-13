#include <stdlib.h>
#include <stdio.h>
#include "strbuf.h"
#include "docket.h"


/*
 * Create new docket instance
 */
struct docket *
docket_new(void) {
    struct docket *d = malloc(sizeof(struct docket));
    d->head = NULL;
    d->title = NULL;
    d->body = NULL;
    return d;
}


/*
 * Dumb text serialization
 */
const char *
docket_serialize(struct docket *d) {
    char *buf = NULL;
    int blen = 0;
	int bpos = 0;
    if(d->head) {
        bufcat(&buf, &blen, &bpos, "[%s]\n", d->head);
    }
    if(d->title) {
        bufcat(&buf, &blen, &bpos, "@@ %s\n", d->title);
    }
    if(d->body) {
        bufcat(&buf, &blen, &bpos, "%s", d->body);
    }
	return buf;
}
