/* motFccEnd.c - END style Motorola FCC Ethernet network interface driver */

/* Copyright 1989-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01n,07dec01,rcs  added muxError() call to alert the mux that the driver has 
                 exhausted its clusters.  SPR# 70254 and  SPR# 30004
01m,30nov01,rcs  replaced motFccEnd.c with m8260FccEnd.c and added 
                 motFccJobQInit(), motFccJobQAdd(), and motFccJobQueue() 
01l,26nov01,rcs  created _func_m82xxDpramLib.c from target/src/mem  
01k,09nov01,rcs  made load string parameters mblkMult and clMult optional  
01j,07nov01,rcs  formatted code to conform to WRS coding conventions.
01i,30oct01,g_h  Removing in-line assembly.
01h,12oct01,g_h  Change inline assembly macro
01g,17sep01,g_h  add support to use without DPRAM lib
01f,13sep01,gev  fix SPR 70254
01e,17jul01,g_h  change to use the DPRAM lib.
01d,10may01,g_h  rename to m8260FccEnd.c
01c,17apr01,g_h  in motFccInitMem() change the memory allocation from memalign() to
                 cacheDmaMalloc() 
01b,08mar01,g_h  modified to run from ROM
01a,18jan01,g_h  written from motFecEnd.c, 01c.
*/

/*
DESCRIPTION
This module implements a Motorola Fast Communication Controller (FCC) 
Ethernet network interface driver. The FCC supports several communication
protocols, and when progammed to operate in Ethernet mode, it is fully 
compliant with the IEEE 802.3u 10Base-T and 100Base-T specifications. 

The FCC establishes a shared memory communication system with the CPU,
which may be divided into three parts: a set of Control/Status Registers (CSR)
and FCC-specific parameters, the buffer descriptors (BD), and the data buffers. 

Both the CSRs and the internal parameters reside in the MPC8260's internal 
RAM. They are used for mode control and to extract status information 
of a global nature. For instance, the types of events that should 
generate an interrupt, or features like the promiscous mode or the 
hearthbeat control may be set programming some of the CSRs properly. 
Pointers to both the Transmit Buffer Descriptors ring (TBD) and the
Receive Buffer Descriptors ring (RBD) are stored in the internal parameter 
RAM. The latter also includes protocol-specific parameters, like the 
individual physical address of this station or the max receive frame length.

The BDs are used to pass data buffers and related buffer information
between the hardware and the software. They may reside either on the 60x
bus, or on the CPM local bus They include local status information and a 
pointer to the incoming or outgoing data buffers. These are located again 
in external memory, and the user may chose whether this is on the 60x bus, 
or the CPM local bus (see below).

This driver is designed to be moderately generic. Without modification, it can
operate across all the FCCs in the MPC8260, regardless of where the internal 
memory base address is located. To achieve this goal, this driver must be 
given several target-specific parameters, and some external support routines 
must be provided.  These parameters, and the mechanisms used to communicate 
them to the driver, are detailed below.

This network interface driver does not include support for trailer protocols
or data chaining.  However, buffer loaning has been implemented in an effort
to boost performance. In addition, no copy is performed of the outgoing packet
before it is sent.
 
BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE

The driver provides the standard external interface, motFccEndLoad(), which
takes a string of colon-separated parameters. The parameters should be
specified in hexadecimal, optionally preceeded by "0x" or a minus sign "-".

The parameter string is parsed using strtok_r() and each parameter is
converted from a string representation to binary by a call to
strtoul(parameter, NULL, 16).

The format of the parameter string is:

"<immrVal>:<fccNum>:<bdBase>:<bdSize>:<bufBase>:<bufSize>:<fifoTxBase>:
<fifoRxBase> :<tbdNum>:<rbdNum>:<phyAddr>:<phyDefMode>:<userFlags>:
<mblkMult>:<clMult>:<txJobMsgQLen>"

TARGET-SPECIFIC PARAMETERS

.IP <immrVal>
Indicates the address at which the host processor presents its internal 
memory (also known as the internal RAM base address). With this address, 
and the fccNum (see below), the driver is able to compute the location of 
the FCC parameter RAM, and, ultimately, to program the FCC for proper 
operations. 

.IP <fccNum>
This driver is written to support multiple individual device units.
This parameter is used to explicitly state which FCC is being used (on the
vads8260 board, FCC2 is wired to the Fast Ethernet tranceiver, thus this 
parameter equals "2").

.IP <bdBase>
The Motorola Fast Communication Controller is a DMA-type device and typically 
shares access to some region of memory with the CPU. This driver is designed
for systems that directly share memory between the CPU and the FCC.

This parameter tells the driver that space for both the TBDs and the 
RBDs needs not be allocated but should be taken from a cache-coherent 
private memory space provided by the user at the given address. The user 
should be aware that memory used for buffers descriptors must be 8-byte 
aligned and non-cacheable. Therefore, the given memory space should allow
for all the buffer descriptors and the 8-byte alignment factor.

If this parameter is "NONE", space for buffer descriptors is obtained 
by calling cacheDmaMalloc() in motFccEndLoad().

.IP <bdSize>
The memory size parameter specifies the size of the pre-allocated memory
region for the BDs. If <bdBase> is specified as NONE (-1), the driver ignores 
this parameter. Otherwise, the driver checks the size of the provided memory 
region is adequate with respect to the given number of Transmit Buffer
Descriptors and Receive Buffer Descriptors.

.IP <bufBase>
This parameter tells the driver that space for data buffers
needs not be allocated but should be taken from a cache-coherent 
private memory space provided by the user at the given address. The user 
should be aware that memory used for buffers must be 32-byte 
aligned and non-cacheable. The FCC poses one more constraint in that DMA
cycles may initiate even when all the incoming data have already been 
transferred to memory. This means that at most 32 bytes of memory at the end of
each receive data buffer, may be overwritten during reception. The driver
pads that area out, thus consuming some additional memory.

If this parameter is "NONE", space for buffer descriptors is obtained 
by calling memalign() in motFccEndLoad().

.IP <bufSize>
The memory size parameter specifies the size of the pre-allocated memory
region for data buffers. If <bufBase> is specified as NONE (-1), the driver 
ignores this parameter. Otherwise, the driver checks the size of the provided 
memory region is adequate with respect to the given number of Receive Buffer 
Descriptors and a non-user-configurable number of trasmit buffers 
(MOT_FCC_TX_CL_NUM).  All the above should fit in the given memory space. 
This area should also include room for buffer management structures.

.IP <fifoTxBase>
Indicate the base location of the transmit FIFO, in internal memory.
The user does not need to initialize this parameter, as the default
value (see MOT_FCC_FIFO_TX_BASE) is highly optimized for best performance.
However, if the user wishes to reserve that very area in internal RAM for 
other purposes, he may set this parameter to a different value. 

If <fifoTxBase> is specified as NONE (-1), the driver uses the default
value.

.IP <fifoRxBase>
Indicate the base location of the receive FIFO, in internal memory.
The user does not need to initialize this parameter, as the default
value (see MOT_FCC_FIFO_TX_BASE) is highly optimized for best performance.
However, if the user wishes to reserve that very area in internal RAM for 
other purposes, he may set this parameter to a different value. 

If <fifoRxBase> is specified as NONE (-1), the driver uses the default
value.

.IP <tbdNum>
This parameter specifies the number of transmit buffer descriptors (TBDs). 
Each buffer descriptor resides in 8 bytes of the processor's external
RAM space, If this parameter is less than a minimum number specified in the 
macro MOT_FCC_TBD_MIN, or if it is "NULL", a default value of 64 (see
MOT_FCC_TBD_DEF_NUM) is used. This number is kept deliberately high, since 
each packet the driver sends may consume more than a single TBD. This 
parameter should always equal a even number.

.IP  <rbdNum>
This parameter specifies the number of receive buffer descriptors (RBDs). 
Each buffer descriptor resides in 8 bytes of the processor's external
RAM space, and each one points to a 1584-byte buffer again in external 
RAM. If this parameter is less than a minimum number specified in the 
macro MOT_FCC_RBD_MIN, or if it is "NULL", a default value of 32 (see
MOT_FCC_RBD_DEF_NUM) is used. This parameter should always equal a even number.

.IP  <phyAddr>
This parameter specifies the logical address of a MII-compliant physical
device (PHY) that is to be used as a physical media on the network.
Valid addresses are in the range 0-31. There may be more than one device
under the control of the same management interface. The default physical 
layer initialization routine will scan the whole range of PHY devices 
starting from the one in <phyAddr>. If this parameter is 
"MII_PHY_NULL", the default physical layer initialization routine will find 
out the PHY actual address by scanning the whole range. The one with the lowest
address will be chosen.

.IP  <phyDefMode>
This parameter specifies the operating mode that will be set up
by the default physical layer initialization routine in case all
the attempts made to establish a valid link failed. If that happens,
the first PHY that matches the specified abilities will be chosen to
work in that mode, and the physical link will not be tested.
 
.IP  <pAnOrderTbl>
This parameter may be set to the address of a table that specifies the 
order how different subsets of technology abilities should be advertised by
the auto-negotiation process, if enabled. Unless the flag <MOT_FCC_USR_PHY_TBL>
is set in the userFlags field of the load string, the driver ignores this
parameter.

The user does not normally need to specify this parameter, since the default
behaviour enables auto-negotiation process as described in IEEE 802.3u.

.IP  <userFlags>
This field enables the user to give some degree of customization to the
driver.

.IP <mblkMult>
Ratio between mBlk's and Rx BD's

.IP <clMult>
Ratio between Clusters and Rx BD's

.IP <txJobMsgQLen>
Length of the message queue from the ISR to motFccJobQueue()


MOT_FCC_USR_DPRAM_ALOC: This option allows multiple FCCs operating in the same 
system to share the Dual Ported RAM. It enables Dual Ported RAM allocation and 
freeing using the  utilities m82xxDpramFccMalloc, m82xxDpramFree, and 
m82xxDpramFccFree via the function pointers _func_m82xxDpramFccMalloc, 
_func_m82xxDpramFree, and _func_m82xxDpramFccFree which must be loaded by the 
BSP if this option is used.
 
MOT_FCC_USR_PHY_NO_AN: the default physical layer initialization
routine will exploit the auto-negotiation mechanism as described in
the IEEE Std 802.3u, to bring a valid link up. According to it, all
the link partners on the media will take part to the negotiation
process, and the highest priority common denominator technology ability
will be chosen. If the user wishes to prevent auto-negotiation from
occurring, he may set this bit in the user flags.
 
MOT_FCC_USR_PHY_TBL: in the auto-negotiation process, PHYs
advertise all their technology abilities at the same time,
and the result is that the maximum common denominator is used. However,
this behaviour may be changed, and the user may affect the order how
each subset of PHY's abilities is negotiated. Hence, when the
MOT_FCC_USR_PHY_TBL bit is set, the default physical layer
initialization routine will look at the motFccAnOrderTbl[] table and
auto-negotiate a subset of abilities at a time, as suggested by the
table itself. It is worth noticing here, however, that if the
MOT_FCC_USR_PHY_NO_AN bit is on, the above table will be ignored.
 
MOT_FCC_USR_PHY_NO_FD: the PHY may be set to operate in full duplex mode,
provided it has this ability, as a result of the negotiation with other
link partners. However, in this operating mode, the FCC will ignore the
collision detect and carrier sense signals. If the user wishes not to
negotiate full duplex mode, he should set the MOT_FCC_USR_PHY_NO_FD bit
in the user flags.
 
MOT_FCC_USR_PHY_NO_HD: the PHY may be set to operate in half duplex mode,
provided it has this ability, as a result of the negotiation with other link
partners. If the user wishes not to negotiate half duplex mode, he should
set the MOT_FCC_USR_PHY_NO_HD bit in the user flags.
 
MOT_FCC_USR_PHY_NO_100: the PHY may be set to operate at 100Mbit/s speed,
provided it has this ability, as a result of the negotiation with
other link partners. If the user wishes not to negotiate 100Mbit/s speed,
he should set the MOT_FCC_USR_PHY_NO_100 bit in the user flags.
 
MOT_FCC_USR_PHY_NO_10: the PHY may be set to operate at 10Mbit/s speed,
provided it has this ability, as a result of the negotiation with
other link partners. If the user wishes not to negotiate 10Mbit/s speed,
he should set the MOT_FCC_USR_PHY_NO_10 bit in the user flags.
 
MOT_FCC_USR_PHY_ISO: some boards may have different PHYs controlled by the
same management interface. In some cases, there may be the need of
electrically isolating some of them from the interface itself, in order
to guarantee a proper behaviour on the medium layer. If the user wishes to
electrically isolate all PHYs from the MII interface, he should set the
MOT_FCC_USR_PHY_ISO bit. The default behaviour is to not isolate any
PHY on the board.
 
MOT_FCC_USR_LOOP: when the MOT_FCC_USR_LOOP bit is set, the driver will
configure the FCC to work in internal loopback mode, with the TX signal 
directly connected to the RX. This mode should only be used for testing.
 
MOT_FCC_USR_RMON: when the MOT_FCC_USR_RMON bit is set, the driver will
configure the FCC to work in RMON mode, thus collecting network statistics
required for RMON support without the need to receive all packets as in
promiscous mode.
 
MOT_FCC_USR_BUF_LBUS: when the MOT_FCC_USR_BUF_LBUS bit is set, the driver will
configure the FCC to work as though the data buffers were located in the 
CPM local bus.

MOT_FCC_USR_BD_LBUS: when the MOT_FCC_USR_BD_LBUS bit is set, the driver will
configure the FCC to work as though the buffer descriptors were located in the 
CPM local bus.

MOT_FCC_USR_HBC: if the MOT_FCC_USR_HBC bit is set, the driver will
configure the FCC to perform heartbeat check following end of transmisson
and the HB bit in the status field of the TBD will be set if the collision
input does not assert within the heartbeat window (also see _func_motFccHbFail,
below). The user does not normally need to set this bit.
 
.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires several external support functions:
.IP sysFccEnetEnable()
.CS
    STATUS sysFccEnetEnable (UINT32 immrVal, UINT8 fccNum);
.CE
This routine is expected to handle any target-specific functions needed 
to enable the FCC. These functions typically include setting the Port B and C
on the MPC8260 so that the MII interface may be used. This routine is 
expected to return OK on success, or ERROR. The driver calls this routine, 
once per device, from the motFccStart () routine.
.IP sysFccEnetDisable()
.CS
    STATUS sysFccEnetDisable (UINT32 immrVal, UINT8 fccNum);
.CE
This routine is expected to perform any target specific functions required
to disable the MII interface to the FCC.  This involves restoring the 
default values for all the Port B and C signals. This routine is expected to 
return OK on success, or ERROR. The driver calls this routine from the 
motFccStop() routine each time a device is disabled.
.IP sysFccEnetAddrGet()
.CS
    STATUS sysFccEnetAddrGet (int unit,UCHAR *address);
.CE
The driver expects this routine to provide the six-byte Ethernet hardware 
address that is used by this device.  This routine must copy the six-byte 
address to the space provided by <enetAddr>.  This routine is expected to 
return OK on success, or ERROR.  The driver calls this routine, once per 
device, from the motFccEndLoad() routine.
.CS
    STATUS sysFccMiiBitWr (UINT32 immrVal, UINT8 fccNum, INT32 bitVal);
.CE
This routine is expected to perform any target specific functions required
to write a single bit value to the MII management interface of a MII-compliant
PHY device. The MII management interface is made up of two lines: management 
data clock (MDC) and management data input/output (MDIO). The former provides
the timing reference for transfer of information on the MDIO signal.
The latter is used to transfer control and status information between the
PHY and the FCC. For this transfer to be successful, the information itself 
has to be encoded into a frame format, and both the MDIO and MDC signals have
to comply with certain requirements as described in the 802.3u IEEE Standard.
There is not buil-in support in the FCC for the MII management interface.
This means that the clocking on the MDC line and the framing of the information
on the MDIO signal have to be done in software. Hence, this routine is 
expected to write the value in <bitVal> to the MDIO line while properly 
sourcing the MDC clock to a PHY, for one bit time.

.CS
    STATUS sysFccMiiBitRd (UINT32 immrVal, UINT8 fccNum, INT8 * bitVal);
.CE
This routine is expected to perform any target specific functions required
to read a single bit value from the MII management interface of a MII-compliant
PHY device. The MII management interface is made up of two lines: management 
data clock (MDC) and management data input/output (MDIO). The former provides
the timing reference for transfer of information on the MDIO signal.
The latter is used to transfer control and status information between the
PHY and the FCC. For this transfer to be successful, the information itself 
has to be encoded into a frame format, and both the MDIO and MDC signals have
to comply with certain requirements as described in the 802.3u IEEE Standard.
There is not buil-in support in the FCC for the MII management interface.
This means that the clocking on the MDC line and the framing of the information
on the MDIO signal have to be done in software. Hence, this routine is 
expected to read the value from the MDIO line in <bitVal>, while properly 
sourcing the MDC clock to a PHY, for one bit time.

.IP `_func_motFccPhyInit'
.CS
    FUNCPTR _func_motFccPhyInit
.CE
This driver sets the global variable `_func_motFccPhyInit' to the 
MII-compliant media initialization routine miiPhyInit(). If the user 
wishes to exploit a different way to configure the PHY, he may set
this variable to his own media initialization routine, tipically
in sysHwInit().
.IP `_func_motFccHbFail'
.CS
    FUNCPTR _func_motFccHbFail
.CE
The FCC may be configured to perform heartbeat check following end 
of transmission, and to report any failure in the relevant TBD status field. 
If this is the case, and if the global variable `_func_motFccHbFail'
is not NULL, the routine referenced to by `_func_motFccHbFail' is called,
with a pointer to the driver control structure as parameter. Hence,
the user may set this variable to his own heart beat check fail routine,
where he can take any action he sees appropriate.
The default value for the global variable `_func_motFccHbFail' is NULL.

.IP `_func_m82xxDpramFccMalloc'
.CS
    FUNCPTR _func_m82xxDpramFccMalloc
.CE

.IP `_func_m82xxDpramFree'
.CS
    FUNCPTR _func_m82xxDpramFree
.CE

.IP `_func_m82xxDpramFccFree'
.CS
    FUNCPTR _func_m82xxDpramFccFree
.CE

The FCC can be configured to utilize the dual ported ram located in
the MPPC8260 CMP. In this case the user flag MOT_FCC_USR_DPRAM_ALOC is set 
and the global variables _func_m82xxDpramFccMalloc, _func_m82xxDpramFree, and 
_func_m82xxDpramFccFree must be populated by the BSP with the FUNCPTRs to 
m82xxDpramFccMalloc(),  m82xxDpramFree(), and m82xxDpramFccFree() (respectively)
from m82xxDpramLib.h. These functions are then used by the motFccEnd driver to 
allocate and free memory in the dual ported ram. If any of these FUNCPTRs 
are left NULL the motFccPramInit() will return an ERROR and the motFccEnd
driver will not initialize.     

.LP

SYSTEM RESOURCE USAGE
If the driver allocates the memory for the BDs to share with the FCC,
it does so by calling the cacheDmaMalloc() routine.  For the default case
of 64 transmit buffers and 32 receive buffers, the total size requested 
is 776 bytes, and this includes the 8-byte alignment requirement of the 
device.  If a non-cacheable memory region is provided by the user, the 
size of this region should be this amount, unless the user has specified 
a different number of transmit or receive BDs. 

This driver can operate only if this memory region is non-cacheable
or if the hardware implements bus snooping.  The driver cannot maintain
cache coherency for the device because the BDs are asynchronously
modified by both the driver and the device, and these fields share 
the same cache line.

If the driver allocates the memory for the data buffers to share with the FCC,
it does so by calling the memalign () routine.  The driver does not need to
use cache-safe memory for data buffers, since the host CPU and the device are
not allowed to modify buffers asynchronously. The related cache lines
are flushed or invalidated as appropriate. For the default case
of 7 transmit clusters and 32 receive clusters, the total size requested 
for this memory region is 112751 bytes, and this includes the 32-byte 
alignment and the 32-byte pad-out area per buffer of the device.  If a 
non-cacheable memory region is provided by the user, the size of this region 
should be this amount, unless the user has specified a different number 
of transmit or receive BDs. 

TUNING HINTS

The only adjustable parameters are the number of TBDs and RBDs that will be
created at run-time.  These parameters are given to the driver when 
motFccEndLoad() is called.  There is one RBD associated with each received 
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

SEE ALSO: ifLib,
.I "MPC8260 Fast Ethernet Controller (Supplement to the MPC860 User's Manual)"
.I "Motorola MPC860 User's Manual",

INTERNAL
This driver contains conditional compilation switch MOT_FCC_DBG.
If defined, adds debug output routines.  Output is further
selectable at run-time via the motFccEndDbg global variable.
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
#include "errnoLib.h"
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
#include "miiLib.h"

#include "drv/intrCtl/m8260IntrCtl.h"
#include "drv/sio/m8260Cp.h"
#include "drv/sio/m8260CpmMux.h"
#include "drv/end/motFccEnd.h"

/* defines */

/* Driver debug control */

/* for debugging */

#ifdef MOT_FCC_DBG 
#undef MOT_FCC_DBG 
#endif

/* Driver debug control */

#ifdef MOT_FCC_DBG
#define MOT_FCC_DBG_OFF		0x0000
#define MOT_FCC_DBG_RX		0x0001
#define MOT_FCC_DBG_TX		0x0002
#define MOT_FCC_DBG_POLL	0x0004
#define MOT_FCC_DBG_MII		0x0008
#define MOT_FCC_DBG_LOAD	0x0010
#define MOT_FCC_DBG_IOCTL	0x0020
#define MOT_FCC_DBG_INT		0x0040
#define MOT_FCC_DBG_START	0x0080
#define MOT_FCC_DBG_INT_RX_ERR	0x0100
#define MOT_FCC_DBG_INT_TX_ERR	0x0200
#define MOT_FCC_DBG_ANY		0xffff

UINT32	motFccEndDbg = MOT_FCC_DBG_INT_RX_ERR|MOT_FCC_DBG_INT_TX_ERR;
UINT32	motFccRxBsyErr = 0x0;
UINT32	motFccTxErr = 0x0;
UINT32	motFccHbFailErr = 0x0;
UINT32	motFccTxLcErr = 0x0;
UINT32	motFccTxUrErr = 0x0;
UINT32	motFccTxCslErr = 0x0;
UINT32	motFccTxRlErr = 0x0;
UINT32	motFccRxLgErr = 0x0;
UINT32	motFccRxNoErr = 0x0;
UINT32	motFccRxCrcErr = 0x0;
UINT32	motFccRxOvErr = 0x0;
UINT32  motFccRxShErr = 0x0;
UINT32  motFccRxLcErr = 0x0;
UINT32  motFccRxMemErr = 0x0;

