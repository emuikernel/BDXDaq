/* dec21x40End.c - END-style DEC 21x40 PCI Ethernet network interface driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
03r,30may02,sru  Add run-time control of byte-swapping for PCI master
                 operations
03q,13may02,rcs  added loanBuf option to End Loan string. #SPR 73414
03p,13may02,rcs  changed pDrvCtrl->pPhyInfo->phyMaxDelay to a multiple of 
                 sysClkRateGet() & referenced pDrvCtrl->pPhyInfo->phyDelayParm 
                 to sysClkRateGet(). SPR# 77012
03o,06may02,rcs  corrected behaviour and documentation for when memBase != -1
                 SPR# 75455
03n,06may02,rcs  define all DRV_DEBUG flags and default DRV_LOG to display 
                 DRV_DEBUG_LOAD conditions. SPR# 75457   
03m,22apr02,rcs  added init of  pDrvCtrl->txPollSendBuf to dec21x40InitMem() 
                 and changed dec21x40PollSend() to usepDrvCtrl->txPollSendBuf. 
                 SPR# 76102
03l,25mar02,pmr  fixed usage of rDesc3 (SPR 73470, 34891).
03k,18jan02,rcs  facilitated common source base with either RFC 1213 or 
                 RFC 2233 depending on INCLUDE_RFC_1213 macro.
03j,14jan02,dat  Removing warnings from Diab compiler
03i,20jul01,rcs  used LE_BYTE_SWAP in dec21140EnetAddrGet() to return correct
                 enetaddr for little endian archs (SPR# 69093)
03h,28aug01,ann  adding the code that copies the pMib2Tbl contents into
                 mib2Tbl for backward compatibility - SPR 69697 & 69758
03g,16aug01,ann  providing the fix for SPR 69673
03f,13jul01,ann  eliminating the mibStyle variable and accomodating the
                 END_MIB_2233 flag in the END flags field
03e,15jun01,rcs  set dec21x40Start() taskDelay to sysClkRateGet() / 30
03d,29mar01,spm  merged changes from version 02h of tor2_0.open_stack
                 branch (wpwr VOB, base 02d) for unified code base
03c,23feb01,rcs  increased taskDelay to 2 in dec21x40Start() to ensure adequate
                 time for the software reset of the dec21x40 device to complete.
                 SPR# 64192
03b,14feb01,rcs  moved intConnect to before the chip's interupts are enabled
                 SPR# 31314.
03a,13feb01,rcs  saved and restored CSR0 before and after software reset in 
                 dec21x40Start() SPR# 64192
02z,08feb01,rcs  moved decode non-register user flags to before 
                 dec21x40EnetAddrGet() call. SPR# 63881   
02y,14dec00,rcs  removed reference to sysDelay()
02x,15nov00,jpd  added _func_dec2114xIntAck (SPR #62330).
02w,25oct00,dat  SPR 34891 removing PHYS_TO_VIRT translations
02v,20sep00,dat  SPR 32058, allow for alternate intConnect rtn
02u,17jul00,bri  Standardisation
02t,17apr00,an   Added support to force HomePNA speed and power via user flags.
02s,13apr00,an   Added support for special dec21145 media change technique.
                 The driver must be called with DEC_USR_HPNA_PREFER_10BT 
		 user flag set to select this mode. This is NOT the default 
		 mode of operation.
02r,18feb00,an   Added support for dec21145. See explaination below.
02q,11jun00,ham  removed reference to etherLib.
02p,08may00,jgn  SPR 31198, Various fixes for use on ARM + Kingston KNE100TX 
                 (MII) card
02o,24apr00,dmw  Fixed extended format block type 2 selection.
02n,24apr00,dmw  Added Full Duplex support for adapters with PHY's.
02m,24mar00,dat  fixed string handling, added fix to reset code
02l,01mar00,stv  fixed backward compatibility issues for the alignment fix.Also
                 corrected documentation & updated a debug message. 
02k,24feb00,stv  made a selective merge to add alignment fix.
02j,27jan00,dat  fixed use of NULL
02i,08nov99,cn   integrated with miiLib. Also fixed SPR #21116, #28315.
02h,01oct99,pfl  Fix for spr 22196 was missing, added it again.
02g,16sep99,stv  Fixed errors due to usage of DRV_LOG macro.
02f,01sep99,dat  SPR 28573 and Debug clean up.
02e,17aug99,dmw  Added MII/Phy force modes.
02d,11mar99,tm   Added MII/Phy r/w for 2114x link status check (SPR 22196)
02c,02mar99,tm   txRingClean in ISR to improve small pkt throughput (SPR 23950)
02b,01feb99,scb  Added support for dec21143. (SPR 22887)
02a,30sep98,dbt  Adding missing code in poll receive routine.
01z,22sep98,dat  SPR 22325, system mode transition + lint fixes.
01y,07aug98,cn   updated documentation about user flags.
01w,31jul98,cn   removed setting CSR6_FD in xxxMediaSelect [SPR# 21683].
		 Also moved some indeces setting in xxxStart [SPR#21557].
01v,20apr98,map  preserve CSR6_PS in xxxChipReset [SPR# 21013]
01u,10dec97,kbw  man page fiddlings
01t,08dec97,gnn  END code review fixes.
01s,20nov97,gnn  fixed spr#9555, configurable TDS and RDS.
01r,19oct97,vin	 moved swapping of loaned buffer before END_RCV_RTN_CALL
01q,17oct97,vin  removed extra free.
01p,09oct97,vin  delete unwanted frees in PollSend routine
01o,07oct97,vin  fixed multicasting, MIB MTU size to ETHER_MTU
01n,25sep97,gnn  SENS beta feedback fixes
01m,24sep97,vin  added clBlk related calls
01l,02sep97,gnn  fixed a race condition in the txRestart handling.
01k,25aug97,vin  adjusted cluster mem size.
01j,22aug97,vin  fixed initialization of cluster mem area.
01i,22aug97,gnn  fixed a bug in polled mode.
01h,21aug97,vin  added changes for newBuffering API
01g,12aug97,gnn  changes necessitated by MUX/END update.
01f,08aug97,map  fixed muxBufInit for user-specified bufpool.
                 added ilevel arg to sysLanIntDisable().
                 update collision statistics.
                 added more flags to userFlags.
01e,10jun97,map  renamed funcDec21x40MediaSelect to _func_dec21x40MediaSelect.
01d,28may97,kbw  general text edit
            map  added DEC SROM support for media configuration.
01c,23apr97,map  cleaned up; replaced SWAP_SHORT with PCISWAP_SHORT.
01b,16apr97,map  added support for dec21140.
01a,07apr97,map  modified if_dc.c to END style.
*/

/*
This module implements a DEC 21x40 PCI Ethernet network interface driver and
supports both the 21040, 21140, 21143, 21145 versions of the chip.

The DEC 21x40 PCI Ethernet controller is little endian because it interfaces
with a little-endian PCI bus.  Although PCI configuration for a device is
handled in the BSP, all other device programming and initialization needs 
are handled in this module.

This driver is designed to be moderately generic. Without modification, it can
operate across the full range of architectures and targets supported by VxWorks.
To achieve this, the driver requires a few external support routines as well
as several target-specific parameters.  These parameters, and the mechanisms
used to communicate them to the driver, are detailed below.  If any of the
assumptions stated below are not true for your particular hardware, you need
to modify the driver before it can operate correctly on your hardware.

On the 21040, the driver configures the 10BASE-T interface by default,waits for
two seconds, and checks the status of the link. If the link status indicates
failure, AUI interface is configured.

On other versions of the 21x40 family, the driver reads media information from
a DEC serial ROM and configures the media.  To configure the media on targets 
that do not support a DEC format serial ROM, the driver calls the 
target-specific media-select routine referenced in
the `_func_dec21x40MediaSelect' hook. 

The 21145 supports HomePNA 1.0 (Home Phone Line) Networking as well as 
10Base-T. The HomePNA port can be forced to 1 MB/sec or 0.7 MB/sec mode
via the DEC_USR_HPNA_FORCE_FAST and DEC_USR_HPNA_FORCE_SLOW user flags,
respectively. If these flags are not set then the speed is set using
the SROM settings.
Unlike the Ethernet phys, the HomePNA phy can not determine link failure and
therefore will never notify the driver when the HomePNA port is 
disconnected. However, to allow media change, the driver can be configured 
to ALWAYS perfer 10Base-T over HomePNA by interrupting on 10Base-T link pass 
interrupt. Upon 10Base-T link failure, the driver will revert back to HomePNA.
Since this method violates the preference rules outlined in Intel/DEC SROM 
format spec, this is not the default mode of operation. 
The driver must be started with DEC_USR_HPNA_PREFER_10BT user flag set to 
set the driver into this mode.

The driver supports big-endian or little-endian architectures (as a 
configurable option).  The driver also and contains error recovery code 
that handles known device errata related to DMA activity.

Big-endian processors can be connected to the PCI bus through some controllers
that take care of hardware byte swapping. In such cases, all the registers
which the chip DMAs to have to be swapped and written to, so that when the
hardware swaps the accesses, the chip would see them correctly. The chip still
has to be programmed to operate in little endian mode as it is on the PCI
bus.  If the cpu board hardware automatically swaps all the accesses to and
from the PCI bus, then input and output byte stream need not be swapped.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
The driver provides one standard external interface, dec21x40EndLoad().  As 
input, this function expects a string of colon-separated parameters. The 
parameters should be specified as hexadecimal strings (optionally preceded 
by "0x" or a minus sign "-").  Although the parameter string is parsed 
using endTok_r(), each parameter is converted from string to binary by a 
call to:
.CS
    strtoul(parameter, NULL, 16).
.CE

The format of the parameter string is:

 "<deviceAddr>:<pciAddr>:<iVec>:<iLevel>:<numRds>:<numTds>:\
<memBase>:<memSize>:<userFlags>:<phyAddr>:<pPhyTbl>:<phyFlags>:<offset>:\
<loanBufs>:<drvFlags>"

TARGET-SPECIFIC PARAMETERS
.IP <deviceAddr>
This is the base address at which the hardware device registers are located.
.IP <pciAddr>
This parameter defines the main memory address over the PCI bus. It is used to
translate a physical memory address into a PCI-accessible address.
.IP <iVec>
This is the interrupt vector number of the hardware interrupt generated by
this Ethernet device.  The driver uses intConnect() to attach an interrupt
handler for this interrupt. The BSP can change this by modifying the global
pointer dec21x40IntConnectRtn with the desired routines (usually pciIntConnect).
.IP <iLevel>
This parameter defines the level of the hardware interrupt.
.IP <numRds>
The number of receive descriptors to use.  This controls how much data
the device can absorb under load.  If this is specified as NONE (-1), 
the default of 32 is used.
.IP <numTds>
The number of transmit descriptors to use.  This controls how much data
the device can absorb under load.  If this is specified as NONE (-1) then
the default of 64 is used.
.IP <memBase>
This parameter specifies the base address of a DMA-able cache-free
pre-allocated memory region for use as a memory pool for transmit/receive
descriptors and buffers including loaner buffers.  If there is no pre-allocated
memory available for the driver, this parameter should be -1 (NONE). In which 
case, the driver allocates cache safe memory for its use using cacheDmaAlloc().
.IP <memSize>
The memory size parameter specifies the size of the pre-allocated memory
region. If memory base is specified as NONE (-1), the driver ignores this
parameter.  When specified this value must account for transmit/receive
descriptors and buffers and loaner buffers 
.IP <userFlags>
User flags control the run-time characteristics of the Ethernet
chip.  Most flags specify non default CSR0 and CSR6 bit values. Refer to
dec21x40End.h for the bit values of the flags and to the device hardware
reference manual for details about device capabilities, CSR6 and CSR0.
.IP <phyAddr>
This optional parameter specifies the address on the MII (Media Independent
Interface) bus of a MII-compliant PHY (Physical Layer Entity). The module 
that is responsible for optimally configuring the media layer will start 
scanning the MII bus from the address in <phyAddr>. It will retrieve the 
PHY's address regardless of that, but, since the MII management interface, 
through which the PHY is configured, is a very slow one, providing an 
incorrect or invalid address may result in a particularly long boot process.
If the flag <DEC_USR_MII> is not set, this parameter is ignored.
.IP <pPhyTbl>
This optional parameter specifies the address of a auto-negotiation table
for the PHY being used. 
The user only needs to provide a valid value for this parameter
if he wants to affect the order how different technology abilities are 
negotiated.
If the flag <DEC_USR_MII> is not set, this parameter is ignored.
.IP <phyFlags>
This optional parameter allows the user to affect the PHY's configuration
and behaviour. See below, for an explanation of each MII flag.
If the flag <DEC_USR_MII> is not set, this parameter is ignored.
.IP <offset>
This parameter defines the offset which is used to solve alignment problem.
.IP <loanBufs>
This optional parameter allows the user to select the amount of loaner buffers
allocated for the driver's net pool to be loaned to the stack in receive 
operations.  The default number of loaner buffers is 32. The number of loaner
buffers must be accounted for when calculating the memory size specified by
memSize. 
.IP <drvFlags>
This optional parameter allows the user to enable driver-specific features.

Device Type: Although the default device type is DEC 21040, specifying 
the DEC_USR_21140 flag bit turns on DEC 21140 functionality.

Ethernet Address: The Ethernet address is retrieved from standard serial 
ROM on both DEC 21040, and DEC 21140 devices. If the retrieve from ROM fails, 
the driver calls the sysDec21x40EnetAddrGet() BSP routine. 
Specifying DEC_USR_XEA flag bit tells the driver should, by default, 
retrieve the Ethernet address using the sysDec21x40EnetAddrGet() BSP routine.

Priority RX processing: The driver programs the chip to process the 
transmit and receive queues at the same priority. By specifying DEC_USR_BAR_RX,
the device is programmed to process receives at a higher priority.

TX poll rate: By default, the driver sets the Ethernet chip into a 
non-polling mode. In this mode, if the transmit engine is idle, it is 
kick-started every time a packet needs to be transmitted.  Alternatively, 
the chip can be programmed to poll for the next available transmit 
descriptor if the transmit engine is in idle state. The poll rate is 
specified by one of DEC_USR_TAP_<xxx> flags.

Cache Alignment: The DEC_USR_CAL_<xxx> flags specify the address 
boundaries for data burst transfers.

DMA burst length: The DEC_USR_PBL_<xxx> flags specify the maximum 
number of long words in a DMA burst.

PCI multiple read: The DEC_USR_RML flag specifies that a device 
supports PCI memory-read-multiple.

Full Duplex Mode: When set, the DEC_USR_FD flag allows the device to 
work in full duplex mode, as long as the PHY used has this
capability. It is worth noting here that in this operation mode, 
the dec21x40 chip ignores the Collision and the Carrier Sense signals.

MII interface: some boards feature an MII-compliant Physical Layer Entity
(PHY). In this case, and if the flag <DEC_USR_MII> is set, then the optional
fields <phyAddr>, <pPhyTbl>, and <phyFlags> may be used to affect the
PHY's configuration on the network. 

10Base-T Mode: when the flag <DEC_USR_MII_10MB> is set, then the PHY will
negotiate this technology ability, if present.

100Base-T Mode: when the flag <DEC_USR_MII_100MB> is set, then the PHY will
negotiate this technology ability, if present.

Half duplex Mode: when the flag <DEC_USR_MII_HD> is set, then the PHY will
negotiate this technology ability, if present.

Full duplex Mode: when the flag <DEC_USR_MII_FD> is set, then the PHY will
negotiate this technology ability, if present.

Auto-negotiation: the driver's default behaviour is to enable auto-negotiation,
as defined in "IEEE 802.3u Standard". However, the user may disable this
feature by setting the flag <DEC_USR_MII_NO_AN> in the <phyFlags> field of
the load string.

Auto-negotiation table: the driver's default behaviour is to enable the
standard auto-negotiation process, as defined in "IEEE 802.3u Standard". 
However, the user may wish to force the PHY to negotiate its technology
abilities a subset at a time, and according to a particular order. The
flag <DEC_USR_MII_AN_TBL> in the <phyFlags> field may be used to tell the
driver that the PHY should negotiate its abilities as dictated by the
entries in the <pPhyTbl> of the load string.
If the flag <DEC_USR_MII_NO_AN> is set, this parameter is ignored.

Link monitoring: this feature enables the netTask to periodically monitor
the PHY's link status for link down events. If any such event occurs, and
if the flag <DEC_USR_MII_BUS_MON> is set, then a driver's optionally provided
routine is executed, and the link is renegotiated.

Transmit treshold value: The DEC_USR_THR_XXX flags enable the user to 
choose among different threshold values for the transmit FIFO. 
Transmission starts when the frame size within the transmit FIFO is 
larger than the treshold value. This should be selected taking into 
account the actual operating speed of the PHY.  Again, see the device 
hardware reference manual for details.

EXTERNAL SUPPORT REQUIREMENTS
This driver requires three external support functions and provides a hook
function:

.IP sysLanIntEnable()
.CS
    void sysLanIntEnable (int level)
.CE
This routine provides a target-specific interface for enabling Ethernet device
interrupts at a specified interrupt level.
.IP sysLanIntDisable()
.CS
    void sysLanIntDisable (void)
.CE
This routine provides a target-specific interface for disabling Ethernet device
interrupts.
.IP sysDec21x40EnetAddrGet()
.CS
    STATUS sysDec21x40EnetAddrGet (int unit, char *enetAdrs)
.CE
This routine provides a target-specific interface for accessing a device
Ethernet address.
.IP `_func_dec21x40MediaSelect'
.CS
    FUNCPTR _func_dec21x40MediaSelect
.CE
If `_func_dec21x40MediaSelect' is NULL, this driver provides a default 
media-select routine that reads and sets up physical media using the 
configuration information from a Version 3 DEC Serial ROM. Any other media
configuration can be supported by initializing `_func_dec21x40MediaSelect',
typically in sysHwInit(), to a target-specific media select routine.

A media select routine is typically defined as:
.ne 8
.CS
   STATUS decMediaSelect
       (
       DEC21X40_DRV_CTRL *	pDrvCtrl,   /@ driver control @/
       UINT *			pCsr6Val    /@ CSR6 return value @/
       )
       {
           ...
       }
.CE

The <pDrvCtrl> parameter is a pointer to the driver control structure that this
routine can use to access the Ethenet device. The driver control structure
member 'mediaCount', is initialized to 0xff at startup, while the other media
control members ('mediaDefault', 'mediaCurrent', and 'gprModeVal') are 
initialized to zero. This routine can use these fields in any manner. 
However, all other driver control structure members should be considered 
read-only and should not be modified.

This routine should reset, initialize, and select an appropriate media.  It
should also write necessary the CSR6 bits (port select, PCS, SCR, and full 
duplex) to the memory location pointed to by <pCsr6Val>. The driver uses 
this value to program register CSR6. This routine should return OK or ERROR.

.iP "VOIDFUNCPTR _func_dec2114xIntAck" "" 9 -1

This driver does acknowledge the LAN interrupts. However if the board hardware
requires specific interrupt acknowledgement, not provided by this driver,
the BSP should define such a routine and attach it to the driver via
_func_dec2114xIntAck.

PCI ID VALUES
The dec21xxx series chips are now owned and manufactured by Intel.  Chips may
be identified by either PCI Vendor ID.  ID value 0x1011 for Digital, or
ID value 0x8086 for Intel.  Check the Intel web site for latest information.
The information listed below may be out of date.

.CS
    Chip         Vendor ID      Device ID
    dec 21040      0x1011        0x0002
    dec 21041      0x1011        0x0014
    dec 21140      0x1011        0x0009
    dec 21143      0x1011        0x0019
    dec 21145      0x8086        0x0039
.CE

SEE ALSO: ifLib, 
.I "DECchip 21040 Ethernet LAN Controller for PCI,"
.I "Digital Semiconductor 21140A PCI Fast Ethernet LAN Controller,"
.I "Using the Digital Semiconductor 21140A with Boot ROM, Serial ROM, \
and External Register: An Application Note"
.I "Intel 21145 Phoneline/Ethernet LAN Controller Hardware Ref. Manual"
.I "Intel 21145 Phoneline/Ethernet LAN Controller Specification Update"

INTERNAL
We modified the driver to use rDesc3 to hold the virtual address of the buffer.
This allows us to eliminate all PHYS_TO_VIRT translations that were previously
used.  PHYS_TO_VIRT translations are extremely costly in T3.0.

*/

/* includes */

#include "vxWorks.h"
#include "wdLib.h"
#include "iv.h"
#include "vme.h"
#include "net/mbuf.h"
#include "net/unixLib.h"
#include "net/protosw.h"
#include "sys/socket.h"
#include "sys/ioctl.h"
#include "errno.h"
#include "memLib.h"
#include "intLib.h"
#include "net/route.h"
#include "errnoLib.h"
#include "cacheLib.h"
#include "logLib.h"
#include "netLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysLib.h"
#include "net/systm.h"
#include "sys/times.h"
#include "net/if_subr.h"
#include "netinet/if_ether.h"
#include "miiLib.h"
#include "drv/end/dec21x40End.h"

#undef	ETHER_MAP_IP_MULTICAST
#include "etherMultiLib.h"
#include "end.h"
#include "endLib.h"

#include "lstLib.h"
#include "semLib.h"

/* defines */
#define DRV_DEBUG_OFF		0x0000
#define DRV_DEBUG_RX		0x0001
#define	DRV_DEBUG_TX		0x0002
#define DRV_DEBUG_INT		0x0004
#define	DRV_DEBUG_POLL		(DRV_DEBUG_POLL_RX | DRV_DEBUG_POLL_TX)
#define	DRV_DEBUG_POLL_RX	0x0008
#define	DRV_DEBUG_POLL_TX	0x0010
#define	DRV_DEBUG_IOCTL		0x0040
#define	DRV_DEBUG_LOAD		0x0020
#define DRV_DEBUG_INIT		0x0100
#define DRV_DEBUG_MII		0x0080
#define DRV_DEBUG_POLL_REDIR	0x10000
#define	DRV_DEBUG_LOG_NVRAM	0x20000
#define DRV_DEBUG_ALL		0xffffffff 

#undef  DRV_DEBUG

#ifdef	DRV_DEBUG
int     decTxInts=0;
int     decRxInts=0;
int	decTxErrors = 0;
int	decRxErrors = 0;
int	decTxTpsErrors = 0;
int	decTxBufErrors = 0;
int	decTxTjtErrors = 0;
int	decTxUnfErrors = 0;
int	decTxLcErrors = 0;
int	decRxBufErrors = 0;
int	decRxRpsErrors = 0;
int	decRxWtErrors = 0;
int	decDescErrors = 0;

IMPORT	int dataDump();
IMPORT	int eAdrsDisplay();

#endif /*DRV_DEBUG*/

/* default to enable DRV_DEBUG_LOAD to display initialization errors. */

int	decDebug = DRV_DEBUG_INIT;

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)                        \
	if (decDebug & FLG)                                             \
            logMsg(X0, X1, X2, X3, X4, X5, X6);

#define END_FLAGS_ISSET(pEnd, setBits)                                  \
            ((pEnd)->flags & (setBits))

 
#ifdef INCLUDE_RFC_1213

/* Old RFC 1213 mib2 interface */

#define END_HADDR(pEnd)                                                 \
                ((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd)                                             \
                ((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

#else  /* Then RFC 2233 */

/* New RFC 2233  mib2 interface */

#define END_HADDR(pEnd)                                                 \
                ((pEnd)->pMib2Tbl->m2Data.mibIfTbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd)                                             \
                ((pEnd)->pMib2Tbl->m2Data.mibIfTbl.ifPhysAddress.addrLength)

#endif /* INCLUDE_RFC_1213 */

#define DEC_SPEED_1	1000000              /* 1 Mbps HomePNA */
#define DEC_SPEED_10	10000000             /* 10 Mbps */
#define DEC_SPEED_100	100000000            /* 100 Mbps */
#define	DEC_SPEED_DEF	DEC_SPEED_10
#define EADDR_LEN	6                    /* ethernet address length */
#define DEC_PKT_SIZE	(ETHERMTU + SIZEOF_ETHERHEADER + EADDR_LEN)

#define	DEC_LOOPS_PER_NS	4
#define DEC_NSDELAY(nsec) do {						\
			volatile int nx = 0;                            \
			volatile int loop = ((nsec)*dec21x40Loops);	\
			for (nx = 0; nx < loop; nx++)			\
				;                  			\
			} while (0)

#define DEC_BUF_ALLOC_ROUTINE                                           \
	(FUNCPTR) (DRV_FLAGS_ISSET(DEC_MEMOWN) ?                        \
                   cacheDmaMalloc : NULL)

#define DEC_BUF_POOL_ADRS                                               \
	(char *)(DRV_FLAGS_ISSET(DEC_MEMOWN) ?                          \
                  NULL:(pDrvCtrl->txRing + pDrvCtrl->numTds))

/* DRV_CTRL flags access macros */
#define DRV_FLAGS_SET(setBits)                                          \
	(pDrvCtrl->flags |= (setBits))

#define DRV_FLAGS_ISSET(setBits)                                        \
	(pDrvCtrl->flags & (setBits))

#define DRV_FLAGS_CLR(clrBits)                                          \
	(pDrvCtrl->flags &= ~(clrBits))

#define DRV_FLAGS_GET()                                                 \
        (pDrvCtrl->flags)

/* DRV_CTRL user flags access macros */
#define DRV_USR_FLAGS_SET(setBits)                                      \
	(pDrvCtrl->usrFlags |= (setBits))

#define DRV_USR_FLAGS_ISSET(setBits)                                    \
	(pDrvCtrl->usrFlags & (setBits))

#define DRV_USR_FLAGS_CLR(clrBits)                                      \
	(pDrvCtrl->usrFlags &= ~(clrBits))

#define DRV_USR_FLAGS_GET()                                             \
        (pDrvCtrl->usrFlags)

#define DRV_PHY_FLAGS_ISSET(setBits)                                    \
	(pDrvCtrl->miiPhyFlags & (setBits))

/* Cache macros */
#define DEC_CACHE_INVALIDATE(address, len)                              \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->cacheFuncs, (address), (len))

#define DEC_CACHE_VIRT_TO_PHYS(address)                                 \
        CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, (address))

#define DEC_CACHE_PHYS_TO_VIRT(address)                                 \
        CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->cacheFuncs, (address))

