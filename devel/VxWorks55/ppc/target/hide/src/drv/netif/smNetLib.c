/* smNetLib.c - VxWorks interface to shared memory network (backplane) driver*/

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01v,05apr02,mas  added volatile pointers and bridge flushing reads (SPRs 68334,
		 68844)
01u,13feb02,mas  fixed staged delay/check for slave node startup (SPR 73163)
01t,24oct01,mas  fixed diab warnings (SPR 71120); doc update (SPR 71149)
01s,02oct01,mas  added staged delay/check for slave node startup in new
		 smNetAttach2() routine (SPR 4547)
01r,15may95,vin  fixed parameter to build_cluster().
01q,30sep93,rrr  Added missing 01p modhist.
01p,30sep93,rrr  Fixed smNetIntr from using an uninialized variable if
                 intType==SM_INT_BUS.
01o,21nov92,jdi  documentation cleanup.
01n,13oct92,jcf  added a hack to protect from bus error during attach.
01m,20oct92,pme  made smNetInit return ERROR instead of disabling data cache
		 if cache coherent buffer cannot be allocated.
01l,09sep92,gae  doc tweaks.
01k,31jul92,dnw  Changed to new cacheLib.
01j,27jul92,elh  Moved smNetShow to smNetShow.c
01i,26jul92,gae  fixed number of parameters to logMsg().
01h,24jul92,elh  added caching, ripple effect of moving heartbeat.
01g,15jun92,elh  changed parameter to smNetInetGet.
01f,02jun92,elh  the tree shuffle
		  -changed includes to have absolute path from h/
01e,27may92,elh  Incorperated the changes caused from restructuring the
		 shared memory libraries.
01d,17may92,elh  renamed from smVxLib to smNetLib
01c,01may92,elh	 added mask, if_resolve hook and smNetAddrResolve.
01b,12apr92,elh  added support for sequential addressing.
01a,17nov90,elh  written.
*/

/*
DESCRIPTION
This library implements the VxWorks-specific portions of the shared memory
network interface driver.  It provides the interface between VxWorks and
the network driver modules (e.g., how the OS initializes and attaches
the driver, interrupt handling, etc.), as well as VxWorks-dependent
system calls.  

There are no user-callable routines.

The backplane master initializes the backplane shared memory and network
structures by first calling smNetInit().  Once the backplane has been
initialized, all processors can be attached to the shared memory network via
the smNetAttach() routine.  Both smNetInit() and smNetAttach() are called
automatically during system initialization when backplane parameters are
specified in the boot line.

For detailed information refer to 
\tb VxWorks Network Programmer's Guide: Data Link Layer Network Compnents.

INTERNAL
This file used to correspond to the smNetLib for SunOS.  SunOS is no longer
supported. 

The smNetInetGet() routine gets the Internet address associated with a
backplane interface.

Refer to if_sm.c for more information about the layering of backplane modules.

INCLUDE FILES: smNetLib.h, smPktLib.h, smUtilLib.h

SEE ALSO: ifLib, if_sm,
\tb VxWorks Network Programmer's Guide

NOROUTINES
*/

/* includes */

#include "vxWorks.h"
#include "net/if.h"
#include "netinet/if_ether.h"
#include "net/mbuf.h"
#include "arpLib.h"
#include "wdLib.h"
#include "sysLib.h"
#include "iv.h"
#include "vxLib.h"
#include "taskLib.h"
#include "stdlib.h"
#include "intLib.h"
#include "stdio.h"
#include "logLib.h"
#include "netLib.h"
#include "cacheLib.h"

#include "drv/netif/if_sm.h"
#include "drv/netif/smNetLib.h"
#include "smPktLib.h"
#include "smUtilLib.h"

/* globals */

int	smNetVerbose		= FALSE;		/* debug aide */
int 	smNetMaxBytesDefault 	= DEFAULT_PKT_SIZE;	/* packet size */
int	smNetLoanNum		= 0;			/* buffers to loan */