DRV_CTRL *  pDrvCtrlDbg = NULL;

#define MOT_FCC_HB_FAIL_ADD	motFccHbFailErr++;
#define MOT_FCC_RX_LG_ADD	motFccRxLgErr++;
#define MOT_FCC_RX_NO_ADD	motFccRxNoErr++;
#define MOT_FCC_RX_CRC_ADD	motFccRxCrcErr++;
#define MOT_FCC_RX_OV_ADD	motFccRxOvErr++;
#define MOT_FCC_RX_LC_ADD	motFccRxLcErr++;
#define MOT_FCC_RX_SH_ADD	motFccRxShErr++;
#define MOT_FCC_RX_MEM_ADD(p) (p)->Stats->motFccRxMemErr++;

#define MOT_FCC_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)			\
    {									\
    if (motFccEndDbg & FLG)						\
        logMsg (X0, X1, X2, X3, X4, X5, X6);			        \
    }

#else /* MOT_FCC_DBG */

#define MOT_FCC_HB_FAIL_ADD
#define MOT_FCC_RX_MEM_ADD(p)
#define MOT_FCC_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)

#endif /* MOT_FCC_DBG */

/* general macros for reading/writing from/to specified locations */

/* Cache and virtual/physical memory related macros */

#define MOT_FCC_PHYS_TO_VIRT(physAddr)					    \
	CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->bdCacheFuncs, (char *)(physAddr))

#define MOT_FCC_VIRT_TO_PHYS(virtAddr)					    \
	CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->bdCacheFuncs, (char *)(virtAddr))

#define MOT_FCC_BD_CACHE_INVAL(address, len)			    	    \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->bdCacheFuncs, (address), (len))

#define MOT_FCC_CACHE_INVAL(address, len)                                   \
	CACHE_DRV_INVALIDATE (&pDrvCtrl->bufCacheFuncs, (address), (len))

#define MOT_FCC_CACHE_FLUSH(address, len)                                   \
	CACHE_DRV_FLUSH (&pDrvCtrl->bufCacheFuncs, (address), (len))

/* driver flags */

#define MOT_FCC_OWN_BUF_MEM	0x01	/* internally provided memory for data*/
#define MOT_FCC_INV_TBD_NUM	0x02	/* invalid tbdNum provided */
#define MOT_FCC_INV_RBD_NUM	0x04	/* invalid rbdNum provided */
#define MOT_FCC_POLLING		0x08	/* polling mode */
#define MOT_FCC_PROM		0x20    /* promiscuous mode */
#define MOT_FCC_MCAST		0x40    /* multicast addressing mode */
#define MOT_FCC_FD		0x80    /* full duplex mode */
#define MOT_FCC_OWN_BD_MEM	0x10	/* internally provided memory for BDs */

/* shortcuts */

#define MOT_FCC_FLAG_CLEAR(clearBits)					\
    (pDrvCtrl->flags &= ~(clearBits))

#define MOT_FCC_FLAG_SET(setBits)					\
    (pDrvCtrl->flags |= (setBits))

#define MOT_FCC_FLAG_GET()						\
    (pDrvCtrl->flags)

#define MOT_FCC_FLAG_ISSET(setBits)					\
    (pDrvCtrl->flags & (setBits))

#define MOT_FCC_PHY_FLAGS_SET(setBits)					\
    (pDrvCtrl->phyInfo->phyFlags |= (setBits))

#define MOT_FCC_PHY_FLAGS_ISSET(setBits)				\
    (pDrvCtrl->phyInfo->phyFlags & (setBits))

#define MOT_FCC_PHY_FLAGS_GET(setBits)					\
    (pDrvCtrl->phyInfo->phyFlags)

#define MOT_FCC_PHY_FLAGS_CLEAR(clearBits)				\
    (pDrvCtrl->phyInfo->phyFlags &= ~(clearBits))

#define MOT_FCC_USR_FLAG_ISSET(setBits)					\
    (pDrvCtrl->userFlags & (setBits))

#define END_FLAGS_ISSET(setBits)					\
    ((&pDrvCtrl->endObj)->flags & (setBits))

