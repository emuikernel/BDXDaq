/* iPiix4Pci.c - low level initalization code for PCI ISA/IDE Xcelerator */

/* Copyright 1984-2000 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,28mar00,ms  written.
*/

/*
DESCRIPTION
The 82371AB PCI ISA IDE Xcelerator (PIIX4) is a multi-function PCI device
implementing a PCI-to-ISA bridge function, a PCI IDE function, a Universal
Serial Bus host/hub function, and an Enhanced Power Management function. As
a PCI-to-ISA bridge, PIIX4 integrates many common I/O functions found in
ISA-based PC systems-two 82C37 DMA Controllers, two 82C59 Interrupt Controllers,
an 82C54 Timer/Counter, and a Real Time Clock. In addition to compatible
transfers, each DMA channel supports Type F transfers. PIIX4 also contains full
support for both PC/PCI and Distributed DMA protocols implementing PCI-based
DMA. The Interrupt Controller has Edge or Level sensitive programmable inputs
and fully supports the use of an external I/O Advanced Programmable Interrupt
Controller (APIC) and Serial Interrupts. Chip select decoding is provided for
BIOS, Real Time Clock, Keyboard Controller, second external microcontroller,
as well as two Programmable Chip Selects.

PIIX4 is a multi-function PCI device that integrates many system-level
functions. PIIX4 is compatible with the PCI Rev 2.1 specification, as well as
the IEEE 996 specification for the ISA (AT) bus.

.IP "PCI to ISA/EIO Bridge"
PIIX4 can be configured for a full ISA bus or a subset of the ISA bus called the
Extended IO (EIO) bus. The use of the EIO bus allows unused signals to be
configured as general purpose inputs and outputs. PIIX4 can directly drive up to
five ISA slots without external data or address buffering. It also provides
byte-swap logic, I/O recovery support, wait-state generation, and SYSCLK
generation. X-Bus chip selects are provided for Keyboard Controller, BIOS,
Real Time Clock, a second microcontroller, as well as two programmable chip
selects. PIIX4 can be configured as either a subtractive decode PCI to ISA
bridge or as a positive decode bridge. This gives a system designer the option
of placing another subtractive decode bridge in the system (e.g., an Intel
380FB Dock Set).

.IP "IDE Interface (Bus Master capability and synchronous DMA Mode)"
The fast IDE interface supports up to four IDE devices providing an interface
for IDE hard disks and CD ROMs. Each IDE device can have independent timings.
The IDE interface supports PIO IDE transfers up to 14 Mbytes/sec and Bus Master
IDE transfers up to 33 Mbytes/sec. It does not consume any ISA DMA resources.
The IDE interface integrates 16x32-bit buffers for optimal transfers.

PIIX4's IDE system contains two independent IDE signal channels. They can be
configured to the standard primary and secondary channels (four devices) or
primary drive 0 and primary drive 1 channels (two devices).This allows
flexibility in system design and device power management.

.IP "Compatibility Modules"
The DMA controller incorporates the logic of two 82C37 DMA controllers, with
seven independently programmable channels. Channels [0:3] are hardwired to
8-bit, count-by-byte transfers, and channels [5:7] are hardwired to 16-bit,
count-by-word transfers. Any two of the seven DMA channels can be programmed to 
support fast Type-F transfers. The DMA controller also generates the ISA refresh
cycles.

The DMA controller supports two separate methods for handling legacy DMA via the
PCI bus. The PC/PCI protocol allows PCI-based peripherals to initiate DMA cycles
by encoding requests and grants via three PC/PCI REQ#/GNT# pairs. The second
method, Distributed DMA, allows reads and writes to 82C37 registers to be
distributed to other PCI devices. The two methods can be enabled concurrently.
The serial interrupt scheme typically associated with Distributed DMA is also
supported.

The timer/counter block contains three counters that are equivalent in function
to those found in one 82C54 programmable interval timer. These three counters
are combined to provide the system timer function, refresh request, and speaker
tone. The 14.31818-MHz oscillator input provides the clock source for these
three counters.

PIIX4 provides an ISA-Compatible interrupt controller that incorporates the
functionality of two 82C59 interrupt controllers. The two interrupt
controllers are cascaded so that 14 external and two internal interrupts are
possible. In addition, PIIX4 supports a serial interrupt scheme. PIIX4 provides
full support for the use of an external IO APIC.

.IP "Enhanced Universal Serial Bus (USB) Controller"
The PIIX4 USB controller provides enhanced support for the Universal Host
Controller Interface (UHCI). This includes support that allows legacy software
to use a USB-based keyboard and mouse.

.IP "RTC"
PIIX4 contains a Motorola MC146818A-compatible real-time clock with 256 bytes
of battery-backed RAM. The real-time clock performs two key functions: keeping
track of the time of day and storing system data, even when the system is
powered down. The RTC operates on a 32.768-kHz crystal and a separate 3V lithium
battery that provides up to 7 years of protection.

The RTC also supports two lockable memory ranges. By setting bits in the
configuration space, two 8-byte ranges can be locked to read and write accesses.
This prevents unauthorized reading of passwords or other system security
information. The RTC also supports a date alarm, that allows for scheduling a
wake up event up to 30 days in advance, rather than just 24 hours in advance.

.IP "GPIO and Chip Selects"
Various general purpose inputs and outputs are provided for custom system
design. The number of inputs and outputs varies depending on PIIX4
configuration. Two programmable chip selects are provided which allows the
designer to place devices on the X-Bus without the need for external decode logic.

.IP "Pentium and Pentium II Processor Interface"
The PIIX4 CPU interface allows connection to all Pentium and Pentium II
processors. The Sleep mode for the Pentium II processors is also supported.

.IP "Enhanced Power Management"
PIIX4's power management functions include enhanced clock control, local and
global monitoring support for 14 individual devices, and various low-power
(suspend) states, such as Power-On Suspend, Suspend-to-DRAM, and
Suspend-to-Disk. A hardware-based thermal management circuit permits
software-independent entrance to low-power states. PIIX4 has dedicated pins to
monitor various external events (e.g., interfaces to a notebook lid,
suspend/resume button, battery low indicators, etc.). PIIX4 contains full
support for the Advanced Configuration and Power Interface (ACPI) Specification.

.IP "System Management Bus (SMBus)"
PIIX4 contains an SMBus Host interface that allows the CPU to communicate with
SMBus slaves and an SMBus Slave interface that allows external masters to
activate power management events.

.IP "Configurability"
PIIX4 provides a wide range of system configuration options. This includes full
16-bit I/O decode on internal modules, dynamic disable on all the internal
modules, various peripheral decode options, and many options on system
configuration.

.LP

.I USAGE
This library provides low level routines for PCI - ISA bridge intialization,
and PCI interrupts routing. There are many functions provided here for
enabling different logical devices existing on ISA bus.

The functions addressed here include:

.IP "   -"
Initialization of the library.
.IP "   -"
Creating a logical device using an instance of physical device
on PCI bus and initializing internal database accordingly.
.IP "   -"
Initializing keyboard (logical device number 11) on PIIX4. 
.IP "   -"
Initializing floppy disk drive (logical device number 5) on PIIX4.
.IP "   -"
Initializing ATA device (IDE interface) on PIIX4.
.IP "   -"
Route PIRQ[A:D] from PCI expansion slots on given PIIX4.
.IP "   -"
Get interrupt level for a given device on PCI expansion slot.
.IP "   -"
Get handle to an internal database using instance on PIIX4 device
on PCI bus.

.LP

.I INTERNAL DATABASES
This library uses an internal database hidden from it's user. The control to
resize the internal database is given to user in the form of two mnemonics.
These mnemonics can be overridden by defining in architecture related BSP
header file. If not redefined, they take their default values as defined in
iPiix4Pci.h file.

.IP "IPIIX4_PCI_DEVMAX"
Defines the maximum number of PIIX4 that can be found on user's board.

.IP "IPIIX4_PCI_XINT_MAX"
Defines the maximum number of interrupt levels that a user would like to route
using this facility.

.LP

.I USER INTERFACE

.CS
STATUS iPiix4PciLibInit
    (
    VOID
    )
.CE

The above mentioned routine should be the first routine that is called amongst
many routine illustrated here. This routine intializes many internal databases
required by this library to function properly. The size of internal databases
can be changed by overridding IPIIX4_PCI_DEVMAX and IPIIX4_PCI_XINT_MAX mnemonics.

.CS
int iPiix4PciDevCreate
    (
    int instance
    )
.CE

The above mentioned routine should be called for a given instance of device on
PCI bus before intializing ISA based devices or interrupt routing. This routine
checks for the existence of given instance of the bus and hands out a handle (
an index into its internal data structures) to use for further call on given
device. The user of this library should always use correct handle for proper
device intialization.

.CS
STATUS iPiix4PciKbdInit
    (
    int handle
    )
.CE

The above mentioned routine does keyboard specific intialization on PIIX4.

.CS
STATUS iPiix4PciFdInit
    (
    int handle
    )
.CE

The above mentioned routine does floppy disk specific intialization on PIIX4.

.CS
STATUS iPiix4PciAtaInit
    (
    int handle
    )
.CE

The above mentioned routine does ATA device specific intialization on PIIX4.

.CS
STATUS iPiix4PciIntrRoute
    (
    int handle
    )
.CE

The above mentioned routines routes PIRQ[A:D] to interrupt routing state
machine embedded in PIIX4 and makes them level triggered. This routine
should be called early in boot process.

.CS
int iPiix4PciGetIntr
    (
    int handle
    )
.CE

This above mentioned routine gives out the interrupt level to driver
to use.

.CS
int iPiix4PciGetHandle
    (
    int vendId,
    int devId,
    int instance
    )
.CE

The above mentioned routine gives handle for already created device (using
iPiix4PciDevCreate () ). This routine checks for the existence of given
device on PCI bus. If found, checks existing entries in internal database
to get a match. If match found return handle to that entry in internal
databases.

INCLUDE FILES: iPiix4Pci.h
*/

