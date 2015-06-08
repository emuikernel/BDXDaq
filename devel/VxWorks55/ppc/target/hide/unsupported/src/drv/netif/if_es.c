/* if_es.c - ETHERSTAR ethernet network interface for the DLAN */

/* Copyright 1984-1998 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01i,06oct98,jmp  doc: cleanup.
01h,19aug98,fle  doc : added routine description for esattach
01g,15jul97,spm  removed driver initialization from ioctl support (SPR #8831)
01f,19may97,spm  eliminated all compiler warnings
01e,15may97,spm  reverted to bcopy routines for mbufs in BSD 4.4
01d,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01c,26may93,ccc  updated for 5.1.
01b,03aug92,dyn  modified by Dynatem to support their new version.
01a,01apr91,ccc  written based on other drivers (if_xx) & ESTAR documentation.
*/

/*
DESCRIPTION
The only user callable routine is:

    esattach (unit, devAdrs, ivec, ilevel, memAdrs, memSize, memWidth)

To enable the use of the Dynatem DLAN board the following needs to be
added to the target's config.h file:

.CS
/@ Device controller I/O addresses: @/

  #define INCLUDE_ES
  #define INT_VEC_ES              0xc0	/@ interrupt vector @/
  #define INT_LVL_ES              5	/@ VMEbus interrupt level @/
  #define IO_ADRS_ES              ((char *) 0x70008000) /@ short I/O address @/
.CE

 LANCE defines 

.CS
  #define ES_POOL_ADRS            NONE    /@ malloc the pool @/
  #define ES_POOL_SIZE            NONE    /@ pool will be default size @/
  #define ES_DATA_WIDTH           NONE    /@ all data widths supported @/
  #define ES_PADDING              FALSE   /@ no padding for RAP @/
  #define ES_RING_BUF_SIZE        0       /@ use the default size @/

  #define INCLUDE_IF_USR
  #define IF_USR_NAME     "es"
  #define IF_USR_ATTACH   esattach
  #define IF_USR_ARG1     IO_ADRS_ES
  #define IF_USR_ARG2     INT_VEC_ES
  #define IF_USR_ARG3     INT_LVL_ES
  #define IF_USR_ARG4     ES_POOL_ADRS
  #define IF_USR_ARG5     ES_POOL_SIZE
  #define IF_USR_ARG6     ES_DATA_WIDTH
  #define IF_USR_ARG7     ES_PADDING
  #define IF_USR_ARG8     ES_RING_BUF_SIZE
.CE

The following needs to be added to sysLib.c under global definitions:

.CS
  char  esEnetAddr [6];
.CE

SEE ALSO: ifLib
*/

#include "vxWorks.h"
#include "stdlib.h"
#include "netLib.h"
#include "iv.h"
#include "memLib.h"
#include "ioctl.h"
#include "etherLib.h"
#include "logLib.h"
#include "intLib.h"
#include "sysLib.h"

#ifndef	DOC		/* don't include when building documentation */
#include "net/mbuf.h"
#endif	/* DOC */

#include "net/protosw.h"
#include "net/unixLib.h"
#include "socket.h"
#include "errno.h"

#include "net/if.h"
#include "net/route.h"

#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"
#include "netinet/if_ether.h"
#include "net/if_subr.h"

#include "drv/netif/if_es.h"

struct dlanptr {
	u_char *dlcr0;
	u_char *dlcr1;
	u_char *dlcr2;
	u_char *dlcr3;
	u_char *dlcr4;
	u_char *dlcr5;
	u_char *dlcr6;
	u_char *dlcr7;
	u_char *dlcr8;
	u_char *dlcr9;
	u_char *dlcr10;
	u_char *dlcr11;
	u_char *dlcr12;
	u_char *dlcr13;
	u_char *dlcr14;
	u_char *dlcr15;
} dlan;

IMPORT VOID ether_attach ();

#define MAX_ES		1	/* max number of ESTAR chips on board */

#define ItoKval(p,type,base)    ((type)((ULONG)(p) + (ULONG)(base)))
#define ItoK(p,type,base)       ((p) = ItoKval(p,type,base))

#define ES_RMD_GIVE_TO_ESTAR(pRmd) 					\
    { 									\
    pRmd->rbuf_rmd1 &= 0xff; 						\
    pRmd->rbuf_mcnt = 0; 						\
    pRmd->es_rmd1.es_rmd1b |= esrmd1_OWN;				\
    }

IMPORT unsigned char esEnetAddr []; /* ethernet address to load into lance */

/* globals */

int esLogCount = 0;

int dbg_read = 0;
int dbg_output = 0;
int recIndex = 0;
u_short newdlan;

/* locals */

LOCAL int esTsize = ES_TMD_TLEN;	/* deflt xmit ring size as power of 2 */
LOCAL int esRsize = ES_RMD_RLEN;	/* deflt recv ring size as power of 2 */

LOCAL struct ls_softc  *ls_softc [MAX_ES];

/* forward declarations */

LOCAL VOID		esInt ();
LOCAL VOID		esHandleRecvInt ();

LOCAL int		esInit ();
#ifdef BSD43_DRIVER
LOCAL int		esOutput ();
#endif
LOCAL int		esIoctl ();
LOCAL VOID		esReset ();
#ifdef BSD43_DRIVER
LOCAL VOID		esStartOutput ();
#else
LOCAL VOID		esStartOutput (struct ls_softc *ls);
#endif

LOCAL es_tmd	       *esGetFreeTMD ();
LOCAL es_rmd	       *esGetFullRMD ();

LOCAL VOID		esRmdFree ();

