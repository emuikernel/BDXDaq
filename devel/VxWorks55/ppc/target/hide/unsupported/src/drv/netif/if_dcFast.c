 /* if_dcFast.c - DEC 21040 PCI Ethernet LAN network interface driver */

/* Copyright 1984-1995 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,06mar96,rcp  added support for the DEC21140 100Mbit Controller
01d,05sep95,vin  added filter Frame setup, added PROMISCOUS MODE to ioctl,
		 changed all ln references to dc, added flag DC_MULTICAST_FLAG.
01c,05apr95,vin  added AUI/10BASE-T dynamic configuration support. 
01b,31mar95,caf  changed name from "dcPci" to "dc".
01a,03mar95,vin	 written.
*/

/* FROM ROB PERSONS - Motorola Computer Group (March 06, 1996)
The comments below describe the primary operation and configuration of
the DEC 21X40 driver.  Differences between the two DEC parts will be 
outlined in this section and how the original driver was modified to
accomodate the differences.

The DEC21140 is very similar to the original DEC21040 but it also has the
ability to operate on networks at 100-Mb/s along with 10-Mb/s.  The
physical interface is no longer part of the chip so configuration of
the CSR registers which control the AUI and TP interfaces is no longer
applicable.  This part now provides 100-Mb/s MII/SYM 10-Mb/s Serial
intefaces which are then converted to the appropriate external interface
with some additional hardware.  Most of the driver will be uneffected
by the new part.  

In the early stages of the driver initialization, the driver will
use the Configuration ID Register (CFID) to determine which of the
two parts is to be used.  A global flag will indicate which device
is in use and its state will be used along with the driver modifications
to accomodate the DEC21140. 

The Status Register (CSR5) has three bits which have different behavior.
Bit 12 (Link Fail Status) and Bit 10 (AUI/TP Status) are no longer used
by the DEC21140.  Bit 11 in the DEC21040 indicates a Full-Duplex Short
Frame was Received which in the DEC21140 it indicates the General Purpose
Timer(GPT)  has expired.  The GPT is a new feature with the DEC21140.  Bit 
11 is currently not used in the drive and there are no plans on adding support 
for the GPT so at this time it will be ignored.

The Operating Mode Register (CSR6) is significantly different.  Many of the
bits reflect the difference in the MII/SYM interface for the 100-Mb/s 
interface.  To make the driver as flexible as possible, logic will be added 
to the dcattach function to allow for the configuration of all these 
control bits at driver configuration.  The following modes will be added:

DEC21140 Flag	- This flag indicates the driver will be operating with
		  a DEC21140 chip and not the DEC21040.
100Mb/s Mode	- This flag will be used to set a bits in the CSR6 for the
		  DEC21140.  The Transmit Threshold Mode and Heartbeat Disable
		  bits will be effect.
Scramble Mode 	- The scrambler function is active and the MII/SYM port 
	          transmits and received scrambled symbols.
PCS Function	- The PCS functions are active and MII/SYM port operates
		  in symbol mode.  All MII/SYM port cont andol signals are 
		  generated internally bits will be effect.
Port Select	- When selected the MII/SYM mode is active.  All 100-Mb/s
		  modes require this.
Full Duplex Mode- When selected the Lance Chip Operates in Full Duplex Mode.
		  It is important that the can operate in Full Duplex mode
		  as well. 
Int Loopback    - An additional mode was added for doing internal loopback.
Ext Loopback    - An additional mode was added for doing external loopback.
HB Enable	- An additional mode was added for testing, enableing heartb.

If none of these mode bits are selected, the DEC21140 will default to the
MII interface with transmit FIFO thresholds appropriate for 10-Mb/s.

Additional code will be added to deal with the different Serial ROM interface.
The DEC21140 now has a new interface to access a R/W ROM part to store ethernet
addresses.  The dcEnetAddrGet will be modified to add the proper logic to
read the DEC21140 Serial ROM interface.  No logic will be added to reprogram
the value.  It turns out the DEC PMC 520-AA only stores the last 2 bytes of the
ethernet address in the ROM.  The driver will make the assumption that the
VENDOR ID portion extends to from 0-3 where bytes 0-2 are the manufacturers
assigned values and byte 3 is assigned by DEC for the class of products.

The remaining Host to Controller interface is identical and should not be 
effected by the new part.

*/



/*
This module implements the DEC 21040-PCI Ethernet 32 bit network interface
driver.

The DEC 21040-PCI ethernet controller is inherently little endian because
the chip is designed to operate on a PCI bus which is a little endian
bus. The software interface to the driver is divided into three parts.
The first part is the PCI configuration registers and their set up. 
This part is done at the BSP level in the various BSPs which use this
driver. The second and third part are dealt in the driver. The second
part of the interface comprises of the I/O control registers and their
programming. The third part of the interface comprises of the descriptors
and the buffers. 

This driver is designed to be moderately generic, operating unmodified
across the range of architectures and targets supported by VxWorks.  To
achieve this, the driver must be given several target-specific parameters,
and some external support routines must be provided.  These parameters,
and the mechanisms used to communicate them to the driver, are detailed
below.  If any of the assumptions stated below are not true for your
particular hardware, this driver will probably not function correctly with
it.

This driver supports upto 4 lance units per CPU.  The driver can be
configured to support big-endian or little-endian architectures.  It
contains error recovery code to handle known device errata related to DMA
activity.

This driver configures the 10BASE-T interface by default and waits for
two seconds to check the status of the link. If the link status is fail
then it configures the AUI interface. 

Big endian processors can be connected to the PCI bus through some controllers
which take care of hardware byte swapping. In such cases all the registers 
which the chip DMA s to have to be swapped and written to, so that when the
hardware swaps the accesses, the chip would see them correctly. The chip still
has to be programmed to operated in little endian mode as it is on the PCI bus.
If the cpu board hardware automatically swaps all the accesses to and from the
PCI bus, then input and output byte stream need not be swapped. 

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine;
there is no separate initialization routine.  Therefore, in the global interface
structure, the function pointer to the initialization routine is NULL.

The only user-callable routine is dcattach(), which publishes the `dc'
interface and initializes the driver and device.

TARGET-SPECIFIC PARAMETERS
.iP "bus mode"
This parameter is a global variable that can be modified at run-time.

The LAN control register #0 determines the bus mode of the device,
allowing the support of big-endian and little-endian architectures.
This parameter, defined as "ULONG dcCSR0Bmr", is the value that will
be placed into LANCE control register #0. The default is mode is little
Endian.
For information about changing this parameter, see the manual
.I "DEC  Local Area Network Controller DEC21040 or DEC21140 for PCI."

.iP "base address of device registers"
This parameter is passed to the driver by dcattach().

.iP "interrupt vector"
This parameter is passed to the driver by dcattach().

This driver configures the LANCE device to generate hardware interrupts
for various events within the device; thus it contains
an interrupt handler routine.  The driver calls intConnect() to connect 
its interrupt handler to the interrupt vector generated as a result of 
the LANCE interrupt.

.iP "interrupt level"
This parameter is passed to the driver by dcattach().

Some targets use additional interrupt controller devices to help organize
and service the various interrupt sources.  This driver avoids all
board-specific knowledge of such devices.  During the driver's
initialization, the external routine sysLanIntEnable() is called to
perform any board-specific operations required to allow the servicing of a
LANCE interrupt.  For a description of sysLanIntEnable(), see "External
Support Requirements" below.

This parameter is passed to the external routine.

.iP "shared memory address"
This parameter is passed to the driver by dcattach().

The LANCE device is a DMA type of device and typically shares access to
some region of memory with the CPU.  This driver is designed for systems
that directly share memory between the CPU and the LANCE.  It
assumes that this shared memory is directly available to it
without any arbitration or timing concerns.

This parameter can be used to specify an explicit memory region for use
by the LANCE.  This should be done on hardware that restricts the LANCE
to a particular memory region.  The constant NONE can be used to indicate
that there are no memory limitations, in which case, the driver 
attempts to allocate the shared memory from the system space.

.iP "shared memory size"
This parameter is passed to the driver by dcattach().

This parameter can be used to explicitly limit the amount of shared
memory (bytes) this driver will use.  The constant NONE can be used to
indicate no specific size limitation.  This parameter is used only if
a specific memory region is provided to the driver.

.iP "shared memory width"
This parameter is passed to the driver by dcattach().

Some target hardware that restricts the shared memory region to a
specific location also restricts the access width to this region by
the CPU.  On these targets, performing an access of an invalid width
will cause a bus error.

This parameter can be used to specify the number of bytes of access
width to be used by the driver during access to the shared memory.
The constant NONE can be used to indicate no restrictions.

Current internal support for this mechanism is not robust; implementation 
may not work on all targets requiring these restrictions.

.iP "shared memory buffer size"
This parameter is passed to the driver by dcattach().

The driver and LANCE device exchange network data in buffers.  This
parameter permits the size of these individual buffers to be limited.
A value of zero indicates that the default buffer size should be used.
The default buffer size is large enough to hold a maximum-size Ethernet
packet.

.iP "pci Memory base"
This parameter is passed to the driver by dcattach(). This parameter
gives the base address of the main memory on the PCI bus. 

.iP "dcOpMode"
This parameter is passed to the driver by dcattach(). This parameter
gives the mode of initialization of the device. The mode flags for both
the DEC21040 and DEC21140 interfaces are listed below.  

DC_PROMISCUOUS_FLAG     0x01 
DC_MULTICAST_FLAG	0x02

The mode flags specific to the DEC21140 interface are listed below.

DC_100_MB_FLAG          0X04
DC_21140_FLAG           0x08
DC_SCRAMBLER_FLAG       0X10
DC_PCS_FLAG             0x20
DC_PS_FLAG              0x40

The Full Duplex Mode was added to the driver.
DC_FULLDUPLEX_FLAG      0x80

Loopback mode flags
DC_ILOOPB_FLAG          0x100 
DC_ELOOPB_FLAG          0x200
DC_HBE_FLAG	        0x400


.iP "Ethernet address"
This is obtained by the driver by reading an ethernet ROM register 
interfaced with the device.

.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires one external support function:
.iP "void sysLanIntEnable (int level)" "" 9 -1
This routine provides a target-specific enable of the interrupt for
the LANCE device.  Typically, this involves interrupt controller hardware,
either internal or external to the CPU.

This routine is called once, from the dcattach() routine.
.LP

SEE ALSO: ifLib, 
.I "DECchip 21040 or 21140 Ethernet LAN Controller for PCI."
*/

