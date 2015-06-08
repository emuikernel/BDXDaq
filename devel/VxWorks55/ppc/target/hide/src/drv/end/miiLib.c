/* miiLib.c - Media Independent Interface library */
 
/* Copyright 1989-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01p,13may02,rcs set the phyAddr before calling pPhyOptRegsRtn also created
                miiPhyOptFuncMultiSet() to facilitate multiple types of devices
                spr# 76711
01o,14jun01,rcs protected calls to new PHY_INFO fields with test for 
                MII_PHY_GMII_TYPE (SPR# 68502)  
01n,23feb01,jln add basic GMII support for 1000T auto-negotiation capability,
                restored call to miiLibInit (). (SPR# 68502)
01m,12dec00,rcs removed global variables miiValue and miiFunc[]
01l,12dec00,rcs fix coding error (SPR# 62972)  
01k,25oct00,ham doc: cleanup for vxWorks AE 1.0.
01j,19sep00,rcs in miiBasicCheck() changed MII_SYS_DELAY argument  
                and ix increment to sysClkRateGet(). (SPR# 33991)
01i,10jun00,dat restored funcBindP.h
01h,06may00,jgn SPR 31705, fixed wdog use, fixed link status change monitoring,
                added more debug messages
01g,05jan00,stv removed private/funcBindP.h (SPR# 29875).
01f,23nov99,cn  removed call to miiLibInit (), to make miiLib a component 
                (SPR #29542).
01e,28oct99,cn  added miiLibInit (), miiLibUnInit (), miiPhyUnInit (),
                miiPhyBusScan (), miiPhyMonitorStart (), miiPhyMonitor (). 
                Also updated documentation.
01d,28sep99,cn  changed miiProbe to better detect PHYs.
01c,13sep99,cn  fixed SPR# 28305. Also added show routines.
01b,16jun99,cn  implemented changes after code review.
01a,16mar99,cn  written from motFecEnd.c, 01c.
*/

/*
DESCRIPTION
This module implements a Media Independent Interface (MII) library. 

The MII is an inexpensive and easy-to-implement interconnection between
the Carrier Sense Multiple Access with Collision Detection (CSMA/CD)
media access controllers and the Physical Layer Entities (PHYs).

The purpose of this library is to provide Ethernet drivers in VxWorks
with a standardized, MII-compliant, easy-to-use interface to various PHYs.
In other words, using the services of this library, network drivers
will be able to scan the existing PHYs, run diagnostics, electrically
isolate a subset of them, negotiate their technology abilities with other
link-partners on the network, and ultimately initialize and configure a
specific PHY in a proper, MII-compliant fashion.

In order to initialize and configure a PHY, its MII management interface 
has to be used. This is made up of two lines: management data clock (MDC) 
and management data input/output (MDIO). The former provides the timing 
reference for transfer of information on the MDIO signal.  The latter is 
used to transfer control and status information between the PHY and the 
MAC controller. For this transfer to be successful, the information itself
has to be encoded into a frame format, and both the MDIO and MDC signals have
to comply with certain requirements as described in the 802.3u IEEE Standard.

Since no assumption can be made as to the specific MAC-to-MII
interface, this library expects the driver's writer to provide it with 
specialized read and write routines to access that interface. See EXTERNAL 
SUPPORT REQUIREMENTS below.

miiPhyUnInit (), miiLibInit (), miiLibUnInit (), miiPhyOptFuncSet ().
    STATUS    	miiLibInit (void);
.CE
.CS
    STATUS    	miiLibUnInit (void);
EXTERNAL SUPPORT REQUIREMENTS
.IP phyReadRtn ()
.CS
    STATUS    	phyReadRtn (DRV_CTRL * pDrvCtrl, UINT8 phyAddr, 
			    UINT8 phyReg, UINT16 * value);
.CE
This routine is expected to perform any driver-specific functions required
to read a 16-bit word from the <phyReg> register of the MII-compliant PHY
whose address is specified by <phyAddr>. Reading is performed through the
MII management interface.

.IP phyWriteRtn ()
.CS
    STATUS    	phyWriteRtn (DRV_CTRL * pDrvCtrl, UINT8 phyAddr, 
			     UINT8 phyReg, UINT16 value);
.CE
This routine is expected to perform any driver-specific functions required
to write a 16-bit word to the <phyReg> register of the MII-compliant PHY
whose address is specified by <phyAddr>. Writing is performed through the
MII management interface.

.IP phyDelayRtn ()
.CS
    STATUS    	phyDelayRtn (UINT32 phyDelayParm);
.CE
This routine is expected to cause a limited delay to the calling task,
no matter whether this is an active delay, or an inactive one.
miiPhyInit () calls this routine on several occasions throughout
the code with <phyDelayParm> as parameter. This represents the granularity 
of the delay itself, whereas the field <phyMaxDelay> in PHY_INFO is the
maximum allowed delay, in <phyDelayParm> units. The minimum elapsed time
(<phyMaxDelay> * <phyDelayParm>) must be 5 seconds.

The user should be aware that some of these events may take as long as
2-3 seconds to be completed, and he should therefore tune this routine
and the parameter <phyMaxDelay> accordingly.

If the related field <phyDelayRtn> in the PHY_INFO structure is initialized 
to NULL, no delay is performed.

.IP phyLinkDownRtn ()
.CS
    STATUS    	phyLinkDownRtn (DRV_CTRL *);
.CE
This routine is expected to take any action necessary to re-initialize the
media interface, including possibly stopping and restarting the driver itself.
It is called when a link down event is detected for any active PHY, with the
pointer to the relevant driver control structure as only parameter.
.LP

To use this feature, include the following component:
INCLUDE_MIILIB

SEE ALSO: 
.I "IEEE 802.3.2000 Standard"


INTERNAL
This library contains conditional compilation switch MII_DBG.
If defined, adds debug output routines.  Output is further
selectable at run-time via the miiDbg global variable.
*/

#include "vxWorks.h"
#include "errnoLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "iosLib.h"
#include "net/mbuf.h"
#include "logLib.h"
#include "netLib.h"
#include "end.h"
#include "sysLib.h"
#include "taskLib.h"
#include "wdLib.h"
#include "lstLib.h"
#include "miiLib.h"
#include "private/funcBindP.h" /* for logMsg */

/* defines */

/* Driver debug control */

#undef MII_DBG 

/* Driver debug control */

#ifdef MII_DBG
#define MII_DBG_ANY    		0xffff

UINT32		miiDbg = 0x0;

PHY_INFO	* phyInfoDbg = NULL;

#define MII_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)			\
    do {								\
	if ((miiDbg & FLG) && (_func_logMsg != NULL))			\
	    _func_logMsg ((X0), (int)(X1), (int)(X2),(int)(X3),		\
			(int)(X4),(int)(X5),(int)(X6)); 	 	\
    } while (0)

#else /* MII_DBG */

#define MII_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)

#endif /* MII_DBG */
 
#define   MII_PHY_CHECK_CABLE						\
      do {								\
	  if (_func_logMsg != NULL)					\
	      _func_logMsg ("miiPhyInit check cable connection \n",	\
			  0, 0, 0, 0, 0, 0);				\
      } while (0)

/*
 * the table below is used to translate user settings
 * into MII-standard values for the control register.
 */
 
LOCAL UINT16 miiDefLookupTbl [] = {
                                  MII_CR_NORM_EN,
                                  MII_CR_FDX,
                                  MII_CR_100,
                                  (MII_CR_100 | MII_CR_FDX),
                                  MII_CR_100,
    				  MII_CR_RESTART | MII_CR_AUTO_EN
                                  };
 
#define MII_FROM_ANSR_TO_ANAR(statReg, adsReg)				\
    {									\
    phyStat &= MII_SR_ABIL_MASK;					\
    phyStat >>= 6;							\
    phyAds = phyStat + (phyAds & 0x7000) + (phyAds & MII_ADS_SEL_MASK); \
    }

#define MII_SEM_TAKE(tmout) 						\
    semTake (miiMutex, (int) (tmout))

#define MII_SEM_GIVE() 							\
    semGive (miiMutex)

/* globals */

/* locals */

LOCAL SEM_ID	miiMutex;			/* mutex semaphore */
LOCAL LIST	miiList;			/* all PHYs in the system */
LOCAL BOOL	miiLibInitialized = FALSE;	/* have we been initialized? */
LOCAL WDOG_ID   miiWd = NULL;			/* monitor watchdog ID */
LOCAL FUNCPTR	pPhyOptRegsRtn = NULL;

/* forward function declarations */

LOCAL STATUS	miiDiag (PHY_INFO * pPhyInfo, UINT8 phyAddr);
LOCAL STATUS	miiPhyIsolate (PHY_INFO * pPhyInfo, UINT8 isoPhyAddr);
LOCAL STATUS	miiPhyPwrDown (PHY_INFO * pPhyInfo, UINT8 phyAddr);
LOCAL STATUS	miiPhyBestModeSet (PHY_INFO * pPhyInfo, UINT8 phyAddr);
LOCAL STATUS	miiPhyDefModeSet (PHY_INFO * pPhyInfo);
LOCAL STATUS	miiAutoNegotiate (PHY_INFO * pPhyInfo, UINT8 phyAddr);
LOCAL STATUS	miiAutoNegStart (PHY_INFO * pPhyInfo, UINT8 phyAddr);
LOCAL STATUS	miiModeForce (PHY_INFO * pPhyInfo, UINT8 phyAddr);
LOCAL STATUS	miiDefForce (PHY_INFO * pPhyInfo, UINT8 phyAddr);
LOCAL STATUS	miiPhyUpdate (PHY_INFO * pPhyInfo, UINT8 phyAddr);
LOCAL STATUS	miiFlagsHandle (PHY_INFO * pPhyInfo, UINT8 phyAddr);
LOCAL STATUS	miiProbe (PHY_INFO * pPhyInfo, UINT8 phyAddr);
LOCAL STATUS	miiAbilFlagSet (PHY_INFO * pPhyInfo, UINT8 phyAddr);
LOCAL STATUS	miiPhyMonitor (void);
LOCAL STATUS	miiPhyListAdd (PHY_INFO * pPhyInfo);
LOCAL STATUS	miiPhyBusScan (PHY_INFO * pPhyInfo);
LOCAL STATUS	miiPhyLinkSet (PHY_INFO * pPhyInfo);
LOCAL STATUS	miiPhyMonitorStart (void);

