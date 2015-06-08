/* tempNetif.c - template ethernet network interface driver */

/* Copyright 1989-1997 Wind River Systems, Inc. */

/*
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

modification history
--------------------
01d,24sep97,dat  documentation cleanup, added TEMPLATE_NETIF_{READ | WRITE}
01c,25aug97,dat  removed IFNET references
01b,08aug97,dat  fixed compilation bugs
01a,30jul97,dat  written (from if_ei.c, ver 03h)
*/

/*
.SH TODO
Replace the documentation for this template driver with documentation
for the driver being written.  

Begin with an overview of the complete device.  Indicate if the new driver
only implements a sub-section of the whole device or not.

Describe all of the operating modes of the device, and indicate which
ones this driver implements.

Document the device initialization steps to be used in the BSP to create
and initialize the device.  Document all the macros that
can be used to customize the driver to a particular hardware environment.

Document anything that will help the user to understand how this device
works and interacts with this driver.

This template assumes the device uses some type of transmit frame descriptor
(tfd) and a similar receive frame descriptor (rfd).  There is an array or
list for each.  There are dummy declarations for a TFD and an RFD structure.

INTERRUPTS
This driver requires the BSP to perform the intConnect function.  The BSP
should connect the templateInt() function using a single argument 'unit'
to the appropriate vector.  This driver will use the TEMPLATE_INT_ENABLE()
and TEMPLATE_INT_DISABLE() macros to enable and disable interrupts. The
macro TEMPLATE_INT_INIT() is used to initialize the interrupt system during
driver initialization time.  The macro TEMPLATE_INT_ACK() is used to
perform interrupt acknowledge functions external to the driver.

HARDWARE 
The macros TEMPLATE_NETIF_READ() and TEMPLATE_NETIF_WRITE() are used for
all accesses to actual chip.  These macros can be redefined to create
custom modules for special situations.

NOTES:
This template driver was abstracted from the Intel 82596 driver, if_ei.c.
It is a fairly common device that has its own DMA controller built in.
Transmit data is assigned to a TFD (transmit frame descriptor) and the TFD
is linked to a command list that the device monitors.  Similar incoming
packets are in RFDs (receive frame descriptors).  An interrupt occurs when
there are RFDs ready with incoming data.  The RFD is then unlinked from the
receive queue and processed.  When the incoming data is no longer needed the
buffer and RFD and returned to the free pool of RFDs, for reuse.

WARNINGS:
Do not use printf statements within code executed by netTask. A deadlock
is possible when the shell is redirected to the network.  Use logMsg()
instead. (SPR 8954)

SEE ALSO: ifLib
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
#include "net/if.h"
#include "net/route.h"
#include "iosLib.h"
#include "errnoLib.h"

#include "cacheLib.h"
#include "logLib.h"
#include "netLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysLib.h"

#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"
#include "netinet/if_ether.h"

#include "etherLib.h"
#include "net/systm.h"
#include "sys/times.h"
#include "net/if_subr.h"


/* local definitions */

#define EH_SIZE		14
#define MAX_UNITS       4                   /* maximum units to support */
#define MAX_RFDS_LOANED 8                   /* max RFDs that can be loaned */

/* Typedefs for external structures that are not typedef'd in their .h files */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;                  /* Interface Data Record wrapper */
typedef struct ifnet IFNET;                 /* real Interface Data Record */
typedef struct sockaddr SOCK;

typedef struct rfd { /* TODO - This is a dummy RFD structure */
    ETH_HDR* pEnetHdr;
    int status;
    int count;
    int reserved;
    int actualCnt;
    int refCnt;
    char * enetData;
    int bufSize;
    } RFD;

typedef RFD	TFD;	/* TODO - This is a dummy TFD structure */

/* The definition of the driver control structure */

typedef struct drv_ctrl	/* TODO - This is a dummy driver control structure */
    {
    IDR			idr;		/* interface data record */
    BOOL		attached;	/* indicates attach() called */
    volatile UCHAR *	pCsr;		/* Control status register */
    char *		memBase;	/* memory pool base */
    volatile BOOL	rcvHandling;	/* flag, indicates netTask active */
    volatile BOOL	txCleaning;	/* flag, indicates netTask active */
    volatile BOOL	txIdle;		/* flag, indicates idle transmitter */
    volatile int	nLoanRfds;	/* number of loanable RFDs left */
    CACHE_FUNCS		cacheFuncs;	/* cache descriptor */
    int			intLevel;	/* interrupt level # */
    } DRV_CTRL;

