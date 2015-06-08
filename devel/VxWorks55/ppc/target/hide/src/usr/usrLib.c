/* usrLib.c - user interface subroutine library */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
09d,14may02,fmk  add support for common symbols and load flags to ld
09c,10may02,pcm  changed sp help message from opt=0 to opt=0x19
09b,08mar02,jn   add error message for ld failing due to undefined symbols
09a,16oct01,jn   use symFindSymbol for symbol lookup (SPR #7453)
08z,16oct01,gls  update printLogo() copyright notice to 2002
                 changed printLogo() and version() to use 
		 runtimeName and runtimeVersion
08y,01oct01,dcb  Fix SPR 21134.  Change code to remove compiler warnings.
08x,03apr01,kab  Fixed _WRS_ALTIVEC_SUPPORT
08w,28mar01,dtr  Putting in #if _WRS_ALTIVEC_SUPPORT.
08v,19mar01,dtr  Changing header file name altiVecLib.h .
08u,29jan01,dtr  Adding support for altivec register set.
08v,18feb99,jpd  Changed 1998 to 1999 in printLogo
08u,13nov98,cdp  make periodRun, repeatRun force Thumb state for all ARM CPUs
		 with ARM_THUMB==TRUE.
08v,27feb01,hk   update printLogo() copyright notice to cover 2001.
08u,21jun00,rsh  upgrade to dosFs 2.0
09d,31jul99,jkf  T2 merge, tidiness & spelling, 1998-2000.
09c,29sep98,lrn  merge DosFs 2.0 changes into T2.0 base
09b,14jul98,lrn  removed all RT-11 references, rt11FsLib is obsolete,
		 refuning configuration issues
09a,30jun98,lrn  moved I/O functions to usrFsLib.c, fixed warnings
08s,08mar98,ms   Changed 1997 to 1998 in printLogo
08r,14jul97,dgp  doc: add windsh x-ref to bootChange(), cd(), help(), ls(), 
		      mRegs(), & printErrno(), change ^D to CTRL-D in copy()
08t,09oct97,cdp  added ARM7TDMI_T (Thumb) support.
08s,26feb97,p_m  Changed 1996 to 1997 in printLogo (SPR# 8068).
		 removed pre 1995 history.
08r,10jan97,cdp  changed copyright to 1997.
08q,28nov96,cdp  added ARM support.
08p,13aug96,dbt  added test in changeFpReg to exit if string = "." (SPR #6740).
08o,09aug96,dbt  modified copy to correctly close file descriptors (SPR #1308).
08n,19jun96,dgp  doc: changed note on ll() with netDrv (SPR #6560), fixed 
		 rev nos.
08m,09may96,myz  removed #if statement for i960 in checkStack routine
08l,17jan96,kkk	 changed copyright year from 1995 to 1996.
08k,06oct95,jdi  doc: changed Debugging .pG's to .tG "Shell", updated pathnames
		 for Tornado.
08j,12jun95,p_m  added reference to spyLib in man pages.
08i,27may95,p_m  added spy(), spyHelp(), spyReport(), spyClkStart()
		 spyClkStop(), spyStop() and spyTask().
08h,01may95,kkk	 fixed incorrect modifications of mRegs (SPR #4241)
08g,30mar95,kdl  removed obsolete stat structure fields; removed unused d() var.
08f,24mar95,tpr  removed INTERRUPT line in checkStack() for the 060 (SPR #4107).
08e,20mar95,jdi  doc tweaks to d() and l().
08d,03mar95,jdi  fixed typo in man page for sp().
08c,10feb95,jdi  doc format tweaks.  Made substrcmp() NOMANUAL.
08b,18jan95,rhp  Doc: explain cd() does no checking for paths over netDrv.
08a,06jan95,kdl  Changed copyright to 1995.
*/

/*
This library consists of routines meant to be executed from the VxWorks
shell.  It provides useful utilities for task monitoring and execution,
system information, symbol table management, etc.

Many of the routines here are simply command-oriented interfaces to more
general routines contained elsewhere in VxWorks.  Users should feel free
to modify or extend this library, and may find it preferable to customize
capabilities by creating a new private library, using this one as a model,
and appropriately linking the new one into the system.

Some routines here have optional parameters.  If those parameters are zero,
which is what the shell supplies if no argument is typed, default
values are typically assumed.

A number of the routines in this module take an optional task name or ID
as an argument.  If this argument is omitted or zero, the "current" task
is used.  The current task (or "default" task) is the last task
referenced.  The usrLib library uses taskIdDefault() to set and get the
last-referenced task ID, as do many other VxWorks routines.

INCLUDE FILES: usrLib.h

SEE ALSO: usrFsLib, tarLib, spyLib,
.pG "Target Shell,"
windsh,
.tG "Shell"
*/

/* LINTLIBRARY */

#include "vxWorks.h"
#if CPU_FAMILY!=I960
#include "a_out.h"
#else	/* CPU_FAMILY!=I960 */
#include "b_out.h"
#endif	/* CPU_FAMILY!=I960 */
#include "ctype.h"
#include "stdio.h"
#include "ioLib.h"
#include "loadLib.h"
#include "memLib.h"
#include "remLib.h"
#include "string.h"
#include "symLib.h"
#include "ftpLib.h"
#include "sysLib.h"
#include "sysSymTbl.h"
#include "usrLib.h"
#include "version.h"
#include "fppLib.h"
#include "dirent.h"
#include "sys/stat.h"
#include "errnoLib.h"
#include "fcntl.h"
#include "unistd.h"
#include "fioLib.h"
#include "shellLib.h"
#include "vxLib.h"
#include "hostLib.h"
#include "pathLib.h"
#include "iosLib.h"
#include "taskArchLib.h"
#include "kernelLib.h"
#include "logLib.h"
#include "usrLib.h"
#include "objLib.h"
#include "private/cplusLibP.h"
#include "netDrv.h"
#include "smObjLib.h"
#include "time.h"
#include "private/taskLibP.h"
#include "private/funcBindP.h"
#include "nfsLib.h"
#include "dosFsLib.h"
#ifdef _WRS_ALTIVEC_SUPPORT
#include "altivecLib.h"
#endif /* _WRS_ALTIVEC_SUPPORT */
#define MAX_BYTES_PER_LINE       16

#define MAXLINE         80	/* max line length for input to 'm' routine */
#define MAX_DSP_TASKS	500	/* max tasks that can be displayed */
#define MAX_DATE_STRING 21	/* dates have 21 characters */

#define SPY_NOT_SUPPORTED_MSG   "spy not configured into this system.\n"

#define USR_DEMANGLE_PRINT_LEN 256  /* Num chars of demangled names to print */

/* sp parameters: change the help message, if these change! */

int spTaskPriority	= 100;
int spTaskOptions	= VX_SUPERVISOR_MODE | VX_FP_TASK | VX_STDIO;
int spTaskStackSize	= 20000;

/* other globals */

BOOL ldCommonMatchAll = TRUE;

/* symbol types in system symbol table */

LOCAL char *typeName [] =
    {
    "????",
    "abs",
    "text",
    "data",
    "bss",
    };

/* things used by lkAddr, lkAddrFind & lkAddrNext */

#define NUM_SYMBLS	3

typedef struct		/* SYMBL - symbol table entry used by lkAddr */
    {
    unsigned int addr;	/* address associated with symbol */
    char *name;		/* points to name in system symbol table */
    SYM_TYPE type;	/* type of this symbol table entry */
    } SYMBL;

typedef struct		/* LKADDR_ARG */
    {
    int count;			/* number of entries printed by lkAddr */
    unsigned int addr;		/* target address */
    SYMBL symbl[NUM_SYMBLS];
    } LKADDR_ARG;


/* externals */

IMPORT int sysClkRateGet (void);
IMPORT char * vxRegIntStackBase;	/* base of register interrupt stack */
IMPORT char * vxMemIntStackBase;	/* base of memory interrupt stack */

#if (CPU_FAMILY == ARM)
IMPORT char *vxSvcIntStackBase;		/* SVC-mode stack for int code */
IMPORT char *vxSvcIntStackEnd;
IMPORT char *vxIrqIntStackBase;		/* IRQ-mode stack for int code */
IMPORT char *vxIrqIntStackEnd;
#endif /* CPU_FAMILY == ARM */


/* forward declarations */

LOCAL BOOL lkAddrFind (char *name, unsigned int value, SYM_TYPE type,
		       LKADDR_ARG *arg);
LOCAL BOOL lkAddrNext (char *name, unsigned int value, SYM_TYPE type,
		       LKADDR_ARG *arg);
LOCAL BOOL lkAddrPrintSame (char *name, unsigned int value, SYM_TYPE type,
		            LKADDR_ARG *arg);
LOCAL void printStackSummary (TASK_DESC *pTd);
LOCAL void printSTE (unsigned int addr, char *name, SYM_TYPE type);
LOCAL STATUS changeFpReg (char *pPrompt, double value, double *pValue);

STATUS changeReg (char *pPrompt, void *value, int width);

/*******************************************************************************
*
* help - print a synopsis of selected routines
*
* This command prints the following list of the calling sequences for
* commonly used routines, mostly contained in usrLib.
* .CS
* help                       Print this list
* ioHelp                     Print I/O utilities help info
* dbgHelp                    Print debug help info
* nfsHelp                    Print nfs help info
* netHelp                    Print network help info
* spyHelp                    Print task histogrammer help info
* timexHelp                  Print execution timer help info
* h         [n]              Print (or set) shell history
* i         [task]           Summary of tasks' TCBs
* ti        task             Complete info on TCB for task
* sp        adr,args...      Spawn a task, pri=100, opt=0x19, stk=20000
* taskSpawn name,pri,opt,stk,adr,args... Spawn a task
* td        task             Delete a task
* ts        task             Suspend a task
* tr        task             Resume a task
* d         [adr[,nunits[,width]]]   Display memory
* m         adr[,width]      Modify memory
* mRegs     [reg[,task]]     Modify a task's registers interactively
* pc        [task]           Return task's program counter
* version                    Print VxWorks version info, and boot line
* iam     "user"[,"passwd"]  Set user name and passwd
* whoami                     Print user name
* devs                       List devices
* ld        [syms[,noAbort][,"name"]] Load std in into memory
*                              (syms = add symbols to table:
*                               -1 = none, 0 = globals, 1 = all)
* lkup      ["substr"]       List symbols in system symbol table
* lkAddr    address          List symbol table entries near address
* checkStack  [task]         List task stack sizes and usage
* printErrno  value          Print the name of a status value
* period    secs,adr,args... Spawn task to call function periodically
* repeat    n,adr,args...    Spawn task to call function n times
*                              (0=forever)
* \&NOTE:  Arguments specifying <task> can be either task ID or name.
* .CE
*
* RETURNS: N/A
*
* SEE ALSO:
* .pG "Target Shell"
* windsh,
* .tG "Shell"
*/

void help (void)
    {
    static char *help_msg [] = {
    /* page 1 */
    "help                           Print this list",
    "ioHelp                         Print I/O utilities help info",
    "dbgHelp                        Print debugger help info",
    "nfsHelp                        Print nfs help info",
    "netHelp                        Print network help info",
    "spyHelp                        Print task histogrammer help info",
    "timexHelp                      Print execution timer help info",
    "h         [n]                  Print (or set) shell history",
    "i         [task]               Summary of tasks' TCBs",
    "ti        task                 Complete info on TCB for task",
    "sp        adr,args...          Spawn a task, pri=100, opt=0x19, stk=20000",
    "taskSpawn name,pri,opt,stk,adr,args... Spawn a task",
    "td        task                 Delete a task",
    "ts        task                 Suspend a task",
    "tr        task                 Resume a task",
    "d         [adr[,nunits[,width]]] Display memory",
    "m         adr[,width]          Modify memory",
    "mRegs     [reg[,task]]         Modify a task's registers interactively",
    "pc        [task]               Return task's program counter",
    "iam       \"user\"[,\"passwd\"]  	Set user name and passwd",
    "whoami                         Print user name",
    /* page 2 */
    "devs                           List devices",
    "ld        [syms[,noAbort][,\"name\"]] Load stdin, or file, into memory",
    "                               (syms = add symbols to table:",
    "                               -1 = none, 0 = globals, 1 = all)",
    "lkup      [\"substr\"]         List symbols in system symbol table",
    "lkAddr    address              List symbol table entries near address",
    "checkStack  [task]             List task stack sizes and usage",
    "printErrno  value              Print the name of a status value",
    "period    secs,adr,args... Spawn task to call function periodically",
    "repeat    n,adr,args...    Spawn task to call function n times (0=forever)",
    "version                        Print VxWorks version info, and boot line",
    "",
    "NOTE:  Arguments specifying 'task' can be either task ID or name.",
    NULL
    };
    FAST int ix;
    char ch;

    printf ("\n");
    for (ix = 0; help_msg [ix] != NULL; ix++)
	{
	if ((ix+1) % 20 == 0)
	    {
	    printf ("\nType <CR> to continue, Q<CR> to stop: ");
	    fioRdString (STD_IN, &ch, 1);
	    if (ch == 'q' || ch == 'Q')
		break;
	    else
		printf ("\n");
	    }
	printf ("%s\n", help_msg [ix]);
	}
    printf ("\n");
    }
/*******************************************************************************
*
* netHelp - print a synopsis of network routines
*
* This command prints a brief synopsis of network facilities that are
* typically called from the shell.
* .CS
* hostAdd      "hostname","inetaddr" - add a host to remote host table;
*                                      "inetaddr" must be in standard
*                                      Internet address format e.g. "90.0.0.4"
* hostShow                           - print current remote host table
* netDevCreate "devname","hostname",protocol
*                                    - create an I/O device to access
*                                      files on the specified host
*                                      (protocol 0=rsh, 1=ftp)
* routeAdd     "destaddr","gateaddr" - add route to route table
* routeDelete  "destaddr","gateaddr" - delete route from route table
* routeShow                          - print current route table
* iam          "usr"[,"passwd"]      - specify the user name by which
*                                      you will be known to remote
*                                      hosts (and optional password)
* whoami                             - print the current remote ID
* rlogin       "host"                - log in to a remote host;
*                                      "host" can be inet address or
*                                      host name in remote host table
*
* ifShow       ["ifname"]            - show info about network interfaces
* inetstatShow                       - show all Internet protocol sockets
* tcpstatShow                        - show statistics for TCP
* udpstatShow                        - show statistics for UDP
* ipstatShow                         - show statistics for IP
* icmpstatShow                       - show statistics for ICMP
* arptabShow                         - show a list of known ARP entries
* mbufShow                           - show mbuf statistics
*
* \&EXAMPLE:  -> hostAdd "wrs", "90.0.0.2"
*           -> netDevCreate "wrs:", "wrs", 0
*           -> iam "fred"
*           -> copy <wrs:/etc/passwd   /@ copy file from host "wrs" @/
*           -> rlogin "wrs"            /@ rlogin to host "wrs"      @/
* .CE
*
* RETURNS: N/A
*
* SEE ALSO:
* .pG "Target Shell"
*/

void netHelp (void)

    {
 static char *help_msg [] =
 {
    /* page 1 */
 "hostAdd   \"hostname\",\"inetaddr\"    - add a host to remote host table;",
 "                                     \"inetaddr\" must be in standard",
 "                                     Internet address format e.g. \"90.0.0.4\"",
 "hostShow                           - print current remote host table",
 "netDevCreate \"devname\",\"hostname\",protocol",
 "                                   - create an I/O device to access files", "                                     on the specified host",
 "                                     (protocol 0=rsh, 1=ftp)",
 "routeAdd     \"destaddr\",\"gateaddr\" - add route to route table",
 "routeDelete  \"destaddr\",\"gateaddr\" - delete route from route table",
 "",
 "routeShow                          - print current route table",
 "iam          \"usr\"[,\"passwd\"]      - specify the user name by which you",
 "                                     will be known to remote hosts",
 "                                     (and optional password)",
 "whoami                             - print the current remote ID",
 "rlogin       \"host\"                - log in to a remote host;",
 "                                     \"host\" can be inet address or",
 "                                     host name in remote host table",
 "",
 "",
 /* page 2 */
 "ifShow       [\"ifname\"]            - show info about network interfaces",
 "inetstatShow                       - show all Internet protocol sockets",
 "tcpstatShow                        - show statistics for TCP",
 "udpstatShow                        - show statistics for UDP",
 "ipstatShow                         - show statistics for IP",
 "icmpstatShow                       - show statistics for ICMP",
 "arptabShow                         - show a list of known ARP entries",
 "mbufShow                           - show mbuf statistics",
 "",
 "EXAMPLE:   -> hostAdd \"wrs\", \"90.0.0.2\"",
 "           -> netDevCreate \"wrs:\", \"wrs\", 0",
 "           -> iam \"fred\"",
 "           -> copy <wrs:/etc/passwd   /* copy file from host \"wrs\" */",
 "           -> rlogin \"wrs\"            /* rlogin to host \"wrs\" */",
 NULL
 };
    FAST int i;
    char ch;

    printf ("\n\n");
    for (i = 0; help_msg [i] != NULL; i++)
	{
	if ((i+1) % 20 == 0)
	    {
	    printf ("\nType <CR> to continue, Q<CR> to stop: ");
	    fioRdString (STD_IN, &ch, 1);
	    if (ch == 'q' || ch == 'Q')
		break;
	    else
		printf ("\n");
	    }
	printf ("%s\n", help_msg [i]);
	}
    printf ("\n");
    }
/*******************************************************************************
*
* bootChange - change the boot line
*
* This command changes the boot line used in the boot ROMs.  This is useful
* during a remote login session.  After changing the boot parameters, you
* can reboot the target with the reboot() command, and then terminate your
* login ( ~. ) and remotely log in again.  As soon as the system has
* rebooted, you will be logged in again.
*
* This command stores the new boot line in non-volatile RAM, if the target
* has it.
*
* RETURNS: N/A
*
* SEE ALSO:
* windsh,
* .tG "Shell"
*/

void bootChange (void)

    {
    bootParamsPrompt (sysBootLine);
    (void)sysNvRamSet (sysBootLine, strlen (sysBootLine) + 1, 0);
    }
/*******************************************************************************
*
* periodRun - call a function periodically
*
* This command repeatedly calls a specified function, with up to eight of its
* arguments, delaying the specified number of seconds between calls.
*
* Normally, this routine is called only by period(), which spawns
* it as a task.
*
* RETURNS: N/A
*
* SEE ALSO: period(),
* .pG "Target Shell"
*/

void periodRun
    (
    int		secs,		/* no. of seconds to delay between calls */
    FUNCPTR	func,		/* function to call repeatedly */
    int		arg1,		/* first of eight args to pass to func */
    int		arg2,		
    int		arg3,	
    int		arg4,
    int		arg5,
    int		arg6,
    int		arg7,
    int		arg8
    )
    {
#if ((CPU_FAMILY == ARM) && ARM_THUMB)
    func = (FUNCPTR)((UINT32)func | 1);	/* force Thumb state */
#endif

    FOREVER
	{
	(* func) (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);

	taskDelay (secs * sysClkRateGet ());
	}
    }
/*******************************************************************************
*
* period - spawn a task to call a function periodically
*
* This command spawns a task that repeatedly calls a specified function,
* with up to eight of its arguments, delaying the specified number of
* seconds between calls.
*
* For example, to have i() display task information every 5 seconds,
* just type:
* .CS
*     -> period 5, i
* .CE
* NOTE
* The task is spawned using the sp() routine.  See the description
* of sp() for details about priority, options, stack size, and task ID.
*
* RETURNS: A task ID, or ERROR if the task cannot be spawned.
*
* SEE ALSO: periodRun(), sp(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

int period
    (
    int		secs,		/* period in seconds	       */
    FUNCPTR	func,		/* function to call repeatedly */
    int		arg1,		/* first of eight args to pass to func */
    int		arg2,
    int		arg3,
    int		arg4,
    int		arg5,
    int		arg6,
    int		arg7,
    int		arg8
    )
    {
    return (sp ((FUNCPTR)periodRun, secs, (int)func, arg1, arg2, arg3, arg4,
				       arg5, arg6, arg7));
    }
/*******************************************************************************
*
* repeatRun - call a function repeatedly
*
* This command calls a specified function <n> times, with up to eight of its
* arguments.  If <n> is 0, the routine is called endlessly.
*
* Normally, this routine is called only by repeat(), which spawns it as a
* task.
*
* RETURNS: N/A
*
* SEE ALSO: repeat(),
* .pG "Target Shell"
*/

void repeatRun
    (
    FAST int	 n,		/* no. of times to call func (0=forever) */
    FAST FUNCPTR func,		/* function to call repeatedly         */
    int		arg1,		/* first of eight args to pass to func */
    int		arg2,
    int		arg3,
    int		arg4,
    int		arg5,
    int		arg6,
    int		arg7,
    int		arg8
    )
    {
    FAST BOOL infinite = (n == 0);

#if ((CPU_FAMILY == ARM) && ARM_THUMB)
    func = (FUNCPTR)((UINT32)func | 1);	/* force Thumb state */
#endif

    while (infinite || (--n >= 0))
	(* func) (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
/*******************************************************************************
*
* repeat - spawn a task to call a function repeatedly
*
* This command spawns a task that calls a specified function <n> times, with
* up to eight of its arguments.  If <n> is 0, the routine is called
* endlessly, or until the spawned task is deleted.
*
* NOTE
* The task is spawned using sp().  See the description of sp() for details
* about priority, options, stack size, and task ID.
*
* RETURNS: A task ID, or ERROR if the task cannot be spawned.
*
* SEE ALSO: repeatRun(), sp(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

int repeat
    (
    FAST int	 n,		/* no. of times to call func (0=forever) */
    FAST FUNCPTR func,		/* function to call repeatedly         */
    int		arg1,		/* first of eight args to pass to func */
    int		arg2,
    int		arg3,
    int		arg4,
    int		arg5,
    int		arg6,
    int		arg7,
    int		arg8
    )
    {
    return (sp ((FUNCPTR)repeatRun, n, (int)func, arg1, arg2, arg3, arg4,
				    arg5, arg6, arg7));
    }
/*******************************************************************************
*
* sp - spawn a task with default parameters
*
* This command spawns a specified function as a task with the following
* defaults:
* .IP "priority:" 15
* 100
* .IP "stack size:"
* 20,000 bytes
* .IP "task ID:"
* highest not currently used
* .IP "task options:"
* VX_FP_TASK - execute with floating-point coprocessor support.
* .IP "task name:"
* A name of the form `tN' where N is an integer which increments as new tasks
* are spawned, e.g., `t1', `t2', `t3', etc.
* .LP
* 
* The task ID is displayed after the task is spawned.
*
* This command is a short form of the underlying taskSpawn() routine,
* convenient for spawning tasks in which the default parameters
* are satisfactory.  If the default parameters are unacceptable, taskSpawn()
* should be called directly.
*
* RETURNS: A task ID, or ERROR if the task cannot be spawned.
*
* SEE ALSO: taskLib, taskSpawn(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*
* VARARGS1
*/

int sp
    (
    FUNCPTR	func,		/* function to call       */
    int		arg1,		/* first of nine args to pass to spawned task */
    int		arg2,
    int		arg3,
    int		arg4,
    int		arg5,
    int		arg6,
    int		arg7,
    int		arg8,
    int		arg9
    )
    {
    int id;

    if (func == (FUNCPTR)0)
	{
	printf ("sorry, won't spawn task at PC = 0.\n");
	return (ERROR);
	}

    /* spawn task, let taskSpawn pick the task ID, report success or failure */

    id = taskSpawn ((char *) NULL, spTaskPriority, spTaskOptions,
		    spTaskStackSize, func, arg1, arg2, arg3, arg4, arg5,
		    arg6, arg7, arg8, arg9, 0);

    if (id == ERROR)
	{
	printf ("not able to spawn task.\n");
	return (ERROR);
	}
    else
	{
	printf ("task spawned: id = %#x, name = %s\n", id, taskName (id));
	return (id);
	}
    }
/******************************************************************************
*
* taskIdFigure - translate a task name or ID to a task ID
*
* Many routines in usrLib take a task ID (an integer) or a task name.  This
* routine determines whether the parameter is a task name or a task ID and
* returns the task ID.  If <taskNameOrId> is omitted or 0, the current task
* is used.
*
* RETURNS: A task ID (an integer), or ERROR.
*
* SEE ALSO: taskNameToId(), taskLib
*
* NOMANUAL
*/

int taskIdFigure
    (
    int taskNameOrId		/* task name or task ID */
    )
    {
    char name [10];	/* number to name */
    int tid;		/* task ID */
    int bb;		/* bit bucket */

    if (taskNameOrId == 0)				/* default task ID */
	return (taskIdDefault (0));
    else if (taskIdVerify (taskNameOrId) == OK)	/* try explicit ID */
	return (taskNameOrId);

    sprintf (name, "%x", taskNameOrId);

    if ((tid = taskNameToId (name)) != ERROR)
	return (tid);				/* name is a number */
    else if (vxMemProbe ((char *)taskNameOrId, O_RDONLY, 4, (char *)&bb) == OK)
	return (taskNameToId ((char *) taskNameOrId)); /* name is a string */
    else
	return (ERROR);			/* unreasonable name, bus error! */
    }
/*******************************************************************************
*
* checkStack - print a summary of each task's stack usage
*
* This command displays a summary of stack usage for a specified task, or
* for all tasks if no argument is given.  The summary includes the total
* stack size (SIZE), the current number of stack bytes used (CUR), the
* maximum number of stack bytes used (HIGH), and the number of bytes never
* used at the top of the stack (MARGIN = SIZE - HIGH).
* For example:
* .CS
*     -> checkStack tShell
*
*         NAME         ENTRY      TID    SIZE   CUR  HIGH  MARGIN
*     ------------ ------------ -------- ----- ----- ----- ------
*     tShell       _shell       23e1c78   9208   832  3632   5576
* .CE
*
* The maximum stack usage is determined by scanning down from the top of the
* stack for the first byte whose value is not 0xee.  In VxWorks, when a task
* is spawned, all bytes of a task's stack are initialized to 0xee.
*
* DEFICIENCIES
* It is possible for a task to write beyond the end of its stack, but
* not write into the last part of its stack.  This will not be detected
* by checkStack().
*
* RETURNS: N/A
*
* SEE ALSO: taskSpawn(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void checkStack
    (
    int taskNameOrId		/* task name or task ID; 0 = summarize all */
    )
    {
    FAST int	nTasks;			/* number of task */
    FAST int	ix;			/* index */
    FAST char  *pIntStackHigh;		/* high interrupt stack usage */
    int		tid;			/* task ID */
    TASK_DESC	td;			/* task info structure */
    int		idList [MAX_DSP_TASKS];	/* list of active IDs */
    static char checkStackHdr [] = "\
  NAME        ENTRY        TID     SIZE   CUR  HIGH  MARGIN\n\
------------ ------------ -------- ----- ----- ----- ------\n";

#if	(CPU_FAMILY == ARM)

    char *intStackBase;
    char *intStackEnd;

#else  /* (CPU_FAMILY == ARM) */

#   if (CPU != MC68060)		/* we dont currently use this for MC68060 */
    IMPORT char *vxIntStackBase;
#   endif /* CPU != MC68060 */

    IMPORT char *vxIntStackEnd;

#   if    (CPU != MC68060)		/* we dont currently use this for MC68060 */
    char *intStackBase = vxIntStackBase;
#   endif /* CPU != MC68060 */

    char *intStackEnd  = vxIntStackEnd;

#endif	/* CPU_FAMILY == ARM */

    if (taskNameOrId != 0)
	{
	/* do specified task */

	tid = taskIdFigure (taskNameOrId);

	if ((tid == ERROR) || (taskInfoGet (tid, &td) != OK))
	    printErr ("Task not found.\n");
	else
	    {
	    printf (checkStackHdr);
	    printStackSummary (&td);
	    }
	}
    else
	{
	/* do all tasks */

	printf (checkStackHdr);

	nTasks = taskIdListGet (idList, NELEMENTS (idList));
	taskIdListSort (idList, nTasks);

	for (ix = 0; ix < nTasks; ++ix)
	    if (taskInfoGet (idList [ix], &td) == OK)
		printStackSummary (&td);

	/* find int stack high usage */

#if (CPU_FAMILY != AM29XXX) && (CPU_FAMILY != ARM)
#if 	(_STACK_DIR == _STACK_GROWS_DOWN)
	for (pIntStackHigh = intStackEnd; * (UINT8 *)pIntStackHigh == 0xee;
	     pIntStackHigh ++)
	     ;
#else	/* _STACK_DIR == _STACK_GROWS_DOWN) */
	for (pIntStackHigh = intStackEnd - 1; * (UINT8 *)pIntStackHigh == 0xee;
	     pIntStackHigh --)
	     ;
#endif	/* _STACK_DIR == _STACK_GROWS_DOWN) */

#if (CPU != MC68060)
	printf ("%-12.12s %-12.12s", "INTERRUPT", "");
	printf (" %8s %5d %5d %5d %6d %s\n",
                 "",						/* task name */
                 (int)((intStackEnd - intStackBase) *_STACK_DIR),	/* stack size */
                 0,						/* current */
                 (int)((pIntStackHigh - intStackBase) *_STACK_DIR),
							  /* high stack usage */
                 (int)((intStackEnd - pIntStackHigh) * _STACK_DIR),	/* margin */
                 (pIntStackHigh == intStackEnd) &&		/* overflow ? */
		 (intStackEnd != intStackBase != 0) ? /* no interrupt stack ? */
                 "OVERFLOW" : "");
#endif 

#else
#if	CPU_FAMILY == AM29XXX
       /*
	* Since the Am29k family uses 3 interrupt stacks (Exception Stack
	* Frame, handler register stack and memory stack), we must compute
	* each stack usage inside the global stack area allocated at each
	* interrupt. The ESF takes 1/5 of the global area, the register stack
	* takes also 1/5 of the global area, and the memory stack takes the
	* remainder (3/5). See kernelInit().
	*/

	for (pIntStackHigh = vxRegIntStackBase;
	     * (UINT8 *)pIntStackHigh == 0xee; pIntStackHigh ++);

	printf ("%-12.12s %-12.12s", "INTERRUPT", "");
	printf (" %8s %5d %5d %5d %6d %s\n",
                "",					/* task name */
                (intStackBase - vxRegIntStackBase),	/* ESF size */
                0,					/* current */
                (intStackBase - pIntStackHigh),		/* high stack usage */
                (pIntStackHigh - vxRegIntStackBase),	/* margin */
                (pIntStackHigh == vxRegIntStackBase) &&	/* overflow ? */
		(intStackEnd != intStackBase != 0) ?	/* no intr stack ? */
                "Ker stack OVERFLOW" : "- Kernel   -");

	for (pIntStackHigh = vxMemIntStackBase;
	     (* (UINT8 *)pIntStackHigh == 0xee) &&
	     (pIntStackHigh != vxRegIntStackBase); pIntStackHigh ++);

	printf ("%-12.12s %-12.12s", "", "");
	printf (" %8s %5d %5d %5d %6d %s\n",
                "",					/* task name */
                (vxRegIntStackBase - vxMemIntStackBase),/* Reg stack size */
                0,					/* current */
                (vxRegIntStackBase - pIntStackHigh),	/* high stack usage */
                (pIntStackHigh - vxMemIntStackBase),	/* margin */
                (pIntStackHigh == vxMemIntStackBase) &&	/* overflow ? */
		(intStackEnd != intStackBase != 0) ?	/* no intr stack ? */
                "Reg stack OVERFLOW" : "- Register -");

	for (pIntStackHigh = intStackEnd;
	     (* (UINT8 *)pIntStackHigh == 0xee) &&
	     (pIntStackHigh != vxMemIntStackBase); pIntStackHigh ++);

	printf ("%-12.12s %-12.12s", "", "");
	printf (" %8s %5d %5d %5d %6d %s\n",
                "",					/* task name */
                (vxMemIntStackBase - intStackEnd),	/* Mem stack size */
                0,					/* current */
                (vxMemIntStackBase - pIntStackHigh),	/* high stack usage */
                (pIntStackHigh - intStackEnd),		/* margin */
                (pIntStackHigh == intStackEnd) &&	/* overflow ? */
		(intStackEnd != intStackBase != 0) ?	/* no intr stack ? */
                "Mem stack OVERFLOW" : "- Memory   -");

#else
#if	(CPU_FAMILY == ARM)
       /*                                       
	* The ARM processor has a separate stack for each of the two
	* interrupt modes, IRQ and FIQ, but FIQ is not handled within
	* VxWorks. The IRQ entry veneer switches to SVC mode (for
	* reentrancy) so there are still two interrupt stacks to print
	* out: IRQ and SVC mode.
        */                                      
                                                
        intStackBase = vxSvcIntStackBase;       
        intStackEnd = vxSvcIntStackEnd;         
        for (pIntStackHigh = intStackEnd;       
             *(UINT8 *)pIntStackHigh == 0xEE && pIntStackHigh < intStackBase;
             pIntStackHigh++)                   
            ;                                   
                                                
        printf ("%-12.12s %-12.12s", "INTERRUPT", "");
        printf (" %8s %5d %5d %5d %6d %s\n",    
                 "",                                    /* task name */
                 (int)(intStackBase - intStackEnd),     /* stack size */
                 0,                                     /* current */
                 (int)(intStackBase - pIntStackHigh),   /* high stack use */
                 (int)(pIntStackHigh - intStackEnd),    /* margin */
                 (pIntStackHigh == intStackEnd) &&      /* overflow ? */
                 (intStackEnd != intStackBase != 0) ?   /* no int stack ? */
                 "SVC stack OVERFLOW" : "- SVC -");
                                                
                                                
        intStackBase = vxIrqIntStackBase;       
        intStackEnd = vxIrqIntStackEnd;
        for (pIntStackHigh = intStackEnd;
             *(UINT8 *)pIntStackHigh == 0xEE && pIntStackHigh < intStackBase;
             pIntStackHigh++)
            ;
 
        printf ("%-12.12s %-12.12s", "", "");
        printf (" %8s %5d %5d %5d %6d %s\n",
                 "",                                    /* task name */
                 (int)(intStackBase - intStackEnd),     /* stack size */
                 0,                                     /* current */
                 (int)(intStackBase - pIntStackHigh),   /* high stack use */
                 (int)(pIntStackHigh - intStackEnd),    /* margin */
                 (pIntStackHigh == intStackEnd) &&      /* overflow ? */
                 (intStackEnd != intStackBase != 0) ?   /* no int stack ? */
                 "IRQ stack OVERFLOW" : "- IRQ -");
 
#endif	/* CPU_FAMILY == ARM */
#endif /* (CPU_FAMILY == AM29XXX) */
#endif /* (CPU_FAMILY != AM29XXX) */
	}
    }
/*******************************************************************************
*
* printStackSummary - print task stack summary line
*
* This command is used by checkStack(), to print each task's stack summary line.
*/

LOCAL void printStackSummary
    (
    TASK_DESC *pTd
    )
    {
    FUNCPTR   entry;			/* task's initial entry point */
    FUNCPTR   symboladdr;		/* address associated with 'name' */
    SYMBOL_ID symId;
    char      *name;       /* ptr to sym tbl copy of name of main routine */
    char      demangled[USR_DEMANGLE_PRINT_LEN + 1];
    char      *nameToPrint;

    /* find task's initial entry point, and name (if any) in symbol table */

    entry = pTd->td_entry;

    symFindSymbol (sysSymTbl, NULL, (void *)entry, 
		   N_EXT | N_TEXT, N_EXT | N_TEXT, &symId);
    symNameGet (symId, &name);
    symValueGet (symId, (void **)&symboladdr); 

    /* Print the summary of the TCB */

    printf ("%-12.12s", pTd->td_name);	/* print the name of the task */

    if (entry == symboladdr)		/* entry address (symbolic if poss.) */
        {
	nameToPrint = cplusDemangle (name, demangled, sizeof (demangled));
	printf (" %-12.12s", nameToPrint);
        }
    else
	printf (" %#-12.12x", (int) entry);

#if (CPU_FAMILY != AM29XXX)
    printf (" %-8x %5d %5d ", pTd->td_id, pTd->td_stackSize,
	    pTd->td_stackCurrent);

    if (pTd->td_options & VX_NO_STACK_FILL)
        printf ("%5s %6s VX_NO_STACK_FILL\n", "???", "???");
    else
	printf ("%5d %6d %s\n", pTd->td_stackHigh, pTd->td_stackMargin,
    		(pTd->td_stackMargin <= 0) ? "OVERFLOW" : "");
#else
    {
    FAST char *pStackHigh;
    UINT regStackEnd;		/* end of the register stack */
    int regStackSize;		/* size of the register stack */
    int regStackHigh;		/* highest position into the register stack */
    int regStackMargin;		/* margin left in the register stack */
    UINT memStackBase;		/* base of the memory stack */
    int memStackSize;		/* size of the memory stack */
    int memStackHigh;		/* highest position into the memory stack */
    int memStackMargin;		/* margin left in the memory stack */
    int memStackCurrent;	/* current position into the memory stack */
    REG_SET regSet;

    /*
     * Since the Am29k family uses 2 stacks (register stack and memory stack),
     * we must compute each stack usage inside the global stack area allocated
     * for each task. The register stack takes 1/4 of the global area, and the
     * memory stack takes the remainder (3/4). See taskRegsInit().
     */

    regStackEnd = (UINT)pTd->td_pStackBase - ((UINT)pTd->td_pStackBase - \
                  (UINT)pTd->td_pStackEnd) / 4;

    if (pTd->td_options & VX_NO_STACK_FILL)
        pStackHigh = (char *)regStackEnd;
    else
        for (pStackHigh = (char *)regStackEnd;
             *(UINT8 *)pStackHigh == 0xee; pStackHigh ++);

    regStackHigh = (int)(pTd->td_pStackBase - pStackHigh);
    regStackSize = (UINT)pTd->td_pStackBase - regStackEnd;
    regStackMargin = regStackSize - regStackHigh;

    printf (" %-8x %5d %5d ", pTd->td_id, regStackSize,
	    pTd->td_stackCurrent);

    if (pTd->td_options & VX_NO_STACK_FILL)
        printf ("%5s %6s VX_NO_STACK_FILL\n", "???", "???");
    else
	printf ("%5d %6d %s\n", regStackHigh, regStackMargin,
    		(regStackMargin <= 0) ? "Reg stack OVERFLOW" :
		"- Register -");

    printf ("                                  ");

    memStackBase = regStackEnd;

    if (pTd->td_options & VX_NO_STACK_FILL)
        pStackHigh = (char *)pTd->td_pStackLimit;
    else
        for (pStackHigh = (char *)pTd->td_pStackLimit;
             (*(UINT8 *)pStackHigh == 0xee) &&
             ((UINT)pStackHigh != memStackBase); pStackHigh ++);

    memStackHigh = (int)(memStackBase - (UINT)pStackHigh);
    memStackSize = memStackBase - (UINT)pTd->td_pStackLimit;
    memStackMargin = memStackSize - memStackHigh;

    taskRegsGet (pTd->td_id, &regSet);
    memStackCurrent = (int)(regSet.msp - memStackBase) * _STACK_DIR;

    if (memStackHigh < memStackCurrent)		/* if msp is decremented but */
	memStackHigh = memStackCurrent;		/* nothing is put in stack */

    printf (" %5d %5d ", memStackSize, memStackCurrent);

    if (pTd->td_options & VX_NO_STACK_FILL)
        printf ("%5s %6s VX_NO_STACK_FILL", "???", "???");
    else
	printf ("%5d %6d %s\n", memStackHigh, memStackMargin,
    		(memStackMargin <= 0) ? "Mem stack OVERFLOW" :
		"- Memory   -");

    }
#endif /* (CPU_FAMILY != AM29XXX) */
    }
/*******************************************************************************
*
* i - print a summary of each task's TCB
*
* This command displays a synopsis of all the tasks in the system.
* The ti() routine provides more complete information on a specific task.
*
* Both i() and ti() use taskShow(); see the documentation for taskShow() for 
* a description of the output format.
*
* EXAMPLE:
* .CS
* -> i
*
*    NAME       ENTRY     TID    PRI   STATUS    PC       SP    ERRNO DELAY
* ---------- ---------- -------- --- --------- ------- -------- ----- -----
* tExcTask   _excTask    20fcb00   0 PEND      200c5fc  20fca6c     0     0
* tLogTask   _logTask    20fb5b8   0 PEND      200c5fc  20fb520     0     0
* tShell     _shell      20efcac   1 READY     201dc90  20ef980     0     0
* tRlogind   _rlogind    20f3f90   2 PEND      2038614  20f3db0     0     0
* tTelnetd   _telnetd    20f2124   2 PEND      2038614  20f2070     0     0
* tNetTask   _netTask    20f7398  50 PEND      2038614  20f7340     0     0
* value = 57 = 0x39 = '9'
* .CE
*
* CAVEAT
* This command should be used only as a debugging aid, since the information
* is obsolete by the time it is displayed.
*
* RETURNS: N/A
*
* SEE ALSO: ti(), taskShow(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void i
    (
    int taskNameOrId		/* task name or task ID, 0 = summarize all */
    )
    {
    int tid;

    if (taskNameOrId == 0)
	show (taskIdSelf (), 2);		/* summarize all task's info */
    else
	{
	tid = taskIdFigure (taskNameOrId);

	if ((tid == ERROR) || (taskIdVerify (tid) != OK))
	    {
	    printErr ("Task not found.\n");
	    return;
	    }

	show (tid, 0);				/* summarize a task's info */
	}
    }
/*******************************************************************************
*
* ti - print complete information from a task's TCB
*
* This command prints the task control block (TCB) contents, including
* registers, for a specified task.  If <taskNameOrId> is omitted or zero,
* the last task referenced is assumed.
*
* The ti() routine uses taskShow(); see the documentation for taskShow() for 
* a description of the output format.
*
* EXAMPLE:
* The following shows the TCB contents for the shell task:
* .CS
* -> ti
*
*   NAME      ENTRY     TID    PRI  STATUS      PC       SP    ERRNO  DELAY
* ---------- --------- -------- --- --------- -------- -------- ------ -----
* tShell     _shell     20efcac   1 READY      201dc90  20ef980      0     0
*
* stack: base 0x20efcac  end 0x20ed59c  size 9532   high 1452   margin 8080
*
* options: 0x1e
* \&VX_UNBREAKABLE      VX_DEALLOC_STACK    VX_FP_TASK         VX_STDIO
*
*
* \&D0 =       0   D4 =       0   A0 =       0   A4 =        0
* \&D1 =       0   D5 =       0   A1 =       0   A5 =  203a084   SR =     3000
* \&D2 =       0   D6 =       0   A2 =       0   A6 =  20ef9a0   PC =  2038614
* \&D3 =       0   D7 =       0   A3 =       0   A7 =  20ef980
* value = 34536868 = 0x20efda4
* .CE
*
* RETURNS: N/A
*
* SEE ALSO:
* taskShow(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void ti
    (
    int taskNameOrId		/* task name or task ID; 0 = use default */
    )
    {
    int tid = taskIdFigure (taskNameOrId);

    if ((tid == ERROR) || (taskIdVerify (tid) != OK))
	{
	printErr ("Task not found.\n");
	return;
	}

    tid = taskIdDefault (tid);			/* set the default task ID */

    show (tid, 1);
    }
/*******************************************************************************
*
* show - print information on a specified object
*
* This command prints information on the specified object.  System objects
* include tasks, local and shared semaphores, local and shared message
* queues, local and shared memory partitions, watchdogs, and symbol tables.
* An information level is interpreted by the objects show routine on a class
* by class basis.  Refer to the object's library manual page for more
* information.
*
* RETURNS: N/A
*
* SEE ALSO: i(), ti(), lkup(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void show
    (
    int objId,			/* object ID */
    int level			/* information level */
    )
    {
    int	bb;	/* bit bucket for vxMemProbe */

    if (ID_IS_SHARED (objId))
	{
	if (vxMemProbe ((char *) SM_OBJ_ID_TO_ADRS (objId), VX_READ, 4, 
	                (char *) &bb) == ERROR)
	    {
	    printf ("Object not found.\n");
	    return;
	    }

	if (_func_smObjObjShow != (FUNCPTR) NULL)
	    {
	    (*_func_smObjObjShow) (objId, level);
	    return;
	    }
        else
	    {
	    printf("Show routine of this object not configured into system.\n");
	    return;
	    }
	}

    /* check for invalid show routine reference */

    if ((vxMemProbe ((char *) objId, VX_READ, 4, (char *) &bb) == ERROR) ||
	((((OBJ_ID) ((int)objId + taskClassId->coreOffset))->pObjClass 
	 != taskClassId) &&
         (vxMemProbe ((char *)((OBJ_ID)objId)->pObjClass,VX_READ,4,(char *)&bb)
	  == ERROR)))
	{
	printf ("Object not found.\n");
	}
    else if (objShow ((OBJ_ID) objId, level) != OK)
	{
	if (errno == S_objLib_OBJ_NO_METHOD)
	    printf("Show routine of this object not configured into system.\n");
	else
	    printf ("Object not found.\n");
	}
    }
/*******************************************************************************
*
* ts - suspend a task
*
* This command suspends the execution of a specified task.  It simply calls
* taskSuspend().
*
* RETURNS: N/A
*
* SEE ALSO: tr(), taskSuspend(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void ts
    (
    int taskNameOrId		/* task name or task ID */
    )
    {
    int tid = taskIdFigure (taskNameOrId);

    if (tid == ERROR)		/* no ID found */
	{
	printErr ("Task not found.\n");
	return;
	}

    if (tid == 0)		/* default task ID never set */
	printf ("sorry, the shell can't suspend itself.\n");
    else if (taskSuspend (tid) != OK)
	printErrno (0);
    }
/*******************************************************************************
*
* tr - resume a task
*
* This command resumes the execution of a suspended task.  It simply calls
* taskResume().
*
* RETURNS: N/A
*
* SEE ALSO: ts(), taskResume(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void tr
    (
    int taskNameOrId		/* task name or task ID */
    )
    {
    int tid = taskIdFigure (taskNameOrId);

    if (tid == ERROR)		/* no ID found */
	printErr ("Task not found.\n");
    else if (taskResume (tid) != OK)
	printErrno (0);
    }
/*******************************************************************************
*
* td - delete a task
*
* This command deletes a specified task.  It simply calls taskDelete().
*
* RETURNS: N/A
*
* SEE ALSO: taskDelete(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void td
    (
    int taskNameOrId		/* task name or task ID */
    )
    {
    int tid = taskIdFigure (taskNameOrId);

    if (tid == ERROR)		/* no such task ID */
	printErr ("Task not found.\n");
    else if (tid == 0)
	printf ("sorry, the shell can't delete itself.\n");
    else if (taskDelete (tid) != OK)
	printErrno (0);
    }
