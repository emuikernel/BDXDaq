/* ppc403Intr.c - IBM 403 EVB-specific interrupt handling library */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/* modification history
-----------------------
01q,24jan02,pch  Fix Diab warnings
01p,24aug98,cjtc windview 2.0 event logging is now single step. Timestamp no
                 longer stored by intEnt. Avoids out-of-sequence timestamps in
                 event log (SPR 21868)
01o,16apr98,pr   moved triggering support from arch/ppc
01n,08apr98,pr   modified support for WindView 20
01m,07aug97,tam  fixed critical exception handling (SPR #8964).
01l,10jun97,wlf  doc: more cleanup.
01k,31mar97,tam  clear DMA interrupts properly in sysPpc403IntHandler(SPR 8285)
01j,19mar97,tam  mask CE bit as well (SPR 8192). Removed while loop in 
		 sysPpc403IntEnable() & sysPpc403IntDisable().
01i,09dec96,tpr  changed _func_intEnableRtn() & _func_intDisableRtn() return
		 type from void to int.
		 Added sysPpc403IntEnable() & sysPpc403IntDisable() return
		 code.
01h,15nov96,wlf  doc: cleanup.
01g,11jul96,pr   included eventP.h
01f,11jul96,pr   added windview instrumentation
01e,11jul96,tam  modified sysPpc403IntHandler to enable nested interrupt.
01d,18mar96,tam  added sysPpc403IntEnable/Disable and cleanup. 
01c,28feb96,tam  renamed and moved src/arch/ppc/intPpp403Lib.c to 
		 src/drv/intrCtl/ppc403Intr.c.
01b,09feb95,yao  enabled nested interrupt.
01a,28nov94,yao  created.
*/

/*
This library provides various interface routines to manipulate and connect 
to external hardware interrupts for the PPC403GA Embedded Controller.  
*/

#include "vxWorks.h"
#include "logLib.h"
#include "stdio.h"
#include "private/eventP.h"

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
LOCAL int      sysPpc403IntEnable (int intLevel);
LOCAL int      sysPpc403IntDisable (int intLevel);

/*******************************************************************************
*
* sysPpc403IntrInit - initialize the PPC403GA interrupt facility 
*
* This routine initializes the interrupt facility for the PPC403GA Embedded
* Controller.
*
* RETURNS: OK, always.
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
* This routine connects a specified C routine to a specified
* interrupt vector.  It is called by intConnect(). 
*
* The PPC403GA Embedded Controller does not provide vectors for external
* interrupts.  Instead, vectors are assigned according to the bits in the
* External Interrupt Enable Register (EXIER).
* In case of DMA interrupts, the DMA Status register is always passed to 
* the ISR as the argument <parameter>.
*
* RETURNS: OK, or ERROR if <vector> is out of range.
* 
* SEE ALSO: intConnect(), sysPpc403IntHandler()
*/

LOCAL STATUS sysPpc403IntConnect 
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
* sysPpc403IntHandler - PPC403GA external interrupt handler
*
* This is the external interrupt handler for the PPC403GA
* Embedded Controller.
* In case of DMA interrupts, the DMA Status register is always passed to 
* the ISR, that was connected to the DMA interrupt via intConnect(), as an 
* argument.
*
* RETURNS: N/A
*/

void sysPpc403IntHandler (void)
    {
    int vector;
    INT32 exisr;
    INT32 exier;
    UINT32 dmaSrVal = 0;	/* DMA Status reg. value */

    /* get interrupts status and interrupt control register */

    exisr = vxExisrGet ();
    exier = vxExierGet ();
    exisr = exisr & exier;

    /* get interrupt number */

    vector = vxFirstBit (exisr);

#ifdef INCLUDE_WINDVIEW
    WV_EVT_INT_ENT(vector)
#endif

    if (vector <= INT_LEVEL_MAX)
	{
	/* is this a DMA interrupt ? */

	if ((vector > 7) && (vector < 12))
	    {
	    /* 
	     * got a DMA interrupt: clear DMASR[CSX,TSX,RIX,CTX] bits for
	     * DMA channel X before interrupts are re-enabled.
	     * Pass the DMA status register to the ISR that proccesses the 
	     * DMA interrupt for channel X.
	     * This does prevent passing another argument than DMASR to the 
	     * ISR via intConnect, only for the DMA interrupts. However it 
	     * enables the ISR to detect the cause of the interrupt according
	     * to DMASR.
	     */

	    dmaSrVal = vxDmasrGet();

	    vxDmasrSet (dmaSrVal & ((0x88880040 >> (vector -8)) & 0xfff80070));	

	    sysIntArg[vector] = (UINT32) dmaSrVal; 
	    }

	/* clear the interrupt status bit */

	vxExisrClear (1 << (INT_LEVEL_MAX - vector));

	/*
	 * Mask this and lower priority interrupt levels:
	 * the critical interrupt has the highest priority while the 
	 * external interrupt 4 has the lowest priority (cf EXIER reg.).
	 */

	vxExierSet ((0xFFFFFFFF << (INT_LEVEL_MAX+1 - vector)) & vxExierGet());
	
	/*
	 * enable interrupt nesting: an external interrupt cannot be nested
	 * with a critical interrupt (CINT or watchdog int.) due to the
	 * implementation of the support for critical interrupt. However
	 * critical interrupts may be nested with each other.
	 */

	if (vector == 0)
	    intUnlock (_PPC_MSR_CE);
	else
	    intUnlock (_PPC_MSR_EE | _PPC_MSR_CE);

    	if (sysIntBlTbl [vector] != (VOIDFUNCPTR) NULL)
	    (*sysIntBlTbl [vector]) (sysIntArg[vector]);
    	else
	    logMsg ("uninitialized interrupt vector %d\n", vector, 0,0,0,0,0);

	/* re-enable interrupt mask */

	vxExierSet (exier);	
	}
    }

/*******************************************************************************
*
* sysPpc403IntDisable - disable an external interrupt level
*
* This routine disables a specified external interrupt for the PPC403GA
* Embedded Controller.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range 0 - 31.
*
* SEE ALSO: sysPpc403IntEnable()
*/

LOCAL int sysPpc403IntDisable
    (
    int intLevel        /* interrupt level to disable */
    )
    {
    UINT32 intMask;

    if (intLevel > INT_LEVEL_MAX || intLevel < INT_LEVEL_MIN)
        return (ERROR);

    intMask = 1 << (INT_LEVEL_MAX - intLevel);

    /* disable the interrupt level */

    vxExierDisable (intMask);

    vxExisrClear (intMask);         	/* clear pending interrupts */

    return (OK);
    }

/*******************************************************************************
*
* sysPpc403IntEnable - enable an external interrupt level
*
* This routine enables a specified external interrupt for the PPC403GA
* Embedded Controller.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range 0 - 31.
*
* SEE ALSO: sysPpc403IntDisable()
*/

LOCAL int sysPpc403IntEnable
    (
    int intLevel        /* interrupt level to enable */
    )
    {
    UINT32 intMask;

    if (intLevel > INT_LEVEL_MAX || intLevel < INT_LEVEL_MIN)
        return (ERROR);

    intMask = 1 << (INT_LEVEL_MAX - intLevel);

    vxExisrClear (intMask);   		/* clear pending interrupts */

    /* enable the interrupt level */

    vxExierEnable (intMask);

    return (OK);
    }

