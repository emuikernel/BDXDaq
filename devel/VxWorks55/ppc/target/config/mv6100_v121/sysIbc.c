/* sysIbc.c - Ibc Interrupt Controller driver */

/* Copyright 2001-2002 Wind River Systems, Inc. */
/* Copyright 2000-2003 Motorola, Inc., All Rights Reserved */

#include "copyright_wrs.h"

/*
modification history
--------------------
01a,08jan03,cak  Ported, with modifications, from ver 01d, lopec/sysIbc.c.
*/

/*

DESCRIPTION
This module implements the Intel 8259 compatable (IBC) ISA 
interrupt controller.

*/

/* includes */

#include "sysIbc.h"

/* defines */

#define IBC_INT_PENDING 0x80
#define IBC_INT_LVL2    2
#define IBC_INT_LVL7    7
#define IBC_INT_LVL15   15

/* external declarations */

IMPORT void   sysOutByte (ULONG, UCHAR);
IMPORT UCHAR  sysInByte (ULONG);
IMPORT void   sysPciRead32 (UINT32, UINT32 *);
IMPORT void   sysPciWrite32 (UINT32, UINT32);
IMPORT UINT   sysVectorIRQ0; 	/* vector for IRQ0 */

/* global declarations */

/* forward declarations */

STATUS        sysIbcInit (void);
int           sysIbcIntEnable (int);
int           sysIbcIntDisable (int);
void          sysIbcIntHandler (void);

LOCAL UCHAR   sysIbcPhantomInt (UCHAR *intNum, int lvl7Int, int lvl15Int);
LOCAL void    sysIbcEndOfInt (int);
LOCAL void    sysIbcIntLevelSet (int);

/* Mask values are the currently disabled sources */

LOCAL UINT8	  sysPicMask1 = 0xfb;	/* all levels disabled */
LOCAL UINT8	  sysPicMask2 = 0xff;

/* Level values are the interrupt level masks */

LOCAL UINT8	  sysPicLevel1;
LOCAL UINT8	  sysPicLevel2;
LOCAL UINT8	  sysPicLevelCur;	/* current priority level, 0 to 16 */

/* level values by real priority */

LOCAL UCHAR   sysPicPriMask1[17] = 
                  {0xFB,0xFA,0xF8,0xF8,0xF0,0xE0,0xC0,0x80,
			       0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0x0};
LOCAL UCHAR   sysPicPriMask2[17] = 
                  {0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
			       0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x0};

/* Hardware access methods */

#ifndef IBC_BYTE_OUT
#   define IBC_BYTE_OUT(reg,data) \
	(sysOutByte (reg,data))
#endif /* IBC_BYTE_OUT */

#ifndef IBC_BYTE_IN
#   define IBC_BYTE_IN(reg,pData) \
	(*pData = sysInByte(reg))
#endif /* IBC_BYTE_IN */

/*******************************************************************************
*
* sysIbcInit - Initialize the IBC
*
* This routine initializes the non-PCI Header configuration registers of the
* IBC within the W83C553 PIB.
*
* RETURNS: OK always
*/

