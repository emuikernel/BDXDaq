/* if_lnIsa.c - AMD Am79C961 PCnet-ISA+ Ethernet network interface driver */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,1sep96,joes	 written from 01a of if_lnPci.c.
*/

/*
This module implements the Advanced Micro Devices Am79C961 PCnet-ISA+
Ethernet  network interface driver.

This driver is designed to be moderately generic, operating unmodified
on any i386 or above PC architecture which supports the ISA bus.  To
achieve this, the driver must be given several target-specific parameters,
and some external support routines must be provided.  These parameters,
and the mechanisms used to communicate them to the driver, are detailed
below.  If any of the assumptions stated below are not true for your
particular hardware, this driver will probably not function correctly with
it.

Plug And Play
This device is compatible with the Microsoft Plug N' Play resource allocation
specification (http://www.microsoft.com, the P n' P spec is available for 
download from the microsoft site.). This device driver does not implement 
the Plug N' Play protocol, but expects the device to power up on the ISA bus
based on Data in the Plug N' Play EEPROM. Included are utilities for 
programming the EEPROM if it is detected as invalid or if you wish to change
the IEEE Ethernet address (Warning: This is not a bright thing to do if you 
do not know what you are doing. It is included as a development/manufacturing
tool only).

If the EEPROM is invalid, the device is enabled onto the ISA Bus using the 
device specific software relocatable key (A backdoor mechanism). This allows
the EEPROM to be accessible and programmed with valid data. To enable the
LANCE after programming the EEPROM, the pack must be rebooted.


Little Endian
It is assumed that the device is on a Intel X86 based platform, which supports
sysInWord, sysOutWord, etc and is Little Endian.

BOARD LAYOUT
This device is jumperless.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine;
there is no separate initialization routine.  Therefore, in the global 
interface structure, the function pointer to the initialization routine is 
NULL.

The only user-callable routine is lnIsaattach(), which publishes the `lnIsa'
interface and initializes the driver and device.

TARGET-SPECIFIC PARAMETERS
.iP "base address of device registers"
This parameter is passed to the driver by lnIsaattach().  It 
indicates to the driver where to find the device's register block. It is 
assumed that 
the Plug N' Play registers are already setup with data from the EEPROM
and the device is enabled on the Bus. If the EEPROM is invalid, it may be 
reprogrammed using lnIsa_eeprog() which takes the IEEE ethernet address and 
(optionally) the IP address of the pack as parameters. It loads the Plug N' 
Play EEPROM data area with values defined in config.h (See diagram in 
lnIsa_eeprom.c). The pack can then be rebooted and the device will be enabled
correctly. By defining IP_ADDR_STORED_IN_EEPROM 

The enabled LANCE presents two registers to the external interface, the RDP 
(register data port) and RAP (register address port) registers.  This driver 
assumes that these two registers occupy two unique addresses in IO space
that is directly accessible by the CPU executing this driver through the PC IO
Read/Write functions sysInByte(), sysOutByte(), sysOutWord(), sysInWord().
The register map is as follows:
   Offset     #Bytes      Register
   0x0		16	IEEE Address
   0x10		2	RDP
   0x12		2	RAP
   0x14		2	Reset
   0x16		2	IDP


.iP "interrupt vector"
This parameter is passed to the driver by lnIsaattach().

This driver configures the LANCE device to generate hardware interrupts
for various events within the device; thus it contains
an interrupt handler routine.  The driver calls intConnect() to connect 
its interrupt handler to the interrupt vector generated as a result of 
the LANCE interrupt.

.iP "interrupt level"
This parameter is passed to the driver by lnIsaattach().

Since this is an ISA device (i.e. PC based, an 8259 PIC is the expected 
interrupt controller. Therefore sysIntEnablePIC() is explicitly referenced
to enable the interrupts. This parameter is used to set up Edge or Level 
triggered interrupts in the ISA brige chip.

.iP "shared memory address"
This parameter is passed to the driver by lnIsaattach().

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
This parameter is passed to the driver by lnIsaattach().

This parameter can be used to explicitly limit the amount of shared
memory (bytes) this driver will use.  The constant NONE can be used to
indicate no specific size limitation.  This parameter is used only if
a specific memory region is provided to the driver.

.iP "shared memory width"
This parameter is passed to the driver by lnIsaattach().

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
This parameter is passed to the driver by lnIsaattach().

The driver and LANCE device exchange network data in buffers.  This
parameter permits the size of these individual buffers to be limited.
A value of zero indicates that the default buffer size should be used.
The default buffer size is large enough to hold a maximum-size Ethernet
packet.

Use of this parameter should be rare.  Network performance
will be affected, since the target will no longer be able to receive
all valid packet sizes.

.iP "Ethernet address"
This parameter is obtained directly from the EEPROM connected to the device.

During initialization, the driver needs to know the Ethernet address for
the LANCE device.  The driver assumes that this address is available in
a global, six-byte character array, lnEnetAddr[].  This array is
stuffed from EEPROM by lnIsaattach().

.iP "DMA Channel"
This parameter is defined in config.h by the MACRO DMA_CHAN_LNISA.

The DMA channel selected must be a 16 bit channel. lnIsaattach() places this 
channel in Cascade mode, which allows DMA requests to be passed though the
DMA controller to the CPU.
.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires one external support function:
.iP "void sysIntEnablePIC (int level)" "" 9 -1
This routine provides a PC specific enable of the interrupt for
the LANCE device.  

This routine is called once, from the lnIsaattach() routine.
.LP

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector
    - 24 bytes in the initialized data section (data)
    - 208 bytes in the uninitialized data section (BSS)

The above data and BSS requirements are for the I80386 architecture 
and may vary for other architectures.  Code size (text) varies greatly between
architectures and is therefore not quoted here.

If the driver is not given a specific region of memory via the lnIsaattach()
routine, then it calls cacheDmaMalloc() to allocate the memory to be shared 
with the LANCE.  The size requested is 80,542 bytes.  If a memory region
is provided to the driver, the size of this region is adjustable to suit
user needs.

The LANCE can only be operated if the shared memory region is write-coherent
with the data cache.  The driver cannot maintain cache coherency
for the device for data that is written by the driver because fields
within the shared structures are asynchronously modified by both the driver
and the device, and these fields may share the same cache line.

SEE ALSO: ifLib, 
.I "Advanced Micro Devices PCnet-ISA+ Ethernet Controller for ISA."
*/

#include "vxWorks.h"
#include "sysLib.h"
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


#include "net/route.h"

#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"
#include "netinet/if_ether.h"
#include "net/if_subr.h"
#include "semLib.h"
#include "bootLib.h"
#include "drv/netif/if_lnIsa.h"		/* device description header */


/***** LOCAL DEFINITIONS *****/

/* descriptor size */
#define RMD_SIZ  sizeof(ln_rmd)
#define TMD_SIZ  sizeof(ln_tmd)

/* Configuration items */

#define LN_MIN_FIRST_DATA_CHAIN 96      /* min size of 1st buf in data-chain */
#define LN_MAX_MDS              128     /* max number of [r|t]md's for LANCE */

