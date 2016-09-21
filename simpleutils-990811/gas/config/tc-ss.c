/* tc-ss.c -- assemble code for SimpleScalar.
   based on tc-mips.c
   Todd Austin wrote the first version for 2.5.2 binutils
   Charles Lefurgy updated for 2.9.x binutils

   This file is part of GAS.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include "as.h"
#include "config.h"
#include "subsegs.h"

#include <ctype.h>

#ifdef USE_STDARG
#include <stdarg.h>
#endif
#ifdef USE_VARARGS
#include <varargs.h>
#endif

#include "opcode/ss.h"

#ifdef DEBUG
#define DBG(x) printf x
#else
#define DBG(x)
#endif

#ifdef OBJ_MAYBE_ELF
/* Clean up namespace so we can include obj-elf.h too.  */
static int ss_output_flavor PARAMS ((void));
static int ss_output_flavor () { return OUTPUT_FLAVOR; }
#undef OBJ_PROCESS_STAB
#undef OUTPUT_FLAVOR
#undef S_GET_ALIGN
#undef S_GET_SIZE
#undef S_SET_ALIGN
#undef S_SET_SIZE
#undef obj_frob_file
#undef obj_frob_file_after_relocs
#undef obj_frob_symbol
#undef obj_pop_insert
#undef obj_sec_sym_ok_for_reloc
#undef OBJ_COPY_SYMBOL_ATTRIBUTES

#include "obj-elf.h"
/* Fix any of them that we actually care about.  */
#undef OUTPUT_FLAVOR
#define OUTPUT_FLAVOR ss_output_flavor()
#endif

#if defined (OBJ_ELF)
/*#include "elf/mips.h" */ /* SimpleScalar does not support ELF yet */
#endif

#ifndef ECOFF_DEBUGGING
#define NO_ECOFF_DEBUGGING
#define ECOFF_DEBUGGING 0
#endif

#include "ecoff.h"

#if defined (OBJ_ELF) || defined (OBJ_MAYBE_ELF)
static char *ss_regmask_frag;
#endif

#define AT  1
#define TREG 24
#define PIC_CALL_REG 25
#define KT0 26
#define KT1 27
#define GP  28
#define SP  29
#define FP  30
#define RA  31

#define ILLEGAL_REG (32)

/* Allow override of standard little-endian ECOFF format.  */

/*  #ifndef ECOFF_LITTLE_FORMAT */
/*  #define ECOFF_LITTLE_FORMAT "ss-coff-little" */
/*  #endif */

extern int target_big_endian;

/* 1 is we should use the 64 bit MIPS ELF ABI, 0 if we should use the
   32 bit ABI.  This has no meaning for ECOFF.
   Note that the default is always 32 bit, even if "configured" for
   64 bit [e.g. --target=mips64-elf].  */
static int ss_64;

/* The default target format to use.  */
const char *
ss_target_format ()
{
  switch (OUTPUT_FLAVOR)
    {
/*      case bfd_target_aout_flavour: */
/*        return target_big_endian ? "a.out-ss-big" : "a.out-ss-little"; */
    case bfd_target_ecoff_flavour:
      return target_big_endian ? "ss-coff-big" : "ss-coff-little";
/*      case bfd_target_elf_flavour: */
/*        return (target_big_endian */
/*  	      ? (ss_64 ? "elf64-bigss" : "elf32-bigss") */
/*  	      : (ss_64 ? "elf64-littless" : "elf32-littless")); */
    default:
      abort ();
      return NULL;
    }
}

/* The name of the readonly data section.  */
#define RDATA_SECTION_NAME (OUTPUT_FLAVOR == bfd_target_aout_flavour \
			    ? ".data" \
			    : OUTPUT_FLAVOR == bfd_target_ecoff_flavour \
			    ? ".rdata" \
			    : OUTPUT_FLAVOR == bfd_target_elf_flavour \
			    ? ".rodata" \
			    : (abort (), ""))

/* This is the set of options which may be modified by the .set
   pseudo-op.  We use a struct so that .set push and .set pop are more
   reliable.  */

struct ss_set_options
{
  /* SS ISA (Instruction Set Architecture) level.  This is set to -1
     if it has not been initialized.  Changed by `.set ssN', and the
     -ssN command line option, and the default CPU.  */
  int isa;
  /* Non-zero if we should not reorder instructions.  Changed by `.set
     reorder' and `.set noreorder'.  */
  int noreorder;
  /* Non-zero if we should not permit the $at ($1) register to be used
     in instructions.  Changed by `.set at' and `.set noat'.  */
  int noat;
  /* Non-zero if we should warn when a macro instruction expands into
     more than one machine instruction.  Changed by `.set nomacro' and
     `.set macro'.  */
  int warn_about_macros;
  /* Non-zero if we should not move instructions.  Changed by `.set
     move', `.set volatile', `.set nomove', and `.set novolatile'.  */
  int nomove;
  /* Non-zero if we should not optimize branches by moving the target
     of the branch into the delay slot.  Actually, we don't perform
     this optimization anyhow.  Changed by `.set bopt' and `.set
     nobopt'.  */
  int nobopt;
  /* Non-zero if we should not autoextend mips16 instructions.
     Changed by `.set autoextend' and `.set noautoextend'.  */
  int noautoextend;
};

/* This is the struct we use to hold the current set of options.  Note
   that we must set the isa and mips16 fields to -1 to indicate that
   they have not been initialized.  */

static struct ss_set_options ss_opts = { -1 };

/* These variables are filled in with the masks of registers used.
   The object format code reads them and puts them in the appropriate
   place.  */
unsigned long ss_gprmask;
unsigned long ss_cprmask[4];

/* The argument of the -mabi= flag. */
static char* ss_abi_string = 0;

/* Wether we should mark the file EABI64 or EABI32. */
static int ss_eabi64 = 0;

/* Whether the processor uses hardware interlocks to protect 
   reads from the HI and LO registers, and thus does not
   require nops to be inserted.

   FIXME: GCC makes a distinction between -mcpu=FOO and -mFOO:
   -mcpu=FOO schedules for FOO, but still produces code that meets the
   requirements of MIPS ISA I.  For example, it won't generate any
   FOO-specific instructions, and it will still assume that any
   scheduling hazards described in MIPS ISA I are there, even if FOO
   has interlocks.  -mFOO gives GCC permission to generate code that
   will only run on a FOO; it will generate FOO-specific instructions,
   and assume interlocks provided by a FOO.

   However, GAS currently doesn't make this distinction; before Jan 28
   1999, GAS's -mcpu=FOO implied -mFOO, which violates GCC's
   assumptions.  The GCC driver passes these flags through to GAS, so
   if GAS actually does anything that doesn't meet MIPS ISA I with
   -mFOO, then GCC's -mcpu=FOO flag isn't going to work.

   And furthermore, it did not assume that -mFOO implied -mcpu=FOO,
   which seems senseless --- why generate code which will only run on
   a FOO, but schedule for something else?

   So now, at least, -mcpu=FOO and -mFOO are exactly equivalent.

   -- Jim Blandy <jimb@cygnus.com> */

#define hilo_interlocks 1

/* Whether the processor uses hardware interlocks to protect reads
   from the GPRs, and thus does not require nops to be inserted.  */
#define gpr_interlocks 1

/* As with other "interlocks" this is used by hardware that has FP
   (co-processor) interlocks.  */
/* Itbl support may require additional care here. */
#define cop_interlocks 1

/* MIPS PIC level.  */

enum ss_pic_level
{
  /* Do not generate PIC code.  */
  NO_PIC,

  /* Generate PIC code as in Irix 4.  This is not implemented, and I'm
     not sure what it is supposed to do.  */
  IRIX4_PIC,

  /* Generate PIC code as in the SVR4 MIPS ABI.  */
  SVR4_PIC,

  /* Generate PIC code without using a global offset table: the data
     segment has a maximum size of 64K, all data references are off
     the $gp register, and all text references are PC relative.  This
     is used on some embedded systems.  */
  EMBEDDED_PIC
};

static enum ss_pic_level ss_pic;

/* 1 if we should generate 32 bit offsets from the GP register in
   SVR4_PIC mode.  Currently has no meaning in other modes.  */
static int ss_big_got;

/* 1 if trap instructions should used for overflow rather than break
   instructions.  */
static int ss_trap;

/* Non-zero if any .set noreorder directives were used.  */

static int ss_any_noreorder;

/* The size of the small data section.  */
static int g_switch_value = 8;
/* Whether the -G option was used.  */
static int g_switch_seen = 0;

#define N_RMASK 0xc4
#define N_VFP   0xd4

/* If we can determine in advance that GP optimization won't be
   possible, we can skip the relaxation stuff that tries to produce
   GP-relative references.  This makes delay slot optimization work
   better.

   This function can only provide a guess, but it seems to work for
   gcc output.  If it guesses wrong, the only loss should be in
   efficiency; it shouldn't introduce any bugs.

   I don't know if a fix is needed for the SVR4_PIC mode.  I've only
   fixed it for the non-PIC mode.  KR 95/04/07  */
static int nopic_need_relax PARAMS ((symbolS *, int));

/* handle of the OPCODE hash table */
static struct hash_control *op_hash = NULL;

/* This array holds the chars that always start a comment.  If the
    pre-processor is disabled, these aren't very useful */
const char comment_chars[] = "#";

/* This array holds the chars that only start a comment at the beginning of
   a line.  If the line seems to have the form '# 123 filename'
   .line and .file directives will appear in the pre-processed output */
/* Note that input_file.c hand checks for '#' at the beginning of the
   first line of the input file.  This is because the compiler outputs
   #NO_APP at the beginning of its output. */
/* Also note that C style comments are always supported.  */
const char line_comment_chars[] = "#";

/* This array holds machine specific line separator characters. */
const char line_separator_chars[] = "";

/* Chars that can be used to separate mant from exp in floating point nums */
const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant */
/* As in 0f12.456 */
/* or    0d1.2345e12 */
const char FLT_CHARS[] = "rRsSfFdDxXpP";

/* Also be aware that MAXIMUM_NUMBER_OF_CHARS_FOR_FLOAT may have to be
   changed in read.c .  Ideally it shouldn't have to know about it at all,
   but nothing is ideal around here.
 */

static char *insn_error;

static int auto_align = 1;

/* When outputting SVR4 PIC code, the assembler needs to know the
   offset in the stack frame from which to restore the $gp register.
   This is set by the .cprestore pseudo-op, and saved in this
   variable.  */
static offsetT ss_cprestore_offset = -1;

/* This is the register which holds the stack frame, as set by the
   .frame pseudo-op.  This is needed to implement .cprestore.  */
static int ss_frame_reg = SP;

/* To output NOP instructions correctly, we need to keep information
   about the previous two instructions.  */

/* Whether we are optimizing.  The default value of 2 means to remove
   unneeded NOPs and swap branch instructions when possible.  A value
   of 1 means to not swap branches.  A value of 0 means to always
   insert NOPs.  */
static int ss_optimize = 2;

/* Debugging level.  -g sets this to 2.  -gN sets this to N.  -g0 is
   equivalent to seeing no -g option at all.  */
static int ss_debug = 0;

/* The previous instruction.  */
static struct ss_cl_insn prev_insn;

/* The instruction before prev_insn.  */
static struct ss_cl_insn prev_prev_insn;

/* If we don't want information for prev_insn or prev_prev_insn, we
   point the insn_mo field at this dummy integer.  */
static const struct ss_opcode dummy_opcode = { 0 };

/* Non-zero if prev_insn is valid.  */
static int prev_insn_valid;

/* The frag for the previous instruction.  */
static struct frag *prev_insn_frag;

/* The offset into prev_insn_frag for the previous instruction.  */
static long prev_insn_where;

/* The reloc type for the previous instruction, if any.  */
static bfd_reloc_code_real_type prev_insn_reloc_type;

/* The reloc for the previous instruction, if any.  */
static fixS *prev_insn_fixp;

/* Non-zero if the previous instruction was in a delay slot.  */
static int prev_insn_is_delay_slot;

/* Non-zero if the previous instruction was in a .set noreorder.  */
static int prev_insn_unreordered;

/* Non-zero if the previous instruction uses an extend opcode (if
   mips16).  */
static int prev_insn_extended;

/* Non-zero if the previous previous instruction was in a .set
   noreorder.  */
static int prev_prev_insn_unreordered;

/* If this is set, it points to a frag holding nop instructions which
   were inserted before the start of a noreorder section.  If those
   nops turn out to be unnecessary, the size of the frag can be
   decreased.  */
static fragS *prev_nop_frag;

/* The number of nop instructions we created in prev_nop_frag.  */
static int prev_nop_frag_holds;

/* The number of nop instructions that we know we need in
   prev_nop_frag. */
static int prev_nop_frag_required;

/* The number of instructions we've seen since prev_nop_frag.  */
static int prev_nop_frag_since;

/* For ECOFF and ELF, relocations against symbols are done in two
   parts, with a HI relocation and a LO relocation.  Each relocation
   has only 16 bits of space to store an addend.  This means that in
   order for the linker to handle carries correctly, it must be able
   to locate both the HI and the LO relocation.  This means that the
   relocations must appear in order in the relocation table.

   In order to implement this, we keep track of each unmatched HI
   relocation.  We then sort them so that they immediately precede the
   corresponding LO relocation. */

struct ss_hi_fixup
{
  /* Next HI fixup.  */
  struct ss_hi_fixup *next;
  /* This fixup.  */
  fixS *fixp;
  /* The section this fixup is in.  */
  segT seg;
};

/* The list of unmatched HI relocs.  */

static struct ss_hi_fixup *ss_hi_fixup_list;


/* Since the MIPS does not have multiple forms of PC relative
   instructions, we do not have to do relaxing as is done on other
   platforms.  However, we do have to handle GP relative addressing
   correctly, which turns out to be a similar problem.

   Every macro that refers to a symbol can occur in (at least) two
   forms, one with GP relative addressing and one without.  For
   example, loading a global variable into a register generally uses
   a macro instruction like this:
     lw $4,i
   If i can be addressed off the GP register (this is true if it is in
   the .sbss or .sdata section, or if it is known to be smaller than
   the -G argument) this will generate the following instruction:
     lw $4,i($gp)
   This instruction will use a GPREL reloc.  If i can not be addressed
   off the GP register, the following instruction sequence will be used:
     lui $at,i
     lw $4,i($at)
   In this case the first instruction will have a HI16 reloc, and the
   second reloc will have a LO16 reloc.  Both relocs will be against
   the symbol i.

   The issue here is that we may not know whether i is GP addressable
   until after we see the instruction that uses it.  Therefore, we
   want to be able to choose the final instruction sequence only at
   the end of the assembly.  This is similar to the way other
   platforms choose the size of a PC relative instruction only at the
   end of assembly.

   When generating position independent code we do not use GP
   addressing in quite the same way, but the issue still arises as
   external symbols and local symbols must be handled differently.

   We handle these issues by actually generating both possible
   instruction sequences.  The longer one is put in a frag_var with
   type rs_machine_dependent.  We encode what to do with the frag in
   the subtype field.  We encode (1) the number of existing bytes to
   replace, (2) the number of new bytes to use, (3) the offset from
   the start of the existing bytes to the first reloc we must generate
   (that is, the offset is applied from the start of the existing
   bytes after they are replaced by the new bytes, if any), (4) the
   offset from the start of the existing bytes to the second reloc,
   (5) whether a third reloc is needed (the third reloc is always four
   bytes after the second reloc), and (6) whether to warn if this
   variant is used (this is sometimes needed if .set nomacro or .set
   noat is in effect).  All these numbers are reasonably small.

   Generating two instruction sequences must be handled carefully to
   ensure that delay slots are handled correctly.  Fortunately, there
   are a limited number of cases.  When the second instruction
   sequence is generated, append_insn is directed to maintain the
   existing delay slot information, so it continues to apply to any
   code after the second instruction sequence.  This means that the
   second instruction sequence must not impose any requirements not
   required by the first instruction sequence.

   These variant frags are then handled in functions called by the
   machine independent code.  md_estimate_size_before_relax returns
   the final size of the frag.  md_convert_frag sets up the final form
   of the frag.  tc_gen_reloc adjust the first reloc and adds a second
   one if needed.  */
#define RELAX_ENCODE(old, new, reloc1, reloc2, reloc3, warn) \
  ((relax_substateT) \
   (((old) << 23) \
    | ((new) << 16) \
    | (((reloc1) + 64) << 9) \
    | (((reloc2) + 64) << 2) \
    | ((reloc3) ? (1 << 1) : 0) \
    | ((warn) ? 1 : 0)))
#define RELAX_OLD(i) (((i) >> 23) & 0x7f)
#define RELAX_NEW(i) (((i) >> 16) & 0x7f)
#define RELAX_RELOC1(i) ((bfd_vma)(((i) >> 9) & 0x7f) - 64)
#define RELAX_RELOC2(i) ((bfd_vma)(((i) >> 2) & 0x7f) - 64)
#define RELAX_RELOC3(i) (((i) >> 1) & 1)
#define RELAX_WARN(i) ((i) & 1)


/* Prototypes for static functions.  */

#ifdef __STDC__
#define internalError() \
    as_fatal (_("internal Error, line %d, %s"), __LINE__, __FILE__)
#else
#define internalError() as_fatal (_("SS internal Error"));
#endif

enum ss_regclass { SS_GR_REG, SS_FP_REG };

static int insn_uses_reg PARAMS ((struct ss_cl_insn *ip,
				  unsigned int reg, enum ss_regclass class));
static int reg_needs_delay PARAMS ((int));
static void append_insn PARAMS ((char *place,
				 struct ss_cl_insn * ip,
				 expressionS * p,
				 bfd_reloc_code_real_type r,
				 boolean));
static void ss_no_prev_insn PARAMS ((int));
static void ss_emit_delays PARAMS ((boolean));
#ifdef USE_STDARG
static void macro_build PARAMS ((char *place, int *counter, expressionS * ep,
				 const char *name, const char *fmt,
				 ...));
#else
static void macro_build ();
#endif
static void macro_build_lui PARAMS ((char *place, int *counter,
				     expressionS * ep, int regnum));
static void set_at PARAMS ((int *counter, int reg, int unsignedp));
static void check_absolute_expr PARAMS ((struct ss_cl_insn * ip,
					 expressionS *));
static void load_register PARAMS ((int *, int, expressionS *, int));
static void macro PARAMS ((struct ss_cl_insn * ip));
#ifdef LOSING_COMPILER
static void macro2 PARAMS ((struct ss_cl_insn * ip));
#endif
static void ss_ip PARAMS ((char *str, struct ss_cl_insn * ip));
static int my_getSmallExpression PARAMS ((expressionS * ep, char *str));
static void my_getExpression PARAMS ((expressionS * ep, char *str));
static symbolS *get_symbol PARAMS ((void));
static void ss_align PARAMS ((int to, int fill, symbolS *label));
static void s_align PARAMS ((int));
static void s_change_sec PARAMS ((int));
static void s_cons PARAMS ((int));
static void s_float_cons PARAMS ((int));
static void s_ss_globl PARAMS ((int));
static void s_option PARAMS ((int));
static void s_ssset PARAMS ((int));
static void s_abicalls PARAMS ((int));
static void s_cpload PARAMS ((int));
static void s_cprestore PARAMS ((int));
static void s_gpword PARAMS ((int));
static void s_cpadd PARAMS ((int));
static void s_insn PARAMS ((int));
static void md_obj_begin PARAMS ((void));
static void md_obj_end PARAMS ((void));
static long get_number PARAMS ((void));
static void s_ss_ent PARAMS ((int));
static void s_ss_end PARAMS ((int));
static void s_ss_frame PARAMS ((int));
static void s_ss_mask PARAMS ((int));
static void s_ss_stab PARAMS ((int));
static void s_ss_weakext PARAMS ((int));
static void s_file PARAMS ((int));


static int validate_ss_insn PARAMS ((const struct ss_opcode *));

/* Pseudo-op table.

   The following pseudo-ops from the Kane and Heinrich MIPS book
   should be defined here, but are currently unsupported: .alias,
   .galive, .gjaldef, .gjrlive, .livereg, .noalias.

   The following pseudo-ops from the Kane and Heinrich MIPS book are
   specific to the type of debugging information being generated, and
   should be defined by the object format: .aent, .begin, .bend,
   .bgnb, .end, .endb, .ent, .fmask, .frame, .loc, .mask, .verstamp,
   .vreg.

   The following pseudo-ops from the Kane and Heinrich MIPS book are
   not MIPS CPU specific, but are also not specific to the object file
   format.  This file is probably the best place to define them, but
   they are not currently supported: .asm0, .endr, .lab, .repeat,
   .struct.  */

static const pseudo_typeS ss_pseudo_table[] =
{
 /* SS specific pseudo-ops.  */
  {"option", s_option, 0},
  {"set", s_ssset, 0},
  {"rdata", s_change_sec, 'r'},
  {"sdata", s_change_sec, 's'},
  {"livereg", s_ignore, 0},
  {"abicalls", s_abicalls, 0},
  {"cpload", s_cpload, 0},
  {"cprestore", s_cprestore, 0},
  {"gpword", s_gpword, 0},
  {"cpadd", s_cpadd, 0},
  {"insn", s_insn, 0},

 /* Relatively generic pseudo-ops that happen to be used on SS
     chips.  */
  {"asciiz", stringer, 1},
  {"bss", s_change_sec, 'b'},
  {"err", s_err, 0},
  {"half", s_cons, 1},
  {"dword", s_cons, 3},
  {"weakext", s_ss_weakext, 0},

 /* These pseudo-ops are defined in read.c, but must be overridden
     here for one reason or another.  */
  {"align", s_align, 0},
  {"byte", s_cons, 0},
  {"data", s_change_sec, 'd'},
  {"double", s_float_cons, 'd'},
  {"float", s_float_cons, 'f'},
  {"globl", s_ss_globl, 0},
  {"global", s_ss_globl, 0},
  {"hword", s_cons, 1},
  {"int", s_cons, 2},
  {"long", s_cons, 2},
  {"octa", s_cons, 4},
  {"quad", s_cons, 3},
  {"short", s_cons, 1},
  {"single", s_float_cons, 'f'},
  {"stabn", s_ss_stab, 'n'},
  {"text", s_change_sec, 't'},
  {"word", s_cons, 2},
  { 0 },
};

static const pseudo_typeS ss_nonecoff_pseudo_table[] = {
 /* These pseudo-ops should be defined by the object file format.
    However, a.out doesn't support them, so we have versions here.  */
  {"aent", s_ss_ent, 1},
  {"bgnb", s_ignore, 0},
  {"end", s_ss_end, 0},
  {"endb", s_ignore, 0},
  {"ent", s_ss_ent, 0},
  {"file", s_file, 0},
  {"fmask", s_ss_mask, 'F'},
  {"frame", s_ss_frame, 0},
  {"loc", s_ignore, 0},
  {"mask", s_ss_mask, 'R'},
  {"verstamp", s_ignore, 0},
  { 0 },
};

extern void pop_insert PARAMS ((const pseudo_typeS *));

void
ss_pop_insert ()
{
  pop_insert (ss_pseudo_table);
  if (! ECOFF_DEBUGGING)
    pop_insert (ss_nonecoff_pseudo_table);
}

/* Symbols labelling the current insn.  */

struct insn_label_list
{
  struct insn_label_list *next;
  symbolS *label;
};

static struct insn_label_list *insn_labels;
static struct insn_label_list *free_insn_labels;

static void ss_clear_insn_labels PARAMS ((void));

static inline void
ss_clear_insn_labels ()
{
  register struct insn_label_list **pl;

  for (pl = &free_insn_labels; *pl != NULL; pl = &(*pl)->next)
    ;
  *pl = insn_labels;
  insn_labels = NULL;
}

static char *expr_end;

/* Expressions which appear in instructions.  These are set by
   ss_ip.  */

static expressionS imm_expr;
static expressionS offset_expr;

/* Relocs associated with imm_expr and offset_expr.  */

static bfd_reloc_code_real_type imm_reloc;
static bfd_reloc_code_real_type offset_reloc;

/* This is set by ss_ip if imm_reloc is an unmatched HI16_S reloc.  */

static boolean imm_unmatched_hi;

#ifdef SS_STABS_ELF
/* The pdr segment for per procedure frame/regmask info */

static segT pdr_seg;
#endif

/*
 * This function is called once, at assembler startup time.  It should
 * set up all the tables, etc. that the MD part of the assembler will need.
 */
