/* if_fei.c - Intel 82557 Ethernet network interface driver */

/* Copyright 1998 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01q,28apr99,dat  Merge from 2_0_0
01s,09mar99,cn   added support for shared PCI interrupts (SPR# 24379).
01r,14dec98,n_s  fixed feiInit to properly set promiscuous mode. spr 24000.
01q,16oct98,cn   changed default to be BSD44_DRIVER, as 01o did not do it.
		 Suppressed warning in feiIoctl() (SPR# 21880).
01p,30sep98,cn   moved call to I82557_INT_DISABLE after sys557Init() 
		 (SPR# 21879). Also suppressed some warnings (SPR# 21880).
01o,16apr98,dat  changed default to be BSD44_DRIVER
01n,14apr98,sbs  added global variable feiIntConnect.
                 changed SYS_PCI_INT_CONNECT to SYS_INT_CONNECT.
                 added documentation on how to use feiIntConnect.
01m,18mar98,dat  changed default to be BSD43_DRIVER. fixed compatibility
01l,12mar98,sbs  changed intConnect() to SYS_PCI_INT_CONNECT.
01k,15jul97,spm  removed driver initialization from ioctl support (SPR #8831)
01j,25apr97,gnn  added multicast by turning on promiscous mode.
01i,03mar97,myz  fixed spr 7663
01h,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01h,10dec96,jdi  doc: correct statement about # of external support functions.
01g,20nov96,myz  doc: removed the document for the sys557Intxxx functions
01f,20nov96,myz  correct the DOC problem in feiDumpPrint and feiErrCounterDump
01e,20nov96,jdi  doc: mangen fixes and general cleanup.
01d,20nov96,hdn  removed CSR_ADDR_GET() and ENET_ADDR_GET().
01c,25oct96,myz  added system memory interface and MDI control 
01b,10oct96,dzb  clean-up and bug fixes.
01a,31aug96,dzb  written, based on v03g of src/drv/netif/if_ei.c.
*/

/*
This module implements the Intel 82557 Ethernet network interface driver.

This driver is designed to be moderately generic, operating unmodified
across the entire range of architectures and targets supported by VxWorks.
This driver must be given several target-specific parameters, and some
external support routines must be provided.  These parameters, and the
mechanisms used to communicate them to the driver, are detailed below.

This driver supports up to four individual units.

EXTERNAL INTERFACE
The user-callable routine is feiattach(), which publishes the `fei'
interface and performs some initialization.

After calling feiattach() to publish the interface, an initialization
routine must be called to bring the device up to an operational state.
The initialization routine is not a user-callable routine; upper layers
call it when the interface flag is set to `UP', or when the interface's
IP address is set.

There is a global variable 'feiIntConnect' which specifies the interrupt
connect routine to be used depending on the BSP. This is by default set 
to intConnect() and the user can override this to use any other interrupt 
connect routine ( say pciIntConnect() ) in sysHwInit() or any device 
specific initialization routine called in sysHwInit().

TARGET-SPECIFIC PARAMETERS
.iP "shared memory address"
This parameter is passed to the driver via feiattach().

The Intel 82557 device is a DMA-type device and typically shares
access to some region of memory with the CPU.  This driver is designed
for systems that directly share memory between the CPU and the 82557.

This parameter can be used to specify an explicit memory region for use
by the 82557.  This should be done on targets that restrict the 82557
to a particular memory region.  The constant `NONE' can be used to indicate
that there are no memory limitations, in which case the driver
attempts to allocate the shared memory from the system space.
.iP "number of Command, Receive, and Loanable-Receive Frame Descriptors"
These parameters are passed to the driver via feiattach().

The Intel 82557 accesses frame descriptors (and their associated buffers)
in memory for each frame transmitted or received.  The number of frame
descriptors can be configured at run-time using these parameters.
.iP "Ethernet address"
This parameter is obtained by a call to an external support routine.
.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires the following external support function:
.CS
    STATUS sys557Init (int unit, BOARD_INFO *pBoard)
.CE
This routine performs any target-specific initialization
required before the 82557 device is initialized by the driver.
The driver calls this routine every time it wants to [re]initialize
the device.  This routine returns OK, or ERROR if it fails.

SYSTEM RESOURCE USAGE
The driver uses cacheDmaMalloc() to allocate memory to share with the 82557.
The size of this area is affected by the configuration parameters specified
in the feiattach() call.  The size of one RFD (Receive Frame Descriptor) is
is the same as one CFD (Command Frame Descriptor): 1536 bytes.  For more
information about RFDs and CFDs, see the
.I "Intel 82557 User's Manual."

Either the shared memory region must be non-cacheable, or else
the hardware must implement bus snooping.  The driver cannot maintain
cache coherency for the device because fields within the command
structures are asynchronously modified by both the driver and the device,
and these fields may share the same cache line.

Additionally, this version of the driver does not handle virtual-to-physical
or physical-to-virtual memory mapping.

TUNING HINTS
The only adjustable parameters are the number of Frame Descriptors that will be
created at run-time.  These parameters are given to the driver when feiattach()
is called.  There is one CFD and one RFD associated with each transmitted
frame and each received frame, respectively.  For memory-limited applications,
decreasing the number of CFDs and RFDs may be desirable.  Increasing the number
of CFDs will provide no performance benefit after a certain point.
Increasing the number of RFDs will provide more buffering before packets are
dropped.  This can be useful if there are tasks running at a higher priority
than the net task.

SEE ALSO: ifLib,
.I "Intel 82557 User's Manual"
*/


#include "vxWorks.h"
#include "wdLib.h"
#include "sockLib.h"
#include "memLib.h"
#include "intLib.h"
#include "iosLib.h"
#include "errnoLib.h"
#include "cacheLib.h"
#include "netLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ioctl.h"
#include "net/if.h"
#include "netinet/in.h"
#include "netinet/in_var.h"
#include "net/unixLib.h"
#include "net/if_subr.h"
#include "iv.h"
#include "sysLib.h"
#include "etherLib.h"
#include "vxLib.h"
#include "logLib.h"
#include "taskLib.h"
#include "drv/netif/netifDev.h"
#include "drv/netif/if_fei.h"
#include "drv/pci/pciIntLib.h"

/* defines */

/* Change default to BSD44_DRIVER */
 
#ifndef BSD43_DRIVER
#ifndef BSD44_DRIVER
#   define BSD44_DRIVER
#endif /* BSD44_DRIVER */
#endif /* BSD43_DRIVER */
#undef	FEI_DEBUG			/* turns on debug output */

#ifdef  FEI_DEBUG
#undef  LOCAL
#define LOCAL
LOCAL BOOL feiDebug = 1;
#endif  /* FEI_DEBUG */

#define FEI_ACTION_TMO	1		/* action command timeout in seconds */
#define FEI_SCB_TMO	1		/* SCB command timeout in seconds */
#define FEI_INIT_TMO	1		/* 557 init timeout in seconds */
#define FEI_TX_RESTART_TMO      1       /* tx restart watchdog timeout */
#define TCB_TX_THRESH	0x10		/* Tx threshold value */

#define BOARD_INT_ENABLE(unit) if ((int)pDrvCtrl->board.intEnable) \
	(*pDrvCtrl->board.intEnable)(unit)
#define BOARD_INT_DISABLE(unit) if ((int)pDrvCtrl->board.intDisable) \
	(*pDrvCtrl->board.intDisable)(unit)
#define BOARD_INT_ACK(unit) if ((int)pDrvCtrl->board.intAck) \
	(*pDrvCtrl->board.intAck)(unit)
#define LOCAL_TO_SYS_ADDR(unit,localAddr) ((int)pDrvCtrl->board.sysLocalToBus? \
	(*pDrvCtrl->board.sysLocalToBus)(unit,localAddr):localAddr)
#define SYS_TO_LOCAL_ADDR(unit,sysAddr) ((int)pDrvCtrl->board.sysBusToLocal? \
        (*pDrvCtrl->board.sysBusToLocal)(unit,sysAddr):sysAddr)
