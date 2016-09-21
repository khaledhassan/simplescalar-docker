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

#if !defined(__STDC__) && !defined(__DECC) && !defined(__ANSI_COMPAT)

#define __ANSI_COMPAT

#ifdef ultrix
#define __ultrix      ultrix
#endif

#ifdef __unix__
#define __unix        unix
#endif

#ifdef bsd4_2
#define __bsd4_2      bsd4_2
#endif

#ifdef __vax__
#define __vax         vax
#endif

#ifdef VAX
#define __VAX         VAX
#endif

#ifdef __mips__
#define __mips        mips
#endif

#ifdef __host_mips__
#define __host_mips   host_mips
#endif

#ifdef __MIPSEL__
#define __MIPSEL      MIPSEL
#endif

#ifdef __MIPSEB__
#define __MIPSEB      MIPSEB
#endif

#ifdef SYSTEM_FIVE
#define __SYSTEM_FIVE SYSTEM_FIVE
#endif

#ifdef POSIX
#define __POSIX       POSIX
#endif

#ifdef GFLOAT
#define __GFLOAT        GFLOAT
#endif

#ifdef LANGUAGE_C
#define __LANGUAGE_C  LANGUAGE_C
#endif

#ifdef vaxc
#define __vaxc   vaxc
#define __VAXC   VAXC
#define __vax11c vax11c
#define __VAX11C VAX11C
#endif

#ifdef MOXIE
#define __MOXIE   MOXIE
#endif

#ifdef ULTRIX022
#define __ULTRIX022 ULTRIX022
#endif

#endif

#include <sysdeps/unix/sysdep.h>

#define zero    $0      /* wired zero */
#define AT      $at     /* assembler temp */
#define v0      $2      /* return value */
#define v1      $3
#define a0      $4      /* argument registers */
#define a1      $5
#define a2      $6
#define a3      $7
#define t0      $8      /* caller saved */
#define t1      $9
#define t2      $10
#define t3      $11
#define t4      $12
#define t5      $13
#define t6      $14
#define t7      $15
#define s0      $16     /* callee saved */
#define s1      $17
#define s2      $18
#define s3      $19
#define s4      $20
#define s5      $21
#define s6      $22
#define s7      $23
#define t8      $24     /* code generator */
#define t9      $25
#define k0      $26     /* kernel temporary */
#define k1      $27
#define gp      $28     /* global pointer */
#define sp      $29     /* stack pointer */
#define s8      $30     /* one more callee saved */
#define ra      $31     /* return address */

#ifdef __STDC__
#define ENTRY(name) \
  .globl name;								      \
  .align 2;								      \
  name##:
#else
#define ENTRY(name) \
  .globl name;								      \
  .align 2;								      \
  name/**/:
#endif

/* Note that while it's better structurally, going back to call syscall_error
   can make things confusing if you're debugging---it looks like it's jumping
   backwards into the previous fn.  */
#ifdef __STDC__
#define PSEUDO(name, syscall_name, args) \
  .set noreorder;							      \
  .align 2;								      \
  99: j syscall_error;							      \
  nop;							      		      \
  ENTRY(name)								      \
  li v0, SYS_##syscall_name;						      \
  syscall;								      \
  bne a3, zero, 99b;							      \
  nop;									      \
syse1:
#else
#define PSEUDO(name, syscall_name, args) \
  .set noreorder;							      \
  .align 2;								      \
  99: j syscall_error;							      \
  nop;							      		      \
  ENTRY(name)								      \
  li v0, SYS_/**/syscall_name;						      \
  syscall;								      \
  bne a3, zero, 99b;							      \
  nop;									      \
syse1:
#endif

#define ret	j ra ; nop
#define r0	v0
#define r1	v1
/* The mips move insn is d,s.  */
#define MOVE(x,y)	move y , x
