/* i8259Pic.c - Intel 8259A PIC (Programmable Interrupt Controller) driver */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01g,17mar97,hdn  added sysIntLock(), sysIntUnlock() for the system mode.
01f,25jun96,hdn  added sysIntLevel() for windview.
01e,23may96,wlf  doc: cleanup.
01d,14jun95,hdn  renamed sysEndOfInt to sysIntEOI.
		 moved global function prototypes to sysLib.h.
01c,08aug94,hdn  stopped toggling IRQ9 in enabling and disabling.
01b,22apr94,hdn  made IRQ9 off in the initialization.
		 moved sysVectorIRQ0 to sysLib.c.
01a,05sep93,hdn  written.
*/

/*
DESCRIPTION
This module implements the Intel 8259A PIC driver.
*/

#include "drv/intrCtl/i8259a.h"


/* globals */

IMPORT UINT	sysVectorIRQ0;		/* vector for IRQ0 */


/* local */

char		sysIntMask1;		/* interrupt mask for PIC1 */
char		sysIntMask2;		/* interrupt mask for PIC2 */


/* forward static functions */

LOCAL void sysIntInitPIC (void);
LOCAL void sysIntEOI   (void);


/*******************************************************************************
*
* sysIntInitPIC - initialize the PIC
*
* This routine initializes the PIC.
*
*/

LOCAL void sysIntInitPIC (void)

    {

    /* initialize the PIC (Programmable Interrupt Controller) */

    sysOutByte (PIC_port1 (PIC1_BASE_ADR),0x11);        /* ICW1 */
    sysOutByte (PIC_port2 (PIC1_BASE_ADR),sysVectorIRQ0); /* ICW2 */
    sysOutByte (PIC_port2 (PIC1_BASE_ADR),0x04);        /* ICW3 */
    sysOutByte (PIC_port2 (PIC1_BASE_ADR),0x01);        /* ICW4 */

    sysOutByte (PIC_port1 (PIC2_BASE_ADR),0x11);        /* ICW1 */
    sysOutByte (PIC_port2 (PIC2_BASE_ADR),sysVectorIRQ0+8); /* ICW2 */
    sysOutByte (PIC_port2 (PIC2_BASE_ADR),0x02);        /* ICW3 */
    sysOutByte (PIC_port2 (PIC2_BASE_ADR),0x01);        /* ICW4 */

    /* disable interrupts */

    sysOutByte (PIC_IMASK (PIC1_BASE_ADR),0xfb);
    sysOutByte (PIC_IMASK (PIC2_BASE_ADR),0xff);
    }

/*******************************************************************************
*
* sysIntEOI - send EOI(end of interrupt) signal.
*
* This routine is called at the end of the interrupt handler.
*
*/

LOCAL void sysIntEOI (void)

    {
    int oldLevel;

    oldLevel = intLock ();
    sysOutByte (PIC_IACK (PIC1_BASE_ADR), 0x20);
    sysOutByte (PIC_IACK (PIC2_BASE_ADR), 0x20);
    intUnlock (oldLevel);
    }

/*******************************************************************************
*
* sysIntDisablePIC - disable a PIC interrupt level
*
* This routine disables a specified PIC interrupt level.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysIntEnablePIC()
*
* ARGSUSED0
*/

STATUS sysIntDisablePIC
    (
    int intLevel        /* interrupt level to disable */
    )
    {

    if (intLevel < 8)
	{
	sysOutByte (PIC_IMASK (PIC1_BASE_ADR),
	    sysInByte (PIC_IMASK (PIC1_BASE_ADR)) | (1 << intLevel));
	}
    else
	{
	sysOutByte (PIC_IMASK (PIC2_BASE_ADR),
	    sysInByte (PIC_IMASK (PIC2_BASE_ADR)) | (1 << (intLevel - 8)));
	}

    return (OK);
    }

/*******************************************************************************
*
* sysIntEnablePIC - enable a PIC interrupt level
*
* This routine enables a specified PIC interrupt level.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysIntDisablePIC()
*
* ARGSUSED0
*/

STATUS sysIntEnablePIC
    (
    int intLevel        /* interrupt level to enable */
    )
    {

    if (intLevel < 8)
	{
	sysOutByte (PIC_IMASK (PIC1_BASE_ADR),
	    sysInByte (PIC_IMASK (PIC1_BASE_ADR)) & ~(1 << intLevel));
	}
    else
	{
	sysOutByte (PIC_IMASK (PIC2_BASE_ADR),
	    sysInByte (PIC_IMASK (PIC2_BASE_ADR)) & ~(1 << (intLevel - 8)));
	}

    return (OK);
    }

/*******************************************************************************
*
* sysIntLevel - Get interrupt level by reading Interrupt Service Register.
*
* This routine is called to get an interrupt level in service.
*
* RETURNS: 0 - 15.
*
* ARGSUSED0
*/

int sysIntLevel (void)

    {
    int inserviceReg;
    int level;
    int oldLevel;
    int retry;

    oldLevel = intLock ();

    for (retry=0; retry < 1; retry ++)
	{
        sysOutByte (PIC_port1 (PIC1_BASE_ADR), 0x0b);
        inserviceReg = sysInByte (PIC_port1 (PIC1_BASE_ADR));
        for (level=0; level < 8; level++)
	    if ((inserviceReg & 1) && (level != 2))
	        goto sysIntLevelExit;
	    else
	        inserviceReg >>= 1;

        sysOutByte (PIC_port1 (PIC2_BASE_ADR), 0x0b);
        inserviceReg = sysInByte (PIC_port1 (PIC2_BASE_ADR));
        for (level=8; level < 16; level++)
	    if (inserviceReg & 1)
	        goto sysIntLevelExit;
	    else
	        inserviceReg >>= 1;
	}

    sysIntLevelExit:
    intUnlock (oldLevel);

    return (level);
    }

/*******************************************************************************
*
* sysIntLock - lock out all PIC interrupts
*
* This routine saves the mask and locks out all PIC interrupts.
* It should be called in the interrupt disable state(IF bit is 0).
*
* SEE ALSO: sysIntUnlock()
*
* ARGSUSED0
*/

VOID sysIntLock (void)

    {

    sysIntMask1 = sysInByte (PIC_IMASK (PIC1_BASE_ADR));
    sysIntMask2 = sysInByte (PIC_IMASK (PIC2_BASE_ADR));
    sysOutByte (PIC_IMASK (PIC1_BASE_ADR), 0xff);
    sysOutByte (PIC_IMASK (PIC2_BASE_ADR), 0xff);
    }

/*******************************************************************************
*
* sysIntUnlock - unlock the PIC interrupts
*
* This routine restores the mask and unlocks the PIC interrupts
* It should be called in the interrupt disable state(IF bit is 0).
*
* SEE ALSO: sysIntLock()
*
* ARGSUSED0
*/

VOID sysIntUnlock (void)

    {

    sysOutByte (PIC_IMASK (PIC1_BASE_ADR), sysIntMask1);
    sysOutByte (PIC_IMASK (PIC2_BASE_ADR), sysIntMask2);
    }
