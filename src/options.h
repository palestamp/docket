#ifndef _H_OPTIONS
#define _H_OPTIONS

/*
 * Commad option (see command.h)
 */
struct option {
    int short_name;
    const char *long_name;
    void *value;
};

#endif
