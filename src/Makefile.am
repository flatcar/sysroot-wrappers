AM_CFLAGS = -D_GNU_SOURCE -Wall
common_sources = common.h args.c execute.c util.c
wrapdir = $(pkglibdir)/wrappers
pkgbindir = $(pkglibdir)/bin

wrap_PROGRAMS = cc_wrap
cc_wrap_SOURCES = cc_wrap.c $(common_sources)
cc_wrap_CFLAGS= $(AM_CFLAGS) -D'MYNAME="cc_wrap"'

sbin_SCRIPTS = sysroot-config
CLEANFILES = $(sbin_SCRIPTS)
EXTRA_DIST = sysroot-config.sh

do_subst = sed -e 's,@pkgbindir@,$(pkgbindir),g'

sysroot-config: sysroot-config.sh Makefile
	$(do_subst) < $(srcdir)/sysroot-config.sh > sysroot-config
	chmod +x sysroot-config