#define LN_BUFSIZ      (ETHERMTU + ENET_HDR_REAL_SIZ + 6)

#define LN_RMD_RLEN     5       /* ring size as a power of 2 -- 32 RMD's */
#define LN_TMD_TLEN     5       /* same for transmit ring    -- 32 TMD's */

#define MAX_UNITS       1       /* maximum units supported */

/*
 * If LN_KICKSTART_TX is TRUE the transmitter is kick-started to force a
 * read of the transmit descriptors, otherwise the internal polling (1.6msec)
 * will initiate a read of the descriptors.  This should be FALSE is there
 * is any chance of memory latency or chip accesses detaining the LANCE DMA,
 * which results in a transmitter UFLO error.  This can be changed with the
 * global lnKickStartTx below.
 */

#define LN_KICKSTART_TX TRUE

/* Cache macros */

#define LN_CACHE_INVALIDATE(address, len) \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->cacheFuncs, (address), (len))

#define LN_CACHE_VIRT_TO_PHYS(address) \
        CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, (address))

#define LN_CACHE_PHYS_TO_VIRT(address) \
        CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->cacheFuncs, (address))

/* Typedefs for external structures that are not typedef'd in their .h files */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;                  /* Interface Data Record wrapper */
typedef struct ifnet IFNET;                 /* real Interface Data Record */
typedef struct sockaddr SOCK;

/* The Ethernet header */

typedef struct enet_hdr
    {
    char dst [6];
    char src [6];
    u_short type;
    } ENET_HDR;

#define ENET_HDR_SIZ        sizeof(ENET_HDR)
#define ENET_HDR_REAL_SIZ   14

/* The definition of the driver control structure */

typedef struct drv_ctrl
    {
    IDR         idr;                /* Interface Data Record */

    ln_ib       *ib;                 /* ptr to Initialization Block */

    struct
        {
        int     r_po2;                  /* RMD ring size as a power of 2! */
        int     r_size;                 /* RMD ring size (power of 2!) */
        int     r_index;                /* index into RMD ring */
        ln_rmd  *r_ring;                /* RMD ring */
        char    *r_bufs;                /* receive buffers base */
        } rmd_ring;

    struct
        {
        int     t_po2;                  /* TMD ring size as a power of 2! */
        int     t_size;                 /* TMD ring size (power of 2!) */
        int     t_index;                /* index into TMD ring */
        int     d_index;                /* index into TMD ring */
        ln_tmd  *t_ring;                /* TMD ring */
        char    *t_bufs;                /* transmit buffers base */
        } tmd_ring;

    BOOL        attached;               /* indicates unit is attached */
    SEM_ID      TxSem;                  /* transmitter semaphore */
    u_char      flags;                  /* misc control flags */
    int         ivec;                   /* interrupt vector */
    int         ilevel;                 /* interrupt level */
    LN_DEVICE   *devAdrs;               /* device structure address */
    char        *memBase;               /* LANCE memory pool base */
    int         memWidth;               /* width of data port */
    int         bufSize;                /* size of buffer in the LANCE ring */
    CACHE_FUNCS cacheFuncs;             /* cache function pointers */

    } DRV_CTRL;

#define DRV_CTRL_SIZ    sizeof(DRV_CTRL)

/* Definitions for the flags field */

#define LS_PROMISCUOUS_FLAG     0x1
#define LS_MEM_ALLOC_FLAG       0x2
#define LS_PAD_USED_FLAG        0x4
#define LS_RCV_HANDLING_FLAG    0x8
#define LS_START_OUTPUT_FLAG    0x10

/* Shorthand structure references */

#define rpo2         rmd_ring.r_po2
#define rsize        rmd_ring.r_size
#define rindex       rmd_ring.r_index
#define rring        rmd_ring.r_ring

#define tpo2         tmd_ring.t_po2
#define tsize        tmd_ring.t_size
#define tindex       tmd_ring.t_index
#define dindex       tmd_ring.d_index
#define tring        tmd_ring.t_ring


/***** GLOBALS *****/

IMPORT STATUS sysIntDisablePIC  (int intLevel);
IMPORT STATUS sysIntEnablePIC(int intLevel);
IMPORT USHORT sysInWord (int address);
IMPORT void sysOutWord (int address, USHORT data);
IMPORT void sysOutByte (int address, UINT8 data);
void panic ();
void ether_attach ();
BOOL arpresolve ();


/***** LOCALS *****/

static int lnTsize = LN_TMD_TLEN;    /* deflt xmit ring size as power of 2 */
static int lnRsize = LN_RMD_RLEN;    /* deflt recv ring size as power of 2 */
static BOOL lnKickStartTx = LN_KICKSTART_TX;

#define LNISA_DEBUG

/* The array of driver control structures */

static DRV_CTRL  drvCtrl [MAX_UNITS];

/* initial word offsets from LANCE base address to access these registers */

static u_int CSROffset = 	0x10;
static u_int RAPOffset = 	0x12;
static u_int IDPOffset = 	0x16;

/* forward static functions */

static void 	lnReset (int unit);
static void 	lnInt (DRV_CTRL *pDrvCtrl);
static void 	lnHandleRecvInt (DRV_CTRL *pDrvCtrl);
static STATUS 	lnRecv (DRV_CTRL *pDrvCtrl, ln_rmd *rmd);
static int 	lnOutput (IDR *ifp, MBUF *m0, SOCK *dst);
static int 	lnIoctl (IDR *ifp, int cmd, caddr_t data);
static int 	lnChipReset (DRV_CTRL *pDrvCtrl);
static ln_rmd *	lnGetFullRMD (DRV_CTRL *pDrvCtrl);
void 	lnCsrWrite (DRV_CTRL * pDrvCtrl, int reg, USHORT value);
USHORT 	lnCsrRead (DRV_CTRL * pDrvCtrl, int reg);
void 	lnIdpWrite (DRV_CTRL * pDrvCtrl, int reg, USHORT value);
USHORT 	lnIdpRead (DRV_CTRL * pDrvCtrl, int reg);
static void 	lnRestart (int unit,int loc,int stat);
static BOOL 	convertDestAddr
                (
		IDR *pIDR,
		SOCK *pDestSktAddr,
		char *pDestEnetAddr,
		u_short *pPacketType,
		MBUF *pMbuf
		);
UINT8 lnEnetAddr [6];
#include "lnIsa_eeprom.c" /* EEPROM and PnP control functions for AMD961 */



/*******************************************************************************
*
* lnIsaattach - publish the `lnIsa' network interface and initialize the driver and device
*
* This routine publishes the `lnIsa' interface by filling in a network 
* interface record and adding this record to the system list.  This routine 
* also initializes the driver and the device to the operational state.
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
* BUGS
* To zero out LANCE data structures, this routine uses bzero(), which
* ignores the <memWidth> specification and uses any size data access to
* write to memory.
*
* RETURNS: OK or ERROR.
*/

