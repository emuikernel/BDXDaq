/* motFecEnd.c - END style Motorola FEC Ethernet network interface driver */

/* Copyright 1989-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02c,02may02,pmr  SPR 75008: only set txStall flag if motFecTbdClean() fails to 
		 free any Tx BDs in motFecSend().
02b,30apr02,rcs  SPR 76102: polling mode fix
02a,28jan02,rcs  changed motRevNumGet() to call vxImmrDevGet() SPR# 7120
01z,06dec01,rcs  changed REV_D_4 back to REV_D_3 SPR# 71420 
01y,04dec01,kab  Removed decl for vxImmrGet(), get from vxPpcLib.h
01x,26oct01,crg  fixed a possible cause of transmitter underruns by preventing
                 generation of TBDs with no data (SPR 29513)
                 added support for proper setting of MII management interface
                 speed (SPR 33812)
		 corrected clearing of receive events in motFecHandleRecvInt()
		 (SPR 69440)
01w,21sep01,dat  Removing ANSI errors for diab compiler (needs more work)
01v,15sep01,dat  Removed special definition of CACHE_PIPE_FLUSH
01u,01feb01,rcs  added motRevNumGet() using vxImmrGet()
01t,01feb01,rcs  merged in Motorola's modifications.
01s,29jan01,rcs  in motFecRxCtrlRegValGet initialized *rxCtrlVal (SPR# 63479)
01r,29jan01,rcs  in motFecTxCtrlRegValGet initialized *txCtrlVal (SPR# 63479)
01q,05dec00,dat  merge from sustaining branch to tor2_0_x
01p,21nov00,rhk  changes to initialization of FEC in order to support
                 rev. D.4 and newer 860T processors.
01o,05jul00,stv  fixed the coding error in motFecHashTblPopulate (SPR #31622).
01n,07jul00,stv  fixed the coding error in motFecHashTblPopulate (SPR# 31283).
01m,11jun00,ham  removed reference to etherLib.
01l,22may00,pai  Repaired (SPR #30673) by adding Keith Wiles' corrections to
                 hash register bit settings and his table driven CRC funtion.
                 Updated comments in motFecHashRegValGet().  Corrected the
                 coding error in motFecHashTblPopulate() (SPR #31283).
                 Released transmit semaphore and incremented MIB-II counter
                 in motFecSend() when returning ERROR as a result of being
                 called in polled mode.
01k,29mar00,pai  allocated memory for PHY_INFO struct in motFecEndLoad() (SPR
                 #30085).
01j,01feb00,pai  set driver control TX stall flag in motFecSend() if clean
                 TBD count becomes 0 (SPR #30135).
01i,27jan00,dat  fixed use of NULL
01h,05jan00,stv  removed private/funcBindP.h (SPR# 29875).
01g,06dec99,stv  corrected return error code and freed mBlk chain (SPR #28492).
01f,11nov99,cn   removed unnecessary freeing of pDrvCtrl in motFecUnload
		 (SPR# 28772).
01e,09sep99,rhk  added a check to make sure driver is operational before
                 shutting down in motFecStop.
01d,31aug99,rhk  disabled the MOT_FEC_DBG switch, pointed to the local
                 copy of ppc860Intr.h.
01c,19feb99,cn   corrected a bug in motFecMiiDiag ().
01b,09feb99,cn   changes required by performance improvement (SPR# 24883).
		 Also up-dated documentation, added probing of the device
		 in the motFecStart () routine.
01a,09nov98,cn   written.
*/

/*
DESCRIPTION
This module implements a Motorola Fast Ethernet Controller (FEC) network
interface driver. The FEC is fully compliant with the IEEE 802.3
10Base-T and 100Base-T specifications. Hardware support of
the Media Independent Interface (MII) is built-in in the chip.

The FEC establishes a shared memory communication system with the CPU,
which is divided into two parts: the Control/Status Registers (CSR),
and the buffer descriptors (BD).

The CSRs reside in the MPC860T Communication Controller's internal RAM.
They are used for mode control and to extract status information
of a global nature. For instance, the types of events that should
generate an interrupt, or features like the promiscous mode or the
max receive frame length may be set programming some of the CSRs properly.
Pointers to both the Transmit Buffer Descriptors ring (TBD) and the
Receive Buffer Descriptors ring (RBD) are also stored in the CSRs.
The CSRs are located in on-chip RAM and must be accessed using the
big-endian mode.

The BDs are used to pass data buffers and related buffer information
between the hardware and the software. They reside in the host main
memory and basically include local status information and a pointer
to the actual buffer, again in external memory.

This driver must be given several target-specific parameters, and
some external support routines must be provided.  These parameters,
and the mechanisms used to communicate them to the driver, are
detailed below.

For versions of the MPC860T starting with revision D.4 and beyond
the functioning of the FEC changes slightly.  An additional bit has
been added to the Ethernet Control Register (ECNTRL), the FEC PIN
MUX bit.  This bit must be set prior to issuing commands involving
the other two bits in the register (ETHER_EN, RESET).  The bit must
also be set when either of the other two bits are being utilized.
For versions of the 860T prior to revision D.4, this bit should not
be set.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE

The driver provides the standard external interface, motFecEndLoad(), which
takes a string of colon-separated parameters. The parameters should be
specified in hexadecimal, optionally preceeded by "0x" or a minus sign "-".

The parameter string is parsed using strtok_r() and each parameter is
converted from a string representation to binary by a call to
strtoul(parameter, NULL, 16).

The format of the parameter string is:

"<motCpmAddr>:<ivec>:<bufBase>:<bufSize>:<fifoTxBase>:<fifoRxBase>
:<tbdNum>:<rbdNum>:<phyAddr>:<isoPhyAddr>:<phyDefMode>:<userFlags>
:<clockSpeed>"

TARGET-SPECIFIC PARAMETERS

.IP <motCpmAddr>
Indicates the address at which the host processor presents its internal
memory (also known as the dual ported RAM base address). With this address,
the driver is able to compute the location of the FEC parameter RAM, and,
ultimately, to program the FEC for proper operations.

.IP <ivec>
This driver configures the host processor to generate hardware interrupts
for various events within the device. The interrupt-vector offset
parameter is used to connect the driver's ISR to the interrupt through
a call to the VxWorks system function intConnect(). It is also used to
compute the interrupt level (0-7) associated with the FEC interrupt (one
of the MPC860T SIU internal interrupt sources). The latter is given as
a parameter to intEnable(), in order to enable this level interrupt to
the PPC core.

.IP <bufBase>
The Motorola Fast Ethernet Controller is a DMA-type device and typically
shares access to some region of memory with the CPU. This driver is designed
for systems that directly share memory between the CPU and the FEC.

This parameter tells the driver that space for the both the TBDs and the
RBDs needs not be allocated but should be taken from a cache-coherent
private memory space provided by the user at the given address. The user
should be aware that memory used for buffers descriptors must be 8-byte
aligned and non-cacheable. All the buffer descriptors should fit
in the given memory space.

If this parameter is "NONE", space for buffer descriptors is obtained
by calling cacheDmaMalloc() in motFecEndLoad().

.IP <bufSize>
The memory size parameter specifies the size of the pre-allocated memory
region. If <bufBase> is specified as NONE (-1), the driver ignores this
parameter. Otherwise, the driver checks the size of the provided memory
region is adequate with respect to the given number of Transmit Buffer
Descriptors and Receive Buffer Descriptors.

.IP <fifoTxBase>
Indicate the base location of the transmit FIFO, in internal memory.
The user does not need to initialize this parameter, as the related
FEC register defaults to a proper value after reset. The specific
reset value is microcode dependent. However, if the user wishes to
reserve some RAM for other purposes, he may set this parameter to a
different value. This should not be less than the default.

If <fifoTxBase> is specified as NONE (-1), the driver ignores it.

.IP <fifoRxBase>
Indicate the base location of the receive FIFO, in internal memory.
The user does not need to initialize this parameter, as the related
FEC register defaults to a proper value after reset. The specific
reset value is microcode dependent. However, if the user wishes to
reserve some RAM for other purposes, he may set this parameter to a
different value. This should not be less than the default.

If <fifoRxBase> is specified as NONE (-1), the driver ignores it.

.IP <tbdNum>
This parameter specifies the number of transmit buffer descriptors (TBDs).
Each buffer descriptor resides in 8 bytes of the processor's external
RAM space, and each one points to a 1536-byte buffer again in external
RAM. If this parameter is less than a minimum number specified in the
macro MOT_FEC_TBD_MIN, or if it is "NULL", a default value of 64 is used.
This default number is kept deliberately hugh, since each packet the driver
sends may consume more than a single TBD. This parameter should always
equal a even number.

.IP  <rbdNum>
This parameter specifies the number of receive buffer descriptors (RBDs).
Each buffer descriptor resides in 8 bytes of the processor's external
RAM space, and each one points to a 1536-byte buffer again in external
RAM. If this parameter is less than a minimum number specified in the
macro MOT_FEC_RBD_MIN, or if it is "NULL", a default value of 48 is used.
This parameter should always equal a even number.

.IP  <phyAddr>
This parameter specifies the logical address of a MII-compliant physical
device (PHY) that is to be used as a physical media on the network.
Valid addresses are in the range 0-31. There may be more than one device
under the control of the same management interface. If this parameter is
"NULL", the default physical layer initialization routine will find out the
PHY actual address by scanning the whole range. The one with the lowest
address will be chosen.

.IP  <isoPhyAddr>
This parameter specifies the logical address of a MII-compliant physical
device (PHY) that is to be electrically isolated by the management
interface. Valid addresses are in the range 0-31. If this parameter
equals 0xff, the default physical layer initialization routine will
assume there is no need to isolate any device. However, this parameter
will be ignored unless the MOT_FEC_USR_PHY_ISO bit in the <userFlags>
is set to one.

.IP  <phyDefMode>
This parameter specifies the operating mode that will be set up
by the default physical layer initialization routine in case all
the attempts made to establish a valid link failed. If that happens,
the first PHY that matches the specified abilities will be chosen to
work in that mode, and the physical link will not be tested.

.IP  <userFlags>
This field enables the user to give some degree of customization to the
driver, especially as regards the physical layer interface.

.IP  <clockSpeed>
This field enables the user to define the speed of the clock being used
to drive the interface.  The clock speed is used to derive the MII
management interface clock, which cannot exceed 2.5 MHz.  <clockSpeed>
is optional in BSPs using clocks that are 50 MHz or less, but it is
required in faster designs to ensure proper MII interface operation.

MOT_FEC_USR_PHY_NO_AN: the default physical layer initialization
routine will exploit the auto-negotiation mechanism as described in
the IEEE Std 802.3, to bring a valid link up. According to it, all
the link partners on the media will take part to the negotiation
process, and the highest priority common denominator technology ability
will be chosen. It the user wishes to prevent auto-negotiation from
occurring, he may set this bit in the user flags.

MOT_FEC_USR_PHY_TBL: in the auto-negotiation process, PHYs
advertise all their technology abilities at the same time,
and the result is that the maximum common denominator is used. However,
this behaviour may be changed, and the user may affect the order how
each subset of PHY's abilities is negotiated. Hence, when the
MOT_FEC_USR_PHY_TBL bit is set, the default physical layer
initialization routine will look at the motFecPhyAnOrderTbl[] table and
auto-negotiate a subset of abilities at a time, as suggested by the
table itself. It is worth noticing here, however, that if the
MOT_FEC_USR_PHY_NO_AN bit is on, the above table will be ignored.

MOT_FEC_USR_PHY_NO_FD: the PHY may be set to operate in full duplex mode,
provided it has this ability, as a result of the negotiation with other
link partners. However, in this operating mode, the FEC will ignore the
collision detect and carrier sense signals. If the user wishes not to
negotiate full duplex mode, he should set the MOT_FEC_USR_PHY_NO_FD bit
in the user flags.

MOT_FEC_USR_PHY_NO_HD: the PHY may be set to operate in half duplex mode,
provided it has this ability, as a result of the negotiation with other link
partners. If the user wishes not to negotiate half duplex mode, he should
set the MOT_FEC_USR_PHY_NO_HD bit in the user flags.

MOT_FEC_USR_PHY_NO_100: the PHY may be set to operate at 100Mbit/s speed,
provided it has this ability, as a result of the negotiation with
other link partners. If the user wishes not to negotiate 100Mbit/s speed,
he should set the MOT_FEC_USR_PHY_NO_100 bit in the user flags.

MOT_FEC_USR_PHY_NO_10: the PHY may be set to operate at 10Mbit/s speed,
provided it has this ability, as a result of the negotiation with
other link partners. If the user wishes not to negotiate 10Mbit/s speed,
he should set the MOT_FEC_USR_PHY_NO_10 bit in the user flags.

MOT_FEC_USR_PHY_ISO: some boards may have different PHYs controlled by the
same management interface. In some cases, there may be the need of
electrically isolating some of them from the interface itself, in order
to guarantee a proper behaviour on the medium layer. If the user wishes to
electrically isolate one PHY from the MII interface, he should set the
MOT_FEC_USR_PHY_ISO bit and provide its logical address in the <isoPhyAddr>
field of the load string. The default behaviour is to not isolate any
PHY on the board.

MOT_FEC_USR_SER: the user may set the MOT_FEC_USR_SER bit to enable
the 7-wire interface instead of the MII which is the default.

MOT_FEC_USR_LOOP: when the MOT_FEC_USR_LOOP bit is set, the driver will
configure the FEC to work in loopback mode, with the TX signal directly
connected to the RX. This mode should only be used for testing.

MOT_FEC_USR_HBC: if the MOT_FEC_USR_HBC bit is set, the driver will
configure the FEC to perform heartbeat check following end of transmisson
and the HB bit in the status field of the TBD will be set if the collision
input does not assert within the heartbeat window (also see _func_motFecHbFail,
below). The user does not normally need to set this bit.

.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires three external support functions:
.IP sysFecEnetEnable()
.CS
    STATUS sysFecEnetEnable (UINT32 motCpmAddr);
.CE
This routine is expected to handle any target-specific functions needed
to enable the FEC. These functions typically include setting the Port D
on the 860T-based board so that the MII interface may be used, and also
disabling the IRQ7 signal. This routine is expected to return OK on
success, or ERROR. The driver calls this routine, once per device, from the
motFecEndLoad() routine.
.IP sysFecEnetDisable()
.CS
    STATUS sysFecEnetDisable (UINT32 motCpmAddr);
.CE
This routine is expected to perform any target specific functions required
to disable the MII interface to the FEC.  This involves restoring the
default values for all the Port D signals. This routine is expected to
return OK on success, or ERROR. The driver calls this routine from the
motFecEndStop() routine each time a device is disabled.
.IP sysFecEnetAddrGet()
.CS
STATUS sysFecEnetAddrGet (UINT32 motCpmAddr, UCHAR * enetAddr);
.CE
The driver expects this routine to provide the six-byte Ethernet hardware
address that is used by this device.  This routine must copy the six-byte
address to the space provided by <enetAddr>.  This routine is expected to
return OK on success, or ERROR.  The driver calls this routine, once per
device, from the motFecEndLoad() routine.
.IP `_func_motFecPhyInit'
.CS
    FUNCPTR _func_motFecPhyInit
.CE
This driver sets the global variable `_func_motFecPhyInit' to the
MII-compliant media initialization routine motFecPhyInit(). If the user
wishes to exploit a different way to configure the PHY, he may set
this variable to his own media initialization routine, tipically
in sysHwInit().
.IP `_func_motFecHbFail'
.CS
    FUNCPTR _func_motFecPhyInit
.CE
The FEC may be configured to perform heartbeat check following end
of transmission, and to generate an interrupt, when this event occurs.
If this is the case, and if the global variable `_func_motFecHbFail'
is not NULL, the routine referenced to by `_func_motFecHbFail' is called,
with a pointer to the driver control structure as parameter. Hence,
the user may set this variable to his own heart beat check fail routine,
where he can take any action he sees appropriate.
The default value for the global variable `_func_motFecHbFail' is NULL.
.LP

SYSTEM RESOURCE USAGE
If the driver allocates the memory to share with the Ethernet device,
it does so by calling the cacheDmaMalloc() routine.  For the default case
of 64 transmit buffers and 48 receive buffers, the total size requested
is 912 bytes, and this includes the 16-byte alignment requirement of the
device.  If a non-cacheable memory region is provided by the user, the
size of this region should be this amount, unless the user has specified
a different number of transmit or receive BDs.

This driver can operate only if this memory region is non-cacheable
or if the hardware implements bus snooping.  The driver cannot maintain
cache coherency for the device because the BDs are asynchronously
modified by both the driver and the device, and these fields might share
the same cache line.

Data buffers are instead allocated in the external memory through the
regular memory allocation routine (memalign), and the related cache lines
are then flushed or invalidated as appropriate. The user should not allocate
memory for them.

TUNING HINTS

The only adjustable parameters are the number of TBDs and RBDs that will be
created at run-time.  These parameters are given to the driver when
motFecEndLoad() is called.  There is one RBD associated with each received
frame whereas a single transmit packet normally uses more than one TBD.  For
memory-limited applications, decreasing the number of RBDs may be
desirable.  Decreasing the number of TBDs below a certain point will
provide substantial performance degradation, and is not reccomended. An
adequate number of loaning buffers are also pre-allocated to provide more
buffering before packets are dropped, but this is not configurable.

The relative priority of the netTask and of the other tasks in the system
may heavily affect performance of this driver. Usually the best performance
is achieved when the netTask priority equals that of the other
applications using the driver.

SPECIAL CONSIDERATIONS

Due to the FEC8 errata in the document: "MPC860 Family Device Errata Reference"
available at the Motorola web site, the number of receive buffer
descriptors (RBD) for the FEC (see configNet.h) is kept deliberately high.
According to Motorola, this problem was fixed in Rev. B3 of the silicon.
In memory-bound applications, when using the above mentioned revision of
the MPC860T processor, the user may decrease the number of RBDs
to fit his needs.

SEE ALSO: ifLib,
.I "MPC860T Fast Ethernet Controller (Supplement to the MPC860 User's Manual)"
.I "Motorola MPC860 User's Manual",

INTERNAL
This driver contains conditional compilation switch MOT_FEC_END_DEBUG.
If defined, adds debug output routines.  Output is further
selectable at run-time via the motFecEndDbg global variable.
*/

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
#include "iosLib.h"
#include "errnoLib.h"
#include "vxLib.h" 

#include "cacheLib.h"
#include "logLib.h"
#include "netLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysLib.h"
#include "taskLib.h"

#include "net/systm.h"
#include "sys/times.h"
#include "net/if_subr.h"

#undef ETHER_MAP_IP_MULTICAST
#include "etherMultiLib.h"
#include "end.h"
#include "semLib.h"

#define    END_MACROS
#include "endLib.h"
#include "lstLib.h"

#include "drv/intrCtl/ppc860Intr.h"
#include "drv/end/motFecEnd.h"

/* defines */

/* initial divisor and final XOR value for reflected CRC */

#define INIT_REFLECTED  0xFFFFFFFF
#define XOROT           INIT_REFLECTED

/* Driver debug control */

#define MOT_FEC_DBG

/* Driver debug control */

#ifdef MOT_FEC_DBG
#define MOT_FEC_DBG_OFF			0x0000
#define MOT_FEC_DBG_RX			0x0001
#define MOT_FEC_DBG_TX			0x0002
#define MOT_FEC_DBG_POLL		0x0004
#define MOT_FEC_DBG_MII			0x0008
#define MOT_FEC_DBG_LOAD		0x0010
#define MOT_FEC_DBG_IOCTL		0x0020
#define MOT_FEC_DBG_INT			0x0040
#define MOT_FEC_DBG_START    		0x0080
#define MOT_FEC_DBG_ANY    		0xffff

UINT32	motFecEndDbg = 0x0;
UINT32	motFecBabRxErr = 0x0;
UINT32	motFecBabTxErr = 0x0;
UINT32	motFecHbFailErr = 0x0;
UINT32	motFecTxLcErr = 0x0;
UINT32	motFecTxUrErr = 0x0;
UINT32	motFecTxCslErr = 0x0;
UINT32	motFecTxRlErr = 0x0;
UINT32	motFecRxLgErr = 0x0;
UINT32	motFecRxNoErr = 0x0;
UINT32	motFecRxCrcErr = 0x0;
UINT32	motFecRxOvErr = 0x0;
UINT32	motFecRxTrErr = 0x0;
UINT32  motFecRxLsErr = 0x0;
UINT32  motFecRxMemErr = 0x0;

DRV_CTRL *  pDrvCtrlDbg = NULL;

#define MOT_FEC_BAB_RX_ADD						\
    motFecBabRxErr++;

#define MOT_FEC_BAB_TX_ADD						\
    motFecBabTxErr++;

#define MOT_FEC_HB_FAIL_ADD						\
    motFecHbFailErr++;

#define MOT_FEC_TX_LC_ADD						\
    motFecTxLcErr++;

#define MOT_FEC_TX_UR_ADD						\
    motFecTxUrErr++;

#define MOT_FEC_TX_CSL_ADD						\
    motFecTxCslErr++;

#define MOT_FEC_TX_RL_ADD						\
    motFecTxRlErr++;

#define MOT_FEC_RX_LG_ADD						\
    motFecRxLgErr++;

#define MOT_FEC_RX_NO_ADD						\
    motFecRxNoErr++;

#define MOT_FEC_RX_CRC_ADD						\
    motFecRxCrcErr++;

#define MOT_FEC_RX_OV_ADD						\
    motFecRxOvErr++;

#define MOT_FEC_RX_TR_ADD						\
    motFecRxTrErr++;

#define MOT_FEC_RX_MEM_ADD						\
    motFecRxMemErr++;

#define MOT_FEC_RX_LS_ADD						\
    motFecRxLsErr++;

#define MOT_FEC_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)                    \
    {                                                                   \
    if (motFecEndDbg & FLG)                                             \
        logMsg (X0, X1, X2, X3, X4, X5, X6);                            \
    }

#else /* MOT_FEC_DBG */

#define MOT_FEC_BAB_RX_ADD
#define MOT_FEC_BAB_TX_ADD
#define MOT_FEC_HB_FAIL_ADD
#define MOT_FEC_TX_LC_ADD
#define MOT_FEC_TX_UR_ADD
#define MOT_FEC_TX_CSL_ADD
#define MOT_FEC_TX_RL_ADD
#define MOT_FEC_RX_MEM_ADD
#define MOT_FEC_RX_LS_ADD
#define MOT_FEC_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)

#endif /* MOT_FEC_DBG */

/* general macros for reading/writing from/to specified locations */

/* Cache and virtual/physical memory related macros */

#define MOT_FEC_PHYS_TO_VIRT(physAddr)					    \
	CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->bdCacheFuncs, (char *)(physAddr))

#define MOT_FEC_VIRT_TO_PHYS(virtAddr)					    \
	CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->bdCacheFuncs, (char *)(virtAddr))

#define MOT_FEC_BD_CACHE_INVAL(address, len)			    	    \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->bdCacheFuncs, (address), (len))

#define MOT_FEC_CACHE_INVAL(address, len)                                   \
	CACHE_DRV_INVALIDATE (&pDrvCtrl->bufCacheFuncs, (address), (len))

#define MOT_FEC_CACHE_FLUSH(address, len)                                   \
	CACHE_DRV_FLUSH (&pDrvCtrl->bufCacheFuncs, (address), (len))

/* driver flags */

#define MOT_FEC_OWN_MEM		0x01	/* internally provided memory */
#define MOT_FEC_INV_TBD_NUM	0x02	/* invalid tbdNum provided */
#define MOT_FEC_INV_RBD_NUM	0x04	/* invalid rbdNum provided */
#define MOT_FEC_POLLING		0x08	/* polling mode */
#define MOT_FEC_PROM		0x20    /* promiscuous mode */
#define MOT_FEC_MCAST		0x40    /* multicast addressing mode */
#define MOT_FEC_FD		0x80    /* full duplex mode */

/* shortcuts */

#define MOT_FEC_FLAG_CLEAR(clearBits)					\
    (pDrvCtrl->flags &= ~(clearBits))

#define MOT_FEC_FLAG_SET(setBits)					\
    (pDrvCtrl->flags |= (setBits))

#define MOT_FEC_FLAG_GET()						\
    (pDrvCtrl->flags)

#define MOT_FEC_FLAG_ISSET(setBits)					\
    (pDrvCtrl->flags & (setBits))

#define MOT_FEC_PHY_FLAGS_SET(setBits)					\
    (pDrvCtrl->phyInfo->phyFlags |= (setBits))

#define MOT_FEC_PHY_FLAGS_ISSET(setBits)				\
    (pDrvCtrl->phyInfo->phyFlags & (setBits))

#define MOT_FEC_PHY_FLAGS_GET(setBits)					\
    (pDrvCtrl->phyInfo->phyFlags)

#define MOT_FEC_PHY_FLAGS_CLEAR(clearBits)				\
    (pDrvCtrl->phyInfo->phyFlags &= ~(clearBits))

#define MOT_FEC_USR_FLAG_ISSET(setBits)					\
    (pDrvCtrl->userFlags & (setBits))

#define END_FLAGS_ISSET(setBits)					\
    ((&pDrvCtrl->endObj)->flags & (setBits))