/*******************************************************************************
*
* version - print VxWorks version information
*
* This command prints the VxWorks version number, the date this copy of
* VxWorks was made, and other pertinent information.
*
* EXAMPLE
* .CS
* -> version
* VxWorks (for Mizar 7170) version 5.1
* Kernel: WIND version 2.1.
* Made on Tue Jul 27 20:26:23 CDT 1997.
* Boot line:
* enp(0,0)host:/usr/wpwr/target/config/mz7170/vxWorks e=90.0.0.50 h=90.0.0.4 u=target
* value = 1 = 0x1
* .CE
*
* RETURNS: N/A
*
* SEE ALSO:
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void version (void)

    {
    printf ("%s (for %s) version %s.\n", runtimeName, sysModel (), 
	    runtimeVersion);
    printf ("Kernel: %s.\n", kernelVersion ());
    printf ("Made on %s.\n", creationDate);
    printf ("Boot line:\n%s\n", sysBootLine);
    }
/*******************************************************************************
*
* getHex - convert a hex string into a 64 bit value
*
* This function converts a string containing hex digits into a binary value.
* The values is returned in the locations pointed to by pHiValue and pLoValue.
* These values can be concatenated together to produce a 64 bit value.
*
* RETURNS: OK or ERROR
*
* INTERNALS: Ideally this function should be replaced by a version of scanf
* that supports long longs.
* This function is derived from scanNum in fioLib.c
*
* NOMANUAL
*/