/* memory to PCI address translation macros */
#define PCI_TO_MEM_PHYS(pciAdrs)                                        \
	(((ULONG)(pciAdrs)) - (pDrvCtrl->pciMemBase))

#define DEC_PCI_TO_VIRT(pciAdrs)                                        \
        DEC_CACHE_PHYS_TO_VIRT (PCI_TO_MEM_PHYS (pciAdrs))
	
#define MEM_TO_PCI_PHYS(memAdrs)                                        \
	(((ULONG)(memAdrs)) + (pDrvCtrl->pciMemBase))

#define DEC_VIRT_TO_PCI(vAdrs)                                          \
	MEM_TO_PCI_PHYS (DEC_CACHE_VIRT_TO_PHYS (vAdrs))

#ifndef TCP_MSS
#define TCP_MSS		536		/* TCP maximum segment size */
#endif
#define RWIN		(TCP_MSS * 4)	/* Receive window size */

/*
 * CSR access macros
 *
 * To optimize CSR accesses, redefine DEC_CSR_READ and
 * DEC_CSR_WRITE macros in a wrapper file.
 */
#ifndef DEC_CSR_READ
#define DEC_CSR_READ(csr)                                               \
	dec21x40CsrRead(pDrvCtrl, (csr))
#endif /* DEC_CSR_READ */

#ifndef DEC_CSR_WRITE
#define DEC_CSR_WRITE(csr,val)                                          \
	dec21x40CsrWrite(pDrvCtrl, (csr), (val))
#endif /* DEC_CSR_WRITE */

#define DEC_CSR_UPDATE(csr,val)                                         \
	DEC_CSR_WRITE((csr), DEC_CSR_READ(csr) | (val))
    
#define DEC_CSR_RESET(csr,val)                                          \
	DEC_CSR_WRITE((csr), DEC_CSR_READ(csr) & ~(val))
    
#define DEC_SROM_CMD_WRITE(adrs,delay)                                  \
	do {                                                               \
        DEC_CSR_WRITE(CSR9, CSR9_21140_SR | CSR9_21140_WR | (adrs));    \
        DEC_NSDELAY (delay);						\
        } while (0)

#define DEC_SROM_CMD_READ()                                             \
	((DEC_CSR_READ(CSR9) & 0x8) >> 3)

/* MII read/write access macros */
   
#define CSR9_MII_WR 0x00000000

#define DEC_MII_BIT_READ(pBData)                                        \
        do {                                                               \
        DEC_CSR_WRITE (CSR9, CSR9_21140_MII | CSR9_21140_RD);           \
        DEC_NSDELAY (100);						\
        DEC_CSR_WRITE (CSR9, CSR9_21140_MII | CSR9_21140_RD | CSR9_21140_MDC);\
        DEC_NSDELAY (100);						\
        *(pBData) |= CSR9_MII_DBIT_RD (DEC_CSR_READ (CSR9));		\
        } while (0)

#define DEC_MII_BIT_WRITE(data)                                      \
        do {                                                            \
        DEC_CSR_WRITE (CSR9, CSR9_MII_DBIT_WR(data) |                \
                      CSR9_MII_WR | CSR9_21140_WR);                  \
        DEC_NSDELAY (100);                                           \
        DEC_CSR_WRITE (CSR9, CSR9_MII_DBIT_WR(data) |                \
                      CSR9_MII_WR | CSR9_21140_WR | CSR9_21140_MDC); \
        DEC_NSDELAY (100);                                           \
        } while (0)

#define DEC_MII_RTRISTATE           \
        do {                        \
        int retVal;                 \
        DEC_MII_BIT_READ (&retVal); \
        } while (0)

#define DEC_MII_WTRISTATE           \
        do {                        \
        DEC_MII_BIT_WRITE(0x1);     \
        DEC_MII_BIT_WRITE(0x0);     \
        } while (0)

#define DEC_MII_WRITE(data, bitCount)                \
        do {                                         \
        int i=(bitCount);                            \
                                                     \
        while (i--)                                  \
            DEC_MII_BIT_WRITE (((data) >> i) & 0x1); \
        } while (0)

#define DEC_MII_READ(pData, bitCount) \
        do {                          \
        int i=(bitCount);             \
                                      \
        while (i--)                   \
            {                         \
            *(pData) <<= 1;           \
            DEC_MII_BIT_READ (pData); \
            }                         \
        } while (0)

/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */
#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult)                       \
    do {                                                                \
    int vector = pDrvCtrl->ivec;                                        \
    if (!pDrvCtrl->intrConnect)                                         \
        {                                                               \
        pDrvCtrl->intrConnect = TRUE;                                   \
       *pResult = (*dec21x40IntConnectRtn) (                            \
                               (VOIDFUNCPTR *)INUM_TO_IVEC (vector),    \
                                (rtn), (int)(arg));                     \
        }                                                               \
    } while (0)
#endif /*SYS_INT_CONNECT*/
 
#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl,rtn,arg,pResult)                    \
    do {                                                                \
    /* set pDrvCtrl->intrConnect to FALSE if appropriate */             \
    *pResult = OK;							\
    } while (0)
#endif /*SYS_INT_DISCONNECT*/

#ifndef SYS_INT_ENABLE
#define SYS_INT_ENABLE(pDrvCtrl)                                        \
    do {                                                                \
    IMPORT void sysLanIntEnable();                                      \
    sysLanIntEnable (pDrvCtrl->ilevel);                                 \
    } while (0)
#endif /*SYS_INT_ENABLE*/

#ifndef SYS_INT_DISABLE
#define SYS_INT_DISABLE(pDrvCtrl)                                       \
    do {                                                                \
    IMPORT void sysLanIntDisable();                                     \
    sysLanIntDisable (pDrvCtrl->ilevel);                                \
    } while (0)
#endif /*SYS_INT_DISABLE*/

#define	END_MIB_SPEED_SET(pEndObj, speed)                               \
	((pEndObj)->mib2Tbl.ifSpeed=speed)

#define NET_BUF_ALLOC()                                                 \
    netClusterGet (pDrvCtrl->endObj.pNetPool, pDrvCtrl->clPoolId)

#define NET_BUF_FREE(pBuf)                                              \
    netClFree (pDrvCtrl->endObj.pNetPool, pBuf)

#define	NET_MBLK_ALLOC()                                                \
    mBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT, MT_DATA)

#define NET_MBLK_FREE(pMblk)                                            \
    netMblkFree (pDrvCtrl->endObj.pNetPool, (M_BLK_ID)pMblk)

#define	NET_CL_BLK_ALLOC()                                              \
    clBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT)

#define NET_CL_BLK_FREE(pClblk)                                         \
    clBlkFree (pDrvCtrl->endObj.pNetPool, (CL_BLK_ID)pClBlk)
            
#define NET_MBLK_BUF_FREE(pMblk)                                        \
    netMblkClFree ((M_BLK_ID)pMblk)

#define NET_MBLK_CHAIN_FREE(pMblk)                                      \
    do {                                                                \
    M_BLK *pNext;                                                       \
                                                                        \
    pNext=pMblk;                                                        \
    while (pNext)                                                       \
        pNext=NET_MBLK_BUF_FREE (pNext);                                \
    } while (0)
    
#define NET_MBLK_CL_JOIN(pMblk, pClBlk)                              	\
    netMblkClJoin ((pMblk), (pClBlk))

#define NET_CL_BLK_JOIN(pClBlk, pBuf, len)                             	\
    netClBlkJoin ((pClBlk), (pBuf), (len), NULL, 0, 0, 0)
        
#define	DRV_CTRL	DEC21X40_DRV_CTRL

/* structure sizes */
#define DRV_CTRL_SIZ		sizeof(DRV_CTRL)
#define RD_SIZ			sizeof(DEC_RD)
#define TD_SIZ			sizeof(DEC_TD)

/* locals */

/* Default network buffer configuration */

int	dec21x40Loops = DEC_LOOPS_PER_NS;	/* spin loops per nsec */

FUNCPTR	dec21x40IntConnectRtn = intConnect;

/* constant data to set PHY control register */

LOCAL const UINT dcForcePhyModes[] = 
    {
    MII_CR_100 | MII_CR_FDX,	/* 100FD */
    MII_CR_100,			/* 100HD */
    MII_CR_FDX,			/* 10FD  */
    0,				/* 10HD  */
    -1
    } ;


/* forward declarations */

