/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

 * pciConfigLib.c - PCI Configuration space access support for PCI drivers */

/* Copyright 1984-1998 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01z,28jul98,tm   PCI_MAX_BUS,.._DEV, .._FUNC moved to pciConfigLib.h and 
                   config.h now included to allow BSP values to override 
01y,20jul98,tm   fixed extistence checks to look only at vendor ID SPR 21934
01x,29may98,tm   added pciConfigModify{Byte,Word}; revised Modify* arguments
01w,05may98,tm   changed pciMaxDev, pciMaxFunc -> PCI_MAX_DEV, PCI_MAX_FUNC
01v,09apr98,tm   fixed pciConfigModifyLong: could return leaving ints locked
                 added alignment checks to word and longword access routines
01u,21mar98,tm   added pciMaxDev, pciMaxFunc in support of autoconfiguration
01t,16mar98,tm   documentation tweaking/cleanup
		 coding standard name tweak: pciConfigBDFPack() -> ..BdfPack()
                 changed non-bus-0 maximum devices from 16 -> 32;
01s,11mar98,tm   renamed to pciConfigLib.c from pciIomapLib.c;
                 removed PCI_DELAY
                 pciPack scope/name changed: LOCAL int -> int pciConfigBdfPack
01r,08mar98,tm   merged new documentation;
                 adapted ebsa285 BSP mechanism '3' to use mech '0' added in v01o
01q,06mar98,tm   fixed pciConfigOut{Byte,Word} UINT32->UINT8/16 problem SPR?????
01p,04mar98,tm   PCI_MAX_BUS is now a variable;
                 Configuration space I/O fn argument types clarified
01o,04mar98,dat  added Mechanism 0, a non-standard config space access method.
01n,26feb98,tm   pciInt(), pciIntConnect() and pciIntDisconnect() moved
                 to pciIntLib.c
01m,24nov97,jpd  made conditional on INCLUDE_PCI.
01l,27oct97,kkk  took out "EOF" line at end of file.
01k,26sep97,jpd  make all config accesses 32-bit. Add Type 1 config addresses.
01j,02sep97,jpd  added support for non-PC/PowerPC configuration.
01i,25apr97,mas  tweaked DELAY loop for long read; no loop for long write.
01h,22apr97,mas  added include of dllLib.h; added pciDevConfig; converted
		 semaphore-based exclusion to intLock-based exclusion; fixed
		 pciFindXxxx to skip nonexistant devices; limited bus scanning
		 to PCI_MAX_BUS, PCI_MAX_DEV and PCI_MAX_FUNC (SPR 8226).
01g,12jan97,hdn  changed variable name "vender" to "vendor".
01f,12jan97,hdn  changed variable name "class" to "classCodeReg".
01e,03dec96,hdn  added single/multi function check.
01d,16sep96,dat  made pciConfigMech global (for pciIomapShow).
01c,06aug96,hdn  added pciInt(), pciIntConnect() and pciIntDisconnect().
01b,14mar96,hdn  re-written.  changed parameters of the functions.
		 removed BIOS dependent codes.
01a,25feb95,bcs  written
*/