LOCAL STATUS getHex
    (
    char *pStr,		/* string to parse */
    ULONG *pHiValue,	/* where to store high part of result */
    ULONG *pLoValue	/* where to store low part of result */
    )
    {
    int         dig;                    /* current digit */
    BOOL        neg     = FALSE;        /* negative or positive? */
    FAST char * pCh     = pStr;         /* pointer to current character */
    FAST int    ch      = *pCh;         /* current character */
    FAST ULONG  hiValue = 0;		/* high part of value accumulator */
    FAST ULONG  loValue = 0;		/* low part of value accumulator */
 
    /* check for sign */
 
    if (ch == '+' || (neg = (ch == '-')))
	ch = *++pCh;
 
     /* check for optional or 0x */
 
    if (ch == '0')
	{
	ch = *++pCh;
	if (ch == 'x' || ch == 'X')
	    ch = *++pCh;
	}
 
    /* scan digits */
    while (ch != '\0')
        {
        if (isdigit (ch))
            dig = ch - '0';
        else if (islower (ch))
            dig = ch - 'a' + 10;
        else if (isupper (ch))
            dig = ch - 'A' + 10;
        else
            break;
 
        if (dig >= 16)
            break;
 
	/* assume that accumulator parts are 32 bits long */
	hiValue = (hiValue * 16) + (loValue >> 28);
	loValue = loValue * 16 + dig;
 
        ch = *++pCh;
        }
 
 
    /* check that we scanned at least one character */
 
    if (pCh == pStr) {
        return (ERROR);
	}
 
    /* return value to caller */
 
    if (neg) {
	/* assume 2's complement arithmetic */
	hiValue = ~hiValue;
	loValue = ~loValue;
	if (++loValue == 0)
	    hiValue++;
	}

    *pHiValue = hiValue;
    *pLoValue = loValue;

    return (ch != '\0' ? ERROR : OK);
    }
