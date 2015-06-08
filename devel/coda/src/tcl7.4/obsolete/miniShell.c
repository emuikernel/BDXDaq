/***************************************************************************
* Module: miniShell.c (mShell)
*
* Copyleft (C) 1995 Software Technology, Inc. All Rights Reserved.
* (see copyleft comments below)
*
* Disclaimer:  This code is provided as a service to the embedded community
*       "at large". STI is not responsible for the reliability of this
*       software.  Use at your own risk.
*
*
* Purpose: Provide minimal shell capabilities in small space.  User has
*       all familiar shell features except history, expressions,
*       symbol creation, redirection, & and * operators.  Execution,
*       parameters, symbol displays, and numeric displays all function the
*       same as the shell.  Complete C interpreting is not done (as hinted
*       above), but enough is there.
*
*       For example, `md 0xa4010000', `scanf "%d" x', `td t2', and even
*       `a' or '1222' are legal.
*
*       Commas, parentheses, and semicolons are universally replaced by a
*       space, so if you did:
*               printf ("joe, sally");
*       the "(),;" are stripped for you, and you would get "joe  sally"
*       sent to printf.
*
*       Newline and any \? escape character will not be printed, so
*               printf ("joe\n");
*       would result in "joe\n".
*
*       To make this shell effective, you must modify usrLib.c to comment
*       out references to the shellLib.o and shell.o files (or the shell
*       will be there anyway).  Then compile and insert into config.a.
*       (I don't have to tell you to back up the original config.a right?)
*
*       Note that time barely permited this, do not have grand
*       expectations.
*
* Future: Since I had written it for v5.0.5 of vxworks, I have modified
*       it to be reentrant and work for 5.1.  I have not been able to
*       test it since making these changes, though.  So for the future,
*       someone run it and let us know :)
*
*       This is not a triumph of programming by any stretch of the
*       imagination.  Its robust enough that we use it when the need
*       arrises.  If time permits then it gets improved.  If you find
*       said time :) please let me in on your improvements. Thanks!
*
* Author: Doug Dyer (STI - dyer@alx.sticomet.com)
* Date: 06/07/94
*
* Modifications:
*       06/07/94        DMD     Initial writing
*       08/22/95        DMD     Made more reentrant, never tested.
*       02/15/96        CT      Look for usrCmd->cmdParams[x] preceded by "_"
*                               in symboltable.  Add mShell(char *cmd) to
*                               allow other routines to access mshell features.
*
* Copyleft: this source code is 100% PD.  All changes must be logged
* under the Modifications section above.  The following changes
* are not allowed: (1) removing any previous author's name from the
* modification history, (2) removing STI's copyleft or adding additional
* copyleft/right constraints.  Any commercial venture using minishell
* must include the source code of this module (or refer where to get it).
* Please share with us your improvements by re-uploading them to the
* exploder.
*
*
*****************************************************************************/
#include "vxWorks.h"
#include "symLib.h"
#include "sysSymTbl.h"
#include "symbol.h"
#include "a_out.h"
#include "stdioLib.h"
#include "strLib.h"
#include "taskLib.h"
#include "excLib.h"
#include "math.h"
#if CPU_FAMILY==MIPS
#include "arch/mips/r3000.h"
#endif

/* all instanses of the shell will quit when this becomes FALSE */
BOOL            runShell = TRUE;

/*
 * note that you cannot just change the number of parameters as I have some
 * scanf's that assume 9
 */
#define NUM_PARAMS      9
#define PARAMSIZE       80

/* structure for parsing input */
struct parseFields {
	unsigned char   string[PARAMSIZE];

	long            numParams;
	unsigned long   cmd[NUM_PARAMS + 1];
	unsigned long   cmdPVal;
	unsigned char   cmdStrs[NUM_PARAMS + 1][PARAMSIZE];
	unsigned char   cmdParams[NUM_PARAMS + 1][PARAMSIZE];

	unsigned long   mShellTID;
	SYM_TYPE        cmdPType;
};

/* shell vars to prevent pulling shell in */
unsigned long   shellTaskId;

/* global prototypes */
VOID            shellInit();
STATUS          miniShell(char *);
STATUS          mShell(char *);

#define MSHELLNAME "t_mShell"

