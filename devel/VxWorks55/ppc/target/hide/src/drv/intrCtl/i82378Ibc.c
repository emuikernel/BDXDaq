/* i82378Ibc.c - Intel i82378 ISA Bridge Controller (IBC) driver */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01q,24aug98,cjtc windview 2.0 event logging is now single step. Timestamp no
                 longer stored by intEnt. Avoids out-of-sequence timestamps in
                 event log (SPR 21868)
01p,16apr98,pr   moved triggering support from arch/ppc
01o,05mar98,pr   added WV_ON mask to WV 2.0 code
01n,22feb98,pr   modified windview support for WV 2.0
01m,22dec96,tpr  removed intUnlock() at the end of sysIbcIntHandler() SPR(#7679)
01l,08dec96,tpr  changed _func_intEnableRtn() return value from void to int.
01o,09jul97,dat  fixed SPR 8866, intDisable did not work for lvls 8 to 15.
01n,17jan97,mas  fixed IRQ number to IRQ level mapping (SPR 7752) and
		 deleted unused SEM_ID extIntHandlerSem.
01m,22dec96,tpr  removed intUnlock() at the end of sysIbcIntHandler() SPR(#7679)
01l,08dec96,tpr  changed _func_intEnableRtn() return value from void to int.
		 added sysIbcIntEnable() return value code.
01k,30oct96,dat  replaced sysInByte and sysOutByte by IBC_BYTE_READ and
	    tam	 IBC_BYTE_WRITE macros (spr #7415). Added description.
01j,29oct96,wlf  doc: cleanup.
01i,24sep96,mas  changed MV1305 to MV1300.
01h,23jul96,tam  fixed sysIbcIntLevelSet() to mask int. level <intLevel> too
01g,08jul96,ms   merged in pr's WindView instrumentation
01f,01jul96,tam  added code to respect interrupt priorities (spr #6212). 
01e,11jun96,tam  fixed sysIbcIntConnect() to enable multiple int. handlers 
		 connected to a same vector (spr #6588).
01d,05may96,mas  fix IBC interrupt mappings for MV1305 vs. ULTRA/MV1600.
01c,16apr96,jds  fix IBC interrupt mappings for ULTRA vs. MV1600
01b,06mar96,tpr  clean-up
01a,29jan96,tam  written from i8259Pic.c.
*/

/*
DESCRIPTION
This module implements the Intel i82378 ISA Bridge Controller (IBC) driver.
The i82378 IBC chip includes many functions.  This driver only implements
the interrupt control functions.  In fact this driver provides the
complete interrupt architecture functionality (which it shouldn't).

The macros IBC_BYTE_READ and IBC_BYTE_WRITE control the actual access
to the PIC registers.  By default, these macros assume memory mapped
access to the registers.  The default write macro includes an eieio
instruction to insure write-ordering is preserved. The BSP can redefine
these macros in config.h or <bsp>.h to specify some other access means
such as i/o mapping.

The IBC_BYTE_READ macro takes two arguments, the register address and
the data address.  Data is read from the register address and stored
at the data address. Defining the macro to return a value is not preferred
because if the macro has to be defined within a code block, then data
cannot easily be returned.

The IBC_BYTE_WRITE macro also takes two arguments, the register address
and the data to be written.  Including an eieio instruction after the
actual data write is recommended.

Neither macro returns any values.
*/

#include "drv/intrCtl/i82378Ibc.h"
#include "private/funcBindP.h"

/* globals */

IMPORT UINT		sysVectorIRQ0; 	/* vector for IRQ0 */
IMPORT STATUS	      (*_func_intConnectRtn) (VOIDFUNCPTR *, VOIDFUNCPTR, int);
IMPORT int	      (*_func_intEnableRtn) (int);
IMPORT int	      (*_func_intDisableRtn) (int);
IMPORT STATUS		excIntConnect (VOIDFUNCPTR *, VOIDFUNCPTR);

#ifdef  INCLUDE_WINDVIEW
#include "private/eventP.h"
#endif

/* system interrupt table */

INT_HANDLER_DESC * sysIntTbl [256];

/* locals */

/* PIC1 & PIC2 IRQ masks ordered by IRQ# */

UCHAR ibcIRQ2Pic1Mask[16] = {0xFB,0xFA,0xF8,0xF8,0xF0,0xE0,0xC0,0x80,
			     0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8};
UCHAR ibcIRQ2Pic2Mask[16] = {0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
			     0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80};

/* forward static functions */

LOCAL void	sysIbcEndOfInt (void);
LOCAL STATUS	sysIbcIntConnect (VOIDFUNCPTR * vector, VOIDFUNCPTR routine,
				  int parameter);
LOCAL int	sysIbcIntEnable (int);
LOCAL int	sysIbcIntDisable (int);
LOCAL void	sysIbcIntLevelSet (UCHAR intNumber, UCHAR  pic1Mask,
				   UCHAR  pic2Mask);

void  		sysIbcIntHandler (void);


/*******************************************************************************
*
* sysIbcInit - initialize the ISA Bridge Controller 
*
* This routine initializes the Intel i82378 ISA Bridge Controller (IBC).
*
* RETURNS: OK, always.
*/

STATUS sysIbcInit (void)

    {
    UINT	vector;
    UCHAR	intVec;


    /* initialize the interrupt table */
    
    for (vector = 0; vector < 256; vector++)
	sysIntTbl[vector] = NULL;

    /* connect the interrupt demultiplexer to the PowerPC external interrupt */

    excIntConnect ((VOIDFUNCPTR *) _EXC_OFF_INTR, sysIbcIntHandler);

#ifdef ULTRA
    /*
     * route PCI interrupts to ISA IRQs and other initialization
     * PCI IRQ0: routed to LN_INT_LVL
     * PCI IRQ1: routed to NOTHING (no device supported)
     * PCI IRQ2: routed to SCSI_INT_LVL
     * PCI IRQ3: routed to NOTHING (no device supported)
     */

    *(ULONG *)(CNFG_IBC_ADRS + 0x60) = LONGSWAP((0x00000000 << 24) | 
						(SCSI_INT_LVL << 16) |
						(0x00000000 << 8) | 
						LN_INT_LVL);

    /* make the IRQ 15,14,11,10, 9 level sensitive */

    (*(char *)i82378_INT2_ELC) = 0x80 | 0x40 | 0x08 | 0x04 | 0x02 ;
#endif  /* #ifdef ULTRA */

#ifdef MV1300
    /*
     * route PCI interrupts to ISA IRQs and other initialization
     * PCI IRQ0: routed to NOTHING (no device supported)
     * PCI IRQ1: routed to VME2PCI_INT_LVL
     * PCI IRQ2: routed to NOTHING (no device supported)
     * PCI IRQ3: routed to IRQ15
     */

    *(ULONG *)(CNFG_IBC_ADRS + 0x60) = LONGSWAP(0x0f000000 | 
						(0x00000000 << 16) |
						(VME2PCI_INT_LVL << 8) | 
						0x00000000);

    /* make the IRQ 15,14,11,10, 9 level sensitive */

    (*(char *)i82378_INT2_ELC) = 0x80 | 0x40 | 0x08 | 0x04 | 0x02 ;
#endif  /* #ifdef MV1300 */

#ifdef MV1600
    /*
     * route PCI interrupts to ISA IRQs and other initialization
     * PCI IRQ0: routed to LN_INT_LVL
     * PCI IRQ1: routed to VME2PCI_INT_LVL
     * PCI IRQ2: routed to SCSI_INT_LVL
     * PCI IRQ3: routed to IRQ15
     */

    *(ULONG *)(CNFG_IBC_ADRS + 0x60) = LONGSWAP(0x0f000000 | 
						(SCSI_INT_LVL << 16) |
						(VME2PCI_INT_LVL << 8) |
						LN_INT_LVL);

    /* make the IRQ 15,14,11,10, 9 level sensitive */

    (*(char *)i82378_INT2_ELC) = 0x80 | 0x40 | 0x08 | 0x04 | 0x02 ;
#endif  /* #ifdef MV1600 */


    /* initialize the IBC (ISA Bridge Controller) */

    IBC_BYTE_WRITE (PIC_port1 (PIC1_BASE_ADR),0x11);        /* ICW1 */
    IBC_BYTE_WRITE (PIC_port2 (PIC1_BASE_ADR),sysVectorIRQ0); /* ICW2 */
    IBC_BYTE_WRITE (PIC_port2 (PIC1_BASE_ADR),0x04);        /* ICW3 */
    IBC_BYTE_WRITE (PIC_port2 (PIC1_BASE_ADR),0x01);        /* ICW4 */

    IBC_BYTE_WRITE (PIC_port1 (PIC2_BASE_ADR),0x11);        /* ICW1 */
    IBC_BYTE_WRITE (PIC_port2 (PIC2_BASE_ADR),sysVectorIRQ0+8); /* ICW2 */
    IBC_BYTE_WRITE (PIC_port2 (PIC2_BASE_ADR),0x02);        /* ICW3 */
    IBC_BYTE_WRITE (PIC_port2 (PIC2_BASE_ADR),0x01);        /* ICW4 */

    /* disable interrupts */

    IBC_BYTE_WRITE (PIC_IMASK (PIC1_BASE_ADR),0xfb);
    IBC_BYTE_WRITE (PIC_IMASK (PIC2_BASE_ADR),0xff);

    /* set the BSP driver specific Interrupt Handler and intConnect routines */

    _func_intConnectRtn = sysIbcIntConnect;
    _func_intEnableRtn = sysIbcIntEnable;
    _func_intDisableRtn = sysIbcIntDisable;

    /* clear the pending interrupts */

    intVec = (*(char *)ISA_INTR_ACK_REG);

    sysIbcEndOfInt ();		/* perform the end of interrupt procedure */

    return (OK);
    }

/*******************************************************************************
*
* sysIbcIntEnable - enable an IBC interrupt number
*
* This routine enables a specified IBC interrupt number.
*
* RETURNS: OK.
*
* ARGSUSED0
*/

LOCAL int sysIbcIntEnable
    (
    int intNumber        /* interrupt number to enable */
    )
    {
    UINT8 temp;
    UINT8 *pReg;

    if (intNumber < 8)
	{
	pReg = (UINT8 *) PIC_IMASK (PIC1_BASE_ADR);
	}
    else
	{
	pReg = (UINT8 *) PIC_IMASK (PIC2_BASE_ADR);
	intNumber -= 8;
	}

    IBC_BYTE_READ (pReg, &temp);
    IBC_BYTE_WRITE (pReg, temp & ~(1 << intNumber));

    return (OK);
    }

/*******************************************************************************
*
* sysIbcIntDisable - disable an IBC interrupt number
*
* This routine disables a specified IBC interrupt number.
*
* RETURNS: OK.
*
* ARGSUSED0
*/

LOCAL int sysIbcIntDisable
    (
    int intNumber        /* interrupt number to disable */
    )
    {
    UINT8* pReg;
    UINT8 temp;

    if (intNumber < 8)
	{
	pReg = (UINT8 *) PIC_IMASK (PIC1_BASE_ADR);
	}
    else
	{
	pReg = (UINT8 *) PIC_IMASK (PIC2_BASE_ADR);
	intNumber -= 8;
	}

    IBC_BYTE_READ (pReg, &temp);
    IBC_BYTE_WRITE (pReg, temp | (1 << intNumber));
    return (OK);
    }

/******************************************************************************
*
* sysIbcIntConnect - connect an interrupt handler to the system vector table
*
* This function connects an interrupt handler to the system vector table.
*
* RETURNS: OK/ERROR.
*/

LOCAL STATUS sysIbcIntConnect
    (
     VOIDFUNCPTR * 	vector,		/* interrupt vector to attach */
     VOIDFUNCPTR   	routine,	/* routine to be called */
     int	        parameter	/* parameter to be passed to routine */
    )
    {
    INT_HANDLER_DESC * newHandler;
    INT_HANDLER_DESC * currHandler;

    if (((int)vector < 0) || ((int)vector > 0xff))	/* Out of Range? */
	return (ERROR);		

    /* create a new interrupt handler */

    newHandler = malloc (sizeof (INT_HANDLER_DESC));

    /* check if the memory allocation succeed */

    if (newHandler == NULL)
	return (ERROR);

    /* initialize the new handler */

    newHandler->vec  = routine;
    newHandler->arg  = parameter;
    newHandler->next  = NULL;

    if (sysIntTbl[(int) vector] == NULL)
	sysIntTbl [(int ) vector] = newHandler;	/* single int. handler case */
    else
	{
	currHandler = sysIntTbl[(int) vector];	/* multiple int. handler case */
	while (currHandler->next != NULL)
	    {
	    currHandler = currHandler->next;
	    }
	currHandler->next = newHandler;
	}

    return (OK);
    }

/******************************************************************************
*
* sysIbcIntHandler - handle an i82378 IBC interrupt
*
* This routine is called to handle an Intel i82378 ISA Bridge Controller
* (IBC) interrupt exception.  
*
* This routine generates the PCI IACK cycle to get the interrupt vector from
* the PIC chips.  Once the vector is obtained, it is used as an index into
* the system vector table to locate the user-defined interrupt routine for
* that vector.  Each entry in the vector table is a part of a singly-linked
* list and all routines on that list are executed in order.
*
* RETURNS: N/A
*/

void sysIbcIntHandler (void)
    {
    UCHAR		intVec;
    INT_HANDLER_DESC *	currHandler;
    char                intReg1;
    char                intReg2;
    int 		lock;

    /* 
     * Reading ISA_INTR_ACK_REG would generate PCI IACK cycles on the 
     * PCI bus which would cause the IBC to put out vector from the 82378
     * on the PCI bus. 
     * Bit 5 of PCI control register in the IBC i82378 should be
     * programmed to 1 (By default it is 1) if the IBC should respond with
     * a vector when PCI IACK cycles are generated. 
     */ 

    intVec = (*(char *)ISA_INTR_ACK_REG);

#ifdef INCLUDE_WINDVIEW
    WV_EVT_INT_ENT(intVec)
#endif

    /* Record current interrupt mask settings */

    IBC_BYTE_READ (PIC_IMASK (PIC1_BASE_ADR), &intReg1);
    IBC_BYTE_READ (PIC_IMASK (PIC2_BASE_ADR), &intReg2);

    /* Mask this and lower priority interrupt levels */

    sysIbcIntLevelSet (intVec, intReg1, intReg2);

    /* 
     * Now the interrupt acknowledge is made, the interrupt can be re-enabled
     */

    intUnlock (_PPC_MSR_EE);
    
    /* call EACH respective interrupt handler */

    if ((currHandler = sysIntTbl [intVec]) == NULL)
	logMsg ("uninitialized interrupt %d\n", intVec, 0,0,0,0,0);
    
    else
	{
	while (currHandler != NULL)
	    {
	    currHandler->vec (currHandler->arg);
	    currHandler = currHandler->next;
	    }
	}

    sysIbcEndOfInt ();		/* perform the end of interrupt procedure */

    /* Set the original interrupt masks back */

    lock = intLock ();

    IBC_BYTE_WRITE (PIC_IMASK (PIC1_BASE_ADR), intReg1);
    IBC_BYTE_WRITE (PIC_IMASK (PIC2_BASE_ADR), intReg2);
    }

/*******************************************************************************
*
* sysIbcEndOfInt - send EOI(end of interrupt) signal.
*
* This routine is called at the end of the interrupt handler.
*
*/

LOCAL void sysIbcEndOfInt (void)
    {
    IBC_BYTE_WRITE (PIC_IACK (PIC1_BASE_ADR), 0x20);
    IBC_BYTE_WRITE (PIC_IACK (PIC2_BASE_ADR), 0x20);
    }

/*******************************************************************************
*
* sysIbcIntLevelSet - set the interrupt mask
*
* This routine masks interrupts from the level of 'intNumber' to 15.
*
* RETURNS: N/A 
*/

LOCAL void sysIbcIntLevelSet
    (
    UCHAR  intNumber,	/* interrupt level where to mask from */
    UCHAR  pic1Mask,	/* state of Pic1 mask at IRQ */
    UCHAR  pic2Mask	/* state of Pic2 mask at IRQ */
    )
    {
    pic1Mask |= ibcIRQ2Pic1Mask[intNumber];		  /* make new masks */
    pic2Mask |= ibcIRQ2Pic2Mask[intNumber];
    IBC_BYTE_WRITE (PIC_IMASK (PIC1_BASE_ADR), pic1Mask); /* set new masks */
    IBC_BYTE_WRITE (PIC_IMASK (PIC2_BASE_ADR), pic2Mask);
    }
