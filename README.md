sysconf-wrappers README
=======================


About
-----

When compiling in an alternate SYSROOT things generally work great as
long as your SYSROOT is the same as the one the toolchain was configured
with when it was compiled, usually /usr/$CHOST. GCC and friends provide
a --sysroot command line argument for changing this at run-time but
don't use the $SYSROOT environment variable so unless every package
build system you encounter is cooperative changing SYSROOT is prone to
errors. Thats where these tools come in. All they do is provide wrappers
for GCC and other utilities that add --sysroot to the command line
arguments if $SYSROOT is defined. The code is based in part on ccache
since it uses the same sort of GCC wrapping scheme.


Installation
------------

TODO


License and copyright
---------------------

Portions of this code came from ccache 3.1.9 and which is licensed under
the GNU GPL version 3 or later. The full license text can be found in
GPL-3.0.txt and at http://www.gnu.org/licenses/gpl-3.0.html. The
copyright for ccache is as follows:

    Copyright (C) 2002-2007 Andrew Tridgell
    Copyright (C) 2009-2011 Joel Rosdahl

The copyright for sysroot-wrappers as a whole is as follows:

    Copyright (C) 2013 The CoreOS Authors.