SM_SOFTC * sm_softc [NSM]	= { NULL };		/* soft_c structures */

/* forward declarations */

LOCAL STATUS    smNetAddrResolve (struct ifnet * pIf, struct in_addr * pIpDest,
				 u_char * pHwDest, int * usetrailers);
LOCAL void	smNetIntr (int intType);
LOCAL void	smNetPulse (SM_PKT_MEM_HDR * pSmPktHdr);
LOCAL STATUS	smNetProbe
    (
    void * ptr,		/* location to probe */
    UINT   maxTicsWait,	/* max system clock ticks period to wait */
    BOOL   tstEqVal,	/* TRUE: test probe value == val */
    int    val		/* specific value for operation */
    );

/* externs */

IMPORT void if_dettach (struct ifnet *);


/******************************************************************************
*
* smNetInit - initialize the shared memory network driver
*
* This routine is called once by the backplane master.  It sets up and 
* initializes the shared memory region of the shared memory network and 
* starts the shared memory heartbeat.  
*
* The <pAnchor> parameter is the local memory address by which the master
* CPU accesses the shared memory anchor.  <pMem> contains either the local
* address of shared memory or the value NONE (-1), which implies that shared
* memory is to be allocated dynamically.  <memSize> is the size, in bytes,
* of the shared memory region.
*
* The <tasType> parameter specifies the test-and-set operation to be used to
* obtain exclusive access to the shared data structures.  It is preferable
* to use a genuine test-and-set instruction, if the hardware permits it.  In
* this case, <tasType> should be SM_TAS_HARD.  If any of the CPUs on the
* backplane network do not support the test-and-set instruction, <tasType>
* should be SM_TAS_SOFT.
*
* The <maxCpus> parameter specifies the maximum number of CPUs that may
* use the shared memory region.
*
* The <maxPktBytes> parameter specifies the size, in bytes, of the data
* buffer in shared memory packets.  This is the largest amount of data
* that may be sent in a single packet.  If this value is not an exact
* multiple of 4 bytes, it will be rounded up to the next multiple of 4.
*
* The <startAddr> parameter is only applicable if sequential addressing is
* desired.  If <startAddr> is non-zero, it specifies the starting address to
* use for sequential addressing on the backplane.  If <startAddr> is zero,
* sequential addressing is disabled.
*
* RETURNS: OK, or ERROR if the shared memory network cannot be initialized.
*
* NOMANUAL
*/

