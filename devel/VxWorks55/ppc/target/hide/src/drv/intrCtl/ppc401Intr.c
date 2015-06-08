/* ppc403Intr.c - 403 specific interrupt handling library */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/* modification history
-----------------------
01a,08sep97,dat  adapted from ES release of evb401 BSP 
*/

/*
This library provides various interface routines to manipulate and connect 
to external hardware interrupts for 403GA.  
*/

#include "vxWorks.h"
#include "logLib.h"
#include "stdio.h"

/* defines */

#define INT_LEVEL_MAX	31
#define INT_LEVEL_MIN	0	

/* externals */

IMPORT STATUS	(*_func_intConnectRtn) (VOIDFUNCPTR *, VOIDFUNCPTR, int);
IMPORT int	(*_func_intEnableRtn) (int);
IMPORT int	(*_func_intDisableRtn) (int);

/* globals */

void            sysPpc403IntHandler (void);

/* locals */

LOCAL VOIDFUNCPTR 	sysIntBlTbl [32] = {(VOIDFUNCPTR) NULL};
LOCAL int	    	sysIntArg [32]   = {0};

/* forward LOCAL functions declarations */

LOCAL STATUS    sysPpc403IntConnect (VOIDFUNCPTR * vector, VOIDFUNCPTR routine,
                               	     int parameter);
LOCAL int       sysPpc403IntEnable (int intLevel);
LOCAL int       sysPpc403IntDisable (int intLevel);


/*******************************************************************************
*
* sysPpc403IntrInit - initialize the interrupt facility 
*
* This routine initializes the interrupt facility 
*
* RETURNS: OK
*/

STATUS sysPpc403IntrInit (void)

    {
    /* set the BSP driver specific Interrupt Handler and intConnect routines */

    _func_intConnectRtn = sysPpc403IntConnect;
    _func_intEnableRtn = sysPpc403IntEnable;
    _func_intDisableRtn = sysPpc403IntDisable;

    return (OK);
    }

/*******************************************************************************
*
* sysPpc403IntConnect - connect a C routine to a hardware interrupt
*
* This routine is called by intConnect () to connect a routine to a specified
* interrupt vector.  403GA does not provide vector for external interrupts.
* We just simply assign vectors according to the bits in the External Interrupt
* Enable Register (EXIER).
*
* RETURNS: OK, or ERROR if <vector> is out of range.
*/

STATUS sysPpc403IntConnect 
    (
    VOIDFUNCPTR * vector,	/* interrupt vector to attach to     */
    VOIDFUNCPTR   routine,	/* routine to be called              */
    int           parameter	/* parameter to be passed to routine */
    )
    {
    if (((int)vector > INT_LEVEL_MAX) || ((int)vector < INT_LEVEL_MIN))
	return (ERROR);
    else
	{
        sysIntBlTbl[(int)vector] = routine;
        sysIntArg[(int)vector]   = parameter;
        return (OK);
	}
    }
/*******************************************************************************
*
* sysPpc403IntHandler - PPC403 interrupt handler for external interrupts. 
*/

void sysPpc403IntHandler (void)
    {
    int vector;
    UCHAR intLevel;

    intLevel = sysInByte (PIC_IACK_ADR);      /* ignore the first one */
    intLevel = sysInByte (PIC_IACK_ADR);      /* interrupt level 0-15 */

    if (intLevel == 13)                       /* serial port 1 */
	vector = SERIAL2_INT_LVL;
    else if (intLevel == 12)                  /* ethernet NIC */
	vector = INT_LVL_NIC;
    else 
	vector = SERIAL2_INT_LVL;

    if (sysIntBlTbl [vector] != (VOIDFUNCPTR) NULL)
	(*sysIntBlTbl [vector]) (sysIntArg[vector]);
    else
	logMsg ("uninitialized interrupt vector %d\n", vector, 0,0,0,0,0);

    if (intLevel < 8)
	sysOutByte (PIC1_BASE_ADR, 0X60 | (1 << intLevel)); 
    else
	{
	sysOutByte (PIC2_BASE_ADR, 0X60 | (intLevel - 8)); 
	sysOutByte (PIC1_BASE_ADR, 0X20); 
	}

    }

/*******************************************************************************
*
* sysPpc403IntDisable - disable an external interrupt level
*
* This routine disables a specified external interrupt.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range 0 - 31.
*
* SEE ALSO: sysPpc403IntEnable()
*/

int sysPpc403IntDisable
    (
    int intLevel        /* interrupt level to disable */
    )
    {

    if (intLevel > INT_LEVEL_MAX || intLevel < INT_LEVEL_MIN)
        return (ERROR);

    /* disable the interrupt level */

    return (OK);
    }

/*******************************************************************************
*
* sysPpc403IntEnable - enable an external interrupt level
*
* This routine enables a specified external interrupt.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range 0 - 31.
*
* SEE ALSO: sysPpc403IntDisable()
*/

int sysPpc403IntEnable
    (
    int intLevel        /* interrupt level to enable */
    )
    {

    if (intLevel > INT_LEVEL_MAX || intLevel < INT_LEVEL_MIN)
        return (ERROR);

    /* enable the interrupt level */

    return (OK);
    }