void
md_begin ()
{
  register const char *retval = NULL;
  register int i = 0;
  int broken = 0;

  if (!bfd_set_arch_mach (stdoutput, bfd_arch_ss, 0))
    as_warn ("Could not set architecture and machine");

  op_hash = hash_new ();

  for (i = 0; i < NUMOPCODES;)
    {
      const char *name = ss_opcodes[i].name;

      retval = hash_insert (op_hash, name, (PTR) &ss_opcodes[i]);
      if (retval != NULL)
	{
	  fprintf (stderr, _("internal error: can't hash `%s': %s\n"),
		   ss_opcodes[i].name, retval);
	  /* Probably a memory allocation problem?  Give up now.  */
	  as_fatal (_("Broken assembler.  No assembly attempted."));
	}
      do
	{
	  if (ss_opcodes[i].pinfo != INSN_MACRO)
	    {
	      if (!validate_ss_insn (&ss_opcodes[i]))
		broken = 1;
	    }
	  ++i;
	}
      while ((i < NUMOPCODES) && !strcmp (ss_opcodes[i].name, name));
    }

  if (broken)
    as_fatal (_("Broken assembler.  No assembly attempted."));

  /* We add all the general register names to the symbol table.  This
     helps us detect invalid uses of them.  */
  for (i = 0; i < 32; i++)
    {
      char buf[5];

      sprintf (buf, "$%d", i);
      symbol_table_insert (symbol_new (buf, reg_section, i,
				       &zero_address_frag));
    }
  symbol_table_insert (symbol_new ("$fp", reg_section, FP,
				   &zero_address_frag));
  symbol_table_insert (symbol_new ("$sp", reg_section, SP,
				   &zero_address_frag));
  symbol_table_insert (symbol_new ("$gp", reg_section, GP,
				   &zero_address_frag));
  symbol_table_insert (symbol_new ("$at", reg_section, AT,
				   &zero_address_frag));
  symbol_table_insert (symbol_new ("$kt0", reg_section, KT0,
				   &zero_address_frag));
  symbol_table_insert (symbol_new ("$kt1", reg_section, KT1,
				   &zero_address_frag));
  symbol_table_insert (symbol_new ("$pc", reg_section, -1,
				   &zero_address_frag));

  ss_gprmask = 0;
  ss_cprmask[0] = 0;
  ss_cprmask[1] = 0;
  ss_cprmask[2] = 0;
  ss_cprmask[3] = 0;

  /* set the default alignment for the text section (2**2) */
  record_alignment (text_section, 2);

  if (USE_GLOBAL_POINTER_OPT)
    bfd_set_gp_size (stdoutput, g_switch_value);

  if (OUTPUT_FLAVOR == bfd_target_elf_flavour)
    {
      /* On a native system, sections must be aligned to 16 byte
	 boundaries.  When configured for an embedded ELF target, we
	 don't bother.  */
      if (strcmp (TARGET_OS, "elf") != 0)
	{
	  (void) bfd_set_section_alignment (stdoutput, text_section, 4);
	  (void) bfd_set_section_alignment (stdoutput, data_section, 4);
	  (void) bfd_set_section_alignment (stdoutput, bss_section, 4);
	}

      /* Create a .reginfo section for register masks and a .mdebug
	 section for debugging information.  */
      {
	segT seg;
	subsegT subseg;
	flagword flags;
	segT sec;

	seg = now_seg;
	subseg = now_subseg;

	/* The ABI says this section should be loaded so that the
	   running program can access it.  However, we don't load it
	   if we are configured for an embedded target */
	flags = SEC_READONLY | SEC_DATA;
	if (strcmp (TARGET_OS, "elf") != 0)
	  flags |= SEC_ALLOC | SEC_LOAD;

	if (! ss_64)
	  {
	    sec = subseg_new (".reginfo", (subsegT) 0);


	    (void) bfd_set_section_flags (stdoutput, sec, flags);
	    (void) bfd_set_section_alignment (stdoutput, sec, 2);
	
#ifdef OBJ_ELF
	    ss_regmask_frag = frag_more (sizeof (Elf32_External_RegInfo));
#endif
	  }
	else
	  {
	    /* The 64-bit ABI uses a .SS.options section rather than
               .reginfo section.  */
	    sec = subseg_new (".SS.options", (subsegT) 0);
	    (void) bfd_set_section_flags (stdoutput, sec, flags);
	    (void) bfd_set_section_alignment (stdoutput, sec, 3);

#ifdef OBJ_ELF
	    /* Set up the option header.  */
	    {
	      Elf_Internal_Options opthdr;
	      char *f;

	      opthdr.kind = ODK_REGINFO;
	      opthdr.size = (sizeof (Elf_External_Options)
			     + sizeof (Elf64_External_RegInfo));
	      opthdr.section = 0;
	      opthdr.info = 0;
	      f = frag_more (sizeof (Elf_External_Options));
	      bfd_ss_elf_swap_options_out (stdoutput, &opthdr,
					     (Elf_External_Options *) f);

	      ss_regmask_frag = frag_more (sizeof (Elf64_External_RegInfo));
	    }
#endif
	  }

	if (ECOFF_DEBUGGING)
	  {
	    sec = subseg_new (".mdebug", (subsegT) 0);
	    (void) bfd_set_section_flags (stdoutput, sec,
					  SEC_HAS_CONTENTS | SEC_READONLY);
	    (void) bfd_set_section_alignment (stdoutput, sec, 2);
	  }

#ifdef SS_STABS_ELF
	pdr_seg = subseg_new (".pdr", (subsegT) 0);
	(void) bfd_set_section_flags (stdoutput, pdr_seg,
			     SEC_READONLY | SEC_RELOC | SEC_DEBUGGING);
	(void) bfd_set_section_alignment (stdoutput, pdr_seg, 2);
#endif

	subseg_set (seg, subseg);
      }
    }

  if (! ECOFF_DEBUGGING)
    md_obj_begin ();
}

void
md_ss_end ()
{
  if (! ECOFF_DEBUGGING)
    md_obj_end ();
}

void
md_assemble (str)
     char *str;
{
  struct ss_cl_insn insn;

  imm_expr.X_op = O_absent;
  imm_reloc = BFD_RELOC_UNUSED;
  imm_unmatched_hi = false;
  offset_expr.X_op = O_absent;
  offset_reloc = BFD_RELOC_UNUSED;

  ss_ip (str, &insn);
  DBG((_("returned from ss_ip(%s) insn_opcode = 0x%x\n"), 
       str, insn.insn_opcode));
  
  if (insn_error)
    {
      as_bad ("%s `%s'", insn_error, str);
      return;
    }

  if (insn.insn_mo->pinfo == INSN_MACRO)
    {
      macro (&insn);
    }
  else
    {
      if (imm_expr.X_op != O_absent)
	append_insn ((char *) NULL, &insn, &imm_expr, imm_reloc,
		     imm_unmatched_hi);
      else if (offset_expr.X_op != O_absent)
	append_insn ((char *) NULL, &insn, &offset_expr, offset_reloc, false);
      else
	append_insn ((char *) NULL, &insn, NULL, BFD_RELOC_UNUSED, false);
    }
}

/* Output an instruction.  PLACE is where to put the instruction; if
   it is NULL, this uses frag_more to get room.  IP is the instruction
   information.  ADDRESS_EXPR is an operand of the instruction to be
   used with RELOC_TYPE.  */

static void
append_insn (place, ip, address_expr, reloc_type, unmatched_hi)
     char *place;
     struct ss_cl_insn *ip;
     expressionS *address_expr;
     bfd_reloc_code_real_type reloc_type;
     boolean unmatched_hi;
{
  char *f;
  fixS *fixp;


  if (place == NULL)   
    f = frag_more (8); 
  else                 
    f = place;         
  fixp = NULL;
  if (address_expr != NULL && reloc_type < BFD_RELOC_UNUSED)
    {
      if (address_expr->X_op == O_constant)
	{
	  switch (reloc_type)
	    {
	    case BFD_RELOC_32:
	      ip->insn_opcode.b |= address_expr->X_add_number;
	      break;

	    case BFD_RELOC_LO16:
	      ip->insn_opcode.b |= address_expr->X_add_number & 0xffff;
	      break;

	    case BFD_RELOC_MIPS_JMP:
	      if ((address_expr->X_add_number & 3) != 0)
		as_bad (_("jump to misaligned address (0x%lx)"),
			(unsigned long) address_expr->X_add_number);
	      ip->insn_opcode.b |= (address_expr->X_add_number >> 2) & 0x3ffffff;
	      break;

	    case BFD_RELOC_16_PCREL_S2:
	      goto need_reloc;

	    default:
	      internalError ();
	    }
	}
      else
	{
	need_reloc:
	  /* Don't generate a reloc if we are writing into a variant
	     frag.  */
	  if (place == NULL)
	    {
	      fixp = fix_new_exp (frag_now, f - frag_now->fr_literal+4, 4,
				  address_expr,
				  reloc_type == BFD_RELOC_16_PCREL_S2,
				  reloc_type);
	      if (unmatched_hi)
		{
		  struct ss_hi_fixup *hi_fixup;

		  assert (reloc_type == BFD_RELOC_HI16_S);
		  hi_fixup = ((struct ss_hi_fixup *)
			      xmalloc (sizeof (struct ss_hi_fixup)));
		  hi_fixup->fixp = fixp;
		  hi_fixup->seg = now_seg;
		  hi_fixup->next = ss_hi_fixup_list;
		  ss_hi_fixup_list = hi_fixup;
		}
	    }
	}
    }

  md_number_to_chars (f, ip->insn_opcode.a, 4);
  md_number_to_chars (f+4, ip->insn_opcode.b, 4);

  /* We just output an insn, so the next one doesn't have a label.  */
  ss_clear_insn_labels ();

  /* We must ensure that a fixup associated with an unmatched %hi
     reloc does not become a variant frag.  Otherwise, the
     rearrangement of %hi relocs in frob_file may confuse
     tc_gen_reloc.  */
  if (unmatched_hi)
    {
      frag_wane (frag_now);
      frag_new (0);
    }
}

static int insn_flags;

/* Build an instruction created by a macro expansion.  This is passed
   a pointer to the count of instructions created so far, an
   expression, the name of the instruction to build, an operand format
   string, and corresponding arguments.  */

#ifdef USE_STDARG
static void
macro_build (char *place,
	     int *counter,
	     expressionS * ep,
	     const char *name,
	     const char *fmt,
	     ...)
#else
static void
macro_build (place, counter, ep, name, fmt, va_alist)
     char *place;
     int *counter;
     expressionS *ep;
     const char *name;
     const char *fmt;
     va_dcl
#endif
{
  struct ss_cl_insn insn;
  bfd_reloc_code_real_type r;
  va_list args;

#ifdef USE_STDARG
  va_start (args, fmt);
#else
  va_start (args);
#endif

  /*
   * If the macro is about to expand into a second instruction,
   * print a warning if needed. We need to pass ip as a parameter
   * to generate a better warning message here...
   */
  if (ss_opts.warn_about_macros && place == NULL && *counter == 1)
    as_warn (_("Macro instruction expanded into multiple instructions"));

  if (place == NULL)
    *counter += 1;		/* bump instruction counter */

  r = BFD_RELOC_UNUSED;
  insn.insn_mo = (struct ss_opcode *) hash_find (op_hash, name);
  assert (insn.insn_mo);
  assert (strcmp (name, insn.insn_mo->name) == 0);

  /* Search until we get a match for NAME.  */
  while (1)
    {
      if (strcmp (fmt, insn.insn_mo->args) == 0
	  && insn.insn_mo->pinfo != INSN_MACRO)
	break;

      ++insn.insn_mo;
      assert (insn.insn_mo->name);
      assert (strcmp (name, insn.insn_mo->name) == 0);
    }

  insn.insn_opcode.a = (insn_flags << 16) | insn.insn_mo->match;
  insn.insn_opcode.b = 0;
  for (;;)
    {
      switch (*fmt++)
	{
	case '\0':
	  break;

	case ',':
	case '(':
	case ')':
	  continue;

	case 't':
	case 'w':
	  /*case 'E': SS deletes*/
	  insn.insn_opcode.b |= va_arg (args, int) << 16;
	  continue;

	case 'c':
	  insn.insn_opcode.b |= va_arg (args, int);
	  continue;                                      

	case 'T':
	case 'W':
	  insn.insn_opcode.b |= va_arg (args, int) << 16;
	  continue;

	case 'd':
	  /*case 'G': ss deletes*/
	  insn.insn_opcode.b |= va_arg (args, int) << 8;
	  continue;

	case 'V':
	case 'S':
	  insn.insn_opcode.b |= va_arg (args, int) << 24;
	  continue;

	case 'z':
	  continue;

	case '<':
	  insn.insn_opcode.b |= va_arg (args, int);
	  continue;

	case 'D':
	  insn.insn_opcode.b |= va_arg (args, int) << 8;
	  continue;

	case 'b':
	case 's':
	case 'r':
	case 'v':
	  insn.insn_opcode.b |= va_arg (args, int) << 24;
	  continue;

	case 'i':
	case 'j':
	case 'o':
	  r = (bfd_reloc_code_real_type) va_arg (args, int);
	  assert (r == BFD_RELOC_MIPS_GPREL
		  || r == BFD_RELOC_MIPS_LITERAL
		  || r == BFD_RELOC_LO16
		  || r == BFD_RELOC_MIPS_GOT16
		  || r == BFD_RELOC_MIPS_CALL16
		  || r == BFD_RELOC_MIPS_GOT_LO16
		  || r == BFD_RELOC_MIPS_CALL_LO16
		  || (ep->X_op == O_subtract
		      && now_seg == text_section
		      && r == BFD_RELOC_PCREL_LO16));
	  continue;

	case 'u':
	  r = (bfd_reloc_code_real_type) va_arg (args, int);
	  assert (ep != NULL
		  && (ep->X_op == O_constant
		      || (ep->X_op == O_symbol
			  && (r == BFD_RELOC_HI16_S
			      || r == BFD_RELOC_HI16
			      || r == BFD_RELOC_MIPS_GOT_HI16
			      || r == BFD_RELOC_MIPS_CALL_HI16))
		      || (ep->X_op == O_subtract
			  && now_seg == text_section
			  && r == BFD_RELOC_PCREL_HI16_S)));
	  if (ep->X_op == O_constant)
	    {
	      insn.insn_opcode.b |= (ep->X_add_number >> 16) & 0xffff;
	      ep = NULL;
	      r = BFD_RELOC_UNUSED;
	    }
	  continue;

	case 'p':
	  assert (ep != NULL);
	  /*
	   * This allows macro() to pass an immediate expression for
	   * creating short branches without creating a symbol.
	   * Note that the expression still might come from the assembly
	   * input, in which case the value is not checked for range nor
	   * is a relocation entry generated (yuck).
	   */
	  if (ep->X_op == O_constant)
	    {
	      insn.insn_opcode.b |= (ep->X_add_number >> 2) & 0xffff;
	      ep = NULL;
	    }
	  else
	    r = BFD_RELOC_16_PCREL_S2;
	  continue;

	case 'a':
	  assert (ep != NULL);
	  r = BFD_RELOC_MIPS_JMP;
	  continue;

	default:
	  internalError ();
	}
      break;
    }
  va_end (args);
  assert (r == BFD_RELOC_UNUSED ? ep == NULL : ep != NULL);

  append_insn (place, &insn, ep, r, false);
}

/*
 * Generate a "lui" instruction.
 */
static void
macro_build_lui (place, counter, ep, regnum)
     char *place;
     int *counter;
     expressionS *ep;
     int regnum;
{
  expressionS high_expr;
  struct ss_cl_insn insn;
  bfd_reloc_code_real_type r;
  CONST char *name = "lui";
  CONST char *fmt = "t,u";

  if (place == NULL)
    high_expr = *ep;
  else
    {
      high_expr.X_op = O_constant;
      high_expr.X_add_number = ep->X_add_number;
    }

  if (high_expr.X_op == O_constant)
    {
      /* we can compute the instruction now without a relocation entry */
      if (high_expr.X_add_number & 0x8000)
	high_expr.X_add_number += 0x10000;
      high_expr.X_add_number =
	((unsigned long) high_expr.X_add_number >> 16) & 0xffff;
      r = BFD_RELOC_UNUSED;
    }
  else
    {
      assert (ep->X_op == O_symbol);
      /* _gp_disp is a special case, used from s_cpload.  */
      assert (ss_pic == NO_PIC
	      || strcmp (S_GET_NAME (ep->X_add_symbol), "_gp_disp") == 0);
      r = BFD_RELOC_HI16_S;
    }

  /*
   * If the macro is about to expand into a second instruction,
   * print a warning if needed. We need to pass ip as a parameter
   * to generate a better warning message here...
   */
  if (ss_opts.warn_about_macros && place == NULL && *counter == 1)
    as_warn (_("Macro instruction expanded into multiple instructions"));

  if (place == NULL)
    *counter += 1;		/* bump instruction counter */

  insn.insn_mo = (struct ss_opcode *) hash_find (op_hash, name);
  assert (insn.insn_mo);
  assert (strcmp (name, insn.insn_mo->name) == 0);
  assert (strcmp (fmt, insn.insn_mo->args) == 0);

  insn.insn_opcode.a = (insn_flags << 16) | insn.insn_mo->match;
  insn.insn_opcode.b = (regnum << 16);
  if (r == BFD_RELOC_UNUSED)
    {
      insn.insn_opcode.b |= high_expr.X_add_number;
      append_insn (place, &insn, NULL, r, false);
    }
  else
    append_insn (place, &insn, &high_expr, r, false);
}

/*			set_at()
 * Generates code to set the $at register to true (one)
 * if reg is less than the immediate expression.
 */
static void
set_at (counter, reg, unsignedp)
     int *counter;
     int reg;
     int unsignedp;
{
  if (imm_expr.X_op == O_constant
      && imm_expr.X_add_number >= -0x8000
      && imm_expr.X_add_number < 0x8000)
    macro_build ((char *) NULL, counter, &imm_expr,
		 unsignedp ? "sltiu" : "slti",
		 "t,r,j", AT, reg, (int) BFD_RELOC_LO16);
  else
    {
      load_register (counter, AT, &imm_expr, 0);
      macro_build ((char *) NULL, counter, NULL,
		   unsignedp ? "sltu" : "slt",
		   "d,v,t", AT, reg, AT);
    }
}

/* Warn if an expression is not a constant.  */
static void
check_absolute_expr (ip, ex)
     struct ss_cl_insn *ip;
     expressionS *ex;
{
  if (ex->X_op == O_big)
    as_bad (_("unsupported large constant"));
  else if (ex->X_op != O_constant)
    as_bad (_("Instruction %s requires absolute expression"), ip->insn_mo->name);
}

/* Count the leading zeroes by performing a binary chop. This is a
   bulky bit of source, but performance is a LOT better for the
   majority of values than a simple loop to count the bits:
       for (lcnt = 0; (lcnt < 32); lcnt++)
         if ((v) & (1 << (31 - lcnt)))
           break;
  However it is not code size friendly, and the gain will drop a bit
  on certain cached systems.
*/
#define COUNT_TOP_ZEROES(v)             \
  (((v) & ~0xffff) == 0                 \
   ? ((v) & ~0xff) == 0                 \
     ? ((v) & ~0xf) == 0                \
       ? ((v) & ~0x3) == 0              \
         ? ((v) & ~0x1) == 0            \
           ? !(v)                       \
             ? 32                       \
             : 31                       \
           : 30                         \
         : ((v) & ~0x7) == 0            \
           ? 29                         \
           : 28                         \
       : ((v) & ~0x3f) == 0             \
         ? ((v) & ~0x1f) == 0           \
           ? 27                         \
           : 26                         \
         : ((v) & ~0x7f) == 0           \
           ? 25                         \
           : 24                         \
     : ((v) & ~0xfff) == 0              \
       ? ((v) & ~0x3ff) == 0            \
         ? ((v) & ~0x1ff) == 0          \
           ? 23                         \
           : 22                         \
         : ((v) & ~0x7ff) == 0          \
           ? 21                         \
           : 20                         \
       : ((v) & ~0x3fff) == 0           \
         ? ((v) & ~0x1fff) == 0         \
           ? 19                         \
           : 18                         \
         : ((v) & ~0x7fff) == 0         \
           ? 17                         \
           : 16                         \
   : ((v) & ~0xffffff) == 0             \
     ? ((v) & ~0xfffff) == 0            \
       ? ((v) & ~0x3ffff) == 0          \
         ? ((v) & ~0x1ffff) == 0        \
           ? 15                         \
           : 14                         \
         : ((v) & ~0x7ffff) == 0        \
           ? 13                         \
           : 12                         \
       : ((v) & ~0x3fffff) == 0         \
         ? ((v) & ~0x1fffff) == 0       \
           ? 11                         \
           : 10                         \
         : ((v) & ~0x7fffff) == 0       \
           ? 9                          \
           : 8                          \
     : ((v) & ~0xfffffff) == 0          \
       ? ((v) & ~0x3ffffff) == 0        \
         ? ((v) & ~0x1ffffff) == 0      \
           ? 7                          \
           : 6                          \
         : ((v) & ~0x7ffffff) == 0      \
           ? 5                          \
           : 4                          \
       : ((v) & ~0x3fffffff) == 0       \
         ? ((v) & ~0x1fffffff) == 0     \
           ? 3                          \
           : 2                          \
         : ((v) & ~0x7fffffff) == 0     \
           ? 1                          \
           : 0)

/*			load_register()
 *  This routine generates the least number of instructions neccessary to load
 *  an absolute expression value into a register.
 */
static void
load_register (counter, reg, ep, dbl)
     int *counter;
     int reg;
     expressionS *ep;
     int dbl;
{
  int freg;
  expressionS hi32, lo32;

  if (ep->X_op != O_big)
    {
      assert (ep->X_op == O_constant);
      if (ep->X_add_number < 0x8000
	  && (ep->X_add_number >= 0
	      || (ep->X_add_number >= -0x8000
		  && (! dbl
		      || ! ep->X_unsigned
		      || sizeof (ep->X_add_number) > 4))))
	{
	  /* We can handle 16 bit signed values with an addiu to
	     $zero.  No need to ever use daddiu here, since $zero and
	     the result are always correct in 32 bit mode.  */
	  macro_build ((char *) NULL, counter, ep, "addiu", "t,r,j", reg, 0,
		       (int) BFD_RELOC_LO16);
	  return;
	}
      else if (ep->X_add_number >= 0 && ep->X_add_number < 0x10000)
	{
	  /* We can handle 16 bit unsigned values with an ori to
             $zero.  */
	  macro_build ((char *) NULL, counter, ep, "ori", "t,r,i", reg, 0,
		       (int) BFD_RELOC_LO16);
	  return;
	}
      else if ((((ep->X_add_number &~ (offsetT) 0x7fffffff) == 0
		 || ((ep->X_add_number &~ (offsetT) 0x7fffffff)
		     == ~ (offsetT) 0x7fffffff))
		&& (! dbl
		    || ! ep->X_unsigned
		    || sizeof (ep->X_add_number) > 4
		    || (ep->X_add_number & 0x80000000) == 0))
	       || ((ss_opts.isa < 3 || ! dbl)
		   && (ep->X_add_number &~ (offsetT) 0xffffffff) == 0)
	       || (ss_opts.isa < 3
		   && ! dbl
		   && ((ep->X_add_number &~ (offsetT) 0xffffffff)
		       == ~ (offsetT) 0xffffffff)))
	{
	  /* 32 bit values require an lui.  */
	  macro_build ((char *) NULL, counter, ep, "lui", "t,u", reg,
		       (int) BFD_RELOC_HI16);
	  if ((ep->X_add_number & 0xffff) != 0)
	    macro_build ((char *) NULL, counter, ep, "ori", "t,r,i", reg, reg,
			 (int) BFD_RELOC_LO16);
	  return;
	}
    }

  /* The value is larger than 32 bits.  */

  /*  if (ss_opts.isa < 3)  SS remove condition...kind of*/
    {
      as_bad (_("Number larger than 32 bits"));
      macro_build ((char *) NULL, counter, ep, "addiu", "t,r,j", reg, 0,
		   (int) BFD_RELOC_LO16);
      return;
    }

  if (ep->X_op != O_big)
    {
      hi32 = *ep;
      hi32.X_add_number = (valueT) hi32.X_add_number >> 16;
      hi32.X_add_number = (valueT) hi32.X_add_number >> 16;
      hi32.X_add_number &= 0xffffffff;
      lo32 = *ep;
      lo32.X_add_number &= 0xffffffff;
    }
  else
    {
      assert (ep->X_add_number > 2);
      if (ep->X_add_number == 3)
	generic_bignum[3] = 0;
      else if (ep->X_add_number > 4)
	as_bad (_("Number larger than 64 bits"));
      lo32.X_op = O_constant;
      lo32.X_add_number = generic_bignum[0] + (generic_bignum[1] << 16);
      hi32.X_op = O_constant;
      hi32.X_add_number = generic_bignum[2] + (generic_bignum[3] << 16);
    }

  if (hi32.X_add_number == 0)
    freg = 0;
  else
    {
      int shift, bit;
      unsigned long hi, lo;

      if (hi32.X_add_number == 0xffffffff)
        {
          if ((lo32.X_add_number & 0xffff8000) == 0xffff8000)
            {
              macro_build ((char *) NULL, counter, &lo32, "addiu", "t,r,j",
			   reg, 0, (int) BFD_RELOC_LO16);
              return;
            }
          if (lo32.X_add_number & 0x80000000)
            {
              macro_build ((char *) NULL, counter, &lo32, "lui", "t,u", reg,
                           (int) BFD_RELOC_HI16);
	      if (lo32.X_add_number & 0xffff)
		macro_build ((char *) NULL, counter, &lo32, "ori", "t,r,i",
			     reg, reg, (int) BFD_RELOC_LO16);
              return;
            }
        }

      /* Check for 16bit shifted constant.  We know that hi32 is
         non-zero, so start the mask on the first bit of the hi32
         value.  */
      shift = 17;
      do
       {
	 unsigned long himask, lomask;

	 if (shift < 32)
	   {
	     himask = 0xffff >> (32 - shift);
	     lomask = (0xffff << shift) & 0xffffffff;
	   }
	 else
	   {
	     himask = 0xffff << (shift - 32);
	     lomask = 0;
	   }
	 if ((hi32.X_add_number & ~ (offsetT) himask) == 0
	     && (lo32.X_add_number & ~ (offsetT) lomask) == 0)
	   {
	     expressionS tmp;

	     tmp.X_op = O_constant;
	     if (shift < 32)
	       tmp.X_add_number = ((hi32.X_add_number << (32 - shift))
				   | (lo32.X_add_number >> shift));
	     else
	       tmp.X_add_number = hi32.X_add_number >> (shift - 32);
	     macro_build ((char *) NULL, counter, &tmp, "ori", "t,r,i", reg, 0,
			  (int) BFD_RELOC_LO16);
	     macro_build ((char *) NULL, counter, NULL,
			  (shift >= 32) ? "dsll32" : "dsll",
			  "d,w,<", reg, reg,
			  (shift >= 32) ? shift - 32 : shift);
	     return;
	   }
         shift++;
       } while (shift <= (64 - 16));

      /* Find the bit number of the lowest one bit, and store the
         shifted value in hi/lo.  */
      hi = (unsigned long) (hi32.X_add_number & 0xffffffff);
      lo = (unsigned long) (lo32.X_add_number & 0xffffffff);
      if (lo != 0)
	{
	  bit = 0;
	  while ((lo & 1) == 0)
	    {
	      lo >>= 1;
	      ++bit;
	    }
	  lo |= (hi & (((unsigned long) 1 << bit) - 1)) << (32 - bit);
	  hi >>= bit;
	}
      else
	{
	  bit = 32;
	  while ((hi & 1) == 0)
	    {
	      hi >>= 1;
	      ++bit;
	    }
	  lo = hi;
	  hi = 0;
	}

      /* Optimize if the shifted value is a (power of 2) - 1.  */
      if ((hi == 0 && ((lo + 1) & lo) == 0)
	  || (lo == 0xffffffff && ((hi + 1) & hi) == 0))
        {
          shift = COUNT_TOP_ZEROES ((unsigned int) hi32.X_add_number);
	  if (shift != 0)
            {
	      expressionS tmp;

	      /* This instruction will set the register to be all
                 ones.  */
              tmp.X_op = O_constant;
              tmp.X_add_number = (offsetT) -1;
              macro_build ((char *) NULL, counter, &tmp, "addiu", "t,r,j",
			   reg, 0, (int) BFD_RELOC_LO16);
              if (bit != 0)
                {
                  bit += shift;
                  macro_build ((char *) NULL, counter, NULL,
                               (bit >= 32) ? "dsll32" : "dsll",
                               "d,w,<", reg, reg,
                               (bit >= 32) ? bit - 32 : bit);
                }
              macro_build ((char *) NULL, counter, NULL,
			   (shift >= 32) ? "dsrl32" : "dsrl",
                           "d,w,<", reg, reg,
			   (shift >= 32) ? shift - 32 : shift);
              return;
            }
        }

      /* Sign extend hi32 before calling load_register, because we can
         generally get better code when we load a sign extended value.  */
      if ((hi32.X_add_number & 0x80000000) != 0)
	hi32.X_add_number |= ~ (offsetT) 0xffffffff;
      load_register (counter, reg, &hi32, 0);
      freg = reg;
    }
  if ((lo32.X_add_number & 0xffff0000) == 0)
    {
      if (freg != 0)
	{
	  macro_build ((char *) NULL, counter, NULL, "dsll32", "d,w,<", reg,
		       freg, 0);
	  freg = reg;
	}
    }
  else
    {
      expressionS mid16;

      if ((freg == 0) && (lo32.X_add_number == 0xffffffff))
        {
	  macro_build ((char *) NULL, counter, &lo32, "lui", "t,u", reg,
		       (int) BFD_RELOC_HI16);
          macro_build ((char *) NULL, counter, NULL, "dsrl32", "d,w,<", reg,
                       reg, 0);
          return;
        }

      if (freg != 0)
	{
	  macro_build ((char *) NULL, counter, NULL, "dsll", "d,w,<", reg,
		       freg, 16);
	  freg = reg;
	}
      mid16 = lo32;
      mid16.X_add_number >>= 16;
      macro_build ((char *) NULL, counter, &mid16, "ori", "t,r,i", reg,
		   freg, (int) BFD_RELOC_LO16);
      macro_build ((char *) NULL, counter, NULL, "dsll", "d,w,<", reg,
		   reg, 16);
      freg = reg;
    }
  if ((lo32.X_add_number & 0xffff) != 0)
    macro_build ((char *) NULL, counter, &lo32, "ori", "t,r,i", reg, freg,
		 (int) BFD_RELOC_LO16);
}