#define MOT_FEC_ADDR_GET(pEnd)                                          \
    ((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define MOT_FEC_ADDR_LEN_GET(pEnd)                                      \
    ((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

/* some BDs definitions */

/*
 * the total is 0x600 and it accounts for the required alignment
 * of receive data buffers, and the cluster overhead.
 */

#define MOT_FEC_MAX_CL_LEN      ((MOT_FEC_MAX_PCK_SZ			\
				 + (MOT_FEC_BD_ALIGN - 1)		\
				 + (CL_OVERHEAD - 1))			\
				 & (~ (CL_OVERHEAD - 1)))

#define MOT_FEC_MAX_RX_BUF	((MOT_FEC_MAX_PCK_SZ + MOT_FEC_BD_ALIGN - 1) \
				& (~(MOT_FEC_BD_ALIGN - 1)))

#define MOT_FEC_BUF_V_LEN	(MOT_FEC_MAX_RX_BUF + 16)

#define MOT_FEC_RX_BUF_SZ       (MOT_FEC_MAX_CL_LEN)
#define MOT_FEC_TX_BUF_SZ       (MOT_FEC_MAX_CL_LEN)

#define MOT_FEC_MEM_SZ(pDrvCtrl)					\
    ((pDrvCtrl)->bufSize)

#define MOT_FEC_LOAN_SZ							\
    (MOT_FEC_RX_BUF_SZ * MOT_FEC_BD_LOAN_NUM)

#define MOT_FEC_RX_MEM(pDrvCtrl)					\
    (MOT_FEC_RX_BUF_SZ * ((pDrvCtrl)->rbdNum + MOT_FEC_BD_LOAN_NUM))

#define MOT_FEC_TX_MEM(pDrvCtrl)					\
    (MOT_FEC_TX_BUF_SZ * (MOT_FEC_TBD_POLL_NUM + MOT_FEC_TX_CL_NUM))

#define MOT_FEC_TBD_MEM(pDrvCtrl)					\
    (MOT_FEC_TBD_SZ * (pDrvCtrl)->tbdNum)

#define MOT_FEC_RBD_MEM(pDrvCtrl)					\
    (MOT_FEC_RBD_SZ * (pDrvCtrl)->rbdNum)

#define MOT_FEC_BD_MEM(pDrvCtrl)					\
    (MOT_FEC_TBD_MEM (pDrvCtrl) + MOT_FEC_RBD_MEM (pDrvCtrl))

/* Control/Status Registers (CSR) definitions */

#define MOT_FEC_CSR_WR(csrOff, csrVal)					\
    {									\
    UINT32 temp = 0;							\
									\
    temp = (UINT32) MOT_FEC_VIRT_TO_PHYS ((pDrvCtrl->motCpmAddr) 	\
					  + (csrOff));			\
									\
    MOT_FEC_LONG_WR ((UINT32 *) (temp), (csrVal));			\
    }									

#define MOT_FEC_CSR_RD(csrOff, csrVal)					\
    {									\
    UINT32 temp = 0;							\
									\
    temp = (UINT32) MOT_FEC_VIRT_TO_PHYS ((pDrvCtrl->motCpmAddr) 	\
					  + (csrOff));			\
									\
    MOT_FEC_LONG_RD ((UINT32 *) (temp), (csrVal));			\
    }									

/* macros to read/write tx/rx buffer descriptors */

#define MOT_FEC_BD_WORD_WR(bdAddr, bdOff, bdVal)			\
    {									\
    UINT32 temp = 0;							\
									\
    temp = (UINT32) MOT_FEC_VIRT_TO_PHYS ((bdAddr) + (bdOff));		\
									\
    MOT_FEC_WORD_WR ((UINT16 *) (temp), (bdVal));			\
    }									

/* here we're writing the data pointer, so it's a physical address */

#define MOT_FEC_BD_LONG_WR(bdAddr, bdOff, bdVal)			\
    {									\
    UINT32 temp1 = 0;							\
    UINT32 temp2 = 0;							\
									\
    temp1 = (UINT32) MOT_FEC_VIRT_TO_PHYS ((bdAddr) + (bdOff));		\
    temp2 = (UINT32) MOT_FEC_VIRT_TO_PHYS (bdVal);			\
									\
    MOT_FEC_LONG_WR ((UINT32 *) (temp1), (temp2));			\
    }									

#define MOT_FEC_BD_WORD_RD(bdAddr, bdOff, bdVal)			\
    {									\
    UINT32 temp = 0;							\
									\
    temp = (UINT32) MOT_FEC_VIRT_TO_PHYS ((bdAddr) + (bdOff));		\
									\
    MOT_FEC_WORD_RD ((UINT16 *) (temp), (bdVal));					\
    }									

#define MOT_FEC_BD_LONG_RD(bdAddr, bdOff, bdVal)			\
    {									\
    UINT32 temp1 = 0;							\
    UINT32 temp2 = 0;							\
									\
    temp1 = (UINT32) MOT_FEC_VIRT_TO_PHYS ((bdAddr) + (bdOff));		\
									\
    MOT_FEC_LONG_RD ((UINT32 *) (temp1), (temp2));			\
									\
    bdVal = (char *) MOT_FEC_PHYS_TO_VIRT (temp2);			\
    }									

#define MOT_FEC_NEXT_TBD(pDrvCtrl, pTbd)				\
    (pTbd) = (MOT_FEC_TBD_ID) ((pDrvCtrl)->tbdBase +   \
			       ((pDrvCtrl)->tbdIndex * MOT_FEC_TBD_SZ))

#define MOT_FEC_USED_TBD(pDrvCtrl, pTbd)				   \
    (pTbd) = (MOT_FEC_TBD_ID) ((pDrvCtrl)->tbdBase + 	   \
			       ((pDrvCtrl)->usedTbdIndex * MOT_FEC_TBD_SZ))

#define MOT_FEC_NEXT_RBD(pDrvCtrl, pRbd)				   \
    (pRbd) = (MOT_FEC_RBD_ID) ((pDrvCtrl)->rbdBase + 	   \
			       (((pDrvCtrl)->rbdIndex) * MOT_FEC_RBD_SZ))

#define MOT_FEC_ETH_SET                                                 \
    MOT_FEC_CSR_WR (MOT_FEC_CTRL_OFF, pinMux);

#define MOT_FEC_ETH_ENABLE						\
    MOT_FEC_CSR_WR (MOT_FEC_CTRL_OFF, (MOT_FEC_ETH_EN | pinMux));

#define MOT_FEC_ETH_DISABLE						\
    MOT_FEC_CSR_WR (MOT_FEC_CTRL_OFF, (MOT_FEC_ETH_DIS | pinMux));

#define MOT_FEC_RX_ACTIVATE						\
    MOT_FEC_CSR_WR (MOT_FEC_RX_ACT_OFF, MOT_FEC_RX_ACT);

#define MOT_FEC_TX_ACTIVATE						\
    MOT_FEC_CSR_WR (MOT_FEC_TX_ACT_OFF, MOT_FEC_TX_ACT);

#define MOT_FEC_INT_DISABLE						\
    MOT_FEC_CSR_WR (MOT_FEC_MASK_OFF, 0);

#define MOT_FEC_INT_ENABLE						\
    MOT_FEC_CSR_WR (MOT_FEC_MASK_OFF, (pDrvCtrl->intMask));

#define MOT_FEC_VECTOR(pDrvCtrl)					\
    ((pDrvCtrl)->ivec)

/* more shortcuts */

#define MOT_FEC_GRA_SEM_CREATE						\
    if ((pDrvCtrl->graSem = semBCreate (SEM_Q_FIFO, SEM_EMPTY)) 	\
	== NULL)							\
	goto errorExit

#define MOT_FEC_GRA_SEM_DELETE						\
    if ((pDrvCtrl->graSem) != NULL) 					\
	semDelete (pDrvCtrl->graSem)

#define MOT_FEC_GRA_SEM_GIVE						\
    (semGive (pDrvCtrl->graSem))

#define MOT_FEC_GRA_SEM_TAKE						\
    (semTake (pDrvCtrl->graSem, WAIT_FOREVER))

#define MOT_FEC_MII_SEM_CREATE						\
    if ((pDrvCtrl->miiSem = semBCreate (SEM_Q_FIFO, SEM_EMPTY)) 	\
	== NULL)							\
	goto errorExit

#define MOT_FEC_MII_SEM_DELETE						\
    if ((pDrvCtrl->miiSem) != NULL) 					\
	semDelete (pDrvCtrl->miiSem)

#define MOT_FEC_MII_SEM_GIVE						\
    (semGive (pDrvCtrl->miiSem))

#define MOT_FEC_MII_SEM_TAKE						\
    (semTake (pDrvCtrl->miiSem, WAIT_FOREVER))

#define NET_BUF_ALLOC()                                                 \
    netClusterGet (pDrvCtrl->endObj.pNetPool, pDrvCtrl->pClPoolId)	\

#define NET_TO_MOT_FEC_BUF(netBuf)                              	\
    (((UINT32) (netBuf) + MOT_FEC_BD_ALIGN - 1) 			\
      & ~(MOT_FEC_BD_ALIGN - 1))

#define NET_BUF_FREE(pBuffer)                                           \
    netClFree (pDrvCtrl->endObj.pNetPool, (pBuffer))

#define NET_MBLK_ALLOC()                                                \
    netMblkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT, MT_DATA)

#define NET_MBLK_FREE(pMblock)                                          \
    netMblkFree (pDrvCtrl->endObj.pNetPool, (M_BLK_ID) (pMblock))

#define NET_CL_BLK_ALLOC()                                              \
    netClBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT)

#define NET_CL_BLK_FREE(pClBlock)                                       \
    netClBlkFree (pDrvCtrl->endObj.pNetPool, (CL_BLK_ID) (pClBlock))

#define NET_MBLK_BUF_FREE(pMblock)                                      \
    netMblkClFree ((M_BLK_ID) (pMblock))

#define NET_MBLK_CL_JOIN(pMblock, pClBlock, ret)                        \
    ret = netMblkClJoin ((pMblock), (pClBlock))

#define NET_CL_BLK_JOIN(pClBlock, pBuffer, length, ret)                 \
    ret = netClBlkJoin ((pClBlock), (pBuffer), (length), NULL, 0, 0, 0)

/* globals */

/* external */
IMPORT UINT32 vxImmrGet(void);

/* locals */

LOCAL int pinMux;

/* intermediate remainders for table-driven CRC calculations */

LOCAL UINT32  crctable[256] =
{
 0x00000000L, 0x77073096L, 0xEE0E612CL, 0x990951BAL,
 0x076DC419L, 0x706AF48FL, 0xE963A535L, 0x9E6495A3L,
 0x0EDB8832L, 0x79DCB8A4L, 0xE0D5E91EL, 0x97D2D988L,
 0x09B64C2BL, 0x7EB17CBDL, 0xE7B82D07L, 0x90BF1D91L,
 0x1DB71064L, 0x6AB020F2L, 0xF3B97148L, 0x84BE41DEL,
 0x1ADAD47DL, 0x6DDDE4EBL, 0xF4D4B551L, 0x83D385C7L,
 0x136C9856L, 0x646BA8C0L, 0xFD62F97AL, 0x8A65C9ECL,
 0x14015C4FL, 0x63066CD9L, 0xFA0F3D63L, 0x8D080DF5L,
 0x3B6E20C8L, 0x4C69105EL, 0xD56041E4L, 0xA2677172L,
 0x3C03E4D1L, 0x4B04D447L, 0xD20D85FDL, 0xA50AB56BL,
 0x35B5A8FAL, 0x42B2986CL, 0xDBBBC9D6L, 0xACBCF940L,
 0x32D86CE3L, 0x45DF5C75L, 0xDCD60DCFL, 0xABD13D59L,
 0x26D930ACL, 0x51DE003AL, 0xC8D75180L, 0xBFD06116L,
 0x21B4F4B5L, 0x56B3C423L, 0xCFBA9599L, 0xB8BDA50FL,
 0x2802B89EL, 0x5F058808L, 0xC60CD9B2L, 0xB10BE924L,
 0x2F6F7C87L, 0x58684C11L, 0xC1611DABL, 0xB6662D3DL,
 0x76DC4190L, 0x01DB7106L, 0x98D220BCL, 0xEFD5102AL,
 0x71B18589L, 0x06B6B51FL, 0x9FBFE4A5L, 0xE8B8D433L,
 0x7807C9A2L, 0x0F00F934L, 0x9609A88EL, 0xE10E9818L,
 0x7F6A0DBBL, 0x086D3D2DL, 0x91646C97L, 0xE6635C01L,
 0x6B6B51F4L, 0x1C6C6162L, 0x856530D8L, 0xF262004EL,
 0x6C0695EDL, 0x1B01A57BL, 0x8208F4C1L, 0xF50FC457L,
 0x65B0D9C6L, 0x12B7E950L, 0x8BBEB8EAL, 0xFCB9887CL,
 0x62DD1DDFL, 0x15DA2D49L, 0x8CD37CF3L, 0xFBD44C65L,
 0x4DB26158L, 0x3AB551CEL, 0xA3BC0074L, 0xD4BB30E2L,
 0x4ADFA541L, 0x3DD895D7L, 0xA4D1C46DL, 0xD3D6F4FBL,
 0x4369E96AL, 0x346ED9FCL, 0xAD678846L, 0xDA60B8D0L,
 0x44042D73L, 0x33031DE5L, 0xAA0A4C5FL, 0xDD0D7CC9L,
 0x5005713CL, 0x270241AAL, 0xBE0B1010L, 0xC90C2086L,
 0x5768B525L, 0x206F85B3L, 0xB966D409L, 0xCE61E49FL,
 0x5EDEF90EL, 0x29D9C998L, 0xB0D09822L, 0xC7D7A8B4L,
 0x59B33D17L, 0x2EB40D81L, 0xB7BD5C3BL, 0xC0BA6CADL,
 0xEDB88320L, 0x9ABFB3B6L, 0x03B6E20CL, 0x74B1D29AL,
 0xEAD54739L, 0x9DD277AFL, 0x04DB2615L, 0x73DC1683L,
 0xE3630B12L, 0x94643B84L, 0x0D6D6A3EL, 0x7A6A5AA8L,
 0xE40ECF0BL, 0x9309FF9DL, 0x0A00AE27L, 0x7D079EB1L,
 0xF00F9344L, 0x8708A3D2L, 0x1E01F268L, 0x6906C2FEL,
 0xF762575DL, 0x806567CBL, 0x196C3671L, 0x6E6B06E7L,
 0xFED41B76L, 0x89D32BE0L, 0x10DA7A5AL, 0x67DD4ACCL,
 0xF9B9DF6FL, 0x8EBEEFF9L, 0x17B7BE43L, 0x60B08ED5L,
 0xD6D6A3E8L, 0xA1D1937EL, 0x38D8C2C4L, 0x4FDFF252L,
 0xD1BB67F1L, 0xA6BC5767L, 0x3FB506DDL, 0x48B2364BL,
 0xD80D2BDAL, 0xAF0A1B4CL, 0x36034AF6L, 0x41047A60L,
 0xDF60EFC3L, 0xA867DF55L, 0x316E8EEFL, 0x4669BE79L,
 0xCB61B38CL, 0xBC66831AL, 0x256FD2A0L, 0x5268E236L,
 0xCC0C7795L, 0xBB0B4703L, 0x220216B9L, 0x5505262FL,
 0xC5BA3BBEL, 0xB2BD0B28L, 0x2BB45A92L, 0x5CB36A04L,
 0xC2D7FFA7L, 0xB5D0CF31L, 0x2CD99E8BL, 0x5BDEAE1DL,
 0x9B64C2B0L, 0xEC63F226L, 0x756AA39CL, 0x026D930AL,
 0x9C0906A9L, 0xEB0E363FL, 0x72076785L, 0x05005713L,
 0x95BF4A82L, 0xE2B87A14L, 0x7BB12BAEL, 0x0CB61B38L,
 0x92D28E9BL, 0xE5D5BE0DL, 0x7CDCEFB7L, 0x0BDBDF21L,
 0x86D3D2D4L, 0xF1D4E242L, 0x68DDB3F8L, 0x1FDA836EL,
 0x81BE16CDL, 0xF6B9265BL, 0x6FB077E1L, 0x18B74777L,
 0x88085AE6L, 0xFF0F6A70L, 0x66063BCAL, 0x11010B5CL,
 0x8F659EFFL, 0xF862AE69L, 0x616BFFD3L, 0x166CCF45L,
 0xA00AE278L, 0xD70DD2EEL, 0x4E048354L, 0x3903B3C2L,
 0xA7672661L, 0xD06016F7L, 0x4969474DL, 0x3E6E77DBL,
 0xAED16A4AL, 0xD9D65ADCL, 0x40DF0B66L, 0x37D83BF0L,
 0xA9BCAE53L, 0xDEBB9EC5L, 0x47B2CF7FL, 0x30B5FFE9L,
 0xBDBDF21CL, 0xCABAC28AL, 0x53B39330L, 0x24B4A3A6L,
 0xBAD03605L, 0xCDD70693L, 0x54DE5729L, 0x23D967BFL,
 0xB3667A2EL, 0xC4614AB8L, 0x5D681B02L, 0x2A6F2B94L,
 0xB40BBE37L, 0xC30C8EA1L, 0x5A05DF1BL, 0x2D02EF8DL
};

/* Function declarations not in any header files */

/* forward function declarations */

LOCAL STATUS    motFecInitParse (DRV_CTRL *pDrvCtrl, char *initString);
LOCAL STATUS    motFecInitMem (DRV_CTRL *pDrvCtrl);
LOCAL STATUS    motFecSend (DRV_CTRL *pDrvCtrl, M_BLK *pMblk);