STATUS lnIsaattach
(
    int		unit,		/* unit number */
    char *	devAdrs,	/* LANCE I/O address */
    int		ivec,		/* interrupt vector */
    int		ilevel,		/* interrupt level */
    char *	memAdrs,	/* address of memory pool (-1 = malloc it) */
    ULONG	memSize,	/* used if memory pool is NOT malloc()'d */
    int		memWidth,	/* byte-width of data (-1 = any width)     */
    int		mode,		/* See CONFIG_LNISA in config.h */
    int		dmaChan		/* DMA channel to use */
    )
    {
    DRV_CTRL     *pDrvCtrl;
    unsigned int sz;           /* temporary size holder */
    char         *pTurkey;     /* start of the LANCE memory pool */
    int i;
#ifdef IP_ADDR_STORED_IN_EEPROM
    char ipStr[BOOT_ADDR_LEN];
    char *subNetMaskStr;
    char scratchStr[BOOT_ADDR_LEN];
    UINT32 ipAddr = 0;
    BOOT_PARAMS bootParams;
#endif /* IP_ADDR_STORED_IN_EEPROM */

    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    /*
     * The DMA channel must be one of 5,6 or 7 as it has 
     * to be a 16 bit channel.
     */
    if((dmaChan < 5) || (dmaChan > 7))
      return ERROR;

    /* Ensure single invocation per system life */

    pDrvCtrl = & drvCtrl [unit];

    if (pDrvCtrl->attached)
        return (OK);
    /* 
     * Fill pDrvCtrl->devAdrs out now as it is used by lnCsrXXXX and 
     * lnIdpXXXX functions for register access.
     */
    pDrvCtrl->devAdrs    = (LN_DEVICE *)devAdrs; /* LANCE I/O address */



    /* set DMA channel to cascade mode to enable device as ISA bus master */
    sysOutByte (0xd6,(0xc0 | (dmaChan - 4))); 
    sysOutByte (0xd4,(dmaChan - 4)); /* enable DMA channel */

    /* 
     * Examine ISACSR3 for a valid EEPROM. If the EEPROM is invalid bring the
     * device out of PnP mode using the Software Relocatable key code. This 
     * will allow the user to program the EEPROM with valid information using 
     * the function lnIsa_eeprog().
     */
     if(lnIdpRead(pDrvCtrl,ISACSR3) != EE_VALID)
     {
       printf("\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
       printf("Ethernet Device EEPROM invalid\n");
       printf("Use lnIsa_eeprog(pDrvCtrl,ENET_HI,ENET_MID,ENET_LO");
#ifdef IP_ADDR_STORED_IN_EEPROM
       printf(",\"IP ADDRESS (Dot Notation)\")\n");
#else
       printf("\n");
#endif
       printf("to initialize EEPROM: pDrvCtrl = %p\n",pDrvCtrl);
       printf("\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
       lnIsaPnPInitiation(); /* send AMD961 SW relocation key */
       lnIsaPnPSetup(devAdrs,(UINT8)ilevel,(UINT8)INT_TYPE_LNISA,
		     (UINT8)dmaChan);
       for(i = 0; i < 6; i++)
	 lnEnetAddr[i] = 0; /* invalidate enet addr */
       return ERROR;
     }
     else
     {
       /* EEPROM is Valid, so read out the Ethernet Address */
       lnCsrWrite(pDrvCtrl,0,4); /* make sure device is stopped */
       WR_EE_REG(EE_EN); /* enable EEPROM access */
       EE_TIMEOUT;
       *(UINT16 *)(&lnEnetAddr[4]) = ntohs(lnIsa_eeread16(pDrvCtrl,2));
       *(UINT16 *)(&lnEnetAddr[2]) = ntohs(lnIsa_eeread16(pDrvCtrl,1));
       *(UINT16 *)(&lnEnetAddr[0]) = ntohs(lnIsa_eeread16(pDrvCtrl,0));

#ifdef IP_ADDR_STORED_IN_EEPROM
       ipAddr = lnIsa_eeread16(pDrvCtrl,EE_WORD_LEN_MIN+1);
       ipAddr = ipAddr << 16;
       ipAddr |= lnIsa_eeread16(pDrvCtrl,EE_WORD_LEN_MIN);
       sprintf(ipStr,"%d.%d.%d.%d",(ipAddr&0x000000ff),
	       ((ipAddr&0x0000ff00)>>8),((ipAddr&0x00ff0000)>> 16),
	       ((ipAddr&0xff000000)>> 24));
       if((ipAddr == 0x0) || (ipAddr == 0xffffffff))
       {
	 printf("\n\nInvalid IP Address: %s\n\n",ipStr);
	 return ERROR;
       }
       if((ipAddr != 0) && (ipAddr != 0xffffffff))
       {
	 bootStringToStruct(BOOT_LINE_ADRS, &bootParams);
	 /* get current subnet mask*/
	 subNetMaskStr = strstr(&bootParams.ead[0],":"); 
	 if(subNetMaskStr != NULL)
	   sprintf(scratchStr,"%s%s",ipStr,subNetMaskStr);
	 else
	   sprintf(scratchStr,"%s",ipStr);
	 sprintf(&bootParams.ead[0],"%s",scratchStr);
	 bootStructToString(BOOT_LINE_ADRS, &bootParams); /* update bootline */
       }
#endif /* IP_ADDR_STORED_IN_EEPROM */

       WR_EE_REG(0);	/* Disable EEPROM access */

     }
     
#ifdef LNISA_DEBUG
    printf("\ndevAdrs = %p, ivec = %d, ilevel = %d\n",devAdrs,ivec,ilevel);
    printf("memAdrs = %p, memSize = %x, memWidth = %d\n",memAdrs, memSize,
	   memWidth);
    printf("Ethernet Address: %02x:%02x:%02x:%02x:%02x:%02x\n",lnEnetAddr[0],
	   lnEnetAddr[1],lnEnetAddr[2],lnEnetAddr[3],lnEnetAddr[4],
	   lnEnetAddr[5]);
    printf("IP Address = %08x = %s\n",ipAddr,ipStr);
#endif

    /* Publish the interface data record */

    ether_attach    (
                    & pDrvCtrl->idr.ac_if,
                    unit,
                    "lnIsa",
                    (FUNCPTR) NULL,
                    (FUNCPTR) lnIoctl,
                    (FUNCPTR) lnOutput,
                    (FUNCPTR) lnReset
                    );

    /* Create the transmit semaphore. */

    pDrvCtrl->TxSem = semMCreate    (
                                    SEM_Q_PRIORITY |
                                    SEM_DELETE_SAFE |
                                    SEM_INVERSION_SAFE
                                    );

    if (pDrvCtrl->TxSem == NULL)
        {
        printf ("lnIsa: error creating transmitter semaphore\n");
        return (ERROR);
        }


    { /***** Establish size of shared memory region we require *****/

    if ((int) memAdrs != NONE)  /* specified memory pool */
        {
        /*
         * With a specified memory pool we want to maximize
         * lnRsize and lnTsize
         */

        sz = (memSize - (RMD_SIZ + TMD_SIZ + sizeof (ln_ib)))
               / ((2 * LN_BUFSIZ) + RMD_SIZ + TMD_SIZ);

        sz >>= 1;               /* adjust for roundoff */

        for (lnRsize = 0; sz != 0; lnRsize++, sz >>= 1)
            ;

        lnTsize = lnRsize;      /* lnTsize = lnRsize for convenience */
        }

    /* limit ring sizes to reasonable values */

    if (lnRsize < 2)
        lnRsize = 2;            /* 4 buffers is reasonable min */

    if (lnRsize > 7)
        lnRsize = 7;            /* 128 buffers is max for chip */

    /* limit ring sizes to reasonable values */

    if (lnTsize < 2)
        lnTsize = 2;            /* 4 buffers is reasonable min */

    if (lnTsize > 7)
        lnTsize = 7;            /* 128 buffers is max for chip */


    /* Add it all up */

    sz = (sizeof (ln_ib)) +
         ( ((1 << lnRsize) + 1) * RMD_SIZ ) +
         (LN_BUFSIZ << lnRsize) +
         ( ((1 << lnTsize) + 1) * TMD_SIZ ) +
         (LN_BUFSIZ << lnTsize) +
         6;                        /* allow for alignment adjustment */
    }


    { /***** Establish a region of shared memory *****/

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
                printf ( "lnIsa: not enough memory provided\n" );
                return ( ERROR );
                }
            pTurkey = memAdrs;             /* set the beginning of pool */

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
                printf ( "lnIsa: device requires cache coherent memory\n" );
                return (ERROR);
                }

            pTurkey = (char *) cacheDmaMalloc ( sz );

            if ((int)pTurkey == NULL)
                {
                printf ( "lnIsa: system memory unavailable\n" );
                return (ERROR);
                }

            /* copy the DMA structure */

            pDrvCtrl->cacheFuncs = cacheDmaFuncs;

            break;
        }
    }

    /*                        Turkey Carving
     *                        --------------
     *
     *                          LOW MEMORY
     *
     *             |-------------------------------------|
     *             |       The initialization block      |
     *             |         (sizeof (ln_ib))            |
     *             |-------------------------------------|
     *             |         The Rx descriptors          |
     *             | (1 << lnRsize) * sizeof (ln_rmd)|
     *             |-------------------------------------|
     *             |          The receive buffers        |
     *             |       (LN_BUFSIZ << lnRsize)        |
     *             |-------------------------------------|
     *             |         The Tx descriptors          |
     *             | (1 << lnTsize) * sizeof (ln_tmd)|
     *             |-------------------------------------|
     *             |           The transmit buffers      |
     *             |       (LN_BUFSIZ << lnTsize)        |
     *             |-------------------------------------|
     */

    /* Save some things */

    pDrvCtrl->memBase  = (char *)((ULONG)pTurkey & 0xffff0000);
    pDrvCtrl->memWidth = memWidth;
    pDrvCtrl->bufSize = LN_BUFSIZ;

    if ((int) memAdrs == NONE)
        pDrvCtrl->flags |= LS_MEM_ALLOC_FLAG;

    { /***** Carve up the turkey *****/

    /* First let's clean the whole turkey */

    bzero ( (char *) pTurkey, (int) sz );

    /* carve out initialization block */

    pDrvCtrl->ib = (ln_ib *)pTurkey;
    sz = sizeof (ln_ib);                /* size of initialization block */

    /* carve out receive message descriptor (RMD) ring structure */

    pDrvCtrl->rpo2 = lnRsize;              /* remember for lnConfig */
    pDrvCtrl->rsize = (1 << lnRsize);      /* receive ring size */

    /* make it 16 byte aligned */

    pDrvCtrl->rring = (ln_rmd *) (((int)pDrvCtrl->ib + sz + 0x0f) & ~0x0f);

    sz = (1 << lnRsize) * RMD_SIZ;

    pDrvCtrl->rmd_ring.r_bufs = (char *)((int)pDrvCtrl->rring + sz);

    sz = (LN_BUFSIZ << lnRsize);   /* room for all the receive buffers */

    /* carve out transmit message descriptor (TMD) ring structure */

    pDrvCtrl->tpo2 = lnTsize;              /* remember for lnConfig */
    pDrvCtrl->tsize = (1 << lnTsize);      /* transmit ring size */

    pDrvCtrl->tring = (ln_tmd *) (((int)pDrvCtrl->rmd_ring.r_bufs + sz + 0x0f)
				  & ~0x0f);

    sz = (1 << lnTsize) * TMD_SIZ;

    /* carve out transmit buffer space */

    pDrvCtrl->tmd_ring.t_bufs = (char *)((int)pDrvCtrl->tring + sz);
    }


    /* Save some values */

    pDrvCtrl->ivec       = ivec;                 /* interrupt vector */
    pDrvCtrl->ilevel     = ilevel;               /* interrupt level */
    pDrvCtrl->devAdrs    = (LN_DEVICE *)devAdrs; /* LANCE I/O address */

    /* Obtain our Ethernet address and save it */

    bcopy ((char *) lnEnetAddr, (char *)pDrvCtrl->idr.ac_enaddr, 6);

    { /***** Device Initialization *****/

    if (lnChipReset (pDrvCtrl) == ERROR)	/* reset lance device */
    	{
	logMsg ("lnIsa: cannot Reset device:\n",0,0,0,0,0,0);
	return (ERROR);
	}

    if (intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC(ivec),lnInt,(int)pDrvCtrl)
        == ERROR)
        return (ERROR);

    { /* Initialize all shared memory structures */

    ln_rmd  *rmd;
    ln_tmd  *tmd;
    char    *buf;
    ln_ib   *ib;
    ULONG    pTemp;
    int     loopy;

    /* Set up the Rx descriptors */

    rmd = pDrvCtrl->rring;                      /* receive ring */
    buf = pDrvCtrl->rmd_ring.r_bufs;
    for (loopy = 0; loopy < pDrvCtrl->rsize; loopy++)
    {
      pTemp = (ULONG)LN_CACHE_VIRT_TO_PHYS(buf);/* convert to physical addr */
      rmd->rbuf_adr = (USHORT)(pTemp & 0x0000ffff); /* bits 15:00 buf addr */
      rmd->rbuf_rmd1 = (USHORT)((pTemp & 0x00ff0000) >> 16); /* bits 7:0 */
      /* neg of buffer byte count */
      rmd->rbuf_bcnt = ((RMD2_BCNT_MSK & -(pDrvCtrl->bufSize)) | RMD2_MBO);
      rmd->rbuf_mcnt = 0;			/* no message byte count yet */
      rmd->rbuf_rmd1 |= RMD1_OWN;	/* buffer now owned by LANCE */
      rmd++;					/* step to next descriptor */
      buf += (pDrvCtrl->bufSize);		/* step to the next buffer */
    }

    pDrvCtrl->rindex = 0;


    /* Setup the Tx descriptors */

    tmd = pDrvCtrl->tring;                 /* transmit ring */
    buf = pDrvCtrl->tmd_ring.t_bufs;

    for (loopy = 0; loopy < pDrvCtrl->tsize; loopy++)
    {
      pTemp = (ULONG)LN_CACHE_VIRT_TO_PHYS(buf); /* convert to physical addr */
      tmd->tbuf_adr = (USHORT) (pTemp & 0x0000ffff); /* bits 15:00 of addr */
      tmd->tbuf_tmd1 = (USHORT)((pTemp & 0x00ff0000) >> 16); /* bits 7:0 */
      tmd->tbuf_tmd2 = 0;		/* no length of Tx Buf yet */
      tmd->tbuf_tmd3 = 0;		/* clear status bits */
      tmd->tbuf_tmd1 |= TMD1_ENP;	/* buffer is end of packet */
      tmd->tbuf_tmd1 |= TMD1_STP;	/* buffer is start of packet */
      tmd++;					/* step to next descriptor */
      buf += (pDrvCtrl->bufSize);		/* step to next buffer */
    }

    pDrvCtrl->tindex = pDrvCtrl->dindex = 0;

    /* Setup the initialization block */
    lnIdpWrite(pDrvCtrl,2, (0x100 | (mode & 0x3))); /* set up mode */


    ib = pDrvCtrl->ib;

    ib->lnIBMode = 0;        /* chip will be in normal receive mode */

    bcopy ((char *)&pDrvCtrl->idr.ac_enaddr[0],&ib->lnIBPadr[0], 6);

    pTemp =(ULONG)LN_CACHE_VIRT_TO_PHYS(pDrvCtrl->rring);/* point to Rx ring */
    ib->lnIBRdraLow = (USHORT)(pTemp & 0x0000ffff);
    ib->lnIBRdraHigh = (UCHAR)((pTemp & 0x00ff0000) >> 16);
    ib->lnRLEN =  pDrvCtrl->rpo2<<5;

    pTemp =(ULONG)LN_CACHE_VIRT_TO_PHYS(pDrvCtrl->tring);/* point to Tx ring */
    ib->lnIBTdraLow = (USHORT)(pTemp & 0x0000ffff);
    ib->lnIBTdraHigh = (UCHAR)((pTemp & 0x00ff0000) >> 16);
    ib->lnTLEN = pDrvCtrl->tpo2<<5;

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();
    }


    { /* Start the device */

    USHORT	stat = 0;
    ULONG	pTemp;
    int		timeoutCount = 0;

    lnCsrWrite (pDrvCtrl, 0, CSR0_STOP);	/* set the stop bit */

    /* Point the device to the initialization block */

    pTemp = (ULONG)LN_CACHE_VIRT_TO_PHYS ( pDrvCtrl->ib );
    lnCsrWrite (pDrvCtrl, 2, (USHORT)((pTemp >> 16) & 0x000000ff));
    lnCsrWrite (pDrvCtrl, 1, (USHORT)(pTemp & 0x0000ffff));
    lnCsrWrite (pDrvCtrl, 0, CSR0_INIT);            /* init chip (read IB) */

    while (((stat = lnCsrRead (pDrvCtrl, 0)) & (CSR0_IDON | CSR0_ERR)) == 0)
        {
        if (timeoutCount++ > 5) 
	  break;
        taskDelay(sysClkRateGet());
        }

    if ((stat & CSR0_ERR) == CSR0_ERR)
	{
	if ((stat & CSR0_MERR)  == CSR0_MERR)
	    {
	    printf ("Lance Memory error during initialization:\t0x%x\n",
		    stat);
	    }
	else
	    {
	    printf ("Lance Error during initialization:\t0x%x\n",
		    stat);
	    }
	}

    /* log chip initialization failure */

    if ( (stat & CSR0_ERR) || (timeoutCount >= 0x10000) )
        {
        printf ("lnIsa%d: Device initialization failed\n",
                    pDrvCtrl->idr.ac_if.if_unit);
        return (ERROR);
        }

    /* Device is initialized.  Start transmitter and receiver.  The device
     * RAP register is left selecting CSR 0.
     */

    lnCsrWrite(pDrvCtrl, 0, CSR0_IDON | CSR0_IENA | CSR0_STRT);
    }

    sysIntEnablePIC (ilevel);                   /* enable LANCE interrupts */
    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);

    } /* Block end; device initialization */

    /* Set our flag */
    pDrvCtrl->attached = TRUE;

    return (OK);
}