/*******************************************************************************
*
* m - modify memory
*
* This command prompts the user for modifications to memory in byte, short
* word, or long word specified by <width>, starting at the specified address.
* It prints each address and the current contents of that address, in turn.
* If <adrs> or <width> is zero or absent, it defaults to the previous value.
* The user can respond in one of several ways:
* .iP RETURN 11
* Do not change this address, but continue, prompting at the next address.
* .iP <number>
* Set the content of this address to <number>.
* .iP ". (dot)"
* Do not change this address, and quit.
* .iP EOF
* Do not change this address, and quit.
* .LP
* All numbers entered and displayed are in hexadecimal.
*
* RETURNS: N/A
*
* SEE ALSO: mRegs(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*
* INTERNAL: further improvement needed.  Add an additional paramater indicating
*            whether a read should be done or not before writing.
*/

void m
    (
    void *adrs,		/* address to change */
    int  width 		/* width of unit to be modified (1, 2, 4, 8) */
    )
    {
    static void *lastAdrs;	/* last location modified */
    static int  lastWidth = 2;	/* last width - default to 2 */
    char line[MAXLINE + 1];	/* leave room for EOS */
    char *pLine;		/* ptr to current position in line */
    ULONG hiValue;		/* high part of value found in line */
    ULONG loValue;		/* low part of value found in line */

    if (adrs != 0)		/* set default address */
	lastAdrs = adrs;

    if (width != 0)		/* check valid width and set the default */
	{
	if (width != 1 && width != 2 && width != 4 && width != 8)
	    width = 1;
	lastWidth = width;
	}

    /* round down to appropriate boundary */

    lastAdrs = (void *)((int)lastAdrs & ~(lastWidth - 1));

    for (;; lastAdrs = (void *)((int)lastAdrs + lastWidth))
	{
	/* prompt for substitution according to width */

	switch (lastWidth)
	    {
	    case 1:
	    	printf ("%08x:  %02x-", (int) lastAdrs, *(UINT8 *)lastAdrs);
	    break;
	    	case 2:
		printf ("%08x:  %04x-", (int) lastAdrs, *(USHORT *)lastAdrs);
		break;
	    case 4:
		printf ("%08x:  %08lx-", (int) lastAdrs, *(ULONG *)lastAdrs);
		break;
	    case 8:
#if _BYTE_ORDER==_LITTLE_ENDIAN
		printf ("%08x:  %08lx%08lx-", (int) lastAdrs, *((ULONG *)lastAdrs+1),*(ULONG *)lastAdrs);
#endif
#if _BYTE_ORDER==_BIG_ENDIAN
		printf ("%08x:  %08lx%08lx-", (int) lastAdrs, *(ULONG *)lastAdrs, *((ULONG *)lastAdrs+1));
#endif
		break;
	    default:
		printf ("%08x:  %08x-", (int) lastAdrs, *(UINT8 *)lastAdrs);
		break;
	    }

	/* get substitution value:
	 *   skip empty lines (CR only);
	 *   quit on end of file or invalid input;
	 *   otherwise put specified value at address
	 */

	if (fioRdString (STD_IN, line, MAXLINE) == EOF)
	    break;

	line[MAXLINE] = EOS;	/* make sure input line has EOS */

	for (pLine = line; isspace (*(u_char *)pLine); ++pLine)	
	/* skip leading spaces*/
	    ;

	if (*pLine == EOS)			/* skip field if just CR */
	    continue;

        if (getHex (pLine, &hiValue, &loValue) != OK)
	    break;

	/* assign new value */

	switch (lastWidth)
	    {
	    case 1:
		*(UINT8 *)lastAdrs = (UINT8) loValue;
		break;
	    case 2:
		*(USHORT *)lastAdrs = (USHORT) loValue;
		break;
	    case 4:
		*(ULONG *)lastAdrs = (ULONG) loValue;
		break;
	    case 8:
#if _BYTE_ORDER==_LITTLE_ENDIAN
		*(ULONG *)lastAdrs = (ULONG) loValue;
		*((ULONG *)lastAdrs+1) = (ULONG) hiValue;
#endif
#if _BYTE_ORDER==_BIG_ENDIAN
		*(ULONG *)lastAdrs = (ULONG) hiValue;
		*((ULONG *)lastAdrs+1) = (ULONG) loValue;
#endif
		break;
	    default:
		*(UINT8 *)lastAdrs = (UINT8) loValue;
		break;
	    }
	}

    printf ("\n");
    }