/*
 *			Build macros
 *   This routine implements the seemingly endless macro or synthesized
 * instructions and addressing modes in the mips assembly language. Many
 * of these macros are simple and are similar to each other. These could
 * probably be handled by some kind of table or grammer aproach instead of
 * this verbose method. Others are not simple macros but are more like
 * optimizing code generation.
 *   One interesting optimization is when several store macros appear
 * consecutivly that would load AT with the upper half of the same address.
 * The ensuing load upper instructions are ommited. This implies some kind
 * of global optimization. We currently only optimize within a single macro.
 *   For many of the load and store macros if the address is specified as a
 * constant expression in the first 64k of memory (ie ld $2,0x4000c) we
 * first load register 'at' with zero and use it as the base register. The
 * mips assembler simply uses register $zero. Just one tiny optimization
 * we're missing.
 */
static void
macro (ip)
     struct ss_cl_insn *ip;
{
  register int treg, sreg, dreg, breg;
  int tempreg;
  int mask;
  int icnt = 0;
  int used_at = 0;
  expressionS expr1;
  const char *s = NULL;
  const char *s2 = NULL;
  const char *fmt = NULL;
  int dbl = 0;
  int coproc = 0;
  int imm = 0;
  offsetT maxnum;
  bfd_reloc_code_real_type r;
  char *p;
  int zero = 0;

  treg = (ip->insn_opcode.b >> 16) & 0xff;
  dreg = (ip->insn_opcode.b >> 8) & 0xff;
  sreg = breg = (ip->insn_opcode.b >> 24) & 0xff;
  mask = ip->insn_mo->mask;

  expr1.X_op = O_constant;
  expr1.X_op_symbol = NULL;
  expr1.X_add_symbol = NULL;
  expr1.X_add_number = 1;

