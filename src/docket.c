#include <stdlib.h>
#include <stdio.h>

#include "scanner.h"
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


struct docket_shelve *
docket_shelve_load_file(const char *source_name) {
    struct cfdmap *m = malloc(sizeof(struct cfdmap));
    struct scanner *s = malloc(sizeof(struct scanner));

    struct docket_shelve *ds = malloc(sizeof(struct docket_shelve));
    DOCKET_SHELVE_INIT(ds);
    ds->source_path = source_name;

    map(source_name, m);
    scanner_own_cfdm(s, m);


    int len = 0;
    int sections = 0;
    while(!SCANNER_END(s)) {
        if(scanner_get_header(s, &len, &sections) == 1) {
            struct docket *d = docket_new();
            // stripping '[' and ']'
            d->head = copy_slice(SCANNER_CURSOR(s) - len + 1, len - 2);

            scanner_next_line(s);
            if(scanner_get_title(s, &len) == 1) {
                d->title = copy_slice(SCANNER_CURSOR(s) - len, len);
            }

            scanner_next_line(s);
            if(scanner_get_body(s, &len) == 1) {
                d->body = copy_slice(SCANNER_CURSOR(s) - len, len);
            }

            trie_insert_by_path(ds->trie, d->head, (void *)d);
        }
    }
    return ds;
}