/*******************************************************************************
*
* lnReset - reset the interface
*
* Mark interface as inactive & reset the chip
*/

static void lnReset
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl = & drvCtrl [unit];

    pDrvCtrl->idr.ac_if.if_flags = 0;
    lnChipReset (pDrvCtrl);                           /* reset LANCE */
    }

/*******************************************************************************
*
* lnInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*/

static void lnInt
    (
    DRV_CTRL  *pDrvCtrl
    )
    {
    ln_tmd         *tmd;
    int            *pDindex;
    int            *pTindex;
    int            *pTsize;
    ln_tmd         *pTring;
    USHORT	   stat;

    /* Read the device status register */
    
    stat = lnCsrRead (pDrvCtrl, 0);

    /* If false interrupt, return. */

    if ( ! (stat & CSR0_INTR) )
        return;

    /*
     * enable interrupts, clear receive and/or transmit interrupts, and clear
     * any errors that may be set.
     */

    lnCsrWrite (pDrvCtrl, 0, ((stat &
            (CSR0_BABL|CSR0_CERR|CSR0_MISS|CSR0_MERR|
             CSR0_RINT|CSR0_TINT|CSR0_IDON)) | CSR0_IENA));

    /* Check for errors */

    if (stat & (CSR0_BABL | CSR0_MISS | CSR0_MERR))
        {
        ++pDrvCtrl->idr.ac_if.if_ierrors;

        /* restart chip on fatal error */

        if (stat & CSR0_MERR)        /* memory error */
            {
            pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);
            (void) netJobAdd (
                             (FUNCPTR)lnRestart,
                             pDrvCtrl->idr.ac_if.if_unit,
                             1,(int)stat,0,0
                             );
            }
        }

    /* Have netTask handle any input packets */

    if ((stat & CSR0_RINT) && (stat & CSR0_RXON))
        {
        if ( ! (pDrvCtrl->flags & LS_RCV_HANDLING_FLAG) )
            {
            pDrvCtrl->flags |= LS_RCV_HANDLING_FLAG;
            (void) netJobAdd (
                             (FUNCPTR)lnHandleRecvInt,
                             (int)pDrvCtrl,
                             0,0,0,0
                             );
            }
        }

    /*
     * Did LANCE update any of the TMD's?
     * If not then don't bother continuing with transmitter stuff
     */

    if (!(stat & CSR0_TINT))
        return;

    pDindex = &pDrvCtrl->dindex;
    pTindex = &pDrvCtrl->tindex;
    pTsize  = &pDrvCtrl->tsize;
    pTring  = pDrvCtrl->tring;

    while (*pDindex != *pTindex)
        {
        /* disposal has not caught up */

        tmd = pTring + *pDindex;

        /* if the buffer is still owned by LANCE, don't touch it */

        LN_CACHE_INVALIDATE (tmd, TMD_SIZ);

        if (tmd->tbuf_tmd1 & TMD1_OWN)
            break;

        /*
         * tbuf_err (TMD1.ERR) is an "OR" of LCOL, LCAR, UFLO or RTRY.
         * Note that BUFF is not indicated in TMD1.ERR.
         * We should therefore check both tbuf_err and tbuf_buff
         * here for error conditions.
         */

        if ((tmd->tbuf_tmd1 & TMD1_ERR) || 
	    (tmd->tbuf_tmd3 & TMD3_BUFF))
            {

            pDrvCtrl->idr.ac_if.if_oerrors++;     /* output error */
            pDrvCtrl->idr.ac_if.if_opackets--;

            /* If error was due to excess collisions, bump the collision
             * counter.  The LANCE does not keep an individual counter of
             * collisions, so in this driver, the collision statistic is not
             * an accurate count of total collisions.
             */

            if (tmd->tbuf_tmd3 & TMD3_RTRY)
                pDrvCtrl->idr.ac_if.if_collisions++;

            /* check for no carrier */

            if (tmd->tbuf_tmd3 & TMD3_LCAR)
                logMsg ("ln%d: no carrier\n",
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

            if ((tmd->tbuf_tmd3 & TMD3_BUFF) || 
		(tmd->tbuf_tmd3 & TMD3_UFLO))
                {
                pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);
                (void) netJobAdd (
                                 (FUNCPTR)lnRestart,
                                 pDrvCtrl->idr.ac_if.if_unit,
                                 2,(int)(tmd->tbuf_tmd3),0,0
                                 );
                return;
                }
            }

        tmd->tbuf_tmd1 &= 0x00ff;   /* clear status/err bits, keep hadr bits */
        tmd->tbuf_tmd3 = 0;		/* clear all error & stat stuff */

        /* now bump the tmd disposal index pointer around the ring */

        *pDindex = (*pDindex + 1) & (*pTsize - 1);
        }

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    }

