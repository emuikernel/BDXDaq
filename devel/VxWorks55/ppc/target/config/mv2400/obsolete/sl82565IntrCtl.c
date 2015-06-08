/* sl82565IntrCtl.c - Motorola sl82565 ISA Bridge Controller (IBC) driver */

/* Copyright 1984-1998 Wind River Systems, Inc. */
/* Copyright 1996,1997,1998 Motorola, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,15jul99,rhv  Incorporating WRS code review changes.
01a,15dec98,mdp  Written (from version 01k of mv2304/sl82565IntrCtl.c).
*/

/*
DESCRIPTION
This module implements the Motorola sl82565 ISA Bridge Controller (IBC)
driver.

 NOTE: The chip is now named the WinBond W83C553F Chip.
 All references to SL82565, should be considered as
 references to W83C553F chip instead.

The sl82565 Chip is a highly integrated ASIC providing PCI-ISA interface
chip.  It provides following major capabilities:

  PCI Bus Master capability for ISA DMA.
  PCI Arbiter capability
  PCI Power management control
  64 byte PCI bus FIFO for burst capability
  Standard ISA interrupt controllers (82C59)
  Standard ISA timer/counter (82C54)

This driver is limited to the interrupt controller feature of the chip.
This driver does not interact with any other drivers supporting the
sl82565 chip.

The chip implements a standard ISA bus interrupt system consisting of
two 82C59A interrupt controller units.  The two units are cascaded 
together to provide 15 actual interrupt lines.  The first unit implements
interrupt number 0 thru 7.  Interrupt number 2 is the cascaded input
from the second 82C59A controller, so it functionally does not exist
as a user input.  The inputs on the second controller are numbered 8 through
15.  Since they are cascaded into interrupt number 2 on the first unit,
their actual priority is higher than inputs 3 through 7 on the first
unit.  The true priority heirarchy (from high to low) is: 0, 1, 8, 9,
10, 11, 12, 13, 14, 15, 3, 4, 5, 6, 7.  The highest priority input that
is active is the unit that shall receive service first.

This driver implements a complete interrupt architecture system, complete
with vector table.
Based on the IBM-PC legacy system, this driver supports 16 levels, each
of which maps to a single vector.  Since PCI interrupt lines are shared, this
driver does provide for overloading of interrupt routines (i.e. there is
a list of interrupt routines for each interrupt vector (level)).  To service
a vector requires that all connected interrupt routines be called in order
of their connection.

This driver provides the vector table for the system.  It can support
a total of 256 vectors.  The interrupt controller device can only generate
16 different vectors though, one for each level.
The actual vector number corresponding to IRQ0
is determined by the value in sysVectorIRQ0 at initialization time.  The
other vector numbers are generated by adding the IRQ number to this vector
number.

If there are other devices in the system capable of generating their own
vectors then we presume that an appropriate interrupt handler is created
and attached to the vector associated with the correct IRQ number.  That
interrupt handler would get a new vector directly from the device and then
call all of the handlers attached to that new vector.  Vector information is
stored in a linked list of INT_HANDLER_DESC structures. The sysIntTbl array
contains a pointer to the first entry for each vector.

An example would be a VME interface chip.
If the VME chip interrupts the CPU on IRQ8, then the BSP should create
a special VME interrupt handler and attach it to vector # (sysVectorIRQ0 + 8).
When the handler is called, it should get the new vector from the VME
device.  It will then use that new vector # to locate the specific VME
device handler routine from sysIntTbl.  The VME interrupt handler will
call each handler connected to the devices vector.  Note that the user
must insure that no VME device uses a vector that matches the vector used
by the VME interface chip itself.  This could cause an infinite loop to
be generated.

.CS
/@ This is the sample VME interrupt handler (IRQ8) @/

VOID sysVmeHandler (void)
    {
    INT_HANDLER_DEC * pVector;
    int newVec;

    newVec = ????; /@ get real vector from device @/

    /@ if no VME interrupt is present, exit immediately @/

    if (newVec < 0 || newVec > 255)
	return;

    /@ process all connected routines @/

    pVector = sysIntTbl[newVec];
    while (pVector != NULL)
        {
	(*pVector->vec) (pVector->arg);
        pVector = pVector->next;
        }
    }

/@ The BSP would connect the VME handler to the vector for IRQ 8 @/

    #define SYS_VME_VEC  (sysVectorIRQ0 + 8) /@ IRQ8 @/

    ...
    intConnect (INUM_TO_IVEC(SYS_VME_VEC), sysVmeHandler, 0);
    ...
.CE

INTERNAL
** HELP **
The sysPciExtIbcInit function does not belong in this driver module.
The excIntConnect function in sysIbcInit does not belong in this module.
These should be removed and the change in initialization sequence should
be updated.

.SH INITIALIZATION

This driver is initialized from the BSP, usually as part of sysHwInit().
The first routine to be called is sysPciExtIbcInit(). The arguments
to this routine are the bus, device, and function numbers that locate
the chip in the PCI configuration space.  The routine verifies the type
of device and then initializes the interrupt pins routine to IRQ numbers.

The second routine to be called is sysIbcInit().  This routine takes no
arguments. This routine allocates the vector table and initializes the
chips to a default state.  All individual interrupt sources are disabled.
Each has to be individually enabled by intEnable() before it will be
unmasked and allowed to generate an interrupt.

Typical device initialization looks like this:

.CS
  /@ Initialize the extended portion of the IBC's PCI Header.  @/

  int pciBusNum;
  int pciDevNum;
  int pciFuncNum;

  if (pciFindDevice ((PCI_ID_IBC & 0xFFFF), (PCI_ID_IBC >> 16) & 0xFFFF, 0,
                       &pciBusNum, &pciDevNum, &pciFuncNum) != ERROR)
      {
      sysPciExtIbcInit (pciBusNum, pciDevNum, pciFuncNum);
      sysIbcInit ();
      }
.CE

.SH CUSTOMIZING THIS DRIVER

The macros IBC_BYTE_OUT and IBC_BYTE_IN provide the hardware access methods.
By default they call the routines sysOutByte() and sysInByte(), which are
presumed to be defined by the BSP.  The user may redefine these macros
as needed.

The macros CPU_INT_LOCK() and CPU_INT_UNLOCK provide the access
to the CPU level interrupt lock/unlock routines.  We presume that there
is a single interrupt line to the CPU.  By default these macros call
intLock() and intUnlock() respectively.
*/