/* includes */
#include "vxWorks.h"
#include "sysLib.h"
#include "string.h"
#include "drv/pci/pciConfigLib.h"
#include "drv/pci/iPiix4Pci.h"


/* locals */
LOCAL IPIIX4PCI_HANDLE iPiix4PciHandle [IPIIX4_PCI_DEVMAX];
LOCAL IPIIX4PCI_HANDLE * iPiix4PciHdlPtr;
LOCAL UCHAR iPiix4PciIntRoute [IPIIX4_PCI_DEVMAX][IPIIX4_PCI_XINT_MAX];
LOCAL UCHAR iPiix4PciIntrIdx[IPIIX4_PCI_DEVMAX];

/*******************************************************************************
*
* iPiix4PciLibInit - intializes PIIX4 device library
*
* This routine will intializes (zero out) data structures used by this library.
* This is a necessary step to guarantee proper functioning of this library.
*
* RETURNS: OK/ERROR
*/

STATUS iPiix4PciLibInit
    (
    VOID
    )
    {
    int i;
    int j;
    unsigned char intrCounter;
    
    /* zero out array of handles before use */
    memset (iPiix4PciHandle, '\0', IPIIX4_PCI_DEVMAX);
    memset (iPiix4PciIntrIdx, '\0', IPIIX4_PCI_DEVMAX);

    for (i = 0; i < IPIIX4_PCI_DEVMAX; i++)
        {
        for (j = 0, intrCounter = IPIIX4_PCI_XINT1_LVL;
             j < IPIIX4_PCI_XINT_MAX;
             j++, intrCounter++)
            {
            iPiix4PciIntRoute [i][j] = intrCounter;
            }
        }

    return (OK);
    }


