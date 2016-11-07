#include <stdlib.h>
#include <string.h>
#include "command.h"

struct command *
get_command(const char *name, const struct command cmds[]) {
    struct command *ccmds = (struct command *)cmds;

    for(;(*ccmds).name != NULL && (*ccmds).cmd != NULL; ccmds++) {
        if (!strcmp(ccmds->name, name)) {
            return ccmds;
        }
    }
    return NULL;
}
