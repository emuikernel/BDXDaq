
/******************************************************************************
*                                                                             *
* Copyright (c) 1991,							      *
* 	National Center for Atmospheric Research                              *
*                                                                             *
******************************************************************************/

#ifdef VXWORKS


#define MAX_SYM 200
#include <vxWorks.h>
#include <sysSymTbl.h>
#include <a_out.h>
#include <stdioLib.h>
#include <ioLib.h>
#include <shellLib.h>
#include <taskLib.h>
#include <logLib.h>
#include <sys/times.h>


#include "tcl.h"

#define MAXLINE 200

extern void     tt(char *cmd);
#define CHAR_EOF ''

/*
 * NOTE - using WRS "execute()" seems to cause heap errors (VXSHELL), but the
 * rshell provided by Real Time Innvations works just fine! You may also use
 * the mshell provided in miniShell.c; however, it has no history,
 * expressions, symbol creation, redirection, & and * operators. If, for
 * example, you need to run a routine with real # arguments, it won't work
 * with the mshell. But it is reentrant and small.
 * 
 * #define USE_VXSHELL #define USE_RSHELL #define USE_MSHELL
 */
#define USE_VXSHELL


extern unsigned long vxTicks;

int
time_now(struct timeval * tv)
{
	tv->tv_sec = vxTicks / 60;
	tv->tv_usec = (vxTicks % 60) * 100000 / 6;
	return (0);
}

/*
 * execute  command under vxWorks first argument must be a vxworks entry
 * point WARNING - because the shell isn't re-entrant, neither is this code!
 */
int
Tcl_VxExecCmd(ClientData * clientData, Tcl_Interp * interp, int argc, char *argv[])
{
	char           *cmd = Tcl_Concat(argc - 1, argv + 1);
	int             rc = TCL_OK;
	int             bytes;
	int             server, conn;
	char            dummy;
	char            buf[2];	/* input buffer */
	extern void     doExecute();
	int             status;

	static int      pipeCreated = ERROR;
	int             slave;
	int             master;


	if (pipeCreated == ERROR) {
		pipeCreated = ptyDevCreate("/pty/0.", 512, 512);
		if (pipeCreated == ERROR) {
			Tcl_AppendResult(interp, argv[0],
					 "ERROR: can't create pipe to capture output of \"", cmd, "\"", (char *) NULL);
			ckfree(cmd);
			return TCL_ERROR;
		}
	}
	if ((master = open("/pty/0.M", O_RDWR, 0)) == ERROR) {
		printErrno(0);
		Tcl_AppendResult(interp, argv[0],
				 "ERROR: can't open master pipe to capture output of \"", cmd, "\"", (char *) NULL);
		ckfree(cmd);
		return TCL_ERROR;
	}
	if ((slave = open("/pty/0.S", O_RDWR, 0)) == ERROR) {
		printErrno(0);
		Tcl_AppendResult(interp, argv[0],
				 "ERROR: can't open slave pipe to capture output of \"", cmd, "\"", (char *) NULL);
		close(master);
		ckfree(cmd);
		return TCL_ERROR;
	}
	/* printf("Got into Tcl_VxExecCmd, command = %s\n",cmd); */

	/* start the task */
	doExecute(slave, cmd, master);

	/*
	 * NOTE- if reading > 1 byte, this task hangs on the last read, even
	 * after our "child" has closed its end of the pty reading 1 byte at
	 * a time is slow, but at least we can identify the '^D'
	 */

	buf[1] = '\0';		/* null terminate string */

	while ((bytes = read(master, buf, 1)) > 0) {
		if (buf[0] == CHAR_EOF)
			break;
		Tcl_AppendResult(interp, buf, (char *) NULL);
	}

	{
		char           *tmp, *place;
		int             i;
		tmp = malloc(strlen(interp->result) + 1);
		strcpy(tmp, interp->result);
		if ((place = strstr(tmp, "\nvalue =")) != NULL)
			*place = 0;
		Tcl_ResetResult(interp);

		Tcl_AppendResult(interp, tmp, (char *) NULL);
		free(tmp);
	}

done:
	(void) close(master);

#ifdef USE_VXSHELL
	shellLock(FALSE);
#endif
	return rc;
}



#define MAXHOST 30

#if defined USE_VXSHELL
extern int      execute(char *);
#elif defined USE_RSHELL
extern int      rshellParseCommand(char *);
#else
extern int      mShell(char *);
#endif

extern void     executeAndClose(int portNum, char *cmd, int master);
int             VxExecPri = 10;	/* patchable priority for created commands */

void
doExecute(int portNum, char *cmd, int master)
{
	extern int      shellTaskPriority;
	extern int      shellTaskId;
	extern int      VxExecPri;
	int             pri;
#ifdef NOTDEF
	if (taskPriorityGet(shellTaskId, &pri) == ERROR) {
		pri = shellTaskPriority;
	}
#else
	pri = VxExecPri;	/* may want at higher priority than shell */
#endif
	/* printf("Got into doExecute; spawn exec task\n"); */

	if (taskSpawn("exec", pri, VX_FP_TASK | VX_STDIO, 10000,
		(FUNCPTR) executeAndClose, portNum, cmd, master) == ERROR) {
		/*
		 * printf("ERROR: %s: %d - taskSpawn failed!\n", __FILE__,
		 * __LINE__);
		 */
		logMsg("ERROR: %s: %d - taskSpawn failed!\n", __FILE__, __LINE__);
		taskSuspend(0);
	}
}