#define DRV_CTRL_SIZ  sizeof(DRV_CTRL)

/* hardware abstraction macros */

#ifndef TEMPLATE_NETIF_READ
#define TEMPLATE_NETIF_READ(pDrvCtrl, reg, result) \
	((result) = (*(pDrvCtrl->reg)))
#endif /* TEMPLATE_NETIF_READ */

#ifndef TEMPLATE_NETIF_WRITE
#define TEMPLATE_NETIF_WRITE(pDrvCtrl, reg, data) \
	((*(pDrvCtrl->reg)) = (data))
#endif /* TEMPLATE_NETIF_WRITE */

/* Dummy declarations of bits in the CSR register */

#define TEMPLATE_CSR_RX_PEND	0x1
#define TEMPLATE_CSR_TX_PEND	0x2
#define TEMPLATE_CSR_RFD_READY	0x4

/* Dummy declarations of CSR commands */

#define TEMPLATE_CMD_RESET	0x1
#define TEMPLATE_CMD_TX_START	0x2
#define TEMPLATE_CMD_RX_START	0x4

/* declarations for default macro definitions */

IMPORT	STATUS sysIntEnable (int);
IMPORT	STATUS sysIntDisable (int);
IMPORT	STATUS sysIntAck (int);
IMPORT	STATUS sysTemplateAddrGet (int, void *);

#ifndef TEMPLATE_INT_ENABLE		/* enable this interrupt */
#define TEMPLATE_INT_ENABLE(pDrv) \
	sysIntEnable (pDrv->intLevel);
#endif TEMPLATE_INT_ENABLE

#ifndef TEMPLATE_INT_DISABLE		/* disable this interrupt */
#define TEMPLATE_INT_DISABLE(pDrv) \
	sysIntEnable (pDrv->intLevel);
#endif TEMPLATE_INT_DISABLE

#ifndef TEMPLATE_INT_INIT		/* initialize interrupts */
#define TEMPLATE_INT_INIT(pDrv) \
	/* do nothing */
#endif TEMPLATE_INT_INIT

#ifndef TEMPLATE_INT_ACK		/* acknowledge interrupt */
#define TEMPLATE_INT_ACK(pDrv) \
	sysIntAck (pDrv->intLevel);
#endif TEMPLATE_INT_ACK

#ifndef TEMPLATE_ENET_GET		/* get ethernet MAC address */
#define TEMPLATE_ENET_GET(pDrvCtrl, pResult) \
	*pResult = sysTemplateAddrGet (pDrvCtrl->idr.ac_if.if_unit, \
				(char *)pDrvCtrl->idr.ac_enaddr);
#endif TEMPLATE_ENET_GET


/* local data */

LOCAL DRV_CTRL drvCtrl [MAX_UNITS]; 	/* array of driver control structs */


/* forward function declarations */

IMPORT STATUS	templateAttach (int unit, UCHAR* pCsr, int iLevel,
				char* memBase, int nTfds, int nRfds);
IMPORT void	templateInt (int unit);

