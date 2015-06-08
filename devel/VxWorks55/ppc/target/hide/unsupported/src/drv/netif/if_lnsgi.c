/* if_lnsgi.c - AMD Am7990 LANCE Ethernet (for SGI) network interface driver */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01k,15jul97,spm  removed driver initialization from ioctl support (SPR #8831);
                 added ARP request to SIOCSIFADDR ioctl handler
01j,08apr97,spm  removed compiler warnings, and errors from changes in if_ln.h
01i,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01h,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
01g,22feb93,jdi  documentation cleanup.
01f,09sep92,gae  documentation tweaks.
01e,29jul92,rfs  Moved driver specific items here from the header file.
01d,22jul92,gae  fixed number of parameters to logMsg().
01c,23jun92,ajm  upgraded to 5.1, ansiized.
01b,28apr92,ajm  changed lnEnetAddr to lnsgiEnetAddr for board specific driver
01a,30mar92,ajm  modified if_ln.c 01x for SGI VIP10, becomes target specific
*/

/*
DESCRIPTION
This module implements the
Advanced Micro Devices Am7990 LANCE Ethernet network interface driver for the
Silicon Graphics VIP10 board.  The only user-callable routine is lnsgiattach(),
which publishes the `lnsgi' interface and initializes the driver and device.

This driver is a special variant of if_ln, designed for the Silicon Graphics 
(SGI) VIP10 board.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

SEE ALSO: ifLib
*/

#include "vxWorks.h"
#include "iv.h"
#include "memLib.h"
#include "cacheLib.h"
#include "sys/ioctl.h"
#include "intLib.h"
#include "logLib.h"
#include "netLib.h"
#include "taskLib.h"
#include "etherLib.h"

#include "net/mbuf.h"
#include "net/protosw.h"
#include "net/unixLib.h"
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

#include "drv/netif/if_ln.h"


/***** LOCAL DEFINITIONS *****/

#define RMD_SIZ		sizeof(ln_rmd)
#define TMD_SIZ		sizeof(ln_tmd)

/* Configuration items */

#define LN_MIN_FIRST_DATA_CHAIN	96	/* min size of 1st buf in data-chain */
#define LN_MAX_MDS		128	/* max number of [r|t]md's for LANCE */

#define	LN_BUFSIZE	(ETHERMTU + sizeof (struct ether_header) + 6)

#define LN_RMD_RLEN	5	/* ring size as a power of 2 -- 32 RMD's */
#define LN_TMD_TLEN	5	/* same for transmit ring    -- 32 TMD's */
#define LN_NUM_RESERVES	8	/* have up to 8 reserved RMD's for loans */

#define MAX_LN		1	/* max number of LANCE chips on board */

/*
 * If LN_KICKSTART_TX is TRUE the transmitter is kick-started to force a
 * read of the transmit descriptors, otherwise the internal polling (1.6msec)
 * will initiate a read of the descriptors.  This should be FALSE is there
 * is any chance of memory latency or chip accesses detaining the LANCE DMA,
 * which results in a transmitter UFLO error.  This can be changed with the
 * global lnKickStartTx below.
 */

#define LN_KICKSTART_TX	FALSE	/* TRUE:  kick-start transmitter,
				   FALSE: wait for internal poll */

/* The definition of the driver control structure */

typedef struct ls_softc
    {
    struct arpcom  ls_ac;		/* ethernet common part */

    ln_ib	  *ib;				/* ptr to Initialization Block */
    struct
		{
        int	r_po2;			/* RMD ring size as a power of 2! */
		int	r_size;			/* RMD ring size (power of 2!) */
		int	r_index;		/* index into RMD ring */
		ln_rmd	*r_ring;		/* RMD ring */
		char	*r_bufs;		/* receive buffers base */
        } rmd_ring;
    struct
		{
        int	t_po2;			/* TMD ring size as a power of 2! */
		int	t_size;			/* TMD ring size (power of 2!) */
		int	t_index;		/* index into TMD ring */
		int	d_index;		/* index into TMD ring */
		ln_tmd	*t_ring;		/* TMD ring */
		char	*t_bufs;		/* transmit buffers base */
        } tmd_ring;

    u_char	ls_flags;		/* misc control flags */
    int		ivec;			/* interrupt vector */
    int		ilevel;			/* interrupt level */
    LN_DEVICE	*devAdrs;		/* device structure address */
    char	*memBase;		/* LANCE memory pool base */
    int	  	memWidth;		/* width of data port */
    int		csr0Errs;		/* count of csr0 errors */

    int		bufSize;		/* size of buffer in the LANCE ring */
    BOOL	canLoanRmds;		/* can we loan out rmd's as clusters? */
    char	*freeBufs[LN_NUM_RESERVES];	/* available reserve buffers */
    int		nFree;			/* number of free reserve buffers */
    u_char	loanRefCnt[LN_MAX_MDS];	/* reference counts for loaned RMD's */

    } DRV_CTRL;

#define DRV_CTRL_SIZ	sizeof(DRV_CTRL)

/* Definitions for the flags field */

#define LS_PROMISCUOUS_FLAG	0x1
#define LS_MEM_ALLOC_FLAG	0x2
#define LS_PAD_USED_FLAG	0x4
#define LS_RCV_HANDLING_FLAG	0x8
#define LS_START_OUTPUT_FLAG	0x10

/* Shorthand structure references */

#define	ls_if		ls_ac.ac_if		/* network-visible interface */
#define	ls_enaddr 	ls_ac.ac_enaddr		/* hardware ethernet address */

#define	ls_rpo2		rmd_ring.r_po2
#define	ls_rsize	rmd_ring.r_size
#define	ls_rindex	rmd_ring.r_index
#define	ls_rring	rmd_ring.r_ring

#define	ls_tpo2		tmd_ring.t_po2
#define	ls_tsize	tmd_ring.t_size
#define	ls_tindex	tmd_ring.t_index
#define	ls_dindex	tmd_ring.d_index
#define	ls_tring	tmd_ring.t_ring

#define LN_RMD_GIVE_TO_LANCE(pRmd) 					\
    { 									\
    pRmd->lnRMD1 &= 0xff; 						\
    pRmd->rbuf_mcnt = 0; 						\
    pRmd->lnRMD1 |= lnrmd1_OWN;      					\
    }

/*
* The following macros let the MIPS architecture allocate and free dymanic
* memory from uncached space.  The calls calloc, and cfree do the same
* as malloc, and free, they are merely there to aviod macro recursion.
*/
#define malloc(x)       	K0_TO_K1(calloc((size_t) (x),\
					 (size_t) sizeof(char)))
#define free(x)         	cfree((void *) (K1_TO_K0((x))))
#define	cacheClearEntry(x,y)

#define EMAP_ADDR       0xbf920802              /* memory map memory */
LOCAL volatile u_short *pEmap = (volatile u_short *) EMAP_ADDR;

/* buffer address to emap translation */

#define	EMAP_PG_SZ		(4096)
#define EMAP_OFFSET(index)      ((index) << 1)
#define EMAP_VALUE(buffaddr)    ((u_int)(buffaddr) >> 12)

/* lance address to physical translation */

