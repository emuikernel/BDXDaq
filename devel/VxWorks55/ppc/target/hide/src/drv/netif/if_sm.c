/* if_sm.c - shared memory backplane network interface driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01o,07jun02,mas  total packet length compared to SM packet size (SPR 78405)
01n,03may02,mas  now receives its own broadcasts (SPR 9676); packets in excess
		 of MTU size are just deleted and an optional error message is
		 printed (SPR 29305); cache flushing and volatile ptr fixes
		 (SPR 68334); bridge flushing fix (SPR 68844); now uses
		 netMblkToBufCopy() rather than copyFromMbufs().
01m,24oct01,mas  fixed diab warnings (SPR 71120); doc update (SPR 71149)
01l,02oct01,mas  no user callable routines, all are NOMANUAL (SPR 4547)
01k,24jul96,vin  upgraded to bsd4.4.
01k,14dec96,dat  fixed SPR 2258. chg'd inet_ntoa to inet_ntoa_b
01j,11jan95,rhp  remove reference to unpublished lib from man page.
01i,19feb93,jdi  documentation cleanup.
01h,09sep92,gae  documentation tweaks.
01g,11jun92,elh  added documentation, more cleanup, added parameter
		 to smIfAttach.
01f,02jun92,elh  the tree shuffle
01e,27may92,elh  Incorperated the changes caused from restructuring the
		 shared memory libraries.
01d,17may92,elh  renamed routines from sm to smIf
01c,03may92,elh  changed to use subnet in shared memory when
		 calculating host address (smIfHwAddrSet).
01b,12apr92,elh  added in support for sequential addressing.
01a,17nov90,elh  written.
*/

/*
DESCRIPTION
This module implements the VxWorks shared memory backplane network
interface driver.  

This driver is designed to be moderately generic, operating unmodified
across the range of hosts and targets supported by VxWorks.  To achieve 
this, the driver must be given several target-specific parameters, and 
some external support routines must be provided.  These parameters are 
detailed below.

There are no user-callable routines.

This driver is layered between the shared memory packet library and the network 
modules.  The backplane driver gives CPUs residing on a common backplane 
the ability to communicate using IP (via shared memory).

BOARD LAYOUT
This device is "software only."  There is no jumpering diagram required.

INTERNAL
This file no longer runs under SunOs.

File Layout:
This network interface driver has been physically split up into the 
the operating system specific code (smNetLib) and the common network
interface code (if_sm).  Dividing the driver up this way layers 
the driver and reduces the number of conditional compilations and 
thereby makes the code cleaner and more readable.

		     ____________________		
vxWorks interface    |smNetLib (vxWorks) |	    |
to backplane driver  ____________________	    |backplane
			|                           |driver
                	|                           |
                    	|                           |
			v     			    |
shared  memory       _______________		    |
backplane driver     |   if_sm	     |		    |
		     _______________		
			     |
		---------------------------------------
			     |			    | shared memory
		             v			    | libraries
 	 		____________		    |
	 		| smPktLib | 
			____________ 


smNetLib 

smNetLib implements the interface between the OS and the driver.
There exists an smNetLib for both UNIX and VxWorks.
Each smNetLib must implement how the OS initializes and attaches 
the driver, interrupt handling, etc as well as the OS dependent 
system calls.  

if_sm 

if_sm contains the network interface code that is common to most 
BSD based systems.  It is the interface to the network modules and does 
the packet processing.


OS Interface:

The interfaces into this module from the OS specific modules follow.  

smIfAttach -  attaches the shared memory network interface driver 
to the network modules.  smIfAttach typically gets called once per 
backplane interface.  The shared memory region must have been 
setup and initialized (via smPktSetup) prior to calling smIfAttach. 

smIfInput -  gets called from the OS specific interrupt service handler 
to process input shared memory packets and to pass them up to the 
network modules.  

smIfLoanReturn - return a previously loaned shared memory buffer.


OS Dependent Routines:

The following routines/macros must be provided by the OS specific 
modules for this module: 

    UNIT_TO_SOFTC	convert unit to softc pointer
    outputHook		output hook for packet
    inputHook		input hook for packet
    etherAddrPtr	return pointer to ethernet address
    netMblkToBufCopy	copy from mbuf chain to passed buffer
    copyToMbufs		copy from buffer to mbuf chain
    deviceValid		if device is a valid device
    USE_CLUSTER		use a cluster mbuf
    loanBuild		build a loan mbuf buffer

There should be better documentation discussing the interfaces, 
functionality and expectations of the OS spcecific library.

Optional function pointer hook for use by netMblkToBufCopy() routine:

FUNCPTR  smBufCpyFuncHook

By default this is set to NULL which denotes use of bcopy().  Any other
routine specified here must adhere to the same input argument specification
as bcopy().

TARGET-SPECIFIC PARAMETERS
A set of target-specific parameters is used to configure shared memory and
backplane networking.

\is
\i `local address of anchor'
This parameter is the local address by which the local CPU accesses the shared
memory anchor.
\i `maximum number of input packets'
This parameter specifies the maximum number of incoming shared memory packets
that can be queued to this CPU at one time.
\i `method of notification'
These four parameters are used to enable a CPU to announce the method by which
it is to be notified of input packets that have been queued to it.
\i `heartbeat frequency'
This parameter specifies the frequency of the shared memory backplane
network's heartbeat, which is expressed in terms of the number of CPU ticks on
the local CPU corresponding to one heartbeat period.
\i `number of buffers to loan'
This parameter, when non-zero, specifies the number of shared memory packets
available to be loaned out.
\i `master CPU number'
This parameter specifies the master CPU number as set during system
configuration.
\ie
For detailed information refer to 
\tb VxWorks Network Programmer's Guide: Data Link Layer Network Compnents.

INCLUDE FILES: smNetLib.h

SEE ALSO: ifLib, smNetLib, 
\tb VxWorks Network Programmer's Guide

NOROUTINES
*/

