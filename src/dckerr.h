#ifndef _H_DCKERR
#define _H_DCKERR

enum dckerr {
    DCK_OK = 0,
    DCK_OPTION_MUST_HAVE_ARG_FORMAT,
    DCK_OPTION_REQUIRED_FORMAT,
    DCK_max,
};

const char * dckerr_s(enum dckerr err_code);
#endif