/*******************************************************************************
*
* lnHandleRecvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/

static void lnHandleRecvInt
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    ln_rmd *rmd;

    do
        {
        pDrvCtrl->flags |= LS_RCV_HANDLING_FLAG;

        while ((rmd = lnGetFullRMD (pDrvCtrl)) != NULL)
            lnRecv (pDrvCtrl, rmd);

        /*
         * There is a RACE right here.  The ISR could add a receive packet
         * and check the boolean below, and decide to exit.  Thus the
         * packet could be dropped if we don't double check before we
         * return.
         */

        pDrvCtrl->flags &= ~LS_RCV_HANDLING_FLAG;
        }
    while (lnGetFullRMD (pDrvCtrl) != NULL);
    /* this double check solves the RACE */
    }

/*******************************************************************************
*
* lnGetFullRMD - get next received message RMD
*
* Returns ptr to next Rx desc to process, or NULL if none ready.
*/

static ln_rmd *lnGetFullRMD
    (
    DRV_CTRL  *pDrvCtrl
    )
    {
    ln_rmd   *rmd;

    /* Refuse to do anything if flags are down */

    if  (
        (pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING) ) !=
        (IFF_UP | IFF_RUNNING)
        )
        return ((ln_rmd *) NULL);

    rmd = pDrvCtrl->rring + pDrvCtrl->rindex;       /* form ptr to Rx desc */

    LN_CACHE_INVALIDATE (rmd, RMD_SIZ);

    if ((rmd->rbuf_rmd1 & RMD1_OWN) == 0)
        return (rmd);
    else
        return ((ln_rmd *) NULL);
    }