#define SYS_INT_CONNECT(vector, routine, parameter) (feiIntConnect \
        ((VOIDFUNCPTR *) vector, routine, (int) parameter)) 

#define I82557_INT_ENABLE pDrvCtrl->pCSR->scbCommand = 0
#define I82557_INT_DISABLE  pDrvCtrl->pCSR->scbCommand = SCB_C_M

/* typedefs */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;
typedef struct ifnet IFNET;
typedef struct sockaddr SOCK;

typedef struct drv_ctrl			/* DRV_CTRL */
    {
    IDR			idr;		/* interface data record */
    BOOL		attached;	/* interface has been attached */
    CSR *		pCSR;		/* Control/Status Register ptr */
    CFD *		pCFD;		/* current Command Frame Descriptor */
    RFD *		pRFD;		/* current Receive Frame Descriptor */
    RFD *		pRFDL;		/* first loanable RFD */
 
    WDOG_ID             txRestart;      /* tx restart watchdog */
    BOOL		rxHandle;	/* rx handler scheduled */
    BOOL                txStall;        /* tx handler stalled - no CFDs */

    /* Board specific routines to manupalate this device */
    BOARD_INFO board;

    } DRV_CTRL;

/* globals */

FUNCPTR feiIntConnect = (FUNCPTR) intConnect; 
IMPORT STATUS	sys557Init (int unit, BOARD_INFO *pBoard);

/* locals */

LOCAL DRV_CTRL	drvCtrl [FEI_MAX_UNITS];
LOCAL int	feiClkRate = 0;

/* forward declarations */

