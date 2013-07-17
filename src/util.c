/*
 * Copyright (C) 2002 Andrew Tridgell
 * Copyright (C) 2009-2013 Joel Rosdahl
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

#include <stdarg.h>

#include "common.h"

/* something went badly wrong! */
void
fatal(const char *format, ...)
{
    va_list ap;
    char msg[1000];

    va_start(ap, format);
    vsnprintf(msg, sizeof(msg), format, ap);
    va_end(ap);

    fprintf(stderr, MYNAME ": FATAL: %s\n", msg);

    exit(1);
}

/* Construct a string according to a format. Caller frees. */
char *
format(const char *format, ...)
{
    va_list ap;
    char *ptr = NULL;

    va_start(ap, format);
    if (vasprintf(&ptr, format, ap) == -1) {
        fatal("Out of memory in format");
    }
    va_end(ap);

    if (!*ptr) fatal("Internal error in format");
    return ptr;
}

/*
  this is like strdup() but dies if the malloc fails
*/
char *
x_strdup(const char *s)
{
    char *ret;
    ret = strdup(s);
    if (!ret) {
        fatal("Out of memory in x_strdup");
    }
    return ret;
}

/*
  this is like strndup() but dies if the malloc fails
*/
char *
x_strndup(const char *s, size_t n)
{
    char *ret;
#ifndef HAVE_STRNDUP
    size_t m;

    if (!s)
        return NULL;
    m = 0;
    while (m < n && s[m]) {
        m++;
    }
    ret = malloc(m + 1);
    if (ret) {
        memcpy(ret, s, m);
        ret[m] = '\0';
    }
#else
    ret = strndup(s, n);
#endif
    if (!ret) {
        fatal("x_strndup: Could not allocate %lu bytes", (unsigned long)n);
    }
    return ret;
}

/*
  this is like malloc() but dies if the malloc fails
*/
void *
x_malloc(size_t size)
{
    void *ret;
    if (size == 0) {
        /*
         * malloc() may return NULL if size is zero, so always do this to make sure
         * that the code handles it regardless of platform.
         */
        return NULL;
    }
    ret = malloc(size);
    if (!ret) {
        fatal("x_malloc: Could not allocate %lu bytes", (unsigned long)size);
    }
    return ret;
}

/* This is like calloc() but dies if the allocation fails. */
void *
x_calloc(size_t nmemb, size_t size)
{
    void *ret;
    if (nmemb * size == 0) {
        /*
         * calloc() may return NULL if nmemb or size is 0, so always do this to
         * make sure that the code handles it regardless of platform.
         */
        return NULL;
    }
    ret = calloc(nmemb, size);
    if (!ret) {
        fatal("x_calloc: Could not allocate %lu bytes", (unsigned long)size);
    }
    return ret;
}

/*
  this is like realloc() but dies if the malloc fails
*/
void *
x_realloc(void *ptr, size_t size)
{
    void *p2;
    if (!ptr) return x_malloc(size);
    p2 = realloc(ptr, size);
    if (!p2) {
        fatal("x_realloc: Could not allocate %lu bytes", (unsigned long)size);
    }
    return p2;
}

/* return the base name of a file - caller frees */
char *
basename(const char *s)
{
    char *p;
    p = strrchr(s, '/');
    if (p) s = p + 1;
#ifdef _WIN32
    p = strrchr(s, '\\');
    if (p) s = p + 1;
#endif

    return x_strdup(s);
}

static long
path_max(const char *path)
{
#ifdef PATH_MAX
    (void)path;
    return PATH_MAX;
#elif defined(MAXPATHLEN)
    (void)path;
    return MAXPATHLEN;
#elif defined(_PC_PATH_MAX)
    long maxlen = pathconf(path, _PC_PATH_MAX);
    if (maxlen >= 4096) {
        return maxlen;
    } else {
        return 4096;
    }
#endif
}

/*
  a sane realpath() function, trying to cope with stupid path limits and
  a broken API
*/
char *
x_realpath(const char *path)
{
    long maxlen = path_max(path);
    char *ret, *p;

    ret = x_malloc(maxlen);

#if HAVE_REALPATH
    p = realpath(path, ret);
#else
    /* yes, there are such systems. This replacement relies on
       the fact that when we call x_realpath we only care about symlinks */
    {
        int len = readlink(path, ret, maxlen-1);
        if (len == -1) {
            free(ret);
            return NULL;
        }
        ret[len] = 0;
        p = ret;
    }
#endif
    if (p) {
        p = x_strdup(p);
        free(ret);
        return p;
    }
    free(ret);
    return NULL;
}

/*
 * Return whether path is absolute.
 */
bool
is_absolute_path(const char *path)
{
    return path[0] == '/';
}
