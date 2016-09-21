/* ss-opc.c.  SimpleScalar opcode list.
   Copyright 1993 Free Software Foundation, Inc.
   Contributed by Ralph Campbell and OSF
   Commented and modified by Ian Lance Taylor, Cygnus Support
   Heavily modified by Todd M. Austin

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include "ansidecl.h"
#include "opcode/ss.h"

/* The order of overloaded instructions matters.  Label arguments and
   register arguments look the same. Instructions that can have either
   for arguments must apear in the correct order in this table for the
   assembler to pick the right one. In other words, entries with
   immediate operands must apear after the same instruction with
   registers.

   Many instructions are short hand for other instructions (i.e., The
   jal <register> instruction is short for jalr <register>).  */

const struct ss_opcode ss_builtin_opcodes[] = {
/* These instructions appear first so that the disassembler will find
   them first.  The assemblers uses a hash table based on the
   instruction name anyhow.  */

{"nop",     "",		0x00000000, 0x0000ffff,	0		},

{"li",      "t,I",	0,    (int) M_LI,		INSN_MACRO	},
{"move",    "d,s",      0,    (int) M_MOVE_M1,		INSN_MACRO      },

{"abs.s",   "D,V",	0x00000078, 0x0000ffff,		0		},
{"abs.d",   "D,V",	0x00000079, 0x0000ffff,		0		},
{"add",     "d,v,t",	0x00000040, 0x0000ffff,		0		},
{"add",     "t,r,I",	0,    (int) M_ADD_I,		INSN_MACRO	},
{"add.s",   "D,V,T",	0x00000070, 0x0000ffff,		0		},
{"add.d",   "D,V,T",	0x00000071, 0x0000ffff,		0		},
{"addi",    "t,r,j",	0x00000041, 0x0000ffff,		0		},
{"addiu",   "t,r,j",	0x00000043, 0x0000ffff,		0		},
{"addu",    "d,v,t",	0x00000042, 0x0000ffff,		0		},
{"addu",    "t,r,I",	0,    (int) M_ADDU_I,		INSN_MACRO	},
{"and",     "d,v,t",	0x0000004e, 0x0000ffff,		0		},
{"and",     "t,r,I",	0,    (int) M_AND_I,		INSN_MACRO	},
{"andi",    "t,r,i",	0x0000004f, 0x0000ffff,		0		},
{"bc1f",    "p",	0x0000000b, 0x0000ffff,		0		},
{"bc1t",    "p",	0x0000000c, 0x0000ffff,		0		},
{"beq",     "s,t,p",	0x00000005, 0x0000ffff,		0		},
{"beq",     "s,I,p",	0,    (int) M_BEQ_I,		INSN_MACRO	},
{"bge",     "s,t,p",	0,    (int) M_BGE,		INSN_MACRO	},
{"bge",     "s,I,p",	0,    (int) M_BGE_I,		INSN_MACRO	},
{"bgeu",    "s,t,p",	0,    (int) M_BGEU,		INSN_MACRO	},
{"bgeu",    "s,I,p",	0,    (int) M_BGEU_I,		INSN_MACRO	},
{"bgez",    "s,p",      0x0000000a, 0x0000ffff, 	0	        },
{"bgt",     "s,t,p",	0,    (int) M_BGT,		INSN_MACRO	},
{"bgt",     "s,I,p",	0,    (int) M_BGT_I,		INSN_MACRO	},
{"bgtu",    "s,t,p",	0,    (int) M_BGTU,		INSN_MACRO	},
{"bgtu",    "s,I,p",	0,    (int) M_BGTU_I,		INSN_MACRO	},
{"bgtz",    "s,p",	0x00000008, 0x0000ffff,		0		},
{"ble",     "s,t,p",	0,    (int) M_BLE,		INSN_MACRO	},
{"ble",     "s,I,p",	0,    (int) M_BLE_I,		INSN_MACRO	},
{"bleu",    "s,t,p",	0,    (int) M_BLEU,		INSN_MACRO	},
{"bleu",    "s,I,p",	0,    (int) M_BLEU_I,		INSN_MACRO	},
{"blez",    "s,p",	0x00000007, 0x0000ffff,		0	},
{"blt",     "s,t,p",	0,    (int) M_BLT,		INSN_MACRO	},
{"blt",     "s,I,p",	0,    (int) M_BLT_I,		INSN_MACRO	},
{"bltu",    "s,t,p",	0,    (int) M_BLTU,		INSN_MACRO	},
{"bltu",    "s,I,p",	0,    (int) M_BLTU_I,		INSN_MACRO	},
{"bltz",    "s,p",	0x00000009, 0x0000ffff,		0		},
{"bne",     "s,t,p",	0x00000006, 0x0000ffff,		0		},
{"bne",     "s,I,p",	0,    (int) M_BNE_I,		INSN_MACRO	},

{"break",   "",		0x000000a1, 0x0000ffff,		0		},
{"break",   "c",	0x000000a1, 0x0000ffff,		0		},

{"c.eq.d",  "S,T",	0x00000091, 0x0000ffff,		0		},
{"c.eq.s",  "S,T",	0x00000090, 0x0000ffff,		0		},
{"c.lt.d",  "S,T",	0x00000093, 0x0000ffff,		0		},
{"c.lt.s",  "S,T",	0x00000092, 0x0000ffff,		0		},
{"c.le.d",  "S,T",	0x00000095, 0x0000ffff,		0		},
{"c.le.s",  "S,T",	0x00000094, 0x0000ffff,		0		},

{"cfc1",    "t,S",	0x000000a4, 0x0000ffff,		0		},
{"ctc1",    "t,S",	0x000000a6, 0x0000ffff, 	0		},

{"cvt.d.s", "D,S",	0x00000082, 0x0000ffff,		0		},
{"cvt.d.w", "D,S",	0x00000083, 0x0000ffff,		0		},
{"cvt.s.d", "D,S",	0x00000080, 0x0000ffff,		0		},
{"cvt.s.w", "D,S",	0x00000081, 0x0000ffff, 	0		},
{"cvt.w.d", "D,S",	0x00000085, 0x0000ffff,		0		},
{"cvt.w.s", "D,S",	0x00000084, 0x0000ffff,		0		},

/* The SS assembler treats the div opcode with two operands as
   though the first operand appeared twice (the first operand is both
   a source and a destination).  To get the div machine instruction,
   you must use an explicit destination of $0.  */
{"div",     "z,s,t",	0x00000048, 0x0000ffff,		0		},
{"div",     "d,v,t",	0,    (int) M_DIV_3,		INSN_MACRO	},
{"div",     "d,v,I",	0,    (int) M_DIV_3I,		INSN_MACRO	},
{"div.d",   "D,V,T",	0x00000077, 0x0000ffff,		0		},
{"div.s",   "D,V,T",	0x00000076, 0x0000ffff,		0		},
/* For divu, see the comments about div.  */
{"divu",    "z,s,t",	0x00000049, 0x0000ffff,		0		},
{"divu",    "d,v,t",	0,    (int) M_DIVU_3,		INSN_MACRO	},
{"divu",    "d,v,I",	0,    (int) M_DIVU_3I,		INSN_MACRO	},

{"jr",      "s",	0x00000003, 0x0000ffff,		0		},
{"j",       "s",	0x00000003, 0x0000ffff,		0		},
{"j",       "a",	0x00000001, 0x0000ffff,		0		},
{"jalr",    "d,s",	0x00000004, 0x0000ffff,		0		},
{"jal",     "d,s",	0,     (int) M_JAL_2,		INSN_MACRO	},
{"jal",     "s",	0,     (int) M_JAL_1,		INSN_MACRO	},
{"jal",     "a",	0,     (int) M_JAL_A,		INSN_MACRO	},
/* This form of jal is used by the disassembler and internally by the
   assembler, but will never match user input (because the line above
   will match first).  */
{"jal",     "a",	0x00000002, 0x0000ffff,		0		},

{"la",      "t,A(b)",	0,    (int) M_LA_AB,		INSN_MACRO	},

{"lb",      "t,(b+d)",	0x000000c0, 0x0000ffff,		0		},
{"lb",      "t,(b+d)+",	0x000001c0, 0x0000ffff,		0		},
{"lb",      "t,(b+d)-",	0x000002c0, 0x0000ffff,		0		},
{"lb",      "t,(b+d)^+",0x000003c0, 0x0000ffff,		0		},
{"lb",      "t,(b+d)^-",0x000004c0, 0x0000ffff, 	0		},
{"lb",      "t,o(b)",	0x00000020, 0x0000ffff,		0		},
{"lb",      "t,o(b)+",	0x00000120, 0x0000ffff,		0		},
{"lb",      "t,o(b)-",	0x00000220, 0x0000ffff,		0		},
{"lb",      "t,o(b)^+",	0x00000320, 0x0000ffff,		0		},
{"lb",      "t,o(b)^-",	0x00000420, 0x0000ffff,		0		},
{"lb",      "t,A(b)",	0,    (int) M_LB_AB,		INSN_MACRO	},
{"lb",      "t,A(b)+",	0,    (int) M_LB_AB_PLUS,	INSN_MACRO	},
{"lb",      "t,A(b)-",	0,    (int) M_LB_AB_MINUS,	INSN_MACRO	},
{"lb",      "t,A(b)^+",	0,    (int) M_LB_AB_PRE_PLUS,	INSN_MACRO	},
{"lb",      "t,A(b)^-",	0,    (int) M_LB_AB_PRE_MINUS,	INSN_MACRO	},

{"lbu",     "t,(b+d)",	0x000000c1, 0x0000ffff,		0		},
{"lbu",     "t,(b+d)+", 0x000001c1, 0x0000ffff,		0		},
{"lbu",     "t,(b+d)-",	0x000002c1, 0x0000ffff,		0		},
{"lbu",     "t,(b+d)^+",0x000003c1, 0x0000ffff,		0		},
{"lbu",     "t,(b+d)^-",0x000004c1, 0x0000ffff, 	0		},
{"lbu",     "t,o(b)",	0x00000022, 0x0000ffff,		0		},
{"lbu",     "t,o(b)+",	0x00000122, 0x0000ffff, 	0		},
{"lbu",     "t,o(b)-",	0x00000222, 0x0000ffff, 	0		},
{"lbu",     "t,o(b)^+",	0x00000322, 0x0000ffff,		0		},
{"lbu",     "t,o(b)^-",	0x00000422, 0x0000ffff,		0		},
{"lbu",     "t,A(b)",	0,    (int) M_LBU_AB,		INSN_MACRO	},
{"lbu",     "t,A(b)+",	0,    (int) M_LBU_AB_PLUS,	INSN_MACRO	},
{"lbu",     "t,A(b)-",	0,    (int) M_LBU_AB_MINUS,	INSN_MACRO	},
{"lbu",     "t,A(b)^+",	0,    (int) M_LBU_AB_PRE_PLUS,	INSN_MACRO	},
{"lbu",     "t,A(b)^-",	0,    (int) M_LBU_AB_PRE_MINUS,	INSN_MACRO	},

{"lh",      "t,(b+d)",	0x000000c2, 0x0000ffff,		0		},
{"lh",      "t,(b+d)+",	0x000001c2, 0x0000ffff,		0		},
{"lh",      "t,(b+d)-",	0x000002c2, 0x0000ffff, 	0		},
{"lh",      "t,(b+d)^+",0x000003c2, 0x0000ffff,		0		},
{"lh",      "t,(b+d)^-",0x000004c2, 0x0000ffff,		0		},
{"lh",      "t,o(b)",	0x00000024, 0x0000ffff,		0		},
{"lh",      "t,o(b)+",	0x00000124, 0x0000ffff, 	0		},
{"lh",      "t,o(b)-",	0x00000224, 0x0000ffff,		0		},
{"lh",      "t,o(b)^+",	0x00000324, 0x0000ffff,		0		},
{"lh",      "t,o(b)^-",	0x00000424, 0x0000ffff,		0		},
{"lh",      "t,A(b)",	0,    (int) M_LH_AB,		INSN_MACRO	},
{"lh",      "t,A(b)+",	0,    (int) M_LH_AB_PLUS,	INSN_MACRO	},
{"lh",      "t,A(b)-",	0,    (int) M_LH_AB_MINUS,	INSN_MACRO	},
{"lh",      "t,A(b)^+",	0,    (int) M_LH_AB_PRE_PLUS,	INSN_MACRO	},
{"lh",      "t,A(b)^-",	0,    (int) M_LH_AB_PRE_MINUS,	INSN_MACRO	},

{"lhu",     "t,(b+d)",	0x000000c3, 0x0000ffff,		0		},
{"lhu",     "t,(b+d)+",	0x000001c3, 0x0000ffff,		0		},
{"lhu",     "t,(b+d)-",	0x000002c3, 0x0000ffff,		0		},
{"lhu",     "t,(b+d)^+",0x000003c3, 0x0000ffff,		0		},
{"lhu",     "t,(b+d)^-",0x000004c3, 0x0000ffff,		0		},
{"lhu",     "t,o(b)",	0x00000026, 0x0000ffff,		0		},
{"lhu",     "t,o(b)+",	0x00000126, 0x0000ffff,		0		},
{"lhu",     "t,o(b)-",	0x00000226, 0x0000ffff,		0		},
{"lhu",     "t,o(b)^+",	0x00000326, 0x0000ffff,		0		},
{"lhu",     "t,o(b)^-",	0x00000426, 0x0000ffff,		0		},
{"lhu",     "t,A(b)",	0,    (int) M_LHU_AB,		INSN_MACRO	},
{"lhu",     "t,A(b)+",	0,    (int) M_LHU_AB_PLUS,	INSN_MACRO	},
{"lhu",     "t,A(b)-",	0,    (int) M_LHU_AB_MINUS,	INSN_MACRO	},
{"lhu",     "t,A(b)^+",	0,    (int) M_LHU_AB_PRE_PLUS,	INSN_MACRO	},
{"lhu",     "t,A(b)^-",	0,    (int) M_LHU_AB_PRE_MINUS,	INSN_MACRO	},

{"lw",      "t,(b+d)",	0x000000c4, 0x0000ffff,		0		},
{"lw",      "t,(b+d)+",	0x000001c4, 0x0000ffff, 	0		},
{"lw",      "t,(b+d)-",	0x000002c4, 0x0000ffff,		0		},
{"lw",      "t,(b+d)^+",0x000003c4, 0x0000ffff,		0		},
{"lw",      "t,(b+d)^-",0x000004c4, 0x0000ffff,		0		},
{"lw",      "t,o(b)",	0x00000028, 0x0000ffff,		0		},
{"lw",      "t,o(b)+",	0x00000128, 0x0000ffff,		0		},
{"lw",      "t,o(b)-",	0x00000228, 0x0000ffff,		0		},
{"lw",      "t,o(b)^+",	0x00000328, 0x0000ffff,		0		},
{"lw",      "t,o(b)^-",	0x00000428, 0x0000ffff,		0		},
{"lw",      "t,A(b)",	0,    (int) M_LW_AB,		INSN_MACRO	},
{"lw",      "t,A(b)+",	0,    (int) M_LW_AB_PLUS,	INSN_MACRO	},
{"lw",      "t,A(b)-",	0,    (int) M_LW_AB_MINUS,	INSN_MACRO	},
{"lw",      "t,A(b)^+",	0,    (int) M_LW_AB_PRE_PLUS,	INSN_MACRO	},
{"lw",      "t,A(b)^-",	0,    (int) M_LW_AB_PRE_MINUS,	INSN_MACRO	},

{"dlw",     "t,(b+d)",	0x000000ce, 0x0000ffff,		0		},
{"dlw",     "t,(b+d)+",	0x000001ce, 0x0000ffff, 	0		},
{"dlw",     "t,(b+d)-",	0x000002ce, 0x0000ffff,		0		},
{"dlw",     "t,(b+d)^+",0x000003ce, 0x0000ffff,		0		},
{"dlw",     "t,(b+d)^-",0x000004ce, 0x0000ffff,		0		},
{"dlw",     "t,o(b)",	0x00000029, 0x0000ffff,		0		},
{"dlw",     "t,o(b)+",	0x00000129, 0x0000ffff,		0		},
{"dlw",     "t,o(b)-",	0x00000229, 0x0000ffff,		0		},
{"dlw",     "t,o(b)^+",	0x00000329, 0x0000ffff,		0		},
{"dlw",     "t,o(b)^-",	0x00000429, 0x0000ffff,		0		},
{"dlw",     "t,A(b)",	0,    (int) M_DLW_AB,		INSN_MACRO	},
{"dlw",     "t,A(b)+",	0,    (int) M_DLW_AB_PLUS,	INSN_MACRO	},
{"dlw",     "t,A(b)-",	0,    (int) M_DLW_AB_MINUS,	INSN_MACRO	},
{"dlw",     "t,A(b)^+",	0,    (int) M_DLW_AB_PRE_PLUS,	INSN_MACRO	},
{"dlw",     "t,A(b)^-",	0,    (int) M_DLW_AB_PRE_MINUS,	INSN_MACRO	},

{"l.s",     "T,(b+d)",	0x000000c5, 0x0000ffff,		0		},
{"l.s",     "T,(b+d)+",	0x000001c5, 0x0000ffff,		0		},
{"l.s",     "T,(b+d)-",	0x000002c5, 0x0000ffff,		0		},
{"l.s",     "T,(b+d)^+",0x000003c5, 0x0000ffff,		0		},
{"l.s",     "T,(b+d)^-",0x000004c5, 0x0000ffff,		0		},
{"l.s",     "T,o(b)",	0x0000002a, 0x0000ffff,		0		},
{"l.s",     "T,o(b)+",	0x0000012a, 0x0000ffff,		0		},
{"l.s",     "T,o(b)-",	0x0000022a, 0x0000ffff,		0		},
{"l.s",     "T,o(b)^+",	0x0000032a, 0x0000ffff,		0		},
{"l.s",     "T,o(b)^-",	0x0000042a, 0x0000ffff,		0		},
{"l.s",     "T,A(b)",	0,    (int) M_L_S_AB,		INSN_MACRO	},
{"l.s",     "T,A(b)+",	0,    (int) M_L_S_AB_PLUS,	INSN_MACRO	},
{"l.s",     "T,A(b)-",	0,    (int) M_L_S_AB_MINUS,	INSN_MACRO	},
{"l.s",     "T,A(b)^+",	0,    (int) M_L_S_AB_PRE_PLUS,	INSN_MACRO	},
{"l.s",     "T,A(b)^-",	0,    (int) M_L_S_AB_PRE_MINUS,	INSN_MACRO	},

{"l.d",     "T,(b+d)",	0x000000cf, 0x0000ffff,		0		},
{"l.d",     "T,(b+d)+",	0x000001cf, 0x0000ffff,		0		},
{"l.d",     "T,(b+d)-",	0x000002cf, 0x0000ffff,		0		},
{"l.d",     "T,(b+d)^+",0x000003cf, 0x0000ffff,		0		},
{"l.d",     "T,(b+d)^-",0x000004cf, 0x0000ffff,		0		},
{"l.d",     "T,o(b)",	0x0000002b, 0x0000ffff,		0		},
{"l.d",     "T,o(b)+",	0x0000012b, 0x0000ffff,		0		},
{"l.d",     "T,o(b)-",	0x0000022b, 0x0000ffff,		0		},
{"l.d",     "T,o(b)^+",	0x0000032b, 0x0000ffff,		0		},
{"l.d",     "T,o(b)^-",	0x0000042b, 0x0000ffff,		0		},
{"l.d",     "T,A(b)",	0,    (int) M_L_D_AB,		INSN_MACRO	},
{"l.d",     "T,A(b)+",	0,    (int) M_L_D_AB_PLUS,	INSN_MACRO	},
{"l.d",     "T,A(b)-",	0,    (int) M_L_D_AB_MINUS,	INSN_MACRO	},
{"l.d",     "T,A(b)^+",	0,    (int) M_L_D_AB_PRE_PLUS,	INSN_MACRO	},
{"l.d",     "T,A(b)^-",	0,    (int) M_L_D_AB_PRE_MINUS,	INSN_MACRO	},

/* no pre- post-increment/decrement support on these */
{"lwl",     "t,o(b)",	0x0000002c, 0x0000ffff,		0		},
{"lwl",     "t,A(b)",	0,    (int) M_LWL_AB,		INSN_MACRO	},
{"lwr",     "t,o(b)",	0x0000002d, 0x0000ffff, 	0		},
{"lwr",     "t,A(b)",	0,    (int) M_LWR_AB,		INSN_MACRO	},

/* li is at the start of the table.  */
{"li.d",    "t,F",	0,    (int) M_LI_D,		INSN_MACRO	},
{"li.d",    "T,L",	0,    (int) M_LI_DD,		INSN_MACRO	},
{"li.s",    "t,f",	0,    (int) M_LI_S,		INSN_MACRO	},
{"li.s",    "T,l",	0,    (int) M_LI_SS,		INSN_MACRO	},
{"lui",     "t,u",	0x000000a2, 0x0000ffff,		0		},

{"mfc1",    "t,S",	0x000000a3, 0x0000ffff,		0		},
{"dmfc1",   "t,S",	0x000000a7, 0x0000ffff,		0		},
{"mfhi",    "d",	0x0000004a, 0x0000ffff,		0		},
{"mflo",    "d",	0x0000004c, 0x0000ffff,		0		},
{"mov.d",   "D,S",	0x0000007b, 0x0000ffff, 	0		},
{"mov.s",   "D,S",	0x0000007a, 0x0000ffff,		0		},

{"mtc1",    "t,S",	0x000000a5, 0x0000ffff,		0		},
{"dmtc1",   "t,S",	0x000000a8, 0x0000ffff,		0		},
{"mthi",    "s",	0x0000004b, 0x0000ffff,		0		},
{"mtlo",    "s",	0x0000004d, 0x0000ffff,		0		},

{"mul.d",   "D,V,T",	0x00000075, 0x0000ffff, 	0		},
{"mul.s",   "D,V,T",	0x00000074, 0x0000ffff,		0		},
{"mul",     "d,v,t",	0,    (int) M_MUL,		INSN_MACRO	},
{"mul",     "d,v,I",	0,    (int) M_MUL_I,		INSN_MACRO	},
{"mulo",    "d,v,t",	0,    (int) M_MULO,		INSN_MACRO	},
{"mulo",    "d,v,I",	0,    (int) M_MULO_I,		INSN_MACRO	},
{"mulou",   "d,v,t",	0,    (int) M_MULOU,		INSN_MACRO	},
{"mulou",   "d,v,I",	0,    (int) M_MULOU_I,		INSN_MACRO	},
{"mult",    "s,t",	0x00000046, 0x0000ffff, 	0		},
{"multu",   "s,t",	0x00000047, 0x0000ffff,		0		},
{"neg.d",   "D,V",	0x0000007d, 0x0000ffff,		0		},
{"neg.s",   "D,V",	0x0000007c, 0x0000ffff, 	0		},

/* nop is at the start of the table.  */
{"nor",     "d,v,t",	0x00000054, 0x0000ffff,		0		},
{"nor",     "d,v,I",	0,    (int) M_NOR_I,		INSN_MACRO	},
{"or",      "d,v,t",	0x00000050, 0x0000ffff,		0		},
{"or",      "t,r,I",	0,    (int) M_OR_I,		INSN_MACRO	},
{"ori",     "t,r,i",	0x00000051, 0x0000ffff,		0		},
{"rem",     "z,s,t",    0x00000048, 0x0000ffff, 	0		},
{"rem",     "d,v,t",    0,    (int) M_REM_3,    	INSN_MACRO      },
{"rem",     "d,v,I",    0,    (int) M_REM_3I,   	INSN_MACRO      },
{"remu",    "z,s,t",    0x00000049, 0x0000ffff, 	0		},
{"remu",    "d,v,t",    0,    (int) M_REMU_3,   	INSN_MACRO      },
{"remu",    "d,v,I",    0,    (int) M_REMU_3I,  	INSN_MACRO      },
{"rol",     "d,v,t",    0,    (int) M_ROL,      	INSN_MACRO      },
{"rol",     "d,v,I",    0,    (int) M_ROL_I,    	INSN_MACRO      },
{"ror",     "d,v,t",    0,    (int) M_ROR,      	INSN_MACRO      },
{"ror",     "d,v,I",    0,    (int) M_ROR_I,    	INSN_MACRO      },

{"sb",      "t,(b+d)",	0x000000c6, 0x0000ffff,		0		},
{"sb",      "t,(b+d)+",	0x000001c6, 0x0000ffff,		0		},
{"sb",      "t,(b+d)-",	0x000002c6, 0x0000ffff,		0		},
{"sb",      "t,(b+d)^+",0x000003c6, 0x0000ffff,		0		},
{"sb",      "t,(b+d)^-",0x000004c6, 0x0000ffff,		0		},
{"sb",      "t,o(b)",	0x00000030, 0x0000ffff,		0		},
{"sb",      "t,o(b)+",	0x00000130, 0x0000ffff,		0		},
{"sb",      "t,o(b)-",	0x00000230, 0x0000ffff,		0		},
{"sb",      "t,o(b)^+",	0x00000330, 0x0000ffff,		0		},
{"sb",      "t,o(b)^-",	0x00000430, 0x0000ffff,		0		},
{"sb",      "t,A(b)",	0,    (int) M_SB_AB,		INSN_MACRO	},
{"sb",      "t,A(b)+",	0,    (int) M_SB_AB_PLUS,	INSN_MACRO	},
{"sb",      "t,A(b)-",	0,    (int) M_SB_AB_MINUS,	INSN_MACRO	},
{"sb",      "t,A(b)^+",	0,    (int) M_SB_AB_PRE_PLUS,	INSN_MACRO	},
{"sb",      "t,A(b)^-",	0,    (int) M_SB_AB_PRE_MINUS,	INSN_MACRO	},

{"sh",      "t,(b+d)",	0x000000c7, 0x0000ffff,		0		},
{"sh",      "t,(b+d)+",	0x000001c7, 0x0000ffff,		0		},
{"sh",      "t,(b+d)-",	0x000002c7, 0x0000ffff,		0		},
{"sh",      "t,(b+d)^+",0x000003c7, 0x0000ffff,		0		},
{"sh",      "t,(b+d)^-",0x000004c7, 0x0000ffff,		0		},
{"sh",      "t,o(b)",	0x00000032, 0x0000ffff,		0		},
{"sh",      "t,o(b)+",	0x00000132, 0x0000ffff,		0		},
{"sh",      "t,o(b)-",	0x00000232, 0x0000ffff,		0		},
{"sh",      "t,o(b)^+",	0x00000332, 0x0000ffff,		0		},
{"sh",      "t,o(b)^-",	0x00000432, 0x0000ffff,		0		},
{"sh",      "t,A(b)",	0,    (int) M_SH_AB,		INSN_MACRO	},
{"sh",      "t,A(b)+",	0,    (int) M_SH_AB_PLUS,	INSN_MACRO	},
{"sh",      "t,A(b)-",	0,    (int) M_SH_AB_MINUS,	INSN_MACRO	},
{"sh",      "t,A(b)^+",	0,    (int) M_SH_AB_PRE_PLUS,	INSN_MACRO	},
{"sh",      "t,A(b)^-",	0,    (int) M_SH_AB_PRE_MINUS,	INSN_MACRO	},

{"sw",      "t,(b+d)",	0x000000c8, 0x0000ffff,		0		},
{"sw",      "t,(b+d)+",	0x000001c8, 0x0000ffff,		0		},
{"sw",      "t,(b+d)-",	0x000002c8, 0x0000ffff,		0		},
{"sw",      "t,(b+d)^+",0x000003c8, 0x0000ffff,		0		},
{"sw",      "t,(b+d)^-",0x000004c8, 0x0000ffff,		0		},
{"sw",      "t,o(b)",	0x00000034, 0x0000ffff,		0		},
{"sw",      "t,o(b)+",	0x00000134, 0x0000ffff,		0		},
{"sw",      "t,o(b)-",	0x00000234, 0x0000ffff,		0		},
{"sw",      "t,o(b)^+",	0x00000334, 0x0000ffff,		0		},
{"sw",      "t,o(b)^-",	0x00000434, 0x0000ffff,		0		},
{"sw",      "t,A(b)",	0,    (int) M_SW_AB,		INSN_MACRO	},
{"sw",      "t,A(b)+",	0,    (int) M_SW_AB_PLUS,	INSN_MACRO	},
{"sw",      "t,A(b)-",	0,    (int) M_SW_AB_MINUS,	INSN_MACRO	},
{"sw",      "t,A(b)^+",	0,    (int) M_SW_AB_PRE_PLUS,	INSN_MACRO	},
{"sw",      "t,A(b)^-",	0,    (int) M_SW_AB_PRE_MINUS,	INSN_MACRO	},

{"dsw",     "t,(b+d)",	0x000000d0, 0x0000ffff,		0		},
{"dsw",     "t,(b+d)+",	0x000001d0, 0x0000ffff,		0		},
{"dsw",     "t,(b+d)-",	0x000002d0, 0x0000ffff,		0		},
{"dsw",     "t,(b+d)^+",0x000003d0, 0x0000ffff,		0		},
{"dsw",     "t,(b+d)^-",0x000004d0, 0x0000ffff,		0		},
{"dsw",     "t,o(b)",	0x00000035, 0x0000ffff,		0		},
{"dsw",     "t,o(b)+",	0x00000135, 0x0000ffff,		0		},
{"dsw",     "t,o(b)-",	0x00000235, 0x0000ffff,		0		},
{"dsw",     "t,o(b)^+",	0x00000335, 0x0000ffff,		0		},
{"dsw",     "t,o(b)^-",	0x00000435, 0x0000ffff,		0		},
{"dsw",     "t,A(b)",	0,    (int) M_DSW_AB,		INSN_MACRO	},
{"dsw",     "t,A(b)+",	0,    (int) M_DSW_AB_PLUS,	INSN_MACRO	},
{"dsw",     "t,A(b)-",	0,    (int) M_DSW_AB_MINUS,	INSN_MACRO	},
{"dsw",     "t,A(b)^+",	0,    (int) M_DSW_AB_PRE_PLUS,	INSN_MACRO	},
{"dsw",     "t,A(b)^-",	0,    (int) M_DSW_AB_PRE_MINUS,	INSN_MACRO	},

{"dsz",     "(b+d)",	0x000000d1, 0x0000ffff,		0		},
{"dsz",     "(b+d)+",	0x000001d1, 0x0000ffff,		0		},
{"dsz",     "(b+d)-",	0x000002d1, 0x0000ffff,		0		},
{"dsz",     "(b+d)^+",	0x000003d1, 0x0000ffff,		0		},
{"dsz",     "(b+d)^-",	0x000004d1, 0x0000ffff,		0		},
{"dsz",     "o(b)",	0x00000038, 0x0000ffff,		0		},
{"dsz",     "o(b)+",	0x00000138, 0x0000ffff,		0		},
{"dsz",     "o(b)-",	0x00000238, 0x0000ffff,		0		},
{"dsz",     "o(b)^+",	0x00000338, 0x0000ffff,		0		},
{"dsz",     "o(b)^-",	0x00000438, 0x0000ffff,		0		},
{"dsz",     "A(b)",	0,    (int) M_DSZ_AB,		INSN_MACRO	},
{"dsz",     "A(b)+",	0,    (int) M_DSZ_AB_PLUS,	INSN_MACRO	},
{"dsz",     "A(b)-",	0,    (int) M_DSZ_AB_MINUS,	INSN_MACRO	},
{"dsz",     "A(b)^+",	0,    (int) M_DSZ_AB_PRE_PLUS,	INSN_MACRO	},
{"dsz",     "A(b)^-",	0,    (int) M_DSZ_AB_PRE_MINUS,	INSN_MACRO	},

{"s.s",     "T,(b+d)",	0x000000c9, 0x0000ffff,		0		},
{"s.s",     "T,(b+d)+",	0x000001c9, 0x0000ffff,		0		},
{"s.s",     "T,(b+d)-",	0x000002c9, 0x0000ffff,		0		},
{"s.s",     "T,(b+d)^+",0x000003c9, 0x0000ffff,		0		},
{"s.s",     "T,(b+d)^-",0x000004c9, 0x0000ffff,		0		},
{"s.s",     "T,o(b)",	0x00000036, 0x0000ffff,		0		},
{"s.s",     "T,o(b)+",	0x00000136, 0x0000ffff,		0		},
{"s.s",     "T,o(b)-",	0x00000236, 0x0000ffff,		0		},
{"s.s",     "T,o(b)^+",	0x00000336, 0x0000ffff,		0		},
{"s.s",     "T,o(b)^-",	0x00000436, 0x0000ffff,		0		},
{"s.s",     "T,A(b)",	0,    (int) M_S_S_AB,		INSN_MACRO	},
{"s.s",     "T,A(b)+",	0,    (int) M_S_S_AB_PLUS,	INSN_MACRO	},
{"s.s",     "T,A(b)-",	0,    (int) M_S_S_AB_MINUS,	INSN_MACRO	},
{"s.s",     "T,A(b)^+",	0,    (int) M_S_S_AB_PRE_PLUS,	INSN_MACRO	},
{"s.s",     "T,A(b)^-",	0,    (int) M_S_S_AB_PRE_MINUS,	INSN_MACRO	},

{"s.d",     "T,(b+d)",	0x000000d2, 0x0000ffff,		0		},
{"s.d",     "T,(b+d)+",	0x000001d2, 0x0000ffff,		0		},
{"s.d",     "T,(b+d)-",	0x000002d2, 0x0000ffff,		0		},
{"s.d",     "T,(b+d)^+",0x000003d2, 0x0000ffff,		0		},
{"s.d",     "T,(b+d)^-",0x000004d2, 0x0000ffff,		0		},
{"s.d",     "T,o(b)",	0x00000037, 0x0000ffff,		0		},
{"s.d",     "T,o(b)+",	0x00000137, 0x0000ffff,		0		},
{"s.d",     "T,o(b)-",	0x00000237, 0x0000ffff,		0		},
{"s.d",     "T,o(b)^+",	0x00000337, 0x0000ffff,		0		},
{"s.d",     "T,o(b)^-",	0x00000437, 0x0000ffff,		0		},
{"s.d",     "T,A(b)",	0,    (int) M_S_D_AB,		INSN_MACRO	},
{"s.d",     "T,A(b)+",	0,    (int) M_S_D_AB_PLUS,	INSN_MACRO	},
{"s.d",     "T,A(b)-",	0,    (int) M_S_D_AB_MINUS,	INSN_MACRO	},
{"s.d",     "T,A(b)^+",	0,    (int) M_S_D_AB_PRE_PLUS,	INSN_MACRO	},
{"s.d",     "T,A(b)^-",	0,    (int) M_S_D_AB_PRE_MINUS,	INSN_MACRO	},

/* no pre- post-increment/decrement support on these, yet */
{"swl",     "t,o(b)",	0x00000039, 0x0000ffff,		0		},
{"swl",     "t,A(b)",	0,    (int) M_SWL_AB,		INSN_MACRO	},
{"swr",     "t,o(b)",	0x0000003a, 0x0000ffff,		0		},
{"swr",     "t,A(b)",	0,    (int) M_SWR_AB,		INSN_MACRO	},

{"sqrt.s",  "D,S",      0x00000096, 0x0000ffff, 	0		},
{"sqrt.d",  "D,S",      0x00000097, 0x0000ffff, 	0		},

{"seq",     "d,v,t",    0,    (int) M_SEQ,      	INSN_MACRO      },
{"seq",     "d,v,I",    0,    (int) M_SEQ_I,    	INSN_MACRO      },
{"sge",     "d,v,t",    0,    (int) M_SGE,      	INSN_MACRO      },
{"sge",     "d,v,I",    0,    (int) M_SGE_I,    	INSN_MACRO      },
{"sgeu",    "d,v,t",    0,    (int) M_SGEU,     	INSN_MACRO      },
{"sgeu",    "d,v,I",    0,    (int) M_SGEU_I,   	INSN_MACRO      },
{"sgt",     "d,v,t",    0,    (int) M_SGT,      	INSN_MACRO      },
{"sgt",     "d,v,I",    0,    (int) M_SGT_I,    	INSN_MACRO      },
{"sgtu",    "d,v,t",    0,    (int) M_SGTU,     	INSN_MACRO      },
{"sgtu",    "d,v,I",    0,    (int) M_SGTU_I,   	INSN_MACRO      },
{"sle",     "d,v,t",    0,    (int) M_SLE,      	INSN_MACRO      },
{"sle",     "d,v,I",    0,    (int) M_SLE_I,    	INSN_MACRO      },
{"sleu",    "d,v,t",    0,    (int) M_SLEU,     	INSN_MACRO      },
{"sleu",    "d,v,I",    0,    (int) M_SLEU_I,   	INSN_MACRO      },
{"slt",     "d,v,t",	0x0000005b, 0x0000ffff,		0		},
{"slt",     "d,v,I",    0,    (int) M_SLT_I,    	INSN_MACRO      },
{"sltu",    "d,v,t",	0x0000005d, 0x0000ffff,		0		},
{"sltu",    "d,v,I",    0,    (int) M_SLTU_I,   	INSN_MACRO      },
{"slti",    "t,r,j",	0x0000005c, 0x0000ffff,		0		},
{"sltiu",   "t,r,j",	0x0000005e, 0x0000ffff,		0		},
{"sne",     "d,v,t",    0,    (int) M_SNE,      	INSN_MACRO      },
{"sne",     "d,v,I",    0,    (int) M_SNE_I,    	INSN_MACRO      },

{"sllv",    "d,t,s",	0x00000056, 0x0000ffff,		0		},
{"sll",     "d,w,s",	0x00000056, 0x0000ffff,		0		},
{"sll",     "d,w,<",	0x00000055, 0x0000ffff,		0		},

{"srav",    "d,t,s",	0x0000005a, 0x0000ffff,		0		},
{"sra",     "d,w,s",	0x0000005a, 0x0000ffff,		0		},
{"sra",     "d,w,<",	0x00000059, 0x0000ffff,		0		},
{"srlv",    "d,t,s",	0x00000058, 0x0000ffff,		0		},
{"srl",     "d,w,s",	0x00000058, 0x0000ffff,		0		},
{"srl",     "d,w,<",	0x00000057, 0x0000ffff,		0		},

{"sub",     "d,v,t",	0x00000044, 0x0000ffff,		0		},
{"sub",     "d,v,I",	0,    (int) M_SUB_I,		INSN_MACRO	},
{"sub.d",   "D,V,T",	0x00000073, 0x0000ffff,		0		},     
{"sub.s",   "D,V,T",	0x00000072, 0x0000ffff,		0		},
{"subu",    "d,v,t",	0x00000045, 0x0000ffff,		0		},
{"subu",    "d,v,I",	0,    (int) M_SUBU_I,		INSN_MACRO	},

{"syscall", "",		0x000000a0, 0x0000ffff,		0		},

{"trunc.w.d", "D,S,t",	0,    (int) M_TRUNCWD,		INSN_MACRO	},
{"trunc.w.s", "D,S,t",	0,    (int) M_TRUNCWS,		INSN_MACRO	},

{"xor",     "d,v,t",	0x00000052, 0x0000ffff,		0		},
{"xor",     "t,r,I",	0,    (int) M_XOR_I,		INSN_MACRO	},
{"xori",    "t,r,i",	0x00000053, 0x0000ffff,		0		},

};

#define SS_NUM_OPCODES \
	((sizeof ss_builtin_opcodes) / (sizeof (ss_builtin_opcodes[0])))
const int bfd_ss_num_builtin_opcodes = SS_NUM_OPCODES;

/* const removed from the following to allow for dynamic extensions to the 
 * built-in instruction set. */
struct ss_opcode *ss_opcodes =
  (struct ss_opcode *) ss_builtin_opcodes;
int bfd_ss_num_opcodes = SS_NUM_OPCODES;
#undef SS_NUM_OPCODES