#define MOT_FCC_ADDR_GET(pEnd)                                          \
    ((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define MOT_FCC_ADDR_LEN_GET(pEnd)                                      \
    ((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

/* some BDs definitions */

/* 
 * the total is 0x630 and it accounts for the required alignment
 * of receive data buffers, and the cluster overhead.
 */

#define XXX_FCC_MAX_CL_LEN      ((MII_ETH_MAX_PCK_SZ			\
				 + (MOT_FCC_BUF_ALIGN - 1)		\
				 + MOT_FCC_BUF_ALIGN			\
				 + (CL_OVERHEAD - 1))			\
				 & (~ (CL_OVERHEAD - 1)))

#define MOT_FCC_MAX_CL_LEN      ROUND_UP(XXX_FCC_MAX_CL_LEN,MOT_FCC_BUF_ALIGN)

#define MOT_FCC_RX_CL_SZ       (MOT_FCC_MAX_CL_LEN)
#define MOT_FCC_TX_CL_SZ       (MOT_FCC_MAX_CL_LEN)

/* read/write macros to access internal memory */

#define MOT_FCC_REG_LONG_WR(regAddr, regVal)				\
    {									\
    UINT32 temp = 0;							\
									\
    temp = (UINT32) MOT_FCC_VIRT_TO_PHYS (regAddr);			\
									\
    MOT_FCC_LONG_WR ((UINT32 *) (temp), (regVal));			\
    }									

#define MOT_FCC_REG_LONG_RD(regAddr, regVal)				\
    {									\
    UINT32 temp = 0;							\
									\
    temp = (UINT32) MOT_FCC_VIRT_TO_PHYS (regAddr);			\
									\
    MOT_FCC_LONG_RD ((UINT32 *) (temp), (regVal));			\
    }									

#define MOT_FCC_REG_WORD_WR(regAddr, regVal)				\
    {									\
    UINT32 temp = 0;							\
									\
    temp = (UINT32) MOT_FCC_VIRT_TO_PHYS (regAddr);			\
									\
    MOT_FCC_WORD_WR ((UINT16 *) (temp), (regVal));			\
    }									

#define MOT_FCC_REG_WORD_RD(regAddr, regVal)				\
    {									\
    UINT32 temp = 0;							\
									\
    temp = (UINT32) MOT_FCC_VIRT_TO_PHYS (regAddr);			\
									\
    MOT_FCC_WORD_RD ((UINT16 *) (temp), (regVal));			\
    }									

/* macros to read/write tx/rx buffer descriptors */

#define MOT_FCC_BD_WORD_WR(bdAddr, bdOff, bdVal)			\
    {									\
    UINT32 temp = 0;							\
									\
    temp = (UINT32) MOT_FCC_VIRT_TO_PHYS ((bdAddr) + (bdOff));		\
									\
    MOT_FCC_WORD_WR ((UINT16 *) (temp), (bdVal));			\
    }									

/* here we're writing the data pointer, so it's a physical address */

#define MOT_FCC_BD_LONG_WR(bdAddr, bdOff, bdVal)			\
    {									\
    UINT32 temp1 = 0;							\
    UINT32 temp2 = 0;							\
									\
    temp1 = (UINT32) MOT_FCC_VIRT_TO_PHYS ((bdAddr) + (bdOff));		\
    temp2 = (UINT32) MOT_FCC_VIRT_TO_PHYS (bdVal);			\
									\
    MOT_FCC_LONG_WR ((UINT32 *) (temp1), (temp2));			\
    }									

#define MOT_FCC_BD_WORD_RD(bdAddr, bdOff, bdVal)			\
    {									\
    UINT32 temp = 0;							\
									\
    temp = (UINT32) MOT_FCC_VIRT_TO_PHYS ((bdAddr) + (bdOff));		\
									\
    MOT_FCC_WORD_RD ((UINT16 *) (temp), (bdVal));			\
    }									

#define MOT_FCC_BD_LONG_RD(bdAddr, bdOff, bdVal)			\
    {									\
    UINT32 temp1 = 0;							\
    UINT32 temp2 = 0;							\
									\
    temp1 = (UINT32) MOT_FCC_VIRT_TO_PHYS ((bdAddr) + (bdOff));		\
									\
    MOT_FCC_LONG_RD ((UINT32 *) (temp1), (temp2));			\
									\
    bdVal = (char *) MOT_FCC_PHYS_TO_VIRT (temp2);			\
    }									

#define MOT_FCC_NEXT_TBD(pDrvCtrl, pTbd)				\
    (pTbd) = (MOT_FCC_TBD_ID) ((pDrvCtrl)->tbdBase +   			\
			      ((pDrvCtrl)->tbdIndex * MOT_FCC_TBD_SZ))

#define MOT_FCC_USED_TBD(pDrvCtrl, pTbd)				\
    (pTbd) = (MOT_FCC_TBD_ID) ((pDrvCtrl)->tbdBase + 	   		\
		              ((pDrvCtrl)->usedTbdIndex * MOT_FCC_TBD_SZ))

#define MOT_FCC_RX_ACTIVATE						\
    {									\
    UINT32 tmp = 0;							\
									\
    MOT_FCC_REG_LONG_RD (pDrvCtrl->fccIramAddr, tmp);			\
    MOT_FCC_REG_LONG_WR (pDrvCtrl->fccIramAddr,				\
			(tmp | M8260_GFMR_ENR));			\
    }

#define MOT_FCC_INT_DISABLE						\
    {									\
    MOT_FCC_REG_WORD_WR ((UINT32) M8260_FGMR1 (pDrvCtrl->immrVal) +	\
			 ((pDrvCtrl->fccNum - 1) * M8260_FCC_IRAM_GAP),	\
			 MOT_FCC_CLEAR_VAL);				\
    }

#define MOT_FCC_INT_ENABLE						\
    {									\
    MOT_FCC_REG_WORD_WR ((UINT32) M8260_FGMR1 (pDrvCtrl->immrVal) +	\
			 ((pDrvCtrl->fccNum - 1) * M8260_FCC_IRAM_GAP),	\
			 (pDrvCtrl->intMask));				\
    }

/* more shortcuts */

#define MOT_FCC_GRA_SEM_CREATE						\
    if ((pDrvCtrl->graSem = semBCreate (SEM_Q_FIFO, SEM_EMPTY)) 	\
	== NULL)							\
	goto errorExit

#define MOT_FCC_GRA_SEM_DELETE						\
    if ((pDrvCtrl->graSem) != NULL) 					\
	semDelete (pDrvCtrl->graSem)

#define MOT_FCC_GRA_SEM_GIVE						\
    (semGive (pDrvCtrl->graSem))

#define MOT_FCC_GRA_SEM_TAKE						\
    (semTake (pDrvCtrl->graSem, WAIT_FOREVER))

#define NET_BUF_ALLOC()                                                 \
    netClusterGet (pDrvCtrl->endObj.pNetPool, pDrvCtrl->pClPoolId)	\

#define NET_TO_MOT_FCC_BUF(netBuf)                              	\
    (((UINT32) (netBuf) + MOT_FCC_BUF_ALIGN - 1) 			\
      & ~(MOT_FCC_BUF_ALIGN - 1))

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
    (ret) = netClBlkJoin ((pClBlock), (pBuffer), (length), NULL, 0, 0, 0)
 
/* locals */

/* Function declarations not in any header files */

/* forward function declarations */

LOCAL STATUS    motFccInitParse (DRV_CTRL * pDrvCtrl, char *initString);
LOCAL STATUS    motFccInitMem (DRV_CTRL *pDrvCtrl);
LOCAL STATUS    motFccSend (DRV_CTRL *pDrvCtrl, M_BLK *pMblk);

LOCAL STATUS    motFccPhyPreInit (DRV_CTRL * pDrvCtrl);
LOCAL STATUS    motFccBdFree (DRV_CTRL * pDrvCtrl);
LOCAL STATUS    motFccRbdInit (DRV_CTRL * pDrvCtrl);
LOCAL STATUS	motFccTbdInit (DRV_CTRL * pDrvCtrl);
LOCAL STATUS	motFccFpsmrValSet (DRV_CTRL * pDrvCtrl);
LOCAL STATUS	motFccHashTblPopulate (DRV_CTRL * pDrvCtrl);
LOCAL STATUS	motFccHashTblAdd (DRV_CTRL * pDrvCtrl, UCHAR * pAddr);

LOCAL STATUS	motFccInit (DRV_CTRL * pDrvCtrl);
LOCAL STATUS	motFccPramInit (DRV_CTRL * pDrvCtrl);
LOCAL STATUS	motFccIramInit (DRV_CTRL * pDrvCtrl);
LOCAL STATUS	motFccCpcrCommand (DRV_CTRL * pDrvCtrl, UINT8 command);
LOCAL void	motFccInt (DRV_CTRL * pDrvCtrl);
LOCAL void	motFccHandleRXFrames(DRV_CTRL *pDrvCtrl);
LOCAL STATUS	motFccMiiRead (DRV_CTRL * pDrvCtrl, UINT8 phyAddr,
		               UINT8 regAddr, UINT16 *retVal);
LOCAL STATUS	motFccMiiWrite (DRV_CTRL * pDrvCtrl, UINT8 phyAddr,
				UINT8 regAddr, UINT16 writeData);
LOCAL STATUS	motFccAddrSet (DRV_CTRL * pDrvCtrl, UCHAR * pAddr,
			                   UINT32 offset);
LOCAL void	motFccPhyLSCInt(DRV_CTRL *pDrvCtrl);
LOCAL STATUS    motFccJobQInit (DRV_CTRL *pDrvCtrl); 
LOCAL void      motFccJobQAdd (DRV_CTRL * pDrvCtrl, FUNCPTR routine, int param1,
                               int param2, int param3, int param4, int param5);
LOCAL STATUS    motFccJobQueue (DRV_CTRL * pDrvCtrl);

/* END Specific interfaces. */

END_OBJ *	motFccEndLoad (char *initString);    
LOCAL STATUS    motFccStart (DRV_CTRL * pDrvCtrl);
LOCAL STATUS	motFccUnload (DRV_CTRL * pDrvCtrl);
LOCAL STATUS    motFccStop (DRV_CTRL * pDrvCtrl);
LOCAL int       motFccIoctl (DRV_CTRL * pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS    motFccSend (DRV_CTRL * pDrvCtrl, M_BLK_ID pMblk);
LOCAL STATUS    motFccMCastAddrAdd (DRV_CTRL * pDrvCtrl, UCHAR * pAddress);
LOCAL STATUS    motFccMCastAddrDel (DRV_CTRL * pDrvCtrl, UCHAR * pAddress);
LOCAL STATUS    motFccMCastAddrGet (DRV_CTRL * pDrvCtrl,
                                    MULTI_TABLE *pTable);
LOCAL STATUS    motFccPollSend (DRV_CTRL * pDrvCtrl, M_BLK_ID pMblk);
LOCAL STATUS    motFccPollReceive (DRV_CTRL * pDrvCtrl, M_BLK_ID pMblk);
LOCAL STATUS    motFccPollStart (DRV_CTRL * pDrvCtrl);
LOCAL STATUS    motFccPollStop (DRV_CTRL * pDrvCtrl);

/* globals */

FUNCPTR		_func_motFccPhyInit = (FUNCPTR) miiPhyInit;
FUNCPTR		_func_motFccPhyLSCInt = (FUNCPTR) motFccPhyLSCInt;
FUNCPTR		_func_motFccHbFail = (FUNCPTR) NULL;
FUNCPTR		_func_motFccPhyDuplex = (FUNCPTR) NULL;
FUNCPTR 	_func_m82xxDpramFree = (FUNCPTR)NULL;
FUNCPTR 	_func_m82xxDpramFccMalloc = (FUNCPTR)NULL;
FUNCPTR 	_func_m82xxDpramFccFree = (FUNCPTR)NULL;

DRV_CTRL	*global_pDrvCtrl;

#ifdef MOT_FCC_DBG
void motFccIramShow (void);
void motFccPramShow (void);
void motFccRbdShow (int);
void motFccTbdShow (int);
void motFccErrorShow (void);
void motFccDrvShow (void);
void motFccMiiShow (void);
void motFccMibShow (void);
UINT16 motFccMiiRegRead(UINT8 regAddr);


#endif /* MOT_FCC_DBG */

/* 
 * Define the device function table.  This is static across all driver
 * instances.
 */

LOCAL NET_FUNCS netFccFuncs = 
    {
    (FUNCPTR) motFccStart,		/* start func. */		 
    (FUNCPTR) motFccStop,		/* stop func. */
    (FUNCPTR) motFccUnload,		/* unload func. */		
    (FUNCPTR) motFccIoctl,		/* ioctl func. */		 
    (FUNCPTR) motFccSend,		/* send func. */		  
    (FUNCPTR) motFccMCastAddrAdd,    	/* multicast add func. */	 
    (FUNCPTR) motFccMCastAddrDel,    	/* multicast delete func. */      
    (FUNCPTR) motFccMCastAddrGet,    	/* multicast get fun. */	  
    (FUNCPTR) motFccPollSend,    	/* polling send func. */	  
    (FUNCPTR) motFccPollReceive,    	/* polling receive func. */
    endEtherAddressForm,   		/* put address info into a NET_BUFFER */
    (FUNCPTR) endEtherPacketDataGet, 	/* get pointer to data in NET_BUFFER */
    (FUNCPTR) endEtherPacketAddrGet  	/* Get packet addresses */
    };		

/*******************************************************************************
*
* motFccEndLoad - initialize the driver and device
*
* This routine initializes both driver and device to an operational state
* using device specific parameters specified by <initString>.
*
* The parameter string, <initString>, is an ordered list of parameters each
* separated by a colon. The format of <initString> is:
*
* "<immrVal>:<ivec>:<bufBase>:<bufSize>:<fifoTxBase>:<fifoRxBase>
* :<tbdNum>:<rbdNum>:<phyAddr>:<phyDefMode>:<pAnOrderTbl>:<userFlags>"
*
* The FCC shares a region of memory with the driver.  The caller of this
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
* .I "MPC8260 Power QUICC II User's Manual"
*/

END_OBJ* motFccEndLoad
    (
    char *initString
    )
    {
    DRV_CTRL	*pDrvCtrl = NULL;	     /* pointer to DRV_CTRL structure */
    UCHAR   	enetAddr[MOT_FCC_ADDR_LEN];  /* ethernet address */


    MOT_FCC_LOG (MOT_FCC_DBG_LOAD, ("Loading end...\n"), 1, 2, 3, 4, 5, 6);

    if (initString == NULL)
	return NULL;

    if (initString[0] == 0)
        {
	bcopy ((char *)MOT_FCC_DEV_NAME, (void *)initString,
               MOT_FCC_DEV_NAME_LEN);
	return NULL;
        }

    /* allocate the device structure */

    pDrvCtrl = (DRV_CTRL *) calloc (sizeof (DRV_CTRL), 1);
    if (pDrvCtrl == NULL)
	return NULL;

    global_pDrvCtrl = pDrvCtrl;

    /* get memory for the phyInfo structure */

    if ((pDrvCtrl->phyInfo = calloc (sizeof (PHY_INFO), 1)) == NULL)
	return NULL;

    /* get memory for the drivers stats structure */

    if ((pDrvCtrl->Stats = calloc (sizeof (FCC_DRIVER_STATS), 1)) == NULL)
	return NULL;

#ifdef MOT_FCC_DBG 
    pDrvCtrlDbg = pDrvCtrl;
#endif /* MOT_FCC_DBG */

    /* Parse InitString */

    if (motFccInitParse (pDrvCtrl, initString) == ERROR)
        {
	goto errorExit;	
        }

    /* sanity check the unit number */

    if (pDrvCtrl->unit < 0 )
	goto errorExit;    

    /* memory initialization */

    if (motFccInitMem (pDrvCtrl) == ERROR)
	goto errorExit;

    /* get our ethernet hardware address */

    SYS_FCC_ENET_ADDR_GET (enetAddr);

    MOT_FCC_LOG (MOT_FCC_DBG_LOAD, ("motFccEndLoad: enetAddr= 
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

    /* Set zeroBufFlag */
    if (pDrvCtrl->userFlags & MOT_FCC_USR_BUF_LBUS || 
        pDrvCtrl->userFlags & MOT_FCC_USR_NO_ZCOPY)
        {
	pDrvCtrl->zeroBufFlag = FALSE;
        }
    else
        {
	pDrvCtrl->zeroBufFlag = TRUE;
        }

    /* store the internal ram base address */

    pDrvCtrl->fccIramAddr = (UINT32) M8260_FGMR1 (pDrvCtrl->immrVal) +
			    ((pDrvCtrl->fccNum - 1) * M8260_FCC_IRAM_GAP);

    pDrvCtrl->fccReg = (FCC_REG_T *)pDrvCtrl->fccIramAddr;

    /* store the parameter ram base address */

    pDrvCtrl->fccPramAddr = (UINT32) M8260_FCC1_BASE (pDrvCtrl->immrVal) +
			    ((pDrvCtrl->fccNum - 1) * M8260_FCC_DPRAM_GAP);

    MOT_FCC_LOG (MOT_FCC_DBG_LOAD, ("motFccEndLoad: pram=0x%x iram=0x%x \n "), 
				    pDrvCtrl->fccPramAddr,
				    pDrvCtrl->fccIramAddr,
				    0, 0, 0, 0);

    pDrvCtrl->fccPar = (FCC_PARAM_T *)pDrvCtrl->fccPramAddr;
    pDrvCtrl->fccEthPar = &pDrvCtrl->fccPar->prot.e;

    /* 
     * create the synchronization semaphore for graceful transmit 
     * command interrupts. 
     */

    MOT_FCC_GRA_SEM_CREATE;
    
	/*
	 * Because we create EMPTY semaphore we need to give it here
	 * other wise the only time that it's given back is in the
	 * motFccInt() and if we have two NI in the bootrom like SCC
	 * and FCC the motFccStop() will be spin forever when it will
	 * try to do MOT_FCC_GRA_SEM_TAKE.                            
	 */

	MOT_FCC_GRA_SEM_GIVE;

    /* endObj initializations */

    if (END_OBJ_INIT (&pDrvCtrl->endObj, 
                      (DEV_OBJ*) pDrvCtrl,
		      MOT_FCC_DEV_NAME, 
                      pDrvCtrl->unit, &netFccFuncs,
		      "Motorola FCC Ethernet Enhanced Network Driver") == ERROR)
        {
	goto errorExit;
        }

    pDrvCtrl->phyInfo->phySpeed = MOT_FCC_100MBS;

    if (END_MIB_INIT (&pDrvCtrl->endObj, 
                      M2_ifType_ethernet_csmacd, 
		      (u_char *) &enetAddr[0], 
                      MOT_FCC_ADDR_LEN,
		      ETHERMTU, 
                      pDrvCtrl->phyInfo->phySpeed) == ERROR)
        {
	goto errorExit;
        }

    /* Mark the device ready */

    END_OBJ_READY (&pDrvCtrl->endObj, IFF_NOTRAILERS | IFF_MULTICAST | 
                   IFF_BROADCAST);

    MOT_FCC_LOG (MOT_FCC_DBG_LOAD, ("motFccEndLoad... Done \n"), 
				    1, 2, 3, 4, 5, 6);

    return (&pDrvCtrl->endObj);

errorExit:
    motFccUnload (pDrvCtrl);
    return NULL;
    }

/*******************************************************************************
*
* motFccUnload - unload a driver from the system
*
* This routine unloads the driver pointed to by <pDrvCtrl> from the system.
*
* RETURNS: OK, always.
*
* SEE ALSO: motFccLoad()
*/
LOCAL STATUS motFccUnload
    (
    DRV_CTRL	*pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    FCC_PARAM_T	*pParam;

    /* get to the beginning of the parameter area */

    pParam = pDrvCtrl->fccPar;

    MOT_FCC_LOG (MOT_FCC_DBG_LOAD, ("Unloading end..."), 1, 2, 3, 4, 5, 6);

    if (pDrvCtrl == NULL)
	return (ERROR);

    pDrvCtrl->loaded = FALSE;

    /* free allocated memory if necessary */

    if ((MOT_FCC_FLAG_ISSET (MOT_FCC_OWN_BUF_MEM)) && 
        (pDrvCtrl->pBufBase != NULL))
        {
	free (pDrvCtrl->pBufBase);
        }

    if ((MOT_FCC_FLAG_ISSET (MOT_FCC_OWN_BD_MEM)) && 
        (pDrvCtrl->pBdBase != NULL))
        {
	cacheDmaFree (pDrvCtrl->pBdBase);
        }
    else if ((MOT_FCC_USR_FLAG_ISSET(MOT_FCC_USR_DPRAM_ALOC)) && 
             (pDrvCtrl->pBdBase != NULL))
	{

	/* 
         * If pBdBase is not equal to NULL then it was allocated from the 
         * DPRAM pool 
         */

	_func_m82xxDpramFree((void*)pDrvCtrl->pBdBase);

	pDrvCtrl->pBdBase = NULL;
	}

    /* free allocated memory if necessary */

    if ((pDrvCtrl->pMBlkArea) != NULL)
	free (pDrvCtrl->pMBlkArea);

    END_OBJECT_UNLOAD (&pDrvCtrl->endObj);

    /* free the semaphores if necessary */

    MOT_FCC_GRA_SEM_DELETE;

    if ((char *) pDrvCtrl->phyInfo != NULL)
	cfree ((char *) pDrvCtrl->phyInfo);

    /* free allocated DPRAM memory if necessary */

    if ((MOT_FCC_USR_FLAG_ISSET(MOT_FCC_USR_DPRAM_ALOC)) && 
        (pParam->riptr != 0))
        {
        _func_m82xxDpramFccFree((void*)((ULONG)pParam->riptr));
        pParam->riptr = 0;
        }

    if ((MOT_FCC_USR_FLAG_ISSET(MOT_FCC_USR_DPRAM_ALOC)) && 
        (pParam->tiptr != 0))
	{
        _func_m82xxDpramFccFree((void*)((ULONG)pParam->tiptr));
        pParam->tiptr = 0;
	}

    MOT_FCC_LOG (MOT_FCC_DBG_LOAD, ("motFccUnload... Done\n"), 
				    1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*******************************************************************************
*
* motFccInitParse - parse parameter values from initString
*
* This routine parses parameter values from initString and stores them in
* the related fiels of the driver control structure.
*
* RETURNS: OK or ERROR
*/
LOCAL STATUS motFccInitParse
    (
    DRV_CTRL *pDrvCtrl,      		/* pointer to DRV_CTRL structure */
    char     *initString		/* parameter string */
    )
    {
    char *  tok;		/* an initString token */
    char *  holder = NULL;	/* points to initString fragment beyond tok */

    tok = strtok_r (initString, ":", &holder);
    if (tok == NULL)
        {
	return ERROR;
        }
    pDrvCtrl->unit = (int) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->immrVal = (UINT32) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->fccNum = (UINT32) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->pBdBase = (char *) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->bdSize = strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->pBufBase = (char *) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->bufSize = strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->fifoTxBase = (UINT32) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->fifoRxBase = (UINT32) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->tbdNum = (UINT16) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->rbdNum = (UINT16) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->phyInfo->phyAddr = (UINT8) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->phyInfo->phyDefMode = (UINT8) strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->phyInfo->phyAnOrderTbl = (MII_AN_ORDER_TBL *) 
					strtoul (tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        {
        return ERROR;
        }

    pDrvCtrl->userFlags = strtoul (tok, NULL, 16);

    /* mblkMult is optional */

    pDrvCtrl->mblkMult = 5;
    tok = strtok_r (NULL, ":", &holder);
    if (tok  != NULL)
        pDrvCtrl->mblkMult = strtoul (tok, NULL, 16);

    /* clMult is optional */

    pDrvCtrl->clMult =  6;
    tok = strtok_r (NULL, ":", &holder);
    if (tok != NULL)
        pDrvCtrl->clMult = strtoul (tok, NULL, 16);

    /* txJobMsgQLen is optional */

    pDrvCtrl->txJobMsgQLen = 128;
    tok = strtok_r (NULL, ":", &holder);
    if (tok != NULL)
        pDrvCtrl->txJobMsgQLen = strtoul (tok, NULL, 16);

    if (!pDrvCtrl->tbdNum || pDrvCtrl->tbdNum <= 2)
	{
	MOT_FCC_FLAG_SET (MOT_FCC_INV_TBD_NUM);
	pDrvCtrl->tbdNum = MOT_FCC_TBD_DEF_NUM;
	}

    if (!pDrvCtrl->rbdNum || pDrvCtrl->rbdNum <= 2)
	{
	MOT_FCC_FLAG_SET (MOT_FCC_INV_RBD_NUM);
	pDrvCtrl->rbdNum = MOT_FCC_RBD_DEF_NUM; 
	}

    MOT_FCC_LOG (MOT_FCC_DBG_LOAD,
		 ("motFccEndParse: unit=%d immrVal=0x%x 
		 bufBase=0x%x bufSize=0x%x,
		 bdBase=0x%x bdSize=0x%x \n"),
		 pDrvCtrl->unit, 
		 (int) pDrvCtrl->immrVal,
		 (int) pDrvCtrl->pBufBase,
		 pDrvCtrl->bufSize,
		 (int) pDrvCtrl->pBdBase,
		 pDrvCtrl->bdSize);

    MOT_FCC_LOG (MOT_FCC_DBG_LOAD,
		 ("motFccEndParse: fifoTxBase=0x%x fifoRxBase=0x%x
		 tbdNum=%d rbdNum=%d flags=0x%x fccNum=%d\n"),
		 pDrvCtrl->fifoTxBase,
		 pDrvCtrl->fifoRxBase,
		 pDrvCtrl->tbdNum,
		 pDrvCtrl->rbdNum,
		 pDrvCtrl->userFlags,
		 pDrvCtrl->fccNum);

    MOT_FCC_LOG (MOT_FCC_DBG_LOAD,
                 ("motFccEndParse: phyAddr=0x%x 
                  phyDefMode=0x%x orderTbl=0x%x \n"),
                 pDrvCtrl->phyInfo->phyAddr,
                 pDrvCtrl->phyInfo->phyDefMode,
                 (int) pDrvCtrl->phyInfo->phyAnOrderTbl,
                 0, 0, 0);
    return (OK);
    }

/*******************************************************************************
* motFccInitMem - initialize memory
*
* This routine initializes all the memory needed by the driver whose control
* structure is passed in <pDrvCtrl>.
*
* RETURNS: OK or ERROR
*/
LOCAL STATUS motFccInitMem 
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    UINT32		bdMemSize,rbdMemSize,tbdMemSize;
    UINT16		clNum;		/* a buffer number holder */

    /* cluster blocks configuration */

    M_CL_CONFIG	mclBlkConfig = {0, 0, NULL, 0};

    /* cluster blocks config table */

    CL_DESC		clDescTbl [] = { {MOT_FCC_MAX_CL_LEN, 0, NULL, 0} };

    /* number of different clusters sizes in pool -- only 1 */

    int		clDescTblNumEnt = 1;

    /* initialize the netPool */

    if ((pDrvCtrl->endObj.pNetPool = malloc (sizeof (NET_POOL))) == NULL)
        {
	return ERROR;
        }

    /* 
     * Establish the memory area that we will share with the device.  This
     * area may be thought of as being divided into two parts: one is the 
     * buffer descriptors (BD) and the second one is for the data buffers. 
     * Since they have different requirements as far as cache are concerned,
     * they may be addressed separately.
     * We'll deal with the BDs area first.  If the caller has provided 
     * an area, then we assume it is non-cacheable and will not require 
     * the use of the special cache routines. If the caller has not provided 
     * an area, then we must obtain it from the system, using the cache 
     * savvy allocation routine.
     */

    switch ((int) pDrvCtrl->pBdBase) 
        {
        case NONE :			     /* we must obtain it */

	/* this driver can't handle write incoherent caches */

	    if (!CACHE_DMA_IS_WRITE_COHERENT ()) 
                {
	        MOT_FCC_LOG (MOT_FCC_DBG_LOAD, 
                            "motFccInitMem: shared memory not cache coherent\n",
                            0,0,0,0,0,0);

	        return ERROR;
	        }

	    rbdMemSize = MOT_FCC_RBD_SZ * pDrvCtrl->rbdNum;
	    tbdMemSize = MOT_FCC_TBD_SZ * pDrvCtrl->tbdNum;
	    bdMemSize = rbdMemSize + tbdMemSize + MOT_FCC_BD_ALIGN;
	    pDrvCtrl->pBdBase = cacheDmaMalloc(bdMemSize);

	    if (pDrvCtrl->pBdBase == NULL)
                {     
                /* no memory available */
	        MOT_FCC_LOG (MOT_FCC_DBG_LOAD, 
                             "motFccInitMem: could not obtain memory\n",
                             0,0,0,0,0,0);
	        return ERROR;
	        }

	    pDrvCtrl->bdSize = bdMemSize;
	    MOT_FCC_FLAG_SET (MOT_FCC_OWN_BD_MEM);
	    pDrvCtrl->bdCacheFuncs = cacheDmaFuncs;
	    break;

        default :	       /* the user provided an area */

            if (!pDrvCtrl->bdSize)
                {
	        MOT_FCC_LOG (MOT_FCC_DBG_LOAD, 
                             "motFccInitMem: not enough memory\n",0,0,0,0,0,0);

	        return ERROR;
	        }

	    /* 
	     * check whether user provided a number for Rx and Tx BDs
	     * fill in the blanks if we can.
	     * check whether enough memory was provided, etc.
	     */
        
	    if (MOT_FCC_FLAG_ISSET (MOT_FCC_INV_TBD_NUM) && 
                MOT_FCC_FLAG_ISSET (MOT_FCC_INV_RBD_NUM)) 
                {
	        pDrvCtrl->tbdNum = pDrvCtrl->bdSize / 
                                   (MOT_FCC_TBD_SZ + MOT_FCC_RBD_SZ);

	        pDrvCtrl->rbdNum = pDrvCtrl->tbdNum;
	        } 
            else if (MOT_FCC_FLAG_ISSET (MOT_FCC_INV_TBD_NUM)) 
                {
	        rbdMemSize = MOT_FCC_RBD_SZ * pDrvCtrl->rbdNum;
	        pDrvCtrl->tbdNum = (pDrvCtrl->bdSize - rbdMemSize) / 
                                   MOT_FCC_TBD_SZ;
	        } 
            else if (MOT_FCC_FLAG_ISSET (MOT_FCC_INV_RBD_NUM)) 
                {
	        tbdMemSize = MOT_FCC_TBD_SZ * pDrvCtrl->tbdNum;
	        pDrvCtrl->rbdNum = (pDrvCtrl->bdSize - tbdMemSize) / 
                                   MOT_FCC_RBD_SZ;
	        } 
            else 
                {
	        rbdMemSize = MOT_FCC_RBD_SZ * pDrvCtrl->rbdNum;
	        tbdMemSize = MOT_FCC_TBD_SZ * pDrvCtrl->tbdNum;
	        bdMemSize = rbdMemSize + tbdMemSize + MOT_FCC_BD_ALIGN;

	        if (pDrvCtrl->bdSize < bdMemSize) 
                    {
		    MOT_FCC_LOG (MOT_FCC_DBG_LOAD, 
                                 "motFccInitMem: not enough memory\n",
                                 0,0,0,0,0,0);
		    return ERROR;
	            }
	        }

            if ((pDrvCtrl->tbdNum < MOT_FCC_TBD_MIN) || 
                (pDrvCtrl->rbdNum < MOT_FCC_RBD_MIN)) 
                {
	        MOT_FCC_LOG (MOT_FCC_DBG_LOAD, 
                             "motFccInitMem: not enough BDs\n",0,0,0,0,0,0);
	        return ERROR;
	        }

	    MOT_FCC_FLAG_CLEAR (MOT_FCC_OWN_BD_MEM);
	    pDrvCtrl->bdCacheFuncs = cacheNullFuncs;
	    break;
        }

    /* zero the shared memory */

    memset (pDrvCtrl->pBdBase, 0, (int) pDrvCtrl->bdSize);

    /* align the shared memory */

    pDrvCtrl->pBdBase = (char *) ROUND_UP((UINT32)pDrvCtrl->pBdBase,
                                          MOT_FCC_BD_ALIGN);

    /* 
     * number of clusters, including loaning buffers, a min number
     * of transmit clusters for copy-mode transmit, and one transmit
     * cluster for polling operation.
     */

    clNum = pDrvCtrl->rbdNum + MOT_FCC_BD_LOAN_NUM + MOT_FCC_TX_POLL_NUM + 
            MOT_FCC_TX_CL_NUM;

    /* pool of mblks */

    if (mclBlkConfig.mBlkNum == 0)
        {
	mclBlkConfig.mBlkNum = clNum * pDrvCtrl->mblkMult;
        }

    /* pool of clusters, including loaning buffers */

    if (clDescTbl[0].clNum == 0) 
        {
	clDescTbl[0].clNum = clNum * pDrvCtrl->clMult;
	clDescTbl[0].clSize = MOT_FCC_MAX_CL_LEN;
        } 

    /* there's a cluster overhead and an alignment issue */

    clDescTbl[0].memSize = clDescTbl[0].clNum * 
                           (clDescTbl[0].clSize + CL_OVERHEAD) + 
                           CL_ALIGNMENT - 1;

    /*
     * Now we'll deal with the data buffers.  If the caller has provided 
     * an area, then we assume it is non-cacheable and will not require 
     * the use of the special cache routines. If the caller has not provided 
     * an area, then we must obtain it from the system, but we will not be
     * using the cache savvy allocation routine, since we will flushing or
     * invalidate the data cache itself as appropriate. This speeds up
     * driver operation, as the network stack will be able to process data
     * in a cacheable area.
     */

    switch ((int) pDrvCtrl->pBufBase) 
        {
        case NONE :		     /* we must obtain it */

	    clDescTbl[0].memArea = (char *) memalign(CL_ALIGNMENT, 
                               clDescTbl[0].memSize);
	    if (clDescTbl[0].memArea == NULL)
                {
	        MOT_FCC_LOG (MOT_FCC_DBG_LOAD, 
                             ("motFccInitMem: could not obtain memory\n"),
                             0,0,0,0,0,0);
	        return ERROR;
	        }

	    /* store the pointer to the clBlock area and its size */

	    pDrvCtrl->pBufBase = clDescTbl[0].memArea;
	    pDrvCtrl->bufSize = clDescTbl[0].memSize;
	    MOT_FCC_FLAG_SET (MOT_FCC_OWN_BUF_MEM);

	    /* cache functions descriptor for data buffers */

	    motFccBufCacheFuncs.flushRtn = cacheArchFlush;
	    motFccBufCacheFuncs.invalidateRtn = cacheArchInvalidate;
	    motFccBufCacheFuncs.virtToPhysRtn = NULL;
	    motFccBufCacheFuncs.physToVirtRtn = NULL;
	    pDrvCtrl->bufCacheFuncs = motFccBufCacheFuncs;

	    break;

        default :	       /* the user provided an area */

	    if (pDrvCtrl->bufSize == 0) 
                {
	        MOT_FCC_LOG (MOT_FCC_DBG_LOAD, 
                            ("motFccInitMem: not enough memory\n"),0,0,0,0,0,0);

	        return ERROR;
	        }

	    /* 
	     * check the user provided enough memory with reference
	     * to the given number of receive/transmit frames, if any.
	     */

	    if (pDrvCtrl->bufSize < clDescTbl[0].memSize) 
                {
	        MOT_FCC_LOG (MOT_FCC_DBG_LOAD, 
                            ("motFccInitMem: not enough memory\n"),0,0,0,0,0,0);
	        return ERROR;
	        }

	    /* Set memArea to the buffer base */

	    clDescTbl[0].memArea = pDrvCtrl->pBufBase;
	    MOT_FCC_FLAG_CLEAR (MOT_FCC_OWN_BUF_MEM);
	    pDrvCtrl->bufCacheFuncs = cacheNullFuncs;

	    break;
        }

    /* zero and align the shared memory */

    memset (pDrvCtrl->pBufBase, 0, (int) pDrvCtrl->bufSize);
    pDrvCtrl->pBufBase = (char *) ROUND_UP((UINT32)pDrvCtrl->pBufBase,
                                           CL_ALIGNMENT);
 
    /* pool of cluster blocks */

    if (mclBlkConfig.clBlkNum == 0)
        {
	mclBlkConfig.clBlkNum = clDescTbl[0].clNum;
        }

    /* get memory for mblks */

    if (mclBlkConfig.memArea == NULL) 
        {

	/* memory size adjusted to hold the netPool pointer at the head */

	mclBlkConfig.memSize = ((mclBlkConfig.mBlkNum * 
                                (M_BLK_SZ + MBLK_ALIGNMENT)) + 
                                (mclBlkConfig.clBlkNum * 
                                (CL_BLK_SZ + CL_ALIGNMENT)));

	mclBlkConfig.memArea = (char *) memalign (MBLK_ALIGNMENT, 
                                                  mclBlkConfig.memSize);
	    if (mclBlkConfig.memArea == NULL) 
                {
	        return ERROR;
	        }
            else
                {
                memset (mclBlkConfig.memArea, 0, (int)mclBlkConfig.memSize);  
                }

	/* store the pointer to the mBlock area */

	pDrvCtrl->pMBlkArea = mclBlkConfig.memArea;
	pDrvCtrl->mBlkSize = mclBlkConfig.memSize;
        }

    /* init the mem pool */

    if (netPoolInit (pDrvCtrl->endObj.pNetPool, 
                     &mclBlkConfig, &clDescTbl[0], clDescTblNumEnt, 
                     NULL) == ERROR) 
        {
	return ERROR;
        }

    if ((pDrvCtrl->pClPoolId = netClPoolIdGet (pDrvCtrl->endObj.pNetPool, 
                                            MOT_FCC_MAX_CL_LEN, FALSE)) == NULL)       
        {
	return ERROR;
        }

    /* wait for 25% available before restart MUX */

    pDrvCtrl->unStallThresh = (pDrvCtrl->tbdNum >> 2);	

    MOT_FCC_LOG (MOT_FCC_DBG_LOAD, ("motFccInitMem... Done\n"), 
                 0, 0, 0, 0, 0, 0);
    return OK;
    }

/**************************************************************************
*
* motFccStart - start the device
*
* This routine starts the FCC device and brings it up to an operational
* state.  The driver must have already been loaded with the motFccEndLoad()
* routine.
*
* INTERNAL
* The speed field inthe phyInfo structure is only set after the call
* to the physical layer init routine. On the other hand, the mib2 
* interface is initialized in the motFccEndLoad() routine, and the default
* value of 10Mbit assumed there is not always correct. We need to
* correct it here.
*
* RETURNS: OK, or ERROR if the device could not be started.
*
*/
LOCAL STATUS motFccStart
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    int	        retVal;		/* convenient holder for return value */
    char	bucket[4];	/* holder for vxMemProbe */

    MOT_FCC_LOG (MOT_FCC_DBG_START, "Starting end...\n", 1, 2, 3, 4, 5, 6);

    /* must have been loaded */

    if (!pDrvCtrl->loaded)
	return ERROR;

    if (vxMemProbe ((char *) (pDrvCtrl->fccIramAddr), 
        VX_READ, 4, &bucket[0]) != OK) 
        {
        MOT_FCC_LOG (MOT_FCC_DBG_START, ": need MMU mapping for address 0x%x\n",
                   (UINT32) pDrvCtrl->fccIramAddr, 2, 3, 4, 5, 6);

	return ERROR;
        }

    if (motFccTbdInit(pDrvCtrl) == ERROR || motFccRbdInit(pDrvCtrl) == ERROR)
	return ERROR;

    /* set some flags to default values */

    pDrvCtrl->txStall = FALSE;
    pDrvCtrl->tbdIndex = 0;
    pDrvCtrl->usedTbdIndex = 0;
    pDrvCtrl->cleanTbdNum = pDrvCtrl->tbdNum;
    pDrvCtrl->rbdIndex = 0;

    /* acknowledge system interrupt */

    SYS_FCC_INT_ACK (pDrvCtrl, retVal);

    if (retVal == ERROR)
	return ERROR;
	
    /* connect the interrupt handler */

    SYS_FCC_INT_CONNECT (pDrvCtrl, motFccInt, (int) pDrvCtrl, retVal);

    if (retVal == ERROR)
	return ERROR;

    /* enable system interrupt: set relevant bit in SIMNR_L */

    SYS_FCC_INT_ENABLE (pDrvCtrl, retVal);

    if (retVal == ERROR)
	return ERROR;
	
    /* call the BSP to do any other initialization (MII interface) */

    SYS_FCC_ENET_ENABLE;

    /* initialize some fields in the PHY info structure */

    if (motFccPhyPreInit (pDrvCtrl) != OK) 
        {
	MOT_FCC_LOG (MOT_FCC_DBG_LOAD, ("Failed to pre-initialize the PHY\n"), 
                     0, 0, 0, 0, 0, 0);

	return ERROR;
        }

    /* initialize the Physical medium layer */

    if (_func_motFccPhyInit == NULL)
	return ERROR;

    if (((* _func_motFccPhyInit) (pDrvCtrl->phyInfo)) != OK) 
        {
	MOT_FCC_LOG (MOT_FCC_DBG_LOAD, ("Failed to initialize the PHY\n"), 
                     0, 0, 0, 0, 0, 0);
	return ERROR;
        }

    /* initialize the chip */

    if (motFccInit (pDrvCtrl) == ERROR)
	return ERROR;

    if (motFccJobQInit(pDrvCtrl) == ERROR)
        return ERROR; 

    /* correct the speed for the mib2 stuff */

    pDrvCtrl->endObj.mib2Tbl.ifSpeed = pDrvCtrl->phyInfo->phySpeed;

    /* startup the transmitter */

    pDrvCtrl->fccReg->fcc_gmr |= M8260_GFMR_ENT;

    /* mark the interface as up */

    END_FLAGS_SET (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));

    /* startup the receiver */

    MOT_FCC_RX_ACTIVATE;

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    MOT_FCC_LOG (MOT_FCC_DBG_START, ("Starting end... Done\n"), 
                 1, 2, 3, 4, 5, 6);

    return OK;
    }

/**************************************************************************
*
* motFccStop - stop the 'motfcc' interface
*
* This routine marks the interface as inactive, disables interrupts and 
* the Ethernet Controller. As a result, reception is stopped immediately,
* and transmission is stopped after a bad CRC is appended to any frame
* currently being transmitted. The reception/transmission control logic
* (FIFO pointers, buffer descriptors, etc.) is reset. To bring the 
* interface back up, motFccStart() must be called.
*
* RETURNS: OK, always.
*/
LOCAL STATUS motFccStop
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    int		retVal;		/* convenient holder for return value */
    UINT8	command = 0;	/* command to issue to the CP */

    MOT_FCC_LOG (MOT_FCC_DBG_LOAD, ("motFccStop...\n"), 1, 2, 3, 4, 5, 6);

    /* mark the interface as down */

    END_FLAGS_CLR (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));

    /* issue a graceful stop transmit command to the CP */

    command = M8260_CPCR_TX_GRSTOP;

    if (motFccCpcrCommand (pDrvCtrl, command) == ERROR)
	return ERROR;

    /* wait for the related interrupt */

    MOT_FCC_GRA_SEM_TAKE;

    /* mask chip interrupts */

    MOT_FCC_INT_DISABLE;	  

    /* disable system interrupt: reset relevant bit in SIMNR_L */

    SYS_FCC_INT_DISABLE (pDrvCtrl, retVal);

    if (retVal == ERROR)
	return ERROR;

    /* disconnect the interrupt handler */

    SYS_FCC_INT_DISCONNECT (pDrvCtrl, motFccInt, (int)pDrvCtrl, retVal);

    if (retVal == ERROR)
	return ERROR;

    /* call the BSP to disable the MII interface */

    SYS_FCC_ENET_DISABLE;

    /* free buffer descriptors */

    if (motFccBdFree (pDrvCtrl) != OK)
	return ERROR;

    MOT_FCC_LOG (MOT_FCC_DBG_LOAD, ("motFccStop... Done \n"), 1, 2, 3, 4, 5, 6);

    return OK;
    }

/*******************************************************************************
*
* motFccSendDummyFrame - send a dummy ethernet packet out for ARP
*
* This routine is run in netTask's context.
*
* RETURNS: N/A
*/
LOCAL void motFccSendDummyFrame
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    int	         intLevel;    /* current intr level */
    int	         len;	      /* length of data to be sent */
    char         *pBuf;	      /* pointer to data to be sent */
    FCC_BD       *pTbd;	      /* pointer to the current ready TBD */
    FCC_TBD_LIST *pTbdList;   /* pointer to the TBD list*/

	END_TX_SEM_TAKE(&pDrvCtrl->endObj, WAIT_FOREVER);

	pTbd = &pDrvCtrl->tbdBase[pDrvCtrl->tbdIndex];
	pTbdList = &pDrvCtrl->pTbdList[pDrvCtrl->tbdIndex];
	pBuf = pDrvCtrl->pTxPollBuf;

	memset(pBuf,0xff,6);
	bcopy((char *)MOT_FCC_ADDR_GET(&pDrvCtrl->endObj),pBuf+6,6);

	len = 100;
	MOT_FCC_CACHE_FLUSH(pBuf,len);	/* flush the cache, if necessary */

	pTbd->bdLen = len;
	pTbd->bdAddr = (unsigned long)pBuf;
	pTbdList->pBuf = NULL;	        /* not a Cluster pointer */
	pTbdList->info = 0;		/* so we know NOT to free in ISR */

	intLevel = intLock();
	pDrvCtrl->cleanTbdNum -= 1;	/* keep accurate count of clean BDs */

	if (pDrvCtrl->tbdIndex == (pDrvCtrl->tbdNum - 1))
	    {
	    pTbd->word1 =
	        	( M8260_FETH_TBD_R
	                | M8260_FETH_TBD_L
		        | M8260_FETH_TBD_TC
		        | M8260_FETH_TBD_I
		        | M8260_FETH_TBD_W
		        | M8260_FETH_TBD_PAD
		        );
	    }
	else
	    {
	    pTbd->word1 =
		        ( M8260_FETH_TBD_R
		        | M8260_FETH_TBD_L
		        | M8260_FETH_TBD_TC
		        | M8260_FETH_TBD_I
		        | M8260_FETH_TBD_PAD
		        );
	    }
	
	/* move on to the next element */

	pDrvCtrl->tbdIndex = (pDrvCtrl->tbdIndex + 1) % pDrvCtrl->tbdNum;
	intUnlock (intLevel);

	END_TX_SEM_GIVE(&pDrvCtrl->endObj);
    }

/*******************************************************************************
*
* motFccHandleLSCJob - task-level link status event processor
*
* This routine is run in netTask's context.
*
* RETURNS: N/A
*/
LOCAL void motFccHandleLSCJob
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    int	        intLevel;	/* current intr level */
    UINT16	miiStat;
    int	        retVal;
    int         duplex;
    UINT32	fccIramAddr;
    UINT32      fpsmrVal;

    fccIramAddr = pDrvCtrl->fccIramAddr;

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, "motFccHandleLSCJob: \n", 0, 0, 0, 0, 0, 0);

    ++pDrvCtrl->Stats->numLSCHandlerEntries;

    /* read MII status register once to unlatch link status bit */
    
    retVal = motFccMiiRead(pDrvCtrl,pDrvCtrl->phyInfo->phyAddr,1,&miiStat);

    /* read again to know it's current value */
    
    retVal = motFccMiiRead(pDrvCtrl,pDrvCtrl->phyInfo->phyAddr,1,&miiStat);

    if(miiStat & 4) /* if link is now up */
	{	

	if(_func_motFccPhyDuplex)
	    {
	    /* get duplex mode from BSP */

	    retVal = (* _func_motFccPhyDuplex)(pDrvCtrl->phyInfo, &duplex);
	    
	    if(retVal == OK)
	        {
	        intLevel = intLock();
				
	        MOT_FCC_REG_LONG_RD (fccIramAddr + M8260_FCC_FPSMR_OFF, 
                                     fpsmrVal);
				
	        if(duplex) /* TRUE when full duplex mode is active */
	            { 
	            fpsmrVal |= (M8260_FPSMR_ETH_LPB | M8260_FPSMR_ETH_FDE);

	            MOT_FCC_LOG(MOT_FCC_DBG_ANY, 
                                "motFccHandleLSCJob: enable full duplex\n", 
                                 0, 0, 0, 0, 0, 0);
	            }
	        else
	            {
	            fpsmrVal &= ~(M8260_FPSMR_ETH_LPB | M8260_FPSMR_ETH_FDE);

	            MOT_FCC_LOG(MOT_FCC_DBG_ANY, 
                                "motFccHandleLSCJob: disable full duplex\n", 
                                0, 0, 0, 0, 0, 0);
		    }
				
	    MOT_FCC_REG_LONG_WR (fccIramAddr + M8260_FCC_FPSMR_OFF, fpsmrVal);

	    intUnlock (intLevel);
		
	    /* check device mode */

	    if(!MOT_FCC_FLAG_ISSET(MOT_FCC_POLLING))
	        {	
	        /* check if any transmits are currently pending */

	        if(pDrvCtrl->cleanTbdNum == pDrvCtrl->tbdNum)
	            {
	            netJobAdd((FUNCPTR)motFccSendDummyFrame,(int)pDrvCtrl,
                              0,0,0,0);
	            netJobAdd((FUNCPTR)motFccSendDummyFrame,(int)pDrvCtrl,
                              0,0,0,0);
	            netJobAdd((FUNCPTR)motFccSendDummyFrame,(int)pDrvCtrl,
                              0,0,0,0);
	            }
	        }
	    }
	}
    }

    intLevel = intLock();
    pDrvCtrl->lscHandling = 0;
    intUnlock (intLevel);

    ++pDrvCtrl->Stats->numLSCHandlerExits;
    }

/*******************************************************************************
*
* motFccJobQInit - Create a msgQueue for motFccJobQueue() 
*  
* This routine creates the msgQueue to be used buy motFccJobQueue(). 
*
* RETURNS: OK, or ERROR
*/

LOCAL STATUS motFccJobQInit
    (
    DRV_CTRL * pDrvCtrl
    )
    {
  
    if (!(pDrvCtrl->txJobMsgQId = msgQCreate(pDrvCtrl->txJobMsgQLen, 
                                     sizeof(TODO_NODE), MSG_Q_FIFO)))
        {
        logMsg("ERROR - motFccjobQueInit: unable to create msgQueue.\n",
                0,0,0,0,0,0);
        return(ERROR);
        }
 
    return (OK);
    }

/*******************************************************************************
*
* motFccJobQAdd - Add a job to the txJobMsgQueue
*
* This routine passes a motFccTxFreeJob to motFccJobQueue() with msgQSend(). 
* If msgQSend() returns an ERROR it then sends the motFccTxFreeJob to 
* the netJobRing with netJobAdd(). 
*
* RETURNS: N/A
*/

LOCAL void motFccJobQAdd
    (
    DRV_CTRL * pDrvCtrl,
    FUNCPTR    routine,
    int        param1,
    int        param2,
    int        param3,
    int        param4,
    int        param5
    )
    { 
    TODO_NODE  pJobNode; 
     
    pJobNode.routine = routine;
    pJobNode.param1 = param1;
    pJobNode.param2 = param2;
    pJobNode.param3 = param3;
    pJobNode.param4 = param4;
    pJobNode.param5 = param5;

    if ((msgQSend (pDrvCtrl->txJobMsgQId, (char *)&pJobNode, 
                   sizeof(TODO_NODE), NO_WAIT, 0)) == ERROR)
        {      
        netJobAdd((FUNCPTR)routine, param1, param2, param3, param4, param5);
        }
    }

/*******************************************************************************
*
* motFccJobQueue - Empties the txJobMsgQueue
*
* This routine is added by the ISR to the netJobRing with netJobAdd(). 
* It de-queues and executes the motFccTxFreeJob s then passed to it by 
* motFccJobQAdd() in the ISR. Its purpose is to reduce the number of jobs 
* added to the netJobRing to prevent "netTask: netJobRing overflow!" panics  
*
* RETURNS: OK, or ERROR
*/

LOCAL STATUS  motFccJobQueue
    (
    DRV_CTRL * pDrvCtrl
    )
    {
    TODO_NODE     jobNode;
    int           msgQRxStatus;
    int           intLockKey;
    int		  localError = 0;
 
    while (msgQNumMsgs(pDrvCtrl->txJobMsgQId) > 0)
        {
        msgQRxStatus = msgQReceive(pDrvCtrl->txJobMsgQId, (char *)&jobNode,
                                   sizeof(TODO_NODE), NO_WAIT);
        if (msgQRxStatus == ERROR)
            {
            localError = errnoGet();

            if (localError != S_objLib_OBJ_TIMEOUT)
                {
                 MOT_FCC_LOG(MOT_FCC_DBG_INT_TX_ERR,
                             "motFccJobQueue: msgQReceive failed! error %x\n", 
                              0, 0, 0, 0, 0, 0);
                }
            }
        else
            {
            (*(jobNode.routine)) (jobNode.param1, 
                                  jobNode.param2,
                                  jobNode.param3, 
                                  jobNode.param4,
                                  jobNode.param5);
            msgQRxStatus = OK;
            }
        } 
        intLockKey = intLock();  
        
        /* motFccJobQueue() is off the netJobRing */

        pDrvCtrl->txJobQState = FALSE; 

        intUnlock (intLockKey);

        return (msgQRxStatus);
    }

/*******************************************************************************
* motFccTxFreeJob - 
*
*
* RETURNS: N/A
*/

LOCAL void motFccTxFreeJob 
    (
    DRV_CTRL *pDrvCtrl,void *pBuf,unsigned long info
    )
    {
    /* up-date statistics */

    if (info & PKT_TYPE_MULTI)
        {
	pDrvCtrl->endObj.mib2Tbl.ifOutNUcastPkts += 1;
        }
    else
        {
	END_ERR_ADD(&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);
        }

    /* free the Mblk or the cluster, whichever is appropriate */

    if ((info & BUF_TYPE_MBLK))
        {
	netMblkClChainFree((M_BLK *)pBuf);
        }
    else if ((info & BUF_TYPE_CL))
        {
	NET_BUF_FREE(pBuf);
        }
    }

/*******************************************************************************
* motFccTxErrRecoverJob - 
*
*
* RETURNS: N/A
*/

LOCAL void motFccTxErrRecoverJob
    (
    DRV_CTRL *pDrvCtrl, 
    void *bdPtr
    )
    {
    UINT32	    immrVal = pDrvCtrl->immrVal; /* holder for the immr value */
    UINT8	    fccNum = pDrvCtrl->fccNum;	 /* holder for the fcc number */
    UINT8	    fcc = fccNum - 1;		 /* a convenience */
    volatile UINT32 cpcrVal;    		 /* a convenience */

    /* issue a init tx command to the CP */

    cpcrVal = 0;

    /* wait until the CP is clear */

    do {
    MOT_FCC_REG_LONG_RD (M8260_CPCR (immrVal), cpcrVal);
    } while (cpcrVal & M8260_CPCR_FLG);

    /* issue the command to the CP */

    cpcrVal = (M8260_CPCR_OP ((UINT8)M8260_CPCR_TX_INIT)
	       | M8260_CPCR_SBC (M8260_CPCR_SBC_FCC1 | (fcc * 0x1))
	       | M8260_CPCR_PAGE (M8260_CPCR_PAGE_FCC1 | (fcc * 0x1))
	       | M8260_CPCR_MCN (M8260_CPCR_MCN_ETH)
	       | M8260_CPCR_FLG);

    MOT_FCC_REG_LONG_WR (M8260_CPCR (immrVal), cpcrVal);
    CACHE_PIPE_FLUSH ();		/* Flush the write pipe */

    /* issue a restart tx command to the CP */

    cpcrVal = 0;

    /* wait until the CP is clear */

    do {
    MOT_FCC_REG_LONG_RD (M8260_CPCR (immrVal), cpcrVal);
    } while (cpcrVal & M8260_CPCR_FLG);

    /* issue the command to the CP */

    cpcrVal = (M8260_CPCR_OP ((UINT8)M8260_CPCR_TX_RESTART)
	       | M8260_CPCR_SBC (M8260_CPCR_SBC_FCC1 | (fcc * 0x1))
	       | M8260_CPCR_PAGE (M8260_CPCR_PAGE_FCC1 | (fcc * 0x1))
	       | M8260_CPCR_MCN (M8260_CPCR_MCN_ETH)
	       | M8260_CPCR_FLG);

    MOT_FCC_REG_LONG_WR (M8260_CPCR (immrVal), cpcrVal);
    CACHE_PIPE_FLUSH ();		/* Flush the write pipe */

    do {
    pDrvCtrl->fccPar->tbptr = (ULONG)bdPtr;
    } while(pDrvCtrl->fccPar->tbptr != (ULONG)bdPtr);

    pDrvCtrl->fccReg->fcc_fccm |= (M8260_FEM_ETH_TXE|M8260_FEM_ETH_TXB);
    pDrvCtrl->fccReg->fcc_gmr |= M8260_GFMR_ENT;
    }

/*******************************************************************************
* motFccInt - entry point for handling interrupts from the FCC
*
* The interrupting events are acknowledged to the device, so that the device
* will de-assert its interrupt signal.  The amount of work done here is kept
* to a minimum; the bulk of the work is deferred to the netTask.
*
* RETURNS: N/A
*/
LOCAL void motFccInt 
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    UINT16	event;		/* event intr register */
    UINT16	mask;		/* holder for the mask intr register */
    UINT16	status;		/* status word */
    FCC_REG_T	*reg = pDrvCtrl->fccReg;
    FCC_BD	*bdPtr;

    ++pDrvCtrl->Stats->numIsrEntries;

    event = reg->fcc_fcce;
    reg->fcc_fcce = event;
    mask = reg->fcc_fccm;
    status = event & mask;

    /*
     * This driver gives highest priority to keeping the receive BDs ready
     * to receive data -- empty bit set, and an attached buffer. When 
     * processing frame interrupts, if a replacement buffer is not available
     * for the current BD, then that frame data is discarded. This is done
     * by simply clearing the length field and resetting the BD status word.
     * The buffer address that was previously attached to the BD will then
     * be over-written the next time the FCC consults that BD. The goal is
     * to prevent the 'buffer busy' condition where the FCC is receiving
     * frame data and has no empty BD in which to store the frame.
     * In practice, the 'busy' condition will still occur in some cases.
     *   1. Excessive interrupt latency caused by either the application
     *      or by other device drivers.
     *   2. Too few receive BDs.
     *   3. An excessive number of consecutive short frames (if enabled) or
     *      an excessive number of consecutive bad CRC frames.
     *      (NOTE: an excessive number of too long frames (LG) is not
     *       likely to be a problem)
     *   4. The intrusion of H/W debug devices (ICE, JTAG, etc.) causing
     *      excessive interrupt latency.
     * In general, if the FCC, for whatever reason, closes all the receive
     * BDs before this ISR gets a chance to process the BDs, busy will
     * likely occur. However, because this ISR immediately processes all
     * the BDs, we don't need any special handling when busy happens.
     * We simply make note of it and maintain a counter for it.
     */

    /* handle receive frame and busy interrupts */

    if (status & M8260_FEM_ETH_BSY)
        {
	++pDrvCtrl->Stats->motFccRxBsyErr;

	MOT_FCC_LOG(MOT_FCC_DBG_INT_RX_ERR,
                    "motFccInt: ETH_BSY error\n",0,0,0,0,0,0);
        }

    if (status & M8260_FEM_ETH_RXF)
        {
	int	lostframes;
	char	*pBuf;
	BPOOL	*bpoolp;

	MOT_FCC_LOG(MOT_FCC_DBG_INT,"motFccInt: rx intr\n",0,0,0,0,0,0);

	++pDrvCtrl->Stats->numRXFInts;
	lostframes = 0;

	do {
	bdPtr = &pDrvCtrl->rbdBase[pDrvCtrl->rbdIndex];

	if ((bdPtr->word1 & M8260_FETH_RBD_E))
            {
	    break;	/* if BD is owned by CPM, don't touch it! */
	    }

	if (!(bpoolp = pDrvCtrl->RxFree))
            {
	    ++lostframes;
	    }
        else
            {
            pDrvCtrl->RxFree = bpoolp->next;
	    bpoolp->next = NULL;

  	    if(pDrvCtrl->RxBufCount)
                {
	        pDrvCtrl->RxTail->next = bpoolp;
	        }
            else
                {
	        pDrvCtrl->RxHead = bpoolp;
	        }

	    pDrvCtrl->RxTail = bpoolp;
	    ++pDrvCtrl->RxBufCount;

            /* copy the whole descriptor to the container */

	    bpoolp->desc = *bdPtr;

	    /* take the replacement cluster buf pointer */

	    pBuf = bpoolp->pBuf;

	    /* and replace it with the previous cluster buf pointer */

	    bpoolp->pBuf = pDrvCtrl->rBufList[pDrvCtrl->rbdIndex];

	    /* then save the about-to-be-used cluster buf pointer */

	    pDrvCtrl->rBufList[pDrvCtrl->rbdIndex] = pBuf;

            bdPtr->bdAddr = ROUND_UP((unsigned long)pBuf, 
                                         MOT_FCC_BUF_ALIGN );
	    }
	bdPtr->bdLen = 0;

	/* 
         * update the status word: set WRAP bit for the last 
         * RBD in the ring 
         */

	if (pDrvCtrl->rbdIndex == (pDrvCtrl->rbdNum - 1)) 
            {
	    bdPtr->word1 = M8260_FETH_RBD_W | M8260_FETH_RBD_E | 
                           M8260_FETH_RBD_I;
	    }
        else
            {
            bdPtr->word1 = M8260_FETH_RBD_E | M8260_FETH_RBD_I;
	    }

	/* move on to the next used RBD */

	pDrvCtrl->rbdIndex = (pDrvCtrl->rbdIndex + 1) % (pDrvCtrl->rbdNum);

	} while(TRUE);

	if (lostframes)
            {
	    pDrvCtrl->Stats->numRXFramesLostToBufferReuse += lostframes;

#if 0
   logMsg("%d receive frames lost due to no buffer\n",lostframes,0,0,0,0,0); 
#endif

	    }
        }

    /* handle transmit errors and interrupts */

    if ((status & (M8260_FEM_ETH_TXE|M8260_FEM_ETH_TXB)))
        {
	FCC_TBD_LIST	*pTbdList;
	UINT16		bdStatus;
	void		*pBuf;

	++pDrvCtrl->Stats->numTXBInts;

	/* process all the consumed TBDs */

	while (pDrvCtrl->cleanTbdNum < pDrvCtrl->tbdNum)
            {
	    bdPtr = &pDrvCtrl->tbdBase[pDrvCtrl->usedTbdIndex];
	    bdStatus = bdPtr->word1;

	    if ((bdStatus & M8260_FETH_TBD_R))
                {
		break;
	        }

	    if (bdStatus & (M8260_FETH_TBD_LC | M8260_FETH_TBD_RL | 
                            M8260_FETH_TBD_UN | M8260_FETH_TBD_HB | 
                            M8260_FETH_TBD_CSL)) 
                {
		++pDrvCtrl->Stats->motFccTxErr;

		if ((bdStatus & M8260_FETH_TBD_RL))
                    {
		    MOT_FCC_LOG (MOT_FCC_DBG_INT_TX_ERR, 
                                 "motFccTbdCheck: retransmit limit", 
                                 1, 2, 3, 4, 5, 6);

		    ++pDrvCtrl->Stats->motFccTxRlErr;
		    }

		if ((bdStatus & M8260_FETH_TBD_LC))
                    {
		    MOT_FCC_LOG (MOT_FCC_DBG_INT_TX_ERR, 
                                 "motFccTbdCheck: late collision\n", 
                                 1, 2, 3, 4, 5, 6);

		    ++pDrvCtrl->Stats->motFccTxLcErr;
		    }

		if ((bdStatus & M8260_FETH_TBD_UN))
                    {
		    MOT_FCC_LOG (MOT_FCC_DBG_INT_TX_ERR, 
                                 "motFccTbdCheck: tx underrun\n", 
                                  1, 2, 3, 4, 5, 6);

		    ++pDrvCtrl->Stats->motFccTxUrErr;
		    }

		if((bdStatus & M8260_FETH_TBD_CSL))
                    {
		    MOT_FCC_LOG (MOT_FCC_DBG_INT_TX_ERR, 
                                 "motFccTbdCheck: carrier sense \n", 
                                 1, 2, 3, 4, 5, 6);

		    ++pDrvCtrl->Stats->motFccTxCslErr;
	            }
                }

            if ((bdStatus & (M8260_FETH_TBD_RL | M8260_FETH_TBD_UN | 
                             M8260_FETH_TBD_LC)))
                {
	        reg->fcc_gmr &= ~M8260_GFMR_ENT;
	        reg->fcc_fcce = (M8260_FEM_ETH_TXE|M8260_FEM_ETH_TXB);
	        reg->fcc_fccm &= ~(M8260_FEM_ETH_TXE|M8260_FEM_ETH_TXB);

	        while (!(bdStatus & M8260_FETH_TBD_L) && 
                       pDrvCtrl->cleanTbdNum < pDrvCtrl->tbdNum)
                    {
	            bdPtr->word1 &= 0x7fff;
	            ++pDrvCtrl->cleanTbdNum;
	            pDrvCtrl->usedTbdIndex = (pDrvCtrl->usedTbdIndex + 1) % 
                                              (pDrvCtrl->tbdNum);

	            bdPtr = &pDrvCtrl->tbdBase[pDrvCtrl->usedTbdIndex];
	            bdStatus = bdPtr->word1;
	            }

	        pTbdList = &pDrvCtrl->pTbdList[pDrvCtrl->usedTbdIndex];

	        if ((bdStatus & M8260_FETH_TBD_L))
                    {
	            if ((pBuf = pTbdList->pBuf))
                        {
		        motFccTxFreeJob(pDrvCtrl,pBuf,pTbdList->info);
	                }

	            bdPtr->word1 &= 0x7fff;
	            ++pDrvCtrl->cleanTbdNum;
	            pDrvCtrl->usedTbdIndex = (pDrvCtrl->usedTbdIndex + 1) % 
                                              (pDrvCtrl->tbdNum);

	            bdPtr = &pDrvCtrl->tbdBase[pDrvCtrl->usedTbdIndex];
	            }
                else
                    {
	            /* should be nothing to do here */
	            }

	        motFccTxErrRecoverJob(pDrvCtrl,(void*)bdPtr);

                break;

                }
            else
                {
	        pTbdList = &pDrvCtrl->pTbdList[pDrvCtrl->usedTbdIndex];

	        /* free the Mblk or the cluster, whichever is appropriate */

	        if ((bdStatus & M8260_FETH_TBD_L) && (pBuf = pTbdList->pBuf))
                    {
                    motFccJobQAdd(pDrvCtrl, (FUNCPTR)motFccTxFreeJob, 
                                  (int) pDrvCtrl, (int) pBuf, pTbdList->info, 
                                  0, 0);

                    /* If motFccJobQueue() isn't on the netJobRing put it on */

                    if (!pDrvCtrl->txJobQState)
                        {
                        pDrvCtrl->txJobQState = TRUE;

                        netJobAdd((FUNCPTR)motFccJobQueue,
                                  (int)pDrvCtrl, 0, 0, 0, 0);
                        }  
	            }
	        pTbdList->pBuf = NULL;
	        pTbdList->info = 0;
	        ++pDrvCtrl->cleanTbdNum;
	        pDrvCtrl->usedTbdIndex = (pDrvCtrl->usedTbdIndex + 1) % 
                                          (pDrvCtrl->tbdNum);
                }
            }
        }

    /* notify upper protocols TBDs are available */

    if (pDrvCtrl->txStall && 
        (pDrvCtrl->cleanTbdNum >= pDrvCtrl->unStallThresh))
        {
	MOT_FCC_LOG (MOT_FCC_DBG_TX,"Restart mux \n",0,0,0,0,0,0);

	if (netJobAdd ((FUNCPTR)muxTxRestart,(int)&pDrvCtrl->endObj,
            0,0,0,0) == OK)
            {
	    ++pDrvCtrl->Stats->numStallsCleared;
	    pDrvCtrl->txStall = FALSE;
	    }
        }

    /* handle graceful transmit stop */

    if (status & M8260_FEM_ETH_GRA)
        {
        MOT_FCC_LOG (MOT_FCC_DBG_INT, 
                     "motFccInt: graceful stop\n", 0, 0, 0, 0, 0, 0);

        /* signal the Stop task that TX_STOP has happened */

        MOT_FCC_GRA_SEM_GIVE;
        }

    if ((pDrvCtrl->RxBufCount && !pDrvCtrl->rxHandling) || 
        (!pDrvCtrl->RxFree && pDrvCtrl->RxEmpty))
        {
        if (netJobAdd((FUNCPTR)motFccHandleRXFrames,
            (int)pDrvCtrl,0,0,0,0)== OK)
            {
            pDrvCtrl->rxHandling = 1 ;
                }
        else 
            {
            logMsg("netJobAdd((FUNCPTR)motFccHandleRXFrames() failed",
                    0,0,0,0,0,0);
            }        
        }
    ++pDrvCtrl->Stats->numIsrExits;
    }

/******************************************************************************
*
* motFccPhyLSCInt - 
*
*
* RETURNS: None
*/

LOCAL void motFccPhyLSCInt
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    /* if no link status change job is pending */

    if(!pDrvCtrl->lscHandling)
	{
	/* and the BSP has supplied a duplex mode function */

	if(_func_motFccPhyDuplex)
	    {	
            if(!netJobAdd((FUNCPTR)motFccHandleLSCJob,(int)pDrvCtrl,0,0,0,0))
		{
		pDrvCtrl->lscHandling = 1;
		}
	    }
	else
	    {    
	    /* 
	     * If the BSP didn't supply a FUNC for getting duplex mode
	     * there's no reason to schedule the link status change job.
	     */
	    }
        }
    }

/******************************************************************************
*
* motFccPktTransmit - transmit a packet
*
* This routine transmits the packet described by the given parameters
* over the network, without copying the mBlk to a driver buffer. 
* It also updates statistics.
*
* RETURNS: OK, or ERROR if no resources were available.
*/
LOCAL STATUS motFccPktTransmit
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    M_BLK * 	pMblk,          /* pointer to the mBlk */
    UINT8 	fragNum,	/* number of fragments */
    UINT16 	pktType		/* packet type */
    )
    {
    int		  intLevel;		  /* current intr level */
    int		  bufLen;		  /* length of this fragment */
    char	  *pBuf;		  /* pointer to data to be sent */
    FCC_BD	  *pTbd = NULL, *pFirst;  /* pointer to the current ready TBD */
    FCC_TBD_LIST  *pTbdList = NULL;       /* pointer to the TBD list*/
    M_BLK	  *pCurr;		  /* holder for the current mBlk */
    UINT16	  status;
    int		  workIx;

    MOT_FCC_LOG(MOT_FCC_DBG_TX,"motFccPktTransmit...\n",1,2,3,4,5,6);

    ++pDrvCtrl->Stats->numZcopySends;
    pFirst = &pDrvCtrl->tbdBase[pDrvCtrl->tbdIndex];
    status = 0;
    pCurr = pMblk;
    workIx = pDrvCtrl->tbdIndex;

    while (pCurr)
        {
 	/* for each mBlk fragment, set up a BD */

	pTbd = &pDrvCtrl->tbdBase[workIx];
	pTbdList = &pDrvCtrl->pTbdList[workIx];

	pTbdList->pBuf = 0;		/* clear the BD 'list' array */
	pTbdList->info = 0;		/* as we go through 'real' BDs */
	pTbd->bdAddr = (ULONG)pBuf = pCurr->mBlkHdr.mData;
	pTbd->bdLen = bufLen = pCurr->mBlkHdr.mLen;

	if (workIx == (pDrvCtrl->tbdNum - 1))
            {
	    pTbd->word1 = status | M8260_FETH_TBD_W;
	    }
        else
            {
	    pTbd->word1 = status;
	    }
	MOT_FCC_CACHE_FLUSH(pBuf, bufLen);	/* flush cache, if necessary */

        /* bump the working BD index */

	workIx = (workIx + 1) % pDrvCtrl->tbdNum;

	pCurr = (M_BLK *)pCurr->mBlkHdr.mNext;

	if(!status)         
            {
            /* OR in the READY bit for rest of BDs */
            status |= M8260_FETH_TBD_R;
            }	
        }

    /*
     * we exited the above loop with pTbd pointing at the last BD 
     * of the frame, pFirst pointing at the first BD of the frame,
     * and pTbdList pointing at the BD list element corresponding to
     * the last BD for this frame. pTbd and pFirst will be the same
     * when there is only one fragment in the mBlk chain.
     */

    intLevel = intLock();

    /* only the BD list element corresponding to the final 'real' BD */
    /* (the last in a frame) gets any info -- all the others are cleared. */

    /* store the Mblk pointer */

    pTbdList->pBuf = pMblk;	

    /* so we know what to free in ISR */

    pTbdList->info = (pktType | BUF_TYPE_MBLK);

    /* keep accurate count of clean BDs */

    pDrvCtrl->cleanTbdNum -= fragNum;	

    /* only update index while interrupts are disabled */

    pDrvCtrl->tbdIndex = workIx;

    pTbd->word1 |=
		( M8260_FETH_TBD_R
		| M8260_FETH_TBD_L
		| M8260_FETH_TBD_TC
		| M8260_FETH_TBD_I
		| M8260_FETH_TBD_PAD
		);

    if(pTbd != pFirst)
        {
	/*
	 * if pTbd == pFirst, there is only one BD and we just
	 * set its READY bit --  it must not be set twice.
	 */
	pFirst->word1 |= M8260_FETH_TBD_R;
        }

    intUnlock (intLevel);
    CACHE_PIPE_FLUSH (); /* Flush the write pipe */
    return OK;
    }