#define EMAP_PG(addr)      	(((addr) >> 12) & 0xff)
#define EMAP_ENTRY(addr)   	(pEmap[(EMAP_PG(addr))])
#define EADDR(addr)        	(((EMAP_ENTRY(addr)) << 12) | \
				 ((u_int)(addr) & 0xfff))
#define LNADDR(index,addr)	(((index) << 12) | ((u_int)(addr) & 0xfff))

/* emap indexes */

#define EMAPLEN         256
#define IBUF_SZ         2
#define RBUF_SZ         64
#define TBUF_SZ         64
#define RBUF_INDEX      0                       /* location of init block */
#define TBUF_INDEX      (RBUF_INDEX + RBUF_SZ)  /* location of rec mbufs */
#define IBUF_INDEX      (TBUF_INDEX + TBUF_SZ)  /* location of xmit mbufs */

/* imports */

IMPORT void sysWbFlush ();
IMPORT void sysLanIntEnable (int ilevel);

IMPORT unsigned char lnsgiEnetAddr []; /* ethernet address to load into LANCE */

/* globals */

int lnsgiLogCount = 0;

/* locals */

LOCAL int lnTsize = LN_TMD_TLEN;	/* deflt xmit ring size as power of 2 */
LOCAL int lnRsize = LN_RMD_RLEN;	/* deflt recv ring size as power of 2 */

LOCAL struct ls_softc  *ls_softc [MAX_LN];

LOCAL u_int RAPOffset = 128;	/* XXX initially 1, calculated in lnReset */
LOCAL u_int CSROffset = 0;	/* XXX initially 0, calculated in lnReset */

/* forward declarations */

static int        lnInit (int unit);
static int        lnIoctl (struct ifnet *ifp, int cmd, caddr_t data);
#ifdef BSD43_DRIVER
static int        lnOutput (struct ifnet *ifp, struct mbuf *m0, 
                            struct sockaddr *dst);
#endif
static void       lnReset(int unit);
static ln_rmd    *lnGetFullRMD (struct ls_softc  *ls);
static ln_tmd    *lnGetFreeTMD (struct ls_softc  *ls);
static STATUS     lnRecv (struct ls_softc *ls, ln_rmd *rmd);
static void       lnChipInit (struct ls_softc  *ls);
static void       lnChipReset (struct ls_softc *ls);
static void       lnConfig (struct ls_softc *ls);
static void       lnHandleCsr0Err (struct ls_softc *ls, u_short status);
static void       lnHandleRecvInt (struct ls_softc *ls);
static void       lnInt (struct ls_softc  *ls);
static void       lnRmdFree (struct ls_softc *ls, char *pBuf);
#ifdef BSD43_DRIVER
static void       lnStartOutput (int unit);
#else
static void       lnStartOutput (DRV_CTRL *ls);
#endif
static void       lnCsrWrite (struct ls_softc *ls, int reg, u_short value);
static u_short    lnCsrRead (struct ls_softc *ls, int reg);
static void       lnCsrIntWrite (struct ls_softc *ls, int reg, u_short value);
static u_short    lnCsrIntRead (struct ls_softc *ls, int reg);

/*******************************************************************************
*
* lnsgiattach - publish the `lnsgi' network interface and initialize the driver and device
*
* This routine attaches an `lnsgi' Ethernet interface to the network if the
* interface exists.  It makes the interface available by filling in
* the network interface record.  The system will initialize the interface
* when it is ready to accept packets.
*
* The <memAdrs> parameter specifies the location of the interface memory pool;
* if NONE, the memory pool will be malloc'ed.
*
* The <memWidth> parameter sets the memory pool's data port width (in bytes);
* if NONE, any data width will be used.
*
* BUGS
* To zero out LANCE data structures, this routine uses bzero(), which
* ignores the <memWidth> specification and uses any size data access to
* write to memory.
*
* RETURNS: OK or ERROR.
*/