LOCAL STATUS	feiInit (int unit);
LOCAL void	feiReset (int unit);
LOCAL int	feiIoctl (IFNET *pIfNet, int cmd, caddr_t data);
#ifdef BSD43_DRIVER
LOCAL int	feiOutput (IDR *pIDR, MBUF *pMbuf, SOCK *pDestAddr);
LOCAL void	feiTxStartup (int unit);
#else
LOCAL void	feiTxStartup (DRV_CTRL *pDrvCtrl);
#endif
LOCAL void      feiTxRestart (int unit);
LOCAL void	feiInt (DRV_CTRL *pDrvCtrl);
LOCAL void	feiHandleRecvInt (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	feiReceive (DRV_CTRL *pDrvCtrl, RFD *pRfd);
LOCAL void	feiLoanFree (DRV_CTRL *pDrvCtrl, RFD *pRfd);
LOCAL STATUS	feiNOP (int unit);
LOCAL STATUS	feiDiag (int unit);
LOCAL STATUS	feiConfig (int unit);
LOCAL STATUS	feiIASetup (int unit);
LOCAL UINT16	feiAction (int unit, UINT16 action);
LOCAL STATUS	feiSCBCommand (DRV_CTRL *pDrvCtrl, UINT8 cmd, BOOL addrValid,
		    UINT32 *addr);

#ifdef  FEI_DEBUG
LOCAL STATUS feiErrCounterDump ( int unit, UINT32 *memAddr);
LOCAL STATUS feiDumpPrint (int unit);
#endif /* FEI_DEBUG */

static int feiMDIWrite ( int unit, int regAddr, int phyAddr, UINT16 writeData);
static int feiMDIRead ( int unit, int regAddr, int phyAddr, UINT16 *retVal);
static int feiPhyInit (int unit);
static int feiMDIPhyConfig ( int unit, int phyAddr);
static int feiMDIPhyLinkSet ( int unit, int phyAddr);


/*******************************************************************************
*
* feiattach - publish the `fei' network interface
*
* This routine publishes the `fei' interface by filling in a network interface
* record and adding the record to the system list.
*
* The 82557 shares a region of main memory with the CPU.  The caller of this
* routine can specify the address of this shared memory region through the
* <memBase> parameter; if <memBase> is set to the constant `NONE', the
* driver will allocate the shared memory region.
*
* If the caller provides the shared memory region, the driver assumes
* that this region does not require cache coherency operations.
*
* If the caller indicates that feiattach() must allocate the shared memory
* region, feiattach() will use cacheDmaMalloc() to obtain a block
* of non-cacheable memory.  The attributes of this memory will be checked,
* and if the memory is not both read and write coherent, feiattach() will
* abort and return ERROR.
*
* A default number of 32 command (transmit) and 32 receive frames can be
* selected by passing zero in the parameters <nCFD> and <nRFD>, respectively.
* If <nCFD> or <nRFD> is used to select the number of frames, the values
* should be greater than two.
*
* A default number of 8 loanable receive frames can be selected by passing
* zero in the parameters <nRFDLoan>, else set <nRFDLoan> to the desired
* number of loanable receive frames.  If <nRFDLoan> is set to -1, no
* loanable receive frames will be allocated/used.
*
* RETURNS: OK, or ERROR if the driver could not be published and initialized.
*
* SEE ALSO: ifLib,
* .I "Intel 82557 User's Manual"
*/

STATUS feiattach
    (
    int		unit,		/* unit number */
    char *	memBase,	/* address of shared memory (NONE = malloc) */
    int         nCFD,           /* command frames (0 = default) */
    int         nRFD,           /* receive frames (0 = default) */
    int         nRFDLoan        /* loanable rx frames (0 = default, -1 = 0) */
    )

    {
    DRV_CTRL *	pDrvCtrl;		/* driver control structure pointer */
    WDOG_ID     txWd = NULL;            /* tx restart watchdog */
    CFD *	pCFD;			/* pointer to CFDs */
    RFD *	pRFD;			/* pointer to RFDs */
    UINT32	size;			/* temporary size holder */
    UINT32	sizeCFD = ROUND_UP (sizeof (CFD), 4);
    UINT32	sizeRFD = ROUND_UP (sizeof (RFD), 4);
    int		ix;			/* counter */
    char 	bucket[2];
    BOOL        memAlloc = FALSE;

    /* sanity check the unit number */

    if (unit < 0 || unit >= FEI_MAX_UNITS)
        return (ERROR);

    pDrvCtrl = &drvCtrl [unit];
    if (pDrvCtrl->attached)		/* already attached ? */
        return (ERROR);

    /* initialize the default parameter for the Physical medium layer control*/
    /* user has his chance to override in the BSP, just be CAREFUL */
    pDrvCtrl->board.phyAddr  = 1;
    pDrvCtrl->board.phySpeed = PHY_AUTO_SPEED;
    pDrvCtrl->board.phyDpx   = PHY_AUTO_DPX;
    pDrvCtrl->board.others   = 0;
    pDrvCtrl->board.tcbTxThresh = TCB_TX_THRESH;
    
    /* callout to perform adapter init */

    if (sys557Init (unit, &pDrvCtrl->board) == ERROR)
        return (ERROR);

    /* get CSR address from PCI config space base register */

    if ((int)(pDrvCtrl->pCSR = (CSR *)pDrvCtrl->board.baseAddr) == NULL)
        return (ERROR);

    /* probe for memory-mapped CSR */

    if (vxMemProbe ((char *) &pDrvCtrl->pCSR->scbStatus, VX_READ, 2,
	&bucket[0]) != OK)
        {
        printf ("feiattach(): need MMU mapping for address 0x%x\n", (UINT32)
	    pDrvCtrl->pCSR);
	return (ERROR);
	}

    I82557_INT_DISABLE; 

    /* initialize the Physical medium layer */
    if (feiPhyInit(unit) != OK)
	{
	printf("LINK FAILS, Check line connection\n");
	}

    if (feiClkRate == 0)
	feiClkRate = sysClkRateGet ();

    /* determine number of Command and Receive Frame descriptors to use */

    nCFD     = nCFD ? nCFD : FEI_CFD_DEF;
    nRFD     = nRFD ? nRFD : FEI_RFD_DEF;
    nRFDLoan = nRFDLoan ? nRFDLoan : FEI_RFD_LOAN;
    if (nRFDLoan == -1)
        nRFDLoan = 0;

    /* publish the interface record */

#ifdef BSD43_DRIVER
    ether_attach ((IFNET *) & pDrvCtrl->idr, unit, "fei", (FUNCPTR) feiInit,
        (FUNCPTR) feiIoctl, (FUNCPTR) feiOutput, (FUNCPTR) feiReset);
#else
    ether_attach (
                 (IFNET *) & pDrvCtrl->idr, 
                 unit, 
                 "fei", 
                 (FUNCPTR) feiInit,
                 (FUNCPTR) feiIoctl, 
                 (FUNCPTR) ether_output,
                 (FUNCPTR) feiReset
                 );

    pDrvCtrl->idr.ac_if.if_start = (FUNCPTR)feiTxStartup;
#endif

    /* calculate the total size of 82557 memory pool */

    size = (sizeRFD * (nRFD + nRFDLoan)) + (sizeCFD * nCFD);

    /*
     * Establish the memory area that we will share with the device.  If
     * the caller has provided an area, then we assume it is non-cacheable.
     * If the caller did not provide an area, then we must obtain it from
     * the system, using the cache savvy allocation routine.
     */

    if (memBase == (char *) NONE)	/* allocate on our own */
	{
        /* this driver doesn't handle incoherent caches */

        if (!CACHE_DMA_IS_WRITE_COHERENT () || !CACHE_DMA_IS_READ_COHERENT ())
            {
            printf ("feiattach: shared memory not cache coherent\n");
            goto error;
            }

        if ((memBase = cacheDmaMalloc (size)) == NULL)
            {
            printf ("feiattach: could not obtain memory\n");
            goto error;
            }

        memAlloc = TRUE;
        }

    bzero (memBase, size);              /* zero out entire region */

    /* carve up the shared-memory region */

    /*
     * N.B.
     * We are setting up the CFD list as a ring of TxCBs, tied off with a
     * CFD_C_SUSP as frames are copied into the data buffers.  The
     * susp/resume model is used because the links to the next CFDs do
     * not change -- it is a fixed ring.  Also note that if a CFD is needed
     * for anything else (e.g., DIAG, NOP, DUMP, CONFIG, or IASETUP comands),
     * then the commands will use the current CFD in the list.  After the
     * command is complete, it will be set back to a TxCB by feiAction().
     */

    /* First ready CFD pointer */
    pDrvCtrl->pCFD = pCFD = (CFD *) memBase;

    /* initialize the CFD ring */
    for (ix = 0; ix < nCFD; ix++, pCFD++)
	{
	pCFD->cfdStatus = CFD_S_COMPLETE | CFD_S_OK;  

        /* System bus address to link field */
	LINK_WR (&pCFD->link, 
            LOCAL_TO_SYS_ADDR(unit,((UINT32) pCFD + sizeCFD)) );

	/* Previous CFD pointer */
        pCFD->pPrev = (CFD *) ((UINT32) pCFD - sizeCFD);

	LINK_WR (&pCFD->cfdTcb.pTBD, 0xffffffff); /* no TBDs used */
	pCFD->cfdTcb.txThresh = pDrvCtrl->board.tcbTxThresh;
	pCFD->cfdTcb.tbdNum = 0;
	}
    pCFD--;
    LINK_WR (&pCFD->link, LOCAL_TO_SYS_ADDR(unit,(UINT32)pDrvCtrl->pCFD) )
        ; /* tie end of CFD ring, use system bus address */
    pDrvCtrl->pCFD->pPrev = pCFD;

    /*
     * N.B.
     * Use RFD_C_EL to tie the end of the RBD ring, and not RFD_C_SUSP
     * This is because we want the option of inserting a new RFD into
     * the ring on the fly (i.e., via an SCB_RUSTART command).  Why would
     * we do this?  Buffer loaning....  A suspend/resume reception model
     * will not allow us to do this, so we must use an idle/start model.
     */

    /* first ready RFD pointer */
    pDrvCtrl->pRFD = pRFD = (RFD *) ((UINT32) pCFD + sizeCFD);

    /*initialize the RFD ring,some of the fields will be done later in feiInit*/
    for (ix = 0; ix < nRFD; ix++, pRFD++)
	{
	/* system bus address to link field */
	LINK_WR (&pRFD->link,
            LOCAL_TO_SYS_ADDR(unit, ((UINT32) pRFD + sizeRFD)) );

        /* previous RFD pointer */
        pRFD->pPrev = (RFD *) ((UINT32) pRFD - sizeRFD);
	LINK_WR (&pRFD->pRBD, 0xffffffff);	/* no RBDs used */
	pRFD->bufSize = ETHERMTU + EH_SIZE;
	pRFD->refCnt = 0;
        pRFD->actualCnt = 0;
        }
    pRFD--;
    LINK_WR (&pRFD->link,LOCAL_TO_SYS_ADDR(unit,(UINT32)pDrvCtrl->pRFD) )
        ; /* tie end of RFD ring, use system bus address */
    pDrvCtrl->pRFD->pPrev = pRFD;

    /* initialize the RFD loanable buffers  */
    pDrvCtrl->pRFDL = NULL;
    pRFD++;
    for (ix = 0; ix < nRFDLoan; ix++, pRFD++)
	{
	pRFD->rfdStatus = 0;
	pRFD->rfdCommand = 0;
	LINK_WR (&pRFD->pRBD, 0xffffffff);	/* no RBDs used */
	pRFD->bufSize = ETHERMTU + EH_SIZE;
	pRFD->refCnt = 0;
	pRFD->actualCnt = 0;
        pRFD->pPrev = pDrvCtrl->pRFDL;          /* link into head of list */
        pDrvCtrl->pRFDL = pRFD;
        }

    if ((pDrvCtrl->txRestart = txWd = wdCreate ()) == NULL)
        goto error;

    feiReset (unit);			/* reset the chip */

    /* CU and RU should be idle following feiReset() */

    if (feiSCBCommand (pDrvCtrl, SCB_C_CULDBASE, TRUE, 0x0) == ERROR)
        goto error;

    if (feiSCBCommand (pDrvCtrl, SCB_C_RULDBASE, TRUE, 0x0) == ERROR)
        goto error;

     /* connect the interrupt handler */

    if (pDrvCtrl->board.vector)
        {
        if (SYS_INT_CONNECT(INUM_TO_IVEC (pDrvCtrl->board.vector), 
                                feiInt, pDrvCtrl) == ERROR)
            goto error;
        }

    /* get our ethernet hardware address */

    bcopy ((char *)pDrvCtrl->board.enetAddr, (char *) pDrvCtrl->idr.ac_enaddr, 
	   sizeof (pDrvCtrl->board.enetAddr));

    if (feiDiag (unit) == ERROR)	/* run diagnostics */
	goto error;

    if (feiIASetup (unit) == ERROR)	/* setup address */
	goto error;

    if (feiConfig (unit) == ERROR)	/* configure chip */
	goto error;

    pDrvCtrl->rxHandle = FALSE;
    pDrvCtrl->txStall = FALSE;
    pDrvCtrl->attached = TRUE;

    if (feiInit (unit) == ERROR)
        goto error;
 
    return (OK);

error:					/* error cleanup */
        {
        if (memAlloc)
            cacheDmaFree (memBase);

        if (txWd)
            wdDelete (txWd);

        return (ERROR);
        }
    }

/*******************************************************************************
*
* feiInit - initialize the 82557 device
*
* This routine initializes the 82557 device and brings it up to an operational
* state.  The driver must have already been attached with the feiattach()
* routine.
*
* This routine is called at boot time and by feiWatchDog() if a reset is
* required.
*
* RETURNS: OK, or ERROR if the device could not be initialized.
*/

LOCAL STATUS feiInit 
    (
    int		unit		/* unit number */
    )
    {
    DRV_CTRL *  pDrvCtrl = &drvCtrl [unit];
    RFD *       pRFD;
    UINT16      event;
    int         ix;

    if (!pDrvCtrl->attached)            /* must have been attached */
        return (ERROR);

    /* wait for the receive handler to catch up to the [reset] 557 */

    for (ix = (FEI_INIT_TMO * feiClkRate); --ix;)
        {
        if (!pDrvCtrl->rxHandle)
            break;

        taskDelay (1);
        }

    if (!ix)
        return (ERROR);

    /* initiailize RFD ring */

    pRFD = pDrvCtrl->pRFD;

    do {
        pRFD->rfdStatus = 0;
        pRFD->rfdCommand = 0;
        pRFD = (RFD *) SYS_TO_LOCAL_ADDR(unit,(UINT32)LINK_RD (&pRFD->link));
        } while (pRFD != pDrvCtrl->pRFD);

    pRFD->pPrev->rfdCommand = RFD_C_EL;

    if (feiNOP (unit) == ERROR)         /* put CU into suspended state */
        return (ERROR);

    event = pDrvCtrl->pCSR->scbStatus;  /* save int events */
    pDrvCtrl->pCSR->scbStatus = event;

    BOARD_INT_ACK(unit);   /* acknowledge interrupts */

    I82557_INT_ENABLE;
    BOARD_INT_ENABLE(unit);         /* enable interrupts at system */

    /* mark the interface as up */

#ifdef BSD43_DRIVER
    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);
#else
    pDrvCtrl->idr.ac_if.if_flags |= (IFF_MULTICAST| IFF_UP | IFF_RUNNING
				    | IFF_NOTRAILERS);
#endif

    /* startup the receiver */

    if (feiSCBCommand (pDrvCtrl, SCB_C_RUSTART, TRUE,
        (UINT32 *)LOCAL_TO_SYS_ADDR(unit, (UINT32) pRFD) ) == ERROR)
        return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* feiReset - reset the `fei' interface
*
* This routine marks the interface as inactive and resets the chip.  It brings
* down the interface to a non-operational state.  To bring the interface back
* up, feiInit() must be called.
*
* RETURNS: N/A
*/

LOCAL void feiReset
    (
    int		unit		/* unit number of interface */
    )
    {
    DRV_CTRL *	pDrvCtrl = &drvCtrl [unit];

    BOARD_INT_DISABLE(unit);     /* disable system interrupt */

    I82557_INT_DISABLE;          /* disable chip interrupt   */

    /* mark the interface as down */

    pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);

    /* issue a selective reset to the 82557 chip */

    PORT_WR (&pDrvCtrl->pCSR->port, FEI_PORT_SELRESET);

    taskDelay (2);			/* wait for the reset... */
    }

/*******************************************************************************
*
* feiIoctl - interface control request routine
*
* This routine processes the interface's ioctl() request.
*
* RETURNS: OK, or EINVAL if <cmd> is not recognized.
*/

LOCAL int feiIoctl
    (
    IFNET *	pIfNet,		/* interface to act upon */
    int		cmd,		/* command to process */
    caddr_t	data		/* command-specific data */
    )
    {
    DRV_CTRL *  pDrvCtrl = & drvCtrl [pIfNet->if_unit];
    int		retVal = OK;
    int		s = splimp ();		/* not taken in some calling funcs */

    switch (cmd)
        {
        case SIOCSIFADDR:		/* set interface address */
#ifdef BSD43_DRIVER
            retVal = set_if_addr (pIfNet, data, pDrvCtrl->idr.ac_enaddr);
#else
            ((struct arpcom *)pIfNet)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas ((struct arpcom *) pIfNet, &IA_SIN (data)->sin_addr);
#endif
            break;

        case SIOCSIFFLAGS:		/* set interface flags */
            break;

#ifdef BSD44_DRIVER
        case SIOCADDMULTI:
        case SIOCDELMULTI:
            pDrvCtrl->idr.ac_if.if_flags |= IFF_PROMISC;
            retVal = OK;
            break;
#endif
            
        default:
            retVal = EINVAL;
        }

    splx (s);
    return (retVal);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* feiOutput - interface output routine
*
* This routine simply calls ether_output().  ether_output() resolves
* the hardware addresses and calls feiTxStartup() with the unit number
* passed as an argument.
*
* RETURNS: OK if successful, otherwise `errno'.
*/

LOCAL int feiOutput
    (
    IDR *	pIDR,
    MBUF *	pMbuf,
    SOCK *	pDestAddr
    )
    {
    return (ether_output ((IFNET *) pIDR, pMbuf, pDestAddr,
	(FUNCPTR) feiTxStartup, pIDR));
    }
#endif

/*******************************************************************************
*
* feiTxStartup - transmit frames on the device
*
* Look for packets on the IP output queue; output frames if there is anything
* there.
*
* This routine is called from several possible threads.  Each will be described
* below.
*
* The first (and most common thread) is when a user task requests the
* transmission of data.  Under BSD 4.3, this will cause feiOutput() to be  
* called, which will cause ether_output() to be called, which will cause this 
* routine to be called (usually). This routine will not be called if  
* ether_output() finds that our interface output queue is full.  In this case, 
* the outgoing data will be thrown out. BSD 4.4 uses a slight different model
* in which the generic ether_output() is called directly, followed by a call 
* to this routine.
*
* The second, and most obscure thread, is when the reception of certain
* packets causes an immediate (attempted) response.  For example, ICMP echo
* packets (ping), and ICMP "no listener on that port" notifications.  All
* functions in this driver that handle the reception side are executed in the
* context of netTask().  Always.  So, in the case being discussed, netTask()
* will receive these certain packets, cause IP to be stimulated, and cause the
* generation of a response to be sent.  We then find ourselves following the
* first thread explained above, with the important distinction that the
* context is that of netTask().
*
* The third thread occurs when this routine runs out of CFDs and returns.  If
* this occurs when our output queue is not empty, this routine would typically
* not get called again until new output was requested.  Even worse, if the
* output queue was also full, this routine would never get called again and
* we would have a lock state.  It DOES happen.  To guard against this,
* we turn on a watchdog timer to scedule us again after an appropriate timeout.
*
* Note that this function is ALWAYS called between an splnet() and an splx().
* This is true because netTask(), and ether_output() take care of
* this when calling this function.  Therefore, no calls to these spl functions
* are needed anywhere in this output thread.  Thus, mutual exclusion is
* guaranteed.
*
* RETURNS: N/A
*/

LOCAL void feiTxStartup
    (
#ifdef BSD43_DRIVER
    int		unit
#else
    DRV_CTRL * 	pDrvCtrl 
#endif
    )
    {
#ifdef BSD43_DRIVER
    DRV_CTRL *	pDrvCtrl = &drvCtrl [unit];
#else
    int		unit = pDrvCtrl->idr.ac_if.if_unit;
#endif
    CFD *pCFD = pDrvCtrl->pCFD;
    CFD *pCFDNext;
    MBUF *	pMbuf;
    int		length;
    int status = OK;

    /* loop until no more tx packets or we are out of tx resources */

    while (pDrvCtrl->idr.ac_if.if_snd.ifq_head)
        {
        pCFDNext = (CFD *) SYS_TO_LOCAL_ADDR(unit,(UINT32)LINK_RD(&pCFD->link));

        if (!(pCFDNext->cfdStatus & CFD_S_COMPLETE)) /* next CFD free ? */
            {
            pDrvCtrl->txStall = TRUE;

            wdStart (pDrvCtrl->txRestart, FEI_TX_RESTART_TMO * feiClkRate,
                (FUNCPTR) feiTxRestart, unit);

            break;
            }

        if (pDrvCtrl->txStall)
            wdCancel (pDrvCtrl->txRestart);

        IF_DEQUEUE (&pDrvCtrl->idr.ac_if.if_snd, pMbuf); /* dequeue a packet */

        copy_from_mbufs (pCFD->cfdTcb.enetHdr, pMbuf, length);

        length = max (ETHERSMALL, length); /* short frames are padded */

        if ((etherOutputHookRtn != NULL) && (*etherOutputHookRtn)
            (&pDrvCtrl->idr, pCFD->cfdTcb.enetHdr, length))
            continue;			/* hook routine ate packet */

	/* get frame ready for transmission */

        pCFD->cfdTcb.count = (length & 0x3fff) | TCB_CNT_EOF;
	pCFD->cfdStatus = 0;
	pCFD->cfdCommand = CFD_C_XMIT | CFD_C_SUSP;
	pCFD->pPrev->cfdCommand &= ~CFD_C_SUSP; /* hook frame into CBL */

	/* undates the first ready RFD pointer */
	pDrvCtrl->pCFD =
	    (CFD *)SYS_TO_LOCAL_ADDR(unit,(UINT32)LINK_RD(&pCFD->link) );

        /* Bump the statistic counter. */

        pDrvCtrl->idr.ac_if.if_opackets++;

	/* resume CU operation ? (check suspended, not sent yet, cmd return) */

	if (((pDrvCtrl->pCSR->scbStatus & SCB_S_CUMASK) == SCB_S_CUSUSP) &&
	    (pCFD->cfdStatus == 0) )
	    {
	    status =feiSCBCommand (pDrvCtrl, SCB_C_CURESUME, FALSE, 0);

	    if (status == ERROR)
	        break;
            }


        }
        /* make sure the last CFD is  transmitted */            
        if (pCFD->cfdStatus == 0)
            feiSCBCommand (pDrvCtrl, SCB_C_CURESUME, FALSE, 0);
    }

/*******************************************************************************
*
* feiTxRestart - reschedule transmit processing
*
* Reschedule transmit processing because we previously had no available
* CFDs.
*
* RETURNS: N/A
*/

LOCAL void feiTxRestart
    (
    int         unit
    )
    {
    DRV_CTRL *  pDrvCtrl = &drvCtrl [unit];

    pDrvCtrl->txStall = FALSE;
#ifdef BSD43_DRIVER
    (void) netJobAdd ((FUNCPTR) feiTxStartup, unit, 0, 0, 0, 0);
#else
    (void) netJobAdd ((FUNCPTR) feiTxStartup, (int)pDrvCtrl, 0, 0, 0, 0);
#endif
    }

/*******************************************************************************
*
* feiInt - entry point for handling interrupts from the 82557
*
* The interrupting events are acknowledged to the device, so that the device
* will de-assert its interrupt signal.  The amount of work done here is kept
* to a minimum; the bulk of the work is defered to the netTask.
*
* RETURNS: N/A
*/

LOCAL void feiInt
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    UINT16	event;
    int		unit = pDrvCtrl->idr.ac_if.if_unit;

    event = pDrvCtrl->pCSR->scbStatus;	/* save int events */

    if ((event & SCB_S_STATMASK) == 0)
        {
        /* This device was not the cause of the shared int */
 
        return; 
        }
 
    /* clear chip level interrupt pending */
    pDrvCtrl->pCSR->scbStatus = (event&SCB_S_STATMASK); 

    /* board level interrupt acknowledge */
    BOARD_INT_ACK(unit);

#ifdef	FEI_DEBUG
    if (feiDebug)
        logMsg ("feiInt: event 0x%x, scbStatus 0x%x\n", event, 
	     pDrvCtrl->pCSR->scbStatus, 0, 0, 0, 0);
#endif	/* FEI_DEBUG */

    /* handle receive interrupts */

    if ( (event & SCB_S_FR) && !(pDrvCtrl->pCSR->scbCommand & SCB_C_M)) 
	{
        I82557_INT_DISABLE;
        pDrvCtrl->rxHandle = TRUE;

        (void) netJobAdd ((FUNCPTR) feiHandleRecvInt, (int) pDrvCtrl,
            0, 0, 0, 0);
        }
    }
    