/******************************************************************************
*
* motFccPktCopyTransmit - copy and transmit a packet
*
* This routine transmits the packet described by the given parameters
* over the network, after copying the mBlk to the driver's buffer. 
* It also updates statistics.
*
* RETURNS: OK, or END_ERR_BLOCK if no resources were available.
*/
LOCAL STATUS motFccPktCopyTransmit
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    M_BLK * 	pMblk,          /* pointer to the mBlk */
    UINT16 	pktType		/* packet type */
    )
    {
    int		  intLevel;	/* current intr level */
    int		  len;		/* length of data to be sent */
    char	  *pBuf;	/* pointer to data to be sent */
    FCC_BD	  *pTbd;	/* pointer to the current ready TBD */
    FCC_TBD_LIST*  pTbdList;	/* pointer to the TBD list*/

    MOT_FCC_LOG (MOT_FCC_DBG_TX, "motFccPktCopyTransmit...\n", 
                 1, 2, 3, 4, 5, 6);

    pTbd	= &pDrvCtrl->tbdBase[pDrvCtrl->tbdIndex];
    pTbdList	= &pDrvCtrl->pTbdList[pDrvCtrl->tbdIndex];

    if(!(pBuf = NET_BUF_ALLOC()))
        {
	return END_ERR_BLOCK;
        }

    ++pDrvCtrl->Stats->numNonZcopySends;

    /* copy data and free the Mblk */

    len = netMblkToBufCopy(pMblk,pBuf,NULL);	
    netMblkClChainFree(pMblk);

    /* flush the cache, if necessary */

    MOT_FCC_CACHE_FLUSH(pBuf,len);

    pTbd->bdLen = len;
    pTbd->bdAddr = (unsigned long)pBuf;
    pTbdList->pBuf = pBuf;			/* store the Cluster pointer */
    pTbdList->info = (pktType | BUF_TYPE_CL);	/* what to free in ISR */

    intLevel = intLock();

    pDrvCtrl->cleanTbdNum -= 1;		/* keep accurate count of clean BDs */
    if (pDrvCtrl->tbdIndex == (pDrvCtrl->tbdNum - 1))
        {
	pTbd->word1 =
		( M8260_FETH_TBD_R
		| M8260_FETH_TBD_L
		| M8260_FETH_TBD_TC
		| M8260_FETH_TBD_I
		| M8260_FETH_TBD_W
		| M8260_FETH_TBD_PAD
		);
        }
    else
        {
	pTbd->word1 =
		( M8260_FETH_TBD_R
		| M8260_FETH_TBD_L
		| M8260_FETH_TBD_TC
		| M8260_FETH_TBD_I
		| M8260_FETH_TBD_PAD
		);

        }

    /* move on to the next element */

    pDrvCtrl->tbdIndex = (pDrvCtrl->tbdIndex + 1) % pDrvCtrl->tbdNum;
    intUnlock (intLevel);
    CACHE_PIPE_FLUSH ();		/* Flush the write pipe */
    return OK;
    }