LOCAL STATUS	templateInit (int unit);
LOCAL void	templateReset (int unit);
LOCAL int	templateIoctl (IDR *pIDR, int cmd, caddr_t data);
LOCAL int	templateOutput (IDR *pIDR, MBUF *pMbuf, SOCK *pDestAddr);
LOCAL void	templateTxStart (int unit);
LOCAL void	templateHandleRecv (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	templateReceive (DRV_CTRL *pDrvCtrl, RFD *pRfd);
LOCAL void	templateLoanFree (DRV_CTRL *pDrvCtrl, RFD *pRfd);
LOCAL void	templateRxStart (DRV_CTRL *pDrvCtrl);
LOCAL void	templateTfdSend (DRV_CTRL *pDrvCtrl, TFD *pTfd);
LOCAL void	templateTfdStart (DRV_CTRL *pDrvCtrl);
LOCAL void	templateRfdReturn (DRV_CTRL *pDrvCtrl, RFD *pRfd);
LOCAL RFD	*templateRfdGet (DRV_CTRL *pDrvCtrl);
LOCAL BOOL	templateRfdReady (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	templateDeviceStart (int unit);

/*******************************************************************************
*
* templateAttach - publish the network interface and initialize the device
*
* This routine publishes the `template' interface by filling in a network
* interface record and adding this record to the system list.  This routine
* also initializes the driver and the device to the operational state.
*
* The <memBase> parameter is used to inform the driver about the shared
* memory region.  If this parameter is set to the constant "NONE," then this
* routine will attempt to allocate the shared memory from the system.  Any
* other value for this parameter is interpreted by this routine as the address
* of the shared memory region to be used.
*
* If the caller provides the shared memory region, then the driver assumes
* that this region does not require cache coherency operations, nor does it
* require conversions between virtual and physical addresses.
*
* If the caller indicates that this routine must allocate the shared memory
* region, then this routine will use cacheDmaMalloc() to obtain
* some  non-cacheable memory.  The attributes of this memory will be checked,
* and if the memory is not both read and write coherent, this routine will
* abort and return ERROR.
*
* RETURNS: OK or ERROR if arguments are invalid or a hardware fault is detected.
*
* SEE ALSO: ifLib
*/

STATUS templateAttach
    (
    int         unit,       /* unit number */
    UCHAR *	pCsr,	    /* ptr to CSR */
    int         iLevel,     /* interrupt level */
    char *      memBase,    /* address of memory pool or NONE */
    int         nTfds,      /* no. of transmit frames (0 = default) */
    int         nRfds       /* no. of receive frames (0 = default) */
    )
    {
    DRV_CTRL    *pDrvCtrl;
    UINT        size;		/* temporary size holder */
    int		temp;

    static char *errMsg1 = "\nattach: could not obtain memory\n";

    /* Sanity checks */

    if (unit < 0|| unit >= MAX_UNITS
     || pCsr == NULL)
        return (ERROR);

    /* Ensure single invocation per system life */

    pDrvCtrl = & drvCtrl [unit];

    if (pDrvCtrl->attached)
        return (OK);

    /* Publish the interface record */

    ether_attach    (
                    &pDrvCtrl->idr.ac_if,
                    unit,
                    "template",
                    (FUNCPTR) NULL,
                    (FUNCPTR) templateIoctl,
                    (FUNCPTR) templateOutput,
                    (FUNCPTR) templateReset
                    );

    size = 1024 * 16; /* TODO - calculate the total size of memory pool */

    /*
     * Establish the memory area that we will share with the device.  If
     * the caller has provided an area, then we assume it is non-cacheable
     * and will not require the use of the special cache routines.
     * If the caller did not provide an area, then we must obtain it from
     * the system, using the cache savvy allocation routine.
     */

    switch ((int) memBase)
        {
        case NONE :                            /* we must obtain it */

	    /*
	     * TODO - if driver needs coherent memory, test for it and
	     * report error if not coherent.
	     */

	    /* allocate memory */

            pDrvCtrl->memBase = cacheDmaMalloc (size);

            if (pDrvCtrl->memBase == NULL)    /* no memory available */
                {
                printf (errMsg1);
                goto error;
                }

            pDrvCtrl->cacheFuncs = cacheDmaFuncs;
            break;

        default :                               /* the user provided an area */
            pDrvCtrl->memBase = memBase;        /* use the provided address */
            pDrvCtrl->cacheFuncs = cacheNullFuncs;
            break;
        }

    /* TODO - Save device specific parameters */

    pDrvCtrl->intLevel = iLevel;
    pDrvCtrl->pCsr = pCsr;

    /*
     * TODO - If needed, carve up the shared memory region into
     * specific sections.
     */

    /* get our enet (MAC) addr */

    TEMPLATE_ENET_GET (pDrvCtrl, &temp);
    if (temp == ERROR)
	{
	errnoSet (S_iosLib_INVALID_ETHERNET_ADDRESS);
        goto error;
	}

    /* NOTE: The BSP will connect the ISR to the interrupt vector */

    /* reset chip, set flags to indicate device is active */

    if (templateInit (unit) == ERROR)
        goto error;

    pDrvCtrl->attached = TRUE;
    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);

    return (OK);

error:

    /* TODO - Release allocated objects/memory */

    return (ERROR);
    }


/*******************************************************************************
*
* templateInit - Initialize the interface.
*
* Initialization of interface; clear recorded pending operations.
* Called by templateAttach().
*
* RETURNS: OK or ERROR if a hardware failure is detected.
*/

LOCAL STATUS templateInit 
    (
    int unit			/* unit number */
    )
    {
    DRV_CTRL    *pDrvCtrl = & drvCtrl [unit];

    pDrvCtrl->rcvHandling   = FALSE;  /* netTask not running our receive job */
    pDrvCtrl->txCleaning    = FALSE;  /* netTask not running our clean job */
    pDrvCtrl->txIdle        = TRUE;         /* transmitter idle */

    /* Perform device initialization */

    TEMPLATE_INT_DISABLE (pDrvCtrl);

    TEMPLATE_INT_INIT (pDrvCtrl);	/* board specific initialization */

    /* TODO - configure chip, set up MAC address, etc */

    /* TODO - setup TX and RX buffer lists/queues */

    /* TODO - Setup free RFD list, and loan count */

    pDrvCtrl->nLoanRfds = MAX_RFDS_LOANED;

    /* Clear and enable interrupts */

    TEMPLATE_INT_ACK (pDrvCtrl);	/* to clear any pending interrupts */
    TEMPLATE_INT_ENABLE (pDrvCtrl);

    /* Start the device */

    if (templateDeviceStart (unit) == ERROR)
        return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* templateReset - reset the ei interface
*
* Mark interface as inactive and reset the chip.  Called from the interface
* layer as needed to shut down the device.
*
* RETURNS: N/A.
*/

LOCAL void templateReset
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];
    pDrvCtrl->idr.ac_if.if_flags = 0;

    TEMPLATE_INT_DISABLE (pDrvCtrl);

    /* TODO - reset the device */

    TEMPLATE_NETIF_WRITE(pDrvCtrl, pCsr, TEMPLATE_CMD_RESET);
    }

