/* if_seeq.c - Seeq 8005 Ethernet network interface driver */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,21apr96,mem  written, based on other drivers.
*/

/*
This module implements the Seeq 8005 Ethernet network interface
driver.

This driver was originally developed for the Motorola IDP ethernet
card.

At this time, the driver makes the following assumptions:
  - A 16-bit path to the SEEQ chip is available.

This driver is designed to be moderately generic, operating unmodified
across the range of architectures and targets supported by VxWorks.
To achieve this, the driver must be given several target-specific
parameters, and some external support routines must be provided.
These parameters, and the mechanisms used to communicate them to the
driver, are detailed below.  If any of the assumptions stated below
are not true for your particular hardware, this driver will probably
not function correctly with it.

BOARD LAYOUT
We have yet to see documentation on the jumpers, so don't change them.

EXTERNAL INTERFACE
This driver provides the standard external interface with the
following exceptions.  All initialization is performed within the
attach routine; there is no separate initialization routine.
Therefore, in the global interface structure, the function pointer to
the initialization routine is NULL.

The only user-callable routine is seeqattach(), which publishes the
`seeq' interface and initializes the driver and device.

TARGET-SPECIFIC PARAMETERS
.iP "base address of device registers"
This parameter is passed to the driver by seeqattach().  It indicates
to the driver where to find the device registers.

.iP "interrupt vector"
This parameter is passed to the driver by seeqattach().

.iP "interrupt level"
This parameter is passed to the driver by seeqattach().

.iP "Ethernet address"
This parameter is obtained directly from a global memory location.

During initialization, the driver needs to know the Ethernet address for
the SEEQ device.  The driver assumes that this address is available in
a global, six-byte character array, seeqEnetAddr[].  This array is
typically created and stuffed by the BSP code.
.LP

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one interrupt vector
    - one watchdog timer
    - 12 bytes in the initialized data section (data)
    - 4264 bytes in the uninitialized data section (BSS)
    - 4096 bytes of allocated buffer space.

The above data and BSS requirements are for the MCF5200 architecture
and may vary for other architectures.  Code size (text) varies greatly
between architectures and is therefore not quoted here.

SEE ALSO: ifLib, 
.I "SEEQ 8005 Advanced Ethernet Data Link Controller"
*/

/* includes */

#include "vxWorks.h"
#include "stdlib.h"
#include "taskLib.h"
#include "logLib.h"
#include "intLib.h"
#include "netLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysLib.h"
#include "wdLib.h"
#include "iv.h"
#include "memLib.h"
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
/*HELP#include "drv/netif/if_seeq.h"            /* device description header */
#include "if_seeq.h"            /* device description header */

/* defines */

#define DEBUG

#define MAX_UNITS		1	/* maximum units supported */
#define SEEQ_TX_OUTSTANDING	1	/* seeqTransmit outstanding */
#define SEEQ_RX_OUTSTANDING	2	/* seeqReceive outstanding */

/* Offset to LSB */
#if _BYTE_ORDER == _BIG_ENDIAN
#define SEEQ_LSB_OFFSET		1	/* offset to LSB */
#define SEEQ_MSB_SHIFT		8	/* shift to MSB */
#define SEEQ_CFG2_BSWAP 	SEEQ_CONF2_BYTE_SWAP /* config2 bswap bit */
#else /* _BYTE_ORDER == _BIG_ENDIAN */
#define SEEQ_LSB_OFFSET		0	/* offset to LSB */
#define SEEQ_MSB_SHIFT		0	/* shift to MSB */
#define SEEQ_CFG2_BSWAP 	0	/* config2 bswap bit */
#endif /* _BYTE_ORDER == _BIG_ENDIAN */

/* How much free space is in the TX buffer */
#define TX_SPACE(pDrv) \
	(((pDrv)->txNext < (pDrv)->txCurr) \
	 ? ((pDrv)->txCurr - (pDrv)->txNext) \
	 : ((((pDrv)->tea + 1) << 8) - ((pDrv)->txNext - (pDrv)->txCurr)))

/* typedefs */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;                  /* Interface Data Record wrapper */
typedef struct ifnet IFNET;                 /* real Interface Data Record */
typedef struct sockaddr SOCK;
typedef struct ex_msg EX_MSG;               /* an EXOS message */

/* The definition of the driver control structure */

typedef struct drv_ctrl		/* DRV_CTRL */
    {
    IDR		idr;		/* Interface Data Record */

    int		unit;		/* unit # */
    BOOL	attached;	/* indicates unit is attached */
    int		flags;		/* misc flags */
    int		inum;		/* interrupt number */
    int		ilevel;		/* interrupt level */
    SEEQ_DEVICE	*devAdrs;	/* device structure address */

    WDOG_ID	watchDog;	/* watchDog for waiting for chip */
    BOOL	expired;	/* flag watchDog has expired */

    USHORT	rea;		/* receive end area */
    UINT	rxNext;		/* local buffer pointer for next packet */
    UCHAR	*rxBuf;		/* receive buffer */

    int		txCount;	/* # packets waiting to transmit */
    USHORT	tea;		/* transmit end area */
    UINT	txEnd;		/* end of tx area */
    UINT	txCurr;		/* end of current xmit packet */
    UINT	txNext;		/* local buffer pointer for next packet */
    UCHAR	*txBuf;		/* transmit buffer */
    int		txLow;		/* count of TX low reports */
    } DRV_CTRL;

#define DRV_CTRL_SIZ    sizeof(DRV_CTRL)

/* globals */

IMPORT	UCHAR	seeqEnetAddr []; /* Ethernet address to load */