LOCAL STATUS    motFecPhyInit (DRV_CTRL *pDrvCtrl);
LOCAL STATUS    motFecPhyPreInit (DRV_CTRL *pDrvCtrl);
LOCAL STATUS 	motFecReset (DRV_CTRL *pDrvCtrl);
LOCAL STATUS    motFecBdFree (DRV_CTRL *pDrvCtrl);
LOCAL STATUS    motFecRbdInit (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	motFecTbdInit (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	motFecRxCtrlRegValGet (DRV_CTRL *pDrvCtrl, UINT32 * rxCtrlVal);
LOCAL STATUS	motFecTxCtrlRegValGet (DRV_CTRL *pDrvCtrl, UINT32 * txCtrlVal);
LOCAL STATUS	motFecHashRegValGet (DRV_CTRL *pDrvCtrl, UINT32  crcVal,
			     UINT32 * csrVal, UINT16 * offset);
LOCAL STATUS	motFecAddrRegValGet (DRV_CTRL *pDrvCtrl, UINT32  * csr0Val,
			     UINT32 * csr1Val);
LOCAL UINT32	motFecCrcGet (char * pAddr);
LOCAL STATUS	motFecHashTblPopulate (DRV_CTRL * pDrvCtrl);
LOCAL STATUS	motFecHashTblAdd (DRV_CTRL * pDrvCtrl, char * pAddr);

LOCAL STATUS	motFecPrePhyConfig (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	motFecPostPhyConfig (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	motFecMblkWalk (M_BLK * pMblk, UINT8 * pFragNum,
				UINT16 *    pPktType);
LOCAL STATUS	motFecPktTransmit (DRV_CTRL *pDrvCtrl, M_BLK * pMblk,
				   UINT8  fragNum, UINT16     pktType);
LOCAL STATUS	motFecPktCopyTransmit (DRV_CTRL *pDrvCtrl, M_BLK * pMblk,
				   UINT8  fragNum, UINT16     pktType);
LOCAL void	motFecTbdClean (DRV_CTRL *pDrvCtrl);
LOCAL UINT32	motFecTbdCheck (DRV_CTRL *pDrvCtrl, MOT_FEC_TBD_ID pUsedTbd);
LOCAL MOT_FEC_TBD_LIST_ID motFecTbdListSet (DRV_CTRL *pDrvCtrl, UINT8 tbdNum);
LOCAL MOT_FEC_TBD_ID	motFecTbdGet (DRV_CTRL *pDrvCtrl);
LOCAL void	motFecInt (DRV_CTRL *pDrvCtrl);

LOCAL void      motFecRxTxHandle (DRV_CTRL *pDrvCtrl);

LOCAL void      motFecHandleRecvInt (DRV_CTRL *pDrvCtrl);
LOCAL void	motFecReceive (DRV_CTRL *pDrvCtrl, MOT_FEC_RBD_ID pRfd);
LOCAL STATUS	motFecMiiRead (DRV_CTRL *pDrvCtrl, UINT8 phyAddr,
			       UINT8 regAddr, UINT16 *retVal);
LOCAL STATUS	motFecMiiWrite (DRV_CTRL *pDrvCtrl, UINT8 phyAddr,
				UINT8 regAddr, UINT16 writeData);

LOCAL STATUS	motFecMiiDiag (DRV_CTRL *pDrvCtrl, UINT8 phyAddr);
LOCAL STATUS	motFecMiiIsolate (DRV_CTRL *pDrvCtrl, UINT8 isoPhyAddr);

LOCAL STATUS	motFecMiiAnRun (DRV_CTRL *pDrvCtrl, UINT8 phyAddr);
LOCAL STATUS	motFecMiiAnStart (DRV_CTRL *pDrvCtrl, UINT8 phyAddr);
LOCAL STATUS	motFecMiiModeForce (DRV_CTRL *pDrvCtrl, UINT8 phyAddr);
LOCAL STATUS	motFecMiiDefForce (DRV_CTRL *pDrvCtrl, UINT8 phyAddr);
LOCAL STATUS	motFecMiiAnCheck (DRV_CTRL *pDrvCtrl, UINT8 phyAddr);
LOCAL STATUS	motFecMiiProbe (DRV_CTRL *pDrvCtrl, UINT8 phyAddr);
LOCAL void	motFecRbdClean (DRV_CTRL *pDrvCtrl,
				volatile MOT_FEC_RBD_ID pRBDNew);
LOCAL int       motRevNumGet (void);

/* END Specific interfaces. */

END_OBJ *	motFecEndLoad (char *initString);
LOCAL STATUS    motFecStart (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	motFecUnload (DRV_CTRL *pDrvCtrl);
LOCAL STATUS    motFecStop (DRV_CTRL *pDrvCtrl);
LOCAL int       motFecIoctl (DRV_CTRL *pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS    motFecSend (DRV_CTRL *pDrvCtrl, M_BLK_ID pMblk);
LOCAL STATUS    motFecMCastAddrAdd (DRV_CTRL *pDrvCtrl, char* pAddress);
LOCAL STATUS    motFecMCastAddrDel (DRV_CTRL *pDrvCtrl, char* pAddress);
LOCAL STATUS    motFecMCastAddrGet (DRV_CTRL *pDrvCtrl,
                                        MULTI_TABLE *pTable);
LOCAL STATUS    motFecPollSend (DRV_CTRL *pDrvCtrl, M_BLK_ID pMblk);
LOCAL STATUS    motFecPollReceive (DRV_CTRL *pDrvCtrl, M_BLK_ID pMblk);
LOCAL STATUS    motFecPollStart (DRV_CTRL *pDrvCtrl);
LOCAL STATUS    motFecPollStop (DRV_CTRL *pDrvCtrl);

/* globals */

FUNCPTR _func_motFecPhyInit = (FUNCPTR) motFecPhyInit;
FUNCPTR _func_motFecHbFail = (FUNCPTR) NULL;

#ifdef MOT_FEC_DBG
void motFecCsrShow (void);
void motFecRbdShow (int);
void motFecTbdShow (int);
void motFecErrorShow (void);
void motFecDrvShow (void);
void motFecMiiShow (void);
void motFecMibShow (void);

#endif /* MOT_FEC_DBG */

/*
 * Define the device function table.  This is static across all driver
 * instances.
 */

LOCAL NET_FUNCS netFuncs =
    {
    (FUNCPTR) motFecStart,		/* start func. */		
    (FUNCPTR) motFecStop,		/* stop func. */
    (FUNCPTR) motFecUnload,		/* unload func. */		
    (FUNCPTR) motFecIoctl,		/* ioctl func. */		
    (FUNCPTR) motFecSend,		/* send func. */		
    (FUNCPTR) motFecMCastAddrAdd,    	/* multicast add func. */	
    (FUNCPTR) motFecMCastAddrDel,    	/* multicast delete func. */
    (FUNCPTR) motFecMCastAddrGet,    	/* multicast get fun. */	
    (FUNCPTR) motFecPollSend,    	/* polling send func. */	
    (FUNCPTR) motFecPollReceive,    	/* polling receive func. */
    endEtherAddressForm,   		/* put address info into a NET_BUFFER */
    (FUNCPTR) endEtherPacketDataGet, 	/* get pointer to data in NET_BUFFER */
    (FUNCPTR) endEtherPacketAddrGet  	/* Get packet addresses */
    };		

/*******************************************************************************
*
* motFecEndLoad - initialize the driver and device
*
* This routine initializes both driver and device to an operational state
* using device specific parameters specified by <initString>.
*
* The parameter string, <initString>, is an ordered list of parameters each
* separated by a colon. The format of <initString> is:
*
* "<motCpmAddr>:<ivec>:<bufBase>:<bufSize>:<fifoTxBase>:<fifoRxBase>
* :<tbdNum>:<rbdNum>:<phyAddr>:<isoPhyAddr>:<phyDefMode>:<userFlags>
* :<clockSpeed>"
*
* The FEC shares a region of memory with the driver.  The caller of this
* routine can specify the address of this memory region, or can specify that
* the driver must obtain this memory region from the system resources.
*
* A default number of transmit/receive buffer descriptors of 32 can be
* selected by passing zero in the parameters <tbdNum> and <rbdNum>.
* In other cases, the number of buffers selected should be greater than two.
*
* The <bufBase> parameter is used to inform the driver about the shared
* memory region.  If this parameter is set to the constant "NONE," then this
* routine will attempt to allocate the shared memory from the system.  Any
* other value for this parameter is interpreted by this routine as the address
* of the shared memory region to be used. The <bufSize> parameter is used
* to check that this region is large enough with respect to the provided
* values of both transmit/receive buffer descriptors.
*
* If the caller provides the shared memory region, then the driver assumes
* that this region does not require cache coherency operations, nor does it
* require conversions between virtual and physical addresses.
*
* If the caller indicates that this routine must allocate the shared memory
* region, then this routine will use cacheDmaMalloc() to obtain
* some  cache-safe memory.  The attributes of this memory will be checked,
* and if the memory is not write coherent, this routine will abort and
* return NULL.
*
* RETURNS: an END object pointer, or NULL on error.
*
* SEE ALSO: ifLib,
* .I "MPC860T Fast Ethernet Controller (Supplement to MPC860 User's Manual)"
*/

END_OBJ* motFecEndLoad
    (
    char *initString      /* parameter string */
    )
    {
    DRV_CTRL *	pDrvCtrl = NULL;	/* pointer to DRV_CTRL structure */
    UCHAR   	enetAddr[6];		/* ethernet address */

    MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("Loading end...\n"), 1, 2, 3, 4, 5, 6);

    if (initString == NULL)
	return (NULL);

    if (initString[0] == 0)
	{
	bcopy ((char *)MOT_FEC_DEV_NAME, (void *)initString,
		MOT_FEC_DEV_NAME_LEN);
	return (NULL);
	}

    /* allocate the device structure */

    pDrvCtrl = (DRV_CTRL *) calloc (sizeof (DRV_CTRL), 1);
    if (pDrvCtrl == NULL)
	return (NULL);

    pDrvCtrl->phyInfo = (PHY_INFO *) calloc (sizeof (PHY_INFO), 1);
    if (pDrvCtrl->phyInfo == NULL)
	{
	free ((char *)pDrvCtrl);
	return (NULL);
	}

#ifdef MOT_FEC_DBG
    pDrvCtrlDbg = pDrvCtrl;
#endif /* MOT_FEC_DBG */

    /* Parse InitString */

    if (motFecInitParse (pDrvCtrl, initString) == ERROR)
	goto errorExit;	

    pDrvCtrl->ilevel = (int) IVEC_TO_ILEVEL (pDrvCtrl->ivec);

    /* sanity check the unit number */

    if (pDrvCtrl->unit < 0 )
	goto errorExit;

    /* memory initialization */

    if (motFecInitMem (pDrvCtrl) == ERROR)
	goto errorExit;

    /* get our ethernet hardware address */

    SYS_FEC_ENET_ADDR_GET (enetAddr);

    MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("motFecEndLoad: enetAddr= \n\
				    0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n "),
				    enetAddr[0],
				    enetAddr[1],
				    enetAddr[2],
				    enetAddr[3],
				    enetAddr[4],
				    enetAddr[5]);

    /* initialize some flags */

    pDrvCtrl->loaded = TRUE;
    pDrvCtrl->intrConnect = FALSE;

    /*
     * create the synchronization semaphores for mii intr handling
     * and for graceful transmit command interrupts.
     */

    MOT_FEC_MII_SEM_CREATE;
    MOT_FEC_GRA_SEM_CREATE;

    /* endObj initializations */

    if (END_OBJ_INIT (&pDrvCtrl->endObj, (DEV_OBJ*) pDrvCtrl,
		      MOT_FEC_DEV_NAME, pDrvCtrl->unit, &netFuncs,
		      "Motorola FEC Ethernet Enhanced Network Driver")
	== ERROR)
	goto errorExit;

    pDrvCtrl->phyInfo->phySpeed = MOT_FEC_10MBS;
    if (END_MIB_INIT (&pDrvCtrl->endObj, M2_ifType_ethernet_csmacd,
		      (u_char *) &enetAddr[0], MOT_FEC_ADDR_LEN,
		      ETHERMTU, pDrvCtrl->phyInfo->phySpeed) == ERROR)
	goto errorExit;

    /* Mark the device ready */

    END_OBJ_READY (&pDrvCtrl->endObj,
                   IFF_NOTRAILERS | IFF_MULTICAST | IFF_BROADCAST);

    MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("motFecEndLoad... Done \n"),
				    1, 2, 3, 4, 5, 6);

    /*
     * determine the revision number of the processor, and set pinMux
     * bit accordingly
     */

    if (motRevNumGet () >= REV_D_3)
        pinMux = MOT_FEC_ETH_PINMUX;
    else
        pinMux = 0;

    return (&pDrvCtrl->endObj);

errorExit:
    motFecUnload (pDrvCtrl);
    free ((char *) pDrvCtrl);
    return NULL;
    }

/*******************************************************************************
*
* motFecUnload - unload a driver from the system
*
* This routine unloads the driver pointed to by <pDrvCtrl> from the system.
*
* RETURNS: OK, always.
*
* SEE ALSO: motFecLoad()
*/

LOCAL STATUS motFecUnload
    (
    DRV_CTRL	*pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    int		ix = 0;		/* a counter */

    MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("Unloading end..."), 1, 2, 3, 4, 5, 6);

    if (pDrvCtrl == NULL)
	return (ERROR);

    pDrvCtrl->loaded = FALSE;

    /* free lists */

    for (ix = 0; ix < pDrvCtrl->tbdNum; ix++)
	{
	cfree ((char *) pDrvCtrl->pTbdList [ix]);
	}

    END_OBJECT_UNLOAD (&pDrvCtrl->endObj);

    /* free allocated memory if necessary */

    if ((MOT_FEC_FLAG_ISSET (MOT_FEC_OWN_MEM)) &&
	(pDrvCtrl->pBufBase != NULL))
	cacheDmaFree (pDrvCtrl->pBufBase);

    /* free allocated memory if necessary */

    if ((pDrvCtrl->pMBlkArea) != NULL)
	free (pDrvCtrl->pMBlkArea);

    /* free the semaphores if necessary */

    MOT_FEC_MII_SEM_DELETE;
    MOT_FEC_GRA_SEM_DELETE;

    free (pDrvCtrl->phyInfo);

    MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("motFecUnload... Done\n"),
				    1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*******************************************************************************
*
* motFecInitParse - parse parameter values from initString
*
* This routine parses parameter values from initString and stores them in
* the related fiels of the driver control structure.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS motFecInitParse
    (
    DRV_CTRL *	pDrvCtrl,      		/* pointer to DRV_CTRL structure */
    char *	initString		/* parameter string */
    )
    {
    char *  tok;		/* an initString token */
    char *  holder = NULL;	/* points to initString fragment beyond tok */

    tok = strtok_r (initString, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->unit = atoi (tok);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->motCpmAddr = (UINT32) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->ivec = (int) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->pBufBase = (char *) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->bufSize = strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->fifoTxBase = (UINT32) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->fifoRxBase = (UINT32) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->tbdNum = (UINT16) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->rbdNum = (UINT16) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->phyInfo->phyAddr = (UINT8) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->phyInfo->isoPhyAddr = (UINT8) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->phyInfo->phyDefMode = (UINT8) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->userFlags = strtoul (tok, NULL, 16);

    /* start of optional parameters */

    tok = strtok_r (NULL, ":", &holder);
    if (tok != NULL)
	pDrvCtrl->clockSpeed = atoi (tok);

    if (!pDrvCtrl->tbdNum || pDrvCtrl->tbdNum <= 2)
	{
	MOT_FEC_FLAG_SET (MOT_FEC_INV_TBD_NUM);
	pDrvCtrl->tbdNum = MOT_FEC_TBD_DEF_NUM;
	}

    if (!pDrvCtrl->rbdNum || pDrvCtrl->rbdNum <= 2)
	{
	MOT_FEC_FLAG_SET (MOT_FEC_INV_RBD_NUM);
	pDrvCtrl->rbdNum = MOT_FEC_RBD_DEF_NUM;
	}

    MOT_FEC_LOG (MOT_FEC_DBG_LOAD,
		 ("motFecEndParse: unit=%d motCpmAddr=0x%x ivec=0x%x\n\
		 bufBase=0x%x bufSize=0x%x \n"),
		 pDrvCtrl->unit,
		 (int) pDrvCtrl->motCpmAddr,
		 (int) pDrvCtrl->ivec,
		 (int) pDrvCtrl->pBufBase,
		 pDrvCtrl->bufSize,
		 0);

    MOT_FEC_LOG (MOT_FEC_DBG_LOAD,
		 ("motFecEndParse: fifoTxBase=0x%x fifoRxBase=0x%x\n\
		 tbdNum=%d rbdNum=%d flags=0x%x\n"),
		 pDrvCtrl->fifoTxBase,
		 pDrvCtrl->fifoRxBase,
		 pDrvCtrl->tbdNum,
		 pDrvCtrl->rbdNum,
		 pDrvCtrl->userFlags,
		 0);

    MOT_FEC_LOG (MOT_FEC_DBG_LOAD,
                 ("motFecEndParse: phyAddr=0x%x isoPhyAddr=0x%x\n\
                  phyDefMode=0x%x \n"),
                 pDrvCtrl->phyInfo->phyAddr,
                 pDrvCtrl->phyInfo->isoPhyAddr,
                 pDrvCtrl->phyInfo->phyDefMode,
                 0, 0, 0);
    return (OK);
    }

/*******************************************************************************
*
* motFecInitMem - initialize memory
*
* This routine initializes all the memory needed by the driver whose control
* structure is passed in <pDrvCtrl>.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS motFecInitMem
    (
    DRV_CTRL *  pDrvCtrl       		/* pointer to DRV_CTRL structure */
    )
    {
    UINT32		bdSize;	   	/* temporary size holder */
    UINT32		clSize;	   	/* temporary size holder */
    UINT16		clNum;		/* a buffer number holder */
    M_CL_CONFIG	 	mclBlkConfig = {0, 0, NULL, 0};
					/* cluster blocks configuration */
    CL_DESC	 	clDescTbl [] = { {MOT_FEC_MAX_CL_LEN, 0, NULL, 0} };
					/* cluster blocks config table */
    int clDescTblNumEnt = (NELEMENTS (clDescTbl));
					/* number of different clusters */

    /* initialize the netPool */

    if ((pDrvCtrl->endObj.pNetPool = malloc (sizeof (NET_POOL))) == NULL)
	return (ERROR);

    /*
     * we include here room for both TBDs and RBDs,
     * and the alignment factor.
     */

    bdSize = (MOT_FEC_BD_MEM (pDrvCtrl) + MOT_FEC_BD_ALIGN);

    /*
     * Establish the memory area that we will share with the device.  If
     * the caller has provided an area, then we assume it is non-cacheable
     * and will not require the use of the special cache routines.
     * If the caller did provide an area, then we must obtain it from
     * the system, using the cache savvy allocation routine.
     */

    switch ((int) pDrvCtrl->pBufBase)
	{
	case NONE :			     /* we must obtain it */

	    /* this driver can't handle write incoherent caches */

	    if (!CACHE_DMA_IS_WRITE_COHERENT ())
		{
		MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("motFecInitMem: shared \n\
					        memory not cache coherent\n"),
					        1, 2, 3, 4, 5, 6);
		return (ERROR);
		}

	    pDrvCtrl->pBufBase = cacheDmaMalloc (bdSize);

	    if (pDrvCtrl->pBufBase == NULL)      /* no memory available */
		{
		MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("motFecInitMem: could not \n\
					        obtain memory\n"),
					        1, 2, 3, 4, 5, 6);
		return (ERROR);
		}

            pDrvCtrl->pBufBase = (char *) (((UINT32) (pDrvCtrl->pBufBase) +
                                  MOT_FEC_BD_ALIGN - 1) &
                                  ~(MOT_FEC_BD_ALIGN -1));

	    pDrvCtrl->bufSize = bdSize;

	    MOT_FEC_FLAG_SET (MOT_FEC_OWN_MEM);

	    pDrvCtrl->bdCacheFuncs = cacheDmaFuncs;

	    break;

	default :			       /* the user provided an area */

	    if (pDrvCtrl->bufSize == 0)
		{
		MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("motFecInitMem: not enough \n\
					        memory\n"),
					        1, 2, 3, 4, 5, 6);
		return (ERROR);
		}

	    /*
	     * check the user provided enough memory with reference
	     * to the given number of receive/transmit frames, if any.
	     */

	    if (MOT_FEC_FLAG_ISSET (MOT_FEC_INV_TBD_NUM) &&
		MOT_FEC_FLAG_ISSET (MOT_FEC_INV_RBD_NUM))
		{
		pDrvCtrl->tbdNum = (MOT_FEC_MEM_SZ (pDrvCtrl) /
				    (MOT_FEC_TBD_SZ + MOT_FEC_RBD_SZ));

		pDrvCtrl->rbdNum = pDrvCtrl->tbdNum;
		}
	    else if (MOT_FEC_FLAG_ISSET (MOT_FEC_INV_TBD_NUM))
		{
		pDrvCtrl->tbdNum = ((MOT_FEC_MEM_SZ (pDrvCtrl)
				    - MOT_FEC_RBD_MEM (pDrvCtrl))
				    / MOT_FEC_TBD_SZ);
		}
	    else if (MOT_FEC_FLAG_ISSET (MOT_FEC_INV_RBD_NUM))
		{
		pDrvCtrl->rbdNum = ((MOT_FEC_MEM_SZ (pDrvCtrl)
				    - MOT_FEC_TBD_MEM (pDrvCtrl))
				    / MOT_FEC_RBD_SZ);
		}
	    else
		{
		if (MOT_FEC_MEM_SZ (pDrvCtrl) < bdSize)
		    {
		    MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("motFecInitMem: not enough \n\
						   memory\n"),
						   1, 2, 3, 4, 5, 6);
		    return (ERROR);
		    }
		}

	    if ((pDrvCtrl->tbdNum <= MOT_FEC_TBD_MIN)
		|| (pDrvCtrl->rbdNum <= MOT_FEC_RBD_MIN))
		{
		MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("motFecInitMem: not enough \n\
					       memory\n"),
					       1, 2, 3, 4, 5, 6);
		return (ERROR);
		}

	    MOT_FEC_FLAG_CLEAR (MOT_FEC_OWN_MEM);

	    pDrvCtrl->bdCacheFuncs = cacheNullFuncs;

	    break;
	}

    /* cache functions descriptor for data buffers */

    motFecBufCacheFuncs.flushRtn = motFecBufFlushRtn;
    motFecBufCacheFuncs.invalidateRtn = motFecBufInvRtn;
    motFecBufCacheFuncs.virtToPhysRtn = NULL;
    motFecBufCacheFuncs.physToVirtRtn = NULL;

    pDrvCtrl->bufCacheFuncs = motFecBufCacheFuncs;

    /* zero the shared memory */

    memset (pDrvCtrl->pBufBase, 0, (int) pDrvCtrl->bufSize);

    /*
     * number of clusters, including loaning buffers, a min number
     * of transmit clusters for copy-mode transmit, and one transmit
     * cluster for polling operation.
     */

    clNum = pDrvCtrl->rbdNum + MOT_FEC_BD_LOAN_NUM
	    + 1 + MOT_FEC_TX_CL_NUM;

    clSize = (MOT_FEC_TX_MEM (pDrvCtrl) + MOT_FEC_RX_MEM (pDrvCtrl)
	    + (CL_OVERHEAD * clNum)
	    + MOT_FEC_BD_ALIGN);

    /* pool of mblks */

    if (mclBlkConfig.mBlkNum == 0)
	mclBlkConfig.mBlkNum = clNum * 2;

    /* pool of clusters, including loaning buffers */

    if (clDescTbl[0].clNum == 0)
	{
	clDescTbl[0].clNum = clNum;
	clDescTbl[0].clSize = MOT_FEC_MAX_CL_LEN;
	}

    /* there's a cluster overhead and an alignment issue */

    clDescTbl[0].memSize = (clDescTbl[0].clNum *
			    (clDescTbl[0].clSize + CL_OVERHEAD));
    clDescTbl[0].memArea = (char *) (memalign (CL_ALIGNMENT, clSize));

    if (clDescTbl[0].memArea == NULL)
	{
	return (ERROR);
	}

    /* store the pointer to the clBlock area */

    pDrvCtrl->pClBlkArea = clDescTbl[0].memArea;
    pDrvCtrl->clBlkSize = clDescTbl[0].memSize;

    /* pool of cluster blocks */

    if (mclBlkConfig.clBlkNum == 0)
	mclBlkConfig.clBlkNum = clDescTbl[0].clNum;

    /* get memory for mblks */

    if (mclBlkConfig.memArea == NULL)
	{
	/* memory size adjusted to hold the netPool pointer at the head */

	mclBlkConfig.memSize = ((mclBlkConfig.mBlkNum
				* (M_BLK_SZ + MBLK_ALIGNMENT))
				+ (mclBlkConfig.clBlkNum
				* (CL_BLK_SZ + CL_ALIGNMENT)));

	mclBlkConfig.memArea = (char *) memalign (MBLK_ALIGNMENT,
						  mclBlkConfig.memSize);

	if (mclBlkConfig.memArea == NULL)
	    {
	    return (ERROR);
	    }

	/* store the pointer to the mBlock area */

	pDrvCtrl->pMBlkArea = mclBlkConfig.memArea;
	pDrvCtrl->mBlkSize = mclBlkConfig.memSize;
	}

    /* init the mem pool */

    if (netPoolInit (pDrvCtrl->endObj.pNetPool, &mclBlkConfig,
		     &clDescTbl[0], clDescTblNumEnt, NULL)
	== ERROR)
	{
	return (ERROR);
	}

    if ((pDrvCtrl->pClPoolId = netClPoolIdGet (pDrvCtrl->endObj.pNetPool,
					       MOT_FEC_MAX_CL_LEN, FALSE))
        == NULL)
	{
	return (ERROR);
	}

    MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("motFecInitMem... Done\n"),
				    0, 0, 0, 0, 0, 0);

    return OK;
    }

/**************************************************************************
*
* motFecStart - start the device
*
* This routine starts the FEC device and brings it up to an operational
* state.  The driver must have already been loaded with the motFecEndLoad()
* routine.
*
* RETURNS: OK, or ERROR if the device could not be initialized.
*
* INTERNAL
* The speed field inthe phyInfo structure is only set after the call
* to the physical layer init routine. On the other hand, the mib2
* interface is initialized in the motFecEndLoad() routine, and the default
* value of 10Mbit assumed there is not always correct. We need to
* correct it here.
*
*/

LOCAL STATUS motFecStart
    (
    DRV_CTRL *pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    int		retVal;		/* convenient holder for return value */
    char        bucket[4];

    MOT_FEC_LOG (MOT_FEC_DBG_START, ("Starting end...\n"), 1, 2, 3, 4, 5, 6);

    /* must have been loaded */

    if (!pDrvCtrl->loaded)
	return (ERROR);

    if (vxMemProbe ((char *) (pDrvCtrl->motCpmAddr + MOT_FEC_CSR_OFF),
		    VX_READ, 4, &bucket[0]) != OK)
        {
        MOT_FEC_LOG (MOT_FEC_DBG_START,
                   (": need MMU mapping for address 0x%x\n"),
                   (UINT32) pDrvCtrl->motCpmAddr, 2, 3, 4, 5, 6);
	return (ERROR);
        }

    /* reset the chip */

    MOT_FEC_ETH_SET;
    if (motFecReset (pDrvCtrl) != OK)
	return (ERROR);

    if (motFecTbdInit (pDrvCtrl) == ERROR)
	return (ERROR);

    if (motFecRbdInit (pDrvCtrl) == ERROR)
	return (ERROR);

    /* set some flags to default values */

    pDrvCtrl->txStall = FALSE;
    pDrvCtrl->tbdIndex = 0;
    pDrvCtrl->usedTbdIndex = 0;
    pDrvCtrl->cleanTbdNum = pDrvCtrl->tbdNum;
    pDrvCtrl->rbdIndex = 0;

    /* connect the interrupt handler */

    SYS_FEC_INT_CONNECT (pDrvCtrl, motFecInt, (int) pDrvCtrl, retVal);
    if (retVal == ERROR)
	return (ERROR);

    /* enable system interrupt: set relevant bit in SIMASK */

    SYS_FEC_INT_ENABLE (pDrvCtrl, retVal);
    if (retVal == ERROR)
	return (ERROR);
	
    /* call the BSP to do any other initialization (port D) */

    SYS_FEC_ENET_ENABLE;

    /* configure some chip's registers */

    if (motFecPrePhyConfig (pDrvCtrl) == ERROR)
	return (ERROR);

    /* initialize some fields in the PHY info structure */

    if (motFecPhyPreInit (pDrvCtrl) != OK)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("Failed to pre-initialize the PHY\n"),
					0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    /* initialize the Physical medium layer */

    if (_func_motFecPhyInit == NULL)
	return (ERROR);

    if (((* _func_motFecPhyInit) (pDrvCtrl)) != OK)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("Failed to initialize the PHY\n"),
					0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    /* configure other chip's registers */

    if (motFecPostPhyConfig (pDrvCtrl) == ERROR)
	return (ERROR);

    /* correct the speed for the mib2 stuff */

    pDrvCtrl->endObj.mib2Tbl.ifSpeed = pDrvCtrl->phyInfo->phySpeed;

    if (END_FLAGS_ISSET (IFF_MULTICAST))
        MOT_FEC_FLAG_SET (MOT_FEC_MCAST);

    /* enable the Ethernet Controller */

    MOT_FEC_ETH_ENABLE;

    /* mark the interface as up */

    END_FLAGS_SET (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));

    /* startup the receiver */

    MOT_FEC_RX_ACTIVATE;

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    MOT_FEC_LOG (MOT_FEC_DBG_START, ("Starting end... Done\n"),
				     1, 2, 3, 4, 5, 6);

    return (OK);
    }

/**************************************************************************
*
* motFecStop - stop the 'motfec' interface
*
* This routine marks the interface as inactive, disables interrupts and
* the Ethernet Controller. As a result, reception is stopped immediately,
* and transmission is stopped after a bad CRC is appended to any frame
* currently being transmitted. The reception/transmission control logic
* (FIFO pointers, buffer descriptors, etc.) is reset. To bring the
* interface back up, motFecStart() must be called.
*
* RETURNS: OK, always.
*/

LOCAL STATUS motFecStop
    (
    DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    int		retVal;		/* convenient holder for return value */

    MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("motFecStop...\n"), 1, 2, 3, 4, 5, 6);

    /* make sure driver is operational before stopping it */

    if (END_FLAGS_GET(&pDrvCtrl->endObj) & (IFF_UP | IFF_RUNNING))
        {

        /* mark the interface as down */

        END_FLAGS_CLR (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));

        /* issue a graceful transmit command */

        MOT_FEC_CSR_WR (MOT_FEC_TX_CTRL_OFF, MOT_FEC_TX_CTRL_GRA);

        /* wait for the related interrupt */

        MOT_FEC_GRA_SEM_TAKE;

        /* mask chip interrupts */

        MOT_FEC_INT_DISABLE;	

        /* disable the Ethernet Controller */

        MOT_FEC_ETH_DISABLE;

        /* disable system interrupt: reset relevant bit in SIMASK */

        SYS_FEC_INT_DISABLE (pDrvCtrl, retVal);
        if (retVal == ERROR)
	    return (ERROR);

        /* disconnect the interrupt handler */

        SYS_FEC_INT_DISCONNECT (pDrvCtrl, motFecInt, (int)pDrvCtrl, retVal);
        if (retVal == ERROR)
	    return (ERROR);

        /* call the BSP to disable the Port D */

        SYS_FEC_ENET_DISABLE;

        if (motFecBdFree (pDrvCtrl) != OK)
	    return (ERROR);

        }

    MOT_FEC_LOG (MOT_FEC_DBG_LOAD, ("motFecStop... Done \n"),
				    1, 2, 3, 4, 5, 6);

    return OK;
    }

/**************************************************************************
*
* motFecReset - reset the `motFec' interface
*
* This routine resets the chip by setting the Reset bit in the Ethernet
* Control Register. The ETHER_EN bit is cleared, and all the FEC registers
* take their default values. In addition, any transmission/reception
* currently in progress is abruptly aborted.
*
* RETURNS: OK, always.
*/

LOCAL STATUS motFecReset
    (
    DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    /* issue a reset command to the FEC chip */

    MOT_FEC_CSR_WR (MOT_FEC_CTRL_OFF, (MOT_FEC_ETH_RES | pinMux));

    /* wait at least 16 clock cycles */

    taskDelay (1);

    /* enable the FEC PIN MUX bit if necessary */

    MOT_FEC_CSR_WR (MOT_FEC_CTRL_OFF, pinMux);

    /* wait at least 16 clock cycles */

    taskDelay (1);

    return (OK);
    }

/*******************************************************************************
*
* motFecInt - entry point for handling interrupts from the FEC
*
* The interrupting events are acknowledged to the device, so that the device
* will de-assert its interrupt signal.  The amount of work done here is kept
* to a minimum; the bulk of the work is deferred to the netTask.
*
* RETURNS: N/A
*/