/*******************************************************************************
*
* templateIoctl - interface ioctl procedure
*
* Process an interface ioctl request.
*
* RETURNS: 0 (OK) always.
*/

LOCAL int templateIoctl
    (
    IDR     *pIDR,
    int     cmd,
    caddr_t data
    )
    {
    DRV_CTRL *	pDrvCtrl = & drvCtrl [pIDR->ac_if.if_unit];
    int		retVal = OK;
    int		s;

    s = splimp ();

    switch (cmd)
        {
        case SIOCSIFADDR:
            retVal = set_if_addr (&pIDR->ac_if, data, pDrvCtrl->idr.ac_enaddr);
            break;

        case SIOCSIFFLAGS:
            /* Flags are set outside this module. No additional work to do. */
            break;

        default:
            retVal = EINVAL;
        }

    splx (s);
    return (retVal);
    }

/*******************************************************************************
*
* templateOutput - interface output routine.
*
* This is the entry point for packets arriving from protocols above.  This
* routine merely calls our specific output routines that eventually lead
* to a transmit to the network.
*
* RETURNS: 0 or appropriate errno (see ether_output)
*
* SEE ALSO:
* ether_output
*/

LOCAL int templateOutput
    (
    IDR *    pIDR,
    MBUF *     pMbuf,
    SOCK * pDestAddr
    )
    {
    return (ether_output (&pIDR->ac_if, pMbuf, pDestAddr,
            (FUNCPTR) templateTxStart, pIDR));
    }