/*

DESCRIPTION

This module contains routines to support accessing the PCI bus Configuration
Space.  The library is PCI Revision 2.1 compliant.

In general, functions in this library should not be called from the interrupt
level, (except pciInt()) because Configuration Space access, which is slow,
should be limited to initialization only.

The functions addressed here include:

.IP "   -"
Initialization of the library.
.IP "   -"
Locating a device by Device ID and Vendor ID.
.IP "   -"
Locating a device by Class Code.
.IP "   -"
Generation of Special Cycles.
.IP "   -"
Accessing Configuration Space structures.

.LP

.SH PCI BUS CONCEPTS

The PCI bus is an unterminated, high impedence CMOS bus using reflected wave
signalling as opposed to incident wave.  Because of this, the PCI bus is
physically limited in length and the number of electrical loads that can be
supported.  Each device on the bus represents one load, including adapters
and bridges.

To accomodate additional devices, the PCI standard allows multiple PCI buses
to be interconnected via PCI-to-PCI bridge (PPB) devices to form one large bus.
Each constituent bus is refered to as a bus segment and is subject to the above
limitations.

The bus segment accessible from the host bus adapter is designated the primary
bus segment (see figure).  Progressing outward from the primary bus (designated
segment number zero from the PCI architecture point of view) are the secondary
and tertiary buses, numbered as segments one and two, respectively.  Due to
clock skew concerns and propagation delays, practical PCI bus architectures do
not implement bus segments beyond the tertiary level.

.CS
                      ---------
                      |       |
                      |  CPU  |
                      |       |
                      ---------
                          |
       Host bus           |
    ----------------------+-------------------------- ...
                          |
                     ------------
                     | Bridge 0 |
                     |  (host   |
                     | adapter) |
                     ------------
                          |
       PCI bus segment 0  |  (primary bus segment)
    ----------------------+-------------------------- ...
             |            |    |                  |
           dev 0          |   dev 1              dev 2
                          |
                     ------------
                     |          |
                     | Bridge 1 |
                     |  (P2P)   |
                     ------------
                          |
       PCI bus segment 1  |  (secondary bus segment)
    ----------------------+-------------------------- ...
             |            |    |                  |
           dev 0          |   dev 1              dev 2
                          |
                     ------------
                     |          |
                     | Bridge 2 |
                     |  (P2P)   |
                     ------------
                          |
       PCI bus segment 2  |  (tertiary bus segment)
    ----------------------+-------------------------- ...
             |                 |                  |
           dev 0              dev 1              dev 2
.CE

For further details, refer to the PCI to PCI Bridge Architecture Specification.

.SH I/O MACROS AND CPU ENDIANESS

PCI bus I/O operations must adhere to little endian byte ordering.  Thus if
an I/O operation larger than one byte is performed, the lower I/O addresses
contain the least signficant bytes of the multi-byte quantity of interest.

For architectures that adhere to big-endian byte ordering, byte-swapping
must be performed.  The architecture-specific byte-order translation is done 
as part of the I/O operation in the following routines: sysPciInByte, 
sysPciInWord, sysPciInLong, sysOutPciByte, sysPciOutWord, sysPciOutLong.
The interface to these routines is mediated by the following macros:

.IP "PCI_IN_BYTE"
read a byte from PCI I/O Space
.IP "PCI_IN_WORD"
read a word from PCI I/O Space
.IP "PCI_IN_LONG"
read a longword from PCI I/O Space
.IP "PCI_OUT_BYTE"
write a byte from PCI I/O Space
.IP "PCI_OUT_WORD"
write a word from PCI I/O Space
.IP "PCI_OUT_LONG"
write a longword from PCI I/O Space
.LP

By default, these macros call the appropriate PCI I/O routine, such
as sysPciInWord.  For architectures that do not require byte swapping,
these macros simply call the appropriate default I/O routine, such as
sysInWord.  These macros may be redefined by the BSP if special
processing is required.

.SH INITIALIZATION

pciConfigLibInit() should be called before any other pciConfigLib functions.
Generally, this is performed by sysHwInit().

After the library has been initialized, it may be utilized to find devices,
and access PCI configuration space.

Any PCI device can be uniquely addressed within Configuration Space
by the 'geographic' specification of a Bus segment number, Device number, and a
Function number (BDF).  The configuration registers of a PCI device are
arranged by the PCI standard according to a Configuration Header structure.
The BDF triplet specifies the location of the header structure of one device.
To access a configuration register, its location in the header must be given.
The location of a configuration register of interest is simply the structure
member offset defined for the register.  For further details, refer to the
PCI Local Bus Specification, Revision 2.1.  Refer to the header file
pciConfigLib.h for the defined standard configuration register offsets.

The maximum number of Type-1 Configuration Space buses supported in the 2.1
Specifications is 256 (0x00 - 0xFF), far greater than most systems currently
support.  Most buses are numbered sequentially from 0.  An optional define
called PCI_MAX_BUS may be declared in config.h to override the default
definition of 256.  Similarly, the default number of devices and functions
may be overriden by defining PCI_MAX_DEV and/or PCI_MAX_FUNC.  Note that
the number of devices applies only to bus zero, all others being restricted
to 16 by the 2.1 spec.


ACCESS MECHANISM 1:
This is the preferred access mechanism for a PC-AT class machines.  It
uses two standard PCI I/O registers to initiate a configuration cycle.
The type of cycle is determined by the Host-bridge device based on
the devices primary bus number.  If the configuration bus number matches
the primary bus number then a type 0 configuration cycle occurs.  Otherwise
a type 1 cycle is generated.  This is all transparent to the user.

The two arguments used for mechanism 1 are the CAR register address which
by default is PCI_CONFIG_ADDR (0xCF8), and the CDR register address which
is normally PCI_CONFIG_DATA (0xCFC).

.CS
    e.g.

    pciConfigLibInit (PCI_MECHANISM_1, PCI_CONFIG_ADDR,
                     PCI_CONFIG_DATA, NULL);
.CE

ACCESS MECHANISM 2:
This is the non-preferred legacy mechanism for PC-AT class machines.
The three arguments used for mechanism 2 are the CSE register address which
by default is PCI_CONFIG_CSE (0xCF8), and the Forward register address which
is normally PCI_CONFIG_FORWARD (0xCFA), and the configuration base address
which is normally PCI_CONFIG_BASE (0xC000).

.CS
    e.g.

    pciConfigLibInit (PCI_MECHANISM_2, PCI_CONFIG_CSE,
                      PCI_CONFIG_FORWARD, PCI_CONFIG_BASE);
.CE

ACCESS MECHANISM 0:
We have added a non-standard access method that we call method 0.  Selecting
method 0 installs user supplied read and write routines to actually
handle configuration read and writes (32 bit accesses only).  The BSP
will supply pointers to these routines as arguments 2 and 3 (read routine
is argument 2, write routine is argument 3).  A user provided special
cycle routine is argument 4.  The special cycle routine is optional and 
a NULL pointer should be used if the special cycle routine is not provided
by the BSP.

All accesses are expected to be 32 bit accesses with these routines.
The code in this library will perform bit manipulation to emulate
byte and word operations. All routines return OK to indicate successful
operation and ERROR to indicate failure.

Initialization examples using special access method 0:

.CS
    pciConfigLibInit (PCI_MECHANISM_0, myReadRtn,
                     myWriteRtn, mySpecialRtn);
    -or-
  
    pciConfigLibInit (PCI_MECHANISM_0, myReadRtn,
                     myWriteRtn, NULL);
.CE

The calling convention for the user read routine is:

.CS
    STATUS myReadRtn (int bus, int dev, int func,
                      int reg, UINT32 * pResult);
.CE

The calling convention for the user write routine is:

.CS
    STATUS myWriteRtn (int bus, int dev, int func,
                       int reg, UINT32 data);
.CE

The calling convention for the optional special cycle routine is:

.CS
    STATUS mySpecialRtn (int bus, UINT32 data);
.CE

In the Type-1 method, PCI Configuration Space accesses are made by the
sequential access of two 32-bit hardware registers: the Configuration Address
Register (CAR) and the Configuration Data Register (CDR).  The CAR is written
to first with the 32-bit value designating the PCI bus number, the device on
that bus, and the offset to the configuration register being accessed in the
device.  The CDR is then read or written, depending on whether the register
of interest is to be read or written.  The CDR access may be 8-bits, 16-bits,
or 32-bits in size.  Both the CAR and CDR are mapped by the standard to
predefined addresses in the PCI I/O Space: CAR = 0xCF8 and CDR = 0xCFC.

The Type-2 access method maps any one configuration header into a fixed 4K byte
window of PCI I/O Space.  In this method, any PCI I/O Space access within the
range of 0xC000 to 0xCFFF will be translated to a Configuration Space access.
This access method utilizes two 8-bit hardware registers: the Configuration
Space Enable register (CSE) and the Forward register (CFR).  Like the CAR and
CDR, these regiters occupy preassigned PCI I/O Space addresses: CSE = 0xCF8,
CFR = 0xCFA.  The CSE specifies the device to be accessed and the function
within the device.  The CFR specifies the bus number on which the device of
interest resides.  The access sequence is 1) write the bus number to CFR,
2) write the device location information to CSE, and 3) perform an 8-bit,
16-bit, or 32-bit read or write at an offset into the PCI I/O Space starting
at 0xC000.  The offset specifies the configuration register within the
configuration header which now appears in the 4K byte Configuration Space
window.

.SH PCI DEVICE LOCATION

After the library has been initialized, the Configuration Space of any PCI
device may be accessed after first locating the device.

Locating a device is accomplished using either pciFindDevice() or
pciFindClass().  Both routines require an index parameter indicating
which instance of the device should be returned, since multiple instances
of the same device may be present in a system.  The instance number is
zero-based.

pciFindDevice() accepts the following parameters:

.IP "vendorId"
the vendor ID of the device
.IP "deviceId"
the device ID of the device
.IP "index"
the instance number
.LP

pciFindClass() simply requires a class code and the index:

.IP "classCode"
the 24-bit class of the device
.IP "index"
the instance number
.LP

In addition, both functions return the following parameters by reference:

.IP "pBusNo"
where to return bus segment number containing the device
.IP "pDeviceNo"
where to return the device ID of the device
.IP "pFuncNo"
where to return the function number of the device
.LP

These three parameters, Bus segment number, Device number, and Function number
(BDF), provide a means to access the Configuration Space of any PCI device.

.SH PCI BUS SPECIAL CYCLE GENERATION

The PCIbus Special Cycle is a cycle used to broadcast data to one or many 
devices on a target PCI bus. It is common, for example, for Intel x86-based
systems to broadcast to PCI devices that the system is about to go into
a halt or shutdown condition.

The special cycle is initiated by software. Utilizing CSAM-1, a 32-bit
write to the configuration address port specifying the following

.IP "Bus Number"
is the PCI bus of interest
.IP "Device Number"
is set to all 1's (01Fh)
.IP "Function Number"
is set to all 1's (07d)
.IP "Configuration Register Number"
is zeroed
.LP

The pciSpecialCycle() function facilitates generation of a Special Cycle
by generating the correct address data noted above.  The data passed to 
the function is driven onto the bus during the Special Cycle's data phase.
The parameters to the pciSpecialCycle() function are:

.IP "busNo"
bus on which Special Cycle is to be initiated
.IP "message"
data driven onto AD[31:0] during the Special Cycle
.LP

.SH PCI DEVICE CONFIGURATION SPACE ACCESS

The routines pciConfigInByte(), pciConfigInWord(), pciConfigInLong(),
pciConfigOutByte(), pciConfigOutWord(), and pciConfigOutLong() may be used
to access the Configuration Space of any PCI device, once the library
has been properly initialized.  It should be noted that, if no device exists
at the given BDF address, the resultant behavior of the Configuration Space
access routines is to return a value with all bits set, as set forth in the
PCI bus standard.

In addition to the BDF numbers obtained from the pciFindXxx functions,
an additional parameter specifying an offset into the PCI Configuration Space
must be specified when using the access routines.  VxWorks includes defined
offsets for all of the standard PCI Configuration Space structure members as
set forth in the PCI Local Bus Specification 2.1 and the PCI Local Bus PCI
to PCI Bridge Architecture Specification 1.0.  The defined offsets are all
prefixed by "PCI_CFG_".  For example, if Vendor ID information is required,
PCI_CFG_VENDOR_ID would be passed as the offset argument to the access routines.

In summary, the pci configuration space access functions described above
accept the following parameters.

Input routines:

.IP "busNo"
bus segment number on which the device resides
.IP "deviceNo"
device ID of the device
.IP "funcNo"
function number of the device
.IP "offset"
offset into the device configuration space
.IP "pData"
where to return the data
.LP

Ouput routines:

.IP "busNo"
bus segment number on which the device resides
.IP "deviceNo"
device ID of the device
.IP "funcNo"
function number of the device
.IP "offset"
offset into the device configuration space
.IP "Data"
data to be written
.LP

.SH PCI DEVICE CONFIGURATION

The function pciDevConfig() is used to configure PCI devices that require no
more than one Memory Space and one I/O Space.  According to the PCI standard,
a device may have up to six 32-bit Base Address Registers (BARs) each of which
can have either a Memory Space or I/O Space base address.  In 64-bit PCI
devices, the registers double up to give a maximum of three 64-bit BARs.  The
64-bit BARs are not supported by this function nor are more than one 32-bit
BAR of each type, Memory or I/O.

The pciDevConfig() function sets up one PCI Memory Space and/or one I/O Space
BAR and issues a specified command to the device to enable it.  It takes the
following parameters:

.IP "pciBusNo"
PCI bus segment number
.IP "pciDevNo"
PCI device number
.IP "pciFuncNo"
PCI function number
.IP "devIoBaseAdrs"
base address of one IO-mapped resource
.IP "devMemBaseAdrs"
base address of one memory-mapped resource
.IP "command"
command to issue to device after configuration
.LP

.SH USAGE

The following code sample illustrates the usage of this library.  Initializa-
tion of the library is performed first, then a sample device is found and 
initialized.

.CS

    #include "drv/pci/pciConfigLib.h"

    #define PCI_ID_LN_DEC21140      0x00091011

    IMPORT pciInt();
    LOCAL VOID deviceIsr(int);

    int         param;
    STATUS      result;
    int         pciBusNo;       /@ PCI bus number @/
    int         pciDevNo;       /@ PCI device number @/
    int         pciFuncNo;      /@ PCI function number @/

/@
 * Initialize module to use CSAM-1 
 * (if not performed in sysHwInit()) 
 *
 @/

    if (pciConfigLibInit (PCI_MECHANISM_1, 
                          PCI_PRIMARY_CAR, 
                          PCI_PRIMARY_CDR, 
                          0)
        != OK)
        {
        sysToMonitor (BOOT_NO_AUTOBOOT);
        }

/@
 * Find a device by its device ID, and use the 
 * Bus, Device, and Function number of the found 
 * device to configure it, using pciDevConfig(). In 
 * this case, the first instance of a DEC 21040 
 * Ethernet NIC is searched for.  If the device
 * is found, the Bus, Device Number, and Function
 * Number are fed to pciDevConfig, along with the
 * constant PCI_IO_LN2_ADRS, which defines the start
 * of the I/O space utilized by the device. The
 * device and its I/O space is then enabled.
 *
 @/

    if (pciFindDevice (PCI_ID_LN_DEC21040 & 0xFFFF,
                       (PCI_ID_LN_DEC21040 >> 16) & 0xFFFF, 
                       0,
                       &pciBusNo, 
                       &pciDevNo, 
                       &pciFuncNo) 
        != ERROR)
       {
       (void)pciDevConfig (pciBusNo, pciDevNo, pciFuncNo,
                           PCI_IO_LN2_ADRS,
                           NULL,
                           (PCI_CMD_MASTER_ENABLE |
                            PCI_CMD_IO_ENABLE));
       }

.CE 

INCLUDE FILES:
pciConfigLib.h

SEE ALSO: 
.I "PCI Local Bus Specification, Revision 2.1, June 1, 1996"
.I "PCI Local Bus PCI to PCI Bridge Architecture Specification, Revision 1.0,
April 5, 1994"
*/
#include "intLib.h"
#include "vxWorks.h"
#include "config.h"
#include "dllLib.h"
#include "sysLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "pciConfigLib.h"