/* includes */

#ifdef	UNIX
#include "smNetLib.h"
#else
#include "drv/netif/smNetLib.h"
#endif
#include "cacheLib.h"
#include "netBufLib.h"
#include "smLib.h"
#include "smPktLib.h"
#include "smUtilLib.h"


/* globals */

BOOL		smIfVerbose	= FALSE;	/* verbose mode */

/* packet copy routine ptr (NULL = bcopy) */

FUNCPTR		smBufCpyFuncHook = NULL;

/* forward declarations  */
						/* network hooks */
LOCAL void 	smIfReset (int unit);
LOCAL int 	smIfInit (int unit);
LOCAL int 	smIfIoctl (struct ifnet * pIf, UINT cmd, caddr_t data);
LOCAL void 	smIfRecv (SM_SOFTC * sm, SM_PKT volatile * pPkt,
                          BOOL * retBuffer);
LOCAL void 	smIfHwAddrSet (int unit);
LOCAL void 	smIfStartOutput (SM_SOFTC * xs);
IMPORT void	if_down (struct ifnet *);


/******************************************************************************
*
* smIfAttach - publish the `sm' interface and initialize the driver and device
*
* This routine attaches an `sm' Ethernet interface to the network, if the
* interface exists.  This routine makes the interface available by filling in
* the network interface record.  The system will initialize the interface
* when it is ready to accept packets.
*
* The shared memory region must have been initialized, via smPktSetup(), prior
* to calling this routine (typically by an OS-specific initialization routine).
* The smIfAttach() routine can be called only once per unit number.
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
* The <ticksPerBeat> parameter specifies the frequency of the shared memory
* anchor's heartbeat.  The frequency is expressed in terms of the number of CPU
* ticks on the local CPU corresponding to one heartbeat period.
*
* If <numLoan> is non-zero, it specifies the number of shared memory 
* packets available to be loaned out.
*
* RETURNS: OK or ERROR.
*
* NOMANUAL
*/

