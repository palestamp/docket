#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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


void
docket_parse(struct kvsrc *kv, struct scanner *s) {
    // these variav=bles will ben inited in scanner_get_header
    int len = 0;
    int sections = 0;
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
        trie_insert_by_path(kv->trie, d->head, (void *)d);
    }
}

char *
docket_fix_title(const char *title) {
    if (title == NULL) {
        return strdup("@@@@");
    }

    size_t len = strlen(title);
    if (len > 2 && title[2] != ' ') {
        // Nul and space
        char *fixed = malloc(len + 2);
        fixed[0] = '@';
        fixed[1] = '@';
        fixed[2] = ' ';
        memcpy((fixed + 3), (title + 2), len - 2);
        fixed[len + 1] = '\0';
        return fixed;
    }
    return strdup(title);
}

size_t
docket_get_body_len(struct docket *d) {
    if (d->body) {
        return strlen(d->body);
    }
    return 0;
}

int
cmp_str(const void *a, const void *b) {
    if(!(a && b)) {
        return -1;
    }

    const char *ad = (const char *)a;
    const char *bd = (const char *)b;
    return strcmp(ad, bd);
}