STATUS sysIbcInit (void)
    {
    UCHAR	intVec;

    /* Initialize the Interrupt Controller #1 */

    IBC_BYTE_OUT (PIC_port1 (PIC1_BASE_ADR),0x11);        /* ICW1 */
    IBC_BYTE_OUT (PIC_port2 (PIC1_BASE_ADR),sysVectorIRQ0); /* ICW2 */
    IBC_BYTE_OUT (PIC_port2 (PIC1_BASE_ADR),0x04);	/* ICW3 */
    IBC_BYTE_OUT (PIC_port2 (PIC1_BASE_ADR),0x01);	/* ICW4 */

    /*
     *	Mask interrupts IRQ 0, 1, and 3-7 by writing to OCW1 register
     *	IRQ 2 is the cascade input
     */

    IBC_BYTE_OUT (PIC_IMASK (PIC1_BASE_ADR),0xfb);

    /* Insure that all controller interrupts are edge sensitive */

    IBC_BYTE_OUT (SL82565_INT1_ELC, 0x00);

    /* Initialize the Interrupt Controller #2 */

    IBC_BYTE_OUT (PIC_port1 (PIC2_BASE_ADR),0x11);	/* ICW1 */
    IBC_BYTE_OUT (PIC_port2 (PIC2_BASE_ADR),sysVectorIRQ0+8); /* ICW2 */
    IBC_BYTE_OUT (PIC_port2 (PIC2_BASE_ADR),0x02);	/* ICW3 */
    IBC_BYTE_OUT (PIC_port2 (PIC2_BASE_ADR),0x01);	/* ICW4 */

    /* Mask interrupts IRQ 8-15 by writing to OCW1 register */

    IBC_BYTE_OUT (PIC_IMASK (PIC2_BASE_ADR),0xff);

    /* Make IRQ 14, 10, and 9 level sensitive */

    IBC_BYTE_OUT (SL82565_INT2_ELC, 0x40 | 0x04 | 0x02);

    /* Permanently turn off ISA refresh by never completing init steps */

    IBC_BYTE_OUT (SL82565_TMR1_CMOD, 0x74);

    /*	Perform the PCI Interrupt Ack cycle */

    IBC_BYTE_IN (PCI_MSTR_IACK_REG, &intVec);

    /* Perform the end of interrupt procedure */

    sysIbcEndOfInt (15);

    sysIbcIntLevelSet (16);

    return (OK);
    }

/*******************************************************************************
*
* sysIbcIntEnable - enable a IBC interrupt level
*
* This routine enables a specified IBC interrupt level.
*
* RETURNS: ISA interrupt number. 
*/

int sysIbcIntEnable
    (
    int intNum        /* interrupt level to enable */
    )
    {
    intNum -= ISA_INT_NUM_MIN;

    if (intNum < 8)
        {
        sysPicMask1 &= ~(1 << intNum);
        IBC_BYTE_OUT (PIC_IMASK (PIC1_BASE_ADR), sysPicMask1 | sysPicLevel1);
        }
    else
        {
        sysPicMask2 &= ~(1 << (intNum - 8));
        IBC_BYTE_OUT (PIC_IMASK (PIC2_BASE_ADR), sysPicMask2 | sysPicLevel2);
        }
    return (intNum);
    }

/*******************************************************************************
*
* sysIbcIntDisable - disable a IBC interrupt level
*
* This routine disables a specified IBC interrupt level.
*
* RETURNS: ISA interrupt number. 
*/

int sysIbcIntDisable
    (
    int intNum        /* interrupt level to disable */
    )
    {
    intNum -= ISA_INT_NUM_MIN;
 
    if (intNum < 8)
        {
        sysPicMask1 |= (1 << intNum);
        IBC_BYTE_OUT (PIC_IMASK (PIC1_BASE_ADR), sysPicMask1 | sysPicLevel1 );
        }
    else
        {
        sysPicMask2 |= (1 << (intNum - 8));
        IBC_BYTE_OUT (PIC_IMASK (PIC2_BASE_ADR), sysPicMask2 | sysPicLevel2);
        }
    return (intNum);
    }

/******************************************************************************
*
* sysIbcIntHandler - handler of the sl82565 IBC interrupt.
*
* This routine handles interrupts originating from the W83C553 PIB ISA Bus
* Controller (IBC).  This device implements the functional equivalent of two
* cascaded 8259 PICs.
*
* This routine is entered with CPU external interrupts enabled.
*
* Because the ISA bus is only accessible via the PCI bus, this driver first
* initiates a PCI interrupt acknowledge cycle to read the interrupt number
* (vector) coming from the IBC.
*
* This routine then processes the interrupt by calling all interrupt service
* routines chained to the vector.
*
* Finally, this routine re-arms the interrupt at the IBC by performing a
* IBC EOI.
*
* RETURNS: N/A
*/