/*******************************************************************************
*
* iPiix4PciDevCreate - creates the PIIX4 device 
*
* This routine will logically create PIIX4 device and hands out a handle to
* caller to use.
*
* RETURNS: index into array of handles (PASS), or -1 (FAIL).
*/

int iPiix4PciDevCreate
    (
    int instance
    )
    {
    int i;
    int tempInt;

    /* check for first available free slot in array of handles */
    for (i = 0; i < IPIIX4_PCI_DEVMAX; i++)
        {
        iPiix4PciHdlPtr = &iPiix4PciHandle [i];

        if (iPiix4PciHdlPtr->handleInUse != TRUE)
            {
            break;
            }
        }

    if (i >= IPIIX4_PCI_DEVMAX)
        return (IPIIX4_PCI_NOIDX);  /* haven't found any slot free */

    /* yup, found a free slot. check to see if device on pci bus exists */
    if (pciFindDevice (IPIIX4_PCI_VENID,
                       IPIIX4_PCI_DEV0,
                       instance,
                       &iPiix4PciHdlPtr->busId,
                       &iPiix4PciHdlPtr->devId,
                       &tempInt) != OK)
        {
        return (IPIIX4_PCI_NOIDX);
        }
    
    /* device found. set handleInUse to TRUE */
    iPiix4PciHdlPtr->instance = instance;
    iPiix4PciHdlPtr->handleInUse = TRUE;
    
    return (i);
    }


