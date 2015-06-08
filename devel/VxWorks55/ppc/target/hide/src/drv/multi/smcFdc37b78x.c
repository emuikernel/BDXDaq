/* smcFdc37b78x.c - a superIO (fdc37b78x) initialization source module */

/* Copyright 1984-2000 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,28mar00,ms  written.
*/

/*
DESCRIPTION
The FDC37B78x with advanced Consumer IR and IrDA v1.0 support incorporates a
keyboard interface, real-time clock, SMSC's true CMOS 765B floppy disk
controller, advanced digital data separator, 16 byte data FIFO, two 16C550
compatible UARTs, one Multi-Mode parallel port which includes ChiProtect
circuitry plus EPP and ECP support, on-chip 12 mA AT bus drivers, and two
floppy direct drive support, soft power management and SMI support and
Intelligent Power Management including PME and SCI/ACPI support. The true
CMOS 765B core provides 100% compatibility with IBM PC/XT and PC/AT
architectures in addition to providing data overflow and underflow protection.
The SMSC advanced digital data separator incorporates SMSC's patented data
separator technology, allowing for ease of testing and use. Both on-chip UARTs
are compatible with the NS16C550. The parallel port, the IDE interface, and
the game port select logic are compatible with IBM PC/AT architecture,
as well as EPP and ECP.

The FDC37B78x incorporates sophisticated power control circuitry (PCC) which
includes support for keyboard, mouse, modem ring, power button support and
consumer infrared wake-up events. The PCC supports multiple low power down
modes.

The FDC37B78x provides features for compliance with the "Advanced Configuration
and Power Interface Specification" (ACPI). These features include support of
both legacy and ACPI power management models through the selection of SMI or
SCI. It implements a power button override event (4 second button hold to turn
off the system) and either edge triggered interrupts.

The FDC37B78x provides support for the ISA Plug-and-Play Standard (Version 1.0a)
and provides for the recommended functionality to support Windows95, PC97 and
PC98. Through internal configuration registers, each of the FDC37B78x's
logical device's I/O address, DMA channel and IRQ channel may be programmed.
There are 480 I/O address location options, 12 IRQ options or Serial IRQ option,
and four DMA channel options for each logical device.

.I USAGE
This library provides routines to intialize various logical devices on superIO
chip (fdc37b78x).

The functions addressed here include:

.IP "   -"
Creating a logical device and initializing internal database accordingly.

.IP "   -"
Enabling as many device as permitted by this facility by single call. The
user of thie facility can selectively intialize a set of devices on superIO
chip.

.IP "   -"
Intializing keyboard by sending commands to its controller embedded in superIO
chip.

.LP

.I INTERNAL DATABASES
This library provides it's user to changes superIO's config, index, and data
I/O port addresses. The default I/O port addresses are defined in
target/h/drv/smcFdc37b78x.h file. These mnemonics can be overridden by defining
in architecture related BSP header file. These default setting can also be
changed on-the-fly by passing in a pointer of type SMCFDC37B78X_IOPORTS with
different I/O port addresses. If not redefined, they take their default values
as defined in smcFdc37b78x.h file.

.IP "SMCFDC37B78X_CONFIG_PORT"
Defines the config I/O port for SMC-FDC37B78X superIO chip.

.IP "SMCFDC37B78X_INDEX_PORT"
Defines the index I/O port for SMC-FDC37B78X superIO chip.

.IP "SMCFDC37B78X_DATA_PORT"
Defines the data I/O port for SMC-FDC37B78X superIO chip.

.LP

.I USER INTERFACE

.CS
VOID smcFdc37b78xDevCreate
    (
    SMCFDC37B78X_IOPORTS *smcFdc37b78x_iop
    )
.CE

This is a very first routine that should be called by the user of this library.
This routine sets up IO port address that will subsequentally be used later on.
The IO PORT setting could either be overridden by redefining
SMCFDC37B78X_CONFIG_PORT, SMCFDC37B78X_INDEX_PORT and SMCFDC37B78X_DATA_PORT
or on-the-fly by passing in a pointer of type SMCFDC37B78X_IOPORTS.

.CS
VOID smcFdc37b78xInit
    (
    int devInitMask
    )
.CE

This is routine intakes device intialization mask and intializes only those
devices that are requested by user. Device initialization mask holds bitwise
ORed values of all devices that are requested by user to enable on superIO
device.

The mnemonics that are supported in current version of this facility are:

.IP "SMCFDC37B78X_COM1_EN"
Use this mnemonic to enable COM1 only.

.IP "SMCFDC37B78X_COM2_EN"
Use this mnemonic to enable COM2 only.

.IP "SMCFDC37B78X_LPT1_EN"
Use this mnemonic to enable LPT1 only.

.IP "SMCFDC37B78X_KBD_EN"
Use this mnemonic to enable KBD only.

.IP "SMCFDC37B78X_FDD_EN"
Use this mnemonic to enable FDD only.

.LP
The above mentioned can be bitwise ORed to enable more than one device at
a time. e.g. if you want COM1 and COM2 to be enable on superIO chip call:

.CS
smcFdc37b78xInit (SMCFDC37B78X_COM1_EN | SMCFDC37B78X_COM2_EN);
.CE

The prerequisites for above mentioned call, superIO chip library should be
intialized using smcFdc37b78xDevCreate() with parameter as per user's need.

.CS
STATUS smcFdc37b78xKbdInit
    (
    VOID
    )
.CE

This routine sends some keyboard commands to keyboard controller embedded
in superIO chip. Call to this function is required for proper functioning
of keyboard driver.

INCLUDE FILES: smcFdc37b78x.h
*/