#include "vxWorks.h"
#include "stdlib.h"
#include "taskLib.h"
#include "logLib.h"
#include "intLib.h"
#include "netLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysLib.h"
#include "iv.h"
#include "memLib.h"
#include "cacheLib.h"
#include "sys/ioctl.h"
#include "etherLib.h"

#ifndef DOC             /* don't include when building documentation */
#include "net/mbuf.h"
#endif  /* DOC */

#include "net/protosw.h"
#include "sys/socket.h"
#include "errno.h"

#include "net/if.h"
#include "net/route.h"

#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"
#include "netinet/if_ether.h"
#include "net/if_subr.h"
#include "semLib.h"

#include "drv/netif/if_dcFast.h"	/* device description header */

/* defines */

#define DC_BUFSIZ      (ETHERMTU + SIZEOF_ETHERHEADER + 6)
#define MAX_UNITS       4	/* maximum units supported */
#define DC_L_POOL	0x10	/* number of Rx loaner buffers in pool */
#define LOOP_PER_NS 4
#define DELAY(count)	{\
			volatile int cx = 0;\
			for (cx = 0; cx < (count); cx++);\
			}
#define NSDELAY(nsec)	{\
			volatile int nx = 0;\
			volatile int loop = (int)(nsec*LOOP_PER_NS);\
			for (nx = 0; nx < loop; nx++);\
			}
/* RCP: Added DEC Part Check */
#define DEC21140(x)	 ((x & DC_21140_FLAG) && (x != NONE)) 
/*
 * If DC_KICKSTART_TX is TRUE the transmitter is kick-started to force a
 * read of the transmit descriptors, otherwise the internal polling (1.6msec)
 * will initiate a read of the descriptors.  This should be FALSE is there
 * is any chance of memory latency or chip accesses detaining the LANCE DMA,
 * which results in a transmitter UFLO error.  This can be changed with the
 * global dcKickStartTx below.
 */

#define DC_KICKSTART_TX TRUE

/* Cache macros */

#define DC_CACHE_INVALIDATE(address, len) \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->cacheFuncs, (address), (len))

#define DC_CACHE_VIRT_TO_PHYS(address) \
        CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, (address))

#define DC_CACHE_PHYS_TO_VIRT(address) \
        CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->cacheFuncs, (address))

/* memory to PCI address translation macros */

#define PCI_TO_MEM_PHYS(pciAdrs) \
	((pciAdrs) - (pDrvCtrl->pciMemBase))
	
#define MEM_TO_PCI_PHYS(memAdrs) \
	((memAdrs) + (pDrvCtrl->pciMemBase))

/* Typedefs for external structures that are not typedef'd in their .h files */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;                  /* Interface Data Record wrapper */
typedef struct ifnet IFNET;                 /* real Interface Data Record */
typedef struct sockaddr SOCK;

/* The definition of the driver control structure */

typedef struct drv_ctrl
    {
    IDR		idr;			/* Interface Data Record */
    int		dcNumRds;		/* RMD ring size */
    int		rxIndex;                /* index into RMD ring */
    DC_RDE *	rxRing;			/* RMD ring */
    int		dcNumTds;		/* TMD ring size */
    int		txIndex;		/* index into TMD ring */
    int		txDiIndex;		/* disposal index into TMD ring */
    DC_TDE *	txRing;			/* TMD ring */
    BOOL        attached;               /* indicates unit is attached */
    SEM_ID      TxSem;                  /* transmitter semaphore */
    ULONG	dcOpMode;		/* mode of operation */
    int         ivec;                   /* interrupt vector */
    int         ilevel;                 /* interrupt level */
    ULONG	devAdrs;                /* device structure address */
    ULONG	pciMemBase;		/* memory base as seen from PCI*/
    int         memWidth;               /* width of data port */
    CACHE_FUNCS cacheFuncs;             /* cache function pointers */
    int		nLoanRx;		/* number of Rx buffers left to loan */
    char	*lPool[DC_L_POOL];	/* receive loaner pool ptrs */
    UINT8	*pRefCnt[DC_L_POOL];	/* stack of reference count pointers */
    UINT8	refCnt[DC_L_POOL];	/* actual reference count values */
    ULONG *	pFltrFrm;		/* pointer to setup filter frame */
    } DRV_CTRL;

#define DRV_CTRL_SIZ		sizeof(DRV_CTRL)
#define RMD_SIZ			sizeof(DC_RDE)
#define TMD_SIZ			sizeof(DC_TDE)

/* globals */

/*  RCP: Reference Removed for new function
IMPORT void sysLanIntEnable ();
*/
IMPORT BOOL arpresolve ();
/*
IMPORT unsigned char dcEnetAddr []; *//* Ethernet address to load into lance */


BOOL 	dcKickStartTx 	= DC_KICKSTART_TX;
ULONG	dcCSR0Bmr	= 0;

/* locals */

LOCAL DRV_CTRL  drvCtrl [MAX_UNITS];	/* array of driver control structs */

LOCAL int	dcNumRds  = NUM_RDS;	/* number of ring descriptors */
LOCAL int	dcNumTds  = NUM_TDS;	/* number of xmit descriptors */
LOCAL int 	dcLPool = DC_L_POOL;

/* forward static functions */

LOCAL void 	dcReset (int unit);
LOCAL void 	dcInt (DRV_CTRL *pDrvCtrl);
LOCAL void 	dcHandleRecvInt (DRV_CTRL *pDrvCtrl);
LOCAL STATUS 	dcRecv (DRV_CTRL *pDrvCtrl, DC_RDE *rmd);
LOCAL int 	dcOutput (IDR *ifp, MBUF *m0, SOCK *dst);
LOCAL int 	dcIoctl (IDR *ifp, int cmd, caddr_t data);
LOCAL int 	dcChipReset (DRV_CTRL *pDrvCtrl);
LOCAL DC_RDE *	dcGetFullRMD (DRV_CTRL *pDrvCtrl);
LOCAL void	dcAuiTpInit (ULONG devAdrs);
LOCAL void 	dcCsrWrite (ULONG devAdrs, int reg, ULONG value);
LOCAL ULONG 	dcCsrRead (ULONG devAdrs, int reg);
LOCAL void 	dcRestart (int unit);
LOCAL STATUS 	dcEnetAddrGet (ULONG devAdrs, char * enetAdrs, int len);
LOCAL STATUS 	dc21140EnetAddrGet (ULONG devAdrs, char * enetAdrs, int len);
LOCAL BOOL 	convertDestAddr (IDR * pIDR, SOCK * pDestSktAddr,
				 char * pDestEnetAddr, u_short * pPacketType,
				 MBUF * pMbuf);
LOCAL void 	dcLoanFree (DRV_CTRL *pDrvCtrl, char *pRxBuf, UINT8 *pRef);
LOCAL int	dcFltrFrmSetup (DRV_CTRL * pDrvCtrl, char * pPhysAdrsTbl, 
				int tblLen);
LOCAL int	dcFltrFrmXmit (DRV_CTRL * pDrvCtrl, char * pPhysAdrsTbl,
			       int tblLen);


#ifdef DC_DEBUG
void dcCsrShow (ULONG devAdrs);
#endif /* DC_DEBUG */

/* RCP: Add utilities to read entries in Serial ROM for DEC21140 */
USHORT dcReadRom (ULONG devAdrs, UCHAR lineCnt);
int dcViewRom (ULONG devAdrs, UCHAR lineCnt, int cnt);

/* RCP: Temporary reference of new global symbol */
LOCAL STATUS sysLanIntEnable (int Level);

/*******************************************************************************
*
* dcattach - publish the `dc' network interface.
*
* This routine publishes the `dc' interface by filling in a network interface
* record and adding this record to the system list.  This routine also
* initializes the driver and the device to the operational state.
*
* The <unit> parameter is used to specify the device unit to initialize.
*
* The <devAdrs> is used to specify the I/O address base of the device.
*
* The <ivec> parameter is used to specify the interrupt vector associated
* with the device interrupt.
*
* The <ilevel> parater is used to specify the level of the interrupt which
* the device would use.
*
* The <memAdrs> parameter can be used to specify the location of the
* memory that will be shared between the driver and the device.  The value
* NONE is used to indicate that the driver should obtain the memory.
*
* The <memSize> parameter is valid only if the <memAdrs> parameter is not
* set to NONE, in which case <memSize> indicates the size of the
* provided memory region.
*
* The <memWidth> parameter sets the memory pool's data port width (in bytes);
* if it is NONE, any data width is used.
*
* The <pciMemBase> parameter defines the main memory base as seen from PCI bus.
* 
* The <dcOpMode> parameter defines the mode in which the device should be
* operational.
*
* RCP:  Added for DEC21140 Serial ROM
*
* BUGS
* To zero out LANCE data structures, this routine uses bzero(), which
* ignores the <memWidth> specification and uses any size data access to
* write to memory.
*
* RETURNS: OK or ERROR.
*/

