/* usrDsp.c - dsp support library */

/* Copyright 1984-1999 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,08nov00,zl   set spTaskOptions for DSP.
01a,10mar98,jmb  fix mem's dspLib.c scalablity problem.
*/

/*
DESCRIPTION
This library calls dspInit to activate DSP support.  It must be
called before any tasks using the dsp are spawned.  This is done
automatically by the root task, usrRoot(), in usrConfig.c when 
INCLUDE_DSP is defined in configAll.h.

This libraray also provides debugging support for DSP registers
in the target shell and initializes other configurable DSP
support facilties.

For information about architecture-dependent dsp routines, see
the manual entry for dspArchLib.

The dspShow() routine displays DSP registers on a per-task basis.
For information on this facility, see the manual entries for 
dspShow().

VX_DSP_TASK OPTION
Saving and restoring dsp registers adds to the context switch
time of a task.  Therefore, dsp registers are not saved
and restored for every task.  Only those tasks spawned with the task
option VX_DSP_TASK will have dsp registers saved and restored.

.RS 4 4
\%NOTE:  If a task does any dsp operations,
it must be spawned with VX_DSP_TASK.
.RE

INCLUDE FILES: dspLib.h

SEE ALSO: dspArchLib, dspShow, intConnect(),
.pG "Basic OS"
*/

#ifndef  __INCusrDspc
#define  __INCusrDspc

#ifdef INCLUDE_DEBUG
#include "stdio.h"
#endif /* INCLUDE_DEBUG */
#include "regs.h"
#include "dspLib.h"

#ifdef INCLUDE_DEBUG
/* externals (from usrLib.c) */

IMPORT STATUS	changeReg (char *pPrompt, void *value, int width);
BOOL substrcmp (char *s, char *s1);

/* forward declarations */

LOCAL void dspRegsListHook ();
LOCAL STATUS dspMregsHook (int tid, char *regName, int *pDone);	
#endif /* INCLUDE_DEBUG */


/******************************************************************************
*
* usrDspInit - initialize dsp support
*
* This routine initializes dsp support and must be
* called before using the dsp.  This is done
* automatically by the root task, usrRoot(), in usrConfig.c when INCLUDE_DSP
* is defined in configAll.h.
* 
* RETURNS: N/A
*/

void usrDspInit (void)
    {
    dspInit();

#ifdef INCLUDE_DEBUG
    _func_dspMregsHook = (FUNCPTR) dspMregsHook;
    _func_dspRegsListHook = (VOIDFUNCPTR) dspRegsListHook;
#endif

#ifdef  INCLUDE_SHELL
    {
    extern int shellTaskOptions;
    extern int spTaskOptions;
    shellTaskOptions |= VX_DSP_TASK;
    spTaskOptions |= VX_DSP_TASK;
    }
#endif  /* INCLUDE_SHELL */

#ifdef  INCLUDE_SHOW_ROUTINES
    dspShowInit ();                     /* install dsp show routine */
#endif  /* INCLUDE_SHOW_ROUTINES */

    }

#ifdef INCLUDE_DEBUG

/******************************************************************************
*
* dspMRegHook - hook for mRegs to get/set DSP registers
*
* Given a task id:
*   If regName is NULL, then walk through all DSP regs and
*	allow user to set any; done is never TRUE.
*   If regName is not NULL, then try to match the register
*	name, setting done = TRUE if match is found.
*
* This routine allows mRegs to walk through DSP register set.
* It is here so that vxWorks will link w/out DSP support.
* Usage: dspMregsHookRtn = dspMregsHook.
*
* RETURNS: OK or ERROR; also sets '*done = TRUE' if the user
*	asked for one register by name and it was matched here.
*
* NOMANUAL
*/

LOCAL STATUS dspMregsHook 
    (
    int tid, 			/* IN: task id of DSP register set */
    char *regName, 		/* IN: NULL means all regs */
    int *pDone			/* OUT: set TRUE if regName != NULL && found */
    )
    {
    int width = 4;
    int ix;
    void * tmp;
    DSPREG_SET dspRegSet;

    if (dspTaskRegsGet (tid, &dspRegSet) != OK)
	return (ERROR);

    for (ix = 0; dspRegName[ix].regName != (char *) NULL; ix ++)
	{
	if ((regName == NULL) || substrcmp (dspRegName[ix].regName, regName))
	    {
	    tmp = (void *) ((int) &dspRegSet + dspRegName[ix].regOff);
	    if (changeReg (dspRegName[ix].regName, tmp, width) == ERROR)
		return (ERROR);
	    
	    if (regName != NULL)
		{
		*pDone = TRUE;
		break;
		}
	    }
	}

    if (!*pDone)
	{
	for (ix = 0; dspCtlRegName[ix].regName != (char *) NULL; ix ++)
	    {
	    if ((regName == NULL) || 
		substrcmp (dspCtlRegName[ix].regName, regName))
		{
		tmp = (void *) ((int) &dspRegSet + dspCtlRegName[ix].regOff);
		if (changeReg (dspCtlRegName[ix].regName, tmp, width) == ERROR)
		    return (ERROR);
		
		if (regName != NULL)
		    {
		    *pDone = TRUE;
		    break;
		    }
		}
	    }
	}

    dspTaskRegsSet (tid, &dspRegSet);
    return (OK);
    }

/******************************************************************************
*
* dspRegsListHook - hook for mRegs to list DSP registers
*
* This routine allows mRegs to list DSP register set.
* It is here so that vxWorks will link w/out DSP support.
* Usage: dspRegsListHookRtn = dspRegsListHook.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void dspRegsListHook ()
    {
    int ix;

    for (ix = 0; dspRegName[ix].regName != (char *) NULL; ix++)
	printf ("%s ", dspRegName[ix].regName);
    
    for (ix = 0; dspCtlRegName[ix].regName != (char *) NULL; ix++)
	printf ("%s ", dspCtlRegName[ix].regName);

    printf ("\n");
    }
#endif /* INCLUDE_DEBUG */


#endif /* __INCusrDspc */