/*******************************************************************************
*
* templateTxStart - start output on the chip
*
* Looks for any action on the queue, and begins output if there is anything
* there.  This routine is called from several possible threads.  Each will be
* described below.
*
* The first, and most common thread, is when a user task requests the
* transmission of data.  This will cause templateOutput() to be called,
* which will cause ether_output() to be called, which will cause this routine
* to be called (usually).  This routine will not be called if ether_output()
* finds that our interface output queue is full.  In this case, the outgoing
* data will be thrown out.
*
* The second, and most obscure thread, is when the reception of certain
* packets causes an immediate (attempted) response.  For example, ICMP echo
* packets (ping), and ICMP "no listener on that port" notifications.  All
* functions in this driver that handle the reception side are executed in the
* context of netTask().  Always.  So, in the case being discussed, netTask() 
* will receive these certain packets, cause IP to be stimulated, and cause the
* generation of a response to be sent.  We then find ourselves following the
* thread explained in the second example, with the important distinction that
* the context is that of netTask().
*
* The third thread occurs when this routine runs out of TFDs and returns.  If
* this occurs when our output queue is not empty, this routine would typically
* not get called again until new output was requested.  Even worse, if the
* output queue was also full, this routine would never get called again and
* we would have a lock state.  It DOES happen.  To guard against this, the
* transmit clean-up handler detects the out-of-TFDs state and calls this
* function.  The clean-up handler also runs from netTask.
*
* Note that this function is ALWAYS called between an splnet() and an splx().
* This is true because netTask(), and ether_output() take care of
* this when calling this function.  Therefore, no calls to these spl functions
* are needed anywhere in this output thread.
*
* RETURNS: N/A.
*/

LOCAL void templateTxStart
    (
    int unit
    )
    {
    MBUF * pMbuf;
    int length;
    TFD * pTfd = NULL;
    DRV_CTRL * pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    /*
     * Loop until there are no more packets ready to send or we
     * have insufficient resources left to send another one.
     */

    while (pDrvCtrl->idr.ac_if.if_snd.ifq_head)
        {

	/* TODO - get a free Tx packet buffer */

        if (pTfd == NULL)
            break;                              /* out of TFD's */

        IF_DEQUEUE (&pDrvCtrl->idr.ac_if.if_snd, pMbuf);  /* dequeue a packet */

        copy_from_mbufs (pTfd->pEnetHdr, pMbuf, length);
        length = max (ETHERSMALL, length);
        length = min (0x3fff, length);

        if ((etherOutputHookRtn != NULL) &&
            (* etherOutputHookRtn)
            (&pDrvCtrl->idr, pTfd->pEnetHdr, length))
	    {
	    /* TODO - release the TFD */
            continue;
	    }

	/* TODO - update TFD data fields: length, etc */

        pTfd->count = length;

	templateTfdSend (pDrvCtrl, pTfd);	/* txmit the TFD */
        }
    }

/*******************************************************************************
*
* templateInt - entry point for handling interrupts
*
* The interrupting events are acknowledged to the device, so that the device
* will deassert its interrupt signal.  The amount of work done here is kept
* to a minimum; the bulk of the work is defered to the netTask.  Several flags
* are used here to synchronize with task level code and eliminate races.
*
* RETURNS: N/A.
*/

void templateInt
    (
    int unit
    )
    {
    UINT16  devStatus;
    DRV_CTRL *pDrvCtrl;

    pDrvCtrl = & drvCtrl [unit];

    /* TODO - read device status */

    TEMPLATE_NETIF_READ (pDrvCtrl, pCsr, devStatus);

    /* Quick exit if device did not generate interrupt */

    if (devStatus == 0)
	return;

    /* Acknowledge interrupt to reset it, either here or at end of routine. */

    TEMPLATE_INT_ACK (pDrvCtrl);

    /* Handle transmitter interrupt */

    if (devStatus & TEMPLATE_CSR_TX_PEND)
        {
	/* TODO - reclaim tx buffers */

        if (1) /* TODO - if tx queue is not empty */
            templateTfdStart (pDrvCtrl); 
        else
            pDrvCtrl->txIdle = TRUE;                    /* transmitter idle */
        }

    /* Handle receiver interrupt */

    if (devStatus & TEMPLATE_CSR_RX_PEND)
	{
	if (!(pDrvCtrl->rcvHandling))
            {
            pDrvCtrl->rcvHandling = TRUE;
            netJobAdd ((FUNCPTR) templateHandleRecv, (int) pDrvCtrl,0, 0, 0, 0);
	    }
        }
    }

/*******************************************************************************
*
* templateHandleRecv - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*
* RETURNS: N/A.
*/

LOCAL void templateHandleRecv
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    RFD *pRfd;

    do
        {
        pDrvCtrl->rcvHandling = TRUE;	/* interlock with templateInt() */

        while ((pRfd = templateRfdGet (pDrvCtrl)) != NULL)
	    {
            if (templateReceive (pDrvCtrl, pRfd) == OK)
                templateRfdReturn (pDrvCtrl, pRfd);
	    }

        pDrvCtrl->rcvHandling = FALSE;	/* interlock with templateInt() */

	templateRxStart (pDrvCtrl);	/* restart Rcvr */
        }
    while (templateRfdReady (pDrvCtrl));	/* make sure rx q still empty */
    }

