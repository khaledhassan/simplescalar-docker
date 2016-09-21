/* ss.h.  SimpleScalar opcode list.
   Copyright 1993 Free Software Foundation, Inc.
   Contributed by Ralph Campbell and OSF
   Commented and modified by Ian Lance Taylor, Cygnus Support

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

/* These are bit masks and shift counts to use to access the various
   fields of an instruction.  To retrieve the X field of an
   instruction, use the expression
	(i >> OP_SH_X) & OP_MASK_X
   To set the same field (to j), use
	i = (i &~ (OP_MASK_X << OP_SH_X)) | (j << OP_SH_X)

   Make sure you use fields that are appropriate for the instruction,
   of course.  

   The 'i' format uses OP, RS, RT and IMMEDIATE.  

   The 'j' format uses OP and TARGET.

   The 'r' format uses OP, RS, RT, RD, SHAMT and FUNCT.

   The 'b' format uses OP, RS, RT and DELTA.

   The floating point 'i' format uses OP, RS, RT and IMMEDIATE.

   The floating point 'r' format uses OP, FMT, FT, FS, FD and FUNCT.

   A breakpoint instruction uses OP, CODE and SPEC (10 bits of the
   breakpoint instruction are not defined; Kane says the breakpoint
   code field in BREAK is 20 bits; yet MIPS assemblers and debuggers
   only use ten bits).

   The syscall instruction uses SYSCALL.

   The general coprocessor instructions use COPZ.  */

/* This structure holds information for a particular instruction.  */

struct ss_opcode
{
  /* The name of the instruction.  */
  const char *name;
  /* A string describing the arguments for this instruction.  */
  const char *args;
  /* The basic opcode for the instruction.  When assembling, this
     opcode is modified by the arguments to produce the actual opcode
     that is used.  If pinfo is INSN_MACRO, then this is instead the
     ISA level of the macro (0 or 1 is always supported, 2 is ISA 2,
     etc.).  */
  unsigned long match;
  /* If pinfo is not INSN_MACRO, then this is a bit mask for the
     relevant portions of the opcode when disassembling.  If the
     actual opcode anded with the match field equals the opcode field,
     then we have found the correct instruction.  If pinfo is
     INSN_MACRO, then this field is the macro identifier.  */
  unsigned long mask;
  /* For a macro, this is INSN_MACRO.  Otherwise, it is a collection
     of bits describing the instruction, notably any relevant hazard
     information.  */
  unsigned long pinfo;
};

/* These are the characters which may appears in the args field of an
   instruction.  They appear in the order in which the fields appear
   when the instruction is used.  Commas and parentheses in the args
   string are ignored when assembling, and written into the output
   when disassembling.

   Each of these characters corresponds to a mask field defined above.

   "<" 5 bit shift amount (OP_*_SHAMT)
   ">" shift amount between 32 and 63, stored after subtracting 32 (OP_*_SHAMT)
   "a" 26 bit target address (OP_*_TARGET)
   "b" 5 bit base register (OP_*_RS)
   "c" 10 bit breakpoint code (OP_*_CODE)
   "d" 5 bit destination register specifier (OP_*_RD)
   "i" 16 bit unsigned immediate (OP_*_IMMEDIATE)
   "j" 16 bit signed immediate (OP_*_DELTA)
   "k" 5 bit cache opcode in target register position (OP_*_CACHE)
   "o" 16 bit signed offset (OP_*_DELTA)
   "p" 16 bit PC relative branch target address (OP_*_DELTA)
   "r" 5 bit same register used as both source and target (OP_*_RS)
   "s" 5 bit source register specifier (OP_*_RS)
   "t" 5 bit target register (OP_*_RT)
   "u" 16 bit upper 16 bits of address (OP_*_IMMEDIATE)
   "v" 5 bit same register used as both source and destination (OP_*_RS)
   "w" 5 bit same register used as both target and destination (OP_*_RT)
   "C" 25 bit coprocessor function code (OP_*_COPZ)
   "B" 20 bit syscall function code (OP_*_SYSCALL)
   "x" accept and ignore register name
   "z" must be zero register

   Floating point instructions:
   "D" 5 bit destination register (OP_*_FD)
   "S" 5 bit fs source 1 register (OP_*_FS)
   "T" 5 bit ft source 2 register (OP_*_FT)
   "V" 5 bit same register used as floating source and destination (OP_*_FS)
   "W" 5 bit same register used as floating target and destination (OP_*_FT)

   Coprocessor instructions:
   "E" 5 bit target register (OP_*_RT)
   "G" 5 bit destination register (OP_*_RD)

   Macro instructions:
   "A" General 32 bit expression
   "I" 32 bit immediate
   "F" 64 bit floating point constant in .rdata
   "L" 64 bit floating point constant in .lit8
   "f" 32 bit floating point constant
   "l" 32 bit floating point constant in .lit4
*/

/* These are the bits which may be set in the pinfo field of an
   instructions, if it is not equal to INSN_MACRO.  */

/* Instruction is actually a macro.  It should be ignored by the
   disassembler, and requires special treatment by the assembler.  */
#define INSN_MACRO                  0xffffffff

/* This is a list of macro expanded instructions.
 *
 * _I appended means immediate
 * _A appended means address
 * _AB appended means address with base register
 * _D appended means 64 bit floating point constant
 * _S appended means 32 bit floating point constant
 */
