/*
 * Copyright (C) 2002 Andrew Tridgell
 * Copyright (C) 2011 Joel Rosdahl
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

#include <errno.h>
#include <sys/stat.h>

#include "common.h"

static char *find_executable_in_path(const char *name,
                                     struct args *exclude_list,
                                     char *path);
static char *find_base_in_list(const char *name, struct args *file_list);

/*
 * execute a wrapped program, the full path to run is in argv[0]
*/
void
execute(char **argv)
{
    char *verbose;

    verbose = getenv("SYSROOT_WRAPPERS_VERBOSE");
    if (verbose && strlen(verbose)) {
        fprintf(stderr, "%s: Executing ", MYNAME);
        print_command(stderr, argv);
    }

    execv(argv[0], argv);
    fatal("execv failed: %s", strerror(errno));
}

/*
 * Find an executable by name in $PATH. Exclude any that are links to
 * items in exclude_list.
*/
char *
find_executable(const char *name, const char *exclude_names)
{
    char *path, *result;
    struct args *exclude_list;

    path = getenv("SYSROOT_WRAPPERS_PATH");
    if (!path) {
        path = getenv("PATH");
    }
    if (!path) {
        fatal("No PATH variable");
    }

    exclude_list = args_init_from_string(exclude_names);
    result = find_executable_in_path(name, exclude_list, path);
    args_free(exclude_list);
    return result;
}

/*
 * Find all executables by name in $PATH one that is not symlinked to a wrapper
 * such as ccache, returning the list of executable paths including each
 * wrapper and the final real target. This ensures that chaining multiple
 * wrapper types works gracefully.
*/
struct args *
find_all_executables(const char *name,
                     const char *exclude_names,
                     const char *wrapper_names)
{
    bool search_more;
    char *path;
    struct args *cmd_list, *exclude_list, *wrapper_list;
    struct stat link_stat;

    path = getenv("SYSROOT_WRAPPERS_PATH");
    if (!path) {
        path = getenv("PATH");
    }
    if (!path) {
        fatal("No PATH variable");
    }

    cmd_list = args_init(0, NULL);
    exclude_list = args_init_from_string(exclude_names);
    wrapper_list = args_init_from_string(wrapper_names);

    /* Continue searching path until a real command is found. */
    do {
        char *cmd_path = find_executable_in_path(name, exclude_list, path);
        if (cmd_path == NULL) {
            goto failed;
        }

        args_add(cmd_list, cmd_path);
        search_more = false;

        if (lstat(cmd_path, &link_stat) == 0 &&
                S_ISLNK(link_stat.st_mode)) {
            char *real_path, *wrapper;

            real_path = x_realpath(cmd_path);
            wrapper = find_base_in_list(real_path, wrapper_list);
            if (wrapper) {
                /* pop off cmd_path, replace with real_path */
                args_pop(cmd_list, 1);
                args_add(cmd_list, real_path);
                args_add(exclude_list, wrapper);
                search_more = true;
            }

            free(real_path);
        }

        free(cmd_path);
    } while (search_more);

    args_free(exclude_list);
    args_free(wrapper_list);
    return cmd_list;

failed:
    args_free(exclude_list);
    args_free(wrapper_list);
    args_free(cmd_list);
    return NULL;
}

static char *
find_executable_in_path(const char *name,
                        struct args *exclude_list,
                        char *path)
{
    char *tok, *saveptr = NULL;

    if (is_absolute_path(name)) {
        return x_strdup(name);
    }

    path = x_strdup(path);

    /* search the path looking for the first compiler of the right name
       that isn't us (or other excluded wrapper) */
    for (tok = strtok_r(path, PATH_DELIM, &saveptr);
         tok;
         tok = strtok_r(NULL, PATH_DELIM, &saveptr)) {
        struct stat st1, st2;
        char *fname = format("%s/%s", tok, name);
        /* look for a normal executable file */
        if (access(fname, X_OK) == 0 &&
            lstat(fname, &st1) == 0 &&
            stat(fname, &st2) == 0 &&
            S_ISREG(st2.st_mode)) {

            /* Check that it isn't a link to an excluded name. */
            if (S_ISLNK(st1.st_mode) &&
                    find_base_in_list(fname, exclude_list) != NULL) {
                continue;
            }

            /* Found it! */
            free(path);
            return fname;
        }
        free(fname);
    }

    free(path);
    return NULL;
}

void
print_command(FILE *fp, char **argv)
{
	int i;
	for (i = 0; argv[i]; i++) {
		fprintf(fp, "%s%s",  (i == 0) ? "" : " ", argv[i]);
	}
	fprintf(fp, "\n");
}

static char *
find_base_in_list(const char *name, struct args *file_list)
{
    char *real, *base;
    int i;

    real = x_realpath(name);
    if (real != NULL) {
        base = basename(real);
        for (i = 0; i < file_list->argc; i++) {
            if (str_eq(base, file_list->argv[i])) {
                free(base);
                free(real);
                return file_list->argv[i];
            }
        }
    }

    return NULL;
}
