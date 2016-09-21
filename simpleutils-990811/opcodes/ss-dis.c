/* Print SimpleScalar instructions for GDB, the GNU debugger, or for objdump.
   Copyright 1989, 1991, 1992 Free Software Foundation, Inc.
   Contributed by Nobuyuki Hikichi(hikichi@sra.co.jp).

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <ansidecl.h>
#include "sysdep.h"
#include "dis-asm.h"
#include "opcode/ss.h"

/* Mips instructions are never longer than this many bytes.  */
#define MAXLEN 4

/* FIXME: This should be shared with gdb somehow.  */
#define REGISTER_NAMES 	\
    {	"zero[0]",	"at[1]",	"v0[2]",	"v1[3]",	\
	"a0[4]",	"a1[5]",	"a2[6]",	"a3[7]", 	\
	"t0[8]",	"t1[9]",	"t2[10]",	"t3[11]",	\
	"t4[12]",	"t5[13]",	"t6[14]",	"t7[15]", 	\
	"s0[16]",	"s1[17]",	"s2[18]",	"s3[19]",	\
	"s4[20]",	"s5[21]",	"s6[22]",	"s7[23]", 	\
	"t8[24]",	"t9[25]",	"k0[26]",	"k1[27]",	\
	"gp[28]",	"sp[29]",	"s8[30]",	"ra[31]", 	\
	"sr",	"lo",	"hi",	"bad",	"cause","pc",    		\
	"f0",   "f1",   "f2",   "f3",   "f4",   "f5",   "f6",   "f7", 	\
	"f8",   "f9",   "f10",  "f11",  "f12",  "f13",  "f14",  "f15", 	\
	"f16",  "f17",  "f18",  "f19",  "f20",  "f21",  "f22",  "f23",	\
	"f24",  "f25",  "f26",  "f27",  "f28",  "f29",  "f30",  "f31",	\
	"fsr",  "fir",  "fp",   "inx",  "rand", "tlblo","ctxt", "tlbhi",\
	"epc",  "prid"							\
    }

static CONST char * CONST reg_names[] = REGISTER_NAMES;


/* instruction/address formats */
typedef unsigned int SS_ADDR_TYPE;
typedef struct {
  unsigned int a;              /* simplescalar opcode */
  unsigned int b;              /* simplescalar immediate fields */
} SS_INST_TYPE;
#define SS_INST_SIZE            sizeof(SS_INST_TYPE)

/* SimpleScalar opcode format */
#define RS              (inst.b >> 24)
#define RT              ((inst.b >> 16) & 0xff)
#define RD              ((inst.b >> 8) & 0xff)

#define FS              RS
#define FT              RT
#define FD              RD

#define IMM             ((int)((short)(inst.b & 0xffff)))

#define UIMM            (inst.b & 0xffff)

#define TARG            (inst.b & 0x3ffffff)

/* break code */
#define BCODE           (inst.b & 0xfffff)

#define OFS             IMM             /* alias to IMM */
#define BS              RS              /* alias to rs */

#define SHAMT           (inst.b & 0xff)

/* subroutine */
static void
print_insn_arg (d, l, pc, info)
     const char *d;
     SS_INST_TYPE l;
     bfd_vma pc;
     struct disassemble_info *info;
{
  int delta;
  SS_INST_TYPE inst;

  inst = l;
  switch (*d)
    {
    case ',':
    case '(':
    case ')':
    case '+':
    case '-':
    case '^':
      (*info->fprintf_func) (info->stream, "%c", *d);
      break;

    case 's':
    case 'b':
    case 'r':
    case 'v':
      (*info->fprintf_func) (info->stream, "$%d", RS /*reg_names[RS]*/);
      break;

    case 't':
    case 'w':
      (*info->fprintf_func) (info->stream, "$%d", RT /*reg_names[RT]*/);
      break;

    case 'p':
      /* sign extend the displacement */
      delta = OFS;
      (*info->print_address_func)((delta << 2) + pc + 8, info);
      break;

    case 'c':
      (*info->fprintf_func) (info->stream, "0x%x", BCODE);
      break;

    case 'i':
    case 'u':
      (*info->fprintf_func) (info->stream, "%d", UIMM);
      break;

    case 'j': /* same as i, but sign-extended */
    case 'o':
      delta = OFS;
      (*info->fprintf_func) (info->stream, "%d", delta);
      break;

    case 'a':
      (*info->print_address_func)
	((pc & 0xf0000000) | (TARG << 2), info);
      break;

    case 'd':
      (*info->fprintf_func) (info->stream, "$%d", RD/*reg_names[RD]*/);
      break;

    case 'z':
      (*info->fprintf_func) (info->stream, "$%d", 0/*reg_names[0]*/);
      break;

    case '<':
      (*info->fprintf_func) (info->stream, "0x%x", SHAMT);
      break;

    case 'S':
    case 'V':
      (*info->fprintf_func) (info->stream, "$f%d", FS);
      break;

    case 'T':
    case 'W':
      (*info->fprintf_func) (info->stream, "$f%d", FT);
      break;

    case 'D':
      (*info->fprintf_func) (info->stream, "$f%d", FD);
      break;

    default:
      (*info->fprintf_func) (info->stream,
			     "# internal error, undefined modifier(%c)", *d);
      break;
    }
}

/* Print the ss instruction at address MEMADDR in debugged memory,
   on using INFO.  Returns length of the instruction, in bytes, which is
   always 4.  BIGENDIAN must be 1 if this is big-endian code, 0 if
   this is little-endian code.  */

int
_print_insn_ss (memaddr, word, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
     SS_INST_TYPE word;
{
  register int i;
  register const char *d;

  for (i = 0; i < NUMOPCODES; i++)
    {
      if (ss_opcodes[i].pinfo != INSN_MACRO)
	{
	  register unsigned int match = ss_opcodes[i].match;
	  register unsigned int mask = ss_opcodes[i].mask;
	  if ((word.a & mask) == match)
	    break;
	}
    }

  /* Handle undefined instructions.  */
  if (i == NUMOPCODES)
    {
      (*info->fprintf_func) (info->stream, "0x%08x:%08x", word.a, word.b);
      return 8;
    }

  (*info->fprintf_func) (info->stream, "%s", ss_opcodes[i].name);

  if (!(d = ss_opcodes[i].args))
    return 8;

  (*info->fprintf_func) (info->stream, " ");

  while (*d)
    print_insn_arg (d++, word, memaddr, info);

  return 8;
}

int
print_insn_big_ss (memaddr, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
{
  int status;
  SS_INST_TYPE buffer, insn;

  status = (*info->read_memory_func) (memaddr, (bfd_byte *)&buffer, 8, info);
  if (status == 0)
    {
      insn.a = bfd_getb32((char *)&buffer.a);
      insn.b = bfd_getb32((char *)&buffer.b);
      return _print_insn_ss (memaddr, insn, info);
    }
  else
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }
}

int
print_insn_little_ss (memaddr, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
{
  int status;
  SS_INST_TYPE insn, buffer;

  status = (*info->read_memory_func) (memaddr, (bfd_byte *)&buffer, 8, info);
  if (status == 0)
    {
      insn.a = bfd_getl32((char *)&buffer.a);
      insn.b = bfd_getl32((char *)&buffer.b);
      return _print_insn_ss (memaddr, insn, info);
    }
  else
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }
}
