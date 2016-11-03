#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "options.h"
#include "dckerr.h"



int
options_populate(char *err, int *iargc, const char ***iargv, struct option sopt[]) {
    int argc = *iargc;
    const char **argv = *iargv;

    int i;
    for (i = 0; i < argc; i++) {
        // we have options
        if (argv[i] && argv[i][0] == '-' && argv[i][1] != '\0') {

            int sopt_idx = 0;
            while(sopt[sopt_idx].short_name || sopt[sopt_idx].long_name) {
                struct option *o = &(sopt[sopt_idx]);
                if (
                        // short opt
                        (argv[i][1] != '-' && (strcmp((argv[i] + 1), o->short_name) == 0))
                        ||
                        // long opt
                        (argv[i][1] == '-' && (strcmp((argv[i] + 2), o->long_name) == 0))
                   ) {
                    // has args flag
                    if((o->flags.has_args & 1) != 0) {
                        if (argv[i + 1]) {
                            *(o->value) = (char *)(argv[i + 1]);
                            i++;
                        } else {
                            sprintf(err, dckerr_s(DCK_OPTION_MUST_HAVE_ARG_FORMAT),
                                    (o->long_name ? o->long_name : o->short_name));
                            return 1;
                        }
                    }
                }
                sopt_idx++;
            }
        } else {
            break;
        }
    }
    (*iargc) -= i;
    (*iargv) += i;

    int sopt_c_idx = 0;
    while(sopt[sopt_c_idx].short_name || sopt[sopt_c_idx].long_name) {
        struct option *o = &(sopt[sopt_c_idx]);

        if((o->flags.required & 1) != 0 && *(o->value) == NULL) {
            sprintf(err, dckerr_s(DCK_OPTION_REQUIRED_FORMAT),
                    (o->long_name ? o->long_name : o->short_name));
            return 1;
        }
        sopt_c_idx++;
    }

    return 0;
}