STATUS dcattach
    (
    int		unit,		/* unit number */
    ULONG	devAdrs,	/* LANCE I/O address */
    int		ivec,		/* interrupt vector */
    int		ilevel,		/* interrupt level */
    char *	memAdrs,	/* address of memory pool (-1 = malloc it) */
    ULONG	memSize,	/* only used if memory pool is NOT malloc()'d */
    int		memWidth,	/* byte-width of data (-1 = any width) */
    ULONG	pciMemBase,	/* main memory base as seen from PCI bus */
    int		dcOpMode	/* mode of operation */
    )
    {
    DRV_CTRL *		pDrvCtrl;	/* pointer to drvctrl */
    unsigned int	sz;		/* temporary size holder */
    char *		pShMem;		/* start of the LANCE memory pool */
    char *		buf;		/* temp buffer pointer */
    void *		pTemp;		/* temp pointer */
    DC_RDE *		rmd;		/* pointer to rcv descriptor */
    DC_TDE *		tmd;		/* pointer to xmit descriptor */
    int			ix;		/* counter */
/* RCP: Added temporary value */
    ULONG		uTemp;		/* temporary storage */
    
    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    /* Ensure single invocation per system life */

    pDrvCtrl = & drvCtrl [unit];

    if (pDrvCtrl->attached)
        return (OK);

    pDrvCtrl->ivec	= ivec;			/* interrupt vector */
    pDrvCtrl->ilevel	= ilevel;		/* interrupt level */
    pDrvCtrl->devAdrs	= devAdrs;		/* LANCE I/O address */
    pDrvCtrl->pciMemBase  = pciMemBase;		/* pci memory base */
    pDrvCtrl->memWidth 	= memWidth;		/* memory width */
    pDrvCtrl->dcOpMode	= dcOpMode;		/* mode of operation */

    /* Publish the interface data record */

    ether_attach (
		  & pDrvCtrl->idr.ac_if,
		  unit,
		  "dc",
		  (FUNCPTR) NULL,
		  (FUNCPTR) dcIoctl,
		  (FUNCPTR) dcOutput,
		  (FUNCPTR) dcReset
		 );

    /* Create the transmit semaphore. */

    if ((pDrvCtrl->TxSem = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
				       SEM_INVERSION_SAFE)) == NULL)
        {
        printf ("dc: error creating transmitter semaphore\n");
        return (ERROR);
        }

    /* Establish size of shared memory region we require */
    
    if ((int) memAdrs != NONE)  /* specified memory pool */
	{
	sz  = (memSize - (RMD_SIZ + TMD_SIZ + (dcLPool * DC_BUFSIZ) + 
			  FLTR_FRM_SIZE))
	      / ((2 * DC_BUFSIZ) + RMD_SIZ + TMD_SIZ);
	dcNumRds = max (sz, MIN_RDS);
	dcNumTds = max (sz, MIN_TDS);
	}

    /* add it all up - allow for alignment adjustment */

    sz = ((dcNumRds + 1) * RMD_SIZ) + (dcNumRds * DC_BUFSIZ) +
	 ((dcNumTds + 1) * TMD_SIZ) + (dcNumTds * DC_BUFSIZ) + 
	 (DC_BUFSIZ * dcLPool) + FLTR_FRM_SIZE;

    /* Establish a region of shared memory */

    /* OK. We now know how much shared memory we need.  If the caller
     * provides a specific memory region, we check to see if the provided
     * region is large enough for our needs.  If the caller did not
     * provide a specific region, then we attempt to allocate the memory
     * from the system, using the cache aware allocation system call.
     */

    switch ( (int) memAdrs )
        {
        default :       /* caller provided memory */
            if ( memSize < sz )     /* not enough space */
                {
                printf ( "dc: not enough memory provided\n" );
                return ( ERROR );
                }
            pShMem = memAdrs;             /* set the beginning of pool */

            /* assume pool is cache coherent, copy null structure */

            pDrvCtrl->cacheFuncs = cacheNullFuncs;

            break;

        case NONE :     /* get our own memory */

            /* Because the structures that are shared between the device
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
                printf ( "dc: device requires cache coherent memory\n" );
                return (ERROR);
                }

            pShMem = (char *) cacheDmaMalloc ( sz );

            if ((int)pShMem == NULL)
                {
                printf ( "dc: system memory unavailable\n" );
                return (ERROR);
                }

            /* copy the DMA structure */

            pDrvCtrl->cacheFuncs = cacheDmaFuncs;

            break;
        }

    if (intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC(ivec),dcInt,(int)pDrvCtrl)
        == ERROR)
        return (ERROR);

    /*                        Turkey Carving
     *                        --------------
     *
     *                          LOW MEMORY
     *             |-------------------------------------|
     *             |         The Rx descriptors          |
     *             | (dcNumRds * sizeof (DC_RDE))        |
     *             |-------------------------------------|
     *             |          The receive buffers        |
     *             |       (DC_BUFSIZ * dcNumRds)        |
     *             |-------------------------------------|
     *             |         The Rx loaner pool          |
     *             |        (DC_BUFSIZ * dcLPool)        |
     *             |-------------------------------------|
     *             |         The Tx descriptors          |
     *             | (dcNumTds *  sizeof (DC_TDE))       |
     *             |-------------------------------------|
     *             |           The transmit buffers      |
     *             |       (DC_BUFSIZ * dcNumTds)        |
     *             |-------------------------------------|
     *             |           Setup Filter Frame        |
     *             |           (FLTR_FRM_SIZE)           |
     *             |-------------------------------------|
     */

    /* zero the shared memory */

    bzero ( (char *) pShMem, (int) sz );

    /* carve Rx memory structure */

    pDrvCtrl->rxRing = (DC_RDE *) (((int)pShMem + 0x03) & ~0x03); /* wd align */
    pDrvCtrl->dcNumRds = dcNumRds;		/* receive ring size */
    pDrvCtrl->rxIndex = 0;

    /* Set up the Rx descriptors */

    rmd = pDrvCtrl->rxRing;                      /* receive ring */
    buf = (char *)(rmd + pDrvCtrl->dcNumRds);
    for (ix = 0; ix < pDrvCtrl->dcNumRds; ix++, rmd++, buf += DC_BUFSIZ)
        {
        pTemp = DC_CACHE_VIRT_TO_PHYS (buf);	/* convert to PCI  phys addr */
	pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));
	rmd->rDesc2 = PCISWAP((ULONG)pTemp);	/* buffer address 1 */
	rmd->rDesc3 = 0;			/* no second buffer */

	/* buffer size */
	rmd->rDesc1 = PCISWAP(RDESC1_RBS1_VAL(DC_BUFSIZ) | 
			      RDESC1_RBS2_VAL(0));

	if (ix == (pDrvCtrl->dcNumRds - 1))	/* if its is last one */
	    rmd->rDesc1 |= PCISWAP(RDESC1_RER);	/* end of receive ring */

	rmd->rDesc0 = PCISWAP(RDESC0_OWN);	/* give ownership to lance */
        }

    /* setup Rx loaner pool */

    pDrvCtrl->nLoanRx = dcLPool;
    for (ix = 0; ix < dcLPool; ix++, buf += DC_BUFSIZ)
	{
        pDrvCtrl->lPool[ix] = buf;
        pDrvCtrl->refCnt[ix] = 0;
        pDrvCtrl->pRefCnt[ix] = & pDrvCtrl->refCnt[ix];
	}

    /* carve Tx memory structure */

    pDrvCtrl->txRing = (DC_TDE *) (((int)buf + 0x03) & ~0x03); /* word align */
    pDrvCtrl->dcNumTds = dcNumTds;	/* transmit ring size */
    pDrvCtrl->txIndex = 0;
    pDrvCtrl->txDiIndex = 0;

    /* Setup the Tx descriptors */

    tmd = pDrvCtrl->txRing;                 /* transmit ring */
    buf = (char *)(tmd + pDrvCtrl->dcNumTds);
    for (ix = 0; ix < pDrvCtrl->dcNumTds; ix++, tmd++, buf += DC_BUFSIZ)
        {
        pTemp = DC_CACHE_VIRT_TO_PHYS (buf);	/* convert to PCI phys addr */
	pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));
	tmd->tDesc2 = PCISWAP((ULONG)pTemp);	/* buffer address 1 */
	tmd->tDesc3 = 0;			/* no second buffer */

	tmd->tDesc1 = PCISWAP((TDESC1_TBS1_PUT(0) |
			       TDESC1_TBS2_PUT(0) |
			       TDESC1_IC	  | 	/* intrpt on xmit */
			       TDESC1_LS          |	/* last segment */
			       TDESC1_FS));		/* first segment */

	if (ix == (pDrvCtrl->dcNumTds - 1))	/* if its is last one */
	    tmd->tDesc1 |= PCISWAP(TDESC1_TER);	/* end of Xmit ring */

	tmd->tDesc0 = 0 ;			/* clear status */
        }

    /* carve Setup Filter Frame buffer */
    
    pDrvCtrl->pFltrFrm = (ULONG *)(((int)buf + 0x03) & ~0x03); /* word align */
    buf += FLTR_FRM_SIZE;

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Device Initialization */

    dcChipReset (pDrvCtrl);			/* reset the chip */

    pTemp = DC_CACHE_VIRT_TO_PHYS (pDrvCtrl->rxRing);
    pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));

    dcCsrWrite (devAdrs, CSR3, ((ULONG)pTemp));	/* recv descriptor */

    pTemp = DC_CACHE_VIRT_TO_PHYS (pDrvCtrl->txRing);
    pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));

    dcCsrWrite (devAdrs, CSR4, ((ULONG)pTemp));	/* xmit descriptor */

    /* get the ethernet address */