LOCAL VOID		esConfig ();
LOCAL VOID		esChipReset ();
LOCAL VOID		esChipInit ();
LOCAL STATUS		esRecv ();
LOCAL u_short 		esReadBmpr0 ();

/*******************************************************************************
*
* esattach - attaches an EtherStar target
*
* Interface exists: make available by filling in network interface
* record.  System will initialize the interface when it is ready
* to accept packets.
* The memory pool will be malloced if NONE is specified.  The memWidth
* determines the data port width (in bytes) of the memory pool.
*
* BUGS:
* Currently uses bzero to zero lance data structures, which ignores the
* specification of memWidth and may use any size data access to write to memory.
*
*/

STATUS esattach
    (
    int	      unit,		/* unit number */
    char     *devAdrs,		/* ESTAR i/o address */
    int	      ivec,		/* interrupt vector */
    int	      ilevel,		/* interrupt level */
    char     *memAdrs,		/* address of memory pool (-1 == malloc it) */
    ULONG     memSize,		/* only used if memory pool is NOT malloced */
    int	      memWidth,		/* byte-width of data (-1 == any width)     */
    BOOL      usePadding,	/* use padding when accessing RAP? */
    int	      bufSize		/* size of a buffer in the ESTAR ring */
    )
    {
    FAST struct ls_softc  *ls;
    FAST struct ifnet	  *ifp;
    FAST unsigned int	   sz;		/* temporary size holder */
    FAST ES_DEVICE	  *dv;
    char 		  *memPool;	/* start of the ESTAR memory pool */

    if (bufSize == 0)
        bufSize = ES_BUFSIZE;

    if ((int) memAdrs != NONE)	/* specified memory pool */
	{
	/*
	 * With a specified memory pool we want to maximize
	 * esRsize and esTsize
	 */

	sz = (memSize - (sizeof (es_rmd) + sizeof (es_tmd) + sizeof (es_ib)))
	       / ((2 * bufSize) + sizeof (es_rmd) + sizeof (es_tmd));

	sz >>= 1;		/* adjust for roundoff */

	for (esRsize = 0; sz != 0; esRsize++, sz >>= 1)
	    ;

	esTsize = esRsize;	/* esTsize = esRsize for convenience */
	}

    /* limit ring sizes to reasonable values */

    if (esRsize < 2)
        esRsize = 2;		/* 4 buffers is reasonable min */

    if (esRsize > 7)
        esRsize = 7;		/* 128 buffers is max for chip */

    /* limit ring sizes to reasonable values */

    if (esTsize < 2)
        esTsize = 2;		/* 4 buffers is reasonable min */

    if (esTsize > 7)
        esTsize = 7;		/* 128 buffers is max for chip */


    /* calculate the total size of ESTAR memory pool.  (basic softc structure
     * is just allocated from free memory pool since ESTAR never references).
     */
    sz = ((sizeof (es_ib))        + (((1 << esRsize) + 1) * sizeof (es_rmd)) +
	  (bufSize << esRsize) + (((1 << esTsize) + 1) * sizeof (es_tmd)) +
	  (bufSize << esTsize));

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
     *					|         (sizeof (es_ib))	      |
     *					|				      |
     *   		  ls->ls_rring  |-------------------------------------|
     *					|				      |
     * 					| ((1 << esRsize) + 1)*sizeof (es_rmd)|
     *					|				      |
     *   	   ls->rmd_ring.r_bufs	|-------------------------------------|
     *					|				      |
     * 					|       (bufSize << esRsize)          |
     *					|				      |
     *  		  ls->ls_tring	|-------------------------------------|
     *					|				      |
     *					| ((1 << esTsize) + 1)*sizeof (es_tmd)|
     *					|				      |
     *   	   ls->tmd_ring.t_bufs	|-------------------------------------|
     *					|				      |
     * 					|       (bufSize << esTsize)          |
     *					|				      |
     *  			        |-------------------------------------|
     *
     *                                                HIGH MEMORY
     */

    /* allocate basic softc structure */

    ls = (struct ls_softc *)malloc (sizeof (struct ls_softc));
    if (ls == NULL)
	{
	if ((int) memAdrs == NONE)	/* if we malloced memPool */
	    (void) free (memPool);	/* free the memPool */
	return (ERROR);
	}

    /* zero out entire softc structure */
    bzero ((char *)ls, sizeof (struct ls_softc));

    /* fill in high byte of memory base (A24:A31) and data port width */

    ls->memBase  = (char *) ((ULONG)memPool & 0xff000000);
    ls->memWidth = memWidth;

    if ((int) memAdrs == NONE)
        ls->ls_flags |= LS_MEM_ALLOC_FLAG;
   
    if (usePadding == TRUE)		/* Is CSR padding necessary? */
	ls->ls_flags |= LS_PAD_USED_FLAG;
    else
        ls->ls_flags &= ~LS_PAD_USED_FLAG;

    ls->bufSize = bufSize;

    /* if memWidth is NONE (we can copy any byte size/boundaries) and
     * bufSize is big enough to hold the biggest ethernet frame
     * we can loan out rmds.  On systems that cannot afford to have
     * big enough RMD's, ESTAR will use data-chaining on receive
     * buffers.  Our current implementation of RMD loans does not work
     * with receive side data-chains.
     */

    if (ls->memWidth == NONE && ls->bufSize == ES_BUFSIZE)
	{
	int	ix;
	char	*pBuf;
	
        ls->canLoanRmds		= TRUE;
	ls->nFree		= ES_NUM_RESERVES;

	if ((pBuf = (char *) malloc ((u_int) (ES_NUM_RESERVES * bufSize))) == NULL)
	    {
	    (void) free (memPool);
	    (void) free ((char *) ls);
	    return (ERROR);
	    }

	for (ix = 0; ix < ES_NUM_RESERVES; ix++)
	    ls->freeBufs [ix] = (char *) ((int) pBuf + (bufSize * ix));
	}
    else
        ls->canLoanRmds = FALSE;

    /* allocate initialization block */

    ls->ib = (es_ib *)memPool;
    sz = sizeof (es_ib);		/* size of initialization block */
    bzero ((char *)ls->ib, (int) sz);	/* zero out entire ib */

    /* allocate receive message descriptor (RMD) ring structure */

    ls->ls_rpo2 = esRsize;		/* remember for esConfig */
    ls->ls_rsize = (1 << esRsize);	/* receive msg descriptor ring size */

    /* leave room to adjust low three bits */

    ls->ls_rring = (es_rmd *) ((int)ls->ib + sz);   /* of pointer to be 000 */
    sz = ((1 << esRsize) + 1) * sizeof (es_rmd);
    bzero ((char *)ls->ls_rring, (int)sz);	/* zero out entire RMD ring */

    /* allocate receive buffer space */

    ls->rmd_ring.r_bufs = (char *)((int)ls->ls_rring + sz);
    sz = (bufSize << esRsize);	 /* room for all the receive buffers */
    bzero (ls->rmd_ring.r_bufs, (int)sz);/* zero out entire rbuf area */

    /* allocate transmit message descriptor (TMD) ring structure */

    ls->ls_tpo2 = esTsize;		/* remember for esConfig */
    ls->ls_tsize = (1 << esTsize);	/* transmit msg descriptor ring size */

    ls->ls_tring = (es_tmd *) ((int)ls->rmd_ring.r_bufs + sz);
    sz = ((1 << esTsize) + 1) * sizeof (es_tmd);
    bzero ((char *)ls->ls_tring, (int)sz);	/* zero out entire TMD ring */

    /* allocate transmit buffer space */

    ls->tmd_ring.t_bufs = (char *)((int)ls->ls_tring + sz);
    sz = (bufSize << esTsize);	/* room for all the transmit buffers */

    bzero (ls->tmd_ring.t_bufs, (int)sz);	/* zero out entire tbuf area */

    ls_softc [unit] = ls;		/* remember address for this unit */

    /* initialize device structure */

    ls->ivec    	= ivec;			/* interrupt vector */
    ls->ilevel		= ilevel;		/* interrupt level */
    ls->devAdrs		= (ES_DEVICE *)devAdrs;	/* ESTAR i/o address */

    /* copy the enet address into the softc */

    dv = ls->devAdrs;
    esEnetAddr[0] = dv->ROM_IDf;
    esEnetAddr[1] = dv->ROM_IDe;
    esEnetAddr[2] = dv->ROM_IDd;
    esEnetAddr[3] = dv->ROM_IDc;
    esEnetAddr[4] = dv->ROM_IDb;
    esEnetAddr[5] = dv->ROM_IDa;
    if (esEnetAddr[3]) {
	newdlan = TRUE;
	dlan.dlcr0 = &(dv->dlcr0);
	dlan.dlcr1 = &(dv->dlcr1);
	dlan.dlcr2 = &(dv->dlcr2);
	dlan.dlcr3 = &(dv->dlcr3);
 	dlan.dlcr4 = &(dv->dlcr4);
	dlan.dlcr5 = &(dv->dlcr5);
	dlan.dlcr6 = &(dv->dlcr6);
	dlan.dlcr7 = &(dv->dlcr7);
	dlan.dlcr8 = &(dv->dlcr8);
	dlan.dlcr9 = &(dv->dlcr9);
	dlan.dlcr10 = &(dv->dlcr10);
	dlan.dlcr11 = &(dv->dlcr11);
	dlan.dlcr12 = &(dv->dlcr12);
	dlan.dlcr13 = &(dv->dlcr13);
	dlan.dlcr14 = &(dv->dlcr14);
	dlan.dlcr15 = &(dv->dlcr15);
    }
    else {
	newdlan = FALSE;
	dlan.dlcr0 = &(dv->dlcr1);
	dlan.dlcr1 = &(dv->dlcr0);
	dlan.dlcr2 = &(dv->dlcr3);
	dlan.dlcr3 = &(dv->dlcr2);
 	dlan.dlcr4 = &(dv->dlcr5);
	dlan.dlcr5 = &(dv->dlcr4);
	dlan.dlcr6 = &(dv->dlcr7);
	dlan.dlcr7 = &(dv->dlcr6);
	dlan.dlcr8 = &(dv->dlcr9);
	dlan.dlcr9 = &(dv->dlcr8);
	dlan.dlcr10 = &(dv->dlcr11);
	dlan.dlcr11 = &(dv->dlcr10);
	dlan.dlcr12 = &(dv->dlcr13);
	dlan.dlcr13 = &(dv->dlcr12);
	dlan.dlcr14 = &(dv->dlcr15);
	dlan.dlcr15 = &(dv->dlcr14);
    }

    bcopy ((char *) esEnetAddr, (char *)ls->ls_enaddr, sizeof (ls->ls_enaddr));

    ifp = &ls->ls_if;

    /* attach and enable the ESTAR interrupt service routine to vector */

    esChipReset (ls);				/* reset ESTAR */

    (void) intConnect (INUM_TO_IVEC (ivec), esInt, (int)ls);

#ifdef BSD43_DRIVER
    ether_attach (ifp, unit, "es", (FUNCPTR) esInit, (FUNCPTR) esIoctl,
		 (FUNCPTR) esOutput, (FUNCPTR) esReset);
#else
    ether_attach (
                 &ls->ls_ac.ac_if, 
                 unit, 
                 "es", 
                 (FUNCPTR) esInit, 
                 (FUNCPTR) esIoctl,
		 (FUNCPTR) ether_output,
                 (FUNCPTR) esReset
                 );
    ifp->if_start = (FUNCPTR)esStartOutput;
#endif

    sysIntEnable (ilevel);

    esInit (unit);

    return (OK);
    }