void sysIbcIntHandler (void)
    {
    UCHAR		intNum;
    INT_HANDLER_DESC *	currHandler;
    static int		lvl7Int = 0;
    static int		lvl15Int = 0;

    IBC_BYTE_IN (PCI_MSTR_IACK_REG, &intNum);

    /* Special check for phantom interrupts */

    if ((intNum & IBC_INT_LVL7) == IBC_INT_LVL7)	
        {
        if (sysIbcPhantomInt (&intNum, lvl7Int, lvl15Int) == TRUE)
            return;		/* It's phantom so just return. */
        }

    /*
     * If cascade from IBC2 just EIO and return. It has been seen on 
     * certain PCI ISA bridge devices, that it is possible to see the
     * cascade IRQ2.  In particular, this has been observed on the 
     * Winbond W83C553F on certain revs of the chip prior to Rev G.
     */

    if (intNum == IBC_INT_LVL2) 
        {
        sysIbcEndOfInt (intNum);
        return;
        }

    /* Keep track of level 7 and 15 nesting for phantom interrupt handling */

    if (intNum == IBC_INT_LVL7)
        lvl7Int++;
    else if (intNum == IBC_INT_LVL15)
        lvl15Int++;

    if ((currHandler = sysIntTbl [intNum + ISA_INT_NUM_MIN]) == NULL)
        {
        logMsg ("uninitialized IBC interrupt level %x\r\n", intNum, 0,0,0,0,0);
        }
    else
        {
        /* Call EACH respective chained interrupt handler */

        while (currHandler != NULL)
            {
            currHandler->vec (currHandler->arg);
            currHandler = currHandler->next;
            }
        }

    /* Keep track of level 7 and 15 nesting for phantom interrupt handling */

    if (intNum == IBC_INT_LVL7)
        lvl7Int--;
    else if (intNum == IBC_INT_LVL15)
        lvl15Int--;

    /* Re-arm (enable) the interrupt on the IBC */

    sysIbcEndOfInt (intNum);
    }

/*******************************************************************************
*
* sysIbcPhantomInt - Determine if IRQ interrupt number 7 or 15 is "phantom".
* 
* This routine determines if an IRQ number of 7 or 15 is a phantom
* interrupt.  According to Intel 82C59A-2 documentation, the IR (interrupt
* request) inputs must remain high until after the falling edge of the first
* INTA (interrupt acknowledge).  If the IR goes low before this, a DEFAULT
* (phantom) IRQ7 will occur when the CPU acknowledges the interrupt.  Note
* that if an IRQ7 is generated it may really be another interrupt, IRQ4 for
* example.  IRQ 7 is associated  with the master 8259, IRQ 15 is associated 
* with the slave 8259.  This function should only be called if the 
* acknowledged IRQ number is 7 or 15 but does behave sanely if called with 
* other IRQ numbers.
*
* As mentioned above, IRQ 7 is supposed to be associated with the master
* 8259. However, it has been observed that a phantom IRQ 7 was caused by
* an interrupt on the slave 8259. Thus, the algorithm now implemented is
* to scan both the master and the slave 8259 in the correct priority order
* (from highest to lowest: 0 1 8 9 10 11 12 13 14 15 3 4 5 6 7).
*
* RETURNS: TRUE if phantom IRQ, *intNum unaltered.
* FALSE if no phantom interrupt, *intNum is "real" IRQ number.
*/