STATUS smIfAttach
    (
    int		unit,		/* interface unit number */
    SM_ANCHOR * pAnchor,	/* local addr of anchor */
    int		maxInputPkts,	/* max no. of input pkts */
    int		intType,	/* method of notif. */
    int		intArg1,	/* interrupt argument #1 */
    int		intArg2,	/* interrupt argument #2 */
    int		intArg3,	/* interrupt argument #3 */
    int		ticksPerBeat,	/* heartbeat freq. */
    int		numLoan		/* no. of buffers to loan */ 
    )
    {
    SM_SOFTC *	xs;		/* softc pointer */

    if ((xs = UNIT_TO_SOFTC (unit)) == NULL)
        {
	return (ERROR);
        }

    bzero ((caddr_t) xs, sizeof (SM_SOFTC));


    /* Initialize the shared memory descriptor.  */

    smPktInit (&xs->smPktDesc, pAnchor, maxInputPkts, ticksPerBeat,
	       intType, intArg1, intArg2, intArg3);

    if (numLoan)
	{
	xs->bufFree = numLoan;
	if (smIfVerbose)
	    printf ("smIfAttach:%d loan buffers\n", xs->bufFree);
        }

    ether_attach (
    		 &xs->xs_if, 
		 unit, 
		 "sm", 
		 smIfInit, 
		 smIfIoctl, 
		 (FUNCPTR) ether_output, /* generic ether_output */
		 (FUNCPTR) smIfReset
    		 );
    xs->xs_if.if_start = (FUNCPTR) smIfStartOutput; 
    return (OK);
    }


/******************************************************************************
*
* smIfInit - initialize the backplane interface
*
* smIfInit initializes the backplane interface by attaching to shared memory
* and by marking the interface as up and running.  This routine gets called
* (indirectly) through smIfIoctl which has the spl semaphore.
*
* INTERNAL
* We must attach to shared memory here (instead of smIfAttach) because we
* can not wait around at boot time (in UNIX the case) waiting for shared
* memory to become alive.
*
* RETURNS:  OK if successful, errno otherwise.
*
* NOMANUAL
*/

LOCAL int smIfInit
    (
    int  unit			/* unit number */
    )
    {
    SM_SOFTC *  xs;			/* softc pointer */
    SM_PKT_INFO smPktInfo;		/* info structure */

    xs = UNIT_TO_SOFTC (unit);

    /* attach to shared memory */

    if (smPktAttach (&xs->smPktDesc) == ERROR)
	{
	if (smIfVerbose)
	    printf ("smIfInit:couldn't attach [%x]\n", errno);
    	return (errno);
	}

    smIfHwAddrSet (unit);			/* set hardware address */
    (void) smPktInfoGet (&xs->smPktDesc, &smPktInfo);

    xs->xs_if.if_mtu = smPktInfo.maxPktBytes - SIZEOF_ETHERHEADER;

    /*
     * Set current interface status and characteristics.
     * Interface must be marked simplex so that it can receive its
     * own broadcasts.  [SPR 9676]
     */

    xs->xs_if.if_flags |= IFF_UP | IFF_RUNNING | IFF_MULTICAST | IFF_SIMPLEX;

    return (OK);
    }