/* RCP: If DEC21140 make an alternate call */
    if (!DEC21140(pDrvCtrl->dcOpMode))
    {
       if (dcEnetAddrGet (devAdrs, (char *) pDrvCtrl->idr.ac_enaddr, 6) != OK)
	   logMsg ("dc: error reading ethernet rom\n", 0,0,0,0,0,0);
    }
    else
/* Get the ethernet address for the DEC21140 */
    {
       if (dc21140EnetAddrGet (devAdrs, (char *) pDrvCtrl->idr.ac_enaddr, 6) 
	   != OK)
	   logMsg ("dc21140: error reading ethernet rom\n", 0,0,0,0,0,0);
    }

/* RCP: Check to see if Full Duplex mode set */
    if ((pDrvCtrl->dcOpMode & DC_FULLDUPLEX_FLAG) && 
	(pDrvCtrl->dcOpMode != NONE))
    	dcCsrWrite (devAdrs, CSR6, (dcCsrRead(devAdrs, CSR6) | CSR6_FD));

/* RCP: DEC21040 Specific Setup */
    if (!DEC21140(pDrvCtrl->dcOpMode))
    {
        dcAuiTpInit (devAdrs);			/* configure AUI or 10BASE-T */
/* RCP: Added initialization of CSR6 to turn off Promiscuous and Multicast */
	dcCsrWrite (devAdrs, CSR6, 0x0);
    }
    else
    {
/* RCP: DEC21140 Setup - these modes must be set before start Xmitter */

/* Default to 10BT */
    	uTemp = dcCsrRead(devAdrs, CSR6);
        uTemp = uTemp | CSR6_TTM | CSR6_BIT25;

/* Now verify the parameters passed to configure the interface further */
   	if ((pDrvCtrl->dcOpMode & DC_100_MB_FLAG) &&
	    (pDrvCtrl->dcOpMode != NONE))
	{

/* Hard Coded values from SROM offset to start 100MB testing.  The actual code should
   be imbellished to handle the configuration for other possible interfaces other then
   100BaseTx.  This information will be stored in the DC21140 Info Leaf of the SROM.
   In the case of the DEC520-AA PMC, the starting offset for the DEC21140 Info Leaf
   is 0x41.  This location contains some basic capabilities of the interface and the
   initial state of CSR12.  Also there is a block count for the total number of interfaces
   supported by the interface card.  For testing the default values for the SYM_SCR 
   (100MBaseTx) port will be used.  The information for this interface starts in offset
   0x49. The configuration for Full Duplex starts in offset 0x4D.  Potentially the driver
   could be altered to do some autosensing and configuration, but for now arguments will
   be passed to the driver to configure it.

*/
	   dcCsrWrite (devAdrs, CSR12, INIT_CSR12);
	   dcCsrWrite (devAdrs, CSR12, SYM_MODE);
	   
   	   if ((pDrvCtrl->dcOpMode & DC_HBE_FLAG) &&
		(pDrvCtrl->dcOpMode != NONE))
		uTemp = (uTemp & ~CSR6_TTM) | CSR6_PS;
	   else
		uTemp = (uTemp & ~CSR6_TTM) | CSR6_PS | CSR6_HBD ;

	   uTemp = uTemp | (((pDrvCtrl->dcOpMode & DC_SCRAMBLER_FLAG) &&
		  (pDrvCtrl->dcOpMode != NONE)) ? CSR6_SCR : 0) |
		  (((pDrvCtrl->dcOpMode & DC_PCS_FLAG) &&
		  (pDrvCtrl->dcOpMode != NONE)) ? CSR6_PCS : 0);
	}

	dcCsrWrite (devAdrs, CSR6, uTemp);
    }

/* RCP: End of DEC21140 Code Addition */

    /* clear the status register */

    dcCsrWrite (devAdrs, CSR5, 0xffffffff);

    /* set the operating mode to start Xmitter only */

/* RCP: Modify this call to accomodate DEC21140 , add the Setting of Bit 25*/
    dcCsrWrite (devAdrs, CSR6, 
	       (dcCsrRead (devAdrs, CSR6) |  CSR6_ST | CSR6_BIT25));

/* RCP: Added Loopback modes */

    if ((pDrvCtrl->dcOpMode & DC_ILOOPB_FLAG) &&
	(pDrvCtrl->dcOpMode != NONE))

/* RCP: Internal loopback selected */
    	dcCsrWrite (devAdrs, CSR6, (dcCsrRead(devAdrs, CSR6) | CSR6_OM_ILB));
    else
	
    	if ((pDrvCtrl->dcOpMode & DC_ELOOPB_FLAG) &&
	    (pDrvCtrl->dcOpMode != NONE))
/* RCP: External loopback selected */
	   dcCsrWrite (devAdrs, CSR6, (dcCsrRead(devAdrs, CSR6) | CSR6_OM_ELB));

    /* Xmit a Filter Setup Frame */

    dcFltrFrmXmit (pDrvCtrl, (char *) pDrvCtrl->idr.ac_enaddr, 1);

    /* set operating mode any additional operational mode set by user */

    dcCsrWrite (devAdrs, CSR6, (dcCsrRead(devAdrs, CSR6) | 
                               CSR6_SR                   | /* start receiver */
   			       (((pDrvCtrl->dcOpMode & DC_MULTICAST_FLAG) &&
				 (pDrvCtrl->dcOpMode != NONE)) ? CSR6_PM: 0) |
   			       (((pDrvCtrl->dcOpMode & DC_PROMISCUOUS_FLAG) &&
				 (pDrvCtrl->dcOpMode != NONE)) ? CSR6_PR: 0)
			      )); 

    /* set up the interrupts */

/* RCP: Modified to accomodate DEC21140 */
    if (!DEC21140(pDrvCtrl->dcOpMode))
    	dcCsrWrite (devAdrs, CSR7, (
			       CSR7_NIM |	/* normal interrupt mask */
			       CSR7_RIM |	/* rcv  interrupt mask */
			       CSR7_TIM |	/* xmit interrupt mask */
			       CSR7_TUM |	/* xmit buff unavailble mask */
			       CSR7_AIM |	/* abnormal interrupt mask */
			       CSR7_SEM |	/* system error mask */
			       CSR7_LFM |	/* link fail mask */
			       CSR7_RUM		/* rcv buff unavailable mask */
			       ));
/* RCP: Removed CSR7_LFM for DEC21140 */
    else
    	dcCsrWrite (devAdrs, CSR7, (
			       CSR7_NIM |	/* normal interrupt mask */
			       CSR7_RIM |	/* rcv  interrupt mask */
			       CSR7_TIM |	/* xmit interrupt mask */
			       CSR7_TUM |	/* xmit buff unavailble mask */
			       CSR7_AIM |	/* abnormal interrupt mask */
			       CSR7_SEM |	/* system error mask */
			       CSR7_RUM		/* rcv buff unavailable mask */
			       ));

    sysLanIntEnable (ilevel);                   /* enable LANCE interrupts */


    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);

    /* Set our flag */

    pDrvCtrl->attached = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* dcReset - reset the interface
*
* Mark interface as inactive & reset the chip
*/

LOCAL void dcReset
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl = & drvCtrl [unit];

    pDrvCtrl->idr.ac_if.if_flags = 0;
    dcChipReset (pDrvCtrl);                           /* reset LANCE */
    }

/*******************************************************************************
*
* dcInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*/

