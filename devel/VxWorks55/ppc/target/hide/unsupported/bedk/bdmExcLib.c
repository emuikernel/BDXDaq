/* bdmExcLib.c - library to catch exceptions on CPU32 under BDM */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,07oct96,bss  Added NOP to flush pipeline.
01a,23apr96,bss  written.
*/

/*
DESCRIPTION
This library provides an exception handling hook to catch
information about exceptions which occur on targets with 
Motorla CPU32 processors.  This library should only be used
when debugging via background debug mode (BDM), and using an
appropriate Tornado back end.  The hook gathers the necessary 
information about the exception, and then notifies the host 
that the exception occurred by invoking the 'bgnd' instruction, 
which puts the target in background debug mode.
*/

/* includes */

#include "vxWorks.h"
#include "private/funcBindP.h"
#include "wdb/wdbRegs.h"

#include "bdmExcLib.h"

/* globals */

LOCAL BOOL   bdmIsInitialized = FALSE;

/* information on the exception */
BDM_EXC_INFO bdmExcInfo = {0, 0, 0, FALSE};

/* forwards */

LOCAL int bdmExcHook (int vec, char *pESF, WDB_IU_REGS *pRegs);


/*******************************************************************************
*
* bdmExcLibInit - initialize bdmExcLib.
*
* This function installs the exception handling hook, which will
* gather information about exepctions for the Tornado tools.
*/

STATUS bdmExcLibInit (void)

    {
    
    if (bdmIsInitialized)
        return (ERROR);

    if (_func_excBaseHook != NULL)
        return (ERROR);

    /* Install exception hook */
    _func_excBaseHook = bdmExcHook;
    bdmIsInitialized  = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* bdmExcHook - captures exception information for BDM back end.
*
* This function is invoked by vxWorks's exception handler whenever 
* an exception occurs on the target.  When invoked, this function
* stores information about the exception in a global variable,
* 'bdmExcInfo', and then notifies the host that an exception occurred
* by invoking the 'bgnd' instruction.  The 'bgnd' instruction is a 
* special Motorola CPU32 instruction which puts the processor in
* background debug mode.  This function runs in the context of the 
* exception handler, and, thus, is subject to all interrupt-time 
* restrictions.
*/

LOCAL int bdmExcHook 
    (
    int vec, 
    char *pESF, 
    WDB_IU_REGS *pRegs
    )

    {
    bdmExcInfo.bdmIsException = TRUE;

    bdmExcInfo.excVector = vec;
    bdmExcInfo.pESF      = pESF;
    bdmExcInfo.pIuRegs   = pRegs;

    /* Put target in background mode        */
    /* by executing the 'bgnd' instruction. */

    __asm__ ("nop");          /* flush pipeline */
    __asm__ (".word 0x4afa"); /* 'BGND' */

    bdmExcInfo.bdmIsException = FALSE;


    /* Return FALSE so that vxWorks's exception handler will continue */
    /* processing the exception.                                      */

    return (FALSE);
    }