/*******************************************************************************
* motFccSend - send an Ethernet packet
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

LOCAL STATUS motFccSend
    (
    DRV_CTRL *pDrvCtrl, M_BLK *pMblk
    )
    {
    int		intLevel;	/* current intr level */
    int		fragNum = 0;	/* number of fragments in this mBlk */
    UINT16	pktType = 0;	/* packet type (unicast or multicast) */
    M_BLK	*pCurr;
    BOOL	zFlag;

    MOT_FCC_LOG (MOT_FCC_DBG_TX, "motFccSend...\n", 1, 2, 3, 4, 5, 6);

    END_TX_SEM_TAKE(&pDrvCtrl->endObj, WAIT_FOREVER);

    if (MOT_FCC_FLAG_ISSET(MOT_FCC_POLLING))
        {	/* check device mode */
        netMblkClChainFree(pMblk);		/* free the given mBlk chain */
	END_TX_SEM_GIVE(&pDrvCtrl->endObj);
        errno = EINVAL;
        return ERROR;
        }

    /* check we are not already stalled and have enough resources */

    if (pDrvCtrl->txStall || (pDrvCtrl->cleanTbdNum < (MOT_FCC_TBD_MIN / 2)))
        {
	/* the choice of 1/2 min tbds as a stall threshold is arbitrary. */
	goto tx_stall_exit;
        }

    if (pMblk->mBlkHdr.mData[0] & (UINT8)0x01)
        {
	pktType = PKT_TYPE_MULTI;	/* packet type is multicast */
        }
    else
        {
	pktType = PKT_TYPE_UNI;		/* packet type is unicast */
        }

    fragNum = -1;

    if ((zFlag = pDrvCtrl->zeroBufFlag) )
        {
	if ((pCurr = pMblk) != NULL)
            {
	    while(pCurr)
                {
		if(pCurr->mBlkHdr.mLen)
                    {
		    ++fragNum;
		    pCurr = (M_BLK*)pCurr->mBlkHdr.mNext;
		    }
                else
                    {
		    fragNum = 1;	/* force a copy send */
		    zFlag = 0;		/* force a copy send */
		    break;
		    }
	        }
	    ++fragNum;
	    }
	if(fragNum <= 0 || fragNum > (pDrvCtrl->tbdNum / 2))
            {
	    netMblkClChainFree(pMblk);		/* free the given mBlk chain */
	    END_ERR_ADD(&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
	    END_TX_SEM_GIVE(&pDrvCtrl->endObj);
	    return ERROR;
	    }
        } 
    else
        {
        fragNum = 1;
        }

    if(zFlag && pDrvCtrl->cleanTbdNum > fragNum)
        {
	/* transmit the packet in zero-copy mode */

	if(motFccPktTransmit(pDrvCtrl, pMblk, fragNum, pktType) != OK)
            {
	    goto tx_stall_exit;
	    }
        }
    else
        {
	/* transmit the packet in copy mode */

	if(motFccPktCopyTransmit(pDrvCtrl, pMblk, pktType) != OK)
            {
	    goto tx_stall_exit;
	    }
        }
    END_TX_SEM_GIVE(&pDrvCtrl->endObj);

    MOT_FCC_LOG(MOT_FCC_DBG_TX, "motFccSend...Done\n", 1, 2, 3, 4, 5, 6);

    return OK;

tx_stall_exit:

    END_ERR_ADD(&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
    intLevel = intLock();
    if(!pDrvCtrl->txStall)
        {
	pDrvCtrl->txStall = TRUE;
	++pDrvCtrl->Stats->numStallsEntered;
        }
    intUnlock (intLevel);
    END_TX_SEM_GIVE (&pDrvCtrl->endObj);
    return END_ERR_BLOCK;
    }

/**************************************************************************
*
* motFccTbdInit - initialize the transmit buffer ring
*
* This routine initializes the transmit buffer descriptors ring.
*
* SEE ALSO: motFccRbdInit()
* 
* RETURNS: OK, or ERROR.
*/
LOCAL STATUS motFccTbdInit
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    UINT16		ix;		/* a counter */
    FCC_BD		*pTbd;		/* generic TBD pointer */

    /* carve up the shared-memory region */

    pDrvCtrl->tbdBase = pTbd = (FCC_BD *)pDrvCtrl->pBdBase;

    for(ix = 0; ix < pDrvCtrl->tbdNum; ix++)
        {
	pTbd->word1 = 0;
	++pTbd;
        }

    /* set the wrap bit in last BD */

    --pTbd;
    pTbd->word1 = M8260_FETH_TBD_W;

    /* get a cluster buffer from the pool */

    pDrvCtrl->pTxPollBuf = NET_BUF_ALLOC();

    if (pDrvCtrl->pTxPollBuf == NULL)
	return ERROR;

    return OK;
    }