LOCAL void motFecInt
    (
    DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    UINT32	event = 0;	/* event intr register */
    UINT32	mask = 0;	/* holder for the mask intr register */
    UINT32	status;		/* status word */

    CACHE_PIPE_FLUSH ();

    /* read and save the interrupt event register */

    MOT_FEC_CSR_RD (MOT_FEC_EVENT_OFF, event);

    /* clear all interrupts */

    MOT_FEC_CSR_WR (MOT_FEC_EVENT_OFF, (event & MOT_FEC_EVENT_MSK));

    CACHE_PIPE_FLUSH ();

    /* read the interrupt mask register */

    MOT_FEC_CSR_RD (MOT_FEC_MASK_OFF, mask);

    /* read CSR status word again */

    MOT_FEC_CSR_RD (MOT_FEC_EVENT_OFF, status);

    MOT_FEC_LOG (MOT_FEC_DBG_INT, ("motFecInt: event 0x%x, status 0x%x\n"),
				   (event & MOT_FEC_EVENT_MSK), status,
				   0, 0, 0, 0);

    /* handle bus error interrupts */

    if ((event & MOT_FEC_EVENT_BERR) == MOT_FEC_EVENT_BERR)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_INT, ("motFecInt: bus error, restart chip\n"),
				       0, 0, 0, 0, 0, 0);

	/* stop and restart the device */

	(void) netJobAdd ((FUNCPTR) motFecStop, (int) pDrvCtrl,
			  event, 0, 0, 0);
	(void) netJobAdd ((FUNCPTR) motFecStart, (int) pDrvCtrl,
			  event, 0, 0, 0);
	}

    /* handle babbling transmit error interrupts */

    if ((event & MOT_FEC_EVENT_BABT) == MOT_FEC_EVENT_BABT)
	{
	MOT_FEC_BAB_TX_ADD;

	MOT_FEC_LOG (MOT_FEC_DBG_INT, ("motFecInt: babbling tx error\n"),
				       0, 0, 0, 0, 0, 0);
	}

    /* handle babbling receive error interrupts */

    if ((event & MOT_FEC_EVENT_BABR) == MOT_FEC_EVENT_BABR)
	{
	MOT_FEC_BAB_RX_ADD;

	MOT_FEC_LOG (MOT_FEC_DBG_INT, ("motFecInt: babbling rx error\n"),
				       0, 0, 0, 0, 0, 0);
	}

    /* handle transmit and receive interrupts */

    if ((((event & MOT_FEC_EVENT_TXF) == MOT_FEC_EVENT_TXF)
	&& ((mask & MOT_FEC_EVENT_TXF) == MOT_FEC_EVENT_TXF)) ||
	(((event & MOT_FEC_EVENT_RXF) == MOT_FEC_EVENT_RXF)
	&& ((mask & MOT_FEC_EVENT_RXF) == MOT_FEC_EVENT_RXF)))
	{
        /* disable tx/rx interrupts */

        MOT_FEC_CSR_WR (MOT_FEC_MASK_OFF, (mask & (~(MOT_FEC_EVENT_TXF
						     | MOT_FEC_EVENT_RXF))));

	MOT_FEC_LOG (MOT_FEC_DBG_INT, ("motFecInt: tx/rx intr\n"),
				       0, 0, 0, 0, 0, 0);

	(void) netJobAdd ((FUNCPTR) motFecRxTxHandle, (int) pDrvCtrl,
			  0, 0, 0, 0);
	}

    /* handle heartbeat check fail interrupts */

    if ((event & MOT_FEC_EVENT_HB) == MOT_FEC_EVENT_HB)
	{
	MOT_FEC_HB_FAIL_ADD;

	if (_func_motFecHbFail != NULL)
	    (* _func_motFecHbFail) (pDrvCtrl);

	MOT_FEC_LOG (MOT_FEC_DBG_INT, ("motFecInt: heartbeat check fail\n"),
				       0, 0, 0, 0, 0, 0);
	}

    /* handle mii frame completion interrupts */

    if ((event & MOT_FEC_EVENT_MII) == MOT_FEC_EVENT_MII)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_INT, ("motFecInt: mii transfer\n"),
				       0, 0, 0, 0, 0, 0);

	/* let the PHY configuration task finish off its job */

	MOT_FEC_MII_SEM_GIVE;
	}

    /* handle graceful transmit interrupts */

    if ((event & MOT_FEC_EVENT_GRA) == MOT_FEC_EVENT_GRA)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_INT, ("motFecInt: graceful transmit\n"),
				       0, 0, 0, 0, 0, 0);

	/* let the Stop task finish off its job */

	MOT_FEC_GRA_SEM_GIVE;
	}
    }

/*******************************************************************************
*
* motFecSend - send an Ethernet packet
*
* This routine() takes a M_BLK_ID and sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it. This is done by a higher layer.
*
* muxSend() calls this routine each time it wants to send a packet.
*
* RETURNS: OK, or END_ERR_BLOCK, if no resources are available, or ERROR,
* if the device is currently working in polling mode.
*/

LOCAL STATUS motFecSend
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    M_BLK * 	pMblk           /* pointer to the mBlk/cluster pair */
    )
    {
    UINT8		fragNum = 0;	/* number of fragments in this mBlk */
    UINT16		pktType = 0;	/* packet type (unicast or multicast) */

    MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecSend...\n"), 1, 2, 3, 4, 5, 6);

    END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);

    /* check device mode */

    if (MOT_FEC_FLAG_ISSET (MOT_FEC_POLLING))
        {
        netMblkClChainFree (pMblk); /* free the given mBlk chain */

        errno = EINVAL;
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);

        END_TX_SEM_GIVE (&pDrvCtrl->endObj);

        return (ERROR);
        }

    /* walk the mBlk */

    if (motFecMblkWalk (pMblk, &fragNum, &pktType) == ERROR)
	{
        goto motFecSendError;
	}

    /* check we have enough resources */

    if ((pDrvCtrl->cleanTbdNum) == 0)
	{

        /*
         * SPR 75008 - first run the clean routine and then if there still
         * are not any free Tx buffer descriptors, set the stall flag and
         * the stall condition will be handled in motFecTbdClean()
         */

        motFecTbdClean (pDrvCtrl);

        if (pDrvCtrl->cleanTbdNum == 0)
            {
            /* SPR 30135 - make sure muxTxRestart() will be called later */

            pDrvCtrl->txStall = TRUE;
            motFecTbdClean (pDrvCtrl);

            goto motFecSendError;
            }
	}

    if ((pDrvCtrl->cleanTbdNum) >= fragNum)
	{
	/* transmit the packet in zero-copy mode */

	if (motFecPktTransmit (pDrvCtrl, pMblk, fragNum, pktType) == ERROR)
	    {
	    goto motFecSendError;
	    }
	}
    else
	{
	/* transmit the packet in copy mode */

	fragNum = 1;

	if (motFecPktCopyTransmit (pDrvCtrl, pMblk, fragNum, pktType) == ERROR)
	    {
	    goto motFecSendError;
	    }
	}

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecSend...Done\n"), 1, 2, 3, 4, 5, 6);

    return (OK);

motFecSendError:

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    return (END_ERR_BLOCK);
    }

/******************************************************************************
*
* motFecPktTransmit - transmit a packet
*
* This routine transmits the packet described by the given parameters
* over the network, without copying the mBlk to the driver's buffer.
* It also updates statistics.
*
* RETURNS: OK, or ERROR if no resources were available.
*/

LOCAL STATUS motFecPktTransmit
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    M_BLK * 	pMblk,          /* pointer to the mBlk */
    UINT8 	fragNum,	/* number of fragments */
    UINT16 	pktType		/* packet type */
    )
    {
    int			bufLen = 0;	/* length of this fragment */
    int			pktLen = 0;	/* length of this packet */
    int			totLen = 0;	/* length of data to be sent */
    char *		pBuf = NULL;	/* pointer to data to be sent */
    MOT_FEC_TBD_ID	pTbd = NULL;	/* pointer to the current ready TBD */
    MOT_FEC_TBD_LIST_ID	pTbdList = NULL;/* pointer to the TBD list*/
    UINT16		tbdStatus;	/* holder for the TBD status */
    M_BLK *		pCurr = pMblk;	/* holder for the current mBlk */

    MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecSend...\n"), 1, 2, 3, 4, 5, 6);

    /* get the current free TBD */

    pTbdList = motFecTbdListSet (pDrvCtrl, fragNum);

    if (pTbdList == (MOT_FEC_TBD_LIST_ID) NULL)
	{
	/* set to stall condition */

	pDrvCtrl->txStall = TRUE;

	return (ERROR);
	}

    /* store the Mblk pointer */

    pTbdList->pBuf = (UCHAR *) pMblk;
    pTbdList->bufType = BUF_TYPE_MBLK;

    /* store the packet type */

    pTbdList->pktType = pktType;

    pCurr = pMblk;

    /* get the total packet length */

    pktLen = pMblk->mBlkPktHdr.len;

    /* for each mBlk fragment, set up the related TBD */

    while (fragNum > 0)
	{
	pBuf = pCurr->mBlkHdr.mData;

	bufLen = pCurr->mBlkHdr.mLen;

	pCurr = ((M_BLK *) pCurr->mBlkHdr.mNext);
	
	/*
	 * skip fragments that contain no data (SPR 29513).
	 * fragments without data were not counted in motFecMblkWalk()
	 */
	if (bufLen == 0)
	    continue;

	/* just in case we have to pad it */

	totLen += bufLen;

	/* flush the cache, if necessary */

	MOT_FEC_CACHE_FLUSH (pBuf, bufLen);

	/* get and set up the current TBD */

	pTbd = pTbdList->pTbd;

	MOT_FEC_BD_LONG_WR (pTbd, MOT_FEC_BD_ADDR_OFF, (UINT32) pBuf);

	MOT_FEC_BD_WORD_RD (pTbd, MOT_FEC_BD_STAT_OFF, tbdStatus);

	/*
	 * complete TBD construction based on whether or not this is the last
	 * fragment
	 */
	if (--fragNum > 0)
	    {
	    MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecSend more mblks \n"),
					  1, 2, 3, 4, 5, 6);

	    MOT_FEC_BD_WORD_WR (pTbd, MOT_FEC_BD_LEN_OFF, (UINT32) bufLen);

	    MOT_FEC_BD_WORD_WR (pTbd, MOT_FEC_BD_STAT_OFF,
				(tbdStatus | MOT_FEC_TBD_RDY));
	    }
	else
	    {
	    if (totLen < ETHERSMALL)
		bufLen = max (bufLen, (ETHERSMALL - totLen + bufLen));

	    MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecSend last len=0x%x\n"),
					  bufLen, 2, 3, 4, 5, 6);

	    MOT_FEC_BD_WORD_WR (pTbd, MOT_FEC_BD_LEN_OFF, (UINT32) bufLen);

	    MOT_FEC_BD_WORD_WR (pTbd, MOT_FEC_BD_STAT_OFF,
				(MOT_FEC_TBD_LAST | MOT_FEC_TBD_CRC
				| MOT_FEC_TBD_RDY | tbdStatus));
	    }

	/* move on to the next element */

	pTbdList = pTbdList->pNext;
	}

    /* kick the transmitter */

    MOT_FEC_TX_ACTIVATE;

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* up-date statistics */

    if (pktType == PKT_TYPE_MULTI)
	{
	pDrvCtrl->endObj.mib2Tbl.ifOutNUcastPkts += 1;
	}
    else
	{
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);
	}

    return (OK);
    }

/******************************************************************************
*
* motFecPktCopyTransmit - copy and transmit a packet
*
* This routine transmits the packet described by the given parameters
* over the network, after copying the mBlk to the driver's buffer.
* It also updates statistics.
*
* RETURNS: OK, or ERROR if no resources were available.
*/

LOCAL STATUS motFecPktCopyTransmit
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    M_BLK * 	pMblk,          /* pointer to the mBlk */
    UINT8 	fragNum,	/* number of fragments */
    UINT16 	pktType		/* packet type */
    )
    {
    int			len = 0;	/* length of data to be sent */
    char *		pBuf = NULL;	/* pointer to data to be sent */
    MOT_FEC_TBD_ID	pTbd = NULL;	/* pointer to the current ready TBD */
    MOT_FEC_TBD_LIST_ID	pTbdList = NULL;/* pointer to the TBD list*/
    UINT16		tbdStatus;	/* holder for the TBD status */

    MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecSend...\n"), 1, 2, 3, 4, 5, 6);

    /* get a cluster buffer from the pool */

    pBuf = NET_BUF_ALLOC();

    if (pBuf == NULL)
	{
	/* set to stall condition */

	pDrvCtrl->txStall = TRUE;

	return (ERROR);
	}

    /* get the current free TBD list */

    pTbdList = motFecTbdListSet (pDrvCtrl, fragNum);

    if (pTbdList == (MOT_FEC_TBD_LIST_ID) NULL)
	{
	/* set to stall condition */

        NET_BUF_FREE ((UCHAR *) pBuf);

	pDrvCtrl->txStall = TRUE;

	return (ERROR);
	}

    /* store the packet type */

    pTbdList->pktType = pktType;

    /* store the buffer pointer and its type */

    pTbdList->bufType = BUF_TYPE_CL;
    pTbdList->pBuf = (UINT8 *)pBuf;

    /* copy data and free the Mblk */

    len = netMblkToBufCopy (pMblk, (char *) pBuf, NULL);
    netMblkClChainFree (pMblk);
    len = max (ETHERSMALL, len);

    /* flush the cache, if necessary */

    MOT_FEC_CACHE_FLUSH (pBuf, len);

    /* get and set up the current TBD */

    pTbd = pTbdList->pTbd;

    MOT_FEC_BD_LONG_WR (pTbd, MOT_FEC_BD_ADDR_OFF, (UINT32) pBuf);

    MOT_FEC_BD_WORD_WR (pTbd, MOT_FEC_BD_LEN_OFF, (UINT32) len);

    MOT_FEC_BD_WORD_RD (pTbd, MOT_FEC_BD_STAT_OFF, tbdStatus);

    MOT_FEC_BD_WORD_WR (pTbd, MOT_FEC_BD_STAT_OFF,
			(MOT_FEC_TBD_LAST | MOT_FEC_TBD_CRC
			| MOT_FEC_TBD_RDY | tbdStatus));

    /* kick the transmitter */

    MOT_FEC_TX_ACTIVATE;

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* up-date statistics */

    if (pktType == PKT_TYPE_MULTI)
	{
	pDrvCtrl->endObj.mib2Tbl.ifOutNUcastPkts += 1;
	}
    else
	{
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);
	}

    return (OK);
    }

/******************************************************************************
*
* motFecMblkWalk - walk the mBlk
*
* This routine walks the mBlk whose address is in <pMblk>, computes
* the number of fragments it is made of, and returns it in the parameter
* <pFragNum>. In addition, it finds out whether the specified packet
* is unicast or multicast, and sets <pPktType> accordingly.
*
* RETURNS: OK, or ERROR in case of invalid mBlk.
*/

LOCAL STATUS motFecMblkWalk
    (
    M_BLK * 	pMblk,          /* pointer to the mBlk */
    UINT8 *	pFragNum,	/* number of fragments */
    UINT16 *	pPktType	/* packet type */
    )
    {
    M_BLK *		pCurr = pMblk;		/* the current mBlk */

    if (pMblk == NULL)
        {
        MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecSend: invalid pMblk\n"),
                                      0, 0, 0, 0, 0, 0);
	return (ERROR);
        }

    /* walk this mBlk */

    while (pCurr != NULL)
	{
	/*
	 * keep track of the number of fragments in this mBlk.
	 * Do not count fragments with no data (SPR 29513)
	 */

	if (pCurr->mBlkHdr.mLen != 0)
	    (*pFragNum)++;

	pCurr = ((M_BLK *) pCurr->mBlkHdr.mNext);
	}

    /* set the packet type to multicast or unicast */

    if (pMblk->mBlkHdr.mData[0] & (UINT8) 0x01)
	{
	(*pPktType) = PKT_TYPE_MULTI;
	}
    else
	{
	(*pPktType) = PKT_TYPE_UNI;
	}


    return (OK);
    }

/******************************************************************************
*
* motFecTbdListSet - set up a list of available transmit descriptors
*
* This routine sets up a list of available buffer descriptors for use by
* the send routine.
*
* RETURNS: a pointer to a transmit descriptors list, or NULL.
*/

LOCAL MOT_FEC_TBD_LIST_ID motFecTbdListSet
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UINT8	tbdNum 		/* number of tbd */
    )
    {
    MOT_FEC_TBD_ID	pTbd = NULL;		/* pointer to the current TBD */
    UINT16		tbdIndex = pDrvCtrl->tbdIndex;	/* TBD index */
    UINT16		nextIndex = 0;		/* index to the next TBD */
    UINT16		currIndex = 0;		/* index to the current TBD */
    UINT16		i = 0;			/* an index */
    MOT_FEC_TBD_LIST_ID	pTbdList = NULL;

    /* set the number of TBDs in this list */

    pDrvCtrl->pTbdList [tbdIndex]->fragNum = tbdNum;

    /* initialize the list */

    for (i = 0; i < tbdNum; i++)
	{

	/* get the current free TBD */

	if ((pTbd = motFecTbdGet (pDrvCtrl)) == NULL)
	    {
	    return (NULL);
	    }

	currIndex = pDrvCtrl->tbdIndex;
	nextIndex = (currIndex + 1) % (pDrvCtrl->tbdNum);

	pTbdList = pDrvCtrl->pTbdList [currIndex];

	pTbdList->pTbd = pTbd;
	pTbdList->pBuf = NULL;

	pTbdList->pNext = pDrvCtrl->pTbdList [nextIndex];


	/* update some indeces for a correct handling of the TBD ring */

	pDrvCtrl->tbdIndex = (pDrvCtrl->tbdIndex + 1) % pDrvCtrl->tbdNum;
	}

	/* terminate the list properly */

	pTbdList->pNext = NULL;

	/* keep track of the clean TBDs */

	pDrvCtrl->cleanTbdNum -= tbdNum;

    return (pDrvCtrl->pTbdList [tbdIndex]);
    }

/******************************************************************************
*
* motFecTbdGet - get the first available transmit descriptor
*
* This routine gets the first available buffer descriptor in the transmit
* ring. It checks the descriptor is owned by the host and it has been
* cleaned by the motFecTbdClean() routine.
*
* RETURNS: an available transmit descriptor, otherwise NULL.
*/

LOCAL MOT_FEC_TBD_ID motFecTbdGet
    (
    DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    MOT_FEC_TBD_ID	nextTbd = NULL;	/* holder for the next TBD */
    UINT16	tbdStatus;	/* holder for the TBD status */

    MOT_FEC_NEXT_TBD (pDrvCtrl, nextTbd);

    /* Make cache consistent with memory */

    MOT_FEC_BD_CACHE_INVAL (nextTbd, MOT_FEC_TBD_SZ);

    MOT_FEC_BD_WORD_RD ((char *) nextTbd, MOT_FEC_BD_STAT_OFF, tbdStatus);

    if ((tbdStatus & MOT_FEC_TBD_RDY) == MOT_FEC_TBD_RDY)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecSend... not our Tbd \n"),
				      1, 2, 3, 4, 5, 6);
	return (NULL);
	}

    return ((MOT_FEC_TBD_ID) nextTbd);
    }

/******************************************************************************
*
* motFecTbdClean - clean the transmit buffer descriptors ring
*
* This routine is run in netTask's context. It cleans the transmit
* buffer descriptors ring. It also calls motFecTbdCheck() to check
* for any transmit errors, and possibly up-dates statistics.
*
* SEE ALSO: motFecTbdCheck()
*
* RETURNS: N/A.
*/

LOCAL void motFecTbdClean
    (
    DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    MOT_FEC_TBD_ID	pUsedTbd;		/* first used TBD */
    BOOL		tbdError = FALSE;	/* no error reported */
    UINT32 		retVal = 0;		/* return value */

    MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecTbdClean\n"), 1, 2, 3, 4, 5, 6);

    END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);

    /* process all the consumed TBDs */

    while (pDrvCtrl->cleanTbdNum < pDrvCtrl->tbdNum)
	{
	MOT_FEC_TBD_LIST_ID	pTbdList;
	MOT_FEC_TBD_LIST_ID	pTempList;

	pTbdList = pDrvCtrl->pTbdList [pDrvCtrl->usedTbdIndex];
	pTempList = pTbdList;

	/* first, we detect abnormal events */

	while (pTempList != NULL)
	    {
	    pUsedTbd = pTempList->pTbd;

	    /* check on this TBD */

	    retVal = motFecTbdCheck (pDrvCtrl, pUsedTbd);

	    if (retVal == MOT_FEC_TBD_BUSY)
		{
		END_TX_SEM_GIVE (&pDrvCtrl->endObj);
		return;
		}

	    if (retVal == MOT_FEC_TBD_ERROR)
		{
		tbdError = TRUE;
		break;
		}

	    /* move on to the next element in the list */

	    pTempList = pTempList->pNext;
	    }

        /* free the Mblk or the cluster, whichever is appropriate */

        if ((pTbdList->bufType == ((UINT16) BUF_TYPE_MBLK))
	    && (pTbdList->pBuf != NULL))
	    {
	    netMblkClChainFree ((M_BLK *) pTbdList->pBuf);

	    pTbdList->pBuf = NULL;
	    }
	else
	    {
	    if ((pTbdList->bufType == ((UINT16) BUF_TYPE_CL))
		&& (pTbdList->pBuf != NULL))
		{
		/* it's a cluster */

		NET_BUF_FREE (pTbdList->pBuf);

		pTbdList->pBuf = NULL;
		}
	    }

	/* correct statistics, if necessary */

	if (tbdError == TRUE)
	    {
	    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);

	    if (pTbdList->pktType == PKT_TYPE_MULTI)
		{
		pDrvCtrl->endObj.mib2Tbl.ifOutNUcastPkts -= 1;
		}
	    else
		{
		END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, -1);
		}
	    tbdError = FALSE;
	    }

	pTempList = pTbdList;

	/* finally, we clean this TBD list */

	while (pTempList != NULL)
	    {
	    pUsedTbd = pTempList->pTbd;

	    /* clean this buffer descriptor, mirror motFecTbdInit() */

	    if (pDrvCtrl->usedTbdIndex == (pDrvCtrl->tbdNum - 1))
		{
		MOT_FEC_BD_WORD_WR (pUsedTbd, MOT_FEC_BD_STAT_OFF,
				    MOT_FEC_TBD_WRAP);
		}
	    else
		{
		MOT_FEC_BD_WORD_WR (pUsedTbd, MOT_FEC_BD_STAT_OFF, 0);
		}

	    /* update some indeces for a correct handling of the TBD ring */

	    pDrvCtrl->cleanTbdNum++;
	    pDrvCtrl->usedTbdIndex = (pDrvCtrl->usedTbdIndex + 1)
				      % (pDrvCtrl->tbdNum);

	    /* move on to the next element in the list */

	    pTempList = pTempList->pNext;
	    }

	/* soon notify upper protocols TBDs are available */

	if (pDrvCtrl->txStall)
	    {
	    MOT_FEC_LOG (MOT_FEC_DBG_TX, ("Restart mux \n"),
					  0, 0, 0, 0, 0, 0);

	    (void) netJobAdd ((FUNCPTR) muxTxRestart,
			      (int) &pDrvCtrl->endObj,
			      0, 0, 0, 0);

	    pDrvCtrl->txStall = FALSE;
	    }
	}

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecTbdClean...Done\n"),
				  1, 2, 3, 4, 5, 6);
    }

/******************************************************************************
*
* motFecTbdCheck - check the transmit buffer descriptor
*
* This routine checks for any transmit errors by looking at the
* descriptor status.
*
* SEE ALSO: motFecTbdClean()
*
* RETURNS: MOT_FEC_TBD_OK, or MOT_FEC_TBD_BUSY if the specified TBD is
* not ready, or MOT_FEC_TBD_ERROR in case of errored TBD.
*/

LOCAL UINT32 motFecTbdCheck
    (
    DRV_CTRL *  	pDrvCtrl,       /* pointer to DRV_CTRL structure */
    MOT_FEC_TBD_ID	pUsedTbd	/* holder for the first used TBD */
    )
    {
    UINT16	tbdStatus;	/* holder for the TBD status */

    /* Make cache consistent with memory */

    MOT_FEC_BD_CACHE_INVAL (pUsedTbd, MOT_FEC_TBD_SZ);

    MOT_FEC_BD_WORD_RD ((char *) pUsedTbd, MOT_FEC_BD_STAT_OFF, tbdStatus);

    /* if it's not ready, don't touch it! */
	
    if ((tbdStatus & MOT_FEC_TBD_RDY) == MOT_FEC_TBD_RDY)
	{
	return (MOT_FEC_TBD_BUSY);
	}

#ifdef MOT_FEC_DBG

    if ((tbdStatus & MOT_FEC_TBD_LC) == MOT_FEC_TBD_LC)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecTbdCheck: late collision\n"),
				      1, 2, 3, 4, 5, 6);
	MOT_FEC_TX_LC_ADD;
	}

    if ((tbdStatus & MOT_FEC_TBD_RL) == MOT_FEC_TBD_RL)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecTbdCheck: retransmit limit"),
				      1, 2, 3, 4, 5, 6);
	MOT_FEC_TX_RL_ADD;
	}

    if ((tbdStatus & MOT_FEC_TBD_UN) == MOT_FEC_TBD_UN)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecTbdCheck: tx underrun\n"),
				      1, 2, 3, 4, 5, 6);
	MOT_FEC_TX_UR_ADD;
	}

    if ((tbdStatus & MOT_FEC_TBD_CSL) == MOT_FEC_TBD_CSL)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_TX, ("motFecTbdCheck: carrier sense \n"),
				      1, 2, 3, 4, 5, 6);
	MOT_FEC_TX_CSL_ADD;
	}