/*******************************************************************************
*
* iPiix4PciKbdInit - initializes the PCI-ISA/IDE bridge 
*
* This routine will initialize PIIX4 - PCI-ISA/IDE bridge to enable keyboard
* device and IRQ routing
*
* RETURNS: OK/ERROR
*/

STATUS iPiix4PciKbdInit
    (
    int handle
    )
    {
    int longData;
    short int shortData;

    /* check if handle passed is with in limited range */
    if (handle >= IPIIX4_PCI_DEVMAX)
      return ERROR;

    iPiix4PciHdlPtr = &iPiix4PciHandle [handle];
    
    pciConfigInWord (iPiix4PciHdlPtr->busId, iPiix4PciHdlPtr->devId,
                     IPIIX4_PCI_FUNC3, IPIIX4_PCI_DEVRESD, &shortData);
    pciConfigOutWord (iPiix4PciHdlPtr->busId, iPiix4PciHdlPtr->devId,
                      IPIIX4_PCI_FUNC3, IPIIX4_PCI_DEVRESD,
                      (shortData | IPIIX4_PCI_IRQ1ENDEV11));
    
    pciConfigInLong (iPiix4PciHdlPtr->busId, iPiix4PciHdlPtr->devId,
                     IPIIX4_PCI_FUNC3, IPIIX4_PCI_DEVRESA, &longData);
    pciConfigOutLong (iPiix4PciHdlPtr->busId, iPiix4PciHdlPtr->devId,
                      IPIIX4_PCI_FUNC3, IPIIX4_PCI_DEVRESA,
                      (longData | IPIIX4_PCI_KBCENDEV11));
    
    pciConfigInLong (iPiix4PciHdlPtr->busId, iPiix4PciHdlPtr->devId,
                     IPIIX4_PCI_FUNC3, IPIIX4_PCI_DEVRESB, &longData);
    pciConfigOutLong (iPiix4PciHdlPtr->busId, iPiix4PciHdlPtr->devId,
                      IPIIX4_PCI_FUNC3, IPIIX4_PCI_DEVRESB,
                      (longData | IPIIX4_PCI_KBCEIOEN));

    return (OK);
    }


/*******************************************************************************
*
* iPiix4PciFdInit - initializes the floppy disk device
*
* This routine will initialize PIIX4 - PCI-ISA/IDE bridge and DMA
* for proper working of floppy disk device
*
* RETURNS: OK/ERROR
*/

STATUS iPiix4PciFdInit
    (
    int handle
    )
    {
    unsigned char data;

    /* check if handle passed is with in limited range */    
    if (handle >= IPIIX4_PCI_DEVMAX)
      return ERROR;

    iPiix4PciHdlPtr = &iPiix4PciHandle [handle];
    
    /* DMA controller and Floppy */    
    sysOutByte (IPIIX4_PCI_FD_DCM,
                (IPIIX4_PCI_FD_DCM_CASCADE | IPIIX4_PCI_FD_DCM_AUTOINIT));
    
    sysOutByte (IPIIX4_PCI_FD_RWAMB, IPIIX4_PCI_FD_RWAMB_MASKALL);
    
    pciConfigInByte (iPiix4PciHdlPtr->busId, iPiix4PciHdlPtr->devId,
                     IPIIX4_PCI_FUNC3, (IPIIX4_PCI_DEVRESD + 1), &data);
    pciConfigOutByte (iPiix4PciHdlPtr->busId, iPiix4PciHdlPtr->devId,
                      IPIIX4_PCI_FUNC3, (IPIIX4_PCI_DEVRESD + 1),
                      (data | IPIIX4_PCI_RESENDEV5));
    return (OK);
    }


/*******************************************************************************
*
* iPiix4PciAtaInit - low level initialization of ATA device 
*
* This routine will initialize PIIX4 - PCI-ISA/IDE bridge for proper
* working of ATA device.
*
* RETURNS: OK/ERROR
*/