  switch (mask)
    {
    case M_MOVE_M1:
      macro_build ((char *)NULL, &icnt, NULL, "addu", "d,v,t", dreg, 0, sreg);
      return;

    case M_ADD_I:
      s = "addi";
      s2 = "add";
      goto do_addi;
    case M_ADDU_I:
      s = "addiu";
      s2 = "addu";
      goto do_addi;

    do_addi:
      if (imm_expr.X_op == O_constant
	  && imm_expr.X_add_number >= -0x8000
	  && imm_expr.X_add_number < 0x8000)
	{
	  macro_build ((char *) NULL, &icnt, &imm_expr, s, "t,r,j", treg, sreg,
		       (int) BFD_RELOC_LO16);
	  return;
	}
      load_register (&icnt, AT, &imm_expr, dbl);
      macro_build ((char *) NULL, &icnt, NULL, s2, "d,v,t", treg, sreg, AT);
      break;

    case M_AND_I:
      s = "andi";
      s2 = "and";
      goto do_bit;
    case M_OR_I:
      s = "ori";
      s2 = "or";
      goto do_bit;
    case M_NOR_I:
      s = "";
      s2 = "nor";
      goto do_bit;
    case M_XOR_I:
      s = "xori";
      s2 = "xor";
    do_bit:
      if (imm_expr.X_op == O_constant
	  && imm_expr.X_add_number >= 0
	  && imm_expr.X_add_number < 0x10000)
	{
	  if (mask != M_NOR_I)
	    macro_build ((char *) NULL, &icnt, &imm_expr, s, "t,r,i", treg,
			 sreg, (int) BFD_RELOC_LO16);
	  else
	    {
	      macro_build ((char *) NULL, &icnt, &imm_expr, "ori", "t,r,i",
			   treg, sreg, (int) BFD_RELOC_LO16);
	      macro_build ((char *) NULL, &icnt, NULL, "nor", "d,v,t",
			   treg, treg, 0);
	    }
	  return;
	}

      load_register (&icnt, AT, &imm_expr, 0);
      macro_build ((char *) NULL, &icnt, NULL, s2, "d,v,t", treg, sreg, AT);
      break;

    case M_BEQ_I:
      s = "beq";
      goto beq_i;
    case M_BNE_I:
      s = "bne";
      goto beq_i;
    beq_i:
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr, s, "s,t,p", sreg,
		       0);
	  return;
	}
      load_register (&icnt, AT, &imm_expr, 0);
      macro_build ((char *) NULL, &icnt, &offset_expr, s, "s,t,p", sreg, AT);
      break;

    case M_BGE:
      if (treg == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "bgez",
		       "s,p", sreg);
	  return;
	}
      if (sreg == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "blez",
		       "s,p", treg);
	  return;
	}
      macro_build ((char *) NULL, &icnt, NULL, "slt", "d,v,t", AT, sreg, treg);
      macro_build ((char *) NULL, &icnt, &offset_expr,
		   "beq",
		   "s,t,p", AT, 0);
      break;

    case M_BGT_I:
      /* check for > max integer */
      maxnum = 0x7fffffff;
      if (imm_expr.X_add_number >= maxnum && sizeof (maxnum) > 4)
	{
	do_false:
	  /* result is always false */
	      as_warn (_("Branch %s is always false (nop)"), ip->insn_mo->name);
	      macro_build ((char *) NULL, &icnt, NULL, "nop", "", 0);
	  return;
	}
      if (imm_expr.X_op != O_constant)
	as_bad (_("Unsupported large constant"));
      imm_expr.X_add_number++;
      /* FALLTHROUGH */
    case M_BGE_I:
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "bgez",
		       "s,p", sreg);
	  return;
	}
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number == 1)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "bgtz",
		       "s,p", sreg);
	  return;
	}
      maxnum = 0x7fffffff;
      maxnum = - maxnum - 1;
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number <= maxnum && sizeof (maxnum) > 4)
	{
	do_true:
	  /* result is always true */
	  as_warn (_("Branch %s is always true"), ip->insn_mo->name);
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "beq",
		       "s,t,p", 0, 0);
      /*	  macro_build ((char *) NULL, &icnt, &offset_expr, "b", "p");*/
	  return;
	}
      set_at (&icnt, sreg, 0);
      macro_build ((char *) NULL, &icnt, &offset_expr,
		   "beq",
		   "s,t,p", AT, 0);
      break;

    case M_BGEU:
      if (treg == 0)
	goto do_true;
      if (sreg == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "beq",
		       "s,t,p", 0, treg);
	  return;
	}
      macro_build ((char *) NULL, &icnt, NULL, "sltu", "d,v,t", AT, sreg,
		   treg);
      macro_build ((char *) NULL, &icnt, &offset_expr,
		   "beq",
		   "s,t,p", AT, 0);
      break;

    case M_BGTU_I:
      if (sreg == 0
	  || (imm_expr.X_op == O_constant
	      && imm_expr.X_add_number == 0xffffffff))
	goto do_false;
      if (imm_expr.X_op != O_constant)
	as_bad (_("Unsupported large constant"));
      imm_expr.X_add_number++;
      /* FALLTHROUGH */
    case M_BGEU_I:
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number == 0)
	goto do_true;
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number == 1)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "bne",
		       "s,t,p", sreg, 0);
	  return;
	}
      set_at (&icnt, sreg, 1);
      macro_build ((char *) NULL, &icnt, &offset_expr,
		   "beq",
		   "s,t,p", AT, 0);
      break;

    case M_BGT:
      if (treg == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "bgtz",
		       "s,p", sreg);
	  return;
	}
      if (sreg == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "bltz",
		       "s,p", treg);
	  return;
	}
      macro_build ((char *) NULL, &icnt, NULL, "slt", "d,v,t", AT, treg, sreg);
      macro_build ((char *) NULL, &icnt, &offset_expr,
		   "bne",
		   "s,t,p", AT, 0);
      break;

    case M_BGTU:
      if (treg == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "bne",
		       "s,t,p", sreg, 0);
	  return;
	}
      if (sreg == 0)
	goto do_false;
      macro_build ((char *) NULL, &icnt, NULL, "sltu", "d,v,t", AT, treg,
		   sreg);
      macro_build ((char *) NULL, &icnt, &offset_expr,
		   "bne",
		   "s,t,p", AT, 0);
      break;

    case M_BLE:
      if (treg == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "blez",
		       "s,p", sreg);
	  return;
	}
      if (sreg == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "bgez",
		       "s,p", treg);
	  return;
	}
      macro_build ((char *) NULL, &icnt, NULL, "slt", "d,v,t", AT, treg, sreg);
      macro_build ((char *) NULL, &icnt, &offset_expr,
		   "beq",
		   "s,t,p", AT, 0);
      break;

    case M_BLE_I:
      maxnum = 0x7fffffff;
      if (imm_expr.X_add_number >= maxnum && sizeof (maxnum) > 4)
	goto do_true;
      if (imm_expr.X_op != O_constant)
	as_bad (_("Unsupported large constant"));
      imm_expr.X_add_number++;
      /* FALLTHROUGH */
    case M_BLT_I:
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "bltz",
		       "s,p", sreg);
	  return;
	}
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number == 1)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "blez",
		       "s,p", sreg);
	  return;
	}
      set_at (&icnt, sreg, 0);
      macro_build ((char *) NULL, &icnt, &offset_expr,
		   "bne",
		   "s,t,p", AT, 0);
      break;

    case M_BLEU:
      if (treg == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "beq",
		       "s,t,p", sreg, 0);
	  return;
	}
      if (sreg == 0)
	goto do_true;
      macro_build ((char *) NULL, &icnt, NULL, "sltu", "d,v,t", AT, treg,
		   sreg);
      macro_build ((char *) NULL, &icnt, &offset_expr,
		   "beq",
		   "s,t,p", AT, 0);
      break;

    case M_BLEU_I:
      if (sreg == 0
	  || (ss_opts.isa < 3
	      && imm_expr.X_op == O_constant
	      && imm_expr.X_add_number == 0xffffffff))
	goto do_true;
      if (imm_expr.X_op != O_constant)
	as_bad (_("Unsupported large constant"));
      imm_expr.X_add_number++;
      /* FALLTHROUGH */
    case M_BLTU_I:
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number == 0)
	goto do_false;
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number == 1)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "beq",
		       "s,t,p", sreg, 0);
	  return;
	}
      set_at (&icnt, sreg, 1);
      macro_build ((char *) NULL, &icnt, &offset_expr,
		   "bne",
		   "s,t,p", AT, 0);
      break;

    case M_BLT:
      if (treg == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "bltz",
		       "s,p", sreg);
	  return;
	}
      if (sreg == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "bgtz",
		       "s,p", treg);
	  return;
	}
      macro_build ((char *) NULL, &icnt, NULL, "slt", "d,v,t", AT, sreg, treg);
      macro_build ((char *) NULL, &icnt, &offset_expr,
		   "bne",
		   "s,t,p", AT, 0);
      break;

    case M_BLTU:
      if (treg == 0)
	goto do_false;
      if (sreg == 0)
	{
	  macro_build ((char *) NULL, &icnt, &offset_expr,
		       "bne",
		       "s,t,p", 0, treg);
	  return;
	}
      macro_build ((char *) NULL, &icnt, NULL, "sltu", "d,v,t", AT, sreg,
		   treg);
      macro_build ((char *) NULL, &icnt, &offset_expr,
		   "bne",
		   "s,t,p", AT, 0);
      break;

    case M_DIV_3:
      s = "mflo";
      goto do_div3;
    case M_REM_3:
      s = "mfhi";
    do_div3:
      if (treg == 0)
	{
	  as_warn (_("Divide by zero."));
	  macro_build ((char *) NULL, &icnt, NULL, "break", "c", 7);
	  return;
	}

      macro_build ((char *) NULL, &icnt, NULL, "div", "z,s,t", sreg, treg);
      expr1.X_add_number = 8;
      macro_build ((char *) NULL, &icnt, &expr1, "bne", "s,t,p", treg, 0);
      macro_build ((char *) NULL, &icnt, NULL, "break", "c", 7);
      expr1.X_add_number = -1;
      macro_build ((char *) NULL, &icnt, &expr1,
		   "addiu",
		   "t,r,j", AT, 0, (int) BFD_RELOC_LO16);
      expr1.X_add_number = 24;
      macro_build ((char *) NULL, &icnt, &expr1, "bne", "s,t,p", treg, AT);
      expr1.X_add_number = 0x80000000;
      macro_build ((char *) NULL, &icnt, &expr1, "lui", "t,u", AT,
		   (int) BFD_RELOC_HI16);
      expr1.X_add_number = 8;
      macro_build ((char *) NULL, &icnt, &expr1, "bne", "s,t,p", sreg, AT);
      macro_build ((char *) NULL, &icnt, NULL, "break", "c", 6);
      macro_build ((char *) NULL, &icnt, NULL, s, "d", dreg);
      break;

    case M_DIV_3I:
      s = "div";
      s2 = "mflo";
      goto do_divi;
    case M_DIVU_3I:
      s = "divu";
      s2 = "mflo";
      goto do_divi;
    case M_REM_3I:
      s = "div";
      s2 = "mfhi";
      goto do_divi;
    case M_REMU_3I:
      s = "divu";
      s2 = "mfhi";
      goto do_divi;
    do_divi:
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number == 0)
	{
	  as_warn (_("Divide by zero."));
	  macro_build ((char *) NULL, &icnt, NULL, "break", "c", 7);
	}
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number == 1)
	{
	  if (strcmp (s2, "mflo") == 0)
	    macro_build ((char *) NULL, &icnt, NULL, "move", "d,s", dreg,
			 sreg);
	  else
	    macro_build ((char *) NULL, &icnt, NULL, "move", "d,s", dreg, 0);
	  return;
	}
      if (imm_expr.X_op == O_constant
	  && imm_expr.X_add_number == -1
	  && s[strlen (s) - 1] != 'u')
	{
	  if (strcmp (s2, "mflo") == 0)
	    {
	      macro_build ((char *) NULL, &icnt, NULL, "neg", "d,w", dreg,
			   sreg);
	    }
	  else
	    macro_build ((char *) NULL, &icnt, NULL, "move", "d,s", dreg, 0);
	  return;
	}

      load_register (&icnt, AT, &imm_expr, dbl);
      macro_build ((char *) NULL, &icnt, NULL, s, "z,s,t", sreg, AT);
      macro_build ((char *) NULL, &icnt, NULL, s2, "d", dreg);
      break;

    case M_DIVU_3:
      s = "divu";
      s2 = "mflo";
      goto do_divu3;
    case M_REMU_3:
      s = "divu";
      s2 = "mfhi";
      goto do_divu3;
    do_divu3:
      macro_build ((char *) NULL, &icnt, NULL, s, "z,s,t", sreg, treg);
      expr1.X_add_number = 8;
      macro_build ((char *) NULL, &icnt, &expr1, "bne", "s,t,p", treg, 0);
      macro_build ((char *) NULL, &icnt, NULL, "break", "c", 7);
      macro_build ((char *) NULL, &icnt, NULL, s2, "d", dreg);
      return;

    case M_LA_AB:
      /* Load the address of a symbol into a register.  If breg is not
	 zero, we then add a base register to it.  */

      if (offset_expr.X_op != O_symbol
	  && offset_expr.X_op != O_constant)
	{
	  as_bad (_("expression too complex"));
	  offset_expr.X_op = O_constant;
	}

      if (treg == breg)
	{
	  tempreg = AT;
	  used_at = 1;
	}
      else
	{
	  tempreg = treg;
	  used_at = 0;
	}

      if (offset_expr.X_op == O_constant)
	load_register (&icnt, tempreg, &offset_expr, dbl);
      else
	{
	  /* If this is a reference to an GP relative symbol, we want
	       addiu	$tempreg,$gp,<sym>	(BFD_RELOC_MIPS_GPREL)
	     Otherwise we want
	       lui	$tempreg,<sym>		(BFD_RELOC_HI16_S)
	       addiu	$tempreg,$tempreg,<sym>	(BFD_RELOC_LO16)
	     If we have a constant, we need two instructions anyhow,
	     so we may as well always use the latter form.  */
	  if ((valueT) offset_expr.X_add_number >= MAX_GPREL_OFFSET
	      || nopic_need_relax (offset_expr.X_add_symbol, 1))
	    p = NULL;
	  else
	    {
	      frag_grow (40);
	      macro_build ((char *) NULL, &icnt, &offset_expr, "addiu",
			   "t,r,j", tempreg, GP, (int) BFD_RELOC_MIPS_GPREL);
	      p = frag_var (rs_machine_dependent, 16, 0,
			    RELAX_ENCODE (8, 16, 4, 12, 0,
					  ss_opts.warn_about_macros),
			    offset_expr.X_add_symbol, (offsetT) 0,
			    (char *) NULL);
	    }
	  macro_build_lui (p, &icnt, &offset_expr, tempreg);
	  if (p != NULL)
	    p += 8;
	  macro_build (p, &icnt, &offset_expr,
		       "addiu",
		       "t,r,j", tempreg, tempreg, (int) BFD_RELOC_LO16);
	}

      if (breg != 0)
	macro_build ((char *) NULL, &icnt, (expressionS *) NULL,
		     "addu",
		     "d,v,t", treg, tempreg, breg);

      if (! used_at)
	return;

      break;

      /* The jal instructions must be handled as macros because when
	 generating PIC code they expand to multi-instruction
	 sequences.  Normally they are simple instructions.  */
    case M_JAL_1:
      dreg = RA;
      /* Fall through.  */
    case M_JAL_2:
      macro_build ((char *) NULL, &icnt, (expressionS *) NULL, "jalr",
		   "d,s", dreg, sreg);
      return;

    case M_JAL_A:
      macro_build ((char *) NULL, &icnt, &offset_expr, "jal", "a");
      return;

    case M_LB_AB:
      s = "lb";
      fmt = "t,o(b)";
      goto ld;
    case M_LB_AB_PLUS:
      s = "lb";
      fmt = "t,o(b)+";
      goto ld;
    case M_LB_AB_MINUS:
      s = "lb";
      fmt = "t,o(b)-";
      goto ld;
    case M_LB_AB_PRE_PLUS:
      s = "lb";
      fmt = "t,o(b)^+";
      goto ld;
    case M_LB_AB_PRE_MINUS:
      s = "lb";
      fmt = "t,o(b)^-";
      goto ld;

    case M_LBU_AB:
      s = "lbu";
      fmt = "t,o(b)";
      goto ld;
    case M_LBU_AB_PLUS:
      s = "lbu";
      fmt = "t,o(b)+";
      goto ld;
    case M_LBU_AB_MINUS:
      s = "lbu";
      fmt = "t,o(b)-";
      goto ld;
    case M_LBU_AB_PRE_PLUS:
      s = "lbu";
      fmt = "t,o(b)^+";
      goto ld;
    case M_LBU_AB_PRE_MINUS:
      s = "lbu";
      fmt = "t,o(b)^-";
      goto ld;

    case M_LH_AB:
      s = "lh";
      fmt = "t,o(b)";
      goto ld;
    case M_LH_AB_PLUS:
      s = "lh";
      fmt = "t,o(b)+";
      goto ld;
    case M_LH_AB_MINUS:
      s = "lh";
      fmt = "t,o(b)-";
      goto ld;
    case M_LH_AB_PRE_PLUS:
      s = "lh";
      fmt = "t,o(b)^+";
      goto ld;
    case M_LH_AB_PRE_MINUS:
      s = "lh";
      fmt = "t,o(b)^-";
      goto ld;

    case M_LHU_AB:
      s = "lhu";
      fmt = "t,o(b)";
      goto ld;
    case M_LHU_AB_PLUS:
      s = "lhu";
      fmt = "t,o(b)+";
      goto ld;
    case M_LHU_AB_MINUS:
      s = "lhu";
      fmt = "t,o(b)-";
      goto ld;
    case M_LHU_AB_PRE_PLUS:
      s = "lhu";
      fmt = "t,o(b)^+";
      goto ld;
    case M_LHU_AB_PRE_MINUS:
      s = "lhu";
      fmt = "t,o(b)^-";
      goto ld;

    case M_LW_AB:
      s = "lw";
      fmt = "t,o(b)";
      goto ld;
    case M_LW_AB_PLUS:
      s = "lw";
      fmt = "t,o(b)+";
      goto ld;
    case M_LW_AB_MINUS:
      s = "lw";
      fmt = "t,o(b)-";
      goto ld;
    case M_LW_AB_PRE_PLUS:
      s = "lw";
      fmt = "t,o(b)^+";
      goto ld;
    case M_LW_AB_PRE_MINUS:
      s = "lw";
      fmt = "t,o(b)^-";
      goto ld;

    case M_L_S_AB:
      coproc = 1;
      s = "l.s";
      fmt = "T,o(b)";
      goto ld;
    case M_L_S_AB_PLUS:
      coproc = 1;
      s = "l.s";
      fmt = "T,o(b)+";
      goto ld;
    case M_L_S_AB_MINUS:
      coproc = 1;
      s = "l.s";
      fmt = "T,o(b)-";
      goto ld;
    case M_L_S_AB_PRE_PLUS:
      coproc = 1;
      s = "l.s";
      fmt = "T,o(b)^+";
      goto ld;
    case M_L_S_AB_PRE_MINUS:
      coproc = 1;
      s = "l.s";
      fmt = "T,o(b)^-";
      goto ld;

    case M_LWL_AB:
      s = "lwl";
      fmt = "t,o(b)";
      goto ld;
    case M_LWR_AB:
      s = "lwr";
      fmt = "t,o(b)";
      goto ld;

    ld:
      if (breg == treg || coproc)
	{
	  tempreg = AT;
	  used_at = 1;
	}
      else
	{
	  tempreg = treg;
	  used_at = 0;
	}
      goto ld_st;

    case M_SB_AB:
      s = "sb";
      fmt = "t,o(b)";
      goto st;
    case M_SB_AB_PLUS:
      s = "sb";
      fmt = "t,o(b)+";
      goto st;
    case M_SB_AB_MINUS:
      s = "sb";
      fmt = "t,o(b)-";
      goto st;
    case M_SB_AB_PRE_PLUS:
      s = "sb";
      fmt = "t,o(b)^+";
      goto st;
    case M_SB_AB_PRE_MINUS:
      s = "sb";
      fmt = "t,o(b)^-";
      goto st;

    case M_SH_AB:
      s = "sh";
      fmt = "t,o(b)";
      goto st;
    case M_SH_AB_PLUS:
      s = "sh";
      fmt = "t,o(b)+";
      goto st;
    case M_SH_AB_MINUS:
      s = "sh";
      fmt = "t,o(b)-";
      goto st;
    case M_SH_AB_PRE_PLUS:
      s = "sh";
      fmt = "t,o(b)^+";
      goto st;
    case M_SH_AB_PRE_MINUS:
      s = "sh";
      fmt = "t,o(b)^-";
      goto st;

    case M_SW_AB:
      s = "sw";
      fmt = "t,o(b)";
      goto st;
    case M_SW_AB_PLUS:
      s = "sw";
      fmt = "t,o(b)+";
      goto st;
    case M_SW_AB_MINUS:
      s = "sw";
      fmt = "t,o(b)-";
      goto st;
    case M_SW_AB_PRE_PLUS:
      s = "sw";
      fmt = "t,o(b)^+";
      goto st;
    case M_SW_AB_PRE_MINUS:
      s = "sw";
      fmt = "t,o(b)^-";
      goto st;

    case M_S_S_AB:
      coproc = 1;
      s = "s.s";
      fmt = "T,o(b)";
      goto st;
    case M_S_S_AB_PLUS:
      coproc = 1;
      s = "s.s";
      fmt = "T,o(b)+";
      goto st;
    case M_S_S_AB_MINUS:
      coproc = 1;
      s = "s.s";
      fmt = "T,o(b)-";
      goto st;
    case M_S_S_AB_PRE_PLUS:
      coproc = 1;
      s = "s.s";
      fmt = "T,o(b)^+";
      goto st;
    case M_S_S_AB_PRE_MINUS:
      coproc = 1;
      s = "s.s";
      fmt = "T,o(b)^-";
      goto st;

    case M_SWL_AB:
      s = "swl";
      fmt = "t,o(b)";
      goto st;
    case M_SWR_AB:
      s = "swr";
      fmt = "t,o(b)";
      goto st;

    st:
      tempreg = AT;
      used_at = 1;
    ld_st:
      if (offset_expr.X_op != O_constant
	  && offset_expr.X_op != O_symbol)
	{
	  as_bad (_("expression too complex"));
	  offset_expr.X_op = O_constant;
	}

	{
	  /* If this is a reference to a GP relative symbol, and there
	     is no base register, we want
	       <op>	$treg,<sym>($gp)	(BFD_RELOC_MIPS_GPREL)
	     Otherwise, if there is no base register, we want
	       lui	$tempreg,<sym>		(BFD_RELOC_HI16_S)
	       <op>	$treg,<sym>($tempreg)	(BFD_RELOC_LO16)
	     If we have a constant, we need two instructions anyhow,
	     so we always use the latter form.

	     If we have a base register, and this is a reference to a
	     GP relative symbol, we want
	       addu	$tempreg,$breg,$gp
	       <op>	$treg,<sym>($tempreg)	(BFD_RELOC_MIPS_GPREL)
	     Otherwise we want
	       lui	$tempreg,<sym>		(BFD_RELOC_HI16_S)
	       addu	$tempreg,$tempreg,$breg
	       <op>	$treg,<sym>($tempreg)	(BFD_RELOC_LO16)
	     With a constant we always use the latter case.  */
	  if (breg == 0)
	    {
	      if ((valueT) offset_expr.X_add_number >= MAX_GPREL_OFFSET
		  || nopic_need_relax (offset_expr.X_add_symbol, 1))
		p = NULL;
	      else
		{
		  frag_grow (40);
		  macro_build ((char *) NULL, &icnt, &offset_expr, s, fmt,
			       treg, (int) BFD_RELOC_MIPS_GPREL, GP);
		  p = frag_var (rs_machine_dependent, 16, 0,
				RELAX_ENCODE (8, 16, 4, 12, 0,
					      (ss_opts.warn_about_macros
					       || (used_at
						   && ss_opts.noat))),
				offset_expr.X_add_symbol, (offsetT) 0,
				(char *) NULL);
		  used_at = 0;
		}
	      macro_build_lui (p, &icnt, &offset_expr, tempreg);
	      if (p != NULL)
		p += 8;
	      macro_build (p, &icnt, &offset_expr, s, fmt, treg,
			   (int) BFD_RELOC_LO16, tempreg);
	    }
	  else
	    {
	      if ((valueT) offset_expr.X_add_number >= MAX_GPREL_OFFSET
		  || nopic_need_relax (offset_expr.X_add_symbol, 1))
		p = NULL;
	      else
		{
		  frag_grow (56);
		  macro_build ((char *) NULL, &icnt, (expressionS *) NULL,
			       "addu",
			       "d,v,t", tempreg, breg, GP);
		  macro_build ((char *) NULL, &icnt, &offset_expr, s, fmt,
			       treg, (int) BFD_RELOC_MIPS_GPREL, tempreg);
		  p = frag_var (rs_machine_dependent, 24, 0,
				RELAX_ENCODE (16, 24, 4, 20, 0, 0),
				offset_expr.X_add_symbol, (offsetT) 0,
				(char *) NULL);
		}
	      macro_build_lui (p, &icnt, &offset_expr, tempreg);
	      if (p != NULL)
		p += 8;
	      macro_build (p, &icnt, (expressionS *) NULL,
			   "addu",
			   "d,v,t", tempreg, tempreg, breg);
	      if (p != NULL)
		p += 8;
	      macro_build (p, &icnt, &offset_expr, s, fmt, treg,
			   (int) BFD_RELOC_LO16, tempreg);
	    }
	}

      if (! used_at)
	return;

      break;

    case M_LI:
    case M_LI_S:
      load_register (&icnt, treg, &imm_expr, 0);
      return;

    case M_LI_SS:
      if (imm_expr.X_op == O_constant)
	{
	  load_register (&icnt, AT, &imm_expr, 0);
	  macro_build ((char *) NULL, &icnt, (expressionS *) NULL,
		       "mtc1", "t,G", AT, treg);
	  break;
	}
      else
	{
	  assert (offset_expr.X_op == O_symbol
		  && strcmp (segment_name (S_GET_SEGMENT
					   (offset_expr.X_add_symbol)),
			     ".lit4") == 0
		  && offset_expr.X_add_number == 0);
	  macro_build ((char *) NULL, &icnt, &offset_expr, "l.s", "T,o(b)",
		       treg, (int) BFD_RELOC_MIPS_LITERAL, GP);
	  return;
	}

    case M_LI_D:
      /* We know that sym is in the .rdata section.  First we get the
	 upper 16 bits of the address.  */
      /* FIXME: This won't work for a 64 bit address.  */
      macro_build_lui ((char *) NULL, &icnt, &offset_expr, AT);

      /* Now we load the register(s).  */
      macro_build ((char *) NULL, &icnt, &offset_expr, "dlw", "t,o(b)",
		   treg, (int) BFD_RELOC_LO16, AT);

      /* To avoid confusion in tc_gen_reloc, we must ensure that this
	 does not become a variant frag.  */
      frag_wane (frag_now);
      frag_new (0);

      break;

    case M_LI_DD:
      assert (offset_expr.X_op == O_symbol
	      && offset_expr.X_add_number == 0);
      s = segment_name (S_GET_SEGMENT (offset_expr.X_add_symbol));
      if (strcmp (s, ".lit8") == 0)
	{
	  breg = GP;
	  r = BFD_RELOC_MIPS_LITERAL;
	  goto dob;
	}
      else
	{
	  assert (strcmp (s, RDATA_SECTION_NAME) == 0);
	  /* FIXME: This won't work for a 64 bit address.  */
	  macro_build_lui ((char *) NULL, &icnt, &offset_expr, AT);
	      
	  breg = AT;
	  r = BFD_RELOC_LO16;
	  goto dob;
	}

    dob:
      macro_build ((char *) NULL, &icnt, &offset_expr, "l.d", "T,o(b)",
		   treg, 
		   (int) r, breg);

      /* To avoid confusion in tc_gen_reloc, we must ensure that this
	 does not become a variant frag.  */
      frag_wane (frag_now);
      frag_new (0);

      if (breg != AT)
	return;
      break;

    case M_DLW_AB:
      s = "dlw";
      fmt = "t,o(b)";
      goto ldd_std;
    case M_DLW_AB_PLUS:
      s = "dlw";
      fmt = "t,o(b)+";
      goto ldd_std;
    case M_DLW_AB_MINUS:
      s = "dlw";
      fmt = "t,o(b)-";
      goto ldd_std;
    case M_DLW_AB_PRE_PLUS:
      s = "dlw";
      fmt = "t,o(b)^+";
      goto ldd_std;
    case M_DLW_AB_PRE_MINUS:
      s = "dlw";
      fmt = "t,o(b)^-";
      goto ldd_std;

    case M_L_D_AB:
      s = "l.d";
      fmt = "T,o(b)";
      goto ldd_std;
    case M_L_D_AB_PLUS:
      s = "l.d";
      fmt = "T,o(b)+";
      goto ldd_std;
    case M_L_D_AB_MINUS:
      s = "l.d";
      fmt = "T,o(b)-";
      goto ldd_std;
    case M_L_D_AB_PRE_PLUS:
      s = "l.d";
      fmt = "T,o(b)^+";
      goto ldd_std;
    case M_L_D_AB_PRE_MINUS:
      s = "l.d";
      fmt = "T,o(b)^-";
      goto ldd_std;

    case M_DSW_AB:
      s = "dsw";
      fmt = "t,o(b)";
      goto ldd_std;
    case M_DSW_AB_PLUS:
      s = "dsw";
      fmt = "t,o(b)+";
      goto ldd_std;
    case M_DSW_AB_MINUS:
      s = "dsw";
      fmt = "t,o(b)-";
      goto ldd_std;
    case M_DSW_AB_PRE_PLUS:
      s = "dsw";
      fmt = "t,o(b)^+";
      goto ldd_std;
    case M_DSW_AB_PRE_MINUS:
      s = "dsw";
      fmt = "t,o(b)^-";
      goto ldd_std;

    case M_DSZ_AB:
      s = "dsz";
      zero = 1;
      fmt = "o(b)";
      goto ldd_std;
    case M_DSZ_AB_PLUS:
      s = "dsz";
      zero = 1;
      fmt = "o(b)+";
      goto ldd_std;
    case M_DSZ_AB_MINUS:
      s = "dsz";
      zero = 1;
      fmt = "o(b)-";
      goto ldd_std;
    case M_DSZ_AB_PRE_PLUS:
      s = "dsz";
      zero = 1;
      fmt = "o(b)^+";
      goto ldd_std;
    case M_DSZ_AB_PRE_MINUS:
      s = "dsz";
      zero = 1;
      fmt = "o(b)^-";
      goto ldd_std;

    case M_S_D_AB:
      s = "s.d";
      fmt = "T,o(b)";
      goto ldd_std;
    case M_S_D_AB_PLUS:
      s = "s.d";
      fmt = "T,o(b)+";
      goto ldd_std;
    case M_S_D_AB_MINUS:
      s = "s.d";
      fmt = "T,o(b)-";
      goto ldd_std;
    case M_S_D_AB_PRE_PLUS:
      s = "s.d";
      fmt = "T,o(b)^+";
      goto ldd_std;
    case M_S_D_AB_PRE_MINUS:
      s = "s.d";
      fmt = "T,o(b)^-";
      goto ldd_std;

    ldd_std:
      if (offset_expr.X_op != O_symbol
	  && offset_expr.X_op != O_constant)
	{
	  as_bad (_("expression too complex"));
	  offset_expr.X_op = O_constant;
	}

      /* If this is a reference to a GP relative symbol, we want
	 <op>	$treg,<sym>($gp)	(BFD_RELOC_MIPS_GPREL)
	 <op>	$treg+1,<sym>+4($gp)	(BFD_RELOC_MIPS_GPREL)
	 If we have a base register, we use this
	 addu	$at,$breg,$gp
	 <op>	$treg,<sym>($at)	(BFD_RELOC_MIPS_GPREL)
	 <op>	$treg+1,<sym>+4($at)	(BFD_RELOC_MIPS_GPREL)
	 If this is not a GP relative symbol, we want
	 lui	$at,<sym>		(BFD_RELOC_HI16_S)
	 <op>	$treg,<sym>($at)	(BFD_RELOC_LO16)
	 <op>	$treg+1,<sym>+4($at)	(BFD_RELOC_LO16)
	 If there is a base register, we add it to $at after the
	 lui instruction.  If there is a constant, we always use
	 the last case.  */
      if ((valueT) offset_expr.X_add_number >= MAX_GPREL_OFFSET
	  || nopic_need_relax (offset_expr.X_add_symbol, 1))
	{
	  p = NULL;
	  used_at = 1;
	}
      else
	{
	  int off;
	  
	  if (breg == 0)
	    {
	      frag_grow (56);
	      tempreg = GP;
	      off = 4;
	      used_at = 0;
	    }
	  else
	    {
	      frag_grow (72);
	      macro_build ((char *) NULL, &icnt, (expressionS *) NULL,
			   "addu",
			   "d,v,t", AT, breg, GP);
	      tempreg = AT;
	      off = 12;
	      used_at = 1;
	    }

	  if (zero)
	    macro_build ((char *) NULL, &icnt, &offset_expr, s, fmt,
			 (int) BFD_RELOC_MIPS_GPREL, tempreg);
	  else
	    macro_build ((char *) NULL, &icnt, &offset_expr, s, fmt,
			 treg, (int) BFD_RELOC_MIPS_GPREL, tempreg);

	  p = frag_var (rs_machine_dependent, 24 + (off - 4), 0,
			RELAX_ENCODE (8 + (off - 4), 16 + (off - 4),
				      0 + 4, 8 + off, 0,
				      used_at && ss_opts.noat),
			offset_expr.X_add_symbol, (long) 0,
			(char *) NULL);

	  /* We just generated two relocs.  When tc_gen_reloc handles
	     this case, it will skip the first reloc and handle the
	     second.  The second reloc already has an extra addend of
	     4, which we added above.  We must subtract it out, and
	     then subtract another 4 to make the first reloc come out
	     right.  The second reloc will come out right because we
	     are going to add 4 to offset_expr when we build its
	     instruction below.  */
	  /* offset_expr.X_add_number -= 8; */
	  offset_expr.X_op = O_constant;
	}
      macro_build_lui (p, &icnt, &offset_expr, AT);
      if (p != NULL)
	p += 8;
      if (breg != 0)
	{
	  macro_build (p, &icnt, (expressionS *) NULL,
		       "addu", "d,v,t", AT, breg, AT);
	  if (p != NULL)
	    p += 8;
	}
      if (zero)
	macro_build (p, &icnt, &offset_expr, s, fmt,
		     (int) BFD_RELOC_LO16, AT);
      else
	macro_build (p, &icnt, &offset_expr, s, fmt,
		     treg, (int) BFD_RELOC_LO16, AT);

      if (! used_at)
	return;
      break;


    case M_MUL:
      macro_build ((char *) NULL, &icnt, NULL,
		   "multu",
		   "s,t", sreg, treg);
      macro_build ((char *) NULL, &icnt, NULL, "mflo", "d", dreg);
      return;

    case M_MUL_I:
      /* The SS assembler some times generates shifts and adds.  I'm
	 not trying to be that fancy. GCC should do this for us
	 anyway.  */
      load_register (&icnt, AT, &imm_expr, dbl);
      macro_build ((char *) NULL, &icnt, NULL,
		   "mult", "s,t", sreg, AT);
      macro_build ((char *) NULL, &icnt, NULL, "mflo", "d", dreg);
      break;

    case M_MULO_I:
      imm = 1;
      goto do_mulo;

    case M_MULO:
    do_mulo:
      if (imm)
	load_register (&icnt, AT, &imm_expr, dbl);
      macro_build ((char *) NULL, &icnt, NULL,
		   "mult",
		   "s,t", sreg, imm ? AT : treg);
      macro_build ((char *) NULL, &icnt, NULL, "mflo", "d", dreg);
      macro_build ((char *) NULL, &icnt, NULL,
		   "sra",
		   "d,w,<", dreg, dreg, 31);
      macro_build ((char *) NULL, &icnt, NULL, "mfhi", "d", AT);
      expr1.X_add_number = 8;
      macro_build ((char *) NULL, &icnt, &expr1, "beq", "s,t,p", dreg, AT);
      macro_build ((char *) NULL, &icnt, NULL, "break", "c", 6);
      macro_build ((char *) NULL, &icnt, NULL, "mflo", "d", dreg);
      break;

    case M_MULOU_I:
      imm = 1;
      goto do_mulou;

    case M_MULOU:
    do_mulou:
      if (imm)
	load_register (&icnt, AT, &imm_expr, dbl);
      macro_build ((char *) NULL, &icnt, NULL,
		   "multu",
		   "s,t", sreg, imm ? AT : treg);
      macro_build ((char *) NULL, &icnt, NULL, "mfhi", "d", AT);
      macro_build ((char *) NULL, &icnt, NULL, "mflo", "d", dreg);
      expr1.X_add_number = 8;
      macro_build ((char *) NULL, &icnt, &expr1, "beq", "s,t,p", AT, 0);
      macro_build ((char *) NULL, &icnt, NULL, "break", "c", 6);
      break;

    case M_ROL:
      macro_build ((char *) NULL, &icnt, NULL, "subu", "d,v,t", AT, 0, treg);
      macro_build ((char *) NULL, &icnt, NULL, "srlv", "d,t,s", AT, sreg, AT);
      macro_build ((char *) NULL, &icnt, NULL, "sllv", "d,t,s", dreg, sreg,
		   treg);
      macro_build ((char *) NULL, &icnt, NULL, "or", "d,v,t", dreg, dreg, AT);
      break;

    case M_ROL_I:
      if (imm_expr.X_op != O_constant)
	as_bad (_("rotate count too large"));
      macro_build ((char *) NULL, &icnt, NULL, "sll", "d,w,<", AT, sreg,
		   (int) (imm_expr.X_add_number & 0x1f));
      macro_build ((char *) NULL, &icnt, NULL, "srl", "d,w,<", dreg, sreg,
		   (int) ((0 - imm_expr.X_add_number) & 0x1f));
      macro_build ((char *) NULL, &icnt, NULL, "or", "d,v,t", dreg, dreg, AT);
      break;

    case M_ROR:
      macro_build ((char *) NULL, &icnt, NULL, "subu", "d,v,t", AT, 0, treg);
      macro_build ((char *) NULL, &icnt, NULL, "sllv", "d,t,s", AT, sreg, AT);
      macro_build ((char *) NULL, &icnt, NULL, "srlv", "d,t,s", dreg, sreg,
		   treg);
      macro_build ((char *) NULL, &icnt, NULL, "or", "d,v,t", dreg, dreg, AT);
      break;

    case M_ROR_I:
      if (imm_expr.X_op != O_constant)
	as_bad (_("rotate count too large"));
      macro_build ((char *) NULL, &icnt, NULL, "srl", "d,w,<", AT, sreg,
		   (int) (imm_expr.X_add_number & 0x1f));
      macro_build ((char *) NULL, &icnt, NULL, "sll", "d,w,<", dreg, sreg,
		   (int) ((0 - imm_expr.X_add_number) & 0x1f));
      macro_build ((char *) NULL, &icnt, NULL, "or", "d,v,t", dreg, dreg, AT);
      break;

    case M_SEQ:
      if (sreg == 0)
	macro_build ((char *) NULL, &icnt, &expr1, "sltiu", "t,r,j", dreg,
		     treg, (int) BFD_RELOC_LO16);
      else if (treg == 0)
	macro_build ((char *) NULL, &icnt, &expr1, "sltiu", "t,r,j", dreg,
		     sreg, (int) BFD_RELOC_LO16);
      else
	{
	  macro_build ((char *) NULL, &icnt, NULL, "xor", "d,v,t", dreg,
		       sreg, treg);
	  macro_build ((char *) NULL, &icnt, &expr1, "sltiu", "t,r,j", dreg,
		       dreg, (int) BFD_RELOC_LO16);
	}
      return;

    case M_SEQ_I:
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number == 0)
	{
	  macro_build ((char *) NULL, &icnt, &expr1, "sltiu", "t,r,j", dreg,
		       sreg, (int) BFD_RELOC_LO16);
	  return;
	}
      if (sreg == 0)
	{
	  as_warn (_("Instruction %s: result is always false"),
		   ip->insn_mo->name);
	  macro_build ((char *) NULL, &icnt, NULL, "move", "d,s", dreg, 0);
	  return;
	}
      if (imm_expr.X_op == O_constant
	  && imm_expr.X_add_number >= 0
	  && imm_expr.X_add_number < 0x10000)
	{
	  macro_build ((char *) NULL, &icnt, &imm_expr, "xori", "t,r,i", dreg,
		       sreg, (int) BFD_RELOC_LO16);
	  used_at = 0;
	}
      else if (imm_expr.X_op == O_constant
	       && imm_expr.X_add_number > -0x8000
	       && imm_expr.X_add_number < 0)
	{
	  imm_expr.X_add_number = -imm_expr.X_add_number;
	  macro_build ((char *) NULL, &icnt, &imm_expr,
		       "addiu",
		       "t,r,j", dreg, sreg,
		       (int) BFD_RELOC_LO16);
	  used_at = 0;
	}
      else
	{
	  load_register (&icnt, AT, &imm_expr, 0);
	  macro_build ((char *) NULL, &icnt, NULL, "xor", "d,v,t", dreg,
		       sreg, AT);
	  used_at = 1;
	}
      macro_build ((char *) NULL, &icnt, &expr1, "sltiu", "t,r,j", dreg, dreg,
		   (int) BFD_RELOC_LO16);
      if (used_at)
	break;
      return;

    case M_SGE:		/* sreg >= treg <==> not (sreg < treg) */
      s = "slt";
      goto sge;
    case M_SGEU:
      s = "sltu";
    sge:
      macro_build ((char *) NULL, &icnt, NULL, s, "d,v,t", dreg, sreg, treg);
      macro_build ((char *) NULL, &icnt, &expr1, "xori", "t,r,i", dreg, dreg,
		   (int) BFD_RELOC_LO16);
      return;

    case M_SGE_I:		/* sreg >= I <==> not (sreg < I) */
    case M_SGEU_I:
      if (imm_expr.X_op == O_constant
	  && imm_expr.X_add_number >= -0x8000
	  && imm_expr.X_add_number < 0x8000)
	{
	  macro_build ((char *) NULL, &icnt, &imm_expr,
		       mask == M_SGE_I ? "slti" : "sltiu",
		       "t,r,j", dreg, sreg, (int) BFD_RELOC_LO16);
	  used_at = 0;
	}
      else
	{
	  load_register (&icnt, AT, &imm_expr, 0);
	  macro_build ((char *) NULL, &icnt, NULL,
		       mask == M_SGE_I ? "slt" : "sltu",
		       "d,v,t", dreg, sreg, AT);
	  used_at = 1;
	}
      macro_build ((char *) NULL, &icnt, &expr1, "xori", "t,r,i", dreg, dreg,
		   (int) BFD_RELOC_LO16);
      if (used_at)
	break;
      return;

    case M_SGT:		/* sreg > treg  <==>  treg < sreg */
      s = "slt";
      goto sgt;
    case M_SGTU:
      s = "sltu";
    sgt:
      macro_build ((char *) NULL, &icnt, NULL, s, "d,v,t", dreg, treg, sreg);
      return;

    case M_SGT_I:		/* sreg > I  <==>  I < sreg */
      s = "slt";
      goto sgti;
    case M_SGTU_I:
      s = "sltu";
    sgti:
      load_register (&icnt, AT, &imm_expr, 0);
      macro_build ((char *) NULL, &icnt, NULL, s, "d,v,t", dreg, AT, sreg);
      break;

    case M_SLE:		/* sreg <= treg  <==>  treg >= sreg  <==>  not (treg < sreg) */
      s = "slt";
      goto sle;
    case M_SLEU:
      s = "sltu";
    sle:
      macro_build ((char *) NULL, &icnt, NULL, s, "d,v,t", dreg, treg, sreg);
      macro_build ((char *) NULL, &icnt, &expr1, "xori", "t,r,i", dreg, dreg,
		   (int) BFD_RELOC_LO16);
      return;

    case M_SLE_I:		/* sreg <= I <==> I >= sreg <==> not (I < sreg) */
      s = "slt";
      goto slei;
    case M_SLEU_I:
      s = "sltu";
    slei:
      load_register (&icnt, AT, &imm_expr, 0);
      macro_build ((char *) NULL, &icnt, NULL, s, "d,v,t", dreg, AT, sreg);
      macro_build ((char *) NULL, &icnt, &expr1, "xori", "t,r,i", dreg, dreg,
		   (int) BFD_RELOC_LO16);
      break;

    case M_SLT_I:
      if (imm_expr.X_op == O_constant
	  && imm_expr.X_add_number >= -0x8000
	  && imm_expr.X_add_number < 0x8000)
	{
	  macro_build ((char *) NULL, &icnt, &imm_expr, "slti", "t,r,j",
		       dreg, sreg, (int) BFD_RELOC_LO16);
	  return;
	}
      load_register (&icnt, AT, &imm_expr, 0);
      macro_build ((char *) NULL, &icnt, NULL, "slt", "d,v,t", dreg, sreg, AT);
      break;

    case M_SLTU_I:
      if (imm_expr.X_op == O_constant
	  && imm_expr.X_add_number >= -0x8000
	  && imm_expr.X_add_number < 0x8000)
	{
	  macro_build ((char *) NULL, &icnt, &imm_expr, "sltiu", "t,r,j",
		       dreg, sreg, (int) BFD_RELOC_LO16);
	  return;
	}
      load_register (&icnt, AT, &imm_expr, 0);
      macro_build ((char *) NULL, &icnt, NULL, "sltu", "d,v,t", dreg, sreg,
		   AT);
      break;

    case M_SNE:
      if (sreg == 0)
	macro_build ((char *) NULL, &icnt, NULL, "sltu", "d,v,t", dreg, 0,
		     treg);
      else if (treg == 0)
	macro_build ((char *) NULL, &icnt, NULL, "sltu", "d,v,t", dreg, 0,
		     sreg);
      else
	{
	  macro_build ((char *) NULL, &icnt, NULL, "xor", "d,v,t", dreg,
		       sreg, treg);
	  macro_build ((char *) NULL, &icnt, NULL, "sltu", "d,v,t", dreg, 0,
		       dreg);
	}
      return;

    case M_SNE_I:
      if (imm_expr.X_op == O_constant && imm_expr.X_add_number == 0)
	{
	  macro_build ((char *) NULL, &icnt, NULL, "sltu", "d,v,t", dreg, 0,
		       sreg);
	  return;
	}
      if (sreg == 0)
	{
	  as_warn (_("Instruction %s: result is always true"),
		   ip->insn_mo->name);
	  macro_build ((char *) NULL, &icnt, &expr1,
		       "addiu",
		       "t,r,j", dreg, 0, (int) BFD_RELOC_LO16);
	  return;
	}
      if (imm_expr.X_op == O_constant
	  && imm_expr.X_add_number >= 0
	  && imm_expr.X_add_number < 0x10000)
	{
	  macro_build ((char *) NULL, &icnt, &imm_expr, "xori", "t,r,i",
		       dreg, sreg, (int) BFD_RELOC_LO16);
	  used_at = 0;
	}
      else if (imm_expr.X_op == O_constant
	       && imm_expr.X_add_number > -0x8000
	       && imm_expr.X_add_number < 0)
	{
	  imm_expr.X_add_number = -imm_expr.X_add_number;
	  macro_build ((char *) NULL, &icnt, &imm_expr,
		       "addiu",
		       "t,r,j", dreg, sreg, (int) BFD_RELOC_LO16);
	  used_at = 0;
	}
      else
	{
	  load_register (&icnt, AT, &imm_expr, 0);
	  macro_build ((char *) NULL, &icnt, NULL, "xor", "d,v,t", dreg,
		       sreg, AT);
	  used_at = 1;
	}
      macro_build ((char *) NULL, &icnt, NULL, "sltu", "d,v,t", dreg, 0, dreg);
      if (used_at)
	break;
      return;

    case M_SUB_I:
      if (imm_expr.X_op == O_constant
	  && imm_expr.X_add_number > -0x8000
	  && imm_expr.X_add_number <= 0x8000)
	{
	  imm_expr.X_add_number = -imm_expr.X_add_number;
	  macro_build ((char *) NULL, &icnt, &imm_expr,
		       "addi",
		       "t,r,j", dreg, sreg, (int) BFD_RELOC_LO16);
	  return;
	}
      load_register (&icnt, AT, &imm_expr, dbl);
      macro_build ((char *) NULL, &icnt, NULL,
		   "sub",
		   "d,v,t", dreg, sreg, AT);
      break;

    case M_SUBU_I:
      if (imm_expr.X_op == O_constant
	  && imm_expr.X_add_number > -0x8000
	  && imm_expr.X_add_number <= 0x8000)
	{
	  imm_expr.X_add_number = -imm_expr.X_add_number;
	  macro_build ((char *) NULL, &icnt, &imm_expr,
		       "addiu",
		       "t,r,j", dreg, sreg, (int) BFD_RELOC_LO16);
	  return;
	}
      load_register (&icnt, AT, &imm_expr, dbl);
      macro_build ((char *) NULL, &icnt, NULL,
		   "subu",
		   "d,v,t", dreg, sreg, AT);
      break;

    case M_TRUNCWD:
    case M_TRUNCWS:
      sreg = (ip->insn_opcode.b >> 24) & 0xff;	/* floating reg */
      dreg = (ip->insn_opcode.b >> 8) & 0xff;	/* floating reg */

      /*
       * Is the double cfc1 instruction a bug in the mips assembler;
       * or is there a reason for it?
       */
      macro_build ((char *) NULL, &icnt, NULL,
	      mask == M_TRUNCWD ? "cvt.w.d" : "cvt.w.s", "D,S", dreg, sreg);
      break;

    default:
      /* FIXME: Check if this is one of the itbl macros, since they
	 are added dynamically. */
      as_bad (_("Macro %s not implemented yet"), ip->insn_mo->name);
      break;
    }
  if (ss_opts.noat)
    as_warn (_("Macro used $at after \".set noat\""));
}