#ifdef DEBUG
BOOL	seeqDebug = 0;
#endif

int	seeqIntCnt = 0;
int	seeqTimeoutCnt = 0;

/* locals */

LOCAL	DRV_CTRL  drvCtrl [MAX_UNITS];

/* forward declarations */

IMPORT	void ether_attach ();

	void	seeqChipReset (int unit);
	void	seeqRxReset (int unit);
	void	seeqTxReset (int unit);
	void	seeqInt	(int unit);

LOCAL	void	seeqReset (int unit);
LOCAL	void	seeqReceive (int unit);
LOCAL	void	seeqTransmit (int unit);
LOCAL	void	seeqStart (int unit);

LOCAL	int	seeqOutput (IDR *ifp, MBUF *m0, SOCK *dst);
LOCAL	int	seeqIoctl (IDR *ifp, int cmd, caddr_t data);
LOCAL	void	seeqTimeout (DRV_CTRL *pDrvCtrl);
LOCAL	STATUS	seeqFlushFIFO (DRV_CTRL *pDrvCtrl);
LOCAL	STATUS	seeqSetDMARead (DRV_CTRL *pDrvCtrl, USHORT addr);
LOCAL	STATUS	seeqSetDMAWrite (DRV_CTRL *pDrvCtrl, USHORT addr);

#ifndef SEEQ_READ
#define SEEQ_READ(reg) \
	*(USHORT *)(reg)
#endif

#ifndef SEEQ_READ_BYTE
#define SEEQ_READ_BYTE(reg) \
	*((volatile UCHAR *)(reg))
#endif

#ifndef SEEQ_WRITE
#define SEEQ_WRITE(reg,data) \
	*(USHORT *)(reg) = (USHORT)data
#endif

#ifndef SEEQ_WRITE_BYTE
#define SEEQ_WRITE_BYTE(reg,data) \
	*(volatile UCHAR *)(reg) = (UCHAR)data
#endif

/******************************************************************************
*
* seeqattach - publish the `seeq' network interface and initialize
*		the driver and device
*
* This routine publishes the `seeq' interface by filling in a network
* interface record and adding this record to the system list.  This
* routine also initializes the driver and the device to the operational
* state.
*
* RETURNS: OK or ERROR.
*/

STATUS seeqattach
    (
    int	      unit,		/* unit number */
    char     *devAdrs,		/* SEEQ I/O address */
    int	      inum,		/* interrupt number */
    int	      ilevel		/* interrupt level */
    )
    {
    DRV_CTRL *pDrvCtrl;

    /* Sanity check the unit number */
    if (unit < 0 || unit >= MAX_UNITS)
	return (ERROR);

    /* Ensure single invocation per system life */
    pDrvCtrl = & drvCtrl [unit];
    if (pDrvCtrl->attached)
	return (OK);

    if ((pDrvCtrl->watchDog = wdCreate()) == NULL)
	return (ERROR);
    if (!(pDrvCtrl->txBuf = (UCHAR *) malloc (2048)))
	{
	wdDelete (pDrvCtrl->watchDog);
	return (ERROR);
	}
    if (!(pDrvCtrl->rxBuf = (UCHAR *) malloc (2048)))
	{
	wdDelete (pDrvCtrl->watchDog);
	free (pDrvCtrl->rxBuf);
	return (ERROR);
	}

    /* Publish the interface data record */
    ether_attach (&pDrvCtrl->idr.ac_if, unit, "seeq",
		  (FUNCPTR) NULL, (FUNCPTR) seeqIoctl,
		  (FUNCPTR) seeqOutput, (FUNCPTR) seeqReset);

    /* Save some values */
    pDrvCtrl->inum	 = inum;		 /* interrupt vector */
    pDrvCtrl->ilevel	 = ilevel;		 /* interrupt level */
    pDrvCtrl->devAdrs	 = (SEEQ_DEVICE *)devAdrs; /* SEEQ I/O address */

    /* Obtain our Ethernet address and save it */
    bcopy ((char *) seeqEnetAddr, (char *) pDrvCtrl->idr.ac_enaddr, 6);

    if (intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC(inum), seeqInt, unit) == ERROR)
	return (ERROR);

    /* Reset the device */
    seeqChipReset (pDrvCtrl->unit);

    /* The interface is now up. */
    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);

    /* Set our flag */
    pDrvCtrl->attached = TRUE;

    /*
     * Enable the receiver, enable TX & RX interrupts,
     * ack any pending interrupts.
     */

    SEEQ_WRITE	(pDrvCtrl->devAdrs->pCmd,
		SEEQ_CMD_RX_ON  | SEEQ_CMD_RX_EN
		| SEEQ_CMD_TX_OFF | SEEQ_CMD_TX_EN
		| SEEQ_CMD_ALL_ACK);

    return (OK);
    }

/******************************************************************************
*
* seeqTimeout - note that the watchdog timer has expired.
*
* NOMANUAL
*/
LOCAL void seeqTimeout
    (
    DRV_CTRL  *pDrvCtrl
    )
    {
#ifdef DEBUG
    if (seeqDebug)
	logMsg ("seeqTimeout(%x)\n", (int) pDrvCtrl, 0, 0, 0, 0, 0);
#endif
    ++seeqTimeoutCnt;
    pDrvCtrl->expired = TRUE;
    }