STATUS lnsgiattach
    (
    int	      unit,		/* unit number */
    char     *devAdrs,		/* LANCE i/o address */
    int	      ivec,		/* interrupt vector */
    int	      ilevel,		/* interrupt level */
    char     *memAdrs,		/* address of memory pool (-1 = malloc it) */
    ULONG     memSize,		/* only used if memory pool is NOT malloc()'d */
    int	      memWidth,		/* byte-width of data (-1 = any width)     */
    BOOL      usePadding,       /* use padding when accessing RAP? */
    int	      bufSize 		/* size of a buffer in the LANCE ring */
    )
    {
    FAST DRV_CTRL * 	ls;
    FAST struct ifnet	  *ifp;
    FAST unsigned int	   sz;		/* temporary size holder */
    char 		  *memPool;	/* start of the LANCE memory pool */

    if (bufSize == 0)
        bufSize = LN_BUFSIZE;

    if ((int) memAdrs != NONE)	/* specified memory pool */
	{
	/*
	 * With a specified memory pool we want to maximize
	 * lnRsize and lnTsize
	 */

	sz = (memSize - (sizeof (ln_rmd) + sizeof (ln_tmd) + sizeof (ln_ib)))
	       / ((2 * bufSize) + sizeof (ln_rmd) + sizeof (ln_tmd));

	sz >>= 1;		/* adjust for roundoff */

	for (lnRsize = 0; sz != 0; lnRsize++, sz >>= 1)
	    ;

	lnTsize = lnRsize;	/* lnTsize = lnRsize for convenience */
	}

    /* limit ring sizes to reasonable values */

    if (lnRsize < 2)
        lnRsize = 2;		/* 4 buffers is reasonable min */

    if (lnRsize > 7)
        lnRsize = 7;		/* 128 buffers is max for chip */

    /* limit ring sizes to reasonable values */

    if (lnTsize < 2)
        lnTsize = 2;		/* 4 buffers is reasonable min */

    if (lnTsize > 7)
        lnTsize = 7;		/* 128 buffers is max for chip */


    /* calculate the total size of LANCE memory pool.  (basic softc structure
     * is just allocated from free memory pool since LANCE never references).
     */
    sz = ((sizeof (ln_ib))        + (((1 << lnRsize) + 1) * sizeof (ln_rmd)) +
	  (bufSize << lnRsize) + (((1 << lnTsize) + 1) * sizeof (ln_tmd)) +
	  (bufSize << lnTsize));

    sz = sz + 6;			/* allow for alignment adjustment */

    if ((int) memAdrs == NONE)	/* if -1 then allocate memPool from free pool */
	{
	memPool = (char *) malloc (sz);	/* allocate memory */

	if ((int)memPool == NULL)
	    return (ERROR);		/* bail out if we don't get memory */
	}
    else
	{
	if (memSize < sz)
	    return (ERROR);	/* bail out if memSize specified is too small */

	memPool = memAdrs;	/* set the beginning of pool */
	}

    /*                        memPool partitioning
     *                        --------------------
     *
     *                                                 LOW MEMORY
     *
     *   		memPool,ls->ib	|-------------------------------------|
     *					|				      |
     *					|         (sizeof (ln_ib))	      |
     *					|				      |
     *   		  ls->ls_rring  |-------------------------------------|
     *					|				      |
     * 					| ((1 << lnRsize) + 1)*sizeof (ln_rmd)|
     *					|				      |
     *   	   ls->rmd_ring.r_bufs	|-------------------------------------|
     *					|				      |
     * 					|       (bufSize << lnRsize)          |
     *					|				      |
     *  		  ls->ls_tring	|-------------------------------------|
     *					|				      |
     *					| ((1 << lnTsize) + 1)*sizeof (ln_tmd)|
     *					|				      |
     *   	   ls->tmd_ring.t_bufs	|-------------------------------------|
     *					|				      |
     * 					|       (bufSize << lnTsize)          |
     *					|				      |
     *  			        |-------------------------------------|
     *
     *                                                HIGH MEMORY
     */

    /* allocate basic softc structure */

    ls = (DRV_CTRL *)malloc (sizeof (DRV_CTRL));
    if (ls == NULL)
	{
	if ((int) memAdrs == NONE)	/* if we malloced memPool */
	    (void) free (memPool);	/* free the memPool */
	return (ERROR);
	}

    /* zero out entire softc structure */
    bzero ( (char *)ls, sizeof (DRV_CTRL));

    /* fill in high byte of memory base (A24:A31) and data port width */

    ls->memBase  = (char *)((ULONG)memPool & 0xff000000);
    ls->memWidth = memWidth;

    if ((int) memAdrs == NONE)
        ls->ls_flags |= LS_MEM_ALLOC_FLAG;
    
    ls->bufSize = bufSize;

    /* if memWidth is NONE (we can copy any byte size/boundaries) and
     * bufSize is big enough to hold the biggest ethernet frame
     * we can loan out rmds.  On systems that cannot afford to have
     * big enough RMD's, LANCE will use data-chaining on receive
     * buffers.  Our current implementation of RMD loans does not work
     * with receive side data-chains.
     */

    if (ls->memWidth == NONE && ls->bufSize == LN_BUFSIZE)
	{
	int	ix;
	char	*pBuf;
	
        ls->canLoanRmds		= TRUE;
	ls->nFree		= LN_NUM_RESERVES;

	if ((pBuf = (char *) malloc ((u_int) (LN_NUM_RESERVES * bufSize))) 
	    == NULL)
	    {
	    (void) free (memPool);
	    (void) free ((char *) ls);
	    return (ERROR);
	    }

	for (ix = 0; ix < LN_NUM_RESERVES; ix++)
	    ls->freeBufs [ix] = (char *) ((int) pBuf + (bufSize * ix));
	}
    else
        ls->canLoanRmds = FALSE;

    /* allocate initialization block */

    ls->ib = (ln_ib *)memPool;
    sz = sizeof (ln_ib);		/* size of initialization block */
    bzero ((char *)ls->ib, (int) sz);	/* zero out entire ib */

    /* allocate receive message descriptor (RMD) ring structure */

    ls->ls_rpo2 = lnRsize;		/* remember for lnConfig */
    ls->ls_rsize = (1 << lnRsize);	/* receive msg descriptor ring size */

    /* leave room to adjust low three bits */

    ls->ls_rring = (ln_rmd *) ((int)ls->ib + sz);   /* of pointer to be 000 */
    sz = ((1 << lnRsize) + 1) * sizeof (ln_rmd);
    bzero ((char *)ls->ls_rring, (int)sz);	/* zero out entire RMD ring */

    /* allocate receive buffer space */

    sz = sz + 2;		/* this makes the rx_buffers un-aligned, */
                                /* but with ether-header prepended, they are */

    ls->rmd_ring.r_bufs = (char *)((int)ls->ls_rring + sz);
    sz = (bufSize << lnRsize);	 /* room for all the receive buffers */
    bzero (ls->rmd_ring.r_bufs, (int)sz);/* zero out entire rbuf area */

    /* allocate transmit message descriptor (TMD) ring structure */

    ls->ls_tpo2 = lnTsize;		/* remember for lnConfig */
    ls->ls_tsize = (1 << lnTsize);	/* transmit msg descriptor ring size */

    sz = sz + 2;			/* make ls_tring aligned again */

    ls->ls_tring = (ln_tmd *) ((int)ls->rmd_ring.r_bufs + sz);
    sz = ((1 << lnTsize) + 1) * sizeof (ln_tmd);
    bzero ((char *)ls->ls_tring, (int)sz);	/* zero out entire TMD ring */

    /* allocate transmit buffer space */

    sz = sz + 2;	/* this makes the tx_buffers un-aligned, */
		        /* but with ether-header prepended, they are */

    ls->tmd_ring.t_bufs = (char *)((int)ls->ls_tring + sz);
    sz = (bufSize << lnTsize);	/* room for all the transmit buffers */
    bzero (ls->tmd_ring.t_bufs, (int)sz);	/* zero out entire tbuf area */
    ls_softc [unit] = ls;		/* remember address for this unit */

    /* initialize device structure */

    ls->ivec    	= ivec;			/* interrupt vector */
    ls->ilevel		= ilevel;		/* interrupt level */
    ls->devAdrs		= (LN_DEVICE *)devAdrs;	/* LANCE i/o address */

    /* copy the enet address into the softc */

    bcopy ((char *) lnsgiEnetAddr, (char *)ls->ls_enaddr, sizeof (ls->ls_enaddr));

    ifp = &ls->ls_if;

    /* attach and enable the LANCE interrupt service routine to vector */

    lnChipReset (ls);				/* reset LANCE */

    (void) intConnect (INUM_TO_IVEC (ivec), lnInt, (int)ls);

#ifdef BSD43_DRIVER
    ether_attach (ifp, unit, "lnsgi", (FUNCPTR) lnInit, (FUNCPTR) lnIoctl, 
		  (FUNCPTR) lnOutput, (FUNCPTR) lnReset);
#else
    ether_attach (
                 ifp, 
                 unit, 
                 "lnsgi",
                 (FUNCPTR) lnInit, 
                 (FUNCPTR) lnIoctl, 
		 (FUNCPTR) ether_output, 
                 (FUNCPTR) lnReset
                 );
    ifp->if_start = (FUNCPTR) lnStartOutput; 
#endif

    sysLanIntEnable (ilevel);

    lnInit (unit);

    return (OK);
    }

/*******************************************************************************
*
* lnReset - reset the interface
*
* Mark interface as inactive & reset the chip
*/

LOCAL void lnReset
    (
    int unit 
    )
    {
    FAST DRV_CTRL * 	ls = ls_softc [unit];

    ls->ls_if.if_flags &= ~IFF_RUNNING;
    lnChipReset (ls);				/* reset LANCE */
    }

/*******************************************************************************
*
* lnInit -
*
* Initialization of interface; clear recorded pending operations.
* Called during driver attach routine at boot time and by interrupt service
* routines when fatal errors occur.
*/

