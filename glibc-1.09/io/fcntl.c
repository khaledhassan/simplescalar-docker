/* Copyright (C) 19911993 Free Software Foundation, Inc.
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
#include <fcntl.h>
#include <sys/file.h>

#undef	fcntl

/* fcntl takes a third argument of unknown type, so function_alias cannot
   work for it (without symbol aliases).  We trust that the argument passed
   is always a word the same size as a pointer.  */

#ifdef	HAVE_GNU_LD

#include <gnu-stabs.h>

symbol_alias(__fcntl, fcntl);

#else	/* No GNU stabs.  */

#include <stdarg.h>

int
DEFUN(fcntl, (fd, cmd), int fd AND int cmd DOTS)
{
  va_list args;
  __ptr_t arg;
  va_start (args, cmd);
  arg = va_arg (args, __ptr_t);
  va_end (args);

  return __fcntl (fd, cmd, arg);
}

#endif	/* GNU stabs.  */
