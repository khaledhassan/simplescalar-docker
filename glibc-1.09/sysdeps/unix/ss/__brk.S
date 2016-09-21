/* Copyright (C) 1992 Free Software Foundation, Inc.
   Contributed by Brendan Kehoe (brendan@zen.org).

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

#include <sysdep.h>

#ifndef SYS_brk
#define SYS_brk 17
#endif

#ifndef       HAVE_GNU_LD
#define _end           end
#endif

.data
.sdata
ENTRY(__curbrk)
	.word _end

ENTRY(__minbrk)
	.word _end
.text
.set noreorder
.set noat

ENTRY(__brk)
	/* Minimum is one page.  */
	lw v0, __minbrk

	/* If they ask for less than a page, givvem the whole
	   thing anyway.  */
	sltu AT, a0, v0
	beq AT, zero, down1
	move a0, v0
down1:
	li v0, SYS_brk
	syscall
	bne a3, zero, error

.set at

	/* Update __curbrk and exit cleanly.  */
	sw a0, __curbrk
	move v0, zero
	j ra

	/* What a horrible way to die.  */
error:	j syscall_error
