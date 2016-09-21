/* Copyright (C) 1992, 1994 Free Software Foundation, Inc.
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

#include <gnu-stabs.h>

symbol_alias (__hurd_file_name_lookup, hurd_file_name_lookup);
symbol_alias (__hurd_file_name_lookup_retry, hurd_file_name_lookup_retry);
symbol_alias (__file_name_lookup, file_name_lookup);
symbol_alias (__hurd_file_name_split, hurd_file_name_split);
symbol_alias (__file_name_split, file_name_split);

symbol_alias (_hurd_sig_post, hurd_sig_post);

symbol_alias (__hurd_invoke_translator, hurd_invoke_translator);