/******************************************************************************
*
* seeqFlushFIFO - Wait for the seeq DMA FIFO to flush
*
* NOMANUAL
*/
LOCAL STATUS seeqFlushFIFO
    (
    DRV_CTRL  *pDrvCtrl
    )
    {
    SEEQ_DEVICE *pDev = pDrvCtrl->devAdrs;

    pDrvCtrl->expired = FALSE;
    wdStart (pDrvCtrl->watchDog, 2 * sysClkRateGet(),
	     (FUNCPTR) seeqTimeout, (int) pDrvCtrl);

    /* flush the FIFO */

    while (!(SEEQ_READ(pDev->pStat) & SEEQ_STAT_FIFO_EMPTY)
       && !pDrvCtrl->expired)
	taskDelay(1);

    wdCancel (pDrvCtrl->watchDog);

    if (pDrvCtrl->expired)
	return (ERROR);

    return (OK);
    }

/******************************************************************************
*
* seeqSetDMARead - Setup the FIFO for reading
*
* NOMANUAL
*/
LOCAL STATUS seeqSetDMARead
    (
    DRV_CTRL  *pDrvCtrl,
    USHORT    addr
    )
    {
    SEEQ_DEVICE *pDev = pDrvCtrl->devAdrs;

    /* Point into local buffer memory */

    SEEQ_WRITE (pDev->pCfg1, ((SEEQ_READ(pDev->pCfg1) & 0xfff0)
				| SEEQ_CONF1_LBUFFER));
    
    /* Wait for FIFO ready if in write direction */

    if (!(SEEQ_READ(pDev->pCmd) & SEEQ_STAT_FIFO_DIR))
	if (seeqFlushFIFO (pDrvCtrl) != OK)
	    return (ERROR);

    /* Set DMA address */

    SEEQ_WRITE (pDev->pDma, addr);

    /* Set direction to read */

    SEEQ_WRITE (pDev->pCmd, ((SEEQ_READ(pDev->pStat) & SEEQ_STAT_CMD_MASK)
					| SEEQ_CMD_FIFO_READ) );

    pDrvCtrl->expired = FALSE;
    wdStart (pDrvCtrl->watchDog, 2 * sysClkRateGet(),
	     (FUNCPTR) seeqTimeout, (int) pDrvCtrl);

    /* Wait for ready */

    while (!(SEEQ_READ(pDev->pStat) & SEEQ_STAT_BUF_INT) && !pDrvCtrl->expired)
	taskDelay(1);

    wdCancel (pDrvCtrl->watchDog);

    /* ack buffer window interrupt */

    SEEQ_WRITE (pDev->pCmd, ((SEEQ_READ(pDev->pStat) & SEEQ_STAT_CMD_MASK)
				    | SEEQ_CMD_BUF_ACK));

    if (pDrvCtrl->expired)
	return (ERROR);

    return (OK);
    }

/******************************************************************************
*
* seeqSetDMAWrite - Setup the FIFO for writing.
*
* NOMANUAL
*/
LOCAL STATUS seeqSetDMAWrite
    (
    DRV_CTRL  *pDrvCtrl,
    USHORT    addr
    )
    {
    SEEQ_DEVICE *pDev = pDrvCtrl->devAdrs;

    /* Point into local buffer memory */

    SEEQ_WRITE (pDev->pCfg1,  ((SEEQ_READ(pDev->pCfg1) & 0xfff0)
				| SEEQ_CONF1_LBUFFER));
    
    
    /* If already in write mode, wait until buffer writing is complete. */

    if (SEEQ_READ(pDev->pCmd) & SEEQ_STAT_FIFO_DIR)
	{
	/* read direction */

	SEEQ_WRITE (pDev->pCmd, ((SEEQ_READ(pDev->pStat) & SEEQ_STAT_CMD_MASK)
					    | SEEQ_CMD_FIFO_WRITE) );
	}
    else
	{
	/* write direction: wait until buffer writing is complete. */

	if (seeqFlushFIFO (pDrvCtrl) != OK)
	    return (ERROR);
	}

    /* Set DMA address */

    SEEQ_WRITE (pDev->pDma, addr);

    return (OK);
    }


/******************************************************************************
*
* seeqChipReset - hardware reset of chip (stop it)
*
* NOMANUAL
*/

