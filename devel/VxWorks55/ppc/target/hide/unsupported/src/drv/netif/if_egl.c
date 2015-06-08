/* if_egl.c - Interphase Eagle 4207 Ethernet network interface driver */

/* Copyright 1984-1997 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
02l,15jul97,spm  removed calls to init routine from ioctl support (SPR #8831)
02k,19may97,spm  eliminated all compiler warnings
02j,15may97,spm  reverted to bcopy routines for mbufs in BSD 4.4
02i,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
02h,23apr93,caf  ansification: added cast to cacheInvalidate parameter.
02g,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
02f,15jan93,rfs  Non-functional.  Comments and documentation changes.
02e,09sep92,gae  documentation tweaks.
02d,18jul92,smb  Changed errno.h to errnoLib.h.
02c,08jul92,ajm  made cache safe
02b,26may92,rrr  the tree shuffle
                 -changed includes to have absolute path from h/
02a,16jan92,gae  major cleanup: more ANSI fixes, eliminated occassional fatal
                 bootrom crashes (reports only 1 in 100 receive errors),
                 nearly made work on 68k, eliminated bit fields.
01c,07oct91,rrr  some fixes.
01b,04oct91,rrr  passed through the ansification filter
                 -changed functions to ansi style
                 -changed includes to have absolute path from h/
                 -fixed #else and #endif
                 -changed TINY and UTINY to INT8 and UINT8
                 -changed READ, WRITE and UPDATE to O_RDONLY O_WRONLY & O_RDWR
                 -changed VOID to void
                 -changed copyright notice
01a,14nov90,ajm   written for vxWorks using Unix driver as a base.
*/


/*
DESCRIPTION

GENERAL INFORMATION

This module implements the Interphase Eagle 4207 Ethernet network
interface driver.

This driver was created to support the MIPS-based BSPs that are supported
by WRS.  It is therefore not considered generic.  It has not been verified
to work with other BSPs, and hence is not supported for all targets.


BOARD LAYOUT

The diagram below shows the relevant jumpers for VxWorks configuration.
Default values: I/O address 0x00004000, Short Addressing (A16,D16).
.bS
______________________________              _______________________________
|             P1             |  Eagle 4207  |             P2              |
|                            ----------------                             |
|               SW1 SW2 SW3                 JA7 -.                        |
|               "   "   "                                                 |
|               -   "   "                                                 |
|               -   "   "                                                 |
|               -   -   "                                                 |
|               -   "   "                                                 |
|               -   -   "                                                 |
|               -   -   "                                                 |
|               -   "   "                                                 |
|       Short I/O Adr                                                     |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|                                                                         |
|_________________________________________________________________________|
.bE


EXTERNAL INTERFACE

This driver provides the standard external interface.

There is one user-callable routine: eglattach().  See the manual entry for this
routine for usage details.

SEE ALSO: ifLib
*/

/*
Copyright (C) 1987,1988 Interphase Corporation
Any use, copy or alteration is strictly prohibited
unless authorized by Interphase.

Porting priveledge granted to Wind River Systems on Jan. 9, 1992 by:
Rob Brant - Interphase FAE, 214-919-9110 - rbrant@iphase.com
*/

#include "vxWorks.h"
#include "cacheLib.h"
#include "net/mbuf.h"
#include "net/protosw.h"
#include "sys/ioctl.h"
#include "sys/socket.h"
#include "errnoLib.h"
#include "net/if.h"
#include "net/route.h"
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/ip.h"
#include "netinet/in_var.h"
#include "netinet/if_ether.h"

#include "iv.h"
#include "vme.h"
#include "stdlib.h"
#include "memLib.h"
#include "logLib.h"
#include "iosLib.h"
#include "intLib.h"
#include "ioLib.h"
#include "vxLib.h"
#include "stdio.h"
#include "net/if_subr.h"
#include "netLib.h"
#include "sysLib.h"
#include "drv/netif/if_egl.h"


/* Externals */

IMPORT int      sysClkRateGet ();
IMPORT int      sysBusIntAck ();
IMPORT ULONG    tickGet ();
IMPORT STATUS   taskDelay ();

IMPORT FUNCPTR  etherInputHookRtn;
IMPORT FUNCPTR  etherOutputHookRtn;

#define NEGL    4       /* Maximum number of Eagle units supported */

#define BLOCKMODE       TRUE    /* allow block mode transmisions */
#define NOINT           0       /* don't want interrupt */
#define INT             1       /* want interrupt */

#define EGL_TOPT        (TT_NORMAL | MEMT_32BIT | VME_AM_EXT_USR_DATA)
#define EGL_TOPT_BLK    (TT_BLOCK  | MEMT_32BIT | VME_AM_EXT_USR_ASCENDING)

#define EGL_MTU         (ETHERMTU+18)   /* Max Packet size, 1500+14+4 */
#define NUM_CQE         MAX_CQE         /* Num Command Queue Entries */


/* Next two MUST be power of 2 */

#define EGL_RX_NRINGS    16              /* Num LANCE Rx Rings */
#define EGL_TX_NRINGS   16              /* Num LANCE Tx Rings */
#define EGL_RX_BUFSZ    2000            /* Max Rx packet */
#define EGL_TX_BUFSZ    2000            /* Max Tx packet */
#define EGL_RX_NBUFF    20              /* Num receive buffers (max possible) */
#define EGL_RX_SLOTS    20              /* Num receive workq entries */
#define EGL_TX_NBUFF    16              /* Num transmit buffers */
#define EGL_TX_SLOTS    16              /* Num transmit workq entries */
#define EGL_RX_IOPBF    0               /* Receive IOPB starting offset */
#define EGL_TX_IOPBF    (EGL_RX_IOPBF+EGL_RX_NBUFF)
#define EGL_MS_SLOTS    1               /* Num misc workq entries */
#define EGL_DM_SLOTS    1               /* Num DMA workq entries */
#define EGL_RX_PRIORITY 1               /* Receive workq priority */
#define EGL_TX_PRIORITY 1               /* Transmit workq priority */
#define EGL_MS_PRIORITY 1               /* misc workq priority */
#define EGL_DM_PRIORITY 1               /* DMA workq priority */
#define EGL_RX_WDIV     0               /* Num receive workq processed */
#define EGL_TX_WDIV     0               /* Num transmit workq processed */
#define EGL_MS_WDIV     0               /* Num misc workq processed */
#define EGL_DM_WDIV     0               /* Num DMA workq processed */
#define EGL_NHBUF       1               /* Num Host Managed Buffers */
#define EGL_NIBUF       16              /* Num Internal Tx Buffers */
#define EGL_BURST       0               /* Send Whole Message */

#define TXINC(x) ((x) = (ULONG)(++x&(EGL_TX_NBUFF-1)))
#define TXDEC(x) ((x) = (ULONG)(--x&(EGL_TX_NBUFF-1)))
#define TXFULL(x) ((x) == EGL_TX_NBUFF)

typedef struct eth_buf
    {
    struct      eth_buf *eth_next;      /* next buffer in chain */
    u_char      eth_dat[EGL_MTU];       /* data storage */
    ULONG       eth_dat_adr;            /* bus address of data */
    int         eth_len;                /* actual data usage */
    IOPB        *iopb;                  /* associated iopb */
    } ETH_BUF;

#define ETH_BUF_SIZE    sizeof(ETH_BUF)
#define ETH_BUF_COUNT   (EGL_RX_NRINGS + EGL_TX_NRINGS)

ETH_BUF *eth_free = NULL;

#define GET_ETH_BUF(buf) \
        { int ms = splimp ();                   \
          if (((buf)=eth_free) != NULL)         \
          {                                     \
                eth_free = (buf)->eth_next;     \
                (buf)->eth_next = NULL;         \
          }                                     \
          else                                  \
                panic("out of ETH buffers");    \
          splx (ms);                            \
        }

#define REL_ETH_BUF(buf) \
        { int ms = splimp ();                   \
          (buf)->eth_next = eth_free;           \
          eth_free = (buf);                     \
          splx (ms);                            \
          (buf)->eth_len = 0;                   \
        }

/*
* Invalidate the CPU cache since the start and end of an mbuf may occupy
* the same secondary cache line as other data.  Even if we flush and
* invalidate before performing I/O, other data in the line may have been
* read, causing the old data to be placed back in the cache.  We then will
* see stale data after the I/O completes (this occurred in an SA system
* where the mbuf header is in the same secondary cache line as the data --
* and we end up reading mbuf->m_len after invalidating the CPU cache).
*/

