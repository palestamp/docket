#include <stdio.h>
#include <string.h>

#include "abspath.h"
#include "command.h"
#include "commands.h"


int cmd_help(int argc, const char **argv);
static const struct command commands[] = {
    {"add", cmd_add},
    {"list", cmd_list},
    {"tree", cmd_tree},
    {"help", cmd_help},
    {NULL, NULL},
};

// XXX SMELLS
int
main(int argc, const char **argv) {
    // skip program name
    argc--; argv++;
    const char *root_cmd = NULL;

    root_cmd =  (argc < 1) ? "help" : argv[0];

    struct command *cmd = get_command(root_cmd, commands);
    if (cmd == NULL) {
        return get_command("help", commands)->cmd(argc, argv);
    }

    if (cmd->cmd(--argc, ++argv) == 0) {
        cmd = get_command("help", commands);
        return cmd->cmd(argc, argv);
    }

    return 0;
}


int
cmd_help(int argc, const char **argv) {
    printf("help\n");
    return 1;
}