/*******************************************************************************
*
* lnRecv - process the next incoming packet
*
*/

static STATUS lnRecv
    (
    DRV_CTRL *pDrvCtrl,
    ln_rmd   *rmd
    )
    {
    ENET_HDR    *pEnetHdr;
    MBUF        *pMbuf;
    u_char      *pData;
    int         len;
    u_short     ether_type;
    BOOL        hookAte;


    /* Packet must be checked for errors. */

    if  (
        /* If error flag */
        (rmd->rbuf_rmd1 & (RMD1_ERR | RMD1_FRAM)) ||

        /* OR if packet is not completely in one buffer */
        (
        (rmd->rbuf_rmd1 & (RMD1_STP | RMD1_ENP) ) !=
        (RMD1_STP | RMD1_ENP)
        )

        )
        {
        ++pDrvCtrl->idr.ac_if.if_ierrors;       /* bump error stat */
        goto cleanRXD;                          /* skip to clean up */
        }

    ++pDrvCtrl->idr.ac_if.if_ipackets;			/* bump statistic */
    len = rmd->rbuf_mcnt & RMD3_MCNT_MSK;	/* get packet length */

    /* Get pointer to packet */

    pEnetHdr = (ENET_HDR *)
                (pDrvCtrl->rmd_ring.r_bufs + (pDrvCtrl->rindex * LN_BUFSIZ));
    LN_CACHE_INVALIDATE (pEnetHdr, len);   /* make the packet data coherent */

    /* call input hook if any */

    hookAte = FALSE;
    if  (etherInputHookRtn != NULL)
        {
        if  (
            (* etherInputHookRtn) (&pDrvCtrl->idr.ac_if, (char *)pEnetHdr, len)
            )
            hookAte = TRUE;
        }

    /* Normal path; send packet upstairs */

    if (hookAte == FALSE)
        {
        /* Adjust length to size of data only */

        len -= ENET_HDR_REAL_SIZ;

        /* Get pointer to packet data */

        pData = ((u_char *) pEnetHdr) + ENET_HDR_REAL_SIZ;

        ether_type = ntohs ( pEnetHdr->type );

        /* Copy packet data into MBUFs, using the specified width */

        pMbuf = bcopy_to_mbufs (pData, len, 0, (IFNET *) & pDrvCtrl->idr.ac_if,
                            pDrvCtrl->memWidth);

        if (pMbuf != NULL)
            do_protocol_with_type (ether_type, pMbuf, & pDrvCtrl->idr, len);
	  
        }

    /* Done with descriptor, clean up and give it to the device. */

    cleanRXD:
    /* clear status bits */
    rmd->rbuf_rmd1 &= 0x00ff;	/* clear status/err bits, keep address */
    rmd->rbuf_mcnt = 0;		/* reset count */
    rmd->rbuf_rmd1 |= RMD1_OWN;	/* give device ownership */

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Advance our management index */

    pDrvCtrl->rindex = (pDrvCtrl->rindex + 1) & (pDrvCtrl->rsize - 1);

    return (OK);
    }

/*******************************************************************************
*
* lnOutput - the driver output routine
*
*/