/******************************************************************************
*
* smIfStartOutput - put packet in shared memory
*
* This routine removes an mbuf chain from the interface send queue, copies
* the data into a shared memory packet and calls smPktSend to send it off.
* This routine gets called from ether_output, after it has taken the 
* spl semaphore.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void smIfStartOutput
    (
    SM_SOFTC * xs		/* ptr to ctrl structure */
    )
    {
    struct mbuf *	  pMbuf;	/* mbuf pointer */
    int			  len;		/* data length */
    int                   tmp;		/* temp storage */
    UINT		  destCPU;	/* destination cpu */
    struct ether_header * pEh;		/* enet header */
    SM_PKT volatile *	  pPkt = NULL;	/* packet pointer */
    BOOL 		  retBuffer;	/* return buffer */
    u_char * 		  pEaddr;

    while (xs->xs_if.if_snd.ifq_head != NULL)
    	{
	/* dequeue mbuf chain, get a free shared memory packet */

    	IF_DEQUEUE (&xs->xs_if.if_snd, pMbuf);

        /* drop packet if it exceeds MTU size  (SPR 29305, 78405) */

        if (pMbuf->m_pkthdr.len > xs->smPktDesc.maxPktBytes)
            {
            m_freem (pMbuf);
            if (smIfVerbose)
                {
                printf ("smIfStartOutput: pkt len [%#x] > MTU [%#x]\n",
                        pMbuf->m_pkthdr.len, xs->smPktDesc.maxPktBytes);
                }
            xs->xs_if.if_oerrors++;
            continue;
    	    }

    	if ((smPktFreeGet (&xs->smPktDesc, (SM_PKT **)&pPkt) == ERROR) ||
    	    (pPkt == NULL))
    	    {
	    m_freem (pMbuf);
	    if (smIfVerbose)
	    	printf ("smIfStartOutput:smPktFreeGet [%x]\n", errno);
    	    xs->xs_if.if_oerrors++;
	    continue;
    	    }

    	pEh = (struct ether_header *) pPkt->data;	/* copy data to pkt */
        len = netMblkToBufCopy ((M_BLK_ID)pMbuf, (char*)pEh, smBufCpyFuncHook);
    	pPkt->header.nBytes = len;
        CACHE_PIPE_FLUSH ();		/* CACHE FLUSH   [SPR 68334] */
        tmp = pPkt->header.nBytes;	/* BRIDGE FLUSH  [SPR 68334] */
        m_freem (pMbuf);

   	if (outputHook (&xs->xs_if, (char *)pEh, pPkt->header.nBytes) != 0)
	    {
	    (void) smPktFreePut (&xs->smPktDesc, (SM_PKT *)pPkt);
    	    continue;
	    }

    	if (bcmp ((caddr_t) etherAddrPtr (pEh->ether_dhost),
		  (caddr_t) etherAddrPtr (etherbroadcastaddr) ,
		  sizeof (pEh->ether_dhost)) == 0)
            {
    	    destCPU = SM_BROADCAST;	 /* specify broadcast */
            }

    	else
    	    {
	    pEaddr = etherAddrPtr (pEh->ether_dhost);

            if (xs->masterAddr != 0)		/* sequential addressing */
		{
		destCPU = ((pEaddr [3] << 16)|(pEaddr [4] << 8)| pEaddr [5]) -
		          xs->masterAddr;
		}
	    else
		destCPU = pEaddr [5];

            /* receive packets sent to self */

    	    if (destCPU == xs->smPktDesc.smDesc.cpuNum)
    		{
    		smIfRecv (xs, pPkt, &retBuffer);
                if (retBuffer)
                    (void) smPktFreePut (&xs->smPktDesc, (SM_PKT *)pPkt);
                continue;                 /* don't use SM to send to self */
    		}
    	    }

  	if (smIfVerbose)
	    {
	    printf ("smIfStartOutput: [0x%x] len:%d src:%s ", pEh->ether_type,
		    len, ether_sprintf (etherAddrPtr (pEh->ether_shost)));
	    printf ("dst:%s cpu [%d]\n",
		    ether_sprintf (etherAddrPtr (pEh->ether_dhost)), destCPU);
	    }
	xs->xs_if.if_opackets++;
	    
    	if (smPktSend (&xs->smPktDesc, (SM_PKT *)pPkt, destCPU) == ERROR)
    	    {
	    if (smIfVerbose)
	    	printf ("smIfStartOutput:smPktSend failed 0x%x\n", errno);

    	    xs->xs_if.if_oerrors++;
	    xs->xs_if.if_opackets--;

	    /*
	     * need to return shared memory packet on error,
	     * unless it's a incomplete broadcast error.
	     */

    	    if (errno != S_smPktLib_INCOMPLETE_BROADCAST)
    		(void) smPktFreePut (&xs->smPktDesc, (SM_PKT *)pPkt);
    	    }
    	}
    }