/* For consistency checking, verify that all bits are specified either
   by the match/mask part of the instruction definition, or by the
   operand list.  */
static int
validate_ss_insn (opc)
     const struct ss_opcode *opc;
{
  unsigned long used_bits = opc->mask;

  if ((used_bits & opc->match) != opc->match)
    {
      as_bad (_("internal: bad ss opcode (mask error): %s %s"),
	      opc->name, opc->args);
      return 0;
    }
  return 1;
}

/* parse instruction flags, of the form:
     /a-z	- set bit flag-0
     /N:M(X)	- set bits M-N with value X
   returns when the first white space is encountered
*/

static void
ProcessInsnFlag(s)
char *s;
{
  char c;
  int bits, start, end, value;

  if (sscanf(s, "/%d:%d(%d)/", &end, &start, &value) == 3) {
    if (start >= end)
      as_fatal ("Bad flag format: start >= end, '%s' (ignore trailing /)", s);
    bits = (end - start) + 1;
    if ((1 << bits) <= value)
      as_fatal ("Bad flag value: too large, '%s' (ignore trailing /)", s);
    if (value < 0)
      as_fatal ("Bad flag value: negative, '%s' (ignore trailing /)", s);
    insn_flags |= value << start;
  }
  else if (sscanf(s, "/%c/", &c) == 1) {
    if (!islower(c))
      as_fatal ("Unknown flag: `/%c'", c);
    insn_flags |= 1 << (c - 'a');
  }
  else
    as_fatal ("Unknown flag: `%s' (ignore trailing /)", s);
}

static char *
ParseInsnFlags(s)
char *s;
{
  char arg[256], *p = arg;
  for (;;) {
    /* / */
    if (*s != '/')
      return s;
    p = arg;
    *p++ = *s++;

    /* flag */
    while (*s != '/' && *s != ' ' && *s != '\t' && *s != '\0')
      *p++ = *s++;
    *p++ = '/';

    /* real flag */
    if (arg[0] != '/' || !arg[1] || arg[1] == '/')
      as_fatal ("Unknown flag: `%s' (ignore trailing /)", arg);

    ProcessInsnFlag(arg);

    /* done? */
    if (*s != '/')
      return s;
  }
}

/* This routine assembles an instruction into its binary format.  As a
   side effect, it sets one of the global variables imm_reloc or
   offset_reloc to the type of relocation to do if one of the operands
   is an address expression.  */

static void
ss_ip (str, ip)
     char *str;
     struct ss_cl_insn *ip;
{
  char opc[256];
  char *s;
  const char *args;
  char c;
  struct ss_opcode *insn;
  char *argsStart;
  unsigned int regno;
  unsigned int lastregno = 0;
  char *s_reset;
  char save_c = 0;
  int full_opcode_match = 1;

  insn_error = NULL;

  /* If the instruction contains a '.', we first try to match an instruction
     including the '.'.  Then we try again without the '.'.  */
  insn = NULL;
  for (s = str; (*s != '\0') && (*s != '/') && !isspace ((unsigned char) *s); ++s)
    continue;

  /* If we stopped on whitespace, then replace the whitespace with null for
     the call to hash_find.  Save the character we replaced just in case we
     have to re-parse the instruction.  */
  if (isspace ((unsigned char) *s) || *s == '/')
    {
      save_c = *s;
      *s++ = '\0';
    }

  insn = (struct ss_opcode *) hash_find (op_hash, str);

  /* If we didn't find the instruction in the opcode table, try again, but
     this time with just the instruction up to, but not including the
     first '.'.  */
  if (insn == NULL)
    {
      /* Restore the character we overwrite above (if any).  */ 
      if (save_c)
	*(--s) = save_c;

      /* Scan up to the first '.' or whitespace.  */
      for (s = str; *s != '\0' && *s != '.' && !isspace ((unsigned char) *s); ++s)
	continue;

      /* If we did not find a '.', then we can quit now.  */
      if (*s != '.')
	{
	  insn_error = "unrecognized opcode";
	  return;
	}

      /* Lookup the instruction in the hash table.  */
      *s++ = '\0';
      if ((insn = (struct ss_opcode *) hash_find (op_hash, str)) == NULL)
	{
	  insn_error = "unrecognized opcode";
	  return;
	}

      full_opcode_match = 0;
    }

  /* str now points to the opc with /0 after it.  save the opcode into
      opc.*/
  strcpy(opc,str);

  /* If we found a '/', then there are annotations next. But first we need
     to restore the '/' */
  insn_flags = 0;
  if (save_c=='/')
    {
      *(--s) = save_c;
      /* parse any instruction flags */
      s = ParseInsnFlags(s);
    }

  argsStart = s;
  for (;;)
    {
      assert (strcmp (insn->name, opc) == 0);

      ip->insn_mo = insn;
      ip->insn_opcode.a = (insn_flags << 16) | insn->match;
      ip->insn_opcode.b = 0;
      for (args = insn->args;; ++args)
	{
	  if (*s == ' ')
	    ++s;
	  switch (*args)
	    {
	    case '\0':		/* end of args */
	      if (*s == '\0')
		return;
	      break;

	    case ',':
	      if (*s++ == *args)
		continue;
	      s--;
	      switch (*++args)
		{
		case 'r':
		case 'v':
		  ip->insn_opcode.b |= lastregno << 24;
		  continue;

		case 'w':
		case 'W':
		  ip->insn_opcode.b |= lastregno << 16;
		  continue;

		case 'V':
		  ip->insn_opcode.b |= lastregno << 8;
		  continue;
		}
	      break;

	    case '(':
	      /* Handle optional base register.
		 Either the base register is omitted or
		 we must have a left paren. */
	      /* This is dependent on the next operand specifier
		 is a base register specification.  */
	      assert (args[1] == 'b' || args[1] == '5'
		      || args[1] == '-' || args[1] == '4');
	      if (*s == '\0')
		return;

	    case ')':		/* these must match exactly */
	      if (*s++ == *args)
		continue;
	      break;

	    case '<':		/* must be at least one digit */
	      /*
	       * According to the manual, if the shift amount is greater
	       * than 31 or less than 0 the the shift amount should be
	       * mod 32. In reality the mips assembler issues an error.
	       * We issue a warning and mask out all but the low 5 bits.
	       */
	      my_getExpression (&imm_expr, s);
	      check_absolute_expr (ip, &imm_expr);
	      if ((unsigned long) imm_expr.X_add_number > 31)
		{
		  as_warn (_("Improper shift amount (%ld)"),
			   (long) imm_expr.X_add_number);
		  imm_expr.X_add_number = imm_expr.X_add_number & 0x1f;
		}
	      ip->insn_opcode.b |= imm_expr.X_add_number;
	      imm_expr.X_op = O_absent;
	      s = expr_end;
	      continue;

	    case 'c':		/* break code */
	      my_getExpression (&imm_expr, s);
	      check_absolute_expr (ip, &imm_expr);
	      if ((unsigned) imm_expr.X_add_number > 1023)
		{
		  as_warn (_("Illegal break code (%ld)"),
			   (long) imm_expr.X_add_number);
		  imm_expr.X_add_number &= 0x3ff;
		}
	      ip->insn_opcode.b |= (imm_expr.X_add_number && 0xfffff);
	      imm_expr.X_op = O_absent;
	      s = expr_end;
	      continue;

	    case 'b':		/* base register */
	    case 'd':		/* destination register */
	    case 's':		/* source register */
	    case 't':		/* target register */
	    case 'r':		/* both target and source */
	    case 'v':		/* both dest and source */
	    case 'w':		/* both dest and target */
	    case 'E':		/* coprocessor target register */
	    case 'G':		/* coprocessor destination register */
	    case 'x':		/* ignore register name */
	    case 'z':		/* must be zero register */
	      s_reset = s;
	      if (s[0] == '$')
		{

		  if (isdigit ((unsigned char) s[1]))
		    {
		      ++s;
		      regno = 0;
		      do
			{
			  regno *= 10;
			  regno += *s - '0';
			  ++s;
			}
		      while (isdigit ((unsigned char) *s));
		      if (regno > 255)
			as_bad (_("Invalid register number (%d)"), regno);
		    }
		  else if (*args == 'E' || *args == 'G')
		    goto notreg;
		  else
		    {
		      if (s[1] == 'f' && s[2] == 'p')
			{
			  s += 3;
			  regno = FP;
			}
		      else if (s[1] == 's' && s[2] == 'p')
			{
			  s += 3;
			  regno = SP;
			}
		      else if (s[1] == 'g' && s[2] == 'p')
			{
			  s += 3;
			  regno = GP;
			}
		      else if (s[1] == 'a' && s[2] == 't')
			{
			  s += 3;
			  regno = AT;
			}
		      else if (s[1] == 'k' && s[2] == 't' && s[3] == '0')
			{
			  s += 4;
			  regno = KT0;
			}
		      else if (s[1] == 'k' && s[2] == 't' && s[3] == '1')
			{
			  s += 4;
			  regno = KT1;
			}
		      else
			goto notreg;
		    }
		  if (regno == AT
		      && ! ss_opts.noat
		      && *args != 'E'
		      && *args != 'G')
		    as_warn (_("Used $at without \".set noat\""));
		  c = *args;
		  if (*s == ' ')
		    s++;
		  if (args[1] != *s)
		    {
		      if (c == 'r' || c == 'v' || c == 'w')
			{
			  regno = lastregno;
			  s = s_reset;
			  args++;
			}
		    }
		  /* 'z' only matches $0.  */
		  if (c == 'z' && regno != 0)
		    break;

	/* Now that we have assembled one operand, we use the args string 
	 * to figure out where it goes in the instruction. */
		  switch (c)
		    {
		    case 'r':
		    case 's':
		    case 'v':
		    case 'b':
		      ip->insn_opcode.b |= regno << 24;
		      break;
		    case 'd':
		    case 'G':
		      ip->insn_opcode.b |= regno << 8;
		      break;
		    case 't':
		      if ((regno & 1) != 0
			  && (strcmp (opc, "dlw") == 0 ||
			      strcmp (opc, "dsw") == 0 ||
			      strcmp (opc, "dmtc1") == 0 ||
			      strcmp (opc, "dmfc1") == 0))
			{
			  as_warn ("Integer register should be even, was %d",
				   regno);
			  regno &= ~1;
			}
		      /* FALL THROUGH */
		    case 'w':
		    case 'E':
		      ip->insn_opcode.b |= regno << 16;
		      break;
		    case 'x':
		      /* This case exists because on the r3000 trunc
			 expands into a macro which requires a gp
			 register.  On the r6000 or r4000 it is
			 assembled into a single instruction which
			 ignores the register.  Thus the insn version
			 is MIPS_ISA2 and uses 'x', and the macro
			 version is MIPS_ISA1 and uses 't'.  */
		      break;
		    case 'z':
		      /* This case is for the div instruction, which
			 acts differently if the destination argument
			 is $0.  This only matches $0, and is checked
			 outside the switch.  */
		      break;
		    case 'D':
		      /* Itbl operand; not yet implemented. FIXME ?? */
		      break;
		      /* What about all other operands like 'i', which
			 can be specified in the opcode table? */
		    }
		  lastregno = regno;
		  continue;
		}
	    notreg:
	      switch (*args++)
		{
		case 'r':
		case 'v':
		  ip->insn_opcode.b |= lastregno << 24;
		  continue;
		case 'w':
		  ip->insn_opcode.b |= lastregno << 16;
		  continue;
		}
	      break;

	    case 'D':		/* floating point destination register */
	    case 'S':		/* floating point source register */
	    case 'T':		/* floating point target register */
	    case 'R':		/* floating point source register */
	    case 'V':
	    case 'W':
	      s_reset = s;
	      if (s[0] == '$' && s[1] == 'f' && isdigit ((unsigned char) s[2]))
		{
		  s += 2;
		  regno = 0;
		  do
		    {
		      regno *= 10;
		      regno += *s - '0';
		      ++s;
		    }
		  while (isdigit ((unsigned char) *s));

		  if (regno > 255)
		    as_bad (_("Invalid float register number (%d)"), regno);

		  if ((regno & 1) != 0
		      && ! (strcmp (opc, "mtc1") == 0
			    || strcmp (opc, "mfc1") == 0
			    || strcmp (opc, "l.s") == 0
			    || strcmp (opc, "s.s") == 0))
		    as_warn (_("Float register should be even, was %d"),
			     regno);

		  c = *args;
		  if (*s == ' ')
		    s++;
		  if (args[1] != *s)
		    {
		      if (c == 'V' || c == 'W')
			{
			  regno = lastregno;
			  s = s_reset;
			  args++;
			}
		    }
		  switch (c)
		    {
		    case 'D':
		      ip->insn_opcode.b |= regno << 8;
		      break;
		    case 'V':
		    case 'S':
		      ip->insn_opcode.b |= regno << 24;
		      break;
		    case 'W':
		    case 'T':
		      ip->insn_opcode.b |= regno << 16;
		      break;
		    }
		  lastregno = regno;
		  continue;
		}


	      switch (*args++)
		{
		case 'V':
		  ip->insn_opcode.b |= lastregno << 24;
		  continue;
		case 'W':
		  ip->insn_opcode.b |= lastregno << 16;
		  continue;
		}
	      break;

	    case 'I':
	      my_getExpression (&imm_expr, s);
	      if (imm_expr.X_op != O_big
		  && imm_expr.X_op != O_constant)
		insn_error = _("absolute expression required");
	      s = expr_end;
	      continue;

	    case 'A':
	      my_getExpression (&offset_expr, s);
	      imm_reloc = BFD_RELOC_32;
	      s = expr_end;
	      continue;

	    case 'F':
	    case 'L':
	    case 'f':
	    case 'l':
	      {
		int f64;
		char *save_in;
		char *err;
		unsigned char temp[8];
		int len;
		unsigned int length;
		segT seg;
		subsegT subseg;
		char *p;

		/* These only appear as the last operand in an
		   instruction, and every instruction that accepts
		   them in any variant accepts them in all variants.
		   This means we don't have to worry about backing out
		   any changes if the instruction does not match.

		   The difference between them is the size of the
		   floating point constant and where it goes.  For 'F'
		   and 'L' the constant is 64 bits; for 'f' and 'l' it
		   is 32 bits.  Where the constant is placed is based
		   on how the MIPS assembler does things:
		    F -- .rdata
		    L -- .lit8
		    f -- immediate value
		    l -- .lit4

		    The .lit4 and .lit8 sections are only used if
		    permitted by the -G argument.

		    When generating embedded PIC code, we use the
		    .lit8 section but not the .lit4 section (we can do
		    .lit4 inline easily; we need to put .lit8
		    somewhere in the data segment, and using .lit8
		    permits the linker to eventually combine identical
		    .lit8 entries).  */

		f64 = *args == 'F' || *args == 'L';

		save_in = input_line_pointer;
		input_line_pointer = s;
		err = md_atof (f64 ? 'd' : 'f', (char *) temp, &len);
		length = len;
		s = input_line_pointer;
		input_line_pointer = save_in;
		if (err != NULL && *err != '\0')
		  {
		    as_bad (_("Bad floating point constant: %s"), err);
		    memset (temp, '\0', sizeof temp);
		    length = f64 ? 8 : 4;
		  }

		assert (length == (f64 ? 8 : 4));

		if (*args == 'f'
		    || (*args == 'l'
			&& (! USE_GLOBAL_POINTER_OPT
			    || g_switch_value < 4
			    || (temp[0] == 0 && temp[1] == 0)
			    || (temp[2] == 0 && temp[3] == 0))))
		  {
		    imm_expr.X_op = O_constant;
		    if (! target_big_endian)
		      imm_expr.X_add_number = bfd_getl32 (temp);
		    else
		      imm_expr.X_add_number = bfd_getb32 (temp);
		  }
		else
		  {
		    const char *newname;
		    segT new_seg;

		    /* Switch to the right section.  */
		    seg = now_seg;
		    subseg = now_subseg;
		    switch (*args)
		      {
		      default: /* unused default case avoids warnings.  */
		      case 'L':
			newname = RDATA_SECTION_NAME;
			if (USE_GLOBAL_POINTER_OPT && g_switch_value >= 8)
			  newname = ".lit8";
			break;
		      case 'F':
			newname = RDATA_SECTION_NAME;
			break;
		      case 'l':
			assert (!USE_GLOBAL_POINTER_OPT
				|| g_switch_value >= 4);
			newname = ".lit4";
			break;
		      }
		    new_seg = subseg_new (newname, (subsegT) 0);
		    if (OUTPUT_FLAVOR == bfd_target_elf_flavour)
		      bfd_set_section_flags (stdoutput, new_seg,
					     (SEC_ALLOC
					      | SEC_LOAD
					      | SEC_READONLY
					      | SEC_DATA));
		    frag_align (*args == 'l' ? 2 : 3, 0, 0);
		    if (OUTPUT_FLAVOR == bfd_target_elf_flavour
			&& strcmp (TARGET_OS, "elf") != 0)
		      record_alignment (new_seg, 4);
		    else
		      record_alignment (new_seg, *args == 'l' ? 2 : 3);
		    if (seg == now_seg)
		      as_bad (_("Can't use floating point insn in this section"));

		    /* Set the argument to the current address in the
		       section.  */
		    offset_expr.X_op = O_symbol;
		    offset_expr.X_add_symbol =
		      symbol_new ("L0\001", now_seg,
				  (valueT) frag_now_fix (), frag_now);
		    offset_expr.X_add_number = 0;

		    /* Put the floating point number into the section.  */
		    p = frag_more ((int) length);
		    memcpy (p, temp, length);

		    /* Switch back to the original section.  */
		    subseg_set (seg, subseg);
		  }
	      }
	      continue;

	    case 'i':		/* 16 bit unsigned immediate */
	    case 'j':		/* 16 bit signed immediate */
	      imm_reloc = BFD_RELOC_LO16;
	      c = my_getSmallExpression (&imm_expr, s);
	      if (c != '\0')
		{
		  if (c != 'l')
		    {
		      if (imm_expr.X_op == O_constant)
			imm_expr.X_add_number =
			  (imm_expr.X_add_number >> 16) & 0xffff;
		      else if (c == 'h')
			{
			  imm_reloc = BFD_RELOC_HI16_S;
			  imm_unmatched_hi = true;
			}
		      else
			imm_reloc = BFD_RELOC_HI16;
		    }
		  else if (imm_expr.X_op == O_constant)
		    imm_expr.X_add_number &= 0xffff;
		}
	      if (*args == 'i')
		{
		  if ((c == '\0' && imm_expr.X_op != O_constant)
		      || ((imm_expr.X_add_number < 0
                           || imm_expr.X_add_number >= 0x10000)
                          && imm_expr.X_op == O_constant))
		    {
		      if (insn + 1 < &ss_opcodes[NUMOPCODES] &&
			  !strcmp (insn->name, insn[1].name))
			break;
		      if (imm_expr.X_op != O_constant
			  && imm_expr.X_op != O_big)
			insn_error = _("absolute expression required");
		      else
			as_bad (_("16 bit expression not in range 0..65535"));
		    }
		}
	      else
		{
		  int more;
		  offsetT max;

		  /* The upper bound should be 0x8000, but
		     unfortunately the MIPS assembler accepts numbers
		     from 0x8000 to 0xffff and sign extends them, and
		     we want to be compatible.  We only permit this
		     extended range for an instruction which does not
		     provide any further alternates, since those
		     alternates may handle other cases.  People should
		     use the numbers they mean, rather than relying on
		     a mysterious sign extension.  */
		  more = (insn + 1 < &ss_opcodes[NUMOPCODES] &&
			  strcmp (insn->name, insn[1].name) == 0);
		  if (more)
		    max = 0x8000;
		  else
		    max = 0x10000;
		  if ((c == '\0' && imm_expr.X_op != O_constant)
		      || ((imm_expr.X_add_number < -0x8000
                           || imm_expr.X_add_number >= max)
                          && imm_expr.X_op == O_constant)
		      || (more
			  && imm_expr.X_add_number < 0
			  && ss_opts.isa >= 3
			  && imm_expr.X_unsigned
			  && sizeof (imm_expr.X_add_number) <= 4))
		    {
		      if (more)
			break;
		      if (imm_expr.X_op != O_constant
			  && imm_expr.X_op != O_big)
			insn_error = _("absolute expression required");
		      else
			as_bad (_("16 bit expression not in range -32768..32767"));
		    }
		}
	      s = expr_end;
	      continue;

	    case 'o':		/* 16 bit offset */
	      c = my_getSmallExpression (&offset_expr, s);

	      /* If this value won't fit into a 16 bit offset, then go
		 find a macro that will generate the 32 bit offset
		 code pattern.  As a special hack, we accept the
		 difference of two local symbols as a constant.  This
		 is required to suppose embedded PIC switches, which
		 use an instruction which looks like
		     lw $4,$L12-$LS12($4)
		 The problem with handling this in a more general
		 fashion is that the macro function doesn't expect to
		 see anything which can be handled in a single
		 constant instruction.  */
	      if (c == 0
		  && (offset_expr.X_op != O_constant
		      || offset_expr.X_add_number >= 0x8000
		      || offset_expr.X_add_number < -0x8000)
		  && (ss_pic != EMBEDDED_PIC
		      || offset_expr.X_op != O_subtract
		      || now_seg != text_section
		      || (S_GET_SEGMENT (offset_expr.X_op_symbol)
			  != text_section)))
		break;

	      if (c == 'h' || c == 'H')
		{
		  if (offset_expr.X_op != O_constant)
		    break;
		  offset_expr.X_add_number =
		    (offset_expr.X_add_number >> 16) & 0xffff;
		}
	      offset_reloc = BFD_RELOC_LO16;
	      s = expr_end;
	      continue;

	    case 'p':		/* pc relative offset */
	      offset_reloc = BFD_RELOC_16_PCREL_S2;
	      my_getExpression (&offset_expr, s);
	      s = expr_end;
	      continue;

	    case 'u':		/* upper 16 bits */
	      c = my_getSmallExpression (&imm_expr, s);
	      imm_reloc = BFD_RELOC_LO16;
	      if (c)
		{
		  if (c != 'l')
		    {
		      if (imm_expr.X_op == O_constant)
			imm_expr.X_add_number =
			  (imm_expr.X_add_number >> 16) & 0xffff;
		      else if (c == 'h')
			{
			  imm_reloc = BFD_RELOC_HI16_S;
			  imm_unmatched_hi = true;
			}
		      else
			imm_reloc = BFD_RELOC_HI16;
		    }
		  else if (imm_expr.X_op == O_constant)
		    imm_expr.X_add_number &= 0xffff;
		}
	      if (imm_expr.X_op == O_constant
		  && (imm_expr.X_add_number < 0
		      || imm_expr.X_add_number >= 0x10000))
		as_bad (_("lui expression not in range 0..65535"));
	      s = expr_end;
	      continue;

	    case 'a':		/* 26 bit address */
	      my_getExpression (&offset_expr, s);
	      s = expr_end;
	      offset_reloc = BFD_RELOC_MIPS_JMP;
	      continue;

	    case '+':           /* these must match exactly */
            case '-':
            case '^':
              if (*s++ == *args)
                continue;
              break;

	    case 'N':		/* 3 bit branch condition code */
	    case 'M':		/* 3 bit compare condition code */
	      assert(0); /* don't think SS uses */
	      /*	      if (strncmp (s, "$fcc", 4) != 0)
		break;
	      s += 4;
	      regno = 0;
	      do
		{
		  regno *= 10;
		  regno += *s - '0';
		  ++s;
		}
	      while (isdigit ((unsigned char) *s));
	      if (regno > 7)
		as_bad (_("invalid condition code register $fcc%d"), regno);
	      if (*args == 'N')
		ip->insn_opcode |= regno << OP_SH_BCC;
	      else
		ip->insn_opcode |= regno << OP_SH_CCC;
              continue;
	      */
	    default:
	      as_bad (_("bad char = '%c'\n"), *args);
	      internalError ();
	    }
	  break;
	}
      /* Args don't match.  */
      if (insn + 1 < &ss_opcodes[NUMOPCODES] &&
	  !strcmp (insn->name, insn[1].name))
	{
	  ++insn;
	  s = argsStart;
	  continue;
	}
      insn_error = _("illegal operands");
      return;
    }
}

