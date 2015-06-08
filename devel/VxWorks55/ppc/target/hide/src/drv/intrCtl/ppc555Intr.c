/* ppc555Intr.c - PowerPC 555 interrupt driver */

/* Copyright 1984-1998 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,12apr99,zl   lock interrupts before restoring mask
01a,16mar99,zl   written based on ppc860Intr.c.
*/

/*
DESCRIPTION
The PowerPC 555 CPU series is divided in several units: PowerPC core, System
Interface Unit (SIU), and on-chip periferal units. The PowerPC core accepts 
only one external interrupt exception (vector 0x500). The SIU provides an 
interrupt controller which provides 32 levels but only 16 are used. The 
Interrupt controller is connected to the PowerPC core external interrupt. This 
library provides the routines to manage this interrupt controller.


ppc555IntrInit() connects the default demultiplexer, ppc555IntrDemux, to the 
external interrupt vector and initializes a table containing a function pointer
and an associated parameter for each interrupt source. ppc555IntrInit() is 
called by sysHwInit() from sysLib.c.

The default demultimplexer, ppc555IntrDeMux() detects which peripheral or 
controller has raised an interrupt and calls the associated routine with its 
parameter. 

Note: at this time only one interrupt source can be connected to a given level.
This gives only 16 different sources, of which 8 are external. To relieve this
restriction, interrupt handlers sharing the same level should be stored in a 
linked list and should be called one by one. It would be the ISRs responsability
to determine if it is really an interrupt condition. If not then simply return.

INCLUDE FILES: drv/intrCtl/ppc555Intr.h, drv/multi/ppc555Siu.h
*/

/* includes */

#include "intLib.h"
#include "iv.h"
#include "drv/intrCtl/ppc555Intr.h"
#include "drv/multi/ppc555Siu.h"


#ifdef  INCLUDE_WINDVIEW

#include "private/funcBindP.h"
#include "private/eventP.h"

#endif /* INCLUDE_WINDVIEW */


IMPORT UINT32 vxImemBaseGet();

/* local */

LOCAL INTR_HANDLER	intrVecTable[NUM_VEC_MAX];	/*  Intr vector table */

/* forward declarations */

LOCAL void	ppc555IntrDemux (void);
LOCAL STATUS	ppc555IntConnect (VOIDFUNCPTR *, VOIDFUNCPTR, int);
LOCAL int	ppc555IntEnable (int);
LOCAL int	ppc555IntDisable (int);

/*******************************************************************************
*
* ppc555IntrInit - initialize the interrupt manager for the PowerPC 500 series
*
* This routine connects the default demultiplexers, ppc555IntHandler() to 
* the external interrupt vector and associates all interrupt sources with 
* the default interrupt handler.  This routine is called by sysHwInit() 
* in sysLib.c.
*
* RETURN : OK
*/

STATUS ppc555IntrInit 
    (
    )
    {
    VOIDFUNCPTR defaultVec;     	/* default vector */
    int		vector;

    /* Get the default vector connected to the External Interrupt (0x500) */

    defaultVec = (VOIDFUNCPTR) excVecGet ((FUNCPTR *) _EXC_OFF_INTR);

    /* Connect the interrupt demultiplexer to External Interrupt (0x500) */

    excIntConnect ((VOIDFUNCPTR *) _EXC_OFF_INTR, ppc555IntrDemux);

    /* Install the driver routines in the architecture hooks */

    if (_func_intConnectRtn == NULL)
        _func_intConnectRtn = ppc555IntConnect;

    if (_func_intEnableRtn == NULL)
        _func_intEnableRtn = ppc555IntEnable;

    if (_func_intDisableRtn == NULL)
        _func_intDisableRtn = ppc555IntDisable;

    /* Set all vectors to default handler */

    for (vector = 0; vector < NUM_VEC_MAX; vector++)
	intConnect (INUM_TO_IVEC(vector), defaultVec, 0);

    return (OK);
 
   }