LOCAL int lnInit
    (
    int unit 
    )
    {
    FAST DRV_CTRL * 	ls = ls_softc [unit];
    FAST struct ifnet * ifp = &ls->ls_if;

    ifp->if_flags &= ~(IFF_UP | IFF_RUNNING | IFF_PROMISC);

    lnChipReset (ls);				/* disable chip during init */
    lnConfig (ls);				/* reset all ring structures */

    ifp->if_flags |= (IFF_UP | IFF_RUNNING);

    if (ls->ls_flags & LS_PROMISCUOUS_FLAG)
	ifp->if_flags |= (IFF_PROMISC);

    lnChipInit (ls);			/* on return LANCE is running */

#ifdef BSD43_DRIVER
    lnStartOutput (ls->ls_if.if_unit);	/* tell chip about any pending output */
#else
    lnStartOutput (ls);
#endif

    return (0);
    }

/*******************************************************************************
*
* lnConfig - fill in initialization block with mode information.
*
* Fill in all fields in the Initialization Block with relevant values.
* In addition, fill in all fields in Transmit Message Descriptor ring
* and Receive Message Descriptor ring.
*/

LOCAL void lnConfig
    (
    FAST struct ls_softc  *ls 
    )
    {
    FAST ln_rmd	  *rmd;
    FAST ln_tmd	  *tmd;
    FAST char	  *buf;
    FAST ln_ib	  *ib;
    FAST int	   index;
    int		   i;

    rmd = ls->ls_rring;			/* receive message descriptor ring */
    rmd = (ln_rmd *)(((int)rmd + 7) & ~7);	/* low 3 bits must be 000 */
    ls->ls_rring = rmd;				/* fix softc as well */
    sysWbFlush ();

    buf = ls->rmd_ring.r_bufs; 
    for (i = 0; i < ls->ls_rsize; i++)
        {

        /* fill VIP10 dma table entry */
	index = RBUF_INDEX + i;
	pEmap [EMAP_OFFSET(index)] = (u_short) EMAP_VALUE(buf);
	sysWbFlush ();

	/* bits 15:00 of buffer address */
	rmd->rbuf_ladr = (u_short) LNADDR(index, buf);

	/* bits 23:16 of buffer address */
	rmd->lnRMD1 = (u_short) ((u_int) (LNADDR(index, buf) >> 16) & 0xff);
	rmd->rbuf_bcnt = -(ls->bufSize);/* neg of buffer byte count */
	rmd->rbuf_mcnt = 0;		/* no message byte count yet */
	rmd->lnRMD1 |= lnrmd1_OWN;	/* buffer now owned by LANCE */
	sysWbFlush ();				/* make sure it gets out */
	rmd++;				/* step to next message descriptor */
	buf += (ls->bufSize);		/* step to start of next buffer */

	ls->loanRefCnt [i] = (u_char) 0;
	}

    ls->ls_rindex = 0;			/* LANCE will use at start of ring */
    sysWbFlush ();

    tmd = ls->ls_tring;			/* transmit message descriptor ring */
    tmd = (ln_tmd *)(((int)tmd + 7) & ~7);	/* low 3 bits must be 000 */
    ls->ls_tring = tmd;				/* fix softc as well */
    sysWbFlush ();
    buf = ls->tmd_ring.t_bufs; 

    for (i = 0; i < ls->ls_tsize; i++)
        {
	/* fill VIP10 dma table entry */
	index = TBUF_INDEX + i;
        pEmap [EMAP_OFFSET(index)] = (u_short) EMAP_VALUE(buf);
	sysWbFlush ();

	/* bits 15:00 of buffer address */
	tmd->tbuf_ladr = (u_short) LNADDR(index, buf);

	/* bits 23:16 of buffer address */
	tmd->lnTMD1 = (u_short) ((u_int) (LNADDR(index, buf) >> 16) & 0xff);
	tmd->tbuf_bcnt = 0;		/* no message byte count yet */
	tmd->lnTMD3 = 0;		/* no error status yet */
	tmd->lnTMD1 |= lntmd1_ENP;	/* buffer is end of packet */
	tmd->lnTMD1 |= lntmd1_STP;	/* buffer is start of packet */
	sysWbFlush ();				/* make sure it gets out */
	tmd++;				/* step to next message descriptor */
	buf += (ls->bufSize);		/* step to start of next buffer */
	}

    ls->ls_tindex = 0;			/* LANCE will use at start of ring */
    ls->ls_dindex = 0;			/* buffer disposal will lag tindex */
    sysWbFlush ();

    ib = ls->ib;

    /* fill VIP10 dma table entry for initialization block */
    index = IBUF_INDEX;
    pEmap [EMAP_OFFSET(index)] = (u_short) EMAP_VALUE(ib);
    sysWbFlush ();

    if (ls->ls_flags & LS_PROMISCUOUS_FLAG)
        ib->lnIBMode = 0x8000;	/* chip will be in promiscuous receive mode */
    else
        ib->lnIBMode = 0;	/* chip will be in normal receive mode */

    swab ((char *)ls->ls_enaddr, ib->lnIBPadr, 6);

    /* 
    *  Fill VIP10 RMD Desc and Bufs. Two buffers are allocated to accomadate
    *  the crossing page boundries.  RMD Desc and buffers should come right 
    *  after IB 
    */

    index++;
    pEmap [EMAP_OFFSET(index)] = (u_short) EMAP_VALUE(ls->ls_rring);
    sysWbFlush ();

    pEmap [EMAP_OFFSET(index + 1)] = (u_short) (EMAP_VALUE(ls->ls_rring) + 1);
    sysWbFlush ();

    ib->lnIBRdraLow = (u_short) LNADDR(index, ls->ls_rring);
    ib->lnIBRdraHigh = (u_short) ((u_int) (LNADDR(index, ls->ls_rring) >> 16) 
			& 0xff) | (ls->ls_rpo2 << 13);
    sysWbFlush ();

    index += 2;
    pEmap [EMAP_OFFSET(index)] = (u_short) EMAP_VALUE(ls->ls_tring);
    sysWbFlush ();

    pEmap [EMAP_OFFSET(index + 1)] = (u_short) (EMAP_VALUE(ls->ls_tring) + 1);
    sysWbFlush ();

    ib->lnIBTdraLow = (u_short) LNADDR(index, ls->ls_tring);
    ib->lnIBTdraHigh = (u_short) ((u_int) (LNADDR(index, ls->ls_tring) >> 16) 
			& 0xff) | (ls->ls_tpo2 << 13);
    sysWbFlush ();
    }
/*******************************************************************************
*
* lnInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*/