/*******************************************************************************
*
* feiHandleRecvInt - service task-level interrupts for receive frames
*
* This routine is run in netTask's context.  The ISR scheduled
* this routine so that it could handle receive packets at task level.
*
* RETURNS: N/A
*/

LOCAL void feiHandleRecvInt
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    RFD *	pRFD = pDrvCtrl->pRFD;
    RFD *	pRFDNew;
    int		unit = pDrvCtrl->idr.ac_if.if_unit;

    while (pRFD->rfdStatus & RFD_S_COMPLETE)   /* while packets to process */
        {

        pDrvCtrl->rxHandle = TRUE;             /* interlock with eiInt() */
	if (!(pRFD->rfdStatus & RFD_S_OK))
	    pDrvCtrl->idr.ac_if.if_ierrors++;
	else
	    {
	    if (feiReceive (pDrvCtrl, pRFD) == ERROR)
		{

		/* buffer loaning was done */

                pRFDNew = pDrvCtrl->pRFDL;
                pDrvCtrl->pRFDL = pRFDNew->pPrev;

		LINK_WR (&pRFD->pPrev->link, 
		    LOCAL_TO_SYS_ADDR(unit,(UINT32)pRFDNew));

		LINK_WR (&pRFDNew->link, LINK_RD (&pRFD->link));
		pRFDNew->pPrev = pRFD->pPrev;
		pRFD = (RFD *) SYS_TO_LOCAL_ADDR(unit,
		    (UINT32)LINK_RD (&pRFD->link));

		pRFD->pPrev = pRFDNew;
		pRFD = pRFDNew;
		}
	    }

        pRFD->rfdStatus = 0;
        pRFD->rfdCommand = RFD_C_EL;            /* chip can have it.... */
	pRFD->pPrev->rfdCommand &= ~RFD_C_EL;   /* hook frame into CBL */

        pRFD = (RFD *) SYS_TO_LOCAL_ADDR(unit,(UINT32)LINK_RD (&pRFD->link) )
            ;   /* bump to next RFD in ring */

        pDrvCtrl->pCSR->scbStatus &= (SCB_S_FR | SCB_S_RNR);
        }

    pDrvCtrl->pRFD = pRFD;                      /* keep track of current RFD */

    /* kick receiver (if needed) */

    if ((pDrvCtrl->pCSR->scbStatus & SCB_S_RUMASK) != SCB_S_RURDY)
        {
        if (feiSCBCommand (pDrvCtrl, SCB_C_RUSTART, TRUE, 
                    (UINT32 *)LOCAL_TO_SYS_ADDR(unit, (UINT32)pRFD)) == ERROR)
            {
	    pDrvCtrl->rxHandle = FALSE;
            return;
	    }

        while ((pDrvCtrl->pCSR->scbCommand)&SCB_C_RUMASK)
            ;      /* wait for RUSTART accept */
        }

    pDrvCtrl->rxHandle = FALSE;

    /* re-enable receiver interrupts */
    I82557_INT_ENABLE;
    }