/* includes */
#include "vxWorks.h"
#include "sysLib.h"
#include "string.h"
#include "drv/multi/smcFdc37b78x.h"


/* globals */
LOCAL SMCFDC37B78X_IOPORTS smcFdc37b78xIoPorts;
LOCAL SMCFDC37B78X_IOPORTS * smcFdc37b78xIopPtr;

/*******************************************************************************
*
* smcFdc37b78xModReg - modifies I/O port contents
*
* This routine will modify contents of a given I/O port
*
* RETURNS: NONE
*/

LOCAL VOID smcFdc37b78xModReg
    (
    int ioPort,
    unsigned char value,
    unsigned char mask
    )
    {
    unsigned char c;
    
    c = SMCFDC37B78XRD (ioPort);
    c &= ~mask;
    c |= value;
    SMCFDC37B78XWRT (ioPort, c);
    }

/*******************************************************************************
*
* smcFdc37b78xEnterCfg - enter into super i/o configuration cycle
*
* This routine will begin super i/o configuration mode
*
* RETURNS: NONE
*/

LOCAL VOID smcFdc37b78xEnterCfg
    (
    VOID
    )
    {
    SMCFDC37B78XWRT (smcFdc37b78xIopPtr->config, SMCFDC37B78X_CONFIG_START);
    }

/*******************************************************************************
*
* smcFdc37b78xExitCfg - Exits super i/o configuration cycle
*
* This routine will exit from super i/o configuration mode
*
* RETURNS: NONE
*/

LOCAL VOID smcFdc37b78xExitCfg
    (
    VOID
    )
    {
    SMCFDC37B78XWRT (smcFdc37b78xIopPtr->config, SMCFDC37B78X_CONFIG_END);
    }


/*******************************************************************************
*
* smcFdc37b78xCfgDevReg - modifies contents of a register 
*
* This routine will modify register content of a given logical device
*
* RETURNS: NONE
*/

LOCAL VOID smcFdc37b78xCfgDevReg
    (
    unsigned char logDevice,
    unsigned char regIndex,
    unsigned char regValue,
    unsigned char mask
    )
    {
    unsigned char c;
    
    /* select the logical device register */
    SMCFDC37B78XWRT (smcFdc37b78xIopPtr->config, SMCFDC37B7X_LOGDEVCFGREG);
    
    /* select the logical device */
    SMCFDC37B78XWRT (smcFdc37b78xIopPtr->data, logDevice);
    
    /* select the index */
    SMCFDC37B78XWRT (smcFdc37b78xIopPtr->index, regIndex);
    
    c = SMCFDC37B78XRD (smcFdc37b78xIopPtr->data);
    c &= ~mask;
    c |= regValue;
    SMCFDC37B78XWRT (smcFdc37b78xIopPtr->data, c);
    }


/*******************************************************************************
*
* smcFdc37b78xCfgDev - configures complete device
*
* This routine will activate logical device, sets IO register
* addresses and set up an interrupt for given logical device
*
* RETURNS: NONE
*/