/******************************************************************************
*
* motFccHandleRXFrames - task-level receive frames processor
*
* This routine is run in netTask's context.
*
* RETURNS: N/A
*/
LOCAL void motFccHandleRXFrames
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    UINT16		rbdStatus = 0;	/* holder for the status of this rbd */
    BPOOL		*bpoolp;
    FCC_BD		*pRbd;
    M_BLK		*pMblk;		/* MBLK to send upstream */
    CL_BLK		*pClBlk;	/* pointer to clBlk */
    int		        len,s;

                 /* A replacement buffer for the current container */

    char		*pBuf;	

    char		*pData;	/* Data pointer for the current container */
    void		*retVal;

    MOT_FCC_LOG (MOT_FCC_DBG_RX, "motFccHandleRecvInt: \n", 0, 0, 0, 0, 0, 0);

    ++pDrvCtrl->Stats->numRXFHandlerEntries;
    /* 
     * empty containers have no buffer associated with them.
     * here, we try to get buffers for as many empties as can.
     */

    while (pDrvCtrl->RxEmpty)
        {
	if (!(pBuf = NET_BUF_ALLOC()))
            {
	    ++pDrvCtrl->Stats->numRXFHandlerEmptyLoopBreaks;
	    break;
	    }

	/* 
         * since the empty list is only manipulated here in this function,
	 * we don't need to lock out interrupts while we remove empties.
	 */

	bpoolp = pDrvCtrl->RxEmpty;
	pDrvCtrl->RxEmpty = bpoolp->next;
	bpoolp->pBuf = pBuf;

	/* ISR code also manipulates the free list, so do intLock(). */

	s = intLock();				/* start critical region */
	bpoolp->next = pDrvCtrl->RxFree;
	pDrvCtrl->RxFree = bpoolp;
	intUnlock(s);				/* end critical region */
        }

    pMblk = NULL;
    pClBlk = NULL;

    while (pDrvCtrl->RxBufCount)
        {
	if (!pMblk)    
            pMblk  = NET_MBLK_ALLOC();
            
	if (!pClBlk)   
            pClBlk = NET_CL_BLK_ALLOC(); 

	if (!pMblk || !pClBlk)
            {

	    /* If we couldn't obtain the resources to send this packet up
	     * the ladder right now, we'll just leave it at the head of the
	     * receive linked list and return.
	     * If either ptr is not NULL, it will be freed outside this loop.
	     */
            /* Tell the mux to give back some clusters */

            pDrvCtrl->lastError.errCode = END_ERR_NO_BUF;
            muxError(&pDrvCtrl->endObj, &pDrvCtrl->lastError);

	    MOT_FCC_RX_MEM_ADD(pDrvCtrl);
	    MOT_FCC_LOG (MOT_FCC_DBG_RX, 
                         "motFccReceive mem problem\n", 0, 0, 0, 0, 0, 0);
	    ++pDrvCtrl->Stats->numRXFHandlerErrQuits;
	    break;
	    }

	s = intLock();			/* start critical region */
	bpoolp = pDrvCtrl->RxHead;
	pDrvCtrl->RxHead = bpoolp->next; /* remove BD container from RX chain */

	if ( --pDrvCtrl->RxBufCount == 0 )
            {
	    pDrvCtrl->RxHead = NULL;
	    pDrvCtrl->RxTail = NULL;
	    }

	intUnlock(s);		/* end critical region */

	/* 
         * At this point, the container is off the input chain 
	 * take what we need out of it and, if the frame is OK, 
         * pass it up to the stack 
         */

	++pDrvCtrl->Stats->numRXFHandlerFramesProcessed;

	pRbd = &bpoolp->desc;

	/* 
         * Get the original cluster buffer address which we must pass to 
         * NET_CL_BLK_JOIN 
         */

	pBuf = bpoolp->pBuf;

	/* 
         * get the actual receive buffer address -- the 
         * rounded-up-to-cache-boundary address 
         */

	pData = (char*)pRbd->bdAddr;

	len = pRbd->bdLen;
	rbdStatus = pRbd->word1;

	if (rbdStatus & (M8260_FETH_RBD_MC|M8260_FETH_RBD_BC))
            {
	    pDrvCtrl->endObj.mib2Tbl.ifInNUcastPkts += 1;
	    }
        else
            {
	    END_ERR_ADD(&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);
	    }

	/* 
         * Handle errata CPM-21... this code should NOT break the driver
	 *  when CPM-21 is ultimately fixed.
	 */

	if ((rbdStatus & 
            (M8260_FETH_RBD_CL | M8260_FETH_RBD_CR)) == M8260_FETH_RBD_CL)
            {
	    rbdStatus &= (UINT16)(~M8260_FETH_RBD_CL);
	    }

	if(rbdStatus & MOT_FCC_RBD_ERR)
            {
	    ++pDrvCtrl->Stats->numRXFHandlerFramesRejected;
	    logMsg("RX addr = 0x%x  Len = 0x%x  Stat = 0x%x\n",
				(unsigned int)pData,len,rbdStatus,0,0,0);
	    /* 
             * This container already has a buffer in it, but contains a bad 
             * frame just put it back onto the free chain and we'll re-use 
             * the buffer 
             */

	    s = intLock();			/* start critical region */

	    /* memset((void*)&bpoolp->desc,0,8); */

	    bpoolp->next = pDrvCtrl->RxFree;
	    pDrvCtrl->RxFree = bpoolp;
	    intUnlock(s);			/* end critical region */

	    continue;	/* go back and process remaining (if any) containers */

	    /* 
             * note that pMblk and pClBlk are not NULL and will be used in 
             * next iteration 
             */
	    }

	NET_CL_BLK_JOIN (pClBlk, pBuf, MOT_FCC_MAX_CL_LEN, retVal);
	NET_MBLK_CL_JOIN(pMblk, pClBlk, retVal);

	/* 
         * After joining the original cluster address, adjust the M_BLK to 
         * account for alignment 
         */

	pMblk->mBlkHdr.mData	+= ( (unsigned int)pData - (unsigned int)pBuf );
	pMblk->mBlkHdr.mFlags 	|= M_PKTHDR;	/* set the packet header flag */
	pMblk->mBlkHdr.mLen     = (len - MII_CRC_LEN) & ~0xc000;    
	pMblk->mBlkPktHdr.len   = pMblk->mBlkHdr.mLen;

	/* Make cache consistent with memory */

	MOT_FCC_CACHE_INVAL(pData, pMblk->mBlkHdr.mLen);

        /* send the frame to the upper layer */

	END_RCV_RTN_CALL(&pDrvCtrl->endObj,pMblk);	

	CACHE_PIPE_FLUSH();			/* Flush the write pipe */

	/* at this point, we're done with pMblk and pClBlk -- NULL them out */

	pMblk = NULL;
	pClBlk = NULL;

        /* Now try to allocate a replacement buffer for the container */

	pBuf = NET_BUF_ALLOC();	

	s = intLock();				/* start critical region */

	/* memset((void*)&bpoolp->desc,0,8); */

	if ((bpoolp->pBuf = pBuf))
            {		
            /* buf_alloc succeeded */

	    bpoolp->next = pDrvCtrl->RxFree;	/* put container on free list */
	    pDrvCtrl->RxFree = bpoolp;
	    }
        else
            {
	    bpoolp->next = pDrvCtrl->RxEmpty;  /* buf_alloc failed */
	    pDrvCtrl->RxEmpty = bpoolp;        /* put container on empty list */
	    }
        intUnlock(s);			/* end critical region */
        }
    s = intLock();			/* start critical region */
   
    pDrvCtrl->rxHandling = FALSE; 

    ++pDrvCtrl->Stats->numRXFHandlerExits;

    intUnlock(s);			/* end critical region */

    /* if the only (or last) frame had an error, these pointers
     * will not have been freed so do it now...
     */

    if(pMblk)	
        NET_MBLK_FREE(pMblk);

    if(pClBlk)	
        NET_CL_BLK_FREE(pClBlk);

    MOT_FCC_LOG (MOT_FCC_DBG_RX, "motFccHandleRecvInt... Done, \n", 
                 0, 0, 0, 0, 0, 0);
    }

/**************************************************************************
*
* motFccRbdInit - initialize the receive buffer ring
*
* This routine initializes the receive buffer descriptors ring.
*
* SEE ALSO: motFccTbdInit()
* 
* RETURNS: OK, or ERROR if not enough clusters were available.
*/
LOCAL STATUS motFccRbdInit
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    char	*pBuf;		/* a rx data pointer */
    FCC_BD	*pRbd;		/* generic rbd pointer */
    BPOOL	*bpoolp;
    void	**pRbuf;
    int	        ix,nContainers;
    nContainers = pDrvCtrl->rbdNum;

#define EXTRA_CONTAINERS 1
#if EXTRA_CONTAINERS
    nContainers += pDrvCtrl->rbdNum /* / 2 */ ;
#endif

    if (!pDrvCtrl->RxContainers)
        {
        pDrvCtrl->RxContainers = (BPOOL *)calloc(sizeof(BPOOL),nContainers + 1);
	pDrvCtrl->RxFree = NULL;
	pDrvCtrl->RxEmpty = NULL;
	pDrvCtrl->RxHead = NULL;
	pDrvCtrl->RxTail = NULL;
        }

    for (ix = 0; ix < nContainers; ix++) 
        {
	if (!(pBuf = NET_BUF_ALLOC()))
            {
	    logMsg("motFCCRbdInit() need more clusters\n",0,0,0,0,0,0);
	    return ERROR;
	    }

	bpoolp = &pDrvCtrl->RxContainers[ix];
	bpoolp->pBuf = pBuf;
	bpoolp->next = pDrvCtrl->RxFree;
	pDrvCtrl->RxFree = bpoolp;
        }

    /* the receive ring is located right after the transmit ring */

    pDrvCtrl->rbdBase = (FCC_BD *) (pDrvCtrl->pBdBase + 
                        (MOT_FCC_TBD_SZ * pDrvCtrl->tbdNum));

    pRbd = pDrvCtrl->rbdBase;
    pRbuf = pDrvCtrl->rBufList;

    for (ix = 0; ix < pDrvCtrl->rbdNum; ix++) 
        {
	if (!(pBuf = NET_BUF_ALLOC()))
            {
	    logMsg("motFCCRbdInit() need more clusters\n",0,0,0,0,0,0);
	    return ERROR;
	    }

	pRbd->bdLen = 0;
	*pRbuf = pBuf;
	pRbd->bdAddr = ROUND_UP( (unsigned long)pBuf, MOT_FCC_BUF_ALIGN );
	pRbd->word1 = (M8260_FETH_RBD_E | M8260_FETH_RBD_I);
	++pRbd;
	++pRbuf;
        }

    --pRbd;
    pRbd->word1 |= M8260_FETH_RBD_W;
    return OK;
    }

/**************************************************************************
*
* motFccBufFree - free the receive and transmit buffers
*
* This routine frees the netpool clusters associated with the receive 
* buffer descriptors ring. It is called by motFccStop(), in order to
* properly release the driver's resources when the device is stopped.
*
* SEE ALSO: motFccStop()
* 
* RETURNS: OK, always.
*/
LOCAL STATUS motFccBdFree
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    MOT_FCC_RBD_ID	pRbd = NULL;	/* generic rbd pointer */
    void		*pBuf;
    UINT16		ix;		/* a counter */

    pRbd = (MOT_FCC_RBD_ID) (pDrvCtrl->rbdBase);

    for (ix = 0; ix < pDrvCtrl->rbdNum; ix++) 
        {
	/* return the cluster buffer to the pool */

	if((pBuf = pDrvCtrl->rBufList[ix]))
	    NET_BUF_FREE(pBuf);
        }

    /* free lists and clusters */

    for (ix = 0; ix < pDrvCtrl->tbdNum; ix++) 
        {
	if((pBuf = pDrvCtrl->pTbdList[ix].pBuf))
            {
	    /* free the Mblk or the cluster, whichever is appropriate */

	    if(pDrvCtrl->pTbdList[ix].info & BUF_TYPE_MBLK)
                {
		netMblkClChainFree((M_BLK*)pBuf);
	        }
            else if(pDrvCtrl->pTbdList[ix].info & BUF_TYPE_CL)
                {
		NET_BUF_FREE(pBuf);
	        }
	    }
	pDrvCtrl->pTbdList[ix].pBuf = NULL;
	pDrvCtrl->pTbdList[ix].info = 0;
        }

    /* free the transmit poll buffer */

    if ((pDrvCtrl->pTxPollBuf) != NULL)
	NET_BUF_FREE ((UCHAR *) (pDrvCtrl->pTxPollBuf));
    return OK;
    }

/**************************************************************************
*
* motFccInit - initialize the chip 
*
* This routine programs both the internal ram registers and the parameter
* RAM in the DPRAM for Ethernet operation. It calls motFccPramInit () and
* motFccIramInit () to do the job.
*
* SEE ALSO: motFccPramInit (), motFccIramInit ().
* 
* RETURNS: OK, or ERROR.
*/
LOCAL STATUS motFccInit
    (
    DRV_CTRL *pDrvCtrl
    )
    {

    /* initialize the parameter RAM for Ethernet operation */

    if (motFccPramInit (pDrvCtrl) != OK)
	return ERROR;

    /* initialize the internal RAM for Ethernet operation */

    if (motFccIramInit (pDrvCtrl) != OK)
	return ERROR;

    return OK;
    }

/**************************************************************************
*
* motFccPramInit - initialize the parameter RAM for this FCC
*
* This routine programs the parameter ram registers for this FCC and set it
* to work for Ethernet operation. It follows the initialization sequence 
* recommended in the MPC8260 PowerQuicc II User's Manual.
*
* SEE ALSO: motFccIramInit (), motFccInit ().
* 
* RETURNS: OK, always.
*/
LOCAL STATUS motFccPramInit
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    UINT32	    fcrVal;
    FCC_PARAM_T	    *pParam;
    FCC_ETH_PARAM_T *pEthPar;

    /* get to the beginning of the parameter area */

    pParam = pDrvCtrl->fccPar;
    pEthPar = pDrvCtrl->fccEthPar;
    memset(pParam,0,sizeof(*pParam));

    /* figure out the value for the FCR registers */

    fcrVal = MOT_FCC_FCR_DEF_VAL;

    if (MOT_FCC_USR_FLAG_ISSET (MOT_FCC_USR_BD_LBUS))
        {
	fcrVal |= M8260_FCR_BDB;
        }
    if (MOT_FCC_USR_FLAG_ISSET (MOT_FCC_USR_BUF_LBUS))
        {
	fcrVal |= M8260_FCR_DTB;
        }
    fcrVal <<= M8260_FCR_SHIFT;

    /* Allocate DPRAM memory for the riptr & tiptr */

    if (MOT_FCC_USR_FLAG_ISSET(MOT_FCC_USR_DPRAM_ALOC))
	{
	/* Use auto allocation */

        /* Test that function pointers are populated. */

        if (_func_m82xxDpramFccMalloc == NULL || _func_m82xxDpramFree == NULL ||
            _func_m82xxDpramFccFree == NULL )
            {

            return ERROR;
            }
        else
            {
	    if ((pParam->riptr = (0xffff & 
                                 (ULONG)_func_m82xxDpramFccMalloc(32,32))) == 0)
                {
	        return ERROR;
                }    

	    if ((pParam->tiptr = (0xffff & 
                                 (ULONG)_func_m82xxDpramFccMalloc(32,32))) == 0)
                {
	        return ERROR;
                }
            } 
	}
	else
	{
	/* Use hardcoded allocation */

        pParam->riptr	= MOT_FCC_RIPTR_VAL;
        pParam->tiptr	= MOT_FCC_TIPTR_VAL;
	}

    pParam->mrblr	= 1536; /* MII_ETH_MAX_PCK_SZ; */
    pParam->rstate	= fcrVal;
    pParam->rbase	= (unsigned long)pDrvCtrl->rbdBase;
    pParam->tstate	= fcrVal;
    pParam->tbase	= (unsigned long)pDrvCtrl->tbdBase;

    pEthPar->c_mask	= MOT_FCC_C_MASK_VAL;
    pEthPar->c_pres	= MOT_FCC_C_PRES_VAL;
    pEthPar->ret_lim	= /* 4 */ MOT_FCC_RET_LIM_VAL;
    pEthPar->maxflr	= MII_ETH_MAX_PCK_SZ;
    pEthPar->minflr	= MOT_FCC_MINFLR_VAL;
    pEthPar->maxd1	= MOT_FCC_MAXD_VAL;
    pEthPar->maxd2	= MOT_FCC_MAXD_VAL;
    pEthPar->pad_ptr	= MOT_FCC_PAD_VAL;

    /* program the individual physical address */

    if (motFccAddrSet (pDrvCtrl, (UCHAR *) MOT_FCC_ADDR_GET (&pDrvCtrl->endObj),
                       M8260_FCC_PADDR_H_OFF) != OK)
        {
	return ERROR;
        }

    CACHE_PIPE_FLUSH (); /* Flush the write pipe */
    return OK;
    }

/**************************************************************************
*
* motFccIramInit - initialize the internal RAM for this FCC
*
* This routine programs the internal ram registers for this FCC and set it
* to work for Ethernet operation. It follows the initialization sequence 
* recommended in the MPC8260 PowerQuicc II User's Manual. It also issues
* a rx/tx init command to the CP.
*
* SEE ALSO: motFccPramInit (), motFccInit ().
* 
* RETURNS: OK, always.
*/
LOCAL STATUS motFccIramInit 
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    UINT32	fccIramAddr = 0;		/* a convenience */
    UINT8	command = 0;			/* command to issue to the CP */

    fccIramAddr = pDrvCtrl->fccIramAddr;

    /* 
     * program the GSMR, but enable neither the transmitter
     * nor the receiver for now.
     */

#ifdef INCLUDE_GFMR_TCI
    MOT_FCC_REG_LONG_WR (fccIramAddr + M8260_FCC_GFMR_OFF,
			 (M8260_GFMR_ETHERNET | M8260_GFMR_NORM
			  | M8260_GFMR_RENC_NRZ | M8260_GFMR_TCI));
#else
    /* Leave TCI low to correct problem in 100Base-TX mode */

    MOT_FCC_REG_LONG_WR (fccIramAddr + M8260_FCC_GFMR_OFF,
			 (M8260_GFMR_ETHERNET | M8260_GFMR_NORM
			  | M8260_GFMR_RENC_NRZ));
#endif

    /* program the DSR */

    MOT_FCC_REG_WORD_WR (fccIramAddr + M8260_FCC_FDSR_OFF, MOT_FCC_DSR_VAL);

    /* zero-out the TODR for the sake of clarity */

    MOT_FCC_REG_WORD_WR (fccIramAddr + M8260_FCC_FTODR_OFF, MOT_FCC_CLEAR_VAL);

    /* the value for the FPSMR */

    if (motFccFpsmrValSet (pDrvCtrl) != OK)
	return ERROR;

    /* 
     * we enable the following event interrupts:
     * graceful transmit command;
     * tx error;
     * rx frame;
     * busy condition;
     * tx buffer, although we'll only enable the last TBD in a frame
     * to generate interrupts. See motFccSend ();
     */

    pDrvCtrl->intMask = (M8260_FEM_ETH_GRA | M8260_FEM_ETH_TXE
			| M8260_FEM_ETH_RXF | M8260_FEM_ETH_BSY
			| M8260_FEM_ETH_TXB);

    MOT_FCC_REG_WORD_WR (fccIramAddr + M8260_FCC_FCCMR_OFF, 
                         (pDrvCtrl->intMask));

    /* clear all events */

    MOT_FCC_REG_WORD_WR (fccIramAddr + M8260_FCC_FCCER_OFF, MOT_FCC_FCCE_VAL);

    /* issue a init rx/tx command to the CP */

    command = M8260_CPCR_RT_INIT;

    return motFccCpcrCommand (pDrvCtrl, command);
    }

/**************************************************************************
*
* motFccAddrSet - set an address to some internal register
*
* This routine writes the address pointed to by <pAddr> to some FCC's internal 
* register. It may be used to set the individual physical address, or
* an address to be added to the hash table filter. It assumes that <offset>
* is the offset in the parameter RAM to the highest-order halfword that
* will contain the address.
* 
* RETURNS: OK, always.
*/

LOCAL STATUS motFccAddrSet
    (
    DRV_CTRL *  pDrvCtrl,	/* pointer to DRV_CTRL structure */
    UCHAR *	pAddr,		/* address to write to register */
    UINT32	offset		/* offset to highest-word in register */
    )
    {
    UINT16	word;				/* a convenience */
    UINT32	fccPramAddr = 0;		/* a convenience */

    /* get to the beginning of the parameter area */

    fccPramAddr = pDrvCtrl->fccPramAddr;

    word = (((UINT16) (pAddr [5])) << 8) | (pAddr [4]);
    MOT_FCC_REG_WORD_WR (fccPramAddr + offset, 
			 word);

    word = (((UINT16) (pAddr [3])) << 8) | (pAddr [2]);
    MOT_FCC_REG_WORD_WR (fccPramAddr + offset + 2, 
			 word);

    word = (((UINT16) (pAddr [1])) << 8) | (pAddr [0]);
    MOT_FCC_REG_WORD_WR (fccPramAddr + offset + 4, 
			 word);

    return (OK);
    }

/**************************************************************************
*
* motFccFpsmrValSet - set the proper value for the FPSMR
*
* This routine finds out the proper value to be programmed in the
* FPSMR register by looking at some of the user/driver flags. It deals
* with options like promiscous mode, full duplex, loopback mode, RMON
* mode, and heartbeat control. It then writes to the FPSMR.
*
* SEE ALSO: motFccIramInit (), motFccInit ()
* 
* RETURNS: OK, always.
*/

LOCAL STATUS motFccFpsmrValSet
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    UINT32	fpsmrVal = 0;			/* a convenience */
    UINT32	fccIramAddr = 0;		/* a convenience */

    /* get to the beginning of the internal memory for this FCC */

    fccIramAddr = pDrvCtrl->fccIramAddr;

    /* use the standard CCITT CRC */

    fpsmrVal |= M8260_FPSMR_ETH_CRC_32;

    /* set the heartbeat check if the user wants it */

    if (MOT_FCC_USR_FLAG_ISSET (MOT_FCC_USR_HBC)) 
        {
	fpsmrVal |= M8260_FPSMR_ETH_HBC;

	MOT_FCC_LOG (MOT_FCC_DBG_START, 
                     ("motFccFpsmrValSet: heartbeat control \n"), 
                     0, 0, 0, 0, 0, 0);
        } 
    else
        {
	fpsmrVal &= ~M8260_FPSMR_ETH_HBC;
        }

    /* set RMON mode if the user requests it */

    if (MOT_FCC_USR_FLAG_ISSET (MOT_FCC_USR_RMON)) 
        {
	fpsmrVal |= M8260_FPSMR_ETH_MON;

	MOT_FCC_LOG (MOT_FCC_DBG_START, 
                     ("motFccFpsmrValSet: heartbeat control \n"), 
                     0, 0, 0, 0, 0, 0);
        }
    else
        {
	fpsmrVal &= ~M8260_FPSMR_ETH_MON;
        }

    /* deal with promiscous mode */

    if (MOT_FCC_FLAG_ISSET (MOT_FCC_PROM)) 
        {
	fpsmrVal |= M8260_FPSMR_ETH_PRO;

	MOT_FCC_LOG (MOT_FCC_DBG_START, 
                     ("motFccFpsmrValSet: promiscous mode \n"), 
                     0, 0, 0, 0, 0, 0);
        }
    else
        {
	fpsmrVal &= ~M8260_FPSMR_ETH_PRO;
        }

    /* 
     * Handle user's diddling with LPB first. That way, if we must 
     * set LPB due to FD it will stick. 
     */

    /* 
     * If the user wishes to go in external loopback mode,
     * enable it. Do not enable internal loopback mode.
     */

    if (MOT_FCC_USR_FLAG_ISSET (MOT_FCC_USR_LOOP)) 
        {
	fpsmrVal |= M8260_FPSMR_ETH_LPB;

	MOT_FCC_LOG (MOT_FCC_DBG_START, 
                     ("motFccFpsmrValSet: loopback mode \n"), 0, 0, 0, 0, 0, 0);
        }
    else
        {
	fpsmrVal &= ~M8260_FPSMR_ETH_LPB;
        }

    /* 
     * Enable full duplex mode if the PHY was configured to work in 
     * full duplex mode. 
     */

    if (MOT_FCC_PHY_FLAGS_ISSET (MII_PHY_FD)) 
        {
	fpsmrVal |= (M8260_FPSMR_ETH_LPB | M8260_FPSMR_ETH_FDE);

	MOT_FCC_LOG (MOT_FCC_DBG_START, 
                     ("motFccFpsmrValSet: full duplex \n"), 0, 0, 0, 0, 0, 0);
        }
    else
        {
	fpsmrVal &= ~M8260_FPSMR_ETH_FDE;
        }

    MOT_FCC_REG_LONG_WR (fccIramAddr + M8260_FCC_FPSMR_OFF, fpsmrVal);
    return OK;
    }

/*******************************************************************************
* motFccIoctl - interface ioctl procedure
*
* Process an interface ioctl request.
*
* RETURNS: OK, or ERROR.
*/

