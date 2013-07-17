#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"

#ifdef __GNUC__
#define ATTR_FORMAT(x, y, z) __attribute__((format (x, y, z)))
#else
#define ATTR_FORMAT(x, y, z)
#endif

#ifndef MYNAME
#define MYNAME PACKAGE_NAME
#endif

#define VERSION_INFO MYNAME \
    " from " PACKAGE_NAME \
    " version " PACKAGE_VERSION

#define str_eq(s1, s2) (strcmp((s1), (s2)) == 0)
#define str_startswith(s, p) (strncmp((s), (p), strlen((p))) == 0)

/* ------------------------------------------------------------------------- */
/* args.c */

struct args {
    char **argv;
    int argc;
};

struct args *args_init(int, char **);
struct args *args_init_from_string(const char *);
struct args *args_copy(struct args *args);
void args_free(struct args *args);
void args_add(struct args *args, const char *s);
void args_add_prefix(struct args *args, const char *s);
void args_extend(struct args *args, struct args *to_append);
void args_pop(struct args *args, int n);
void args_set(struct args *args, int index, const char *value);
void args_strip(struct args *args, const char *prefix);
void args_remove_first(struct args *args);
char *args_to_string(struct args *args);
bool args_equal(struct args *args1, struct args *args2);

/* ------------------------------------------------------------------------- */
/* util.c */

void fatal(const char *format, ...) ATTR_FORMAT(printf, 1, 2);
char *format(const char *format, ...) ATTR_FORMAT(printf, 1, 2);
char *x_strdup(const char *s);
char *x_strndup(const char *s, size_t n);
void *x_realloc(void *ptr, size_t size);
void *x_malloc(size_t size);
void *x_calloc(size_t nmemb, size_t size);
char *basename(const char *s);
char *x_realpath(const char *path);
bool is_absolute_path(const char *path);

/* ------------------------------------------------------------------------- */
/* execute.c */

void execute(char **argv);
char *find_executable(const char *name, const char *exclude_names);
struct args *find_all_executables(const char *name,
                                  const char *exclude_names,
                                  const char *wrapper_names);
void print_command(FILE *fp, char **argv);

/* ------------------------------------------------------------------------- */

#define PATH_DELIM ":"

#endif /* ifndef COMMON_H */