#include "vxWorks.h"
#include "config.h"

#include "stdlib.h"
#include "sysLib.h"
#include "intLib.h"
#include "logLib.h"
#include "drv/pci/pciConfigLib.h"
#include "sl82565IntrCtl.h"
#include "arch/ppc/excPpcLib.h"
#include "private/eventP.h"

/* globals */

IMPORT STATUS	      (*_func_intConnectRtn) (VOIDFUNCPTR *, VOIDFUNCPTR, int);
IMPORT int	      (*_func_intEnableRtn) (int);
IMPORT int	      (*_func_intDisableRtn) (int);
IMPORT void		sysOutByte (ULONG, UCHAR);
IMPORT UCHAR		sysInByte (ULONG);
IMPORT STATUS		excIntConnect (VOIDFUNCPTR *, VOIDFUNCPTR);
void			sysIbcIntHandler (void);

IMPORT UINT		sysVectorIRQ0;		/* vector for IRQ0 */
INT_HANDLER_DESC * 	sysIntTbl [256];	/* system interrupt table */

/* forward declarations */

LOCAL void	sysIbcEndOfInt (int intNum);
LOCAL STATUS	sysIbcIntConnect (VOIDFUNCPTR * vector, VOIDFUNCPTR routine,
				int parameter);
LOCAL int	sysIbcIntEnable (int);
LOCAL int	sysIbcIntDisable (int);
LOCAL void	sysIbcIntLevelSet (int);

/* Mask values are the currently disabled sources */

LOCAL UINT8	sysPicMask1 = 0xfb;	/* all levels disabled */
LOCAL UINT8	sysPicMask2 = 0xff;

/* Level values are the interrupt level masks */

LOCAL UINT8	sysPicLevel1 = 0;
LOCAL UINT8	sysPicLevel2 = 0;
LOCAL UINT8	sysPicLevelCur = 16;	/* curr intNum is 15, all enabled */

/* level values by real priority */