LOCAL int motFccIoctl
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

    MOT_FCC_LOG (MOT_FCC_DBG_IOCTL,
		 ("Ioctl unit=0x%x cmd=%d data=0x%x\n"),
		 pDrvCtrl->unit, cmd, (int)data, 0, 0, 0);

    switch (cmd)
        {
        case EIOCSADDR:
            if (data == NULL)
                error = EINVAL;
            else
                {
                /* Copy and install the new address */

		bcopy ((char *) data,
		       (char *) MOT_FCC_ADDR_GET (&pDrvCtrl->endObj),
		       MOT_FCC_ADDR_LEN_GET (&pDrvCtrl->endObj));

		/* stop the FCC */

		if (motFccStop (pDrvCtrl) != OK)
		    return (ERROR);

		/* program the individual enet address */

		if (motFccAddrSet (pDrvCtrl, (UCHAR *) data, 
				   M8260_FCC_PADDR_H_OFF) 
		    != OK)
		    return (ERROR);

		/* restart the FCC */

		if (motFccStart (pDrvCtrl) != OK)
		    return (ERROR);
                }

            break;

        case EIOCGADDR:                      
            if (data == NULL)
                error = EINVAL;
            else
		bcopy ((char *) MOT_FCC_ADDR_GET (&pDrvCtrl->endObj),
		       (char *) data,
		       MOT_FCC_ADDR_LEN_GET (&pDrvCtrl->endObj));

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

            MOT_FCC_LOG (MOT_FCC_DBG_IOCTL, ("endFlags=0x%x \n"),
					     END_FLAGS_GET(pEndObj), 
					     0, 0, 0, 0, 0);

            /* handle IFF_PROMISC */

            savedFlags = MOT_FCC_FLAG_GET();

            if (END_FLAGS_ISSET (IFF_PROMISC))
		{
                MOT_FCC_FLAG_SET (MOT_FCC_PROM);

		if ((MOT_FCC_FLAG_GET () != savedFlags) &&
		    (END_FLAGS_GET (pEndObj) & IFF_UP))
		    {
		    /* config down */

		    END_FLAGS_CLR (pEndObj, IFF_UP | IFF_RUNNING); 

		    /* program the FPSMR to promiscous mode */

		    if (motFccFpsmrValSet (pDrvCtrl) == ERROR)
			return (ERROR);

		    /* config up */

		    END_FLAGS_SET (pEndObj, IFF_UP | IFF_RUNNING); 
		    }
		}
            else
                MOT_FCC_FLAG_CLEAR (MOT_FCC_PROM);

            MOT_FCC_LOG (MOT_FCC_DBG_IOCTL, ("EIOCSFLAGS: 0x%x: 0x%x\n"),
					     pEndObj->flags, savedFlags, 
					     0, 0, 0, 0);

            break;

        case EIOCGFLAGS:
            MOT_FCC_LOG (MOT_FCC_DBG_IOCTL, ("EIOCGFLAGS: 0x%x: 0x%x\n"),
					    pEndObj->flags, *(long *)data, 
					    0, 0, 0, 0);

            if (data == NULL)
                error = EINVAL;
            else
                *(long *)data = END_FLAGS_GET(pEndObj);

            break;

        case EIOCMULTIADD:
            error = motFccMCastAddrAdd (pDrvCtrl, (UCHAR *) data);
            break;

        case EIOCMULTIDEL:
            error = motFccMCastAddrDel (pDrvCtrl, (UCHAR *) data);
            break;

        case EIOCMULTIGET:
            error = motFccMCastAddrGet (pDrvCtrl, (MULTI_TABLE *) data);
            break;

        case EIOCPOLLSTART:
	    MOT_FCC_LOG (MOT_FCC_DBG_POLL, ("IOCTL call motFccPollStart\n"), 
					    0, 0, 0, 0, 0, 0);

            motFccPollStart (pDrvCtrl);
            break;

        case EIOCPOLLSTOP:
	    MOT_FCC_LOG (MOT_FCC_DBG_POLL, ("IOCTL call motFccPollStop\n"), 
					    0, 0, 0, 0, 0, 0);

            motFccPollStop (pDrvCtrl);
            break;

        case EIOCGMIB2:  
            if (data == NULL)
                error=EINVAL;
            else
		bcopy ((char *) &pEndObj->mib2Tbl, (char *) data,
			sizeof (pEndObj->mib2Tbl));

            break;

        default:
            MOT_FCC_LOG (MOT_FCC_DBG_IOCTL, ("INVALID IO COMMAND!! \n"),
					     0, 0, 0, 0, 0, 0);
            error = EINVAL;
        }

    return (error);
    }

/**************************************************************************
*
* motFccCpcrCommand - issue a command to the CP
*
* This routine issues the command specified in <command> to the Communication
* Processor (CP).
*
* RETURNS: OK, or ERROR if the command was unsuccessful.
*
*/
LOCAL STATUS motFccCpcrCommand
    (
    DRV_CTRL *pDrvCtrl, UINT8 command
    )
    {
    UINT32	    immrVal = pDrvCtrl->immrVal; /* holder for the immr value */
    volatile UINT32 cpcrVal = 0;		 /* a convenience */
    UINT32	    ix = 0;	                 /* a counter */
    UINT8	    fccNum = pDrvCtrl->fccNum;	 /* holder for the fcc number */
    UINT8	    fcc = fccNum - 1;		 /* a convenience */

    /* wait until the CP is clear */

    do {
	MOT_FCC_REG_LONG_RD (M8260_CPCR (immrVal), cpcrVal);

	if (ix++ == M8260_CPCR_LATENCY)
	    break;
    } while (cpcrVal & M8260_CPCR_FLG);

    if (ix >= M8260_CPCR_LATENCY) 
        {
        MOT_FCC_LOG (MOT_FCC_DBG_ANY, 
                     "motFccCpcrCommand: cpcr problem \n", 0, 0, 0, 0, 0, 0);
	return ERROR;
        }

    /* issue the command to the CP */

    cpcrVal = (M8260_CPCR_OP (command)
	       | M8260_CPCR_SBC (M8260_CPCR_SBC_FCC1 | (fcc * 0x1))
	       | M8260_CPCR_PAGE (M8260_CPCR_PAGE_FCC1 | (fcc * 0x1))
	       | M8260_CPCR_MCN (M8260_CPCR_MCN_ETH)
	       | M8260_CPCR_FLG);

    MOT_FCC_LOG (MOT_FCC_DBG_START, 
                 "motFccCpcrCommand: cpcr=0x%x \n", cpcrVal, 0, 0, 0, 0, 0);

    MOT_FCC_REG_LONG_WR (M8260_CPCR (immrVal), cpcrVal);

    CACHE_PIPE_FLUSH ();		/* Flush the write pipe */

    /* wait until the CP is clear */

    ix = 0;

    do {
	MOT_FCC_REG_LONG_RD (M8260_CPCR (immrVal), cpcrVal);

	if (ix++ == M8260_CPCR_LATENCY)
	    break;

    } while (cpcrVal & M8260_CPCR_FLG) ;

    if (ix >= M8260_CPCR_LATENCY) 
        {
	MOT_FCC_LOG (MOT_FCC_DBG_ANY, 
                     "motFccCpcrCommand: cpcr problem \n", 0, 0, 0, 0, 0, 0);
	return ERROR;
        }

    MOT_FCC_LOG (MOT_FCC_DBG_START, 
                 "motFccCpcrCommand: done \n", 0, 0, 0, 0, 0, 0);
    return OK;
    }

/**************************************************************************
*
* motFccMiiRead - read the MII interface
*
* This routine reads the register specified by <phyReg> in the PHY device
* whose address is <phyAddr>. The value read is returned in the location
* pointed to by <miiData>.
*
* SEE ALSO: motFccMiiWrite()
* 
* RETURNS: OK, or ERROR.
*
*/
LOCAL STATUS motFccMiiRead
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to DRV_CTRL structure */
    UINT8	phyAddr,	/* the PHY being read */
    UINT8	regAddr,	/* the PHY's register being read */
    UINT16 *	miiData		/* value read from the MII interface */
    )
    {
    UINT16 	temp = 0;	/* temporary variable */

    /* write the preamble pattern first */

    MOT_FCC_MII_WR (MII_MF_PREAMBLE, MII_MF_PREAMBLE_LEN);

    /* write the start of frame */

    MOT_FCC_MII_WR (MII_MF_ST, MII_MF_ST_LEN);

    /* write the operation code */

    MOT_FCC_MII_WR (MII_MF_OP_RD, MII_MF_OP_LEN);

    /* write the PHY address */

    MOT_FCC_MII_WR (phyAddr, MII_MF_ADDR_LEN);

    /* write the PHY register */

    MOT_FCC_MII_WR (regAddr, MII_MF_REG_LEN);

    /* write the turnaround pattern */

    SYS_FCC_MII_BIT_WR ((INT32) NONE);

    SYS_FCC_MII_BIT_WR (0);

    /* read the data on the MDIO */

    MOT_FCC_MII_RD (temp, MII_MF_DATA_LEN);

    /* leave it in idle state */

    SYS_FCC_MII_BIT_WR ((INT32) NONE);

    *miiData = temp;

    MOT_FCC_LOG (MOT_FCC_DBG_MII, ("motFccMiiRead reg=%d 
				   PHY=%d data=0x%x\n"),
				   regAddr, phyAddr, *miiData, 0, 0, 0);

    return (OK);
    }

/**************************************************************************
*
* motFccMiiWrite - write to the MII register
*
* This routine writes the register specified by <phyReg> in the PHY device,
* whose address is <phyAddr>, with the 16-bit value included in <miiData>.
*
* SEE ALSO: motFccMiiRead()
* 
* RETURNS: OK, or ERROR.
*/
LOCAL STATUS motFccMiiWrite
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to DRV_CTRL structure */
    UINT8	phyAddr,	/* the PHY being written */
    UINT8	regAddr,	/* the PHY's register being written */
    UINT16 	miiData		/* value written to the MII interface */
    )
    {

    /* write the preamble pattern first */

    MOT_FCC_MII_WR (MII_MF_PREAMBLE, MII_MF_PREAMBLE_LEN);

    /* write the start of frame */

    MOT_FCC_MII_WR (MII_MF_ST, MII_MF_ST_LEN);

    /* write the operation code */

    MOT_FCC_MII_WR (MII_MF_OP_WR, MII_MF_OP_LEN);

    /* write the PHY address */

    MOT_FCC_MII_WR (phyAddr, MII_MF_ADDR_LEN);

    /* write the PHY register */

    MOT_FCC_MII_WR (regAddr, MII_MF_REG_LEN);

    /* write the turnaround pattern */

    SYS_FCC_MII_BIT_WR (1);

    SYS_FCC_MII_BIT_WR (0);

    /* write the data on the MDIO */

    MOT_FCC_MII_WR (miiData, MII_MF_DATA_LEN);

    /* leave it in idle state @@@ */

    SYS_FCC_MII_BIT_WR ((INT32) NONE);

    MOT_FCC_LOG (MOT_FCC_DBG_MII, ("motFccMiiWrite reg=%d 
				   PHY=%d data=0x%x\n"),
				   regAddr, phyAddr, miiData, 0, 0, 0);

    return (OK);
    }

/**************************************************************************
*
* motFccPhyPreInit - initialize some fields in the phy info structure
*
* This routine initializes some fields in the phy info structure, 
* for use of the phyInit() routine.
*
* RETURNS: OK, or ERROR if could not obtain memory.
*/

LOCAL STATUS motFccPhyPreInit
    (
    DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    /* set some default values */

    pDrvCtrl->phyInfo->pDrvCtrl = (void *) pDrvCtrl;

    /* in case of link failure, set a default mode for the PHY */

    pDrvCtrl->phyInfo->phyFlags = MII_PHY_DEF_SET;

    pDrvCtrl->phyInfo->phyWriteRtn = (FUNCPTR) motFccMiiWrite;
    pDrvCtrl->phyInfo->phyReadRtn = (FUNCPTR) motFccMiiRead;
    pDrvCtrl->phyInfo->phyDelayRtn = (FUNCPTR) taskDelay;
    pDrvCtrl->phyInfo->phyMaxDelay = MII_PHY_DEF_DELAY;
    pDrvCtrl->phyInfo->phyDelayParm = 1;

    /* handle some user-to-physical flags */

    if (!(MOT_FCC_USR_FLAG_ISSET (MOT_FCC_USR_PHY_NO_AN)))
	MOT_FCC_PHY_FLAGS_SET (MII_PHY_AUTO);
    else
	MOT_FCC_PHY_FLAGS_CLEAR (MII_PHY_AUTO);

    if (MOT_FCC_USR_FLAG_ISSET (MOT_FCC_USR_PHY_TBL))
	MOT_FCC_PHY_FLAGS_SET (MII_PHY_TBL);
    else
	MOT_FCC_PHY_FLAGS_CLEAR (MII_PHY_TBL);

    if (!(MOT_FCC_USR_FLAG_ISSET (MOT_FCC_USR_PHY_NO_100)))
	MOT_FCC_PHY_FLAGS_SET (MII_PHY_100);
    else
	MOT_FCC_PHY_FLAGS_CLEAR (MII_PHY_100);

    if (!(MOT_FCC_USR_FLAG_ISSET (MOT_FCC_USR_PHY_NO_FD)))
	MOT_FCC_PHY_FLAGS_SET (MII_PHY_FD);
    else
	MOT_FCC_PHY_FLAGS_CLEAR (MII_PHY_FD);

    if (!(MOT_FCC_USR_FLAG_ISSET (MOT_FCC_USR_PHY_NO_10)))
	MOT_FCC_PHY_FLAGS_SET (MII_PHY_10);
    else
	MOT_FCC_PHY_FLAGS_CLEAR (MII_PHY_10);

    if (!(MOT_FCC_USR_FLAG_ISSET (MOT_FCC_USR_PHY_NO_HD)))
	MOT_FCC_PHY_FLAGS_SET (MII_PHY_HD);
    else
	MOT_FCC_PHY_FLAGS_CLEAR (MII_PHY_HD);

    /* mark it as pre-initilized */

    MOT_FCC_PHY_FLAGS_SET (MII_PHY_PRE_INIT);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("motFccPhyPreInit phyInfo = 0x%x read=0x%x
				   write=0x%x tbl=0x%x addr=0x%x 
				   flags=0x%x \n"), 
				   (int) pDrvCtrl->phyInfo,
				   (int) pDrvCtrl->phyInfo->phyReadRtn,
				   (int) pDrvCtrl->phyInfo->phyWriteRtn,
				   (int) pDrvCtrl->phyInfo->phyAnOrderTbl,
				   (int) pDrvCtrl->phyInfo->phyAddr, 
				   (int) pDrvCtrl->phyInfo->phyFlags);

    return (OK);
    }

/*******************************************************************************
* motFccMCastAddrAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.
*
* SEE ALSO: motFccMCastAddrDel() motFccMCastAddrGet()
* 
* RETURNS: OK or ERROR.
*/
LOCAL STATUS motFccMCastAddrAdd
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UCHAR *      pAddr		/* address to be added */
    )
    {
    int		retVal;		/* convenient holder for return value */

    MOT_FCC_LOG (MOT_FCC_DBG_IOCTL, ("motFccMCastAddrAdd addr = 0x%x 0x%x
				     0x%x 0x%x 0x%x 0x%x\n"), 
				     (int) *(UCHAR*) (pAddr + 0),
				     (int) *(UCHAR*) (pAddr + 1), 
				     (int) *(UCHAR*) (pAddr + 2), 
				     (int) *(UCHAR*) (pAddr + 3), 
				     (int) *(UCHAR*) (pAddr + 4), 
				     (int) *(UCHAR*) (pAddr + 5));

    retVal = etherMultiAdd (&pDrvCtrl->endObj.multiList, pAddr);

    if (retVal == ENETRESET)
	{
        pDrvCtrl->endObj.nMulti++;
 
	return (motFccHashTblAdd (pDrvCtrl, pAddr));
	}

    return ((retVal == OK) ? OK : ERROR);
    }

/*****************************************************************************
*
* motFccMCastAddrDel - delete a multicast address for the device
*
* This routine deletes a multicast address from the current list of
* multicast addresses.
*
* SEE ALSO: motFccMCastAddrAdd() motFccMCastAddrGet()
* 
* RETURNS: OK or ERROR.
*/
LOCAL STATUS motFccMCastAddrDel
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UCHAR *      pAddr		/* address to be deleted */
    )
    {
    int		retVal;		/* convenient holder for return value */

    MOT_FCC_LOG (MOT_FCC_DBG_IOCTL, ("motFccMCastAddrDel addr = 0x%x 0x%x
				     0x%x 0x%x 0x%x 0x%x\n"), 
				     (int) *(pAddr + 0), (int) *(pAddr + 1), 
				     (int) *(pAddr + 2), (int) *(pAddr + 3), 
				     (int) *(pAddr + 4), (int) *(pAddr + 5));

    retVal = etherMultiDel (&pDrvCtrl->endObj.multiList, pAddr);

    if (retVal == ENETRESET)
	{
	pDrvCtrl->endObj.nMulti--;

	/* stop the FCC */

	if (motFccStop (pDrvCtrl) != OK)
	    return (ERROR);

	/* populate the hash table */

	retVal = motFccHashTblPopulate (pDrvCtrl);

	/* restart the FCC */

	if (motFccStart (pDrvCtrl) != OK)
	    return (ERROR);
	}

    return ((retVal == OK) ? OK : ERROR);
    }

/*******************************************************************************
* motFccMCastAddrGet - get the current multicast address list
*
* This routine returns the current multicast address list in <pTable>
*
* SEE ALSO: motFccMCastAddrAdd() motFccMCastAddrDel()
*
* RETURNS: OK or ERROR.
*/
LOCAL STATUS motFccMCastAddrGet
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    MULTI_TABLE *pTable		/* table into which to copy addresses */
    )
    {
    MOT_FCC_LOG (MOT_FCC_DBG_IOCTL, ("motFccMCastAddrGet\n"), 
				     0, 0, 0, 0, 0, 0);

    return (etherMultiGet (&pDrvCtrl->endObj.multiList, pTable));
    }