void seeqChipReset
    (
    int unit
    )
    {
    int i;
    int oldLevel;
    DRV_CTRL *pDrvCtrl = &drvCtrl[unit];
    SEEQ_DEVICE *pDev;
    volatile UCHAR *bwind;

    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
	return;

#ifdef DEBUG
    if (seeqDebug)
	logMsg ("seeqChipReset(%d)\n", unit, 0, 0, 0, 0, 0);
#endif

    pDrvCtrl->flags = 0;
    pDrvCtrl->txCount = 0;

    /* Reset the chip */

    pDev = pDrvCtrl->devAdrs;
    SEEQ_WRITE (pDev->pCfg1, 0);
    SEEQ_WRITE (pDev->pCfg2, SEEQ_CONF2_RESET);

    /* Delay at least 4us */

    taskDelay (1);

    /* Check status register for 0x5800 */

    if (SEEQ_READ(pDev->pStat) != 0x5800)
	{
	logMsg ("seeq: probe at 0x%x failed, got 0x%04x\n",
		(int) pDrvCtrl->devAdrs->pCmd, SEEQ_READ(pDev->pStat),
		0, 0, 0, 0);
	}

    /* lock interrupts */

    oldLevel = intLock ();

    /* Program the interrupt vector */

    SEEQ_WRITE (pDev->pCfg1, ((SEEQ_READ(pDev->pCfg1) & 0xfff0)
				| SEEQ_CONF1_IVEC));
    SEEQ_WRITE (pDev->pBwind, pDrvCtrl->inum);

    /* Program station addr 0 w/ our e-net addr */

    /* Turn Tx, Rx and DMA off, then ack any interrupts */

    SEEQ_WRITE (pDev->pCmd, (SEEQ_CMD_FIFO_WRITE | SEEQ_CMD_ALL_OFF));
    SEEQ_WRITE (pDev->pCmd, (SEEQ_CMD_FIFO_WRITE | SEEQ_CMD_ALL_OFF
			    | SEEQ_CMD_ALL_ACK));

    /* Select address 0 */

    SEEQ_WRITE (pDev->pCfg1, ((SEEQ_READ(pDev->pCfg1) & 0xfff0) | 0));

    /* Get byte pointer to buffer window */
    bwind = ((volatile UCHAR *) pDev->pBwind) + SEEQ_LSB_OFFSET;

    /* Program the ethernet address */

    for (i = 0; i < 6; ++i)
	SEEQ_WRITE_BYTE (bwind, ((UCHAR *)pDrvCtrl->idr.ac_enaddr)[i]);

    /*
     * Configure the local memory buffer to allow half of the
     * space for transmit and half for receive.
     */

    pDrvCtrl->tea = 0x7f;
    pDrvCtrl->rea = 0xffff;
    pDrvCtrl->rxNext = (pDrvCtrl->tea + 1) << 8;
    pDrvCtrl->txEnd = ((pDrvCtrl->tea + 1) << 8) - 1;
    pDrvCtrl->txNext = 0;
    pDrvCtrl->txCurr = 0;
    pDrvCtrl->txLow = 0;

    SEEQ_WRITE(pDev->pCfg1, (*pDev->pCfg1 & 0xfff0) | SEEQ_CONF1_TXEND);
    SEEQ_WRITE(pDev->pBwind, pDrvCtrl->tea);
    SEEQ_WRITE(pDev->pTxptr, pDrvCtrl->txNext);
    SEEQ_WRITE(pDev->pRxptr, pDrvCtrl->rxNext);
    SEEQ_WRITE(pDev->pRxend, pDrvCtrl->rea);

    /*
     * Config1 register setup:
     *
     * Specific Addr 0: Enabled
     * Specific Addr 1: Disabled
     * Specific Addr 2: Disabled
     * Specific Addr 3: Disabled
     * Specific Addr 4: Disabled
     * Specific Addr 5: Disabled
     * DMA Burst Length: 1 byte
     * DMA Burst Interval: continuous
     * MatchMode: Specific Addresses + Broadcast Addr
     */

    *pDev->pCfg1	= SEEQ_CONF1_ADDR0_EN
			| SEEQ_CONF1_RX_SPEC_BROAD
			| SEEQ_CONF1_DMA_BURST_INTERVAL_0;

    /*
     * Config2 register setup:
     *
     * select byte swapping, or not. Yes for BIG_ENDIAN
     * AutoUpdate REA:  Disabled.
     * Receive While Transmitting: Enabled.
     * CRC ErrorEnable:	CRC Error packets not accepted.
     * Dribble Error Enable: Dribble packets not accepted.
     * Pass Long/Short Enable: Neither long or short packets accepted.
     * Preamble Select:  Std 802.3 64 bit preamble selected.
     * RecCRC: CRC is stripped from incoming packets.
     * XmitNoCRC: Outgoing packets have CRC appended.
     * Loopback:  Loopback disabled.
     * CTRLO: CTRL0 pin is low.
     * RESET: Not asserted.
     */

    SEEQ_WRITE (pDev->pCfg2, SEEQ_CFG2_BSWAP);

    /*
     * Configure for RX and TX off and interrupts disabled.
     * Ack any pending interrupts.
     */

    SEEQ_WRITE (pDev->pCmd, (SEEQ_CMD_RX_OFF | SEEQ_CMD_TX_OFF
				    | SEEQ_CMD_ALL_ACK));

    intUnlock (oldLevel);
    }

/******************************************************************************
*
* seeqTxReset - Reset the TX
*
* NOMANUAL
*/

void seeqTxReset
    (
    int unit
    )
    {
    int oldLevel;
    DRV_CTRL *pDrvCtrl = &drvCtrl[unit];
    SEEQ_DEVICE *pDev;

    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
	return;

#ifdef DEBUG
    if (seeqDebug)
	logMsg ("seeqTxReset(%d)\n", unit, 0, 0, 0, 0, 0);
#endif

    pDev = pDrvCtrl->devAdrs;
    oldLevel = intLock ();

    /* Report an error */

    ++pDrvCtrl->idr.ac_if.if_oerrors;

    /* Recover - disable the TX */

    SEEQ_WRITE (pDev->pCmd, ((SEEQ_READ(pDev->pStat) & SEEQ_STAT_CMD_MASK)
				    | SEEQ_CMD_TX_OFF));
    pDrvCtrl->txNext = 0;
    pDrvCtrl->txCurr = 0;
    pDrvCtrl->txCount = 0;
    SEEQ_WRITE (pDev->pTxptr, 0);

    /* Ack the buffer and TX interrupts */

    SEEQ_WRITE (pDev->pCmd, ((SEEQ_READ(pDev->pStat) & SEEQ_STAT_CMD_MASK)
				    | SEEQ_CMD_TX_ACK | SEEQ_CMD_BUF_ACK));

    intUnlock (oldLevel);
    }

/******************************************************************************
*
* seeqRxReset - Reset the RX
*
* NOMANUAL
*/

