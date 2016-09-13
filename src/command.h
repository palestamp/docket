#ifndef _H_COMMAND
#define _H_COMMAND


/*
 * Main command handler
 */
struct command {
    const char *name;
    int(*cmd)(int argc, const char **argv);
};

#endif