/******************************************************************************
*
* smIfReset - reset the backplane interface
*
* smIfReset resets the interface.  This gets called when the system
* receives a reset.  Basically this routine cleans up the state and marks
* the interface as inactive before it goes away.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void smIfReset
    (
    int  unit				/* unit number */
    )
    {
    SM_SOFTC * xs = UNIT_TO_SOFTC (unit);	/* softc pointer */
    int        spl;				/* spl level 	 */

    if (!deviceValid (unit))
    	return;

    /* detach from network */

    spl = splimp ();
    xs->xs_if.if_flags &= ~IFF_RUNNING;
    if_down (&xs->xs_if);
    splx (spl);

    /* detach from shared mem */

    (void) smPktDetach (&xs->smPktDesc, SM_FLUSH);
    }


/******************************************************************************
*
* smIfIoctl - process an ioctl request
*
* smIfIoctl gets called to perform a control operation on the backplane network
* interface.  The implemented commands include: SIOCSIFADDR (set interface
* address) SIOCSIFFLAGS (set interface flags), SIOCGIFADDR get (link-level)
* interface address.
*
* RETURNS: OK if successful, otherwise errno.
*
* NOMANUAL
*/

LOCAL int smIfIoctl
    (
    struct ifnet * pIf,		/* interface pointer */
    UINT	   cmd,		/* ioctl command */
    caddr_t 	   data		/* command data */
    )
    {
    struct ifreq * pIfr;		/* if request */
    int		   retVal = OK;		/* error value */
    int		   spl    = splimp ();	/* spl level */
    SM_SOFTC *	   xs     = UNIT_TO_SOFTC (pIf->if_unit);
    struct in_addr ipAddr;		/* address passed */
    struct in_addr expAddr;		/* expected address */
    char	   netString[32];

    pIfr = (struct ifreq *) data;

    switch (cmd)
        {
        case SIOCSIFADDR:			/* set interface address */
    	    retVal = set_if_addr (pIf, data, etherAddrPtr (xs->xs_enaddr));

	    if ((retVal == OK) && (xs->masterAddr != 0))
		{
	    	/*
	    	 * We validate address after set_if_addr because
	         * shared memory must be up (smPktAttach) before master
	     	 * address can be determined.
	     	 */

		ipAddr = ((struct in_ifaddr *) data)->ia_addr.sin_addr;
	    	expAddr.s_addr = htonl (
				 ntohl (xs->smPktDesc.hdrLocalAdrs->reserved1)+
			  	 xs->smPktDesc.smDesc.cpuNum);

	    	if (expAddr.s_addr != ipAddr.s_addr)
		    {
		    inet_ntoa_b (expAddr, netString);
	            printf ("Seqential addressing enabled - expected address:"
			    "%s\n", netString);
		    smIfReset (pIf->if_unit);
	    	    retVal = ERROR;
	            }
	        }
    	    break;

	case SIOCSIFFLAGS:			/* set interface flags */
    	    if (pIf->if_flags & IFF_UP)
		smIfInit (pIf->if_unit);
	    break;

        case SIOCGIFADDR:			/* get link level address */
	    bcopy ((caddr_t) etherAddrPtr (xs->xs_enaddr),
		   (caddr_t)(pIfr->ifr_addr.sa_data), sizeof (xs->xs_enaddr));
            break;

	case SIOCADDMULTI:
	case SIOCDELMULTI:
	    if (pIfr == 0)
		{
		retVal = EINVAL; 		/* invalid argument passed */
		break;
		}

	    switch (pIfr->ifr_addr.sa_family)
		{
#ifdef INET
		case AF_INET:			/* do nothing */
		    break;
#endif
		default:
		    retVal = EAFNOSUPPORT;
		    break;
		}
	    break;


    	default:
	    if (smIfVerbose)
		printf ("smIfIoctl: [0x%x] command not implemented\n", cmd);
    	    retVal = EINVAL;
	}

    (void) splx (spl);
    return (retVal);
    }