/*******************************************************************************
*
* feiReceive - pass a received frame to the next layer up
*
* Strips the Ethernet header and passes the packet to the appropriate
* protocol.  The return value indicates if buffer loaning was used to hold
* the data.  A return value of OK means that loaning was not done.
*
* RETURNS: OK, or ERROR if buffer-loaning the RFD.
*/

LOCAL STATUS feiReceive
    (
    DRV_CTRL *	pDrvCtrl,
    RFD *	pRFD
    )
    {
    struct ether_header *	pEh;
    u_char *	pData;
    int		len;
    UINT16	etherType;
    MBUF *	pMbuf = NULL;
    STATUS	retVal = OK;

    len = pRFD->actualCnt & 0x3fff;	/* get frame length */
    pEh = (struct ether_header *)pRFD->enetHdr;	/* get ptr to ethernet header */

    pDrvCtrl->idr.ac_if.if_ipackets++;          /* bump statistic */

    /* call hook routine if one is connected */

    if ((etherInputHookRtn != NULL) && ((*etherInputHookRtn)
	(& pDrvCtrl->idr.ac_if, (char *) pEh, len)))
        return (OK);

    len -= EH_SIZE;
    if (len <= 0)
	{
	pDrvCtrl->idr.ac_if.if_ierrors++;           /* bump error stat */
        pDrvCtrl->idr.ac_if.if_ipackets--;          /* fix statistic */
	return (OK);
	}

    pData = (u_char *) pRFD->enetData;
    etherType = ntohs (pEh->ether_type);

    if ((pDrvCtrl->pRFDL != NULL) && (USE_CLUSTER (len)) &&
        ((pMbuf = build_cluster (pData, len, (struct ifnet *) &pDrvCtrl->idr, 
				 MC_FEI, &pRFD->refCnt, (FUNCPTR) feiLoanFree, 
				 (int) pDrvCtrl, (int) pRFD, NULL)) 
		 != NULL))
        retVal = ERROR;			/* we loaned a frame */
    else
        pMbuf = copy_to_mbufs (pData, len, 0, 
			       (struct ifnet *) &pDrvCtrl->idr);

    if (pMbuf != NULL)
	{
#ifdef BSD43_DRIVER
        do_protocol_with_type (etherType, pMbuf, &pDrvCtrl->idr, len);
#else
        do_protocol (pEh, pMbuf, &pDrvCtrl->idr, len);
#endif
	}
    else
	{
	pDrvCtrl->idr.ac_if.if_ierrors++;           /* bump error stat */
        pDrvCtrl->idr.ac_if.if_ipackets--;          /* fix statistic */
	}

    return (retVal);
    }