LOCAL UCHAR sysPicPriMask1[17] = {0xFB,0xFA,0xF8,0xF8,0xF0,0xE0,0xC0,0x80,
			     0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0x0};
LOCAL UCHAR sysPicPriMask2[17] = {0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
			     0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x0};


/* Hardware access methods */

#ifndef IBC_BYTE_OUT
#   define IBC_BYTE_OUT(reg,data) \
	(sysOutByte (reg,data))
#endif

#ifndef IBC_BYTE_IN
#   define IBC_BYTE_IN(reg,pData) \
	(*pData = sysInByte(reg))
#endif

#ifndef CPU_INT_LOCK
#   define CPU_INT_LOCK(pData) \
	(*pData = intLock ())
#endif

#ifndef CPU_INT_UNLOCK
#   define CPU_INT_UNLOCK(data) \
	(intUnlock (data))
#endif

/*******************************************************************************
*
* sysPciExtIbcInit - initialize the extended portion of the IBC PCI header
*
* This routine initializes the extended portion of the ISA Bridge Controller
* (IBC) PCI header.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: sysPciExtHawkInit()
*/

STATUS sysPciExtIbcInit
    (
    int         pciBusNo,               /* PCI bus number */
    int         pciDevNo,               /* PCI device number */
    int         pciFuncNo               /* PCI function number */
    )
    {
    UINT16	tmp16;

    /* Configuring Winbond ? */

    pciConfigInWord (pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_VENDOR_ID, &tmp16);
    if (tmp16 != 0x10ad)
	{
	/* No! */

	return (ERROR);
	}

    /*
     * route PCI interrupts to IBC IRQs
     *
     * PCI IRQ0 routed to IBC IRQ10
     * PCI IRQ1 routed to IBC IRQ11
     * PCI IRQ2 routed to IBC IRQ14
     * PCI IRQ3 routed to IBC IRQ15
     */

    pciConfigOutWord (pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_IBC_INTR_ROUTE,
                      0xabef);

    return (OK);
    }


/*******************************************************************************
*
* sysIbcInit - initialize the non-PCI header configuration registers of the IBC
*
* This routine initializes the non-PCI header configuration registers of the
* Motorola sl82565 ISA Bridge Controller.
*
* RETURNS: OK, always.
*/

