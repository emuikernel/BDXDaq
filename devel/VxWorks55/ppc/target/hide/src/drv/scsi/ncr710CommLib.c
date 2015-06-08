/* ncr710CommLib.c - common library for ncr710Lib.c and ncr710Lib2.c */

/* Copyright 1989-1998 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,28mar99,jdi  doc: cleaned up format, text.
01c,24mar99,sut  removed NO_MANUAL [SPR 26006]
01b,27aug98,fle  doc : removed first empty line
01a,20aug98,sut  written. To fix multiply defined symbols
*/

/*
DESCRIPTION
Contains ncr710Lib and ncr710Lib2 common driver interfaces which can be called
from user code.

SEE ALSO: ncr710Lib.c, ncr710Lib2.c,
.I "NCR 53C710 SCSI I/O Processor Programming Guide,"
.pG "I/O System"
*/

#include "vxWorks.h"
#include "drv/scsi/ncr710.h"


/* defines */

typedef NCR_710_SCSI_CTRL SIOP;

/* External */

/* variables */ 

VOIDFUNCPTR ncr710SingleStepRtn = NULL; /* Global Single Step routine */
VOIDFUNCPTR ncr710StepEnableRtn = NULL; /* Global Single Enable routine */

/*******************************************************************************
*
* ncr710SingleStep - perform a single-step 
*
* This routine performs a single-step by writing the STD bit in the DCNTL
* register.
* The <pSiop> parameter is a pointer to the SIOP information. 
* Before executing, enable the single-step facility by calling
* ncr710StepEnable().
* 
* RETURNS: N/A
*
* SEE ALSO:
* ncr710StepEnable()
*/

void ncr710SingleStep
    (
    SIOP * pSiop, 		/* pointer to SIOP info */ 
    BOOL   verbose		/* show all registers   */
    )

    {
    if (ncr710SingleStepRtn != NULL)
	ncr710SingleStepRtn (pSiop, verbose);
    }


/*******************************************************************************
*
* ncr710StepEnable - enable/disable script single-step 
*
* This routine enables/disables the single-step facility on the chip.  
* It also unmasks/masks the single-step interrupt in the Dien register.
* Before executing any SCSI routines, enable the single-step facility by
* calling ncr710StepEnable() with <boolValue> set to TRUE.
* To disable, call it with <boolValue> set to FALSE.
* 
* RETURNS: N/A
*
* SEE ALSO:
* ncr710SingleStep()
*/

void ncr710StepEnable
    (
    SIOP *pSiop,		/* pointer to SIOP info */ 
    BOOL boolValue 		/* TRUE/FALSE to enable/disable single step */
    )
    {
    if (ncr710StepEnableRtn != NULL)
	ncr710StepEnableRtn (pSiop, boolValue);
    }