#define egl_ioflush(eth_buf) \
    { \
    ULONG physical = K1_TO_PHYS(((ULONG)&eth_buf->eth_dat[0])); \
    cacheInvalidate (DATA_CACHE, (void *) PHYS_TO_K0(physical), \
                     eth_buf->eth_len); \
    }

int eglAM       = VME_AM_EXT_USR_DATA;  /* 32-bit addresses */
int eglMemWidth = 4;                    /* VME bcopy transfers = 1,2,4 */

int delay_mult = 13;    /* tuned on the STAR MVP (VLANE = 1) */
/*
 * DELAY(n) should be n microseconds, roughly.
 * [taskDelay(1) at 60Mhz == 16666 usecs.]
 */
#define DELAY(n)        { \
        register int ix = delay_mult*(n); \
        while (--ix > 0); \
        }

/*
 * Ethernet software status per interface.
 *
 * Each interface is referenced by a network interface structure, egl_if,
 * which the routing code uses to locate the interface.  This structure
 * contains the output queue for the interface, its address, ...
 */

typedef struct egl_softc {
    struct arpcom egl_ac;       /* Ethernet Common Part */
#define egl_if          egl_ac.ac_if     /* network-visible interface */
#define egl_enaddr      egl_ac.ac_enaddr /* hardware Ethernet address */
    SHIO        *eglAddr;       /* Pointer to I/O Space */
    USHORT      xmit_vec;       /* Transmit Interrupt Vector */
    USHORT      xmiterr_vec;    /* Transmit Error Interrupt Vector*/
    USHORT      rcv_vec;        /* Receive Interrupt Vector */
    USHORT      rcverr_vec;     /* Receive Error Interrupt Vector*/
    USHORT      qav_vec;        /* Queue Available Interrupt Vector */
    ETH_BUF     *egl_tbinfo[EGL_TX_NBUFF]; /* phys addrs of txbufs */
    ETH_BUF     *egl_rbinfo[EGL_RX_NBUFF]; /* phys addrs of rxbufs */
    ULONG       egl_txadd;      /* add index to xmit packet ring */
    ULONG       egl_txrem;      /* remove index to xmit packet ring */
    CQE         *egl_cqe;       /* index for feed queue */
    ULONG       egl_qmode;      /* queue mode is started */
    ULONG       egl_qfull;      /* feed queue is full */
    ULONG       egl_txcnt;      /* # of mbufs queued in tbuf array */
    ULONG       egl_quecnt;     /* # of transmit commands queued on Eagle */
    ULONG       egl_csr0miss;   /* driver count of receive misses */
    ULONG       egl_csr0mem;    /* driver count of LANCE memory errors */
    CSTB        egl_cstb;       /* Firmware info */
    int         eglIntLevel;    /* VME interrupt level */
    int         eglIntVec;      /* VME interrupt vector */
    CSB         CSBstats;       /* copy of CSB for tracking statistics */
} EGL_SOFTC;

LOCAL EGL_SOFTC *egl_softc [NEGL];

#if     CPU_FAMILY!=MIPS
#define sysWbFlush()
#define K0_TO_PHYS(x)   (x)
#define K1_TO_PHYS(x)   (x)
#define  PHYS_TO_K0(x)   (x)
#define PHYS_TO_K1(x)   (x)
#endif  /* CPU_FAMILY!=MIPS */

/*
 * Macros to set various bits in Queue Entry Control Register (QECR)
 * of the Command Queue Entry (CQE).
 */

/* Tell Eagle to initiate action on this cqe */
#define CQE_GO(qecr)            { sysWbFlush(); qecr = M_QECR_GO; }

/* Check if cqe is available */
#define CQE_BUSY(qecr)  (qecr & M_QECR_GO)

/* Clear cqe GO bit; when should host be doing this??? */
#define CQE_CLR_BUSY(qecr)      { sysWbFlush(); qecr &= ~M_QECR_GO; }

/* Indicate that any commands that are queued up should be aborted on error */
#define CQE_AA_GO(qecr) { sysWbFlush(); qecr |= (M_QECR_GO + M_QECR_AA);}

/* Get the next command queue entry in the command queue XXX bad macro */
#define INC_CQE(cqe) ((x)==&shio->sh_CQE[NUM_CQE-1] ? &shio->sh_CQE[0] ? cqe+1)

/*
 * Macros to check/set bits in the Command Response Status (CRSW) field of the
 * Command Response Block (CRB).
 */

/* clear the crsw */
#define CRB_CLR_DONE(crsw)       { crsw = 0; sysWbFlush(); }
/*#define CRB_CLR_DONE(crsw)     { crsw &= ~M_CRSW_CRBV; sysWbFlush();}*/

/* Check if command is done */
#define CRB_DONE(crsw)          (crsw & M_CRSW_CRBV)

/* Check is the command represented by this CRB is done. */
#define CRB_COMPLETE(crsw)       (crsw & (M_CRSW_CRBV | M_CRSW_CC))

/* Check if the command to start queue mode has completed. */
#define CRB_QSTARTED(crsw)      (crsw & (M_CRSW_CRBV | M_CRSW_QMS))

/* Check if this command queue entry is available. */
#define CRB_QAVAIL(crsw)        (crsw & (M_CRSW_CRBV | M_CRSW_QEA))

#define CRB_ERROR(crsw)         (crsw & (M_CRSW_ER | M_CRSW_EX))

/* Define to display error/status messages. */

#undef EGL_DEBUG


/* forward declarations */

int eglattach ();
int eglShow (int unit, BOOL zap);

static int  egl_config (int unit, int itype);
static int  egl_hangrcv (int unit, int index);
static int  egl_init (int unit);
static void egl_intr (int unit);
static int  egl_ioctl (struct ifnet *ifp, int cmd, caddr_t data);
static int  egl_iworkq (int unit, int itype);
#ifdef BSD43_DRIVER
static int  egl_output (struct ifnet *ifp,struct mbuf *m0,struct sockaddr *dst);
#endif
static int  egl_physreset (int unit);
static void egl_qint (int unit, EGL_SOFTC *egl);
static void egl_recv (int unit, int len, ETH_BUF *eth_buf);
static int  egl_reset (int unit);
static void egl_rint (int unit, EGL_SOFTC *egl, int index, int len);
static void egl_setiopbs (int unit);
#ifdef BSD43_DRIVER
static void egl_start (int unit);
#else
static void egl_start (EGL_SOFTC *egl);
#endif
static void egl_tint (int unit, EGL_SOFTC *egl, int index);
static int  egl_wait (volatile CRB *crb);


/*******************************************************************************
*
* eglattach - publish the interface, and initialize the driver and device
*
* This routine attaches an `egl' Ethernet interface to the network if the
* interface exists.  The routine makes the interface available by filling in
* the network interface record.  The system will initialize the interface
* when it is ready to accept packets.
*
* RETURNS: OK or ERROR.
*/