LOCAL void dcInt
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to device control struct */
    )
    {
    DC_TDE *	tmd;		/* pointer to Xmit ring descriptor */
    ULONG	stat;		/* status register */
    ULONG	devAdrs;	/* device address */

    devAdrs = pDrvCtrl->devAdrs;

    /* Read the device status register */

    stat = dcCsrRead(devAdrs, CSR5);

    /* clear the interrupts */

    dcCsrWrite(devAdrs, CSR5, stat);

    /* If false interrupt, return. */

    if ( ! (stat & (CSR5_NIS | CSR5_AIS)) )
        return;

    /* Check for system error */

    if (stat & CSR5_SE)
        {
        ++pDrvCtrl->idr.ac_if.if_ierrors;

        /* restart chip on fatal error */

	pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);

	(void) netJobAdd (
			 (FUNCPTR)dcRestart,
			 pDrvCtrl->idr.ac_if.if_unit,
			 0,0,0,0
			 );
	return;
        }

    /* Have netTask handle any input packets */

    if ((stat & CSR5_RI) && (!(stat & CSR5_RPS)))
        {
	(void) netJobAdd (
			 (FUNCPTR)dcHandleRecvInt,
			 (int)pDrvCtrl,
			 0,0,0,0
			 );
	/* disable the Rx intr, re-enable in dcHandleRecvInt() */

	dcCsrWrite(devAdrs, CSR7, (stat & ~CSR7_RIM));
        }

    /*
     * Did LANCE update any of the TMD's?
     * If not then don't bother continuing with transmitter stuff
     */

    if (!(stat & CSR5_TI))
        return;

    while (pDrvCtrl->txDiIndex != pDrvCtrl->txIndex)
        {
        /* disposal has not caught up */

        tmd = pDrvCtrl->txRing + pDrvCtrl->txDiIndex;

        /* if the buffer is still owned by LANCE, don't touch it */

        DC_CACHE_INVALIDATE (tmd, TMD_SIZ);
        if (tmd->tDesc0 & PCISWAP(TDESC0_OWN))
            break;

        /* now bump the tmd disposal index pointer around the ring */

        pDrvCtrl->txDiIndex = (pDrvCtrl->txDiIndex + 1) % pDrvCtrl->dcNumTds;

        /*
         * TDESC0.ES is an "OR" of LC, NC, UF, EC.
         * here for error conditions.
         */

        if (tmd->tDesc0 & PCISWAP(TDESC0_ES))
            {

            pDrvCtrl->idr.ac_if.if_oerrors++;     /* output error */
            pDrvCtrl->idr.ac_if.if_opackets--;

            /* If error was due to excess collisions, bump the collision
             * counter.
             */
            if (tmd->tDesc0 & PCISWAP(TDESC0_EC))
                pDrvCtrl->idr.ac_if.if_collisions += 16;

	    /* bump the collision counter if late collision */

            if (tmd->tDesc0 & PCISWAP(TDESC0_LC))
                pDrvCtrl->idr.ac_if.if_collisions++;

            /* check for no carrier */

            if (tmd->tDesc0 & PCISWAP(TDESC0_NC | TDESC0_LO | TDESC0_LF))
                logMsg ("dc%d: no carrier\n",
                    pDrvCtrl->idr.ac_if.if_unit, 0,0,0,0,0);

            /* Restart chip on fatal errors.
             * The following code handles the situation where the transmitter
             * shuts down due to an underflow error.  This is a situation that
             * will occur if the DMA cannot keep up with the transmitter.
             * It will occur if the LANCE is being held off from DMA access
             * for too long or due to significant memory latency.  DRAM
             * refresh or slow memory could influence this.  Many
             * implementation use a dedicated LANCE buffer.  This can be
             * static RAM to eliminate refresh conflicts; or dual-port RAM
             * so that the LANCE can have free run of this memory during its
             * DMA transfers.
             */

            if (tmd->tDesc0 & PCISWAP(TDESC0_UF))
                {
                pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);
                (void) netJobAdd (
                                 (FUNCPTR)dcRestart,
                                 pDrvCtrl->idr.ac_if.if_unit,
                                 0,0,0,0
                                 );
                return;
                }
            }
	tmd->tDesc0 = 0;	/* clear all error & stat stuff */
        }

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    }

/*******************************************************************************
*
* dcHandleRecvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/

LOCAL void dcHandleRecvInt
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to device control struct */
    )
    {
    DC_RDE *	rmd;		/* pointer to Rx ring descriptor */

    while ((rmd = dcGetFullRMD (pDrvCtrl)) != NULL)
    	dcRecv (pDrvCtrl, rmd);

    /* re-enable the Rx interrupt */

    dcCsrWrite (pDrvCtrl->devAdrs, CSR7, (dcCsrRead (pDrvCtrl->devAdrs, CSR7) |
					  CSR7_RIM));
    }

/*******************************************************************************
*
* dcGetFullRMD - get next received message RMD
*
* Returns ptr to next Rx desc to process, or NULL if none ready.
*/

LOCAL DC_RDE * dcGetFullRMD
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to device control struct */
    )
    {
    DC_RDE *	rmd;		/* pointer to Rx ring descriptor */

    /* Refuse to do anything if flags are down */

    if  (
        (pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING) ) !=
        (IFF_UP | IFF_RUNNING)
        )
        return ((DC_RDE *) NULL);


    rmd = pDrvCtrl->rxRing + pDrvCtrl->rxIndex;       /* form ptr to Rx desc */

    DC_CACHE_INVALIDATE (rmd, RMD_SIZ);


    if ((rmd->rDesc0 & PCISWAP(RDESC0_OWN)) == 0)
        return (rmd);
    else
        return ((DC_RDE *) NULL);
    }

/*******************************************************************************
*
* dcRecv - process the next incoming packet
*
*/


LOCAL STATUS dcRecv
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to device control struct */
    DC_RDE *	rmd		/* pointer to Rx ring descriptor */
    )
    {
    ENET_HDR *	pEnetHdr;	/* pointer to ethernet header */
    MBUF *	pMbuf = NULL;	/* pointer to mbuf */
    u_long	pPhys;
    UCHAR *	pData;		/* pointer to data */
    int		len;		/* length */
    USHORT	ether_type;	/* ether packet type */

    /* Packet must be checked for errors. */

    if  (rmd->rDesc0 & PCISWAP(RDESC0_ES)) /* If error flag */
        {
	/* error can occur on overflow, crc error, collision, 
	 * frame too long, runt frame or length error
	 */
        ++pDrvCtrl->idr.ac_if.if_ierrors;       /* bump error stat */
        goto cleanRXD;                          /* skip to clean up */
        }

    ++pDrvCtrl->idr.ac_if.if_ipackets;		/* bump statistic */

    len = RDESC0_FL_GET(PCISWAP(rmd->rDesc0));	/* frame length */

    len -= 4; 	/* Frame length includes CRC in it so subtract it */

    /* Get pointer to packet */

    pEnetHdr = DC_CACHE_PHYS_TO_VIRT(PCI_TO_MEM_PHYS(PCISWAP(rmd->rDesc2)));

    DC_CACHE_INVALIDATE (pEnetHdr, len);   /* make the packet data coherent */


    /* call input hook if any */

    if ((etherInputHookRtn == NULL) || ((*etherInputHookRtn)
       (& pDrvCtrl->idr.ac_if, (char *) pEnetHdr, len)) == 0)
        {
        /* Adjust length to size of data only */

        len -= SIZEOF_ETHERHEADER;

        /* Get pointer to packet data */

        pData = ((u_char *) pEnetHdr) + SIZEOF_ETHERHEADER;

        ether_type = ntohs ( pEnetHdr->type );

        /* OK to loan out buffer ? -> build an mbuf cluster */
    
	if ((pDrvCtrl->nLoanRx > 0) && (USE_CLUSTER (len))
	    &&
	    ((pMbuf = build_cluster (pData, len, & pDrvCtrl->idr, MC_LANCE,
	     pDrvCtrl->pRefCnt [(pDrvCtrl->nLoanRx - 1)], dcLoanFree,
	     (int) pDrvCtrl, (int) pEnetHdr,
	     (int) pDrvCtrl->pRefCnt [(pDrvCtrl->nLoanRx - 1)])) != NULL))
	    {
            pPhys = (u_long) DC_CACHE_VIRT_TO_PHYS(MEM_TO_PCI_PHYS(
		    (pDrvCtrl->lPool[--pDrvCtrl->nLoanRx])));
	    rmd->rDesc2 = PCISWAP(pPhys);
	    }
        else
            {
            if ((pMbuf = bcopy_to_mbufs (pData, len, 0, & pDrvCtrl->idr.ac_if,
                pDrvCtrl->memWidth)) == NULL)
		{
                ++pDrvCtrl->idr.ac_if.if_ierrors;	/* bump error stat */
                goto cleanRXD;
		}
            }

        /* send on up... */

        do_protocol_with_type (ether_type, pMbuf, & pDrvCtrl->idr, len);

/* RCP: This is a redundant increment of incoming packets, removed from original driver */
/*        ++pDrvCtrl->idr.ac_if.if_ipackets;*/	/* bump statistic */

        }

    /* Done with descriptor, clean up and give it to the device. */

    cleanRXD:

    /* clear status bits and give ownership to device */

    rmd->rDesc0 = PCISWAP(RDESC0_OWN);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Advance our management index */

    pDrvCtrl->rxIndex = (pDrvCtrl->rxIndex + 1) % pDrvCtrl->dcNumRds;

    return (OK);
    }

/*******************************************************************************
*
* dcOutput - the driver output routine
*
*/

LOCAL int dcOutput
    (
    IDR  *	pIDR,		/* pointer to interface data record */
    MBUF *	pMbuf,		/* pointer to mbuf */
    SOCK *	pDest		/* pointer to destination sock */
    )
    {
    char 	destEnetAddr [6];	/* space for enet addr */
    USHORT	packetType;		/* type field for the packet */
    DRV_CTRL *	pDrvCtrl;		/* pointer to device control struct */
    DC_TDE *	tmd;			/* pointer to Tx ring descriptor */
    char *	buf;			/* pointer to buffer */
    int		len;			/* length */

    /* Check ifnet flags. Return error if incorrect. */

    if  (
        (pIDR->ac_if.if_flags & (IFF_UP | IFF_RUNNING)) !=
        (IFF_UP | IFF_RUNNING)
        )
        return (ENETDOWN);

    /* Attempt to convert socket addr into enet addr and packet type.
     * Note that if ARP resolution of the address is required, the ARP
     * module will call our routine again to transmit the ARP request
     * packet.  This means we may actually call ourselves recursively!
     */

    if (convertDestAddr (pIDR,pDest, destEnetAddr, &packetType, pMbuf) == FALSE)
        return (OK);    /* I KNOW returning OK is stupid, but it is correct */

    /* Get driver control pointer */

    pDrvCtrl = & drvCtrl [pIDR->ac_if.if_unit];

    /* Obtain exclusive access to transmitter.  This is necessary because
     * certain events can cause netTask to run a job that attempts to transmit
     * a packet.  We can only allow one task here at a time.
     */

    semTake (pDrvCtrl->TxSem, WAIT_FOREVER);

    /* See if next TXD is available */

    tmd = pDrvCtrl->txRing + pDrvCtrl->txIndex;

    DC_CACHE_INVALIDATE (tmd, TMD_SIZ);

    if  (
        ((tmd->tDesc0 & PCISWAP(TDESC0_OWN)) != 0) ||
        (
        ((pDrvCtrl->txIndex + 1) % pDrvCtrl->dcNumTds) == pDrvCtrl->txDiIndex
	)
        )
        {
        m_freem (pMbuf);            /* Discard data */
        goto outputDone;
        }

    /* Get pointer to transmit buffer */

    buf = (char *)DC_CACHE_PHYS_TO_VIRT(PCI_TO_MEM_PHYS(PCISWAP(tmd->tDesc2)));

    /* Copy packet from MBUFs to our transmit buffer.  MBUFs are
     * transparently freed.
     */
    bcopy_from_mbufs ((buf + SIZEOF_ETHERHEADER), pMbuf, len,
		      pDrvCtrl->memWidth);

    /* Fill in the Ethernet header */

    bcopy (destEnetAddr, buf, 6);
    bcopy ( (char *) pIDR->ac_enaddr, (buf + 6), 6);
    ((ENET_HDR *)buf)->type = packetType;

    tmd->tDesc0 = 0;				/* clear buffer error status */
    tmd->tDesc1 &= PCISWAP(~TDESC1_TBS1_MSK);
    tmd->tDesc1 |= PCISWAP(TDESC1_TBS1_PUT(max (ETHERSMALL, 
						(len + SIZEOF_ETHERHEADER))));
    tmd->tDesc0 = PCISWAP(TDESC0_OWN); 		/* give ownership to device */

    CACHE_PIPE_FLUSH ();			/* Flush the write pipe */

    /* Advance our management index */

    pDrvCtrl->txIndex = (pDrvCtrl->txIndex + 1) % pDrvCtrl->dcNumTds;
    
    if (dcKickStartTx)
    	dcCsrWrite(pDrvCtrl->devAdrs, CSR1, CSR1_TPD);	/* xmit poll demand */

    /* Bump the statistic counter. */

    pIDR->ac_if.if_opackets++;

    outputDone:

    /* Release exclusive access. */

    semGive (pDrvCtrl->TxSem);

    return (OK);

    }