/*******************************************************************************
*
* esReset - reset the interface
*
* Mark interface as inactive & reset the chip
*/

LOCAL VOID esReset
    (
    int unit
    )
    {
    FAST struct ls_softc  *ls = ls_softc [unit];
    ls->ls_if.if_flags &= ~IFF_RUNNING;
    esChipReset (ls);				/* reset ESTAR */
    }
/*******************************************************************************
*
* esInit - initializes EtherStar connection
*
* Initialization of interface; clear recorded pending operations.
* Called at boot time (with interrupts disabled?),
* and at ifconfig time via esIoctl, with interrupts disabled.
*/

LOCAL int esInit
    (
    int unit
    )
    {
    FAST struct ls_softc  *ls = ls_softc [unit];
    FAST struct ifnet	  *ifp = &ls->ls_if;

    ifp->if_flags &= ~(IFF_UP | IFF_RUNNING | IFF_PROMISC);

    esChipReset (ls);				/* disable chip during init */
    esConfig (ls);				/* reset all ring structures */

    ifp->if_flags |= (IFF_UP | IFF_RUNNING);

    if (ls->ls_flags & LS_PROMISCUOUS_FLAG)
	ifp->if_flags |= (IFF_PROMISC);

    esChipInit (ls);			/* on return ESTAR is running */

#ifdef BSD43_DRIVER
    esStartOutput (ls->ls_if.if_unit);	/* tell chip about any pending output */
#else
    esStartOutput (ls);
#endif

    return (0);
    }