STATUS iPiix4PciAtaInit
    (
    int handle
    )
    {
    /* check if handle passed is with in limited range */      
    if (handle >= IPIIX4_PCI_DEVMAX)
      return ERROR;

    iPiix4PciHdlPtr = &iPiix4PciHandle [handle];
    
    
    pciConfigOutWord (iPiix4PciHdlPtr->busId, iPiix4PciHdlPtr->devId,
                      IPIIX4_PCI_FUNC1, IPIIX4_PCI_PCICMD,
                      (IPIIX4_PCI_PCICMD_BME | IPIIX4_PCI_PCICMD_IOSE));
    
    pciConfigOutByte (iPiix4PciHdlPtr->busId, iPiix4PciHdlPtr->devId,
                      IPIIX4_PCI_FUNC1, IPIIX4_PCI_MLT, IPIIX4_PCI_MLT_MLTCV);
    
    pciConfigOutLong (iPiix4PciHdlPtr->busId, iPiix4PciHdlPtr->devId,
                      IPIIX4_PCI_FUNC1, IPIIX4_PCI_BMIBA, IPIIX4_PCI_BMIBA_RTE);
    
    pciConfigOutLong (iPiix4PciHdlPtr->busId, iPiix4PciHdlPtr->devId,
                      IPIIX4_PCI_FUNC1, IPIIX4_PCI_IDETIM, IPIIX4_PCI_IDETIM_VAL);

    return (OK);
    }


/*******************************************************************************
*
* iPiix4PciIntrRoute - Route PIRQ[A:D]
*
* This routine will initialize PIIX4 - PCI-ISA/IDE bridge for PCI expansion
* slot interrupt routing.
*
* RETURNS: OK/ERROR
*/

STATUS iPiix4PciIntrRoute
    (
    int handle
    )
    {
    unsigned char regValue;
    unsigned char tmpValue;
    int i;

    /* check if handle passed is with in limited range */          
    if (handle >= IPIIX4_PCI_DEVMAX)
      return ERROR;

    iPiix4PciHdlPtr = &iPiix4PciHandle [handle];
    
    for (i = 0; i < IPIIX4_PCI_XINT_MAX; i++)
        {
        tmpValue = iPiix4PciIntRoute[handle][i];
        
        /* route PIRQ signal to IRQ# input of PIC */
        pciConfigOutByte (iPiix4PciHdlPtr->busId,
                          iPiix4PciHdlPtr->devId,
                          IPIIX4_PCI_FUNC0,
                          (IPIIX4_PCI_PIRQRCA + i),
                          tmpValue);
        
        /* make IRQs level triggered */
        regValue = sysInByte (IPIIX4_PCI_ELCR2);
        sysOutByte (IPIIX4_PCI_ELCR2,
                    (regValue | (1 << (tmpValue - 0x8))));
        }

    return (OK);
    }

/*******************************************************************************
*
* iPiix4PciGetIntr - give device an interrupt level to use
*
* This routine will give device an interrupt level to use. An autoroute in
* disguise.
*
* RETURNS: int - interrupt level
*/

int iPiix4PciGetIntr
    (
    int handle
    )
    {
    int tmpInt;
    
    /* check if handle passed is with in limited range */            
    if ((handle >= IPIIX4_PCI_DEVMAX) ||
        (iPiix4PciIntrIdx[handle] >= IPIIX4_PCI_XINT_MAX))
      {
        return (IPIIX4_PCI_NOINT);
      }

    tmpInt = iPiix4PciIntrIdx[handle]++;

    return (iPiix4PciIntRoute [handle][tmpInt]);
    }

/*******************************************************************************
*
* iPiix4PciGetHandle - give caller a handle for given device information
*
* This routine will give a handle to caller from array of logical PIIX4s
* already configured using iPiix4PciDevCreate ().
*
* RETURNS: int. -1 (FAIL); handle (PASS)
*/

int iPiix4PciGetHandle
    (
    int vendId,
    int devId,
    int instance
    )
    {
    int i;
    int tmpBus;
    int tmpDev;
    int tmpFunc;
    
    /* check to see if device on pci bus exists */
    if (pciFindDevice (vendId,
                       devId,
                       instance,
                       &tmpBus,
                       &tmpDev,
                       &tmpFunc) != OK)
        {
        return (IPIIX4_PCI_NOIDX);
        }

    /* check for first available free slot in array of handles */
    for (i = 0; i < IPIIX4_PCI_DEVMAX; i++)
        {
        iPiix4PciHdlPtr = &iPiix4PciHandle [i];

        if ((iPiix4PciHdlPtr->handleInUse == TRUE) &&
        (iPiix4PciHdlPtr->busId == tmpBus) &&
        (iPiix4PciHdlPtr->devId == tmpDev) &&
        (iPiix4PciHdlPtr->instance == instance))
            {
            break;
            }
        }

    if (i >= IPIIX4_PCI_DEVMAX)
        return (IPIIX4_PCI_NOIDX);  /* haven't found any already created device */

    return (i);
    }

