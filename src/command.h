#ifndef _H_COMMAND
#define _H_COMMAND

#include "command.h"

enum command_flags {
    EMPTY_FLAG = 0,
    HAVE_SOURCES = 1,
};

/*
 * Main command handler
 */
struct command {
    const char *name;
    int(*cmd)(int argc, const char **argv);
    enum command_flags flags;
};

#endif