void seeqRxReset
    (
    int unit
    )
    {
    int oldLevel;
    DRV_CTRL *pDrvCtrl = &drvCtrl[unit];
    SEEQ_DEVICE *pDev;

    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
	return;

#ifdef DEBUG
    if (seeqDebug)
	logMsg ("seeqRxReset(%d)\n", unit, 0, 0, 0, 0, 0);
#endif

    pDev = pDrvCtrl->devAdrs;
    oldLevel = intLock ();

    /* Report an error */

    ++pDrvCtrl->idr.ac_if.if_ierrors;

    /* Recover - disable the RX */

    SEEQ_WRITE (pDev->pCmd, ((SEEQ_READ(pDev->pStat) & SEEQ_STAT_CMD_MASK)
				    | SEEQ_CMD_RX_OFF));
    pDrvCtrl->rxNext = (pDrvCtrl->tea + 1) << 8;

    SEEQ_WRITE (pDev->pRxptr, pDrvCtrl->rxNext);

    /* Ack the buffer and RX interrupts */

    SEEQ_WRITE (pDev->pCmd, ((SEEQ_READ(pDev->pStat) & SEEQ_STAT_CMD_MASK)
				    | SEEQ_CMD_RX_ACK | SEEQ_CMD_BUF_ACK
				    | SEEQ_CMD_RX_ON ));

    intUnlock (oldLevel);
    }

/******************************************************************************
*
* seeqReset - reset the interface
*
* Mark interface as inactive & reset the chip
*
* NOMANUAL
*/

LOCAL void seeqReset
    (
    int unit
    )
    {
    int oldLevel;
    DRV_CTRL *pDrvCtrl = &drvCtrl[unit];

    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
	return;

    oldLevel = intLock ();

    /* Reset the chip */

    SEEQ_WRITE (pDrvCtrl->devAdrs->pCfg1, 0);
    SEEQ_WRITE (pDrvCtrl->devAdrs->pCfg2, SEEQ_CONF2_RESET);

    /* Mark the interface as down */

    pDrvCtrl->idr.ac_if.if_flags = 0;

    intUnlock (oldLevel);
    }

/******************************************************************************
*
* seeqReceive - handle receiving packets.
*
* NOMANUAL
*/
LOCAL void seeqReceive
    (
    int unit
    )
    {
    USHORT	nextFrame;
    USHORT	headerStatus;
    int		pktByteCnt = 0;
    DRV_CTRL	*pDrvCtrl = &drvCtrl[unit];
    SEEQ_DEVICE *pDev = pDrvCtrl->devAdrs;

    /* If interface was shutdown, abort service request */

    if (!(pDrvCtrl->idr.ac_if.if_flags & IFF_UP))
	{
	pDrvCtrl->flags &= ~SEEQ_RX_OUTSTANDING;
	return;
	}

    if (!(pDrvCtrl->flags & SEEQ_RX_OUTSTANDING))
	{
#ifdef DEBUG
	if (seeqDebug)
	    logMsg ("seeq: no RX request outstanding\n",
		    0, 0, 0, 0, 0, 0);
#endif
	return;
	}
    pDrvCtrl->flags &= ~SEEQ_RX_OUTSTANDING;

    do
	{
	int i;
	USHORT frameStatus;
	USHORT frameLength;
	USHORT tmp;

	if (seeqSetDMARead(pDrvCtrl, pDrvCtrl->rxNext) != OK)
	    {
	    /* re-queue for later processing. */
#ifdef DEBUG
	    if (seeqDebug)
		logMsg ("RX: Set DMA failed\n", 0, 0, 0, 0, 0, 0);
#endif
	    seeqRxReset (pDrvCtrl->unit);
	    pDrvCtrl->flags |= SEEQ_RX_OUTSTANDING;
	    netJobAdd ((FUNCPTR) seeqReceive, unit, 0, 0, 0, 0);
	    break;
	    }
	nextFrame = SEEQ_READ (pDev->pBwind);

	/* Get the header and frame status */

	tmp = SEEQ_READ (pDev->pBwind);
	headerStatus = tmp >> 8;
	frameStatus = tmp & 0x00ff;

	/*
	 * Stop under any of the following conditions:
	 *   The packet isn't done
	 *   The next packet is this packet (looped back).
	 */

	if (!(frameStatus & SEEQ_RX_PSTAT_DONE)
	    || (nextFrame == pDrvCtrl->rxNext))
	    break;

	if (nextFrame < pDrvCtrl->rxNext)
	    {
	    /* The receiver wrapped around */
	    frameLength = (pDrvCtrl->rea+1) - pDrvCtrl->rxNext - 4
		+ (nextFrame - (int)((pDrvCtrl->tea + 1)<<8));
	    }
	else
	    {
	    /* The frame is sequential, not wrapped */
	    frameLength = nextFrame - pDrvCtrl->rxNext - 4;
	    }

	/* account for space consumed in the buffer. */

	pktByteCnt += frameLength + 4;
	if (pktByteCnt > 0x8000)
	    {
#ifdef DEBUG
	    if (seeqDebug)
		logMsg ("RX packet overflows\n", 0, 0, 0, 0, 0, 0);
#endif
	    seeqRxReset (pDrvCtrl->unit);
	    break;
	    }
#ifdef DEBUG
	if (seeqDebug)
	    logMsg ("RX pkt: @ 0x%04x Frame 0x%02x Header 0x%02x len %4d nxt 0x%04x\n",
		    pDrvCtrl->rxNext, frameStatus, headerStatus,
		    frameLength, nextFrame, 0);
#endif
	if (frameStatus != SEEQ_RX_PSTAT_DONE)
	    {

	    /* one of the errors */

	    if (frameStatus & SEEQ_RX_PSTAT_OVERSIZE)
		++pDrvCtrl->idr.ac_if.if_ierrors;
	    if (frameStatus & SEEQ_RX_PSTAT_CRC)
		++pDrvCtrl->idr.ac_if.if_ierrors;
	    if (frameStatus & SEEQ_RX_PSTAT_DRIBBLE)
		++pDrvCtrl->idr.ac_if.if_ierrors;
	    if (frameStatus & SEEQ_RX_PSTAT_SHORT)
		++pDrvCtrl->idr.ac_if.if_ierrors;
	    }
	else
	    {
	    ETH_HDR	*pHdr = (ETH_HDR *) pDrvCtrl->rxBuf;
	    char	*bufp = (char *) pDrvCtrl->rxBuf;

	    /* Read in the entire packet */

	    for (i = 0; i < frameLength/2; ++i)
		{
		((USHORT *) bufp)[i] = SEEQ_READ (pDev->pBwind);
		}

	    /* Get the last odd byte */

	    if (frameLength & 1)
		{
		bufp[frameLength-1] = SEEQ_READ_BYTE (pDev->pBwind);
		}

	    /* call input hook if any */

	    if ((etherInputHookRtn == NULL)
		|| !(*etherInputHookRtn) (&pDrvCtrl->idr.ac_if,
					  bufp, frameLength))
		{
		MBUF *pMbuf;

		bufp += SIZEOF_ETHERHEADER;
		frameLength -= SIZEOF_ETHERHEADER;
		pMbuf = copy_to_mbufs (bufp, frameLength, 0,
				       (IFNET *) &pDrvCtrl->idr.ac_if);
		if (pMbuf != NULL)
		    {
		    do_protocol_with_type (ntohs (pHdr->ether_type), pMbuf,
					   &pDrvCtrl->idr, frameLength);
		    pDrvCtrl->idr.ac_if.if_ipackets++;
		    }
		}
	    }

	/* adjust circular buffer */

	pDrvCtrl->rxNext = nextFrame;
	SEEQ_WRITE (pDev->pRxend, (nextFrame >> 8));

	} while (headerStatus & SEEQ_RX_HSTAT_CHAIN);
    }