STATUS smNetInit
    (
    SM_ANCHOR * pAnchor,	/* local addr of anchor */
    char *	pMem,		/* local addr of shared memory */
    int		memSize,	/* size of shared memory */
    BOOL	tasType,	/* TRUE = hardware supports TAS */
    int		cpuMax,		/* max numbers of cpus */
    int		maxPktBytes, 	/* size of data packets */
    u_long	startAddr 	/* beginning address */
    )
    {
    SM_ANCHOR volatile *	pAnchorv = (SM_ANCHOR volatile *) pAnchor;
    SM_PKT_MEM_HDR volatile *	pSmPktHdr;	  /* packet header */
    WDOG_ID			beatWd;
    BOOL			malloced = FALSE; /* malloced memory here! */

    /* validate parameters */

    if ((pAnchor == NULL) || (pMem == NULL))
	{
	logMsg ("smNetInit: invalid %s\n", (pAnchor == NULL) ?
		(int) "pAnchor" : (int) "pMem", 0, 0, 0, 0, 0);
	return (ERROR);
	}

    if (sysProcNumGet () != SM_MASTER)
	{
	logMsg ("smNetInit:called by cpu [%d] non-master\n", sysProcNumGet(),
		0, 0, 0, 0, 0);
	return (ERROR);
	}

    maxPktBytes = (maxPktBytes == 0) ?  smNetMaxBytesDefault : maxPktBytes;

    if ((startAddr != 0) && (smNetVerbose))
	logMsg ("smNetInit:sequential addressing activated (0x%x) \n", 
		startAddr, 0, 0, 0, 0, 0);

    if (pMem == (char *) NONE) 		/* allocate the shared memory */
	{
	if (!CACHE_DMA_IS_WRITE_COHERENT () || !CACHE_DMA_IS_READ_COHERENT ())
	    {
	    logMsg ("smNetInit - cache coherent buffer not available. Giving up. \n", 0, 0, 0, 0, 0, 0);
	    return (ERROR);
	    }

    	if ((pMem = (char *) cacheDmaMalloc (memSize)) == NULL)
    	    return (ERROR);

    	malloced = TRUE;
        }
    else
	{
    	if (pMem == (char *) pAnchor)
	    {
	    pMem    +=  sizeof (SM_ANCHOR);
	    memSize -=  sizeof (SM_ANCHOR);
	    }
	}

    if (smNetVerbose)
	logMsg ("smNetInit: anchor:[0x%x] memory:[0x%x] size:%d \n",
		(int) pAnchor, (int) pMem, memSize, 0, 0, 0);

    /* set up shared memory region */

    if ((smPktSetup (pAnchor, pMem, memSize, tasType, cpuMax,
		     maxPktBytes + SIZEOF_ETHERHEADER) == OK) &&
 	((beatWd = wdCreate ()) != NULL))
	{
        pSmPktHdr = SM_OFFSET_TO_LOCAL(ntohl (pAnchorv->smPktHeader),
				       (int)pAnchor, SM_PKT_MEM_HDR volatile *);
    	pSmPktHdr->reserved1 = htonl (startAddr);
        pSmPktHdr->reserved2 = (int) beatWd;

        CACHE_PIPE_FLUSH ();                  /* CACHE FLUSH   [SPR 68334] */
        maxPktBytes = pSmPktHdr->maxPktBytes; /* BRIDGE FLUSH  [SPR 68334] */

        /* start heartbeat */

        smNetPulse ((SM_PKT_MEM_HDR *)pSmPktHdr);
	return (OK);
	}

    if (malloced)
	(void) free (pMem);

    return (ERROR);
    }


/******************************************************************************
*
* smNetAttach2 - attach the shared memory network interface
*
* This routine attaches the shared memory interface to the network.  It is
* called once by each CPU on the shared memory network.  The <unit> parameter
* specifies the backplane unit number.
*
* The <pAnchor> parameter is the local address by which the local CPU may
* access the shared memory anchor.
*
* The <maxInputPkts> parameter specifies the maximum number of incoming
* shared memory packets which may be queued to this CPU at one time.
*
* The <intType>, <intArg1>, <intArg2>, and <intArg3> parameters allow a
* CPU to announce the method by which it is to be notified of input packets
* which have been queued to it.
*
* The <masterCpu> parameter specifies the CPU number of the shared memory
* master node.
*
* The <maxTicsWait> parameter sets the maximum wait interval allowed during
* probing of shared memory locations prior to attaching.  If equal to zero,
* a default value of 3000 is used.
*
* RETURNS: OK, or ERROR if the shared memory interface cannot be attached.
*
* INTERNAL
* The delayed start for slave processors used below is required.  The anchor
* may not yet be mapped to the bus.  So, probing of shared memory locations is
* used to overcome Bus Errors which occur on many boards if the slave accesses
* shared memory (SM) before the master has finished initializing it.  The code
* here simply delays access to the SM region until the SM master has finished
* initializing it.
*
* The method used is to repetitively probe key locations in the SM region
* after delay periods until valid values are detected or a time-out occurs.
* The initial delay period is set based on processor number.  (The master
* processor does not delay.)  If the first probe of a location fails, an
* exponential increase in delay period is used to reduce bus contention on
* subsequent probes.
*
* This method is no better than receiving raw BERRs and does reduce bus
* contention and the number of BERRs.
*
* NOMANUAL
*/