#endif /* MOT_FEC_DBG */

    /* in case of errors, report it to the calling routine */

    if (tbdStatus & (MOT_FEC_TBD_LC |
		     MOT_FEC_TBD_RL |
		     MOT_FEC_TBD_UN |
		     MOT_FEC_TBD_CSL))
	{
	return (MOT_FEC_TBD_ERROR);
	}

    return (MOT_FEC_TBD_OK);
    }

/**************************************************************************
*
* motFecTbdInit - initialize the transmit buffer ring
*
* This routine initializes the transmit buffer descriptors ring.
*
* SEE ALSO: motFecRbdInit()
*
* RETURNS: OK, always.
*/

LOCAL STATUS motFecTbdInit
    (
    DRV_CTRL *  pDrvCtrl        /* pointer to DRV_CTRL structure */
    )
    {
    UINT16		ix;		/* a counter */
    MOT_FEC_TBD_ID	pTbd;		/* generic TBD pointer */

    /* carve up the shared-memory region */

    pDrvCtrl->tbdBase = pTbd = (MOT_FEC_TBD_ID) pDrvCtrl->pBufBase;

    for (ix = 0; ix < pDrvCtrl->tbdNum; ix++)
	{
	/*
	 * put a fragment of each frame in a single buffer and
	 * generate the 4-byte CRC at the end of each frame.
	 * Do not make it ready for transmission yet.
	 */

	MOT_FEC_BD_WORD_WR (pTbd, MOT_FEC_BD_STAT_OFF, 0);

	pTbd += MOT_FEC_TBD_SZ;

	/* get memory for the TBD list */

	pDrvCtrl->pTbdList [ix] = (MOT_FEC_TBD_LIST_ID)
				   calloc (sizeof (MOT_FEC_TBD_LIST), 1);
	}

    /* have the last TBD to close the ring */

    pTbd -= MOT_FEC_TBD_SZ;
    MOT_FEC_BD_WORD_WR (pTbd, MOT_FEC_BD_STAT_OFF,
			(MOT_FEC_TBD_WRAP | 0));

    /* get a cluster buffer from the pool */

    pDrvCtrl->pTxPollBuf = (UINT8 *)NET_BUF_ALLOC();

    return (OK);
    }

/******************************************************************************
*
* motFecRxTxHandle - service task-level interrupts for rx and tx frames
*
* This routine is run in netTask's context.  The ISR scheduled
* this routine so that it could handle receive and transmit packets
* at task level.
*
* RETURNS: N/A
*/

LOCAL void motFecRxTxHandle
    (
    DRV_CTRL *  pDrvCtrl	/* pointer to DRV_CTRL structure */
    )
    {
    int        	intLevel;	/* current intr level */
    UINT32	event = 0;	/* event intr register */
    UINT32	mask = 0;	/* holder for the mask intr register */

    MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecRxTxHandle: \n"),
				  0, 0, 0, 0, 0, 0);

    /* while tx/rx interrupts to be processed */

motFecRxTxProcess:

    motFecHandleRecvInt (pDrvCtrl);

    motFecTbdClean (pDrvCtrl);

    intLevel = intLock();

    /* 
     * This piece of code is to deal with any events that have occurred 
     * since the events were masked off in the ISR. These events will 
     * have occurred but no interrupt would have been generated. 
     */


    MOT_FEC_CSR_RD (MOT_FEC_EVENT_OFF, event);

    /* clear all interrupts */

    MOT_FEC_CSR_WR (MOT_FEC_EVENT_OFF, (event & MOT_FEC_EVENT_MSK));

    if (((event & MOT_FEC_EVENT_TXF) == MOT_FEC_EVENT_TXF) ||
	((event & MOT_FEC_EVENT_RXF) == MOT_FEC_EVENT_RXF))
	{
	intUnlock (intLevel);
	goto motFecRxTxProcess;
	}
    else
	{
	/* re-enable receive and transmit interrupts */

	MOT_FEC_CSR_RD (MOT_FEC_MASK_OFF, mask);
	MOT_FEC_CSR_WR (MOT_FEC_MASK_OFF, (mask | MOT_FEC_EVENT_TXF
					   | MOT_FEC_EVENT_RXF));
	/* Flush the write pipe */

	CACHE_PIPE_FLUSH ();

	intUnlock (intLevel);
	}
    }

/******************************************************************************
*
* motFecHandleRecvInt - service task-level interrupts for receive frames
*
* This routine is run in netTask's context. It is called by motFecRxTxHandle()
* so that it could handle receive packets at task level.
*
* RETURNS: N/A
*/

LOCAL void motFecHandleRecvInt
    (
    DRV_CTRL *  pDrvCtrl	/* pointer to DRV_CTRL structure */
    )
    {
    MOT_FEC_RBD_ID	pUsedRbd;	/* a used rbd pointer */
    UINT16		rbdStatus = 0;	/* holder for the status of this rbd */
    UINT32		rxStatus = 0;	/* holder for the receiver status */
    UINT32		event = 0;	/* event intr register */
    int         	intLevel;	/* current intr level */

    MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecHandleRecvInt: \n"),
				  0, 0, 0, 0, 0, 0);

    /* while RBDs to be processed */

    do
        {
	/* get the first used RBD */

	MOT_FEC_NEXT_RBD (pDrvCtrl, pUsedRbd);

	/* Make cache consistent with memory */

	MOT_FEC_BD_CACHE_INVAL (pUsedRbd, MOT_FEC_RBD_SZ);
					
	/* read the RBD status word */		

	MOT_FEC_BD_WORD_RD (pUsedRbd, MOT_FEC_BD_STAT_OFF,
			    rbdStatus);

        /* if it's not ready, don't touch it! */
	
	if ((rbdStatus & MOT_FEC_RBD_EMPTY) == MOT_FEC_RBD_EMPTY)
	    {
	    break;
	    }

	/* process this RBD */

	motFecReceive (pDrvCtrl, pUsedRbd);

	} while (TRUE);

    /* kick receiver (if needed) */

    MOT_FEC_CSR_RD (MOT_FEC_RX_ACT_OFF, rxStatus);

    if (!(rxStatus & MOT_FEC_RX_ACT))
	{
        MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecHandleRecvInt: RU NOT READY\n"),
				      0, 0, 0, 0, 0, 0);

	MOT_FEC_RX_ACTIVATE;
	}

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecHandleRecvInt... Done, \n"),
				  0, 0, 0, 0, 0, 0);
    }

/******************************************************************************
*
* motFecReceive - pass a received frame to the next layer up
*
* This routine hands the packet off to the upper layer after cleaning the
* RBD that was used for its reception.
*
* RETURNS: N/A
*/

LOCAL void motFecReceive
    (
    DRV_CTRL *  	pDrvCtrl,       /* pointer to DRV_CTRL structure */
    MOT_FEC_RBD_ID      pUsedRbd        /* pointer to a RBD */
    )
    {
    char *	pData = NULL;	/* a rx data pointer */
    char *	pBuf = NULL;	/* a rx data pointer */
    M_BLK_ID	pMblk  = NULL;	/* mBlock pointer */
    CL_BLK_ID	pClBlk = NULL;	/* cluster block pointer */
    UINT16	rbdStatus = 0;	/* holder for the status of this RBD */
    UINT16	rbdLen = 0;	/* number of bytes received by this RBD */
    void *	retVal = NULL;	/* convenient holder for return value */

    MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecReceive rbdIndex=%d\n"),
				  pDrvCtrl->rbdIndex, 0, 0, 0, 0, 0);

    /* pass the packet up only if reception was Ok */

    MOT_FEC_BD_WORD_RD (pUsedRbd, MOT_FEC_BD_STAT_OFF,
			rbdStatus);

    if (rbdStatus & MOT_FEC_RBD_ERR)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecReceive: bad rbd\n"),
				      1, 2, 3, 4, 5, 6);

        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);

#ifdef MOT_FEC_DBG
	if ((rbdStatus & MOT_FEC_RBD_LG) == MOT_FEC_RBD_LG)
	    {
	    MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecReceive: len violation\n"),
					  1, 2, 3, 4, 5, 6);
	    MOT_FEC_RX_LG_ADD;
	    }

	if ((rbdStatus & MOT_FEC_RBD_NO) == MOT_FEC_RBD_NO)
	    {
	    MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecReceive: not aligned\n"),
					  1, 2, 3, 4, 5, 6);
	    MOT_FEC_RX_NO_ADD;
	    }

	if ((rbdStatus & MOT_FEC_RBD_CRC) == MOT_FEC_RBD_CRC)
	    {
	    MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecReceive: CRC error\n"),
					  1, 2, 3, 4, 5, 6);
	    MOT_FEC_RX_CRC_ADD;
	    }

	if ((rbdStatus & MOT_FEC_RBD_OV) == MOT_FEC_RBD_OV)
	    {
	    MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecReceive: rx overrun\n"),
					  1, 2, 3, 4, 5, 6);
	    MOT_FEC_RX_OV_ADD;
	    }

	if ((rbdStatus & MOT_FEC_RBD_TR) == MOT_FEC_RBD_TR)
	    {
	    MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecReceive: trunc frame \n"),
					  1, 2, 3, 4, 5, 6);
	    MOT_FEC_RX_TR_ADD;
	    }

#endif /* MOT_FEC_DBG */

	/* put the errored RBD on the RBD queue */

	motFecRbdClean (pDrvCtrl, pBuf);

	return;
	}

    /* get the actual amount of received data */

    MOT_FEC_BD_WORD_RD (pUsedRbd, MOT_FEC_BD_LEN_OFF,
			rbdLen);

    if (rbdLen < ETHERSMALL)
	{
	MOT_FEC_RX_LS_ADD;

	/* put the errored RBD on the RBD queue */

	motFecRbdClean (pDrvCtrl, pBuf);

	return;
	}

    /* Allocate an MBLK, and a replacement buffer */

    pMblk = NET_MBLK_ALLOC();
    pBuf = NET_BUF_ALLOC ();
    pClBlk = NET_CL_BLK_ALLOC();

    if ((pMblk == NULL) || (pBuf == NULL) ||
	(pClBlk == NULL))
	{
	MOT_FEC_RX_MEM_ADD;

	MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecReceive mem problem\n"),
				      0, 0, 0, 0, 0, 0);

	goto motFecRecvError;
	}

    MOT_FEC_BD_LONG_RD (pUsedRbd, MOT_FEC_BD_ADDR_OFF, pData);

    NET_CL_BLK_JOIN (pClBlk, (char *) (pDrvCtrl->rxBuf [pDrvCtrl->rbdIndex]),
		     MOT_FEC_MAX_CL_LEN, retVal);

    if (retVal == NULL)
	{
        goto motFecRecvError;
	}

    NET_MBLK_CL_JOIN(pMblk, pClBlk, retVal);
    if (retVal == NULL)
	{
        goto motFecRecvError;
	}

    /* set up the mBlk properly */

    pMblk->mBlkHdr.mFlags   |= M_PKTHDR;
    pMblk->mBlkHdr.mData    = pData;
    pMblk->mBlkHdr.mLen = (rbdLen - ETHER_CRC_LEN) & ~0xc000;
    pMblk->mBlkPktHdr.len   = pMblk->mBlkHdr.mLen;

    /* Make cache consistent with memory */

    MOT_FEC_CACHE_INVAL ((char *) pData, pMblk->mBlkHdr.mLen);
					
    /* up-date statistics */

    if ((*pData ) & (UINT8) 0x01)
        {
        pDrvCtrl->endObj.mib2Tbl.ifInNUcastPkts += 1;
        }
    else
        {
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);
        }

    /* put the new RBD on the RBD queue */

    motFecRbdClean (pDrvCtrl, pBuf);

    END_RCV_RTN_CALL (&pDrvCtrl->endObj, pMblk);

    MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecReceive... Done, \n"),
				  0, 0, 0, 0, 0, 0);

    return;

motFecRecvError:

    /* free buffers/clusters, clean the RBD */

    if (pMblk != NULL)
	NET_MBLK_FREE (pMblk);
    if (pClBlk != NULL)
	NET_CL_BLK_FREE (pClBlk);
    if (pBuf != NULL)
	NET_BUF_FREE ((UCHAR *) pBuf);

    /* put the errored RBD on the RBD queue */

    motFecRbdClean (pDrvCtrl, pBuf);

    }

/**************************************************************************
*
* motFecRbdClean - clean a receive buffer descriptor
*
* This routine cleans a receive buffer descriptor and initializes it
* with the data pointer <pBuf>.
*
* RETURNS: N/A
*
*/

LOCAL void motFecRbdClean
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    char *	pBuf		/* pointer to a new data buffer */
    )
    {
    MOT_FEC_RBD_ID      pUsedRbd;	/* pointer to a RBD */
    char *		pData = NULL;	/* a rx data pointer */

    /* get the first used RBD */

    MOT_FEC_NEXT_RBD (pDrvCtrl, pUsedRbd);

    /* up-date the data pointer with the one provided by the pool */

    if (pBuf != NULL)
	{
	pDrvCtrl->rxBuf [(pDrvCtrl->rbdIndex)] = (UCHAR *) pBuf;

	pData = (char *) NET_TO_MOT_FEC_BUF (pBuf);

	MOT_FEC_BD_LONG_WR (pUsedRbd, MOT_FEC_BD_ADDR_OFF, (UINT32) pData);
	}

    MOT_FEC_BD_WORD_WR (pUsedRbd, MOT_FEC_BD_LEN_OFF, 0);

    /* up-date the status word: treat the last RBD in the ring differently */

    if ((pDrvCtrl->rbdIndex) == (pDrvCtrl->rbdNum - 1))
	{
	MOT_FEC_BD_WORD_WR (pUsedRbd, MOT_FEC_BD_STAT_OFF,
			    (MOT_FEC_RBD_WRAP | MOT_FEC_RBD_EMPTY));
	}
	else
	{
	MOT_FEC_BD_WORD_WR (pUsedRbd, MOT_FEC_BD_STAT_OFF,
			    MOT_FEC_RBD_EMPTY);
	}

    /* let's move on to the next used RBD */

    pDrvCtrl->rbdIndex = (pDrvCtrl->rbdIndex + 1) % (pDrvCtrl->rbdNum);

    MOT_FEC_LOG (MOT_FEC_DBG_RX, ("motFecRbdClean... Done, \n"),
				  0, 0, 0, 0, 0, 0);
    }

/**************************************************************************
*
* motFecRbdInit - initialize the receive buffer ring
*
* This routine initializes the receive buffer descriptors ring.
*
* SEE ALSO: motFecTbdInit()
*
* RETURNS: OK, or ERROR if not enough clusters were available.
*/

LOCAL STATUS motFecRbdInit
    (
    DRV_CTRL *  pDrvCtrl        /* pointer to DRV_CTRL structure */
    )
    {
    char *		pData = NULL;	/* a rx data pointer */
    char *		pBuf = NULL;	/* a rx data pointer */
    MOT_FEC_RBD_ID	pRbd = NULL;	/* generic rbd pointer */
    UINT16		ix;		/* a counter */

    /* the receive ring is located right after the transmit one */

    pDrvCtrl->rbdBase = pRbd = (MOT_FEC_RBD_ID) (pDrvCtrl->pBufBase
						 + MOT_FEC_TBD_MEM (pDrvCtrl));

    for (ix = 0; ix < pDrvCtrl->rbdNum; ix++)
	{
	/* get a cluster buffer from the pool */
	
	if ((pBuf = NET_BUF_ALLOC()) == NULL)
	    return (ERROR);

	/*
	 * fill each buffer with en entire frame and make it available
	 * for the receiver. Do not activate the receiver yet.
	 */

	MOT_FEC_BD_WORD_WR (pRbd, MOT_FEC_BD_STAT_OFF,
			    MOT_FEC_RBD_EMPTY);

	MOT_FEC_BD_WORD_WR (pRbd, MOT_FEC_BD_LEN_OFF, 0);

	/*
	 * what we need to do here is to save the cluster pointer
	 * for use by the net pool later. We won't be using that
	 * cluster as is, because the FEC requires receive buffers
	 * being aligned by 16. So we move that cluster pointer to
	 * the next 16-byte aligned boundary, and program the chip
	 * with that value. When we receive the packet, we'll have
	 * to join the saved cluster pointer to the mBlk.
	 */

	pDrvCtrl->rxBuf [ix] = (UCHAR *) pBuf;

	pData = (char *) NET_TO_MOT_FEC_BUF (pBuf);

	MOT_FEC_BD_LONG_WR (pRbd, MOT_FEC_BD_ADDR_OFF, (UINT32) pData);

	/* move on to the next RBD */

	pRbd += MOT_FEC_RBD_SZ;
	}

    /* have the last RBD to close the ring */

    pRbd -= MOT_FEC_RBD_SZ;

    MOT_FEC_BD_WORD_WR (pRbd, MOT_FEC_BD_STAT_OFF,
			(MOT_FEC_RBD_WRAP | MOT_FEC_RBD_EMPTY));

    return (OK);
    }

/**************************************************************************
*
* motFecBdFree - free the receive buffers
*
* This routine frees the netpool clusters associated with the receive
* buffer descriptors ring. It is called by motFecStop(), in order to
* properly release the driver's resources when the device is stopped.
*
* SEE ALSO: motFecStop()
*
* RETURNS: OK, always.
*/

LOCAL STATUS motFecBdFree
    (
    DRV_CTRL *  pDrvCtrl        /* pointer to DRV_CTRL structure */
    )
    {
    MOT_FEC_RBD_ID	pRbd = NULL;	/* generic rbd pointer */
    UINT16		ix;		/* a counter */

    pRbd = (MOT_FEC_RBD_ID) (pDrvCtrl->rbdBase);

    for (ix = 0; ix < pDrvCtrl->rbdNum; ix++)
	{
	/* return the cluster buffer to the pool */
	
	if ((pDrvCtrl->rxBuf [ix]) != NULL)
	    NET_BUF_FREE ((pDrvCtrl->rxBuf [ix]));

	pRbd += MOT_FEC_RBD_SZ;
	}

    /* free the transmit poll buffer */

    if ((pDrvCtrl->pTxPollBuf) != NULL)
	NET_BUF_FREE ((UCHAR *) (pDrvCtrl->pTxPollBuf));

    return (OK);
    }

/**************************************************************************
*
* motFecPrePhyConfig - configure the chip before the PHY is initialized
*
* This routine programs all the CSRs that are not concerned with the
* PHY configuration. It follows closely but not entirely the
* initialization sequence recommended in the FEC User's Manual.
*
* SEE ALSO: motFecPostPhyConfig()
*
* RETURNS: OK, always.
*/

LOCAL STATUS motFecPrePhyConfig
    (
    DRV_CTRL *  pDrvCtrl        /* pointer to DRV_CTRL structure */
    )
    {
    UINT32	csrVal = 0;	/* holder for the CSR value */
    UINT32	csr0Val = 0;	/* holder for the CSR0 value */
    UINT32	csr1Val = 0;	/* holder for the CSR1 value */
    static BOOL first = TRUE;	/* first initialization */
    UINT32      miiSpeed;	/* calculated value for MII_SPEED */

    /*
     * we enable the following event interrupts:
     * heartbeat check fail, only if the user requested it;
     * tx and rx babbling errors;
     * tx and rx frame completion;
     * graceful transmit command;
     * mii management interface frame completion;
     * U-bus access error.
     */

    if (MOT_FEC_USR_FLAG_ISSET (MOT_FEC_USR_HBC))
	{
	csrVal |= MOT_FEC_EVENT_HB;

	MOT_FEC_LOG (MOT_FEC_DBG_START,
		     ("motFecPrePhyConfig: heartbeat control \n"),
		     0, 0, 0, 0, 0, 0);
	}
    else
	csrVal &= ~MOT_FEC_EVENT_HB;

    csrVal |= (MOT_FEC_EVENT_GRA | MOT_FEC_EVENT_MII | MOT_FEC_EVENT_TXF |
	       MOT_FEC_EVENT_RXF | MOT_FEC_EVENT_BABR | MOT_FEC_EVENT_BABT |
	       MOT_FEC_EVENT_BERR);

    MOT_FEC_CSR_WR (MOT_FEC_MASK_OFF, csrVal);

    /* save the interrupt mask register */

    pDrvCtrl->intMask = csrVal;

    /* clear all interrupts */

    MOT_FEC_CSR_WR (MOT_FEC_EVENT_OFF, MOT_FEC_EVENT_MSK);

    /* set the interrupt level */

    MOT_FEC_CSR_WR (MOT_FEC_VEC_OFF, (pDrvCtrl->ilevel << MOT_FEC_LVL_SHIFT));

    if (pDrvCtrl->fifoTxBase != NONE)
	MOT_FEC_CSR_WR (MOT_FEC_TX_FIFO_OFF, pDrvCtrl->fifoTxBase);

    if (pDrvCtrl->fifoRxBase != NONE)
	MOT_FEC_CSR_WR (MOT_FEC_RX_FIFO_OFF, pDrvCtrl->fifoRxBase);

    /* program the individual enet address */

    if (motFecAddrRegValGet (pDrvCtrl, &csr0Val, &csr1Val) != OK)
	return (ERROR);

    MOT_FEC_CSR_WR (MOT_FEC_ADDR_L_OFF, csr0Val);
    MOT_FEC_CSR_WR (MOT_FEC_ADDR_H_OFF, csr1Val);

    /* reset the hash table */
    /* question: does muxDevStop/Start re-init the multi table? */

    if (first)
	{
	first = FALSE;
	MOT_FEC_CSR_WR (MOT_FEC_HASH_L_OFF, 0);
	MOT_FEC_CSR_WR (MOT_FEC_HASH_H_OFF, 0);
	}

    /*
     * if no clock speed is defined, assume a clock speed of 50 MHz.
     * Otherwise, derive the MII clock speed according to the formula
     * MII_SPEED = ceil(pDrvCtrl->clockSpeed / (2 * MII_CLOCK_MAX))
     */

    if (pDrvCtrl->clockSpeed == 0)
	miiSpeed = MOT_FEC_MII_SPEED_50;
    else
	miiSpeed = (pDrvCtrl->clockSpeed + (2 * MOT_FEC_MII_CLOCK_MAX) - 1) /
		   (2 * MOT_FEC_MII_CLOCK_MAX);

    /* MII_SPEED is left-shifted in the MII_SPEED register */

    MOT_FEC_CSR_WR (MOT_FEC_MII_SPEED_OFF, miiSpeed << MOT_FEC_MII_SPEED_SHIFT);
	
    /* program the max receive buffer length */

    MOT_FEC_CSR_WR (MOT_FEC_RX_BUF_OFF, MOT_FEC_MAX_RX_BUF);

    /* program the receive and transmit rings registers */

    MOT_FEC_CSR_WR (MOT_FEC_RX_START_OFF, (UINT32) (pDrvCtrl->rbdBase));
    MOT_FEC_CSR_WR (MOT_FEC_TX_START_OFF, (UINT32) (pDrvCtrl->tbdBase));

    MOT_FEC_CSR_WR (MOT_FEC_SDMA_OFF, MOT_FEC_SDMA_DATA_BE |
				      MOT_FEC_SDMA_BD_BE |
				      MOT_FEC_SDMA_FUNC_0);

    MOT_FEC_CSR_WR (MOT_FEC_RX_FR_OFF, MOT_FEC_BUF_V_LEN);

    return (OK);
    }


/**************************************************************************
*
* motFecPostPhyConfig - configure the chip after the PHY is initialized
*
* This routine programs all the CSRs that are concerned with the
* PHY configuration.
*
* SEE ALSO: motFecPrePhyConfig()
*
* RETURNS: OK, always.
*/

LOCAL STATUS motFecPostPhyConfig
    (
    DRV_CTRL *  pDrvCtrl        /* pointer to DRV_CTRL structure */
    )
    {
    UINT32	rxCtrlVal;	/* holder for the rx CSR value */
    UINT32	txCtrlVal;	/* holder for the tx CSR value */

    /* get the proper value for the rx CSR and program it accordingly */

    if (motFecRxCtrlRegValGet (pDrvCtrl, &rxCtrlVal) == ERROR)
	return (ERROR);

    MOT_FEC_CSR_WR (MOT_FEC_RX_CTRL_OFF, rxCtrlVal);

    /*
     * get the proper value for the tx CSR and program it accordingly.
     * At this point we do not have to check whether the FEC is
     * active, as we already know.
     */

    if (motFecTxCtrlRegValGet (pDrvCtrl, &txCtrlVal) == ERROR)
	return (ERROR);

    MOT_FEC_CSR_WR (MOT_FEC_TX_CTRL_OFF, txCtrlVal);

    /*
     * disable the MII management interface, as we do not
     * need to change the MII setting at any time.
     */

    MOT_FEC_CSR_WR (MOT_FEC_MII_SPEED_OFF, MOT_FEC_MII_MAN_DIS);

    return (OK);
    }

/**************************************************************************
*
* motFecRxCtrlRegValGet - get the proper value for the receive CSR
*
* This routine finds out the proper value to be programmed in the
* receive CSR by looking at some of the user/driver flags. It deals
* with options like promiscous mode, full duplex, loopback and
* the physical serial interface.
*
* This routine does not program the receive CSR.
*
* SEE ALSO: motFecTxCtrlRegValGet(), motFecPostPhyConfig()
*
* RETURNS: OK or ERROR if rxCtrlVal is NULL pointer.
*/