LOCAL void lnInt
    (
    FAST struct ls_softc  *ls 
    )
    {
    FAST ln_rmd		*rmd;
    FAST ln_tmd		*tmd;
    FAST int 		*pDindex;
    FAST int		*pTindex;
    FAST int		*pTsize;
    FAST ln_tmd		*pTring;
    FAST u_short	stat;
   
    if (!((stat = lnCsrIntRead (ls, 0)) & lncsr_INTR))
        {
        return;
        }

    /*
     * enable interrupts, clear receive and/or transmit interrupts, 
     * and clear any errors that may be set.
     */

    lnCsrIntWrite (ls, 0, ((stat & 
	    (lncsr_BABL | lncsr_CERR | lncsr_MISS | lncsr_MERR |
	     lncsr_RINT | lncsr_TINT | lncsr_IDON)) | lncsr_INEA));

    /*
     * have task level handle csr0 errors;
     */

    if (stat & lncsr_ERR)
        {
	lnHandleCsr0Err (ls, stat);
	}

    /* have task level handle any input packets */

    if ((stat & lncsr_RINT) && (stat & lncsr_RXON))
	{
	if ((rmd = lnGetFullRMD (ls)) != NULL)
	    {
	    /* discard error input ASAP - don't queue it */
	    if (rmd->lnRMD1 & RMD_ERR) 
		{
		if (lnsgiLogCount && (ls->ls_if.if_ierrors % lnsgiLogCount) == 0)
	            logMsg ("ln%d lnInt: ERROR rmd1=0x%x mcnt %d\n", 
			    ls->ls_if.if_unit, rmd->lnRMD1 & 0xff00, 
			    rmd->rbuf_mcnt, 0, 0, 0);

		ls->ls_if.if_ierrors++;

		ls->ls_rindex = (ls->ls_rindex + 1) & (ls->ls_rsize - 1);
    		sysWbFlush ();
		
		rmd->lnRMD1 &= ~RMD_ERR;
		LN_RMD_GIVE_TO_LANCE (rmd); sysWbFlush ();
		}
	    else if ((ls->ls_flags & LS_RCV_HANDLING_FLAG) == 0)
		{
		ls->ls_flags |= LS_RCV_HANDLING_FLAG;
		(void) netJobAdd ((FUNCPTR) lnHandleRecvInt, (int) ls, 
				  0, 0, 0, 0);
		}
	    }
	}

    /* 
     * Did LANCE update any of the TMD's?
     * If not then don't bother continuing with transmitter stuff 
     */
    
    if (!(stat & lncsr_TINT))
	{	/* common return path */
        return;
	}

    pDindex = &ls->ls_dindex;
    pTindex = &ls->ls_tindex;
    pTsize  = &ls->ls_tsize;
    pTring  = ls->ls_tring;

    while (*pDindex != *pTindex)
        {
	/* disposal has not caught up */

	tmd = pTring + *pDindex;

        cacheClearEntry(tmd, sizeof(ln_tmd));

	/* if the buffer is still owned by LANCE, don't touch it */

	if (tmd->lnTMD1 & TMD_OWN)
	    break;

	/*
	 * tbuf_err (TMD1.ERR) is an "OR" of LCOL, LCAR, UFLO or RTRY.
	 * Note that BUFF is not indicated in TMD1.ERR.
	 * We should therefore check both tbuf_err and tbuf_buff
	 * here for error conditions.
	 */

	if ((tmd->lnTMD1 & TMD_ERR) || (tmd->lnTMD3 & TMD_BUFF))
	    {

	    ls->ls_if.if_oerrors++;	/* output error */
	    ls->ls_if.if_opackets--;

	    /* check for no carrier */

	    if (tmd->lnTMD3 & TMD_LCAR)
		logMsg ("ln%d lnInt: no carrier\n", ls->ls_if.if_unit, 0, 0, 0, 0, 0);

	    /* every lnsgiLogCount errors show other interesting bits of tmd3 */

	    if ((tmd->lnTMD3 & 0xfc00) && lnsgiLogCount &&
		(ls->ls_if.if_oerrors % lnsgiLogCount) == 0)
		logMsg ("ln%d lnInt: xmit error, status(tmd3)=0x%x, err count=%d\n",
			ls->ls_if.if_unit, tmd->lnTMD3 & 0xfc00,
			ls->ls_if.if_oerrors, 0, 0, 0);

	    /* restart chip on fatal errors */

	    /*
	     * The following code handles the situation where the transmitter 
	     * shuts down due to an underflow error.  This is a situation that 
	     * will occur if the DMA cannot keep up with the transmitter.
	     * It will occur if the LANCE is being held off from DMA access 
	     * for too long or due to significant memory latency.  DRAM fresh
	     * or slow memory could influence this.  Many implementation use a 
	     * dedicated LANCE buffer.  This can be static RAM to eliminate 
	     * refresh conflicts or dual-port RAM so that the LANCE can have 
	     * free run of this memory during its DMA transfers.
	     */

	    if ((tmd->lnTMD3 & TMD_BUFF) || (tmd->lnTMD3 & TMD_UFLO))
		{
/*		logMsg ("ln%d lnInt: xmit error, tmd3=0x%x. (%s/%d)\n",
 *			ls->ls_if.if_unit, tmd->tbuf_tmd3 & 0xffff,
 *                        __FILE__, __LINE__);
 */
 		lnCsrIntWrite (ls, 0, lncsr_STOP);	/* stop the LANCE */
		(void) netJobAdd ((FUNCPTR) lnInit, ls->ls_if.if_unit, 
				  0, 0, 0, 0);
		return;
		}
	    }

	tmd->lnTMD1 &= 0xff;		/* clear high byte */
	tmd->lnTMD3 = 0;		/* clear all error & stat stuff */
	sysWbFlush ();
	
	/* now bump the tmd disposal index pointer around the ring */

	*pDindex = (*pDindex + 1) & (*pTsize - 1);
	}

    if (ls->ls_if.if_snd.ifq_head != NULL &&
	(ls->ls_flags & LS_START_OUTPUT_FLAG) == 0)
	{
	ls->ls_flags |= LS_START_OUTPUT_FLAG;
#ifdef BSD43_DRIVER
        (void) netJobAdd ( (FUNCPTR)lnStartOutput, ls->ls_if.if_unit, 
			  0, 0, 0, 0);
#else
        (void) netJobAdd ( (FUNCPTR)lnStartOutput, (int)ls, 0, 0, 0, 0);
#endif
	}
    }

/*******************************************************************************
*
* lnHandleCsr0Err - task level interrupt service for csr0 errors
*/

LOCAL void lnHandleCsr0Err
    (
    FAST struct ls_softc	*ls,
    u_short			status 
    )
    {
    if (status & lncsr_CERR)	/* collision error */
	ls->ls_if.if_collisions++;

    if (status & (lncsr_BABL | lncsr_MISS | lncsr_MERR))
	{
	++ls->csr0Errs;
	++ls->ls_if.if_ierrors;

	/* every lnsgiLogCount errors show interesting bits of csr0 */

	if (lnsgiLogCount && (ls->csr0Errs % lnsgiLogCount) == 0)
	    logMsg ("ln%d lnInt: csr0 error, csr0=0x%x, err count=%d\n",
		    ls->ls_if.if_unit, status, ls->csr0Errs, 0, 0, 0);

	/*
	 * restart chip on fatal error 
	 */
	if (status & lncsr_MERR)	/* memory error */
	    (void) netJobAdd ((FUNCPTR) lnInit, ls->ls_if.if_unit, 0, 0, 0, 0);
	}
    }