/******************************************************************************
*
* seeqTransmit - handle transmitted packets.
*
* NOMANUAL
*/
LOCAL void seeqTransmit
    (
    int unit
    )
    {
    int		pktCount;
    DRV_CTRL	*pDrvCtrl = &drvCtrl[unit];
    SEEQ_DEVICE *pDev = pDrvCtrl->devAdrs;
    BOOL resetTx = FALSE;

#ifdef DEBUG
    if (seeqDebug)
	logMsg ("TX Cnt %d Curr 0x%04x Next 0x%04x Space 0x%04x\n",
		pDrvCtrl->txCount, pDrvCtrl->txCurr,
		pDrvCtrl->txNext, TX_SPACE(pDrvCtrl),
		0, 0);
#endif
    /* If interface was shutdown, abort service request */

    if (!(pDrvCtrl->idr.ac_if.if_flags & IFF_UP))
	{
	pDrvCtrl->flags &= ~SEEQ_TX_OUTSTANDING;
	return;
	}

    if (!(pDrvCtrl->flags & SEEQ_TX_OUTSTANDING))
	{
#ifdef DEBUG
	if (seeqDebug)
	    logMsg ("seeq: no TX request outstanding\n",
		    0, 0, 0, 0, 0, 0);
#endif
	return;
	}
    pDrvCtrl->flags &= ~SEEQ_TX_OUTSTANDING;

    pktCount = pDrvCtrl->txCount;
    if (!pDrvCtrl->txCount)
	{
#ifdef DEBUG
	if (seeqDebug)
	    logMsg ("seeq: no xmit curr 0x%04x next 0x%04x space 0x%04x\n",
		    pDrvCtrl->txCurr, pDrvCtrl->txNext, TX_SPACE(pDrvCtrl),
		    0, 0, 0);
#endif
	return;
	}

    /* Now process the packets */

    while (1)
	{
	USHORT tmp;
	USHORT nextFrame;
	USHORT frameCommand;
	USHORT frameStatus;

	if (seeqSetDMARead(pDrvCtrl, pDrvCtrl->txCurr) != OK)
	    {
	    /* re-queue for later processing. */

#ifdef DEBUG
	    if (seeqDebug)
		logMsg ("TX: Set DMA failed\n", 0, 0, 0, 0, 0, 0);
#endif
	    seeqTxReset (pDrvCtrl->unit);
	    pDrvCtrl->flags |= SEEQ_TX_OUTSTANDING;
	    netJobAdd ((FUNCPTR) seeqTransmit, unit, 0, 0, 0, 0);
	    break;
	    }

	/* Move curr to the end of the last transmitted packet */

	nextFrame = SEEQ_READ (pDev->pBwind);

	/* get the frame status and command */

	tmp = SEEQ_READ (pDev->pBwind);
	frameCommand = tmp >> 8;
	frameStatus = tmp & 0x00ff;

#ifdef DEBUG
	if (seeqDebug)
	    logMsg ("TX @ 0x%04x command %02x status 0x%02x next 0x%04x\n",
		    pDrvCtrl->txCurr, frameCommand, frameStatus, nextFrame,
		    0, 0);
#endif
	/*
	 * At this point, we should be at a valid TX header since
	 * txCount > 0.
	 */

	if (nextFrame > pDrvCtrl->txEnd)
	    {
#ifdef DEBUG
	    if (seeqDebug)
		logMsg ("TX: bad next pointer\n", 0, 0, 0, 0, 0, 0);
#endif
	    resetTx = TRUE;
	    }
	if (!(frameCommand & SEEQ_TX_CMD_XMIT))
	    {
#ifdef DEBUG
	    if (seeqDebug)
		logMsg ("TX: not an xmit header\n", 0, 0, 0, 0, 0, 0);
#endif
	    resetTx = TRUE;
	    }
	if (!(frameCommand & SEEQ_TX_CMD_CHAIN))
	    {
#ifdef DEBUG
	    if (seeqDebug)
		logMsg ("TX: ran off end\n", 0, 0, 0, 0, 0, 0);
#endif
	    resetTx = TRUE;
	    }
	if (!(frameCommand & SEEQ_TX_CMD_DATA))
	    {
#ifdef DEBUG
	    if (seeqDebug)
		logMsg ("TX: no data in packet\n", 0, 0, 0, 0, 0, 0);
#endif
	    resetTx = TRUE;
	    }

	if (resetTx)
	    {
#ifdef DEBUG
	    if (seeqDebug)
		logMsg ("TX: reset cnt %d curr 0x%04x cmd %02x stat 0x%02x next 0x%04x\n",
			pDrvCtrl->txCount, pDrvCtrl->txCurr, frameCommand,
			frameStatus, nextFrame, 0);
#endif
	    seeqTxReset (pDrvCtrl->unit);
	    break;
	    }

	/* If not done, stop processing. */

	if (!(frameStatus & SEEQ_TX_PSTAT_DONE))
	    break;

	/* Check for error status. */
	if (frameStatus & SEEQ_TX_PSTAT_BABBLE)
	    ++pDrvCtrl->idr.ac_if.if_oerrors;
	if (frameStatus & SEEQ_TX_PSTAT_COLL16)
	    pDrvCtrl->idr.ac_if.if_collisions += 16;
	else if (frameStatus & SEEQ_TX_PSTAT_COLL)
	    ++pDrvCtrl->idr.ac_if.if_collisions;

	/* Move past this packet. */
	pDrvCtrl->txCurr = nextFrame;

	/* Exit if all packets accounted for. */
	if (!--pDrvCtrl->txCount)
	    break;
	}

    /* If packets where waiting on the queue for free space, re-start */

    if (((pktCount != pDrvCtrl->txCount) || resetTx)
	&& (pDrvCtrl->idr.ac_if.if_snd.ifq_head != NULL))
	(void)netJobAdd ((FUNCPTR) seeqStart, unit, 0, 0, 0, 0);
    }