/**************************************************************************
*
* miiPhyInit - initialize and configure the PHY devices
*
* This routine scans, initializes and configures the PHY device described
* in <phyInfo>. Space for <phyInfo> is to be provided by the calling
* task.
*
* This routine is called from the driver's Start routine to
* perform media inialization and configuration. To access the PHY
* device through the MII-management interface, it uses the read and
* write routines which are provided by the driver itself
* in the fields  phyReadRtn(), phyWriteRtn() of the phyInfo structure.
* Before it attempts to use this routine, the driver has to properly 
* initialize some of the fields in the <phyInfo> structure, and optionally
* fill in others, as below:
* .CS
*
*    /@ fill in mandatory fields in phyInfo @/
* 
*    pDrvCtrl->phyInfo->pDrvCtrl = (void *) pDrvCtrl;
*    pDrvCtrl->phyInfo->phyWriteRtn = (FUNCPTR) xxxMiiWrite;
*    pDrvCtrl->phyInfo->phyReadRtn = (FUNCPTR) xxxMiiRead;
* 
*    /@ fill in some optional fields in phyInfo @/
* 
*    pDrvCtrl->phyInfo->phyFlags = 0;
*    pDrvCtrl->phyInfo->phyAddr = (UINT8) MII_PHY_DEF_ADDR;
*    pDrvCtrl->phyInfo->phyDefMode = (UINT8) PHY_10BASE_T;
*    pDrvCtrl->phyInfo->phyAnOrderTbl = (MII_AN_ORDER_TBL *)
*                                        &xxxPhyAnOrderTbl;
* 
*    /@ 
*     @ fill in some more optional fields in phyInfo: the delay stuff
*     @ we want this routine to use our xxxDelay () routine, with
*     @ the constant one as an argument, and the max delay we may
*     @ tolerate is the constant MII_PHY_DEF_DELAY, in xxxDelay units
*     @/
* 
*    pDrvCtrl->phyInfo->phyDelayRtn = (FUNCPTR) xxxDelay;
*    pDrvCtrl->phyInfo->phyMaxDelay = MII_PHY_DEF_DELAY;
*    pDrvCtrl->phyInfo->phyDelayParm = 1;
*
*    /@ 
*     @ fill in some more optional fields in phyInfo: the PHY's callback
*     @ to handle "link down" events. This routine is invoked whenever 
*     @ the link status in the PHY being used is detected to be low.
*     @/
* 
*    pDrvCtrl->phyInfo->phyStatChngRtn = (FUNCPTR) xxxRestart;
*
* .CE
*
* Some of the above fields may be overwritten by this routine, since 
* for instance, the logical address of the PHY actually used may differ
* from the user's initial setting. Likewise, the specific PHY being
* initialized, may not support all the technology abilities the user
* has allowed for its operations.
* 
* This routine first scans for all possible PHY addresses in the range 0-31,
* checking for an MII-compliant PHY, and attempts at running some diagnostics 
* on it. If none is found, ERROR is returned.
*
* Typically PHYs are scanned from address 0, but if the user specifies
* an alternative start PHY address via the parameter phyAddr in the 
* phyInfo structure, PHYs are scanned in order starting 
* with the specified PHY address. In addition, if the flag <MII_ALL_BUS_SCAN> 
* is set, this routine will scan the whole bus even if a valid PHY has 
* already been found, and stores bus topology information. If the flags 
* <MII_PHY_ISO>, <MII_PHY_PWR_DOWN> are set, all of the PHYs found but the
* first will be respectively electrically isolated from the MII interface
* and/or put in low-power mode. These two flags are meaningless in a 
* configuration where only one PHY is present.
*
* The phyAddr parameter is very important from a performance point of view.
* Since the MII management interface, through which the PHY is configured,
* is a very slow one, providing an incorrect or invalid address in this
* field may result in a particularly long boot process.
*
* If the flag <MII_ALL_BUS_SCAN> is not set, this routine will 
* assume that the first PHY found is the only one.
*
* This routine then attempts to bring the link up.
* This routine offers two strategies to select a PHY and establish a
* valid link. The default strategy is to use the standard 802.3 style 
* auto-negotiation, where both link partners negotiate all their 
* technology abilities at the same time, and the highest common 
* denominator ability is chosen. Before the auto-negotiation
* is started, the next-page exchange mechanism is disabled.
*
* If GMII interface is used, users can specify it through userFlags --
* <MII_PHY_GMII_TYPE>.
*
* The user can prevent the PHY from negotiating certain abilities via 
* userFlags -- <MII_PHY_FD>, <MII_PHY_100>, <MII_PHY_HD>, and <MII_PHY_10>. 
* as well as <MII_PHY_1000T_HD> and <MII_PHY_1000T_FD> if GMII is used.
* When <MII_PHY_FD> is not specified, full duplex will not be 
* negotiated; when <MII_PHY_HD> is not specified half duplex 
* will not be negotiated, when <MII_PHY_100> is not specified, 
* 100Mbps ability will not be negotiated; when <MII_PHY_10> is not 
* specified, 10Mbps ability will not be negotiated.
* Also, if GMII is used, when <MII_PHY_1000T_HD> is not specified,
* 1000T with half duplex mode will not be negotiated. Same thing 
* applied to 1000T with full duplex mode via <MII_PHY_1000T_FD>.

* Flow control ability can also be negotiated via user flags --
* <MII_PHY_TX_FLOW_CTRL> and <MII_PHY_RX_FLOW_CTRL>. For symmetric
* PAUSE ability (MII), user can set/clean both flags together. For
* asymmetric PAUSE ability (GMII), user can seperate transmit and receive
* flow control ability. However, user should be aware that flow control
* ability is meaningful only if full duplex mode is used.
*
* When <MII_PHY_TBL> is set in the user flags, the BSP specific 
* table whose address may be provided in the <phyAnOrderTbl>
* field of the <phyInfo> structure, is used to obtain the list, and 
* the order of technology abilities to be negotiated.
* The entries in this table are ordered such that entry 0 is the
* highest priority, entry 1 in next and so on. Entries in this table 
* may be repeated, and multiple technology abilities can
* be OR'd to create a single  entry. If a PHY cannot support a
* ability in an entry, that entry is ignored. 
*
* If no PHY provides a valid link, and if <MII_PHY_DEF_SET> is set in the
* phyFlags field of the PHY_INFO structure, the first PHY that supports 
* the default abilities defined in the <phyDefMode> of the phyInfo structure 
* will be selected, regardless of the link status.
*
* In addition, this routine adds an entry in a linked list of PHY devices
* for each active PHY it found. If the flag <MII_PHY_MONITOR> is set, the 
* link status for the relevant PHY is continually monitored for a link down 
* event. If such event is detected, and if the <phyLinkDownRtn> in the PHY_INFO * structure is a valid function pointer, then the routine it points at is 
* executed in the context of the netTask ().  The parameter MII_MONITOR_DELAY
* may be used to define the period in seconds with which the link status is 
* checked. Its default value is 5.
*
* RETURNS: OK or ERROR if the PHY could not be initialised,
*
*/