/*******************************************************************************
*
* dcIoctl - the driver I/O control routine
*
* Process an ioctl request.
*/

LOCAL int dcIoctl
    (
    IDR  *	ifp,		/* pointer interface data record */
    int		cmd,		/* command */
    caddr_t	data		/* data */
    )
    {
    int 	error = 0;
    DRV_CTRL *  pDrvCtrl = (DRV_CTRL *)ifp;	/* pointer to device */

    switch (cmd)
        {
        case SIOCSIFADDR:
            ifp->ac_ipaddr = IA_SIN (data)->sin_addr;
            break;

        case SIOCSIFFLAGS:
            /* No further work to be done */
            break;

        case IFF_PROMISC:	/* set device in promiscuous mode */
	    dcCsrWrite (pDrvCtrl->devAdrs, CSR6, 
			(dcCsrRead(pDrvCtrl->devAdrs, CSR6) | CSR6_PR));
	    break;

        default:
            error = EINVAL;
        }

    return (error);
    }

/*******************************************************************************
*
* dcChipReset - hardware reset of chip (stop it)
*/

LOCAL int dcChipReset
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to device control structure */
    )
    {
    ULONG  	devAdrs = pDrvCtrl->devAdrs;	/* device base address */
    ULONG	ix;				/* index */
	
    dcCsrWrite(devAdrs, CSR6, 0);		/* stop rcvr & xmitter */

    dcCsrWrite(devAdrs, CSR7, 0);		/* mask interrupts */
    
    dcCsrWrite(devAdrs, CSR0, CSR0_SWR);

    /* Wait Loop, Loop for at least 50 PCI cycles according to chip spec */

    for (ix = 0; ix < 0x1000; ix++)
	;

    /* any additional bus mode | give xmit & rcv process equal priority */

    dcCsrWrite (devAdrs, CSR0, dcCSR0Bmr | CSR0_BAR);

    for (ix = 0; ix < 0x1000; ix++)		/* loop for some cycles */
	;

    return (OK);
    }

/*******************************************************************************
*
* dcAuiTpInit - initialize either AUI or 10BASE-T connection
*
* This function configures 10BASE-T interface. If the link pass state is 
* not achieved within two seconds then the AUI interface is initialized.
*/

LOCAL void dcAuiTpInit
    (
    ULONG       devAdrs        /* device base I/O address */
    )
    {

    /* reset the SIA registers */

    dcCsrWrite (devAdrs, CSR13, 0);
    dcCsrWrite (devAdrs, CSR14, 0);
    dcCsrWrite (devAdrs, CSR15, 0);

    /* configure for the 10BASE-T */

    dcCsrWrite (devAdrs, CSR13, CSR13_CAC_CSR);	/* 10BT auto configuration */

    taskDelay (sysClkRateGet() * 2);		/* 2 second delay */

    if (dcCsrRead (devAdrs, CSR12) & (CSR12_LKF | CSR12_NCR))
	{
	/* 10BASE-T not connected initialize interface for AUI */

	dcCsrWrite (devAdrs, CSR13, 0);		/* reset SIA registers */
	dcCsrWrite (devAdrs, CSR14, 0);
	dcCsrWrite (devAdrs, CSR15, 0);

	/* AUI auto configuration */
	dcCsrWrite (devAdrs, CSR13, (CSR13_AUI_TP | CSR13_CAC_CSR));
	}
    }

/*******************************************************************************
*
* dcCsrWrite - select and write a CSR register
*
*/
LOCAL void dcCsrWrite
    (
    ULONG 	devAdrs,		/* device address base */
    int 	reg,			/* register to select */
    ULONG 	value			/* value to write */
    )
    {
    ULONG *	csrReg;

    csrReg = (ULONG *)(devAdrs + (reg * DECPCI_REG_OFFSET));

    /* write val to CSR */

    *(csrReg) = PCISWAP(value);
    }

/*******************************************************************************
*
* dcCsrRead - select and read a CSR register
*
*/

LOCAL ULONG dcCsrRead
    (
    ULONG	devAdrs,		/* device address base */
    int		reg			/* register to select */
    )
    {
    ULONG *	csrReg;			/* csr register */
    ULONG	csrData;		/* data in csr register */

    csrReg = (ULONG *)(devAdrs + (reg * DECPCI_REG_OFFSET));

    csrData = *csrReg;

    /* get contents of CSR */

    return ( PCISWAP(csrData));
    }

/*******************************************************************************
*
* dcRestart - restart the device after a fatal error
*
* This routine takes care of all the messy details of a restart.  The device
* is reset and re-initialized.  The driver state is re-synchronized.
*/

LOCAL void dcRestart
    (
    int 	unit		/* unit to restart */
    )
    {
    ULONG	status;		/* status register */	
    int		ix;		/* index variable */
    DC_RDE *	rmd;		/* pointer to receive descriptor */
    DC_TDE *	tmd;		/* pointer to Xmit descriptor */
    DRV_CTRL *	pDrvCtrl = & drvCtrl [unit];

    rmd = pDrvCtrl->rxRing;      /* receive ring */
    tmd = pDrvCtrl->txRing;      /* transmit ring */

    pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING );
    fprintf (stderr, "Fatal Error. DEC Ethernet Chip Restarted\n");

    status = dcCsrRead(pDrvCtrl->devAdrs, CSR6);
    dcCsrWrite (pDrvCtrl->devAdrs, CSR6, 0);	/* reset mode register */

    for (ix = 0; ix < pDrvCtrl->dcNumRds; ix++, rmd++)
        {
	/* buffer size */
	rmd->rDesc1 = PCISWAP(RDESC1_RBS1_VAL(DC_BUFSIZ) | 
			      RDESC1_RBS2_VAL(0));

	if (ix == (pDrvCtrl->dcNumRds - 1)) /* if its is last one */
	rmd->rDesc1 |= PCISWAP(RDESC1_RER); /* end of receive ring */

	rmd->rDesc0 = PCISWAP(RDESC0_OWN); /* give ownership to lance */
        }

    for (ix = 0; ix < pDrvCtrl->dcNumTds; ix++, tmd++)
        {
	tmd->tDesc1 = PCISWAP((TDESC1_TBS1_PUT(0) |
			       TDESC1_TBS2_PUT(0) |
			       TDESC1_IC	  | 	/* intrpt on xmit */
			       TDESC1_LS          |	/* last segment */
			       TDESC1_FS));		/* first segment */

	if (ix == (pDrvCtrl->dcNumTds - 1))	/* if its is last one */
	    tmd->tDesc1 |= PCISWAP(TDESC1_TER);	/* end of Xmit ring */

	tmd->tDesc0 = 0 ;			/* clear status */
        }

    /* clear the status register */

    dcCsrWrite (pDrvCtrl->devAdrs, CSR5, 0xffffffff);

    dcCsrWrite (pDrvCtrl->devAdrs, CSR6, status);	/* restore mode */

    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);

    }

/*******************************************************************************
*
* dcLoanFree - return the given buffer to loaner pool
*
* This routine returns <pRxBuf> to the pool of available loaner buffers.
* It also returns <pRef> to the pool of available loaner reference counters,
* then zeroes the reference count.
*
* RETURNS: N/A
*/
 
LOCAL void dcLoanFree
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to device control structure */
    char *	pRxBuf,		/* pointer to receive buffer to free */
    UINT8 *	pRef		/* pointer to reference count */
    )
    {
    /* return loaned buffer to pool */
 
    pDrvCtrl->lPool[pDrvCtrl->nLoanRx] = pRxBuf;
 
    /* return loaned reference count to pool */
 
    pDrvCtrl->pRefCnt[pDrvCtrl->nLoanRx++] = pRef;

    /* reset reference count - should have been done from above, but... */
 
    *pRef = 0;
    }