/*******************************************************************************
*
* feiLoanFree - return a loaned receive frame descriptor
*
* This routine is called by the protocol code when it has completed use of
* an RFD that we loaned to it.
*
* RETURNS: N/A
*/

LOCAL void feiLoanFree
    (
    DRV_CTRL *	pDrvCtrl,
    RFD *	pRFD
    )
    {
    pRFD->refCnt = 0;			/* should already be zero... */

    pRFD->pPrev = pDrvCtrl->pRFDL;
    pDrvCtrl->pRFDL = pRFD;
    }

/*******************************************************************************
*
* feiNOP - format and issue a NOP command
*
* RETURNS: OK, or ERROR if the NOP command failed.
*/

LOCAL STATUS feiNOP
    (
    int		unit
    )
    {
    if (!(feiAction (unit, CFD_C_NOP) & CFD_S_OK))
	return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* feiDiag - format and issue a diagnostic command
*
* RETURNS: OK, or ERROR if the diagnostic command failed.
*/

LOCAL STATUS feiDiag
    (
    int		unit
    )
    {
    UINT16	stat;

    if (((stat = feiAction (unit, CFD_C_DIAG)) &
        (CFD_S_OK | CFD_S_DIAG_F)) != CFD_S_OK)
        {
#ifdef  FEI_DEBUG
        if (feiDebug)
            printf ("if_fei.c: 82557 diagnostics failed, cfdStatus 0x%x\n",
                stat);
#endif  /* FEI_DEBUG */
        return (ERROR);
        }

    return (OK);
    }

/*******************************************************************************
*
* feiConfig - format and issue a config command
*
* RETURNS: OK, or ERROR if the config command failed.
*/

LOCAL STATUS feiConfig
    (
    int unit
    )
    {
    DRV_CTRL *	pDrvCtrl = & drvCtrl [unit];
    CFD *	pCFD = pDrvCtrl->pCFD;

    /* set to config values recommeded by the i82557 User's Manual */

    pCFD->cfdConfig.ccByte0  = 0x16;
    pCFD->cfdConfig.ccByte1  = 0x88;  /* 0x08;  */
    pCFD->cfdConfig.ccByte2  = 0x00;
    pCFD->cfdConfig.ccByte3  = 0x00;
    pCFD->cfdConfig.ccByte4  = 0x00;   /* 0x00; */
    pCFD->cfdConfig.ccByte5  = 0x00;   /* 0x80; */
    pCFD->cfdConfig.ccByte6  = 0x38;   /* save bad frame,no TNO 0x3a; */
    pCFD->cfdConfig.ccByte7  = 0x03;   
    pCFD->cfdConfig.ccByte8  = 0x01;	/* MII operation */
    pCFD->cfdConfig.ccByte9  = 0x00;
    pCFD->cfdConfig.ccByte10 = 0x2e;
    pCFD->cfdConfig.ccByte11 = 0x00;    
    pCFD->cfdConfig.ccByte12 = 0x60;
    pCFD->cfdConfig.ccByte13 = 0x00;
    pCFD->cfdConfig.ccByte14 = 0xf2;
    if (pDrvCtrl->idr.ac_if.if_flags & IFF_PROMISC)
        pCFD->cfdConfig.ccByte15 = 0x49;     /* 0x49;  */
    else
        pCFD->cfdConfig.ccByte15 = 0x48;     /* 0x49 PROMISC;  */
    pCFD->cfdConfig.ccByte16 = 0x00;
    pCFD->cfdConfig.ccByte17 = 0x40;
    pCFD->cfdConfig.ccByte18 = 0xf2;
if (pDrvCtrl->board.phyDpx != PHY_HALF_DPX)
    pCFD->cfdConfig.ccByte19 = 0x80;
else
    pCFD->cfdConfig.ccByte19 = 0x00; /*c0 force full deplex    0x80;   */

    pCFD->cfdConfig.ccByte20 = 0x3f;
    pCFD->cfdConfig.ccByte21 = 0x05;

    if (!(feiAction (unit, CFD_C_CONFIG) & CFD_S_OK))
	return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* feiIASetup - format and issue an interface address command
*
* RETURNS: OK, or ERROR if the address command failed.
*/

LOCAL STATUS feiIASetup
    (
    int unit
    )
    {
    DRV_CTRL *	pDrvCtrl = & drvCtrl [unit];

    bcopy ((char *) pDrvCtrl->idr.ac_enaddr,
	(char *) pDrvCtrl->pCFD->cfdIASetup.ciAddress, 6);

    if (!(feiAction (unit, CFD_C_IASETUP) & CFD_S_OK))
	return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* feiAction - execute the specified action command
*
* This routine executes the specified action
*
* We do the command contained in the CFD synchronously, so that we know
* it's complete upon return.
*
* RETURNS: The status return of the action command, or 0 on failure.
*/

LOCAL UINT16 feiAction
    (
    int		unit,
    UINT16	action
    )
    {
    DRV_CTRL *	pDrvCtrl = & drvCtrl [unit];
    CFD *	pCFD = pDrvCtrl->pCFD;
    UINT16	retVal = 0;
    int		ix;
    UINT8       state;

    if (!(pCFD->cfdStatus & CFD_S_COMPLETE))	/* next CFD free ? */
	return (0);

    /* fill in CFD */

    pCFD->cfdStatus = 0;			/* clear status */
    pCFD->cfdCommand = action | CFD_C_SUSP;	/* fill in action */
    pCFD->pPrev->cfdCommand &= ~CFD_C_SUSP;	/* hook into CBL */

    pDrvCtrl->pCFD = (CFD *) SYS_TO_LOCAL_ADDR(unit,
	(UINT32)LINK_RD(&pCFD->link) ); /* bump current CFD */


    /* check CU operation -- kick if needed */

    if ((((state = pDrvCtrl->pCSR->scbStatus) & SCB_S_CUMASK) != SCB_S_CUACTIVE)
        && (pCFD->cfdStatus == 0))
        {
        if (state == SCB_S_CUIDLE)
            {
            if (feiSCBCommand (pDrvCtrl, SCB_C_CUSTART, TRUE,
                (UINT32 *) LOCAL_TO_SYS_ADDR(unit,(UINT32)pCFD) ) == ERROR)
                {
#ifdef  FEI_DEBUG
                if (feiDebug)
                    printf ("feiAction: feiSCBCommand failed\n");
#endif  /* FEI_DEBUG */
                goto errorAction;
                }
            }
        else
            {
            if (feiSCBCommand (pDrvCtrl, SCB_C_CURESUME, FALSE, 0x0) == ERROR)
                {
#ifdef  FEI_DEBUG
                if (feiDebug)
                    printf ("feiAction: feiSCBCommand failed\n");
#endif  /* FEI_DEBUG */
                goto errorAction;
                }
            }
        }

    /* wait for command to complete */

    for (ix = (FEI_ACTION_TMO * feiClkRate); --ix;)
	{
        if ((retVal = pCFD->cfdStatus) & CFD_S_COMPLETE)
            break;

	taskDelay (1);
        }

    if (!ix)
	{
#ifdef	FEI_DEBUG
         printf ("feiAction: command 0x%x, CFD status 0x%x\n", action,
	     pCFD->cfdStatus);
#endif	/* FEI_DEBUG */

        retVal = 0;
        goto errorAction;
	}

    /* put CFD back to a TxCB - mirrors feiattach() */

errorAction:

    pCFD->cfdStatus = CFD_S_COMPLETE | CFD_S_OK;
    LINK_WR (&pCFD->cfdTcb.pTBD, 0xffffffff); /* no TBDs used */
    pCFD->cfdTcb.txThresh = pDrvCtrl->board.tcbTxThresh;
    pCFD->cfdTcb.tbdNum = 0;

    return (retVal);
    }

/*******************************************************************************
*
* feiSCBCommand - deliver a command to the 82557 via the SCB
*
* This function causes the device to execute a command. An error status is
* returned if the command field does not return to zero, from a previous
* command, in a reasonable amount of time.
*
* RETURNS: OK, or ERROR if the command field appears to be frozen.
*/

LOCAL STATUS feiSCBCommand
    (
    DRV_CTRL *	pDrvCtrl,
    UINT8	cmd,
    BOOL	addrValid,
    UINT32 *	addr
    )
    {
    int		ix;

    for (ix = 0x8000; --ix;)
	{
	if ((pDrvCtrl->pCSR->scbCommand & SCB_CR_MASK) == 0)
	    break;

        }

    if (!ix)
	{
#ifdef  FEI_DEBUG
        if (feiDebug)
            printf ("feiSCBCommand: command 0x%x failed, scb 0x%x\n", cmd,
                pDrvCtrl->pCSR->scbCommand);
#endif  /* FEI_DEBUG */
        return (ERROR);
        }

    if (addrValid)			/* optionally fill the GP */
	LINK_WR (&pDrvCtrl->pCSR->scbGP, addr);

    pDrvCtrl->pCSR->scbCommand |= cmd;	/* write SCB command byte */
    return (OK);
    }

#ifdef  FEI_DEBUG
/*******************************************************************************
*
* feiDumpPrint - Display statistical counters
*
* This routine displays i82557 statistical counters
*
* RETURNS: OK, or ERROR if the DUMP command failed.
*/

LOCAL STATUS feiDumpPrint
    (
    int         unit
    )
    {
    UINT32      dumpArray [18];
    UINT32 *    pDump;
    STATUS status = OK;

    /* dump area must be long-word allign */

    pDump = (UINT32 *) (((UINT32) dumpArray + 4) & 0xfffffffc);

    status = feiErrCounterDump(unit,pDump); 

    printf ("\n");
    printf ("Tx good frames:             %d\n", *pDump++);
    printf ("Tx MAXCOL errors:           %d\n", *pDump++);
    printf ("Tx LATECOL errors:          %d\n", *pDump++);
    printf ("Tx underrun errors:         %d\n", *pDump++);
    printf ("Tx lost CRS errors:         %d\n", *pDump++);
    printf ("Tx deferred:                %d\n", *pDump++);
    printf ("Tx single collisions:       %d\n", *pDump++);
    printf ("Tx multiple collisions:     %d\n", *pDump++);
    printf ("Tx total collisions:        %d\n", *pDump++);
    printf ("Rx good frames:             %d\n", *pDump++);
    printf ("Rx CRC errors:              %d\n", *pDump++);
    printf ("Rx allignment errors:       %d\n", *pDump++);
    printf ("Rx resource errors:         %d\n", *pDump++);
    printf ("Rx overrun errors:          %d\n", *pDump++);
    printf ("Rx collision detect errors: %d\n", *pDump++);
    printf ("Rx short frame errors:      %d\n", *pDump++);

    return (status);
    }


/*****************************************************************************
*
* feiErrCounterDump - dump statistical counters 
*
* This routine dumps statistical counters for the purpose of debugging and
* tuning the 82557.
*
* The <memAddr> parameter is the pointer to an array of 68 bytes in the
* local memory.  This memory region must be allocated before this routine is
* called.  The memory space must also be DWORD (4 bytes) aligned.  When the
* last DWORD (4 bytes) is written to a value, 0xa007, it indicates the dump
* command has completed.  To determine the meaning of each statistical
* counter, see the Intel 82557 manual.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS feiErrCounterDump 
    (
    int unit,
    UINT32 *memAddr
    )

    {
    DRV_CTRL *  pDrvCtrl = &drvCtrl [unit];
    STATUS status = OK;
    int i; 
    int         s = splimp ();   /* block network access to 82557 resource */

    memAddr[16]=0;    /* make sure the last DWORD is 0 */

    /* tells the 82557 where to write dump data */
    if (feiSCBCommand (pDrvCtrl, SCB_C_CULDDUMP, TRUE,
	    (UINT32 *) LOCAL_TO_SYS_ADDR(unit,(UINT32)memAddr) ) == ERROR)
        status = ERROR;

    /* issue the dump and reset counter command */
    if (feiSCBCommand (pDrvCtrl, SCB_C_CUDUMPRST,FALSE,(UINT32)0) == ERROR)
	status = ERROR;
    
    splx (s);   /* release 82557 resource to network */

    /* waits for it done */ 
    for (i=0;i<60;i++)
	{
	if (memAddr[16] == (UINT32)0xa007)
	    break;
        taskDelay(2);
	}

    if (i==60)
	status = ERROR;

    return (status);
    }
#endif  /* FEI_DEBUG */

/****************************************************************************
*
* feiMDIRead - read the MDI register
*
* This routine reads the specific register in the PHY device
* Valid PHY address is 0-31
*
* RETURNS: OK or ERROR.
*
*/

static int feiMDIRead 
    (
    int unit,
    int regAddr,
    int phyAddr,
    UINT16 *retVal
    )

    {

    DRV_CTRL *  pDrvCtrl = &drvCtrl [unit];
    int i;
    UINT32 mdrValue;
    volatile UINT32 *mdiReg = (volatile UINT32 *)&pDrvCtrl->pCSR->mdiCR;
   
    mdrValue = ((regAddr<<16) | (phyAddr << 21) | (MDI_READ<<26) );
   
    /* write to MDI */
    *mdiReg = mdrValue;

    taskDelay(2);    /* wait for a while */

    /* check if it's done */
    for (i=0;i<40;i++)
        {
        if ( (*mdiReg) & (1<<28) )
            break;
        taskDelay(1);
        }

     if (i==40)
         return (ERROR);

     *retVal = (UINT16)((*mdiReg)&0xffff);
     return(OK);

     } 

/**************************************************************************
*
* feiMDIWrite - write to the MDI register
*
* This routine writes the specific register in the PHY device
* Valid PHY address is 0-31
*
* RETURNS: OK or ERROR.
*/

static int feiMDIWrite
    (
    int unit,
    int regAddr,
    int phyAddr,
    UINT16 writeData 
    )

    {

    DRV_CTRL *  pDrvCtrl = &drvCtrl [unit];
    int i;
    UINT32 mdrValue;
    volatile UINT32 *mdrReg = (volatile UINT32 *)&pDrvCtrl->pCSR->mdiCR;
  
    mdrValue = ((regAddr<<16) | (phyAddr << 21) | (MDI_WRITE<<26) | writeData );
   
    /* write to M */
    *mdrReg = mdrValue;

    taskDelay(2);    /* wait for a while */


    /* check if it's done */
    for (i=0;i<40;i++)
        {
        if ( (*mdrReg) & (1<<28) )
            break;
        taskDelay(1);
        }

     if (i==40)
         return (ERROR);

     return(OK);
     }


/****************************************************************************
*
* feiMDIPhyLinkSet - detect and set the link for the PHY device
*
* This routine first checks if the link has been established.  If not, it
* isolates the other one, and tries to establish the link.  PHY device 0 is
* always at PHY address 0.  PHY 1 can be at 1-31 PHY addresses.
*
* RETURNS: OK or ERROR.
*
*/

static int feiMDIPhyLinkSet
    (
    int unit,
    int phyAddr
    )

    {
    UINT16 ctlReg;
    UINT16 statusReg;
    int isoDev,i;

    /* read control register */
    feiMDIRead (unit, MDI_CTRL_REG, phyAddr, &ctlReg);

    /* read again */
    feiMDIRead (unit, MDI_CTRL_REG, phyAddr, &ctlReg);

    /* check if the PHY is there */
    if (ctlReg == (UINT16)0xffff)
        return (ERROR);   /* no PHY present */

#if 0
    /* CR RESET will cause link down */
    feiMDIWrite (unit,MDI_CTRL_REG,phyAddr,MDI_CR_RESET); 
    taskDelay(4);
#endif

    /* The PHY is there, read status register  */
    feiMDIRead (unit, MDI_STATUS_REG, phyAddr,&statusReg);

    /* in case the status bit is the latched bit */
    if ( !(statusReg & MDI_SR_LINK_STATUS))
        feiMDIRead (unit, MDI_STATUS_REG, phyAddr, &statusReg);

    if (statusReg & MDI_SR_LINK_STATUS)
        return (OK);  /* Device found and link OK */

    /* no link is established, let's configure it */
    
    /* isolates the other PHY */
    isoDev = (phyAddr)?0:1;
    feiMDIWrite (unit,MDI_CTRL_REG,isoDev,MDI_CR_ISOLATE);

    /* wait for a while */
    taskDelay (2);

    /* enable the PHY device we try to configure */
    feiMDIWrite (unit,MDI_CTRL_REG,phyAddr,MDI_CR_SELECT);

    taskDelay (2); /* wait for a while for command take effect */

    /* restart the auto negotiation process, execute anyways even if
     * it has no such capability.
     */
    feiMDIWrite (unit,MDI_CTRL_REG,phyAddr,MDI_CR_RESTART | MDI_CR_SELECT);

    /* wait for auto-negotiation to complete */
    for (i=0;i<80;i++)
        {
        /* read the status register */
        feiMDIRead (unit, MDI_STATUS_REG, phyAddr, &statusReg);
        feiMDIRead (unit, MDI_STATUS_REG, phyAddr, &statusReg);
   
        if (statusReg & (MDI_SR_AUTO_NEG | MDI_SR_REMOTE_FAULT) )
            break;

        taskDelay (2);

	if (!(statusReg & MDI_SR_AUTO_SELECT) )
	    break;  /* no such capability */
        }

    /* Read the status register */
    feiMDIRead (unit, MDI_STATUS_REG, phyAddr, &statusReg);

    /* some of the status bits require to clear a latch */
    if ( !(statusReg & MDI_SR_LINK_STATUS))
        feiMDIRead (unit, MDI_STATUS_REG, phyAddr, &statusReg);

    if (statusReg & MDI_SR_LINK_STATUS)
        return (OK);  /* Link configure done and successful */
        
    return (PHY_LINK_ERROR);   /* device is there, cann't establish link */
    }

/***************************************************************************
*
* feiMDIPhyConfig - configure the PHY device
*
* This routine configures the PHY device according to the parameters
* specified by users or the default value.
*
* RETURNS: OK or ERROR.
*
*/

static int feiMDIPhyConfig 
    (
    int unit,
    int phyAddr
    )

    {
    DRV_CTRL *  pDrvCtrl = &drvCtrl [unit];
    UINT16 ctlReg = 0;
    int fullDpx=FALSE;
    int autoSelect =FALSE;
    UINT16 statusReg;
    int status,i;

    /* find out what capabilities the device have */

    /*  read status register  */
    feiMDIRead (unit, MDI_STATUS_REG, phyAddr,&statusReg);

    /* some of the status bits require to read twice */
    feiMDIRead (unit, MDI_STATUS_REG, phyAddr, &statusReg);

    /* The device at least has to have the half duplex and 10mb speed */
    if (statusReg & (MDI_SR_10T_FULL_DPX | MDI_SR_TX_FULL_DPX) )
        fullDpx = TRUE;

    if (statusReg & MDI_SR_AUTO_SELECT)
        autoSelect = TRUE;

#ifdef	FEI_DEBUG 
    logMsg ("status REG = %x !!!! \n",statusReg,0,0,0,0,0);
#endif	/* FEI_DEBUG */

    if (pDrvCtrl->board.phyDpx == PHY_FULL_DPX && fullDpx == TRUE)
        ctlReg |= MDI_CR_FDX;

    if (pDrvCtrl->board.phySpeed == PHY_100MBS)
        ctlReg |= MDI_CR_100;

    if (pDrvCtrl->board.phySpeed == PHY_AUTO_SPEED || 
			       pDrvCtrl->board.phyDpx == PHY_AUTO_DPX )
        {
	if (autoSelect != TRUE)
	    {
	    /* set back to default */
	    pDrvCtrl->board.phySpeed = PHY_10MBS;
	    pDrvCtrl->board.phyDpx = PHY_HALF_DPX;
	    ctlReg |= (PHY_10MBS | PHY_HALF_DPX);
	    }
	    else
                ctlReg |= (MDI_CR_SELECT|MDI_CR_RESTART);
         }

    /* or other possible board level selection */
    ctlReg |= pDrvCtrl->board.others;

    /* configure the PHY */
    feiMDIWrite (unit,MDI_CTRL_REG,phyAddr,ctlReg);
   
    taskDelay (2);   /* wait for a while */

    if (! (ctlReg & MDI_CR_RESTART) )
        return (OK);

    /* we are here if the restart auto negotiation is selected */
#ifdef	FEI_DEBUG
    logMsg ("auto NEGOTIATION STARTS !!!! \n",0,0,0,0,0,0);
#endif	/* FEI_DEBUG */

    /* wait for it done */
    for (status = PHY_AUTO_FAIL,i=0;i<80;i++)
       {
       /*  read status register, some bits have the latch,first read clears */
       feiMDIRead (unit, MDI_STATUS_REG, phyAddr,&statusReg);
       feiMDIRead (unit, MDI_STATUS_REG, phyAddr, &statusReg);

       if (statusReg & MDI_SR_AUTO_NEG)
           {
           status = OK;  /* auto negotiation completed */
           break;
           }

       if (statusReg & MDI_SR_REMOTE_FAULT)
           {
           status = PHY_AUTO_FAIL;  /* auto negotiation fails */
           break;
           }
        }
              
      return (status);
      } 

/**************************************************************************
*
* feiPhyInit - initialize and configure the PHY device if there is one
*
* This routine initialize and configure the PHY device if there is one.
*
* RETURNS: OK or ERROR.
*
*/

static int feiPhyInit
    (
    int unit
    )

    {
    int status;
    int phyDevice;
    DRV_CTRL *  pDrvCtrl = &drvCtrl [unit];

    if (pDrvCtrl->board.phyAddr > 31)
        return (OK);  /* not a MII interface,no need to initialize PHY */

     /* configure the Physical layer medium if it's MII interface */
     /* starts with logical PHY 1 */

     phyDevice = pDrvCtrl->board.phyAddr?pDrvCtrl->board.phyAddr:1;

     status = feiMDIPhyLinkSet(unit,phyDevice);

     if (status != OK)
         {
         phyDevice = 0;
         status = feiMDIPhyLinkSet(unit,phyDevice);
         }

     if (status != OK)
	 {
         printf ("LINK_FAIL error, check the line connection !!!\n");
         return (status);
         }

     /* we are here if a valid link is established */
     status = feiMDIPhyConfig (unit,phyDevice); 

     if (status == PHY_AUTO_FAIL)
         {
         /* force default speed and duplex */
         pDrvCtrl->board.phySpeed = PHY_10MBS;
         pDrvCtrl->board.phyDpx = PHY_HALF_DPX; 
         pDrvCtrl->board.others = 0;

         /* and configure it again */
         status = feiMDIPhyConfig (unit,phyDevice);  
         }

     return (status);
     } 