/*******************************************************************************
*
* d - display memory
*
* This command displays the contents of memory, starting at <adrs>.
* If <adrs> is omitted or zero, d() displays the next memory block, starting
* from where the last d() command completed.
*
* Memory is displayed in units specified by <width>.  If <nunits> is omitted
* or zero, the number of units displayed defaults to last use.  If
* <nunits> is non-zero, that number of units is displayed and that number
* then becomes the default.  If <width> is omitted or zero, it defaults
* to the previous value.  If <width> is an invalid number, it is set to 1.
* The valid values for <width> are 1, 2, 4, and 8.  The number of units d()
* displays is rounded up to the nearest number of full lines.
*
* RETURNS: N/A
*
* SEE ALSO: m(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void d
    (
    FAST void *adrs,	/* address to display (if 0, display next block */
    int nunits,		/* number of units to print (if 0, use default) */
    int width 		/* width of displaying unit (1, 2, 4, 8) */
    )
    {
    static int dNitems = 0x80;	/* default number of item to display */
    static int dWidth  = 2;	/* default width */
    static void *last_adrs = 0;	/* last location displayed */

    FAST int item;		/* item counter displayed per line */
    char ascii [MAX_BYTES_PER_LINE + 1]; /* ascii buffer for displaying */
    int ix;			/* temporary count */
    UINT8 *pByte;		/* byte pointer for filling ascii buffer */
    UINT8 *tmpByte;		/* temporary byte pointer */
    USHORT *tmpShort;		/* temporary short word pointer */
    ULONG *tmpLong;		/* temporary long word pointer */

    ascii [MAX_BYTES_PER_LINE] = EOS;	/* put an EOS on the string */

    if (nunits == 0)
	nunits = dNitems;	/* no count specified: use default count */
    else
	dNitems = nunits;	/* change default count */

    if (width == 0)
	width = dWidth;
    else
	{			/* check for valid width */
	if (width != 1 && width != 2 && width != 4 && width != 8)
	    width = 1;
	dWidth = width;
	}

    if (adrs == 0) 		/* no address specified: use last address */
	adrs = last_adrs;
    else
	last_adrs = adrs;

    /* round address down to appropriate boundary */

    last_adrs = (void *)((int) last_adrs & ~(width - 1));

    /* print leading spaces on first line */

    bfill (ascii, 16, '.');

    printf ("%08x:  ", (int) last_adrs & ~0xf);

    for (item = 0; item < ((int) last_adrs & 0xf) / width; item++)
	{
	printf ("%*s ", 2*width, " ");
	bfill (&ascii[item * width], 2*width, ' ');
	}

    /* print out all the words */

    while (nunits-- > 0)
	{
	if (item == MAX_BYTES_PER_LINE/width)
	    {
	    /* end of line:
	     *   print out ascii format values and address of next line */

	    printf ("  *%16s*\n%08x:  ", ascii, (int) last_adrs);
	    bfill (ascii, MAX_BYTES_PER_LINE, '.'); /* clear out ascii buffer */
	    item = 0;				/* reset word count */
	    }

	switch (width)			/* display in appropriate format */
	    {
	    case 1:
		tmpByte = (UINT8 *)last_adrs;
	        printf ("%02x", *tmpByte);
		break;
	    case 2:
		tmpShort = (USHORT *)last_adrs;
	        printf ("%04x", *tmpShort);
		break;
	    case 4:
		tmpLong = (ULONG *)last_adrs;
	        printf ("%08lx", *tmpLong);
		break;
	    case 8:
		tmpLong = (ULONG *)last_adrs;
#if _BYTE_ORDER==_LITTLE_ENDIAN
		printf ("%08lx%08lx", *(tmpLong+1), *tmpLong);
#endif
#if _BYTE_ORDER==_BIG_ENDIAN
		printf ("%08lx%08lx", *tmpLong, *(tmpLong+1));
#endif
		break;
	    default:
		tmpByte = (UINT8 *)last_adrs;
	        printf ("%02x", *tmpByte);
		break;
	    }

        printf (" ");	/* space between words */

	/* set ascii buffer */

	pByte = (UINT8 *) last_adrs;
	for (ix = 0; ix < width; ix ++)
	    {
	    if (*pByte == ' ' || (isascii (*pByte) && isprint (*pByte)))
		{
	        ascii[item*width + ix] = *pByte;
		}
	    pByte ++;
	    }

	last_adrs = (void *)((int)last_adrs + width);
	item++;
	}

    /* print remainder of last line */

    for (; item < MAX_BYTES_PER_LINE/width; item++)
	printf ("%*s ", 2*width, " ");

    printf ("  *%16s*\n", ascii);	/* print out ascii format values */
    }