STATUS smNetAttach2
    (
    int		unit,			/* interface unit number */
    SM_ANCHOR * pAnchor,		/* addr of anchor */
    int		maxInputPkts,		/* max queued packets */
    int		intType,		/* interrupt method */
    int		intArg1,		/* interrupt argument #1 */
    int		intArg2,		/* interrupt argument #2 */
    int		intArg3, 		/* interrupt argument #3 */
    int		masterCpu,		/* master CPU number */
    int		maxTicsWait		/* max clock ticks to wait */
    )
    {
    SM_SOFTC *			xs;		/* softc pointer */
    SM_ANCHOR volatile *	pAnchorv = (SM_ANCHOR volatile *) pAnchor;
    SM_PKT_MEM_HDR volatile *	pSmPktHdr;	/* SM packet header */
    SM_PKT_CPU_DESC volatile *	pPktCpuDesc;			
    int				tmp;		/* temp storage */
    BOOL			cont = FALSE;

    if (unit >= NSM)
        {
	return (ERROR);
        }

    if ((xs = (SM_SOFTC *) calloc (sizeof (SM_SOFTC), 1)) == NULL)
        {
    	return (ERROR);
        }

    if (maxTicsWait == 0)
        {
        maxTicsWait = 3000;
        }

    sm_softc [unit] = xs;

    if (smIfAttach (unit, pAnchor, maxInputPkts, intType, intArg1, intArg2,
		    intArg3, sysClkRateGet (), smNetLoanNum) == OK)
    	{
        /*
         * If we are on master CPU, do not delay in probing shared memory,
         * else, use delayed probe of key locations to wait for master to
         * finish initialization of shared memory.  Slaves must wait because
         * all subsequent steps assume that the master has completed its
         * initialization of shared memory.
         */

        if (smUtilProcNumGet () != masterCpu)
            {
            /* First wait for valid anchor region */

            if (smNetProbe (pAnchor, maxTicsWait, TRUE, SM_READY) == ERROR)
                {
                printf ("smNetAttach2: Error: SM probe 1 time out\n");
                }

            /* Now wait for valid SM packet memory header area */

            else if (smNetProbe ((void *)&pAnchorv->smPktHeader,
                                 maxTicsWait, FALSE, 0) == ERROR)
                {
                printf ("smNetAttach2: Error: SM probe 2 time out\n");
                }

            /*
             * Wait for master to finish initializing packet memory header
             * before continuing.
             */

            else if (((pSmPktHdr =
                       SM_OFFSET_TO_LOCAL (ntohl (pAnchorv->smPktHeader),
                                           (unsigned)pAnchor,
                                           SM_PKT_MEM_HDR volatile *))
                                 == NULL) ||
                     (smNetProbe ((void *)&pSmPktHdr->pktCpuTbl,
                                  maxTicsWait, FALSE, 0) != OK))
                {
                printf ("smNetAttach2: Error: SM probe 3 time out\n");
                }

            /* Finally, wait for master to attach to SM backplane network */

            else if (((pPktCpuDesc =
                       SM_OFFSET_TO_LOCAL (ntohl (pSmPktHdr->pktCpuTbl),
                                           (unsigned)pAnchor,
                                           SM_PKT_CPU_DESC volatile *))
                                 == NULL) ||
                     (smNetProbe ((void *)&pPktCpuDesc->status,
                                  maxTicsWait, TRUE, SM_CPU_ATTACHED) != OK))
                {
                printf ("smNetAttach2: Error: SM probe 4 time out\n");
                }

            else
                {
                /* if sequential addressing - can calculate hw address */

                CACHE_PIPE_FLUSH ();            /* CACHE FLUSH   [SPR 68334] */
                tmp = pSmPktHdr->reserved1;     /* PCI bug       [SPR 68844] */
                if (pSmPktHdr->reserved1 != 0)
                    {
                    xs->xs_ac.ac_if.if_resolve = smNetAddrResolve;
                    }
                cont = TRUE;
                }
            }
        else
            cont = TRUE;

        if (cont)
            {
            /*
             * finish attachment by setting current receive state in
             * SM_SOFTC and connecting the SM interrupt service routine
             */

            xs->taskRecvActive = FALSE;

            if (smUtilIntConnect (LOW_PRIORITY, (FUNCPTR) smNetIntr, intType,
                                  intType, intArg1, intArg2, intArg3) == OK)
                {
                return (OK);
                }
            }

        /* clean up before taking error exit */

	if_dettach (&xs->xs_if);
	}

    (void) free ((caddr_t) xs);
    return (ERROR);
    }