LOCAL STATUS	dec21x40InitParse (DRV_CTRL *pDrvCtrl, char *InitString);
LOCAL STATUS	dec21x40InitMem (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	dec21x40IASetup (DRV_CTRL *pDrvCtrl);
LOCAL void 	dec21x40ModeSet (DRV_CTRL *pDrvCtrl);

LOCAL int	dec21x40HashIndex (char *eAddr);
LOCAL void 	dec21x40Int (DRV_CTRL *pDrvCtrl);
LOCAL STATUS 	dec21x40Recv (DRV_CTRL *pDrvCtrl, DEC_RD *rmd);
LOCAL STATUS 	dec21x40ChipReset (DRV_CTRL *pDrvCtrl);

LOCAL void	dec21040AuiTpInit (DRV_CTRL *pDrvCtrl);
LOCAL void 	dec21x40CsrWrite (DRV_CTRL *pDrvCtrl, int reg, ULONG value);
LOCAL ULONG 	dec21x40CsrRead (DRV_CTRL *pDrvCtrl, int reg);
LOCAL STATUS 	dec21x40EnetAddrGet (DRV_CTRL *pDrvCtrl, char *enetAdrs);

LOCAL STATUS 	dec21040EnetAddrGet (DRV_CTRL *pDrvCtrl, char *enetAdrs);
LOCAL STATUS 	dec21140EnetAddrGet (DRV_CTRL *pDrvCtrl, char *enetAdrs);
LOCAL void	dec21x40Restart (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	dec21x40MediaChange (DRV_CTRL *pDrvCtrl);

LOCAL DEC_TD *	dec21x40TxDGet (DRV_CTRL *pDrvCtrl);
LOCAL DEC_RD *	dec21x40RxDGet (DRV_CTRL *pDrvCtrl);
LOCAL void 	dec21x40TxRingClean (DRV_CTRL *pDrvCtrl);
LOCAL void 	dec21x40RxIntHandle (DRV_CTRL *pDrvCtrl);

LOCAL STATUS 	dec21140MediaSelect (DRV_CTRL *pDrvCtrl, UINT *pCsr6Val);
LOCAL STATUS 	dec21143MediaSelect (DRV_CTRL *pDrvCtrl, UINT *pCsr6Val);
LOCAL STATUS dec21145DecodeExt7InfoBlock ( DRV_CTRL *pDrvCtrl,
                                           UCHAR * pInfoBlock);
LOCAL STATUS 	dec21145MediaSelect (DRV_CTRL *pDrvCtrl, UINT *pCsr6Val);
LOCAL STATUS 	dec21x40PhyPreInit  (DRV_CTRL *pDrvCtrl, UCHAR * pInfoBlock);
LOCAL STATUS 	dec21x40MiiInit  (DRV_CTRL *pDrvCtrl, UINT *pCsr6Val,
				  UCHAR * pInfoBlock);

LOCAL STATUS dec21x40MiiRead ( DRV_CTRL *pDrvCtrl, UINT8 phyAdrs, UINT8 phyReg,
              	               UINT16 *pRetVal);
LOCAL STATUS dec21x40MiiWrite (DRV_CTRL *pDrvCtrl, UINT8 phyAdrs, UINT8 phyReg,
                               USHORT data);

/* externals */
IMPORT STATUS	sysDec21x40EnetAddrGet (int unit, char *enetAdrs);

/* globals */
USHORT		dec21140SromWordRead (DRV_CTRL *pDrvCtrl, UCHAR lineCnt);
FUNCPTR		_func_dec21x40MediaSelect = (FUNCPTR) NULL;
VOIDFUNCPTR	_func_dec2114xIntAck = (VOIDFUNCPTR) NULL;

#ifdef DRV_DEBUG
#include "netShow.h"
IMPORT void netPoolShow (NET_POOL_ID);
void decCsrShow (int inst);
void decShow (int inst);
#endif /* DRV_DEBUG */

/* END Specific interfaces. */

END_OBJ *	dec21x40EndLoad (char *initString);
LOCAL STATUS	dec21x40Unload (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	dec21x40Start (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	dec21x40Stop (DRV_CTRL *pDrvCtrl);
LOCAL int	dec21x40Ioctl (DRV_CTRL *pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS	dec21x40Send (DRV_CTRL *pDrvCtrl, M_BLK *pMblk);
LOCAL STATUS	dec21x40MCastAddrAdd (DRV_CTRL *pDrvCtrl, char* pAddress);
LOCAL STATUS	dec21x40MCastAddrDel (DRV_CTRL *pDrvCtrl, char* pAddress);
LOCAL STATUS	dec21x40MCastAddrGet (DRV_CTRL *pDrvCtrl, 
                                      MULTI_TABLE *pTable);
LOCAL STATUS	dec21x40PollSend (DRV_CTRL *pDrvCtrl, M_BLK *pMblk);
LOCAL STATUS	dec21x40PollReceive (DRV_CTRL *pDrvCtrl, M_BLK *pMblk);
LOCAL STATUS	dec21x40PollStart (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	dec21x40PollStop (DRV_CTRL *pDrvCtrl);

/* 
 * Define the device function table.  This is static across all driver
 * instances.
 */

LOCAL NET_FUNCS netFuncs = 
    {
    (FUNCPTR)dec21x40Start,	/* start func. */                 
    (FUNCPTR)dec21x40Stop,	/* stop func. */
    (FUNCPTR)dec21x40Unload,	/* unload func. */                
    (FUNCPTR)dec21x40Ioctl,	/* ioctl func. */                 
    (FUNCPTR)dec21x40Send,	/* send func. */                  
    (FUNCPTR)dec21x40MCastAddrAdd,/* multicast add func. */         
    (FUNCPTR)dec21x40MCastAddrDel,/* multicast delete func. */      
    (FUNCPTR)dec21x40MCastAddrGet,/* multicast get fun. */          
    (FUNCPTR)dec21x40PollSend,	/* polling send func. */          
    (FUNCPTR)dec21x40PollReceive, /* polling receive func. */       
    endEtherAddressForm,	/* put address info into a NET_BUFFER */
    endEtherPacketDataGet,	/* get pointer to data in NET_BUFFER */
    endEtherPacketAddrGet	/* Get packet addresses. */
    };				

/******************************************************************************
*
* endTok_r - get a token string (modified version)
*
* This modified version can be used with optional parameters.  If the
* parameter is not specified, this version returns NULL.  It does not
* signify the end of the original string, but that the parameter is null.
*
* .CS
*
*    /@ required parameters @/
*
*    string = endTok_r (initString, ":", &pLast);
*    if (string == NULL)
*        return ERROR;
*    reqParam1 = strtoul (string);
*
*    string = endTok_r (NULL, ":", &pLast);
*    if (string == NULL)
*        return ERROR;
*    reqParam2 = strtoul (string);
*
*    /@ optional parameters @/
*
*    string = endTok_r (NULL, ":", &pLast);
*    if (string != NULL)
*        optParam1 = strtoul (string);
*
*    string = endTok_r (NULL, ":", &pLast);
*    if (string != NULL)
*        optParam2 = strtoul (string);
* .CE
*/
 
char * endTok_r
    (
    char *       string,      /* string to break into tokens */
    const char * separators,  /* the separators */
    char **      ppLast               /* pointer to serve as string index */
    )
    {
    if ((string == NULL) && ((string = *ppLast) == NULL))
      return (NULL);

    if ((*ppLast = strpbrk (string, separators)) != NULL)
      *(*ppLast)++ = EOS;

    /* Return NULL, if string is empty */
    if (*string == EOS)
      return NULL;

    return (string);
    }

/***************************************************************************
*
* dec21x40EndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to an operational state.
* All of the device-specific parameters are passed in the <initStr>.
* If this routine is called with an empty but allocated string, it puts the 
* name of this device (that is, "dc") into the <initStr> and returns 0.
* If the string is allocated but not empty, this routine tries to load
* the device.
*
* RETURNS: An END object pointer or NULL on error.
*/

END_OBJ* dec21x40EndLoad
    (
    char* initStr		/* String to be parse by the driver. */
    )
    {
    DRV_CTRL	*pDrvCtrl;
    char	eAdrs[EADDR_LEN];            /* ethernet address */

    if (initStr == NULL)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "dec21x40EndLoad: NULL initStr\r\n",
		 0,0,0,0,0,0);
        return (NULL);
        }
    
    if (initStr[0] == '\0')
        {
        bcopy((char *)DRV_NAME, initStr, DRV_NAME_LEN);
        return (0);
        }

    /* Allocate a control structure for this device */

    pDrvCtrl = calloc (sizeof(DRV_CTRL), 1);

    if (pDrvCtrl == NULL)
        {
        DRV_LOG (DRV_DEBUG_LOAD,"%s - Failed to allocate control structure\n",
                  (int)DRV_NAME, 0,0,0,0,0 ); 
        return (NULL);
        }

    pDrvCtrl->flags = 0;
    pDrvCtrl->intrConnect = FALSE;
    pDrvCtrl->mediaCount = 0xff;
    pDrvCtrl->homePNAPhyValuesFound = FALSE;
    pDrvCtrl->offset = 0;


    if (dec21x40InitParse (pDrvCtrl, initStr) == ERROR)
        {
        DRV_LOG  (DRV_DEBUG_INIT, "%s - Failed to parse initialization"
	"parameters\n",
        (int)DRV_NAME,0,0,0,0,0);
        return (NULL);
        }

    if (END_OBJ_INIT (&pDrvCtrl->endObj, (DEV_OBJ*)pDrvCtrl, DRV_NAME,
                      pDrvCtrl->unit, &netFuncs,
                      "dec21x40 Enhanced Network Driver") == ERROR)
        {
        DRV_LOG  (DRV_DEBUG_INIT, "%s%d - Failed to initialize END object\n",
                  (int)DRV_NAME, pDrvCtrl->unit, 0,0,0,0);
        return (NULL);
        }

    if (dec21x40InitMem (pDrvCtrl) == ERROR)
        {
        DRV_LOG (DRV_DEBUG_INIT, "dec21x40InitMem failed",0,0,0,0,0,0);
        goto error;
        }

    /* decode non-register user flags */

    if (pDrvCtrl->usrFlags & DEC_USR_XEA)
        DRV_FLAGS_SET (DEC_BSP_EADRS);

    switch (pDrvCtrl->usrFlags & DEC_USR_VER_MSK)
        {
        case DEC_USR_21143 :
            DRV_FLAGS_SET (DEC_21143);
            break;

        case DEC_USR_21140 :
            DRV_FLAGS_SET (DEC_21140);
            break;

        case DEC_USR_21145 :
            DRV_FLAGS_SET (DEC_21145);
            break;

        default :
            DRV_FLAGS_SET (DEC_21040);
            break;
        }

    if (dec21x40EnetAddrGet (pDrvCtrl, eAdrs) == ERROR)
        {
        DRV_LOG  (DRV_DEBUG_INIT, "%s%d - Failed to read ethernet address\n",
                  (int)DRV_NAME, pDrvCtrl->unit,0,0,0,0);
        goto error;
        }

    DRV_LOG (DRV_DEBUG_LOAD,
             "ENET: %x:%x:%x:%x:%x:%x\n",
             eAdrs[0], eAdrs[1], eAdrs[2], eAdrs[3], eAdrs[4], eAdrs[5]);

#ifdef INCLUDE_RFC_1213  

    /* Old RFC 1213 mib2 interface */

    /* Initialize MIB-II entries */

    if (END_MIB_INIT (&pDrvCtrl->endObj, M2_ifType_ethernet_csmacd,
                      (UINT8*) eAdrs, 6,
                       ETHERMTU, DEC_SPEED_DEF) == ERROR)
        {
        DRV_LOG  (DRV_DEBUG_INIT, "%s%d - MIB-II initializations failed\n",
                  (int)DRV_NAME, pDrvCtrl->unit,0,0,0,0);
        goto error;
        }

    /* Mark the device ready with default flags */

    END_OBJ_READY (&pDrvCtrl->endObj, 
                   IFF_NOTRAILERS | IFF_MULTICAST | IFF_BROADCAST);
   
#else 

    /* New RFC 2233  mib2 interface */

    /* Initialize MIB-II entries (for RFC 2233 ifXTable) */
    pDrvCtrl->endObj.pMib2Tbl = m2IfAlloc(M2_ifType_ethernet_csmacd,
                                          (UINT8*) eAdrs, 6,
                                          ETHERMTU, DEC_SPEED_DEF,
                                          DRV_NAME, pDrvCtrl->unit);
    if (pDrvCtrl->endObj.pMib2Tbl == NULL)
        {
        printf ("%s%d - MIB-II initializations failed\n",
                DRV_NAME, pDrvCtrl->unit);
        goto error;
        }

    /*
     * Set the RFC2233 flag bit in the END object flags field and
     * install the counter update routines.
     */

#if defined(END_MIB_2233)
    pDrvCtrl->endObj.flags |= END_MIB_2233;
#endif  /* defined(END_MIB_2233) */

    m2IfPktCountRtnInstall(pDrvCtrl->endObj.pMib2Tbl, m2If8023PacketCount);

    /*
     * Make a copy of the data in mib2Tbl struct as well. We do this
     * mainly for backward compatibility issues. There might be some
     * code that might be referencing the END pointer and might
     * possibly do lookups on the mib2Tbl, which will cause all sorts
     * of problems.
     */

    bcopy ((char *)&pDrvCtrl->endObj.pMib2Tbl->m2Data.mibIfTbl,
                   (char *)&pDrvCtrl->endObj.mib2Tbl, sizeof (M2_INTERFACETBL));

    /* Mark the device ready with default flags */

#if defined(END_MIB_2233)
    END_OBJ_READY (&pDrvCtrl->endObj, IFF_NOTRAILERS | IFF_MULTICAST | 
                                      IFF_BROADCAST | END_MIB_2233);
#else /* !defined(END_MIB_2233) */
    END_OBJ_READY (&pDrvCtrl->endObj, IFF_NOTRAILERS | IFF_MULTICAST | 
                                      IFF_BROADCAST);
#endif /* !defined(END_MIB_2233) */

#endif /* INCLUDE_RFC_1213 */

    return (&pDrvCtrl->endObj);

    /* Handle error cases */

error:
    dec21x40Unload (pDrvCtrl);
    return (NULL);
    }

/***************************************************************************
*
* dec21x40Unload - unload a driver from the system
*
* This routine deallocates lists, and free allocated memory.
*
* RETURNS: OK, always.
*/

LOCAL STATUS dec21x40Unload
    (
    DRV_CTRL *pDrvCtrl
    )
    {

    DRV_LOG (DRV_DEBUG_LOAD, "EndUnload\n", 0, 0, 0, 0, 0, 0);

#ifndef INCLUDE_RFC_1213

    /* New RFC 2233 mib2 interface */

    /* Free MIB-II entries */

    m2IfFree(pDrvCtrl->endObj.pMib2Tbl);
    pDrvCtrl->endObj.pMib2Tbl = NULL;

#endif /* INCLUDE_RFC_1213 */

    /* deallocate lists */

    END_OBJ_UNLOAD (&pDrvCtrl->endObj);

    /* deallocate allocated shared memory */

    if (DRV_FLAGS_ISSET (DEC_MEMOWN)  && pDrvCtrl->memBase)
        cacheDmaFree (pDrvCtrl->memBase);

    return (OK);
    }

/***************************************************************************
*
* dec21x40InitParse - parse parameter values from initString
*
* The initialization string is modified by muxLib.o to include the unit number
* as the first parameter.
*
* Parse the input string.  Fill in values in the driver control structure.
*
* The initialization string format is:
* "<device addr>:<PCI addr>:<ivec>:<ilevel>:<mem base>:<mem size>:	\
*  <user flags>:<phyAddr>:<pPhyTbl>:<phyFlags>:<offset>:<loanBufs>:<drvFlags>"
*
* .bS
* device addr	base address of hardware device registers
* PCI addr	main memory address over the PCI bus
* ivec		interrupt vector number
* ilevel	interrupt level
* mem base	base address of a DMA-able, cache free,pre-allocated  memory
* mem size	size of the pre-allocated memory
* user flags	User flags control the run-time characteristics of the chip
* phyAddr	MII PHY address (optional)
* pPhyTbl	address of auto-negotiation table (optional)
* phyFlags	PHY configuration flags (optional)
* offset        Memory offset for alignment (optional)
* loanBufs      number of loaner clusters (optional)
* drvFlags	Driver flags control the runtime characteristics of the driver
*
* The last six arguments are optional. If the PHY address is to be specified
* then phyAddr, pPhyTbl, and phyFlags should all be specified together.
*
* RETURNS: OK or ERROR for invalid arguments.
*/

LOCAL STATUS dec21x40InitParse
    (
    DRV_CTRL	*pDrvCtrl,
    char 	*initString
    )
    {
    char *	tok;		/* an initString token */
    char *	holder=NULL;	/* points to initString fragment beyond tok */
    UINT32	tokValue;
    
    DRV_LOG (DRV_DEBUG_LOAD, "InitParse: Initstr=%s\n",
             (int) initString, 0, 0, 0, 0, 0);

    tok = endTok_r(initString, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->unit = atoi(tok);

    tok=endTok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->devAdrs = strtoul (tok, NULL, 16);
    
    tok=endTok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->pciMemBase = strtoul (tok, NULL, 16);

    tok=endTok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->ivec = strtoul (tok, NULL, 16);

    tok=endTok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->ilevel = strtoul (tok, NULL, 16);
    
    tok = endTok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    if (atoi(tok) < 0)
        pDrvCtrl->numRds = NUM_RDS_DEF;
    else
        pDrvCtrl->numRds = atoi(tok);
    
    tok = endTok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    if (atoi(tok) < 0)
        pDrvCtrl->numTds = NUM_TDS_DEF;
    else
        pDrvCtrl->numTds = atoi(tok);

    tok=endTok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->memBase = (char *) strtoul (tok, NULL, 16);

    tok=endTok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->memSize = strtoul (tok, NULL, 16);

    tok=endTok_r(NULL, ":", &holder);
    if (tok == NULL)
        return (ERROR);
    pDrvCtrl->usrFlags = strtoul(tok, NULL, 16);
 
    /*
     * Start of optional parameters.  Set each optional parameter to
     * its default value, so that each will have a reasonable value
     * if we run out of fields during the string parse.
     */

    pDrvCtrl->phyAddr = (UINT8) 0xFF;
    pDrvCtrl->pMiiPhyTbl = NULL;
    pDrvCtrl->miiPhyFlags = (DEC_USR_MII_10MB | DEC_USR_MII_HD |
			    DEC_USR_MII_100MB | DEC_USR_MII_FD |
			    DEC_USR_MII_BUS_MON);
    pDrvCtrl->offset   = 0;
    pDrvCtrl->loanBufs = NUM_LOAN;
    pDrvCtrl->drvFlags = 0;

    /* phyAddr */

    tok = endTok_r (NULL, ":", &holder);
    if (tok == NULL)
	goto endOptionalParams;
    pDrvCtrl->phyAddr = (UINT8) strtoul (tok, NULL, 16);

    /* pMiiPhyTbl - must be supplied if phyAddr was given */

    tok = endTok_r (NULL, ":", &holder);
    if (tok == NULL)
	return (ERROR);
    tokValue = strtoul (tok, NULL, 16);
    if ((INT32) tokValue != -1)
	pDrvCtrl->pMiiPhyTbl = (MII_AN_ORDER_TBL *) tokValue;

    /* miiPhyFlags - must be supplied if phyAddr was given */

    tok = endTok_r (NULL, ":", &holder);
    if (tok == NULL)
	return (ERROR);
    tokValue = strtoul (tok, NULL, 16);
    if ((INT32) tokValue != -1)
	pDrvCtrl->miiPhyFlags = tokValue;
 
    /* offset */

    tok = endTok_r (NULL, ":", &holder);
    if (tok == NULL)
	goto endOptionalParams;
    if (atoi(tok) >= 0)
	pDrvCtrl->offset = atoi (tok);

    /* loanBufs */

    tok = endTok_r (NULL, ":", &holder);
    if (tok == NULL)
	goto endOptionalParams;
    tokValue = (UINT32) strtoul (tok, NULL, 16);
    if ((INT32) tokValue != -1)
	pDrvCtrl->loanBufs = tokValue;

    /* drvFlags */

    tok = endTok_r (NULL, ":", &holder);
    if (tok == NULL)
	goto endOptionalParams;
    pDrvCtrl->drvFlags = (UINT32) strtoul (tok, NULL, 16);
    if (pDrvCtrl->drvFlags & DEC_DRV_NOSWAP_MASTER)
	DRV_FLAGS_SET (DEC_NOSWAP_MASTER);

endOptionalParams:

    /* print debug info */

    DRV_LOG (DRV_DEBUG_LOAD,
            "EndLoad: unit=%d devAdrs=0x%x ivec=0x%x ilevel=0x%x\n",
            pDrvCtrl->unit, pDrvCtrl->devAdrs, pDrvCtrl->ivec,
	    pDrvCtrl->ilevel, 0, 0);

    DRV_LOG (DRV_DEBUG_LOAD,
            "         membase=0x%x memSize=0x%x\n",
            (int)pDrvCtrl->memBase, pDrvCtrl->memSize, 0,0,0,0);
    
    DRV_LOG (DRV_DEBUG_LOAD,
             "        pciMemBase=0x%x flags=0x%x usrFlags=0x%x offset=%d\n",
             (int)pDrvCtrl->pciMemBase, pDrvCtrl->flags, pDrvCtrl->usrFlags,
             pDrvCtrl->offset, 0, 0);

    DRV_LOG (DRV_DEBUG_LOAD,
	     "        phyAddr=0x%x pMiiPhyTbl=0x%x miiPhyFlags=0x%x\n",
	     (int) pDrvCtrl->phyAddr, (int) pDrvCtrl->pMiiPhyTbl,
	     (int) pDrvCtrl->miiPhyFlags,
	     0,0,0);

    DRV_LOG (DRV_DEBUG_LOAD,
	     "        loanBufs=%d drvFlags=0x%x\n", 
	     (int) pDrvCtrl->loanBufs, (int) pDrvCtrl->drvFlags, 0, 0, 0, 0);

    return OK;
    }

/***************************************************************************
*
* dec21x40InitMem - initialize memory
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS dec21x40InitMem
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    DEC_RD *	pRxD = pDrvCtrl->rxRing;
    DEC_TD *	pTxD = pDrvCtrl->txRing;
    M_CL_CONFIG	dcMclBlkConfig;
    CL_DESC	clDesc;                      /* cluster description */
    char *	pBuf;
    int		ix;
    int		sz;
    char *	pShMem;
    

    DRV_LOG (DRV_DEBUG_LOAD, "InitMem\n", 0, 0, 0, 0, 0, 0);

    /* Establish a region of shared memory */
     
    /* We now know how much shared memory we need.  If the caller
     * provides a specific memory region, we check to see if the provided
     * region is large enough for our needs.  If the caller did not
     * provide a specific region, then we attempt to allocate the memory
     * from the system, using the cache aware allocation system call.
     */

    switch ((int)pDrvCtrl->memBase)
        {
        default :       /* caller provided memory */

            /* Establish size of shared memory region we require */

            sz = ((pDrvCtrl->numRds * (DEC_BUFSIZ + RD_SIZ + 4))  +
                  (pDrvCtrl->numTds * (DEC_BUFSIZ + TD_SIZ + 4))  +
                  (pDrvCtrl->loanBufs * (DEC_BUFSIZ + 4)) + 4);

            if ( pDrvCtrl->memSize < sz )     /* not enough space */
                {
                DRV_LOG ( DRV_DEBUG_INIT, "%s%d: not enough memory provided\n",
                         (int)DRV_NAME, pDrvCtrl->unit,0,0,0,0);
                return ( ERROR );
                }
            pShMem = pDrvCtrl->memBase;      /* set the beginning of pool */

            /* assume pool is cache coherent, copy null structure */

            pDrvCtrl->cacheFuncs = cacheNullFuncs;

            break;

        case NONE :     /* get our own memory */

            /* 
             * Because the structures that are shared between the device
             * and the driver may share cache lines, the possibility exists
             * that the driver could flush a cache line for a structure and
             * wipe out an asynchronous change by the device to a neighboring
             * structure. Therefore, this driver cannot operate with memory
             * that is not write coherent.  We check for the availability of
             * such memory here, and abort if the system did not give us what
             * we need.
             */

            if (!CACHE_DMA_IS_WRITE_COHERENT ())
                {
                DRV_LOG  ( DRV_DEBUG_INIT, "dc: device requires cache"
		"coherent memory\n",
	        0,0,0,0,0,0);
                return (ERROR);
                }

            sz = (((pDrvCtrl->numRds + 1) * RD_SIZ) +
                  ((pDrvCtrl->numTds + 1) * TD_SIZ));

            pDrvCtrl->memBase = 
                pShMem = (char *) cacheDmaMalloc ( sz );

            if (pShMem == NULL)
                {
                DRV_LOG ( DRV_DEBUG_LOAD, "%s%d - system memory unavailable\n",
                          (int)DRV_NAME, pDrvCtrl->unit, 0,0,0,0);
                return (ERROR);
                }

            pDrvCtrl->memSize = sz;

            DRV_FLAGS_SET (DEC_MEMOWN);

            /* copy the DMA structure */

            pDrvCtrl->cacheFuncs = cacheDmaFuncs;

            break;
        }

    /* zero the shared memory */

    bzero (pShMem, (int) sz);

    /* carve Rx memory structure */

    pRxD =
        pDrvCtrl->rxRing = (DEC_RD *) (((int)pShMem + 0x03) & ~0x03);

    /* carve Tx memory structure */

    pTxD =
        pDrvCtrl->txRing = (DEC_TD *) (pDrvCtrl->rxRing + pDrvCtrl->numRds);

    /* Initialize net buffer pool for tx/rx buffers */

    bzero ((char *)&dcMclBlkConfig, sizeof(dcMclBlkConfig));
    bzero ((char *)&clDesc, sizeof(clDesc));
    
    dcMclBlkConfig.mBlkNum  = (pDrvCtrl->numRds + pDrvCtrl->loanBufs) * 4;
    clDesc.clNum 	    = pDrvCtrl->numRds + pDrvCtrl->numTds + 
                              pDrvCtrl->loanBufs;
    dcMclBlkConfig.clBlkNum = clDesc.clNum;

    /*
     * mBlk and cluster configuration memory size initialization
     * memory size adjusted to hold the netPool pointer at the head.
     */

    dcMclBlkConfig.memSize = ((dcMclBlkConfig.mBlkNum *
                               (MSIZE + sizeof (long))) +
                              (dcMclBlkConfig.clBlkNum *
                               (CL_BLK_SZ + sizeof (long))));
    
    if ((dcMclBlkConfig.memArea = (char *)memalign(sizeof (long),
                                                   dcMclBlkConfig.memSize))
        == NULL)
        return (ERROR);
    
    clDesc.clSize 	= DEC_BUFSIZ;
    clDesc.memSize 	= ((clDesc.clNum * (clDesc.clSize + 4)) + 4);

    if (DRV_FLAGS_ISSET(DEC_MEMOWN))
        {
        clDesc.memArea = (char *) cacheDmaMalloc (clDesc.memSize);
        if (clDesc.memArea == NULL)
            {
            DRV_LOG  (DRV_DEBUG_INIT,  "%s%d - system memory unavailable\n",
                      (int)DRV_NAME, pDrvCtrl->unit, 0,0,0,0);
            return (ERROR);
            }
        }
    else
        clDesc.memArea = (char *) (pDrvCtrl->txRing + pDrvCtrl->numTds);
    
    if ((pDrvCtrl->endObj.pNetPool = malloc (sizeof(NET_POOL))) == NULL)
        return (ERROR);

    /* Initialize the net buffer pool with transmit buffers */

    if (netPoolInit (pDrvCtrl->endObj.pNetPool, &dcMclBlkConfig,
                     &clDesc, 1, NULL) == ERROR)
        {
        DRV_LOG  (DRV_DEBUG_INIT, "%s%d - netPoolInit failed\n",
		  (int)DRV_NAME, pDrvCtrl->unit,0,0,0,0);
        return (ERROR);
        }

    /* Save the cluster pool id */

    pDrvCtrl->clPoolId = clPoolIdGet (pDrvCtrl->endObj.pNetPool,
                                      DEC_BUFSIZ, FALSE);
    /* Clear all indices */

    pDrvCtrl->rxIndex=0;
    pDrvCtrl->txIndex=0;
    pDrvCtrl->txDiIndex=0;

    /* Setup the receive ring */

    for (ix = 0; ix < pDrvCtrl->numRds; ix++, pRxD++)
        {
        pBuf = (char *) NET_BUF_ALLOC();
        if (pBuf == NULL)
            {
            DRV_LOG  (DRV_DEBUG_INIT, "%s%d - netClusterGet failed\n",
		      (int)DRV_NAME, pDrvCtrl->unit,0,0,0,0);
            return (ERROR);
            }
	pRxD->rDesc2 = PCISWAP (DEC_VIRT_TO_PCI (pBuf)); /* buffer 1 */
        pRxD->rDesc3 = PCISWAP ((ULONG)pBuf);      /* save virt addr */

	/* buffer size */

	pRxD->rDesc1 = PCISWAP (RDESC1_RBS1_VAL (DEC_BUFSIZ) | 
                                RDESC1_RBS2_VAL (0));

	if (ix == (pDrvCtrl->numRds - 1))	/* if its is last one */
	    pRxD->rDesc1 |= PCISWAP (RDESC1_RER); /* end of receive ring */

	pRxD->rDesc0 = PCISWAP (RDESC0_OWN);	/* give ownership to chip */
        }


    /* Setup the transmit ring */

    for (ix = 0; ix < pDrvCtrl->numTds; ix++, pTxD++)
        {

        /* empty -- no buffers at this time */

	pTxD->tDesc2 = 0;
	pTxD->tDesc3 = 0;

	pTxD->tDesc1 = PCISWAP ((TDESC1_TBS1_PUT(0) |   /* buffer1 size */
                                 TDESC1_TBS2_PUT(0) |   /* buffer2 size */
                                 TDESC1_IC          | 	/* intrpt on xmit */
                                 TDESC1_LS          |	/* last segment */
                                 TDESC1_FS));		/* first segment */

	if (ix == (pDrvCtrl->numTds - 1))    /* if its is last one */
	    pTxD->tDesc1 |= PCISWAP (TDESC1_TER); /* end of Xmit ring */

	pTxD->tDesc0 = 0;                    /* owner is host */
        }

    /* Fill the buffer for txPollSend */

    pDrvCtrl->txPollSendBuf = NET_BUF_ALLOC();

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    return (OK);
    }

/***************************************************************************
*
* dec21x40Start - start the device
*
* This function initializes the device and calls BSP functions to connect
* interrupts and start the device running in interrupt mode.
*
* The complement of this routine is dec21x40Stop.  Once a unit is reset by
* dec21x40Stop, it may be re-initialized to a running state by this routine.
*
* RETURNS: OK if successful, otherwise ERROR
*/

LOCAL STATUS dec21x40Start
    (
    DRV_CTRL *	pDrvCtrl /* device to start */
    )
    {
    int    retVal;
    UINT   csr0Val; 
    UINT   csr6Val   = 0;
    UINT   usrFlags  = pDrvCtrl->usrFlags;
    UINT   tries     = 0;

    DRV_LOG (DRV_DEBUG_LOAD, 
     "Start, IO base addr: 0x%x ivec %d, ilevel %d, mempool base addr: 0x%x\n",
      pDrvCtrl->devAdrs, pDrvCtrl->ivec, pDrvCtrl->ilevel,
      (int)(pDrvCtrl->memBase), 
      0, 0);

    DRV_LOG (DRV_DEBUG_LOAD,
             "        pciMemBase=0x%x flags=0x%x usrFlags=0x%x\n",
             (int)pDrvCtrl->pciMemBase, pDrvCtrl->flags, pDrvCtrl->usrFlags,
             0, 0, 0);

restart:

    /* Reset the device */

    DEC_CSR_WRITE (CSR6, 0);
    dec21x40ChipReset (pDrvCtrl);

    /* Clear all indices */

    pDrvCtrl->rxIndex=0;
    pDrvCtrl->txIndex=0;
    pDrvCtrl->txDiIndex=0;
 
    pDrvCtrl->txCleaning = FALSE;
    pDrvCtrl->rxHandling = FALSE;
    pDrvCtrl->txBlocked = FALSE;

    if (! DRV_FLAGS_ISSET (DEC_21040))
        {
        if (_func_dec21x40MediaSelect != NULL)
            retVal = (* _func_dec21x40MediaSelect) (pDrvCtrl, &csr6Val);
	else if (DRV_FLAGS_ISSET (DEC_21140))
            retVal = dec21140MediaSelect (pDrvCtrl, &csr6Val);
	else if (DRV_FLAGS_ISSET (DEC_21143))
	    retVal = dec21143MediaSelect (pDrvCtrl, &csr6Val);
	else if (DRV_FLAGS_ISSET (DEC_21145))
	    retVal = dec21145MediaSelect (pDrvCtrl, &csr6Val);
	else
	    retVal = ERROR;

        if (retVal == ERROR)
            return (ERROR);
        
        if (csr6Val & CSR6_21140_PS)
            {
           
            /* changing PS requires a software reset */

            DEC_CSR_UPDATE (CSR6, CSR6_21140_PS);

            /* copy CSR0 before the software reset */

            csr0Val = DEC_CSR_READ (CSR0); 

            DEC_CSR_WRITE (CSR0, CSR0_SWR);

            /* wait for software reset to complete before restoring CSRO */ 

            taskDelay(sysClkRateGet()/30);

            /* restore CSR0 */

            DEC_CSR_WRITE (CSR0, csr0Val);  

            csr6Val |= CSR6_21140_HBD;
   
            /* clear CSR13 and 14 if 21143 - Appendix D, Port Selection */
 
            if (usrFlags & DEC_USR_21143)
                {   
                DEC_CSR_WRITE (CSR13, 0);
                DEC_CSR_WRITE (CSR14, 0);
                }
            }

        csr6Val &= DEC_USR_CSR6_MSK;
        csr6Val |= CSR6_21140_MB1;

        /* decode CSR6 specific options from userFlags */

        if (usrFlags & DEC_USR_SF)
            csr6Val |= CSR6_21140_SF;

        if (usrFlags & DEC_USR_THR_MSK)
            csr6Val |= (usrFlags & DEC_USR_THR_MSK) >> DEC_USR_THR_SHF;

        if (usrFlags & DEC_USR_SB)
            csr6Val |= CSR6_SB;

        if (usrFlags & DEC_USR_PB)
            csr6Val |= CSR6_PB;

        if (usrFlags & DEC_USR_SC)
            csr6Val |= CSR6_21140_SC;

        if (usrFlags & DEC_USR_CA)
            csr6Val |= CSR6_CAE;
        }
    else
        dec21040AuiTpInit (pDrvCtrl);

    /* Write start of receive ring */

    DEC_CSR_WRITE (CSR3, DEC_VIRT_TO_PCI(pDrvCtrl->rxRing));

    /* Write start of transmit ring */

    DEC_CSR_WRITE (CSR4, DEC_VIRT_TO_PCI(pDrvCtrl->txRing));

    /* clear the status register */

    DEC_CSR_WRITE (CSR5, 0xffffffff);

    /* setup CSR6 - start transmitter */

    DEC_CSR_WRITE (CSR6, csr6Val | CSR6_ST);

    DRV_LOG (DRV_DEBUG_LOAD, "Writing CSR6 = %#08x\nCSR7 = %#08x\n",
	     csr6Val | CSR6_ST, DEC_CSR_READ (CSR7),
	     0,0,0,0);

    taskDelay (sysClkRateGet() * 2);

    /* Check status of link */

    tries++;		/* Increment the number of attempts to find media */

    if ((csr6Val & CSR6_21140_PS) && (csr6Val & CSR6_21140_PCS))
	{
	/* 100 Mbps non-MII mode */

	if (DEC_CSR_READ (CSR12) & CSR12_21143_LS100)
	    {
	    /* LS100 indicates link state down */

	    if (tries < pDrvCtrl->mediaCount)
		{
		DRV_LOG (DRV_DEBUG_INIT, "100Mbps link failed - restarting\n",
			 0,0,0,0,0,0);

		goto restart;
		}
	    else
		{
		DRV_LOG (DRV_DEBUG_INIT, "100Mbps link failed - aborting\n",
			 0,0,0,0,0,0);

		return ERROR;
		}
	    }
	}
    else if ((csr6Val & CSR6_21140_PS) == 0)
	{
	/* 10 Mbps mode */

	if (DEC_CSR_READ (CSR12) & (CSR12_21143_LS10 | CSR12_21040_LKF))
	    {
	    /* Link down */

	    if (tries < pDrvCtrl->mediaCount)
		{
		DRV_LOG (DRV_DEBUG_INIT, "10Mbps link failed - restarting\n",
			 0,0,0,0,0,0);

		goto restart;
		}
	    else
		{
		DRV_LOG (DRV_DEBUG_INIT, "10Mbps link failed - aborting\n",
			 0,0,0,0,0,0);

	        return ERROR;
		}
	    }
	}

    /* setup ethernet address and filtering mode */

    dec21x40IASetup (pDrvCtrl);

    /* set operating mode and start the receiver */

    dec21x40ModeSet (pDrvCtrl);

    /* Connect the interrupt handler */

    SYS_INT_CONNECT (pDrvCtrl, dec21x40Int, pDrvCtrl, &retVal);
    if (retVal == ERROR)
        return (ERROR);

    /* set up the interrupts */

    pDrvCtrl->intrMask = (CSR7_NIM |        /* normal interrupt mask */
                          CSR7_RIM |         /* rcv  interrupt mask */
                          CSR7_TIM |         /* xmit interrupt mask */
                          CSR7_TUM |         /* xmit buff unavailble mask */
                          CSR7_AIM |         /* abnormal interrupt mask */
                          CSR7_SEM |         /* system error mask */
                          CSR7_RUM );       /* rcv buff unavailable mask */

    if (DRV_FLAGS_ISSET(DEC_21145) 
	 && (pDrvCtrl->usrFlags & (DEC_USR_HPNA_PREFER_10BT)))
	{
	/* link fail, pass/anc mask */

	pDrvCtrl->intrMask |= (CSR7_ANC | CSR7_21040_LFM );  
	}

    DEC_CSR_WRITE (CSR7, pDrvCtrl->intrMask);

    if (DRV_FLAGS_ISSET (DEC_21040))
        DEC_CSR_UPDATE (CSR7, CSR7_21040_LFM);   /* link fail mask */

    /* mark the interface -- up */

    END_FLAGS_SET (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));

    /* Enable LAN interrupts */

    SYS_INT_ENABLE (pDrvCtrl);

    pDrvCtrl->rxLen = 0;
    pDrvCtrl->rxMaxLen = RWIN;

    return OK;
    }

/***************************************************************************
*
* dec21x40Stop - stop the device
*
* This routine disables interrupts, and resets the device.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS dec21x40Stop
    (
    DRV_CTRL	*pDrvCtrl
    )
    {
    int		retVal	 = OK;
    PHY_INFO *	pPhyInfo = pDrvCtrl->pPhyInfo;

    DRV_LOG (DRV_DEBUG_LOAD, "Stop\n", 0, 0, 0, 0, 0, 0);

    /* mark the interface -- down */

    END_FLAGS_CLR (&pDrvCtrl->endObj, IFF_UP | IFF_RUNNING);

    /* Disable LAN interrupts */

    SYS_INT_DISABLE (pDrvCtrl);

    /* stop transmit and receive */

    DEC_CSR_RESET (CSR6, CSR6_ST | CSR6_SR);

    if ((pDrvCtrl->usrFlags & DEC_USR_21145) == 0)
	{
	if ((pPhyInfo != NULL) && MII_PHY_FLAGS_ARE_SET (MII_PHY_INIT))
	    {
	    if (intCnt == 0)
		{
		if (miiPhyUnInit (pPhyInfo) == ERROR)
		    return (ERROR);

		free (pPhyInfo);
		}
	    else
		{
		/*
		 * Defer these to task level since they cannot be performed at
		 * interrupt level
		 */

		netJobAdd ((FUNCPTR) miiPhyUnInit, (int) pPhyInfo, 0,0,0,0);
		netJobAdd ((FUNCPTR) free, (int) pPhyInfo, 0,0,0,0);
		}

	    pDrvCtrl->pPhyInfo = NULL;
	    }
	}

    /* disconnect interrupt */

    SYS_INT_DISCONNECT (pDrvCtrl, dec21x40Int, (int)pDrvCtrl, &retVal);

    return (retVal);
    }

/***************************************************************************
*
* dec21x40MediaChange - start the device
*
* Restarts the device after cleaning up the transmit and receive queues. This
* routine should be called only after dec21x40Stop().
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS dec21x40MediaChange
    (
    DRV_CTRL	*pDrvCtrl
    )
    {
    if (dec21x40Stop (pDrvCtrl) == ERROR)
	{
	DRV_LOG (DRV_DEBUG_INIT, "dec21x40MediaChange, unable to stop\n",
		0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    DRV_LOG (DRV_DEBUG_ALL, "%s%d - stopped device.\n",
            (int)DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);

    netJobAdd ((FUNCPTR) dec21x40Restart, (int) pDrvCtrl, 0,0,0,0);

    return (OK);
    }
/***************************************************************************
*
* dec21x40Restart - start the device
*
* Restarts the device after cleaning up the transmit and receive queues. This
* routine should be called only after dec21x40Stop().
*
* RETURNS: OK or ERROR
*/

LOCAL void dec21x40Restart
    (
    DRV_CTRL	*pDrvCtrl
    )
    {
    DEC_TD 	*pTxD = pDrvCtrl->txRing;
    FREE_BUF	*pFreeBuf = pDrvCtrl->freeBuf;
    int		count;
    
    DRV_LOG (DRV_DEBUG_ALL, "%s%d - restarting device.\n",
            (int)DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
    
    /* cleanup the tx and rx queues */

    dec21x40TxRingClean (pDrvCtrl);
    dec21x40RxIntHandle (pDrvCtrl);

    /* drop rest of the queued tx packets */

    for (count=pDrvCtrl->numTds; count; count--, pTxD++, pFreeBuf++)
        {
        if (pFreeBuf->pClBuf != NULL)
            {
            NET_BUF_FREE(pFreeBuf->pClBuf);
            pFreeBuf->pClBuf = NULL;
            }

        DEC_CACHE_INVALIDATE (pTxD, TD_SIZ);
        
        if (pTxD->tDesc1 & PCISWAP(TDESC1_SET))
            {
            pTxD->tDesc1 &= PCISWAP(~(TDESC1_SET | TDESC1_FT0));
            pTxD->tDesc0 = 0;
            }
        else if (pTxD->tDesc0 & PCISWAP(TDESC0_OWN|TDESC0_ES))
            {
#ifdef INCLUDE_RFC_1213

            /* Old RFC 1213 mib2 interface */

            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, -1);
            
#else 

            /* New RFC 2233 mib2 interface */

            if (pDrvCtrl->endObj.pMib2Tbl != NULL)
                {
                pDrvCtrl->endObj.pMib2Tbl->m2CtrUpdateRtn(pDrvCtrl->endObj.
                                                                    pMib2Tbl,
                                                          M2_ctrId_ifOutErrors,
                                                          1);
                }
#endif /* INCLUDE_RFC_1213 */
            pTxD->tDesc0 = 0;
            }
        }

    /* Reset indices */

    pDrvCtrl->rxIndex=0;
    pDrvCtrl->txIndex=0;
    pDrvCtrl->txDiIndex=0;

    /* Restart the device */

    dec21x40Start (pDrvCtrl);
    }


/***************************************************************************
*
* dec21x40Ioctl - interface ioctl procedure
*
* Process an interface ioctl request.
*
* This routine implements the network interface control functions.
* It handles EIOCSADDR, EIOCGADDR, EIOCSFLAGS, EIOCGFLAGS, EIOCMULTIADD,
* EIOCMULTIDEL, EIOCMULTIGET, EIOCPOLLSTART, EIOCPOLLSTOP, EIOCGMIB2 commands.
*
* RETURNS: OK if successful, otherwise EINVAL.
*/

LOCAL int dec21x40Ioctl
    (
    DRV_CTRL *	pDrvCtrl,
    int		cmd,
    caddr_t	data
    )
    {
    int		error=0;
    long	value;
    int		savedFlags;
    END_OBJ *	pEndObj=&pDrvCtrl->endObj;
    
    DRV_LOG (DRV_DEBUG_IOCTL,
             "Ioctl unit=0x%x cmd=%d data=0x%x\n",
             pDrvCtrl->unit, cmd, (int)data, 0, 0, 0);

    switch ((UINT)cmd)
        {
        case EIOCSADDR:
            if (data == NULL)
                error = EINVAL;
            else
                {
                /* Copy and install the new address */

                bcopy ((char *)data, (char *)END_HADDR(pEndObj),
                       END_HADDR_LEN(pEndObj));
                dec21x40IASetup (pDrvCtrl);
                }

            break;
            
        case EIOCGADDR:
            if (data == NULL)
                error = EINVAL;
            else
                bcopy ((char *)END_HADDR(pEndObj), (char *)data,
                       END_HADDR_LEN(pEndObj));
#ifdef	DRV_DEBUG
            {
            char *cp = (char *)data;
            DRV_LOG (DRV_DEBUG_IOCTL,
                          "EIOCGADDR: %x:%x:%x:%x:%x:%x\n",
                          cp[0], cp[1], cp[2], cp[3], cp[4], cp[5]);
            }
#endif /*DRV_DEBUG*/
            break;

        case EIOCSFLAGS:
            value = (long) data;
            if (value < 0)
                {
                value = -value;
                value--;
                END_FLAGS_CLR (pEndObj, value);
                }
            else
                END_FLAGS_SET (pEndObj, value);

            /* handle IIF_PROMISC and IFF_ALLMULTI */

            savedFlags = DRV_FLAGS_GET();
            if (END_FLAGS_ISSET (pEndObj, IFF_PROMISC))
                DRV_FLAGS_SET (DEC_PROMISC);
            else
                DRV_FLAGS_CLR (DEC_PROMISC);

            if (END_FLAGS_GET(pEndObj) & (IFF_MULTICAST | IFF_ALLMULTI))
                DRV_FLAGS_SET (DEC_MCAST);
            else
                DRV_FLAGS_CLR (DEC_MCAST);

            DRV_LOG (DRV_DEBUG_IOCTL,
                       "endFlags=0x%x savedFlags=0x%x newFlags=0x%x\n",
                        END_FLAGS_GET(pEndObj), savedFlags, DRV_FLAGS_GET(),
			0, 0, 0);

            if ((DRV_FLAGS_GET() != savedFlags) &&
                (END_FLAGS_GET(pEndObj) & IFF_UP))
                dec21x40ModeSet (pDrvCtrl);

            break;

        case EIOCGFLAGS:                     /* move to mux */
            DRV_LOG (DRV_DEBUG_IOCTL, "EIOCGFLAGS: 0x%x: 0x%x\n",
                    pEndObj->flags, *(long *)data, 0, 0, 0, 0);

            if (data == NULL)
                error = EINVAL;
            else
                *(long *)data = END_FLAGS_GET(pEndObj);

            break;

        case EIOCMULTIADD:                   /* move to mux */
            error = dec21x40MCastAddrAdd (pDrvCtrl, (char *)data);
            break;

        case EIOCMULTIDEL:                   /* move to mux */
            error = dec21x40MCastAddrDel (pDrvCtrl, (char *)data);
            break;

        case EIOCMULTIGET:                   /* move to mux */
            error = dec21x40MCastAddrGet (pDrvCtrl, (MULTI_TABLE *)data);
            break;

        case EIOCPOLLSTART:                  /* move to mux */
            dec21x40PollStart (pDrvCtrl);
            break;

        case EIOCPOLLSTOP:                   /* move to mux */
            dec21x40PollStop (pDrvCtrl);
            break;

        case EIOCGMIB2:                      /* move to mux */
            if (data == NULL)
                error=EINVAL;
            else
                bcopy((char *)&pEndObj->mib2Tbl, (char *)data,
                      sizeof(pEndObj->mib2Tbl));
            break;

#ifndef INCLUDE_RFC_1213

        /* New RFC 2233 mib2 interface */

        case EIOCGMIB2233:
            if ((data == NULL) || (pEndObj->pMib2Tbl == NULL))
                error = EINVAL;
            else
                *((M2_ID **)data) = pEndObj->pMib2Tbl;
            break;
#endif /* INCLUDE_RFC_1213 */

        default:
            error = EINVAL;
        }

    return (error);
    }

/***************************************************************************
*
* dec21x40ModeSet - promiscuous & multicast operation and start receiver
*
* RETURNS: N/A
*/

LOCAL void dec21x40ModeSet
    (
    DRV_CTRL	*pDrvCtrl
    )
    {
    int	opMode = CSR6_SR;                    /* start receiver */

    DRV_LOG (DRV_DEBUG_IOCTL, "ModeSet\n", 0, 0, 0, 0, 0, 0);

    if (DRV_FLAGS_ISSET (DEC_MCAST))
        opMode |= CSR6_PM;                   /* rx multicast */

    if (DRV_FLAGS_ISSET (DEC_PROMISC))
        opMode |= CSR6_PR;                   /* rx promiscuous */
    
    DEC_CSR_WRITE (CSR6, (DEC_CSR_READ (CSR6) & ~(CSR6_PM|CSR6_PR)) | opMode);

    return;
    }

/***************************************************************************
*
* dec21x40HashIndex - compute the hash index for an ethernet address
*
* RETURNS: hash index for an ethernet address.
*/

LOCAL int dec21x40HashIndex
    (
    char *	eAddr
    )
    {
    UINT8	eAddrByte;
    int		index;                       /* hash index - return value */
    int		byte;                        /* loop - counter */
    int		bit;                         /* loop - counter */
    UINT	crc = 0xffffffff;
    UINT8	msb;

    for (byte=0; byte<6; byte++)
        {
        eAddrByte = eAddr[byte];

        for (bit=0; bit<8; bit++)
            {
            msb = crc >> 31;
            crc <<= 1;
            if (msb ^ (eAddrByte & 0x1))
                {
                crc ^= DEC_CRC_POLY;
                crc |= 0x1;
                }
            eAddrByte >>= 1;
            }
        }

    /*
     * return the upper 9 bits of the CRC in a decreasing order of
     * significance.
     * crc <31..23> as index <0..8>
     */

    index = 0;
    for (bit=0; bit<9; bit++)
        index |= ((crc >> (31-bit)) & 0x1) << bit;

    return index;
    }

/***************************************************************************
*
* dec21x40IASetup - set up physical and multicast addresses
*
* This routine sets up a filter frame to filter the physical addresses
* and all the current multicast addresses.
*
* While the first call to this routine during chip initialization requires
* that the receiver be turned off, subsequent calls do not.
* 
* RETURNS: OK or ERROR.
*/

LOCAL STATUS dec21x40IASetup
    (
    DRV_CTRL *	pDrvCtrl 	/* pointer to device control structure */
    )
    {
    UINT8	ethBcastAdrs[]={ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    UINT8 	*pFltrFrm;
    UINT8	*pAdrs;
    ETHER_MULTI	*pMCastNode;
    DEC_TD	*pTxD;
    ULONG	csr7Val;
    char *	pBuf;
    int		index;
    int		timeout;

    /* gain exclusive access to transmitter */
    END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);
    
    /* get a free transmit descriptor */
    pTxD = dec21x40TxDGet (pDrvCtrl);
    if (pTxD == NULL)
        {
        DRV_LOG (DRV_DEBUG_INIT, "dec21x40TxDGet failed\n", 1, 2, 3, 4, 5, 6);
        END_TX_SEM_GIVE (&pDrvCtrl->endObj);
        return (ERROR);
        }

    DRV_LOG (DRV_DEBUG_IOCTL, "IASetup 0x%x\n", pTxD->tDesc0, 0, 0, 0, 0, 0);

    /* get a buffer */

    pBuf = NET_BUF_ALLOC();
    if (pBuf == NULL)
        {
        DRV_LOG (DRV_DEBUG_INIT, "netClusterGet failed\n", 1, 2, 3, 4, 5, 6);
        END_TX_SEM_GIVE (&pDrvCtrl->endObj);
        return (ERROR);
        }

    /* align the frame */

    pFltrFrm = (UINT8 *) (((int)pBuf + 0x3) & ~0x3);

    /* clear all entries */

    bzero (pFltrFrm, FLTR_FRM_SIZE);

    /* install multicast addresses */

    for (pMCastNode = END_MULTI_LST_FIRST (&pDrvCtrl->endObj);
         pMCastNode != NULL;
         pMCastNode = END_MULTI_LST_NEXT (pMCastNode))
        {
        index = dec21x40HashIndex (pMCastNode->addr);
        pFltrFrm [DEC_FLT_INDEX(index/8)] |= 1 << (index % 8);
        }

    /* install an ethernet broadcast address */

    index = dec21x40HashIndex (ethBcastAdrs);
    pFltrFrm [DEC_FLT_INDEX(index/8)] |= 1 << (index % 8);

    /* install the physical address */

    pAdrs = (UINT8 *)END_HADDR (&pDrvCtrl->endObj);

    for (index=0; index<6; index++)
        pFltrFrm [FLTR_FRM_PHY_ADRS_OFF + DEC_FLT_INDEX(index)] = pAdrs[index];

    /* transmit the frame */

    pTxD->tDesc2  = PCISWAP (DEC_VIRT_TO_PCI (pFltrFrm));
    pTxD->tDesc3  = 0;

    /* frame type is SETUP, filtering mode is HASH + 1 perfect */

    pTxD->tDesc1 |= PCISWAP (TDESC1_SET | TDESC1_FT0); 

    pTxD->tDesc1 &= PCISWAP (~TDESC1_TBS1_MSK);
    pTxD->tDesc1 |= PCISWAP (TDESC1_TBS1_PUT(FLTR_FRM_SIZE));
    pTxD->tDesc1 &= PCISWAP (~(TDESC1_LS|TDESC1_FS));

    pTxD->tDesc0  = PCISWAP(TDESC0_OWN);      /* ready for transmit */

    CACHE_PIPE_FLUSH();

    /* Advance our management index */

    pDrvCtrl->txIndex = (pDrvCtrl->txIndex + 1) % pDrvCtrl->numTds;

    /* mask interrupts */

    csr7Val = DEC_CSR_READ (CSR7);
    DEC_CSR_WRITE (CSR7, 0);

    /* start tx */

    if (DRV_FLAGS_ISSET(DEC_TX_KICKSTART))
        DEC_CSR_WRITE (CSR1, CSR1_TPD);

    /* wait for completion */

    timeout=0xffffff;
    while (timeout && (pTxD->tDesc0 != (~PCISWAP (TDESC0_OWN))))
	timeout--;
    if (timeout <= 0)
	{
	DRV_LOG (DRV_DEBUG_INIT, "no toggle on OWN bit 0x%x %x\n", 
		 (int)(&pTxD->tDesc0), pTxD->tDesc0, 3, 4, 5, 6);
        END_TX_SEM_GIVE (&pDrvCtrl->endObj);
	return (ERROR);
	}

    /* restore TXD bits */

    pTxD->tDesc0  = 0;
    pTxD->tDesc1 &= PCISWAP (~(TDESC1_SET | TDESC1_FT0));
    pTxD->tDesc1 |= PCISWAP (TDESC1_LS | TDESC1_FS);
    pTxD->tDesc2  = 0;

    CACHE_PIPE_FLUSH();

    /* restore interrupts */

    DEC_CSR_WRITE (CSR7, csr7Val);

    /* free the buffer */

    NET_BUF_FREE (pBuf);

    /* release exclusive access */

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    /* return success */

    return (OK);
    }

/***************************************************************************
*
* dec21x40MCastAddrAdd - add a multicast address
*
*
* RETURNS: OK on success, ERROR otherwise.
*/

LOCAL STATUS dec21x40MCastAddrAdd
    (
    DRV_CTRL *	pDrvCtrl,
    char *	pAddr
    )
    {
    int retVal;

    DRV_LOG (DRV_DEBUG_IOCTL, "MCastAddrAdd\n", 0, 0, 0, 0, 0, 0);

    retVal = etherMultiAdd (&pDrvCtrl->endObj.multiList, pAddr);

    if (retVal == ENETRESET)
        return dec21x40IASetup (pDrvCtrl);

    return ((retVal == OK) ? OK : ERROR);
    }

/***************************************************************************
*
* dec21x40MCastAddrDel - remove a multicast address
*
*
* RETURNS: OK on success, ERROR otherwise.
*/

LOCAL STATUS dec21x40MCastAddrDel
    (
    DRV_CTRL *	pDrvCtrl,
    char *	pAddr
    )
    {
    int retVal;

    DRV_LOG (DRV_DEBUG_IOCTL, "MCastAddrDel\n", 0, 0, 0, 0, 0, 0);

    retVal = etherMultiDel (&pDrvCtrl->endObj.multiList, pAddr);

    if (retVal == ENETRESET)
        return dec21x40IASetup (pDrvCtrl);

    return ((retVal == OK) ? OK : ERROR);
    }

/***************************************************************************
*
* dec21x40MCastAddrGet - retreive current multicast address list
*
*
* RETURNS: OK on success; otherwise ERROR.
*/

LOCAL STATUS dec21x40MCastAddrGet
    (
    DRV_CTRL *	pDrvCtrl,
    MULTI_TABLE *pTable
    )
    {
    DRV_LOG (DRV_DEBUG_IOCTL, "MCastAddrGet\n", 0, 0, 0, 0, 0, 0);

    return (etherMultiGet (&pDrvCtrl->endObj.multiList, pTable));
    }

/***************************************************************************
*
* dec21x40Send - transmit an ethernet packet
*
* RETURNS: OK on success; and ERROR otherwise.
*/

LOCAL STATUS dec21x40Send
    (
    DRV_CTRL	*pDrvCtrl,
    M_BLK	*pMblk
    )
    {
    DEC_TD *	pTxD;
    char *	pBuf;
    int		len;
    int         s;
    DRV_LOG (DRV_DEBUG_TX, "S:0x%x ", pDrvCtrl->txIndex, 0, 0, 0, 0, 0);

    /* Gain exclusive access to transmit */

    END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);

    /* Get the next TXD */

    pTxD = dec21x40TxDGet (pDrvCtrl);
    pBuf = NET_BUF_ALLOC();

    if ((pTxD == NULL) || (pBuf == NULL))
        {
	DRV_LOG (DRV_DEBUG_TX, "No available TxBufs \n", 0, 0, 0, 0, 0, 0);

#ifdef INCLUDE_RFC_1213

        /* Old RFC 1213 mib2 interface */

        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
#else        

        /* New RFC 2233 mib2 interface */

        if (pDrvCtrl->endObj.pMib2Tbl != NULL)
            {
            pDrvCtrl->endObj.pMib2Tbl->m2CtrUpdateRtn(pDrvCtrl->endObj.pMib2Tbl,
                                                      M2_ctrId_ifOutErrors, 1);
            }
#endif
        END_TX_SEM_GIVE (&pDrvCtrl->endObj);

        if (pBuf)
            NET_BUF_FREE (pBuf);

        if (!pDrvCtrl->txCleaning && !pDrvCtrl->txBlocked)
            dec21x40TxRingClean (pDrvCtrl);
        

        s = intLock();
        pDrvCtrl->txBlocked = TRUE;        /* transmitter not ready */
        intUnlock(s);

        return (END_ERR_BLOCK); /* just return without freeing mBlk chain */
        }

    /* copy and free the MBLK */

    len = netMblkToBufCopy (pMblk, pBuf, NULL);
    NET_MBLK_CHAIN_FREE (pMblk);

    /* setup the transmit buffer pointers */

    pTxD->tDesc2 = PCISWAP (DEC_VIRT_TO_PCI (pBuf));
    pTxD->tDesc3 = 0;

    /* setup frame len */

    pTxD->tDesc1 &= PCISWAP (~TDESC1_TBS1_MSK);
    pTxD->tDesc1 |= PCISWAP (TDESC1_TBS1_PUT(len));

    /* transfer ownership to device */

    pTxD->tDesc0  = PCISWAP(TDESC0_OWN);      

    CACHE_PIPE_FLUSH();

    /* Save the buf info */

    pDrvCtrl->freeBuf[pDrvCtrl->txIndex].pClBuf    = pBuf;

    /* Advance our management index */

    pDrvCtrl->txIndex = (pDrvCtrl->txIndex + 1) % pDrvCtrl->numTds;

    if (DRV_FLAGS_ISSET(DEC_TX_KICKSTART))
        DEC_CSR_WRITE (CSR1, CSR1_TPD);

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

#ifdef INCLUDE_RFC_1213

    /* Old RFC 1213 mib2 interface */

    /* update statistics */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);
    
#endif /* INCLUDE_RFC_1213 */

    return (OK);
    }