#define LP '('
#define RP ')'

static int
my_getSmallExpression (ep, str)
     expressionS *ep;
     char *str;
{
  char *sp;
  int c = 0;

  if (*str == ' ')
    str++;
  if (*str == LP
      || (*str == '%' &&
	  ((str[1] == 'h' && str[2] == 'i')
	   || (str[1] == 'H' && str[2] == 'I')
	   || (str[1] == 'l' && str[2] == 'o'))
	  && str[3] == LP))
    {
      if (*str == LP)
	c = 0;
      else
	{
	  c = str[1];
	  str += 3;
	}

      /*
       * A small expression may be followed by a base register.
       * Scan to the end of this operand, and then back over a possible
       * base register.  Then scan the small expression up to that
       * point.  (Based on code in sparc.c...)
       */
      for (sp = str; *sp && *sp != ','; sp++)
	;
      if (sp - 4 >= str && sp[-1] == RP)
	{
	  if (isdigit ((unsigned char) sp[-2]))
	    {
	      for (sp -= 3; sp >= str && isdigit ((unsigned char) *sp); sp--)
		;
	      if (*sp == '$' && sp > str && sp[-1] == LP)
		{
		  sp--;
		  goto do_it;
		}
	    }
	  else if (sp - 5 >= str
		   && sp[-5] == LP
		   && sp[-4] == '$'
		   && ((sp[-3] == 'f' && sp[-2] == 'p')
		       || (sp[-3] == 's' && sp[-2] == 'p')
		       || (sp[-3] == 'g' && sp[-2] == 'p')
		       || (sp[-3] == 'a' && sp[-2] == 't')))
	    {
	      sp -= 5;
	    do_it:
	      if (sp == str)
		{
		  /* no expression means zero offset */
		  if (c)
		    {
		      /* %xx(reg) is an error */
		      ep->X_op = O_absent;
		      expr_end = str - 3;
		    }
		  else
		    {
		      ep->X_op = O_constant;
		      expr_end = sp;
		    }
		  ep->X_add_symbol = NULL;
		  ep->X_op_symbol = NULL;
		  ep->X_add_number = 0;
		}
	      else
		{
		  *sp = '\0';
		  my_getExpression (ep, str);
		  *sp = LP;
		}
	      return c;
	    }
	}
    }
  my_getExpression (ep, str);
  return c;			/* => %hi or %lo encountered */
}

static void
my_getExpression (ep, str)
     expressionS *ep;
     char *str;
{
  char *save_in;

  save_in = input_line_pointer;
  input_line_pointer = str;
  expression (ep);
  expr_end = input_line_pointer;
  input_line_pointer = save_in;
}

/* Turn a string in input_line_pointer into a floating point constant
   of type type, and store the appropriate bytes in *litP.  The number
   of LITTLENUMS emitted is stored in *sizeP .  An error message is
   returned, or NULL on OK.  */

char *
md_atof (type, litP, sizeP)
     int type;
     char *litP;
     int *sizeP;
{
  int prec;
  LITTLENUM_TYPE words[4];
  char *t;
  int i;

  switch (type)
    {
    case 'f':
      prec = 2;
      break;

    case 'd':
      prec = 4;
      break;

    default:
      *sizeP = 0;
      return _("bad call to md_atof");
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizeP = prec * 2;

  if (! target_big_endian)
    {
      for (i = prec - 1; i >= 0; i--)
	{
	  md_number_to_chars (litP, (valueT) words[i], 2);
	  litP += 2;
	}
    }
  else
    {
      for (i = 0; i < prec; i++)
	{
	  md_number_to_chars (litP, (valueT) words[i], 2);
	  litP += 2;
	}
    }
     
  return NULL;
}

void
md_number_to_chars (buf, val, n)
     char *buf;
     valueT val;
     int n;
{
  if (target_big_endian)
    number_to_chars_bigendian (buf, val, n);
  else
    number_to_chars_littleendian (buf, val, n);
}

CONST char *md_shortopts = "O::g::G:";

struct option md_longopts[] = {
#define OPTION_SS1 (OPTION_MD_BASE + 1)
  {"ss0", no_argument, NULL, OPTION_SS1},
  {"ss1", no_argument, NULL, OPTION_SS1},
#define OPTION_SS2 (OPTION_MD_BASE + 2)
  {"ss2", no_argument, NULL, OPTION_SS2},
#define OPTION_SS3 (OPTION_MD_BASE + 3)
  {"ss3", no_argument, NULL, OPTION_SS3},
#define OPTION_SS4 (OPTION_MD_BASE + 4)
  {"ss4", no_argument, NULL, OPTION_SS4},
#define OPTION_MCPU (OPTION_MD_BASE + 5)
  {"mcpu", required_argument, NULL, OPTION_MCPU},
#define OPTION_MEMBEDDED_PIC (OPTION_MD_BASE + 6)
  {"membedded-pic", no_argument, NULL, OPTION_MEMBEDDED_PIC},
#define OPTION_TRAP (OPTION_MD_BASE + 9)
  {"trap", no_argument, NULL, OPTION_TRAP},
  {"no-break", no_argument, NULL, OPTION_TRAP},
#define OPTION_BREAK (OPTION_MD_BASE + 10)
  {"break", no_argument, NULL, OPTION_BREAK},
  {"no-trap", no_argument, NULL, OPTION_BREAK},
#define OPTION_EB (OPTION_MD_BASE + 11)
  {"EB", no_argument, NULL, OPTION_EB},
#define OPTION_EL (OPTION_MD_BASE + 12)
  {"EL", no_argument, NULL, OPTION_EL},

#define OPTION_MABI (OPTION_MD_BASE + 38)
  {"mabi", required_argument, NULL, OPTION_MABI},

#define OPTION_CALL_SHARED (OPTION_MD_BASE + 7)
#define OPTION_NON_SHARED (OPTION_MD_BASE + 8)
#define OPTION_XGOT (OPTION_MD_BASE + 19)
#define OPTION_32 (OPTION_MD_BASE + 20)
#define OPTION_64 (OPTION_MD_BASE + 21)
#ifdef OBJ_ELF
  {"KPIC", no_argument, NULL, OPTION_CALL_SHARED},
  {"xgot", no_argument, NULL, OPTION_XGOT},
  {"call_shared", no_argument, NULL, OPTION_CALL_SHARED},
  {"non_shared", no_argument, NULL, OPTION_NON_SHARED},
  {"32", no_argument, NULL, OPTION_32},
  {"64", no_argument, NULL, OPTION_64},
#endif

  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof(md_longopts);

int
md_parse_option (c, arg)
     int c;
     char *arg;
{
  switch (c)
    {
    case OPTION_EB:
      target_big_endian = 1;
      break;

    case OPTION_EL:
      target_big_endian = 0;
      break;

    case 'O':
      /* ignored, always assemble the same way */
      break;

    case 'g':
      /* ignored, always assemble the same way */
      break;

      /* The -xgot option tells the assembler to use 32 offsets when
         accessing the got in SVR4_PIC mode.  It is for Irix
         compatibility.  */
    case OPTION_XGOT:
      ss_big_got = 1;
      break;

    case 'G':
      if (! USE_GLOBAL_POINTER_OPT)
	{
	  as_bad (_("-G is not supported for this configuration"));
	  return 0;
	}
      else if (ss_pic == SVR4_PIC || ss_pic == EMBEDDED_PIC)
	{
	  as_bad (_("-G may not be used with SVR4 or embedded PIC code"));
	  return 0;
	}
      else
	g_switch_value = atoi (arg);
      g_switch_seen = 1;
      break;

      /* The -32 and -64 options tell the assembler to output the 32
         bit or the 64 bit MIPS ELF format.  */
    case OPTION_32:
      ss_64 = 0;
      break;

    case OPTION_64:
      {
	const char **list, **l;

	list = bfd_target_list ();
	for (l = list; *l != NULL; l++)
	  if (strcmp (*l, "elf64-bigss") == 0
	      || strcmp (*l, "elf64-littless") == 0)
	    break;
	if (*l == NULL)
	  as_fatal (_("No compiled in support for 64 bit object file format"));
	free (list);
	ss_64 = 1;
      }
      break;


    case OPTION_MABI:
      if (strcmp (arg,"32") == 0
	  || strcmp (arg,"n32") == 0
	  || strcmp (arg,"64") == 0
	  || strcmp (arg,"o64") == 0
	  || strcmp (arg,"eabi") == 0)
	ss_abi_string = arg;
      break;

    default:
      return 0;
    }

  return 1;
}


static void
show (stream, string, col_p, first_p)
     FILE *stream;
     char *string;
     int *col_p;
     int *first_p;
{
  if (*first_p)
    {
      fprintf (stream, "%24s", "");
      *col_p = 24;
    }
  else
    {
      fprintf (stream, ", ");
      *col_p += 2;
    }

  if (*col_p + strlen (string) > 72)
    {
      fprintf (stream, "\n%24s", "");
      *col_p = 24;
    }

  fprintf (stream, "%s", string);
  *col_p += strlen (string);

  *first_p = 0;
}


void
md_show_usage (stream)
     FILE *stream;
{
  fprintf(stream, _("\
SS options:\n\
-membedded-pic		generate embedded position independent code\n\
-EB			generate big endian output\n\
-EL			generate little endian output\n\
-g, -g2			do not remove uneeded NOPs or swap branches\n\
-G NUM			allow referencing objects up to NUM bytes\n\
			implicitly with the gp register [default 8]\n"));
#ifdef OBJ_ELF
  fprintf(stream, _("\
-KPIC, -call_shared	generate SVR4 position independent code\n\
-non_shared		do not generate position independent code\n\
-xgot			assume a 32 bit GOT\n\
-32			create 32 bit object file (default)\n\
-64			create 64 bit object file\n"));
#endif
}

void
ss_init_after_args ()
{
  /* initialize opcodes */
  bfd_ss_num_opcodes = bfd_ss_num_builtin_opcodes;
  ss_opcodes = (struct ss_opcode*) ss_builtin_opcodes;
}

long
md_pcrel_from (fixP)
     fixS *fixP;
{
  if (OUTPUT_FLAVOR != bfd_target_aout_flavour
      && fixP->fx_addsy != (symbolS *) NULL
      && ! S_IS_DEFINED (fixP->fx_addsy))
    {
      /* This makes a branch to an undefined symbol be a branch to the
	 current location.  */
      return 4;
    }

  /* return the address of the delay slot */
  return fixP->fx_size + fixP->fx_where + fixP->fx_frag->fr_address;
}

/* This is called by emit_expr via TC_CONS_FIX_NEW when creating a
   reloc for a cons.  We could use the definition there, except that
   we want to handle 64 bit relocs specially.  */

void
cons_fix_new_ss (frag, where, nbytes, exp)
     fragS *frag;
     int where;
     unsigned int nbytes;
     expressionS *exp;
{
#ifndef OBJ_ELF
  /* If we are assembling in 32 bit mode, turn an 8 byte reloc into a
     4 byte reloc.  */
  if (nbytes == 8 && ! ss_64)
    {
      if (target_big_endian)
	where += 4;
      nbytes = 4;
    }
#endif

  if (nbytes != 2 && nbytes != 4 && nbytes != 8)
    as_bad (_("Unsupported reloc size %d"), nbytes);

  fix_new_exp (frag_now, where, (int) nbytes, exp, 0,
	       (nbytes == 2
		? BFD_RELOC_16
		: (nbytes == 4 ? BFD_RELOC_32 : BFD_RELOC_64)));
}

/* This is called before the symbol table is processed.  In order to
   work with gcc when using mips-tfile, we must keep all local labels.
   However, in other cases, we want to discard them.  If we were
   called with -g, but we didn't see any debugging information, it may
   mean that gcc is smuggling debugging information through to
   mips-tfile, in which case we must generate all local labels.  */

void
ss_frob_file_before_adjust ()
{
#ifndef NO_ECOFF_DEBUGGING
  if (ECOFF_DEBUGGING
      && ss_debug != 0
      && ! ecoff_debugging_seen)
    flag_keep_locals = 1;
#endif
}

/* Sort any unmatched HI16_S relocs so that they immediately precede
   the corresponding LO reloc.  This is called before md_apply_fix and
   tc_gen_reloc.  Unmatched HI16_S relocs can only be generated by
   explicit use of the %hi modifier.  */

void
ss_frob_file ()
{
  struct ss_hi_fixup *l;

  for (l = ss_hi_fixup_list; l != NULL; l = l->next)
    {
      segment_info_type *seginfo;
      int pass;

      assert (l->fixp->fx_r_type == BFD_RELOC_HI16_S);

      /* Check quickly whether the next fixup happens to be a matching
         %lo.  */
      if (l->fixp->fx_next != NULL
	  && l->fixp->fx_next->fx_r_type == BFD_RELOC_LO16
	  && l->fixp->fx_addsy == l->fixp->fx_next->fx_addsy
	  && l->fixp->fx_offset == l->fixp->fx_next->fx_offset)
	continue;

      /* Look through the fixups for this segment for a matching %lo.
         When we find one, move the %hi just in front of it.  We do
         this in two passes.  In the first pass, we try to find a
         unique %lo.  In the second pass, we permit multiple %hi
         relocs for a single %lo (this is a GNU extension).  */
      seginfo = seg_info (l->seg);
      for (pass = 0; pass < 2; pass++)
	{
	  fixS *f, *prev;

	  prev = NULL;
	  for (f = seginfo->fix_root; f != NULL; f = f->fx_next)
	    {
	      /* Check whether this is a %lo fixup which matches l->fixp.  */
	      if (f->fx_r_type == BFD_RELOC_LO16
		  && f->fx_addsy == l->fixp->fx_addsy
		  && f->fx_offset == l->fixp->fx_offset
		  && (pass == 1
		      || prev == NULL
		      || prev->fx_r_type != BFD_RELOC_HI16_S
		      || prev->fx_addsy != f->fx_addsy
		      || prev->fx_offset !=  f->fx_offset))
		{
		  fixS **pf;

		  /* Move l->fixp before f.  */
		  for (pf = &seginfo->fix_root;
		       *pf != l->fixp;
		       pf = &(*pf)->fx_next)
		    assert (*pf != NULL);

		  *pf = l->fixp->fx_next;

		  l->fixp->fx_next = f;
		  if (prev == NULL)
		    seginfo->fix_root = l->fixp;
		  else
		    prev->fx_next = l->fixp;

		  break;
		}

	      prev = f;
	    }

	  if (f != NULL)
	    break;

#if 0 /* GCC code motion plus incomplete dead code elimination
	 can leave a %hi without a %lo.  */
	  if (pass == 1)
	    as_warn_where (l->fixp->fx_file, l->fixp->fx_line,
			   _("Unmatched %%hi reloc"));
#endif
	}
    }
}

/* When generating embedded PIC code we need to use a special
   relocation to represent the difference of two symbols in the .text
   section (switch tables use a difference of this sort).  See
   include/coff/ss.h for details.  This macro checks whether this
   fixup requires the special reloc.  */
#define SWITCH_TABLE(fixp) \
  ((fixp)->fx_r_type == BFD_RELOC_32 \
   && (fixp)->fx_addsy != NULL \
   && (fixp)->fx_subsy != NULL \
   && S_GET_SEGMENT ((fixp)->fx_addsy) == text_section \
   && S_GET_SEGMENT ((fixp)->fx_subsy) == text_section)

/* When generating embedded PIC code we must keep all PC relative
   relocations, in case the linker has to relax a call.  We also need
   to keep relocations for switch table entries.  */

/*ARGSUSED*/
int
ss_force_relocation (fixp)
     fixS *fixp;
{
  if (fixp->fx_r_type == BFD_RELOC_VTABLE_INHERIT
      || fixp->fx_r_type == BFD_RELOC_VTABLE_ENTRY)
    return 1;

  return (ss_pic == EMBEDDED_PIC
	  && (fixp->fx_pcrel
	      || SWITCH_TABLE (fixp)
	      || fixp->fx_r_type == BFD_RELOC_PCREL_HI16_S
	      || fixp->fx_r_type == BFD_RELOC_PCREL_LO16));
}

/* Apply a fixup to the object file.  */

int
md_apply_fix (fixP, valueP)
     fixS *fixP;
     valueT *valueP;
{
  unsigned char *buf;
  long insn, value;

  assert (fixP->fx_size == 4
	  || fixP->fx_r_type == BFD_RELOC_16
	  || fixP->fx_r_type == BFD_RELOC_64
	  || fixP->fx_r_type == BFD_RELOC_VTABLE_INHERIT
	  || fixP->fx_r_type == BFD_RELOC_VTABLE_ENTRY);

  value = *valueP;

  /* If we aren't adjusting this fixup to be against the section
     symbol, we need to adjust the value.  */
#ifdef OBJ_ELF
  if (fixP->fx_addsy != NULL && OUTPUT_FLAVOR == bfd_target_elf_flavour)
    if (S_GET_OTHER (fixP->fx_addsy) == STO_MIPS16 
        || S_IS_WEAK (fixP->fx_addsy)
        || (symbol_used_in_reloc_p (fixP->fx_addsy)
            && (((bfd_get_section_flags (stdoutput,
					 S_GET_SEGMENT (fixP->fx_addsy))
		  & SEC_LINK_ONCE) != 0)
		|| !strncmp (segment_name (S_GET_SEGMENT (fixP->fx_addsy)),
			     ".gnu.linkonce",
			     sizeof (".gnu.linkonce") - 1))))

      {
        value -= S_GET_VALUE (fixP->fx_addsy);
        if (value != 0 && ! fixP->fx_pcrel)
          {
            /* In this case, the bfd_install_relocation routine will
               incorrectly add the symbol value back in.  We just want
               the addend to appear in the object file.  */
            value -= S_GET_VALUE (fixP->fx_addsy);
          }
      }
#endif


  fixP->fx_addnumber = value;	/* Remember value for tc_gen_reloc */

  if (fixP->fx_addsy == NULL && ! fixP->fx_pcrel)
    fixP->fx_done = 1;

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_MIPS_JMP:
    case BFD_RELOC_HI16:
    case BFD_RELOC_HI16_S:
    case BFD_RELOC_MIPS_GPREL:
    case BFD_RELOC_MIPS_LITERAL:
    case BFD_RELOC_MIPS_CALL16:
    case BFD_RELOC_MIPS_GOT16:
    case BFD_RELOC_MIPS_GPREL32:
    case BFD_RELOC_MIPS_GOT_HI16:
    case BFD_RELOC_MIPS_GOT_LO16:
    case BFD_RELOC_MIPS_CALL_HI16:
    case BFD_RELOC_MIPS_CALL_LO16:
    case BFD_RELOC_MIPS16_GPREL:
      if (fixP->fx_pcrel)
	as_bad_where (fixP->fx_file, fixP->fx_line,
		      _("Invalid PC relative reloc"));
      /* Nothing needed to do. The value comes from the reloc entry */
      break;

    case BFD_RELOC_MIPS16_JMP:
      /* We currently always generate a reloc against a symbol, which
         means that we don't want an addend even if the symbol is
         defined.  */
      fixP->fx_addnumber = 0;
      break;

    case BFD_RELOC_PCREL_HI16_S:
      /* The addend for this is tricky if it is internal, so we just
	 do everything here rather than in bfd_install_relocation.  */
      if ((symbol_get_bfdsym (fixP->fx_addsy)->flags & BSF_SECTION_SYM) == 0)
	{
	  /* For an external symbol adjust by the address to make it
	     pcrel_offset.  We use the address of the RELLO reloc
	     which follows this one.  */
	  value += (fixP->fx_next->fx_frag->fr_address
		    + fixP->fx_next->fx_where);
	}
      if (value & 0x8000)
	value += 0x10000;
      value >>= 16;
      buf = (unsigned char *) fixP->fx_frag->fr_literal + fixP->fx_where;
      if (target_big_endian)
	buf += 2;
      md_number_to_chars (buf, value, 2);
      break;

    case BFD_RELOC_PCREL_LO16:
      /* The addend for this is tricky if it is internal, so we just
	 do everything here rather than in bfd_install_relocation.  */
      if ((symbol_get_bfdsym (fixP->fx_addsy)->flags & BSF_SECTION_SYM) == 0)
	value += fixP->fx_frag->fr_address + fixP->fx_where;
      buf = (unsigned char *) fixP->fx_frag->fr_literal + fixP->fx_where;
      if (target_big_endian)
	buf += 2;
      md_number_to_chars (buf, value, 2);
      break;

    case BFD_RELOC_64:
      /* This is handled like BFD_RELOC_32, but we output a sign
         extended value if we are only 32 bits.  */
      if (fixP->fx_done
	  || (ss_pic == EMBEDDED_PIC && SWITCH_TABLE (fixP)))
	{
	  if (8 <= sizeof (valueT))
	    md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
				value, 8);
	  else
	    {
	      long w1, w2;
	      long hiv;

	      w1 = w2 = fixP->fx_where;
	      if (target_big_endian)
		w1 += 4;
	      else
		w2 += 4;
	      md_number_to_chars (fixP->fx_frag->fr_literal + w1, value, 4);
	      if ((value & 0x80000000) != 0)
		hiv = 0xffffffff;
	      else
		hiv = 0;
	      md_number_to_chars (fixP->fx_frag->fr_literal + w2, hiv, 4);
	    }
	}
      break;

    case BFD_RELOC_32:
      /* If we are deleting this reloc entry, we must fill in the
	 value now.  This can happen if we have a .word which is not
	 resolved when it appears but is later defined.  We also need
	 to fill in the value if this is an embedded PIC switch table
	 entry.  */
      if (fixP->fx_done
	  || (ss_pic == EMBEDDED_PIC && SWITCH_TABLE (fixP)))
	md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
			    value, 4);
      break;

    case BFD_RELOC_16:
      /* If we are deleting this reloc entry, we must fill in the
         value now.  */
      assert (fixP->fx_size == 2);
      if (fixP->fx_done)
	md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
			    value, 2);
      break;

    case BFD_RELOC_LO16:
      /* When handling an embedded PIC switch statement, we can wind
	 up deleting a LO16 reloc.  See the 'o' case in ss_ip.  */
      if (fixP->fx_done)
	{
	  if (value < -0x8000 || value > 0x7fff)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("relocation overflow"));
	  buf = (unsigned char *) fixP->fx_frag->fr_literal + fixP->fx_where;
	  if (target_big_endian)
	    buf += 2;
	  md_number_to_chars (buf, value, 2);
	}
      break;

    case BFD_RELOC_16_PCREL_S2:
      /*
       * We need to save the bits in the instruction since fixup_segment()
       * might be deleting the relocation entry (i.e., a branch within
       * the current segment).
       */
      if ((value & 0x3) != 0)
	as_bad_where (fixP->fx_file, fixP->fx_line,
		      _("Branch to odd address (%lx)"), value);
      value >>= 2;

      /* update old instruction data */
      buf = (unsigned char *) (fixP->fx_where + fixP->fx_frag->fr_literal);
      if (target_big_endian)
	insn = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
      else
	insn = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];

      if (value >= -0x8000 && value < 0x8000)
	insn |= value & 0xffff;
      else
	{
	  /* The branch offset is too large.  If this is an
             unconditional branch, and we are not generating PIC code,
             we can convert it to an absolute jump instruction.  */
	  if (ss_pic == NO_PIC
	      && fixP->fx_done
	      && fixP->fx_frag->fr_address >= text_section->vma
	      && (fixP->fx_frag->fr_address
		  < text_section->vma + text_section->_raw_size)
	      && ((insn & 0xffff0000) == 0x10000000	 /* beq $0,$0 */
		  || (insn & 0xffff0000) == 0x04010000	 /* bgez $0 */
		  || (insn & 0xffff0000) == 0x04110000)) /* bgezal $0 */
	    {
	      if ((insn & 0xffff0000) == 0x04110000)	 /* bgezal $0 */
		insn = 0x0c000000;	/* jal */
	      else
		insn = 0x08000000;	/* j */
	      fixP->fx_r_type = BFD_RELOC_MIPS_JMP;
	      fixP->fx_done = 0;
	      fixP->fx_addsy = section_symbol (text_section);
	      fixP->fx_addnumber = (value << 2) + md_pcrel_from (fixP);
	    }
	  else
	    {
	      /* FIXME.  It would be possible in principle to handle
                 conditional branches which overflow.  They could be
                 transformed into a branch around a jump.  This would
                 require setting up variant frags for each different
                 branch type.  The native MIPS assembler attempts to
                 handle these cases, but it appears to do it
                 incorrectly.  */
	      as_bad_where (fixP->fx_file, fixP->fx_line,
			    _("Branch out of range"));
	    }
	}

      md_number_to_chars ((char *) buf, (valueT) insn, 4);
      break;

    case BFD_RELOC_VTABLE_INHERIT:
      fixP->fx_done = 0;
      if (fixP->fx_addsy
          && !S_IS_DEFINED (fixP->fx_addsy)
          && !S_IS_WEAK (fixP->fx_addsy))
        S_SET_WEAK (fixP->fx_addsy);
      break;

    case BFD_RELOC_VTABLE_ENTRY:
      fixP->fx_done = 0;
      break;

    default:
      internalError ();
    }

  return 1;
}

