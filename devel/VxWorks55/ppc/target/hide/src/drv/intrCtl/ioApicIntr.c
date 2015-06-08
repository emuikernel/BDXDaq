/* ioApicIntr.c - Intel IO APIC/xAPIC driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01g,08mar02,hdn  updated ioApicInit(), added ioApicEnable()
		 removed redTable, added ioApicRedS/Get() for HTT (spr 73738)
01f,29nov01,hdn  doc update for 5.5
01e,07nov01,hdn  updated the interrupt delivery type setting
		 added new routine ioApicIrqMove ()
01d,26jun01,hdn  added support for Pentium4 IO xAPIC
		 moved show routine to ioApicIntrShow.c
01c,17nov00,mks  importing loApicBase and loApicId for successful compilation,
                 added redTablePtr and made redTableNumEnt global. SPR 35733.
01b,25mar98,hdn  re-written.
01a,20jun97,sub  written.
*/

/*
DESCRIPTION
This module is a driver for the IO APIC/xAPIC (Advanced Programmable 
Interrupt Controller) for P6 (PentiumPro, II, III) family processors
and P7 (Pentium4) family processors.  The IO APIC/xAPIC is included 
in the Intel's system chip set, such as ICH2.  Software intervention 
may be required to enable the IO APIC/xAPIC in some chip sets.
The 8259A interrupt controller is intended for use in a uni-processor
system, IO APIC can be used in either a uni-processor or multi-processor
system.  The IO APIC handles interrupts very differently than the 8259A.
Briefly, these differences are:
 - Method of Interrupt Transmission. The IO APIC transmits interrupts 
   through a 3-wire bus and interrupts are handled without the need for
   the processor to run an interrupt acknowledge cycle.
 - Interrupt Priority. The priority of interrupts in the IO APIC is 
   independent of the interrupt number.  For example, interrupt 10 can 
   be given a higher priority than interrupt 3.
 - More Interrupts. The IO APIC supports a total of 24 interrupts.

The IO APIC unit consists of a set of interrupt input signals, a 24-entry
by 64-bit Interrupt Redirection Table, programmable registers, and a message
unit for sending and receiving APIC messages over the APIC bus or the 
Front-Side (system) bus.  IO devices inject interrupts into the system by 
asserting one of the interrupt lines to the IO APIC.  The IO APIC selects the 
corresponding entry in the Redirection Table and uses the information in that 
entry to format an interrupt request message.  Each entry in the Redirection 
Table can be individually programmed to indicate edge/level sensitive interrupt 
signals, the interrupt vector and priority, the destination processor, and how 
the processor is selected (statically and dynamically).  The information in 
the table is used to transmit a message to other APIC units (via the APIC bus
or the Front-Side (system) bus).
IO APIC is used in the Symmetric IO Mode (define SYMMETRIC_IO_MODE in the BSP).
The base address of IO APIC is determined in loApicInit() and stored in the
global variable ioApicBase and ioApicData.  ioApicInit() initializes the IO 
APIC with information stored in ioApicRed0_15 and ioApicRed16_23.
The ioApicRed0_15 is the default lower 32 bit value of the redirection table 
entries for IRQ 0 to 15 that are edge triggered positive high, and
the ioApicRed16_23 is for IRQ 16 to 23 that are level triggered positive low.
ioApicRedSet() and ioApicRedGet() are used to access the redirection table.
ioApicEnable() enables the IO APIC or xAPIC.  ioApicIrqSet() set the specific
IRQ to be delivered to the specific Local APIC.
This implementation doesn't support multiple IO APIC configuration.

INCLUDE FILES: loApic.h, ioApic.h

SEE ALSO: loApicIntr.c
*/

/* includes */

#include "intLib.h"
#include "drv/intrCtl/loApic.h"
#include "drv/intrCtl/ioApic.h"


/* externs */