/***************************************************************************
*
* dec21x40Recv - pass a received frame to the upper layer
*
*
* RETURNS: OK, always.
*/

LOCAL STATUS dec21x40Recv
    (
    DRV_CTRL *	pDrvCtrl,
    DEC_RD *	pRxD
    )
    {
    END_OBJ *	pEndObj = &pDrvCtrl->endObj;
    M_BLK_ID	pMblk;      /* MBLK to send upstream */
    CL_BLK_ID	pClBlk;	    /* pointer to clBlk */
    char *	pBuf; 	    /* A replacement buffer for the current RxD */
    char *	pData;      /* Data pointer for the current RxD */
    char *	pTmp;
    int     	len;	    /* Len of the current data */

    DRV_LOG (DRV_DEBUG_RX, ("R"), 0, 0, 0, 0, 0, 0);
    
    /* check for errors */

    if (pRxD->rDesc0 & PCISWAP(RDESC0_ES))
        {
        DRV_LOG (DRV_DEBUG_RX, ("- "), 0, 0, 0, 0, 0, 0);
#ifdef INCLUDE_RFC_1213

        /* Old RFC 1213 mib2 interface */

        END_ERR_ADD (pEndObj, MIB2_OUT_ERRS, +1);
        
#else

        /* New RFC 2233 mib2 interface */

        if (pDrvCtrl->endObj.pMib2Tbl != NULL)
            {
            pDrvCtrl->endObj.pMib2Tbl->m2CtrUpdateRtn(pDrvCtrl->endObj.pMib2Tbl,
                                                      M2_ctrId_ifInErrors, 1);
            }
#endif /* INCLUDE_RFC_1213 */

        goto cleanRxD;
        }

    DRV_LOG (DRV_DEBUG_RX, ("+ "), 0, 0, 0, 0, 0, 0);
#ifdef INCLUDE_RFC_1213

    /* Old RFC 1213 mib2 interface */

    END_ERR_ADD (pEndObj, MIB2_IN_UCAST, +1);
    
#endif /* INCLUDE_RFC_1213 */

    /* Allocate an MBLK, and a replacement buffer */

    pMblk = NET_MBLK_ALLOC();
    pBuf  = NET_BUF_ALLOC();
    pClBlk = NET_CL_BLK_ALLOC(); 

    if ((pMblk == NULL) || (pBuf == NULL) || (pClBlk == NULL))
        {
	DRV_LOG (DRV_DEBUG_RX, "No available RxBufs \n", 0, 0, 0, 0, 0, 0);

#ifdef INCLUDE_RFC_1213

        /* Old RFC 1213 mib2 interface */

        END_ERR_ADD (pEndObj, MIB2_IN_ERRS, +1);
        
#else

        /* New RFC 2233 mib2 interface */

        if (pDrvCtrl->endObj.pMib2Tbl != NULL)
            {
            pDrvCtrl->endObj.pMib2Tbl->m2CtrUpdateRtn(pDrvCtrl->endObj.pMib2Tbl,
                                                      M2_ctrId_ifInErrors, 1);
            }
#endif /* INCLUDE_RFC_1213 */

        if (pMblk)
            NET_MBLK_FREE (pMblk);
        if (pBuf)
            NET_BUF_FREE (pBuf);
        if (pClBlk)
            NET_CL_BLK_FREE (pClBlk);
        goto cleanRxD;
        }

    /* Get the data pointer and len from the current RxD */

    len   = DEC_FRAME_LEN_GET (PCISWAP (pRxD->rDesc0)) - ETH_CRC_LEN; 
    pData = (char *) PCISWAP (pRxD->rDesc3); /* buff virtual address */

    /*
     * The code above solves alignment problem when the CPU and the
     * ethernet chip don't accept longword unaligned addresses.
     * 
     * Pb: When the ethernet chip receives a packet from the network,
     * it needs a longword aligned buffer to copy the data. To process the
     * IP packet, MUX layer adds a SIZEOF_ETHERHEADER(0x14) offset to the
     * data buffer. So the CPU obtains a longword unaligned buffer and
     * a fault execption occurs when it reads "ip_src" and "ip_dst" fields 
     * in the IP structure (ip_src and ip_dst fields are longwords).
     *
     * The problem is solved copying the data in a new buffer in which
     * the IP structure is aligned.
     */  

    if (pDrvCtrl->offset != 0)
	{
	/* exchange buffer addresses */

	pTmp = pData;
	pData = pBuf;
	pBuf = pTmp;

	/* copy data in new unaligned buffer */
 
	pData += pDrvCtrl->offset;
	memcpy(pData, pBuf, len);
        }

    /* Restore orignal buffer address to be able to free it */
 
    pData -= pDrvCtrl->offset; 

#ifndef INCLUDE_RFC_1213

    /* New RFC 2233 mib2 interface */

    /* RFC 2233 mib2 counter update for incoming packet */

    if (pDrvCtrl->endObj.pMib2Tbl != NULL)
        {
        pDrvCtrl->endObj.pMib2Tbl->m2PktCountRtn(pDrvCtrl->endObj.pMib2Tbl,
                                                 M2_PACKET_IN, pData, len);
        }
#endif /* INCLUDE_RFC_1213 */

    /* Associate the data pointer with the MBLK */

    NET_CL_BLK_JOIN (pClBlk, pData, DEC_BUFSIZ);
    
    /* Associate the data pointer with the MBLK */

    NET_MBLK_CL_JOIN (pMblk, pClBlk);

    pMblk->mBlkHdr.mData += pDrvCtrl->offset; 
    pMblk->mBlkHdr.mFlags 	|= M_PKTHDR;	/* set the packet header */
    pMblk->mBlkHdr.mLen		= len;		/* set the data len */
    pMblk->mBlkPktHdr.len 	= len;		/* set the total len */

    pDrvCtrl->rxLen += len;

    /* Make cache consistent with memory */

    DEC_CACHE_INVALIDATE (pData, len);

    /* Install the new data buffer */

    pRxD->rDesc2 = PCISWAP (DEC_VIRT_TO_PCI (pBuf));
    pRxD->rDesc3 = PCISWAP ((ULONG)pBuf); /* virtual buffer address */

    /* mark the descriptor ready to receive */

    pRxD->rDesc0 = PCISWAP (RDESC0_OWN);
    
    /* advance management index */

    pDrvCtrl->rxIndex = (pDrvCtrl->rxIndex + 1) % pDrvCtrl->numRds;

    CACHE_PIPE_FLUSH();

    /* send the frame to the upper layer, and possibly switch to system mode */

    END_RCV_RTN_CALL (pEndObj, pMblk);

    return (OK);

cleanRxD:

    /* mark the descriptor ready to receive */

    pRxD->rDesc0 = PCISWAP (RDESC0_OWN);
    
    CACHE_PIPE_FLUSH();

    /* advance management index */

    pDrvCtrl->rxIndex = (pDrvCtrl->rxIndex + 1) % pDrvCtrl->numRds;

    return (OK);
    }