/*******************************************************************************
*
* esConfig - fill in initialization block with mode information.
*
* Fill in all fields in the Initialization Block with relevant values.
* In addition, fill in all fields in Transmit Message Descriptor ring
* and Receive Message Descriptor ring.
*/

LOCAL VOID esConfig
    (
    FAST struct ls_softc  *ls
    )
    {
    FAST es_rmd	  *rmd;
    FAST es_tmd	  *tmd;
    FAST char	  *buf;
    FAST es_ib	  *ib;
    int		   i;

    rmd = ls->ls_rring;			/* receive message descriptor ring */
    rmd = (es_rmd *)(((int)rmd + 7) & ~7);	/* low 3 bits must be 000 */
    ls->ls_rring = rmd;				/* fix softc as well */
    buf = ls->rmd_ring.r_bufs;
    for (i = 0; i < ls->ls_rsize; i++)
        {
	rmd->rbuf_ladr = (u_short)((int)buf); /* bits 15:00 of buffer address */
	rmd->rbuf_rmd1 = (u_short)((int)buf >> 16) & 0xff;
					/* bits 23:16 of buffer address */

	rmd->rbuf_bcnt = -(ls->bufSize);/* neg of buffer byte count */
	rmd->rbuf_mcnt = 0;		/* no message byte count yet */
	rmd->es_rmd1.es_rmd1b |= esrmd1_OWN;	/* buffer now owned by ESTAR */
	rmd++;				/* step to next message descriptor */
	buf += (ls->bufSize);		/* step to start of next buffer */

	ls->loanRefCnt [i] = (u_char) 0;
	}

    ls->ls_rindex = 0;			/* ESTAR will use at start of ring */

    tmd = ls->ls_tring;			/* transmit message descriptor ring */
    tmd = (es_tmd *)(((int)tmd + 7) & ~7);	/* low 3 bits must be 000 */
    ls->ls_tring = tmd;			/* fix softc as well */
    buf = ls->tmd_ring.t_bufs;

    for (i = 0; i < ls->ls_tsize; i++)
        {
	tmd->tbuf_ladr = (u_short)((int)buf); /* bits 15:00 of buffer address */
	tmd->tbuf_tmd1 = (u_short)((int)buf >> 16) & 0xff;
					/* bits 23:16 of buffer address */
	tmd->tbuf_bcnt = 0;		/* no message byte count yet */
	tmd->tbuf_tmd3 = 0;		/* no error status yet */
	tmd->es_tmd1.es_tmd1b |= estmd1_ENP;	/* buffer is end of packet */
	tmd->es_tmd1.es_tmd1b |= estmd1_STP;	/* buffer is start of packet */
	tmd++;				/* step to next message descriptor */
	buf += (ls->bufSize);		/* step to start of next buffer */
	}

    ls->ls_tindex = 0;			/* ESTAR */
    ls->ls_dindex = 0;			/* buffer disposal will lag tindex */

    ib = ls->ib;
    if (ls->ls_flags & LS_PROMISCUOUS_FLAG)
        ib->esIBMode = 0x8000;	/* chip will be in promiscuous receive mode */
    else
        ib->esIBMode = 0;	/* chip will be in normal receive mode */

    swab ((char *)ls->ls_enaddr, ib->esIBPadr, 6);

    ib->esIBRdraLow = (u_short)((int)ls->ls_rring);
    ib->esIBRdraHigh = (u_short)(((int)ls->ls_rring >> 16) & 0xff)
      			 | (ls->ls_rpo2 << 13);

    ib->esIBTdraLow = (u_short)((int)ls->ls_tring);
    ib->esIBTdraHigh = (u_short)(((int)ls->ls_tring >> 16) & 0xff)
      			 | (ls->ls_tpo2 << 13);
    }