/*
 * execute command, and close STD_OUT when complete, so caller knows we are
 * done I also found it necessary to write CHAR_EOF on the output string, so
 * the parent task could know we're done.
 */
void 
executeAndClose(int portNum, char *cmd, int master)
{
	int             rc = OK;
	int             oldStdOut, oldStdErr;
	char            dummy = CHAR_EOF;
	int             bytesUnread;

	/* printf("executeAndClose: Entered, will redirect stderr.\n"); */

	/*
	 * Set IO redirection.
	 */
	ioTaskStdSet(0, STD_ERR, portNum);	/* set std err for execute() */
	/*
	 * printf("executeAndClose: Got past redirecting stderr, will try
	 * stdout.\n");
	 */
	ioTaskStdSet(0, STD_OUT, portNum);	/* set std out for execute() */

#if defined USE_VXSHELL
	rc = execute(cmd);
#elif defined USE_RSHELL
	rc = rshellParseCommand(cmd);
#else
	rc = mShell(cmd);
#endif
	ckfree(cmd);
	fflush(stderr);
	fflush(stdout);
	write(portNum, &dummy, 1);	/* indicate EOF */
	write(portNum, &dummy, 0);	/* indicate EOF */
	close(portNum);
	/* printf("executeAndClose: Closed port.\n"); */

#ifdef NOTDEF
	/*
	 * This is pretty gross - but I don't know how else to force our
	 * parent to stop reading ! (closing the slave side has no effect,
	 * neither does writing ^D)
	 */

	rc = ioctl(master, FIONREAD, &bytesUnread);
	while ((rc != ERROR) && (bytesUnread > 0)) {
		rc = ioctl(master, FIONREAD, &bytesUnread);
	}
	close(master);
#endif
}


#ifdef NOTDEF

int
cmdVxExec(clientData, interp, argc, argv)
	ClientData      clientData;
	Tcl_Interp     *interp;
	int             argc;
	char           *argv[];
{
	SYM_TYPE        type;
	int             (*pFn) ();
	int             ret;
	char            buf[100];

	if (argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				 "\" vxcmd", (char *) NULL);
		return TCL_ERROR;
	}
	/* We need _symbolname for lookup */
#ifdef VXWORKSPPC
	strcpyt(buf, argv[1]);
#else
	strcpy(buf, "_");
	strcat(buf, argv[1]);
#endif

	/*
	 * Find the named symbol in the system symbol table (text symbols
	 * only)
	 */
	if (symFindByNameAndType(sysSymTbl, buf, (char **) &pFn, &type,
				 N_TEXT, N_TEXT) == OK) {
		ret = pFn(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		printf("return value %d %#x\n", ret, ret);
	} else {
		perror("vx: symFindByNameAndType");
	}

	return TCL_OK;
}
#endif

/******************************************************************************
 * Revision history:
 * $Log: vxUtil.c,v $
 * Revision 1.2  1996/08/23 17:52:54  timmer
 * remove underscore for PPC symFindByName
 *
 * Revision 1.1.1.1  1996/08/21 19:30:20  heyes
 * Imported sources
 *
 * : ----------------------------------------------------------------------
 * Revision 1.6  1995/09/19  20:39:11  timmer
 * : Eliminated subroutines from this file that are not needed for a
 * : barebones implementation of TCL7.4 for vxWorks
 * : ----------------------------------------------------------------------
 * Revision 1.5  1995/01/31  20:39:11  vanandel
 * : Modified Files:
 * : 	README.vxworks RELEASE.vxworks simpleEvent.c tclTCP.c vxUtil.c
 * : ----------------------------------------------------------------------
 * new release notes
 * fixed memory leaks in tclTCP.c
 *
 * Revision 1.4  1994/01/06  18:16:27  vanandel
 * : 	README.vxworks RELEASE.vxworks tclBasic.c tclEnv.c tclUnixAZ.c
 * : 	vxUtil.c
 * : ----------------------------------------------------------------------
 * See change log in README.vxworks
 *
 * Revision 1.3  1993/11/02  20:21:04  vanandel
 * *** empty log message ***
 *
 * Revision 1.2  1993/10/14  14:52:18  vanandel
 * first working version of vxgetvar
 *
 * Revision 1.1  1993/10/12  15:28:14  vanandel
 * Initial revision
 *
 * Revision 1.4  1992/10/21  00:33:48  vanandel
 * command execution with results returned to TCL caller
 *
 * Revision 1.3  1992/10/20  21:40:55  vanandel
 * pass everything BUT the first argument to "execute"
 *
 * Revision 1.1  1992/10/20  20:31:07  vanandel
 * Initial revision
 *
*                                                                             *
*********************************END OF RCS INFO******************************/



#else

void
vxUtil_dummy()
{
  return;
}

#endif /* VXWORKS */
