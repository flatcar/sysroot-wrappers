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
arguments if $SYSROOT is defined.


Installation
------------

TODO


License and copyright
---------------------

The copyright for sysroot-wrappers as a whole is as follows:

    Copyright (C) 2013 The CoreOS Authors.
