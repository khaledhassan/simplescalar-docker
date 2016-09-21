# This file was generated automatically by configure.  Do not edit.
# Put configuration parameters in configparms instead.
config-machine = ssbig
config-vendor = na
config-os = sstrix
config-sysdirs = unix/bsd/sstrix/ss unix/bsd/sstrix unix/bsd unix/common unix/inet unix/ss unix posix ss ieee754 generic stub
prefix = /p/galileo/projects/ss2/release/ssbig-na-sstrix
config-defines =  -DHAVE_GNU_AS -DNO_UNDERSCORES -DHAVE_SYS_SIGLIST=1 -DHAVE_PSIGNAL=1
INSTALL = /afs/cs.wisc.edu/p/galileo/projects/ss2/release/glibc-1.09/install.sh -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
RANLIB = ranlib

stddef.h = # The installed <stddef.h> seems to be libc-friendly.