/******************************************************************************
*
* smNetAttach - attach the shared memory network interface
*
* This routine attaches the shared memory interface to the network.  It is
* called once by each CPU on the shared memory network.  The <unit> parameter
* specifies the backplane unit number.
*
* The <pAnchor> parameter is the local address by which the local CPU may
* access the shared memory anchor.
*
* The <maxInputPkts> parameter specifies the maximum number of incoming
* shared memory packets which may be queued to this CPU at one time.
*
* The <intType>, <intArg1>, <intArg2>, and <intArg3> parameters allow a
* CPU to announce the method by which it is to be notified of input packets
* which have been queued to it.
*
* RETURNS: OK, or ERROR if the shared memory interface cannot be attached.
*
* NOMANUAL
*/

STATUS smNetAttach
    (
    int		unit,			/* interface unit number */
    SM_ANCHOR * pAnchor,		/* addr of anchor */
    int		maxInputPkts,		/* max queued packets */
    int		intType,		/* interrupt method */
    int		intArg1,		/* interrupt argument #1 */
    int		intArg2,		/* interrupt argument #2 */
    int		intArg3 		/* interrupt argument #3 */
    )
    {
    return (smNetAttach2 (unit, pAnchor, maxInputPkts, intType, intArg1,
                          intArg2, intArg3, 0, 0));
    }


/******************************************************************************
*
* smNetIntr - VxWorks interrupt service routine
*
* This routine is the interrupt service routine for shared memory input
* packets.  It processes the interrupt (if appropriate) then notifies the
* interface of the incomming packet by calling smIfInput via the netTask.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void smNetIntr
    (
    int  intType 		/* interrupt type */
    )
    {
    SM_SOFTC *	xs;			/* softc pointer */
    int		ix;			/* unit */

    /* check all SM interfaces for incoming packets */

    for (ix = 0; ix < NSM; ix++)
	{
	if (((xs = sm_softc [ix]) != NULL) &&
	    (xs->smPktDesc.status == SM_CPU_ATTACHED) &&
	    (smIfInputCount (xs, xs->smPktDesc.smDesc.cpuNum) > 0))
	    {
	    if (!xs->taskRecvActive)
		{
		xs->taskRecvActive = TRUE;
    		(void) netJobAdd ((FUNCPTR) smIfInput, (int) xs, 0, 0, 0, 0);
		}
	    }

	/* if bus interrupt, acknowledge it */

	if ((xs != NULL) && (intType == SM_INT_BUS))
	    sysBusIntAck (xs->smPktDesc.smDesc.intArg1);
	}
    }


/******************************************************************************
*
* smNetPulse - continually pulse the shared memory heartbeat
*
* This routine maintains the shared memory heart beat by incrementing 
* the heartbeat count and then re-scheduling itself to run again 
* after a "beat" has passed.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void smNetPulse
    (
    SM_PKT_MEM_HDR * pSmPktHdr	/* pointer to heartbeat */
    )
    {
    smPktBeat (pSmPktHdr);
    (void) wdStart ((WDOG_ID) pSmPktHdr->reserved2, (int) sysClkRateGet (),
	     	    (FUNCPTR) smNetPulse, (int) pSmPktHdr);
    }


