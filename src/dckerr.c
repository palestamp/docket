#include <stdlib.h>
#include "dckerr.h"


static const char *dckerrors[23];
static int inited = 0;

static void
initerr() {
    if (inited != 0) { return; }

    dckerrors[DCK_OK] = NULL;
    dckerrors[DCK_OPTION_MUST_HAVE_ARG_FORMAT] = "'%s' option must have argument";
    dckerrors[DCK_OPTION_REQUIRED_FORMAT] = "'%s' option is required";
}

const char *
dckerr_s(enum dckerr err_code) {
    initerr();
    if (err_code > 0 && err_code < DCK_max) {
        return dckerrors[err_code];
    }
    return NULL;
}