/*******************************************************************************
*
* esInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*/

LOCAL VOID esInt
    (
    FAST struct ls_softc  *ls
    )
    {
    FAST ES_DEVICE	*dv;
    FAST es_rmd		*rmd;
    FAST es_tmd		*tmd;
    FAST int 		*pDindex;
    FAST int		*pTindex;
    FAST int		*pTsize;
    FAST es_tmd		*pTring;
    FAST u_short	w, temp;
    FAST int		i;
    FAST char		*buf;

    
    dv = ls->devAdrs;
    *dlan.dlcr1 = 0x00;	/* clear transmit masks */
    *dlan.dlcr3 = 0x00;	/* clear receive masks */

    /* read any receive packets */

    while (!(*dlan.dlcr5 & 0x40))
    {
	*dlan.dlcr2 = 0x80;	/* clear pkt_rdy */
	rmd = ls->ls_rring + recIndex;
	recIndex = (recIndex + 1) & (ls->ls_rsize - 1);
	rmd->es_rmd1.es_rmd1b &= ~esrmd1_OWN;  /* rbuf_own = 0; */

	buf = (char *) ((u_int) ls->rmd_ring.r_bufs +
	      (u_int) ((((u_int)rmd - (u_int)ls->ls_rring) / sizeof(es_rmd))
	      * ls->bufSize));

	temp = esReadBmpr0 (dv);	/* read status */

	if (newdlan == TRUE) {
		temp = esReadBmpr0 (dv);	/* and count */
		i = (temp>>8 | temp<<8) & 0xffff;
		rmd->rbuf_mcnt = i;	/* save count */
		rmd->es_rmd1.es_rmd1b |= esrmd1_STP;  /* rbuf_stp = 1; */
		rmd->es_rmd1.es_rmd1b |= esrmd1_ENP;  /* rbuf_enp = 1; */
		while (i > 0)
		{
		    w = esReadBmpr0 (dv);
		    *(u_short *)buf = w;
		    buf +=2;
		    i -= 2;		/* count down */
		}
	}
	else {
		i = esReadBmpr0 (dv);	/* and count */
		rmd->rbuf_mcnt = i;	/* save count */
		rmd->es_rmd1.es_rmd1b |= esrmd1_STP;  /* rbuf_stp = 1; */
		rmd->es_rmd1.es_rmd1b |= esrmd1_ENP;  /* rbuf_enp = 1; */
		while (i > 0)
		{
		    w = esReadBmpr0 (dv);
		    *(u_short *)buf = (w>>8 | w<<8);
		    buf +=2;
		    i -= 2;		/* count down */
		}
	}
    }
    if (*dlan.dlcr2 & es_bus_rd_err)
	*dlan.dlcr2 = es_bus_rd_err;

    if ((rmd = esGetFullRMD (ls)) != NULL)
	{
	if ((ls->ls_flags & LS_RCV_HANDLING_FLAG) == 0)
	    {
	    ls->ls_flags |= LS_RCV_HANDLING_FLAG;
	    (void) netJobAdd ((FUNCPTR) esHandleRecvInt, (int) ls, 0, 0, 0, 0);
	    }
	}

    /* Did etherstar update any of the TMD's? */

    if (!(*dlan.dlcr0 & ES_TMT_MASK))
	{
	*dlan.dlcr3 = ES_RCV_MASK;
	return;
	}

    *dlan.dlcr1 = 0x00;		/* clear transmit mask */
    *dlan.dlcr0 = 0x0f;		/* reset error conditions */
    *dlan.dlcr2 = 0x4f;		/* reset packet ready */

    pDindex = &ls->ls_dindex;
    pTindex = &ls->ls_tindex;
    pTsize  = &ls->ls_tsize;
    pTring  = ls->ls_tring;

    while (*pDindex != *pTindex)
        {
	/* disposal has not caught up */

	tmd = pTring + *pDindex;

	/* if the buffer is still owned by ESTAR, don't touch it */

	if (tmd->tbuf_tmd1 & TMD_OWN)
	    {
	    break;
	    }

	/*
	 * tbuf_err (TMD1.ERR) is an "OR" of LCOL, LCAR, UFLO or RTRY.
	 * Note that BUFF is not indicated in TMD1.ERR.
	 * We should therefore check both tbuf_err and tbuf_buff
	 * here for error conditions.
	 */

	if ((tmd->tbuf_tmd1 & TMD_ERR) || (tmd->tbuf_tmd3 & TMD_BUFF))
	    {
	    ls->ls_if.if_oerrors++;	/* output error */
	    ls->ls_if.if_opackets--;

	    /* check for no carrier */

	    if (tmd->tbuf_tmd3 & TMD_LCAR)
		logMsg ("es%d: no carrier\n", ls->ls_if.if_unit, 0, 0, 0, 0, 0);

	    /* every esLogCount errors show other interesting bits of tmd3 */

	    if ((tmd->tbuf_tmd3 & 0xfc00) && esLogCount &&
		(ls->ls_if.if_oerrors % esLogCount) == 0)
		logMsg ("es%d: xmit error, status(tmd3)=0x%x, err count=%d\n",
			ls->ls_if.if_unit, tmd->tbuf_tmd3 & 0xfc00,
			ls->ls_if.if_oerrors, 0, 0, 0);

	    /* restart chip on fatal errors */

	    if ((tmd->tbuf_tmd3 & TMD_BUFF) || (tmd->tbuf_tmd3 & TMD_UFLO))
		{
		(void) netJobAdd ((FUNCPTR) esInit,
				  ls->ls_if.if_unit, 0, 0, 0, 0);
		break;
		}
	    }

	tmd->tbuf_tmd1 &= 0xff;		/* clear high byte */
	tmd->tbuf_tmd3 = 0;		/* clear all error & stat stuff */

	/* now bump the tmd disposal index pointer around the ring */

	*pDindex = (*pDindex + 1) & (*pTsize - 1);
	}

    if (ls->ls_if.if_snd.ifq_head != NULL &&
	(ls->ls_flags & LS_START_OUTPUT_FLAG) == 0)
	{
	ls->ls_flags |= LS_START_OUTPUT_FLAG;
#ifdef BSD43_DRIVER
        (void) netJobAdd ( (FUNCPTR) esStartOutput,
                           ls->ls_if.if_unit, 0, 0, 0, 0);
#else
        (void) netJobAdd ( (FUNCPTR) esStartOutput, (int)ls, 0, 0, 0, 0);
#endif
	}
    *dlan.dlcr3 = ES_RCV_MASK;
    }
