/* pciIntLib.c - PCI Shared Interrupt support */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01f,11oct01,dat  merge from ae 1.1
01e,19jun00,dat  SPR 31194, new pciIntDisconnect2 routine
01d,18nov99,dat  partial fix for SPR 28467, pciIntDisconnect fault.
01c,30sep98, tm  moved PCI_INT_RTN typedef here from private header (SPR 22544)
01b,04mar98, tm  augmented pciIntConnect/Disconnect to do intConnect also
01a,20feb98, tm  derived from pciIomapLib.c of ebsa285 BSP v01m
*/


/*
DESCRIPTION

This component is PCI Revision 2.1 compliant.

The functions addressed here include:

.IP "   -"
Initialize the library.
.IP "   -"
Connect a shared interrupt handler.
.IP "   -"
Disconnect a shared interrupt handler.
.IP "   -"
Master shared interrupt handler.
.LP

Shared PCI interrupts are supported by three functions: pciInt(),
pciIntConnect(), and pciIntDisconnect2().  pciIntConnect() adds the specified
interrupt handler to the link list and pciIntDisconnect2() removes it from
the link list.  The master interrupt handler pciInt() executes these interrupt
handlers in the link list for a PCI interrupt.  Each interrupt handler must
check the device dependent interrupt status bit to determine the source of
the interrupt, since it simply execute all interrupt handlers in the link
list.  pciInt() should be attached by intConnect() function in the BSP 
initialization with its parameter. The parameter is an vector number associated 
to the PCI interrupt.
*/

#include "vxWorks.h"
#include "drv/pci/pciIntLib.h"
#include "dllLib.h"
#include "iv.h"
#include "intLib.h"
#include "stdlib.h"

/*
 * The following defines specify, by default, the maximum number of busses,
 * devices and functions allowed by the PCI 2.1 Specification.
 *
 * Any or all may be overriden by defining them in config.h.
 */

/* macros */

#ifndef INT_NUM_IRQ0
#   define INT_NUM_IRQ0 0
#endif /* INT_NUM_IRQ0 */

#ifndef PCI_INT_BASE
#   define PCI_INT_BASE INT_NUM_IRQ0
#endif /* INT_NUM_IRQ0 */

#ifndef PCI_INT_LINES
#   ifdef PCI_IRQ_LINES	/* backward compatibility */
#	define PCI_INT_LINES	PCI_IRQ_LINES
#   else
#	define PCI_INT_LINES	32
#   endif
#endif

/*
 * This maps a system vector to a PCI irq number, in the range
 * 0 to (PCI_INT_LINES - 1)
 */

#ifndef PCI_INT_VECTOR_TO_IRQ
#define PCI_INT_VECTOR_TO_IRQ(vector)					\
	(IVEC_TO_INUM(vector) - PCI_INT_BASE)
#endif /* PCI_INT_VECTOR_TO_IRQ */

/*
 * Provide intConnect via a macro so that an alternate interrupt binding
 * mechanism can be specified
 *
 */

#ifndef PCI_INT_HANDLER_BIND
#define PCI_INT_HANDLER_BIND(vector, routine, param, pResult)		\
    do {								\
    IMPORT STATUS intConnect();						\
    *pResult = intConnect ( (vector),(routine), (int)(param) );		\
    } while (0)
#endif /* PCI_INT_HANDLER_BIND */

/*
 * Provide an unbind macro to remove an interrupt binding.  The default 
 * is basically a no-op.  This can result in a minor memory leak if there
 * is a lot of pciIntConnect, pciIntDisconnect activity.
 */

#ifndef PCI_INT_HANDLER_UNBIND
#define PCI_INT_HANDLER_UNBIND(vector, routine, param, pResult)		\
	    *pResult = OK;
#endif /* PCI_INT_HANDLER_BIND */

/* typedefs */