#if defined(INCLUDE_PCI)

extern void   sysOutWord (ULONG address, UINT16 data);
extern void   sysOutLong (ULONG address, ULONG data);
extern UINT16 sysInWord  (ULONG address);
extern ULONG  sysInLong  (ULONG address);

extern int bus[],func[],devNum[];
IMPORT void frcPciFindShow();

IMPORT void vxHid1Set(UINT32);
IMPORT UINT32 vxHid1Get();

static void pciConfigEnableMachineCheckException(void);
static void pciConfigDisableMachineCheckException(void);



/* defines */

/* globals */

STATUS  pciLibInitStatus = NONE;	/* initialization done */

int     pciConfigMech = NONE;		/* 1=mechanism-1, 2=mechanism-2 */

int     pciMaxBus = PCI_MAX_BUS;	/* Max number of sub-busses */

/* locals */
/* uri changed at 12/8/99 from local to non local in order to change functions
   and address register and data register from PCI 0 to PCI 1 */
int pciConfigAddr0;		/* config-addr-reg, CSE-reg*/
int pciConfigAddr1;		/* config-data-reg, forward-reg */
int pciConfigAddr2;		/* not-used, base-addr */

FUNCPTR frcPciConfigRead  = NULL;     /* user-defined config space read */
FUNCPTR frcPciConfigWrite = NULL;     /* user-defined config space write */
FUNCPTR pciConfigSpcl  = NULL;     /* user-defined PCI special cycle*/