LOCAL STATUS motFecRxCtrlRegValGet
    (
    DRV_CTRL *  pDrvCtrl,	/* pointer to DRV_CTRL structure */
    UINT32 *	rxCtrlVal	/* holder for the rx CSR value */
    )
    {

    /* Initialize holder for the rx CSR value */

    if (rxCtrlVal == NULL)
        {
        MOT_FEC_LOG (MOT_FEC_DBG_START,
                     ("motFecRxCtrlRegValGet: rxCtrlVal is NULL \n"),
                     0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    else
        *rxCtrlVal = 0;

    /* deal with promiscous mode */

    if (MOT_FEC_FLAG_ISSET (MOT_FEC_PROM))
	{
	*rxCtrlVal |= MOT_FEC_RX_CTRL_PROM;

	MOT_FEC_LOG (MOT_FEC_DBG_START,
		     ("motFecRxCtrlRegValGet: promiscous mode \n"),
		     0, 0, 0, 0, 0, 0);
	}
    else
	*rxCtrlVal &= ~MOT_FEC_RX_CTRL_PROM;

    /*
     * enable full duplex mode if the PHY was configured
     * to work in full duplex mode.
     */

    if (MOT_FEC_PHY_FLAGS_ISSET (MOT_FEC_PHY_FD))
	{
	*rxCtrlVal &= ~MOT_FEC_RX_CTRL_DRT;

	MOT_FEC_LOG (MOT_FEC_DBG_START,
		     ("motFecRxCtrlRegValGet: full duplex \n"),
		     0, 0, 0, 0, 0, 0);
	}
    else
	*rxCtrlVal |= MOT_FEC_RX_CTRL_DRT;

    /*
     * enable the 7-wire serial interface if the
     * related user flag was set.
     */

    if (MOT_FEC_USR_FLAG_ISSET (MOT_FEC_USR_SER))
	{
	*rxCtrlVal &= ~MOT_FEC_RX_CTRL_MII;

	MOT_FEC_LOG (MOT_FEC_DBG_START,
		     ("motFecRxCtrlRegValGet: serial interface \n"),
		     0, 0, 0, 0, 0, 0);
	}
    else
	*rxCtrlVal |= MOT_FEC_RX_CTRL_MII;

    /*
     * if the user wishes to go in loopback mode,
     * enable it. Also enable receiver and transmitter
     * to work independently from each other.
     */

    if (MOT_FEC_USR_FLAG_ISSET (MOT_FEC_USR_LOOP))
	{
	*rxCtrlVal |= MOT_FEC_RX_CTRL_LOOP;
	*rxCtrlVal &= ~MOT_FEC_RX_CTRL_DRT;

	MOT_FEC_LOG (MOT_FEC_DBG_START,
		     ("motFecRxCtrlRegValGet: loopback mode \n"),
		     0, 0, 0, 0, 0, 0);
	}
    else
	*rxCtrlVal &= ~MOT_FEC_RX_CTRL_LOOP;

    return (OK);
    }

/**************************************************************************
*
* motFecTxCtrlRegValGet - get the proper value for the transmit CSR
*
* This routine finds out the proper value to be programmed in the
* transmit CSR by looking at some of the user/driver flags. It deals
* with options like full duplex mode and the heartbeat control.
*
* This routine does not program the transmit CSR.
*
* RETURNS: OK or ERROR if txCtrlVal is NULL pointer.
*
* SEE ALSO: motFecRxCtrlRegValGet(), motFecPostPhyConfig()
*
*/

LOCAL STATUS motFecTxCtrlRegValGet
    (
    DRV_CTRL *  pDrvCtrl,	/* pointer to DRV_CTRL structure */
    UINT32 *	txCtrlVal	/* holder for the tx CSR value */
    )
    {

    /* Initialize holder for the tx CSR value */

    if (txCtrlVal == NULL)
        {
        MOT_FEC_LOG (MOT_FEC_DBG_START,
                     ("motFecTxCtrlRegValGet: txCtrlVal is NULL \n"),
                     0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    else
        *txCtrlVal = 0;

    /* deal with the heartbeat control */

    if (MOT_FEC_USR_FLAG_ISSET (MOT_FEC_USR_HBC))
	{
	*txCtrlVal |= MOT_FEC_TX_CTRL_HBC;

	MOT_FEC_LOG (MOT_FEC_DBG_START,
		     ("motFecTxCtrlRegValGet: heartbeat control \n"),
		     0, 0, 0, 0, 0, 0);
	}
    else
	*txCtrlVal &= ~MOT_FEC_TX_CTRL_HBC;

    /*
     * enable full duplex mode if the PHY was configured
     * to work in full duplex mode.
     */

    if (MOT_FEC_PHY_FLAGS_ISSET (MOT_FEC_PHY_FD))
	{
	*txCtrlVal |= MOT_FEC_TX_CTRL_FD;

	MOT_FEC_LOG (MOT_FEC_DBG_START,
		     ("motFecTxCtrlRegValGet: full duplex \n"),
		     0, 0, 0, 0, 0, 0);
	}
    else
	*txCtrlVal &= ~MOT_FEC_TX_CTRL_FD;

    return (OK);
    }

/**************************************************************************
*
* motFecAddrRegValGet - get the values to program in CSR0 and CSR1
*
* This routine finds out the proper values to be programmed in the
* two 32-bit perfect match address registers (CSR0 and CSR1).
*
* This routine does not program neither CSR0 nor CSR1.
*
* RETURNS: OK, always.
*/

LOCAL STATUS motFecAddrRegValGet
    (
    DRV_CTRL *  pDrvCtrl,	/* pointer to DRV_CTRL structure */
    UINT32 *	csr0Val,	/* holder for the CSR0 value */
    UINT32 *	csr1Val		/* holder for the CSR1 value */
    )
    {
    char *	enetAddr = NULL;	/* holder for the enet address */

    /*
     * programming the enet address is done by writing
     * its low-order 4 bytes to CSR0 and its high-order
     * 2 bytes to the MSW of CSR1.
     */

    enetAddr = (char *) MOT_FEC_ADDR_GET (&pDrvCtrl->endObj);

    *csr0Val = * (UINT32 *) enetAddr;

    /*
     * this way we'll zero-out the low-order 16 bits in the
     * CSR, as recommended in the documentation.
     */

    *csr1Val = ((UINT32) (* (UINT16 *) (enetAddr + 4)) << 16);

    return (OK);
    }

/*******************************************************************************
*
* motFecIoctl - interface ioctl procedure
*
* Process an interface ioctl request.
*
* RETURNS: OK, or ERROR.
*/

LOCAL int motFecIoctl
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    int         cmd,		/* command to process */
    caddr_t     data            /* pointer to data */
    )
    {
    int         error = OK;
    INT8        savedFlags;
    long        value;
    END_OBJ *   pEndObj=&pDrvCtrl->endObj;

    MOT_FEC_LOG (MOT_FEC_DBG_IOCTL,
		 ("Ioctl unit=0x%x cmd=%d data=0x%x\n"),
		 pDrvCtrl->unit, cmd, (int)data, 0, 0, 0);

    switch (cmd)
        {
        case EIOCSADDR:
            if (data == NULL)
                error = EINVAL;
            else
                {
		UINT32	csr0Val;	/* holder for the CSR0 value */
		UINT32	csr1Val;	/* holder for the CSR1 value */

                /* Copy and install the new address */

		bcopy ((char *) data,
		       (char *) MOT_FEC_ADDR_GET (&pDrvCtrl->endObj),
		       MOT_FEC_ADDR_LEN_GET (&pDrvCtrl->endObj));

		/* stop the FEC */

		if (motFecStop (pDrvCtrl) != OK)
		    return (ERROR);

		/* program the individual enet address */

		if (motFecAddrRegValGet (pDrvCtrl, &csr0Val, &csr1Val) != OK)
		    return (ERROR);

		MOT_FEC_CSR_WR (MOT_FEC_ADDR_L_OFF, csr0Val);
		MOT_FEC_CSR_WR (MOT_FEC_ADDR_H_OFF, csr1Val);

		/* restart the FEC */

		if (motFecStart (pDrvCtrl) != OK)
		    return (ERROR);
                }

            break;

        case EIOCGADDR:
            if (data == NULL)
                error = EINVAL;
            else
		bcopy ((char *) MOT_FEC_ADDR_GET (&pDrvCtrl->endObj),
		       (char *) data,
		       MOT_FEC_ADDR_LEN_GET (&pDrvCtrl->endObj));

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

            MOT_FEC_LOG (MOT_FEC_DBG_IOCTL, ("endFlags=0x%x \n"),
					     END_FLAGS_GET(pEndObj),
					     0, 0, 0, 0, 0);

            /* handle IFF_PROMISC */

            savedFlags = MOT_FEC_FLAG_GET();
            if (END_FLAGS_ISSET (IFF_PROMISC))
		{
                MOT_FEC_FLAG_SET (MOT_FEC_PROM);

		if ((MOT_FEC_FLAG_GET () != savedFlags) &&
		    (END_FLAGS_GET (pEndObj) & IFF_UP))
		    {
		    UINT32	rxCtrlVal;

		    /* config down */

		    END_FLAGS_CLR (pEndObj, IFF_UP | IFF_RUNNING);

		    /* program the rx CSR to promiscous mode */

		    if (motFecRxCtrlRegValGet (pDrvCtrl, &rxCtrlVal) == ERROR)
			return (ERROR);

		    MOT_FEC_CSR_WR (MOT_FEC_RX_CTRL_OFF, rxCtrlVal);

		    /* config up */

		    END_FLAGS_SET (pEndObj, IFF_UP | IFF_RUNNING);
		    }
		}
            else
                MOT_FEC_FLAG_CLEAR (MOT_FEC_PROM);

            /* handle IFF_MULTICAST */

            if (END_FLAGS_GET(pEndObj) & (IFF_MULTICAST))
                MOT_FEC_FLAG_SET (MOT_FEC_MCAST);
            else
                MOT_FEC_FLAG_CLEAR (MOT_FEC_MCAST);

            MOT_FEC_LOG (MOT_FEC_DBG_IOCTL, ("EIOCSFLAGS: 0x%x: 0x%x\n"),
					     pEndObj->flags, savedFlags,
					     0, 0, 0, 0);

            break;

        case EIOCGFLAGS:
            MOT_FEC_LOG (MOT_FEC_DBG_IOCTL, ("EIOCGFLAGS: 0x%x: 0x%x\n"),
					    pEndObj->flags, *(long *)data,
					    0, 0, 0, 0);

            if (data == NULL)
                error = EINVAL;
            else
                *(long *)data = END_FLAGS_GET(pEndObj);

            break;

        case EIOCMULTIADD:
            error = motFecMCastAddrAdd (pDrvCtrl, (char *) data);
            break;

        case EIOCMULTIDEL:
            error = motFecMCastAddrDel (pDrvCtrl, (char *) data);
            break;

        case EIOCMULTIGET:
            error = motFecMCastAddrGet (pDrvCtrl, (MULTI_TABLE *) data);
            break;

        case EIOCPOLLSTART:
	    MOT_FEC_LOG (MOT_FEC_DBG_POLL, ("IOCTL call motFecPollStart\n"),
					    0, 0, 0, 0, 0, 0);

            motFecPollStart (pDrvCtrl);
            break;

        case EIOCPOLLSTOP:
	    MOT_FEC_LOG (MOT_FEC_DBG_POLL, ("IOCTL call motFecPollStop\n"),
					    0, 0, 0, 0, 0, 0);

            motFecPollStop (pDrvCtrl);
            break;

        case EIOCGMIB2:
            if (data == NULL)
                error=EINVAL;
            else
		bcopy ((char *) &pEndObj->mib2Tbl, (char *) data,
			sizeof (pEndObj->mib2Tbl));

            break;

        default:
            MOT_FEC_LOG (MOT_FEC_DBG_IOCTL, ("INVALID IO COMMAND!! \n"),
					     0, 0, 0, 0, 0, 0);
            error = EINVAL;
        }

    return (error);
    }

/**************************************************************************
*
* motFecPhyInit - initialize and configure the PHY devices
*
* This routine scans, initializes and configures PHY devices.
*
* This routine is called from the driver's Start routine to
* perform media inialization and configuration. To access the PHY
* device, it uses the routines: motFecPhyRead(), motFecPhyWrite(),
* which exploit the support to the MII interface built-in in the
* FEC.
*
* Before it attempts to bring the link up, this routine checks the
* phyInfo structure in the driver control structure for a device that
* needs to be electrically isolated by the MII interface; if a valid
* device is found it is isolated.
*
* The routine then scans for all possible PHY addresses in the range 0-31,
* checking for an MII-compliant PHY, and attempts to  establish a
* valid link for it. If none is found, ERROR is returned.
* Typically PHYs are scanned from address 0, but if the user specifies
* an alternative start PHY address via the parameter phyAddr in the
* driver's load string, all (32) PHYs are scanned in order starting
* with the specified PHY address.
*
* This routine offers two strategies to select a PHY and establish a
* valid link. The default strategy is to use the standard 802.3 style
* auto-negotiation, where both link partners negotiate all their
* technology abilities at the same time, and the highest common
* denominator ability is chosen. Before the auto-negotiation
* is started, the next-page exchange mechanism is disabled.
*
* The user can prevent the PHY from negotiating certain abilities via
* userFlags -- <MOT_FEC_USR_PHY_NO_FD>, <MOT_FEC_USR_PHY_NO_100>,
* <MOT_FEC_USR_PHY_NO_HD>, and <MOT_FEC_USR_PHY_NO_10>. When
* <MOT_FEC_USR_PHY_NO_FD> is specified, full duplex will not be
* negotiated; when <MOT_FEC_USR_PHY_NO_HD> is specified half duplex
* will not be negotiated, when <MOT_FEC_USR_PHY_NO_100> is specified,
* 100Mbps ability will not negotiated; when <MOT_FEC_USR_PHY_NO_10>
* 10Mbps ability will not be negotiated.
*
* When <MOT_FEC_USR_PHY_TBL> is set in the user flags, the BSP specific
* table <motFecPhyAnOrderTbl> is used to obtain the list, and the order
* of technology abilities to be negotiated.
* The entries in this table are ordered such that entry 0 is the
* highest priority, entry 1 in next and so on. Entries in this table
* may be repeated, and multiple technology abilities can
* be OR'd to create a single  entry. If a PHY cannot support a
* ability in an entry, that entry is ignored. Currently, only one
* table per driver is supported.
*
* If no PHY provides a valid link, the PHYs are scanned again in the
* same order, and the first PHY that supports the default abilities
* defined in the <phyDefMode> of the Load string will be selected,
* regardless of the link status.
*
* RETURNS: OK or ERROR if the PHY could not be initialised.
*
*/

LOCAL STATUS motFecPhyInit
    (
    DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    UINT8	phyAddr;	/* address of a PHY */
    UINT8	isoPhyAddr;	/* address of a PHY to be isolated */
    UINT8	ix;		/* an index */
    int		retVal;		/* convenient holder for return value */
    BOOL	found = FALSE;	/* no PHY has been found */

    /* isolate the PHY in the phyInfo structure */

    isoPhyAddr = pDrvCtrl->phyInfo->isoPhyAddr;

    if (isoPhyAddr != MOT_FEC_PHY_NULL)
	{
	/* check the PHY is there */

	retVal = motFecMiiProbe (pDrvCtrl, isoPhyAddr);
	if (retVal == ERROR)
	    return (ERROR);

	if (retVal == OK)
	    if (motFecMiiIsolate (pDrvCtrl, isoPhyAddr) == ERROR)
		return (ERROR);
	}

    /*
     * there may be several PHYs, with distinct logical addresses
     * and these can be as high as 31. Start with the one the
     * user suggested and, in case of failure, scan the whole range.
     */

    for (ix = 0; ix < MII_MAX_PHY_NUM; ix++)
	{
	phyAddr = (ix + pDrvCtrl->phyInfo->phyAddr) % MII_MAX_PHY_NUM;

	/* check this is not the isolated PHY */

	if (phyAddr == isoPhyAddr)
	    continue;

	/* check the PHY is there */

	retVal = motFecMiiProbe (pDrvCtrl, phyAddr);
	if (retVal == ERROR)
	    return (ERROR);

	if (retVal == MOT_FEC_PHY_NULL)
	    continue;

	found = TRUE;

	/* run some diagnostics */

	if (motFecMiiDiag (pDrvCtrl, phyAddr) != OK)
	    return (ERROR);

	/*
	 * start the auto-negotiation process,
	 * unless the user dictated the contrary.
	 */

	if (pDrvCtrl->phyInfo->phyFlags & MOT_FEC_PHY_AUTO)
	    if (motFecMiiAnRun (pDrvCtrl, phyAddr) == OK)
		return (OK);

	/*
	 * the auto-negotiation process did not succeed
	 * in establishing a valid link: try to do it
	 * manually, enabling as high priority abilities
	 * as you can.
	 */

	if (motFecMiiModeForce (pDrvCtrl, phyAddr) == OK)
	    return (OK);

	/*
	 * Trying to force a specific operating mode was
	 * unsuccessful, too. Force default parameters:
	 * field phyDefMode in the PHY info structure.
	 */

	if (motFecMiiDefForce (pDrvCtrl, phyAddr) == OK)
	    return (OK);
	}

    if (!(found))
	return (ERROR);

    /* if we're here, none of the PHYs could be initialized */

       logMsg ("motFecPhyInit check cable connection \n",
	        0, 0, 0, 0, 0, 0);

    /* try to establish a default operating mode, do not check the link! */

    for (ix = 0; ix < MII_MAX_PHY_NUM; ix++)
	{
	phyAddr = (ix + pDrvCtrl->phyInfo->phyAddr) % MII_MAX_PHY_NUM;

	/* check this is not the isolated PHY */

	if (phyAddr == isoPhyAddr)
	    continue;

	/* check the PHY is there */

	retVal = motFecMiiProbe (pDrvCtrl, phyAddr);
	if (retVal == ERROR)
	    return (ERROR);

	if (retVal == MOT_FEC_PHY_NULL)
	    continue;

	/* return OK even if the link is not up */

	retVal = motFecMiiDefForce (pDrvCtrl, phyAddr);
	if (retVal == ERROR)
	    return (ERROR);

	/* if the PHY does not have the default abilities... */

	if (retVal == MOT_FEC_PHY_NO_ABLE)
	    continue;

	return (OK);
	}

    return (ERROR);
    }

/**************************************************************************
*
* motFecMiiProbe - probe the PHY device
*
* This routine probes the PHY device by reading its control register.
*
* RETURNS: OK, ERROR in case of fatal errors, or MOT_FEC_PHY_NULL, if
* the device was not found.
*/

LOCAL STATUS motFecMiiProbe
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UINT8	phyAddr		/* the PHY being read */
    )
    {
    UINT8	regAddr;	/* the PHY's register being read */
    UINT16	data;		/* data to be written to the control reg */

    regAddr = MII_CTRL_REG;
    if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &data) != OK)
	return (ERROR);

    if (data == 0xffff)
	return (MOT_FEC_PHY_NULL);

    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiProbe... ends\n"),
				   0, 0, 0, 0, 0, 0);

    return (OK);
    }

/**************************************************************************
*
* motFecPhyPreInit - initialize some fields in the phy info structure
*
* This routine initializes some fields in the phy info structure,
* for use of the motFecPhyInit() routine.
*
* RETURNS: OK, always.
*/

LOCAL STATUS motFecPhyPreInit
    (
    DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    /* set some default values */

    pDrvCtrl->phyInfo->phySpeed = MOT_FEC_10MBS;
    pDrvCtrl->phyInfo->phyMode = MOT_FEC_PHY_HD;
    pDrvCtrl->phyInfo->phyFlags = 0;

    /* handle some user-to-physical flags */

    if (!(MOT_FEC_USR_FLAG_ISSET (MOT_FEC_USR_PHY_NO_AN)))
	MOT_FEC_PHY_FLAGS_SET (MOT_FEC_PHY_AUTO);
    else
	MOT_FEC_PHY_FLAGS_CLEAR (MOT_FEC_PHY_AUTO);

    if (MOT_FEC_USR_FLAG_ISSET (MOT_FEC_USR_PHY_TBL))
	MOT_FEC_PHY_FLAGS_SET (MOT_FEC_PHY_TBL);
    else
	MOT_FEC_PHY_FLAGS_CLEAR (MOT_FEC_PHY_TBL);

    if (!(MOT_FEC_USR_FLAG_ISSET (MOT_FEC_USR_PHY_NO_100)))
	MOT_FEC_PHY_FLAGS_SET (MOT_FEC_PHY_100);
    else
	MOT_FEC_PHY_FLAGS_CLEAR (MOT_FEC_PHY_100);

    if (!(MOT_FEC_USR_FLAG_ISSET (MOT_FEC_USR_PHY_NO_FD)))
	MOT_FEC_PHY_FLAGS_SET (MOT_FEC_PHY_FD);
    else
	MOT_FEC_PHY_FLAGS_CLEAR (MOT_FEC_PHY_FD);

    if (!(MOT_FEC_USR_FLAG_ISSET (MOT_FEC_USR_PHY_NO_10)))
	MOT_FEC_PHY_FLAGS_SET (MOT_FEC_PHY_10);
    else
	MOT_FEC_PHY_FLAGS_CLEAR (MOT_FEC_PHY_10);

    if (!(MOT_FEC_USR_FLAG_ISSET (MOT_FEC_USR_PHY_NO_HD)))
	MOT_FEC_PHY_FLAGS_SET (MOT_FEC_PHY_HD);
    else
	MOT_FEC_PHY_FLAGS_CLEAR (MOT_FEC_PHY_HD);

    /* mark it as pre-initilized */

    MOT_FEC_PHY_FLAGS_SET (MOT_FEC_PHY_PRE_INIT);

    return (OK);
    }

/**************************************************************************
*
* motFecMiiRead - read the MII register
*
* This routine reads the register specified by <phyReg> in the PHY device
* whose address is <phyAddr>. The value read is returned in the location
* pointed to by <retVal>.
*
* SEE ALSO: motFecMiiWrite()
*
* RETURNS: OK, always.
*
*/

LOCAL STATUS motFecMiiRead
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to DRV_CTRL structure */
    UINT8	phyAddr,	/* the PHY being read */
    UINT8	regAddr,	/* the PHY's register being read */
    UINT16	*retVal		/* the value read */
    )
    {
    UINT32	reg;	/* convenient holder for the PHY register */
    UINT32	phy;	/* convenient holder for the PHY */
    UINT32	miiVal; /* value written/read to/from the FEC's MII register */

    /*
     * reading from any PHY's register is done by properly
     * programming the FEC's MII data register.
     */

    reg = regAddr << MOT_FEC_MII_RA_SHIFT;
    phy = phyAddr << MOT_FEC_MII_PA_SHIFT;

    miiVal = (MOT_FEC_MII_ST | MOT_FEC_MII_OP_RD | MOT_FEC_MII_TA |
	      phy | reg);

    MOT_FEC_CSR_WR (MOT_FEC_MII_DATA_OFF, miiVal);

    /* wait for the related interrupt */

    MOT_FEC_MII_SEM_TAKE;

    /* it's now safe to read the PHY's register */

    MOT_FEC_CSR_RD (MOT_FEC_MII_DATA_OFF, miiVal);

    *retVal = (UINT16) miiVal;

    return (OK);
    }

/**************************************************************************
*
* motFecMiiWrite - write to the MII register
*
* This routine writes the register specified by <phyReg> in the PHY device,
* whose address is <phyAddr>, with the 16-bit value included in <data>.
*
* SEE ALSO: motFecMiiRead()
*
* RETURNS: OK, always.
*/

LOCAL STATUS motFecMiiWrite
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to DRV_CTRL structure */
    UINT8	phyAddr,	/* the PHY being written */
    UINT8	regAddr,	/* the PHY's register being written */
    UINT16	data		/* the value written to the PHY register */
    )
    {
    UINT32		reg;	/* convenient holder for the PHY register */
    UINT32		phy;	/* convenient holder for the PHY */
    UINT32		miiVal; /* value written to the mii reg */

    reg = regAddr << MOT_FEC_MII_RA_SHIFT;
    phy = phyAddr << MOT_FEC_MII_PA_SHIFT;

    miiVal = (MOT_FEC_MII_ST | MOT_FEC_MII_OP_WR | MOT_FEC_MII_TA |
	      phy | reg | data);

    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiWrite reg=%d \n\
				   PHY=%d data=0x%x\n"),
				   regAddr, phyAddr, miiVal, 0, 0, 0);

    MOT_FEC_CSR_WR (MOT_FEC_MII_DATA_OFF, miiVal);

    /* wait for the related interrupt */

    MOT_FEC_MII_SEM_TAKE;

    return (OK);
    }