/******************************************************************************
*
* smIfInput - handle an input packet
*
* smIfInput gets called from the OS specific interrupt handling routine
* to process a shared memory input packet.  It calls smPktRecv to obtain
* the packets from shared memory and then calls smIfRecv to process the
* packet and pass it up to the network modules.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void smIfInput
    (
    SM_SOFTC * xs		/* softc pointer */
    )
    {
    SM_PKT *   pPkt;		/* shared memory packet */
    int        status;		/* status */
    BOOL       retBuffer;	/* return packet */
    int        cpuNum;		/* my cpu number */

    cpuNum = xs->smPktDesc.smDesc.cpuNum;

    do
	{
	xs->taskRecvActive = TRUE;

    	while (((status = smPktRecv (&xs->smPktDesc, &pPkt)) == OK) &&
    	       (pPkt != NULL))
	    {
    	    smIfRecv (xs, pPkt, &retBuffer);
	    if (retBuffer)
		(void) smPktFreePut (&xs->smPktDesc, pPkt);
	    }

	if (status == ERROR)
	    {
	    xs->xs_if.if_ierrors++;
	    if (smIfVerbose)
	        printf ("smIfInput:error receiving packet 0x%x\n", errno);
	    }

	xs->taskRecvActive = FALSE;
	} while ((xs->smPktDesc.status == SM_CPU_ATTACHED) &&
	         (smIfInputCount (xs, cpuNum) > 0));
    }


/******************************************************************************
*
* smIfRecv - process shared memory input packet
*
* smIfRecv is called once per packet that arrives.  It calls check_trailer
* to process trailers.  Copies that packet into an mbuf chain (if buffer
* loaning is not set).  Then passes the packet to the network layer by calling
* do_protocol.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void smIfRecv
    (
    SM_SOFTC *        xs,		/* softc pointer */
    SM_PKT volatile * pPkt,		/* packet */
    BOOL *            retBuffer		/* kept buffer */
    )
    {
    struct ether_header * pEh;		/* ethernet header */
    int			  len;		/* length */
    u_char *		  pData;	/* pointer to data */
    struct mbuf *	  pMbuf = NULL;	/* mbuf pointer */

    *retBuffer = TRUE;    		/* return buffer by default */

    CACHE_PIPE_FLUSH ();		/* CACHE FLUSH   [SPR 68334] */
    len = pPkt->header.nBytes;		/* PCI bug       [SPR 68844] */
    len = pPkt->header.nBytes;
    pEh = (struct ether_header *) pPkt->data;

    if (inputHook (&xs->xs_if, (char *) pEh, len))
	return;

    if (len < SIZEOF_ETHERHEADER)
	{
	if (smIfVerbose)
	    printf ("smIfRecv:invalid packet len %d\n", len);
	xs->xs_if.if_ierrors++;
	return;					/* invalid packet size */
	}

    if (smIfVerbose)				/* debugging aide */
	{
        printf ("smIfRecv [0x%x] len: %d src:%s ", ntohs (pEh->ether_type), len,
	        ether_sprintf (etherAddrPtr (pEh->ether_shost)));
	printf ("dst:%s\n", ether_sprintf (etherAddrPtr (pEh->ether_dhost)));
	}

    len -= SIZEOF_ETHERHEADER;			/* adj. for enet header */
    pData = ((unsigned char *) pEh) + SIZEOF_ETHERHEADER;

    if (len == 0)
	return;					/* packet with no data */

    /*
     * try to build a loaned mbuf cluster (if buffering loaning is enabled,
     * there are loan buffers available, the packet doesn't use
     * trailers, and the packet is big enough).  Otherwise copy it
     * to an mbuf.
     */

    if ((xs->bufFree > 0) && (USE_CLUSTER (len)))
  	{
	if ((pMbuf = loanBuild (xs, pPkt, pData, len)) != NULL)
  	    {
	    xs->bufFree--;
	    *retBuffer = FALSE;
	    }
	}

    if (pMbuf == NULL)
	{
    	if ((pMbuf = (struct mbuf *)
		     copyToMbufs (pData, len, 0, &xs->xs_if)) == NULL)
	    return;
    	}

    do_protocol (pEh, pMbuf, &xs->xs_ac, len);
    xs->xs_if.if_ipackets++;
    }


