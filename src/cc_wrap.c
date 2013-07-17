/*
 * Copyright (C) 2013 The CoreOS Authors.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


#ifndef MYNAME
#define MYNAME "cc_wrap"
#endif

#include "common.h"

const char USAGE_TEXT[] =
    VERSION_INFO "\n"
    MYNAME " compiler [compiler options]\n"
    "compiler [compiler options]\n";

int
main(int argc, char *argv[])
{
    char *program_name, *compiler_name, *other_wrappers, *sysroot;
    struct args *compiler_args, *cmd;

    compiler_args = args_init(argc, argv);

    /* check if we were called directly. */
    program_name = basename(compiler_args->argv[0]);
    args_remove_first(compiler_args);
    if (str_eq(program_name, MYNAME)) {
        /* the first argument should be a compiler */
        if (compiler_args->argc < 1 || compiler_args->argv[0][0] == '-') {
            fprintf(stderr, "%s", USAGE_TEXT);
            exit(1);
        }

        compiler_name = x_strdup(compiler_args->argv[0]);
        args_remove_first(compiler_args);
    } else {
        compiler_name = x_strdup(program_name);
    }
    free(program_name);

    other_wrappers = getenv(MYNAME "_OTHER_WRAPPERS");
    if (!other_wrappers) {
        other_wrappers = "ccache distcc";
    }

    cmd = find_all_executables(compiler_name, MYNAME, other_wrappers);
    if (!cmd) {
        fatal("No valid executables named %s found!", compiler_name);
    }
    free(compiler_name);

    sysroot = getenv("SYSROOT");
    if (sysroot) {
        args_add(cmd, "--sysroot");
        args_add(cmd, sysroot);
    }

    args_extend(cmd, compiler_args);
    args_free(compiler_args);

    execute(cmd->argv);
    return 1;
}