/*******************************************************************************
*
* pciConfigEnableMachineCheckException - enable Machine Check Exception
*
* This routine enables the Machine Check Exception.
*
* RETURNS: -
*
*/

static void pciConfigEnableMachineCheckException(void)
    {
    UINT32 udwhid0;

    udwhid0 = vxHid1Get();
    vxHid1Set(udwhid0 | 0xb0000000);

    return;
    }

/*******************************************************************************
*
* pciConfigDisableMachineCheckException - disable Machine Check Exception
*
* This routine disables the Machine Check Exception.
*
* RETURNS: -
*
*/

static void pciConfigDisableMachineCheckException(void)
    {
    UINT32 udwhid0;

    udwhid0 = vxHid1Get();
    if(udwhid0 & 0x30000000)
    {
        vxHid1Set(udwhid0 & ~0xb0000000);
    }

    return;
    }

/*******************************************************************************
*
* pciConfigLibInit - initialize the configuration access-method and addresses
*
* This routine initializes the configuration access-method and addresses.
*
* Configuration mechanism one utilizes two 32-bit IO ports located at addresses
* 0x0cf8 and 0x0cfc. These two ports are:
*
* .IP" - "
* 32-bit configuration address port, at 0x0cf8
* .IP" - "
* 32-bit configuration data port, at 0x0cfc
* .LP
*
* Accessing a PCI function's configuration port is two step process.
*
* .IP" - "
* Write the bus number, physical device number, function number and 
* register number to the configuration address port.
* .IP" - "
* Perform an IO read from or an write to the configuration data port.
* .LP
*
* Configuration mechanism two uses following two single-byte IO ports.
* .IP" - "
* Configuration space enable, or CSE, register, at 0x0cf8
* .IP" - "
* Forward register, at 0x0cfa
* .LP
*
* To generate a PCI configuration transaction, the following actions are
* performed.
*
* .IP " - "
* Write the target bus number into the forward register.
* .IP " - "
* Write a one byte value to the CSE register at 0x0cf8.  The bit
* pattern written to this register has three effects: disables the
* generation of special cycles; enables the generation of configuration
* transactions; specifies the target PCI functional device.
* .IP " - "
* Perform a one, two or four byte IO read or write transaction within
* the IO range 0xc000 through 0xcfff.
* .LP
*
* Configuration mechanism zero is for non-PC/PowerPC environments
* where an area of address space produces PCI configuration
* transactions. No support for special cycles is included.
*
* RETURNS:
* OK, or ERROR if a mechanism is not 0, 1, or 2.
*/

STATUS pciConfigLibInit
    (
    int mechanism, /* configuration mechanism: 0, 1, 2 */
    ULONG addr0,   /* config-addr-reg / CSE-reg */
    ULONG addr1,   /* config-data-reg / Forward-reg */
    ULONG addr2	   /* none            / Base-address */
    )
    {

    if (pciLibInitStatus != NONE)
	return (pciLibInitStatus);

    switch (mechanism)
	{
        case PCI_MECHANISM_0:   /* non-standard interface */
            if (addr0 == 0x0 || addr1 == 0x0)
	        {
                pciLibInitStatus = ERROR;
		}
            else
                {
                frcPciConfigRead = (FUNCPTR) addr0;
                frcPciConfigWrite = (FUNCPTR) addr1;
                pciConfigSpcl = (FUNCPTR) addr2;
                }
	    pciConfigMech	= mechanism;
	    pciLibInitStatus	= OK;
            break;
 
	case PCI_MECHANISM_1:
	case PCI_MECHANISM_2:
	    pciConfigMech	= mechanism;
	    pciConfigAddr0	= addr0;
	    pciConfigAddr1	= addr1;
	    pciConfigAddr2	= addr2;
	    pciLibInitStatus	= OK;
	    break;

	default:
    	    pciLibInitStatus	= ERROR;
	    break;
	}

    return (pciLibInitStatus);
    }


/*******************************************************************************
*
* pciFindDevice - find the nth device with the given device & vendor ID
*
* This routine finds the nth device with the given device & vendor ID.
*
* RETURNS:
* OK, or ERROR if the deviceId and vendorId didn't match.
*
*/

