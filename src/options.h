#ifndef _H_OPTIONS
#define _H_OPTIONS

typedef struct {
    int required : 1;
    int has_args : 1;
} flags;

/*
 * Commad option (see command.h)
 */
struct option {
    const char *short_name;
    const char *long_name;
    flags flags;
    char **value;
};



int options_populate(char *err, int *argc, const char ***argv, struct option sopt[]);
#endif