/*******************************************************************************
*
* motFecMiiDiag - run some diagnostics
*
* This routine runs some diagnostics on the PHY whose address is
* specified in the parameter <phyAddr>.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS motFecMiiDiag
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT16	data;		/* data to be written to the control reg */
    UINT8	regAddr;	/* PHY register */
    UINT16	ix = 0;		/*  a counter */

    /* reset the PHY */

    regAddr = MII_CTRL_REG;
    data = MII_CR_RESET;

    if (motFecMiiWrite (pDrvCtrl, phyAddr, regAddr, data) != OK)
	return (ERROR);

    while (ix++ < MOT_FEC_PHY_MAX_WAIT)
	{
	taskDelay (1);

	if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &data) != OK)
	    return (ERROR);

	if (!(data & MII_CR_RESET))
	    break;
	}

    if (ix >= MOT_FEC_PHY_MAX_WAIT)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiDiag reset fail\n"),
				       0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    /* isolate the PHY */

    if (motFecMiiIsolate (pDrvCtrl, phyAddr) == ERROR)
	return (ERROR);

    /* re-enable the chip */

    data = MII_CR_NORM_EN;

    if (motFecMiiWrite (pDrvCtrl, phyAddr, regAddr, data) != OK)
	return (ERROR);

    /* check isolate bit is deasserted */

    ix = 0;
    while (ix++ < MOT_FEC_PHY_MAX_WAIT)
	{
	taskDelay (1);

	if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &data) != OK)
	    return (ERROR);

	if (!(data & MII_CR_ISOLATE))
	    break;
	}

    if (ix >= MOT_FEC_PHY_MAX_WAIT)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiDiag de-isolate fail\n"),
				       0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiDiag... ends\n"),
				   0, 0, 0, 0, 0, 0);
    return (OK);
    }

/*******************************************************************************
*
* motFecMiiIsolate - isolate the PHY device
*
* This routine isolates the PHY device whose address is specified in
* the parameter <isoPhyAddr>.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS motFecMiiIsolate
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UINT8	isoPhyAddr	/* address of a PHY to be isolated */
    )
    {
    UINT8	regAddr;	/* PHY register */
    UINT16	ix = 0;		/*  a counter */
    UINT16	data;		/* data to be written to the control reg */

    if (isoPhyAddr == MOT_FEC_PHY_NULL)
	return (OK);

    data = MII_CR_ISOLATE;
    regAddr = MII_CTRL_REG;

    if (motFecMiiWrite (pDrvCtrl, isoPhyAddr, regAddr, data) != OK)
	return (ERROR);

    /* check isolate bit is asserted */

    ix = 0;
    while (ix++ < MOT_FEC_PHY_MAX_WAIT)
	{
	taskDelay (1);

	if (motFecMiiRead (pDrvCtrl, isoPhyAddr, regAddr, &data) != OK)
	    return (ERROR);

	if ((data & MII_CR_ISOLATE))
	    break;
	}

    if (ix >= MOT_FEC_PHY_MAX_WAIT)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiIsolate fail\n"),
				       0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiIsolate... ends\n"),
				   0, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* motFecMiiAnRun - run the auto-negotiation process
*
* This routine runs the auto-negotiation process for the PHY whose
* address is specified in the parameter <phyAddr>, without enabling the
* next page function. It also calls motFecMiiAnCheck() to ensure
* a valid link has been established.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS motFecMiiAnRun
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT8	regAddr;	/* PHY register */
    UINT16	phyAds;		/* holder for the PHY ads register value */
    UINT16	status;		/* PHY auto-negotiation status */
    UINT16	ix;		/* a counter */
    int		retVal;		/* holder for return value */

    regAddr = MII_AN_ADS_REG;

    if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &phyAds) != OK)
	return (ERROR);

    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiAnRun phyAds=0x%x\n"),
				   phyAds, 0, 0, 0, 0, 0);

    /* disable the next page function */

    phyAds &= (~MII_NP_NP);

    if (motFecMiiWrite (pDrvCtrl, phyAddr, regAddr, phyAds)
	!= OK)
	return (ERROR);

    /* Read ANER to clear status from previous operations */

    regAddr = MII_AN_EXP_REG;
    if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &status) != OK)
	return (ERROR);

    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiAnRun expStat=0x%x\n"),
				   status, 0, 0, 0, 0, 0);

    /* Read ANAR */

    regAddr = MII_AN_ADS_REG;
    if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &phyAds) != OK)
	return (ERROR);

    /* store the current registers values */

    pDrvCtrl->phyInfo->miiRegs.phyAds = phyAds;

    /*
     * start the auto-negotiation process, possibly
     * following the order the user dictated.
     */

    for (ix = 0; ; ix++)
	{
	if (MOT_FEC_PHY_FLAGS_ISSET (MOT_FEC_PHY_TBL))
	    {
	    /* check we're not at the end of the user table */

	    if ((motFecPhyAnOrderTbl [ix]) == -1)
		return (ERROR);

	    /* just negotiate one ability at a time */

	    phyAds &= ~MII_TECH_MASK;

	    /* translate user settings */

	    phyAds |= miiAnLookupTbl [(motFecPhyAnOrderTbl [ix])];

	    /* check the PHY has the desidered ability */

	    if (!(phyAds & pDrvCtrl->phyInfo->miiRegs.phyAds))
		continue;

	    /* set the ANAR accordingly */

	    regAddr = MII_AN_ADS_REG;
	    if (motFecMiiWrite (pDrvCtrl, phyAddr, regAddr, phyAds)
		!= OK)
		return (ERROR);
	    }
	else
	    {
	    /* check the PHY flags and possibly mask some abilities off */

	    if (!(MOT_FEC_PHY_FLAGS_ISSET (MOT_FEC_PHY_FD)))
		phyAds &= ~(MII_TECH_10BASE_FD | MII_TECH_100BASE_TX_FD
			    | MII_TECH_100BASE_T4);

	    if (!(MOT_FEC_PHY_FLAGS_ISSET (MOT_FEC_PHY_HD)))
		phyAds &= ~(MII_TECH_10BASE_T | MII_TECH_100BASE_TX);

	    if (!(MOT_FEC_PHY_FLAGS_ISSET (MOT_FEC_PHY_100)))
		phyAds &= ~(MII_TECH_100BASE_TX | MII_TECH_100BASE_TX_FD
			    | MII_TECH_100BASE_T4);

	    if (!(MOT_FEC_PHY_FLAGS_ISSET (MOT_FEC_PHY_10)))
		phyAds &= ~(MII_TECH_10BASE_T | MII_TECH_10BASE_FD);

	    /* Write ANAR */

	    regAddr = MII_AN_ADS_REG;
	    if (motFecMiiWrite (pDrvCtrl, phyAddr, regAddr, phyAds) != OK)
		return (ERROR);

	    /* store the current registers values */

	    pDrvCtrl->phyInfo->miiRegs.phyAds = phyAds;

	    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiAnRun phyAds=0x%x\n"),
					   phyAds, 0, 0, 0, 0, 0);

	    }

	/*
	 * start the auto-negotiation process: return
	 * only in case of fatal error.
	 */

	retVal = motFecMiiAnStart (pDrvCtrl, phyAddr);
	if (retVal == ERROR)
	    return (ERROR);

	/*
	 * in case of failure, we return only if we're using
	 * the standard auto-negotiation process.
	 */

	if (retVal == MII_AN_FAIL)
            {
	    if (!(MOT_FEC_PHY_FLAGS_ISSET (MOT_FEC_PHY_TBL)))
		return (retVal);
	    else
		continue;
            }

	/* check the negotiation was successful */

	if (motFecMiiAnCheck (pDrvCtrl, phyAddr) == OK)
	    return (OK);

	/*
	 * we are here if the negotiation went wong:
	 * if the user did not force any priority order,
	 * we return ERROR, as all the PHY abilities
	 * were negotiated at once.
	 */

	if (!(MOT_FEC_PHY_FLAGS_ISSET (MOT_FEC_PHY_TBL)))
	    return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* motFecMiiAnStart - start the auto-negotiation process
*
* This routine starts the auto-negotiation process for the PHY whose
* address is specified in the parameter <phyAddr>.
*
* RETURNS: OK, ERROR in case of fatal errors or MII_AN_FAIL, if the
* auto-negotiation did not complete within a reasonable amount of time.
*/

LOCAL STATUS motFecMiiAnStart
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT16	data;		/* data to be written to the control reg */
    UINT8	regAddr;	/* PHY register */
    UINT16	phyStatus;	/* holder for the PHY status */
    UINT16	ix = 0;		/* a counter */

    regAddr = MII_STAT_REG;

    if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &phyStatus) != OK)
	return (ERROR);

    /* check the PHY has this ability */

    if ((phyStatus & MII_SR_AUTO_SEL) != MII_SR_AUTO_SEL)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiAnStart phy not \n\
				       auto neg capable\n"),
				       0, 0, 0, 0, 0, 0);

	return (ERROR);
	}

    /* restart the auto-negotiation process */

    regAddr = MII_CTRL_REG;
    data = (MII_CR_RESTART | MII_CR_AUTO_EN);

    if (motFecMiiWrite (pDrvCtrl, phyAddr, regAddr, data) != OK)
	return (ERROR);

    /* save status info */

    pDrvCtrl->phyInfo->miiRegs.phyCtrl = data;

    /* let's check the PHY status */

    regAddr = MII_STAT_REG;

    /* spin until it is done */

    do
	{
	taskDelay (1);

	if (ix++ == MOT_FEC_PHY_MAX_WAIT)
	    break;

	if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &phyStatus)
	    != OK)
	    return (ERROR);

	} while ((phyStatus & MII_SR_AUTO_NEG) != MII_SR_AUTO_NEG);

    if (ix >= MOT_FEC_PHY_MAX_WAIT)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_MII,
		     ("motFecMiiAnStart auto neg fail\n"),
		     0, 0, 0, 0, 0, 0);
	return (MII_AN_FAIL);
	}
    else
	{
	MOT_FEC_LOG (MOT_FEC_DBG_MII,
		     ("motFecMiiAnStart auto neg done phyStat=0x%x\n"),
		     phyStatus, 0, 0, 0, 0, 0);
	}

    return (OK);
    }

/*******************************************************************************
*
* motFecMiiBasicCheck - run a basic check on the PHY status register
*
* This routine runs a basic check on the PHY status register to
* ensure a valid link has been established and no faults have been
* detected.
*
* RETURNS: OK, MII_STAT_FAIL, if an error was reported in the PHY's
* status register, or ERROR, in case of unrecoverable errors.
*/

LOCAL STATUS motFecMiiBasicCheck
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT8	regAddr;	/* PHY register */
    UINT16	phyStatus;	/* holder for the PHY status */

    /* let's check the PHY status */

    regAddr = MII_STAT_REG;
    if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &phyStatus) != OK)
	return (ERROR);

    if ((phyStatus & MII_SR_LINK_STATUS) != MII_SR_LINK_STATUS)
	{
	/* wait for a while */

	taskDelay (10);

	/* we need to read it twice, as this's a latched function */

	if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &phyStatus) != OK)
	    return (ERROR);

	if ((phyStatus & MII_SR_LINK_STATUS) != MII_SR_LINK_STATUS)
	    {
	    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiBasicCheck \n\
					   phy stat=0x%x\n"),
					   phyStatus, 0, 0, 0, 0, 0);

	    return (MII_STAT_FAIL);
	    }
	}

    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiBasicCheck\n\
				   Link up! status=0x%x\n"),
				   phyStatus, 0, 0, 0, 0, 0);

    /* check for remote fault condition */

    if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &phyStatus) != OK)
	return (ERROR);

    if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &phyStatus) != OK)
	return (ERROR);

    if ((phyStatus & MII_SR_REMOTE_FAULT) == MII_SR_REMOTE_FAULT)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiBasicCheck remote fault\n"),
				       0, 0, 0, 0, 0, 0);
	return (MII_STAT_FAIL);
	}

    /* store the current registers values */

    pDrvCtrl->phyInfo->miiRegs.phyStatus = phyStatus;

    return (OK);
    }

/*******************************************************************************
*
* motFecMiiAnCheck - check the auto-negotiation process result
*
* This routine checks the auto-negotiation process has completed
* successfully and no faults have been detected by any of the PHYs
* engaged in the process.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS motFecMiiAnCheck
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT8	regAddr;	/* PHY register */
    UINT16	phyAds;		/* PHY advertisement register value */
    UINT16	phyPrtn;	/* PHY partner ability register value */
    UINT16	phyExp;		/* PHY expansion register value */
    UINT16	negAbility;	/* abilities after negotiation */
    int		retVal;		/* convenient holder for return value */

    /* run a check on the status bits of basic registers only */

    retVal = motFecMiiBasicCheck (pDrvCtrl, phyAddr);
    if (retVal != OK)
	return (retVal);

    /* we know the auto-negotiation process has terminated */

    regAddr = MII_AN_EXP_REG;

    if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &phyExp) != OK)
	return (ERROR);

    /* check for faults detected by the parallel function */

    if ((phyExp & MII_EXP_FAULT) == MII_EXP_FAULT)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiAnCheck:\n\
				       fault expStat=0x%x\n"),
				       phyExp, 0, 0, 0, 0, 0);

	return (ERROR);
	}

    /* check for remote faults */

    regAddr = MII_AN_PRTN_REG;

    if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &phyPrtn) != OK)
	return (ERROR);

    if ((phyPrtn & MII_BP_FAULT) == MII_BP_FAULT)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiAnCheck partner stat=0x%x\n"),
				       phyPrtn, 0, 0, 0, 0, 0);

	return (ERROR);
	}


    regAddr = MII_AN_ADS_REG;
    if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &phyAds) != OK)
	return (ERROR);

    /* store the current registers values */

    pDrvCtrl->phyInfo->miiRegs.phyAds = phyAds;
    pDrvCtrl->phyInfo->miiRegs.phyPrtn = phyPrtn;
    pDrvCtrl->phyInfo->miiRegs.phyExp = phyExp;

    /* find out the max common abilities */

    negAbility = phyPrtn & phyAds & MII_TECH_MASK;

    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiAnCheck phyAds=0x%x\n\
				   phyPrtn=0x%x common=0x%x phyExp=0x%x\n"),
				   phyAds,
				   phyPrtn,
				   negAbility,
				   phyExp, 0, 0);

    if (negAbility & MII_TECH_100BASE_TX_FD)
	{
	pDrvCtrl->phyInfo->phySpeed = MOT_FEC_100MBS;
	pDrvCtrl->phyInfo->phyMode = MOT_FEC_PHY_FD;

	MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiAnCheck speed=%d mode=%s\n"),
				       pDrvCtrl->phyInfo->phySpeed,
				       (int) "full duplex", 0, 0, 0, 0);
	}
    else if ((negAbility & MII_TECH_100BASE_T4) ||
	     (negAbility & MII_TECH_100BASE_TX))
	{
	pDrvCtrl->phyInfo->phySpeed = MOT_FEC_100MBS;
	pDrvCtrl->phyInfo->phyMode = MOT_FEC_PHY_HD;

	MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiAnCheck speed=%d mode=%s\n"),
				       pDrvCtrl->phyInfo->phySpeed,
				       (int) "half duplex", 0, 0, 0, 0);
	}
    else if (negAbility & MII_TECH_10BASE_FD)
	{
	pDrvCtrl->phyInfo->phySpeed = MOT_FEC_10MBS;
	pDrvCtrl->phyInfo->phyMode = MOT_FEC_PHY_FD;

	MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiAnCheck speed=%d mode=%s\n"),
				       pDrvCtrl->phyInfo->phySpeed,
				       (int) "full duplex", 0, 0, 0, 0);
	}
    else if (negAbility & MII_TECH_10BASE_T)
	{
	pDrvCtrl->phyInfo->phySpeed = MOT_FEC_10MBS;
	pDrvCtrl->phyInfo->phyMode = MOT_FEC_PHY_HD;

	MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiAnCheck speed=%d mode=%s\n"),
				       pDrvCtrl->phyInfo->phySpeed,
				       (int) "half duplex", 0, 0, 0, 0);
	}
    else
	{
	MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiAnCheck fail!\n"),
				       0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    /* handle some flags */

    if (pDrvCtrl->phyInfo->phySpeed == MOT_FEC_100MBS)
	MOT_FEC_PHY_FLAGS_SET (MOT_FEC_PHY_100);
    else
	MOT_FEC_PHY_FLAGS_CLEAR (MOT_FEC_PHY_100);

    if (pDrvCtrl->phyInfo->phyMode == MOT_FEC_PHY_FD)
	MOT_FEC_PHY_FLAGS_SET (MOT_FEC_PHY_FD);
    else
	MOT_FEC_PHY_FLAGS_CLEAR (MOT_FEC_PHY_FD);

    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiAnCheck OK\n"),
				   0, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* motFecMiiModeForce - force an operating mode for the PHY
*
* This routine forces an operating mode for the PHY whose address is
* specified in the parameter <phyAddr>. It also calls motFecMiiBasicCheck()
* to ensure a valid link has been established.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS motFecMiiModeForce
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT16	data;		/* data to be written to the control reg */
    UINT8	regAddr;	/* PHY register */
    UINT16	phyStatus;	/* holder for the PHY status */

    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiModeForce \n"),
				   0, 0, 0, 0, 0, 0);

    /* find out what abilities the PHY features */

    regAddr = MII_STAT_REG;
    if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &phyStatus) != OK)
        return (ERROR);

    /*
     * force as a high priority as possible operating
     * mode, not overlooking what the user dictated.
     */

    data = MII_CR_NORM_EN;

    if ((phyStatus & (MII_SR_TX_FULL_DPX | MII_SR_TX_HALF_DPX))
        && (MOT_FEC_PHY_FLAGS_ISSET (MOT_FEC_PHY_100)))
        {
        data |= MII_CR_100;
        pDrvCtrl->phyInfo->phySpeed = MOT_FEC_100MBS;
        }

    if ((phyStatus & (MII_SR_TX_FULL_DPX | MII_SR_10T_FULL_DPX | MII_SR_T4))
        && (MOT_FEC_PHY_FLAGS_ISSET (MOT_FEC_PHY_FD)))
        {
        pDrvCtrl->phyInfo->phyMode = MOT_FEC_PHY_FD;
        data |= MII_CR_FDX;
        }

    pDrvCtrl->phyInfo->miiRegs.phyCtrl = data;

    regAddr = MII_CTRL_REG;

    if (motFecMiiWrite (pDrvCtrl, phyAddr, regAddr, data) != OK)
        return (ERROR);

    /* run a check on the status bits of basic registers only */

    if (motFecMiiBasicCheck (pDrvCtrl, phyAddr) != OK)
	return (ERROR);

    /* handle some flags */

    if (pDrvCtrl->phyInfo->phySpeed == MOT_FEC_100MBS)
	MOT_FEC_PHY_FLAGS_SET (MOT_FEC_PHY_100);
    else
	MOT_FEC_PHY_FLAGS_CLEAR (MOT_FEC_PHY_100);

    if (pDrvCtrl->phyInfo->phyMode == MOT_FEC_PHY_FD)
	MOT_FEC_PHY_FLAGS_SET (MOT_FEC_PHY_FD);
    else
	MOT_FEC_PHY_FLAGS_CLEAR (MOT_FEC_PHY_FD);

    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiModeForce speed=%d mode=%d\n"),
				   pDrvCtrl->phyInfo->phySpeed,
				   pDrvCtrl->phyInfo->phyMode,
				   0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* motFecMiiDefForce - force a default operating mode for the PHY
*
* This routine forces a default operating mode for the PHY whose address is
* specified in the parameter <phyAddr>. It also calls motFecMiiBasicCheck()
* to ensure a valid link has been established.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS motFecMiiDefForce
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT16	data;		/* data to be written to the control reg */
    UINT8	regAddr;	/* PHY register */
    int		retVal;		/* convenient holder for return value */
    UINT16	phyStatus;	/* holder for the PHY status */

    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiDefForce \n"),
				   0, 0, 0, 0, 0, 0);

    /* translate user settings */

    data = miiCrLookupTbl [(pDrvCtrl->phyInfo->phyDefMode)];

    /* find out what abilities the PHY features */

    regAddr = MII_STAT_REG;
    if (motFecMiiRead (pDrvCtrl, phyAddr, regAddr, &phyStatus) != OK)
        return (ERROR);

    if (data & MII_CR_100)
	{
	if (!(phyStatus & (MII_SR_TX_HALF_DPX
			 | MII_SR_TX_FULL_DPX
			 | MII_SR_T4)))
	    return (MOT_FEC_PHY_NO_ABLE);

	pDrvCtrl->phyInfo->phySpeed = MOT_FEC_100MBS;
	}
    else
	{
	if (!(phyStatus & (MII_SR_10T_HALF_DPX
			 | MII_SR_10T_FULL_DPX)))
	    return (MOT_FEC_PHY_NO_ABLE);

	/* handle phy flags */

	MOT_FEC_PHY_FLAGS_CLEAR (MOT_FEC_PHY_100);

	pDrvCtrl->phyInfo->phySpeed = MOT_FEC_10MBS;
	}

    if (data & MII_CR_FDX)
	{
	if (!(phyStatus & (MII_SR_10T_FULL_DPX
			 | MII_SR_TX_FULL_DPX
			 | MII_SR_T4)))
	    return (MOT_FEC_PHY_NO_ABLE);

	pDrvCtrl->phyInfo->phyMode = MOT_FEC_PHY_FD;
	}
    else
	{
	if (!(phyStatus & (MII_SR_10T_HALF_DPX
			 | MII_SR_TX_HALF_DPX)))
	    return (MOT_FEC_PHY_NO_ABLE);

	/* handle phy flags */

	MOT_FEC_PHY_FLAGS_CLEAR (MOT_FEC_PHY_FD);

	pDrvCtrl->phyInfo->phyMode = MOT_FEC_PHY_HD;
	}

    pDrvCtrl->phyInfo->miiRegs.phyCtrl = data;

    regAddr = MII_CTRL_REG;

    if (motFecMiiWrite (pDrvCtrl, phyAddr, regAddr, data) != OK)
	return (ERROR);

    /* run a check on the status bits of basic registers only */

    retVal = motFecMiiBasicCheck (pDrvCtrl, phyAddr);

    MOT_FEC_LOG (MOT_FEC_DBG_MII, ("motFecMiiDefForce speed=%d mode=%d\n"),
				   pDrvCtrl->phyInfo->phySpeed,
				   pDrvCtrl->phyInfo->phyMode,
				   0, 0, 0, 0);

    return (retVal);
    }

/*******************************************************************************
*
* motFecMCastAddrAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.
*
* SEE ALSO: motFecMCastAddrDel() motFecMCastAddrGet()
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS motFecMCastAddrAdd
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    char *      pAddr		/* address to be added */
    )
    {
    int		retVal;		/* convenient holder for return value */

    MOT_FEC_LOG (MOT_FEC_DBG_IOCTL, ("motFecMCastAddrAdd addr = 0x%x 0x%x\n\
				     0x%x 0x%x 0x%x 0x%x\n"),
				     (int) (*pAddr + 0), (int) (*pAddr + 1),
				     (int) (*pAddr + 2), (int) (*pAddr + 3),
				     (int) (*pAddr + 4), (int) (*pAddr + 5));

    retVal = etherMultiAdd (&pDrvCtrl->endObj.multiList, pAddr);

    if (retVal == ENETRESET)
	{
        pDrvCtrl->endObj.nMulti++;

	return (motFecHashTblAdd (pDrvCtrl, pAddr));
	}

    return ((retVal == OK) ? OK : ERROR);
    }

/*****************************************************************************
*
* motFecMCastAddrDel - delete a multicast address for the device
*
* This routine deletes a multicast address from the current list of
* multicast addresses.
*
* SEE ALSO: motFecMCastAddrAdd() motFecMCastAddrGet()
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS motFecMCastAddrDel
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    char *      pAddr		/* address to be deleted */
    )
    {
    int		retVal;		/* convenient holder for return value */

    MOT_FEC_LOG (MOT_FEC_DBG_IOCTL, ("motFecMCastAddrDel addr = 0x%x 0x%x\n\
				     0x%x 0x%x 0x%x 0x%x\n"),
				     (int) (*pAddr + 0), (int) (*pAddr + 1),
				     (int) (*pAddr + 2), (int) (*pAddr + 3),
				     (int) (*pAddr + 4), (int) (*pAddr + 5));

    retVal = etherMultiDel (&pDrvCtrl->endObj.multiList, pAddr);

    if (retVal == ENETRESET)
	{
	pDrvCtrl->endObj.nMulti--;

	/* stop the FEC */

	if (motFecStop (pDrvCtrl) != OK)
	    return (ERROR);

	/* populate the hash table */

	retVal = motFecHashTblPopulate (pDrvCtrl);

	/* restart the FEC */

	if (motFecStart (pDrvCtrl) != OK)
	    return (ERROR);
	}

    return ((retVal == OK) ? OK : ERROR);
    }

/*******************************************************************************
*
* motFecMCastAddrGet - get the current multicast address list
*
* This routine returns the current multicast address list in <pTable>
*
* SEE ALSO: motFecMCastAddrAdd() motFecMCastAddrDel()
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS motFecMCastAddrGet
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    MULTI_TABLE *pTable		/* table into which to copy addresses */
    )
    {
    MOT_FEC_LOG (MOT_FEC_DBG_IOCTL, ("motFecMCastAddrGet\n"),
				     0, 0, 0, 0, 0, 0);

    return (etherMultiGet (&pDrvCtrl->endObj.multiList, pTable));
    }

/*******************************************************************************
*
* motFecHashTblAdd - add an entry to the hash table
*
* This routine adds an entry to the hash table for the address pointed to
* by <pAddr>.
*
* RETURNS: OK, or ERROR.
*/