STATUS pciFindDevice
    (
    int    vendorId,	/* vendor ID */
    int    deviceId,	/* device ID */
    int    index,	/* desired instance of device */
    int *  pBusNo,	/* bus number */
    int *  pDeviceNo,	/* device number */
    int *  pFuncNo	/* function number */
    )
    {
    STATUS status = ERROR;
    BOOL   cont   = TRUE;
    int    busNo;
    int    deviceNo;
    int    funcNo;
    UINT32 device;
    UINT32 vendor;
    char   header;

    if (pciLibInitStatus != OK)			/* sanity check */
        cont = FALSE;

    for (busNo = 0; cont == TRUE && busNo <= pciMaxBus; busNo++)
        for (deviceNo = 0;
	     ((cont == TRUE) && (deviceNo < PCI_MAX_DEV));
	     ++deviceNo)
            for (funcNo = 0; cont == TRUE && funcNo < PCI_MAX_FUNC; funcNo++)
		{
		/* avoid a special bus cycle */

		if ((deviceNo == 0x1f) && (funcNo == 0x07))
		    continue;

		pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID,
				 &vendor);

		/*
		 * If nonexistent device, skip to next, only look at
		 * vendor ID field for existence check
		 */

		if (((vendor & 0x0000ffff) == 0x0000FFFF) && (funcNo == 0))
		    break;

		device  = vendor >> 16;
		device &= 0x0000FFFF;
		vendor &= 0x0000FFFF;
		if ((vendor == (UINT32)vendorId) &&
		    (device == (UINT32)deviceId) &&
		    (index-- == 0))
		    {
		    *pBusNo	= busNo;
		    *pDeviceNo	= deviceNo;
		    *pFuncNo	= funcNo;
		    status	= OK;
		    cont	= FALSE;	/* terminate all loops */
		    continue;
		    }

		/* goto next if current device is single function */

		pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE, 
				 &header);
		if ((header & PCI_HEADER_MULTI_FUNC) != PCI_HEADER_MULTI_FUNC &&
		    funcNo == 0)
		    break;
		}

    return (status);
    }

/*******************************************************************************
*
* pciFindClass - find the nth occurence of a device by PCI class code.
*
* This routine finds the nth device with the given 24-bit PCI class code
* (class subclass prog_if).
*
* RETURNS:
* OK, or ERROR if the class didn't match.
*
*/

STATUS pciFindClass
    (
    int    classCode,	/* 24-bit class code */
    int	   index,	/* desired instance of device */
    int *  pBusNo,	/* bus number */
    int *  pDeviceNo,	/* device number */
    int *  pFuncNo	/* function number */
    )
    {
    STATUS status = ERROR;
    BOOL   cont   = TRUE;
    int    busNo;
    int    deviceNo;
    int    funcNo;
    int    classCodeReg;
    int    vendor;
    char   header;

    if (pciLibInitStatus != OK)			/* sanity check */
        cont = FALSE;

    for (busNo = 0; cont == TRUE && busNo <= pciMaxBus; busNo++)
        for (deviceNo = 0;
	     ((cont == TRUE) && (deviceNo < PCI_MAX_DEV));
	     ++deviceNo)
            for (funcNo = 0; cont == TRUE && funcNo < PCI_MAX_FUNC; funcNo++)
		{

		/* avoid a special bus cycle */

		if ((deviceNo == 0x1f) && (funcNo == 0x07))
		    continue;

		pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID,
				 &vendor);

		/*
		 * If nonexistent device, skip to next, only look at
		 * vendor ID field for existence check
		 */

		if (((vendor & 0x0000ffff) == 0x0000FFFF) && (funcNo == 0))
		    break;
	
		pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_REVISION,
				 &classCodeReg);

		if ((((classCodeReg >> 8) & 0x00ffffff) == classCode) &&
		    (index-- == 0))
		    {
		    *pBusNo	= busNo;
		    *pDeviceNo	= deviceNo;
		    *pFuncNo	= funcNo;
		    status	= OK;
		    cont 	= FALSE;	/* terminate all loops */
		    continue;
		    }

		/* goto next if current device is single function */

		pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE, 
				 &header);
		if ((header & PCI_HEADER_MULTI_FUNC) != PCI_HEADER_MULTI_FUNC &&
		    funcNo == 0)
		    break;
		}

    return (status);
    }


/******************************************************************************
*
* pciDevConfig - configure a device on a PCI bus
*
* This routine configures a device that is on a Peripheral Component
* Interconnect (PCI) bus by writing to the configuration header of the
* selected device.
*
* It first disables the device by clearing the command register in the
* configuration header.  It then sets the I/O and/or memory space base
* address registers, the latency timer value and the cache line size.
* Finally, it re-enables the device by loading the command register with
* the specified command.
*
* NOTE: This routine is designed for Type 0 PCI Configuration Headers ONLY.
* It is NOT usable for configuring, for example, a PCI-to-PCI bridge.
*
* RETURNS: OK always.
*/

STATUS pciDevConfig
    (
    int pciBusNo,          /* PCI bus number */
    int pciDevNo,          /* PCI device number */
    int pciFuncNo,         /* PCI function number */
    UINT32 devIoBaseAdrs,  /* device IO base address */
    UINT32 devMemBaseAdrs, /* device memory base address */
    UINT32 command         /* command to issue */
    )
    {
    INT32       ix;
    UINT32      tmp32;

    /*
     * Disable device by clearing its command register field in its
     * configuration header.
     */

    pciConfigOutWord (pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_COMMAND, 0);

    /*
     *  Due to system constraints, this is a partial implementation
     *  of enabling the Base Address Registers (BARs).
     *  It is hoped that all PCI devices only require at most one
     *  I/O space and/or one memory space.
     *  If not, this code will re-allocate the device's memory to
     *  each BAR implemented.  Sounds like future trouble!
     */

    for (ix = PCI_CFG_BASE_ADDRESS_0; ix <= PCI_CFG_BASE_ADDRESS_5; ix+=4)
        {
        /* Write all f's and read back value */

        pciConfigOutLong (pciBusNo, pciDevNo, pciFuncNo, ix, 0xffffffff);
        pciConfigInLong  (pciBusNo, pciDevNo, pciFuncNo, ix, &tmp32);

        /* BAR implemented? */

        if (tmp32 == 0)
           {
           /*
            *   No! According to the spec, BARs must be implemented
            *   in sequence starting at BAR 0.  So, all done.
            */
           break;
           }

        /* I/O space requested? */

        /* Yes, set specified I/O space base address  */

        if (tmp32 & 0x1)
           {
           pciConfigOutLong (pciBusNo, pciDevNo, pciFuncNo, ix,
			     devIoBaseAdrs | 0x1);
           }

        /* No, memory space required, set specified base address */

        else
           {
           pciConfigOutLong (pciBusNo, pciDevNo, pciFuncNo, ix,
			     devMemBaseAdrs & ~0x1);
           }
        }

    /* Configure Cache Line Size Register */

    pciConfigOutByte (pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_CACHE_LINE_SIZE,
		      PCI_CACHE_LINE_SIZE);

    /* Configure Latency Timer */

    pciConfigOutByte (pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_LATENCY_TIMER,
		      PCI_LATENCY_TIMER);

    /* Enable the device's capabilities as specified */

    pciConfigOutWord (pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_COMMAND,
  		      (UINT16)command);

    return (OK);
    }