IMPORT UINT32 ioApicBase;	/* IO APIC register select (index) address */
IMPORT UINT32 ioApicData;	/* IO APIC window (data) addr */
IMPORT UINT32 loApicBase;	/* addr of Local APIC */
IMPORT UINT32 loApicId;		/* local APIC Id */
IMPORT UINT32 sysProcessor;	/* 0=386, 1=486, 2=P5, 4=P6, 5=P7 */
IMPORT INT8   excCallTbl [];	/* table of Calls in excALib.s */
IMPORT INT32  sysCsInt;		/* CS for interrupt */
IMPORT VOIDFUNCPTR intEoiGet;   /* function used in intConnect() for B/EOI */
IMPORT UINT32 sysIntIdtType;	/* IDT entry type */
IMPORT BOOL   sysBp;		/* TRUE(default) for BP, FALSE for AP */
IMPORT void   sysPciIoApicEnable(); /* enable/disable the IO APIC */
IMPORT void   sysPciPirqEnable();   /* enable/disable the PCI PIRQ */


/* globals */

UINT32 ioApicVersion;		/* IO APIC version register */
UINT32 ioApicRedEntries = 24;	/* default IO APIC redirection table entries */
UINT32 ioApicRed0_15	= IOAPIC_EDGE | IOAPIC_HIGH | IOAPIC_FIXED;
UINT32 ioApicRed16_23	= IOAPIC_LEVEL | IOAPIC_LOW | IOAPIC_FIXED;


/* locals */

LOCAL	UINT32 * ioApicIntTable;


/* forward declarations */

STATUS	ioApicIrqMove (INT32 srcIrq, INT32 dstIrq);	/* not published yet */


/*******************************************************************************
*
* ioApicInit - initialize the IO APIC or xAPIC
*
* This routine initializes the IO APIC or xAPIC.
*
*/

STATUS ioApicInit (void)
    {
    INT32 ix;
    INT32 offset;

    /* enable the IOAPIC */

    if (sysBp)
        ioApicEnable (TRUE);

    /* get the IOAPIC version */

    ioApicVersion = ioApicGet (ioApicBase, ioApicData, IOAPIC_VERS);

    /* get a number of redirection table entries */

    ioApicRedEntries = ((ioApicVersion & IOAPIC_MRE_MASK) >> 16) + 1;

    /* allocate memory for saving the interrupt mask bit */

    ioApicIntTable = (UINT32 *) memalign (_CACHE_ALIGN_SIZE,
		     ioApicRedEntries * sizeof (UINT32));

    /* return if it is not the Boot Processor (BP) */

    if (sysBp == FALSE)
        return (OK);

    /* set the IOAPIC bus arbitration ID */

    ioApicSet (ioApicBase, ioApicData, IOAPIC_ID,
    	       ioApicGet (ioApicBase, ioApicData, IOAPIC_ID));

    /* set the int-delivery type to Front Side Bus, not APIC Serial Bus */

    if (sysProcessor == X86CPU_PENTIUM4)
        ioApicSet (ioApicBase, ioApicData, IOAPIC_BOOT, IOAPIC_DT_FS);

    /* initialize the redirection table with the default value */

    for (ix = 0; ix < ioApicRedEntries; ix++)
	{
	offset = IOAPIC_REDTBL + (ix * 2);

	if (ix < 16)
	    {
	    ioApicSet (ioApicBase, ioApicData, offset,
	        IOAPIC_INT_MASK | IOAPIC_PHYSICAL |
	        ioApicRed0_15 | sysInumTbl[ix]);
	    }
	else
	    {
	    ioApicSet (ioApicBase, ioApicData, offset,
	        IOAPIC_INT_MASK | IOAPIC_PHYSICAL |
	        ioApicRed16_23 | sysInumTbl[ix]);
	    }
        ioApicSet (ioApicBase, ioApicData, offset + 1, (loApicId  << 24));
	}

    return (OK);
    }

/*******************************************************************************
*
* ioApicEnable - enable or disable the IO xAPIC in ICHx
*
* This routine enables or disables the IO xAPIC in ICHx.  If the parameter is 
* TRUE, the IO xAPIC in ICHx is enabled and the default ISA IRQ routing 
* information is saved in the table.  If the parameter is FALSE, the IO xAPIC
* in ICHx is disabled.
*
* RETURNS: N/A
*/