LOCAL VOID smcFdc37b78xCfgDev
    (
    unsigned char device,
    unsigned char ioBaseHigh,
    unsigned char ioBaseLow,
    unsigned char intr
    )
    {
    /* activate given device */
    smcFdc37b78xCfgDevReg (device, SMCFDC37B78X_LOGDEV_ACTIVATE,
                           SMCFDC37B78X_ACT_DEV, SMCFDC37B78X_MASK_ALL);
    /* set HIGH BYTE of IO base address */
    smcFdc37b78xCfgDevReg (device, SMCFDC37B78X_IOBASE_HIGH,
                           ioBaseHigh, SMCFDC37B78X_MASK_ALL);
    /* set LOW BYTE of IO base address */    
    smcFdc37b78xCfgDevReg (device, SMCFDC37B78X_IOBASE_LOW,
                           ioBaseLow, SMCFDC37B78X_MASK_ALL);
    /* set interrupt # for given device */    
    smcFdc37b78xCfgDevReg (device, SMCFDC37B78X_INTERRUPT, intr,
                           SMCFDC37B78X_MASK_ALL); 
    }


/*******************************************************************************
*
* smcFdc37b78xKbdRdy - indicates if kbd controller is ready to accept
*                   read or write operation
*
* This routine checks if it's a good time to read or write to
* keyboard I/O ports
*
* RETURNS: OK/ERROR
*/

LOCAL STATUS smcFdc37b78xKbdRdy
    (
    int oper
    )
    {
    int retry;
    
    for (retry = 0; retry < 100; retry++)
        {
        if (SMCFDC37B78X_KBD_RD_CTLSTS() & oper)
            return (OK);
        }
    
    return (ERROR);
    }


/*******************************************************************************
*
* smcFdc37b78xEnbKbd - enables keyboard on superIO
*
* This routine will enable everything required for keyboard device
*
* RETURNS: OK/ERROR
*/

LOCAL STATUS smcFdc37b78xEnbKbd
    (
    VOID
    )
    {
    smcFdc37b78xEnterCfg ();

    /* activate keyboard device */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_KBD, SMCFDC37B78X_LOGDEV_ACTIVATE,
                           SMCFDC37B78X_ACT_DEV, SMCFDC37B78X_MASK_ALL);
    /* assign an interrupt # to keyboard device */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_KBD, SMCFDC37B78X_INTERRUPT,
                           SMCFDC37B78X_INTR_KBD, SMCFDC37B78X_MASK_ALL);

    /* activate Auxiliary device */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_LOGDEV_ACTIVATE,
                           SMCFDC37B78X_ACT_DEV, SMCFDC37B78X_MASK_ALL);
    
    /* Keyboard IRQ */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_GP60_INDEX,
                           SMCFDC37B78X_GP60_IRQ1_OUT,
                           SMCFDC37B78X_GP_FUNC_MASK);
    
    /* configure auxiliary device to support parallel interrupts */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_IRQMUX_IDX,
                           SMCFDC37B78X_IRQ_PARALLEL,
                           SMCFDC37B78X_IRQ_PARALLEL_MASK);
    
    smcFdc37b78xExitCfg ();
    return (OK);
    }


/*******************************************************************************
*
* smcFdc37b78xEnbCom1 - enables COM1 on superIO
*
* This routine will enable everything required for COM1 device
*
* RETURNS: OK/ERROR
*/

LOCAL STATUS smcFdc37b78xEnbCom1
    (
    VOID
    )
    {
    smcFdc37b78xEnterCfg ();

    /* configure and activate serial device 1 */
    smcFdc37b78xCfgDev (SMCFDC37B78X_COM1, SMCFDC37B78X_COM1_IOHIGH,
                        SMCFDC37B78X_COM1_IOLOW, SMCFDC37B78X_INTR_COM1);

    /* enable high speed services on serial device 1 */
    smcFdc37b78xModReg (SMCFDC37B78X_COM1_MCR, SMCFDC37B78X_COM_MCROUT2,
                        SMCFDC37B78X_COM_MCROUT2_MASK); 

    /* activate Auxiliary device */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_LOGDEV_ACTIVATE,
                           SMCFDC37B78X_ACT_DEV, SMCFDC37B78X_MASK_ALL);

    /* Serial COM1 IRQ */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_GP62_INDEX,
                           SMCFDC37B78X_GP62_IRQ4_OUT,
                           SMCFDC37B78X_GP_FUNC_MASK);
    
    /* configure auxiliary device to support parallel interrupts */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_IRQMUX_IDX,
                           SMCFDC37B78X_IRQ_PARALLEL,
                           SMCFDC37B78X_IRQ_PARALLEL_MASK);
    
    smcFdc37b78xExitCfg ();
    return (OK);
    }