/* local prototypes */
LOCAL VOID      mShellHandler(int, int, long);
LOCAL STATUS    prepareInput(struct parseFields *);
LOCAL STATUS    installInput(struct parseFields *);
LOCAL STATUS    fixQuoteParams(struct parseFields *);
LOCAL STATUS    parseInput(struct parseFields *);
LOCAL STATUS    handleInput(struct parseFields *);

/*
 * routines
 */

/*********************************************************************
* some (but not all!) routines to avoid pulling shell in:
*   shellInit()
*
*
**********************************************************************/
VOID 
shellInit(stack, bogus)
	int             stack;
	BOOL            bogus;
{
	taskSpawn(MSHELLNAME, 110, VX_STDIO | VX_UNBREAKABLE | VX_FP_TASK,
		  stack, (FUNCPTR) miniShell, (int) "mShell->", 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

/*********************************************************************
* miniShell()
*
* The *entry point* for mShell.  Must be spawned with the SHELL_STACK_SIZE
* and task option VX_STDIO (or just call shellInit).
*
* The prompt string is passed down to it.
*
*********************************************************************/
STATUS 
miniShell(char *prompt)
{
	struct parseFields usrCmd;

	/* install task hook */
	usrCmd.mShellTID = shellTaskId = taskIdSelf();
	excHookAdd((FUNCPTR) mShellHandler);

	do {
		/* display prompt */
		printf("%s", prompt);

		/* interact with user */
		if (installInput(&usrCmd) == OK) {
			if (parseInput(&usrCmd) == OK)
				handleInput(&usrCmd);
		}
	} while (runShell != FALSE);

	return (OK);
}

/*********************************************************************
* mShell()
*
* Routine allowing calls from other routines to use the mShell.
* A C-callable interface.
*
*********************************************************************/
STATUS 
mShell(char *shellcmd)
{
	struct parseFields usrCmd;
	int             x;

	if (shellcmd == NULL)
		return (OK);

	/* clear command inputs */
	bzero(usrCmd.string, PARAMSIZE);
	for (x = 0; x < NUM_PARAMS + 1; x++) {
		bzero(usrCmd.cmdParams[x], PARAMSIZE);
		bzero(usrCmd.cmdStrs[x], PARAMSIZE);
		usrCmd.cmd[x] = 0;
	}

	strcpy(usrCmd.string, shellcmd);

	/* consolidate quotes, remove commas, etc */
	prepareInput(&usrCmd);

	/* loop through each entry in the command input */
	if ((usrCmd.numParams = sscanf(usrCmd.string, "%s%s%s%s%s%s%s%s%s%s",
	      usrCmd.cmdParams[0], usrCmd.cmdParams[1], usrCmd.cmdParams[2],
	      usrCmd.cmdParams[3], usrCmd.cmdParams[4], usrCmd.cmdParams[5],
	      usrCmd.cmdParams[6], usrCmd.cmdParams[7], usrCmd.cmdParams[8],
				       usrCmd.cmdParams[9])) >= 1) {

		fixQuoteParams(&usrCmd);
	}
	if (parseInput(&usrCmd) == OK)
		handleInput(&usrCmd);

	return (OK);
}

/*********************************************************************
* installInput()
*
* Clear previous state variables, and collect new input from user
*
*********************************************************************/
LOCAL STATUS 
installInput(usrCmd)
	struct parseFields *usrCmd;
{
	int             x;

	/* clear command inputs and stdin */
	fflush(stdin);
	bzero(usrCmd->string, PARAMSIZE);
	for (x = 0; x < NUM_PARAMS + 1; x++) {
		bzero(usrCmd->cmdParams[x], PARAMSIZE);
		bzero(usrCmd->cmdStrs[x], PARAMSIZE);
		usrCmd->cmd[x] = 0;
	}

	/* begin command input collection */
	if (gets(usrCmd->string) == NULL) {
		/* oh well, better luck next time */
		printf("\n");
		taskRestart(usrCmd->mShellTID);	/* perhaps reopen stdin? */
	}
	/* consolidate quotes, remove commas, etc */
	prepareInput(usrCmd);

	/* loop through each entry in the command input */
	if ((usrCmd->numParams = sscanf(usrCmd->string, "%s%s%s%s%s%s%s%s%s%s",
	   usrCmd->cmdParams[0], usrCmd->cmdParams[1], usrCmd->cmdParams[2],
	   usrCmd->cmdParams[3], usrCmd->cmdParams[4], usrCmd->cmdParams[5],
	   usrCmd->cmdParams[6], usrCmd->cmdParams[7], usrCmd->cmdParams[8],
					usrCmd->cmdParams[9])) >= 1) {

		fixQuoteParams(usrCmd);
		return (OK);
	}
	return (ERROR);
}

/*********************************************************************
* prepareInput()
*
* Removes parentheses, commas.  Consolidates quoted parameter with
* spaces (scanf will break up on a space)
*
**********************************************************************/
LOCAL STATUS 
prepareInput(usrCmd)
	struct parseFields *usrCmd;
{
	int             x;
	char           *temp1, *temp2, *ptr;

	/* scan for commas, semicolons,  and parens only, replace with spaces */
	for (x = 0; x < strlen(usrCmd->string); x++) {
		if (usrCmd->string[x] == ',' || usrCmd->string[x] == '(' ||
		    usrCmd->string[x] == ')' || usrCmd->string[x] == ';')
			usrCmd->string[x] = ' ';
	}

	/*
	 * consolidate any parameters surrounded by quotes by sticking in
	 * commas
	 */
	ptr = (char *) usrCmd->string;
	while (ptr != NULL) {
		/* point to first quote */
		temp1 = index(ptr, '\"');
		if (temp1 == NULL)
			break;

		/* point to next position */
		temp2 = index(temp1 + 1, '\"');
		if (temp2 == NULL)
			break;

		/* fill the middle! */
		for (x = 0; x < (temp2 - temp1); x++)
			if (temp1[x] == ' ')
				temp1[x] = ',';

		ptr = temp2 + 1;
	}

	return (OK);
}

/*********************************************************************
* fixQuoteParams()
*
* Wraps up the consolidation of quoted parameters. Removes special
* character stuffed in to replace the space.
*
**********************************************************************/
LOCAL STATUS 
fixQuoteParams(usrCmd)
	struct parseFields *usrCmd;
{
	int             x, y;

	/* scan for commas only, replace with spaces */
	for (x = 0; x < usrCmd->numParams; x++) {
		for (y = 0; y < strlen(usrCmd->cmdParams[x]); y++) {
			if (usrCmd->cmdParams[x][y] == ',')
				usrCmd->cmdParams[x][y] = ' ';
		}
	}

}

/*********************************************************************
* parseInput()
*
* Parses user input into cmd[x] array for handleInput.  It also
* does a small amount of reaction if it can detect something funny
* (such as first entry being a number rather than a function call)
*
**********************************************************************/
LOCAL STATUS 
parseInput(usrCmd)
	struct parseFields *usrCmd;
{
	int             x, pval;
	SYM_TYPE        pType;
	int             retval;
	char           *temp1, *temp2;
	char            _usrCmd[PARAMSIZE + 1];

	/* loop on each item */
	for (x = 0; x < usrCmd->numParams; x++) {

		/*
		 * put in cmd[x] the value, address of symbol, or ptr to
		 * string
		 */

		/*
		 * If the item is not a value, then we must do a
		 * symFindByName
		 */
		/* on _usrCmd->Params[x].  Add "_" in front and look for it.    */
#ifdef VXWORKSPPC
		sprintf(_usrCmd, "%s", usrCmd->cmdParams[x]);
#else
		sprintf(_usrCmd, "%s%s", "_", usrCmd->cmdParams[x]);
#endif
		retval = symFindByName(sysSymTbl, _usrCmd, (char **) &usrCmd->cmd[x], &pType);

		/* &usrCmd->cmd[x], &pType, pType, SYM_MASK_NONE); */

		/* does such a thing exist? */
		if (x == 0) {
			if (retval != OK) {

				/* no such routine exists, must be value */
				if (usrCmd->cmdParams[x][0] > '9')
					usrCmd->cmdPVal = (unsigned long) usrCmd->cmdParams[x][0];
				else
					sscanf(usrCmd->cmdParams[x], "%i", &usrCmd->cmdPVal);

				/* print out info, %c if printable */
				if (usrCmd->cmdPVal >= ' ' && usrCmd->cmdPVal <= 255)
					printf("value = %d = 0x%x = '%c'\n",
					       usrCmd->cmdPVal, usrCmd->cmdPVal, usrCmd->cmdPVal);
				else
					printf("value = %d = 0x%x\n",
					  usrCmd->cmdPVal, usrCmd->cmdPVal);
				return (ERROR);
			}
			usrCmd->cmdPType = pType;
			usrCmd->cmdPVal = pval;
		}
		/* is a parameter, determine what to do */
		else if (retval != OK) {
			/* is it a string? */
			temp1 = index(usrCmd->cmdParams[x], '\"');

			if (temp1 != NULL) {
				temp2 = rindex(temp1, '\"');

				if (temp1 == temp2) {
					printf("Missing quote: %s \n",
					       usrCmd->cmdParams[x]);
					return (ERROR);
				} else {
					/* remove quotes */
					temp1++;	/* skip quote */
					*temp2 = '\0';

					/* point to string */
					strcpy(usrCmd->cmdStrs[x], temp1);
					usrCmd->cmd[x] = (unsigned long) usrCmd->cmdStrs[x];
				}
			}
			/* unfound non-numeric symbol? */
			else if (usrCmd->cmdParams[x][0] < '0' ||
				 usrCmd->cmdParams[x][0] > '9') {

				/* point to string */
				strcpy(usrCmd->cmdStrs[x], usrCmd->cmdParams[x]);
				usrCmd->cmd[x] = (unsigned long) usrCmd->cmdStrs[x];
			}
			/* must be an integer then. */
			else
				sscanf(usrCmd->cmdParams[x], "%i", &usrCmd->cmd[x]);
		}
	}

	return (OK);
}

/*********************************************************************
* handleInput()
*
* Actually reacts to input from user.  Ie: calls function or
* displays data value.
*
*********************************************************************/
LOCAL STATUS 
handleInput(usrCmd)
	struct parseFields *usrCmd;
{
	FUNCPTR         ptr;
	unsigned long   retval;

	/* is it data? */
	if (usrCmd->cmdPType & N_ABS) {
		/* print out info.. print a %c if printable character */
		if (usrCmd->cmdPVal >= ' ' && usrCmd->cmdPVal <= 255)
			printf("value = %d = 0x%x = '%c'\n",
			 usrCmd->cmdPVal, usrCmd->cmdPVal, usrCmd->cmdPVal);
		else
			printf("value = %d = 0x%x\n", usrCmd->cmdPVal, usrCmd->cmdPVal);
	}
	/* is it an executable? */
	else if (usrCmd->cmdPType & N_TEXT
#if CPU_FAMILY==MIPS
		 && usrCmd->cmd[0] > K0BASE
#endif
		) {
		ptr = (FUNCPTR) usrCmd->cmd[0];
		retval = (unsigned long) ((FUNCPTR) ptr(usrCmd->cmd[1], usrCmd->cmd[2],
			     usrCmd->cmd[3], usrCmd->cmd[4], usrCmd->cmd[5],
			     usrCmd->cmd[6], usrCmd->cmd[7], usrCmd->cmd[8],
							usrCmd->cmd[9]));

		/* print out info.. print a %c if printable character */
		if (retval >= ' ' && retval <= 255)
			printf("value = %d = 0x%x = '%c'\n", retval, retval, retval);
		else
			printf("value = %d = 0x%x\n", retval, retval);
	}
	/* who knows what it is ??? */
	else {
		sscanf(usrCmd->cmdParams[0], "%i", usrCmd->cmdPVal);

		/* print out info.. print a %c if printable character */
		if (usrCmd->cmdPVal >= ' ' && usrCmd->cmdPVal <= 255)
			printf("??? %s = %d = 0x%x = '%c'\n", usrCmd->cmdParams[0],
			 usrCmd->cmdPVal, usrCmd->cmdPVal, usrCmd->cmdPVal);
		else
			printf("??? %s = %d = 0x%x\n", usrCmd->cmdParams[0],
			       usrCmd->cmdPVal, usrCmd->cmdPVal);
		return (ERROR);
	}

	return (OK);
}

/*********************************************************************
* mShellHandler(int, int, ESFR3K*)
*
* Exception protection for mShell.  If the user were to cause an
* exception (for example, `d 0') the shell should recover itself
* to provide an interface.  This routine does that by restarting
* mShell.
*
*********************************************************************/
LOCAL VOID 
mShellHandler(int task, int vecNum, long ignored)
{

	/* is it us? */
#if 0
	if (task == mShellTID) {
#endif

		if (strcmp(taskName(task), MSHELLNAME) == 0) {
			logMsg("\nmShell restarted\n");
			taskRestart(task);
		}
	}