/*******************************************************************************
*
* ld - load an object module into memory
*
* This command loads an object module from a file or from standard input.
* The object module must be in UNIX `a.out' format.  External references in
* the module are resolved during loading.  The <syms> parameter determines how
* symbols are loaded; possible values are:
*
*     0 - Add global symbols to the system symbol table.
*     1 - Add global and local symbols to the system symbol table.
*    -1 - Add no symbols to the system symbol table.
*
* If there is an error during loading (e.g., externals undefined, too many
* symbols, etc.), then shellScriptAbort() is called to stop any script that
* this routine was called from.  If <noAbort> is TRUE, errors are noted but
* ignored.
*
* The normal way of using ld() is to load all symbols (<syms> = 1) during
* debugging and to load only global symbols later.
*
* The routine ld() is a 'shell command'.  That is, it is designed to
* be used only in the shell, and not in code running on the target.
* In future releases, calling ld() directly from code may not be
* supported.
*
* COMMON SYMBOLS
* On the target shell, for the 'ld' command only, common symbol behavior is 
* determined by the value of the global variable, ldCommonMatchAll.
* The reasoning for ldCommonMatchAll matches the purpose
* of the windsh environment variable, LD_COMMON_MATCH_ALL as explained below.
*
* If ldCommonMatchAll is set to TRUE (equivalent to windsh
* "LD_COMMON_MATCH_ALL=on"), the loader trys to match a common symbol
* with an existing one.  If a symbol with the same name is already
* defined, the loader takes its address. Otherwise, the loader creates
* a new entry. If set to FALSE (equivalent to windsh
* "LD_COMMON_MATCH_ALL=off"), the loader does not try to find an
* existing symbol. It creates an entry for each common symbol.
*
* EXAMPLE
* The following example loads the `a.out' file `module' from the default file
* device into memory, and adds any global symbols to the symbol table:
* .CS
*     -> ld <module
* .CE
* This example loads `test.o' with all symbols:
* .CS
*     -> ld 1,0,"test.o"
* .CE
* RETURNS:
* MODULE_ID, or
* NULL if there are too many symbols, the object file format is invalid, or
* there is an error reading the file.
*
* SEE ALSO: loadLib,
* .pG "Target Shell,"
* windsh,
* .tG "Shell" 
*/

MODULE_ID ld
    (
    int syms,		/* -1, 0, or 1 */
    BOOL noAbort,	/* TRUE = don't abort script on error */
    char *name		/* name of object module, NULL = standard input */
    )
    {
    MODULE_ID 	moduleId;
    int 	fd;
    int         loadFlags;

    if (name != NULL)
        {
        fd = open (name, O_RDONLY, 0);
        if (fd == ERROR)
            {
            printErr ("ld error: unable to open \"%s\"\n", name);
            return (NULL);
            }
        }
    else
        fd = STD_IN;

    /* convert to basic flags */

    switch (syms)
	{
	case NO_SYMBOLS:
	    loadFlags = LOAD_NO_SYMBOLS;
	    break;
	case GLOBAL_SYMBOLS:
	    loadFlags = LOAD_GLOBAL_SYMBOLS;
	    break;
	case ALL_SYMBOLS:
	    loadFlags = LOAD_ALL_SYMBOLS;
	    break;
	default:
	    loadFlags = syms;
        }

    if (ldCommonMatchAll)
        loadFlags = (loadFlags | LOAD_COMMON_MATCH_ALL);

    moduleId = loadModule (fd, loadFlags);

    if (name != NULL)
        close (fd);

    if (moduleId == NULL)
	{
	switch (errno)
	    {
	    case S_loadLib_TOO_MANY_SYMBOLS:
		printErr ("ld error: too many symbols.\n");
		break;
	    case S_symLib_SYMBOL_NOT_FOUND:
		printErr ("ld error: Module contains undefined symbol(s) "
			  "and may be unusable.\n");
		break;
	    default:
		printErr ("ld error: error loading file (errno = %#x).\n",
			    errno);
		break;
	    }
	if (! noAbort)
	    shellScriptAbort ();
	}

    return (moduleId);
    }

/*******************************************************************************
*
* devs - list all system-known devices
*
* This command displays a list of all devices known to the I/O system.
*
* RETURNS: N/A
*
* SEE ALSO: iosDevShow(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void devs (void)

    {
    iosDevShow ();
    }
/*******************************************************************************
*
* lkup - list symbols
*
* This command lists all symbols in the system symbol table whose names
* contain the string <substr>.  If <substr> is omitted or is 0, a short
* summary of symbol table statistics is printed.  If <substr> is the empty
* string (""), all symbols in the table are listed.  
*
* This command also displays symbols that are local, i.e., symbols found in
* the system symbol table only because their module was loaded by ld().
*
* By default, lkup() displays 22 symbols at a time.  This can be changed 
* by modifying the global variable `symLkupPgSz'.  If this variable is set
* to 0, lkup() displays all the symbols without interruption.
*
* RETURNS: N/A
*
* SEE ALSO: symLib, symEach(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void lkup
    (
    char *substr	/* substring to match */
    )
    {
    show ((int) sysSymTbl, (int) substr);	/* symShow() does the work */
    }
/*******************************************************************************
*
* lkAddr - list symbols whose values are near a specified value
*
* This command lists the symbols in the system symbol table that
* are near a specified value.  The symbols that are displayed include:
* .iP "" 4
* symbols whose values are immediately less than the specified value
* .iP
* symbols with the specified value
* .iP
* succeeding symbols, until at least 12 symbols have been displayed
* .LP
*
* This command also displays symbols that are local, i.e., symbols found in
* the system symbol table only because their module was loaded by ld().
*
* INTERNAL
* The LKADDR_ARG structure is used to hold the target address, count, and
* the `best' symbols found by lkAddrFind() and lkAddrNext(), which are the
* routines that get called by symEach() for each symbol in the system
* symbol table.
*
* RETURNS: N/A
*
* SEE ALSO: symLib, symEach(),
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void lkAddr
    (
    unsigned int addr		/* address around which to look */
    )
    {
    FAST int ix;
    LKADDR_ARG arg;

    arg.count = 0;			/* haven't printed anything yet */
    arg.addr  = addr;

    for (ix = 0; ix < NUM_SYMBLS; ++ix)
	arg.symbl[ix].addr = (unsigned int)NULL; /* clear little symbol table */

    /* call lkAddrFind for each symbol */

    symEach (sysSymTbl, (FUNCPTR)lkAddrFind, (int)&arg);

    /* print out the symbols found */

    for (ix = 0; ix < NUM_SYMBLS; ix++)
	{
	if (arg.symbl[ix].addr != (unsigned int)NULL)
	    {
	    arg.addr = arg.symbl[ix].addr;
	    symEach (sysSymTbl, (FUNCPTR)lkAddrPrintSame, (int)&arg);
	    }
	}

    if (arg.symbl[2].addr == (unsigned int)NULL)
	return;			/* couldn't find anything greater */

    /* print until there are enough entries */

    while (arg.count < 12)
	{
	arg.addr = arg.symbl[2].addr;

	arg.symbl[2].addr = (unsigned int)NULL;

	/* find next entry */

	symEach (sysSymTbl, (FUNCPTR)lkAddrNext, (int)&arg);

	if (arg.symbl[2].addr == (unsigned int)NULL)
	    break;			/* couldn't find anything greater */

	/* print them */

	arg.addr = arg.symbl[2].addr;
	symEach (sysSymTbl, (FUNCPTR)lkAddrPrintSame, (int)&arg);
	}
    }
/*******************************************************************************
*
* lkAddrFind - support routine for lkAddr()
*
* This command is called by symEach() to deal with each symbol in the table.
* If the value associated with the symbol is equal to lkAddrTarg(), or closer
* to it than previous close values, the appropriate slot in the array `symbl'
* is filled with the data for this symbol.
*
* RETURNS: TRUE
*/

LOCAL BOOL lkAddrFind
    (
    char * 	 name,		/* name in system symbol table     */
    unsigned int value,		/* value associated with name      */
    SYM_TYPE	 type,		/* type of this symbol table entry */
    LKADDR_ARG * arg		/* handle for search parameters    */
    )
    {
    FAST unsigned int addr = value;

    if (addr < arg->addr)
	{
	if (addr > arg->symbl[0].addr)
	    {
	    /* found closer symbol that is less than target */

	    arg->symbl[0].addr = addr;
	    arg->symbl[0].name = name;
	    arg->symbl[0].type = type;
	    }
	}
    else if (addr == arg->addr)
	{
	/* found target, fill in target entry */

	arg->symbl[1].addr = addr;
	arg->symbl[1].name = name;
	arg->symbl[1].type = type;
	}
    else if (addr > arg->addr)
	{
	if ((addr < arg->symbl[2].addr) ||
	    (arg->symbl[2].addr == (unsigned int)NULL))
	    {
	    /* found closer symbol that is greater than target */

	    arg->symbl[2].addr = addr;
	    arg->symbl[2].name = name;
	    arg->symbl[2].type = type;
	    }
	}

    return (TRUE);
    }
/*******************************************************************************
*
* lkAddrNext - another support routine for lkAddr()
*
* This command is called by symEach() to deal with each symbol in the table.
* If the value associated with the symbol is greater than target, but less
* than symbl[2].addr, it replaces symbl[2].addr.
*
* RETURNS: TRUE
*/

LOCAL BOOL lkAddrNext
    (
    char *	 name,		/* name in system symbol table     */
    unsigned int value,		/* value associated with name      */
    SYM_TYPE     type,		/* type of this symbol table entry */
    LKADDR_ARG * arg 		/* handle for search parameters    */
    )
    {
    FAST unsigned int addr = value;

    if (addr > arg->addr)
	{
	if (addr < arg->symbl[2].addr ||
	    arg->symbl[2].addr == (unsigned int)NULL)
	    {
	    /* found closer symbol that is greater than target */

	    arg->symbl[2].addr = addr;
	    arg->symbl[2].name = name;
	    arg->symbl[2].type = type;
	    }
	}

    return (TRUE);
    }
/*******************************************************************************
*
* lkAddrPrintSame - yet another support routine for lkAddr()
*
* This command is called by symEach() to deal with each symbol in the table.
* If the value associated with the symbol is equal to the target, print it.
*
* RETURNS: TRUE
*/

LOCAL BOOL lkAddrPrintSame
    (
    char *	 name,		/* name in system symbol table     */
    unsigned int value,		/* value associated with name      */
    SYM_TYPE     type,		/* type of this symbol table entry */
    LKADDR_ARG * arg 		/* handle for search parameters    */
    )
    {
    if (value == arg->addr)
	{
	printSTE (value, name, type);
	arg->count++;
	}

    return (TRUE);
    }
/*******************************************************************************
*
* printSTE - print symbol table entry
*/

LOCAL void printSTE
    (
    unsigned int addr,		/* address associated with name    */
    char *	 name,		/* name in system symbol table     */
    SYM_TYPE	 type		/* type of this symbol table entry */
    )
    {
    char demangled[USR_DEMANGLE_PRINT_LEN + 1];
    char *nameToPrint = cplusDemangle (name, demangled, sizeof (demangled));
    
    printf ("0x%08x %-25s %-8s", addr, nameToPrint, typeName[(type >> 1) & 7]);

    if ((type & N_EXT) == 0)
	printf (" (local)");

    printf ("\n");
    }
/*******************************************************************************
*
* substrcmp - determine if string <s1> is a slash-delineated word in string <s>
*
* This routine returns TRUE if string <s1> is a substring of <s>, where the
* substring is a word delineated by slashes.  If there are no slashes, both
* strings must be identical.
*
* EXAMPLES
*  substrcmp("/this/is/a/string/", "string") returns TRUE.
*  substrcmp("/this/is/a/string/", "str") returns FALSE.
*  substrcmp("string", "string") returns TRUE.
*
* RETURNS: TRUE or FALSE.
* NOMANUAL
*/