/*******************************************************************************
*
* smcFdc37b78xEnbCom2 - enables COM2 on superIO
*
* This routine will enable everything required for COM2 device
*
* RETURNS: OK/ERROR
*/

LOCAL STATUS smcFdc37b78xEnbCom2
    (
    VOID
    )
    {
    smcFdc37b78xEnterCfg ();

    /* configure and activate serial device 2 */
    smcFdc37b78xCfgDev (SMCFDC37B78X_COM2, SMCFDC37B78X_COM2_IOHIGH,
                        SMCFDC37B78X_COM2_IOLOW, SMCFDC37B78X_INTR_COM2);

    /* enable high speed services on serial device 2 */    
    smcFdc37b78xModReg (SMCFDC37B78X_COM2_MCR, SMCFDC37B78X_COM_MCROUT2,
                        SMCFDC37B78X_COM_MCROUT2_MASK);

    /* activate Auxiliary device */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_LOGDEV_ACTIVATE,
                           SMCFDC37B78X_ACT_DEV, SMCFDC37B78X_MASK_ALL);

    /* Serial COM2 IRQ */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_GP61_INDEX,
                           SMCFDC37B78X_GP61_IRQ3_OUT,
                           SMCFDC37B78X_GP_FUNC_MASK);
    
    /* configure auxiliary device to support parallel interrupts */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_IRQMUX_IDX,
                           SMCFDC37B78X_IRQ_PARALLEL,
                           SMCFDC37B78X_IRQ_PARALLEL_MASK);
    
    smcFdc37b78xExitCfg ();
    return (OK);
    }


/*******************************************************************************
*
* smcFdc37b78xEnbLpt1 - enables LPT1 on superIO
*
* This routine will enable everything required for LPT1 device
*
* RETURNS: OK/ERROR
*/

LOCAL STATUS smcFdc37b78xEnbLpt1
    (
    VOID
    )
    {
    smcFdc37b78xEnterCfg ();

    /* configure and activate parallel device */
    smcFdc37b78xCfgDev (SMCFDC37B78X_LPT1, SMCFDC37B78X_LPT1_IOHIGH,
                        SMCFDC37B78X_LPT1_IOLOW, SMCFDC37B78X_INTR_LPT1);

    /* activate Auxiliary device */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_LOGDEV_ACTIVATE,
                           SMCFDC37B78X_ACT_DEV, SMCFDC37B78X_MASK_ALL);

    /* Parallel Port IRQ */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_GP65_INDEX,
                           SMCFDC37B78X_GP65_IRQ7_OUT,
                           SMCFDC37B78X_GP_FUNC_MASK); 

    
    /* configure auxiliary device to support parallel interrupts */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_IRQMUX_IDX,
                           SMCFDC37B78X_IRQ_PARALLEL,
                           SMCFDC37B78X_IRQ_PARALLEL_MASK);

    smcFdc37b78xExitCfg ();
    return (OK);
    }


/*******************************************************************************
*
* smcFdc37b78xEnbFdd - enables FDD on superIO
*
* This routine will enable everything required for FDD device
*
* RETURNS: OK/ERROR
*/

LOCAL STATUS smcFdc37b78xEnbFdd
    (
    VOID
    )
    {
    smcFdc37b78xEnterCfg ();

    /* activate floppy device */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_FDD, SMCFDC37B78X_LOGDEV_ACTIVATE,
                           SMCFDC37B78X_ACT_DEV, SMCFDC37B78X_MASK_ALL);

    /* activate Auxiliary device */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_LOGDEV_ACTIVATE,
                           SMCFDC37B78X_ACT_DEV, SMCFDC37B78X_MASK_ALL);

    /* Floppy Disk Drive IRQ */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_GP64_INDEX,
                           SMCFDC37B78X_GP64_IRQ6_OUT,
                           SMCFDC37B78X_GP_FUNC_MASK);

    /* configure auxiliary device to support parallel interrupts */
    smcFdc37b78xCfgDevReg (SMCFDC37B78X_AUX, SMCFDC37B78X_IRQMUX_IDX,
                           SMCFDC37B78X_IRQ_PARALLEL,
                           SMCFDC37B78X_IRQ_PARALLEL_MASK);
    
    smcFdc37b78xExitCfg ();
    return (OK);
    }

