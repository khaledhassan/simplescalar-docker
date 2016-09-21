/* Copyright (C) 1991, 1992, 1993 Free Software Foundation, Inc.
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
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#include <ansidecl.h>
#include <sys/ioctl.h>
#include <gnu-stabs.h>

#undef	ioctl

#ifdef HAVE_GNU_LD

symbol_alias (__ioctl, ioctl);

#else

#include <stdarg.h>

int
DEFUN(ioctl, (fd, request),
      int fd AND unsigned long int request DOTS)
{
  va_list arglist;
  __ptr_t arg;
  va_start (arglist, request);
  arg = va_arg (arglist, __ptr_t);
  va_end (arglist);
  return __ioctl (fd, request, arg);
}

#endif