BOOL substrcmp
    (
    char *s,		/* pointer to string may have a slash */
    char *s1		/* pointer to the string to be found in the first one */
    )
    {
    char *pSubStr=s;	/* pointer to string divided by slashes in the string */
    char *pSlash;	/* pointer to slash in the first string */
    size_t strLen;
    BOOL found = FALSE;


    while (pSubStr != NULL)
	{
        pSlash = index (pSubStr, '/');
	if (pSlash == NULL)
	    {
	    found = (strcmp (pSubStr, s1) == 0);
	    break;
	    }
	else
	    {
	    strLen = (int)pSlash - (int)pSubStr;
	    if ((found = (strncmp (pSubStr, s1, strLen) == 0 &&
			 (strlen(s1) == strLen))))
		break;
	    else
		pSubStr = pSlash + 1;	/* point to char next to slash */
	    }
	}
    return (found);
    }

/*******************************************************************************
*
* mRegs - modify registers
*
* This command modifies the specified register for the specified task.
* If <taskNameOrId> is omitted or zero, the last task referenced is assumed.
* If the specified register is not found, it prints out the valid register list
* and returns ERROR.  If no register is specified, it sequentially prompts
* the user for new values for a task's registers.  It displays each register
* and the current contents of that register, in turn.  The user can respond
* in one of several ways:
* .iP RETURN 11
* Do not change this register, but continue, prompting at the next register.
* .iP <number>
* Set this register to <number>.
* .iP ". (dot)"
* Do not change this register, and quit.
* .iP EOF
* Do not change this register, and quit.
* .LP
*
* All numbers are entered and displayed in hexadecimal, except
* floating-point values, which may be entered in double precision.
*
* RETURNS: OK, or ERROR if the task or register does not exist.
*
* SEE ALSO: m(),
* .pG "Target Shell"
* windsh,
* .tG "Shell"
*/

STATUS mRegs
    (
    char *regName,		/* register name, NULL for all */
    int	  taskNameOrId		/* task name or task ID, 0 = default task */
    )
    {
    WIND_TCB *ptcb;
    int tid;				/* task ID */
    REG_SET regSet;			/* register set */
    int ix;				/* temporary loop count */
    ULONG *tmp;				/* temporary long word pointer */
    FPREG_SET fpRegSet;			/* fp register set */
    double *dTmp;			/* temporary double word pointer */
    BOOL  fppOk;			/* flag for supporting FPU */
#ifdef _WRS_ALTIVEC_SUPPORT
    ALTIVEC_CONTEXT avRegSet;
    BOOL  altivecOK;
#endif  /* _WRS_ALTIVEC_SUPPORT */
    BOOL done;				/* indicates user wants to quit */
    int width;				/* width of register to display */

    tid = taskIdFigure (taskNameOrId);	/* translate to task ID */

    if (tid == ERROR)			/* couldn't figure out super name */
	return (ERROR);

    tid = taskIdDefault (tid);		/* set the default ID */

    if (taskRegsGet (tid, &regSet) != OK)
	return (ERROR);

    if ((fppOk = (fppProbe () == OK)))
	{
        if (fppTaskRegsGet (tid, &fpRegSet) != OK)
	    return (ERROR);
	}
#ifdef _WRS_ALTIVEC_SUPPORT
    if ((altivecOK = altivecProbe ()) == OK)
      {
      ptcb = taskTcb(tid);
      if (altivecTaskRegsGet ((int)ptcb,&avRegSet) !=OK)
	return (ERROR);
      }
#endif /* _WRS_ALTIVEC_SUPPORT */
    /* 
    *  Some architectures organize floats sequentially, but to
    *  display them as doubles the floats need to be word swapped.
    *  However, this needs to be reconsidered.  The fpp registers
    *  are swapped before scanning and later fppTaskRegsSet() is
    *  called that some or all of the fpp registers could still 
    *  remain unneccessary swapped. XXX
    */

    if (fppDisplayHookRtn != NULL)
        (* fppDisplayHookRtn) (&fpRegSet);
#ifdef _WRS_ALTIVEC_SUPPORT
    /*    if (altivecDisplayHookRtn != NULL)
        (* altivecDisplayHookRtn) (&avRegSet);
    */
#endif

    if (regName == (char *) NULL)
	{
	/* default - modify all registers */

	done = FALSE;

	for (ix = 0; taskRegName[ix].regName != (char *) NULL; ix ++)
	    {
	    tmp = (void *) ((int)&regSet + taskRegName[ix].regOff);
#if CPU_FAMILY==MIPS
	    width = taskRegName[ix].regWidth;
#else
	    width = 4;
#endif
	    if (changeReg (taskRegName[ix].regName, tmp, width) == ERROR)
		{
		done = TRUE;
		break;
		}
	    }

	taskRegsSet (tid, &regSet);
	if (done)
	    return (OK);

        if (fppOk)
	    {
	    for (ix = 0; fpRegName[ix].regName != (char *) NULL; ix ++)
		{
		dTmp = (double *) ((int) &fpRegSet + fpRegName[ix].regOff);

		if (changeFpReg (fpRegName[ix].regName, *dTmp,
				 (double *) dTmp) ==ERROR)
		    {
		    done = TRUE;
		    break;
		    }
		}

	    fppTaskRegsSet (tid, &fpRegSet);
	    if (done)
		return (OK);

	    for (ix = 0; fpCtlRegName[ix].regName != (char *) NULL; ix ++)
		{
		tmp = (void *) ((int) &fpRegSet + fpCtlRegName[ix].regOff);

#if CPU_FAMILY==MIPS
		width = fpCtlRegName[ix].regWidth;
#else
		width = 4;
#endif
		if (changeReg (fpCtlRegName[ix].regName, tmp, width) ==ERROR)
		    break;
		}

	    fppTaskRegsSet (tid, &fpRegSet);
	    }

	return (OK);
	}

    else
	{ 				/* check valid register and modify */
	for (ix = 0; taskRegName[ix].regName != (char *) NULL; ix ++)
	    {
	    if (substrcmp (taskRegName[ix].regName, regName))
		{
	        tmp = (void *) ((int)&regSet + taskRegName[ix].regOff);
#if CPU_FAMILY==MIPS
		width = taskRegName[ix].regWidth;
#else
		width = 4;
#endif
		if(changeReg (taskRegName[ix].regName, tmp, width)==ERROR)
		    return (ERROR);

		taskRegsSet (tid, &regSet);
		return (OK);
		}
	    }

	if (fppOk)
	    {
	    for (ix = 0; fpRegName[ix].regName != (char *) NULL; ix ++)
		{
		if (substrcmp (fpRegName[ix].regName, regName))
		    {
		    dTmp = (double *) ((int) &fpRegSet + fpRegName[ix].regOff);
		    if (changeFpReg(fpRegName[ix].regName,*dTmp,dTmp) == ERROR)
			return (ERROR);

		    fppTaskRegsSet (tid, &fpRegSet);
		    return (OK);
		    }
		}

	    for (ix = 0; fpCtlRegName[ix].regName != (char *) NULL; ix ++)
		{
		if (substrcmp (fpCtlRegName[ix].regName, regName))
		    {
		    tmp = (void *) ((int) &fpRegSet+fpCtlRegName[ix].regOff);
#if CPU_FAMILY==MIPS
		    width = fpCtlRegName[ix].regWidth;
#else
		    width = 4;
#endif
		    if (changeReg (fpCtlRegName[ix].regName,tmp,width) == ERROR)
			return (ERROR);

		    fppTaskRegsSet (tid, &fpRegSet);
		    return (OK);
		    }
		}
	    }
	
        /* not found, print out register list */

        printf ("mRegs: no such register %s\n", regName);

	printf ("available registers:\n");

	for (ix = 0; taskRegName[ix].regName != (char *) NULL; ix++)
	    printf ("%s ", taskRegName[ix].regName);

	printf ("\n");

 	if (fppOk)
	    {
	    for (ix = 0; fpRegName[ix].regName != (char *) NULL; ix++)
		printf ("%s ", fpRegName[ix].regName);

	    printf ("\n");

	    for (ix = 0; fpCtlRegName[ix].regName != (char *) NULL; ix++)
		printf ("%s ", fpCtlRegName[ix].regName);

	    printf ("\n");
	    }
#ifdef _WRS_ALTIVEC_SUPPORT
	if(altivecOK)
	  {
	  /* altiVecCtxShow(tid); */
	  }
#endif
        return (ERROR);
    	}

    return (OK);		/* if we ever reach here */
	
    }
/*******************************************************************************
*
* changeReg - prompt the user for a hexadecimal register value
*
* This command reads a hexadecimal value from 'stdin'.
* If a NULL string is read, the routine returns immediately.
* If an invalid hexadecimal value is read, ERROR is returned.
*
* RETURNS: OK or ERROR.
*
* NOMANUAL
*/

STATUS changeReg
    (
    char *	pPrompt,	/* prompt string */
    void *	pValue,		/* ptr to input value, on return contains new*/
    int		width		/* width of register */
    )
    {
    char	stringBuf [20];	/* buffer for input line */
    char *	pLine;		/* pointer into input line */
    ULONG	hiValue = 0;	/* high part of value accumulator */
    ULONG	loValue = 0;	/* low part of value accumulator */

    if (*pPrompt == EOS)
	return (OK);

    /* display the current value */
    switch (width)
    	{
	case 1:
	    printf ("%-6s: %02x - ", pPrompt, *(UINT8 *)pValue);
	    break;
	case 2:
	    printf ("%-6s: %04x - ", pPrompt, *(USHORT *)pValue);
	    break;
	case 4:
	    printf ("%-6s: %08lx - ", pPrompt, *(ULONG *)pValue);
	    break;
	case 8:
#if _BYTE_ORDER==_LITTLE_ENDIAN
	    printf ("%-6s: %08lx%08lx - ", pPrompt, *((ULONG *)pValue+1),*(ULONG *)pValue);
#endif
#if _BYTE_ORDER==_BIG_ENDIAN
	    printf ("%-6s: %08lx%08lx - ", pPrompt, *(ULONG *)pValue, *((ULONG *)pValue+1));
#endif
	    break;
	default:
	    printf ("%-6s: %02x - ", pPrompt, *(UINT8 *)pValue);
	    break;
	}

    if (fioRdString (STD_IN, stringBuf, sizeof (stringBuf)) == EOF)
	return (ERROR);

    stringBuf[sizeof(stringBuf)-1] = EOS;	/* make sure input line has EOS */

    for (pLine = stringBuf; isspace (*(u_char*)pLine); ++pLine)	/* skip leading spaces*/
	;

    if (*pLine == EOS)			/* skip if just CR */
        return (OK);

    if (getHex (pLine, &hiValue, &loValue) != OK)
        return (ERROR);

    switch (width)
	{
	case 1:
	    *(UINT8 *)pValue = (UINT8) loValue;
	    break;
	case 2:
	    *(USHORT *)pValue = (USHORT) loValue;
	    break;
	case 4:
	    *(ULONG *)pValue = (ULONG) loValue;
	    break;
	case 8:
#if _BYTE_ORDER==_LITTLE_ENDIAN
	    *(ULONG *)pValue = (ULONG) loValue;
	    *((ULONG *)pValue+1) = (ULONG) hiValue;
#endif
#if _BYTE_ORDER==_BIG_ENDIAN
	    *(ULONG *)pValue = (ULONG) hiValue;
	    *((ULONG *)pValue+1) = (ULONG) loValue;
#endif
	    break;
	default:
	    *(UINT8 *)pValue = (UINT8) loValue;
	    break;
	}
    return (OK);
    }

/*******************************************************************************
*
* changeFpReg - prompt the user for a floating-point register value
*
* This command reads a floating-point value from stdin.
* If a NULL string is read, the routine returns immediately.
* If an invalid floating-point value is read, ERROR is returned.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS changeFpReg
    (
    char *	pPrompt,	/* prompt string */
    double 	value,		/* original value */
    double *	pValue		/* where to return new value */
    )
    {
    char stringBuf [20];
    double newValue;
    char excess;

    if (*pPrompt == EOS)
        return (OK);

    printf ("%-6s: %20g - ", pPrompt, value);

    if (fioRdString (STD_IN, stringBuf, sizeof (stringBuf)) == 1)
	{
	*pValue = value;
	return (OK);		/* just a CR */
	}

    if (strcmp (".", stringBuf) == OK)
	return (ERROR);

    if (sscanf (stringBuf, "%lf%1s", &newValue, &excess) != 1)
	return (ERROR);

    *pValue = newValue;
    return (OK);
    }