/*******************************************************************************
*
* ppc555IntConnect - connect a routine to an interrupt 
*
* This routine connects any C or assembly routine to one of the multiple 
* sources of interrupts.
*
* The connected routine can be any normal C code, except that it must not 
* invoke certain operating system functions that may block or perform I/O
* operations.
*
* <vector> types are defined in h/drv/intrClt/ppc555Intr.h.
*
* RETURNS: OK, or ERROR if <vector> is unknown.
*
* SEE ALSO: ppc555Intr.h
*/

LOCAL STATUS ppc555IntConnect
    (
    VOIDFUNCPTR *	vector,		/* interrupt vector to attach to */
    VOIDFUNCPTR		routine,	/* routine to be called */
    int 		parameter	/* parameter to be passed to routine */
    )
    {

    /* test the vector */

    if (IVEC_TO_INUM(vector) >= NUM_VEC_MAX)
	return (ERROR);

    intrVecTable[IVEC_TO_INUM(vector)].vec = routine;
    intrVecTable[IVEC_TO_INUM(vector)].arg = parameter;

    return (OK);
    }

/*******************************************************************************
*
* ppc555IntHandler - SIU interrupt demultiplexer 
*
* This routine must be bound to external interrupt exception (vector 0x500). 
* It is used to call the appropriate handler with its argument when an
* interrupt occurs. 
*
* NOTE: when this function is called the interrupts are still locked. It's
* this function responsability to unlock the interrupt.
*
* RETURNS: N/A
*/

void ppc555IntrDemux (void)
    {
    UINT8 	intVec;			/* interrupt vector */
    UINT32	imemBase;		/* internal memory Base Address */
    UINT32	intMask;		/* current interrupt mask */


    imemBase = vxImemBaseGet();		/* Internal memory base address */

    /* read the interrupt vector register */

    intVec = (* SIVEC(imemBase)) >> 26;

#ifdef  INCLUDE_WINDVIEW

    WV_EVT_INT_ENT(intVec)

#endif /* INCLUDE_WINDVIEW */

    /* save the current interrupt mask */ 

    intMask = * SIMASK(imemBase);

    /* lock all levels inferior to the interrupt detected */

    * SIMASK(imemBase) &= (0xffffffff << (32 - intVec));

    /* unlock the interrupt */

    intUnlock (_PPC_MSR_EE);

    /* call the Interrupt Handler */

    intrVecTable[intVec].vec (intrVecTable[intVec].arg);

    /* lock interrupts */

    intLock ();

    /* restore the interrupt mask */

    * SIMASK(imemBase) = intMask;

    return;
    }


/*******************************************************************************
*
* ppc555IntEnable - enable one of the Level or IRQ interrupts into the SIU
*
* This routine will unmask the bit in the SIMASK register corresponding to
* the requested interrupt level.  The interrupt level must be in the range
* of 0 - 31.
* 
* RETURNS: 0, always.
*/

LOCAL int ppc555IntEnable
    (
    int intNum		/* interrupt level to enable (0 - 31) */
    )
    {
    UINT32 imemBase;

    imemBase = vxImemBaseGet();
    if ((intNum >= 0) && (intNum < 32))
	* SIMASK(imemBase) |= (1 << (31 - intNum));
    return 0;
    }

/*******************************************************************************
*
* ppc555IntDisable - Disable one of the Level or IRQ interrupts into the SIU
*
* This routine will mask the bit in the SIMASK register corresponding to
* the requested interrupt level.  The interrupt level must be in the range
* of 0 - 31.
* 
* RETURNS: 0, always.
*/

LOCAL int ppc555IntDisable
    (
    int intNum          /* interrupt level to disable (0 - 31) */
    )
    {
    UINT32 imemBase;
    
    imemBase = vxImemBaseGet();
    if ((intNum >= 0) && (intNum < 32))
        * SIMASK(imemBase) &= ~(1 << (31 - intNum));
    return 0;
    }