static int lnOutput
    (
    IDR  *pIDR,
    MBUF *pMbuf,
    SOCK *pDest
    )
    {
    char destEnetAddr [6];                  /* space for enet addr */
    u_short packetType;                     /* type field for the packet */
    DRV_CTRL    *pDrvCtrl;
    ln_tmd      *tmd;
    char        *buf;
    int         len;
    int         oldLevel;
    BOOL        hookAte;

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

    if (convertDestAddr (pIDR,pDest, destEnetAddr, &packetType, pMbuf) == 
	FALSE)
      return (OK);    /* I KNOW returning OK is stupid, but it is correct */

    /* Get driver control pointer */

    pDrvCtrl = & drvCtrl [pIDR->ac_if.if_unit];

    /* Obtain exclusive access to transmitter.  This is necessary because
     * certain events can cause netTask to run a job that attempts to transmit
     * a packet.  We can only allow one task here at a time.
     */

    semTake (pDrvCtrl->TxSem, WAIT_FOREVER);

    pDrvCtrl->flags |= LS_START_OUTPUT_FLAG;

    /* See if next TXD is available */

    tmd = pDrvCtrl->tring + pDrvCtrl->tindex;

    LN_CACHE_INVALIDATE (tmd, TMD_SIZ);

    if  (((tmd->tbuf_tmd1 & TMD1_OWN) != 0) ||
        (((pDrvCtrl->tindex + 1) & (pDrvCtrl->tsize - 1)) == pDrvCtrl->dindex))
        {
        m_freem (pMbuf);            /* Discard data */
        goto outputDone;
        }

    /* Get pointer to transmit buffer */

    buf = (char *)
        (pDrvCtrl->tmd_ring.t_bufs + (pDrvCtrl->tindex * LN_BUFSIZ));

    /* Fill in the Ethernet header */

    bcopy (destEnetAddr, buf, 6);
    bcopy ( (char *) pIDR->ac_enaddr, (buf + 6), 6);
    /* added htons translation joes 12/9/96 */
    ((ENET_HDR *)buf)->type = htons(packetType);

    /* Copy packet from MBUFs to our transmit buffer.  MBUFs are
     * transparently freed.
     */

    bcopy_from_mbufs    (
                        (buf + ENET_HDR_REAL_SIZ),
                        pMbuf,
                        len,                            /* stuffed by macro */
                        pDrvCtrl->memWidth
                        );

    /* Ensure we send a legal size frame. */

    len += ENET_HDR_REAL_SIZ;
    len = max (ETHERSMALL, len);

    /* call output hook if any */

    hookAte = FALSE;
    if  (etherOutputHookRtn != NULL)
      {
        if  (
            (* etherOutputHookRtn)(&pDrvCtrl->idr.ac_if, buf, len)
            )
            hookAte = TRUE;
      }

    /* place a transmit request */
    if (hookAte == FALSE)
      {

	oldLevel = intLock ();          /* disable ints during update */

	tmd->tbuf_tmd3 = 0;                     /* clear buffer error status */

	/* negative message byte count */
	tmd->tbuf_tmd2 = (TMD2_BCNT_MSK & -len);
	tmd->tbuf_tmd2 |= TMD2_MBO;


	tmd->tbuf_tmd1 |= (TMD1_ENP|TMD1_STP); /* start and end of packet */
	/*tmd->tbuf_tmd1 |= TMD1_STP;  buffer is start of packet */
	tmd->tbuf_tmd1 &= ~(TMD1_DEF|TMD1_MORE|TMD1_ERR);/* clear status bit */
	/*	tmd->tbuf_tmd1 &= ~TMD1_MORE;
	tmd->tbuf_tmd1 &= ~TMD1_ERR;*/
	tmd->tbuf_tmd1 |= TMD1_OWN;

	/* Flush the write pipe */

	CACHE_PIPE_FLUSH ();

	intUnlock (oldLevel);   /* now lnInt won't get confused */

	/* Advance our management index */

	pDrvCtrl->tindex = (pDrvCtrl->tindex + 1) & (pDrvCtrl->tsize - 1);

	/* kick start the transmitter, if selected */

	if (lnKickStartTx)
	  lnCsrWrite (pDrvCtrl, 0, (CSR0_IENA | CSR0_TDMD));

	pDrvCtrl->flags &= ~LS_START_OUTPUT_FLAG;

	/* Bump the statistic counter. */

	pIDR->ac_if.if_opackets++;
      }
    outputDone:

    /* Release exclusive access. */

    semGive (pDrvCtrl->TxSem);

    return (OK);

    }

/*******************************************************************************
*
* lnIoctl - the driver I/O control routine
*
* Process an ioctl request.
*/

static int lnIoctl
    (
    IDR  *ifp,
    int            cmd,
    caddr_t        data
    )
    {
    int error = 0;

    switch (cmd)
        {
        case SIOCSIFADDR:
            ifp->ac_ipaddr = IA_SIN (data)->sin_addr;
            break;

        case SIOCSIFFLAGS:
            /* No further work to be done */
            break;

        default:
            error = EINVAL;
        }

    return (error);
    }

/*******************************************************************************
*
* lnChipReset - hardware reset of chip (stop it)
*/

static int lnChipReset
    (
    DRV_CTRL  *pDrvCtrl
    )
    {

    lnCsrWrite (pDrvCtrl, CSR(0), CSR0_STOP);

    return (OK);
    }

/*******************************************************************************
*
* lnCsrWrite - select and write a CSR register
*
*/

void lnCsrWrite
    (
    DRV_CTRL * 	pDrvCtrl,		/* driver control */
    int 	reg,			/* register to select */
    USHORT 	value			/* value to write */
    )
    {
    int dv = (int)pDrvCtrl->devAdrs;

    /* select CSR */
    sysOutWord(dv+RAPOffset,reg);;

    /* write val to CSR */
    sysOutWord(dv+CSROffset,value);;

    }

/*******************************************************************************
*
* lnCsrRead - select and read a CSR register
*
*/

USHORT lnCsrRead
    (
    DRV_CTRL *	pDrvCtrl,		/* driver control */
    int		reg			/* register to select */
    )
    {
    int dv = (int)pDrvCtrl->devAdrs;

    /* select CSR */
    sysOutWord(dv+RAPOffset,reg);;

    /* get contents of CSR */
    return (sysInWord(dv+CSROffset));

    }

/*******************************************************************************
*
* lnIdpWrite - select and write a IDP register
*
*/

void lnIdpWrite
    (
    DRV_CTRL * 	pDrvCtrl,		/* driver control */
    int 	reg,			/* register to select */
    USHORT 	value			/* value to write */
    )
    {
    int dv = (int)pDrvCtrl->devAdrs;

    /* select IDP */
    sysOutWord(dv+RAPOffset,reg);;

    /* write val to IDP */
    sysOutWord(dv+IDPOffset,value);;

    }

/*******************************************************************************
*
* lnIdpRead - select and read a Idp register
*
*/

USHORT lnIdpRead
    (
    DRV_CTRL *	pDrvCtrl,		/* driver control */
    int		reg			/* register to select */
    )
    {
    int dv = (int)pDrvCtrl->devAdrs;

    /* select IDP register */
    sysOutWord(dv+RAPOffset,reg);;

    /* get contents of IDP register */
    return (sysInWord(dv+IDPOffset));

    }

