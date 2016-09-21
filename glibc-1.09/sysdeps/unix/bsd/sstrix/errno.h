/* Copyright (C) 1991, 1992, 1993, 1994 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the, 1992 Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

/*
 *	ANSI Standard: 4.1.3 Errors	<errno.h>
 */

#ifndef	_ERRNO_H

#define	_ERRNO_H	1
#include <features.h>

__BEGIN_DECLS

/* Get the error number constants.  */

#ifdef _ERRNO_H
#define EPERM 1
#define ENOENT 2
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define ENXIO 6
#define E2BIG 7
#define ENOEXEC 8
#define EBADF 9
#define ECHILD 10
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define ENOTBLK 15
#define EBUSY 16
#define EEXIST 17
#define EXDEV 18
#define ENODEV 19
#define ENOTDIR 20
#define EISDIR 21
#define EINVAL 22
#define ENFILE 23
#define EMFILE 24
#define ENOTTY 25
#define ETXTBSY 26
#define EFBIG 27
#define ENOSPC 28
#define ESPIPE 29
#define EROFS 30
#define EMLINK 31
#define EPIPE 32
#endif /* <errno.h> included.  */
#if !defined(__Emath_defined) &&  (defined(_ERRNO_H) || defined(__need_Emath))
#define EDOM 33
#endif /* Emath not defined and <errno.h> included or need Emath.  */
#ifdef _ERRNO_H
#endif /* <errno.h> included.  */
#if !defined(__Emath_defined) &&  (defined(_ERRNO_H) || defined(__need_Emath))
#define ERANGE 34
#endif /* Emath not defined and <errno.h> included or need Emath.  */
#ifdef _ERRNO_H
#define EWOULDBLOCK EAGAIN /* Translated in glibc. */
#define EWOULDBLOCK_sys /* Value actually returned by kernel. */ 11
#define EINPROGRESS 36
#define EALREADY 37
#define ENOTSOCK 38
#define EDESTADDRREQ 39
#define EMSGSIZE 40
#define EPROTOTYPE 41
#define ENOPROTOOPT 42
#define EPROTONOSUPPORT 43
#define ESOCKTNOSUPPORT 44
#define EOPNOTSUPP 45
#define EPFNOSUPPORT 46
#define EAFNOSUPPORT 47
#define EADDRINUSE 48
#define EADDRNOTAVAIL 49
#define ENETDOWN 50
#define ENETUNREACH 51
#define ENETRESET 52
#define ECONNABORTED 53
#define ECONNRESET 54
#define ENOBUFS 55
#define EISCONN 56
#define ENOTCONN 57
#define ESHUTDOWN 58
#define ETOOMANYREFS 59
#define ETIMEDOUT 60
#define ECONNREFUSED 61
#define ELOOP 62
#define ENAMETOOLONG 63
#define EHOSTDOWN 64
#define EHOSTUNREACH 65
#define ENOTEMPTY 66
#define EPROCLIM 67
#define EUSERS 68
#define EDQUOT 69
#define ESTALE 70
#define EREMOTE 71
#define ENOMSG 72
#define EIDRM 73
#define EALIGN 74
#define EDEADLK 35
#define ENOLCK 75
#define ENOSYS 76
#define EACTIVE 77
#define ENOACTIVE 78
#define ENORESOURCES 79
#define ENOSYSTEM 80
#define ENODUST 81
#define EDUPNOCONN 82
#define EDUPNODISCONN 83
#define EDUPNOTCNTD 84
#define EDUPNOTIDLE 85
#define EDUPNOTWAIT 86
#define EDUPNOTRUN 87
#define EDUPBADOPCODE 88
#define EDUPINTRANSIT 89
#define EDUPTOOMANYCPUS 90
#define EPERM 1
#define ENOENT 2
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define ENXIO 6
#define E2BIG 7
#define ENOEXEC 8
#define EBADF 9
#define ECHILD 10
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define ENOTBLK 15
#define EBUSY 16
#define EEXIST 17
#define EXDEV 18
#define ENODEV 19
#define ENOTDIR 20
#define EISDIR 21
#define EINVAL 22
#define ENFILE 23
#define EMFILE 24
#define ENOTTY 25
#define ETXTBSY 26
#define EFBIG 27
#define ENOSPC 28
#define ESPIPE 29
#define EROFS 30
#define EMLINK 31
#define EPIPE 32
#define EDOM 33
#define ERANGE 34
#define EWOULDBLOCK EAGAIN /* Translated in glibc. */
#define EWOULDBLOCK_sys /* Value actually returned by kernel. */ 11
#define EINPROGRESS 36
#define EALREADY 37
#define ENOTSOCK 38
#define EDESTADDRREQ 39
#define EMSGSIZE 40
#define EPROTOTYPE 41
#define ENOPROTOOPT 42
#define EPROTONOSUPPORT 43
#define ESOCKTNOSUPPORT 44
#define EOPNOTSUPP 45
#define EPFNOSUPPORT 46
#define EAFNOSUPPORT 47
#define EADDRINUSE 48
#define EADDRNOTAVAIL 49
#define ENETDOWN 50
#define ENETUNREACH 51
#define ENETRESET 52
#define ECONNABORTED 53
#define ECONNRESET 54
#define ENOBUFS 55
#define EISCONN 56
#define ENOTCONN 57
#define ESHUTDOWN 58
#define ETOOMANYREFS 59
#define ETIMEDOUT 60
#define ECONNREFUSED 61
#define ELOOP 62
#define ENAMETOOLONG 63
#define EHOSTDOWN 64
#define EHOSTUNREACH 65
#define ENOTEMPTY 66
#define EPROCLIM 67
#define EUSERS 68
#define EDQUOT 69
#define ESTALE 70
#define EREMOTE 71
#define ENOMSG 72
#define EIDRM 73
#define EALIGN 74
#define EDEADLK 35
#define ENOLCK 75
#define ENOSYS 76
#define EACTIVE 77
#define ENOACTIVE 78
#define ENORESOURCES 79
#define ENOSYSTEM 80
#define ENODUST 81
#define EDUPNOCONN 82
#define EDUPNODISCONN 83
#define EDUPNOTCNTD 84
#define EDUPNOTIDLE 85
#define EDUPNOTWAIT 86
#define EDUPNOTRUN 87
#define EDUPBADOPCODE 88
#define EDUPINTRANSIT 89
#define EDUPTOOMANYCPUS 90
#endif /* <errno.h> included.  */
#undef __need_Emath
#ifndef __Emath_defined
#define __Emath_defined 1
#endif

/* Declare the `errno' variable.  */
extern int errno;

#ifdef __USE_GNU
/* The full and simple forms of the name with which the program was
   invoked.  These variables are set up automatically at startup based on
   the value of ARGV[0] (this works only if you use GNU ld).  */
extern char *program_invocation_name, *program_invocation_short_name;
#endif

__END_DECLS

#endif /* errno.h	*/