LOCAL STATUS motFecHashTblAdd
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    char *      pAddr		/* address to be added */
    )
    {
    UINT32	crcVal;		/* convenient holder for the CRC value */
    UINT32	csrOldVal;	/* current value in the hash register */
    UINT16	offset;		/* offset into the Internal RAM */
    UINT32 	csrVal;		/* value to be written to the hash register */

    MOT_FEC_LOG (MOT_FEC_DBG_IOCTL, ("motFecHashTblAdd addr = 0x%x 0x%x\n\
				     0x%x 0x%x 0x%x 0x%x\n"),
				     (int) (*pAddr + 0), (int) (*pAddr + 1),
				     (int) (*pAddr + 2), (int) (*pAddr + 3),
				     (int) (*pAddr + 4), (int) (*pAddr + 5));

    /* get the CRC for the given address */

    crcVal = motFecCrcGet (pAddr);

    /* get the value to be written to the proper hash register */

    if (motFecHashRegValGet (pDrvCtrl, crcVal, &csrVal, &offset) != OK)
	return (ERROR);

    MOT_FEC_LOG (MOT_FEC_DBG_IOCTL, ("motFecHashTblAdd hashReg=0x%x\n"),
				     (int) csrVal,
				     0, 0, 0, 0, 0);

    /*
     * write to the proper hash register: be careful not
     * to override the current value.
     */

    MOT_FEC_CSR_RD (offset, csrOldVal);
    MOT_FEC_CSR_WR (offset, (csrOldVal | csrVal));

    return (OK);
    }

/*******************************************************************************
*
* motFecHashTblPopulate - populate the hash table
*
* This routine populates the hash table with the entries found in the
* multicast table. We have to reset the hash registers first, and
* populate them again, as more than one address may be mapped to
* the same bit.
*
* RETURNS: OK, or ERROR.
*/

LOCAL STATUS motFecHashTblPopulate
    (
    DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    UINT32	crcVal;		/* convenient holder for the CRC value */
    UINT16	offset;		/* offset into the Internal RAM */
    UINT32 	csrVal;		/* value to be written to the hash register */
    UINT32	csrOldVal;	/* current value in the hash register */
    ETHER_MULTI	*	mCastNode = NULL;

    /* reset the hash table registers first */

    MOT_FEC_CSR_WR (MOT_FEC_HASH_H_OFF, 0);
    MOT_FEC_CSR_WR (MOT_FEC_HASH_L_OFF, 0);

    /* scan the multicast address list */

    for (mCastNode = (ETHER_MULTI *) lstFirst (&pDrvCtrl->endObj.multiList);
	 mCastNode != NULL;
	 mCastNode = (ETHER_MULTI *) lstNext (&mCastNode->node))
	{
	/* get the CRC for the current address in the list */

	crcVal = motFecCrcGet ((char *) mCastNode->addr);

	/* get the value to be written to the proper hash register */

	if (motFecHashRegValGet (pDrvCtrl, crcVal, &csrVal, &offset) != OK)
            {
            return (ERROR);
            }

	/*
	 * write to the proper hash register: be careful not
	 * to override the current value.
	 */

	MOT_FEC_CSR_RD (offset, csrOldVal);
	MOT_FEC_CSR_WR (offset, (csrOldVal | csrVal));
	}

    return (OK);
    }

/*******************************************************************************
*
* motFecCrcGet - compute the cyclic redoundant code
*
* This routine computes the 32-bit cyclic redoundant code (CRC) for the
* 6-byte array pointed to by <pAddr>. For details on the polynomium used,
* see:
* .I "MPC860T Fast Ethernet Controller (Supplement to MPC860 User's Manual)"
*
* RETURNS: The 32-bit value representing the CRC.
*/

LOCAL UINT32 motFecCrcGet
    (
    char * pAddr
    )
    {
    UINT32 crc = INIT_REFLECTED;
    UINT32 len = 6;

    while (len--)
        crc = crctable[(crc ^ *pAddr++) & 0xFFL] ^ (crc >> 8);

    return crc ^ XOROT;
    }


/*******************************************************************************
*
* motFecHashRegValGet - get the value to be written to the hash register
*
* This routine computes the value to be written to the hash register
* for the 4-byte value specified by <crcVal>. It also figures out which
* of the two 32-bit hash register that value should be written to, and
* returns its offset into the internal RAM in the variable pointed to
* by <offset>.
*
* RETURNS: OK, always.
*/

LOCAL STATUS motFecHashRegValGet
    (
    DRV_CTRL *  pDrvCtrl,	/* pointer to DRV_CTRL structure */
    UINT32	crcVal,		/* CRC value */
    UINT32 *	csrVal,		/* value to be written to the hash register */
    UINT16 *	offset		/* offset into the Internal RAM */
    )
    {
    UINT32	hashIndex;	/* index into the hash register */

    /*
     * bits 26-30 in the CRC value determine the index in the
     * hash register specified by bit 31.
     */

    hashIndex = ((crcVal & MOT_FEC_HASH_MASK) >> MOT_FEC_HASH_SHIFT);

    *csrVal = (0x80000000 >> hashIndex);

    /* It appears that starting at register 0x0E08 HASH_TABLE_HIGH high
     * order bit is really bit 0 of the hash table and low order bit
     * of 0x0E0C HASH_TABLE_LOW is bit 63 of the hash table.
     *
     * 0x0E0C  HASH_TABLE_LOW  [32:64]   ==>   hash_index[63:32]
     * 0x0E08  HASH_TABLE_HIGH [0 :31]   ==>   hash_index[31: 0]
     *
     * So this means that bit 31 of the CRC if 1 points to
     * the HASH_TABLE_LOW register, not the HASH_TABLE_HIGH.
     */

    *offset = (crcVal & 0x80000000) ? MOT_FEC_HASH_L_OFF : MOT_FEC_HASH_H_OFF;

    return (OK);
    }

/*****************************************************************************
*
* motFecPollSend - transmit a packet in polled mode
*
* This routine is called by a user to try and send a packet on the
* device. It sends a packet directly on the network, without having to
* go through the normal process of queuing a packet on an output queue
* and then waiting for the device to decide to transmit it.
*
* These routine should not call any kernel functions.
*
* SEE ALSO: motFecPollReceive()
*
* RETURNS: OK or EAGAIN.
*/

LOCAL STATUS motFecPollSend
    (
    DRV_CTRL    *pDrvCtrl,	/* pointer to DRV_CTRL structure */
    M_BLK_ID    pMblk		/* pointer to the mBlk/cluster pair */
    )
    {
    UINT16		pktType = 0;	/* packet type (unicast or multicast) */
    int			retVal;		/* holder for return value */
    char *		pBuf = NULL;	/* pointer to data to be sent */
    MOT_FEC_TBD_ID	pTbd = NULL;	/* pointer to the current ready TBD */
    int 		ix = 0;		/* a counter */
    int			len = 0;	/* length of data to be sent */
    UINT16		tbdStatus;	/* holder for the TBD status */

    MOT_FEC_LOG (MOT_FEC_DBG_POLL, ("motFecPollSend\n"), 0, 0, 0, 0, 0, 0);

    if (pMblk == NULL)
	{
	goto motFecPollSendError;
	}

    /* set the packet type to multicast or unicast */

    if (pMblk->mBlkHdr.mData[0] & (UINT8) 0x01)
        {
        pktType = PKT_TYPE_MULTI;
        }
    else
        {
        pktType = PKT_TYPE_UNI;
        }

    /* get the current free TBD */

    pTbd = motFecTbdGet (pDrvCtrl);

    /* get our own cluster */

    pBuf = (char *)pDrvCtrl->pTxPollBuf;

    if ((pTbd == NULL) || (pBuf == NULL))
	{
	goto motFecPollSendError;
	}

    /* copy data but do not free the Mblk */

    len = netMblkToBufCopy (pMblk, pBuf, NULL);
    len = max (ETHERSMALL, len);

    /* flush the cache, if necessary */

    MOT_FEC_CACHE_FLUSH (pBuf, len);

    /* set up the current TBD */

    MOT_FEC_BD_LONG_WR (pTbd, MOT_FEC_BD_ADDR_OFF, (UINT32) pBuf);

    MOT_FEC_BD_WORD_WR (pTbd, MOT_FEC_BD_LEN_OFF, (UINT32) len);

    MOT_FEC_BD_WORD_RD (pTbd, MOT_FEC_BD_STAT_OFF, tbdStatus);

    MOT_FEC_BD_WORD_WR (pTbd, MOT_FEC_BD_STAT_OFF,
			(MOT_FEC_TBD_LAST | MOT_FEC_TBD_CRC
			| MOT_FEC_TBD_RDY | tbdStatus));

    /* kick the transmitter */

    MOT_FEC_TX_ACTIVATE;

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* up-date statistics */

    if (pktType == PKT_TYPE_MULTI)
        {
        pDrvCtrl->endObj.mib2Tbl.ifOutNUcastPkts += 1;
        }
    else
        {
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);
        }

    do
	{
	retVal = motFecTbdCheck (pDrvCtrl, pTbd);

	if (ix++ == MOT_FEC_WAIT_MAX)
	    break;

	} while (retVal == MOT_FEC_TBD_BUSY);

    /* correct statistics, if necessary */

    if ((retVal == MOT_FEC_TBD_ERROR) || (ix == MOT_FEC_WAIT_MAX))
	{
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);

	if (pktType == PKT_TYPE_MULTI)
	    {
	    pDrvCtrl->endObj.mib2Tbl.ifOutNUcastPkts -= 1;
	    }
	else
	    {
	    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, -1);
	    }
	}

    /* clean this buffer descriptor, mirror motFecTbdInit() */

    if (pDrvCtrl->tbdIndex == (pDrvCtrl->tbdNum - 1))
	{
	MOT_FEC_BD_WORD_WR (pTbd, MOT_FEC_BD_STAT_OFF,
			    MOT_FEC_TBD_WRAP);
	}
    else
	{
	MOT_FEC_BD_WORD_WR (pTbd, MOT_FEC_BD_STAT_OFF, 0);
	}

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* update some indeces for a correct handling of the TBD ring */

    pDrvCtrl->tbdIndex = (pDrvCtrl->tbdIndex + 1)
			  % (pDrvCtrl->tbdNum);

    pDrvCtrl->usedTbdIndex = (pDrvCtrl->usedTbdIndex + 1)
			      % (pDrvCtrl->tbdNum);

    if (ix == MOT_FEC_WAIT_MAX)
	return (EAGAIN);

    return (OK);

motFecPollSendError:

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);

    return (EAGAIN);
    }

/*******************************************************************************
*
* motFecPollReceive - receive a packet in polled mode
*
* This routine is called by a user to try and get a packet from the
* device. It returns EAGAIN if no packet is available. The caller must
* supply a M_BLK_ID with enough space to contain the received packet. If
* enough buffer is not available then EAGAIN is returned.
*
* These routine should not call any kernel functions.
*
* SEE ALSO: motFecPollSend()
*
* RETURNS: OK or EAGAIN.
*/

LOCAL STATUS motFecPollReceive
    (
    DRV_CTRL    *pDrvCtrl,      /* pointer to DRV_CTRL structure */
    M_BLK_ID    pMblk           /* pointer to the mBlk/cluster pair */
    )
    {
    int			retVal = OK;	/* holder for return value */
    MOT_FEC_RBD_ID	pRbd = NULL;	/* generic rbd pointer */
    UINT16		rbdStatus = 0;	/* holder for the status of this RBD */
    UINT16		rbdLen = 0;	/* number of bytes received */
    char *		pBuf = NULL;	/* a rx data pointer */
    char *		pData = NULL;	/* a rx data pointer */

    MOT_FEC_LOG (MOT_FEC_DBG_POLL, ("motFecPollReceive\n"), 0, 0, 0, 0, 0, 0);

    if ((pMblk->mBlkHdr.mFlags & M_EXT) != M_EXT)
        return (EAGAIN);

    /* get the first available RBD */

    MOT_FEC_NEXT_RBD (pDrvCtrl, pRbd);

    /* Make cache consistent with memory */

    MOT_FEC_BD_CACHE_INVAL (pRbd, MOT_FEC_RBD_SZ);
					
    /* read the RBD status word */		

    MOT_FEC_BD_WORD_RD (pRbd, MOT_FEC_BD_STAT_OFF,
			rbdStatus);

    /* if it's not ready, don't touch it! */
	
    if ((rbdStatus & MOT_FEC_RBD_EMPTY) == MOT_FEC_RBD_EMPTY)
	{
	return (EAGAIN);
	}

    /* pass the packet up only if reception was Ok */

    if (rbdStatus & MOT_FEC_RBD_ERR)
	{
	MOT_FEC_LOG (MOT_FEC_DBG_POLL, ("motFecReceive: bad rbd\n"),
				        1, 2, 3, 4, 5, 6);
	goto motFecPollReceiveEnd;
	}

    /* get the actual amount of received data */

    MOT_FEC_BD_WORD_RD (pRbd, MOT_FEC_BD_LEN_OFF,
			rbdLen);

    if (rbdLen < ETHERSMALL)
	{
	MOT_FEC_RX_LS_ADD;

	goto motFecPollReceiveEnd;
	}

    /*
     * Upper layer provides the buffer. If buffer is not large enough,
     * we do not copy the received buffer.
     */

    if (pMblk->mBlkHdr.mLen < rbdLen)
	{
	goto motFecPollReceiveEnd;
	}

    MOT_FEC_BD_LONG_RD (pRbd, MOT_FEC_BD_ADDR_OFF, pData);

    /* up-date statistics */

    if ((*pData ) & (UINT8) 0x01)
        {
        pDrvCtrl->endObj.mib2Tbl.ifInNUcastPkts += 1;
        }
    else
        {
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);
        }

    /* set up the mBlk properly */

    pMblk->mBlkHdr.mFlags   |= M_PKTHDR;
    pMblk->mBlkHdr.mLen = (rbdLen - ETHER_CRC_LEN) & ~0xc000;
    pMblk->mBlkPktHdr.len   = pMblk->mBlkHdr.mLen;

    /* Make cache consistent with memory */

    MOT_FEC_CACHE_INVAL ((char *) pData, pMblk->mBlkHdr.mLen);

    bcopy ((char *) pData, (char *) pMblk->mBlkHdr.mData,
	   ((rbdLen - ETHER_CRC_LEN) & ~0xc000));

    /* put the used RBD on the RBD queue */

    motFecRbdClean (pDrvCtrl, pBuf);

    return (retVal);

motFecPollReceiveEnd:

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);

    /* put the errored RBD on the RBD queue */

    motFecRbdClean (pDrvCtrl, pBuf);

    return (EAGAIN);
    }

/*******************************************************************************
*
* motFecPollStart - start polling mode
*
* This routine starts polling mode by disabling ethernet interrupts and
* setting the polling flag in the END_CTRL stucture.
*
* SEE ALSO: motFecPollStop()
*
* RETURNS: OK, or ERROR if polling mode could not be started.
*/

LOCAL STATUS motFecPollStart
    (
    DRV_CTRL    *pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    int         intLevel;	/* current intr level */
    int		retVal;		/* convenient holder for return value */

    MOT_FEC_LOG (MOT_FEC_DBG_POLL, ("motFecPollStart\n"), 0, 0, 0, 0, 0, 0);

    intLevel = intLock();

    /* disable system interrupt: reset relevant bit in SIMASK */

    SYS_FEC_INT_DISABLE (pDrvCtrl, retVal);
    if (retVal == ERROR)
	return (ERROR);

    /* mask chip interrupts   */

    MOT_FEC_INT_DISABLE;	

    MOT_FEC_FLAG_SET (MOT_FEC_POLLING);

    intUnlock (intLevel);

    return (OK);
    }

/*******************************************************************************
*
* motFecPollStop - stop polling mode
*
* This routine stops polling mode by enabling ethernet interrupts and
* resetting the polling flag in the END_CTRL structure.
*
* SEE ALSO: motFecPollStart()
*
* RETURNS: OK, or ERROR if polling mode could not be stopped.
*/

LOCAL STATUS motFecPollStop
    (
    DRV_CTRL    *pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    int         intLevel;
    int		retVal;		/* convenient holder for return value */

    intLevel = intLock();

    /* enable system interrupt: set relevant bit in SIMASK */

    SYS_FEC_INT_ENABLE (pDrvCtrl, retVal);
    if (retVal == ERROR)
	return (ERROR);

    /* enable chip interrupts */

    MOT_FEC_INT_ENABLE;	

    /* set flags */

    MOT_FEC_FLAG_CLEAR (MOT_FEC_POLLING);

    intUnlock (intLevel);

    MOT_FEC_LOG (MOT_FEC_DBG_POLL, ("motFecPollStop... end\n"),
				    0, 0, 0, 0, 0, 0);

    return (OK);
    }

#ifdef MOT_FEC_DBG
void motFecCsrShow
    (
    )
    {
    UINT32	csrVal;
    DRV_CTRL *	pDrvCtrl = pDrvCtrlDbg;

    MOT_FEC_CSR_RD (MOT_FEC_ADDR_L_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Addr low:      0x%x\n"),
				      csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_ADDR_H_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Addr high:      0x%x\n"),
				      csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_HASH_H_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Hash table high:      0x%x\n"),
				      csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_HASH_L_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Hash table low:      0x%x\n"),
				      csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_RX_START_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Rx ring start:      0x%x\n"),
				      csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_TX_START_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Tx ring start:      0x%x\n"),
				      csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_RX_BUF_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Rx max buffer:      0x%x\n"),
				      csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_CTRL_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Ethernet Controller:      0x%x\n"),
				   (csrVal & MOT_FEC_CTRL_MASK),
				   0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_EVENT_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Interrupt event:      0x%x\n"),
				   (csrVal & MOT_FEC_EVENT_MSK),
				   0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_MASK_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Interrupt mask:      0x%x\n"),
				   (csrVal & MOT_FEC_EVENT_MSK),
				   0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_VEC_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Interrupt vector/level:      0x%x\n"),
				   (csrVal & MOT_FEC_VEC_MSK),
				   0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_RX_ACT_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Rx active:      0x%x\n"),
				   csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_TX_ACT_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Tx active:      0x%x\n"),
				   csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_MII_DATA_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("MII data :      0x%x\n"),
				   csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_MII_SPEED_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("MII speed :      0x%x\n"),
				   csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_RX_BOUND_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Rx FIFO bound:      0x%x\n"),
				   csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_RX_FIFO_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Rx FIFO base:      0x%x\n"),
				   csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_TX_FIFO_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Tx FIFO base:      0x%x\n"),
				   csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_SDMA_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("SDMA Function Code:      0x%x\n"),
				   csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_RX_CTRL_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Rx Control Register:      0x%x\n"),
				   csrVal, 0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_RX_FR_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Rx Max frame length:      0x%x\n"),
				   (csrVal & MOT_FEC_RX_FR_MSK),
				   0, 0, 0, 0, 0);

    MOT_FEC_CSR_RD (MOT_FEC_TX_CTRL_OFF, csrVal);
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("Tx Control Register:      0x%x\n"),
				   csrVal, 0, 0, 0, 0, 0);
    }

void motFecRbdShow
    (
    int	rbdNum
    )
    {
    UINT16	status;
    UINT16	length;
    UINT32	buffer;

    MOT_FEC_WORD_RD ((UINT32 *)(pDrvCtrlDbg->rbdBase +
		     (rbdNum * MOT_FEC_RBD_SZ) +
		     MOT_FEC_BD_STAT_OFF), status);

    MOT_FEC_WORD_RD ((UINT32 *)(pDrvCtrlDbg->rbdBase +
		     (rbdNum * MOT_FEC_RBD_SZ) +
		     MOT_FEC_BD_LEN_OFF), length);

    MOT_FEC_LONG_RD ((UINT32 *)(pDrvCtrlDbg->rbdBase +
		     (rbdNum * MOT_FEC_RBD_SZ) +
		     MOT_FEC_BD_ADDR_OFF), buffer);

    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("rbdStatus=0x%x rbdLen=0x%x rbdBuf=0x%x\n"),
				   status, length, buffer, 0, 0, 0);
    }

void motFecErrorShow
    (
    )
    {
    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("TxBab=0x%x RxBab=0x%x\n"),
				   motFecBabTxErr,
				   motFecBabRxErr,
				   0, 0, 0, 0);

    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("TxLc=0x%x TxUr=0x%x TxCsl=0x%x \n\
				   TxRl=0x%x\n"),
				   motFecTxLcErr,
				   motFecTxUrErr,
				   motFecTxCslErr,
				   motFecTxRlErr,
				   0, 0);

    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("RxLg=0x%x RxNo=0x%x RxCrc=0x%x \n\
				   RxOv=0x%x RxTr=0x%x\n"),
				   motFecRxLgErr,
				   motFecRxNoErr,
				   motFecRxCrcErr,
				   motFecRxOvErr,
				   motFecRxTrErr,
				   0);

    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("RxLs=0x%x RxMem=0x%x\n"),
				   motFecRxLsErr,
				   motFecRxMemErr,
				   0, 0, 0, 0);
    }

void motFecDrvShow
    (
    )
    {
    DRV_CTRL *	pDrvCtrl = pDrvCtrlDbg;

    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("pDrvCtrl=0x%x pNetPool=0x%x \n\
				   pClPool=0x%x\n"),
				   (int) pDrvCtrl,
				   (int) pDrvCtrl->endObj.pNetPool,
				   (int) pDrvCtrl->pClPoolId, 0, 0, 0);

    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("bufBase=0x%x bufSize=0x%x pClBlkArea=0x%x\n\
				   clBlkSize=0x%x pMBlkArea=0x%x \n\
				   mBlkSize=0x%x\n"),
				   (int) pDrvCtrl->pBufBase,
				   pDrvCtrl->bufSize,
				   (int) pDrvCtrl->pClBlkArea,
				   pDrvCtrl->clBlkSize,
				   (int) pDrvCtrl->pMBlkArea,
				   pDrvCtrl->mBlkSize);

    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("tbdNum=%d tbdBase=0x%xtbdIndex=%d \n\
				   usedTbdIndex=%d \n\
				   cleanTbdNum=%d txStall=%d \n"),
				   pDrvCtrl->tbdNum,
				   (int) pDrvCtrl->tbdBase,
				   pDrvCtrl->tbdIndex,
				   pDrvCtrl->usedTbdIndex,
				   pDrvCtrl->cleanTbdNum,
				   pDrvCtrl->txStall);

    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("rbdNum=%d rbdBase=0x%x rbdIndex=%d \n"),
				   pDrvCtrl->rbdNum,
				   (int) pDrvCtrl->rbdBase,
				   pDrvCtrl->rbdIndex,
				   0, 0, 0);
    }

void motFecMiiShow
    (
    )
    {
    DRV_CTRL *  pDrvCtrl = pDrvCtrlDbg;
    UCHAR       speed [20];
    UCHAR       mode [20];

    strcpy ((char *) speed, (pDrvCtrl->phyInfo->phySpeed == MOT_FEC_100MBS) ?
                    "100Mbit/s" : "10Mbit/s");
    strcpy ((char *) mode, (pDrvCtrl->phyInfo->phyMode == MOT_FEC_PHY_FD) ?
                   "full duplex" : "half duplex");

    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("phySpeed=%s phyMode=%s phyAddr=0x%x\n\
                                   isoPhyAddr=0x%x phyFlags=0x%x\n\
                                   phyDefMode=0x%x\n"),
                                   (int) &speed[0],
                                   (int) &mode[0],
                                   pDrvCtrl->phyInfo->phyAddr,
                                   pDrvCtrl->phyInfo->isoPhyAddr,
                                   pDrvCtrl->phyInfo->phyFlags,
                                   pDrvCtrl->phyInfo->phyDefMode);

    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("phyStatus=0x%x phyCtrl=0x%x phyAds=0x%x\n\
                                   phyPrtn=0x%x phyExp=0x%x phyNext=0x%x\n"),
                                   pDrvCtrl->phyInfo->miiRegs.phyStatus,
                                   pDrvCtrl->phyInfo->miiRegs.phyCtrl,
                                   pDrvCtrl->phyInfo->miiRegs.phyAds,
                                   pDrvCtrl->phyInfo->miiRegs.phyPrtn,
                                   pDrvCtrl->phyInfo->miiRegs.phyExp,
                                   pDrvCtrl->phyInfo->miiRegs.phyNext);
    }
void motFecMibShow
    (
    )
    {
    DRV_CTRL *  pDrvCtrl = pDrvCtrlDbg;

    MOT_FEC_LOG (MOT_FEC_DBG_ANY, ("ifOutNUcastPkts=%d ifOutUcastPkts=%d\n\
                                   ifInNUcastPkts=%d\n\
                                   ifInUcastPkts=%d ifOutErrors=%d\n"),
                                   pDrvCtrl->endObj.mib2Tbl.ifOutNUcastPkts,
                                   pDrvCtrl->endObj.mib2Tbl.ifOutUcastPkts,
                                   pDrvCtrl->endObj.mib2Tbl.ifInNUcastPkts,
                                   pDrvCtrl->endObj.mib2Tbl.ifInUcastPkts,
                                   pDrvCtrl->endObj.mib2Tbl.ifOutErrors,
                                   0);

    }
#endif /* MOT_FEC_DBG */

/*******************************************************************************
*
* motRevNumGet - Get the processor revision number
*
* This routine will return the lower 16 bits of the IMMR (SPR #638)
* which will contain the processor revision number.
#
* RETURNS: processor revision number (in r3)
*
* NOMANUAL
*/

LOCAL int motRevNumGet(void)
    {

    int rev;
    rev = vxImmrDevGet();
    return(rev);
    }