/***************************************************************************
*
* dec21x40RxIntHandle - perform receive processing
*
* RETURNS: N/A
*/

LOCAL void dec21x40RxIntHandle
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    DEC_RD	*pRxD;

    pDrvCtrl->rxHandling = TRUE;

    pDrvCtrl->rxLen = 0;
   
    while ((pRxD = dec21x40RxDGet (pDrvCtrl)) && 
            (pDrvCtrl->rxLen < pDrvCtrl->rxMaxLen))
            dec21x40Recv (pDrvCtrl, pRxD);

    if (dec21x40RxDGet (pDrvCtrl) != NULL)
        {
        pDrvCtrl->rxMaxLen = (TCP_MSS * 30); /* get more packets next time */
        netJobAdd ((FUNCPTR)dec21x40RxIntHandle, (int)pDrvCtrl, 0, 0, 0, 0);
        }
    else
        {
        pDrvCtrl->rxMaxLen = RWIN;
        DEC_CSR_UPDATE (CSR7, CSR7_RIM);     /* turn on Rx interrupts */
        pDrvCtrl->rxHandling = FALSE;
        }
    }

/***************************************************************************
*
* dec21x40RxDGet - get a ready receive descriptor
*
* RETURNS: a filled receive descriptor, otherwise NULL.
*/

LOCAL DEC_RD * dec21x40RxDGet
    (
    DRV_CTRL	*pDrvCtrl
    )
    {
    DEC_RD	*pRxD = pDrvCtrl->rxRing + pDrvCtrl->rxIndex;
    
    DEC_CACHE_INVALIDATE (pRxD, RD_SIZ);

    /* check if the descriptor is owned by the chip */

    if (pRxD->rDesc0 & PCISWAP (RDESC0_OWN))
        return NULL;

    return pRxD;
    }

/***************************************************************************
*
* dec21x40TxDGet - get an available transmit descriptor
*
* RETURNS: an available transmit descriptor, otherwise NULL.
*/

LOCAL DEC_TD * dec21x40TxDGet
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    DEC_TD	*pTxD = pDrvCtrl->txRing + pDrvCtrl->txIndex;

    DEC_CACHE_INVALIDATE (pTxD, TD_SIZ);

    /* check if this descriptor is owned by the chip or is out of bounds */

    if ((pTxD->tDesc0 & PCISWAP(TDESC0_OWN)) ||
        (((pDrvCtrl->txIndex + 1) % pDrvCtrl->numTds) == pDrvCtrl->txDiIndex))
        return (NULL);

    return pTxD;
    }

/***************************************************************************
*
* dec21x40TxRingClean - clean up processed tx descriptors
*
* This routine processes the transmit queue, freeing all transmitted
* descriptors.
*
* RETURNS: None
*/
LOCAL void dec21x40TxRingClean
    (
    DRV_CTRL	*pDrvCtrl
    )
    {
    DEC_TD	*pTxD;

    pDrvCtrl->txCleaning = TRUE;
    
    while (pDrvCtrl->txDiIndex != pDrvCtrl->txIndex)
        {
        pTxD = pDrvCtrl->txRing + pDrvCtrl->txDiIndex;
        DEC_CACHE_INVALIDATE (pTxD, TD_SIZ);

        if (pTxD->tDesc0 & PCISWAP(TDESC0_OWN))
            break;

        DRV_LOG (DRV_DEBUG_TX, "Tc:0x%x ", pDrvCtrl->txDiIndex, 0, 0, 0, 0, 0);

        /* free the buffer */

        if (pDrvCtrl->freeBuf[pDrvCtrl->txDiIndex].pClBuf != NULL)
            {
            NET_BUF_FREE(pDrvCtrl->freeBuf[pDrvCtrl->txDiIndex].pClBuf);
            pDrvCtrl->freeBuf[pDrvCtrl->txDiIndex].pClBuf = NULL;
            }
        
        pDrvCtrl->txDiIndex =  (pDrvCtrl->txDiIndex + 1) % pDrvCtrl->numTds;

        /* clear frame-type of a setup frame and process errors for others */
        if (pTxD->tDesc1 & PCISWAP(TDESC1_SET))
            {
            pTxD->tDesc1 &= PCISWAP(~(TDESC1_SET | TDESC1_FT0));
	    CACHE_PIPE_FLUSH();
            }
        else if (pTxD->tDesc0 & PCISWAP(TDESC0_ES))
            {
#ifdef INCLUDE_RFC_1213

            /* Old RFC 1213 mib2 interface */

            END_OBJ     *pEndObj = &pDrvCtrl->endObj;
            END_ERR_ADD (pEndObj, MIB2_OUT_ERRS, +1);
            END_ERR_ADD (pEndObj, MIB2_OUT_UCAST, -1);
#else            

            /* New RFC 2233 mib2 interface */

            if (pDrvCtrl->endObj.pMib2Tbl != NULL)
                {
                pDrvCtrl->endObj.pMib2Tbl->m2CtrUpdateRtn(pDrvCtrl->endObj.
                                                                    pMib2Tbl,
                                                          M2_ctrId_ifOutErrors,
                                                          1);
                }

#endif /* INCLUDE_RFC_1213 */

           /*
            * NOTE: NC and LO TxDesc bits not valid in internal
            * loop-back mode (CSR6_OM_IL). 
            */
            
	   /* we dont have MII phy on a 21145 */

	   if( (pDrvCtrl->usrFlags & DEC_USR_21145) == 0)
	       {
	       if ( !(DEC_CSR_READ(CSR6) & PCISWAP(CSR6_OM_IL)) &&
		    (pTxD->tDesc0 & PCISWAP(TDESC0_NC|TDESC0_LO)) )
		   {

		   /* check for no carrier */

		   if ((pTxD->tDesc0 & PCISWAP(TDESC0_NC)) &&
		       !(pDrvCtrl->pPhyInfo->phyFlags & MII_PHY_FD))
		       {
		       DRV_LOG (DRV_DEBUG_ALL, "%s%d - no carrier\n",
				(int)DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
		       }
		   if ((pTxD->tDesc0 & PCISWAP(TDESC0_LO)) &&
		       !(pDrvCtrl->pPhyInfo->phyFlags & MII_PHY_FD))
		       {
		       DRV_LOG (DRV_DEBUG_ALL, "%s%d - loss of carrier\n",
			        (int)DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
		       }
		   dec21x40MediaChange (pDrvCtrl);
		   }
	       }
	   
	   /*
            * check for link failure (LF)
	    * NOTE: LF in invalid in HomePNA mode
            */

           if ( !(DEC_CSR_READ(CSR6) & PCISWAP(CSR6_OM_IL)) &&
               (pTxD->tDesc0 & PCISWAP(TDESC0_LF)) )
                {
		if((pDrvCtrl->usrFlags & DEC_USR_21145) == 0)
		    {
		    DRV_LOG (DRV_DEBUG_ALL, "%s%d - link fail\n",
		             (int)DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
		    dec21x40MediaChange (pDrvCtrl);
		    }

		/* we are a 21145, make sure we are not in HomePNA mode */

		else if( (DEC_CSR_READ(CSR13) & CSR13_AUI_TP) == 0)
		    {
		    DRV_LOG (DRV_DEBUG_ALL, "%s%d - link fail\n",
		             (int)DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
		    dec21x40MediaChange (pDrvCtrl);
		    }
		else if( pDrvCtrl->usrFlags & DEC_USR_HPNA_PREFER_10BT)
		    {

		    /*
                     * we are in HOMEPNA mode and usr flags are set to
		     * prefer 10BT so we must
		     * restart autonegotiation
                     */

	            DEC_CSR_WRITE(CSR12, (DEC_CSR_READ(CSR12) & 
				  ~CSR12_AN_MASK) | 
				  CSR12_AN_TX_DISABLED);
		    }
                }

            /* restart if DMA underflow is detected */
            
            if (pTxD->tDesc0 & PCISWAP(TDESC0_UF))
                {
                DRV_LOG (DRV_DEBUG_ALL, "%s%d - fatal DMA underflow\n",
                           (int)DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);

                dec21x40Stop (pDrvCtrl);
                pDrvCtrl->txCleaning = FALSE;
                netJobAdd ((FUNCPTR)dec21x40Restart, (int)pDrvCtrl, 0, 0, 0, 0);
                return;
                }
            }

#ifdef DRV_DEBUG
	    {
	    UINT32 debugStat = pTxD->tDesc0 & PCISWAP (0x0000cf87);

	    if (debugStat & (PCISWAP(TDESC0_ES | 
		TDESC0_LF | TDESC0_DE))) 
		{
		decDescErrors++;
		}
	    }
#endif /* DRV_DEBUG */


        pTxD->tDesc0 = 0;                    /* clear errors and stats */
	CACHE_PIPE_FLUSH();
        }

    pDrvCtrl->txCleaning = FALSE;

    return;
    }

/***************************************************************************
*
* dec21x40Int - handle device interrupts
*
* This interrupt service routine, reads device interrupt status, acknowledges
* the interrupting events, and schedules receive and transmit processing when
* required.
*
* RETURNS: None
*/

LOCAL void dec21x40Int
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    ULONG	status;

    CACHE_PIPE_FLUSH();

    /* get the status bits and clear the interrupts */

    status = DEC_CSR_READ (CSR5);

    while (status & (pDrvCtrl->intrMask))
	{
	DEC_CSR_WRITE (CSR5, status);

	DRV_LOG (DRV_DEBUG_INT, "i=0x%x:\n", status, 0, 0, 0, 0, 0);

	/* Check for autonego/link pass int 21145 */
	/* if HPNA_PREFER_10BT is set and */
	/*  we are in HOME_PNA mode */

	if ((status & CSR5_ANC) &&
	    DRV_FLAGS_ISSET(DEC_21145) && 
	     (pDrvCtrl->usrFlags & DEC_USR_HPNA_PREFER_10BT) &&
	     (DEC_CSR_READ(CSR13) & CSR13_AUI_TP)) 
	    {
	    DRV_LOG (DRV_DEBUG_INT, "anc_int: %lx\n", DEC_CSR_READ(CSR12), 
		     0, 0, 0, 0, 0);
	    dec21x40Stop (pDrvCtrl);
	    netJobAdd ((FUNCPTR)dec21x40Restart, (int)pDrvCtrl, 0, 0, 0, 0);

	    }

	if ((status & CSR5_21040_LNF) &&
	    DRV_FLAGS_ISSET(DEC_21145) && 
	     (pDrvCtrl->usrFlags & DEC_USR_HPNA_PREFER_10BT) &&
	     ( (DEC_CSR_READ(CSR13) & CSR13_AUI_TP) == 0)) 
	    {
	    DRV_LOG (DRV_DEBUG_INT, "lnk_fail_int: %lx\n", DEC_CSR_READ(CSR12), 
		     0, 0, 0, 0, 0);
	    dec21x40Stop (pDrvCtrl);
	    netJobAdd ((FUNCPTR)dec21x40Restart, (int)pDrvCtrl, 0, 0, 0, 0);

	    }

	/* return on false interrupt */

	if ((status & (CSR5_NIS | CSR5_AIS)) == 0)
	    {
	    DRV_LOG (DRV_DEBUG_INT, "false intr\n", 0, 0, 0, 0, 0, 0);
	    return;
	    }

	/* Check for system error */

	if (status & CSR5_SE)
	    {
	    DRV_LOG (DRV_DEBUG_INT, "E", 0, 0, 0, 0, 0, 0);
	    DRV_LOG (DRV_DEBUG_INT, "%s%d - fatal system error\n",
		    (int)DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
	    dec21x40Stop (pDrvCtrl);
	    netJobAdd ((FUNCPTR) dec21x40Restart, (int)pDrvCtrl, 0, 0, 0, 0);
	    return;
	    }


	/* Handle received packets */

	if ((status & CSR5_RI) && (! (status & CSR5_RPS)) &&
	    (!pDrvCtrl->rxHandling))
	    {
	    DRV_LOG (DRV_DEBUG_INT, "r ", 0, 0, 0, 0, 0, 0);

	    pDrvCtrl->rxHandling = TRUE;

	    netJobAdd ((FUNCPTR)dec21x40RxIntHandle, (int)pDrvCtrl, 0, 0, 0, 0);
	    
	    /* disable Rx intr; re-enable in dec21x40RxIntHandle() */

	    DEC_CSR_RESET (CSR7, CSR7_RIM);
	    }

#ifdef DRV_DEBUG
	if (status & CSR5_TI)
	    decTxInts++;
	if (status & CSR5_RI)
	    decRxInts++;
	if (status & (CSR5_TPS | CSR5_TU | CSR5_TJT | CSR5_UNF))
	    decTxErrors++;
	if (status & (CSR5_RU | CSR5_RPS | CSR5_RWT))
	    decRxErrors++;
	if (status & (CSR5_TPS))
	    decTxTpsErrors++;
	if (status & (CSR5_TU))
	    decTxBufErrors++;
	if (status & (CSR5_TJT))
	    decTxTjtErrors++;
	if (status & (CSR5_UNF))
	    decTxUnfErrors++;
	if (status & (CSR5_RU))
	    decRxBufErrors++;
	if (status & (CSR5_RPS))
	    decRxRpsErrors++;
	if (status & (CSR5_RWT))
	    decRxWtErrors++;

#endif
	
	/* Cleanup TxRing */

	if ((status & CSR5_TI) && (!pDrvCtrl->txCleaning))
	    {
	    DRV_LOG (DRV_DEBUG_INT, "t ", 0, 0, 0, 0, 0, 0);
	    pDrvCtrl->txCleaning = TRUE;
	    dec21x40TxRingClean (pDrvCtrl);
	    }

	if (_func_dec2114xIntAck != NULL)
	    {
	    /*
	     * Call BSP specific interrupt ack. routine, if defined.
	     * This must be called after all the interrupt  sources
	     * have been cleared
	     */

	    (* _func_dec2114xIntAck) ();
	    }
	
	if (pDrvCtrl->txBlocked)
	    {
	    pDrvCtrl->txBlocked = FALSE;
	    netJobAdd ((FUNCPTR)muxTxRestart, (int)&pDrvCtrl->endObj, 0, 0,
		       0, 0);
	    }

	status = DEC_CSR_READ (CSR5); /* Get more interrupt status bits */
	}
    return;
    }

/***************************************************************************
*
* dec21x40PollStart - starting polling mode
*
* RETURNS: OK, always.
*/

LOCAL STATUS dec21x40PollStart
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    int		intLevel;

    DRV_LOG (DRV_DEBUG_POLL, "S ", 0, 0, 0, 0, 0, 0);

    intLevel = intLock();

    DEC_CSR_RESET (CSR7, CSR7_NIM | CSR7_AIM);

    DRV_FLAGS_SET (DEC_POLLING);

    intUnlock (intLevel);

    return (OK);
    }

/***************************************************************************
*
* dec21x40PollStop - stop polling mode
*
* RETURNS: OK, always.
*/

LOCAL STATUS dec21x40PollStop
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    int	intLevel;

    intLevel = intLock();

    DEC_CSR_UPDATE (CSR7, (CSR7_NIM | CSR7_AIM));

    DRV_FLAGS_CLR (DEC_POLLING);

    intUnlock (intLevel);

    DRV_LOG (DRV_DEBUG_POLL, "s", 0, 0, 0, 0, 0, 0);

    return (OK);
    }

/***************************************************************************
*
* dec21x40PollSend - send a packet in polled mode
*
* RETURNS: OK on success, EAGAIN on failure
*/

LOCAL STATUS dec21x40PollSend
    (
    DRV_CTRL *		pDrvCtrl,
    M_BLK *		pMblk
    )
    {
    DEC_TD	*pTxD;
    char *	pBuf;
    int		len;

    pTxD = dec21x40TxDGet (pDrvCtrl); 
    pBuf = pDrvCtrl->txPollSendBuf;

    /* copy and free the MBLK */

    len = netMblkToBufCopy (pMblk, pBuf, NULL);

#ifndef INCLUDE_RFC_1213
 
    /* New RFC 2233 mib2 interface */

    /* RFC 2233 mib2 counter update for outgoing packet */

    if (pDrvCtrl->endObj.pMib2Tbl != NULL)
        {
        pDrvCtrl->endObj.pMib2Tbl->m2PktCountRtn(pDrvCtrl->endObj.pMib2Tbl,
                                                 M2_PACKET_OUT, pBuf, len);
        }
#endif /* INCLUDE_RFC_1213 */

    pTxD->tDesc2 = PCISWAP (DEC_VIRT_TO_PCI (pBuf));
    pTxD->tDesc3 = 0;

    /* Associate the data pointer with the MBLK */
    /* setup descriptor fields for tansmit */

    pTxD->tDesc0 = 0;
    pTxD->tDesc1 &= PCISWAP (~TDESC1_TBS1_MSK);
    pTxD->tDesc1 |= PCISWAP (TDESC1_TBS1_PUT(len));
    pTxD->tDesc0 = PCISWAP(TDESC0_OWN);      /* ready for transmit */

    CACHE_PIPE_FLUSH();

    /* Advance our management index */

    pDrvCtrl->txIndex = (pDrvCtrl->txIndex + 1) % pDrvCtrl->numTds;

    if (DRV_FLAGS_ISSET(DEC_TX_KICKSTART))
        DEC_CSR_WRITE (CSR1, CSR1_TPD);

    /* The buffer does not belong to us; Spin until it can be freed */

    while (pTxD->tDesc0 & PCISWAP(TDESC0_OWN))
        ;

    /* Try again on transmit errors */

    if (pTxD->tDesc0 & PCISWAP(TDESC0_ES))
        return (EAGAIN);

    return (OK);
    }

/***************************************************************************
*
* dec21x40PollReceive - get a packet in polled mode
*
* RETURNS: OK on success, EAGAIN on failure.
*/

LOCAL STATUS dec21x40PollReceive
    (
    DRV_CTRL	*pDrvCtrl,
    M_BLK 	*pMblk
    )
    {
    DEC_RD	*pRxD;
    char	*pRxBuf;
    int		len;
    BOOL	gotOne=FALSE;

    if ((pMblk->mBlkHdr.mFlags & M_EXT) != M_EXT)
        return (EAGAIN);

    while (((pRxD = dec21x40RxDGet (pDrvCtrl)) != NULL) && !gotOne)
        {

        /* Check if the packet was received OK */

        if (pRxD->rDesc0 & PCISWAP (RDESC0_ES))
            {
            /* Update the error statistics and discard the packet */
#ifdef INCLUDE_RFC_1213

            /* Old RFC 1213 mib2 interface */

            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
#else            

            /* New RFC 2233 mib2 interface */

            if (pDrvCtrl->endObj.pMib2Tbl != NULL)
                {
                pDrvCtrl->endObj.pMib2Tbl->m2CtrUpdateRtn(pDrvCtrl->endObj.
                                                                    pMib2Tbl,
                                                          M2_ctrId_ifInErrors,
                                                          1);
                }
#endif /* INCLUDE_RFC_1213 */
            }
        else
            {
            len = DEC_FRAME_LEN_GET (PCISWAP(pRxD->rDesc0)) - ETH_CRC_LEN;

            if (pMblk->mBlkHdr.mLen >= len)
                {
                gotOne = TRUE;

#ifdef INCLUDE_RFC_1213

                /* Old RFC 1213 mib2 interface */

                END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);

#endif /* INCLUDE_RFC_1213 */

                /* Deal with memory alignment */

		if (((int) pMblk->mBlkHdr.mData & 0x3) == 0)
                    pMblk->mBlkHdr.mData += pDrvCtrl->offset;
 
		pMblk->mBlkHdr.mFlags |= M_PKTHDR; /* set the packet header */
		pMblk->mBlkHdr.mLen = len;	   /* set the data len */
		pMblk->mBlkPktHdr.len = len;	   /* set the total len */

                pRxBuf = (char *) PCISWAP (pRxD->rDesc3); /* virt address */

#ifndef INCLUDE_RFC_1213

                /* RFC 2233 mib2 counter update for incoming packet */

                if (pDrvCtrl->endObj.pMib2Tbl != NULL)
                    {
                    pDrvCtrl->endObj.pMib2Tbl->m2PktCountRtn(pDrvCtrl->endObj.
                                                                       pMib2Tbl,
                                                             M2_PACKET_IN,
                                                             pRxBuf, len);
                    }
#endif /* INCLUDE_RFC_1213 */ 

                DEC_CACHE_INVALIDATE (pRxBuf, len);
                bcopy (pRxBuf, (char *)pMblk->mBlkHdr.mData, len);
                }
            }

        pRxD->rDesc0 = PCISWAP (RDESC0_OWN);

        CACHE_PIPE_FLUSH();

        pDrvCtrl->rxIndex = (pDrvCtrl->rxIndex + 1) % pDrvCtrl->numRds;
        }

    return (gotOne ? OK : EAGAIN);
    }

/***************************************************************************
*
* dec21x40CsrRead - read a Command and Status Register
*
* RETURNS: contents of the CSR register.
*/

LOCAL ULONG dec21x40CsrRead
    (
    DRV_CTRL *	pDrvCtrl,	             /* device control */
    int		reg                          /* register to read */
    )
    {
    ULONG	*csrReg;
    ULONG	csrData;
    
    csrReg = (ULONG *)(pDrvCtrl->devAdrs + (reg * DECPCI_REG_OFFSET));
    csrData = *csrReg;

    return REGSWAP(csrData);
    }

/***************************************************************************
*
* dec21x40CsrWrite - write a Command and Status Register
*
* RETURNS: None
*/

LOCAL void dec21x40CsrWrite
    (
    DRV_CTRL *	pDrvCtrl,	             /* device control */
    int		reg,
    ULONG	value
    )
    {
    ULONG	*csrReg;

    csrReg = (ULONG *)(pDrvCtrl->devAdrs + (reg * DECPCI_REG_OFFSET));
    *csrReg = REGSWAP (value);

    return;
    }

/***************************************************************************
*
* dec21040AuiTpInit - initialize either AUI or 10BASE-T connection
*
* This function configures 10BASE-T interface. If the link pass state is 
* not achieved within two seconds then the AUI interface is initialized.
*/

LOCAL void dec21040AuiTpInit
    (
    DRV_CTRL *	pDrvCtrl
    )
    {

    /* reset the SIA registers */

    DEC_CSR_WRITE (CSR13, 0);
    DEC_CSR_WRITE (CSR14, 0);
    DEC_CSR_WRITE (CSR15, 0);

    /* configure 10BASE-T */

    DEC_CSR_WRITE (CSR13, CSR13_CAC_CSR);    /* 10BT auto configuration */

    taskDelay (sysClkRateGet() * 2);         /* 2 second delay */

    /* configure AUI if 10BASE-T is not connected */

    if (DEC_CSR_READ (CSR12) & (CSR12_21040_LKF | CSR12_21040_NCR))
	{

        /* reset SIA registers */

	DEC_CSR_WRITE (CSR13, 0);            
	DEC_CSR_WRITE (CSR14, 0);
	DEC_CSR_WRITE (CSR15, 0);

	/* AUI auto configuration */

	DEC_CSR_WRITE (CSR13, (CSR13_AUI_TP | CSR13_CAC_CSR));
	}

    return;
    }

/***************************************************************************
*
* dec21x40EnetAddrGet - gets the ethernet address
*
* This routine gets the ethernet address by calling the appropriate
* EnetAddrGet() routine.
* 
* RETURNS: OK/ERROR
*
* SEE ALSO: dec20140EnetAddrGet(), dec21140EnetAddrGet(),
* sysDec21x40EnetAddrGet()
*/

LOCAL STATUS dec21x40EnetAddrGet
    (
    DRV_CTRL *	pDrvCtrl,
    char *	enetAdrs
    )
    {
    int retVal=ERROR;

    if (! DRV_FLAGS_ISSET (DEC_BSP_EADRS))
        {
        if (DRV_FLAGS_ISSET (DEC_21040))
            retVal=dec21040EnetAddrGet (pDrvCtrl, enetAdrs);
        else
            retVal=dec21140EnetAddrGet (pDrvCtrl, enetAdrs);
        }
    
    if (retVal == ERROR)
        retVal = sysDec21x40EnetAddrGet (pDrvCtrl->unit, enetAdrs);

    return retVal;
    }


/***************************************************************************
*
* dec21040EnetAddrGet - gets the ethernet address from the ROM register
*
* This routine gets the ethernet address from the ROM register.
* This routine returns the ethernet address into the pointer supplied to it.
* 
* RETURNS: OK/ERROR
*/

LOCAL STATUS dec21040EnetAddrGet
    (
    DRV_CTRL *	pDrvCtrl,
    char * 	enetAdrs	/* pointer to the ethernet address */ 
    )
    {
    FAST ULONG	csr9Value;		/* register to hold CSR9 */
    BOOL	eRomReady = FALSE;	/* ethernet ROM register state */
    int		ix;			/* index register */
    int		len = EADDR_LEN;

    DEC_CSR_WRITE (CSR9, 0);	/* reset rom pointer */

    while (len > 0)
	{
	for (ix = 0; ix < 10; ix++)	/* try at least 10 times */
	    {
	    if ((csr9Value = DEC_CSR_READ (CSR9)) & CSR9_21040_DNV)
		{
		eRomReady = FALSE;
	    	DEC_NSDELAY(125);
		}
	    else
		{
		*enetAdrs++ = (UCHAR) csr9Value;
		len--;
		eRomReady = TRUE;
		break;
		}
	    }

        if (!eRomReady)
            return (ERROR);
	}

    return (OK);
    }

/***************************************************************************
*
* dec21140EnetAddrGet - gets the ethernet address from the ROM register 
*
* This routine reads an ethernet address from the serial ROM. It supports
* legacy, ver 1/A, and 3.0 serial ROM formats.
* 
* RETURNS: OK on success, and ERROR if the ethernet address bytes cannot be
* read.
*/

LOCAL STATUS dec21140EnetAddrGet
    (
    DRV_CTRL *	pDrvCtrl,
    char * 	enetAdrs	/* pointer to the ethernet address */ 
    )
    {
    USHORT	sromData;
    int		adrsOffset;
    int		len;

    /* Check if SROM is programmed. */

    sromData = dec21140SromWordRead (pDrvCtrl, 0);
    if ( sromData == 0xFFFF ) 
	return (ERROR);

    sromData = dec21140SromWordRead (pDrvCtrl, 13);

    /*
     * Set MAC address offset from the ROM format.
     * Legacy ROMs have ethernet address start at offset 0,
     * while the rest (ver 1/A and 3.0) have it at byte offset 20.
     */

    adrsOffset = ((sromData == 0xAA55) ||
		  (sromData == 0xFFFF)) ? 0: 10;

    for (len=EADDR_LEN; len; len-=2, adrsOffset++)
        {
        sromData = dec21140SromWordRead (pDrvCtrl, adrsOffset);

        /* byte swap it for little endian */

        sromData = LE_BYTE_SWAP(sromData);

        *enetAdrs++ = MSB(sromData);
        *enetAdrs++ = LSB(sromData);
        }

    return (OK);
    }

/***************************************************************************
*
* dec21140SromWordRead - read two bytes from the serial ROM
*
* This routine returns the two bytes of information that is associated 
* with it the specified ROM line number.  This will later be used by the 
* dec21140GetEthernetAdr function.  It can also be used to 
* review the ROM contents itself.
* The function must first send some initial bit patterns to the CSR9 that 
* contains the Serial ROM Control bits.  Then the line index into the ROM
* is evaluated bit-by-bit to program the ROM.  The 2 bytes of data
* are extracted and processed into a normal pair of bytes. 
* 
* RETURNS: Value from ROM or ERROR.
*/

USHORT dec21140SromWordRead
    (
    DRV_CTRL *	pDrvCtrl,
    UCHAR	lineCnt 	/* Serial ROM line Number */ 
    )
    {
    int		ix;		/* index register */
    int		loop;           /* address loop counter */
    ULONG	adrsBase;
    USHORT	romData;

    /* Is the line offset valid, and if so, how large is it */

    if (lineCnt > DEC21140_SROM_SIZE)
    	return (ERROR);

    if (lineCnt < 64)
    	{
	loop = 6;
	lineCnt = lineCnt << 2;  /* Prepare lineCnt for processing */
    	}
    else
	loop = 8;

    /* Command the Serial ROM to the correct Line */

    /* Preamble */

    DEC_SROM_CMD_WRITE (0x0, 30);           /* Command 1 */
    DEC_SROM_CMD_WRITE (0x1, 50);           /* Command 2 */
    DEC_SROM_CMD_WRITE (0x3, 250);          /* Command 3 */
    DEC_SROM_CMD_WRITE (0x1, 150);          /* Command 4 */

    /* Command Phase */

    DEC_SROM_CMD_WRITE (0x5, 150);          /* Command 5 */
    DEC_SROM_CMD_WRITE (0x7, 250);          /* Command 6 */
    DEC_SROM_CMD_WRITE (0x5, 250);          /* Command 7 */
    DEC_SROM_CMD_WRITE (0x7, 250);          /* Command 8 */
    DEC_SROM_CMD_WRITE (0x5, 100);          /* Command 9 */
    DEC_SROM_CMD_WRITE (0x1, 150);          /* Command 10 */
    DEC_SROM_CMD_WRITE (0x3, 250);          /* Command 11 */
    DEC_SROM_CMD_WRITE (0x1, 100);          /* Command 12 */
    
    /* Address Phase */

    for (ix=0; ix < loop; ix++)
    	{
	adrsBase = ((lineCnt & 0x80) >> 5); 

	/* Write the command */

        DEC_SROM_CMD_WRITE (adrsBase | 0x1, 150);  /* Command 13 */
        DEC_SROM_CMD_WRITE (adrsBase | 0x3, 250);  /* Command 14 */
        DEC_SROM_CMD_WRITE (adrsBase | 0x1, 100);  /* Command 15 */

	lineCnt = lineCnt<<1;
    	}

    DEC_NSDELAY (150);

    /* Data Phase */

    romData = 0;
    for (ix=15; ix >= 0; ix--)
    	{

	/* Write the command */

        DEC_SROM_CMD_WRITE (0x3, 100);      /* Command 16 */

	/* Extract data */

    	romData |= (DEC_SROM_CMD_READ() << ix);
    	DEC_NSDELAY (150);			/* Command 17 */

        DEC_SROM_CMD_WRITE (0x1, 250);      /* Command 18 */

    	}

    /* Finish up command */

    DEC_SROM_CMD_WRITE (0x0, 100);           /* Command 19 */
    return (PCISWAP_SHORT(romData));
    }


/***************************************************************************
*
* dec21x40ChipReset - reset the chip and setup CSR0 register
*
* This routine stops the transmitter and receiver, masks all interrupts, then
* resets the ethernet chip. Once the device comes out of the reset state, it
* initializes CSR0 register.
*
* RETURNS: OK, or ERROR if a new TxD could not be obtained.
*/

LOCAL STATUS dec21x40ChipReset
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to device control structure */
    )
    {
    ULONG	usrFlags=pDrvCtrl->usrFlags;
    ULONG	csr0Val=0x0;
    DEC_TD *	pTxD;
    char *	pBuf;

    pTxD = dec21x40TxDGet (pDrvCtrl);
    pBuf = NET_BUF_ALLOC ();

    if ((pTxD == NULL) || (pBuf == NULL))
	{
	if (pBuf)
	    NET_BUF_FREE (pBuf);
	return ERROR;
	}

    /*
     * "Kick" the dec21143 chip on startup to handle a dec21143
     * known problem.  Note the "kicking" will not hurt the dec21140
     * but seems to hurt the 21145.
     */

    /* setup the transmit buffer pointers */

    pTxD->tDesc2 = PCISWAP (DEC_VIRT_TO_PCI (pBuf));
    pTxD->tDesc3 = 0;

    /* setup frame len */

    pTxD->tDesc1 &= PCISWAP (~TDESC1_TBS1_MSK);
    pTxD->tDesc1 |= PCISWAP (TDESC1_TBS1_PUT(4));

    /* transfer ownership to device */

    pTxD->tDesc0  = PCISWAP(TDESC0_OWN);      

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH();

    /*
     * start xmit dummy packet, this will never go
     * out to the physical bus
     */

    DEC_CSR_WRITE (CSR4, DEC_VIRT_TO_PCI((ULONG)pTxD));
    DEC_CSR_WRITE (CSR7, 0);
    DEC_CSR_WRITE (CSR6, CSR6_21140_MB1 |
			CSR6_21140_TTM |
			CSR6_21140_SF |
			CSR6_21140_PS |
			CSR6_ST |
			CSR6_OM_IL);
    DEC_CSR_WRITE (CSR1, CSR1_TPD );      /* xmit poll demand */
    CACHE_PIPE_FLUSH ();

    /* We're done "kicking" */

    DEC_NSDELAY (0x2000);

    /*
     * Reset the device while xmitting, it ensures the device will not hang
     * up in its 1st xmit later on
     */

    DEC_CSR_WRITE (CSR0, DEC_CSR_READ (CSR0) | CSR0_SWR);
    CACHE_PIPE_FLUSH ();

    /* Re-claim buffer(s) */

    pTxD->tDesc0 = 0;
    NET_BUF_FREE(pBuf);
    taskDelay (sysClkRateGet() / 2);

    DEC_CSR_WRITE (CSR6, 0);          /* stop rcvr & xmitter */
    DEC_CSR_WRITE (CSR7, 0);		/* mask interrupts */
    DEC_CSR_WRITE (CSR0, CSR0_SWR);
    CACHE_PIPE_FLUSH ();

    /* Wait Loop, Loop for at least 50 PCI cycles according to chip spec */

    DEC_NSDELAY (1000);

    /* Decode user setting into CSR0 bits */

    csr0Val |= (usrFlags & DEC_USR_BAR_RX)? 0 : CSR0_BAR;
    csr0Val |= (usrFlags & DEC_USR_BE)? CSR0_BLE : 0;
    csr0Val |= (usrFlags & DEC_USR_TAP_MSK) << DEC_USR_TAP_SHF;
    csr0Val |= (usrFlags & DEC_USR_PBL_MSK) << DEC_USR_PBL_SHF;
    csr0Val |= (usrFlags & DEC_USR_RML)? CSR0_21140_RML : 0;

    if (usrFlags & DEC_USR_CAL_MSK)
        csr0Val |= (usrFlags & DEC_USR_CAL_MSK) << DEC_USR_CAL_SHF;
    else
        {
	csr0Val &= ~(CSR0_CAL_MSK | CSR0_PBL_MSK);
	csr0Val |= (CSR0_CAL_32 | CSR0_PBL_32);
        }
    
    if (! (usrFlags & DEC_USR_TAP_MSK))
        DRV_FLAGS_SET (DEC_TX_KICKSTART);

    DEC_CSR_WRITE (CSR0, csr0Val);
    CACHE_PIPE_FLUSH ();

    DEC_NSDELAY (1000);

    return (OK);
    }