LOCAL UCHAR sysIbcPhantomInt
    (
    UCHAR *intNum,      /* interrupt number received on acknowledge */
    int	  lvl7Int,	/* interrupt 7 nesting level */
    int	  lvl15Int 	/* interrupt 15 nesting level */
    )
    {
    UCHAR irqBit;
    UINT  irqNum;

    /* Read the master in-service register (ISR) */

    IBC_BYTE_OUT (PIC_port1 (PIC1_BASE_ADR), PIC_OCW3_SEL + PIC_ISR_READ);

    IBC_BYTE_IN (PIC_port1 (PIC1_BASE_ADR), &irqBit);

    if (irqBit == 0)
        {
        logMsg ("sysIbcPhantomInt: no interrupt in master\n", 0,0,0,0,0,0);
        return (TRUE);	/* No in-service int so it MUST be phantom */
        }

    for (irqNum = 0; ((irqBit & 1) == 0) ; irqNum++, irqBit >>= 1)
        ;

    if (irqNum == IBC_INT_LVL7)
        if (lvl7Int > 1)
            {
            logMsg ("sysIbcPhantomInt: nested interrupt in master\n",
                    0,0,0,0,0,0);
            return (TRUE);  /* We're nested so it MUST be phantom */
            }

    /* if irqNum isn't 2 we have an interrupt on the master */

    if (irqNum != IBC_INT_LVL2)
        {
        *intNum = irqNum;
        return (FALSE);
        }

    /* Read the slave in-service register (ISR) */

    IBC_BYTE_OUT (PIC_port1 (PIC2_BASE_ADR), PIC_OCW3_SEL + PIC_ISR_READ);

    IBC_BYTE_IN (PIC_port1 (PIC2_BASE_ADR), &irqBit);

    if (irqBit == 0)	/* should never happen, really */
        {
        logMsg ("sysIbcPhantomInt: no interrupt in slave\n", 0,0,0,0,0,0);
        return (TRUE);	/* No in-service int so it MUST be phantom */
        }

    for (irqNum = 8; ((irqBit & 1) == 0) ; irqNum++, irqBit >>= 1)
        ;

    if (irqNum == 15)
        if (lvl15Int > 1)
            {
            logMsg ("sysIbcPhantomInt: nested interrupt in slave\n",
                    0,0,0,0,0,0);
            return (TRUE);  /* We're nested so it MUST be phantom */
            }

    *intNum = irqNum;
    return (FALSE);
    }

/*******************************************************************************
*
* sysIbcEndOfInt - send EOI (end of interrupt) signal.
*
* This routine is called at the end of the interrupt handler to
* send a non-specific end of interrupt (EOI) signal.
*
* The second PIC is acked only if the interrupt came from that PIC.
* The first PIC is always acked.
*/

LOCAL void sysIbcEndOfInt 
    (
    int intNum /* interrupt number to retire */
    )
    {
    if (intNum > IBC_INT_LVL7)
        {
        IBC_BYTE_OUT (PIC_IACK (PIC2_BASE_ADR), 0x20);
        }
    IBC_BYTE_OUT (PIC_IACK (PIC1_BASE_ADR), 0x20);
    }

/*******************************************************************************
*
* sysIbcIntLevelSet - set the interrupt priority level
*
* This routine masks interrupts with real priority equal to or lower than
* <intNum>.  The special
* value 16 indicates all interrupts are enabled. Individual interrupt
* numbers have to be specifically enabled by sysIbcIntEnable() before they
* are ever enabled by setting the interrupt level value.
*
* Note because of the IBM cascade scheme, the actual priority order for
* interrupt numbers is (high to low) 0, 1, 8, 9, 10, 11, 12, 13, 14, 15,
* 3, 4, 5, 6, 7, 16 (all enabled)
*
* INTERNAL: It is possible that we need to determine if we are raising
* or lowering our priority level.  It may be that the order of loading the
* two mask registers is dependent upon raising or lowering the priority.
*
* RETURNS: N/A
*/

LOCAL void sysIbcIntLevelSet
    (
    int intNum	/* interrupt level to implement */
    )
    {
    if (intNum > 16)
        intNum = 16;

    sysPicLevelCur = intNum;

    if (sysPicLevel2 != sysPicPriMask2[intNum])
        {
        sysPicLevel2 = sysPicPriMask2[intNum];
        IBC_BYTE_OUT (PIC_IMASK (PIC2_BASE_ADR), sysPicMask2 | sysPicLevel2);
        }

    if (sysPicLevel1 != sysPicPriMask1[intNum])
        {
        sysPicLevel1 = sysPicPriMask1[intNum];
        IBC_BYTE_OUT (PIC_IMASK (PIC1_BASE_ADR), sysPicMask1 | sysPicLevel1);
        }
    }

/*******************************************************************************
*
* sysIbcMpicConnect - routine to connect IBC interrupts to MPIC
*
* This function is called from sysHwInit2 and sets the IBC interrupt
* handler into the MPIC interrupt vector table.
*
* RETURNS: N/A
*/

LOCAL void sysIbcMpicConnect (void)
    {
    intConnect (INUM_TO_IVEC(PIB_INT_VEC), sysIbcIntHandler, 0); 
    intEnable (PIB_INT_LVL);
    }