typedef struct pciIntRtn
    {
    DL_NODE	node;		/* double link list */
    VOIDFUNCPTR	routine;	/* interrupt handler */
    int		parameter;	/* parameter of the handler */
    } PCI_INT_RTN;

/* globals */

DL_LIST	pciIntList[PCI_INT_LINES];	/* linked list of int handlers */

/* locals */

LOCAL STATUS pciIntLibInitStatus = NONE;

/*******************************************************************************
*
* pciIntLibInit - initialize the pciIntLib module
*
* This routine initializes the linked lists used to chain together the PCI 
* interrupt service routines.
*
* RETURNS: OK, or ERROR upon link list failures.
*/

STATUS pciIntLibInit (void)
    {
    int ix;

    if (pciIntLibInitStatus != NONE)
	return (pciIntLibInitStatus);

    /* Initialize shared interrupt handler chains */

    for (ix = 0; ix < NELEMENTS(pciIntList); ix++)
        dllInit (&pciIntList[ix]);

    return (pciIntLibInitStatus = OK);
    }

/*******************************************************************************
*
* pciInt - interrupt handler for shared PCI interrupt.
*
* This routine executes multiple interrupt handlers for a PCI interrupt.
* Each interrupt handler must check the device dependent interrupt status bit
* to determine the source of the interrupt, since it simply execute all
* interrupt handlers in the link list.  
*
* This is not a user callable routine
*
* RETURNS: N/A
*/

VOID pciInt
    (
    int irq		/* IRQ associated to the PCI interrupt */
    )
    {
    PCI_INT_RTN *pRtn;

    for (pRtn = (PCI_INT_RTN *)DLL_FIRST (&pciIntList[irq]); pRtn != NULL;
	 pRtn = (PCI_INT_RTN *)DLL_NEXT (&pRtn->node))
	(* pRtn->routine) (pRtn->parameter);
    }

/*******************************************************************************
*
* pciIntConnect - connect the interrupt handler to the PCI interrupt.
*
* This routine connects an interrupt handler to a shared PCI interrupt vector.
* A link list is created for each shared interrupt used in the system.  It
* is created when the first interrupt handler is attached to the vector.
* Subseqent calls to pciIntConnect just add their routines to the
* linked list for that vector.
*
* RETURNS:
* OK, or ERROR if the interrupt handler cannot be built.
*
*/

STATUS pciIntConnect
    (
    VOIDFUNCPTR *vector,        /* interrupt vector to attach to     */
    VOIDFUNCPTR routine,        /* routine to be called              */
    int parameter               /* parameter to be passed to routine */
    )
    {
    int irq = PCI_INT_VECTOR_TO_IRQ (vector);
    PCI_INT_RTN *pRtn;
    int oldLevel;
    STATUS retStatus;

    if (pciIntLibInitStatus != OK)
	return ERROR;

    if (irq < 0 || irq >= PCI_INT_LINES)
	return ERROR;

    if ((pRtn = (PCI_INT_RTN *)malloc (sizeof (PCI_INT_RTN))) == NULL)
        {
        return ERROR;
        }

    /* If pciIntList[irq] is null, then bind the handler using intConnect */

    if ( DLL_EMPTY(&pciIntList[irq]) )
        {
        PCI_INT_HANDLER_BIND(vector, pciInt, irq , &retStatus );
	if (retStatus == ERROR)
	    {
            return ERROR;
	    }
        }

    pRtn->routine   = routine;
    pRtn->parameter = parameter;

    oldLevel = intLock ();			/* LOCK INTERRUPT */
    dllAdd (&pciIntList[irq], &pRtn->node);
    intUnlock (oldLevel);			/* UNLOCK INTERRUPT */

    retStatus = ( (DLL_EMPTY(&pciIntList[irq])) ? ERROR : OK );
    return retStatus;
    }