/*******************************************************************************
*
* esReadBmpr0 - prevents optimizer from stripping required accesses to device
*
* prevents optimizer from stripping required accesses to the device
*/
LOCAL u_short esReadBmpr0
    (
    FAST ES_DEVICE *dv
    )
    {
    return ( dv->bmpr0);
    }

/*******************************************************************************
*
* esHandleRecvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/

LOCAL VOID esHandleRecvInt
    (
    FAST struct ls_softc *ls
    )
    {
    FAST es_rmd		*rmd;
    FAST int 		oldIndex;

    do
	{
	ls->ls_flags |= LS_RCV_HANDLING_FLAG;

	while ((rmd = esGetFullRMD (ls)) != NULL)
	    {				
	    /* RMD available */

	    oldIndex = ls->ls_rindex;
	    if (esRecv (ls, rmd) == OK)
		for (; oldIndex != ls->ls_rindex; 
		     oldIndex = (oldIndex + 1) & (ls->ls_rsize - 1))
		    {
		    rmd = ls->ls_rring + oldIndex;
		    ES_RMD_GIVE_TO_ESTAR (rmd);
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
  while (esGetFullRMD (ls) != NULL);	/* this double check solves the RACE */
	{
	}
    *dlan.dlcr3 = ES_RCV_MASK;
    }

/*******************************************************************************
*
* esRecv - process Ethernet receive completion
*
* Process Ethernet receive completion:
*	If input error just drop packet.
*	Otherwise purge input buffered data path and examine 
*	packet to determine type.  If can't determine length
*	from type, then have to drop packet.  Otherwise decapsulate
*	packet based on type and pass to type-specific higher-level
*	input routine.
*
* RETURNS: ERROR if RMD shouldn't be returned back to ESTAR yet.
*          Otherwise, returns OK to return the RMD back to ESTAR.
*/

LOCAL STATUS esRecv
    (
    struct ls_softc	   *ls,
    FAST es_rmd		   *rmd
    )
    {
    FAST struct ether_header	*eh;
    FAST struct mbuf		*m;
    FAST u_char			*pData;
    int				len;
    int				off;
#ifdef BSD43_DRIVER
    u_short			ether_type;
#endif

    /*
     * If both STP and ENP are set, ESTAR didn't try to
     * use data chaining.
     */

    if (((rmd->es_rmd1.es_rmd1b & esrmd1_STP) == esrmd1_STP) &&
	((rmd->es_rmd1.es_rmd1b & esrmd1_ENP) == esrmd1_ENP))
	{
	len = rmd->rbuf_mcnt;
	ls->ls_rindex = (ls->ls_rindex + 1) & (ls->ls_rsize - 1);
	}


    else
	{
	ls->ls_rindex = (ls->ls_rindex + 1) & (ls->ls_rsize - 1);

	++ls->ls_if.if_ierrors;

	if (esLogCount && (ls->ls_if.if_ierrors % esLogCount) == 0)
	    logMsg ("es%d: receive error, stp %d enp %d\n",
		    ls->ls_if.if_unit,
		    (rmd->es_rmd1.es_rmd1b & esrmd1_STP) >> 9,
		    (rmd->es_rmd1.es_rmd1b & esrmd1_ENP) >> 8, 0, 0, 0);

	return (OK);		/* intentional */
	}

    /*
    eh = (struct ether_header *)(rmd->rbuf_ladr | (rmd->rbuf_hadr << 16));
    ItoK (eh, struct ether_header *, ls->memBase);
    */

    ++ls->ls_if.if_ipackets;    /* bump statistic */

    eh =(struct ether_header *) ((u_int) ls->rmd_ring.r_bufs +
	(u_int) ((((u_int)rmd - (u_int)ls->ls_rring) / sizeof(es_rmd))
	* ls->bufSize));

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

    pData = ((u_char *) eh) + (sizeof (struct ether_header));

#ifdef BSD43_DRIVER
    check_trailer (eh, pData, &len, &off, &ls->ls_if);

    if (len == 0)
	return (OK);

    ether_type = eh->ether_type;
#endif

    m = NULL;

    /* we can loan out receive buffers from ESTAR receive ring if:
     *
     * 1) canLoanRmd is TRUE.  canLoanRmd is set to TRUE if memWidth
     *    is NONE (no special restriction in copying data in terms of
     *    size and boundary conditions) and each of the buffers in the
     *    ESTAR ring is big enough to hold the maximum sized ethernet
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
			   (FUNCPTR)esRmdFree, (int) ls, (int) eh, NULL);

	if (m != NULL)
	    {
	    FAST char *pBuf;
	    
	    /* get a free buffer from the free list to replace the
	     * rbuf loaned out.  replace the rbuf pointers in the RMD
	     * to point to the new buffer.
	     */
	    
	    pBuf = ls->freeBufs [--ls->nFree];
	    
	    rmd->rbuf_ladr = (u_short) ((int) pBuf & 0xff);
	    rmd->es_rmd1.es_rmd1b = (rmd->es_rmd1.es_rmd1b & ~esrmd1_HADR)
				    | (((int) pBuf >> 16) & esrmd1_HADR);
	    }
	}

    if (m == NULL)