/******************************************************************************
*
* seeqInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.  All of the real work is handled at task level through
* netJobAdd.
*
* NOMANUAL
*/

void seeqInt
    (
    int unit
    )
    {
    USHORT	status;
    DRV_CTRL	*pDrvCtrl = &drvCtrl[unit];
    SEEQ_DEVICE	*pDev = pDrvCtrl->devAdrs;

    ++seeqIntCnt;
    status = SEEQ_READ (pDev->pStat);
    SEEQ_WRITE (pDev->pCmd, ((SEEQ_READ(pDev->pStat) & SEEQ_STAT_CMD_MASK) | SEEQ_CMD_ALL_ACK));

    if (pDrvCtrl->idr.ac_if.if_flags & IFF_UP)
	{

	/* Make sure only one TX and RX request is outstanding at a time. */

	if ((status & SEEQ_STAT_RX_INT)
	    && !(pDrvCtrl->flags & SEEQ_RX_OUTSTANDING))
	    {
	    pDrvCtrl->flags |= SEEQ_RX_OUTSTANDING;
	    netJobAdd ((FUNCPTR) seeqReceive, unit, 0, 0, 0, 0);
	    }

	if ((status & SEEQ_STAT_TX_INT)
	    && !(pDrvCtrl->flags & SEEQ_TX_OUTSTANDING))
	    {
	    pDrvCtrl->flags |= SEEQ_TX_OUTSTANDING;
	    netJobAdd ((FUNCPTR) seeqTransmit, unit, 0, 0, 0, 0);
	    }
	}
    }

/******************************************************************************
*
* seeqOutput - the driver output routine
*
* NOMANUAL
*/

LOCAL int seeqOutput
    (
    IDR  *pIDR,
    MBUF *pMbuf,
    SOCK *pDest
    )
    {
    return ether_output ((IFNET*)pIDR, pMbuf, pDest,
			 (FUNCPTR) seeqStart, pIDR);
    }

/******************************************************************************
*
* seeqStart - start outputing a packet.
*
* Get another datagram to send off of the interface queue,
* and map it to the interface before starting the output.
* This routine is called by seeqOutput().
*
* NOMANUAL
*/