void ioApicEnable
    (
    BOOL enable		/* enable the IO xAPIC in ICHx */
    )
    {
    /* enables or disables the IO APIC */

    sysPciIoApicEnable (enable);

    /* enables or disables the PIRQ direct handling */
    
    sysPciPirqEnable (enable);
    }

/*******************************************************************************
*
* ioApicIntEoi -  send EOI (End Of Interrupt) signal to IO APIC
*
* This routine sends an EOI signal to the IO APIC's interrupting source.
*
* RETURNS: N/A
*/

LOCAL void ioApicIntEoi 
    (
    INT32 irqNo		/* INTIN number to send EOI */
    )
    {
#ifdef	SYS_INT_DEBUG
    extern UINT32 sysIntCount [];
    sysIntCount [irq]++;
#endif	/* SYS_INT_DEBUG */

#if	FALSE	/* don't need it for now */
    if (ioApicVersion & IOAPIC_PRQ)
        *(int *)(ioApicBase + IOAPIC_EOI) = INT_NUM_GET(irq);
#endif

    *(int *)(loApicBase + LOAPIC_EOI) = 0;
    }

/*******************************************************************************
*
* ioApicIntEnable - enable a specified APIC interrupt input line
*
* This routine enables a specified APIC interrupt input line.
*
* RETURNS: OK or ERROR if the interrupt input line number is invalid 
*
* SEE ALSO: ioApicIntDisable()
*
* ARGSUSED0
*/

LOCAL STATUS ioApicIntEnable
    (
    INT32 irqNo		/* INTIN number to enable */
    )
    {
    INT32 offset = IOAPIC_REDTBL + (irqNo * 2);

    /* check if IRQ is within range */

    if (irqNo >= ioApicRedEntries)
	return (ERROR);

    /* set the destination address (my APIC ID), and enable the interrupt */

    ioApicSet (ioApicBase, ioApicData, offset + 1, (loApicId << 24));
    ioApicSet (ioApicBase, ioApicData, offset, 
	       ioApicGet (ioApicBase, ioApicData, offset) & ~IOAPIC_INT_MASK);

    return (OK);
    }

/*******************************************************************************
*
* ioApicIntDisable - disable a specified APIC interrupt input line
*
* This routine disables a specified APIC interrupt input line.
*
* RETURNS: OK or ERROR if the interrupt input line number is invalid 
*
* SEE ALSO: ioApicIntEnable()
*
* ARGSUSED0
*/

LOCAL STATUS ioApicIntDisable
    (
    INT32 irqNo		/* INTIN number to disable */
    )
    {
    INT32 offset = IOAPIC_REDTBL + (irqNo * 2);

    /* check if IRQ is within range */

    if (irqNo >= ioApicRedEntries)
	return (ERROR);

    /* disable the interrupt */

    ioApicSet (ioApicBase, ioApicData, offset, 
	       ioApicGet (ioApicBase, ioApicData, offset) | IOAPIC_INT_MASK);

    return (OK);
    }

/*******************************************************************************
*
* ioApicIntLock - lock out all IO APIC interrupts
*
* This routine saves the mask and locks out all IO APIC interrupts.
* It should be called in the interrupt disable state(IF bit is 0).
*
* RETURNS: N/A 
*
* SEE ALSO: ioApicIntUnlock()
*
* ARGSUSED0
*/

LOCAL VOID ioApicIntLock (void)

    {
    INT32 ix;
    INT32 upper32;
    INT32 lower32;

    for (ix = 0; ix < ioApicRedEntries; ix++) 
	{
    	ioApicRedGet (ix, &upper32, &lower32);

	/* lock if the destination is my Local APIC and unlocked */

	if ((((upper32 & IOAPIC_DESTINATION) >> 24) == loApicId) &&
	    ((lower32 & IOAPIC_INT_MASK) == 0))
	    {
    	    ioApicSet (ioApicBase, ioApicData, IOAPIC_REDTBL + (ix *2),
    	               lower32 | IOAPIC_INT_MASK);
	    *(ioApicIntTable + ix) = lower32;	/* save the old state */
	    }
	}
    }