/******************************************************************************
*
* smNetProbe - probe a shared memory location
*
* This routine probes a shared memory location.  If the location can be
* accessed and the value there is neither zero nor 0xffffffff, OK is returned.
* The probe is repeated periodically until the above conditions are met or
* until a maximum number of probes has been attempted.  If the maximum number
* of probes is exceeded, ERROR is returned.
*
* RETURNS: OK if successful probe, otherwise ERROR
*
* NOMANUAL
*/

LOCAL STATUS smNetProbe
    (
    void *	ptr,		/* location to probe */
    UINT	maxTicsWait,	/* max system clock ticks period to wait */
    BOOL	tstEqVal,	/* TRUE: test probe value == val */
    int		val		/* specific value for operation */
    )
    {
    int		probeVal;	/* value read during probe */
    UINT	tics;		/* SM probe delay period */
    int		chkVal = htonl (val);
    char *	cp     = (char *)ptr;

    tics = smUtilProcNumGet ();
    tics <<= 1;
    do
        {
        smUtilDelay (NULL, tics);
        if (smUtilMemProbe (cp, READ, 4, (char *)&probeVal) == OK)
            {
            if (tstEqVal)
                {
                if (probeVal == chkVal)
                    {
                    return (OK);
                    }
                }

            else if ((probeVal != (int)~0) && (probeVal != 0))
                {
                return (OK);
                }
            }
        tics <<= 1;
        } while ((tics < maxTicsWait) && (tics != 0));

    return (ERROR);
    }


/******************************************************************************
*
* loanBuild - build an mbuf cluster with a shared memory buffer
*
* This routine calls build cluster to build an mbuf cluster from a shared
* memory packet.
*
* RETURNS: pointer to the cluster if successful, otherwise NULL
*
* NOMANUAL
*/

struct mbuf * loanBuild
    (
    SM_SOFTC *		xs,		/* softc pointer */
    SM_PKT volatile *	pPkt,		/* shared memory packet */
    u_char *		pData,		/* pointer to data */
    int			len		/* length of packet */
    )
    {
    int			tmp;		/* temp storage */

    pPkt->header.type = 0;
    CACHE_PIPE_FLUSH ();            /* CACHE FLUSH   [SPR 68334] */
    tmp = pPkt->header.nBytes;      /* BRIDGE FLUSH  [SPR 68334] */
    return (build_cluster (pData, len, &xs->xs_if, MC_BACKPLANE,
			   (u_char *)&(pPkt->header.type),
			   (FUNCPTR)smIfLoanReturn, (int) xs, (int) pPkt, 0));
    }


/******************************************************************************
*
* smNetInetGet - get address associated with a shared memory network interface
*
* This routine returns the IP address in <smInet> for the CPU
* specified by <cpuNum> on the shared memory network specified by <smName>.
* If <cpuNum> is NONE (-1), this routine returns information about the local
* (calling) CPU.
*
* This routine can only be called after a call to smNetAttach().  It will
* block if the shared memory region has not yet been initialized.
*
* This routine is only applicable if sequential addressing is being used
* over the backplane.
*
* RETURNS: OK, or ERROR if the Internet address cannot be found.
*
* NOMANUAL
*/

STATUS smNetInetGet
    (
    char *	smName, 		/* device name */
    char *	smInet,			/* return inet */
    int		cpuNum			/* cpu number */
    )
    {
    SM_SOFTC *			xs;		/* softc pointer */
    SM_ANCHOR volatile *	pAnchor;	/* ptr to anchor */
    SM_PKT_MEM_HDR volatile *	pSmPktHdr;	/* packet header */
    struct in_addr		smAddr;		/* address */
    int				tmp;		/* temp storage */

    if ((xs = (SM_SOFTC *) ifunit (smName)) == NULL)
	{
	printf ("smNetInetGet:%s not attached\n", smName);
	return (ERROR);
	}

    pAnchor = (SM_ANCHOR volatile *) xs->smPktDesc.smDesc.anchorLocalAdrs;

    if (smIsAlive ((SM_ANCHOR *)pAnchor, (int *)&pAnchor->smPktHeader,
                   xs->smPktDesc.smDesc.base, 0, 0) == FALSE)
        {
	return (ERROR);
        }

    pSmPktHdr = SM_OFFSET_TO_LOCAL (ntohl (pAnchor->smPktHeader),
				    xs->smPktDesc.smDesc.base,
				    SM_PKT_MEM_HDR volatile *);

    CACHE_PIPE_FLUSH ();            /* CACHE FLUSH   [SPR 68334] */
    tmp = pSmPktHdr->reserved1;     /* PCI bug       [SPR 68844] */

    if (pSmPktHdr->reserved1 == 0)
        {
        /* no sequential addressing */

	return (ERROR);
        }

    if (cpuNum == NONE)
        {
	cpuNum = xs->smPktDesc.smDesc.cpuNum;
        }

    smAddr.s_addr = htonl (ntohl (pSmPktHdr->reserved1) + cpuNum);
    inet_ntoa_b (smAddr, smInet);

    return (OK);
    }