/*******************************************************************************
*
* lnRestart - restart the device after a fatal error
*
* This routine takes care of all the messy details of a restart.  The device
* is reset and re-initialized.  The driver state is re-synchronized.
*/

static void lnRestart
    (
    int unit,
    int loc,
    int stat
    )
    {
    printf("ln: RESTART called, loc = %d, stat = %x\n",loc,stat);
    taskSuspend (0);
    }


#include "sys/socket.h"

/*******************************************************************************
*
* convertDestAddr - converts socket addr into enet addr and packet type
*
*/

static BOOL convertDestAddr
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

#ifdef LNISA_DEBUG

STATUS lnIsa_restart(DRV_CTRL     *pDrvCtrl)
{

    sysIntDisablePIC (pDrvCtrl->ilevel);        /* disable LANCE interrupts */
    pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);
    if (lnChipReset (pDrvCtrl) == ERROR)	/* reset lance device */
    	{
	printf ("Unable to Reset lnIsa%d\n",pDrvCtrl->idr.ac_if.if_unit);
	return (ERROR);
	}

    { /* Initialize all shared memory structures */

    ln_rmd  *rmd;
    ln_tmd  *tmd;
    char    *buf;
    ULONG    pTemp;
    int     loopy;

    /* Set up the Rx descriptors */

    rmd = pDrvCtrl->rring;                      /* receive ring */
    buf = pDrvCtrl->rmd_ring.r_bufs;
    for (loopy = 0; loopy < pDrvCtrl->rsize; loopy++)
    {
        pTemp = (ULONG)LN_CACHE_VIRT_TO_PHYS(buf);/* convert to physical addr */
      rmd->rbuf_adr = (USHORT)(pTemp & 0x0000ffff); /* bits 15:00 buf addr */
      rmd->rbuf_rmd1 = (USHORT)((pTemp & 0x00ff0000) >> 16); /* bits 7:0 */
      /* neg of buffer byte count */
      rmd->rbuf_bcnt = ((RMD2_BCNT_MSK & -(pDrvCtrl->bufSize)) | RMD2_MBO);
      rmd->rbuf_mcnt = 0;			/* no message byte count yet */
      rmd->rbuf_rmd1 |= RMD1_OWN;	/* buffer now owned by LANCE */
      rmd++;					/* step to next descriptor */
      buf += (pDrvCtrl->bufSize);		/* step to the next buffer */
    }

    pDrvCtrl->rindex = 0;

     /* Setup the Tx descriptors */

    tmd = pDrvCtrl->tring;                 /* transmit ring */
    buf = pDrvCtrl->tmd_ring.t_bufs;

    for (loopy = 0; loopy < pDrvCtrl->tsize; loopy++)
    {
      pTemp = (ULONG)LN_CACHE_VIRT_TO_PHYS(buf); /* convert to physical addr */
      tmd->tbuf_adr = (USHORT) (pTemp & 0x0000ffff); /* bits 15:00 of addr */
      tmd->tbuf_tmd1 = (USHORT)((pTemp & 0x00ff0000) >> 16); /* bits 7:0 */
      tmd->tbuf_tmd2 = 0;		/* no length of Tx Buf yet */
      tmd->tbuf_tmd3 = 0;		/* clear status bits */
      tmd->tbuf_tmd1 |= TMD1_ENP;	/* buffer is end of packet */
      tmd->tbuf_tmd1 |= TMD1_STP;	/* buffer is start of packet */
      tmd++;					/* step to next descriptor */
      buf += (pDrvCtrl->bufSize);		/* step to next buffer */
    }

    pDrvCtrl->tindex = pDrvCtrl->dindex = 0;

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();
    }

    { /* Start the device */

    USHORT	stat = 0;
    ULONG	pTemp;
    int		timeoutCount = 0;

    lnCsrWrite (pDrvCtrl, 0, CSR0_STOP);	/* set the stop bit */

    /* Point the device to the initialization block */

    pTemp = (ULONG)LN_CACHE_VIRT_TO_PHYS ( pDrvCtrl->ib );
    lnCsrWrite (pDrvCtrl, 2, (USHORT)((pTemp >> 16) & 0x000000ff));
    lnCsrWrite (pDrvCtrl, 1, (USHORT)(pTemp & 0x0000ffff));
    lnCsrWrite (pDrvCtrl, 0, CSR0_INIT);            /* init chip (read IB) */

    while (((stat = lnCsrRead (pDrvCtrl, 0)) & (CSR0_IDON | CSR0_ERR)) == 0)
        {
        if (timeoutCount++ > 5) 
	  break;
        taskDelay(sysClkRateGet());
        }

    if ((stat & CSR0_ERR) == CSR0_ERR)
	{
	if ((stat & CSR0_MERR)  == CSR0_MERR)
	    {
	    printf ("Lance Memory error during initialization:\t0x%x\n",
		    stat);
	    }
	else
	    {
	    printf ("Lance Error during initialization:\t0x%x\n",
		    stat);
	    }
	}

    /* log chip initialization failure */

    if ( (stat & CSR0_ERR) || (timeoutCount >= 0x10000) )
        {
        printf ("lnIsa%d: Device initialization failed\n",
                    pDrvCtrl->idr.ac_if.if_unit);
        return (ERROR);
        }
    /* Device is initialized.  Start transmitter and receiver.  The device
     * RAP register is left selecting CSR 0.
     */

    lnCsrWrite(pDrvCtrl, 0, CSR0_IDON | CSR0_IENA | CSR0_STRT);

    sysIntEnablePIC (pDrvCtrl->ilevel);         /* enable LANCE interrupts */

    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);

    } /* Block end; device initialization */

    return (OK);
}

#endif /* LNISA_DEBUG */

BOOL outHook (struct ifnet *pIf,char *buf,int len)
{
  UINT8 *myHdr = (UINT8 *)buf;

  printf("\nSending Packet:\n");
  printf("Dest: %02x:%02x:%02x:%02x:%02x:%02x\n",myHdr[0],myHdr[1],
	 myHdr[2],myHdr[3],myHdr[4],myHdr[5]);
  printf("Src : %02x:%02x:%02x:%02x:%02x:%02x\n",myHdr[6],myHdr[7],
	 myHdr[8],myHdr[9],myHdr[10],myHdr[11]);
  printf("len = %d\n",len);
  printf("Data at %p\n",buf);
  return FALSE;
}

BOOL inHook (struct ifnet *pIf,char *buf,int len)
{
  UINT8 *myHdr = (UINT8 *)buf;

  printf("\nGot Packet:\n");
  printf("Dest: %02x:%02x:%02x:%02x:%02x:%02x\n",myHdr[0],myHdr[1],
	 myHdr[2],myHdr[3],myHdr[4],myHdr[5]);
  printf("Src : %02x:%02x:%02x:%02x:%02x:%02x\n",myHdr[6],myHdr[7],
	 myHdr[8],myHdr[9],myHdr[10],myHdr[11]);
  printf("len = %d\n",len);
  printf("Data at %p\n",buf);
  return FALSE;
}


/* END OF FILE */