/*******************************************************************************
*
* pciConfigBdfPack - pack parameters for the Configuration Address Register
*
* This routine packs three parameters into one integer for accessing the
* Configuration Address Register
*
* RETURNS: packed integer
*
*/

int pciConfigBdfPack
    (
    int	busNo,    /* bus number */
    int	deviceNo, /* device number */
    int	funcNo    /* function number */
    )
    {
    return (((busNo    << 16) & 0x00ff0000) |
	    ((deviceNo << 11) & 0x0000f800) |
	    ((funcNo   << 8)  & 0x00000700));
    }

/*******************************************************************************
*
* pciConfigInByte - read one byte from the PCI configuration space
*
* This routine reads one byte from the PCI configuration space
*
* RETURNS: OK, or ERROR if this library is not initialized
*
*/

STATUS pciConfigInByte
    (
    int	busNo,    /* bus number */
    int	deviceNo, /* device number */
    int	funcNo,	  /* function number */
    int	offset,	  /* offset into the configuration space */
    UINT8 * pData /* data read from the offset */
    )
    {
    int	retval = 0;
    int	key;
    STATUS retStat = ERROR;

    if (pciLibInitStatus != OK)			/* sanity check */
        return (retStat);

    key = intLock ();				/* mutual exclusion start */

    pciConfigDisableMachineCheckException();

    switch (pciConfigMech)
	{
        case PCI_MECHANISM_0:
            if (frcPciConfigRead (busNo, deviceNo, funcNo, (offset & (int)(~3)),
                                &retval) == ERROR) {
                	retval = 0xffffffff;
			}
	    	else {
		    	retStat = OK;
	    	}
            retval >>= ((offset & 0x03) * 8);
            retval &= 0xff;
            break;

	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0,
		          pciConfigBdfPack (busNo, deviceNo, funcNo) |
		          (offset & 0xfc) | 0x80000000);
	    retval = PCI_IN_BYTE (pciConfigAddr1 + (offset & 0x3));
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xf0 | (funcNo << 1));
	    PCI_OUT_BYTE (pciConfigAddr1, busNo);
	    retval = PCI_IN_LONG (pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
				  (offset & 0xfc));
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    retval >>= (offset & 0x03) * 8;
	    break;

	default:
	    break;
	}

    pciConfigEnableMachineCheckException();

    intUnlock (key);				/* mutual exclusion stop */

    *pData = (char)retval;

    return (retStat);
    }

/*******************************************************************************
*
* pciConfigInWord - read one word from the PCI configuration space
*
* This routine reads one word from the PCI configuration space
*
* RETURNS: OK, or ERROR if this library is not initialized
*
*/

STATUS pciConfigInWord
    (
    int	busNo,      /* bus number */
    int	deviceNo,   /* device number */
    int	funcNo,     /* function number */
    int	offset,     /* offset into the configuration space */
    UINT16 * pData  /* data read from the offset */
    )
    {
    int	retval = 0;
    int	key;
    STATUS retStat = ERROR;

    /* check for library initialization or unaligned access */

    if ((pciLibInitStatus != OK) || ((offset & (int)0x1) > 0) )
	{
        	return (retStat);
	}

    key = intLock ();				/* mutual exclusion start */

    pciConfigDisableMachineCheckException();

    switch (pciConfigMech)
	{
        case PCI_MECHANISM_0:
            if (frcPciConfigRead (busNo, deviceNo, funcNo, (offset & (int)(~3) ),
                                &retval) == ERROR) {
                	retval = 0xffffffff;

		}
	    else {
		    retStat = OK;
	    }

            retval >>= ((offset & (int)(0x02) ) * 8);
            retval &= 0xffff;
            break;

	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0,
		          pciConfigBdfPack (busNo, deviceNo, funcNo) |
		          (offset & 0xfc) | 0x80000000);
	    retval = PCI_IN_WORD (pciConfigAddr1 + (offset & 0x2));
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xf0 | (funcNo << 1));
	    PCI_OUT_BYTE (pciConfigAddr1, busNo);
	    retval = PCI_IN_LONG (pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
				  (offset & 0xfc));
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    retval >>= (offset & 0x02) * 8;
	    break;

	default:
	    break;
	}

    pciConfigEnableMachineCheckException();

    intUnlock (key);				/* mutual exclusion stop */

    *pData = retval;

    return (retStat);
    }

/*******************************************************************************
*
* pciConfigInLong - read one longword from the PCI configuration space
*
* This routine reads one longword from the PCI configuration space
*
* RETURNS: OK, or ERROR if this library is not initialized
*
*/

STATUS pciConfigInLong
    (
    int	busNo,     /* bus number */
    int	deviceNo,  /* device number */
    int	funcNo,    /* function number */
    int	offset,    /* offset into the configuration space */
    UINT32 * pData /* data read from the offset */
    )
    {
    int		 key;
    volatile int retval = 0;
    STATUS retStat = ERROR;

    /* check for library initialization or unaligned access */

    if ((pciLibInitStatus != OK) || ((offset & (int)0x3) > 0) )
    {
      	return (retStat);
    }

    key = intLock ();				/* mutual exclusion start */

    pciConfigDisableMachineCheckException();

    switch (pciConfigMech)
	{

        case PCI_MECHANISM_0:
            if (frcPciConfigRead (busNo, deviceNo, funcNo, (offset & ~3),
                                &retval) == ERROR){
		    retval = 0xffffffff;
	    }
	    else {
		    retStat=OK;
	    }
        break;


	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0,
		          pciConfigBdfPack (busNo, deviceNo, funcNo) |
		          (offset & 0xfc) | 0x80000000);
	    retval = PCI_IN_LONG (pciConfigAddr1);
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xf0 | (funcNo << 1));
	    PCI_OUT_BYTE (pciConfigAddr1, busNo);
	    retval = PCI_IN_LONG (pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
				  (offset & 0xfc));
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    break;

	default:
	    break;
	}

    pciConfigEnableMachineCheckException();

    intUnlock (key);				/* mutual exclusion stop */

    *pData = retval;

    return (retStat);
    }