#ifdef BSD43_DRIVER
	/* Instead of calling copy_to_mbufs (), we call bcopy_to_mbufs ()
	 * with ls->memWidth as an addtional argument to specify unit of a
	 * copy op.
	 *
	 * Most drivers would use macro copy_to_mbufs (), which will in turn
	 * call bcopy_to_mbufs () telling it to use the normal bcopy ().  Some
	 * of the on-board ESTAR hardware implementations require that you
	 * copy the data by certain number of bytes from dedicated memory
	 * to system memory, so ESTAR driver has a es_bcopy () routine that
	 * conforms to this requirement.
	 */

	m = bcopy_to_mbufs (pData, len, off, (struct ifnet *) &ls->ls_if,
			    ls->memWidth);
#else
        m = bcopy_to_mbufs (pData, len, 0, &ls->ls_if, ls->memWidth);
#endif

    if (m == NULL)
        ++ls->ls_if.if_ierrors;    /* bump error counter */
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
* esOutput - Ethernet output routine
*
* Ethernet output routine.
* Encapsulate a packet of type family for the local net.
* Use trailer local net encapsulation if enough data in first
* packet leaves a multiple of 512 bytes of data in remainder.
*/

LOCAL int esOutput
    (
    FAST struct ifnet	*ifp,
    FAST struct mbuf	*m0,
    struct sockaddr	*dst
    )
    {
    return (ether_output (ifp, m0, dst, (FUNCPTR) esStartOutput, 
			  &ls_softc [ifp->if_unit]->ls_ac));
    }
#endif

/*******************************************************************************
*
* esStartOutput - start pending output
*
* Start output to ESTAR.
* Queue up all pending datagrams for which transmit buffers are available.
* Kick start the transmitter to avoid the polling interval latency.
* This routine is called by esInit (). With BSD 4.3 drivers, it is also called
* by esOutput (). BSD 4.4 drivers use a slightly different model in which it
* is called directly from the generic ether_output() routine.
* It is very important that this routine be executed with splimp set.
* If this is not done, another task could allocate the same tmd!
*/