/***************************************************************************
*
* dec21140MediaInit - Initailize media information
*
*/

LOCAL STATUS dec21140MediaInit
    (
    DRV_CTRL *	pDrvCtrl,
    UCHAR *	pSromData
    )
    {
    USHORT *	pSromWord = (USHORT *)pSromData;
    UCHAR *	pInfoLeaf0;
    UCHAR	ix;
    
    /* read the serial ROM into pSromData */

    for (ix=0; ix<DEC21140_SROM_WORDS; ix++)
        *pSromWord++ = dec21140SromWordRead (pDrvCtrl, ix);
    
    /* Initialize the media summary */

    if (pDrvCtrl->mediaCount == 0xFF)
        {

        /* check version */

        if (SROM_VERSION( pSromData ) < DEC21140_SROM_VERSION_3)
            return (ERROR);

        /* get the leaf offset */

        pInfoLeaf0 = pSromData + SROM_ILEAF0_OFFSET( pSromData );
        pDrvCtrl->gprModeVal = ILEAF_GPR_MODE( pInfoLeaf0 );
        pDrvCtrl->mediaCount = ILEAF_MEDIA_COUNT( pInfoLeaf0 );
        pDrvCtrl->mediaCurrent = 0xFF;
        pDrvCtrl->mediaDefault = 0xFF;
        DRV_LOG (DRV_DEBUG_LOAD,
                 "gpMask=0x%x mediaCount = 0x%d\n",
                 pDrvCtrl->gprModeVal, pDrvCtrl->mediaCount, 0, 0, 0, 0);
        }
    return (OK);
    }

/***************************************************************************
* 
* dec21x40MiiRead - read a PHY device register via MII
* 
* RETURNS: the contents of a PHY device register in retVal arg, OK always
*/
LOCAL STATUS dec21x40MiiRead
    (
    DRV_CTRL    *pDrvCtrl,
    UINT8       phyAdrs,                     /* PHY address to access */
    UINT8       phyReg,                      /* PHY register to read */
    UINT16	*pRetVal						
    )
    {

    /* Write 34-bit preamble */

    DEC_MII_WRITE (MII_PREAMBLE, 32);
    DEC_MII_WRITE (MII_PREAMBLE, 2);

    /* start of frame + op-code nibble */

    DEC_MII_WRITE ((MII_SOF | MII_RD), 4);

    /* device address */

    DEC_MII_WRITE (phyAdrs, 5);
    DEC_MII_WRITE (phyReg, 5);

    /* turn around */

    DEC_MII_RTRISTATE;

    /* read data */

    DEC_MII_READ (pRetVal, 16);

    return OK;
    }

/***************************************************************************
*
* dec21x40MiiWrite - write to a PHY device register via MII
*
* RETURNS: OK, always
*/

LOCAL STATUS dec21x40MiiWrite
    (
    DRV_CTRL    *pDrvCtrl,
    UINT8       phyAdrs,                     /* PHY address to access */
    UINT8       phyReg,                      /* PHY register to write */
    UINT16      data                         /* Data to write */
    )
    {
    /* write 34-bit preamble */

    DEC_MII_WRITE (MII_PREAMBLE, 32);
    DEC_MII_WRITE (MII_PREAMBLE, 2);

    /* start of frame + op-code nibble */

    DEC_MII_WRITE (MII_SOF | MII_WR, 4);

    /* device address */

    DEC_MII_WRITE (phyAdrs, 5);
    DEC_MII_WRITE (phyReg, 5);

    /* turn around */

    DEC_MII_WTRISTATE;

    /* write data */

    DEC_MII_WRITE (data, 16);

    return OK;
    }

/***************************************************************************
*
* dec21x40PhyFind - Find the first PHY connected to DEC MII port.
*
* RETURNS: Address of PHY or 0xFF if not found.
*/

UINT8 dec21x40PhyFind
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    UINT16  miiData;
    UINT8   phyAddr;

    for (phyAddr = 0; phyAddr < (UINT8)DEC_MAX_PHY; phyAddr++) 
        {
         dec21x40MiiRead(pDrvCtrl, phyAddr, MII_PHY_ID0, &miiData);

        /* Verify PHY address read */

        if ((miiData != 0xFFFF) && (miiData != 0)) /* Found PHY */
            {
	    DRV_LOG (DRV_DEBUG_LOAD, "PHY @ %#x\n", phyAddr, 0,0,0,0,0);
            return (phyAddr);
            }
        }

    DRV_LOG (DRV_DEBUG_INIT, "No PHY found\n", 0,0,0,0,0,0);
    return (0xFF);
    }

/***************************************************************************
*
* dec21140MediaSelect - select the current media
*
* RETURNS: OK, always
*/

LOCAL STATUS dec21140MediaSelect
    (
    DRV_CTRL *	pDrvCtrl,
    UINT *	pCsr6Val
    )
    {
    UCHAR	sromData[128];
    UCHAR *	pInfoLeaf0;
    UCHAR *	pInfoBlock;
    UINT8	ix;

    if (dec21140MediaInit (pDrvCtrl, sromData) == ERROR)
        return (ERROR);

    /* Get Info Leaf 0 */

    pInfoLeaf0 = sromData + SROM_ILEAF0_OFFSET (sromData);

    /* Get the current media */

    if ((pDrvCtrl->mediaCurrent == 0) ||
        (pDrvCtrl->mediaCurrent >= pDrvCtrl->mediaCount))
        pDrvCtrl->mediaCurrent = pDrvCtrl->mediaCount - 1;
    else
        pDrvCtrl->mediaCurrent --;

    /* Seek to the correct media Info Block */

    pInfoBlock = ILEAF_INFO_BLK0( pInfoLeaf0);
    for (ix=0; ix<pDrvCtrl->mediaCurrent; ix++)
        {
        if (IBLK_IS_COMPACT (pInfoBlock))
            pInfoBlock += IBLK_COMPACT_SIZE;
        else
            pInfoBlock += IBLK_EXT_SIZE (pInfoBlock) + 1;
        }

    /* Convert ext0 into compact */

    if (IBLK_IS_EXT0 (pInfoBlock))
        pInfoBlock = IBLK_EXT0_TO_COMPACT (pInfoBlock);

    /* Setup the GP port */

    DEC_CSR_WRITE (CSR12, CSR12_21140_GPC | pDrvCtrl->gprModeVal);
    DEC_NSDELAY (150);
    
    if (IBLK_IS_COMPACT (pInfoBlock))
        {
        USHORT	compactCmd;

        DRV_LOG ( DRV_DEBUG_LOAD,
                  "COMPACT: mediaCode=0x%x gpData=0x%x command=0x%x\n",
                  IBLK_COMPACT_MCODE( pInfoBlock ),
                  IBLK_COMPACT_GPDATA( pInfoBlock ),
                  IBLK_COMPACT_CMD( pInfoBlock ), 0, 0, 0);

        /* Initialize the PHY interface */

        DEC_CSR_WRITE (CSR12, IBLK_COMPACT_GPDATA(pInfoBlock));
        DEC_NSDELAY(150);

        /* Get CSR6 settings */

        compactCmd = IBLK_COMPACT_CMD( pInfoBlock );
        if (compactCmd & COMPACT_CMD_SCR)
            *pCsr6Val |= CSR6_21140_SCR;
        if (compactCmd & COMPACT_CMD_PCS)
            *pCsr6Val |= CSR6_21140_PCS;
        if (compactCmd & COMPACT_CMD_PS)
            *pCsr6Val |= CSR6_21140_PS;
        }
    else
        {
        UCHAR *	pGpStr;

        DRV_LOG (DRV_DEBUG_LOAD,
                 "EXT1: PHY#=0x%x InitLen=0x%x resetLen=0x%x \n",
                 IBLK_EXT1_PHY(pInfoBlock),
                 IBLK_EXT1_INIT_LEN(pInfoBlock),
                 IBLK_EXT1_RESET_LEN(pInfoBlock), 0, 0, 0);
        DRV_LOG (DRV_DEBUG_LOAD,
                 "mediaCap=0x%x autoAd=0x%x FDMap=0x%x TTMmap=0x%x\n",
                 IBLK_EXT1_MEDIA_CAP(pInfoBlock),
                 IBLK_EXT1_AUTO_AD(pInfoBlock),
                 IBLK_EXT1_FD_MAP(pInfoBlock),
                 IBLK_EXT1_TTM_MAP(pInfoBlock), 0, 0);

        /* Reset the media */

        pGpStr = IBLK_EXT1_RESET_STR (pInfoBlock);
        for (ix=IBLK_EXT1_RESET_LEN(pInfoBlock); ix; ix--, pGpStr++)
            DEC_CSR_WRITE (CSR12, *pGpStr);
        DEC_NSDELAY(150);

	/* if there's an MII-compliant PHY */

	if (IBLK_IS_EXT1 (pInfoBlock))
	    {
	    DRV_LOG (DRV_DEBUG_LOAD, "PHY compliant device\n",
 		     0,0,0,0,0,0);

	    if (dec21x40MiiInit (pDrvCtrl, pCsr6Val, pInfoBlock) == ERROR)
		return (ERROR);
	    }
        }
    
    return (OK);
    }


/***************************************************************************
*
* dec21143MediaInit - Initialize media information
*
* This routine initializes media information for dec21143 chip.
*
* RETURNS: OK or ERROR
*/
 
LOCAL STATUS dec21143MediaInit
    (
    DRV_CTRL *  pDrvCtrl,
    UCHAR *     pSromData
    )
    {
    USHORT *    pSromWord = (USHORT *)pSromData;
    UCHAR *     pInfoLeaf0;
    UCHAR       ix;
   
    /* read the serial ROM into pSromData */
 
    for (ix=0; ix < DEC21140_SROM_WORDS; ix++)
        *pSromWord++ = dec21140SromWordRead (pDrvCtrl, ix);

    /* Initialize the media summary */
 
    if (pDrvCtrl->mediaCount == 0xFF)
        {
        /* check version */
 
        if (SROM_VERSION( pSromData ) < DEC21140_SROM_VERSION_3)
	    {
	    DRV_LOG (DRV_DEBUG_LOAD,
		 "dec21143MediaInit: incorrect SROM version %d (expected >= %d",
		 SROM_VERSION (pSromData), DEC21140_SROM_VERSION_3, 0,0,0,0);
            return (ERROR);
	    }
 
        /* get the leaf offset */
 
        pInfoLeaf0 = pSromData + SROM_ILEAF0_OFFSET(pSromData);
        pDrvCtrl->mediaCount = ILEAF_21143_MEDIA_COUNT(pInfoLeaf0);
        pDrvCtrl->mediaCurrent = 0xFF;
        pDrvCtrl->mediaDefault = 0xFF;
 
        DRV_LOG (DRV_DEBUG_LOAD, "mediaCount = %d\n", 
                      pDrvCtrl->mediaCount, 0, 0, 0, 0, 0);
        }
    return (OK);
    }
 

/***************************************************************************
*
* dec21143MediaSelect - select the current media for dec21143
*
* This routine reads and sets up physical media with configuration
* information from a Version 3 DEC Serial ROM. Any other media configuration
* can be supported by initializing <_func_dec21x40MediaSelect>.
*
* RETURN: OK or ERROR
*/