/******************************************************************************
*
* smIfLoanReturn - return shared memory packet to shared memory pool
*
* smPktReturn gets called from the network modules to return the loaned
* shared memory buffer once the network code has finished with it.  It gets
* called with the spl semaphore taken.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void smIfLoanReturn
    (
    SM_SOFTC * xs,            /* softc pointer */
    SM_PKT *   pPkt           /* shared memory packet */
    )
    {
    if (smPktFreePut (&xs->smPktDesc, pPkt) == ERROR)
	return;

    xs->bufFree++;
    }


/******************************************************************************
*
* smIfHwAddrSet - set hardware address
*
* This routine sets the hardware address associated with the backplane 
* interface specified by <unit>.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void smIfHwAddrSet
    (
    int  unit 		/* unit number */
    )
    {
    u_char * 	     pEaddr;			/* ethernet address */
    u_long	     addr;			/* address */
    int              tmp;			/* temp storage */
    SM_PKT_MEM_HDR volatile * pSmPktHdr;	/* packet header */
    SM_SOFTC *       xs = UNIT_TO_SOFTC (unit); /* softc pointer */

    pEaddr = etherAddrPtr(xs->xs_enaddr);

    bzero ((caddr_t)pEaddr, 6);		/* fill in ethernet address */
    pEaddr [0] = 0x00;
    pEaddr [1] = 0x02;
    pEaddr [2] = 0xE2;

    pSmPktHdr = (SM_PKT_MEM_HDR volatile *)xs->smPktDesc.hdrLocalAdrs;
    CACHE_PIPE_FLUSH ();		/* CACHE FLUSH   [SPR 68334] */
    tmp = pSmPktHdr->reserved1;		/* PCI bug       [SPR 68844] */

    if (pSmPktHdr->reserved1 != 0)		/* sequential addressing */
	{
	/*
	 * The backplane hw address consists of
	 * { 0x00, 0x02, 0xE2, ip[1], ip[2], ip[3] }.
	 * where ip is the lower three bytes of
	 * the IP interface address.
	 */

	xs->masterAddr = ntohl (pSmPktHdr->reserved1) & 0x00ffffff;
	addr = xs->masterAddr + xs->smPktDesc.smDesc.cpuNum;

	pEaddr [3] = (u_char)((addr >> 16) & 0xff);
	pEaddr [4] = (u_char)((addr >> 8)  & 0xff);
	pEaddr [5] = (u_char)(addr & 0xff);
	}
    else
	{
    	/*
    	 * Backplane hw address consists of
     	 * { 0x00, 0x02, 0xE2, 0x00, unit, cpu }.
     	 */

	pEaddr [4] =  (u_char)unit;
	pEaddr [5] =  (u_char)xs->smPktDesc.smDesc.cpuNum;
	}

    if (smIfVerbose)
	printf ("hw address:%s\n", ether_sprintf (pEaddr));
    }