/*******************************************************************************
*
* pciIntDisconnect - disconnect the interrupt handler (OBSOLETE)
*
* This routine disconnects the interrupt handler from the PCI interrupt line.
*
* In a system where one driver and one ISR services multiple devices, this
* routine removes all instances of the ISR because it completely ignores the
* parameter argument used to install the handler.
*
* NOTE: Use of this routine is discouraged and will be obsoleted in the future.
* New code should use the pciIntDisconnect2() routine instead.  
*
* RETURNS:
* OK, or ERROR if the interrupt handler cannot be removed.
*
*/

STATUS pciIntDisconnect
    (
    VOIDFUNCPTR *vector,        /* interrupt vector to attach to     */
    VOIDFUNCPTR routine         /* routine to be called              */
    )
    {
    int irq = PCI_INT_VECTOR_TO_IRQ (vector);
    PCI_INT_RTN *pRtn;
    PCI_INT_RTN *pNext;
    int oldLevel;
    STATUS retStatus = ERROR;

    if (pciIntLibInitStatus != OK)
	return (ERROR);

    if (irq < 0 || irq >= PCI_INT_LINES)
	return ERROR;

    for (pRtn = (PCI_INT_RTN *)DLL_FIRST (&pciIntList[irq]); pRtn != NULL;
	 pRtn = pNext)
	{
	pNext = (PCI_INT_RTN *)DLL_NEXT (&pRtn->node);

	if (pRtn->routine == routine)
	    {
	    oldLevel = intLock ();			/* LOCK INTERRUPT */
	    dllRemove (&pciIntList[irq], &pRtn->node);
	    intUnlock (oldLevel);			/* UNLOCK INTERRUPT */
	    free ((char *)pRtn);

	    /* If the last ISR was just removed, then do intDisconnect */

            if ( DLL_EMPTY(&pciIntList[irq]) )
	        {
		PCI_INT_HANDLER_UNBIND (vector, pciInt, irq, &retStatus);
	        }

	    retStatus = OK;
	    }
	}

    return (retStatus);
    }

/*******************************************************************************
*
* pciIntDisconnect2 - disconnect an interrupt handler from the PCI interrupt.
*
* This routine disconnects a single instance of an interrupt handler from the
* PCI interrupt line.
*
* NOTE: This routine should be used in preference to the original
* pciIntDisconnect() routine.  This routine is compatible with drivers that
* are managing multiple device instances, using the same basic ISR, but with
* different parameters.
*
* RETURNS:
* OK, or ERROR if the interrupt handler cannot be removed.
*/

STATUS pciIntDisconnect2
    (
    VOIDFUNCPTR *vector,        /* interrupt vector to attach to     */
    VOIDFUNCPTR routine,        /* routine to be called              */
    int parameter		/* routine parameter		     */
    )
    {
    int irq = PCI_INT_VECTOR_TO_IRQ(vector);
    PCI_INT_RTN *pRtn;
    PCI_INT_RTN *pNext;
    int oldLevel;
    STATUS retStatus = ERROR;

    if (pciIntLibInitStatus != OK)
	return (ERROR);

    if (irq < 0 || irq >= PCI_INT_LINES)
	return ERROR;

    for (pRtn = (PCI_INT_RTN *)DLL_FIRST (&pciIntList[irq]); pRtn != NULL;
	 pRtn = pNext)
	{
	pNext = (PCI_INT_RTN *)DLL_NEXT (&pRtn->node);

	if (pRtn->routine == routine && pRtn->parameter == parameter)
	    {
	    oldLevel = intLock ();			/* LOCK INTERRUPT */
	    dllRemove (&pciIntList[irq], &pRtn->node);
	    intUnlock (oldLevel);			/* UNLOCK INTERRUPT */
	    free ((char *)pRtn);

	    /* If the last ISR was just removed, then do intDisconnect */

            if ( DLL_EMPTY(&pciIntList[irq]) )
	        {
		PCI_INT_HANDLER_UNBIND (vector, pciInt, irq, &retStatus);
	        }

	    retStatus = OK;
	    }
	}

    return retStatus;
    }