LOCAL STATUS dec21143MediaSelect
    (
    DRV_CTRL *	pDrvCtrl,
    UINT *	pCsr6Val
    )
    {
    UCHAR       sromData[128];
    UCHAR *     pInfoLeaf0;
    UCHAR *     pInfoBlock;
    UINT	mediaCmd;
    UINT8       ix;
    UINT32	csr15Val;

    if (dec21143MediaInit (pDrvCtrl, sromData) == ERROR)
        return (ERROR);
 
    /* Get Info Leaf 0 */
 
    pInfoLeaf0 = sromData + SROM_ILEAF0_OFFSET (sromData);
 
    /* Get the current media */
 
    if ((pDrvCtrl->mediaCurrent == 0) ||
        (pDrvCtrl->mediaCurrent >= pDrvCtrl->mediaCount))
        pDrvCtrl->mediaCurrent = pDrvCtrl->mediaCount - 1;
    else
        pDrvCtrl->mediaCurrent --;

    DRV_LOG (DRV_DEBUG_LOAD, "dec21143MediaSelect trying %d\n",
	     pDrvCtrl->mediaCurrent, 0, 0, 0, 0, 0);

    /* Seek to the correct media Info Block */
 
    pInfoBlock = ILEAF_21143_INFO_BLK0( pInfoLeaf0);
    for (ix=0; ix < pDrvCtrl->mediaCurrent; ix++)
        pInfoBlock += IBLK_EXT_SIZE (pInfoBlock) + 1;

    if (IBLK_IS_EXT(pInfoBlock))
	{
        switch (IBLK_EXT_TYPE(pInfoBlock))
            {

            case IBLK_IS_EXT2 :         /* Extended format block - type 2 */

	        DEC_CSR_WRITE (CSR6, 0);
		DEC_CSR_WRITE (CSR0, CSR0_SWR);
		DEC_NSDELAY(150);

		*pCsr6Val &= ~CSR6_21140_PS;

                DRV_LOG ( DRV_DEBUG_LOAD, "EXT2: mediaCode=%#x EXT=%#x\n",
                        IBLK_EXT2_MCODE(pInfoBlock),
                        IBLK_EXT2_EXT(pInfoBlock), 0, 0, 0, 0);

                DRV_LOG ( DRV_DEBUG_LOAD, "GP Ctrl=%#x GP Data=%#x\n",
                        IBLK_EXT2_GPC(pInfoBlock),
                        IBLK_EXT2_GPD(pInfoBlock), 0, 0, 0, 0);

                /* get Media-specific data */
  
                if (IBLK_EXT2_EXT(pInfoBlock))
                    {
                    DRV_LOG (DRV_DEBUG_LOAD, 
                        "CSR13=%#x CSR14=%#x CSR15=%#x\n",
                             IBLK_EXT2_MSD_CSR13(pInfoBlock),
                             IBLK_EXT2_MSD_CSR14(pInfoBlock),
                             IBLK_EXT2_MSD_CSR15(pInfoBlock), 0, 0, 0);
  
  
                    DEC_CSR_WRITE (CSR15, (IBLK_EXT2_GPC(pInfoBlock) << 16) |
					  IBLK_EXT2_MSD_CSR15(pInfoBlock));
                    DEC_CSR_WRITE (CSR15, (IBLK_EXT2_GPD(pInfoBlock) << 16) |
					  IBLK_EXT2_MSD_CSR15(pInfoBlock));
                    DEC_CSR_WRITE (CSR14, IBLK_EXT2_MSD_CSR14(pInfoBlock));
                    DEC_CSR_WRITE (CSR13, IBLK_EXT2_MSD_CSR13(pInfoBlock));
                    }
	        else
		    {
		    /* setup the GP port */

		    csr15Val = 0;		/* OR'd into GPC/GPD values */

		    DEC_CSR_WRITE (CSR13, 0);

		    switch (IBLK_EXT2_MCODE(pInfoBlock))
			{
			case EXT2_MEDIA_10TP:
			    DRV_LOG (DRV_DEBUG_LOAD, "EXT2: 10TP\n",
                                       0,0,0,0,0,0);
                            DEC_CSR_WRITE (CSR14, CSR14_SPP | CSR14_APE |
                                           CSR14_LTE | CSR14_SQE | CSR14_CLD |
                                           CSR14_CSQ | CSR14_RSQ |
                                           CSR14_CPEN_HP |
                                           CSR14_CPEN_DM | CSR14_LSE |
                                           CSR14_DREN | CSR14_LBK | CSR14_ECEN);
                            DEC_CSR_WRITE (CSR13, CSR13_SRL_SIA);
                            break;
                        case EXT2_MEDIA_BNC:
                            DRV_LOG (DRV_DEBUG_LOAD, "EXT2: BNC\n",0,0,0,0,0,0);
                            DEC_CSR_WRITE (CSR14, CSR14_ECEN | CSR14_DREN |
                                           CSR14_CLD | CSR14_CSQ | CSR14_RSQ);
                            DEC_CSR_WRITE (CSR13, CSR13_SRL_SIA | CSR13_AUI_TP);
                            break;
                        case EXT2_MEDIA_AUI:
                            DRV_LOG (DRV_DEBUG_LOAD, "EXT2: AUI\n",0,0,0,0,0,0);
                            DEC_CSR_WRITE (CSR14, CSR14_ECEN | CSR14_DREN |
                                           CSR14_CLD | CSR14_CSQ | CSR14_RSQ);
                            csr15Val = CSR15_21143_ABM;
                            DEC_CSR_WRITE (CSR13, CSR13_SRL_SIA | CSR13_AUI_TP);
                            break;
                        case EXT2_MEDIA_10FD:
                            DRV_LOG (DRV_DEBUG_LOAD, "EXT2: 10FD\n",
                                     0,0,0,0,0,0);
                            DEC_CSR_WRITE (CSR14, CSR14_SPP | CSR14_APE |
                                           CSR14_LTE | CSR14_SQE | CSR14_CLD |
                                           CSR14_CSQ | CSR14_RSQ |
           				   CSR14_CPEN_NC |
                                           CSR14_LSE | CSR14_DREN | CSR14_ECEN);
                            DEC_CSR_WRITE (CSR13, CSR13_SRL_SIA);
                            break;
                        default:
                            DRV_LOG (DRV_DEBUG_LOAD, "EXT2: UNK\n",0,0,0,0,0,0);
                            break;
			}

		    DEC_CSR_WRITE (CSR15, (IBLK_EXT2_GPC(pInfoBlock) << 16) | 
					  csr15Val);
		    DEC_CSR_WRITE (CSR15, (IBLK_EXT2_GPD(pInfoBlock) << 16) |
					  csr15Val);
		    }

		DEC_NSDELAY(150);
                break;

            case IBLK_IS_EXT3 :         /* Extended format block - type 3 */

                DRV_LOG (DRV_DEBUG_LOAD,
                        "EXT3: PHY#=%#x InitLen=%#x resetLen=%#x \n",
                                IBLK_EXT3_PHY(pInfoBlock),
                                IBLK_EXT3_INIT_LEN(pInfoBlock),
                                IBLK_EXT3_RESET_LEN(pInfoBlock), 0, 0, 0);
                DRV_LOG (DRV_DEBUG_LOAD,
                        "mediaCap=%#x autoAd=%#x FDMap=%#x"
                        "TTMmap=%#x MIIci=%#x\n",
                                IBLK_EXT3_MEDIA_CAP(pInfoBlock),
                                IBLK_EXT3_AUTO_AD(pInfoBlock),
                                IBLK_EXT3_FD_MAP(pInfoBlock),
                                IBLK_EXT3_TTM_MAP(pInfoBlock),
                                IBLK_EXT3_MII_CI(pInfoBlock), 0);
 
	        if (dec21x40MiiInit (pDrvCtrl, pCsr6Val, pInfoBlock) == ERROR)
		    return (ERROR);
                break;

            case IBLK_IS_EXT4 :         /* Extended format block - type 4 */

                DRV_LOG ( DRV_DEBUG_LOAD, 
                        "EXT4: mediaCode=%#x GPCtrl=%#x GPData=%#x Cmd=%#x\n",
                                IBLK_EXT4_MCODE(pInfoBlock),
                                IBLK_EXT4_GPC(pInfoBlock),
                                IBLK_EXT4_GPD(pInfoBlock),
                                IBLK_EXT4_CMD(pInfoBlock), 0, 0);
  
                /* setup the GP port */
  
                DEC_CSR_WRITE (CSR15, IBLK_EXT4_GPC(pInfoBlock) << 16);
                DEC_CSR_WRITE (CSR15, IBLK_EXT4_GPD(pInfoBlock) << 16);
                DEC_NSDELAY(150);

                /* Get CSR6 settings */
  
                mediaCmd = IBLK_EXT4_CMD(pInfoBlock);
		/* *pCsr6Val = 0; */
  
                if (mediaCmd & IBLK_EXT4_CMD_PS)
                    *pCsr6Val |= CSR6_21140_PS;
                if (mediaCmd & IBLK_EXT4_CMD_TTM)
                    *pCsr6Val |= CSR6_21140_TTM;
                if (mediaCmd & IBLK_EXT4_CMD_PCS)
                    *pCsr6Val |= CSR6_21140_PCS;
                if (mediaCmd & IBLK_EXT4_CMD_SCR)
                    *pCsr6Val |= CSR6_21140_SCR;

		if (IBLK_EXT4_MCODE(pInfoBlock) == 3)
		    {
		    /* Half duplex, 100BaseTx */
		    *pCsr6Val &= ~(CSR6_FD);
		    }
		else if (IBLK_EXT4_MCODE(pInfoBlock) == 5)
		    {
		    /* Full duplex, 100BaseTx */
		    *pCsr6Val |= CSR6_FD;
		    }
		else
		    {
		    DRV_LOG (DRV_DEBUG_INIT, "Unsupported media code\n",
			     0,0,0,0,0,0);
		    }
                break;
  
            case IBLK_IS_EXT5 :         /* Extended format block - type 5 */
  
                DRV_LOG ( DRV_DEBUG_LOAD, "EXT5: Rst Seq length=%#x\n",
                                IBLK_EXT5_RESET_LEN(pInfoBlock), 0, 0, 0, 0, 0);
  
                /* Reset the media */
  
                /* not implemented */
  
                break;
  
            default :
                return (ERROR);
            }
	}
    else
	{
        return (ERROR);
	}

    DRV_LOG (DRV_DEBUG_LOAD, "CSR6 = %#08x\n", *pCsr6Val, 0,0,0,0,0);

    return (OK);
    }


/***************************************************************************
*
* dec21145SPIStart - Setup HomePNA SPI port 
*
* RETURNS: nothing 
*/
 
LOCAL void dec21145SPIStart
    (
    DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    /* Clear SPI Chip Select to HomePNA Phy, and all other SPI bits */

    DEC_CSR_RESET (CSR9, CSR9_21145_SPI_MSK); 	

    /* SPI Chip Select the HomePNA PHY, set output to zero */

    DEC_CSR_UPDATE (CSR9, CSR9_21145_SPI_CS); 	
    }

/***************************************************************************
*
* dec21145SPIStop - Reset HomePNA SPI port 
*
* RETURNS: nothing 
*/
 
LOCAL void dec21145SPIStop
    (
    DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {

    /* Clear SPI Chip Select to HomePNA Phy, and all other SPI bits */

    DEC_CSR_RESET (CSR9, CSR9_21145_SPI_MSK); 	

    /* SPI Chip Select the HomePNA PHY, set output to zero */

    DEC_CSR_UPDATE (CSR9, CSR9_21145_SPI_CLK); 	
    }	  


/***************************************************************************
*
* dec21145SPIByteTransfer - transfer a byte over the HomePNA SPI port 
*
* This routine writes and, as a result also, reads a byte of data to/from
* the SPI port. This routine is generally used to write/read registers on the 
* HomePNA PHY. 
*
* RETURNS: byte value read from SPI port
*/
 
LOCAL UCHAR dec21145SPIByteTransfer
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UCHAR   WriteData
    )
    {
    int i;
    UCHAR ReadData = 0;

    for (i=7; i>=0; i--)
	{
        /* falling edge clock */

	DEC_CSR_RESET (CSR9, CSR9_21145_SPI_CLK); 	

        /* read data coming out of PHY */

	if(DEC_CSR_READ(CSR9) & CSR9_21145_SPI_DO)
	    ReadData |= (0x1<<i);

        if(WriteData & (0x1<<i))
	    
            /* set Phy input to a ONE */
	    
            DEC_CSR_UPDATE(CSR9, CSR9_21145_SPI_DI);
	else
	    
            /* set Phy input to a ZERO */
	    
            DEC_CSR_RESET (CSR9, CSR9_21145_SPI_DI); 	
	  
        /* rising edge clock, write the Phy input out */

	DEC_CSR_UPDATE(CSR9, CSR9_21145_SPI_CLK);
	}	

      return ReadData;
    }

#ifdef DRV_DEBUG
/***************************************************************************
*
* dec21145SPIReadBack - Read all PHY registers out 
*
* RETURNS: nothing 
*/
 
void dec21145SPIReadBack
    (
    DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    int i;

    /* read back the PHY registers */

    for(i=0; i<0x20; i++) 
	{
	dec21145SPIStart(pDrvCtrl);

	/* output HomePNA SPI read opcode */

	dec21145SPIByteTransfer(pDrvCtrl, (UCHAR) PNA_SPI_OPCODE_READ);

	/* output HomePNA Phy register index */

	dec21145SPIByteTransfer(pDrvCtrl, i);

	/* read HomePNA Phy register data */ 

	DRV_LOG (DRV_DEBUG_LOAD, "Read PNA Phy Reg %x: %x\n",
		 i,dec21145SPIByteTransfer(pDrvCtrl,0x0),0,0,0,0);
	dec21145SPIStop(pDrvCtrl);
	} 
    }
#endif /* DRV_DEBUG */

/***************************************************************************
*
* dec21145HomePNAInit - initialize the chip and PHY to use the HomePNA interface
*
* This routine initializes the chip and PHY to use the HomePNA interface.
*
* RETURNS: OK, or ERROR
*/
 
LOCAL STATUS dec21145HomePNAInit
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UINT *    pCsr6Val
    )
    {
    int i = 0;
    int j = 0;
    int regValue;
    char  WriteableHRRegisters[]={0x00, 0x01, 0x04, 0x05, 0x08, 0x09, 
		                  0x0A, 0x0B, 0x10, 0x12, 0x13, 
			          0x14, 0x15, 0x19, 0x1A, 0x1C, 0x1D, 
				  0x1E, 0x1F, -1};

    if(pDrvCtrl->pHomePNAPhyInfo == NULL)
	{
	printf("HomePNAInit: No Ext7 PHY parameters found in SROM\n");
	exit(ERROR);
	}
    else
	{  

	/* Get clocks going to the HomePNA Phy */
#if 0
	/* hard code analog control values from datasheet */

	DEC_CSR_WRITE(CSR13, (0x708a<<16) | CSR13_AUI_TP
		      | CSR13_SRL_SIA);

#endif
        /* use analog control values from srom */

	DEC_CSR_WRITE(CSR13, ((pDrvCtrl->pHomePNAPhyInfo->sp_csr13)<<16) | 
		      CSR13_AUI_TP);

	/* make sure the Phy is not going to be powered down */

	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_LOW][0] &=
	        ~PHA_PHY_CTRL_LOW_PDN;
	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_LOW][1]= 1;

        /* force the HomePNA speed if user flags are set */

	if(pDrvCtrl->usrFlags & DEC_USR_HPNA_FORCE_SLOW)
	    {

	    /* force slow speed mode - 0.7 MB/sec */

	    pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_LOW][0] &=
		 ~PHA_PHY_CTRL_LOW_HS;
	    pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_LOW][1]= 1;
	    }
	else if (pDrvCtrl->usrFlags & DEC_USR_HPNA_FORCE_FAST)
	    {

	    /* force high speed mode  - 1 MB/sec */

	    pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_LOW][0] |=
	  	PHA_PHY_CTRL_LOW_HS;
	    pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_LOW][1]= 1;
	    }

        /* force the HomePNA power if user flags are set */

	if(pDrvCtrl->usrFlags & DEC_USR_HPNA_FORCE_LOW_PWR)
	    {

	    /* force low power mode */

	    pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_LOW][0] &=
		~PHA_PHY_CTRL_LOW_HP;
	    pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_LOW][1]= 1;
	    }
	else if (pDrvCtrl->usrFlags & DEC_USR_HPNA_FORCE_HI_PWR)
	    {

	    /* force high power mode */

	    pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_LOW][0] |=
	 	PHA_PHY_CTRL_LOW_HP;
	    pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_LOW][1]= 1;
	    }


        /* ZERO out CSR9!! This must happen or we wont be able to talk SPI */

        DEC_CSR_WRITE (CSR9, 0); 	

	/* output HomePNA SPI latch write enable opcode */

	dec21145SPIStart(pDrvCtrl);
	dec21145SPIByteTransfer(pDrvCtrl, (UCHAR) PNA_SPI_OPCODE_SET_WE);
	dec21145SPIStop(pDrvCtrl);	

	/* initialize the PHY registers */

	for(i=0,j=0; (j != -1) && (j<0x20); j=WriteableHRRegisters[i++] ) 
	    {
	    if(j != -1)
		{
		DRV_LOG(DRV_DEBUG_LOAD, "phy reg %x: %x updated %x\r\n", 
		        j, pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[j][0],
			pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[j][1], 4,5,6); 

	        /* did we need to update it? */

		if( pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[j][1] == 1)
		    {

		    /* output HomePNA SPI write opcode */

		    dec21145SPIStart(pDrvCtrl);
		    dec21145SPIByteTransfer(pDrvCtrl,
                                            (UCHAR) PNA_SPI_OPCODE_WRITE);

		    /* output HomePNA Phy register index */

		    dec21145SPIByteTransfer(pDrvCtrl, j);

		    /* output HomePNA Phy register data */

		    regValue = pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[j][0];
		    DRV_LOG(DRV_DEBUG_LOAD, "Writing reg %x: %x\r\n", 
			    j, regValue,3,4,5,6);        
		    dec21145SPIByteTransfer(pDrvCtrl, regValue); 
		    dec21145SPIStop(pDrvCtrl);
		    }
		}
	    } 
	
#ifdef DRV_DEBUG
	dec21145SPIReadBack(pDrvCtrl);
#endif
	/* output HomePNA SPI latch clear write enable opcode */

	dec21145SPIStart(pDrvCtrl);
	dec21145SPIByteTransfer(pDrvCtrl, (UCHAR) PNA_SPI_OPCODE_CLEAR_WE);
	dec21145SPIStop(pDrvCtrl);	

	}
          /* reset the SIA  and wait 5ms */

	  DEC_CSR_WRITE(CSR13, ((pDrvCtrl->pHomePNAPhyInfo->sp_csr13)<<16) | 
			CSR13_SRL_SIA );
	  taskDelay((sysClkRateGet() / 200) + 2); 


      	  /* enable collison detect, receive squelch, decoder/encoder */

	  DEC_CSR_WRITE(CSR14, CSR14_CLD | CSR14_RSQ | CSR14_DREN | CSR14_ECEN);

	  /* enable 10-BT autonegotitation when the usr flag is set */

	  if(pDrvCtrl->usrFlags & DEC_USR_HPNA_PREFER_10BT)
	    DEC_CSR_UPDATE(CSR14,  CSR14_21143_TAS | CSR14_SPP | CSR14_APE |
                           CSR14_LTE | CSR14_21143_ANE | CSR14_CPEN_NC |
                           CSR14_LSE | CSR14_21143_TH );

	  /* disable receive watchdog, and jabber timer*/

	  DEC_CSR_WRITE(CSR15, CSR15_21143_RWD | CSR15_JBD);
	  DEC_CSR_WRITE(CSR13, ((pDrvCtrl->pHomePNAPhyInfo->sp_csr13)<<16) | 
			 CSR13_AUI_TP | CSR13_SRL_SIA);
      return (OK);
    }


/***************************************************************************
*
* dec21145DecodeExt7InfoBlock - Decode and load HomePNA Phy defaults from SROM
*
* This routine decodes an EXT7 record from the SROM containing the
* HomePNA PHY register defaults. This routine load up the pHomePNAPhyInfo
* structure with the values it finds in the SROM.
*
* RETURNS: OK or ERROR if decoding fails
*/
 
LOCAL STATUS dec21145DecodeExt7InfoBlock
    (
    DRV_CTRL *  pDrvCtrl,
    UCHAR *     pInfoBlock    /* ptr to infoBlock */
    )
    {
    UCHAR       iy;
    UCHAR       tmpExt7ExtraIndex;
    UCHAR       tmpExt7ExtraData;

    /* assume decode failure */

    pDrvCtrl->homePNAPhyValuesFound = FALSE;
    if(IBLK_EXT_TYPE(pInfoBlock) == 0x07)
        {

	/* get memory for the HomePNAphyInfo structure */

	if ((pDrvCtrl->pHomePNAPhyInfo = 
	     calloc (sizeof (HOMEPNA_PHY_INFO), 1)) == NULL)
	    return(ERROR);
	pDrvCtrl->pHomePNAPhyInfo->sp_csr13 = 
	       IBLK_EXT7_ANALOG_CTRL(pInfoBlock);
	DRV_LOG ( DRV_DEBUG_LOAD, "EXT7: analog_ctrl 0x%x\n",
		  IBLK_EXT7_ANALOG_CTRL(pInfoBlock),2,3,4,5,6);
	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_LOW][0] = 
	       IBLK_EXT7_CTRL_LOW(pInfoBlock);

	/* flag this register for update in the phy init */

	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_LOW][1] = 1; 

	DRV_LOG ( DRV_DEBUG_LOAD, "EXT7: ctrl_low 0x%x\n",
		  IBLK_EXT7_CTRL_LOW(pInfoBlock),2,3,4,5,6);
	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_HI][0] = 
	  IBLK_EXT7_CTRL_HI(pInfoBlock);

	/* flag this register for update in the phy init */

	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_CTRL_HI][1] =1; 
	DRV_LOG ( DRV_DEBUG_LOAD, "EXT7: ctrl_hi 0x%x\n",
		  IBLK_EXT7_CTRL_HI(pInfoBlock),2,3,4,5,6);
	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_NOISE][0] = 
	  IBLK_EXT7_NOISE(pInfoBlock);

	/* flag this register for update in the phy init */

	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_NOISE][1] = 1;
	DRV_LOG ( DRV_DEBUG_LOAD, "EXT7: noise 0x%x\n",
		  IBLK_EXT7_NOISE(pInfoBlock),2,3,4,5,6);
	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_NSE_FLOOR][0] = 
	  IBLK_EXT7_NSE_FLOOR(pInfoBlock);

	/* flag this register for update in the phy init */

	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_NSE_FLOOR][1] = 1;
	DRV_LOG ( DRV_DEBUG_LOAD, "EXT7: floor 0x%x\n",
		  IBLK_EXT7_NSE_FLOOR(pInfoBlock),2,3,4,5,6);
	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_NSE_CEILING][0] = 
	  IBLK_EXT7_NSE_CEILING(pInfoBlock);

	/* flag this register for update in the phy init */

	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_NSE_CEILING][1] =1; 
	DRV_LOG ( DRV_DEBUG_LOAD, "EXT7: ceiling 0x%x\n",
		  IBLK_EXT7_NSE_CEILING(pInfoBlock),2,3,4,5,6);
	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_NSE_ATTACK][0] = 
	  IBLK_EXT7_NSE_ATTACK(pInfoBlock);

	/* flag this register for update in the phy init */

	pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs[PNA_PHY_NSE_ATTACK][1] =1; 
	DRV_LOG ( DRV_DEBUG_LOAD, "EXT7: attack 0x%x\n",
		  IBLK_EXT7_NSE_ATTACK(pInfoBlock),2,3,4,5,6);

	if( IBLK_EXT_SIZE(pInfoBlock) > 11) 

	    /* additional fields */

	    {
	    if((IBLK_EXT_SIZE(pInfoBlock) & 0x1) != 0x1)
	        {
		for(iy=10; iy < IBLK_EXT_SIZE(pInfoBlock); iy += 2)
		    {
		    tmpExt7ExtraIndex = 
		      IBLK_EXT7_EXTRA_INDEX(pInfoBlock + iy); 
		    tmpExt7ExtraData = 
		      IBLK_EXT7_EXTRA_DATA(pInfoBlock + iy); 
		    pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs [tmpExt7ExtraIndex][0]=
		      tmpExt7ExtraData; 

		    /* flag this register for update in the phy init */

		    pDrvCtrl->pHomePNAPhyInfo->pna_phy_regs [tmpExt7ExtraIndex][1]=1;
		    DRV_LOG ( DRV_DEBUG_LOAD, 
			      "EXT7: Extra index 0x%x data 0x%x\n",
			      tmpExt7ExtraIndex, tmpExt7ExtraData,3,4,5,6);
		    }
	        }
	    else
	        {
		DRV_LOG( DRV_DEBUG_LOAD, "Invalid SROM Ext7 format\n",
			 1,2,3,4,5,6);
		return (ERROR);
	        }
	    }

	/* we decoded everything ok! */

	pDrvCtrl->homePNAPhyValuesFound = TRUE;
        }
    else
        {

	/* no EXT7 type found */

	DRV_LOG( DRV_DEBUG_LOAD, "Not an Ext7 Info Block\n",
		 1,2,3,4,5,6);
	return(ERROR);
        }
    return (OK);
    }
 
/***************************************************************************
*
* dec21145FindHomePNAPhyValues - find HomePNA Phy defaults from SROM
*
* This routine searches the SROM for HomePNA defaults.
* 
* HomePNA PHY register defaults. This routine load up the pHomePNAPhyInfo
* structure with the values it finds in the SROM.
*
* RETURNS: OK or ERROR if decoding fails
*/
 
LOCAL STATUS dec21145FindHomePNAPhyValues
    (
    DRV_CTRL *  pDrvCtrl,
    UCHAR *     infoBlockPtrs[]    /* array of ptr to infoBlocks */
    )
    {
    UCHAR       ix;
    for(ix=0; ix< pDrvCtrl->mediaCount; ix++)
      {
	if(IBLK_EXT_TYPE(infoBlockPtrs[ix]) == 0x07)
	  {
	    dec21145DecodeExt7InfoBlock(pDrvCtrl, infoBlockPtrs[ix]);

	    /* keep searching until we found some good values */

	    if(pDrvCtrl->homePNAPhyValuesFound == TRUE)
	      return (OK);
	  }
      }
    return (ERROR);
    }

/***************************************************************************
*
* dec21145MediaInit - Initialize media information
*
* This routine initializes media information for dec21145 chip.
*
* RETURNS: OK or ERROR
*/
 
LOCAL STATUS dec21145MediaInit
    (
    DRV_CTRL *  pDrvCtrl,
    UCHAR *     pSromData
    )
    {
    USHORT *    pSromWord = (USHORT *)pSromData;
    UCHAR *     pInfoLeaf0;
    UCHAR       ix;
   
    DRV_LOG (DRV_DEBUG_LOAD, "dec21145MediaInit\n", 
	     pDrvCtrl->mediaCount, 0, 0, 0, 0, 0);

    /* read the serial ROM into pSromData */

 
    for (ix=0; ix < DEC21140_SROM_WORDS; ix++)
        *pSromWord++ = dec21140SromWordRead (pDrvCtrl, ix);

    /* Initialize the media summary */
 
    if (pDrvCtrl->mediaCount == 0xFF)
      {
        /* check version */
	
        if (SROM_VERSION( pSromData ) < DEC21140_SROM_VERSION_4)
	  {
	    DRV_LOG (DRV_DEBUG_LOAD, 
		     "dec21145MediaInit: unsupport SROM version\n", 
		     pDrvCtrl->mediaCount, 0, 0, 0, 0, 0);
            return (ERROR);
	  }
	
        /* get the leaf offset */
	
        pInfoLeaf0 = pSromData + SROM_ILEAF0_OFFSET(pSromData);
        pDrvCtrl->mediaCount = ILEAF_21145_MEDIA_COUNT(pInfoLeaf0);
        pDrvCtrl->mediaCurrent = 0xFF;
        pDrvCtrl->mediaDefault = 0xFF;
        DRV_LOG (DRV_DEBUG_LOAD, "mediaCount = %d\n", 
		 pDrvCtrl->mediaCount, 0, 0, 0, 0, 0);
      }
    return (OK);
    }
 

/***************************************************************************
*
* dec21145MediaSelect - select the current media for dec21143
*
* This routine reads and sets up physical media with configuration
* information from a Version 4 Intel/DEC Serial ROM. Any other
* media configuration can be supported by initializing
* <_func_dec21x40MediaSelect>.
*
* RETURN: OK or ERROR
*/