/*******************************************************************************
*
* lnHandleRecvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/

LOCAL void lnHandleRecvInt
    (
    FAST struct ls_softc *ls 
    )
    {
    FAST ln_rmd         *rmd;
    FAST int oldIndex;

    do
	{
	ls->ls_flags |= LS_RCV_HANDLING_FLAG;

	while ((rmd = lnGetFullRMD (ls)) != NULL)
	    {				
	    /* RMD available */

	    oldIndex = ls->ls_rindex;

	    if (lnRecv (ls, rmd) == OK)
		{
		for (; oldIndex != ls->ls_rindex; 
		     oldIndex = (oldIndex + 1) & (ls->ls_rsize - 1))
		    {
		    rmd = ls->ls_rring + oldIndex;
    		    cacheClearEntry (rmd, sizeof(ln_rmd));
		    LN_RMD_GIVE_TO_LANCE (rmd); sysWbFlush ();
		    }
		}
	    else
	        break;
	    }
	
	/*
	 * There is a RACE right here.  The ISR could add a receive packet
	 * and check the boolean below, and decide to exit.  Thus the
	 * packet could be dropped if we don't double check before we
	 * return.
	 */

	ls->ls_flags &= ~LS_RCV_HANDLING_FLAG;
	}
    while (lnGetFullRMD (ls) != NULL);	/* this double check solves the RACE */
    }
/*******************************************************************************
*
* lnRecv -
*
* Process Ethernet receive completion:
*	If input error just drop packet.
*	Otherwise purge input buffered data path and examine 
*	packet to determine type.  If can't determine length
*	from type, then have to drop packet.  Otherwise decapsulate
*	packet based on type and pass to type-specific higher-level
*	input routine.
*
* RETURNS: ERROR if RMD shouldn't be returned back to LANCE yet.
*          Otherwise, returns OK to return the RMD back to LANCE.
*/