STATUS miiPhyInit
    (
    PHY_INFO *  pPhyInfo       /* pointer to PHY_INFO structure */
    )
    {
    int		retVal;		/* convenient holder for return value */

    /* sanity checks */

    if (pPhyInfo == NULL ||
	(pPhyInfo->phyReadRtn == NULL) ||
	(pPhyInfo->phyWriteRtn == NULL) ||
	(pPhyInfo->pDrvCtrl == NULL))
	{
	MII_LOG (MII_DBG_ANY, ("miiPhyInit... missing params \n"),
			       0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    /* Ensure that pPhyInfo->phyMaxDelay is minimum 5 seconds */

    if (pPhyInfo->phyMaxDelay < (sysClkRateGet() * 5))
        {
        /* Set max delay porportionally */
 
        pPhyInfo->phyMaxDelay = sysClkRateGet() * 5;
        }


    /* initialize the MII library  */

    if (miiLibInit () == ERROR)
        return (ERROR);

#ifdef MII_DBG
    phyInfoDbg = pPhyInfo;
#endif /* MII_DBG */

    /* scan the whole bus */

    if (miiPhyBusScan (pPhyInfo) == ERROR)
	return (ERROR);

    /* set the mode for a PHY, return in case of success or fatal error */

    retVal = miiPhyLinkSet (pPhyInfo);
    if (retVal == OK)
	return (OK);

    if (errno != S_miiLib_PHY_LINK_DOWN)
	return (ERROR);

    /* if we're here, none of the PHYs could be initialized */

    MII_PHY_CHECK_CABLE;

    if (!(MII_PHY_FLAGS_ARE_SET (MII_PHY_DEF_SET)))
	    return (ERROR);

    /* set the default mode for a PHY, do not check the link */

    return (miiPhyDefModeSet (pPhyInfo));
    } 

/**************************************************************************
*
* miiPhyUnInit - uninitialize a PHY
*
* This routine uninitializes the PHY specified in <pPhyInfo>. It brings it
* in low-power mode, and electrically isolate it from the MII management 
* interface to which it is attached. In addition, it frees resources 
* previously allocated.
*
* RETURNS: OK, ERROR in case of fatal errors.
*
*/

STATUS miiPhyUnInit
    (
    PHY_INFO *  pPhyInfo       /* pointer to PHY_INFO structure */
    )
    {
    if (pPhyInfo == NULL)
	return (ERROR);

    MII_LOG (MII_DBG_ANY, ("miiPhyUnInit \n"),
			   0, 0, 0, 0, 0, 0);

    /* this counts also for isolate */
    if (miiProbe (pPhyInfo, pPhyInfo->phyAddr) == ERROR)
	return (ERROR);

    if (miiPhyPwrDown (pPhyInfo, pPhyInfo->phyAddr) == ERROR)
	return (ERROR);

    MII_PHY_FLAGS_CLEAR (MII_PHY_INIT);

    /* protect it from other MII routines */

    MII_SEM_TAKE (WAIT_FOREVER);

    lstDelete (&miiList, (NODE *) pPhyInfo->pMiiPhyNode);

    /* we're done, release the mutex */
    MII_SEM_GIVE ();

    /* free memory */
    free ((char *) pPhyInfo->pMiiPhyNode);
    free ((char *) pPhyInfo->miiPhyPresent);

    pPhyInfo->pMiiPhyNode = NULL;

    /* stop the monitor if necessary */
    if (miiList.count == 0)
	return (wdCancel (miiWd));

    return (OK);
    }

/**************************************************************************
*
* miiProbe - probe the PHY device
*
* This routine probes the PHY device by reading its control register.
* It also checks the PHY can be successfully electrically isolated from its
* MII interface.
*
* RETURNS: OK, ERROR in case of fatal errors.
*
* ERRNO: S_miiLib_PHY_NULL
*
*/

LOCAL STATUS miiProbe
    (
    PHY_INFO *  pPhyInfo,       /* pointer to PHY_INFO structure */
    UINT8	phyAddr		/* the PHY being read */
    )
    {
    UINT8	regAddr;	/* the PHY's register being read */
    UINT16	data;		/* data to be written to the PHY's reg */
    int		retVal;		/* convenient holder for return value */

    MII_LOG (MII_DBG_ANY, "miiProbe for addr %#x starts\n",phyAddr,0,0,0,0,0);

    regAddr = MII_CTRL_REG;
    MII_READ (phyAddr, regAddr, &data, retVal);

    if (retVal == ERROR)
	return (ERROR);

    /* check the reserved bits, and the ability to isolate the PHY */

    MII_LOG (MII_DBG_ANY, "miiProbe checking addr %#x\n",phyAddr,0,0,0,0,0);

    if (((data & MII_CR_RES_MASK) > 0x0) ||
        ((miiPhyIsolate (pPhyInfo, phyAddr) != OK)))
        {
	errnoSet (S_miiLib_PHY_NULL); 
	return (ERROR);
	}

    MII_LOG (MII_DBG_ANY, ("miiProbe... ends\n"),
			   0, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* miiDiag - run some diagnostics
*
* This routine runs some diagnostics on the PHY whose address is 
* specified in the parameter <phyAddr>.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS miiDiag
    (
    PHY_INFO *  pPhyInfo,       /* pointer to PHY_INFO structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT16	data;		/* data to be written to the control reg */
    UINT8	regAddr;	/* PHY register */
    UINT16	ix = 0;		/*  a counter */
    int		retVal;		/* convenient holder for return value */

    /* reset the PHY */

    regAddr = MII_CTRL_REG;
    data = MII_CR_RESET;

    MII_WRITE (phyAddr, regAddr, data, retVal);
    if (retVal != OK)
	return (ERROR);

    while (ix++ < pPhyInfo->phyMaxDelay)
	{
	MII_SYS_DELAY (pPhyInfo->phyDelayParm);

	MII_READ (phyAddr, regAddr, &data, retVal);
	if (retVal == ERROR)
	    return (ERROR);

	if (!(data & MII_CR_RESET))
	    break;
	}

    if (ix >= pPhyInfo->phyMaxDelay)
	{
	MII_LOG (MII_DBG_ANY, ("miiDiag reset fail\n"),
			       0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    /* re-enable the chip */

    data = MII_CR_NORM_EN;

    MII_WRITE (phyAddr, regAddr, data, retVal);

    if (retVal != OK)
	{
	MII_LOG (MII_DBG_ANY, ("miiDiag write fail\n"),
			       0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    /* check isolate bit is deasserted */

    ix = 0;
    while (ix++ < pPhyInfo->phyMaxDelay)
	{
	MII_SYS_DELAY (pPhyInfo->phyDelayParm);

	MII_READ (phyAddr, regAddr, &data, retVal);

	if (retVal != OK)
	    {
	    MII_LOG (MII_DBG_ANY, ("miiDiag read fail\n"),
				   0, 0, 0, 0, 0, 0);
	    return (ERROR);
	    }

	if (!(data & MII_CR_ISOLATE))
	    break;
	}

    if (ix >= pPhyInfo->phyMaxDelay)
	{
	MII_LOG (MII_DBG_ANY, ("miiDiag de-isolate fail\n"),
			       0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    MII_LOG (MII_DBG_ANY, ("miiDiag... ends\n"),
			   0, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* miiPhyIsolate - isolate the PHY device
*
* This routine isolates the PHY device whose address is specified in 
* the parameter <isoPhyAddr>.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS miiPhyIsolate
    (
    PHY_INFO *  pPhyInfo,       /* pointer to PHY_INFO structure */
    UINT8	isoPhyAddr	/* address of a PHY to be isolated */
    )
    {
    UINT8	regAddr;	/* PHY register */
    UINT16	ix = 0;		/*  a counter */
    UINT16	data;		/* data to be written to the control reg */
    int		retVal;		/* convenient holder for return value */

    if (isoPhyAddr == MII_PHY_NULL)
	return (OK);

    data = MII_CR_ISOLATE;
    regAddr = MII_CTRL_REG;

    MII_WRITE (isoPhyAddr, regAddr, data, retVal);
    if (retVal != OK)
	return (ERROR);

    /* check isolate bit is asserted */

    while (ix++ < pPhyInfo->phyMaxDelay)
	{
	MII_SYS_DELAY (pPhyInfo->phyDelayParm);

	MII_READ (isoPhyAddr, regAddr, &data, retVal);
	if (retVal != OK)
	    return (ERROR);

	if ((data & MII_CR_ISOLATE))
	    break;
	}

    if (ix >= pPhyInfo->phyMaxDelay)
	{
	MII_LOG (MII_DBG_ANY, ("miiPhyIsolate fail\n"),
			       0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    MII_LOG (MII_DBG_ANY, ("miiPhyIsolate... ends\n"),
			   0, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* miiPhyPwrDown - put the PHY in power down mode
*
* This routine puts the PHY specified in <phyAddr> in power down mode.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS miiPhyPwrDown
    (
    PHY_INFO *  pPhyInfo,      	/* pointer to PHY_INFO structure */
    UINT8 	phyAddr		/* phy to be put in power down mode */
    )
    {
    int		retVal;		/* convenient holder for return value */
    UINT8	regAddr;	/* PHY register */
    UINT16	data;		/* data to be written to the control reg */

    if (phyAddr == MII_PHY_NULL)
	return (OK);

    data = MII_CR_POWER_DOWN;
    regAddr = MII_CTRL_REG;

    MII_WRITE (phyAddr, regAddr, data, retVal);

    return (retVal);
    }

/*******************************************************************************
*
* miiPhyBusScan - scan the MII bus
*
* This routine scans the MII bus, in the search for an MII-compliant PHY.
* If it succeeds, and if the flag MII_ALL_BUS_SCAN is not set, it returns
* OK. Otherwise, it keeps scanning the bus and storing relevant information
* in the miiPhyPresent field of the structure referenced to by <pPhyInfo>.
* Other PHYs than the first one found, will be put in
* low-power mode and electrically isolated from the MII interface.
*
* SEE ALSO: miiAutoNegotiate (), miiModeForce ().
* 
* RETURNS: OK or ERROR.
*
*/

LOCAL STATUS miiPhyBusScan
    (
    PHY_INFO *  pPhyInfo        /* pointer to PHY_INFO structure */
    )
    {
    UINT16	ix;		/* a counter */
    int		retVal;		/* holder for return value */
    UINT8	phyAddr;	/* address of a PHY */

    if ((pPhyInfo->miiPhyPresent = (MII_PHY_BUS *) 
				   calloc (1, sizeof (MII_PHY_BUS)))
	== NULL)
	return (ERROR);

    /* 
     * there may be as many as 32 PHYs, with distinct logical addresses
     * Start with the one the user suggested and, in case of failure in 
     * probing the device, scan the whole range.
     */

    for (ix = 0; ix < MII_MAX_PHY_NUM; ix++)
	{
	phyAddr = (ix + pPhyInfo->phyAddr) % MII_MAX_PHY_NUM;

	MII_LOG (MII_DBG_ANY,
		      ("miiPhyBusScan trying phyAddr=0x%x phyInfo=0x%x \n"),
		      phyAddr, pPhyInfo, 0, 0, 0, 0);

	/* check the PHY is there */

	retVal = miiProbe (pPhyInfo, phyAddr);

	/* deal with the error condition if possible */

	if (retVal == ERROR) 
	    {
	    if (errno != S_miiLib_PHY_NULL) 
		return (ERROR);
	    else
		{
		if (ix == (MII_MAX_PHY_NUM - 1))
		    return (ERROR);

		/* no PHY was found with this address: keep scanning */

		MII_LOG (MII_DBG_ANY, "No PHY at address %#x\n", phyAddr,
			 0,0,0,0,0);

		errno = 0;
		continue;
		}
	    }
	else
	    {
	    /* run some diagnostics */

	    if (miiDiag (pPhyInfo, phyAddr) != OK)
		{
		MII_LOG (MII_DBG_ANY, "miiDiag failed for addr %#x\n",
			 phyAddr,0,0,0,0,0);
		return (ERROR);
		}

	    /* record this information */

	    * ((BOOL *) pPhyInfo->miiPhyPresent + phyAddr) = TRUE;

	    /* should we scan the whole bus? */

	    if (!(MII_PHY_FLAGS_ARE_SET (MII_ALL_BUS_SCAN))) 
		{
		MII_LOG (MII_DBG_ANY, "Not scanning whole bus\n", 0,0,0,0,0,0);
		return (OK);
		}

	    MII_LOG (MII_DBG_ANY, ("miiPhyBusScan phyAddr=0x%x \n"),
				   phyAddr, 0, 0, 0, 0, 0);
	    }
	}

    /* set optional features for the other PHYs */

    for (ix = 0; ix < MII_MAX_PHY_NUM; ix++)
	{
	/* check the PHY is there */

	if (* ((BOOL *) pPhyInfo->miiPhyPresent + phyAddr) == TRUE)
	    {
	    /* set it in power down mode */

	    if (MII_PHY_FLAGS_ARE_SET (MII_PHY_PWR_DOWN))
		{
		MII_LOG (MII_DBG_ANY, "Powering down PHY %#x\n", ix,
			 0,0,0,0,0);
		if (miiPhyPwrDown (pPhyInfo, ix) == ERROR)
		    return (ERROR);
		}

	    /* eletrically isolate it from the MII interface */

	    if (MII_PHY_FLAGS_ARE_SET (MII_PHY_ISO))
		{
		MII_LOG (MII_DBG_ANY, "miiPhyBusScan isolating phyAddr=0x%x\n",
				       ix, 0, 0, 0, 0, 0);
		if (miiPhyIsolate (pPhyInfo, ix) == ERROR)
		    return (ERROR);
		}
	    }

	}

    return (OK);
    }

/*******************************************************************************
*
* miiPhyBestModeSet - set the best operating mode for a PHY
*
* This routine sets the best operating mode for a PHY looking at the 
* parameters in <pPhyInfo>. It may call miiAutoNegotiate (), and/or 
* miiModeForce (). Upon success, it stores the <phyAddr> in the relevant
* field of the structure pointed to by <pPhyInfo>.
*
* SEE ALSO: miiAutoNegotiate (), miiModeForce ().
* 
* RETURNS: OK or ERROR.
*
*/

LOCAL STATUS miiPhyBestModeSet
    (
    PHY_INFO *  pPhyInfo,       /* pointer to PHY_INFO structure */
    UINT8 	phyAddr         /* PHY's address */
    )
    {
    /* 
     * start the auto-negotiation process,
     * unless the user dictated the contrary. 
     */

    if (pPhyInfo->phyFlags & MII_PHY_AUTO)
	if (miiAutoNegotiate (pPhyInfo, phyAddr) == OK)
	    {
            if (pPhyInfo->phyFlags & MII_PHY_GMII_TYPE)
                {
                pPhyInfo->phyLinkMethod = MII_PHY_LINK_AUTO;
	        }
            goto miiPhyOk;
            }

    /* 
     * the auto-negotiation process did not succeed 
     * in establishing a valid link: try to do it
     * manually, enabling as many high priority abilities
     * as possible.
     */

    if (miiModeForce (pPhyInfo, phyAddr) == OK)
        {
        if (pPhyInfo->phyFlags & MII_PHY_GMII_TYPE)
            {
            pPhyInfo->phyLinkMethod = MII_PHY_LINK_FORCE;
            }
         goto miiPhyOk;
         }

    return (ERROR);

miiPhyOk:
    /* store this PHY and return */

    pPhyInfo->phyAddr = phyAddr;
    MII_PHY_FLAGS_SET (MII_PHY_INIT);
    return (OK);
    }

/*******************************************************************************
*
* miiPhyLinkSet - set the link for a PHY
*
* This routine sets the link for the PHY pointed to by <pPhyInfo>. To do
* so, it calls miiPhyBestModeSet (). Upon success it returns OK.
* Otherwise, it checks whether other PHYs are on the bus, and attempts at
* establishing a link for them starting from the first and scanning the 
* whole range. In case of failure, ERROR is returned.
*
* SEE ALSO: miiPhyBestModeSet ().
* 
* RETURNS: OK or ERROR.
*
* ERRNO: S_miiLib_PHY_LINK_DOWN
*
*/

LOCAL STATUS miiPhyLinkSet
    (
    PHY_INFO *  pPhyInfo        /* pointer to PHY_INFO structure */
    )
    {
    UINT16	ix;		/* a counter */
    UINT8	phyAddr;	/* address of a PHY */
    UINT32	phyFlags;	/* default PHY's flags */

    /* store the default phy's flags */

    phyFlags = pPhyInfo->phyFlags;

    for (ix = 0; ix < MII_MAX_PHY_NUM; ix++)
	{
	phyAddr = (ix + pPhyInfo->phyAddr) % MII_MAX_PHY_NUM;

	MII_LOG (MII_DBG_ANY, ("miiPhyLinkSet phyAddr=0x%x \n"),
			       phyAddr, 0, 0, 0, 0, 0);

	if (* ((BOOL *) pPhyInfo->miiPhyPresent + phyAddr) == FALSE)
	    continue;

	/* add this PHY to the linked list */

	if (miiPhyListAdd (pPhyInfo) == ERROR)
	    return (ERROR);

	MII_LOG (MII_DBG_ANY, ("miiPhyLinkSet phyAddr=0x%x pres=0x%x \n"),
			       phyAddr, 
			       (* ((BOOL *) pPhyInfo->miiPhyPresent + phyAddr)),
			       0, 0, 0, 0);

	/* find out the PHY's abilities and set flags accordingly */

	if (miiAbilFlagSet (pPhyInfo, phyAddr) != OK)
	    return (ERROR);

        /* we need this phyAddr before we call pPhyOptRegsRtn */

        pPhyInfo->phyAddr = phyAddr;

	/* 
	 * check with the BSP if we need to do something else before
	 * we try and establish the link 
	 */

        if (pPhyInfo->pPhyOptRegsRtn != NULL)
            (* pPhyInfo->pPhyOptRegsRtn) (pPhyInfo);
        else
	    if (pPhyOptRegsRtn != NULL)
	        (* pPhyOptRegsRtn) (pPhyInfo);

        /* initialize phyLinkMethod */

        if (pPhyInfo->phyFlags & MII_PHY_GMII_TYPE)
            {
            pPhyInfo->phyLinkMethod = MII_PHY_LINK_UNKNOWN;
            } 

	/* establist the link */

	if (miiPhyBestModeSet (pPhyInfo, phyAddr) == OK)
	    {
	    return (OK);
	    }

	/* restore the default flags, miiAbilFlagSet may have changed them */

	pPhyInfo->phyFlags = phyFlags;
	}

    /* set errno, since we did not establish a valid link */

    errnoSet (S_miiLib_PHY_LINK_DOWN);

    return (ERROR);
    }

/*******************************************************************************
*
* miiPhyDefModeSet - set the default operating mode for a PHY
*
* This routine sets the default operating mode for a PHY looking at the 
* parameters in <pPhyInfo>. It calls miiDefForce () for each PHY found.
*
* SEE ALSO: miiDefForce ().
* 
* RETURNS: OK or ERROR.
*
*/

LOCAL STATUS miiPhyDefModeSet
    (
    PHY_INFO *  pPhyInfo        /* pointer to PHY_INFO structure */
    )
    {
    UINT16	ix;		/* a counter */
    int		retVal;		/* holder for return value */
    UINT8	phyAddr;	/* address of a PHY */

    /* try to establish a default operating mode, do not check the link! */

    for (ix = 0; ix < MII_MAX_PHY_NUM; ix++)
	{
	phyAddr = (ix + pPhyInfo->phyAddr) % MII_MAX_PHY_NUM;

	/* check the PHY is there */

	if (* ((BOOL *) pPhyInfo->miiPhyPresent + phyAddr) == FALSE)
	    continue;

	/* 
	 * Force default parameters: field phyDefMode in the PHY info 
	 * structure. Return OK even if the link is not up.
	 */

	retVal = miiDefForce (pPhyInfo, phyAddr);
	if (retVal == OK) 
	    {
	    /* store this PHY and return */

	    pPhyInfo->phyAddr = phyAddr;
	    MII_PHY_FLAGS_SET (MII_PHY_INIT);

	    return (OK);
	    }

	/* if the PHY does not have the default abilities... */

	if (errno == S_miiLib_PHY_NO_ABLE)
	    {
	    if (ix == (MII_MAX_PHY_NUM - 1))
		return (ERROR);

	    errno = 0;
	    continue;
	    }
	}

    return (ERROR);
    }

/*******************************************************************************
*
* miiAutoNegotiate - run the auto-negotiation process
*
* This routine runs the auto-negotiation process for the PHY whose
* address is specified in the parameter <phyAddr>, without enabling the 
* next page function. It also calls miiAnCheck() to ensure
* a valid link has been established.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS miiAutoNegotiate
    (
    PHY_INFO *  pPhyInfo,  /* pointer to PHY_INFO structure */
    UINT8	phyAddr        /* address of a PHY */
    )
    {
    UINT8	regAddr;       /* PHY register */
    UINT16	phyAds;        /* holder for the PHY ads register value */
    UINT16	status;        /* PHY auto-negotiation status */
    UINT16	ix;            /* a counter */
    int		retVal;        /* holder for return value */
    UINT16	phyStat;       /* PHY auto-negotiation status */
    UINT16  phyMstSlaCtrl; /* PHY Mater-slave Control */
    UINT16  phyExtStat;    /* PHY extented status */

    /* save phyFlags for phyAutoNegotiateFlags */

    if (pPhyInfo->phyFlags & MII_PHY_GMII_TYPE)
        {
        pPhyInfo->phyAutoNegotiateFlags = pPhyInfo->phyFlags; 
        }

    /* Read ANER to clear status from previous operations */
  
    regAddr = MII_AN_EXP_REG;
    MII_READ (phyAddr, regAddr, &status, retVal);
    if (retVal != OK)
	return (ERROR);

    /* 
     * copy the abilities in ANSR to ANAR. This is necessary because
     * according to the 802.3 standard, the technology ability
     * field in ANAR "is set based on the value in the MII status
     * register or equivalent". What does "equivalent" mean?
     */
 
    regAddr = MII_AN_ADS_REG;

    MII_READ (phyAddr, regAddr, &phyAds, retVal);
    if (retVal != OK)
	return (ERROR);

    MII_LOG (MII_DBG_ANY, ("miiAutoNegotiate phyAds=0x%x expStat=0x%x \n"),
			   phyAds, status, 0, 0, 0, 0);

    regAddr = MII_STAT_REG;
    MII_READ (phyAddr, regAddr, &phyStat, retVal);
    if (retVal != OK)
        return (ERROR);
 
    MII_FROM_ANSR_TO_ANAR (phyStat, phyAds);

    /* also disable the next page function */

    phyAds &= (~MII_NP_NP);

    MII_LOG (MII_DBG_ANY, ("miiAutoNegotiate write to ANAR=0x%x\n"),
                           phyAds, 0, 0, 0, 0, 0);
 
    /* configure flow control */

    /* MII defines symmetric PAUSE ability */ 

    if ((MII_PHY_FLAGS_ARE_SET (MII_PHY_TX_FLOW_CTRL)) &&
                (MII_PHY_FLAGS_ARE_SET (MII_PHY_RX_FLOW_CTRL)))
        phyAds |= MII_ANAR_PAUSE;
    else
        phyAds &= ~MII_ANAR_PAUSE;

    if (pPhyInfo->phyFlags & MII_PHY_GMII_TYPE)
        {

        /* GMII also defines asymmetric PAUSE ability */

        if (!(MII_PHY_FLAGS_ARE_SET (MII_PHY_TX_FLOW_CTRL)) &&
                !(MII_PHY_FLAGS_ARE_SET (MII_PHY_RX_FLOW_CTRL)))
             {
             /* not flow control */

             phyAds &= ~MII_ANAR_ASM_PAUSE;
             phyAds &= ~MII_ANAR_PAUSE;
             }
         else if ((MII_PHY_FLAGS_ARE_SET (MII_PHY_TX_FLOW_CTRL)) && 
                    !(MII_PHY_FLAGS_ARE_SET (MII_PHY_RX_FLOW_CTRL)))
             {
             /* TX flow control */

             phyAds |= MII_ANAR_ASM_PAUSE;
             phyAds &= ~MII_ANAR_PAUSE;
             }
         else    
             {
             /* RX flow control */

             phyAds |= MII_ANAR_ASM_PAUSE;
             phyAds |= MII_ANAR_PAUSE;
             }
         }

    /* write ANAR */

    regAddr = MII_AN_ADS_REG;
    MII_WRITE (phyAddr, regAddr, phyAds, retVal);
    if (retVal != OK)
	return (ERROR);
 
    /* store the current registers values */

    pPhyInfo->miiRegs.phyAds = phyAds;
    
    if (pPhyInfo->phyFlags & MII_PHY_GMII_TYPE)
        {
        /* get Master-Slave Control (MSC) Register */

        regAddr =  MII_MASSLA_CTRL_REG;
        MII_READ (phyAddr, regAddr, &phyMstSlaCtrl, retVal);
        if (retVal != OK)
            return (ERROR);

        /* get extended status register */

        regAddr = MII_EXT_STAT_REG;
        MII_READ (phyAddr, regAddr, &phyExtStat, retVal);
        if (retVal != OK)
            return (ERROR);
     
        /* set MSC register value based on PHY ability on EXTS */
   
        phyMstSlaCtrl = (phyExtStat & MII_EXT_STAT_1000T_HD) ?
                        (phyMstSlaCtrl | MII_MASSLA_CTRL_1000T_HD) :
                        (phyMstSlaCtrl & ~MII_MASSLA_CTRL_1000T_HD);

        phyMstSlaCtrl = (phyExtStat & MII_EXT_STAT_1000T_FD) ?
                        (phyMstSlaCtrl | MII_MASSLA_CTRL_1000T_FD) :
                        (phyMstSlaCtrl & ~MII_MASSLA_CTRL_1000T_FD);                         
          
        /* write MSC register value */

        regAddr = MII_MASSLA_CTRL_REG;
        MII_WRITE (phyAddr, regAddr, phyMstSlaCtrl, retVal);
        if (retVal != OK)
            return (ERROR);

        /* store the current registers values */

        pPhyInfo->miiGRegs.phyMSCtrl = phyMstSlaCtrl;
        }

    /* 
     * start the auto-negotiation process, possibly
     * following the order the user dictated.
     */

    for (ix = 0; ; ix++)
	{
	if (MII_PHY_FLAGS_ARE_SET (MII_PHY_TBL))
	    {
	    if (pPhyInfo->phyAnOrderTbl == NULL)
		{
		MII_LOG (MII_DBG_ANY, ("miiAutoNegotiate no auto-neg table\n"),
				       0, 0, 0, 0, 0, 0);

		return (ERROR);
		}

	    /* check we're not at the end of the user table */

	    if (*((INT16*) pPhyInfo->phyAnOrderTbl + ix) == -1)
		return (ERROR);

	    /* just negotiate one ability at a time */

	    phyAds &= ~MII_ADS_TECH_MASK;

	    /* translate user settings */

	    phyAds |= *((INT16*) pPhyInfo->phyAnOrderTbl + ix);

	    /* check the PHY has the desidered ability */

	    if (!(phyAds & pPhyInfo->miiRegs.phyAds))
		continue;
	    }
	else
	    {
	    /* check the PHY flags and possibly mask some abilities off */

	    if (!(MII_PHY_FLAGS_ARE_SET (MII_PHY_FD)))
		phyAds &= ~(MII_TECH_10BASE_FD | MII_TECH_100BASE_TX_FD);

	    if (!(MII_PHY_FLAGS_ARE_SET (MII_PHY_HD)))
		phyAds &= ~(MII_TECH_10BASE_T | MII_TECH_100BASE_TX 
			    | MII_TECH_100BASE_T4);

	    if (!(MII_PHY_FLAGS_ARE_SET (MII_PHY_100)))
		phyAds &= ~(MII_TECH_100BASE_TX | MII_TECH_100BASE_TX_FD
			    | MII_TECH_100BASE_T4);

	    if (!(MII_PHY_FLAGS_ARE_SET (MII_PHY_10)))
		phyAds &= ~(MII_TECH_10BASE_T | MII_TECH_10BASE_FD);

	    /* store the current registers values */

	    pPhyInfo->miiRegs.phyAds = phyAds;

	    if (pPhyInfo->phyFlags & MII_PHY_GMII_TYPE)
	        {

	        /* check phyFlags with 1000T FD mode */

	        if (!(MII_PHY_FLAGS_ARE_SET (MII_PHY_1000T_FD)))
	            phyMstSlaCtrl &= ~MII_MASSLA_CTRL_1000T_FD;

	        /* check phyFlags with 1000T HD mode */

	        if (!(MII_PHY_FLAGS_ARE_SET (MII_PHY_1000T_HD)))
	            phyMstSlaCtrl &= ~MII_MASSLA_CTRL_1000T_HD;

	       /* store the current registers values */

	        pPhyInfo->miiGRegs.phyMSCtrl = phyMstSlaCtrl;;

	        MII_LOG (MII_DBG_ANY, ("miiAutoNegotiate phyMSCtrl=0x%x\n"),
					   phyMstSlaCtrl, 0, 0, 0, 0, 0);
	        }

	    MII_LOG (MII_DBG_ANY, ("miiAutoNegotiate phyAds=0x%x\n"),
				   phyAds, 0, 0, 0, 0, 0);
	    }

	/* set the ANAR accordingly */

	regAddr = MII_AN_ADS_REG;

	MII_WRITE (phyAddr, regAddr, phyAds, retVal);
	if (retVal != OK)
	    return (ERROR);

	    MII_LOG (MII_DBG_ANY, ("miiAutoNegotiate phyAds=0x%x\n"),
				   phyAds, 0, 0, 0, 0, 0);

    /* set MSC register accordingly */

    if (pPhyInfo->phyFlags & MII_PHY_GMII_TYPE)
        {
        regAddr = MII_MASSLA_CTRL_REG;
        MII_WRITE (phyAddr, regAddr, phyMstSlaCtrl, retVal);
        if (retVal != OK)
            return (ERROR);
        }

	/* 
	 * start the auto-negotiation process: return
	 * only in case of fatal error.
	 */

	retVal = miiAutoNegStart (pPhyInfo, phyAddr);

	/* 
	 * in case of fatal error, we return immediately; otherwise,
	 * we try to recover from the failure, if we're not using 
	 * the standard auto-negotiation process.
	 */

	if (retVal == ERROR)
	    {
	    if (errno != S_miiLib_PHY_AN_FAIL)
		return (ERROR);

	    else
		{
		if (!(MII_PHY_FLAGS_ARE_SET (MII_PHY_TBL)))
		    return (ERROR);
		else
		    {
		    /* let's try the next entry in the table */

		    errno = 0;
		    continue;
		    }
		}
	    }

	/* check the negotiation was successful */

	if (miiAnCheck (pPhyInfo, phyAddr) == OK)
	    return (OK);

	/* 
	 * we are here if the negotiation went wong:
	 * if the auto-negotiation order table was not 
	 * used, we return ERROR, as all the PHY abilities
	 * were negotiated at once.
	 */

	if (!(MII_PHY_FLAGS_ARE_SET (MII_PHY_TBL)))
	    return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* miiAutoNegStart - start the auto-negotiation process
*
* This routine starts the auto-negotiation process for the PHY whose
* address is specified in the parameter <phyAddr>.
*
* RETURNS: OK or ERROR.
*
* ERRNO: S_miiLib_PHY_AN_FAIL
*
*/

LOCAL STATUS miiAutoNegStart
    (
    PHY_INFO *  pPhyInfo,       /* pointer to PHY_INFO structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT16	data;		/* data to be written to the control reg */
    UINT8	regAddr;	/* PHY register */
    UINT16	phyStatus;	/* holder for the PHY status */
    UINT16	ix = 0;		/* a counter */
    int		retVal;		/* convenient holder for return value */

    regAddr = MII_STAT_REG;

    MII_READ (phyAddr, regAddr, &phyStatus, retVal);
    if (retVal != OK)
	return (ERROR);

    MII_LOG (MII_DBG_ANY, ("miiAutoNegStart phy=0x%x reg=0x%x status=0x%x\n"),
			   phyAddr, regAddr, phyStatus, 0, 0, 0);

    /* check the PHY has this ability */

    if ((phyStatus & MII_SR_AUTO_SEL) != MII_SR_AUTO_SEL)
	{
	MII_LOG (MII_DBG_ANY, ("miiAutoNegStart phy not 
			       auto neg capable\n"), 
			       0, 0, 0, 0, 0, 0);

	return (ERROR);
	}

    /* restart the auto-negotiation process */

    regAddr = MII_CTRL_REG;
    data = (MII_CR_RESTART | MII_CR_AUTO_EN);

    MII_WRITE (phyAddr, regAddr, data, retVal);
    if (retVal != OK)
	return (ERROR);

    /* save status info */

    pPhyInfo->miiRegs.phyCtrl = data;

    /* let's check the PHY status for completion */

    regAddr = MII_STAT_REG;

    /* spin until it is done */

    do
	{
	MII_SYS_DELAY (pPhyInfo->phyDelayParm);

	if (ix++ == pPhyInfo->phyMaxDelay)
	    break;

	MII_READ (phyAddr, regAddr, &phyStatus, retVal);
	if (retVal != OK)
	    return (ERROR);

	} while ((phyStatus & MII_SR_AUTO_NEG) != MII_SR_AUTO_NEG);

    MII_LOG (MII_DBG_ANY, ("miiAutoNegStart auto neg attempts=%d \n"),
			   ix, 0, 0, 0, 0, 0);

    if (ix >= pPhyInfo->phyMaxDelay)
	{
	MII_LOG (MII_DBG_ANY, ("miiAutoNegStart auto neg fail\n"),
			       0, 0, 0, 0, 0, 0);
	errnoSet (S_miiLib_PHY_AN_FAIL);
	return (ERROR);
	}
    else
	{
	MII_LOG (MII_DBG_ANY, ("miiAutoNegStart auto neg done phyStat=0x%x\n"),
			       phyStatus, 0, 0, 0, 0, 0);
	}

    return (OK);
    }

/*******************************************************************************
*
* miiBasicCheck - run a basic check on the PHY status register
*
* This routine runs a basic check on the PHY status register to
* ensure a valid link has been established and no faults have been 
* detected.
*
* RETURNS: OK or ERROR.
*
*/

LOCAL STATUS miiBasicCheck
    (
    PHY_INFO *  pPhyInfo,       /* pointer to PHY_INFO structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT8	regAddr;	/* PHY register */
    UINT16	phyStatus;	/* holder for the PHY status */
    UINT16	ix = 0;		/* a counter */
    int		retVal;		/* convenient holder for return value */
    /* let's check the PHY status for completion */

    regAddr = MII_STAT_REG;

    /* spin until it is done */

    do
	{
 	MII_SYS_DELAY (sysClkRateGet());
        ix += sysClkRateGet(); 

	if (ix >= pPhyInfo->phyMaxDelay)
	    break;

	MII_READ (phyAddr, regAddr, &phyStatus, retVal); 
	   if (retVal != OK)
	    return (ERROR);

	} while ((phyStatus & MII_SR_LINK_STATUS) != MII_SR_LINK_STATUS); 

    if (ix >= pPhyInfo->phyMaxDelay)
	{
	MII_LOG (MII_DBG_ANY, ("miiBasicCheck fail\n"),
			       0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    MII_LOG (MII_DBG_ANY, ("miiBasicCheck Link up! status=0x%x\n"),
			   phyStatus, 0, 0, 0, 0, 0);

    /* check for remote fault condition, read twice */

    MII_READ (phyAddr, regAddr, &phyStatus, retVal);
    if (retVal != OK)
	return (ERROR);

    MII_READ (phyAddr, regAddr, &phyStatus, retVal);
    if (retVal != OK)
	return (ERROR);

    if ((phyStatus & MII_SR_REMOTE_FAULT) == MII_SR_REMOTE_FAULT)
	{
	MII_LOG (MII_DBG_ANY, ("miiBasicCheck remote fault\n"),
			       0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    /* store the current registers values */

    pPhyInfo->miiRegs.phyStatus = phyStatus;

    return (OK);
    }

/*******************************************************************************
*
* miiFlagsHandle - handle some flags
*
* This routine sets some fields in the PHY_INFO structure according to the
* values of the related flags.
*
* RETURNS: OK, always.
*/

LOCAL STATUS miiFlagsHandle
    (
    PHY_INFO *  pPhyInfo,       /* pointer to PHY_INFO structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {

    /* check speed ... */ 

    if (MII_PHY_FLAGS_ARE_SET (MII_PHY_1000T_FD) || 
        MII_PHY_FLAGS_ARE_SET (MII_PHY_1000T_HD))
        pPhyInfo->phySpeed = MII_1000MBS;

    else if (MII_PHY_FLAGS_ARE_SET (MII_PHY_100))
	pPhyInfo->phySpeed = MII_100MBS;
    else
	pPhyInfo->phySpeed = MII_10MBS;
 

    if (MII_PHY_FLAGS_ARE_SET (MII_PHY_FD))
        {
        bcopy (MII_FDX_STR, (char *) pPhyInfo->phyMode, MII_FDX_LEN);
        }
    else
        {
        bcopy (MII_HDX_STR, (char *) pPhyInfo->phyMode, MII_HDX_LEN);
        }

    MII_LOG (MII_DBG_ANY, ("miiFlagsHandle speed=%d mode=%s\n"),
			   pPhyInfo->phySpeed, 
			   (char *) pPhyInfo->phyMode, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* miiPhyUpdate - update the phyInfo structure to the latest PHY status
*
* This routine updates the phyInfo structure to the latest PHY status.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS miiPhyUpdate
    (
    PHY_INFO *  pPhyInfo,       /* pointer to PHY_INFO structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT16	phyStatus;	/* holder for the PHY status */
    UINT16	phyAds;		/* PHY advertisement register value */
    UINT16	phyPrtn;	/* PHY partner ability register value */
    UINT16	phyExp;		/* PHY expansion register value */
    UINT16  phyMstSlaCtrl; /* PHY Master-slave Control value */
    UINT16  phyMstSlaStat; /* PHY Master-slave Status value */
    UINT16	negAbility;	/* abilities after negotiation */
    int		retVal;		/* convenient holder for return value */

    MII_READ (phyAddr, MII_STAT_REG, &phyStatus, retVal);
    if (retVal == ERROR)
	return (ERROR);

    /* does the PHY support the extended registers set? */

    if (!(phyStatus & MII_SR_EXT_CAP))
	return (OK);

    MII_READ (phyAddr, MII_AN_ADS_REG, &phyAds, retVal);
    if (retVal == ERROR)
	return (ERROR);

    MII_READ (phyAddr, MII_AN_PRTN_REG, &phyPrtn, retVal);
    if (retVal == ERROR)
	return (ERROR);

    MII_READ (phyAddr, MII_AN_EXP_REG, &phyExp, retVal);
    if (retVal == ERROR)
	return (ERROR);

    /* flow control configuration */

    /* MII defines symmetric PAUSE ability */

    if ((!(phyAds & MII_ANAR_PAUSE)) || (!(phyPrtn & MII_TECH_PAUSE)))
        pPhyInfo->phyFlags &= ~(MII_PHY_TX_FLOW_CTRL | MII_PHY_RX_FLOW_CTRL); 
    else
        pPhyInfo->phyFlags |= (MII_PHY_TX_FLOW_CTRL | MII_PHY_RX_FLOW_CTRL); 

    if (pPhyInfo->phyFlags & MII_PHY_GMII_TYPE)
        {

        /* GMII also defines asymmetric PAUSE ability */

        /* Advertises transmitter but no receiver */

        if (((phyAds & MII_ANAR_PAUSE_MASK) == MII_ANAR_ASM_PAUSE) &&
             ((phyPrtn & MII_TECH_PAUSE_MASK) == MII_TECH_PAUSE_MASK))
            {
            pPhyInfo->phyFlags |= MII_PHY_TX_FLOW_CTRL;
            }

        /* Advertises receiver but no transmitter */

        else if (((phyAds & MII_ANAR_PAUSE_MASK) == MII_ANAR_PAUSE_MASK) &&
            ((phyPrtn & MII_TECH_PAUSE_MASK) == MII_TECH_ASM_PAUSE))
            {
            pPhyInfo->phyFlags |= MII_PHY_RX_FLOW_CTRL;
            }

        /* no flow control */

        else if ((!(phyAds & MII_ANAR_PAUSE)) || (!(phyPrtn & MII_TECH_PAUSE)))
            {
            pPhyInfo->phyFlags &= ~(MII_PHY_TX_FLOW_CTRL | 
                                       MII_PHY_RX_FLOW_CTRL);
            }
        /* TX and RX flow control */

        else
            {
            pPhyInfo->phyFlags |= (MII_PHY_TX_FLOW_CTRL | MII_PHY_RX_FLOW_CTRL);
            }
        }

    /* find out the max common abilities */

    if (pPhyInfo->phyFlags & MII_PHY_GMII_TYPE)
        {
        /* search for 1000T capbility */

        /* get MASTER-SLAVE control register */

        MII_READ (phyAddr, MII_MASSLA_CTRL_REG, &phyMstSlaCtrl, retVal);
        if (retVal == ERROR)
            return (ERROR);

        /* get MASTER-SLAVE status register */

        MII_READ (phyAddr, MII_MASSLA_STAT_REG, &phyMstSlaStat, retVal);
        if (retVal == ERROR)
            return (ERROR);

        MII_READ (phyAddr, MII_MASSLA_STAT_REG, &phyMstSlaStat, retVal);
        if (retVal == ERROR)
            return (ERROR);

        if ((phyMstSlaStat & MII_MASSLA_STAT_LP1000T_FD) && 
            (phyMstSlaCtrl & MII_MASSLA_CTRL_1000T_FD))
            {
            /* 1000T FD supported */
             
            MII_PHY_FLAGS_SET (MII_PHY_FD);
            goto exitPhyUpdate;
            }
        else if ((phyMstSlaStat & MII_MASSLA_STAT_LP1000T_HD) && 
                 (phyMstSlaCtrl & MII_MASSLA_CTRL_1000T_HD))
            {
               /* 1000T HD supported */
                 
            MII_PHY_FLAGS_SET (MII_PHY_HD);
            MII_PHY_FLAGS_CLEAR (MII_PHY_1000T_FD);
            goto exitPhyUpdate;
            }
        else
            {
            /* 1000T not supported, go check other abilities */

            MII_PHY_FLAGS_CLEAR (MII_PHY_1000T_FD); 
            MII_PHY_FLAGS_CLEAR (MII_PHY_1000T_HD);
            }
        }


    negAbility = phyPrtn & phyAds & MII_ADS_TECH_MASK;

    MII_LOG (MII_DBG_ANY, ("miiPhyUpdate phyAds=0x%x
			   phyPrtn=0x%x common=0x%x phyExp=0x%x\n"),
			   phyAds, 
			   phyPrtn, 
			   negAbility, 
			   phyExp, 0, 0);

    if (negAbility & MII_TECH_100BASE_TX_FD)
	{
	}
    else if ((negAbility & MII_TECH_100BASE_TX) ||
	 (negAbility & MII_TECH_100BASE_T4))
	{
	MII_PHY_FLAGS_CLEAR (MII_PHY_FD);
	}
    else if (negAbility & MII_TECH_10BASE_FD)
	{
	MII_PHY_FLAGS_CLEAR (MII_PHY_100); 
	}
    else if (negAbility & MII_TECH_10BASE_T)
	{
	MII_PHY_FLAGS_CLEAR (MII_PHY_FD);
	MII_PHY_FLAGS_CLEAR (MII_PHY_100); 
	}
    else
	{
	MII_LOG (MII_DBG_ANY, ("miiPhyUpdate fail!\n"),
			       0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    /* store the current registers values */

exitPhyUpdate:

    pPhyInfo->miiRegs.phyStatus = phyStatus;
    pPhyInfo->miiRegs.phyAds = phyAds;
    pPhyInfo->miiRegs.phyPrtn = phyPrtn;
    pPhyInfo->miiRegs.phyExp = phyExp;

    if (pPhyInfo->phyFlags & MII_PHY_GMII_TYPE)
        {    
        pPhyInfo->miiGRegs.phyMSStatus = phyMstSlaStat;
        pPhyInfo->miiGRegs.phyMSCtrl = phyMstSlaCtrl;
        }

    /* handle some flags */

    if (miiFlagsHandle (pPhyInfo, phyAddr) != OK)
	return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* miiAnCheck - check the auto-negotiation process result
*
* This routine checks the auto-negotiation process has completed
* successfully and no faults have been detected by any of the PHYs 
* engaged in the process.
*
* NOTE: 
* In case the cable is pulled out and reconnect to the same/different 
* hub/switch again. PHY probably starts a new auto-negotiation process and 
* get different negotiation results. User should call this routine to check 
* link status and update phyFlags. pPhyInfo should include a valid 
* PHY bus number (phyAddr), and include the phyFlags that was used last time 
* to configure auto-negotiation process.
*
* RETURNS: OK or ERROR.
*/

STATUS miiAnCheck
    (
    PHY_INFO *  pPhyInfo,       /* pointer to PHY_INFO structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT8	regAddr;	/* PHY register */
    UINT16	phyPrtn;	/* PHY partner ability register value */
    UINT16	phyExp;		/* PHY expansion register value */
    int		retVal;		/* convenient holder for return value */


    /* 
     * The sysClkRate could have changed since  the link was lost. 
     * Ensure that pPhyInfo->phyMaxDelay is at lease 5 seconds. 
     */

    if (pPhyInfo->phyMaxDelay < (sysClkRateGet() * 5))
        {
        /* Set max delay porportionally */

        pPhyInfo->phyMaxDelay = sysClkRateGet() * 5;
        }

    /* run a check on the status bits of basic registers only */

    retVal = miiBasicCheck (pPhyInfo, phyAddr);
    if (retVal != OK)
	return (retVal);

    /* we know the auto-negotiation process has finished */

    regAddr = MII_AN_EXP_REG;

    MII_READ (phyAddr, regAddr, &phyExp, retVal);
    if (retVal != OK)
	return (ERROR);

    /* check for faults detected by the parallel function */

    if ((phyExp & MII_EXP_FAULT) == MII_EXP_FAULT)
	{
	MII_LOG (MII_DBG_ANY, ("miiAnCheck: fault expStat=0x%x\n"),
			       phyExp, 0, 0, 0, 0, 0);

	/*
	 * Don't fail here as some PHY devices don't support this register,
	 * but seem to return 0xffff when it is read (jgn - 5/8/00)
	 */

	/* return (ERROR); */
	}

    /* check for remote faults */

    regAddr = MII_AN_PRTN_REG;

    MII_READ (phyAddr, regAddr, &phyPrtn, retVal);
    if (retVal != OK)
	return (ERROR);

    if ((phyPrtn & MII_BP_FAULT) == MII_BP_FAULT)
	{
	MII_LOG (MII_DBG_ANY, ("miiAnCheck partner stat=0x%x\n"),
			       phyPrtn, 0, 0, 0, 0, 0);

	return (ERROR);
	}

    if (miiPhyUpdate (pPhyInfo, phyAddr) == ERROR)
	{
	MII_LOG (MII_DBG_ANY, "miiPhyUpdate error\n",0,0,0,0,0,0);
	return (ERROR);
	}

    MII_LOG (MII_DBG_ANY, ("miiAnCheck OK\n"),
			   0, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* miiModeForce - force an operating mode for the PHY
*
* This routine forces an operating mode for the PHY whose address is 
* specified in the parameter <phyAddr>. It also calls miiBasicCheck() 
* to ensure a valid link has been established.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS miiModeForce
    (
    PHY_INFO *  pPhyInfo,       /* pointer to PHY_INFO structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT16	data;		/* data to be written to the control reg */
    UINT8	regAddr;	/* PHY register */
    int		retVal;		/* convenient holder for return value */

    MII_LOG (MII_DBG_ANY, ("miiModeForce \n"),
			   0, 0, 0, 0, 0, 0);

    /*
     * force as a high priority as possible operating
     * mode, not overlooking what the user dictated.
     */
 
    data = MII_CR_NORM_EN;
 
    if (MII_PHY_FLAGS_ARE_SET (MII_PHY_100))
        {
        data |= MII_CR_100;
        }
 
    if (MII_PHY_FLAGS_ARE_SET (MII_PHY_FD))
        {
        data |= MII_CR_FDX;
        }
 
    pPhyInfo->miiRegs.phyCtrl = data;
 
    regAddr = MII_CTRL_REG;
 
    /* try to establish the link */

    MII_WRITE (phyAddr, regAddr, data, retVal);
    if (retVal != OK)
	return (ERROR);

    /* run a check on the status bits of basic registers only */

    if (miiBasicCheck (pPhyInfo, phyAddr) != OK)
	return (ERROR);

    /* handle some flags */

    if (miiFlagsHandle (pPhyInfo, phyAddr) != OK)
	return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* miiDefForce - force a default operating mode for the PHY
*
* This routine forces a default operating mode for the PHY whose address is 
* specified in the parameter <phyAddr>. It also calls miiBasicCheck() 
* to ensure a valid link has been established.
*
* RETURNS: OK or ERROR.
*
* ERRNO: S_miiLib_PHY_NO_ABLE
*
*/

LOCAL STATUS miiDefForce
    (
    PHY_INFO *  pPhyInfo,       /* pointer to PHY_INFO structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT16	data;		/* data to be written to the control reg */
    UINT8	regAddr;	/* PHY register */
    int		retVal;		/* convenient holder for return value */
    UINT16	phyStatus;	/* holder for the PHY status */

    MII_LOG (MII_DBG_ANY, ("miiDefForce \n"),
			   0, 0, 0, 0, 0, 0);

    /* translate user settings */

    data = miiDefLookupTbl [(pPhyInfo->phyDefMode)];

    /* find out what abilities the PHY features */
 
    regAddr = MII_STAT_REG;
    MII_READ (phyAddr, regAddr, &phyStatus, retVal);
    if (retVal == ERROR)
	return (ERROR);

    if (data & MII_CR_100) 
	{
	if (!(phyStatus & (MII_SR_TX_HALF_DPX
			   | MII_SR_TX_FULL_DPX
			   | MII_SR_T4)))
	    {
	    errnoSet (S_miiLib_PHY_NO_ABLE);
	    return (ERROR); 
	    }

	MII_PHY_FLAGS_SET (MII_PHY_100);
	}
    else
	{
	if (!(phyStatus & (MII_SR_10T_HALF_DPX 
			 | MII_SR_10T_FULL_DPX)))
	    {
	    errnoSet (S_miiLib_PHY_NO_ABLE);
	    return (ERROR); 
	    }
	MII_PHY_FLAGS_CLEAR (MII_PHY_100);
	}

    if (data & MII_CR_FDX)
	{
	if (!(phyStatus & (MII_SR_10T_FULL_DPX 
			 | MII_SR_TX_FULL_DPX)))
	    {
	    errnoSet (S_miiLib_PHY_NO_ABLE);
	    return (ERROR); 
	    }

	MII_PHY_FLAGS_SET (MII_PHY_FD);
	}
    else
	{
	if (!(phyStatus & (MII_SR_10T_HALF_DPX 
			 | MII_SR_TX_HALF_DPX)))
	    {
	    errnoSet (S_miiLib_PHY_NO_ABLE);
	    return (ERROR); 
	    }
	MII_PHY_FLAGS_CLEAR (MII_PHY_FD); 
	}

    pPhyInfo->miiRegs.phyCtrl = data;

    regAddr = MII_CTRL_REG;

    /* force this specific operating mode, but do not check the link */

    MII_WRITE (phyAddr, regAddr, data, retVal);

    /* handle some flags */

    if (miiFlagsHandle (pPhyInfo, phyAddr) != OK)
	return (ERROR);

    return (OK);
    }

/**************************************************************************
*
* miiAbilFlagSet - set some ability flags 
*
* This routine sets some ability flags for a later use.
*
* RETURNS: OK, always.
*/

LOCAL STATUS miiAbilFlagSet
    (
    PHY_INFO *  pPhyInfo,       /* pointer to PHY_INFO structure */
    UINT8	phyAddr		/* address of a PHY */
    )
    {
    UINT8	regAddr;	/* PHY register */
    UINT16	phyStatus;	/* holder for the PHY status */
    UINT16	phyExtStat;     /* holder for the PHY Extended status */
    int		retVal;		/* convenient holder for return value */

    /* find out what abilities the PHY features */
 
    regAddr = MII_STAT_REG;
    MII_READ (phyAddr, regAddr, &phyStatus, retVal);
    if (retVal == ERROR)
	return (ERROR);

    /* handle phy flags */

    if (!(phyStatus & (MII_SR_TX_HALF_DPX 
		       | MII_SR_TX_FULL_DPX
		       | MII_SR_T4)))
	{
	MII_PHY_FLAGS_CLEAR (MII_PHY_100);
	}

    if (!(phyStatus & (MII_SR_10T_FULL_DPX 
		     | MII_SR_TX_FULL_DPX)))
	{
	MII_PHY_FLAGS_CLEAR (MII_PHY_FD);
	}

    if (!(phyStatus & (MII_SR_10T_HALF_DPX 
		     | MII_SR_10T_FULL_DPX)))
	{
	MII_PHY_FLAGS_CLEAR (MII_PHY_10);
	}

    if (!(phyStatus & (MII_SR_TX_HALF_DPX 
		     | MII_SR_10T_HALF_DPX)))
	{
	MII_PHY_FLAGS_CLEAR (MII_PHY_HD);
	}

    if (!(phyStatus & MII_SR_AUTO_SEL))
	{
	MII_LOG (MII_DBG_ANY, ("miiAbilFlagSet phy not 
			       auto neg capable\n"), 
			       0, 0, 0, 0, 0, 0);

	MII_PHY_FLAGS_CLEAR (MII_PHY_AUTO);
	}

    if (pPhyInfo->phyFlags & MII_PHY_GMII_TYPE)
        {
        regAddr = MII_EXT_STAT_REG;
        MII_READ (phyAddr, regAddr, &phyExtStat, retVal);
        if (retVal != OK)
            return (ERROR);
 
        /* mask off 1000T FD if PHY not supported */

        if (!(phyExtStat & MII_EXT_STAT_1000T_HD))
             MII_PHY_FLAGS_CLEAR (MII_PHY_1000T_HD);

        /* mask off 1000T HD if PHY not supported */

        if (!(phyExtStat & MII_EXT_STAT_1000T_FD))
             MII_PHY_FLAGS_CLEAR (MII_PHY_1000T_FD);
        }

    return (OK);
    }

/**************************************************************************
*
* miiPhyOptFuncMultiSet - set pointers to MII optional registers handlers
*
* This routine sets the function pointers in <pPhyInfo->optRegsFunc> to the 
* MII optional, PHY-specific registers handler. The handler will be executed
* before the PHY's technology abilities are negotiated. If a system employees
* more than on type of network device requiring a PHY-specific registers 
* handler use this routine instead of miiPhyOptFuncSet() to ensure device
* specific handlers and to avoid overwritting one's with the other's. 
*
* PROTECTION DOMAINS
* (VxAE) This function can only be called from within the kernel protection
* domain. The argument optRegsFunc MUST be a pointer to function in the kernel
* protection domain.
*
* RETURNS: N/A.
*/

void miiPhyOptFuncMultiSet
    (
    PHY_INFO * pPhyInfo,	/* device specific pPhyInfo pointer */ 
    FUNCPTR    optRegsFunc      /* function pointer */
    )
    {
    if (optRegsFunc != NULL)
        pPhyInfo->pPhyOptRegsRtn = optRegsFunc;
    else
        pPhyInfo->pPhyOptRegsRtn = NULL;

    }

/**************************************************************************
*
* miiPhyOptFuncSet - set the pointer to the MII optional registers handler
*
* This routine sets the function pointer in <optRegsFunc> to the MII 
* optional, PHY-specific registers handler. The handler will be executed 
* before the PHY's technology abilities are negotiated.
*
* PROTECTION DOMAINS
* (VxAE) This function can only be called from within the kernel protection
* domain. The argument optRegsFunc MUST be a pointer to function in the kernel
* protection domain.
*
* RETURNS: N/A.
*/
 
void miiPhyOptFuncSet
    (
    FUNCPTR    optRegsFunc      /* function pointer */
    )
    {
    if (optRegsFunc != NULL)
	pPhyOptRegsRtn = optRegsFunc;
    else
	pPhyOptRegsRtn = NULL;

    }

/**************************************************************************
*
* miiPhyMonitorStart - monitor all the PHYs to detect a status change
*
* This routine monitors all the PHYs to detect a status change.
*
* RETURNS: OK or ERROR.
*/
 
LOCAL STATUS miiPhyMonitorStart
    (
    )
    {

    netJobAdd ((FUNCPTR) miiPhyMonitor, (int) NULL, 0, 0, 0, 0);

    return (OK);
    }

/**************************************************************************
*
* miiPhyMonitor - monitor all the PHYs to detect a link down event
*
* This routine monitors all the PHYs to detect a link down event.
*
* RETURNS: OK or ERROR.
*/
 
LOCAL STATUS miiPhyMonitor
    (
    )
    {
    UINT16		phyStatus;	/* holder for the PHY status */
    int			retVal = OK;	/* convenient holder for return value */
    PHY_INFO		* pPhyInfo;	/* PHY info pointer */
    MII_PHY_NODE *	pMiiPhyNode = NULL;	/* pointer to phy node */

    /* protect it from other MII routines */

    MII_SEM_TAKE (WAIT_FOREVER);

    /* loop through all the PHYs */

    for (pMiiPhyNode = (MII_PHY_NODE *) lstFirst (&miiList);
	 pMiiPhyNode != NULL;
	 pMiiPhyNode = (MII_PHY_NODE *) lstNext ((NODE *) pMiiPhyNode))
	{
	pPhyInfo = pMiiPhyNode->pPhyInfo;

	if ((pPhyInfo != NULL) && (MII_PHY_FLAGS_ARE_SET (MII_PHY_INIT))
	    && (MII_PHY_FLAGS_ARE_SET (MII_PHY_MONITOR)))
	    {
	    MII_READ (pPhyInfo->phyAddr, MII_STAT_REG, &phyStatus, retVal);
	    if (retVal == ERROR)
		goto miiMonitorExit;

	    MII_READ (pPhyInfo->phyAddr, MII_STAT_REG, &phyStatus, retVal);
	    if (retVal == ERROR)
		goto miiMonitorExit;

	    /* is the PHY's status link changed? */

            if ((pPhyInfo->miiRegs.phyStatus & MII_SR_LINK_STATUS) !=
		(phyStatus & MII_SR_LINK_STATUS))
		{
		MII_LOG (MII_DBG_ANY, ("miiPhyMonitor link down stat=0x%x\n"),
				       phyStatus, 0, 0, 0, 0, 0);

		/* let the driver know */

		if (pPhyInfo->phyLinkDownRtn != NULL)
		    netJobAdd ((FUNCPTR) (pPhyInfo->phyLinkDownRtn), 
				(int) (pPhyInfo->pDrvCtrl), 0, 0, 0, 0);

		/* record the state so we don't repeatedly tell the driver */

		pPhyInfo->miiRegs.phyStatus = phyStatus;
		}
	    }
	}

miiMonitorExit:
    MII_SEM_GIVE ();

    if ((wdStart ((WDOG_ID) miiWd, (MII_MONITOR_DELAY * sysClkRateGet ()),
			  (FUNCPTR) miiPhyMonitorStart, (int) 0)) == ERROR)
	return (ERROR);

    return (retVal);
    }

/**************************************************************************
*
* miiLibInit - initialize the MII library
*
* This routine initializes the MII library.
*
* PROTECTION DOMAINS
* (VxAE) This function can only be called from within the kernel protection
* domain.
*
* RETURNS: OK or ERROR.
*/
 
STATUS miiLibInit (void)
    {
    if (miiLibInitialized)
	return (OK);

    /* Create the mutex semaphore */
 
    miiMutex = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
			   SEM_INVERSION_SAFE);

    /* create the watchdog, do not start it */

    if ((miiWd = wdCreate ()) == NULL)
	return (ERROR);

    /* initialize the linked list */

    lstInit (&miiList);

    miiLibInitialized = TRUE;

    MII_LOG (MII_DBG_ANY, ("miiLibInit end\n"),
			   0, 0, 0, 0, 0, 0);

    return (OK);
    }

/**************************************************************************
*
* miiLibUnInit - uninitialize the MII library
*
* This routine uninitializes the MII library. Previously allocated resources
* are reclaimed back to the system.
*
* RETURNS: OK or ERROR.
*/
 
STATUS miiLibUnInit
    (
    )
    {
    if (!miiLibInitialized)
	return (OK);

    /* are there any PHYs still being used? */
    if (miiList.count > 0)
	return (ERROR);

    /* delete the watchdog */
    if (wdDelete (miiWd) == ERROR)
	return (ERROR);

    /* delete the mutex @@@ should we take it first? */
    if (semDelete (miiMutex) == ERROR)
	return (ERROR);

    miiLibInitialized = FALSE;

    return (OK);
    }

/**************************************************************************
*
* miiPhyListAdd - add a PHY to the MII linked list
*
* This routine adds the PHY specified in <pPhyInfo> to the MII linked list.
*
* RETURNS: OK or ERROR.
*/
 
LOCAL STATUS miiPhyListAdd
    (
    PHY_INFO *  pPhyInfo       /* pointer to PHY_INFO structure */
    )
    {
    MII_PHY_NODE *	pMiiPhyNode = NULL;	/* pointer to phy node */

    MII_LOG (MII_DBG_ANY, ("miiPhyListAdd start \n"),
			   0, 0, 0, 0, 0, 0);

    /* allocate memory for this node, and populate it */
    pMiiPhyNode = (MII_PHY_NODE *) calloc (1, sizeof (MII_PHY_NODE));
    pMiiPhyNode->pPhyInfo = pPhyInfo;

    /* protect it from other MII routines */

    MII_SEM_TAKE (WAIT_FOREVER);

    /* add it to the list */
    lstAdd (&miiList, (NODE *) pMiiPhyNode);

    /* we're done, release the mutex */

    MII_SEM_GIVE ();

    /* record this node */

    pPhyInfo->pMiiPhyNode = pMiiPhyNode;

    if ((wdStart (miiWd, MII_MONITOR_DELAY * sysClkRateGet (),
		  (FUNCPTR) miiPhyMonitorStart, (int) 0)) == ERROR)
	return (ERROR);

    MII_LOG (MII_DBG_ANY, ("miiPhyListAdd end \n"),
			   0, 0, 0, 0, 0, 0);

    return (OK);
    }

#ifdef MII_DBG
/**************************************************************************
*
* miiShow - show routine for MII library
*
* This is a show routine for the MII library
*
* RETURNS: OK, always.
*/
 
void miiShow
    (
    PHY_INFO    * pPhyInfo      /* pointer to PHY_INFO structure */
    )
    {
    MII_LOG (MII_DBG_ANY, ("miiShow phyCtrl=0x%x phyStat =0x%x phyAds=0x%x "
                           "phyPrtn=0x%x phyExp=0x%x phyNpt=0x%x\n"),
                           pPhyInfo->miiRegs.phyCtrl,
                           pPhyInfo->miiRegs.phyStatus,
                           pPhyInfo->miiRegs.phyAds,
                           pPhyInfo->miiRegs.phyPrtn,
                           pPhyInfo->miiRegs.phyExp,
                           pPhyInfo->miiRegs.phyNext);
 
    MII_LOG (MII_DBG_ANY, ("miiShow phyAddr=0x%x phyFlags=0x%x phySpeed=%d "
                           "phyMode=%s\n pDrvCtrl=0x%x \n"),
                           pPhyInfo->phyAddr,
                           pPhyInfo->phyFlags,
                           pPhyInfo->phySpeed,
                           pPhyInfo->phyMode,
                           pPhyInfo->pDrvCtrl,
                           0);
    }
 
 
/**************************************************************************
*
* miiRegsGet - get the contents of MII registers
*
* This routine gets the contents of the first <regNum> MII registers,
* and, if <buff> is not NULL, copies them to the space pointed to
* by <buff>.
*
* RETURNS: OK, or ERROR if could not perform the read.
*/
 
STATUS miiRegsGet
    (
    PHY_INFO    * pPhyInfo,     /* pointer to PHY_INFO structure */
    UINT        regNum,         /* number of registers to display */
    UCHAR	* buff          /* where to read registers to */
    )
    {
    int         retVal;         /* convenient holder for return value */
    int         i;              /* a counter */
 
    if (buff == NULL)
        return (OK);
 
    regNum = (regNum > MII_MAX_REG_NUM) ? MII_MAX_REG_NUM : regNum;
 
    /* Read all the registers the user specified */
 
    for (i = 0; i < regNum; i++)
        {
        MII_READ (pPhyInfo->phyAddr, i, (UINT16 *) &buff [2 * i], retVal);
        if (retVal != OK)
            return (ERROR);
        }
 
    return (OK);
    }

#endif /* MII_DBG */