/*******************************************************************************
*
* pciConfigOutByte - write one byte to the PCI configuration space
*
* This routine writes one byte to the PCI configuration space.
*
* RETURNS: OK, or ERROR if this library is not initialized
*
*/

STATUS pciConfigOutByte
    (
    int	busNo,    /* bus number */
    int	deviceNo, /* device number */
    int	funcNo,   /* function number */
    int	offset,   /* offset into the configuration space */
    UINT8 data    /* data written to the offset */
    )
    {
    UINT32	retval;
    UINT32	ldata;
    int		mask	= 0x000000ff;
    int		key;
    STATUS retStat = ERROR;

    if (pciLibInitStatus != OK)			/* sanity check */
    	return (retStat);

    key = intLock ();				/* mutual exclusion start */

    pciConfigDisableMachineCheckException();

    switch (pciConfigMech)
    {
        case PCI_MECHANISM_0:

            if (frcPciConfigRead (busNo, deviceNo, funcNo, (offset & (int)(~3) ),&retval) == ERROR){

                retval = 0xffffffff;
	    	}
	    	else {
		    	retStat = OK;
	            ldata = (((UINT32)data) & mask) << ((offset & (int)0x03) * 8);
        	    mask <<= ((offset & 0x03) * 8);
	            retval = (retval & ~mask) | ldata;
        	    frcPciConfigWrite (busNo, deviceNo, funcNo,(offset & (int)(~3)), retval);
	    	}
            break;
	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0,
		          pciConfigBdfPack (busNo, deviceNo, funcNo) |
		          (offset & 0xfc) | 0x80000000);
	    PCI_OUT_BYTE ((pciConfigAddr1 + (offset & 0x3)), data);
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xf0 | (funcNo << 1));
	    PCI_OUT_BYTE (pciConfigAddr1, busNo);
	    retval = PCI_IN_LONG (pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
				  (offset & 0xfc));
	    data = (data & mask) << ((offset & 0x03) * 8);
	    mask <<= (offset & 0x03) * 8;
	    retval = (retval & ~mask) | data;
	    PCI_OUT_LONG ((pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
			  (offset & 0xfc)), retval);
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    break;

	default:
	    break;
	}

    pciConfigEnableMachineCheckException();

    intUnlock (key);				/* mutual exclusion stop */

    return (retStat);
    }

/*******************************************************************************
*
* pciConfigOutWord - write one 16-bit word to the PCI configuration space
*
* This routine writes one 16-bit word to the PCI configuration space.
*
* RETURNS: OK, or ERROR if this library is not initialized
*
*/

STATUS pciConfigOutWord
    (
    int	busNo,    /* bus number */
    int	deviceNo, /* device number */
    int	funcNo,   /* function number */
    int	offset,   /* offset into the configuration space */
    UINT16 data   /* data written to the offset */
    )
    {
    UINT32	retval;
    UINT32	ldata;
    int		mask	= 0x0000ffff;
    int		key;
    STATUS retStat = ERROR;

    /* check for library initialization or unaligned access */

    if ((pciLibInitStatus != OK) || ((offset & (int)0x1) > 0) )
    {
    	return (retStat);
    }
    key = intLock ();				/* mutual exclusion start */

    pciConfigDisableMachineCheckException();

    switch (pciConfigMech)
    {

        case PCI_MECHANISM_0:
            if (frcPciConfigRead (busNo, deviceNo, funcNo, (offset & (int)(~3) ),
                                &retval) == ERROR){
                retval = 0xffffffff;
	    	}
	    	else {
				retStat = OK;
            	ldata = (((UINT32)data) & mask) << ((offset & (int)0x02) * 8);
	        	mask <<= ((offset & (int)0x02) * 8);
        		retval = (retval & ~mask) | ldata;
            	frcPciConfigWrite (busNo, deviceNo, funcNo, (offset & (int)(~3) ), retval);
	    	}
            break;
	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0, pciConfigBdfPack (busNo, deviceNo, funcNo) |
		          (offset & 0xfc) | 0x80000000);
	    PCI_OUT_WORD ((pciConfigAddr1 + (offset & 0x2)), data);
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xf0 | (funcNo << 1));
	    PCI_OUT_BYTE (pciConfigAddr1, busNo);
	    retval = PCI_IN_LONG (pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
				  (offset & 0xfc));
	    data = (data & mask) << ((offset & 0x02) * 8);
	    mask <<= (offset & 0x02) * 8;
	    retval = (retval & ~mask) | data;
	    PCI_OUT_LONG ((pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
			  (offset & 0xfc)), retval);
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    break;

	default:
	    break;
	}

    pciConfigEnableMachineCheckException();

    intUnlock (key);				/* mutual exclusion stop */

    return (retStat);
    }

/*******************************************************************************
*
* pciConfigOutLong - write one longword to the PCI configuration space
*
* This routine writes one longword to the PCI configuration space.
*
* RETURNS: OK, or ERROR if this library is not initialized
*
*/

STATUS pciConfigOutLong
    (
    int	busNo,    /* bus number */
    int	deviceNo, /* device number */
    int	funcNo,   /* function number */
    int	offset,   /* offset into the configuration space */
    UINT32 data   /* data written to the offset */
    )
    {
    int key;
    UINT32 temp =0;
    STATUS retStat = ERROR;

    /* check for library initialization or unaligned access */

    if ((pciLibInitStatus != OK) || ((offset & (int)0x3) > 0) )
    {
    	return (retStat);
    }

    key = intLock ();				/* mutual exclusion start */

    pciConfigDisableMachineCheckException();

    switch (pciConfigMech)
	{
        case PCI_MECHANISM_0:
		if(frcPciConfigRead(busNo,deviceNo,funcNo,(offset & (int) (~3) ), &temp) == OK)
		{
			retStat = OK;
           	frcPciConfigWrite (busNo, deviceNo, funcNo, (offset & (int)(~3)), data);
	    }
		else
		{
			temp = 0xFFFFFFFF;
		}
        break;

	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0, pciConfigBdfPack (busNo, deviceNo, funcNo) |
		          (offset & 0xfc) | 0x80000000);
	    PCI_OUT_LONG (pciConfigAddr1, data);
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xf0 | (funcNo << 1));
	    PCI_OUT_BYTE (pciConfigAddr1, busNo);
	    PCI_OUT_LONG ((pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
			  (offset & 0xfc)), data);
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    break;

	default:
	    break;
	}

    pciConfigEnableMachineCheckException();

    intUnlock (key);				/* mutual exclusion stop */

    return (retStat);
    }
