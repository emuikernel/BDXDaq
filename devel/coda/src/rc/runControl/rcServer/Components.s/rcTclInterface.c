#ifdef USE_TK

/*-----------------------------------------------------------------------------
// copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      Implementation of RunControl interface to tcl
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcTclInterface.c,v $
//   Revision 1.11  1998/11/10 16:37:11  timmer
//   Linux port
//
//   Revision 1.10  1998/11/09 20:40:55  heyes
//   merge with Linux port
//
//   Revision 1.9  1998/11/05 20:11:52  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
*/
#ifdef _CODA_2_0_T


#include <codaConst.h>
#include "rcTclInterface.h"

#ifdef _CODA_USE_THREADS
#include <pthread.h>
pthread_mutex_t tcl_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

Tcl_Interp* rcInterp = 0;


/*sergey static*/ int  interactive = 0;
/*
** Static variables used by the interactive Tcl/Tk processing
*/
static Tcl_DString command;	/* Used to assemble lines of terminal input
				 * into Tcl commands. */
static int tty;			/* Non-zero means standard input is a
				 * terminal-like device.  Zero means it's
				 * a file. */

/*
 *----------------------------------------------------------------------
 *
 * Prompt --
 *
 *	Issue a prompt on standard output, or invoke a script
 *	to issue the prompt.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A prompt gets output, and a Tcl script may be evaluated
 *	in interp.
 *
 *----------------------------------------------------------------------
 */
static void
Prompt(interp, partial)
    Tcl_Interp *interp;			/* Interpreter to use for prompting. */
    int partial;			/* Non-zero means there already
					 * exists a partial command, so use
					 * the secondary prompt. */
{
    char *promptCmd;
    int code;

    promptCmd = Tcl_GetVar(interp,
	partial ? "tcl_prompt2" : "tcl_prompt1", TCL_GLOBAL_ONLY);
    if (promptCmd == NULL) {
	defaultPrompt:
	if (!partial) {
	    fputs("% ", stdout);
	}
    } else {
	code = Tcl_Eval(interp, promptCmd);
	if (code != TCL_OK) {
	    Tcl_AddErrorInfo(interp,
		    "\n    (script that generates prompt)");
	    fprintf(stderr, "%s\n", interp->result);
	    goto defaultPrompt;
	}
    }
    fflush(stdout);
}

/*
 *----------------------------------------------------------------------
 *
 * StdinProc --
 *
 *	This procedure is invoked by the event dispatcher whenever
 *	standard input becomes readable.  It grabs the next line of
 *	input characters, adds them to a command being assembled, and
 *	executes the command if it's complete.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Could be almost arbitrary, depending on the command that's
 *	typed.
 *
 *----------------------------------------------------------------------
 */

    /* ARGSUSED */
static void
StdinProc(clientData, mask)
    ClientData clientData;		/* Not used. */
    int mask;				/* Not used. */
{
    char input[4000];
    static int gotPartial = 0;
    char *cmd;
    int code, count;

    count = read(fileno(stdin), input, sizeof(input)-1);
    if (count <= 0) {
	if (!gotPartial) {
	    if (tty) {
	      Tcl_Eval(rcInterp,"exit");
	      exit(6);
	    } else {
		Tk_DeleteFileHandler(0);
	    }
	    return;
	} else {
	    count = 0;
	}
    }
    cmd = Tcl_DStringAppend(&command, input, count);
    if (count != 0) {
	if ((input[count-1] != '\n') && (input[count-1] != ';')) {
	    gotPartial = 1;
	    goto prompt;
	}
	if (!Tcl_CommandComplete(cmd)) {
	    gotPartial = 1;
	    goto prompt;
	}
    }
    gotPartial = 0;

    /*
     * Disable the stdin file handler while evaluating the command;
     * otherwise if the command re-enters the event loop we might
     * process commands from stdin before the current command is
     * finished.  Among other things, this will trash the text of the
     * command being evaluated.
     */

    Tk_CreateFileHandler(0, 0, StdinProc, (ClientData) 0);
    code = Tcl_RecordAndEval(rcInterp, cmd, 0);
    Tk_CreateFileHandler(0, TK_READABLE, StdinProc, (ClientData) 0);
    Tcl_DStringFree(&command);
    if (*rcInterp->result != 0) {
	if ((code != TCL_OK) || (tty)) {
	    printf("%s\n", rcInterp->result);
	}
    }

    /*
     * Output a prompt.
     */
 prompt:
    if( tty ) 
      Prompt(rcInterp, gotPartial);
}


int
initTcl(void)
{
  /*this routine is called by main thread: no need to lock */
  rcInterp = Tcl_CreateInterp ();

  /*
   * Set the "tcl_interactive" variable.
   */
    Tcl_SetVar(rcInterp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);

    tty = isatty(0);
    Tk_CreateFileHandler(0, TK_READABLE, StdinProc, (ClientData) 0);
    
    if( isatty(0) ) 
      Prompt(rcInterp, 0);
  
  if (Tcl_Init (rcInterp) == TCL_ERROR)
    return CODA_ERROR;

  if (Itcl_Init (rcInterp) == TCL_ERROR)
    return CODA_ERROR;

 /* if (Tdp_Init (rcInterp) == TCL_ERROR)
    return CODA_ERROR;
 */

#ifdef USE_TCLMSQL
  if (MYSQL_Init (rcInterp) == TCL_ERROR)
    return CODA_ERROR;
#endif

  return CODA_SUCCESS;
}

char*
evalScript (char* script, int* status)
{
  int code;
  char* res = 0;

#ifdef _CODA_USE_THREADS
  pthread_mutex_lock (&tcl_lock);
#endif

  if (rcInterp) {
    /*#ifdef _CODA_DEBUG*/
    printf ("begin evaluate script %s\n", script);
    /*#endif*/
    code = Tcl_Eval (rcInterp, script);
    printf ("Tcl code return is %d\n", code);
    if (code == TCL_OK) {
      *status = CODA_SUCCESS;
      if (*rcInterp->result != 0) {
	/*#ifdef _CODA_DEBUG */
	printf ("Tcl Evaluation: %s has status %d result %s\n", 
		script, *status, rcInterp->result);
	/*#endif*/
	
	res = (char *)malloc ((strlen (rcInterp->result) + 1)*sizeof (char));
	strcpy (res, rcInterp->result);
#ifdef _CODA_USE_THREADS
	pthread_mutex_unlock (&tcl_lock);
#endif

	return res;
      }
      else {
#ifdef _CODA_DEBUG
	printf ("TCL error no result resturned\n");
#endif

#ifdef _CODA_USE_THREADS
	pthread_mutex_unlock (&tcl_lock);
#endif

	return 0;
      }
    }
    else {
      /*#ifdef _CODA_DEBUG*/
      printf ("Tcl evaluation: %s : error: %s\n", 
	      script, rcInterp->result);
      /*#endif*/
      /* Tcl_Eval (rcInterp, "global errorInfo; puts \"$errorInfo\" ");*/
      *status = CODA_ERROR;
#ifdef _CODA_USE_THREADS
      pthread_mutex_unlock (&tcl_lock);
#endif

      return 0;
    }
  }
  else {
#ifdef _CODA_DEBUG
    printf ("rcInterp is 0x%x\n",rcInterp);
#endif
    *status = CODA_ERROR;
#ifdef _CODA_USE_THREADS
    pthread_mutex_unlock (&tcl_lock);
#endif
    return 0;
  }
}

void
tkOneEvent (void)
{
  Tk_DoOneEvent (TK_DONT_WAIT);
}
#endif


#endif /*USE_TK*/
    