int eglattach
    (
    int unit,           /* unit number */
    char *addr,         /* address of Eagle's shared memory */
    int ivec,           /* interrupt vector to connect to */
    int ilevel          /* interrupt level */
    )
    {
    EGL_SOFTC *egl;
    ETH_BUF *eth_buf;
    int ix;
    USHORT status;
    char *pool;
    volatile SHIO *pEglDev = (SHIO *) addr;

#ifdef EGL_DEBUG
    printf ("egl%d: attaching at vector = %d, level = %d, addr = %#x\n",
             unit, ivec, ilevel, addr);
#endif  /* EGL_DEBUG */

    /* wait for kernel reset to finish */

    ix = 15 * sysClkRateGet () / 2;     /* 15 seconds (1/30th sec. below) */

    while (vxMemProbe ((char *) &pEglDev->sh_MCSB, O_RDONLY,
                        sizeof (USHORT), (char *)&status) != OK)
        {
        if (--ix < 0)
            {
            errnoSet (S_iosLib_CONTROLLER_NOT_PRESENT);
#ifdef EGL_DEBUG
            printf (
            "eglattach: controller not present at %#x, status register = %#x\n",
                    addr, status);
#endif
            return (ERROR);
            }

        taskDelay (sysClkRateGet () / 30);      /* 1/30th of a second */
        }

    pool = (char*)calloc (1, (ETH_BUF_COUNT + 1) * ETH_BUF_SIZE);

    if (pool == NULL)
        return (ERROR);

    pool = (char *)((int)pool+ETH_BUF_SIZE - ((int)pool & (ETH_BUF_SIZE - 1)));

    eth_buf = (ETH_BUF *)pool;

    for (ix = 0; ix < ETH_BUF_COUNT; ix++)
        {
        REL_ETH_BUF(eth_buf);
        if (sysLocalToBusAdrs (eglAM, &eth_buf->eth_dat[0],
                               (char **)&eth_buf->eth_dat_adr) == ERROR)
            {
#ifdef EGL_DEBUG
            printf ("egl: local address %#x to bus address (AM = %#x) failed\n",
                    &eth_buf->eth_dat[0], eglAM);
#endif
            return (ERROR);
            }
        eth_buf++;
        }

    /* allocate and initialize Eagle descriptor */

    egl = (EGL_SOFTC *) calloc (1, sizeof (EGL_SOFTC));

    if (egl == NULL)
        return (ERROR);

    egl_softc [unit] = egl;             /* set ptr to egl descriptor in array */

    egl->eglAddr = (SHIO *) addr;               /* remember device address */

    /* reset the device */

    if (egl_physreset (unit) != 0)
        {
#ifdef EGL_DEBUG
        printf ("egl%d: reset failure.\n", unit);
#endif
        errnoSet (S_ioLib_DEVICE_ERROR);
        return (ERROR);
        }

    bcopy ((char*)&pEglDev->sh_CSTB, (char*)&egl->egl_cstb, sizeof(CSTB));

    bzero ((char*)egl->egl_cstb.cstb_FILT, sizeof(egl->egl_cstb.cstb_FILT));

    /* configure the settings */

    if (egl_config (unit, NOINT) == -1)
        {
#ifdef EGL_DEBUG
        printf ("egl%d: configuration failure.\n", unit);
#endif
        errnoSet (S_ioLib_DEVICE_ERROR);
        return (ERROR);
        }

    egl->xmit_vec    = LVL_VCT(ilevel,ivec+0);
    egl->xmiterr_vec = LVL_VCT(ilevel,ivec+1);
    egl->rcv_vec     = LVL_VCT(ilevel,ivec+2);
    egl->rcverr_vec  = LVL_VCT(ilevel,ivec+3);
    egl->qav_vec     = LVL_VCT(ilevel,ivec+4);

    bcopy ((char*)egl->egl_cstb.cstb_PHY, (char*)egl->egl_enaddr,
          sizeof(egl->egl_cstb.cstb_PHY));

    egl->eglIntLevel = ilevel;
    egl->eglIntVec   = ivec;

#ifdef BSD43_DRIVER
    ether_attach (&egl->egl_if, unit, "egl", egl_init, egl_ioctl,
                    egl_output, egl_reset);
#else
    ether_attach (
                 &egl->egl_if, 
                 unit, 
                 "egl", 
                 egl_init, 
                 egl_ioctl,
                 ether_output, 
                 egl_reset);
    egl->egl_if.if_start = (FUNCPTR)egl_start;
#endif

    egl->egl_if.if_mtu = ETHERMTU - 4;  /* MTU = 1496 for block bug */

    (void)intConnect (INUM_TO_IVEC (ivec+0), egl_intr, unit);

    /* XXX temporarily while R3K targets don't do vectored VME interrupts */

#if     CPU_FAMILY!=MIPS
    (void)intConnect (INUM_TO_IVEC (ivec+1), egl_intr, unit);
    (void)intConnect (INUM_TO_IVEC (ivec+2), egl_intr, unit);
    (void)intConnect (INUM_TO_IVEC (ivec+3), egl_intr, unit);
    (void)intConnect (INUM_TO_IVEC (ivec+4), egl_intr, unit);
#endif  /* CPU_FAMILY!=MIPS */

    sysIntEnable (ilevel);

    if (egl_init (unit) != 0)
        return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* egl_config - configure the Eagle to default settings
*
* RETURNS: 0 or -1 on error.
*/

LOCAL int egl_config
    (
    int unit,
    int itype
    )
    {
    EGL_SOFTC *egl         = egl_softc [unit];
    volatile SHIO *shio    = egl->eglAddr;
    volatile CIB *cib      = (CIB *)&shio->sh_SCRTCH[0]; /* scratch area */
    volatile IC_IOPB *iopb = (IC_IOPB *)&shio->sh_MCE_IOPB;
    int status             = 0;
    int ix;

    /* Controller Initialization Block */

    cib->cib_RES0 = 0;
    cib->cib_NCQE = NUM_CQE;

    cib->cib_IMOD = (USHORT)M_IMOD_PE;  /* Ethernet */
    cib->cib_NTXR = EGL_TX_NRINGS;      /* LANCE Tx Rings */
    cib->cib_NRXR = EGL_RX_NRINGS;      /* LANCE Rx Rings */

    /* default Physical Address */
    bcopy ((char*)egl->egl_cstb.cstb_PHY, (char*)cib->cib_PHY,
          sizeof(egl->egl_cstb.cstb_PHY));

    /* default Logical Address Filter */
    bcopy ((char*)egl->egl_cstb.cstb_FILT, (char*)cib->cib_FILT,
          sizeof (egl->egl_cstb.cstb_FILT));

    cib->cib_RXSIZ = EGL_RX_BUFSZ;      /* Max Rx Packet */
    cib->cib_NRBUF = EGL_RX_NRINGS;     /* Num Rx Buffers */
    cib->cib_TXSIZ = EGL_TX_BUFSZ;      /* Max Tx Packet */
    cib->cib_NIBUF = EGL_TX_NRINGS;     /* Num Tx Buffers */
    cib->cib_NHBUF = EGL_NHBUF;         /* Num Host Buffers */
    cib->cib_NVECT = egl->qav_vec;      /* needed for error handling */
    cib->cib_EVECT = egl->qav_vec;      /* needed for error handling */
    cib->cib_BURST = EGL_BURST;         /* DMA Burst Count */

    bzero ((char*)cib->cib_RES1, sizeof(cib->cib_RES1));

    /* MCE IOPB */

    iopb->ic_iopb_CMD    = CNTR_INIT;

    /* XXX big-endian or quad alignment? */

    iopb->ic_iopb_OPTION = M_OPT_SG | itype;     /* 0 no int; 1 int */

    iopb->ic_iopb_NVCT   = egl->qav_vec;
    iopb->ic_iopb_EVCT   = egl->qav_vec;
    iopb->ic_iopb_RES0   = 0;
    iopb->ic_iopb_BUFF   = (ULONG)cib - (ULONG)shio;/* CIB offset from shio */

    bzero ((char*)iopb->ic_iopb_RES1, sizeof(iopb->ic_iopb_RES1));

    shio->sh_MCE.cqe_CTAG       = 0;
    shio->sh_MCE.cqe_IOPB_ADDR  = O_MCE_IOPB;
    shio->sh_MCE.cqe_WORK_QUEUE = 0;

    CQE_GO(shio->sh_MCE.cqe_QECR);

    if (!itype)
        {
        ix = egl_wait (&shio->sh_CRB);

        if (CRB_ERROR(ix))
            {
#ifdef EGL_DEBUG
            logMsg ("egl%d: *error* unable to configure interface\n",
                    unit, 0, 0, 0, 0, 0);

            if (iopb->ic_iopb_STATUS)
                logMsg ("iopb error status = %#x\n",
                        iopb->ic_iopb_STATUS, 0, 0, 0, 0, 0);
#endif
            status = -1;
            }
        else if (ix == 0x1000)
            {
#ifdef EGL_DEBUG
            logMsg ("egl%d: *timeout* unable to configure interface\n",
                    unit, 0, 0, 0, 0, 0);
#endif
            status = -1;
            }
        }

    return (status);
    }
/*******************************************************************************
*
* egl_hangrcv - place a receive request for the specified iobp
*
* RETURNS: 1 or 0.
*/

LOCAL int egl_hangrcv
    (
    int unit,
    int index
    )
    {
    ETH_BUF *eth_buf;
    volatile IOPB *iopb;
    EGL_SOFTC *egl      = egl_softc [unit];
    volatile SHIO *shio = egl->eglAddr;
    volatile CQE *cqe   = egl->egl_cqe;

    if (egl->egl_qfull != 0)
        {
#ifdef EGL_DEBUG
        logMsg ("egl%d: egl_hangrcv - packet queue full\n", unit, 0, 0, 0, 0,0);
#endif  /* EGL_DEBUG */
        return (0);
        }

    /* place a receive request */

    if (CQE_BUSY(cqe->cqe_QECR))
        {
        egl->egl_qfull++;       /* feed queue full */
        return (0);
        }

    if ((eth_buf = egl->egl_rbinfo[index]) == NULL)
        GET_ETH_BUF(eth_buf);

    iopb                   = eth_buf->iopb;
    iopb->iopb_BUFF        = eth_buf->eth_dat_adr;

    eth_buf->eth_len       = EGL_MTU;
    egl->egl_rbinfo[index] = eth_buf;
    egl_ioflush (eth_buf);

    cqe->cqe_CTAG          = index;
    cqe->cqe_IOPB_ADDR     = (caddr_t)iopb - (caddr_t)shio;
    cqe->cqe_WORK_QUEUE    = EGL_RECVQ;

    CQE_GO(cqe->cqe_QECR);

    if (++egl->egl_cqe == &shio->sh_CQE[MAX_CQE])
        egl->egl_cqe = (CQE *)&shio->sh_CQE[0];

    return (1);
    }
/*******************************************************************************
*
* egl_init - initialization of interface; clear pending operations
*
* RETURNS: 0 or -1.
*/

LOCAL int egl_init
    (
    int unit
    )
    {
    EGL_SOFTC *egl      = egl_softc [unit];
    volatile SHIO *shio = egl->eglAddr;
    volatile CRB *crb   = &shio->sh_CRB;
    struct ifnet *ifp   = &egl->egl_if;
    int s;
    int ix;

    ifp->if_flags &= ~(IFF_UP|IFF_RUNNING);

    s = splimp ();

    if (egl_reset (unit) == -1)
        {
        splx (s);
        return (-1);
        }

    /* LANCE is turned OFF now */

    if (egl_config (unit, NOINT) == -1)
        {
        splx (s);
        return (-1);
        }

    /* LANCE is turned ON now */

    egl_setiopbs (unit);                        /* setup iopbs */
    egl->egl_qmode = 0;                         /* not queue mode */

    /* start up queue mode */

    if (egl_iworkq (unit, NOINT) == -1)
        {
        splx (s);
#ifdef EGL_DEBUG
        logMsg ("egl%d: egl_init - failed\n", 0, 0, 0, 0, 0, 0);
#endif
        return (-1);
        }

    for (ix = 0; ix < 10000; ix++)
        {
        if (CRB_QSTARTED(crb->crb_CRSW))
            break;
        DELAY(500);
        }

    if (!CRB_QSTARTED(crb->crb_CRSW))
        {
#ifdef EGL_DEBUG
        logMsg ("egl%d: failed to start Q-Mode\n", unit, 0, 0, 0, 0, 0);
#endif
        splx (s);
        return (-1);
        }

    egl->egl_qmode++;

    CRB_CLR_DONE(crb->crb_CRSW);        /* release Eagle */

    /* initial hang receive requests */

    for (ix = 0; ix < EGL_RX_NBUFF; ix++)
        {
        if (egl_hangrcv (unit, ix) == 0)
            break;
        }

    egl->egl_if.if_flags |= IFF_UP | IFF_RUNNING;

#ifdef BSD43_DRIVER
    egl_start (unit);                   /* start transmits */
#else
    egl_start (egl);
#endif

    splx (s);

    return (0);
    }
/*******************************************************************************
*
* egl_intr - read vector and dispatch to routine handler
*/

LOCAL void egl_intr
    (
    int unit
    )
    {
#ifdef EGL_DEBUG
    static char message [150];
    char *nullmsg       = "";
#endif
    EGL_SOFTC *egl      = egl_softc [unit];
    volatile SHIO *shio = egl->eglAddr;
    volatile CRB *crb   = &shio->sh_CRB;
    volatile CSB *csb   = &shio->sh_CSB;
    volatile IOPB tiopb;
    int real_errors;
    int vector;
    int index;
    int len;

    /* ack interrupt, read vector */

    vector = sysBusIntAck (egl->eglIntLevel) & 0x00ff;

    /* XXX temporarily while R3K targets don't do vectored VME interrupts */

    if (vector != (shio->sh_VMEIV & 0x00ff))
        {
#ifdef EGL_DEBUG
        logMsg ("egl%d: vectors disagree: sysBusIntAck = %#x, CSB-vec = %#x\n",
                unit, vector, shio->sh_VMEIV & 0x00ff, 0, 0, 0);
#endif  /* EGL_DEBUG */
        vector = shio->sh_VMEIV & 0x00ff;
        }

    switch (vector - egl->eglIntVec)
        {
        case 0: /* xmit */
            if (shio->sh_RET_IOPB.iopb_LAN1 & (LANCE_TONE | LANCE_TMORE))
                egl->egl_if.if_collisions++;

            index = crb->crb_CTAG;
            CRB_CLR_DONE(crb->crb_CRSW);        /* release Eagle */

            netJobAdd ((FUNCPTR)egl_tint, unit, (int)egl, index, 0, 0);
            break;

        case 1: /* xmiterr */
            tiopb = shio->sh_RET_IOPB;
            if (tiopb.iopb_LAN1 & LANCE_TERR)
                {
#ifdef EGL_DEBUG
                sprintf (message,
                    "egl%d: transmit error LAN1=%#x LAN3=%#x %s%s%s%s%s\n",
                    unit, tiopb.iopb_LAN1, tiopb.iopb_LAN3,
                    ((tiopb.iopb_LAN3&LAN3_BUFF) ? "buffer error "   : nullmsg),
                    ((tiopb.iopb_LAN3&LAN3_UFLO) ? "underflow "      : nullmsg),
                    ((tiopb.iopb_LAN3&LAN3_LCOL) ? "late collision " : nullmsg),
                    ((tiopb.iopb_LAN3&LAN3_LCAR) ? "carrier loss "   : nullmsg),
                    ((tiopb.iopb_LAN3&LAN3_RTRY) ? "retry error"     : nullmsg));
                logMsg (message, 0, 0, 0, 0, 0, 0);
#endif
                }
            else if (tiopb.iopb_STATUS)
                {
#ifdef EGL_DEBUG
                logMsg ("egl%d: transmit error status %#x\n",
                        unit, tiopb.iopb_STATUS, 0, 0, 0, 0);
#endif
                }

            index = crb->crb_CTAG;
            CRB_CLR_DONE(crb->crb_CRSW);        /* release Eagle */

            egl->egl_if.if_oerrors++;
            egl->egl_if.if_opackets--;
            netJobAdd ((FUNCPTR)egl_tint, unit, (int)egl, index, 0, 0);
            break;

        case 2: /* rcv */
            len = shio->sh_RET_IOPB.iopb_LENGTH;/* length of recv packet */
            index = crb->crb_CTAG;
            CRB_CLR_DONE(crb->crb_CRSW);        /* release Eagle */

            netJobAdd ((FUNCPTR)egl_rint, unit, (int)egl, index, len, 0);
            break;

        case 3: /* rcverr */
            len = shio->sh_RET_IOPB.iopb_LENGTH;   /* length of recv packet */
            index = crb->crb_CTAG;
            tiopb = shio->sh_RET_IOPB;
            if (tiopb.iopb_LAN1 & LANCE_RERR)
                {
                /* First, check for bogus LANCE errors:
                 * call normal completion code if no "real" errors.
                 */

                real_errors = tiopb.iopb_LAN1;

                if ((tiopb.iopb_LAN1&LAN1_OFLO) && (tiopb.iopb_LAN1&LANCE_RENP))
                    real_errors ^= LAN1_OFLO;   /* turn off bogus OFLO */

                if (!(tiopb.iopb_LAN1&LANCE_RENP)|| (tiopb.iopb_LAN1&LAN1_OFLO))
                    real_errors &= (0xffff - (LAN1_FRAM | LAN1_CRC));

                if (!(real_errors & (LAN1_FRAM|LAN1_OFLO|LAN1_CRC|LAN1_BUFF)))
                    {
                    /* Error was bogus.  Treat like normal receive int.  */
                    CRB_CLR_DONE(crb->crb_CRSW);        /* release Eagle */
                    netJobAdd ((FUNCPTR)egl_rint, unit, (int)egl, index, len,0);
                    return;
                    }

                /* looks like a real receive error */
#ifdef EGL_DEBUG
                sprintf (message,
                        "egl%d: receive error LAN1=%#x LAN3=%#x %s%s%s%s\n",
                        unit, tiopb.iopb_LAN1, tiopb.iopb_LAN3,
                        (real_errors&LAN1_FRAM) ? "framing error " : nullmsg,
                        (real_errors&LAN1_OFLO) ? "overflow "      : nullmsg,
                        (real_errors&LAN1_CRC)  ? "CRC error "     : nullmsg,
                        (real_errors&LAN1_BUFF) ? "buffer error"   : nullmsg);
#endif
                if ((++egl->egl_if.if_ierrors % 100) == 0)
                    {
                    /* Interphase Inc. only reports every 100 errors;
                     * we seem to get a fair number of CRC errors but
                     * probably they're bogus.
                     */
#ifdef EGL_DEBUG
                    logMsg (message, 0, 0, 0, 0, 0, 0);
#endif
                    }
                }
            else if (tiopb.iopb_STATUS)
                {
                /* only interesting if non RERR */
#ifdef EGL_DEBUG
                logMsg ("egl%d: receive error status %#x\n",
                        unit, tiopb.iopb_STATUS, 0, 0, 0, 0);

                /* valid only if no RERR */
                if (!tiopb.iopb_LENGTH)
                    logMsg ("egl%d: 0 length receive\n", unit, 0, 0, 0, 0, 0);
#endif
                }

            if (csb->csb_CSR0MISS > egl->egl_csr0miss)
                {
                /* any new ones? */
                egl->egl_csr0miss = csb->csb_CSR0MISS;

#ifdef EGL_DEBUG
                logMsg ("egl%d: csr0 missed receive packet, count = %d\n",
                        unit, egl->egl_csr0miss, 0, 0, 0, 0);
#endif
                }

            CRB_CLR_DONE(crb->crb_CRSW);        /* release Eagle */

            egl->egl_if.if_ierrors++;
            netJobAdd ((FUNCPTR)egl_rint, unit, (int)egl, index, -1, 0);
            break;

        case 4: /* qav */
            netJobAdd ((FUNCPTR)egl_qint, unit, (int)egl, 0, 0, 0);
            break;

        default:
#ifdef EGL_DEBUG
            logMsg (
            "egl%d: egl_intr - unknown interrupt vector %#x\n",
            unit, vector, 0, 0, 0, 0);
#endif
            break;
        }
    }
/*******************************************************************************
*
* egl_ioctl - process an ioctl request
*
* RETURNS: 0 or errno.
*/

LOCAL int egl_ioctl
    (
    struct ifnet *ifp,
    int cmd,
    caddr_t data
    )
    {
    int unit       = ifp->if_unit;
    EGL_SOFTC *egl = egl_softc [unit];
    int error      = 0;
    int status;
    int s;

    s = splimp ();

    switch (cmd)
        {
        case SIOCSIFADDR:       /* Set */
            ((struct arpcom *)ifp)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (ifp, &IA_SIN (data)->sin_addr);
            break;

        case SIOCGIFADDR:       /* Get */
            bcopy ((char*)egl->egl_enaddr,
                  (caddr_t) ((struct ifreq *)data)->ifr_addr.sa_data, 6);
#ifdef  EGL_DEBUG
            printf ("egl%d: egl_ioctl - SIOCGIFADDR\n", unit);
#endif  /* EGL_DEBUG */
            break;

        case SIOCGIFFLAGS:      /* Get */
            *(short *)data = ifp->if_flags;
#ifdef EGL_DEBUG
            printf ("egl%d: egl_ioctl - SIOCGIFFLAGS\n", unit);
#endif  /* EGL_DEBUG */
            break;

        case SIOCSIFFLAGS:      /* Set */
            /* Handled outside module - nothing more to do. */
            break;

        default:
#ifdef EGL_DEBUG
            printf ("egl%d: egl_ioctl - unknown ioctl\n", unit);
#endif  /* EGL_DEBUG */
            error = EINVAL;
            break;
        }

    splx (s);

    return (error);
    }
/*******************************************************************************
*
* egl_iworkq - initialize work queues
*
* RETURNS: 0 or -1 on error.
*/

LOCAL int egl_iworkq
    (
    int unit,
    int itype
    )
    {
    EGL_SOFTC *egl       = egl_softc [unit];
    volatile SHIO *shio  = egl->eglAddr;
    volatile WQCF *iopb  = (WQCF *)&shio->sh_MCE_IOPB;
    volatile USHORT *mcr = &shio->sh_MCSB.mcsb_MCR;
    int ix;

    iopb->wqcf_CMD      = CNTR_INIT_WORKQ;

    iopb->wqcf_OPTION  |= itype;        /* 0 no int; 1 int (M_OPT_IE) */

    iopb->wqcf_NVCT     = egl->qav_vec;
    iopb->wqcf_EVCT     = egl->qav_vec;

    bzero ((char*)iopb->wqcf_RES0, sizeof(iopb->wqcf_RES0));

    iopb->wqcf_WORKQ    = EGL_MISCQ;            /* misc queue */
    iopb->wqcf_WOPT     = 0;

    iopb->wqcf_SLOTS    = EGL_MS_SLOTS;         /* misc queue size */
    iopb->wqcf_PRIORITY = EGL_MS_PRIORITY;      /* misc queue priority */
    iopb->wqcf_WDIV     = EGL_MS_WDIV;          /* misc queue work division */

    bzero ((char*)iopb->wqcf_RES1, sizeof(iopb->wqcf_RES1));

    shio->sh_MCE.cqe_CTAG       = 0;
    shio->sh_MCE.cqe_IOPB_ADDR  = O_MCE_IOPB;
    shio->sh_MCE.cqe_WORK_QUEUE = 0;

    CQE_GO(shio->sh_MCE.cqe_QECR);

    if (!itype)
        {
        ix = egl_wait (&shio->sh_CRB);

        if (CRB_ERROR(ix))
            {
#ifdef EGL_DEBUG
            logMsg ("egl%d: *error* unable to initialize misc workq\n",
                    unit, 0, 0, 0, 0, 0);
#endif
            return (-1);
            }
        else if (ix == 0x1000) /* unused value */
            {
#ifdef EGL_DEBUG
            logMsg ("egl%d: *timeout* unable to initialize misc workq\n",
                    unit, 0, 0, 0, 0, 0);
#endif
            return (-1);
            }
        }
    else
        {
#ifdef EGL_DEBUG
        logMsg ("egl%d: initialize misc workq done\n",
                unit, 0, 0, 0, 0, 0);
#endif  /* EGL_DEBUG */
        }

    /* DMA MCE IOPB */

    iopb->wqcf_CMD = CNTR_INIT_WORKQ;

    iopb->wqcf_OPTION |= itype;     /* 0 no int; 1 int (M_OPT_IE) */

    iopb->wqcf_NVCT = egl->qav_vec;
    iopb->wqcf_EVCT = egl->qav_vec;

    bzero ((char*)iopb->wqcf_RES0, sizeof(iopb->wqcf_RES0));

    iopb->wqcf_WORKQ    = EGL_DMAQ;             /* misc queue */
    iopb->wqcf_WOPT     = 0;
    iopb->wqcf_SLOTS    = EGL_DM_SLOTS; /* misc queue size */
    iopb->wqcf_PRIORITY = EGL_DM_PRIORITY;      /* misc queue priority */
    iopb->wqcf_WDIV     = EGL_DM_WDIV;          /* misc queue work division */

    bzero ((char*)iopb->wqcf_RES1, sizeof(iopb->wqcf_RES1));

    shio->sh_MCE.cqe_CTAG       = 0;
    shio->sh_MCE.cqe_IOPB_ADDR  = O_MCE_IOPB;
    shio->sh_MCE.cqe_WORK_QUEUE = 0;

    CQE_GO(shio->sh_MCE.cqe_QECR);

    if (!itype)
        {
        ix = egl_wait (&shio->sh_CRB);

        if (CRB_ERROR(ix))
            {
#ifdef EGL_DEBUG
            logMsg ("egl%d: *error* unable to initialize DMA workq\n",
                    unit, 0, 0, 0, 0, 0);
#endif
            return (-1);
            }
        else if (ix == 0x1000) /* unused value */
            {
#ifdef EGL_DEBUG
            logMsg ("egl%d: *timeout* unable to initialize DMA workq\n",
                    unit, 0, 0, 0, 0, 0);
#endif
            return (-1);
            }
        }
    else
        {
#ifdef EGL_DEBUG
        logMsg ("egl%d: initialize DMA workq done\n",
                unit, 0, 0, 0, 0, 0);
#endif  /* EGL_DEBUG */
        }

    /* receive MCE IOPB */

    iopb->wqcf_CMD     = CNTR_INIT_WORKQ;

    iopb->wqcf_OPTION |= itype;     /* 0 no int; 1 int (M_OPT_IE) */

    iopb->wqcf_NVCT    = egl->qav_vec;
    iopb->wqcf_EVCT    = egl->qav_vec;

    bzero ((char*)iopb->wqcf_RES0, sizeof(iopb->wqcf_RES0));

    iopb->wqcf_WORKQ    = EGL_RECVQ;            /* receive queue */
    iopb->wqcf_WOPT     = 0;
    iopb->wqcf_SLOTS    = EGL_RX_SLOTS;         /* receive queue size */
    iopb->wqcf_PRIORITY = EGL_RX_PRIORITY;      /* receive queue priority */
    iopb->wqcf_WDIV     = EGL_RX_WDIV;          /* receive q work division */

    bzero ((char*)iopb->wqcf_RES1, sizeof(iopb->wqcf_RES1));

    shio->sh_MCE.cqe_CTAG       = 0;
    shio->sh_MCE.cqe_IOPB_ADDR  = O_MCE_IOPB;
    shio->sh_MCE.cqe_WORK_QUEUE = 0;

    CQE_GO(shio->sh_MCE.cqe_QECR);

    if (!itype)
        {
        ix = egl_wait (&shio->sh_CRB);

        if (CRB_ERROR(ix))
            {
#ifdef EGL_DEBUG
            logMsg ("egl%d: *error* unable to initialize receive workq\n",
                    unit, 0, 0, 0, 0, 0);
#endif
            return (-1);
            }
        else if (ix == 0x1000) /* unused value */
            {
#ifdef EGL_DEBUG
            logMsg ("egl%d: *timeout* unable to initialize receive workq\n",
                    unit, 0, 0, 0, 0, 0);
#endif
            return (-1);
            }
        }
    else
        {
#ifdef EGL_DEBUG
        logMsg ("egl%d: initialize receive workq done\n",
                unit, 0, 0, 0, 0, 0);
#endif  /* EGL_DEBUG */
        }

    /* transmit MCE IOPB */

    iopb->wqcf_CMD     = CNTR_INIT_WORKQ;
    iopb->wqcf_OPTION |= itype;     /* 0 no int; 1 int (M_OPT_IE) */
    iopb->wqcf_NVCT    = egl->qav_vec;
    iopb->wqcf_EVCT    = egl->qav_vec;

    bzero ((char*)iopb->wqcf_RES0, sizeof(iopb->wqcf_RES0));

    iopb->wqcf_WORKQ    = EGL_XMTQN(0);         /* transmit queue 0 */
    iopb->wqcf_WOPT     = 0;
    iopb->wqcf_SLOTS    = EGL_TX_SLOTS;         /* transmit queue size */
    iopb->wqcf_PRIORITY = EGL_TX_PRIORITY;      /* transmit queue priority */
    iopb->wqcf_WDIV     = EGL_TX_WDIV;          /* transmit q work division */

    bzero ((char*)iopb->wqcf_RES1, sizeof(iopb->wqcf_RES1));

    shio->sh_MCE.cqe_CTAG       = 0;
    shio->sh_MCE.cqe_IOPB_ADDR  = O_MCE_IOPB;
    shio->sh_MCE.cqe_WORK_QUEUE = 0;

    CQE_GO(shio->sh_MCE.cqe_QECR);

    if (!itype)
        {
        ix = egl_wait (&shio->sh_CRB);

        if (CRB_ERROR(ix))
            {
#ifdef EGL_DEBUG
            logMsg ("egl%d: *error* unable to initialize transmit workq\n",
                    unit, 0, 0, 0, 0, 0);
#endif
            return (-1);
            }
        else if (ix == 0x1000) /* unused value */
            {
#ifdef EGL_DEBUG
            logMsg ("egl%d: *timeout* unable to initialize transmit workq\n",
                    unit, 0, 0, 0, 0, 0);
#endif
            return (-1);
            }
        }
    else
        {
#ifdef EGL_DEBUG
        logMsg ("egl%d: initialize transmit workq done\n",
                unit, 0, 0, 0, 0, 0);
#endif  /* EGL_DEBUG */
        }

    *mcr |= M_MCR_SQM;  /* start queue mode */
    sysWbFlush();

    return (0);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* egl_output - Ethernet output routine
*
* RETURNS: ?
*/

LOCAL int egl_output
    (
    struct ifnet *ifp,
    struct mbuf *m0,
    struct sockaddr *dst
    )
    {
    return (ether_output (ifp, m0, dst, (FUNCPTR) egl_start,
                         &egl_softc [ifp->if_unit]->egl_ac));
    }
#endif

/*******************************************************************************
*
* egl_physreset - physical reset of the Interphase Eagle card
*
* RETURNS: 0 or -1 if reset or diagnostics fail.
*/

LOCAL STATUS egl_physreset
    (
    int unit
    )
    {
    EGL_SOFTC *egl       = egl_softc [unit];
    volatile SHIO *shio  = egl->eglAddr;
    volatile USHORT *msr = &shio->sh_MCSB.mcsb_MSR;
    volatile USHORT *mcr = &shio->sh_MCSB.mcsb_MCR;
    int ix;

#ifdef EGL_DEBUG
    printf ("egl%d: egl_physreset - msr address = %#x\n", unit, msr);
#endif  /* EGL_DEBUG */

    for (ix = 0; ix < 1000; ix++)
        {
        if (!(*msr & M_MSR_CNA) && (*msr & M_MSR_BOK))
            break;

        DELAY(8000);
        }

    if (ix == 1000)
        {
#ifdef EGL_DEBUG
        printf (
        "egl%d: diagnostics failed, or controller not available; msr = %#x\n",
                unit, *msr);
#endif
        return (-1);
        }

    *msr |= M_MSR_CNA;
    sysWbFlush();

    *mcr = M_MCR_RES;   /* reset the Eagle */
    sysWbFlush();

    DELAY(500);         /* at least 50usec required */

    *mcr &= ~M_MCR_RES;
    sysWbFlush();

    for (ix = 0; ix < 1000; ix++)
        {
        if (!(*msr & M_MSR_CNA))
            break;
        DELAY(8000);
        }

    if (ix == 1000)
        {
#ifdef EGL_DEBUG
        printf ("egl%d: diagnostics failed during phase 1; msr = %#x\n",
                unit, *msr);
#endif
        return (-1);
        }

    /* more delay necessary as reset is not yet done! (Interphase bug) */

    for (ix = 0; ix < 1000; ix++)
        {
        if (*msr & M_MSR_BOK)
            break;
        DELAY(8000);
        }

    if (ix == 1000)
        {
#ifdef EGL_DEBUG
        printf ("egl%d: diagnostics failed during phase 2; msr = %#x\n",
                unit, *msr);
#endif
        *msr |= M_MSR_BOK; /* re-set board OK (iphase bug) */
        return (-1);
        }

    return (0);
    }
/*******************************************************************************
*
* egl_qint - Ethernet interface queue entry available interrupt
*/

LOCAL void egl_qint
    (
    int unit,
    EGL_SOFTC *egl
    )
    {
    volatile SHIO *shio  = egl->eglAddr;
    volatile CRB *crb    = &shio->sh_CRB;
    volatile CSB *csb    = &shio->sh_CSB;
    volatile USHORT *msr = &shio->sh_MCSB.mcsb_MSR;
    IOPB tiopb;

    tiopb = shio->sh_RET_IOPB;

    /* controller not available */

    if (*msr & M_MSR_CNA)
        {
#ifdef EGL_DEBUG
        logMsg ("egl%d: died and restarted; error status = %#x\n",
                unit, tiopb.iopb_STATUS, 0, 0, 0, 0);
#endif

        /* these errors (among others) will turn the LANCE transmitter
           off, which will send us here */

        /* any new ones? */
        if (csb->csb_CSR0MEM > egl->egl_csr0mem)
            {
            egl->egl_csr0mem = csb->csb_CSR0MEM;
#ifdef EGL_DEBUG
            logMsg ("egl%d: %d csr0 MEMORY ERRORS caught\n",
                    unit, egl->egl_csr0mem, 0, 0, 0, 0);
#endif
            }

#ifdef EGL_DEBUG
        /* save the returned IOPB */
        logMsg ("returned IOPB: cmd %#x options %#x status %#x nvect %#x\n",
                tiopb.iopb_CMD, tiopb.iopb_OPTION, tiopb.iopb_STATUS,
                tiopb.iopb_NVCT, 0, 0);
        logMsg ("evect %#x topt %#x buff_addr %#x length %#x\n",
                tiopb.iopb_EVCT, tiopb.iopb_TOPT,
                tiopb.iopb_BUFF, tiopb.iopb_LENGTH, 0, 0);
        logMsg ("host_buf %#x ptlf %#x sge_cnt %#x lan1 %#x lan3 %#x\n",
                tiopb.iopb_HBUF, tiopb.iopb_PTLF, tiopb.iopb_SGEC,
                tiopb.iopb_LAN1, tiopb.iopb_LAN3, 0);
        logMsg ("src/dest node %x:%x:%x:%x:%x:%x\n",
                tiopb.iopb_NODE[0],tiopb.iopb_NODE[1],tiopb.iopb_NODE[2],
                tiopb.iopb_NODE[3],tiopb.iopb_NODE[4],tiopb.iopb_NODE[5]);
#endif  /* EGL_DEBUG */

        egl_init (unit);

        return;
        }

    if (CRB_QAVAIL(crb->crb_CRSW))
        {
        /* feed queue is no longer full */
        egl->egl_qfull = 0;     /* at least one slot open now */
        }

    if (CRB_QSTARTED(crb->crb_CRSW))
        {
        /* how about q mode started */
        egl->egl_qmode++;       /* remember for later */
        }

    CRB_CLR_DONE(crb->crb_CRSW);        /* release Eagle */
    }
/*******************************************************************************
*
* egl_recv - process Ethernet receive completion
*
* This procedure handles Ethernet receive completion interrupts.  If
* input error just drop packet, otherwise examine packet to determine
* type.  If can't determine length from type, then have to drop packet,
* otherwise decapsulate packet based on type and pass to type-specific
* higher-level input routine.
*
* RETURNS: ?
*/

LOCAL void egl_recv
    (
    int unit,
    int len,
    ETH_BUF *eth_buf
    )
    {
    EGL_SOFTC *egl = egl_softc [unit];
    struct ether_header *eh;
    struct mbuf *m;
#ifdef BSD43_DRIVER
    int off;
#endif
    unsigned char *pData;

    egl->egl_if.if_ipackets++;

    /* speed fix (in Eagle firmware) for 14 byte Ethernet header */

    eh = (struct ether_header *) ((int)eth_buf->eth_dat + 2);

    /* call input hook if any */

    if (etherInputHookRtn != NULL &&
        (* etherInputHookRtn) (&egl->egl_if, (char *) eh, len))
        {
        return;
        }

    len -=  SIZEOF_ETHERHEADER + 4; /* total length - (ether header + CRC) */

    pData = ((unsigned char *) eh) + SIZEOF_ETHERHEADER;

#ifdef BSD43_DRIVER
    check_trailer (eh, pData, &len, &off, &egl->egl_if);

    if (len == 0)
        return;

    /* copy data from Eagle to mbuf a word at a time */

    m = bcopy_to_mbufs (pData, len, off, (struct ifnet *) &egl->egl_if,
                        eglMemWidth);

    if (m != NULL)
        do_protocol_with_type (eh->ether_type, m, &egl->egl_ac, len);
    else
        egl->egl_if.if_ierrors++;        /* bump error stat */
#else
    m = bcopy_to_mbufs (pData, len, 0, &egl->egl_if, eglMemWidth);
    if (m != NULL)
        do_protocol (eh, m, &egl->egl_ac, len);
    else
        egl->egl_if.if_ierrors++;        /* bump error stat */
#endif
    }
/*******************************************************************************
*
* egl_reset - mark interface as inactive & reset the chip
*
* RETURNS: 0 or -1.
*/

LOCAL STATUS egl_reset
    (
    int unit
    )
    {
    EGL_SOFTC *egl;
    volatile SHIO *shio;
    struct ifnet *ifp;
    int s;

    if (unit < 0 || unit >= NEGL)
        return (-1);

    egl  = egl_softc [unit];
    shio = egl->eglAddr;
    ifp  = &egl->egl_if;

#ifdef EGL_DEBUG
    printf ("egl%d: egl_reset\n", unit, 0, 0, 0, 0, 0);
#endif  /* EGL_DEBUG */

    s = splimp ();
    ifp->if_flags &= ~IFF_RUNNING;

    if (egl_physreset (unit) != OK)
        {
        splx (s);
#ifdef EGL_DEBUG
        logMsg ("egl%d: reset failed\n", unit, 0, 0, 0, 0, 0);
#endif
        return (-1);
        }

    splx (s);

    return (0);
    }
/*******************************************************************************
*
* egl_rint - Ethernet interface receive (& error) interrupt
*/

LOCAL void egl_rint
    (
    int unit,
    EGL_SOFTC *egl,
    int index,
    int len
    )
    {
    ETH_BUF *eth_buf;

    if (len != -1)
        {
        eth_buf = egl->egl_rbinfo[index];
        egl->egl_rbinfo[index] = NULL;

        egl_ioflush (eth_buf);          /* definitly need on receive */

        egl_recv (unit, len, eth_buf);

        REL_ETH_BUF(eth_buf);
        }

    (void)egl_hangrcv (unit, index);    /* hang another receive request */

    egl->egl_qfull = 0;                 /* at least one slot open now */
    }
/*******************************************************************************
*
* egl_setiopbs - setup iopb's
*/

LOCAL void egl_setiopbs
    (
    int unit
    )
    {
    EGL_SOFTC *egl = egl_softc [unit];
    volatile SHIO *shio = egl->eglAddr;
    volatile IOPB *iopb;
    int ix;
    ETH_BUF *eth_buf;

    iopb = &shio->sh_IOPB[EGL_RX_IOPBF];
    for (ix = 0; ix < EGL_RX_NBUFF; ix++)
        {
        iopb->iopb_OPTION = M_OPT_DMA | M_OPT_IE;

        iopb->iopb_CMD  = CNTR_RECEIVE;
        iopb->iopb_NVCT = egl->rcv_vec;
        iopb->iopb_EVCT = egl->rcverr_vec;

        if (BLOCKMODE)
            iopb->iopb_TOPT = EGL_TOPT_BLK | DIR_READ;
        else
            iopb->iopb_TOPT = EGL_TOPT | DIR_READ;

        /* use the Host usable buffer region */

        if ((eth_buf = egl->egl_rbinfo[ix]) == NULL)
            GET_ETH_BUF(eth_buf);

        /* egl_rbinfo[i] always uses iopb at index (EGL_RX_IOPBF+i) */

        eth_buf->eth_len          = EGL_MTU;
        egl->egl_rbinfo[ix]       = eth_buf;
        egl->egl_rbinfo[ix]->iopb = (IOPB*)iopb;

        egl_ioflush (eth_buf);

        iopb->iopb_BUFF   = eth_buf->eth_dat_adr;
        iopb->iopb_LENGTH = EGL_MTU;
        iopb->iopb_HBUF   = 0;
        iopb->iopb_PTLF   = 0;

        bzero ((char*)iopb->iopb_NODE, sizeof(iopb->iopb_NODE));

        iopb->iopb_SGEC = 0;
        iopb->iopb_LAN1 = 0;
        iopb->iopb_LAN3 = 0;
        iopb++;
        }

    iopb = &shio->sh_IOPB[EGL_TX_IOPBF];

    for (ix = 0; ix < EGL_TX_NBUFF; ix++)
        {
        iopb->iopb_CMD = CNTR_TRANSMIT;

        iopb->iopb_OPTION = M_OPT_DMA | M_OPT_IE;

        iopb->iopb_NVCT = egl->xmit_vec;
        iopb->iopb_EVCT = egl->xmiterr_vec;

        /* We need to recover any hung transmit buffers if
         * initialization is due to a HW reset after an error
         * such as a transmit timeout.
         */

        if ((eth_buf = egl->egl_tbinfo[ix]) != NULL)
            {
            egl->egl_tbinfo[ix] = NULL;
            egl_ioflush (eth_buf);
            REL_ETH_BUF(eth_buf);
            }

        if (BLOCKMODE)
            iopb->iopb_TOPT = EGL_TOPT_BLK | DIR_WRITE;
        else
            iopb->iopb_TOPT = EGL_TOPT | DIR_WRITE;

        iopb->iopb_HBUF = 0;
        iopb->iopb_SGEC = 0;
        iopb->iopb_LAN1 = 0;
        iopb->iopb_LAN3 = 0;
        iopb++;
        }

    egl->egl_txadd  = egl->egl_txrem = EGL_TX_NBUFF-1;
    egl->egl_quecnt = egl->egl_txcnt = 0;
    egl->egl_cqe    = (CQE *)&shio->sh_CQE[0];
    }

/*******************************************************************************
*
* egl_start - start or re-start output on interface
*
* Get another datagram to send off of the interface queue,
* and add it to the transmit packet queue for the interface.
*/

#ifdef BSD43_DRIVER
LOCAL void egl_start
    (
    int unit
    )
    {
    EGL_SOFTC *egl      = egl_softc [unit];
#else
LOCAL void egl_start
    (
    EGL_SOFTC * 	egl
    )
    {
#ifdef EGL_DEBUG
    int unit = egl->egl_if.if_unit;
#endif
#endif
    volatile SHIO *shio = egl->eglAddr;
    volatile IOPB *iopb;
    volatile CQE *cqe;
    u_char *bufStart;
    struct mbuf *m;
    struct ether_header *eh;
    ETH_BUF *eth_buf;
    int len;
    int index;
    int s;

    if (TXFULL(egl->egl_txcnt + egl->egl_quecnt))
        {
#ifdef EGL_DEBUG
        logMsg ("egl%d: egl_start - packet queue full\n", unit, 0, 0, 0, 0, 0);
#endif  /* EGL_DEBUG */
        return; /* packet queue is full, can't send anything */
        }

    s = splimp ();

    IF_DEQUEUE(&egl->egl_if.if_snd, m);

    if (m == 0)
        {
        splx (s);
        return;
        }

    TXINC(egl->egl_txadd);
    egl->egl_txcnt++;

    index = egl->egl_txadd;
    iopb  = (IOPB *)&shio->sh_IOPB[EGL_TX_IOPBF + index];

    GET_ETH_BUF(eth_buf);

    bufStart = &eth_buf->eth_dat[0];

    copy_from_mbufs (bufStart, m, len);

    /* call output hook if any */

    if ((etherOutputHookRtn != NULL) &&
        (* etherOutputHookRtn) (&egl->egl_if, bufStart, len))
        {
        REL_ETH_BUF(eth_buf);
        splx (s);
        return;                 /* output hook has processed this packet */
        }

    eh = (struct ether_header *)bufStart;

    if ((len - SIZEOF_ETHERHEADER) < ETHERMIN)
        len = ETHERMIN + SIZEOF_ETHERHEADER;

    /* must be modulo 4 in order to use block mode transfers */
    iopb->iopb_LENGTH = (len + 3) & ~0x3;

    iopb->iopb_BUFF        = eth_buf->eth_dat_adr;
    eth_buf->eth_len       = len;
    egl->egl_tbinfo[index] = eth_buf;
    egl_ioflush (eth_buf);

    if (BLOCKMODE)
        {
        /* we can (try and) use block mode transfer,
         * must be aligned on a long-word boundary
         */
        if (!(iopb->iopb_BUFF & 0x2))
            iopb->iopb_TOPT = EGL_TOPT_BLK | DIR_WRITE;
        else
            iopb->iopb_TOPT = EGL_TOPT | DIR_WRITE; /* put back to non-block */
        }

    iopb->iopb_PTLF = eh->ether_type;

    /*
     * Following code performs an efficient copy of the Ethernet
     * destination address when the src & dst are word aligned.
     * This avoids a sequence of 6 one byte writes on the VMEbus.
     */

    if (((ULONG)eh->ether_dhost & 0x3) == 0)
        {
        *(int *)iopb->iopb_NODE       = *(int *)eh->ether_dhost;
        *(short *)(iopb->iopb_NODE+4) = *(short *)(eh->ether_dhost+4);
        }
    else
        {
        bcopy ((char*)eh->ether_dhost, (char*)iopb->iopb_NODE,
                sizeof(iopb->iopb_NODE));
        }

    bcopy ((char*)egl->egl_enaddr, (char*)eh->ether_shost,
            sizeof(eh->ether_shost));

    /* send */

    if (egl->egl_txcnt == 0)
        {
#ifdef EGL_DEBUG
        logMsg ("egl%d: egl_txcnt is null\n", unit, 0, 0, 0, 0, 0);
#endif
        splx (s);
        return;                 /* this should never happen */
        }

    if (egl->egl_qfull != 0)
        {
        splx (s);
        return;
        }

    cqe = egl->egl_cqe;

    if (CQE_BUSY(cqe->cqe_QECR))
        {
        egl->egl_qfull++;       /* feed queue is full */
        splx (s);
        return;
        }

    TXINC(egl->egl_txrem);
    egl->egl_txcnt--;

    index = egl->egl_txrem;
    cqe->cqe_CTAG = index;

    cqe->cqe_IOPB_ADDR = (caddr_t)(&shio->sh_IOPB[EGL_TX_IOPBF] + index) -
                         (caddr_t)shio;

    cqe->cqe_WORK_QUEUE = EGL_XMTQN(0); /* TX work queue 0 */
    CQE_GO(cqe->cqe_QECR);

    egl->egl_quecnt++;

    if (++egl->egl_cqe == &shio->sh_CQE[MAX_CQE])
        egl->egl_cqe = (CQE *)&shio->sh_CQE[0];

#ifndef BSD43_DRIVER    /* BSD 4.4 ether_output() doesn't bump statistic. */
    egl->egl_if.if_opackets++;
#endif

    splx (s);
    }

/*******************************************************************************
*
* eglShow - display per unit Eagle driver info
*
* RETURNS: 0 or -1 if unit not present.
*
* NOMANUAL
*/

int eglShow
    (
    int unit,   /* interface unit */
    BOOL zap    /* zero totals */
    )
    {
    static char *e_message [] = {
    "transmit attempts",
    "   \"     DMA completions",
    "   \"     LANCE interrupts",
    "   \"     good",
    "   \"     errors",
    "   \"     dones",
    "receive attempts",
    "   \"    starvations",
    "   \"    interrupts",
    "   \"    good",
    "   \"    errors",
    "   \"    DMA completions",
    "   \"    dones",
    "Miscellaneous DMA interrupts",
    "Total LANCE Interrupts",
    "LANCE Inits",
    "CSR0 Babble",
    " \"   Collisions",
    " \"   Misses",
    " \"   Memory Errors"
    };

    int ix;
    volatile SHIO *shio;
    EGL_SOFTC *egl;

    if (unit < 0 || unit >= NEGL || (egl = egl_softc [unit]) == NULL)
        {
#ifdef EGL_DEBUG
        printf ("egl%d: no Eagle driver initialized for unit\n", unit);
#endif
        return (-1);
        }

    shio = egl->eglAddr;

    printf ("egl%d: Firmware revision: ", unit);
    for (ix = 0; ix < 3; ix++)
        printf ("%c", shio->sh_CSTB.cstb_FREV [ix]);
    printf ("  Release date: ");
    for (ix = 0; ix < 8; ix++)
        printf ("%c", shio->sh_CSTB.cstb_FDATE [ix]);
    printf ("\n");

    for (ix = 0; ix < NELEMENTS(e_message); ix++)
        {
        printf ("    %-30.30s  %4ld\n", e_message [ix],
                shio->sh_CSB.csb_stat [ix] - egl->CSBstats.csb_stat [ix]);

        if (zap)
            egl->CSBstats.csb_stat [ix] = shio->sh_CSB.csb_stat [ix];
        }

    return (0);
    }
/*******************************************************************************
*
* egl_tint - Ethernet interface transmit (& error) interrupt
*/

LOCAL void egl_tint
    (
    int unit,
    EGL_SOFTC *egl,
    int index
    )
    {
    ETH_BUF *eth_buf;

    eth_buf = egl->egl_tbinfo[index];
    egl_ioflush (eth_buf);              /* do we need on transmit? */
    egl->egl_tbinfo[index] = NULL;
    REL_ETH_BUF(eth_buf);

    egl->egl_quecnt--;
    egl->egl_qfull = 0;         /* at least one slot open now */

#ifdef BSD43_DRIVER
    egl_start (unit);
#else
    egl_start (egl);
#endif
    }
/*******************************************************************************
*
* egl_wait -
*/

LOCAL int egl_wait
    (
    volatile CRB *crb
    )
    {
    volatile USHORT *lcrsw = &crb->crb_CRSW;
    int ix;

    for (ix = 0; ix < 8000; ix++)
        {
        if (*lcrsw & M_CRSW_CRBV)
            break;
        DELAY(1000);
        }

    /* Check for the Command Response Block Valid bit.  Sometimes
     * the Eagle turns on "Queue Mode Started" just before completing
     * a command (don't know why, but has been observed).
     */

    if (*lcrsw & M_CRSW_CRBV)
        {
        /* Eagle has completed an operation? */
        CRB_CLR_DONE(*lcrsw);   /* release Eagle */
        }

    if (ix == 0)
        return (0x1000); /* indicate a timeout occurred */

    return (*lcrsw);
    }

#if     FALSE
void egl_delay (int x)
    {
    /* use with timexN() from shell for tuning */
    DELAY(x);
    }
#endif  /* FALSE */