#ifdef BSD43_DRIVER
LOCAL VOID esStartOutput
    (
    int unit
    )
    {
    FAST struct ls_softc	*ls = ls_softc [unit];
#else
LOCAL VOID esStartOutput
    (
    struct ls_softc * 	ls
    )
    {
#endif

    FAST struct mbuf		*m;
    FAST es_tmd			*tmd;
    char			*buf, *buf0;
    u_short			*temp, temp1;
    FAST int			len, i;
    FAST int			sx;
    FAST int			oldLevel;
    ES_DEVICE			*dv;

    sx = splimp ();

    dv = ls->devAdrs;

    ls->ls_flags |= LS_START_OUTPUT_FLAG;

    /* Loop placing message buffers in output ring until no more or no room */

    while (ls->ls_if.if_snd.ifq_head != NULL)
        {
	/* there is something to send */

	if ((tmd = esGetFreeTMD (ls)) == NULL)	/* get a transmit buffer */
	    break;

	IF_DEQUEUE (&ls->ls_if.if_snd, m); /* get head of next mbuf chain */

	buf = (char *) ( (u_int) ls->tmd_ring.t_bufs +
                         (u_int) ( ( ( (u_int)tmd - (u_int)ls->ls_tring) / 
                                    sizeof (es_tmd)) * 
                         ls->bufSize));
	buf0 = buf;

	/* copy packet to write buffer */

	temp = (u_short *)buf;

#ifdef BSD43_DRIVER
	bcopy_from_mbufs (buf, m, len, ls->memWidth);
#else
        bcopy_from_mbufs (buf, m, len, ls->memWidth);
#endif

	len = max (ETHERSMALL, len);
	buf += len;

	/* call output hook if any */
    
	if ((etherOutputHookRtn != NULL) &&
	    (* etherOutputHookRtn) (&ls->ls_if, buf, len))
	    continue;

	/* place a transmit request */
    
	oldLevel = intLock ();		/* disable ints during update */

	tmd->tbuf_tmd3 = 0;		/* clear buffer error status */
	tmd->tbuf_bcnt = -len;		/* negative message byte count */
	tmd->es_tmd1.es_tmd1b |= estmd1_ENP;	/* buffer is end of packet */
	tmd->es_tmd1.es_tmd1b |= estmd1_STP;	/* buffer is start of packet */
	tmd->es_tmd1.es_tmd1b &= ~estmd1_DEF;	/* clear status bit */
	tmd->es_tmd1.es_tmd1b &= ~estmd1_MORE;
	tmd->es_tmd1.es_tmd1b &= ~estmd1_ERR;

	/* try to output here */
	*dlan.dlcr1 = 0;		/* disable ints during update */
	i=0;			/* extract the buffer */

	if (newdlan == TRUE) 
            {
            while (i*2 < len)
                {
                if (i*2 < (buf - buf0))
                    dv->bmpr0 = *temp++;
                else 
                    dv->bmpr0 = 0;
                i += 1;
		}
            temp1 = (u_short)((len>>8) | (len<<8) | 0x80);	/* set TMST */
            dv->bmpr2 = temp1;
            }
        else 
            {
            while (i*2 < len)
                {
                if (i*2 < (buf - buf0))
                    {
                    dv->bmpr0 = (*temp>>8 | *temp<<8);
                    temp++;
                    }
                else 
                    dv->bmpr0 = 0;
                i += 1;
                }
            dv->bmpr2 = (u_short)(len | 0x8000);	/* set TMST */
            }

	while ( (*dlan.dlcr0 & es_tmt_ok) == es_tmt_ok)
            ;

	intUnlock (oldLevel);	/* now esInt won't get confused */

	ls->ls_tindex = (ls->ls_tindex + 1) & (ls->ls_tsize - 1);

#ifndef BSD43_DRIVER    /* BSD 4.4 ether_output() doesn't bump statistic. */
        ls->ls_if.if_opackets++;
#endif
        }

    ls->ls_flags &= ~LS_START_OUTPUT_FLAG;

    *dlan.dlcr1 = ES_TMT_MASK;		/* set transmit interrupt mask */

    splx (sx);
    }

/*******************************************************************************
*
* esIoctl - Process an ioctl request
*
* Process an ioctl request.
*/

LOCAL int esIoctl
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
* esChipReset - hardware reset of chip (stop it)
*/

LOCAL VOID esChipReset
    (
    struct ls_softc  *ls
    )
    {
    *dlan.dlcr1 = 0;			/* reset transmit interrupt mask */
    *dlan.dlcr3 = 0;			/* reset receive interrupt mask  */
    *dlan.dlcr6 = es_ena_dlc;		/* stop the etherstar            */
    *dlan.dlcr2 = 0xcf;			/* clear interrupts */
    *dlan.dlcr0 = 0x0f;
    }
/*******************************************************************************
*
* esChipInit - hardware init of chip (init & start it)
*/

LOCAL VOID esChipInit
    (
    FAST struct ls_softc  *ls
    )
    {
    u_short temp;
    FAST ES_DEVICE	*dv = ls->devAdrs;

    /* setup vector */
    dv->eth_vector =  ls->ivec;
 
    *dlan.dlcr6 = es_ena_dlc;	/* stop the etherstar			*/
    *dlan.dlcr2 = 0xcf;		/* clear all receive errors		*/
    *dlan.dlcr3 = 0x8f;		/* reset receive interrupt mask		*/
    *dlan.dlcr0 = 0x0f;		/* clear all transmit errors 		*/
    *dlan.dlcr1 = 0x00;		/* reset transmit interrupt mask	*/

    /* flush the receive buffer memory of the etherstar */
    temp = esReadBmpr0 (dv);
    while ( !(*dlan.dlcr5 & 0x40) ) temp = esReadBmpr0 (dv);
    temp = esReadBmpr0 (dv);
    temp = esReadBmpr0 (dv);

    /* configure etherstar in normal mode */
    *dlan.dlcr4 = es_lbc;	/* loopback disabled, byte swap enabled */
    *dlan.dlcr5 = 0x02;		/* physical, multicast and broadcast 	*/

    /* copy the enet address into the softc */

    *dlan.dlcr8 = esEnetAddr[0];
    *dlan.dlcr9 = esEnetAddr[1];	/* setting these registers is only */
    *dlan.dlcr10 = esEnetAddr[2];	/* possible when etherstar is stopped */
    *dlan.dlcr11 = esEnetAddr[3];
    *dlan.dlcr12 = esEnetAddr[4];
    *dlan.dlcr13 = esEnetAddr[5];

    for ( temp = 20000; temp > 0; temp-- );

    /* start chip */
    *dlan.dlcr6 = 0x00;		/* start the etherstar			*/
    *dlan.dlcr1 = ES_TMT_MASK;	/* set transmit interrupt mask		*/
    *dlan.dlcr3 = ES_RCV_MASK;	/* set receive interrupt mask */
    }
/*******************************************************************************
*
* esGetFreeTMD - get next available TMD
*/

LOCAL es_tmd *esGetFreeTMD
    (
    FAST struct ls_softc  *ls
    )
    {
    FAST es_tmd	  *tmd;

    /* check if buffer is available (owned by host) */
    /* also check for tindex overrun onto dindex */

    tmd = ls->ls_tring + ls->ls_tindex;

    if (((tmd->es_tmd1.es_tmd1b & estmd1_OWN) != 0)
	|| (((ls->ls_tindex + 1) & (ls->ls_tsize - 1)) == ls->ls_dindex))
        tmd = (es_tmd *) NULL;

    return (tmd);
    }
/*******************************************************************************
*
* esGetFullRMD - get next received message RMD
*/

LOCAL es_rmd *esGetFullRMD
    (
    FAST struct ls_softc  *ls
    )
    {
    FAST es_rmd	  *rmd;

    /* check if buffer is full (owned by host) */

    rmd = ls->ls_rring + ls->ls_rindex;
    if ((rmd->es_rmd1.es_rmd1b & esrmd1_OWN) == 0)
	return (rmd);
    else
	return ((es_rmd *) NULL);
    }
/*******************************************************************************
*
* esRmdFree - called when loaned out rbuf is freed by MCLFREE.
*
* Puts the loaned out rbuf into free list to be reused as loan replacements.
*/

LOCAL VOID esRmdFree
    (
    FAST struct ls_softc	*ls,
    FAST char 			*pBuf
    )
    {
    ls->freeBufs [ls->nFree++] = pBuf;
    }