#if 0
void
printInsn (oc)
     unsigned long oc;
{
  const struct ss_opcode *p;
  int treg, sreg, dreg, shamt;
  short imm;
  const char *args;
  int i;

  for (i = 0; i < NUMOPCODES; ++i)
    {
      p = &ss_opcodes[i];
      if (((oc & p->mask) == p->match) && (p->pinfo != INSN_MACRO))
	{
	  printf ("%08lx %s\t", oc, p->name);
	  treg = (oc >> 16) & 0x1f;
	  sreg = (oc >> 21) & 0x1f;
	  dreg = (oc >> 11) & 0x1f;
	  shamt = (oc >> 6) & 0x1f;
	  imm = oc;
	  for (args = p->args;; ++args)
	    {
	      switch (*args)
		{
		case '\0':
		  printf ("\n");
		  break;

		case ',':
		case '(':
		case ')':
		  printf ("%c", *args);
		  continue;

		case 'r':
		  assert (treg == sreg);
		  printf ("$%d,$%d", treg, sreg);
		  continue;

		case 'd':
		case 'G':
		  printf ("$%d", dreg);
		  continue;

		case 't':
		case 'E':
		  printf ("$%d", treg);
		  continue;

		case 'k':
		  printf ("0x%x", treg);
		  continue;

		case 'b':
		case 's':
		  printf ("$%d", sreg);
		  continue;

		case 'a':
		  printf ("0x%08lx", oc & 0x1ffffff);
		  continue;

		case 'i':
		case 'j':
		case 'o':
		case 'u':
		  printf ("%d", imm);
		  continue;

		case '<':
		case '>':
		  printf ("$%d", shamt);
		  continue;

		default:
		  internalError ();
		}
	      break;
	    }
	  return;
	}
    }
  printf (_("%08lx  UNDEFINED\n"), oc);
}
#endif

static symbolS *
get_symbol ()
{
  int c;
  char *name;
  symbolS *p;

  name = input_line_pointer;
  c = get_symbol_end ();
  p = (symbolS *) symbol_find_or_make (name);
  *input_line_pointer = c;
  return p;
}

/* Align the current frag to a given power of two.  The MIPS assembler
   also automatically adjusts any preceding label.  */

static void
ss_align (to, fill, label)
     int to;
     int fill;
     symbolS *label;
{
  frag_align (to, fill, 0);
  record_alignment (now_seg, to);
}

/* Align to a given power of two.  .align 0 turns off the automatic
   alignment used by the data creating pseudo-ops.  */

static void
s_align (x)
     int x;
{
  register int temp;
  register long temp_fill;
  long max_alignment = 15;

  /*

    o  Note that the assembler pulls down any immediately preceeding label
       to the aligned address.
    o  It's not documented but auto alignment is reinstated by
       a .align pseudo instruction.
    o  Note also that after auto alignment is turned off the mips assembler
       issues an error on attempt to assemble an improperly aligned data item.
       We don't.

    */

  temp = get_absolute_expression ();
  if (temp > max_alignment)
    as_bad (_("Alignment too large: %d. assumed."), temp = max_alignment);
  else if (temp < 0)
    {
      as_warn (_("Alignment negative: 0 assumed."));
      temp = 0;
    }
  if (*input_line_pointer == ',')
    {
      input_line_pointer++;
      temp_fill = get_absolute_expression ();
    }
  else
    temp_fill = 0;
  if (temp)
    {
      auto_align = 1;
      ss_align (temp, (int) temp_fill,
		  insn_labels != NULL ? insn_labels->label : NULL);
    }
  else
    {
      auto_align = 0;
    }

  demand_empty_rest_of_line ();
}

void
ss_flush_pending_output ()
{
  ss_clear_insn_labels ();
}

static void
s_change_sec (sec)
     int sec;
{
  segT seg;

  /* When generating embedded PIC code, we only use the .text, .lit8,
     .sdata and .sbss sections.  We change the .data and .rdata
     pseudo-ops to use .sdata.  */
  if (ss_pic == EMBEDDED_PIC
      && (sec == 'd' || sec == 'r'))
    sec = 's';

#ifdef OBJ_ELF
  /* The ELF backend needs to know that we are changing sections, so
     that .previous works correctly.  We could do something like check
     for a obj_section_change_hook macro, but that might be confusing
     as it would not be appropriate to use it in the section changing
     functions in read.c, since obj-elf.c intercepts those.  FIXME:
     This should be cleaner, somehow.  */
  obj_elf_section_change_hook ();
#endif
  switch (sec)
    {
    case 't':
      s_text (0);
      break;
    case 'd':
      s_data (0);
      break;
    case 'b':
      subseg_set (bss_section, (subsegT) get_absolute_expression ());
      demand_empty_rest_of_line ();
      break;

    case 'r':
      if (USE_GLOBAL_POINTER_OPT)
	{
	  seg = subseg_new (RDATA_SECTION_NAME,
			    (subsegT) get_absolute_expression ());
	  if (OUTPUT_FLAVOR == bfd_target_elf_flavour)
	    {
	      bfd_set_section_flags (stdoutput, seg,
				     (SEC_ALLOC
				      | SEC_LOAD
				      | SEC_READONLY
				      | SEC_RELOC
				      | SEC_DATA));
	      if (strcmp (TARGET_OS, "elf") != 0)
		bfd_set_section_alignment (stdoutput, seg, 4);
	    }
	  demand_empty_rest_of_line ();
	}
      else
	{
	  as_bad (_("No read only data section in this object file format"));
	  demand_empty_rest_of_line ();
	  return;
	}
      break;

    case 's':
      if (USE_GLOBAL_POINTER_OPT)
	{
	  seg = subseg_new (".sdata", (subsegT) get_absolute_expression ());
	  if (OUTPUT_FLAVOR == bfd_target_elf_flavour)
	    {
	      bfd_set_section_flags (stdoutput, seg,
				     SEC_ALLOC | SEC_LOAD | SEC_RELOC
				     | SEC_DATA);
	      if (strcmp (TARGET_OS, "elf") != 0)
		bfd_set_section_alignment (stdoutput, seg, 4);
	    }
	  demand_empty_rest_of_line ();
	  break;
	}
      else
	{
	  as_bad (_("Global pointers not supported; recompile -G 0"));
	  demand_empty_rest_of_line ();
	  return;
	}
    }

  auto_align = 1;
}

void
ss_enable_auto_align ()
{
  auto_align = 1;
}

static void
s_cons (log_size)
     int log_size;
{
  symbolS *label;

  label = insn_labels != NULL ? insn_labels->label : NULL;
  if (log_size > 0 && auto_align)
    ss_align (log_size, 0, label);
  ss_clear_insn_labels ();
  cons (1 << log_size);
}

static void
s_float_cons (type)
     int type;
{
  symbolS *label;

  label = insn_labels != NULL ? insn_labels->label : NULL;

  if (auto_align)
    {
      if (type == 'd')
	ss_align (3, 0, label);
      else
	ss_align (2, 0, label);
    }

  ss_clear_insn_labels ();

  float_cons (type);
}

/* Handle .globl.  We need to override it because on Irix 5 you are
   permitted to say
       .globl foo .text
   where foo is an undefined symbol, to mean that foo should be
   considered to be the address of a function.  */

static void
s_ss_globl (x)
     int x;
{
  char *name;
  int c;
  symbolS *symbolP;
  flagword flag;

  name = input_line_pointer;
  c = get_symbol_end ();
  symbolP = symbol_find_or_make (name);
  *input_line_pointer = c;
  SKIP_WHITESPACE ();

  /* On Irix 5, every global symbol that is not explicitly labelled as
     being a function is apparently labelled as being an object.  */
  flag = BSF_OBJECT;

  if (! is_end_of_line[(unsigned char) *input_line_pointer])
    {
      char *secname;
      asection *sec;

      secname = input_line_pointer;
      c = get_symbol_end ();
      sec = bfd_get_section_by_name (stdoutput, secname);
      if (sec == NULL)
	as_bad (_("%s: no such section"), secname);
      *input_line_pointer = c;

      if (sec != NULL && (sec->flags & SEC_CODE) != 0)
	flag = BSF_FUNCTION;
    }

  symbol_get_bfdsym (symbolP)->flags |= flag;

  S_SET_EXTERNAL (symbolP);
  demand_empty_rest_of_line ();
}

static void
s_option (x)
     int x;
{
  char *opt;
  char c;

  opt = input_line_pointer;
  c = get_symbol_end ();

  if (*opt == 'O')
    {
      /* FIXME: What does this mean?  */
    }
  else if (strncmp (opt, "pic", 3) == 0)
    {
      int i;

      i = atoi (opt + 3);
      if (i == 0)
	ss_pic = NO_PIC;
      else if (i == 2)
	ss_pic = SVR4_PIC;
      else
	as_bad (_(".option pic%d not supported"), i);

      if (USE_GLOBAL_POINTER_OPT && ss_pic == SVR4_PIC)
	{
	  if (g_switch_seen && g_switch_value != 0)
	    as_warn (_("-G may not be used with SVR4 PIC code"));
	  g_switch_value = 0;
	  bfd_set_gp_size (stdoutput, 0);
	}
    }
  else
    as_warn (_("Unrecognized option \"%s\""), opt);

  *input_line_pointer = c;
  demand_empty_rest_of_line ();
}

/* This structure is used to hold a stack of .set values.  */

struct ss_option_stack
{
  struct ss_option_stack *next;
  struct ss_set_options options;
};

static struct ss_option_stack *ss_opts_stack;

/* Handle the .set pseudo-op.  */

static void
s_ssset (x)
     int x;
{
  char *name = input_line_pointer, ch;

  while (!is_end_of_line[(unsigned char) *input_line_pointer])
    input_line_pointer++;
  ch = *input_line_pointer;
  *input_line_pointer = '\0';

  if (strcmp (name, "reorder") == 0)
    {
      if (ss_opts.noreorder && prev_nop_frag != NULL)
	{
	  /* If we still have pending nops, we can discard them.  The
	     usual nop handling will insert any that are still
	     needed. */
	  prev_nop_frag->fr_fix -= (prev_nop_frag_holds
				    * (4));
	  prev_nop_frag = NULL;
	}
      ss_opts.noreorder = 0;
    }
  else if (strcmp (name, "noreorder") == 0)
    {
      ss_opts.noreorder = 1;
      ss_any_noreorder = 1;
    }
  else if (strcmp (name, "at") == 0)
    {
      ss_opts.noat = 0;
    }
  else if (strcmp (name, "noat") == 0)
    {
      ss_opts.noat = 1;
    }
  else if (strcmp (name, "macro") == 0)
    {
      ss_opts.warn_about_macros = 0;
    }
  else if (strcmp (name, "nomacro") == 0)
    {
      if (ss_opts.noreorder == 0)
	as_bad (_("`noreorder' must be set before `nomacro'"));
      ss_opts.warn_about_macros = 1;
    }
  else if (strcmp (name, "move") == 0 || strcmp (name, "novolatile") == 0)
    {
      ss_opts.nomove = 0;
    }
  else if (strcmp (name, "nomove") == 0 || strcmp (name, "volatile") == 0)
    {
      ss_opts.nomove = 1;
    }
  else if (strcmp (name, "bopt") == 0)
    {
      ss_opts.nobopt = 0;
    }
  else if (strcmp (name, "nobopt") == 0)
    {
      ss_opts.nobopt = 1;
    }
  else if (strcmp (name, "autoextend") == 0)
    ss_opts.noautoextend = 0;
  else if (strcmp (name, "noautoextend") == 0)
    ss_opts.noautoextend = 1;
  else if (strcmp (name, "push") == 0)
    {
      struct ss_option_stack *s;

      s = (struct ss_option_stack *) xmalloc (sizeof *s);
      s->next = ss_opts_stack;
      s->options = ss_opts;
      ss_opts_stack = s;
    }
  else if (strcmp (name, "pop") == 0)
    {
      struct ss_option_stack *s;

      s = ss_opts_stack;
      if (s == NULL)
	as_bad (_(".set pop with no .set push"));
      else
	{
	  /* If we're changing the reorder mode we need to handle
             delay slots correctly.  */
	  if (s->options.noreorder && ! ss_opts.noreorder)
	    ;
	  else if (! s->options.noreorder && ss_opts.noreorder)
	    {
	      if (prev_nop_frag != NULL)
		{
		  prev_nop_frag->fr_fix -= (prev_nop_frag_holds
					    * (4));
		  prev_nop_frag = NULL;
		}
	    }

	  ss_opts = s->options;
	  ss_opts_stack = s->next;
	  free (s);
	}
    }
  else
    {
      as_warn (_("Tried to set unrecognized symbol: %s\n"), name);
    }
  *input_line_pointer = ch;
  demand_empty_rest_of_line ();
}

/* Handle the .abicalls pseudo-op.  I believe this is equivalent to
   .option pic2.  It means to generate SVR4 PIC calls.  */

static void
s_abicalls (ignore)
     int ignore;
{
  ss_pic = SVR4_PIC;
  if (USE_GLOBAL_POINTER_OPT)
    {
      if (g_switch_seen && g_switch_value != 0)
	as_warn (_("-G may not be used with SVR4 PIC code"));
      g_switch_value = 0;
    }
  bfd_set_gp_size (stdoutput, 0);
  demand_empty_rest_of_line ();
}

/* Handle the .cpload pseudo-op.  This is used when generating SVR4
   PIC code.  It sets the $gp register for the function based on the
   function address, which is in the register named in the argument.
   This uses a relocation against _gp_disp, which is handled specially
   by the linker.  The result is:
	lui	$gp,%hi(_gp_disp)
	addiu	$gp,$gp,%lo(_gp_disp)
	addu	$gp,$gp,.cpload argument
   The .cpload argument is normally $25 == $t9.  */

static void
s_cpload (ignore)
     int ignore;
{
  expressionS ex;
  int icnt = 0;

  /* If we are not generating SVR4 PIC code, .cpload is ignored.  */
  if (ss_pic != SVR4_PIC)
    {
      s_ignore (0);
      return;
    }

  /* .cpload should be a in .set noreorder section.  */
  if (ss_opts.noreorder == 0)
    as_warn (_(".cpload not in noreorder section"));

  ex.X_op = O_symbol;
  ex.X_add_symbol = symbol_find_or_make ("_gp_disp");
  ex.X_op_symbol = NULL;
  ex.X_add_number = 0;

  /* In ELF, this symbol is implicitly an STT_OBJECT symbol.  */
  symbol_get_bfdsym (ex.X_add_symbol)->flags |= BSF_OBJECT;

  macro_build_lui ((char *) NULL, &icnt, &ex, GP);
  macro_build ((char *) NULL, &icnt, &ex, "addiu", "t,r,j", GP, GP,
	       (int) BFD_RELOC_LO16);

  macro_build ((char *) NULL, &icnt, (expressionS *) NULL, "addu", "d,v,t",
	       GP, GP, tc_get_register (0));

  demand_empty_rest_of_line ();
}

/* Handle the .cprestore pseudo-op.  This stores $gp into a given
   offset from $sp.  The offset is remembered, and after making a PIC
   call $gp is restored from that location.  */

static void
s_cprestore (ignore)
     int ignore;
{
  expressionS ex;
  int icnt = 0;

  /* If we are not generating SVR4 PIC code, .cprestore is ignored.  */
  if (ss_pic != SVR4_PIC)
    {
      s_ignore (0);
      return;
    }

  ss_cprestore_offset = get_absolute_expression ();

  ex.X_op = O_constant;
  ex.X_add_symbol = NULL;
  ex.X_op_symbol = NULL;
  ex.X_add_number = ss_cprestore_offset;

  macro_build ((char *) NULL, &icnt, &ex,
	       ((bfd_arch_bits_per_address (stdoutput) == 32)
		? "sw" : "sd"),
	       "t,o(b)", GP, (int) BFD_RELOC_LO16, SP);

  demand_empty_rest_of_line ();
}

/* Handle the .gpword pseudo-op.  This is used when generating PIC
   code.  It generates a 32 bit GP relative reloc.  */

static void
s_gpword (ignore)
     int ignore;
{
  symbolS *label;
  expressionS ex;
  char *p;

  /* When not generating PIC code, this is treated as .word.  */
  if (ss_pic != SVR4_PIC)
    {
      s_cons (2);
      return;
    }

  label = insn_labels != NULL ? insn_labels->label : NULL;
  if (auto_align)
    ss_align (2, 0, label);
  ss_clear_insn_labels ();

  expression (&ex);

  if (ex.X_op != O_symbol || ex.X_add_number != 0)
    {
      as_bad (_("Unsupported use of .gpword"));
      ignore_rest_of_line ();
    }

  p = frag_more (4);
  md_number_to_chars (p, (valueT) 0, 4);
  fix_new_exp (frag_now, p - frag_now->fr_literal, 4, &ex, 0,
	       BFD_RELOC_MIPS_GPREL32);

  demand_empty_rest_of_line ();
}

/* Handle the .cpadd pseudo-op.  This is used when dealing with switch
   tables in SVR4 PIC code.  */

static void
s_cpadd (ignore)
     int ignore;
{
  int icnt = 0;
  int reg;

  /* This is ignored when not generating SVR4 PIC code.  */
  if (ss_pic != SVR4_PIC)
    {
      s_ignore (0);
      return;
    }

  /* Add $gp to the register named as an argument.  */
  reg = tc_get_register (0);
  macro_build ((char *) NULL, &icnt, (expressionS *) NULL,
	       ((bfd_arch_bits_per_address (stdoutput) == 32)
		? "addu" : "daddu"),
	       "d,v,t", reg, reg, GP);

  demand_empty_rest_of_line ();  
}

/* Handle the .insn pseudo-op.  This marks instruction labels in
   mips16 mode.  This permits the linker to handle them specially,
   such as generating jalx instructions when needed.  We also make
   them odd for the duration of the assembly, in order to generate the
   right sort of code.  We will make them even in the adjust_symtab
   routine, while leaving them marked.  This is convenient for the
   debugger and the disassembler.  The linker knows to make them odd
   again.  */

static void
s_insn (ignore)
     int ignore;
{
  demand_empty_rest_of_line ();
}

/* Handle a .stabn directive.  We need these in order to mark a label
   as being a mips16 text label correctly.  Sometimes the compiler
   will emit a label, followed by a .stabn, and then switch sections.
   If the label and .stabn are in mips16 mode, then the label is
   really a mips16 text label.  */

static void
s_ss_stab (type)
     int type;
{
  s_stab (type);
}

/* Handle the .weakext pseudo-op as defined in Kane and Heinrich.
 */

static void
s_ss_weakext (ignore)
     int ignore;
{
  char *name;
  int c;
  symbolS *symbolP;
  expressionS exp;

  name = input_line_pointer;
  c = get_symbol_end ();
  symbolP = symbol_find_or_make (name);
  S_SET_WEAK (symbolP);
  *input_line_pointer = c;

  SKIP_WHITESPACE ();

  if (! is_end_of_line[(unsigned char) *input_line_pointer])
    {
      if (S_IS_DEFINED (symbolP))
	{
	  as_bad ("Ignoring attempt to redefine symbol `%s'.",
		  S_GET_NAME (symbolP));
	  ignore_rest_of_line ();
	  return;
	}
      
      if (*input_line_pointer == ',')
	{
	  ++input_line_pointer;
	  SKIP_WHITESPACE ();
	}
      
      expression (&exp);
      if (exp.X_op != O_symbol)
	{
	  as_bad ("bad .weakext directive");
	  ignore_rest_of_line();
	  return;
	}
      symbol_set_value_expression (symbolP, &exp);
    }

  demand_empty_rest_of_line ();
}

/* Parse a register string into a number.  Called from the ECOFF code
   to parse .frame.  The argument is non-zero if this is the frame
   register, so that we can record it in mips_frame_reg.  */

int
tc_get_register (frame)
     int frame;
{
  int reg;

  SKIP_WHITESPACE ();
  if (*input_line_pointer++ != '$')
    {
      as_warn (_("expected `$'"));
      reg = 0;
    }
  else if (isdigit ((unsigned char) *input_line_pointer))
    {
      reg = get_absolute_expression ();
      if (reg < 0 || reg >= 32)
	{
	  as_warn (_("Bad register number"));
	  reg = 0;
	}
    }
  else
    {
      if (strncmp (input_line_pointer, "fp", 2) == 0)
	reg = FP;
      else if (strncmp (input_line_pointer, "sp", 2) == 0)
	reg = SP;
      else if (strncmp (input_line_pointer, "gp", 2) == 0)
	reg = GP;
      else if (strncmp (input_line_pointer, "at", 2) == 0)
	reg = AT;
      else
	{
	  as_warn (_("Unrecognized register name"));
	  reg = 0;
	}
      input_line_pointer += 2;
    }
  if (frame)
    ss_frame_reg = reg != 0 ? reg : SP;
  return reg;
}

valueT
md_section_align (seg, addr)
     asection *seg;
     valueT addr;
{
  int align = bfd_get_section_alignment (stdoutput, seg);

#ifdef OBJ_ELF
  /* We don't need to align ELF sections to the full alignment.
     However, Irix 5 may prefer that we align them at least to a 16
     byte boundary.  We don't bother to align the sections if we are
     targeted for an embedded system.  */
  if (strcmp (TARGET_OS, "elf") == 0)
    return addr;
  if (align > 4)
    align = 4;
#endif

  return ((addr + (1 << align) - 1) & (-1 << align));
}

/* Utility routine, called from above as well.  If called while the
   input file is still being read, it's only an approximation.  (For
   example, a symbol may later become defined which appeared to be
   undefined earlier.)  */