enum {
    M_ADD_I,
    M_ADDU_I,
    M_AND_I,
    M_BEQ_I,
    M_BEQL_I,
    M_BGE,
    M_BGE_I,
    M_BGEU,
    M_BGEU_I,
    M_BGT,
    M_BGT_I,
    M_BGTU,
    M_BGTU_I,
    M_BLE,
    M_BLE_I,
    M_BLEU,
    M_BLEU_I,
    M_BLT,
    M_BLT_I,
    M_BLTU,
    M_BLTU_I,
    M_BNE_I,
    M_DIV_3,
    M_DIV_3I,
    M_DIVU_3,
    M_DIVU_3I,
    M_J_A,
    M_JAL_1,
    M_JAL_2,
    M_JAL_A,
    M_LA_AB,
    M_LB_AB,
    M_LB_AB_MINUS,
    M_LB_AB_PLUS,
    M_LB_AB_PRE_MINUS,
    M_LB_AB_PRE_PLUS,
    M_LBU_AB,
    M_LBU_AB_MINUS,
    M_LBU_AB_PLUS,
    M_LBU_AB_PRE_MINUS,
    M_LBU_AB_PRE_PLUS,
    M_LH_AB,
    M_LH_AB_MINUS,
    M_LH_AB_PLUS,
    M_LH_AB_PRE_MINUS,
    M_LH_AB_PRE_PLUS,
    M_LHU_AB,
    M_LHU_AB_MINUS,
    M_LHU_AB_PLUS,
    M_LHU_AB_PRE_MINUS,
    M_LHU_AB_PRE_PLUS,
    M_LI,
    M_LI_D,
    M_LI_DD,
    M_LI_S,
    M_LI_SS,
    M_LW_AB,
    M_LW_AB_MINUS,
    M_LW_AB_PLUS,
    M_LW_AB_PRE_MINUS,
    M_LW_AB_PRE_PLUS,
    M_DLW_AB,
    M_DLW_AB_MINUS,
    M_DLW_AB_PLUS,
    M_DLW_AB_PRE_MINUS,
    M_DLW_AB_PRE_PLUS,
    M_L_S_AB,
    M_L_S_AB_MINUS,
    M_L_S_AB_PLUS,
    M_L_S_AB_PRE_MINUS,
    M_L_S_AB_PRE_PLUS,
    M_L_D_AB,
    M_L_D_AB_MINUS,
    M_L_D_AB_PLUS,
    M_L_D_AB_PRE_MINUS,
    M_L_D_AB_PRE_PLUS,
    M_LWL_AB,
    M_LWR_AB,
    M_MOVE_M1,
    M_MUL,
    M_MUL_I, 
    M_MULO,
    M_MULO_I, 
    M_MULOU,
    M_MULOU_I, 
    M_NOR_I,
    M_OR_I,
    M_REM_3,
    M_REM_3I,
    M_REMU_3,
    M_REMU_3I,
    M_ROL,
    M_ROL_I,
    M_ROR,
    M_ROR_I,
    M_SEQ,
    M_SEQ_I,
    M_SGE,
    M_SGE_I,
    M_SGEU,
    M_SGEU_I,
    M_SGT,
    M_SGT_I,
    M_SGTU,
    M_SGTU_I,
    M_SLE,
    M_SLE_I,
    M_SLEU,
    M_SLEU_I,
    M_SLT_I,
    M_SLTU_I,
    M_SNE,
    M_SNE_I,
    M_SB_AB,
    M_SB_AB_MINUS,
    M_SB_AB_PLUS,
    M_SB_AB_PRE_MINUS,
    M_SB_AB_PRE_PLUS,
    M_SH_AB,
    M_SH_AB_MINUS,
    M_SH_AB_PLUS,
    M_SH_AB_PRE_MINUS,
    M_SH_AB_PRE_PLUS,
    M_SW_AB,
    M_SW_AB_MINUS,
    M_SW_AB_PLUS,
    M_SW_AB_PRE_MINUS,
    M_SW_AB_PRE_PLUS,
    M_DSW_AB,
    M_DSW_AB_MINUS,
    M_DSW_AB_PLUS,
    M_DSW_AB_PRE_MINUS,
    M_DSW_AB_PRE_PLUS,
    M_DSZ_AB,
    M_DSZ_AB_MINUS,
    M_DSZ_AB_PLUS,
    M_DSZ_AB_PRE_MINUS,
    M_DSZ_AB_PRE_PLUS,
    M_S_S_AB,
    M_S_S_AB_MINUS,
    M_S_S_AB_PLUS,
    M_S_S_AB_PRE_MINUS,
    M_S_S_AB_PRE_PLUS,
    M_S_D_AB,
    M_S_D_AB_MINUS,
    M_S_D_AB_PLUS,
    M_S_D_AB_PRE_MINUS,
    M_S_D_AB_PRE_PLUS,
    M_SWL_AB,
    M_SWR_AB,
    M_SUB_I,
    M_SUBU_I,
    M_TRUNCWD,
    M_TRUNCWS,
    M_XOR_I
};

/* The order of overloaded instructions matters.  Label arguments and
   register arguments look the same. Instructions that can have either
   for arguments must apear in the correct order in this table for the
   assembler to pick the right one. In other words, entries with
   immediate operands must apear after the same instruction with
   registers.

   Many instructions are short hand for other instructions (i.e., The
   jal <register> instruction is short for jalr <register>).  */

extern const struct ss_opcode ss_opcodes[];
extern const int bfd_ss_num_opcodes;
#define NUMOPCODES bfd_ss_num_opcodes
