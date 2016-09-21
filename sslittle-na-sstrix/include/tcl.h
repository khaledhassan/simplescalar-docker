/*
 * tcl.h --
 *
 *	This header file describes the externally-visible facilities
 *	of the Tcl interpreter.
 *
 * Copyright 1987 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /sprite/src/lib/tcl/RCS/tcl.h,v 1.35 90/04/18 16:05:56 ouster Exp $ SPRITE (Berkeley)
 */

#ifndef _TCL
#define _TCL

/*
 * Data structures defined opaquely in this module.  The definitions
 * below just provide dummy types.  A few fields are made visible in
 * Tcl_Interp structures, namely those for returning string values.
 * Note:  any change to the Tcl_Interp definition below must be mirrored
 * in the "real" definition in tclInt.h.
 */

typedef struct {
    char *result;		/* Points to result string returned by last
				 * command. */
    int dynamic;		/* Non-zero means result is dynamically-
				 * allocated and must be freed by Tcl_Eval
				 * before executing the next command. */
    int errorLine;		/* When TCL_ERROR is returned, this gives
				 * the line number within the command where
				 * the error occurred (1 means first line). */
} Tcl_Interp;

typedef int *Tcl_Trace;
typedef int *Tcl_CmdBuf;

/*
 * When a TCL command returns, the string pointer interp->result points to
 * a string containing return information from the command.  In addition,
 * the command procedure returns an integer value, which is one of the
 * following:
 *
 * TCL_OK		Command completed normally;  interp->result contains
 *			the command's result.
 * TCL_ERROR		The command couldn't be completed successfully;
 *			interp->result describes what went wrong.
 * TCL_RETURN		The command requests that the current procedure
 *			return;  interp->result contains the procedure's
 *			return value.
 * TCL_BREAK		The command requests that the innermost loop
 *			be exited;  interp->result is meaningless.
 * TCL_CONTINUE		Go on to the next iteration of the current loop;
 *			interp->result is meaninless.
 */

#define TCL_OK		0
#define TCL_ERROR	1
#define TCL_RETURN	2
#define TCL_BREAK	3
#define TCL_CONTINUE	4

#define TCL_RESULT_SIZE 199

/*
 * Flag values passed to Tcl_Eval (see the man page for details;  also
 * see tclInt.h for additional flags that are only used internally by
 * Tcl):
 */

#define TCL_BRACKET_TERM	1

/*
 * Flag value passed to Tcl_RecordAndEval to request no evaluation
 * (record only).
 */

#define TCL_NO_EVAL		-1

/*
 * Flag values passed to Tcl_Return (see the man page for details):
 */

#define TCL_STATIC	0
#define TCL_DYNAMIC	1
#define TCL_VOLATILE	2

/*
 * Exported Tcl procedures:
 */

extern char *		Tcl_AssembleCmd();
extern void		Tcl_AddErrorInfo();
extern char		Tcl_Backslash();
extern char *		Tcl_Concat();
extern Tcl_CmdBuf	Tcl_CreateCmdBuf();
extern void		Tcl_CreateCommand();
extern Tcl_Interp *	Tcl_CreateInterp();
extern Tcl_Trace	Tcl_CreateTrace();
extern void		Tcl_DeleteCmdBuf();
extern void		Tcl_DeleteCommand();
extern void		Tcl_DeleteInterp();
extern void		Tcl_DeleteTrace();
extern int		Tcl_Eval();
extern int		Tcl_Expr();
extern char *		Tcl_GetVar();
extern char *		Tcl_Merge();
extern char *		Tcl_ParseVar();
extern int		Tcl_RecordAndEval();
extern void		Tcl_Return();
extern void		Tcl_SetVar();
extern int		Tcl_SplitList();
extern int		Tcl_StringMatch();
extern char *		Tcl_TildeSubst();
extern void		Tcl_WatchInterp();

/*
 * Built-in Tcl command procedures:
 */

extern int		Tcl_BreakCmd();
extern int		Tcl_CaseCmd();
extern int		Tcl_CatchCmd();
extern int		Tcl_ConcatCmd();
extern int		Tcl_ContinueCmd();
extern int		Tcl_ErrorCmd();
extern int		Tcl_EvalCmd();
extern int		Tcl_ExecCmd();
extern int		Tcl_ExprCmd();
extern int		Tcl_FileCmd();
extern int		Tcl_ForCmd();
extern int		Tcl_ForeachCmd();
extern int		Tcl_FormatCmd();
extern int		Tcl_GlobCmd();
extern int		Tcl_GlobalCmd();
extern int		Tcl_HistoryCmd();
extern int		Tcl_IfCmd();
extern int		Tcl_InfoCmd();
extern int		Tcl_IndexCmd();
extern int		Tcl_LengthCmd();
extern int		Tcl_ListCmd();
extern int		Tcl_PrintCmd();
extern int		Tcl_ProcCmd();
extern int		Tcl_RangeCmd();
extern int		Tcl_RenameCmd();
extern int		Tcl_ReturnCmd();
extern int		Tcl_ScanCmd();
extern int		Tcl_SetCmd();
extern int		Tcl_SourceCmd();
extern int		Tcl_StringCmd();
extern int		Tcl_TimeCmd();
extern int		Tcl_UplevelCmd();

/*
 * Miscellaneous declarations (to allow Tcl to be used stand-alone,
 * without the rest of Sprite).
 */

#ifndef NULL
#define NULL 0
#endif

#ifndef _CLIENTDATA
typedef int *ClientData;
#define _CLIENTDATA
#endif

#endif _TCL