/*******************************************************************************
*
* smcFdc37b78xDevCreate - set correct IO port addresses for Super I/O chip
*
* This routine will initialize smcFdc37b78xIoPorts data structure. These ioports
* can either be changed on-the-fly or overriding SMCFDC37B78X_CONFIG_PORT, 
* SMCFDC37B78X_INDEX_PORT and SMCFDC37B78X_DATA_PORT. This is a necassary step
* in intialization of superIO chip and logical devices embedded in it.
*
* RETURNS: NONE
*/

VOID smcFdc37b78xDevCreate
    (
    SMCFDC37B78X_IOPORTS *smcFdc37b78x_iop
    )
    {

    if (smcFdc37b78x_iop == NULL)
        {
        smcFdc37b78xIopPtr = &smcFdc37b78xIoPorts;
        smcFdc37b78xIopPtr->config = SMCFDC37B78X_CONFIG_PORT;
        smcFdc37b78xIopPtr->index  = SMCFDC37B78X_INDEX_PORT;
        smcFdc37b78xIopPtr->data   = SMCFDC37B78X_DATA_PORT;
        }
    else
        {
        memcpy (&smcFdc37b78xIoPorts, smcFdc37b78x_iop,
                sizeof (SMCFDC37B78X_IOPORTS));
        }
    }

/*******************************************************************************
*
* smcFdc37b78xInit - initializes Super I/O chip Library
*
* This routine will initialize serial, keyboard, floppy disk,
* parallel port and gpio pins as a part super i/o intialization
*
* RETURNS: NONE
*/

VOID smcFdc37b78xInit
    (
    int devInitMask
    )
    {
    if (devInitMask & SMCFDC37B78X_FDD_EN)
        {
        smcFdc37b78xEnbFdd ();
        }

    if (devInitMask & SMCFDC37B78X_KBD_EN)
        {
        smcFdc37b78xEnbKbd ();
        }

    if (devInitMask & SMCFDC37B78X_COM1_EN)
        {
        smcFdc37b78xEnbCom1 ();
        }

    if (devInitMask & SMCFDC37B78X_COM2_EN)
        {
        smcFdc37b78xEnbCom2 ();
        }

    if (devInitMask & SMCFDC37B78X_LPT1_EN)
        {
        smcFdc37b78xEnbLpt1 ();
        }
    }

/*******************************************************************************
*
* smcFdc37b78xKbdInit - initializes the keyboard controller
*
* This routine will initialize keyboard controller
*
* RETURNS: OK/ERROR
*/

STATUS smcFdc37b78xKbdInit
    (
    VOID
    )
    {
    /* Self Test */
    SMCFDC37B78X_KBD_WRT_CTLSTS(SMCFDC37B78X_KBD_CCMD_SELFTEST);   
    if (smcFdc37b78xKbdRdy (SMCFDC37B78X_KBD_CTLSTS_OBF) != OK)
        return (ERROR);
    /* Self Test PASSED ? */
    if (SMCFDC37B78X_KBD_RD_DATA() != SMCFDC37B78X_KBD_SELFTEST_PASS) 
        return (ERROR);

    /* Interface Test */
    SMCFDC37B78X_KBD_WRT_CTLSTS(SMCFDC37B78X_KBD_CCMD_IFCTEST); 
    if (smcFdc37b78xKbdRdy (SMCFDC37B78X_KBD_CTLSTS_OBF) != OK)
        return (ERROR);
    /* Interface Test PASSED ? */
    if (SMCFDC37B78X_KBD_RD_DATA() != SMCFDC37B78X_KBD_IFCTEST_PASS)
        return (ERROR);
    
    /* Write to output port 2 */
    SMCFDC37B78X_KBD_WRT_CTLSTS(SMCFDC37B78X_KBD_CCMD_WRTPORT2);
    if (smcFdc37b78xKbdRdy (SMCFDC37B78X_KBD_CTLSTS_IBF) == OK)
        return (ERROR);
    /* P24 ENABLED */
    SMCFDC37B78X_KBD_WRT_DATA(SMCFDC37B78X_KBD_IRQ_KSC_EN);

    return (OK);
    }




















