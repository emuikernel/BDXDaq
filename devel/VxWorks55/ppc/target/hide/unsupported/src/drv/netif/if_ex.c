/* if_ex.c - Excelan EXOS 201/202/302 Ethernet network interface driver */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
 * Copyright (c) 1980, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specify the terms and conditions for redistribution.
 *
 *      @(#)if_ex.c     7.1 (Berkeley) 6/5/86
 */

/*
modification history
--------------------
03u,15jul97,spm  added ARP request to SIOCSIFADDR ioctl handler
03t,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
03s,14oct93,wmd  Added use of ntohs macro to correct parameter passed in
		 do_protocol_with_type(), spr #2571.
03r,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
03q,27apr93,jdi  restored '/' to beginning of line 1 (spr 2180).
03p,22feb93,jdi  documentation cleanup.
03o,28jan93,rfs  Reworked sequence within attach routine that gleans the
                 Ethernet address from the exos board.  The reply message
                 is now handled by the interrupt handler.  SPR #1931.
03n,14oct92,rfs  Added documentation.
03m,02oct92,rfs  Made multiple attach calls per unit return OK, not ERROR.
03l,30sep92,rfs  Gathered all shared memory areas into one area that is
                 allocated from cache-safe memory.  The driver will abort if
                 a cache-safe area cannot be provided.  Also brought the
                 driver up to conventions used in the others, except I left in
                 the outgoing mbuf queue, since the driver only uses one
                 transmit buffer.
03k,09sep92,gae  documentation tweaks.
03j,04sep92,ccc  Added address modifier parameter to exattach() routine.
                 Now uses sysBusToLocalAdrs() to convert base address.
03i,18jul92,smb  Changed errno.h to errnoLib.h.
03h,26may92,rrr  the tree shuffle
                 -changed includes to have absolute path from h/
03g,04may92,elh  changed exInit to return 0 if running.  cleaned up
                 some ansi warnings.
03f,22apr92,jwt  converted CPU==SPARC to CPU_FAMILY==SPARC; copyright.
03e,11nov91,jpb  fixed bug in exoutput status return path.
                 disable data cache (currently SPARC only).
03d,04oct91,rrr  passed through the ansification filter
                 -changed functions to ansi style
                 -changed includes to have absolute path from h/
                 -fixed #else and #endif
                 -changed READ, WRITE and UPDATE to O_RDONLY O_WRONLY and O_RDWR
                 -changed VOID to void
                 -changed copyright notice
03c,13sep91,jpb  added argument to exattach to specify address modifier for
                 DME access to memory.  Made exBusAdrsSpace local.
03b,29apr91,hdn  use SIZEOF_ETHERHEADER instead of sizeof(ether_header).
03a,10apr91,jdi  documentation cleanup; fixed comment block in exattach() that
                 had effectively commented out declaration of ivec;
                 doc review by elh.
02z,02oct90,hjb  made exinit() return int always.
02y,19sep90,dab  made exinit() return int.
02x,10aug90,dnw  added forward declarations of void routines.
                 added include of if_subr.h.
02w,15jul90,dnw  coerced calloc() to (char*) where necessary
02v,11jul90,hjb  removed references to ipintr().  changed calls to do_protocol()
                 to use do_protocol_with_type() instead.
02u,26jun90,hjb  copy_from_mbufs() & m_freem() fix
02t,02may90,gae  fixed EXOS 302 initialization of address modifier register.
           +hjb  changed ETHERMIN+X to ETHERSMALL.  Turned off DEBUG.
                 increased exconfig test delay from 5 secs.
02s,19apr90,hjb  deleted param.h, de-linted, modifications for
                 copy_from_mbufs().
02r,18mar90,hjb  reduction of redundant code and addition of cluster support.
02q,07aug89,gae  SPARC cleanup.
    07apr89,ecs  fixed references to unaligned structure in exhangrcv & exstart.
02p,29jul88,gae  fixed printf happening at interrupt level.
02o,30may88,dnw  changed to v4 names.
02n,28jan88,jcf  made kernel independent.
02m,05jan88,rdc  added include of systm.h
02l,20nov87,ecs  lint
           &dnw  changed exattach to take int vec num instead of vec adrs.
02k,17nov87,dnw  fixed printing of transmit errors.
02j,03nov87,dnw  added vxTdelay(1) to init and configuration wait loops so that
                 entire cpu (and in some cases entire bus!) isn't tied up.
                 added global variable exBusAdrsSpace to allow modification
                 of what address space excelan should use to address buffers.
                 added output hook code.
                 changed calling sequence to input hook.
                 changed exreset to actually reset hardware.
02i,21oct87,ecs  delinted
02h,03may87,dnw  removed unnecessary includes.
                 changed BUS_ADDR macro to exHostAdrs() routine
                 which uses new call to sysLocalToBusAdrs().
                 added input hook code.
02g,29apr87,dnw  changed exconfig() to set meaninfug status on error.
02f,08apr87,llk  enable interrupts from the excelan board with sysIntEnable.
02e,03apr87,ecs  added copyright.
02d,01apr87,jlf  delinted.
02c,20mar87,dnw  Added probe of excelan board and timeout of self-test
                 in exconfig.
02b,14feb87,dnw  Formatted and cleaned up.
                 Changed to keep array of ptrs to ex_softc structure
                 and to dynamically allocate those structures as attached.
                 Up to 10 controllers allowed.
                 Fixed call to intConnect to pass unit to interrupt routine.
                 Changed to use global sysBus, sysLocalToBusAdrs(), and
                 sysIntAck(), instead of passing bus type, bus adrs offset,
                 and ack address, respectively, to exattach().
02a,02feb87,jlf  Removed CLEAN ifdefs.
01c,30nov86,dnw  added exIntrAck kludge to ack ints for hkv2f.
                 changed vmeOffset to exBusOffset.
01b,26nov86,llk  got rid of conditional compiles dependent on BUS.
                 Added LOCAL varibles exBus and vmeOffset.
01a,31oct86,jlf  changed BUS_ADDR to mask out upper address bits
                 added HK kludge.
... adapted from original Excelan interface by George Powers
*/

/*
This module implements the Excelan EXOS 201/202/302 Ethernet network
interface driver.

This driver is designed to be moderately generic, operating unmodified
across the range of architectures and targets supported by VxWorks.
To achieve this, the driver must be given several target-specific
parameters.  These parameters, and the mechanisms used to communicate them
to the driver, are detailed below.  This driver supports up to four
individual units per CPU.

All packet control information and data moves between the EXOS board and the
target board in a shared memory region.  This shared memory must reside on the
target board, since the EXOS board does not present any memory to the VME bus. Therefore, this driver will obtain an area of local memory, and assumes that
this area of memory can be accessed from the VME bus.

BOARD LAYOUT
The diagram below shows the relevant jumpers for VxWorks configuration.
Default values are: I/O address 0x00ff0000, Standard Addressing (A24),
interrupt level 2.
.bS
______________________________                                            _
|             P1             | EXOS 202 OLD             NO P2!            |
|                            |___________________________________________ |
|:::XDDDU:::X:::::::X:::::X:                                              |
|BGINBGOTBREQ IO ADRS INT LVL                                             |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                  "                                      |
|                                   X INT ACK                             |
|                                   X                                     |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|_________________________________________________________________________|
.bE
.bS
______________________________              _______________________________
|             P1             | EXOS 202 NEW |             P2              |
|                            ----------------                             |
|:::::X UUUD :::X  :::::::X  :::::X:                                      |
|BGIN   BGOT BREQ   IO ADRS  INT LVL                                      |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
| -"-                                                                     |
|                                                                         |
| INT ACK                                                                 |
|                                                                         |
|                                                                         |
|                                                                         |
|_________________________________________________________________________|
.bE
.bS
______________________________              _______________________________
|             P1             |   EXOS 302   |             P2              |
|                            ----------------                             |
|             :X:  :::::X:    :::::    ::::::::XXXXXXXX                   |
|         INT ACK  INT LVL    *24/32 ADR   IO ADRS                        |
|         J54-J56  J47-J53    J42-J46      J26-J41                        |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|         ::::::X:                                                        |
|           SQE                       * remove J44 for A32 master mode    |
|          J2-J9                      * remove J46 for A32 slave mode     |
|_________________________________________________________________________|
.bE

EXTERNAL INTERFACE
This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine;
there is no separate initialization routine.  Therefore, in the global interface
structure, the function pointer to the initialization routine is NULL.

The only user-callable routine is exattach(), which publishes the `ex'
interface and initializes the driver and device.

TARGET-SPECIFIC PARAMETERS
.iP "base VME address of EXOS I/O ports"
This parameter is passed to the driver by exattach().
The EXOS board presents a small set of I/O ports onto the VME bus.  This
address is jumper selectable on the EXOS board.  This parameter is the
same as the address selected on the EXOS board.
.iP "VME address modifier code, EXOS access"
This parameter is passed to the driver by exattach().
It specifies the AM (address modifier) code to use when 
the driver accesses the VME address space (ports) of the EXOS board.
.iP "VME address modifier code, target access"
This parameter is passed to the driver by exattach().
It specifies the AM code that the EXOS board needs to use
when it accesses the shared memory on the target board.
.iP "interrupt vector"
This parameter is passed to the driver by exattach().
It specifies the interrupt vector to be used by the driver
to service an interrupt from the EXOS board.  The driver connects
the interrupt handler to this vector by calling intConnect().
.iP "interrupt level"
This parameter is passed to the driver by exattach().
It specifies the interrupt level that is associated with the
interrupt vector.  The driver enables the interrupt from the EXOS by
calling sysIntEnable() with this parameter.
.LP

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one interrupt vector
    - 8 bytes in the initialized data section (data)
    - 668 bytes in the uninitialized data section (BSS)

The above data and BSS requirements are for the MC68020 architecture 
and may vary for other architectures.  Code size (text) varies greatly between
architectures and is therefore not quoted here.

This driver uses cacheDmaMalloc() to allocate the memory to be shared 
with the EXOS board.  The size requested is 3512 bytes.

This driver can only be operated if this shared memory region is non-cacheable.
The driver cannot maintain cache coherency for the shared memory because 
asynchronous modifications by the EXOS board may share cache lines with
locations being operated on by the driver.

SEE ALSO: ifLib
*/

#include "vxWorks.h"
#include "vme.h"
#include "net/mbuf.h"
#include "net/protosw.h"
#include "sys/socket.h"
#include "sys/ioctl.h"
#include "errnoLib.h"
#include "iv.h"
#include "memLib.h"
#include "cacheLib.h"

#include "net/if.h"
#include "net/route.h"

#ifdef  INET
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"
#include "netinet/if_ether.h"
#endif  /* INET */

#include "drv/netif/if_exreg.h"

#include "sysLib.h"
#include "wdLib.h"
#include "iosLib.h"
#include "intLib.h"
#include "etherLib.h"
#include "netLib.h"
#include "stdlib.h"
#include "stdio.h"
#include "logLib.h"
#include "vxLib.h"
#include "net/systm.h"
#include "net/if_subr.h"
#include "net/unixLib.h"


/***** LOCAL DEFINITIONS *****/

#define EX_DEBUG                /* enable some debug checks and messages */

#define MAX_UNITS  4            /* max number of EXOS controllers supported */

#define NH2X    4               /* a sufficient number is critical */
#define NX2H    1               /* this is pretty arbitrary */

/* Typedefs for external structures that are not typedef'd in their .h files */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;                  /* Interface Data Record wrapper */
typedef struct ifnet IFNET;                 /* real Interface Data Record */
typedef struct sockaddr SOCK;               /* generic socket address */
typedef struct exdevice DEV_CTRL;           /* EXOS device control */
typedef struct ex_msg EX_MSG;               /* an EXOS message */
typedef struct confmsg CFG_MSG;             /* an EXOS configuration message */
typedef struct stat_array STAT_ARRAY;       /* EXOS status area */

#define EX_MSG_SIZ      sizeof(EX_MSG)
#define CFG_MSG_SIZ     sizeof(CFG_MSG)
#define STAT_ARRAY_SIZ  sizeof(STAT_ARRAY)

/* The definition of the driver control structure */

typedef struct drv_ctrl
    {
    IDR                 idr;            /* interface data record */

    BOOL                attached;       /* indicates attach completed */
    int                 ivec;           /* interrupt vector */
    int                 ilevel;         /* interrupt level */
    int                 flags;          /* private flags */

    int                 shMemSize;      /* size of shared memory area */
    caddr_t             pShMem;         /* pointer to shared memory area */
    DEV_CTRL            *pDev;          /* device address */
    EX_MSG              *pH2XNext;      /* host pointer to request queue */
    EX_MSG              *pX2HNext;      /* host pointer to reply queue */

    /* Pointers to shared memory members.
     * These are static after initialization.
     */

    CFG_MSG             *pCfgMsg;       /* configuration message */
    STAT_ARRAY          *pStatArray;    /* EXOS writes stats here */
    u_short             *pH2XHdr;       /* ptr to EXOS' host outgoing header */
    u_short             *pX2HHdr;       /* ptr toEXOS' host incoming header */
    EX_MSG              *pH2XQueue;     /* request msg buffers */
    EX_MSG              *pX2HQueue;     /* reply msg buffers */
    caddr_t             pWriteBuf;      /* pointer to write buffer */
    caddr_t             pReadBuf;       /* pointer to read buffer */

    } DRV_CTRL;

/* Some values for the flags */

#define EX_XPENDING     1                   /* xmit rqst pending on EXOS */
#define EX_STATPENDING  (1<<1)              /* stats rqst pending on EXOS */
#define EX_RUNNING      (1<<2)              /* board is running */
#define EX_SETADDR      (1<<3)              /* physaddr has been changed */


/***** GLOBALS *****/


/***** LOCALS *****/

/* The array of driver control structures, one per unit supported */

LOCAL DRV_CTRL drvCtrl [MAX_UNITS];

#undef EX_RESET
LOCAL int EX_RESET = 0; /* because using constant causes "clr" inst. */

LOCAL BOOL exBit32 = FALSE;     /* TRUE = in VME extended address space */

/*
 * VME address space that the EXOS should use to access host buffers, etc.
 * This is normally standard space but can be changed through the exattach
 * argument (exDmaAm).
 */

LOCAL int exBusAdrsSpace;


/* forward static functions */

static void exReset (int unit);
static STATUS exConfig (int unit, int itype);
#ifdef BSD43_DRIVER
static void exStart (int unit);
#else
static void exStart (DRV_CTRL *pDrvCtrl);
#endif
static void exIntr (int unit);
static EX_MSG *exMsgBlkGet (DRV_CTRL *pDrvCtrl);
static void exRecvAndHang (int unit, int len, int status);
static void exRecv (int unit, int len, int status);
static void exRxRqst (int unit);
#ifdef BSD43_DRIVER
static int exOutput (IDR *pIDR, MBUF *pMbuf, SOCK *pDest);
#endif
static int exIoctl (IDR *pIDR, int cmd, caddr_t data);
static char *exHostAdrs (char *localAdrs);
static void exGiveRequest (EX_MSG *pMsg, DEV_CTRL *pDev);


/*******************************************************************************
*
* exattach - publish the `ex' network interface and initialize the driver and device
*
* This routine publishes the `ex' interface by filling in a network 
* interface record and adding this record to the system list.  It also
* initializes the driver and the device to the operational state.
*
* RETURNS: OK or ERROR.
*/

STATUS exattach
    (
    int unit,           /* logical number of this interface */
    char *pDev,         /* bus address of EXOS device ports */
    int ivec,           /* interrupt vector */
    int ilevel,         /* interrupt level */
    int exDmaAm,        /* VME addr modifier to access CPU memory */
    int exAdrsAm        /* VME addr modifier to access EXOS ports */
    )
    {
    int             loopy;
    STATUS          ret;                /* return value of called funcs */
    EX_MSG          *pMsg;
    DRV_CTRL        *pDrvCtrl;


    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    /* Ensure single invocation per system life */

    pDrvCtrl = & drvCtrl [unit];
    if (pDrvCtrl->attached)
        return (OK);

    /* Save some parms */

    pDrvCtrl->ivec  = ivec;
    pDrvCtrl->ilevel= ilevel;

    /* Convert the buss address of the EXOS board's registers to the local
     * address to use for the specified address space.
     */

    if  (
        sysBusToLocalAdrs (exAdrsAm, pDev, (char **) & pDrvCtrl->pDev)
        == ERROR
        )
        return (ERROR);

    /* Publish the interface data record */

#ifdef BSD43_DRIVER
    ether_attach (&pDrvCtrl->idr.ac_if, unit, "ex", (FUNCPTR) NULL,
                  (FUNCPTR) exIoctl, (FUNCPTR) exOutput, (FUNCPTR) exReset);
#else
    ether_attach (
                 &pDrvCtrl->idr.ac_if, 
                 unit, 
                 "ex", 
                 (FUNCPTR) NULL,
                 (FUNCPTR) exIoctl, 
                 ether_output, 
                 (FUNCPTR) exReset
                 );
    pDrvCtrl->idr.ac_if.if_start = (FUNCPTR)exStart;
#endif

    /* setup address modifier */

    if  (
        (VME_AM_EXT_USR_DATA <= exDmaAm) &&
        (exDmaAm <= VME_AM_STD_SUP_ASCENDING)
        )
        exBusAdrsSpace = exDmaAm;
    else
        exBusAdrsSpace = VME_AM_STD_SUP_DATA;

    exBit32 = VME_AM_IS_EXT(exBusAdrsSpace);


    { /***** Obtain memory area that will be shared with the EXOS board *****/

    /* this driver can't handle incoherent caches */

    if ( !CACHE_DMA_IS_WRITE_COHERENT () || !CACHE_DMA_IS_READ_COHERENT () )
        {
        printErr ( "\nex%d: requires cache coherent memory\n", unit );
        return (ERROR);
        }

    pDrvCtrl->shMemSize =
                CFG_MSG_SIZ +               /* the config message */
                STAT_ARRAY_SIZ +            /* the status array */
                                            /* the EXOS firmware headers */
                sizeof (*pDrvCtrl->pH2XHdr) +
                sizeof (*pDrvCtrl->pX2HHdr) +
                (EX_MSG_SIZ * NH2X) +       /* outbound message queue */
                (EX_MSG_SIZ * NX2H) +       /*  inbound message queue */
                EXMAXRBUF +                 /* one packet write buffer */
                EXMAXRBUF +                 /* one packet read buffer */
                16;                         /* to allow alignment */

    /* Attempt to get cache coherent memory */

    pDrvCtrl->pShMem = cacheDmaMalloc (pDrvCtrl->shMemSize);

    if ( pDrvCtrl->pShMem == NULL )
        {
#ifdef EX_DEBUG
        printErr ( "\nex%d: no memory available\n", unit );
#endif
        return ( ERROR );
        }

    /* Clear the whole block */

    bzero (pDrvCtrl->pShMem, pDrvCtrl->shMemSize);

    } /* Block end */


    { /***** Carve up the shared memory *****/

    caddr_t pCur;                   /* pointer to current location */

    pCur = pDrvCtrl->pShMem;        /* start at location assigned above */
    pCur = (caddr_t) ((int)pCur + 15);   /* bump up to next 16 byte page */
    pCur = (caddr_t) ((int) pCur & ~0xf);

    /* This 16-byte-page aligned address is used as the base address of the
     * shared memory area.  This base address is used in offset calculations
     * for various values communicated to the EXOS firmware.  The config
     * message starts at this base address, and can therefore be used as an
     * alias.
     */

    pDrvCtrl->pCfgMsg = (CFG_MSG *) pCur;
    pCur = (caddr_t)((int)pCur + CFG_MSG_SIZ);

    pDrvCtrl->pStatArray = (STAT_ARRAY *) pCur;
    pCur = (caddr_t)((int)pCur + STAT_ARRAY_SIZ);

    pDrvCtrl->pH2XHdr = (u_short *) pCur;
    pCur = (caddr_t)((int)pCur + sizeof (*pDrvCtrl->pH2XHdr));

    pDrvCtrl->pX2HHdr = (u_short *) pCur;
    pCur = (caddr_t)((int)pCur + sizeof (*pDrvCtrl->pX2HHdr));

    pDrvCtrl->pH2XQueue = (EX_MSG *) pCur;
    pCur = (caddr_t)((int)pCur + (EX_MSG_SIZ * NH2X));

    pDrvCtrl->pX2HQueue = (EX_MSG *) pCur;
    pCur = (caddr_t)((int)pCur + (EX_MSG_SIZ * NX2H));

    pDrvCtrl->pWriteBuf = pCur;
    pCur = (caddr_t)((int)pCur + EXMAXRBUF);

    pDrvCtrl->pReadBuf = pCur;

    } /* Block end */


    { /***** Initialize the message queues and headers *****/

    int     base;
    u_short offset;

    /* The offset of certain objects from a base address is a common theme
     * in the shared data.  The config message happens to start at the base
     * address of the shared memory region, so we cast it and store it as an
     * integer once, to make the multiple uses easier to read.
     */

    base = (int) pDrvCtrl->pCfgMsg;

    /* First the request queue (host-to-EXOS) */

    pMsg = pDrvCtrl->pH2XQueue;
    for ( loopy = NH2X; loopy--; )
        {
        /* The link is the offset of the next message from the base of
         * the shared memory area.
         */
        offset          = (u_short) ((int)(pMsg+1) - base);
        pMsg->mb_link     = offset;

        pMsg->mb_rsrv     = 0;
        pMsg->mb_length   = MBDATALEN;
        pMsg->mb_status   = MH_HOST;            /* we own it */
        pMsg->mb_next     = pMsg+1;             /* our ptr to next */
        pMsg++;
        }

    /* Fix up the last message to point back at first message */

    pMsg--;
    offset = (u_short) ((int)pDrvCtrl->pH2XQueue - base);
    pMsg->mb_link = offset;
    pMsg->mb_next = pDrvCtrl->pH2XQueue;

    /* Initialize the variables that indicate the next message.
     * Ours is a direct pointer, while the EXOS firmware uses an offset.
     */

    *pDrvCtrl->pH2XHdr = offset;
    pDrvCtrl->pH2XNext = pDrvCtrl->pH2XQueue;


    /* Now the reply queue (EXOS-to-host) */

    pMsg = pDrvCtrl->pX2HQueue;
    for ( loopy = NX2H; loopy--; )
        {
        /* The link is the offset of the next message from the base of
         * the shared memory area.
         */
        offset            = (u_short) ((int)(pMsg+1) - base);
        pMsg->mb_link     = offset;

        pMsg->mb_rsrv     = 0;
        pMsg->mb_length   = MBDATALEN;
        pMsg->mb_status   = MH_EXOS;            /* EXOS owns it */
        pMsg->mb_next     = pMsg+1;             /* our ptr to next */
        pMsg++;
        }

    /* Fix up the last message to point back at first message */

    pMsg--;
    offset = (u_short) ((int)pDrvCtrl->pX2HQueue - base);
    pMsg->mb_link = offset;
    pMsg->mb_next = pDrvCtrl->pX2HQueue;

    /* Initialize the variables that indicate the next message.
     * Ours is a direct pointer, while the EXOS firmware uses an offset.
     */

    *pDrvCtrl->pX2HHdr = offset;
    pDrvCtrl->pX2HNext = pDrvCtrl->pX2HQueue;

    } /* Block end */


    /* Configure the board */

    if (sysBus == VME_BUS)
        ret = exConfig (unit, 4);              /* with vectored interrupts */
    else
        ret = exConfig (unit, 3);              /* with level interrupts */

    if (ret != OK)
        return (ERROR);

    /* Connect interrupt handler, enable interrupt */

    (void) intConnect (INUM_TO_IVEC (ivec), exIntr, unit);
    (void) sysIntEnable (ilevel);

    /* Ask the board for its physical Ethernet address.  The reply will be
     * processed by the interrupt handler.  We wait for the handler to complete
     * by using the ethernet address field as a sync flag: it will be zero
     * until filled in by the reply processing.
     */

    pMsg = exMsgBlkGet (pDrvCtrl);
    pMsg->mb_rqst       = LLNET_ADDRS;
    pMsg->mb_na.na_mask = READ_OBJ;
    pMsg->mb_na.na_slot = PHYSSLOT;

    exGiveRequest (pMsg, pDrvCtrl->pDev);

    for ( loopy = 10; ; loopy-- )
        {
        taskDelay (10);
        if ( *(long *)pDrvCtrl->idr.ac_enaddr )  /* check 'sync flag' */
            break;
        if ( loopy < 0 )                      /* if reached retry limit */
            return (ERROR);
        }

    /* Enable the EXOS board onto the Ethernet, using NET_MODE command */

    pMsg = exMsgBlkGet (pDrvCtrl);
    pMsg->mb_rqst       = LLNET_MODE;
    pMsg->mb_nm.nm_mask = WRITE_OBJ;
    pMsg->mb_nm.nm_optn = 0;
    pMsg->mb_nm.nm_mode = MODE_PERF;

    exGiveRequest (pMsg, pDrvCtrl->pDev);

    exRxRqst (unit);                   /* post a receive request */

    /* Set flags */

    pDrvCtrl->attached = TRUE;
    pDrvCtrl->idr.ac_if.if_flags |= ( IFF_RUNNING | IFF_UP | IFF_NOTRAILERS );
    pDrvCtrl->flags = EX_RUNNING;

    return (OK);
    }

/*******************************************************************************
*
* exReset - reset the EXOS board and bring down interface
*/

LOCAL void exReset
    (
    int unit
    )
    {
    int temp;
    DRV_CTRL *pDrvCtrl;

    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return;

    pDrvCtrl = & drvCtrl [unit];

    pDrvCtrl->idr.ac_if.if_flags = 0;
    pDrvCtrl->flags &= ~EX_RUNNING;

    if (sysBus == VME_BUS)
        pDrvCtrl->pDev->bus.vme.xd_porta = EX_RESET;
    else
        temp = pDrvCtrl->pDev->bus.multi.xd_porta;    /* access location */
    }

/*******************************************************************************
*
* exConfig - configure EXOS board
*
* Reset, test, and configure EXOS.
*/

LOCAL STATUS exConfig
    (
    int unit,
    int itype                       /* interrupt type */
    )
    {
    char            val;           /* sizeof xd_porta */
    int             ix;
    u_long          shiftreg;
    DRV_CTRL        *pDrvCtrl;
    DEV_CTRL        *pDev;
    CFG_MSG         *pCfgMsg;

    pDrvCtrl = & drvCtrl [unit];
    pDev     = pDrvCtrl->pDev;
    pCfgMsg  = pDrvCtrl->pCfgMsg;

   /* Reset EXOS via probe (in case board doesn't exist) */

    if (sysBus == VME_BUS)
        {
        val = EX_RESET;

        if (vxMemProbe ((char *)&pDev->bus.vme.xd_porta, O_WRONLY,
                        sizeof (pDev->bus.vme.xd_porta), &val) == ERROR)
            {
            errnoSet (S_iosLib_CONTROLLER_NOT_PRESENT);
            return (ERROR);
            }
        }
    else
        {
        if (vxMemProbe ((char *)&pDev->bus.multi.xd_porta, O_RDONLY,
                        sizeof (pDev->bus.multi.xd_porta), &val) == ERROR)
            {
            errnoSet (S_iosLib_CONTROLLER_NOT_PRESENT);
            return (ERROR);
            }
        }

    /* wait for test to complete (10 secs max) */

    for (ix = 10 * sysClkRateGet (); ix > 0; ix--)
        {
        if (sysBus == VME_BUS)
            {
            if ((pDev->bus.vme.xd_portb & EX_TESTOK) != 0)
                break;
            }
          else
            {
            if ((pDev->bus.multi.xd_portb & EX_TESTOK) != 0)
                break;
            }

        taskDelay (1);
        }

    if (ix == 0)
        {
#ifdef EX_DEBUG
        printErr ("ex%d: self-test failed.\n", unit);
#endif
        errnoSet (S_ioLib_DEVICE_ERROR);
        return (ERROR);
        }

    /* Write address modifier to Port C on EXOS 302 (after reset). */

    if (sysBus == VME_BUS && exBit32)
        pDrvCtrl->pDev->bus.vme.xd_portc = exBusAdrsSpace;

    /* Set up configuration message. */

    pCfgMsg->cm_1rsrv       = 1;
    pCfgMsg->cm_cc          = 0xFF;
    pCfgMsg->cm_opmode      = 0;        /* link-level controller mode */
    pCfgMsg->cm_dfo         = 0x0101;   /* enable host data order conversion */
    pCfgMsg->cm_dcn1        = 1;
    pCfgMsg->cm_2rsrv[0]    = 0;
    pCfgMsg->cm_2rsrv[1]    = 0;
    pCfgMsg->cm_ham         = 3;          /* absolute address mode */
    pCfgMsg->cm_3rsrv       = 0;
    pCfgMsg->cm_mapsiz      = 0;
    pCfgMsg->cm_byteptrn[0] = 0x01;       /* EXOS deduces data order of host */
    pCfgMsg->cm_byteptrn[1] = 0x03;       /*  by looking at this pattern */
    pCfgMsg->cm_byteptrn[2] = 0x07;
    pCfgMsg->cm_byteptrn[3] = 0x0F;
    pCfgMsg->cm_wordptrn[0] = 0x0103;
    pCfgMsg->cm_wordptrn[1] = 0x070F;
    pCfgMsg->cm_lwordptrn   = 0x0103070F;

    for (ix = 0; ix < 20; ix++)
        pCfgMsg->cm_rsrvd[ix] = 0;

    pCfgMsg->cm_mba         = 0xFFFFFFFF;
    pCfgMsg->cm_nproc       = 0xFF;
    pCfgMsg->cm_nmbox       = 0xFF;
    pCfgMsg->cm_nmcast      = 0xFF;
    pCfgMsg->cm_nhost       = 1;
    pCfgMsg->cm_h2xba       = (u_long) exHostAdrs ( (char *) pCfgMsg );
    pCfgMsg->cm_h2xhdr      = (u_short)
                              ( (int)(pDrvCtrl->pH2XHdr) - (int)pCfgMsg );
    pCfgMsg->cm_h2xtyp      = 0;        /* should never wait for rqst buffer */
    pCfgMsg->cm_x2hba       = pCfgMsg->cm_h2xba;
    pCfgMsg->cm_x2hhdr      = (u_short)
                              ( (int)(pDrvCtrl->pX2HHdr) - (int)pCfgMsg );
    pCfgMsg->cm_x2htyp      = itype;         /* 0=none, 4=vectored, 3=level */

    if (sysBus == VME_BUS)
        pCfgMsg->cm_x2haddr = pDrvCtrl->ivec;

    /*
     * Write config msg address to EXOS and wait for
     * configuration to complete (guaranteed response within 2 seconds).
     */

    shiftreg = (u_long)0x0000FFFF;

    for (ix = 0; ix < 8; ix++)
        {
        if (ix == 4)
            shiftreg = (u_long) exHostAdrs ((char *)pCfgMsg);

        if (sysBus == VME_BUS)
            {
            while (pDev->bus.vme.xd_portb & EX_UNREADY)
                ;
            pDev->bus.vme.xd_portb = (u_char)(shiftreg & 0xFF);
            }
        else
            {
            while (pDev->bus.multi.xd_portb & EX_UNREADY)
                ;
            pDev->bus.multi.xd_portb = (u_char)(shiftreg & 0xFF);
            }

        shiftreg >>= 8;
        }

    for (ix = 3 * sysClkRateGet (); (pCfgMsg->cm_cc == 0xFF) && ix; --ix)
        taskDelay (1);

    if (pCfgMsg->cm_cc)
        {
#ifdef EX_DEBUG
        printErr    (
                    "ex%d: configuration failed; cc=0x%x\n",
                    unit,
                    pCfgMsg->cm_cc
                    );
#endif
        errnoSet (S_ioLib_DEVICE_ERROR);
        return (ERROR);
        }

    return (OK);
    }

/*******************************************************************************
*
* exStart - start or re-start output on interface
*
* Get another datagram to send off of the interface queue,
* and map it to the interface before starting the output.
* This routine is called by exOutput(), and exIntr().
* In all cases, interrupts by EXOS are disabled.
*/

#ifdef BSD43_DRIVER
LOCAL void exStart
    (
    int unit
    )
    {
    DRV_CTRL        *pDrvCtrl;
#else
LOCAL void exStart
    (
    DRV_CTRL * 	pDrvCtrl
    )
    {
#endif
    EX_MSG          *pMsg;
    MBUF            *pMbuf;
    int             len;
    int             s;


#ifdef BSD43_DRIVER
    pDrvCtrl = & drvCtrl [unit];
#endif

    s = splnet ();

    if ((pDrvCtrl->flags & EX_XPENDING) == 0
        && pDrvCtrl->idr.ac_if.if_snd.ifq_head != NULL)
        {
        IF_DEQUEUE(&pDrvCtrl->idr.ac_if.if_snd, pMbuf);

        /* copy packet to write buffer */

        copy_from_mbufs (pDrvCtrl->pWriteBuf, pMbuf, len);

        /* Adjust length to ensure minimum packet size */

        len = max (ETHERSMALL, len);

        /* Place a transmit request. */

#if (CPU_FAMILY == SPARC)
        {
        ULONG blockAddr;

        pMsg = exMsgBlkGet (pDrvCtrl);
        pMsg->mb_rqst                 = LLRTRANSMIT;
        pMsg->mb_et.et_nblock         = 1;
        pMsg->mb_et.et_blks[0].bb_len = (u_short)len;

        blockAddr = (ULONG) exHostAdrs ((char *) pDrvCtrl->pWriteBuf);
        pMsg->mb_et.et_blks[0].bb_addr[0] = blockAddr >> 16;
        pMsg->mb_et.et_blks[0].bb_addr[1] = blockAddr & 0xffff;
        }
#else   /* CPU_FAMILY == SPARC */
        pMsg = exMsgBlkGet (pDrvCtrl);
        pMsg->mb_rqst                 = LLRTRANSMIT;
        pMsg->mb_et.et_nblock         = 1;
        pMsg->mb_et.et_blks[0].bb_len = (u_short)len;
        *(char **)pMsg->mb_et.et_blks[0].bb_addr =
                     exHostAdrs ((char *) pDrvCtrl->pWriteBuf);
#endif  /* CPU_FAMILY == SPARC */

        pDrvCtrl->flags |= EX_XPENDING;    /* indicate that xmit in progress */

        exGiveRequest (pMsg, pDrvCtrl->pDev);
#ifndef BSD43_DRIVER    /* BSD 4.4 ether_output() doesn't bump statistic. */
        pDrvCtrl->idr.ac_if.if_opackets++;
#endif
        }

    splx (s);
    }

/*******************************************************************************
*
* exIntr - interrupt handler
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*/

LOCAL void exIntr
    (
    int unit
    )
    {
    DRV_CTRL        *pDrvCtrl;
    EX_MSG          *pMsg;

    pDrvCtrl = & drvCtrl [unit];
    pMsg = pDrvCtrl->pX2HNext;

    sysBusIntAck (pDrvCtrl->ilevel);  /* acknowledge interrupt */

    while ((pMsg->mb_status & MH_OWNER) == MH_HOST)
        {
        switch (pMsg->mb_rqst)
            {
            case LLRECEIVE:
                (void) netJobAdd    (
                                    (FUNCPTR) exRecvAndHang,
                                    unit,
                                    pMsg->mb_er.er_blks[0].bb_len,
                                    pMsg->mb_rply,
                                    0,
                                    0
                                    );
                break;

            case LLRTRANSMIT:
#ifdef EX_DEBUG
                if ((pDrvCtrl->flags & EX_XPENDING) == 0)
                    panic ("exxmit: no xmit pending");
#endif  /* EX_DEBUG */
                pDrvCtrl->flags &= ~EX_XPENDING;

                if (pMsg->mb_rply == LL_OK)
                    ;
                else if (pMsg->mb_rply & LLXM_1RTRY)
                    {
                    pDrvCtrl->idr.ac_if.if_collisions++;
                    }
                else if (pMsg->mb_rply & LLXM_RTRYS)
                    {
                    pDrvCtrl->idr.ac_if.if_collisions += 2;       /* guess */
                    }
                else if (pMsg->mb_rply & LLXM_ERROR)
                    {
                    pDrvCtrl->idr.ac_if.if_oerrors++;
                    pDrvCtrl->idr.ac_if.if_opackets--;
#ifdef EX_DEBUG
                    logMsg ("ex%d: transmit error=%#x\n", unit, pMsg->mb_rply,
                            0, 0, 0, 0);
#endif
                    }

                if (pDrvCtrl->idr.ac_if.if_snd.ifq_head != NULL)
#ifdef BSD43_DRIVER
                    (void)netJobAdd ( (FUNCPTR)exStart, unit, 0, 0, 0, 0);
#else
                    (void)netJobAdd ( (FUNCPTR)exStart, (int)pDrvCtrl, 
                                      0, 0, 0, 0);
#endif
                break;

            case LLNET_STSTCS:
                pDrvCtrl->idr.ac_if.if_ierrors = pDrvCtrl->pStatArray->sa_crc;
                pDrvCtrl->flags &= ~EX_STATPENDING;
                break;

            case LLNET_ADDRS:

                /* Copy the enet addr from the message to our IDR */

                bcopy   (
                        (caddr_t) pMsg->mb_na.na_addrs,
                        (caddr_t) pDrvCtrl->idr.ac_enaddr,
                        sizeof (pDrvCtrl->idr.ac_enaddr)
                        );
                break;

            case LLNET_RECV:
            case LLNET_MODE:
                break;
            }

        if (sysBus == MULTI_BUS)
            pDrvCtrl->pDev->bus.multi.xd_porta = 7;  /* ack the interrupt */

        pMsg->mb_length = MBDATALEN;

        exGiveRequest (pMsg, pDrvCtrl->pDev);

        pMsg = pDrvCtrl->pX2HNext = pDrvCtrl->pX2HNext->mb_next;
        }
    }

/*******************************************************************************
*
* exMsgBlkGet - get a request buffer
*
* Fill in standard values, advance pointer.
*/

LOCAL EX_MSG *exMsgBlkGet
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    EX_MSG *pMsg;

    pMsg = pDrvCtrl->pH2XNext;

#ifdef EX_DEBUG
    if ((pMsg->mb_status & MH_OWNER) == MH_EXOS)
        panic ("ex: EXOS owns message buffer");
#endif  /* EX_DEBUG */

    pMsg->mb_1rsrv  = 0;
    pMsg->mb_length = MBDATALEN;

    pDrvCtrl->pH2XNext = pDrvCtrl->pH2XNext->mb_next;

    return (pMsg);
    }

/*******************************************************************************
*
* exRecvAndHang - receive packet and post another receive request
*/

LOCAL void exRecvAndHang
    (
    int unit,
    int len,
    int status
    )
    {
    exRecv (unit, len, status);
    exRxRqst (unit);
    }

/*******************************************************************************
*
* exRecv - process Ethernet receive completion
*
* If input error just drop packet.
* Otherwise purge input buffered data path and examine
* packet to determine type.  If can't determine length
* from type, then have to drop packet.  Otherwise decapsulate
* packet based on type and pass to type-specific higher-level
* input routine.
*/

LOCAL void exRecv
    (
    int unit,
    int len,
    int status
    )
    {
    DRV_CTRL            *pDrvCtrl;
    struct ether_header *eh;
    MBUF                *pMbuf;
    u_char              *pData;


    pDrvCtrl = & drvCtrl [unit];

    if (status != LL_OK)
        {
        pDrvCtrl->idr.ac_if.if_ierrors++;
#ifdef EX_DEBUG
        printErr ("ex%d: receive error=%b\n", unit, status, RECV_BITS);
#endif
        return;
        }

    /* get input packet length and ptr to packet */

    len -= 4;                                       /* subtract FCS */
    eh = (struct ether_header *)(pDrvCtrl->pReadBuf);

    pDrvCtrl->idr.ac_if.if_ipackets++;

    /* call input hook if any */

    if ( etherInputHookRtn != NULL )
        {
        if ( (* etherInputHookRtn)(&pDrvCtrl->idr.ac_if, (char *) eh, len) )
            return; /* input hook has already processed this packet */
        }

    /* Subtract header size from length */

    len -= SIZEOF_ETHERHEADER;

    /* Get pointer to packet data */

    pData = ((u_char *) eh) + (SIZEOF_ETHERHEADER);

    pMbuf = copy_to_mbufs (pData, len, 0, & pDrvCtrl->idr.ac_if);

    if (pMbuf == NULL)                  /* no room at the inn */
        {
        pDrvCtrl->idr.ac_if.if_ierrors++;
        return;
        }

#ifdef BSD43_DRIVER
    do_protocol_with_type (ntohs(eh->ether_type), pMbuf, &pDrvCtrl->idr, len);
#else
    do_protocol (eh, pMbuf, &pDrvCtrl->idr, len);
#endif
    }

/*******************************************************************************
*
* exRxRqst - send receive request to EXOS
*
* Called by exIntr, with interrupts disabled in both cases.
*/

LOCAL void exRxRqst
    (
    int unit
    )
    {
    DRV_CTRL        *pDrvCtrl;
    EX_MSG          *pMsg;


    pDrvCtrl = & drvCtrl [unit];
    pMsg = exMsgBlkGet(pDrvCtrl);

    pMsg->mb_rqst                  = LLRECEIVE;
    pMsg->mb_er.er_nblock          = 1;
    pMsg->mb_er.er_blks[0].bb_len  = EXMAXRBUF;
#if     (CPU_FAMILY == SPARC)
    {
    ULONG blockAddr;

    blockAddr = (ULONG) exHostAdrs ((char *) pDrvCtrl->pReadBuf);
    pMsg->mb_er.er_blks[0].bb_addr[0] = blockAddr >> 16;
    pMsg->mb_er.er_blks[0].bb_addr[1] = blockAddr & 0xffff;
    }
#else
    *(char **)pMsg->mb_er.er_blks[0].bb_addr =
                    exHostAdrs ((char *) pDrvCtrl->pReadBuf);
#endif  /* CPU_FAMILY == SPARC */

    exGiveRequest (pMsg, pDrvCtrl->pDev);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* exOutput - the output routine
*/

LOCAL int exOutput
    (
    IDR *pIDR,
    MBUF *pMbuf,
    SOCK *pDest
    )
    {
    return (ether_output ((IFNET*)pIDR, pMbuf, pDest, (FUNCPTR) exStart, pIDR));
    }
#endif

/*******************************************************************************
*
* exIoctl - process an ioctl request
*/

LOCAL int exIoctl
    (
    IDR *pIDR,
    int cmd,
    caddr_t data
    )
    {
    int ret;

    ret = 0;

    switch (cmd)
        {
        case SIOCSIFADDR:
           ((struct arpcom *)pIDR)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (pIDR, &IA_SIN (data)->sin_addr);
            break;

        case SIOCSIFFLAGS:
            /* Flags are set outside this module. No additional work to do. */
            break;

        default:
            ret = EINVAL;
        }

    return (ret);
    }

/*******************************************************************************
*
* exHostAdrs - get host address as required by EXOS board
*
* This routine converts the specified local address into the form
* that the EXOS board wants to see it, namely converted into
* the proper vme address space and, in the VME version, with the
* address modifier in the upper byte.
*
* RETURNS: proper EXOS access address or ERROR.
*/

LOCAL char *exHostAdrs
    (
    char *localAdrs
    )
    {
    char *busAdrs;

    if (sysLocalToBusAdrs (exBusAdrsSpace, localAdrs, &busAdrs) != OK)
        {
#ifdef EX_DEBUG
        printErr ("ex: no bus access to local address 0x%x (AM=0x%x)\n",
                  localAdrs, exBusAdrsSpace);
#endif
        return ((char*)ERROR);
        }

    if (sysBus == VME_BUS && !exBit32)
        {
        /* EXOS 202 (VME version) requires AM code in upper byte */
        busAdrs = (char *) ((int) busAdrs | (exBusAdrsSpace << 24));
        }

    return (busAdrs);
    }

/*******************************************************************************
*
* exGiveRequest - give request buffer to EXOS
*/

LOCAL void exGiveRequest
    (
    EX_MSG      *pMsg,            /* pointer to msg buffer to give to EXOS */
    DEV_CTRL    *pDev             /* device address */
    )
    {
    pMsg->mb_status |= MH_EXOS;   /* give request to EXOS */

    if (sysBus == VME_BUS)
        pDev->bus.vme.xd_portb = EX_NTRUPT;
    else
        pDev->bus.multi.xd_portb = EX_NTRUPT;
    }

/* END OF FILE */
