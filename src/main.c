#include <stdio.h>
#include <string.h>

#include "abspath.h"
#include "commands.h"



int cmd_help(int argc, const char **argv) {
    printf("help\n");
    return 1;
}

static const struct command commands[] = {
    {"add", cmd_add, EMPTY_FLAG},
    {"list", cmd_list, EMPTY_FLAG},
    {"tree", cmd_tree, EMPTY_FLAG},
    {"help", cmd_help, EMPTY_FLAG},
    {NULL, NULL, 0},
};


struct command *
get_command(const char *name) {
    struct command *ccmds = (struct command *)commands;

    for(;(*ccmds).name != NULL; ccmds++) {
        if (!strcmp(ccmds->name, name)) {
            return ccmds;
        }
    }
    return get_command("help");
}


/*
 * Interceptor between caller and real command execution
 * Should handle optional option flags
 */
int
handle_cmd(struct command *cmd, int argc, const char **argv) {
    return cmd->cmd(argc, argv);
}


int
main(int argc, const char **argv) {
    const char *root_cmd = NULL;
    if (argc == 1) {
        root_cmd = "help";
    } else {
        root_cmd = argv[1];
        argv++;
        argc--;
    }
    struct command *cmd = get_command(root_cmd);
    if (cmd) {
        if (handle_cmd(cmd, argc, argv) == 0) {
            cmd = get_command("help");
            return handle_cmd(cmd, argc, argv);
        }
    }

    return 0;
}