#undef	pc			/* undefine predefined pc if any */
/*******************************************************************************
*
* pc - return the contents of the program counter
*
* This command extracts the contents of the program counter for a specified
* task from the task's TCB.  If <task> is omitted or 0, 
* the current task is used.
*
* RETURNS: The contents of the program counter.
*
* SEE ALSO: ti(),
* .pG "Target Shell"
*/

int pc
    (
    int task			/* task ID */
    )
    {
    REG_SET regSet;
    int tid = taskIdFigure (task);

    if (tid == ERROR)
	return (ERROR);

    tid = taskIdDefault (tid);

    if (taskRegsGet (tid, &regSet) != OK)
	return (ERROR);
    
    return (*(int *)((int) &regSet + PC_OFFSET));
    }
/*******************************************************************************
*
* printErrno - print the definition of a specified error status value
*
* This command displays the error-status string, corresponding to a
* specified error-status value.  It is only useful if the error-status
* symbol table has been built and included in the system.  If <errNo> is
* zero, then the current task status is used by calling errnoGet().
*
* This facility is described in errnoLib.
*
* RETURNS: N/A
*
* SEE ALSO: errnoLib, errnoGet(),
* .pG "Target Shell"
* windsh,
* .tG "Shell"
*/

void printErrno
    (
    FAST int errNo	/* status code whose name is to be printed */
    )
    {
    char *statName;     /* pointer to stat tbl copy of error string */ 
    void *pval;
    SYMBOL_ID symId;    /* symbol identifier */               

    /* if no errno specified, get current tasks errno */

    if (errNo == 0)
	{
	errNo = errnoGet ();
	if (errNo == OK)
	    {
	    printf ("OK.\n");
	    return;
	    }
	}

    if (statSymTbl == NULL)
	printf ("Error status symbol table not included (errno = %#x).\n",
		errNo);
    else
	{
	symFindSymbol (statSymTbl, NULL, (void *)errNo, 
		       SYM_MASK_NONE, SYM_MASK_NONE, &symId);
	symNameGet (symId, &statName);
	symValueGet (symId, &pval);
        
	if ((int)pval != errNo)
	    printf ("Unknown errno = %#x.\n", errNo);
	else
	    printf ("%s\n", statName);
	}
    }
/*******************************************************************************
*
* printLogo - print the VxWorks logo
*
* This command displays the VxWorks banner seen at boot time.  It also
* displays the VxWorks version number and kernel version number.
*
* RETURNS: N/A
*
* SEE ALSO:
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void printLogo (void)

    {
    static char *logo[] =
{
"\n",
"]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]",
"]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]",
"]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]",
"     ]]]]]]]]]]]  ]]]]     ]]]]]]]]]]       ]]              ]]]]         (R)",
"]     ]]]]]]]]]  ]]]]]]     ]]]]]]]]       ]]               ]]]]            ",
"]]     ]]]]]]]  ]]]]]]]]     ]]]]]] ]     ]]                ]]]]            ",
"]]]     ]]]]] ]    ]]]  ]     ]]]] ]]]   ]]]]]]]]]  ]]]] ]] ]]]]  ]]   ]]]]]",
"]]]]     ]]]  ]]    ]  ]]]     ]] ]]]]] ]]]]]]   ]] ]]]]]]] ]]]] ]]   ]]]]  ",
"]]]]]     ]  ]]]]     ]]]]]      ]]]]]]]] ]]]]   ]] ]]]]    ]]]]]]]    ]]]] ",
"]]]]]]      ]]]]]     ]]]]]]    ]  ]]]]]  ]]]]   ]] ]]]]    ]]]]]]]]    ]]]]",
"]]]]]]]    ]]]]]  ]    ]]]]]]  ]    ]]]   ]]]]   ]] ]]]]    ]]]] ]]]]    ]]]]",
"]]]]]]]]  ]]]]]  ]]]    ]]]]]]]      ]     ]]]]]]]  ]]]]    ]]]]  ]]]] ]]]]]",
"]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]",
"]]]]]]]]]]]]]]]]]]]]]]]]]]]]]       Development System",
"]]]]]]]]]]]]]]]]]]]]]]]]]]]]",
};
    FAST int ix;

    for (ix = 0; ix < (int)NELEMENTS(logo); ix++)
	printf (" %s\n", logo [ix]);

    printf (" %s%s%s%s\n",
"]]]]]]]]]]]]]]]]]]]]]]]]]]]       ", runtimeName," version ", runtimeVersion);
    printf (" %s%s\n",
"]]]]]]]]]]]]]]]]]]]]]]]]]]       KERNEL: ", kernelVersion ());
    printf (" %s\n",
"]]]]]]]]]]]]]]]]]]]]]]]]]       Copyright Wind River Systems, Inc., 1984-2002");

    printf ("\n");
    }
/*******************************************************************************
*
* logout - log out of the VxWorks system
*
* This command logs out of the VxWorks shell.  If a remote login is active
* (via `rlogin' or `telnet'), it is stopped, and standard I/O is restored to
* the console.
*
* SEE ALSO: rlogin(), telnet(), shellLogout(),
* .pG "Target Shell"
*/

void logout (void)

    {
    shellLogout ();
    }
/*******************************************************************************
*
* h - display or set the size of shell history
*
* This command displays or sets the size of VxWorks shell history.  If no
* argument is specified, shell history is displayed.  If <size> is
* specified, that number of the most recent commands is saved for display.
* The value of <size> is initially 20.
*
* RETURNS: N/A
*
* SEE ALSO: shellHistory(), ledLib,
* .pG "Target Shell,"
* windsh,
* .tG "Shell"
*/

void h
    (
    int size	/* 0 = display, >0 = set history to new size */
    )
    {
    shellHistory (size);
    }

/*******************************************************************************
*
* spyReport - display task activity data
*
* This routine reports on data gathered at interrupt level for the amount of
* CPU time utilized by each task, the amount of time spent at interrupt level,
* the amount of time spent in the kernel, and the amount of idle time.  Time
* is displayed in ticks and as a percentage, and the data is shown since both
* the last call to spyClkStart() and the last spyReport().  If no interrupts
* have occurred since the last spyReport(), nothing is displayed.
*
* RETURNS: N/A
*
* SEE ALSO: spyLib, spyClkStart(),
* .pG "Target Shell"
*/

void spyReport (void)
    {
    if (_func_spyReport != NULL)
	(* _func_spyReport) ((FUNCPTR) printf);
    else
	printf (SPY_NOT_SUPPORTED_MSG);
    }

/*******************************************************************************
*
* spyTask - run periodic task activity reports
*
* This routine is spawned as a task by spy() to provide periodic task
* activity reports.  It prints a report, delays for the specified number of
* seconds, and repeats.
*
* RETURNS: N/A
*
* SEE ALSO: spyLib, spy(),
* .pG "Target Shell"
*/

void spyTask
    (
    int freq            /* reporting frequency, in seconds */
    )
    {
    if (_func_spyTask != NULL)
	(* _func_spyTask) (freq, (FUNCPTR) printf);
    else
	printf (SPY_NOT_SUPPORTED_MSG);
    }

/*******************************************************************************
*
* spy - begin periodic task activity reports
*
* This routine collects task activity data and periodically runs spyReport().
* Data is gathered <ticksPerSec> times per second, and a report is made every
* <freq> seconds.  If <freq> is zero, it defaults to 5 seconds.  If
* <ticksPerSec> is omitted or zero, it defaults to 100.
*
* This routine spawns spyTask() to do the actual reporting.
*
* It is not necessary to call spyClkStart() before running spy().
*
* RETURNS: N/A
*
* SEE ALSO: spyLib, spyClkStart(), spyTask(),
* .pG "Target Shell"
*/

void spy
    (
    int freq,               /* reporting freq in sec, 0 = default of 5 */
    int ticksPerSec         /* interrupt clock freq, 0 = default of 100 */
    )
    {
    if (_func_spy != NULL)
	(* _func_spy) (freq, ticksPerSec, (FUNCPTR) printf);
    else
	printf (SPY_NOT_SUPPORTED_MSG);
    }

/*******************************************************************************
*
* spyClkStart - start collecting task activity data
*
* This routine begins data collection by enabling the auxiliary clock
* interrupts at a frequency of <intsPerSec> interrupts per second.  If
* <intsPerSec> is omitted or zero, the frequency will be 100.  Data from
* previous collections is cleared.
*
* RETURNS:
* OK, or ERROR if the CPU has no auxiliary clock, or if task create and
* delete hooks cannot be installed.
*
* SEE ALSO: spyLib, sysAuxClkConnect(),
* .pG "Target Shell"
*/

STATUS spyClkStart
    (
    int intsPerSec      /* timer interrupt freq, 0 = default of 100 */
    )
    {
    if (_func_spyClkStart != NULL)
	return ((* _func_spyClkStart) (intsPerSec, (FUNCPTR) printf));
    else
	{
	printf (SPY_NOT_SUPPORTED_MSG);
	return (ERROR);
	}
    }

/*******************************************************************************
*
* spyClkStop - stop collecting task activity data
*
* This routine disables the auxiliary clock interrupts.
* Data collected remains valid until the next spyClkStart() call.
*
* RETURNS: N/A
*
* SEE ALSO: spyLib, spyClkStart(),
* .pG "Target Shell"
*/

void spyClkStop (void)
    {
    if (_func_spyClkStop != NULL)
	(* _func_spyClkStop) ();
    else
	printf (SPY_NOT_SUPPORTED_MSG);
    }

/*******************************************************************************
*
* spyStop - stop spying and reporting
*
* This routine calls spyClkStop().  Any periodic reporting by spyTask()
* is terminated.
*
* RETURNS: N/A
*
* SEE ALSO: spyLib, spyClkStop(), spyTask(),
* .pG "Target Shell"
*/

void spyStop (void)
    {
    if (_func_spyStop != NULL)
	(* _func_spyStop) ();
    else
	printf (SPY_NOT_SUPPORTED_MSG);
    }

/*******************************************************************************
*
* spyHelp - display task monitoring help menu
*
* This routine displays a summary of spyLib utilities:
* .CS
* .ne 3
* spyHelp                       Print this list
* spyClkStart [ticksPerSec]     Start task activity monitor running
*                                 at ticksPerSec ticks per second
* spyClkStop                    Stop collecting data
* spyReport                     Prints display of task activity
*                                 statistics
* spyStop                       Stop collecting data and reports
* spy     [freq[,ticksPerSec]]  Start spyClkStart and do a report
*                                 every freq seconds
*
* ticksPerSec defaults to 100.  freq defaults to 5 seconds.
* .CE
*
* RETURNS: N/A
*
* SEE ALSO: spyLib,
* .pG "Target Shell"
*/

void spyHelp (void)
    {
    static char *spy_help[] = {
    "spyHelp                       Print this list",
    "spyClkStart [ticksPerSec]     Start task activity monitor running",
    "                                at ticksPerSec ticks per second",
    "spyClkStop                    Stop collecting data",
    "spyReport                     Prints display of task activity",
    "                                statistics",
    "spyStop                       Stop collecting data and reports",
    "spy     [freq[,ticksPerSec]]  Start spyClkStart and do a report",
    "                                every freq seconds",
    "",
    "ticksPerSec defaults to 100.  freq defaults to 5 seconds.",
    "",
    };
    FAST int ix;

    /* check if facility is available */

    if (_func_spyTask == NULL)
	{
	printf (SPY_NOT_SUPPORTED_MSG);
	return;
	}

    for (ix = 0; ix < (int)NELEMENTS(spy_help); ix++)
	printf ("%s\n", spy_help [ix]);
    }
