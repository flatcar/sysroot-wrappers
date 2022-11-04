#!/bin/sh
#
# sysroot-config - helper script for sysroot-wrappers
#
# Copyright 2013 The CoreOS Authors.
# Copyright 2003-2010 Superlucidity Services, LLC
# This program licensed under the GNU GPL version 2.
#
# This script is is based on ccache-config which was based on distcc-config

. /etc/init.d/functions.sh 2>/dev/null || {
	ebegin() { echo " * $* ..."; }
	eend() {
		local r=${1:-$?}
		[ ${r} -eq 0 ] && echo " [ OK ]" || echo " [ !! ]"
		return $r
	}
}

# this should be getopt'd someday (override with CC_QUIET=1)
CC_VERBOSE=1
unset _CC_QUIET
c_quiet() {
	[ -n "${CC_QUIET:-${_CC_QUIET}}" ] || [ -z "${CC_VERBOSE}" ]
}

c_ebegin() { c_quiet || ebegin "$@" ; }
c_eend()   { c_quiet || eend "$@" ; }

###
# the following functions manage the wrapper symlinks
#  they allow the user or other scripts (namely gcc-config) to
#  automatically update the links when upgrading toolchains
#
cc_path() {
	echo ${ROOT%/}@pkgbindir@/$1
}
cc_remove_link() {
	local t=$(cc_path "$1")
	if [ -L ${t} ]; then
		c_ebegin "Removing ${t}"
		rm -f "${t}"
		c_eend $?
	fi
}
cc_install_link() {
	# Search the PATH for the specified compiler
	#  then create shadow link in /usr/lib/sysroot-wrappers/bin to cc-wrap

	if [ -n "$(type -p ${1})" ]; then
		# first be sure any old link is removed
		_CC_QUIET=1
		cc_remove_link "${1}"
		unset _CC_QUIET

		# then create the new link
		local t=$(cc_path "$1")
		c_ebegin "Creating sysroot shadow link ${t}"
		mkdir -p -m 0755 "${t%/*}" && ln -s "../wrappers/cc_wrap" "${t}"
		c_eend $?
	fi
}
cc_links() {
	local a
	for a in gcc cc c++ g++ cpp ; do
		if [ -n "${2}" ] ; then
			a="${2}-${a}"
		fi
		eval "cc_${1}_link" "${a}"
	done
}

###
# main routine

case "${1}" in
	--install-links )
		cc_links install "${2}"
		;;
	--remove-links )
		cc_links remove "${2}"
		;;
	* )
		echo "usage: ${0} {--install-links|--remove-links} [ CHOST ]"
		;;
esac
