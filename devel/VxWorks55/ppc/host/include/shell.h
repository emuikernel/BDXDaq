/* shell.h - header for shell core */

/* Copyright 1984-1992 Wind River Systems, Inc. */

/*
modification history
--------------------
01k,14oct98,jmp  fixed win32Console management.
01j,13oct98,jmp  updated for windsh directory restructuration.
01i,25may98,fle  moved shell related extarnals from wpwrutil.c into here
01h,06apr98,jmp  added SHELL_TCL_PROMPT, SHELL_WIND_PROMPT and
		 MAX_PROMPT_LEN defines.
01g,28jan98,c_c  Fixed compile Pb if /usr/include/syms.h included before.
01f,22jan98,c_c  Modified include section (for WIN32).
01e,03dec95,c_s  add alignDoubleParams to SHELL_OPS structure (for 960 floats).
01d,10nov95,c_s  add targetCrossEndian to SHELL_OPS structure (SPR #4459).
01c,30may95,p_m  added Solaris-2 and HP-UX  support.
01b,16mar95,c_s  made SHELL_OPS.funcCall{Float,Double}Rtn {FLT,DBL}FUNCPTRs.
01a,29dec93,jcf  written
*/

#ifndef __INCshellh
#define __INCshellh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "tcl.h"
#include "wtx.h"

#ifndef WIN32
#include <sys/time.h>
#endif

#define SHELL_TCL_PROMPT	"tcl> "
#define	SHELL_WIND_PROMPT	"-> "
#define MAX_PROMPT_LEN		80
#define TCL_INTERACTIVE_VAR	"tcl_interactive"
#define TCL_TARGETNAME_VAR	"windShTarget"

extern Tcl_Interp *pTclInterp;		/* WindSh Tcl interpreter */
extern int	shMode;			/* current shell mode WindSh or Tcl */
extern int	haveTerminal;		/* 1 --> we are talking to real tty */
extern int	shEndian;		/* Endiannes of target */
extern char *	targetName;
extern HWTX	shHwtx;			/* WindSh WTX handle */
extern BOOL	shellQuiet;		/* echo script input */
extern BOOL	shellPrintResult;	/* print result after exec? */
extern int	shellLedId;		/* line editor id (-1 = none) */
extern FUNCPTR	shellPreParseRtn;	/* pre-parse callout */
extern FUNCPTR	shellParseRtn;		/* alternate parse routine */
extern char	tyBackspaceChar;	/* ledutil's backspace */
extern char	tyDeleteLineChar;	/* ledutil's kill */
extern char	tyEofChar;		/* ledutil's EOF */
extern BOOL	shellNewLine;		/* TRUE when windsh parse a new line */

#ifdef WIN32
extern BOOL	win32Console;		/* if WIN32 console runnig */
#endif

/* typedefs */

typedef struct		/* SHELL_OPS */
    {
    FUNCPTR cellReadRtn;
    FUNCPTR cellWriteRtn;
    FUNCPTR symFindByCNameRtn;
    FUNCPTR symFindByAdaNameRtn;
    FUNCPTR symFindByCplusNameRtn;
    FUNCPTR newStringRtn;
    FUNCPTR newSymRtn;
    FUNCPTR printSymRtn;
    FUNCPTR isFuncRtn;
    FUNCPTR funcCallIntRtn;
    FLTFUNCPTR funcCallFloatRtn;
    DBLFUNCPTR funcCallDoubleRtn;
    FUNCPTR funcCallHost;
    FUNCPTR targetCrossEndian;
    FUNCPTR alignDoubleParams;
    } SHELL_OPS;

/* those defines may have been set in /usr/include/syms.h. Remove them */
 
#ifdef T_INT
#undef T_INT
#endif
 
#ifdef T_FLOAT
#undef T_FLOAT
#endif
 
#ifdef T_DOUBLE
#undef T_DOUBLE
#endif
 
typedef enum		/* TYPE */
    {
    T_UNKNOWN,
    T_BYTE,
    T_WORD,
    T_INT,
    T_FLOAT,
    T_DOUBLE,
    T_HOST
    } TYPE;

typedef enum		/* SIDE */
    {
    LHS,
    RHS,
    FHS			/* function: rhs -> lhs */
    } SIDE;

typedef struct		/* VALUE */
    {
    SIDE side;
    TYPE type;
    union
	{
	int *lv;	/* pointer to any of the below */

	char byte;
	short word;
	int rv;
	char *string;
	float fp;
	double dp;
	} value;
    } VALUE;


/* variable declarations */

extern BOOL 		shellInteractive;	/* is shell interactive */
extern int		shellRedirInFd;		/* shell redirection fd */
extern int		shellRedirOutFd;	/* shell redirection fd */
extern BOOL	 	shellPrintResult;	/* print result after parsing */
extern int 		shellResult;		/* last result after parsing */
extern SHELL_OPS	shellOps;		/* shell virtual functions */


/* function declarations */

extern STATUS shell
    (
    BOOL		interactive,
    char *		promptmeval,
    struct timeval *	pTimeout,
    void		(*callback) (void)
    );

extern STATUS shellExec
    (
    BOOL	interactive
    );

extern STATUS shellExecLine
    (
    char *	line,
    BOOL interactive
    );

extern void shellHistory
    (
    int		size
    );

extern STATUS shellInit
    (
    BOOL printResult
    );

extern void shellOpsSet
    (
    SHELL_OPS *	operations
    );

extern STATUS shellParse
    (
    char *	line,
    int		fdIn,
    int		fdOut,
    int		fdErr,
    int *	pRes
    );

extern void shellPromptSet
    (
    char *	newPrompt
    );

extern void yystart
    (
    char *	line
    );

extern int yyparse (void);

#ifdef __cplusplus
}
#endif

#endif /* __INCshellh */