LOCAL STATUS dec21145MediaSelect
    (
    DRV_CTRL *        pDrvCtrl,
    UINT *    pCsr6Val
    )
    {
    UCHAR       sromData[128];
    UCHAR *     pInfoLeaf0;
    UCHAR *     pInfoBlock;
    UINT        mediaCmd;
    UCHAR *     infoBlockPtrs[10];   
    BOOL        mediaOK;
    int         ix;

    pDrvCtrl->pHomePNAPhyInfo = NULL;
    mediaOK = FALSE;

    DRV_LOG (DRV_DEBUG_LOAD, " dec21145MediaSelect\n", 0, 0, 0, 0, 0, 0);

    if (dec21145MediaInit (pDrvCtrl, sromData) == ERROR)
        return (ERROR);
 
    /* Get Info Leaf 0 */
 
    pInfoLeaf0 = sromData + SROM_ILEAF0_OFFSET (sromData);
 
    /* Get the current media */
 
    if ((pDrvCtrl->mediaCurrent == 0) ||
        (pDrvCtrl->mediaCurrent >= pDrvCtrl->mediaCount))
        pDrvCtrl->mediaCurrent = pDrvCtrl->mediaCount - 1;
    else
        pDrvCtrl->mediaCurrent --;
 
 
    pInfoBlock = ILEAF_21145_INFO_BLK0( pInfoLeaf0);
    infoBlockPtrs[0] = pInfoBlock;
    for (ix=0; ix < pDrvCtrl->mediaCurrent; ix++)
      {
        pInfoBlock += IBLK_EXT_SIZE (pInfoBlock) + 1;
	infoBlockPtrs[ix+1] = pInfoBlock;
      }

    /* Media fallback loop */

    do
    {
    DRV_LOG (DRV_DEBUG_LOAD, " dec21145MediaSelect media current %d\n", 
	     pDrvCtrl->mediaCurrent, 0, 0, 0, 0, 0);
    pInfoBlock = infoBlockPtrs[pDrvCtrl->mediaCurrent];
    if (IBLK_IS_EXT(pInfoBlock))
	{
	DRV_LOG (DRV_DEBUG_LOAD, " dec21145MediaSelect media is EXT\n", 
	         0, 0, 0, 0, 0, 0);
	/* Seek to the correct media Info Block */

	switch (IBLK_EXT_TYPE(pInfoBlock))
	    {
	    case IBLK_IS_EXT2 :         /* Extended format block - type 2 */
		DRV_LOG ( DRV_DEBUG_LOAD, "EXT2: mediaCode=%#x EXT=%#x\n",
			  IBLK_EXT2_MCODE(pInfoBlock),
			  IBLK_EXT2_EXT(pInfoBlock), 0, 0, 0, 0);

		/* make sure we use 10BASE-T/HomePNA port */

		*pCsr6Val &= ~CSR6_21140_PS;

		/* get Media-specific data */

		if (IBLK_EXT2_EXT(pInfoBlock))
		    {
		    DRV_LOG (DRV_DEBUG_LOAD, 
			     "CSR13=%#x CSR14=%#x CSR15=%#x\n",
			     IBLK_EXT2_MSD_CSR13(pInfoBlock),
			     IBLK_EXT2_MSD_CSR13(pInfoBlock),
			     IBLK_EXT2_MSD_CSR13(pInfoBlock), 0, 0, 0);
	            DEC_CSR_WRITE (CSR13, IBLK_EXT2_MSD_CSR13(pInfoBlock));
		    DEC_CSR_WRITE (CSR14, IBLK_EXT2_MSD_CSR13(pInfoBlock));
		    DEC_CSR_WRITE (CSR15, IBLK_EXT2_MSD_CSR13(pInfoBlock));
		    }
		/* setup the GP port */

		DRV_LOG ( DRV_DEBUG_LOAD, "GP Ctrl=%#x GP Data=%#x\n",
			  IBLK_EXT2_GPC(pInfoBlock),
			  IBLK_EXT2_GPD(pInfoBlock), 0, 0, 0, 0);
		
		DEC_CSR_UPDATE(CSR15, IBLK_EXT2_GPC(pInfoBlock) << 8);
		DEC_CSR_UPDATE(CSR15, IBLK_EXT2_GPD(pInfoBlock) << 8);
		DEC_NSDELAY(500);
		
		/* see if we want to setup for HomePNA */

		switch (IBLK_EXT2_MCODE(pInfoBlock))
		    {
		    case EXT2_MEDIA_10TP:
		        DRV_LOG ( DRV_DEBUG_LOAD, "EXT2: Media 10Base-T\n",
			          1,2,3,4,5,6);		  

		        /* enable collison detect, receive squelch, decoder/encoder */

		        DEC_CSR_WRITE(CSR14, CSR14_CLD | CSR14_RSQ |
                                      CSR14_DREN | CSR14_ECEN);
		        DEC_CSR_UPDATE(CSR14,  CSR14_21143_TAS | CSR14_SPP |
                                       CSR14_APE | CSR14_LTE | CSR14_SQE |
                                       CSR14_21143_ANE | CSR14_CPEN_NC |
                                       CSR14_LSE | CSR14_21143_TH ); 

		        /* disable receive watchdog, and jabber timer*/

		        DEC_CSR_WRITE(CSR15, 0);
		        DEC_CSR_WRITE(CSR13,
                                      ((pDrvCtrl->pHomePNAPhyInfo->sp_csr13)
                                        <<16) |
				      CSR13_SRL_SIA);
		        *pCsr6Val &= ~CSR6_FD;
		        mediaOK = TRUE;

		        /* if we fail, go back to the higher priority link */

		        pDrvCtrl->mediaCurrent = 0xff;
		        break;
		    case EXT2_MEDIA_10FD:                  
		        DRV_LOG (DRV_DEBUG_LOAD,
                                 "EXT2: Media 10Base-T Full Duplex\n",
			         1,2,3,4,5,6);      	  
		        /*
                         * enable collison detect, receive squelch,
                         * decoder/encoder
                         */

		        DEC_CSR_WRITE(CSR14, CSR14_CLD | CSR14_RSQ |
                                      CSR14_DREN | CSR14_ECEN);
		        DEC_CSR_UPDATE(CSR14,  CSR14_21143_TAS | CSR14_SPP |
                                       CSR14_APE | CSR14_LTE | CSR14_SQE |
                                       CSR14_21143_ANE | CSR14_CPEN_NC |
                                       CSR14_LSE | CSR14_21143_TH ); 

		        /* disable receive watchdog, and jabber timer*/

		        DEC_CSR_WRITE(CSR15, 0);
		        DEC_CSR_WRITE(CSR13,
                                      ((pDrvCtrl->pHomePNAPhyInfo->sp_csr13)
                                        <<16) | 
				      CSR13_SRL_SIA);
		        *pCsr6Val |= CSR6_FD;
		        mediaOK = TRUE;

		        /* if we fail, go back to the higher priority link */

		        pDrvCtrl->mediaCurrent = 0xff;
		        break;
		    case EXT2_MEDIA_HOMEPNA:
		        DRV_LOG ( DRV_DEBUG_LOAD, "EXT2: Media HomePNA\n",
			          1,2,3,4,5,6);

		        /*
                         * HomePNA operates at half duplex and
                         * Heartbeat disabled
                         */

		        *pCsr6Val &= ~CSR6_FD;
		        *pCsr6Val |= CSR6_21140_HBD;

		        /* set TX treshold */

		        *pCsr6Val |= CSR6_THR_096;

		        /*
                         * see if we can find PNA PHY values in SROM 
                         * as type EXT7
                         */

		        if(pDrvCtrl->homePNAPhyValuesFound == FALSE)
		            {
			    if(dec21145FindHomePNAPhyValues(pDrvCtrl, 
							infoBlockPtrs ) 
			       == ERROR)
			        {
			        DRV_LOG ( DRV_DEBUG_LOAD, 
				         "EXT2: Media HomePNA No Phy Values" 
                                         "Found\n",
				         1,2,3,4,5,6);

			        /* 
                                 * attach BSP hook here to provide 
                                 * PNA PHY defaults
                                 */

			        return(ERROR);
			        }
		            }
		        if(dec21145HomePNAInit ( pDrvCtrl, pCsr6Val) == ERROR)
		            return (ERROR);

		        /* we have determined this is the media we are using */

		        mediaOK = TRUE;
		        break;
		  }
		break;
		
	    case IBLK_IS_EXT3 :         /* Extended format block - type 3 */

		DRV_LOG (DRV_DEBUG_LOAD,
			 "EXT3: PHY#=%#x InitLen=%#x resetLen=%#x \n",
			 IBLK_EXT3_PHY(pInfoBlock),
			 IBLK_EXT3_INIT_LEN(pInfoBlock),
			 IBLK_EXT3_RESET_LEN(pInfoBlock), 0, 0, 0);
		DRV_LOG (DRV_DEBUG_LOAD,
			 "mediaCap=%#x autoAd=%#x FDMap=%#x"
                         "TTMmap=%#x MIIci=%#x\n",
			 IBLK_EXT3_MEDIA_CAP(pInfoBlock),
			 IBLK_EXT3_AUTO_AD(pInfoBlock),
			 IBLK_EXT3_FD_MAP(pInfoBlock),
			 IBLK_EXT3_TTM_MAP(pInfoBlock),
			 IBLK_EXT3_MII_CI(pInfoBlock), 0);
		
		if (dec21x40MiiInit (pDrvCtrl, pCsr6Val, pInfoBlock) == ERROR)
		    return (ERROR);
		break;

	    case IBLK_IS_EXT4 :         /* Extended format block - type 4 */
		
		DRV_LOG (DRV_DEBUG_LOAD, 
			 "EXT4: mediaCode=%#x GPCtrl=%#x GPData=%#x Cmd=%#x\n",
			 IBLK_EXT4_MCODE(pInfoBlock),
			 IBLK_EXT4_GPC(pInfoBlock),
			 IBLK_EXT4_GPD(pInfoBlock),
			 IBLK_EXT4_CMD(pInfoBlock), 0, 0);
		
		/* setup the GP port */
		
		DEC_CSR_UPDATE (CSR15, IBLK_EXT4_GPC(pInfoBlock) << 8);
		DEC_CSR_UPDATE (CSR15, IBLK_EXT4_GPD(pInfoBlock) << 8);
		DEC_NSDELAY(150);
		
		/* Get CSR6 settings */
		
		mediaCmd = IBLK_EXT4_CMD(pInfoBlock);
		if (mediaCmd & IBLK_EXT4_CMD_PS)
		    *pCsr6Val |= CSR6_21140_PS;
		if (mediaCmd & IBLK_EXT4_CMD_TTM)
		    *pCsr6Val |= CSR6_21140_TTM;
		if (mediaCmd & IBLK_EXT4_CMD_PCS)
		    *pCsr6Val |= CSR6_21140_PCS;
		if (mediaCmd & IBLK_EXT4_CMD_SCR)
		    *pCsr6Val |= CSR6_21140_SCR;
		break;
		
	    case IBLK_IS_EXT5 :         /* Extended format block - type 5 */
		
		DRV_LOG ( DRV_DEBUG_LOAD, "EXT5: Rst Seq length=%#x\n",
			  IBLK_EXT5_RESET_LEN(pInfoBlock), 0, 0, 0, 0, 0);
		
		/* Reset the media */
		
		/* not implemented */
		
		pDrvCtrl->mediaCurrent--;
		break;
	    case IBLK_IS_EXT7 :          /* Extended format block - type 7 */
		DRV_LOG ( DRV_DEBUG_LOAD, "EXT7:\n", 0, 0, 0, 0, 0, 0);
		if( dec21145DecodeExt7InfoBlock(pDrvCtrl, pInfoBlock) == ERROR)
		    {
		    DRV_LOG ( DRV_DEBUG_LOAD, 
			      "dec21145DecodeExt7InfoBlock return ERROR\n", 
			      0, 0, 0, 0, 0, 0);
		    return (ERROR);
		    }
		pDrvCtrl->mediaCurrent--;
		mediaOK = FALSE;
		break;
		
	    default :
		DRV_LOG ( DRV_DEBUG_LOAD, "Unknown EXT SROM Type %d\n",
			  IBLK_EXT_TYPE(pInfoBlock), 0, 0, 0, 0, 0);
		return (ERROR);
		break;
	    }
        }
        else
	    {
	    DRV_LOG ( DRV_DEBUG_LOAD, 
		      "No EXT types found in SROM:\n", 0, 0, 0, 0, 0, 0);
	    return (ERROR);
	    }

        } while (mediaOK == FALSE);
    return (OK);
    }


/***************************************************************************
*
* dec21x40MiiInit - initialize the chip to use the MII interface
*
* This routine initializes the chip to use the MII interface.
*
* RETURNS: OK, or ERROR
*/
 
LOCAL STATUS dec21x40MiiInit
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UINT *      pCsr6Val,	/* pointer to location for new CSR6 value */
    UCHAR *	pInfoBlock	/* SROM info block for this MII leaf */
    )
    {
    /* initialize some fields in the PHY info structure */

    if (dec21x40PhyPreInit (pDrvCtrl, pInfoBlock) != OK)
	{
	DRV_LOG (DRV_DEBUG_LOAD, ("Failed to pre-initialize PHY\n"),
				  0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    if (miiPhyInit (pDrvCtrl->pPhyInfo) != OK)
        {
	DRV_LOG (DRV_DEBUG_LOAD, "Failed to initialise PHY\n", 0,0,0,0,0,0);
	return (ERROR);
        }

    *pCsr6Val |= (CSR6_21140_HBD | CSR6_21140_PS);

    /* deal with full duplex mode and speed */

    if (pDrvCtrl->pPhyInfo->phyFlags & MII_PHY_FD)
	{
	*pCsr6Val |= CSR6_FD;
	DRV_LOG (DRV_DEBUG_LOAD, "In full duplex mode\n", 0,0,0,0,0,0);
	}
    else
	{
	DRV_LOG (DRV_DEBUG_LOAD, "In half duplex mode\n", 0,0,0,0,0,0);
	}

    if (pDrvCtrl->pPhyInfo->phyFlags & MII_PHY_100)
	{
	*pCsr6Val &= ~CSR6_21140_TTM;
	DRV_LOG (DRV_DEBUG_LOAD, "In 100Mbps mode\n", 0,0,0,0,0,0);
	}
    else
	{
	*pCsr6Val |= CSR6_21140_TTM;
	DRV_LOG (DRV_DEBUG_LOAD, "In 10Mbps mode\n", 0,0,0,0,0,0);
	}

    return (OK);
    }

/***************************************************************************
*
* dec21x40PhyPreInit - initialize some fields in the phy info structure
*
* This routine initializes some fields in the phy info structure,
* for use of the phyInit() routine.
*
* RETURNS: OK, or ERROR if could not obtain memory.
*/
 
LOCAL STATUS dec21x40PhyPreInit
    (
    DRV_CTRL *  pDrvCtrl,	/* pointer to DRV_CTRL structure */
    UCHAR *	pInfoBlock	/* pointer to SROM info block for this leaf */
    )
    {
    PHY_INFO *	pPhyInfo = NULL;
    
    /* get memory for the phyInfo structure */
	 
    if ((pDrvCtrl->pPhyInfo = calloc (sizeof (PHY_INFO), 1)) == NULL)
	return (ERROR);

    pPhyInfo = pDrvCtrl->pPhyInfo;

    /* set some default values */
 
    pDrvCtrl->pPhyInfo->pDrvCtrl = (void *) pDrvCtrl;
 
    pDrvCtrl->pPhyInfo->phyAnOrderTbl = pDrvCtrl->pMiiPhyTbl;

    if (pDrvCtrl->phyAddr == ((UINT8) 0xFF))
	{
	/* Default is to take the value from the SROM */
        if (IBLK_IS_EXT1 (pInfoBlock))
	    {
	    pDrvCtrl->pPhyInfo->phyAddr = IBLK_EXT1_PHY(pInfoBlock);
	    }
	else
	    {
	    pDrvCtrl->pPhyInfo->phyAddr = IBLK_EXT3_PHY(pInfoBlock);
	    }
	}
    else
	{
	/*
	 * Take the value specified in the load string assuming the user
	 * knows best...
	 */

	pDrvCtrl->pPhyInfo->phyAddr = pDrvCtrl->phyAddr;
	}

    /* 
     * in case of link failure, set a default mode for the PHY 
     * if we intend to use a different media, this flag should 
     * be cleared
     */
 
    pDrvCtrl->pPhyInfo->phyFlags |= MII_PHY_DEF_SET;
 
    pDrvCtrl->pPhyInfo->phyWriteRtn = (FUNCPTR) dec21x40MiiWrite;
    pDrvCtrl->pPhyInfo->phyReadRtn = (FUNCPTR) dec21x40MiiRead;
    pDrvCtrl->pPhyInfo->phyLinkDownRtn = (FUNCPTR) dec21x40MediaChange;
    pDrvCtrl->pPhyInfo->phyDelayRtn = (FUNCPTR) taskDelay;
    pDrvCtrl->pPhyInfo->phyMaxDelay = (sysClkRateGet() * 5);
    pDrvCtrl->pPhyInfo->phyDelayParm = max (1, sysClkRateGet()/60);

    /* in case the cable is not there, leave the PHY ready to auto-negotiate */

    pDrvCtrl->pPhyInfo->phyDefMode = PHY_AN_ENABLE;
 
    /* handle some user-to-physical flags */
 
    if (!(DRV_PHY_FLAGS_ISSET (DEC_USR_MII_NO_AN)))
        MII_PHY_FLAGS_SET (MII_PHY_AUTO);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_AUTO);
 
    if (DRV_PHY_FLAGS_ISSET (DEC_USR_MII_AN_TBL))
        MII_PHY_FLAGS_SET (MII_PHY_TBL);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_TBL);
 
    if (DRV_PHY_FLAGS_ISSET (DEC_USR_MII_100MB))
        MII_PHY_FLAGS_SET (MII_PHY_100);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_100);
 
    if (DRV_PHY_FLAGS_ISSET (DEC_USR_MII_FD))
        MII_PHY_FLAGS_SET (MII_PHY_FD);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_FD);
 
    if (DRV_PHY_FLAGS_ISSET (DEC_USR_MII_10MB))
        MII_PHY_FLAGS_SET (MII_PHY_10);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_10);
 
    if (DRV_PHY_FLAGS_ISSET (DEC_USR_MII_HD))
        MII_PHY_FLAGS_SET (MII_PHY_HD);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_HD);
 
    if (DRV_PHY_FLAGS_ISSET (DEC_USR_MII_BUS_MON))
        MII_PHY_FLAGS_SET (MII_PHY_MONITOR);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_MONITOR);
 
    /* mark it as pre-initilized */
 
    MII_PHY_FLAGS_SET (MII_PHY_PRE_INIT);
 
    DRV_LOG (DRV_DEBUG_LOAD, ("dec21x40PhyPreInit pPhyInfo = 0x%x read=0x%x "
			      "write=0x%x tbl=0x%x addr=0x%x "
			      "flags=0x%x \n"),
			       (int) pDrvCtrl->pPhyInfo,
			       (int) pDrvCtrl->pPhyInfo->phyReadRtn,
			       (int) pDrvCtrl->pPhyInfo->phyWriteRtn,
			       (int) pDrvCtrl->pPhyInfo->phyAnOrderTbl,
			       (int) pDrvCtrl->pPhyInfo->phyAddr,
			       (int) pDrvCtrl->pPhyInfo->phyFlags);
 
    return (OK);
    }

#ifdef DRV_DEBUG
void decPoolShow
    (
    int inst	/* instance number, starting at 0 */
    )
    {
    DRV_CTRL	*pDrvCtrl = (DRV_CTRL *)endFindByName ("dc", inst);

    netPoolShow (pDrvCtrl->endObj.pNetPool);
    }

void decCsrShow
    (
    int inst	/* instance number, starting at 0 */
    )
    {
    DRV_CTRL * pDrvCtrl = (DRV_CTRL *) endFindByName ("dc", inst);
    
    if (pDrvCtrl != NULL)
	{
	printf ("\n");
	printf ("CSR0\t 0x%x\n", (int) dec21x40CsrRead(pDrvCtrl, CSR0));
	printf ("CSR1\t 0x%x\n", (int) dec21x40CsrRead(pDrvCtrl, CSR1));
	printf ("CSR2\t 0x%x\n", (int) dec21x40CsrRead(pDrvCtrl, CSR2));
	printf ("CSR3\t 0x%x\n", (int) dec21x40CsrRead(pDrvCtrl, CSR3));
	printf ("CSR4\t 0x%x\n", (int) dec21x40CsrRead(pDrvCtrl, CSR4));
	printf ("CSR5\t 0x%x\n", (int) dec21x40CsrRead(pDrvCtrl, CSR5));
	printf ("CSR6\t 0x%x\n", (int) dec21x40CsrRead(pDrvCtrl, CSR6));
	printf ("CSR7\t 0x%x\n", (int) dec21x40CsrRead(pDrvCtrl, CSR7));
	printf ("CSR8\t 0x%x\n", (int) dec21x40CsrRead(pDrvCtrl, CSR8));
	printf ("CSR9\t 0x%x\n", (int) dec21x40CsrRead(pDrvCtrl, CSR9));
	printf ("CSR10\t 0x%x\n",(int) dec21x40CsrRead(pDrvCtrl, CSR10));
	printf ("CSR11\t 0x%x\n",(int) dec21x40CsrRead(pDrvCtrl, CSR11));
	printf ("CSR12\t 0x%x\n",(int) dec21x40CsrRead(pDrvCtrl, CSR12));
	printf ("CSR13\t 0x%x\n",(int) dec21x40CsrRead(pDrvCtrl, CSR13));
	printf ("CSR14\t 0x%x\n",(int) dec21x40CsrRead(pDrvCtrl, CSR14));
	printf ("CSR15\t 0x%x\n",(int) dec21x40CsrRead(pDrvCtrl, CSR15));
	}
    }

void decCsrWrite
    (
    UINT	inst,
    UINT	csr,
    UINT	value
    )
    {
    DRV_CTRL * pDrvCtrl = (DRV_CTRL *) endFindByName ("dc", inst);

    DEC_CSR_WRITE (csr, value);
    }

void decTxDShow
    (
    int inst,
    int numTxD
    )
    {
    DRV_CTRL * pDrvCtrl = (DRV_CTRL *) endFindByName ("dc", inst);
    DEC_TD	*pTxD = &pDrvCtrl->txRing[numTxD];

    printf ("txd[%d] at 0x%x:\n", numTxD, (int)pTxD);
    printf ("tDesc0=0x%x tDesc1=0x%x tDesc2=0x%x tDesc3=0x%x\n",
            (int) PCISWAP (pTxD->tDesc0), (int) PCISWAP (pTxD->tDesc1),
            (int) PCISWAP (pTxD->tDesc2), (int) PCISWAP (pTxD->tDesc3));
    }

void decRxDShow
    (
    int inst,
    int numRxD
    )
    {
    DRV_CTRL * pDrvCtrl = (DRV_CTRL *) endFindByName ("dc", inst);
    DEC_TD	*pTxD = &pDrvCtrl->txRing[numRxD];

    printf ("rxd[%d] at 0x%x:\n", numRxD, (int)pTxD);
    printf ("tDesc0=0x%x tDesc1=0x%x tDesc2=0x%x tDesc3=0x%x\n",
            (int) PCISWAP (pTxD->tDesc0), (int) PCISWAP (pTxD->tDesc1),
            (int) PCISWAP (pTxD->tDesc2), (int) PCISWAP (pTxD->tDesc3));
    }

void decShow
    (
    int inst
    )
    {
    DRV_CTRL * pDrvCtrl = (DRV_CTRL *) endFindByName ("dc", inst);

    printf ("pDrvCtrl=0x%x txNum=%d txIndex=%d txDiIndex=%d\n",
            (int) pDrvCtrl,
            pDrvCtrl->numTds,
            pDrvCtrl->txIndex,
            pDrvCtrl->txDiIndex);
    
    printf ("rxNum=%d rxIndex=%d\n",
            pDrvCtrl->numRds,
            pDrvCtrl->rxIndex);
    }

int eAdrsDisplay
    (
    UINT8 *pAddr
    )
    {
    printf ("EtherAddr=%x:%x:%x:%x:%x:%x\n",
            pAddr[0], pAddr[1], pAddr[2], pAddr[3], pAddr[4], pAddr[5]);
    return 0;
    }

void mcAdd (int inst, char *eAddr)
    {
    DRV_CTRL * pDrvCtrl = (DRV_CTRL *) endFindByName ("dc", inst);
    dec21x40MCastAddrAdd (pDrvCtrl, eAddr);
    }

void mcDel (int inst, char *eAddr)
    {
    DRV_CTRL * pDrvCtrl = (DRV_CTRL *) endFindByName ("dc", inst);
    dec21x40MCastAddrDel (pDrvCtrl, eAddr);
    }

void mcShow (int inst)
    {
    DRV_CTRL * pDrvCtrl = (DRV_CTRL *) endFindByName ("dc", inst);
    ETHER_MULTI  *pMCastNode;

    pMCastNode = END_MULTI_LST_FIRST (&pDrvCtrl->endObj);
    while (pMCastNode != NULL)
        {
        eAdrsDisplay (pMCastNode->addr);
        pMCastNode = END_MULTI_LST_NEXT (pMCastNode);
        }
    }

char decSerialRom[128];
void decSerialRomUpload (int inst)
    {
    DRV_CTRL * pDrvCtrl = (DRV_CTRL *) endFindByName ("dc", inst);
    int 	count;
    USHORT 	*pSerialRom = (USHORT *)decSerialRom;

    bzero (decSerialRom, 128);
    for (count=0; count<64; count++)
        *pSerialRom++ = dec21140SromWordRead (pDrvCtrl, count);
    }

int decReset(int inst)
    {
    DRV_CTRL * pDrvCtrl = (DRV_CTRL *) endFindByName ("dc", inst);

    dec21x40Stop (pDrvCtrl);
    taskDelay (max (2, sysClkRateGet()/30));
    netJobAdd ((FUNCPTR)dec21x40Restart, (int)pDrvCtrl, 0, 0, 0, 0);
    return 0;
    }

short decMiiRead (int inst, int phyAddr, int reg)
    {
    int retStat;
    short miiData = 0;
    DRV_CTRL * pDrvCtrl = (DRV_CTRL *) endFindByName ("dc", inst);

    retStat = dec21x40MiiRead(pDrvCtrl, phyAddr, reg, &miiData);
    return (miiData);
    }

int decMiiWrite (int inst, int phyAddr, int reg, short miiData)
    {
    DRV_CTRL * pDrvCtrl = (DRV_CTRL *) endFindByName ("dc", inst);

    return (dec21x40MiiWrite (pDrvCtrl, phyAddr, reg, miiData));
    }
#endif /* DC_DEBUG */