static int
nopic_need_relax (sym, before_relaxing)
     symbolS *sym;
     int before_relaxing;
{
  if (sym == 0)
    return 0;

  if (USE_GLOBAL_POINTER_OPT)
    {
      const char *symname;
      int change;

      /* Find out whether this symbol can be referenced off the GP
	 register.  It can be if it is smaller than the -G size or if
	 it is in the .sdata or .sbss section.  Certain symbols can
	 not be referenced off the GP, although it appears as though
	 they can.  */
      symname = S_GET_NAME (sym);
      if (symname != (const char *) NULL
	  && (strcmp (symname, "eprol") == 0
	      || strcmp (symname, "etext") == 0
	      || strcmp (symname, "_gp") == 0
	      || strcmp (symname, "edata") == 0
	      || strcmp (symname, "_fbss") == 0
	      || strcmp (symname, "_fdata") == 0
	      || strcmp (symname, "_ftext") == 0
	      || strcmp (symname, "end") == 0
	      || strcmp (symname, "_gp_disp") == 0))
	change = 1;
      else if ((! S_IS_DEFINED (sym) || S_IS_COMMON (sym))
	       && (0
#ifndef NO_ECOFF_DEBUGGING
		   || (symbol_get_obj (sym)->ecoff_extern_size != 0
		       && (symbol_get_obj (sym)->ecoff_extern_size
			   <= g_switch_value))
#endif
		   /* We must defer this decision until after the whole
		      file has been read, since there might be a .extern
		      after the first use of this symbol.  */
		   || (before_relaxing
#ifndef NO_ECOFF_DEBUGGING
		       && symbol_get_obj (sym)->ecoff_extern_size == 0
#endif
		       && S_GET_VALUE (sym) == 0)
		   || (S_GET_VALUE (sym) != 0
		       && S_GET_VALUE (sym) <= g_switch_value)))
	change = 0;
      else
	{
	  const char *segname;

	  segname = segment_name (S_GET_SEGMENT (sym));
	  assert (strcmp (segname, ".lit8") != 0
		  && strcmp (segname, ".lit4") != 0);
	  change = (strcmp (segname, ".sdata") != 0
		    && strcmp (segname, ".sbss") != 0);
	}
      return change;
    }
  else
    /* We are not optimizing for the GP register.  */
    return 1;
}

/* Estimate the size of a frag before relaxing.  Unless this is the
   mips16, we are not really relaxing here, and the final size is
   encoded in the subtype information.  For the mips16, we have to
   decide whether we are using an extended opcode or not.  */

/*ARGSUSED*/
int
md_estimate_size_before_relax (fragp, segtype)
     fragS *fragp;
     asection *segtype;
{
  int change;

  if (ss_pic == NO_PIC)
    {
      change = nopic_need_relax (fragp->fr_symbol, 0);
    }
  else if (ss_pic == SVR4_PIC)
    {
      symbolS *sym;
      asection *symsec;

      sym = fragp->fr_symbol;

      /* Handle the case of a symbol equated to another symbol.  */
      while (symbol_equated_p (sym)
	     && (! S_IS_DEFINED (sym) || S_IS_COMMON (sym)))
	{
	  symbolS *n;

	  /* It's possible to get a loop here in a badly written
             program.  */
	  n = symbol_get_value_expression (sym)->X_add_symbol;
	  if (n == sym)
	    break;
	  sym = n;
	}

      symsec = S_GET_SEGMENT (sym);

      /* This must duplicate the test in adjust_reloc_syms.  */
      change = (symsec != &bfd_und_section
		&& symsec != &bfd_abs_section
		&& ! bfd_is_com_section (symsec));
    }
  else
    abort ();

  if (change)
    {
      /* Record the offset to the first reloc in the fr_opcode field.
	 This lets md_convert_frag and tc_gen_reloc know that the code
	 must be expanded.  */
      fragp->fr_opcode = (fragp->fr_literal
			  + fragp->fr_fix
			  - RELAX_OLD (fragp->fr_subtype)
			  + RELAX_RELOC1 (fragp->fr_subtype));
      /* FIXME: This really needs as_warn_where.  */
      if (RELAX_WARN (fragp->fr_subtype))
	as_warn (_("AT used after \".set noat\" or macro used after \".set nomacro\""));
    }

  if (! change)
    return 0;
  else
    return RELAX_NEW (fragp->fr_subtype) - RELAX_OLD (fragp->fr_subtype);
}

/* This is called to see whether a reloc against a defined symbol
   should be converted into a reloc against a section.  Don't adjust
   MIPS16 jump relocations, so we don't have to worry about the format
   of the offset in the .o file.  Don't adjust relocations against
   mips16 symbols, so that the linker can find them if it needs to set
   up a stub.  */

int
ss_fix_adjustable (fixp)
     fixS *fixp;
{
  if (fixp->fx_r_type == BFD_RELOC_VTABLE_INHERIT
      || fixp->fx_r_type == BFD_RELOC_VTABLE_ENTRY)
    return 0;
  if (fixp->fx_addsy == NULL)
    return 1;
  return 1;
}

/* Translate internal representation of relocation info to BFD target
   format.  */

arelent **
tc_gen_reloc (section, fixp)
     asection *section;
     fixS *fixp;
{
  static arelent *retval[4];
  arelent *reloc;
  bfd_reloc_code_real_type code;

  reloc = retval[0] = (arelent *) xmalloc (sizeof (arelent));
  retval[1] = NULL;

  reloc->sym_ptr_ptr = (asymbol **) xmalloc (sizeof (asymbol *));
  *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);
  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;

  if (ss_pic == EMBEDDED_PIC
      && SWITCH_TABLE (fixp))
    {
      /* For a switch table entry we use a special reloc.  The addend
	 is actually the difference between the reloc address and the
	 subtrahend.  */
      reloc->addend = reloc->address - S_GET_VALUE (fixp->fx_subsy);
      if (OUTPUT_FLAVOR != bfd_target_ecoff_flavour)
	as_fatal (_("Double check fx_r_type in tc-ss.c:tc_gen_reloc"));
      fixp->fx_r_type = BFD_RELOC_GPREL32;
    }
  else if (fixp->fx_r_type == BFD_RELOC_PCREL_LO16)
    {
      /* We use a special addend for an internal RELLO reloc.  */
      if (symbol_section_p (fixp->fx_addsy))
	reloc->addend = reloc->address - S_GET_VALUE (fixp->fx_subsy);
      else
	reloc->addend = fixp->fx_addnumber + reloc->address;
    }
  else if (fixp->fx_r_type == BFD_RELOC_PCREL_HI16_S)
    {
      assert (fixp->fx_next != NULL
	      && fixp->fx_next->fx_r_type == BFD_RELOC_PCREL_LO16);
      /* We use a special addend for an internal RELHI reloc.  The
	 reloc is relative to the RELLO; adjust the addend
	 accordingly.  */
      if (symbol_section_p (fixp->fx_addsy))
	reloc->addend = (fixp->fx_next->fx_frag->fr_address
			 + fixp->fx_next->fx_where
			 - S_GET_VALUE (fixp->fx_subsy));
      else
	reloc->addend = (fixp->fx_addnumber
			 + fixp->fx_next->fx_frag->fr_address
			 + fixp->fx_next->fx_where);
    }
  else if (fixp->fx_pcrel == 0)
    reloc->addend = fixp->fx_addnumber;
  else
    {
      if (OUTPUT_FLAVOR != bfd_target_aout_flavour)
	/* A gruesome hack which is a result of the gruesome gas reloc
	   handling.  */
	reloc->addend = reloc->address;
      else
	reloc->addend = -reloc->address;
    }

  /* If this is a variant frag, we may need to adjust the existing
     reloc and generate a new one.  */
  if (fixp->fx_frag->fr_opcode != NULL
      && (fixp->fx_r_type == BFD_RELOC_MIPS_GPREL
	  || fixp->fx_r_type == BFD_RELOC_MIPS_GOT16
	  || fixp->fx_r_type == BFD_RELOC_MIPS_CALL16
	  || fixp->fx_r_type == BFD_RELOC_MIPS_GOT_HI16
	  || fixp->fx_r_type == BFD_RELOC_MIPS_GOT_LO16
	  || fixp->fx_r_type == BFD_RELOC_MIPS_CALL_HI16
	  || fixp->fx_r_type == BFD_RELOC_MIPS_CALL_LO16))
    {
      arelent *reloc2;

      /* If this is not the last reloc in this frag, then we have two
	 GPREL relocs, or a GOT_HI16/GOT_LO16 pair, or a
	 CALL_HI16/CALL_LO16, both of which are being replaced.  Let
	 the second one handle all of them.  */
      if (fixp->fx_next != NULL
	  && fixp->fx_frag == fixp->fx_next->fx_frag)
	{
	  assert ((fixp->fx_r_type == BFD_RELOC_MIPS_GPREL
		   && fixp->fx_next->fx_r_type == BFD_RELOC_MIPS_GPREL)
		  || (fixp->fx_r_type == BFD_RELOC_MIPS_GOT_HI16
		      && (fixp->fx_next->fx_r_type
			  == BFD_RELOC_MIPS_GOT_LO16))
		  || (fixp->fx_r_type == BFD_RELOC_MIPS_CALL_HI16
		      && (fixp->fx_next->fx_r_type
			  == BFD_RELOC_MIPS_CALL_LO16)));
	  retval[0] = NULL;
	  return retval;
	}

      fixp->fx_where = fixp->fx_frag->fr_opcode - fixp->fx_frag->fr_literal;
      reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
      reloc2 = retval[1] = (arelent *) xmalloc (sizeof (arelent));
      retval[2] = NULL;
      reloc2->sym_ptr_ptr = (asymbol **) xmalloc (sizeof (asymbol *));
      *reloc2->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);
      reloc2->address = (reloc->address
			 + (RELAX_RELOC2 (fixp->fx_frag->fr_subtype)
			    - RELAX_RELOC1 (fixp->fx_frag->fr_subtype)));
      reloc2->addend = fixp->fx_addnumber;
      reloc2->howto = bfd_reloc_type_lookup (stdoutput, BFD_RELOC_LO16);
      assert (reloc2->howto != NULL);

      if (RELAX_RELOC3 (fixp->fx_frag->fr_subtype))
	{
	  arelent *reloc3;

	  reloc3 = retval[2] = (arelent *) xmalloc (sizeof (arelent));
	  retval[3] = NULL;
	  *reloc3 = *reloc2;
	  reloc3->address += 8;
	}

      if (ss_pic == NO_PIC)
	{
	  assert (fixp->fx_r_type == BFD_RELOC_MIPS_GPREL);
	  fixp->fx_r_type = BFD_RELOC_HI16_S;
	}
      else if (ss_pic == SVR4_PIC)
	{
	  switch (fixp->fx_r_type)
	    {
	    default:
	      abort ();
	    case BFD_RELOC_MIPS_GOT16:
	      break;
	    case BFD_RELOC_MIPS_CALL16:
	    case BFD_RELOC_MIPS_GOT_LO16:
	    case BFD_RELOC_MIPS_CALL_LO16:
	      fixp->fx_r_type = BFD_RELOC_MIPS_GOT16;
	      break;
	    }
	}
      else
	abort ();
    }

  /* Since SS ELF uses Rel instead of Rela, encode the vtable entry
     to be used in the relocation's section offset.  */
  if (fixp->fx_r_type == BFD_RELOC_VTABLE_ENTRY)
    {
      reloc->address = reloc->addend;
      reloc->addend = 0;
    }

  /* Since DIFF_EXPR_OK is defined in tc-ss.h, it is possible that
     fixup_segment converted a non-PC relative reloc into a PC
     relative reloc.  In such a case, we need to convert the reloc
     code.  */
  code = fixp->fx_r_type;
  if (fixp->fx_pcrel)
    {
      switch (code)
	{
	case BFD_RELOC_8:
	  code = BFD_RELOC_8_PCREL;
	  break;
	case BFD_RELOC_16:
	  code = BFD_RELOC_16_PCREL;
	  break;
	case BFD_RELOC_32:
	  code = BFD_RELOC_32_PCREL;
	  break;
	case BFD_RELOC_64:
	  code = BFD_RELOC_64_PCREL;
	  break;
	case BFD_RELOC_8_PCREL:
	case BFD_RELOC_16_PCREL:
	case BFD_RELOC_32_PCREL:
	case BFD_RELOC_64_PCREL:
	case BFD_RELOC_16_PCREL_S2:
	case BFD_RELOC_PCREL_HI16_S:
	case BFD_RELOC_PCREL_LO16:
	  break;
	default:
	  as_bad_where (fixp->fx_file, fixp->fx_line,
			_("Cannot make %s relocation PC relative"),
			bfd_get_reloc_code_name (code));
	}
    }

  /* To support a PC relative reloc when generating embedded PIC code
     for ECOFF, we use a Cygnus extension.  We check for that here to
     make sure that we don't let such a reloc escape normally.  */
  if (OUTPUT_FLAVOR == bfd_target_ecoff_flavour
      && code == BFD_RELOC_16_PCREL_S2
      && ss_pic != EMBEDDED_PIC)
    reloc->howto = NULL;
  else
    reloc->howto = bfd_reloc_type_lookup (stdoutput, code);

  if (reloc->howto == NULL)
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
		    _("Can not represent %s relocation in this object file format"),
		    bfd_get_reloc_code_name (code));
      retval[0] = NULL;
    }

  return retval;
}

/* Relax a machine dependent frag.  This returns the amount by which
   the current size of the frag should change.  */

int
ss_relax_frag (fragp, stretch)
     fragS *fragp;
     long stretch;
{
  return 0;
}

/* Convert a machine dependent frag.  */

void
md_convert_frag (abfd, asec, fragp)
     bfd *abfd;
     segT asec;
     fragS *fragp;
{
  int old, new;
  char *fixptr;

  if (fragp->fr_opcode == NULL)
    return;

  old = RELAX_OLD (fragp->fr_subtype);
  new = RELAX_NEW (fragp->fr_subtype);
  fixptr = fragp->fr_literal + fragp->fr_fix;
  
  if (new > 0)
    memcpy (fixptr - old, fixptr, new);
  
  fragp->fr_fix += new - old;
}

#ifdef OBJ_ELF

/* This function is called after the relocs have been generated.
   We've been storing mips16 text labels as odd.  Here we convert them
   back to even for the convenience of the debugger.  */

void
ss_frob_file_after_relocs ()
{
  asymbol **syms;
  unsigned int count, i;

  if (OUTPUT_FLAVOR != bfd_target_elf_flavour)
    return;

  syms = bfd_get_outsymbols (stdoutput);
  count = bfd_get_symcount (stdoutput);
}

#endif

/* This function is called whenever a label is defined.  It is used
   when handling branch delays; if a branch has a label, we assume we
   can not move it.  */

void
ss_define_label (sym)
     symbolS *sym;
{
  struct insn_label_list *l;

  if (free_insn_labels == NULL)
    l = (struct insn_label_list *) xmalloc (sizeof *l);
  else
    {
      l = free_insn_labels;
      free_insn_labels = l->next;
    }

  l->label = sym;
  l->next = insn_labels;
  insn_labels = l;
}

#if defined (OBJ_ELF) || defined (OBJ_MAYBE_ELF)

/* Some special processing for a MIPS ELF file.  */

void
ss_elf_final_processing ()
{
  /* Write out the register information.  */
  if (! ss_64)
    {
      Elf32_RegInfo s;

      s.ri_gprmask = ss_gprmask;
      s.ri_cprmask[0] = ss_cprmask[0];
      s.ri_cprmask[1] = ss_cprmask[1];
      s.ri_cprmask[2] = ss_cprmask[2];
      s.ri_cprmask[3] = ss_cprmask[3];
      /* The gp_value field is set by the SS ELF backend.  */

      bfd_ss_elf32_swap_reginfo_out (stdoutput, &s,
				       ((Elf32_External_RegInfo *)
					ss_regmask_frag));
    }
  else
    {
      Elf64_Internal_RegInfo s;

      s.ri_gprmask = ss_gprmask;
      s.ri_pad = 0;
      s.ri_cprmask[0] = ss_cprmask[0];
      s.ri_cprmask[1] = ss_cprmask[1];
      s.ri_cprmask[2] = ss_cprmask[2];
      s.ri_cprmask[3] = ss_cprmask[3];
      /* The gp_value field is set by the SS ELF backend.  */

      bfd_ss_elf64_swap_reginfo_out (stdoutput, &s,
				       ((Elf64_External_RegInfo *)
					ss_regmask_frag));
    }

  /* Set the SS ELF flag bits.  FIXME: There should probably be some
     sort of BFD interface for this.  */
  if (ss_any_noreorder)
    elf_elfheader (stdoutput)->e_flags |= EF_SS_NOREORDER;
  if (ss_pic != NO_PIC)
    elf_elfheader (stdoutput)->e_flags |= EF_SS_PIC;

  /* Set the SS ELF ABI flags. */
  if (ss_abi_string == 0)
    ;
  else if (strcmp (ss_abi_string,"32") == 0)
    elf_elfheader (stdoutput)->e_flags |= E_SS_ABI_O32;
  else if (strcmp (ss_abi_string,"o64") == 0)
    elf_elfheader (stdoutput)->e_flags |= E_SS_ABI_O64;
  else if (strcmp (ss_abi_string,"eabi") == 0)
    {
      if (ss_eabi64)
	elf_elfheader (stdoutput)->e_flags |= E_SS_ABI_EABI64;
      else
	elf_elfheader (stdoutput)->e_flags |= E_SS_ABI_EABI32;
    }

  if (ss_32bitmode)
    elf_elfheader (stdoutput)->e_flags |= EF_SS_32BITMODE;
}

#endif /* OBJ_ELF || OBJ_MAYBE_ELF */

typedef struct proc
  {
    symbolS *isym;
    unsigned long reg_mask;
    unsigned long reg_offset;
    unsigned long fpreg_mask;
    unsigned long fpreg_offset;
    unsigned long frame_offset;
    unsigned long frame_reg;
    unsigned long pc_reg;
  }
procS;

static procS cur_proc;
static procS *cur_proc_ptr;
static int numprocs;

static void
md_obj_begin ()
{
}

static void
md_obj_end ()
{
  /* check for premature end, nesting errors, etc */
  if (cur_proc_ptr)
    as_warn (_("missing `.end' at end of assembly"));
}

static long
get_number ()
{
  int negative = 0;
  long val = 0;

  if (*input_line_pointer == '-')
    {
      ++input_line_pointer;
      negative = 1;
    }
  if (!isdigit ((unsigned char) *input_line_pointer))
    as_bad (_("Expected simple number."));
  if (input_line_pointer[0] == '0')
    {
      if (input_line_pointer[1] == 'x')
	{
	  input_line_pointer += 2;
	  while (isxdigit ((unsigned char) *input_line_pointer))
	    {
	      val <<= 4;
	      val |= hex_value (*input_line_pointer++);
	    }
	  return negative ? -val : val;
	}
      else
	{
	  ++input_line_pointer;
	  while (isdigit ((unsigned char) *input_line_pointer))
	    {
	      val <<= 3;
	      val |= *input_line_pointer++ - '0';
	    }
	  return negative ? -val : val;
	}
    }
  if (!isdigit ((unsigned char) *input_line_pointer))
    {
      printf (_(" *input_line_pointer == '%c' 0x%02x\n"),
	      *input_line_pointer, *input_line_pointer);
      as_warn (_("Invalid number"));
      return -1;
    }
  while (isdigit ((unsigned char) *input_line_pointer))
    {
      val *= 10;
      val += *input_line_pointer++ - '0';
    }
  return negative ? -val : val;
}

/* The .file directive; just like the usual .file directive, but there
   is an initial number which is the ECOFF file index.  */

static void
s_file (x)
     int x;
{
  int line;

  line = get_number ();
  s_app_file (0);
}


/* The .end directive.  */

static void
s_ss_end (x)
     int x;
{
  symbolS *p;
  int maybe_text;

  if (!is_end_of_line[(unsigned char) *input_line_pointer])
    {
      p = get_symbol ();
      demand_empty_rest_of_line ();
    }
  else
    p = NULL;

#ifdef BFD_ASSEMBLER
  if ((bfd_get_section_flags (stdoutput, now_seg) & SEC_CODE) != 0)
    maybe_text = 1;
  else
    maybe_text = 0;
#else
  if (now_seg != data_section && now_seg != bss_section)
    maybe_text = 1;
  else
    maybe_text = 0;
#endif

  if (!maybe_text)
    as_warn (_(".end not in text section"));

  if (!cur_proc_ptr)
    {
      as_warn (_(".end directive without a preceding .ent directive."));
      demand_empty_rest_of_line ();
      return;
    }

  if (p != NULL)
    {
      assert (S_GET_NAME (p));
      if (strcmp (S_GET_NAME (p), S_GET_NAME (cur_proc_ptr->isym)))
	as_warn (_(".end symbol does not match .ent symbol."));
    }
  else
    as_warn (_(".end directive missing or unknown symbol"));

#ifdef SS_STABS_ELF
  {
    segT saved_seg = now_seg;
    subsegT saved_subseg = now_subseg;
    fragS *saved_frag = frag_now;
    valueT dot;
    segT seg;
    expressionS exp;
    char *fragp;

    dot = frag_now_fix ();

#ifdef md_flush_pending_output
    md_flush_pending_output ();
#endif

    assert (pdr_seg);
    subseg_set (pdr_seg, 0);

    /* Write the symbol */
    exp.X_op = O_symbol;
    exp.X_add_symbol = p;
    exp.X_add_number = 0;
    emit_expr (&exp, 4);

    fragp = frag_more (7*4);

    md_number_to_chars (fragp,     (valueT) cur_proc_ptr->reg_mask, 4);
    md_number_to_chars (fragp + 4, (valueT) cur_proc_ptr->reg_offset, 4);
    md_number_to_chars (fragp + 8, (valueT) cur_proc_ptr->fpreg_mask, 4);
    md_number_to_chars (fragp +12, (valueT) cur_proc_ptr->fpreg_offset, 4);
    md_number_to_chars (fragp +16, (valueT) cur_proc_ptr->frame_offset, 4);
    md_number_to_chars (fragp +20, (valueT) cur_proc_ptr->frame_reg, 4);
    md_number_to_chars (fragp +24, (valueT) cur_proc_ptr->pc_reg, 4);

    subseg_set (saved_seg, saved_subseg);
  }
#endif

  cur_proc_ptr = NULL;
}

/* The .aent and .ent directives.  */

static void
s_ss_ent (aent)
     int aent;
{
  int number = 0;
  symbolS *symbolP;
  int maybe_text;

  symbolP = get_symbol ();
  if (*input_line_pointer == ',')
    input_line_pointer++;
  SKIP_WHITESPACE ();
  if (isdigit ((unsigned char) *input_line_pointer)
      || *input_line_pointer == '-')
    number = get_number ();

#ifdef BFD_ASSEMBLER
  if ((bfd_get_section_flags (stdoutput, now_seg) & SEC_CODE) != 0)
    maybe_text = 1;
  else
    maybe_text = 0;
#else
  if (now_seg != data_section && now_seg != bss_section)
    maybe_text = 1;
  else
    maybe_text = 0;
#endif

  if (!maybe_text)
    as_warn (_(".ent or .aent not in text section."));

  if (!aent && cur_proc_ptr)
    as_warn (_("missing `.end'"));

  if (!aent)
    {
      cur_proc_ptr = &cur_proc;
      memset (cur_proc_ptr, '\0', sizeof (procS));

      cur_proc_ptr->isym = symbolP;

      symbol_get_bfdsym (symbolP)->flags |= BSF_FUNCTION;

      numprocs++;
    }

  demand_empty_rest_of_line ();
}

/* The .frame directive. If the mdebug section is present (IRIX 5 native)
   then ecoff.c (ecoff_directive_frame) is used. For embedded targets, 
   s_ss_frame is used so that we can set the PDR information correctly.
   We can't use the ecoff routines because they make reference to the ecoff 
   symbol table (in the mdebug section).  */

static void
s_ss_frame (ignore)
     int ignore;
{
#ifdef SS_STABS_ELF

  long val;

  if (cur_proc_ptr ==  (procS *) NULL)
    {
      as_warn (_(".frame outside of .ent"));
      demand_empty_rest_of_line ();
      return;
    }

  cur_proc_ptr->frame_reg = tc_get_register (1);

  SKIP_WHITESPACE ();
  if (*input_line_pointer++ != ','
      || get_absolute_expression_and_terminator (&val) != ',')
    {
      as_warn (_("Bad .frame directive"));
      --input_line_pointer;
      demand_empty_rest_of_line ();
      return;
    }

  cur_proc_ptr->frame_offset = val;
  cur_proc_ptr->pc_reg = tc_get_register (0);

  demand_empty_rest_of_line ();
#else
  s_ignore (ignore);
#endif /* SS_STABS_ELF */
}

/* The .fmask and .mask directives. If the mdebug section is present 
   (IRIX 5 native) then ecoff.c (ecoff_directive_mask) is used. For 
   embedded targets, s_ss_mask is used so that we can set the PDR
   information correctly. We can't use the ecoff routines because they 
   make reference to the ecoff symbol table (in the mdebug section).  */

static void
s_ss_mask (reg_type)
     char reg_type;
{
#ifdef SS_STABS_ELF
  long mask, off;
  
  if (cur_proc_ptr == (procS *) NULL)
    {
      as_warn (_(".mask/.fmask outside of .ent"));
      demand_empty_rest_of_line ();
      return;
    }

  if (get_absolute_expression_and_terminator (&mask) != ',')
    {
      as_warn (_("Bad .mask/.fmask directive"));
      --input_line_pointer;
      demand_empty_rest_of_line ();
      return;
    }

  off = get_absolute_expression ();

  if (reg_type == 'F')
    {
      cur_proc_ptr->fpreg_mask = mask;
      cur_proc_ptr->fpreg_offset = off;
    }
  else
    {
      cur_proc_ptr->reg_mask = mask;
      cur_proc_ptr->reg_offset = off;
    }

  demand_empty_rest_of_line ();
#else
  s_ignore (reg_type);
#endif /* SS_STABS_ELF */
}

/* The .loc directive.  */

#if 0
static void
s_loc (x)
     int x;
{
  symbolS *symbolP;
  int lineno;
  int addroff;

  assert (now_seg == text_section);

  lineno = get_number ();
  addroff = frag_now_fix ();

  symbolP = symbol_new ("", N_SLINE, addroff, frag_now);
  S_SET_TYPE (symbolP, N_SLINE);
  S_SET_OTHER (symbolP, 0);
  S_SET_DESC (symbolP, lineno);
  symbolP->sy_segment = now_seg;
}
#endif


  