/*****************************************************************************
*
* pciConfigModifyLong - Perform a masked longword register update
*
* This function writes a field into a PCI configuration header without
* altering any bits not present in the field.  It does this by first
* doing a PCI configuration read (into a temporary location) of the PCI
* configuration header word which contains the field to be altered.
* It then alters the bits in the temporary location to match the desired
* value of the field.  It then * writes back the temporary location with
* a configuration write.  All configuration accesses are long and the
* field to alter is specified * by the "1" bits in the 'bitMask' parameter.
*
* RETURNS: OK if operation succeeds.
*          ERROR if operation fails.
*/

STATUS pciConfigModifyLong
    (
    int busNo,          /* bus number */
    int deviceNo,       /* device number */
    int funcNo,         /* function number */
    int offset,         /* offset into the configuration space */
    UINT32 bitMask,     /* Mask which defines field to alter */
    UINT32 data         /* data written to the offset */
    )
    {
    UINT32 temp;
    STATUS stat;
    int key;

    /* check for library initialization or unaligned access */

    if ((pciLibInitStatus != OK) || ((offset & (int)0x3) > 0) )
	{
        return (ERROR);
	}

 
    key = intLock ();

    stat = pciConfigInLong (busNo, deviceNo, funcNo, offset, &temp);

    if (stat == OK)
	{
	temp = (temp & ~bitMask) | (data & bitMask);
	stat = pciConfigOutLong (busNo, deviceNo, funcNo, offset, temp);
	}

    intUnlock (key);

    return (stat);

    }




/*****************************************************************************
*
* pciConfigModifyWord - Perform a masked longword register update
*
* This function writes a field into a PCI configuration header without
* altering any bits not present in the field.  It does this by first
* doing a PCI configuration read (into a temporary location) of the PCI
* configuration header word which contains the field to be altered.
* It then alters the bits in the temporary location to match the desired
* value of the field.  It then * writes back the temporary location with
* a configuration write.  All configuration accesses are long and the
* field to alter is specified * by the "1" bits in the 'bitMask' parameter.
*
* RETURNS: OK if operation succeeds.
*          ERROR if operation fails.
*/

STATUS pciConfigModifyWord
    (
    int busNo,          /* bus number */
    int deviceNo,       /* device number */
    int funcNo,         /* function number */
    int offset,         /* offset into the configuration space */
    UINT16 bitMask,     /* Mask which defines field to alter */
    UINT16 data         /* data written to the offset */
    )
    {
    UINT16 temp;
    STATUS stat;
    int key;

    /* check for library initialization or unaligned access */

    if ((pciLibInitStatus != OK) || ((offset & (int)0x1) > 0) )
	{
        return (ERROR);
	}

 
    key = intLock ();

    stat = pciConfigInWord (busNo, deviceNo, funcNo, offset, &temp);

    if (stat == OK)
	{
	temp = (temp & ~bitMask) | (data & bitMask);
	stat = pciConfigOutWord (busNo, deviceNo, funcNo, offset, temp);
	}

    intUnlock (key);

    return (stat);

    }



/*****************************************************************************
*
* pciConfigModifyByte - Perform a masked longword register update
*
* This function writes a field into a PCI configuration header without
* altering any bits not present in the field.  It does this by first
* doing a PCI configuration read (into a temporary location) of the PCI
* configuration header word which contains the field to be altered.
* It then alters the bits in the temporary location to match the desired
* value of the field.  It then * writes back the temporary location with
* a configuration write.  All configuration accesses are long and the
* field to alter is specified * by the "1" bits in the 'bitMask' parameter.
*
* RETURNS: OK if operation succeeds.
*          ERROR if operation fails.
*/

STATUS pciConfigModifyByte
    (
    int busNo,          /* bus number */
    int deviceNo,       /* device number */
    int funcNo,         /* function number */
    int offset,         /* offset into the configuration space */
    UINT8 bitMask,      /* Mask which defines field to alter */
    UINT8 data          /* data written to the offset */
    )
    {
    UINT8 temp;
    STATUS stat;
    int key;

    /* check for library initialization or unaligned access */

    if (pciLibInitStatus != OK)
	{
        return (ERROR);
	}

 
    key = intLock ();

    stat = pciConfigInByte (busNo, deviceNo, funcNo, offset, &temp);

    if (stat == OK)
	{
	temp = (temp & ~bitMask) | (data & bitMask);
	stat = pciConfigOutByte (busNo, deviceNo, funcNo, offset, temp);
	}

    intUnlock (key);

    return (stat);

    }

/*******************************************************************************
*
* pciSpecialCycle - generate a special cycle with a message
*
* This routine generates a special cycle with a message.
*
* RETURNS: OK, or ERROR if this library is not initialized
*
*/

STATUS pciSpecialCycle
    (
    int	busNo,     /* bus number */
    UINT32 message /* data driven onto AD[31:0] */
    )
    {
    int deviceNo	= 0x0000001f;
    int funcNo		= 0x00000007;
    int key;

    if (pciLibInitStatus != OK)			/* sanity check */
        return (ERROR);

    key = intLock ();				/* mutual exclusion start */

    switch (pciConfigMech)
	{

        case PCI_MECHANISM_0:
            if (pciConfigSpcl != NULL)
                pciConfigSpcl (busNo, message);
            break;
  
	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0, 
		          pciConfigBdfPack (busNo, deviceNo, funcNo) |
		          0x80000000);
	    PCI_OUT_LONG (pciConfigAddr1, message);
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xff);
	    PCI_OUT_BYTE (pciConfigAddr1, 0x00);
	    PCI_OUT_LONG ((pciConfigAddr2 | ((deviceNo & 0x000f) << 8)),
			  message);
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    break;

	default:
	    break;
	}

    intUnlock (key);				/* mutual exclusion stop */

    return (OK);
    }
/*
void sysOutByte (ULONG address, UINT8 data)
{
	*((UINT8*)(address)) = data;
}

UINT8 sysInByte (ULONG address)
{
   return *((UINT8*)(address));
}
*/
#endif /* defined(INCLUDE_PCI) */