/*******************************************************************************
* motFccHashTblAdd - add an entry to the hash table
*
* This routine adds an entry to the hash table for the address pointed to
* by <pAddr>.
*
* RETURNS: OK, or ERROR.
*/
LOCAL STATUS motFccHashTblAdd
    (
    DRV_CTRL *  pDrvCtrl,       /* pointer to DRV_CTRL structure */
    UCHAR *     pAddr		/* address to be added */
    )
    {
    UINT8	command = 0;			/* command to issue to the CP */

    MOT_FCC_LOG (MOT_FCC_DBG_IOCTL, ("motFccHashTblAdd addr = 0x%x 0x%x
				     0x%x 0x%x 0x%x 0x%x\n"), 
				     (int) *(pAddr + 0), (int) *(pAddr + 1), 
				     (int) *(pAddr + 2), (int) *(pAddr + 3), 
				     (int) *(pAddr + 4), (int) *(pAddr + 5));

    /* program the group enet address */

    if (motFccAddrSet (pDrvCtrl, (UCHAR *) pAddr, 
		       M8260_FCC_TADDR_H_OFF) != OK)
	return (ERROR);

    /* issue a set group address command to the CP */
    command = M8260_CPCR_SET_GROUP;
    if (motFccCpcrCommand (pDrvCtrl, command) == ERROR)
	return (ERROR);

    MOT_FCC_LOG (MOT_FCC_DBG_IOCTL, ("motFccHashTblAdd end\n"), 
				     0, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
* motFccHashTblPopulate - populate the hash table
*
* This routine populates the hash table with the entries found in the
* multicast table. We have to reset the hash registers first, and 
* populate them again, as more than one address may be mapped to
* the same bit.
*
* RETURNS: OK, or ERROR.
*/
LOCAL STATUS motFccHashTblPopulate
    (
    DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    ETHER_MULTI	*	mCastNode = NULL;
    UINT32		fccPramAddr = 0;		/* a convenience */

    /* get to the beginning of the parameter area */

    fccPramAddr = pDrvCtrl->fccPramAddr;

    /* reset the hash table registers first */

    MOT_FCC_REG_LONG_WR (fccPramAddr + M8260_FCC_GADDR_H_OFF, 
			 MOT_FCC_CLEAR_VAL);

    MOT_FCC_REG_LONG_WR (fccPramAddr + M8260_FCC_GADDR_L_OFF, 
			 MOT_FCC_CLEAR_VAL);

    /* scan the multicast address list */

    for (mCastNode = (ETHER_MULTI *) lstFirst (&pDrvCtrl->endObj.multiList);
	 mCastNode != NULL; 
	 mCastNode = (ETHER_MULTI *) lstNext (&mCastNode->node))
	{
	/* add this entry */

	if (motFccHashTblAdd (pDrvCtrl, mCastNode->addr) == ERROR)
	    return (ERROR);
	}

    return (OK);
    }

/***************************************************************************** 
* 
* motFccPollSend - transmit a packet in polled mode
*
* This routine is called by a user to try and send a packet on the
* device. It sends a packet directly on the network, without having to
* go through the normal process of queuing a packet on an output queue
* and then waiting for the device to decide to transmit it.
*
* These routine should not call any kernel functions.
*
* SEE ALSO: motFccPollReceive()
*
* RETURNS: OK or EAGAIN.
*/

LOCAL STATUS motFccPollSend
    (
    DRV_CTRL *pDrvCtrl, M_BLK_ID pMblk
    )
    {
    UINT16	pktType = 0;	/* packet type (unicast or multicast) */
    int	        retVal = OK;	/* holder for return value */
    char	*pBuf = NULL;	/* pointer to data to be sent */
    FCC_BD	*pTbd = NULL;	/* pointer to the current ready TBD */
    int 	ix = 0;		/* a counter */
    int	        len = 0;	/* length of data to be sent */
    UINT16	tbdStatus;	/* holder for the TBD status */

    MOT_FCC_LOG (MOT_FCC_DBG_POLL, ("motFccPollSend\n"), 0, 0, 0, 0, 0, 0);

    if (!pMblk)
        {
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
	return EAGAIN;
        }

    if (pMblk->mBlkHdr.mData[0] & (UINT8)0x01)
        {
	pktType = PKT_TYPE_MULTI;		/* packet type is multicast */
        }
    else
        {
	pktType = PKT_TYPE_UNI;			/* packet type is unicast */
        }

    pTbd = &pDrvCtrl->tbdBase[pDrvCtrl->tbdIndex];
    pBuf = pDrvCtrl->pTxPollBuf;

    /* copy data but do not free the Mblk */

    len = netMblkToBufCopy (pMblk, (char *) pBuf, NULL);
    len = max(MOT_FCC_MIN_TX_PKT_SZ, len); 

    MOT_FCC_CACHE_FLUSH (pBuf, len);	/* flush the cache, if necessary */

    pTbd->bdAddr = (unsigned long)pBuf;
    pTbd->bdLen = len;

    if (pDrvCtrl->tbdIndex == (pDrvCtrl->tbdNum - 1))
        {
	pTbd->word1 = (M8260_FETH_TBD_R | M8260_FETH_TBD_L | M8260_FETH_TBD_TC |                       M8260_FETH_TBD_PAD | M8260_FETH_TBD_W);
        }
    else
        {
	pTbd->word1 = (M8260_FETH_TBD_R | M8260_FETH_TBD_L | M8260_FETH_TBD_TC |
                       M8260_FETH_TBD_PAD);
        }

    CACHE_PIPE_FLUSH ();		/* Flush the write pipe */

    ix = 0;

    while ((tbdStatus = pTbd->word1) & M8260_FETH_TBD_R)
        {
	CACHE_PIPE_FLUSH ();		/* Flush the write pipe */

	if(++ix == MOT_FCC_WAIT_MAX)
            {
	    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
	    return EAGAIN;	/* going to need to do way more than this */
	    }
        }

    if (tbdStatus & (M8260_FETH_TBD_LC | M8260_FETH_TBD_RL |
	            M8260_FETH_TBD_UN | M8260_FETH_TBD_HB | M8260_FETH_TBD_CSL))
        {
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);

#ifdef MOT_FCC_DBG

	if ((tbdStatus & M8260_FETH_TBD_LC))
            {
	    MOT_FCC_LOG (MOT_FCC_DBG_TX, 
                         ("motFccTbdCheck: late collision\n"), 
                         1, 2, 3, 4, 5, 6);
	    ++pDrvCtrl->Stats->motFccTxLcErr;
	    }
	if ((tbdStatus & M8260_FETH_TBD_RL))
            {
	    MOT_FCC_LOG (MOT_FCC_DBG_TX, 
                         ("motFccTbdCheck: retransmit limit\n"), 
                         1, 2, 3, 4, 5, 6);
	    ++pDrvCtrl->Stats->motFccTxRlErr;
	    }

	if ((tbdStatus & M8260_FETH_TBD_UN))
            {
	    MOT_FCC_LOG (MOT_FCC_DBG_TX, 
                         ("motFccTbdCheck: tx underrun\n"), 1, 2, 3, 4, 5, 6);
	    ++pDrvCtrl->Stats->motFccTxUrErr;
	    }

	if ((tbdStatus & M8260_FETH_TBD_CSL))
            {
	    MOT_FCC_LOG (MOT_FCC_DBG_TX, 
                         ("motFccTbdCheck: carrier sense\n"), 1, 2, 3, 4, 5, 6);
	    ++pDrvCtrl->Stats->motFccTxCslErr;
	    }
#endif /* MOT_FCC_DBG */

	retVal = EAGAIN;
        }
    else
        {
	/* up-date statistics */

	if(pktType == PKT_TYPE_MULTI)
            {
            pDrvCtrl->endObj.mib2Tbl.ifOutNUcastPkts += 1;
	    }
        else
            {
            END_ERR_ADD(&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);
	    }
	retVal = OK;
        }

    /* clean this buffer descriptor, mirror motFccTbdInit() */

    if (pDrvCtrl->tbdIndex == (pDrvCtrl->tbdNum - 1))
        {
	pTbd->word1 = M8260_FETH_TBD_W;
        }
    else
        {
	pTbd->word1 = 0;
        }

    CACHE_PIPE_FLUSH ();	/* Flush the write pipe */

    /* update some indices for correct handling of the TBD ring */

    pDrvCtrl->tbdIndex = (pDrvCtrl->tbdIndex + 1) % (pDrvCtrl->tbdNum);
    pDrvCtrl->usedTbdIndex = (pDrvCtrl->usedTbdIndex + 1) % (pDrvCtrl->tbdNum);

    return retVal;
    }

/*******************************************************************************
* motFccPollReceive - receive a packet in polled mode
*
* This routine is called by a user to try and get a packet from the
* device. It returns EAGAIN if no packet is available. The caller must
* supply a M_BLK_ID with enough space to contain the received packet. If
* enough buffer is not available then EAGAIN is returned.
*
* These routine should not call any kernel functions.
*
* SEE ALSO: motFccPollSend()
*
* RETURNS: OK or EAGAIN.
*/

LOCAL STATUS motFccPollReceive
    (
    DRV_CTRL *pDrvCtrl, M_BLK *pMblk
    )
    {
    int		retVal = OK;	/* holder for return value */
    FCC_BD	*pRbd = NULL;	/* generic rbd pointer */
    UINT16	rbdStatus = 0;	/* holder for the status of this RBD */
    UINT16	rbdLen = 0;	/* number of bytes received */
    char	*pData = NULL;	/* a rx data pointer */

    MOT_FCC_LOG (MOT_FCC_DBG_POLL, ("motFccPollReceive\n"), 0, 0, 0, 0, 0, 0);

    if ((pMblk->mBlkHdr.mFlags & M_EXT) != M_EXT)
        return EAGAIN;

    /* get the first available RBD */

    pRbd = &pDrvCtrl->rbdBase[pDrvCtrl->rbdIndex];

    rbdStatus = pRbd->word1;	/* read the RBD status word */		
    rbdLen = pRbd->bdLen;	/* get the actual amount of received data */

    /* if it's not ready, don't touch it! */

    if ((rbdStatus & M8260_FETH_RBD_E))
	return EAGAIN;

    /* up-date statistics */

    if (rbdStatus & (M8260_FETH_RBD_MC|M8260_FETH_RBD_BC) )
        {
	pDrvCtrl->endObj.mib2Tbl.ifInNUcastPkts += 1;
        }
    else
        {
	END_ERR_ADD(&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);
        }

    /* 
     * Handle errata CPM-21... this code should NOT break the driver
     * when CPM-21 is ultimately fixed.
     */

    if ((rbdStatus & (M8260_FETH_RBD_CL | M8260_FETH_RBD_CR)) == 
                     M8260_FETH_RBD_CL)
        {
	rbdStatus &= (UINT16)(~M8260_FETH_RBD_CL);
        }

    /* 
     * pass the packet up only if reception was Ok AND buffer is large enough,       */

    if ((rbdStatus & MOT_FCC_RBD_ERR) || pMblk->mBlkHdr.mLen < rbdLen)
        {
	MOT_FCC_LOG (MOT_FCC_DBG_POLL, 
                     ("motFccReceive: bad rbd\n"), 1, 2, 3, 4, 5, 6);

	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
	retVal = EAGAIN;
        }
    else
        {
	pData = (char*)pRbd->bdAddr;

	/* set up the mBlk properly */

	pMblk->mBlkHdr.mFlags |= M_PKTHDR;
	pMblk->mBlkHdr.mLen    = (rbdLen - MII_CRC_LEN) & ~0xc000;    
	pMblk->mBlkPktHdr.len  = pMblk->mBlkHdr.mLen;

	/* Make cache consistent with memory */

	MOT_FCC_CACHE_INVAL ((char *) pData, pMblk->mBlkHdr.mLen);
	bcopy ((char *) pData, (char *) pMblk->mBlkHdr.mData, 
               ((rbdLen - MII_CRC_LEN) & ~0xc000));

	retVal = OK;
    }
    pRbd->bdLen = 0;

    /* up-date the status word: treat the last RBD in the ring differently */

    if ((pDrvCtrl->rbdIndex) == (pDrvCtrl->rbdNum - 1)) 
        {
	pRbd->word1 = M8260_FETH_RBD_W | M8260_FETH_RBD_E | M8260_FETH_RBD_I;
        }
    else
        {
	pRbd->word1 = M8260_FETH_RBD_E | M8260_FETH_RBD_I;
        }

    /* move on to the next used RBD */

    pDrvCtrl->rbdIndex = (pDrvCtrl->rbdIndex + 1) % (pDrvCtrl->rbdNum);

    return retVal;
    }

/*******************************************************************************
* motFccPollStart - start polling mode
*
* This routine starts polling mode by disabling ethernet interrupts and
* setting the polling flag in the END_CTRL stucture.
*
* SEE ALSO: motFccPollStop()
*
* RETURNS: OK, or ERROR if polling mode could not be started.
*/
LOCAL STATUS motFccPollStart
    (
    DRV_CTRL    * pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    int         intLevel;	/* current intr level */
    int		retVal;		/* convenient holder for return value */

    MOT_FCC_LOG (MOT_FCC_DBG_POLL, ("motFccPollStart\n"), 0, 0, 0, 0, 0, 0);

    intLevel = intLock();

    /* disable system interrupt: reset relevant bit in SIMNR_L */

    SYS_FCC_INT_DISABLE (pDrvCtrl, retVal);
    if (retVal == ERROR)
	return (ERROR);

    /* mask chip interrupts   */

    MOT_FCC_INT_DISABLE;	  

    MOT_FCC_FLAG_SET (MOT_FCC_POLLING);

    intUnlock (intLevel);

    return (OK);
    }

/*******************************************************************************
* motFccPollStop - stop polling mode
*
* This routine stops polling mode by enabling ethernet interrupts and
* resetting the polling flag in the END_CTRL structure.
*
* SEE ALSO: motFccPollStart()
*
* RETURNS: OK, or ERROR if polling mode could not be stopped.
*/

LOCAL STATUS motFccPollStop
    (
    DRV_CTRL    * pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    int         intLevel;
    int		retVal;		/* convenient holder for return value */

    intLevel = intLock();

    /* enable system interrupt: set relevant bit in SIMNR_L */

    SYS_FCC_INT_ENABLE (pDrvCtrl, retVal);
    if (retVal == ERROR)
	return (ERROR);

    /* enable chip interrupts */

    MOT_FCC_INT_ENABLE;	  

    /* set flags */

    MOT_FCC_FLAG_CLEAR (MOT_FCC_POLLING);

    intUnlock (intLevel);

    MOT_FCC_LOG (MOT_FCC_DBG_POLL, ("motFccPollStop... end\n"), 
				    0, 0, 0, 0, 0, 0);

    return (OK);
    }

#ifdef MOT_FCC_DBG 
void motFccIramShow
    (
    )
    {
    UINT32	temp32;				/* a convenience */
    volatile UINT16	temp16;				/* a convenience */
    DRV_CTRL *	pDrvCtrl = pDrvCtrlDbg;
    UINT32	fccIramAddr = 0;		/* a convenience */

    fccIramAddr = pDrvCtrl->fccIramAddr;

    MOT_FCC_REG_LONG_RD (fccIramAddr + M8260_FCC_GFMR_OFF,
			 temp32);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("gfmr=0x%x\n"),
				    temp32, 0, 0, 0, 0, 0);

    MOT_FCC_REG_LONG_RD (fccIramAddr + M8260_FCC_FPSMR_OFF,
			 temp32);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("fpsmr=0x%x\n"),
				    temp32, 0, 0, 0, 0, 0);

    MOT_FCC_REG_WORD_RD (fccIramAddr + M8260_FCC_FDSR_OFF, 
			 temp16);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("fdsr=0x%x\n"),
				    temp16, 0, 0, 0, 0, 0);

    MOT_FCC_REG_WORD_RD (fccIramAddr + M8260_FCC_FTODR_OFF, 
			 temp16);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("ftodr=0x%x\n"),
				    temp16, 0, 0, 0, 0, 0);

    MOT_FCC_REG_WORD_RD (fccIramAddr + M8260_FCC_FCCMR_OFF,
			 temp16);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("fccmr=0x%x\n"),
				    temp16, 0, 0, 0, 0, 0);

    MOT_FCC_REG_WORD_RD (fccIramAddr + M8260_FCC_FCCER_OFF,
			 temp16);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("fccer=0x%x\n"),
				    temp16, 0, 0, 0, 0, 0);

    }

void motFccPramShow
    (
    )
    {
    UINT32	tempVal [6];			/* a convenience */
    DRV_CTRL *	pDrvCtrl = pDrvCtrlDbg;		/* the driver control struct */
    UINT32	fccPramAddr = 0;		/* a convenience */

    fccPramAddr = pDrvCtrl->fccPramAddr;

    MOT_FCC_REG_WORD_RD (fccPramAddr + M8260_FCC_RIPTR_OFF, 
			 tempVal [0]);

    MOT_FCC_REG_WORD_RD (fccPramAddr + M8260_FCC_TIPTR_OFF, 
			 tempVal [1]);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_RSTATE_OFF,
			 tempVal[2]);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_RBASE_OFF,
			 tempVal[3]);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_TSTATE_OFF,
			 tempVal[4]);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_TBASE_OFF,
			 tempVal[5]);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("\nriptr=0x%x 
				    tiptr=0x%x
				    rstate=0x%x 
				    rbase=0x%x
				    tstate=0x%x
				    tbase=0x%x\n"),
				    tempVal [0], tempVal [1], tempVal [2], 
				    tempVal [3], tempVal [4], tempVal [5]);

    MOT_FCC_REG_WORD_RD (fccPramAddr + M8260_FCC_RET_LIM_OFF, 
			 tempVal [0]);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_C_MASK_OFF, 
			 tempVal [1]);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_C_PRES_OFF,
			 tempVal[2]);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_CRCEC_OFF,
			 tempVal[3]);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_ALEC_OFF,
			 tempVal[4]);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_DISFC_OFF,
			 tempVal[5]);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("\nretry limit=%d
				    crc mask=0x%x
				    preset crc=0x%x 
				    crc errored frames=0x%x
				    misaligned frames=0x%x
				    discarded frames=0x%x\n"),
				    tempVal [0], tempVal [1], tempVal [2], 
				    tempVal [3], tempVal [4], tempVal [5]);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_GADDR_H_OFF,
			 tempVal[0]);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_GADDR_L_OFF,
			 tempVal[1]);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_IADDR_H_OFF,
			 tempVal[2]);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_IADDR_L_OFF,
			 tempVal[3]);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("\ngroup addr filter high=0x%x
				    group addr filter low=0x%x
				    individual addr filter high=0x%x 
				    individual addr filter low=0x%x \n"),
				    tempVal [0], tempVal [1], tempVal [2], 
				    tempVal [3], 0, 0);

    MOT_FCC_REG_WORD_RD (fccPramAddr + M8260_FCC_PADDR_H_OFF,
			 tempVal[0]);

    MOT_FCC_REG_WORD_RD (fccPramAddr + M8260_FCC_PADDR_M_OFF,
			 tempVal[1]);

    MOT_FCC_REG_WORD_RD (fccPramAddr + M8260_FCC_PADDR_L_OFF,
			 tempVal[2]);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("\nindividual addr high=0x%x 
				    individual addr medium=0x%x 
				    individual addr low=0x%x \n"),
				    tempVal [0], tempVal [1], tempVal [2], 
				    0, 0, 0);

    MOT_FCC_REG_WORD_RD (fccPramAddr + M8260_FCC_MRBLR_OFF,
			 tempVal[0]);

    MOT_FCC_REG_WORD_RD (fccPramAddr + M8260_FCC_MFLR_OFF,
			 tempVal[1]);

    MOT_FCC_REG_WORD_RD (fccPramAddr + M8260_FCC_MINFLR_OFF,
			 tempVal[2]);

    MOT_FCC_REG_WORD_RD (fccPramAddr + M8260_FCC_MAXD1_OFF,
			 tempVal[3]);

    MOT_FCC_REG_WORD_RD (fccPramAddr + M8260_FCC_MAXD2_OFF,
			 tempVal[4]);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("\nmax rx buf length=0x%x 
				    max frame length=0x%x 
				    min frame length=0x%x
				    max DMA1 length=0x%x
				    max DMA2 length=0x%x \n"),
				    tempVal [0], tempVal [1], tempVal [2], 
				    tempVal [3], tempVal [4], 0);

    MOT_FCC_REG_LONG_RD (fccPramAddr + M8260_FCC_CAM_PTR_OFF,
			 tempVal[0]);

    MOT_FCC_REG_WORD_RD (fccPramAddr + M8260_FCC_P_PER_OFF,
			 tempVal[1]);

    MOT_FCC_REG_WORD_RD (fccPramAddr + M8260_FCC_PAD_PTR_OFF,
			 tempVal[2]);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("\ncam address pointer=0x%x
				    persistence=0x%x 
				    internal pad pointer=0x%x \n"),
				    tempVal [0], tempVal [1], tempVal [2], 
				    0, 0, 0);
    }

void motFccRbdShow(int rbdNum)
{
FCC_BD	*pBd;

    pBd = &global_pDrvCtrl->rbdBase[rbdNum];
    MOT_FCC_LOG (MOT_FCC_DBG_ANY,
		("rbdStatus=0x%x rbdLen=0x%x rbdBuf=0x%x\n"),
		pBd->word1, pBd->bdLen, pBd->bdAddr, 0, 0, 0);
}

void motFccTbdShow(int tbdNum)
{
FCC_BD	*pBd;
 
    pBd = &global_pDrvCtrl->tbdBase[tbdNum];
    MOT_FCC_LOG (MOT_FCC_DBG_ANY,
		("tbdStatus=0x%x tbdLen=0x%x tbdBuf=0x%x\n"),
		pBd->word1, pBd->bdLen, pBd->bdAddr, 0, 0, 0);
    }
 
void motFccErrorShow (void)
{
    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("\nTxErr=0x%x 
				   RxBsy=0x%x\n"),
				   motFccTxErr, 
				   motFccRxBsyErr, 
				   0, 0, 0, 0);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("\nTxLc=0x%x 
   				   TxUr=0x%x 
				   TxCsl=0x%x 
				   TxRl=0x%x\n"),
				   motFccTxLcErr, 
				   motFccTxUrErr, 
				   motFccTxCslErr, 
				   motFccTxRlErr, 
				   0, 0);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("\nRxLg=0x%x 
   				   RxNo=0x%x 
				   RxCrc=0x%x 
				   RxOv=0x%x 
				   RxLc=0x%x 
				   RxSh=0x%x \n"),
				   motFccRxLgErr, 
				   motFccRxNoErr, 
				   motFccRxCrcErr, 
				   motFccRxOvErr, 
				   motFccRxLcErr, 
				   motFccRxShErr);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("\nRxMem=0x%x\n"),
				   motFccRxMemErr, 
				   0, 0, 0, 0, 0);
}

void motFccDrvShow
    (
    )
    {
    DRV_CTRL *	pDrvCtrl = pDrvCtrlDbg;

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, "pDrvCtrl=0x%x pNetPool=0x%x pClPool=0x%x\n",
				   (int) pDrvCtrl, 
				   (int) pDrvCtrl->endObj.pNetPool, 
				   (int) pDrvCtrl->pClPoolId, 0, 0, 0);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, "bdBase=0x%x bdSize=0x%x pClBlkArea=0x%x "
                                  "clBlkSize=0x%x pMBlkArea=0x%x mBlkSize=0x%x\n",
				   (int) pDrvCtrl->pBdBase,
				   pDrvCtrl->bdSize,
				   (int) pDrvCtrl->pBufBase,
				   pDrvCtrl->bufSize,
				   (int) pDrvCtrl->pMBlkArea, 
				   pDrvCtrl->mBlkSize);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("tbdNum=%d 
				   tbdBase=0x%x
				   tbdIndex=%d 
				   usedTbdIndex=%d 
				   cleanTbdNum=%d 
				   txStall=%d \n"),
				   pDrvCtrl->tbdNum, 
				   (int) pDrvCtrl->tbdBase, 
				   pDrvCtrl->tbdIndex,
				   pDrvCtrl->usedTbdIndex,
				   pDrvCtrl->cleanTbdNum, 
				   pDrvCtrl->txStall);

    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("rbdNum=%d 
				   rbdBase=0x%x 
				   rbdIndex=%d
				   iramAddr=0x%x 
				   pramAddr=0x%x
				   fccNum=%d \n"),
				   pDrvCtrl->rbdNum, 
				   (int) pDrvCtrl->rbdBase, 
				   pDrvCtrl->rbdIndex,
				   pDrvCtrl->fccIramAddr,
				   pDrvCtrl->fccPramAddr,
				   pDrvCtrl->fccNum);
    }

void motFccMiiShow
    (
    )
    {
    DRV_CTRL *  pDrvCtrl = pDrvCtrlDbg;
    UCHAR       speed [20];
    UCHAR       mode [20];
 
    strcpy ((char *) speed, (pDrvCtrl->phyInfo->phySpeed == MOT_FCC_100MBS) ?
                    "100Mbit/s" : "10Mbit/s");
 
    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("\nphySpeed=%s 
				   phyMode=%s 
				   phyAddr=0x%x
                                   phyFlags=0x%x
                                   phyDefMode=0x%x\n"),
                                   (int) &speed[0],
                                   (int) &mode[0],
                                   pDrvCtrl->phyInfo->phyAddr,
                                   pDrvCtrl->phyInfo->phyFlags,
                                   pDrvCtrl->phyInfo->phyDefMode,
				   0);
 
    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("\nphyStatus=0x%x 
				   phyCtrl=0x%x 
				   phyAds=0x%x
                                   phyPrtn=0x%x 
				   phyExp=0x%x 
				   phyNext=0x%x\n"),
                                   pDrvCtrl->phyInfo->miiRegs.phyStatus,
                                   pDrvCtrl->phyInfo->miiRegs.phyCtrl,
                                   pDrvCtrl->phyInfo->miiRegs.phyAds,
                                   pDrvCtrl->phyInfo->miiRegs.phyPrtn,
                                   pDrvCtrl->phyInfo->miiRegs.phyExp,
                                   pDrvCtrl->phyInfo->miiRegs.phyNext);
    }

void motFccMibShow
    (
    )
    {
    DRV_CTRL *  pDrvCtrl = pDrvCtrlDbg;
 
    MOT_FCC_LOG (MOT_FCC_DBG_ANY, ("\nifOutNUcastPkts=%d 
				   ifOutUcastPkts=%d
                                   ifInNUcastPkts=%d
                                   ifInUcastPkts=%d 
				   ifOutErrors=%d\n"),
                                   pDrvCtrl->endObj.mib2Tbl.ifOutNUcastPkts,
                                   pDrvCtrl->endObj.mib2Tbl.ifOutUcastPkts,
                                   pDrvCtrl->endObj.mib2Tbl.ifInNUcastPkts,
                                   pDrvCtrl->endObj.mib2Tbl.ifInUcastPkts,
                                   pDrvCtrl->endObj.mib2Tbl.ifOutErrors,
                                   0);
 
    }

UINT16 motFccMiiRegRead
    (
    UINT8 regAddr
    )
    {
    int status;
    UINT16 value;

    status = motFccMiiRead(pDrvCtrlDbg, 
			   pDrvCtrlDbg->phyInfo->phyAddr, 
			   regAddr,
			   &value);

    if (status != OK)
	MOT_FCC_LOG (MOT_FCC_DBG_MII, ("motFccMiiRead failed, retval %d\n"),
				       status, 2, 3, 4, 5, 6);
    
    return(value);
    }


					
#endif /* MOT_FCC_DBG */