/*******************************************************************************
*
* ioApicIntUnlock - unlock the IO APIC interrupts
*
* This routine restores the mask and unlocks the IO APIC interrupts
* It should be called in the interrupt disable state(IF bit is 0).
*
* RETURNS: N/A 
*
* SEE ALSO: ioApicIntLock()
*
* ARGSUSED0
*/

LOCAL VOID ioApicIntUnlock (void)

    {
    INT32 ix;
    INT32 upper32;
    INT32 lower32;

    for (ix = 0; ix < ioApicRedEntries; ix++)
	{
    	ioApicRedGet (ix, &upper32, &lower32);

	/* unlock if the destination is my Local APIC and unlocked */

	if ((((upper32 & IOAPIC_DESTINATION) >> 24) == loApicId) &&
	    ((*(ioApicIntTable + ix) & IOAPIC_INT_MASK) == 0))
	    {
    	    ioApicSet (ioApicBase, ioApicData, IOAPIC_REDTBL + (ix *2),
    	               lower32 & ~IOAPIC_INT_MASK);
	    }
	}
    }

/*******************************************************************************
*
* ioApicGet - get a value from the IO APIC register.
*
* This routine gets a value from the IO APIC register.
*
* RETURNS: A value of the IO APIC register.
*/

INT32 ioApicGet
    (
    UINT32 index,		/* IO register select (index) */
    UINT32 data,		/* IO window (data) */
    INT32  offset		/* offset to the register */
    )
    {
    INT32 value;
    INT32 oldLevel;

    oldLevel = intLock ();		/* LOCK INTERRUPT */
    *(char *)index = (char)offset;	/* select the register */
    value = *(int *)data;		/* must be a 32bit read */
    intUnlock (oldLevel);		/* UNLOCK */

    return (value);
    }

/*******************************************************************************
*
* ioApicSet - set a value into the IO APIC register.
*
* This routine sets a value into the IO APIC register.
*
* RETURNS: N/A
*/

void ioApicSet
    (
    UINT32 index,		/* IO register select (index) */
    UINT32 data,		/* IO window (data) */
    INT32  offset,		/* offset to the register */
    INT32  value		/* value to set the register */
    )
    {
    INT32 oldLevel;

    oldLevel = intLock ();		/* LOCK INTERRUPT */
    *(char *)index = (char)offset;	/* select the register */
    *(int *)data = value;		/* must be a 32bit write */
    intUnlock (oldLevel);		/* UNLOCK */
    }

/*******************************************************************************
*
* ioApicRedGet - get a specified entry in the Redirection Table
*
* This routine gets a specified entry in the Redirection Table
*
* RETURNS: OK, or ERROR if intNum is out of range
*/

STATUS ioApicRedGet
    (
    INT32 irq,			/* index of the table */
    INT32 * pUpper32,		/* upper 32 bit (MS LONG) */
    INT32 * pLower32		/* lower 32 bit (LS LONG) */
    )
    {
    INT32 offset = IOAPIC_REDTBL + (irq * 2);

    if (irq >= ioApicRedEntries)
	return (ERROR);

    *pLower32 = ioApicGet (ioApicBase, ioApicData, offset);
    *pUpper32 = ioApicGet (ioApicBase, ioApicData, offset + 1);

    return (OK);
    }

/*******************************************************************************
*
* ioApicRedSet - set a specified entry in the Redirection Table
*
* This routine sets a specified entry in the Redirection Table
*
* RETURNS: OK, or ERROR if intNum is out of range
*/

STATUS ioApicRedSet
    (
    INT32 irq,			/* index of the table */
    INT32 upper32,		/* upper 32 bit (MS LONG) */
    INT32 lower32		/* lower 32 bit (LS LONG) */
    )
    {
    INT32 offset = IOAPIC_REDTBL + (irq * 2);

    if (irq >= ioApicRedEntries)
	return (ERROR);

    ioApicSet (ioApicBase, ioApicData, offset, lower32);
    ioApicSet (ioApicBase, ioApicData, offset + 1, upper32);

    return (OK);
    }