LOCAL void seeqStart
    (
    int unit
    )
    {
    DRV_CTRL	*pDrvCtrl = &drvCtrl[unit];
    SEEQ_DEVICE *pDev = pDrvCtrl->devAdrs;
    int		space;
    int		s;

    s = splnet ();

    space = TX_SPACE(pDrvCtrl);
    if (space < (ETHERMTU + 8))
	{
#ifdef DEBUG
	if (seeqDebug)
	    logMsg ("TX: low space %4d: flags %d curr 0x%04x next 0x%04x\n",
		    space, pDrvCtrl->flags, pDrvCtrl->txCurr, pDrvCtrl->txNext,
		    0, 0);
#endif

	/* May have dropped a TX interrupt */

	if (pDrvCtrl->txLow++ > 2)
	    {
	    pDrvCtrl->txLow = 0;
	    seeqTxReset (pDrvCtrl->unit);
	    }
	else if (!(pDrvCtrl->flags & SEEQ_TX_OUTSTANDING))
	    {
	    pDrvCtrl->flags |= SEEQ_TX_OUTSTANDING;
	    netJobAdd ((FUNCPTR) seeqTransmit, unit, 0, 0, 0, 0);
	    }
	}

    /* Only start if we have at least enough room */

    if ((space >= ETHERMTU + 8)
	&& pDrvCtrl->idr.ac_if.if_snd.ifq_head != NULL)
	{
	MBUF	*pMbuf;
	int	len;
	UCHAR	*buf;
	USHORT	txNext;
	BOOL error = FALSE;

	pDrvCtrl->txLow = 0;

	IF_DEQUEUE(&pDrvCtrl->idr.ac_if.if_snd, pMbuf);

	/* copy packet to write buffer */

	copy_from_mbufs (pDrvCtrl->txBuf, pMbuf, len);
	buf = (UCHAR *) pDrvCtrl->txBuf;

	/* After this point, if there's a problem with the chip, the
	   packet is lost. */

	/* Adjust length to ensure minimum packet size */

	len = max (ETHERSMALL, len);

	++pDrvCtrl->txCount;
#ifdef DEBUG
	if (seeqDebug > 1)
	    {
	    int i;
	    char *ptr;
	    static char lbuf[4096];

	    sprintf(lbuf, "TX %4d:", len);
	    ptr = lbuf + strlen(lbuf);
	    for (i = 0; i < len; ++i, ptr += 3)
		sprintf(ptr , " %02x", buf[i]);
	    logMsg ("%s\n", (int) lbuf, 0, 0, 0, 0, 0);
	    }
#endif
	/* Compute where the next packet will go */

	txNext = pDrvCtrl->txNext + len + 4;
	if (txNext > pDrvCtrl->txEnd)
	    txNext -= (pDrvCtrl->txEnd + 1);
#ifdef DEBUG
	if (seeqDebug)
	    logMsg ("TX pkt: @ %04x len %4d space %5d next %04x\n",
		    pDrvCtrl->txNext, len, TX_SPACE(pDrvCtrl), txNext,
		    0, 0);
#endif
	/* place a transmit request (while TX possibly running) */

	/* Setup for writing */

	if (seeqSetDMAWrite(pDrvCtrl, pDrvCtrl->txNext) != OK)
	    error = TRUE;

	if (error == FALSE)
	    {
	    /* Write a dummy header, so TX will stop if it catches up. */

	    SEEQ_WRITE (pDev->pBwind, 0);
	    SEEQ_WRITE (pDev->pBwind, 0);

	    /* Write out the packet, two bytes at a time */

	    for ( ; len >= 2; len -= 2, buf += 2)
		{
		SEEQ_WRITE (pDev->pBwind, *(USHORT *)buf);
		}

	    if (len)
		{
		SEEQ_WRITE_BYTE (pDev->pBwind, *buf);
		}

	    /* Write dummy end header to stop chain */

	    SEEQ_WRITE (pDev->pBwind, 0);
	    SEEQ_WRITE (pDev->pBwind, 0);

	    /* Flush FIFO and move back and write the real header */

	    if (seeqSetDMAWrite (pDrvCtrl, pDrvCtrl->txNext) != OK)
		error = TRUE;
	    }
	if (error == FALSE)
	    {
	    /* TX packet header: next pointer (2 bytes) */

	    SEEQ_WRITE (pDev->pBwind, txNext);

	    /* TX packet header: cmd and status bytes */

	    SEEQ_WRITE (pDev->pBwind,
			((SEEQ_TX_CMD_SUCC_EN
			 | SEEQ_TX_CMD_DATA
			 | SEEQ_TX_CMD_CHAIN
			 | SEEQ_TX_CMD_XMIT) << SEEQ_MSB_SHIFT));

	    /* Flush FIFO */

	    if (seeqFlushFIFO (pDrvCtrl) != OK)
		error = TRUE;
	    }
	if (error == FALSE)
	    {
	    /* If TX has stopped, reload xmit pointer. */

	    if (!(SEEQ_READ(pDev->pStat) & SEEQ_STAT_TX_ON))
		{
		SEEQ_WRITE (pDev->pTxptr, pDrvCtrl->txNext);
		}

	    /* Adjust pointer to next buffer */

	    pDrvCtrl->txNext = txNext;

	    /* Turn on Tx and ack the buffer interrupt */

	    SEEQ_WRITE (pDev->pCmd,  
			((SEEQ_READ(pDev->pStat) & SEEQ_STAT_CMD_MASK)
			 | SEEQ_CMD_TX_ON | SEEQ_CMD_BUF_ACK));
	    }
	if (error == TRUE)
	    seeqTxReset (pDrvCtrl->unit);
	}
    splx (s);
    }

/******************************************************************************
*
* seeqIoctl - the driver I/O control routine
*
* Process an ioctl request.
*
* NOMANUAL
*/

LOCAL int seeqIoctl
    (
    IDR  *ifp,
    int            cmd,
    caddr_t        data
    )
    {
    int error = OK;

    switch (cmd)
        {
      case (int)SIOCSIFADDR:
	ifp->ac_ipaddr = IA_SIN (data)->sin_addr;
	break;

      case (int)SIOCSIFFLAGS:
	/* No further work to be done */
	break;

      default:
	error = EINVAL;
        }
    return (error);
    }