/******************************************************************************
*
* smNetAddrResolve - resolve an Ethernet address
*
* This routine converts a destination IP address into a destination
* Ethernet address.  It only resolves the address if sequential addressing
* is being used and the destination is attached to shared memory.
*
* RETURNS: OK if successful, otherwise ERROR.
*
* NOMANUAL
*/

LOCAL STATUS smNetAddrResolve
    (
    struct ifnet *   pIf,		/* ifnet pointer */
    struct in_addr * pIpDest,		/* dest ip address */
    u_char *	     pHwDest,		/* return hardware address */
    int *	     usetrailers	/* not used */
    )
    {
    SM_PKT_CPU_DESC volatile *	pPktCpuDesc;			
    u_long			dstHostAddr;
    int				destCpu;
    int				tmp;            /* temp storage */
    SM_SOFTC *			xs = sm_softc [pIf->if_unit]; /*softc pointer*/

    if (xs->masterAddr == 0)
	return (ERROR);		/* sequential addressing not enabled */

    dstHostAddr = pIpDest->s_addr & 0x00ffffff;

    destCpu = dstHostAddr - xs->masterAddr;

    if ((destCpu < 0) || (destCpu >= xs->smPktDesc.smDesc.maxCpus))
        return (ERROR);

    pPktCpuDesc = &((xs->smPktDesc.cpuLocalAdrs) [destCpu]);

    CACHE_PIPE_FLUSH ();            /* CACHE FLUSH   [SPR 68334] */
    tmp = pPktCpuDesc->status;      /* PCI bug       [SPR 68844] */

    if (pPktCpuDesc->status != SM_CPU_ATTACHED)
	return (ERROR);

    /*
     * Synthesize SM physical (MAC) address.
     *
     * Under IPv4, SM ethernet physical addresses (EUI-48) consist of 48 bits:
     * { 0x00, 0x02, 0xE2, hostIpAdrs[3], hostIpAdrs[4], hostIpAdrs[5] }
     *
     * Under IPv6 (EUI-64) the 64-bit format is:
     * { 0x00, 0x02, 0xE2, 0xFF, 0xFE,
     *   hostIpAdrs[3], hostIpAdrs[4], hostIpAdrs[5] }
     *
     * XXXmas: FUTURE: The most significant 24 bits are the vendor ID.  Wind
     * River's vendor ID is 0x00 0x40 0x47, not 0x00 0x02 0xE2!  Must be
     * changed in next major release.
     */

    pHwDest [0] = 0x00;
    pHwDest [1] = 0x02;
    pHwDest [2] = 0xE2;
    pHwDest [3] = (u_char)((dstHostAddr >> 16) & 0xff);
    pHwDest [4] = (u_char)((dstHostAddr >> 8) & 0xff);
    pHwDest [5] = (u_char)(dstHostAddr & 0xff);

    /* entered into arp table for efficency */

    arpCmd ((int)SIOCSARP, pIpDest, pHwDest, NULL);
    *usetrailers = 0;
    return (OK);
    }

