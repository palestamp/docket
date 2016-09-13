#ifndef _H_DOCKET
#define _H_DOCKET


/*
 * Main docket struct
 */
struct docket {
    const char *head;
    const char *title;
    const char *body;
};

struct docket* docket_new(void);
const char* docket_serialize(struct docket *d);

#endif