/*******************************************************************************
*
* templateReceive - pass a received frame to the next layer up
*
* Strips the Ethernet header and passes the packet to the appropriate
* protocol.  The return value indicates if buffer loaning was used to hold
* the data.  A return value of OK means that loaning was not done, and it
* is therefore 'ok' to return the RFD to the Rx queue.  A return value of ERROR
* means that buffer loaning was employed, and so the RFD is still in use and
* should not be returned to the Rx queue.  In this latter case, the RFD will
* eventually be returned by the protocol, via a call to our templateLoanFree().
*
* RETURNS: OK if RFD was not loaned, ERROR if it was loaned.
*/

LOCAL STATUS templateReceive
    (
    DRV_CTRL *pDrvCtrl,
    RFD *pRfd
    )
    {
    ETH_HDR     *pEh;
    u_char      *pData;
    int         len;
    UINT16      etherType;
    MBUF        *m = NULL;
    BOOL        rfdLoaned = FALSE;

    if  (0)	/* TODO - check RFD for errors */
        {
        ++pDrvCtrl->idr.ac_if.if_ierrors;	/* bump error counter */
        templateRfdReturn (pDrvCtrl, pRfd);	/* free the RFD */
        return (ERROR);
        }

    /* Bump input packet counter. */

    ++pDrvCtrl->idr.ac_if.if_ipackets;

    len = pRfd->actualCnt;		/* get frame length */
    pEh = pRfd->pEnetHdr;	/* get ptr to ethernet header */

    /* Service input hook */

    if (etherInputHookRtn != NULL)
        {
        if ((* etherInputHookRtn) (&pDrvCtrl->idr, (char *)pEh, len))
            {
            templateRfdReturn (pDrvCtrl, pRfd); /* free the RFD */
            return (OK);
            }
        }

    len -= EH_SIZE;
    pData = (u_char *) pRfd->enetData;
    etherType = ntohs (pEh->ether_type);

    /*
     * we can loan out receive frames from receive queue if:
     *
     * 1) the threshold of loanable frames has not been exceeded
     * 2) size of the input ethernet frame is large enough to be used with
     *    clustering.
     */

    if ((pDrvCtrl->nLoanRfds > 0) && (USE_CLUSTER (len)) &&
        ((m = build_cluster (pData, len, &pDrvCtrl->idr, MC_EI, &pRfd->refCnt,
                 templateLoanFree, (int) pDrvCtrl, (int) pRfd, NULL)) != NULL))
        {
        pDrvCtrl->nLoanRfds--;		/* one less to loan */
        rfdLoaned = TRUE;               /* we loaned a frame */
        }
    else
	{
        m = copy_to_mbufs (pData, len, 0, &pDrvCtrl->idr);
	}

    if (m != NULL)
        do_protocol_with_type (etherType, m, &pDrvCtrl->idr, len);

    return ((rfdLoaned) ? ERROR : OK);
    }

/*******************************************************************************
*
* templateLoanFree - return a loaned receive frame descriptor
*
* This routine is called by the protocol code when it has completed use of
* an RFD that we loaned to it.
*
* RETURNS: N/A.
*/

LOCAL void templateLoanFree
    (
    DRV_CTRL *pDrvCtrl,
    RFD *pRfd
    )
    {
    templateRfdReturn (pDrvCtrl, pRfd);
    pDrvCtrl->nLoanRfds ++;
    }

/*******************************************************************************
*
* templateDeviceStart - reset and start the device
*
* This routine assumes interrupts from the device have been disabled, and
* that the driver control structure has been initialized.
*
* RETURNS: OK upon success, or ERROR for a hardware failure.
*/

LOCAL STATUS templateDeviceStart
    (
    int unit                              /* physical unit number */
    )
    {
    DRV_CTRL *pDrvCtrl;

    /* Get pointers */

    pDrvCtrl = & drvCtrl [unit];

    /* Issue the reset operation to the device */

    TEMPLATE_NETIF_WRITE(pDrvCtrl, pCsr, TEMPLATE_CMD_RESET);

    /* TODO - initialize the device */

    /* TODO - generate error message if needed */

    templateRxStart (pDrvCtrl);

    return (OK);
    }