/*******************************************************************************
*
* ioApicIrqMove - move interrupt handler from source IRQ to destination IRQ
*
* This routine moves the interrupt handler from the source IRQ to the 
* destination IRQ.  If the source IRQ's interrupt handler is the default 
* handler, it just returns.  Otherwise it is moved and the destination IRQ's 
* vector table entry will be overridden.  This routine is for the chipset
* that have programmable IRQ capability.  This routine should be called at
* the initialization time, otherwise there are no guarantee for losing 
* interrupts.  The polarity and the trigger mode of the IRQ is not changed.
*
* RETURNS: OK, or ERROR if the source intHandler is the default handler.
*/

STATUS ioApicIrqMove
    (
    INT32 srcIrq,	/* source IRQ moving from */
    INT32 dstIrq	/* destination IRQ moving to */
    )
    {
    FUNCPTR intHandler;		/* interrupt handler */
    INT32 idtType;		/* IDT type */
    INT32 selector;		/* code selector */
    INT32 srcInum = INT_NUM_GET (srcIrq);
    INT32 dstInum = INT_NUM_GET (dstIrq);
    INT8 * pCode;		/* pointer to intConnect code */
    INT32 oldLevel;

    /* lock interrupts */

    oldLevel = intLock ();		/* LOCK INTERRUPTS */
    ioApicIntDisable (srcIrq);
    ioApicIntDisable (dstIrq);

    /* get the info from the source IRQ vector table entry */

    intVecGet2 ((FUNCPTR *)INUM_TO_IVEC (srcInum), &intHandler, 
		&idtType, &selector);

    /* return if the intHandler is the default handler */

    if (intHandler == (FUNCPTR)&excCallTbl[srcInum * 5])
        return (ERROR);

    /* set the info to the destination IRQ vector table entry */

    intVecSet2 ((FUNCPTR *)INUM_TO_IVEC (dstInum), intHandler, 
		idtType, selector);

    /* update the IRQ (parameter of EOI routine) in the intConnect code */

    if (intEoiGet != NULL)
	{
	pCode = (INT8 *)intHandler;
	*(INT32 *)(pCode + ICC_EOI_PARAM) = dstIrq;
	}

    /* set the default interrupt handler to the source IRQ */

    intVecSet2 ((FUNCPTR *)INUM_TO_IVEC (srcInum), 
		(FUNCPTR)&excCallTbl[srcInum * 5],
		sysIntIdtType, sysCsInt);

    /* unlock interrupts */

    intUnlock (oldLevel);		/* UNLOCK INTERRUPTS */
    ioApicIntEnable (dstIrq);

    return (OK);
    }
    
/*******************************************************************************
*
* ioApicIrqSet - set the Local APIC ID of the specified IRQ
*
* This routine sets the Local APIC ID of the specified IRQ in the IO APIC
* Redirection Table.
*
* RETURNS: OK, or ERROR if the IRQ is out of bound
*/

STATUS ioApicIrqSet
    (
    INT32 irq,		/* IRQ */
    INT32 apicId	/* Local APIC ID */
    )
    {
    INT32 upper32;
    INT32 oldLevel;
    INT32 offset = IOAPIC_REDTBL + (irq * 2);

    if (irq >= ioApicRedEntries)
	return (ERROR);

    /* lock interrupts */

    oldLevel = intLock ();		/* LOCK INTERRUPTS */
    ioApicIntDisable (irq);

    /* set the loApicId */

    upper32 = ioApicGet (ioApicBase, ioApicData, offset + 1);
    upper32 = (upper32 & ~IOAPIC_DESTINATION) | (apicId << 24);
    ioApicSet (ioApicBase, ioApicData, offset + 1, upper32);

    /* unlock interrupts */

    intUnlock (oldLevel);		/* UNLOCK INTERRUPTS */
    ioApicIntEnable (irq);

    return (OK);
    }