LOCAL STATUS lnRecv
    (
    struct ls_softc	   *ls,
    FAST ln_rmd		   *rmd 
    )
    {
    FAST struct ether_header	*eh;
    FAST struct mbuf		*m;
    FAST u_char			*pData;
    FAST ln_rmd			*nextRmd;
    int				ix, len;
    int off = 0;
#ifdef BSD43_DRIVER
    u_short			ether_type;
#endif

    /*
     * If both STP and ENP are set, LANCE didn't try to
     * use data chaining.
     */

    if (((rmd->lnRMD1 & lnrmd1_STP) == lnrmd1_STP) &&
	((rmd->lnRMD1 & lnrmd1_ENP) == lnrmd1_ENP))
	{
	len = rmd->rbuf_mcnt;
	ls->ls_rindex = (ls->ls_rindex + 1) & (ls->ls_rsize - 1);
    	sysWbFlush ();
	}
    else if (((rmd->lnRMD1 & lnrmd1_STP) == lnrmd1_STP) &&
	     ((rmd->lnRMD1 & lnrmd1_ENP) == 0))
	{
	/* STP is set but ENP is not set, so LANCE is trying
	 * to use data chaining.  We scan the following sequence of
	 * RMD's to find the last buffer in this data chain.
	 */

	ix = (ls->ls_rindex + 1) & (ls->ls_rsize - 1);

	do
	    {
	    nextRmd = ls->ls_rring + ix;		/* next rmd */

	    /* Return if the next rmd is not ready.  Try later. */

   	    cacheClearEntry(rmd, sizeof(ln_rmd));

	    if ((nextRmd->lnRMD1 & lnrmd1_OWN) != 0)
	        return (ERROR);

	    ix = (ix + 1) & (ls->ls_rsize - 1);
	    }
	/* end of packet? */
	while ((nextRmd->lnRMD1 & lnrmd1_ENP) == 0);	

	/* This last RMD contains the valid MCNT. */

	ls->ls_rindex = ix;
        sysWbFlush ();
	len = nextRmd->rbuf_mcnt;
	}
    else
	{
	ls->ls_rindex = (ls->ls_rindex + 1) & (ls->ls_rsize - 1);
        sysWbFlush ();

	++ls->ls_if.if_ierrors;

	if (lnsgiLogCount && (ls->ls_if.if_ierrors % lnsgiLogCount) == 0)
	    logMsg ("ln%d lnRecv: receive error, stp %d enp %d\n",
		    ls->ls_if.if_unit,
	            (rmd->lnRMD1 & lnrmd1_STP) >> 9,
	            (rmd->lnRMD1 & lnrmd1_ENP) >> 8, 0, 0, 0);

	return (OK);		/* intentional */
	}

    /*
    eh = (struct ether_header *)(rmd->rbuf_ladr | (rmd->rbuf_hadr << 16));
    ItoK (eh, struct ether_header *, ls->memBase);
    */

    ++ls->ls_if.if_ipackets;    /* bump statistic */

    eh = (struct ether_header *) ((u_int) ls->rmd_ring.r_bufs + 
	(u_int) ((((u_int)rmd - (u_int)ls->ls_rring) / sizeof(ln_rmd)) 
	* ls->bufSize));

    cacheClearEntry (eh, len);

    /* call input hook if any */

    if ((etherInputHookRtn != NULL) &&
	(* etherInputHookRtn) (&ls->ls_if, (char *)eh, len))
	return (OK);

#ifdef BSD43_DRIVER

    /* This legacy code is not correct for the BSD 4.4 stack. It would
     * also treat multicast addresses like alien packets, and not send
     * them up the stack. The higher-level protocols in the new stack
     * can handle these addresses, and will discard them if they do not
     * match an existing multicast group.
     */

    /* do software filter if controller is in promiscuous mode */


    if (ls->ls_flags & LS_PROMISCUOUS_FLAG)
        if ( (bcmp ( (char *)eh->ether_dhost,	/* not our adrs? */
                     (char *)ls->ls_enaddr,
                    sizeof (eh->ether_dhost)) != 0) &&
             (bcmp ( (char *)eh->ether_dhost,	/* not broadcast? */
                     (char *)etherbroadcastaddr,
                    sizeof (eh->ether_dhost)) != 0))
            return (OK);
#endif

    if (len >= sizeof (struct ether_header))
        len -= sizeof (struct ether_header);
    else
        len = 0;

    pData = ( (u_char *)eh) + (sizeof (struct ether_header)); 

#ifdef BSD43_DRIVER
    check_trailer (eh, pData, &len, &off, &ls->ls_if);

    if (len == 0)
	return (OK);

    ether_type = eh->ether_type;
#endif

    m = NULL;

    /*
     * we can loan out receive buffers from LANCE receive ring if:
     *
     * 1) canLoanRmd is TRUE.  canLoanRmd is set to TRUE if memWidth
     *    is NONE (no special restriction in copying data in terms of
     *    size and boundary conditions) and each of the buffers in the
     *    LANCE ring is big enough to hold the maximum sized ethernet
     *    frame (data-chaining is not being used for the input).
     * 2) trailer protocol is not being used for the given input ethernet frame.
     * 3) there is available free buffers that can be used to replace the
     *    rbuf to be loaned out in the free list 'freeBufs'.
     * 4) size of the input ethernet frame is large enough to be used with
     *    clustering.
     */
    
    if (ls->canLoanRmds && off == 0 && ls->nFree > 0 && USE_CLUSTER (len))
	{
        m = build_cluster (pData, len, &ls->ls_if, MC_LANCE,
                           &(ls->loanRefCnt [ls->ls_rindex]),
                           lnRmdFree, (int) ls, (int) eh, NULL);
        if (m != NULL)
            {
            FAST char *pBuf;

            /* get a free buffer from the free list to replace the
             * rbuf loaned out.  replace the rbuf pointers in the RMD
             * to point to the new buffer.
	     */
	    
            pBuf = ls->freeBufs [--ls->nFree];
	    
            rmd->rbuf_ladr = (int) pBuf;
            rmd->lnRMD1 = (rmd->lnRMD1 & ~lnrmd1_HADR)
	                    | (((int) pBuf >> 16) & lnrmd1_HADR);
            sysWbFlush ();
            }
        }

    if (m == NULL)
#ifdef BSD43_DRIVER
        /*
         * Instead of calling copy_to_mbufs (), we call bcopy_to_mbufs ()
         * with ls->memWidth as an addtional argument to specify unit of a
         * copy op.
         *
         * Most drivers would use macro copy_to_mbufs (), which will in turn
         * call bcopy_to_mbufs () telling it to use the normal bcopy ().  Some
         * of the on-board LANCE hardware implementations require that you
         * copy the data by certain number of bytes from dedicated memory
         * to system memory, so LANCE driver has a ln_bcopy () routine that
         * conforms to this requirement.
         */

        m = bcopy_to_mbufs (pData, len, off, (struct ifnet *) &ls->ls_if,
                            ls->memWidth);
#else
        m = copy_to_mbufs (pData, len, 0, &ls->ls_if);
#endif

    if (m == NULL)
        ls->ls_if.if_ierrors++;
    else
#ifdef BSD43_DRIVER
        do_protocol_with_type (ether_type, m, &ls->ls_ac, len);
#else
        do_protocol (eh, m, &ls->ls_ac, len);
#endif        

    return (OK);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* lnOutput -
*
* Ethernet output routine.
* Encapsulate a packet of type family for the local net.
* Use trailer local net encapsulation if enough data in first
* packet leaves a multiple of 512 bytes of data in remainder.
*/

LOCAL int lnOutput
    (
    FAST struct ifnet	*ifp,
    FAST struct mbuf	*m0,
    struct sockaddr	*dst 
    )
    {
    return (ether_output (ifp, m0, dst, (FUNCPTR) lnStartOutput, 
			  &ls_softc [ifp->if_unit]->ls_ac));
    }
#endif

/*******************************************************************************
*
* lnStartOutput - start pending output
*
* Start output to LANCE.
* Queue up all pending datagrams for which transmit buffers are available.
* Kick start the transmitter to avoid the polling interval latency.
* This routine is called by lnInit () and lnOutput ().
* It is very important that this routine be executed with splimp set.
* If this is not done, another task could allocate the same tmd!
*/

#ifdef BSD43_DRIVER
LOCAL void lnStartOutput
    (
    int unit 
    )
    {
    FAST DRV_CTRL * 	ls = ls_softc [unit];
#else
LOCAL void lnStartOutput
    (
    DRV_CTRL * 	ls
    )
    {
#endif
    FAST struct mbuf	*m;
    FAST ln_tmd		*tmd;
    FAST char		*buf;
    FAST int		len;
    FAST int		sx;
    FAST int		oldLevel;

    sx = splimp ();

    ls->ls_flags |= LS_START_OUTPUT_FLAG;

    /* Loop placing message buffers in output ring until no more or no room */

    while (ls->ls_if.if_snd.ifq_head != NULL)
        {

	/* there is something to send so get a transmit buffer */
	if ((tmd = lnGetFreeTMD (ls)) == NULL)	
	    break;

	IF_DEQUEUE (&ls->ls_if.if_snd, m); /* get head of next mbuf chain */

        buf = (char *) ((u_int) ls->tmd_ring.t_bufs + 
	    (u_int) ((((u_int)tmd - (u_int)ls->ls_tring) / sizeof(ln_tmd)) 
	    * ls->bufSize));

	/* Copy packet to write buffer */

#ifdef BSD43_DRIVER
	bcopy_from_mbufs (buf, m, len, ls->memWidth);
#else
        copy_from_mbufs (buf, m, len);
#endif

	len = max (ETHERSMALL, len);

	/* call output hook if any */
    
	if ((etherOutputHookRtn != NULL) &&
	    (* etherOutputHookRtn) (&ls->ls_if, buf, len))
	    continue;

	/* place a transmit request */
    
	oldLevel = intLock ();		/* disable ints during update */
	
	tmd->lnTMD3 = 0;		/* clear buffer error status */
	tmd->tbuf_bcnt = -len;		/* negative message byte count */
	tmd->lnTMD1 |= lntmd1_ENP;	/* buffer is end of packet */
	tmd->lnTMD1 |= lntmd1_STP;	/* buffer is start of packet */
        tmd->lnTMD1 &= ~lntmd1_DEF;   /* clear status bit */
        tmd->lnTMD1 &= ~lntmd1_MORE;
        tmd->lnTMD1 &= ~lntmd1_ERR;
        tmd->lnTMD1 |= lntmd1_OWN;
	sysWbFlush ();
	
	intUnlock (oldLevel);	/* now lnInt won't get confused */

	ls->ls_tindex = (ls->ls_tindex + 1) & (ls->ls_tsize - 1);
        sysWbFlush ();

#ifndef BSD43_DRIVER    /* BSD 4.4 ether_output() doesn't bump statistic. */
        ls->ls_if.if_opackets++;
#endif
        }

/*
 * to minimize chip accesses, don't kick start the transmitter.
 * it will start after the next internal poll.
 */

    ls->ls_flags &= ~LS_START_OUTPUT_FLAG;

    splx (sx);
    }

/*******************************************************************************
*
* lnIoctl -
*
* Process an ioctl request.
*/

LOCAL int lnIoctl
    (
    FAST struct ifnet	*ifp,
    int			 cmd,
    caddr_t		 data 
    )
    {
    int			   unit = ifp->if_unit;
    FAST struct ls_softc  *ls = ls_softc [unit];
    int			   s = splimp ();
    int			   error = 0;

    switch (cmd)
	{
	case SIOCSIFADDR:
            ((struct arpcom *)ifp)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (ifp, &IA_SIN (data)->sin_addr);
	    break;

	case SIOCGIFADDR:
	    bcopy((caddr_t) ls->ls_enaddr,
		  (caddr_t) ((struct ifreq *)data)->ifr_addr.sa_data, 6);
	    break;

	case SIOCGIFFLAGS:
	    *(short *)data = ifp->if_flags;
	    break;

	case SIOCSIFFLAGS:
	    ls->ls_if.if_flags = ifp->if_flags;

	    if (ifp->if_flags & IFF_PROMISC)
	        ls->ls_flags |= LS_PROMISCUOUS_FLAG;
	    else
	        ls->ls_flags &= ~LS_PROMISCUOUS_FLAG;

	    if (ifp->if_flags & IFF_UP)
		ls->ls_if.if_flags |= (IFF_UP|IFF_RUNNING);
	    else
	        ls->ls_if.if_flags &= ~(IFF_UP|IFF_RUNNING);
	    break;

	default:
	    error = EINVAL;
	}

    splx (s);
    return (error);
    }

/*******************************************************************************
*
* lnChipReset - hardware reset of chip (stop it)
*/

LOCAL void lnChipReset
    (
    struct ls_softc  *ls 
    )
    {
    lnCsrWrite (ls, 0, lncsr_STOP);     /* set the stop bit */
    }
/*******************************************************************************
*
* lnChipInit - hardware init of chip (init & start it)
*/

LOCAL void lnChipInit
    (
    FAST struct ls_softc  *ls 
    )
    {
    FAST u_short	stat;
    int			timeoutCount = 0;

    lnCsrWrite (ls, 0, lncsr_STOP);	/* set the stop bit */
				/* can't write csr1..3 if not stopped */
    lnCsrWrite (ls, 3, lncsr3_BSWP);

    lnCsrWrite (ls, 2, (u_short) ((u_int) (LNADDR(IBUF_INDEX, ls->ib) >> 16) 
			& 0xff));

    lnCsrWrite (ls, 1, (u_short) (LNADDR(IBUF_INDEX, ls->ib)));

    lnCsrWrite (ls, 0, lncsr_INIT);	/* init chip (read IB) */

    /* hang until Initialization done, error, or timeout */

    while (((stat = lnCsrRead (ls, 0)) & (lncsr_IDON | lncsr_ERR)) == 0)
        {
        if (timeoutCount++ > 0x10000) break;
        taskDelay(100);
        }

    /* log chip initialization failure */

    if (stat & lncsr_ERR)
        {
        logMsg ("ln%d: ERROR: Lance chip initialization failure, csr0=0x%x\n",
                ls->ls_if.if_unit, stat, 0, 0, 0, 0);
        }
    else if (timeoutCount >= 0x10000)
        {
        logMsg ("ln%d: ERROR: Lance chip initialization time-out\n",
		0, 0, 0, 0, 0, 0);
        }

    /* start chip */

    ls->ls_rindex = 0;		/* chip will start at beginning */
    ls->ls_tindex = 0;
    ls->ls_dindex = 0;
    sysWbFlush ();

    lnCsrWrite(ls, 0, lncsr_IDON | lncsr_INEA | lncsr_STRT);
					/* reset IDON state */
					/* enable interrupts from LANCE */
					/* start chip operation */
    }
/*******************************************************************************
*
* lnGetFreeTMD - get next available TMD
*/

LOCAL ln_tmd *lnGetFreeTMD
    (
    FAST struct ls_softc  *ls 
    )
    {
    FAST ln_tmd	  *tmd;

    /* check if buffer is available (owned by host) */
    /* also check for tindex overrun onto dindex */

    tmd = ls->ls_tring + ls->ls_tindex;

    cacheClearEntry(tmd, sizeof(ln_tmd));

    if (((tmd->lnTMD1 & lntmd1_OWN) != 0)
	|| (((ls->ls_tindex + 1) & (ls->ls_tsize - 1)) == ls->ls_dindex))
	{
        tmd = (ln_tmd *) NULL;
	}

    return (tmd);
    }
/*******************************************************************************
*
* lnGetFullRMD - get next received message RMD
*/

LOCAL ln_rmd *lnGetFullRMD
    (
    FAST struct ls_softc  *ls 
    )
    {
    FAST ln_rmd	  *rmd;

    /* check if buffer is full (owned by host) */

    rmd = ls->ls_rring + ls->ls_rindex;

    cacheClearEntry(rmd, sizeof(ln_rmd));

    if ((rmd->lnRMD1 & lnrmd1_OWN) == 0)
	{
        return (rmd);
	}
    else
	{
        return ((ln_rmd *) NULL);
	}
    }

/*******************************************************************************
*
* lnCsrRead - select and read a CSR register 
*
* NOTE: LANCE must already be stopped to read CSR1, CSR2 or CSR3.
*/

LOCAL u_short lnCsrRead
    (
    FAST struct ls_softc  *ls,	/* LANCE device software structure */
    int reg 			/* which CSR to be selected */
    )
    {

    FAST u_short value;
    FAST u_short *dv = (u_short *)ls->devAdrs;
    volatile u_short *pRAP;
    volatile u_short *pCSR0;

    pRAP  = (volatile u_short *) (dv + RAPOffset);
    pCSR0 = (volatile u_short *) (dv + CSROffset);

    *pRAP = reg;		/* select CSR */
    sysWbFlush ();

    value = *pCSR0;		/* get contents of CSR */

    return(value);		/* return contents of CSR */
    }

/*******************************************************************************
*
* lnCsrWrite - select and write a CSR register 
*
*/

LOCAL void lnCsrWrite
    (
    FAST struct ls_softc  *ls,	/* LANCE device software structure */
    int reg,			/* which CSR to be selected */
    u_short value 		/* value to write */
    )
    {
    FAST u_short *dv = (u_short *)ls->devAdrs;
    volatile u_short *pRAP;
    volatile u_short *pCSR0;

    pRAP  = (volatile u_short *) (dv + RAPOffset);
    pCSR0 = (volatile u_short *) (dv + CSROffset);

    *pRAP = reg;		/* select CSR */
    sysWbFlush ();

    *pCSR0 = value;		/* write value to CSR */
    sysWbFlush ();
    }

/*******************************************************************************
*
* lnCsrIntRead - select and read a CSR register in an ISR 
*
* NOTE: LANCE must already be stopped to read CSR1, CSR2 or CSR3.
*/

LOCAL u_short lnCsrIntRead
    (
    FAST struct ls_softc  *ls,	/* LANCE device software structure */
    FAST int reg 		/* which CSR to be selected */
    )
    {

    FAST u_short value;
    FAST u_short *dv = (u_short *)ls->devAdrs;
    volatile u_short *pRAP;
    volatile u_short *pCSR0;

    pRAP  = (volatile u_short *) (dv + RAPOffset);
    pCSR0 = (volatile u_short *) (dv + CSROffset);

    *pRAP = reg;		/* select CSR */
    sysWbFlush ();

    value = *pCSR0;		/* get contents of CSR */

    return(value);		/* return contents of CSR */
    }

/*******************************************************************************
*
* lnCsrIntWrite - select and write a CSR register in an ISR.
*
*/

LOCAL void lnCsrIntWrite
    (
    FAST struct ls_softc  *ls,	/* LANCE device software structure */
    FAST int reg,		/* which CSR to be selected */
    FAST u_short value 		/* value to write */
    )
    {
    FAST u_short *dv = (u_short *)ls->devAdrs;
    volatile u_short *pRAP;
    volatile u_short *pCSR0;

    pRAP  = (volatile u_short *) (dv + RAPOffset);
    pCSR0 = (volatile u_short *) (dv + CSROffset);

    *pRAP = reg;		/* select CSR */
    sysWbFlush ();

    *pCSR0 = value;		/* write value to CSR */
    sysWbFlush ();
    }

/*******************************************************************************
*
* lnRmdFree - called when loaned out rbuf is freed by MCLFREE.
*
* Puts the loaned out rbuf into free list to be reused as loan replacements.
*/

LOCAL void lnRmdFree
    (
    FAST struct ls_softc	*ls,
    FAST char 			*pBuf 
    )
    {
    ls->freeBufs [ls->nFree++] = pBuf;
    }