/*******************************************************************************
*
* templateTfdSend - place a transmit frame on the transmit queue
*
* The TFD has been filled in with the network pertinent data.  This
* routine will enqueue the TFD for transmission.
*
* RETURNS: N/A.
*/

LOCAL void templateTfdSend
    (
    DRV_CTRL *pDrvCtrl,
    TFD *pTfd
    )
    {
    int unit;

    unit = pDrvCtrl->idr.ac_if.if_unit;

    /* TODO - setup the TFD data fields */
    pTfd->status    = 0;                    /* fill in TFD fields */
    pTfd->reserved  = 0;                    /* must be zero */

    TEMPLATE_INT_DISABLE (pDrvCtrl);

    /* TODO - queue the TFD */

    if (pDrvCtrl->txIdle)               /* transmitter idle */
        templateTfdStart (pDrvCtrl);	/* flush txQueue */

    TEMPLATE_INT_ENABLE (pDrvCtrl);
    }

/*******************************************************************************
*
* templateTfdStart - start/restart the xmit list
*
* This routine is the device level xmit routine.  The packet is given to the
* device for transmission.
*
* RETURNS: N/A.
*/

LOCAL void templateTfdStart
    (
    DRV_CTRL *pDrvCtrl		/* device pointer */
    )
    {
    /* TODO - setup tx queues */

    pDrvCtrl->txIdle = FALSE;                   /* transmitter busy */

    /* TODO - send start command */

    TEMPLATE_NETIF_WRITE (pDrvCtrl, pCsr, TEMPLATE_CMD_TX_START);
    }

/*******************************************************************************
*
* templateRxStart - start up the Receive Unit
*
* Starts up the Receive Unit.  Assumes that the receive structures are set up.
*
* RETURNS: N/A.
*/

LOCAL void templateRxStart
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    /* TODO - send start command */

    TEMPLATE_NETIF_WRITE(pDrvCtrl, pCsr, TEMPLATE_CMD_RX_START);
    }

/*******************************************************************************
*
* templateRfdReturn - return a RFD to the receive queue for use by the device
*
* This routine is used by the protocol layer to return a loaned receive buffer
* back to the driver.
*
* RETURNS: N/A.
*/

LOCAL void templateRfdReturn
    (
    DRV_CTRL* pDrvCtrl,		/* device pointer */
    RFD* pRfd			/* buffer being returned */
    )
    {
    int unit;

    unit = pDrvCtrl->idr.ac_if.if_unit;

    /* TODO - update RFD data */

    pRfd->status    = 0;                    /* clear status */
    pRfd->actualCnt  = 0;                   /* clear actual count */
    pRfd->bufSize   = ETHERMTU + EH_SIZE;   /* fill in size */
    pRfd->refCnt    = 0;                    /* initialize ref cnt */

    /* TODO - link RFD back into queue */
    }

/*******************************************************************************
*
* templateRfdGet - get a received frame from the receive queue
*
* This routine returns a received frame descriptor (incoming packet) to
* the caller.  If no RFD is available, the NULL pointer is returned.
*
* RETURNS:
* A pointer to a valid RFD, or NULL if none available
*/

LOCAL RFD *templateRfdGet
    (
    DRV_CTRL *pDrvCtrl		/* device pointer */
    )
    {
    RFD *pRfd = NULL;

    if (templateRfdReady (pDrvCtrl))
	{
        /* TODO - get RFD from device queue */
	}

    return (pRfd);
    }

/*******************************************************************************
*
* templateRfdReady - determine if a RFD is ready for processing
*
* This routine is used to determine if a received frame descriptor (incoming
* packet) is ready for processing.
*
* RETURNS:
* TRUE if complete received frame is available, FALSE otherwise.
*/

LOCAL BOOL templateRfdReady
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    UCHAR devStatus;

    TEMPLATE_NETIF_READ(pDrvCtrl, pCsr, devStatus);

    if (devStatus & TEMPLATE_CSR_RFD_READY)
	return TRUE;

    return FALSE;
    }