STATUS sysIbcInit (void)
    {
    UINT	vector;
    UCHAR	intVec;

    /* Initialize the interrupt table */

    for (vector = 0; vector < 256; vector++)
	sysIntTbl[vector] = NULL;

    /* Connect the interrupt demultiplexer to the PowerPC external interrupt */

    excIntConnect ((VOIDFUNCPTR *) _EXC_OFF_INTR, sysIbcIntHandler);

    /* Set up the BSP specific routines */

    _func_intConnectRtn = sysIbcIntConnect;
    _func_intEnableRtn = sysIbcIntEnable;
    _func_intDisableRtn = sysIbcIntDisable;

    /* Initialize the Interrupt Controller #1 */

    IBC_BYTE_OUT (PIC_port1 (PIC1_BASE_ADR),0x11);	/* ICW1 */
    IBC_BYTE_OUT (PIC_port2 (PIC1_BASE_ADR),sysVectorIRQ0); /* ICW2 */
    IBC_BYTE_OUT (PIC_port2 (PIC1_BASE_ADR),0x04);	/* ICW3 */
    IBC_BYTE_OUT (PIC_port2 (PIC1_BASE_ADR),0x01);	/* ICW4 */

    /*
     *	Mask interrupts IRQ 0, 1, and 3-7 by writing to OCW1 register
     *	IRQ 2 is the cascade input
     */

    IBC_BYTE_OUT (PIC_IMASK (PIC1_BASE_ADR),0xfb);

    /* Make IRQ 5 level sensitive */

    IBC_BYTE_OUT (SL82565_INT1_ELC, 0x20);

    /* Initialize the Interrupt Controller #2 */

    IBC_BYTE_OUT (PIC_port1 (PIC2_BASE_ADR),0x11);	/* ICW1 */
    IBC_BYTE_OUT (PIC_port2 (PIC2_BASE_ADR),sysVectorIRQ0+8); /* ICW2 */
    IBC_BYTE_OUT (PIC_port2 (PIC2_BASE_ADR),0x02);	/* ICW3 */
    IBC_BYTE_OUT (PIC_port2 (PIC2_BASE_ADR),0x01);	/* ICW4 */

    /* Mask interrupts IRQ 8-15 by writing to OCW1 register */

    IBC_BYTE_OUT (PIC_IMASK (PIC2_BASE_ADR),0xff);

    /* Make IRQ 15, 14, 11, 10, and 9 level sensitive */

    IBC_BYTE_OUT (SL82565_INT2_ELC, 0x80 | 0x40 | 0x08 | 0x04 | 0x02);

    /* Permanently turn off ISA refresh by never completing init steps */

    IBC_BYTE_OUT (SL82565_TMR1_CMOD, 0x74);

    /*	Perform the PCI Interrupt Ack cycle */

    IBC_BYTE_IN (HAWK_PHB_BASE_ADRS + 0x30, &intVec);

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
* RETURNS: OK.
*
* ARGSUSED0
*/

LOCAL int sysIbcIntEnable
    (
    int intNum        /* interrupt level to enable */
    )
    {

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

    return (OK);
    }

/*******************************************************************************
*
* sysIbcIntDisable - disable a IBC interrupt level
*
* This routine disables a specified IBC interrupt level.
*
* RETURNS: OK.
*
* ARGSUSED0
*/

LOCAL int sysIbcIntDisable
    (
    int intNum        /* interrupt level to disable */
    )
    {

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

    return (OK);
    }

/******************************************************************************
*
* sysIbcIntConnect - connect an interrupt handler to the system vector table
*
* This function connects an interrupt handler to the system vector table.
*
* RETURNS: OK or ERROR.
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

    /* install the handler in the system interrupt table */

    if (sysIntTbl[(int) vector] == NULL)
	sysIntTbl [(int ) vector] = newHandler; /* single int. handler case */
    else
	{
	currHandler = sysIntTbl[(int) vector]; /* multiple int. handler case */
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
* sysIbcIntHandler - sl82565 IBC interrupt handler
*
* This routine is called to service interrupts generated by the sl82565 ISA
* Bridge Controller (IBC).
*
* RETURNS: N/A
*/

void sysIbcIntHandler (void)
    {
    UCHAR		intNum;
    INT_HANDLER_DESC *	currHandler;
    int			dontCare;
    int			oldLevel;

    /* Perform the PCI Interrupt Ack cycle */

    IBC_BYTE_IN (HAWK_PHB_BASE_ADRS + 0x30, &intNum);

#ifdef INCLUDE_WINDVIEW
    WV_EVT_INT_ENT(intNum)
#endif

    /* Save current level, reset pic masks to the new interrupt level */

    oldLevel = sysPicLevelCur;
    sysIbcIntLevelSet (intNum);

    /* Re-arm (enable) the interrupt chip */

    sysIbcEndOfInt (intNum);

    /* Allow external interrupts to the CPU. */

    CPU_INT_UNLOCK (_PPC_MSR_EE);

    if ((currHandler = sysIntTbl [intNum]) == NULL)
	{
	logMsg ("uninitialized interrupt level %d\n", intNum, 0,0,0,0,0);
	}
    else
	{
	/* Call EACH respective interrupt handler */

	while (currHandler != NULL)
	    {
	    currHandler->vec (currHandler->arg);
	    currHandler = currHandler->next;
	    }
	}

    /*
     *	Disable External Interrupts
     *	External Interrupts will be re-enabled in the kernel's wrapper
     *  of this Interrupt.
     */

    CPU_INT_LOCK (&dontCare);

    /* Restore original interrupt level */

    sysIbcIntLevelSet (oldLevel);
    }

/*******************************************************************************
*
* sysIbcEndOfInt - send EOI(end of interrupt) signal.
*
* This routine is called at the end of the interrupt handler to
* send a non-specific end of interrupt (EOI) signal.
*
* The second PIC is acked only if the interrupt came from that PIC.
* The first PIC is always acked.
*/

LOCAL void sysIbcEndOfInt 
    (
    int intNum
    )
    {
    if (intNum > 7)
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

void sysIbcIntLevelSet
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