/*******************************************************************************
*
* dcEnetAddrGet - gets the ethernet address from the ROM register.
*
* This routine gets the ethernet address from the ROM register.
* This routine returns the ethernet address into the pointer supplied to it.
* 
* RETURNS: OK/ERROR
*/

LOCAL STATUS dcEnetAddrGet
    (
    ULONG	devAdrs,	/* device base I/O address */
    char * 	enetAdrs,	/* pointer to the ethernet address */ 
    int		len		/* number of bytes to read */
    )
    {
    FAST ULONG	csr9Value;		/* register to hold CSR9 */
    int		ix;			/* index register */
    BOOL	eRomReady = FALSE;	/* ethernet ROM register state */

    dcCsrWrite (devAdrs, CSR9, 0);	/* reset rom pointer */

    while (len > 0)
	{
	for (ix = 0; ix < 10; ix++)	/* try at least 10 times */
	    {
	    if ((csr9Value = dcCsrRead (devAdrs, CSR9)) & CSR9_DNV)
		{
		eRomReady = FALSE;
	    	DELAY(500);
		}
	    else
		{
		*enetAdrs++ = (UCHAR) csr9Value;
		len--;
		eRomReady = TRUE;
		break;
		}
	    }
	}

    if (!eRomReady)
    	return (ERROR);
    else
	return (OK);
    }

/*******************************************************************************
*
* RCP : New utility for DEC21140
* dc21140EnetAddrGet - gets the ethernet address from the ROM register 
*
* This routine gets the last two bytes of the ethernet address from the ROM
* register and concantenates this value to the predefined values for DEC PMCs..
* This routine returns the ethernet address into the pointer supplied to it.
*
* The procedure could be expanded in the future to allow for modification of
* Ethernet addresses in the serial ROM.  It will have to be modified to
* accomodate the differences in the onboard 100Mb/s ethernet on future 
* Motorola products.
* 
* RETURNS: OK/ERROR
*/

LOCAL STATUS dc21140EnetAddrGet
    (
    ULONG	devAdrs,	/* device base I/O address */
    char * 	enetAdrs,	/* pointer to the ethernet address */ 
    int		len		/* number of bytes to read */
    )
    {
    int		i,ix,		/* index register */
    		tmpInt;		/* temporary int */
    union mixed
    {
    	char	Char[4];	/* temporary 2 by char */
	USHORT	Short[2];	/* temporary USHORT */
	ULONG   Long;		/* temporay ULONG */
    } tmp;

    for (i = DEC_PMC_POS, ix = 0; i<(3+DEC_PMC_POS); i++, ix++)
    {
    	if ((tmp.Short[0] = dcReadRom (devAdrs, i)) == ERROR)
	   return (ERROR);
	enetAdrs[ix++] = tmp.Char[1];
	enetAdrs[ix] = tmp.Char[0];
    }
    tmpInt = -1;
    return (OK);
    }


/*******************************************************************************
*
* dcFltrFrmXmit - Transmit the setup filter frame.
*
* This routine transmits the setup filter frame.
* The setup frame is not transmitted actually over the wire. The setup frame
* which is transmitted is 192 bytes. The tranmitter should be in an on state
* before this function is called. This call has been coded so that the 
* setup frame can be transmitted after the chip is done with the 
* intialization taking into consideration for adding multicast support.
* 
* RETURNS: OK/ERROR
*/

LOCAL int dcFltrFrmXmit
    (
    DRV_CTRL *	pDrvCtrl, 	/* pointer to device control structure */
    char * 	pPhysAdrsTbl, 	/* pointer to physical address table */ 
    int 	tblLen		/* size of the physical address table */
    )
    {
    DC_TDE *	tmd;		/* pointer to Tx ring descriptor */
    ULONG * 	pBuff;		/* pointer to the Xmit buffer */
    int		status = OK;	/* intialize status as OK */
    ULONG	csr7Val;	/* value in CSR7 */

    semTake (pDrvCtrl->TxSem, WAIT_FOREVER);

    if (dcFltrFrmSetup (pDrvCtrl, pPhysAdrsTbl, tblLen) != OK)
	{
	status = ERROR;		/* not able to set up filter frame */
	goto setupDone;		/* set up done */
	}

    /* See if next TXD is available */

    tmd = pDrvCtrl->txRing + pDrvCtrl->txIndex;

    DC_CACHE_INVALIDATE (tmd, TMD_SIZ);

    if  (
        ((tmd->tDesc0 & PCISWAP(TDESC0_OWN)) != 0) ||
        (
        ((pDrvCtrl->txIndex + 1) % pDrvCtrl->dcNumTds) == pDrvCtrl->txDiIndex
	)
        )
	{
	status = ERROR;		/* not able to set up filter frame */
	goto setupDone;		/* set up done */
	}

    /* Get pointer to transmit buffer */

    pBuff = DC_CACHE_PHYS_TO_VIRT(PCI_TO_MEM_PHYS(PCISWAP(tmd->tDesc2)));

    /* copy into Xmit buffer */

    bcopyLongs ((char *)pDrvCtrl->pFltrFrm, (char *)pBuff, 
		FLTR_FRM_SIZE_ULONGS);		

    tmd->tDesc0 = 0;				/* clear buffer error status */
    tmd->tDesc1 |= PCISWAP(TDESC1_SET);		/* frame type as set up */
    tmd->tDesc1 &= PCISWAP(~TDESC1_TBS1_MSK);
    tmd->tDesc1 |= PCISWAP(TDESC1_TBS1_PUT(FLTR_FRM_SIZE));
    tmd->tDesc0 = PCISWAP(TDESC0_OWN); 		/* give ownership to device */

    CACHE_PIPE_FLUSH ();			/* Flush the write pipe */

    /* Advance our management index */

    pDrvCtrl->txIndex = (pDrvCtrl->txIndex + 1) % pDrvCtrl->dcNumTds;
    pDrvCtrl->txDiIndex = (pDrvCtrl->txDiIndex + 1) % pDrvCtrl->dcNumTds;

    csr7Val = dcCsrRead (pDrvCtrl->devAdrs, CSR7);

    dcCsrWrite(pDrvCtrl->devAdrs, CSR7, 0);	/* mask interrupts */
    
    if (dcKickStartTx)
    	dcCsrWrite(pDrvCtrl->devAdrs, CSR1, CSR1_TPD);	/* xmit poll demand */

    /* wait for the own bit to change */

    while (tmd->tDesc0 & PCISWAP (TDESC0_OWN))
	;

    tmd->tDesc0 = 0;				/* clear status bits */
    tmd->tDesc1 &= PCISWAP(~TDESC1_SET);

    dcCsrWrite(pDrvCtrl->devAdrs, CSR7, csr7Val); /* restore value */

    setupDone:

    /* Release exclusive access. */

    semGive (pDrvCtrl->TxSem);

    return (status);
    }

/*******************************************************************************
*
* dcFltrFrmSetup - set up the filter frame.
*
* This routine sets up the filter frame to filter the physical addresses
* on the incoming frames. The setup filter frame buffer is 192 bytes. This
* setup frame needs to be transmitted before transmitting and receiving 
* any frames. 
* 
* RETURNS: OK/ERROR
*/

LOCAL int dcFltrFrmSetup
    (
    DRV_CTRL *	pDrvCtrl, 	/* pointer to device control structure */
    char * 	pPhysAdrsTbl, 	/* pointer to physical address table */ 
    int 	tblLen		/* size of the physical address table */
    )
    {
    int		ix;		/* index variable */
    int		jx; 		/* inner index variable */
    ULONG *	pFltrFrm;	/* pointer to the filter frame */
    USHORT *	pPhysAddrs;	/* pointer to the physical addresses */

    if (tblLen > FLTR_FRM_ADRS_NUM)
	return (ERROR);

    pFltrFrm = pDrvCtrl->pFltrFrm;

    for (ix = 0; ix < FLTR_FRM_SIZE_ULONGS; ix++)
	*pFltrFrm++ = FLTR_FRM_DEF_ADRS;

    pFltrFrm = pDrvCtrl->pFltrFrm;
    pPhysAddrs = (USHORT *) (pPhysAdrsTbl);

    for (ix = 0; ix < tblLen; ix++)
	{
	for (jx = 0; jx < (FLTR_FRM_ADRS_SIZE/sizeof (USHORT)); jx++)
	    {
#if (_BYTE_ORDER == _BIG_ENDIAN)
	    *pFltrFrm =  (*pPhysAddrs << 16);
#else
	    *pFltrFrm = ((LSB(*pPhysAddrs) << 8) | MSB(*pPhysAddrs));
#endif
	    pPhysAddrs++;
	    pFltrFrm++;
	    }
	}
    return (OK);
    }

#include "sys/socket.h"

/*******************************************************************************
*
* convertDestAddr - converts socket addr into enet addr and packet type
*
*/

LOCAL BOOL convertDestAddr
    (
    IDR *pIDR,                          /* ptr to i/f data record */
    SOCK *pDestSktAddr,                 /* ptr to a generic sock addr */
    char *pDestEnetAddr,                /* where to write enet addr */
    u_short *pPacketType,               /* where to write packet type */
    MBUF *pMbuf                         /* ptr to mbuf data */
    )
    {

    /***** Internet family *****/

    {
    struct in_addr destIPAddr;              /* not used */
    int trailers;                           /* not supported */

     if (pDestSktAddr->sa_family == AF_INET)
        {
        *pPacketType = ETHERTYPE_IP;            /* stuff packet type */
        destIPAddr = ((struct sockaddr_in *) pDestSktAddr)->sin_addr;

        if (!arpresolve (pIDR, pMbuf, &destIPAddr, pDestEnetAddr, &trailers))
            return (FALSE);     /* if not yet resolved */

        return (TRUE);
        }
    }


    /***** Generic family *****/

    {
    ENET_HDR *pEnetHdr;

    if (pDestSktAddr->sa_family == AF_UNSPEC)
        {
        pEnetHdr = (ENET_HDR *) pDestSktAddr->sa_data;      /* ptr to hdr */
        bcopy (pEnetHdr->dst, pDestEnetAddr, 6);            /* copy dst addr */
        *pPacketType = pEnetHdr->type;                      /* copy type */
        return (TRUE);
        }
    }


    /* Unsupported family */

    return (FALSE);

    } /* End of convertDestAddr() */

/* END OF FILE */


/*******************************************************************************
*
* RCP : New utility for DEC21140
* dcReadRom - reads an specified entry in the Serial ROM 
*
* This routine uses the line number passed to the function and returns
* the two bytes of information that is associated with it.  This will later
* be used by the dc21140GetEthernetAdr function.  It can also be used to 
* review the ROM contents itself.
*
* The function must first send some initial bit paterns to the CSR9 which 
* contains the Serial ROM Control bits.  Then the line index into the ROM
* will be evaluated bit by bit to program the ROM.  The 2 bytes of data
* will be extracted and processed into a normal pair of bytes. 
* 
* RETURNS: Short Value in ROM/ERROR
*/

#define BASE CSR9_SSR_FLAG|CSR9_SWO_FLAG

USHORT dcReadRom
    (
    ULONG	devAdrs,	/* device base I/O address */
    UCHAR	lineCnt 	/* Serial ROM line Number */ 
    )
    {
    int		ix, 			/* index register */
    		intCnt;		/* address loop count */
    USHORT	tmpShort;
    ULONG	tmpLong; 

/* Is the line offset valid, and if so, how large is it */

    if (lineCnt > SROM_SIZE || lineCnt < 0)
    {
	printf ("dcReadRom FAILED, bad lineCnt\n");
    	return (ERROR);
    }
    if (lineCnt < 64)
    {
	intCnt = 6;
	lineCnt = lineCnt << 2;  /* Prepare lineCnt for processing */
    }
    else
	intCnt = 8;

/* Command the Serial ROM to the correct Line */

/* Preamble of Command */
    dcCsrWrite (devAdrs, CSR9, 0x00000000|BASE);	/* Command 1 */
    NSDELAY (30);

    dcCsrWrite (devAdrs, CSR9, 0x00000001|BASE);	/* Command 2 */
    NSDELAY (50);
    dcCsrWrite (devAdrs, CSR9, 0x00000003|BASE);	/* Command 3 */
    NSDELAY (250);
    dcCsrWrite (devAdrs, CSR9, 0x00000001|BASE);	/* Command 4 */
    NSDELAY (100);


/* Command Phase */

    dcCsrWrite (devAdrs, CSR9, 0x00000005|BASE);	/* Command 5 */
    NSDELAY (150);
    dcCsrWrite (devAdrs, CSR9, 0x00000007|BASE);	/* Command 6 */
    NSDELAY (250);
    dcCsrWrite (devAdrs, CSR9, 0x00000005|BASE);	/* Command 7 */
    NSDELAY (250);
    dcCsrWrite (devAdrs, CSR9, 0x00000007|BASE);	/* Command 8 */
    NSDELAY (250);
    dcCsrWrite (devAdrs, CSR9, 0x00000005|BASE);	/* Command 9 */
    NSDELAY (100);
    dcCsrWrite (devAdrs, CSR9, 0x00000001|BASE);	/* Command 10 */
    NSDELAY (150);
    dcCsrWrite (devAdrs, CSR9, 0x00000003|BASE);	/* Command 11 */
    NSDELAY (250);
    dcCsrWrite (devAdrs, CSR9, 0x00000001|BASE);	/* Command 12 */
    NSDELAY (100);
    
/* Address Phase */

    for (ix=0; ix < intCnt; ix++)
    {
	tmpLong = (lineCnt & 0x80)>>5;	/* Extract and move bit to bit 3)
/* Write the command */
    	dcCsrWrite (devAdrs, CSR9, tmpLong | 0x00000001|BASE);/* Command 13 */
    	NSDELAY (150);
    	dcCsrWrite (devAdrs, CSR9, tmpLong | 0x00000003|BASE);/* Command 14 */
    	NSDELAY (250);
    	dcCsrWrite (devAdrs, CSR9, tmpLong | 0x00000001|BASE);/* Command 15 */
    	NSDELAY (100);

	lineCnt = lineCnt<<1;  /* Adjust significant address bit */
    }

    	NSDELAY (150);

/* Data Phase */

    tmpShort =0;
    for (ix=15; ix >= 0; ix--)
    {
/* Write the command */
    	dcCsrWrite (devAdrs, CSR9, 0x00000003|BASE);		/* Command 16 */
    	NSDELAY (100);
/* Extract the data */
    	tmpShort |= (((dcCsrRead (devAdrs, CSR9) & 0x00000008)>>3)
		   <<ix); 				/* Command 17 */
    	NSDELAY (150);
    	dcCsrWrite (devAdrs, CSR9, 0x00000001|BASE);		/* Command 18 */
    	NSDELAY (250);

    }

/* Finish up command */
    dcCsrWrite (devAdrs, CSR9, 0x00000000);	/* Command 19 */
    NSDELAY (100);

    return (tmpShort);
    }

/* RCP: Function to read all of serial rom and store the data in the
        data structure passed to the function.  The count value will
        indicate how much of the serial rom to read.  The routine with also
        swap the the bytes as the come in. */

void dcReadAllRom
    (
    ULONG	devAdrs,	/* device base I/O address */
    UCHAR	*buffer,	/* destination bufferr */ 
    int		cnt             /* Amount to extract in bytes */
    )
    {

    USHORT total, index;
    union tmp_type {
        USHORT Short;
    	char   Byte [2];	/* temporary 2 by char */
    } tmp;

    total = 0;
    for (index=0, total=0; total < cnt/2; index+=2, total++)
    {
	tmp.Short = dcReadRom (devAdrs, total);
	buffer[index]=tmp.Byte[1];
	buffer[index+1]=tmp.Byte[0];
    }
    }

#ifdef DC_DEBUG

int dcViewRom
    (
    ULONG	devAdrs,	/* device base I/O address */
    UCHAR	lineCnt,	/* Serial ROM line Number */ 
    int		cnt             /* Amount to display */
    )
    {

    USHORT total;
    union tmp_type {
        USHORT Short;
    	char   Byte [2];	/* temporary 2 by char */
    } tmp;


    if (lineCnt/2 < 0 || lineCnt/2 > SROM_SIZE)
	return (ERROR);

    total = 0;
    while (lineCnt/2 < SROM_SIZE && cnt > 0)
    {
	if (!(total%16))
	   printf ("\nValues for line %2d =>0x  ", total);
	tmp.Short = dcReadRom (devAdrs, lineCnt);
	printf ("%4x ",tmp.Short);
	lineCnt++ ;
	total +=2;
	cnt -=2;
    }	
    printf ("\n");
    return (total);
    }


void dcCsrShow
    (
    ULONG devAdrs
    )
    {
    printf ("\n");
    printf ("CSR0\t 0x%x\n", dcCsrRead(devAdrs, CSR0));
    printf ("CSR1\t 0x%x\n", dcCsrRead(devAdrs, CSR1));
    printf ("CSR2\t 0x%x\n", dcCsrRead(devAdrs, CSR2));
    printf ("CSR3\t 0x%x\n", dcCsrRead(devAdrs, CSR3));
    printf ("CSR4\t 0x%x\n", dcCsrRead(devAdrs, CSR4));
    printf ("CSR5\t 0x%x\n", dcCsrRead(devAdrs, CSR5));
    printf ("CSR6\t 0x%x\n", dcCsrRead(devAdrs, CSR6));
    printf ("CSR7\t 0x%x\n", dcCsrRead(devAdrs, CSR7));
    printf ("CSR8\t 0x%x\n", dcCsrRead(devAdrs, CSR8));
    printf ("CSR9\t 0x%x\n", dcCsrRead(devAdrs, CSR9));
    printf ("CSR10\t 0x%x\n", dcCsrRead(devAdrs, CSR10));
    printf ("CSR11\t 0x%x\n", dcCsrRead(devAdrs, CSR11));
    printf ("CSR12\t 0x%x\n", dcCsrRead(devAdrs, CSR12));
    printf ("CSR13\t 0x%x\n", dcCsrRead(devAdrs, CSR13));
    printf ("CSR14\t 0x%x\n", dcCsrRead(devAdrs, CSR14));
    printf ("CSR15\t 0x%x\n", dcCsrRead(devAdrs, CSR15));

    }
#endif /* DC_DEBUG */

/*  RCP:  New Versions of code that will be added to the BSP later. */
/******************************************************************************
*
* sysLanIntEnable - enable the LAN interrupt
*
* This routine enables interrupts at a specified level for the on-board
* LAN chip.  LAN interrupts are controlled by the IBC chip.
* The LANCE chip on this board is on the PCI bus. The PCI interrupts should
* be routed through the IBC to the processor. LANCE chip asserts PCI IRQ0
* which has to be routed through IBC to generate ISA IRQ10. This routing 
* done through programming the PCI route registers in the IBC chip.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysLanIntDisable()
*/

LOCAL STATUS sysLanIntEnable
    (
     int intLevel 		/* interrupt level to enable */
    )
    {
    sysIntEnablePIC (intLevel);
    return (OK);
    }
