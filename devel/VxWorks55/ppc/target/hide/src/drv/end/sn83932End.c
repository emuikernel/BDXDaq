/* sn83932End.c - Nat. Semi DP83932B SONIC Ethernet driver */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"


/*
modification history
--------------------
01t,20sep01,dat  Removing ANSI errors for diab compiler
01s,21jun01,rcs  Merge SPR# 63876 fix to Tornado-Comp-Drv
01r,16feb01,pai  fixed mutual exclusion in sn83932Send().
01q,16feb01,pai  removed int[Lock|Unlock] frame around <txBlocked> flag set.
01p,16feb01,pai  fixed handling of IFF_UP and IFF_RUNNING flags (SPR #63876).
01o,16feb01,pai  made strtok_r arguments consistent with SPR 62224 fix.
01n,16feb01,pai  allocated DRV_CTRL structure from heap (SPR #32774).
01m,16feb01,pai  removed reference to etherLib.
01l,16feb01,pai  fixed use of NULL
01k,16feb01,pai  corrected return error codes and freed mBlk chain (SPR #28492)
01j,22sep98,dat  lint warnings removed
01i,18aug98,fle  doc : made first line in one-line
01h,11dec97,kbw  making man page edits
01g,08dec97,gnn  END code review fixes.
01f,05dec97,pul  restored sonic driver after major rework
01e,20aug97,pul  modified buffering scheme to use MBLKs
01d,12aug97,gnn  changes necessitated by MUX/END update.
01c,02May97,sal  Man page fix - SPR8487
01b,29Apr97,sal  set LBR bit in snEndDcr by default
01a,04Feb97,sal  adapted from if_sn.c (01s,23apr96,wkm)
*/

/*
DESCRIPTION
This module implements the National Semiconductor DP83932 SONIC Ethernet 
network interface driver.

This driver is designed to be moderately generic. Thus, it operates 
unmodified across the range of architectures and targets supported by 
VxWorks.  To achieve this, the driver load routine requires several 
target-specific parameters.  The driver also depends on a few external 
support routines.  These parameters and support routines are described 
below.  If any of the assumptions stated below are not true for
your particular hardware, this driver probably cannot function correctly
with that hardware. This driver supports up to four individual units per CPU.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver provides the END external interface. Thus, the 
only normal external interface is the sn83932EndLoad() routine, 
although snEndClkEnable() and snEndClkDisable() are provided for 
the use (optional) of the internal clock.  All required parameters 
are passed into the load function by means of a single colon-delimited 
string.  The sn83932Load() function uses strtok() to parse the string,
which it expects to be of the following format:

    <unit_ID>:<devIO_addr>:<ivec>:<e_addr>

The entry point for sn83932EndLoad() is defined within the 'endDevTbl' 
in configNet.h.


TARGET-SPECIFIC PARAMETERS
.IP <unit_ID>
A convenient holdover from the former model, this is only used in the
string name for the driver.

.IP <devIO_addr>
Denotes the base address of the device's I/O register set. 

.IP <ivec>
Denotes the interrupt vector to be used by the driver to service an 
interrupt from the SONIC device.  The driver connects the interrupt 
handler to this vector by calling intConnect().

.IP <e_addr>
This parameter is obtained by calling sysEnetAddrGet(), an external 
support routine. It specifies the unique six-byte address assigned to 
the VxWorks target on the Ethernet.
.LP


EXTERNAL SUPPORT REQUIREMENTS
This driver requires the following external support routines:
.IP sysEnetInit()
.CS
    void sysEnetInit (int unit)
.CE
This routine performs any target-specific
operations that must be executed before the SONIC device is initialized.
The driver calls this routine, once per unit, during the unit start-up 
phase.
.IP sysEnetAddrGet()
.CS
    STATUS sysEnetAddrGet (int unit, char *pCopy)
.CE
This routine provides the six-byte Ethernet address used by <unit>.  It
must copy the six-byte address to the space provided by <pCopy>.  This
routine returns OK, or ERROR if it fails.  The driver calls this routine, 
once per unit, during the unit start-up phase. 
.IP sysEnetIntEnable()
.CS
    void sysEnetIntEnable (int unit), void sysEnetIntDisable (int unit)
.CE
These routines enable or disable the interrupt from
the SONIC device for the specified <unit>.  Typically,
this involves interrupt controller hardware,
either internal or external to the CPU.  The driver calls these routines 
only during initialization, during the unit start-up phase.
.IP sysEnetIntAck()
.CS
    void sysEnetIntAck (int unit)
.CE
This routine performs any interrupt acknowledgment or
clearing that may be required.  This typically
involves an operation to some interrupt control hardware. 
The driver calls this routine from the interrupt handler.
.LP

DEVICE CONFIGURATION
Two global variables, 'snEndDcr' and 'snEndDcr2', are used to set the SONIC
device configuration registers.  By default, the device is programmed in
32-bit mode with zero-wait states.  If these values are not suitable,
the 'snEndDcr' and 'snEndDcr2' variables should be modified before 
loading the driver.  See the SONIC manual for information on appropriate
values for these parameters.

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:
    
    - one interrupt vector
    - 0 bytes in the initialized data section (data)
    - 696 bytes in the uninitialized data section (BSS)

The above data and BSS requirements are for the MC68020 architecture 
and can vary for other architectures.  Code size (text) varies greatly 
between architectures and is therefore not quoted here.

This driver uses cacheDmaMalloc() to allocate the memory to be shared with 
the SONIC device.  The size requested is 117,188 bytes.

The SONIC device can only be operated if the shared memory region is
write-coherent with the data cache.  The driver cannot maintain cache
coherency for the device for data that is written by the driver
because fields within the shared structures are asynchronously modified by
the driver and the device, and these fields may share the same cache
line.

SEE ALSO: ifLib
*/

/*
INTERNAL
This driver contains conditional compilation switch DEBUG.
If defined, adds debug output routines.  Output is further
selectable at run-time via the snDebug global variable.
See also the NOTES section at the end of this file.
Also contains LED DEBUG switch which enables the Led output.
*/

#include "vxWorks.h"
#include "sys/types.h"
#include "taskLib.h"
#include "iv.h"
#include "memLib.h"
#include "sys/ioctl.h"
#include "etherMultiLib.h"
#include "net/protosw.h"
#include "errno.h"
#include "cacheLib.h"
#include "stdio.h"
#include "intLib.h"
#include "logLib.h"
#include "netLib.h"
#include "netBufLib.h"
#include "muxLib.h"
#include "netinet/if_ether.h"

/* Include END specific headers */
#include "endLib.h"

/* Device Header File */
#include "drv/end/sn83932End.h"

/* CONDITIONAL SWITCHES */

#undef DEBUG
#undef LED_DEBUG

#define SN_DEV_NAME "sn"
#define SN_DEV_NAME_LEN 3

#if defined(LED_DEBUG) 
#include "../../../config/p4000/p4000.h"
#endif

/* STDOUT MACRO */

#define SN_LOGMSG(s,x1,x2,x3,x4,x5,x6) logMsg (s,x1,x2,x3,x4,x5,x6) 

/* LED */
#if defined(LED_DEBUG) || defined(PULI_LED_DEBUG)
#define PutLED(x,y)  	*P4000_ALPHAN_CHAR(y) = x << 24;
#define ClrLED					\
    *P4000_ALPHAN_CLR_ = P4000_RESET_ONE;	\
    *P4000_ALPHAN_CHAR(0) = ' ' << 24;		\
    *P4000_ALPHAN_CHAR(1) = ' ' << 24;		\
    *P4000_ALPHAN_CHAR(2) = ' ' << 24;		\
    *P4000_ALPHAN_CHAR(3) = ' ' << 24
#endif
 
/* LOCAL DEFINITIONS */

/* Maximum number of Multicast addresses supported */
#define MAX_MCASTS   15

/* configurable cluster sizes of the two cluster pools */
/* one, optimal cluster size */
#define OPTIMAL_CLUSTER_SIZE 128
/*second, MTU cluster size */
#define MTU_CLUSTER_SIZE  (ETHERMTU + SIZEOF_ETHERHEADER + 6)

/* 10 Mb */
#define SN_SPEED    10000000

/* Data Interrupt bits */
#define SN_IMR_DATA (PRXEN | PTXEN | TXEREN)
#define SN_IMR_INIT (SN_IMR_DATA | BREN)

/* max number of units to support */
#define MAX_UNITS   4                   

/* Minimum buffer fragment */
#define MIN_1STXMTBUF_SIZE	4

/* size of one receive buffer */
#define RX_BUF_SIZE (0x2000)            
#define RX_BUF_EXTRA (0x200)		/* Extra space needed to accomodate */
					/* Sonic buffer overrun in Rev. C */
					/* parts - found by Algorithmics  */
/* Total Receive Buffer Area */
#define RBA_SIZE    ((RX_BUF_SIZE + RX_BUF_EXTRA) * NUM_RRA_DESC)

/*** END MACROS ***/	/* Should be in endlib.h in my opinion */
#define END_M2_INUCAST(pEnd)		(pEnd)->mib2Tbl.ifInUcastPkts++
#define END_M2_INNUCAST(pEnd)		(pEnd)->mib2Tbl.ifInNUcastPkts++
#define END_M2_INERRORS(pEnd)		(pEnd)->mib2Tbl.ifInErrors++
#define END_M2_INDISCARDS(pEnd)		(pEnd)->mib2Tbl.ifInDiscards++
#define END_M2_INOCTETS(pEnd,bytes)	(pEnd)->mib2Tbl.ifInOctets += bytes
#define END_M2_OUTUCAST(pEnd)		(pEnd)->mib2Tbl.ifOutUcastPkts++
#define END_M2_OUTNUCAST(pEnd)		(pEnd)->mib2Tbl.ifOutNUcastPkts++
#define END_M2_OUTDISCARDS(pEnd)	(pEnd)->mib2Tbl.ifOutDiscards++
#define END_M2_OUTERRORS(pEnd)		(pEnd)->mib2Tbl.ifOutErrors++
#define END_M2_OUTOCTETS(pEnd,bytes)	(pEnd)->mib2Tbl.ifOutOctets += bytes

/* Struc to keep track of frag free routine: used in the driver struct below */
typedef struct free_routine     
    {
    FUNCPTR pfreeRtn;                     /* Free routine for the */
                         		 /* frag buffer */
    void* pSpare1;                       /* Spare agurment */
    void* pSpare2;                       /* Spare agurment */
    }FREE_ROUTINE;            

/* Driver control structure.  One per unit supported. */
typedef struct drv_ctrl
    {
    END_OBJ	endData;		     /* Enhance network driver  */
   					     /* structure */
    SONIC* pDev;                             /* ptr to the device registers */
    int   unit;				     /* unit # of the device */
    int	  ivec;				     /* interrupt vectore */
    char* pMem;                              /* ptr to allocated chunk */
    char* pShMem;                            /* ptr to area shared with  */
                                             /* device */
    unsigned long shMemSize;                 /* size of the shared area */
    unsigned long RSA;                       /* Start of Resource Area */
    unsigned long REA;                       /* End of Resource Area */
    unsigned long RBA1;                      /* Start of Receive Buffer Area */
    unsigned long RBA2;                      /* Start of Receive Buffer Area */
    unsigned long RBA3;                      /* Start of Receive Buffer Area */
    unsigned long RBA4;                      /* Start of Receive Buffer Area */
    unsigned long RDA;                       /* Start of Receive Desc Area */
    unsigned long CDA;                       /* Start of CAM Desc Area */
    TX_DESC* pTDA;                           /* Start of Transmit Desc Area */
    TX_DESC* pTXDFree;                       /* ptr to next free TXD */
    TX_DESC* pTXDReclaim;                    /* ptr to next reclaimable TXD */
    TX_DESC* pTXDLast;			     /* ptr to last transmitted TXD */
    BOOL     txBlocked;                      /* transmit flow control */
    RX_DESC* pRXDNext;                       /* Next Receive descriptor */
    RX_DESC* pRXDLast;                       /* Last one in link */
    u_long* pCamEnableMask;		     /* Pointer to CAM enable */
                                             /* mask entry */
    u_char  mcastTable[MAX_MCASTS * 6];      /* Our own mcast table */
    FUNCPTR timerIntHandler;                 /* Interrupt timer handler */
    int	 ticks;                              /* number of interrupts */
					     /* generated per sec */
    unsigned int imr;			     /* Record of interrupt mask */
    BOOL	online;			     /* Denotes device is */
                                             /* online (started)*/
    u_char	flags;			     /* Local Flags */
    UCHAR	enetAddr[6];		     /* ethernet address */
    FREE_ROUTINE freeBuf[NUM_TX_DESC];       /* per Tx descriptor */
    CL_POOL_ID pClPoolId[NUM_CLUSTER_POOLS]; /* array of cluster pools each */
				             /* of different size */
    } DRV_CTRL;

/*network buffer configuration */
M_CL_CONFIG SnMclConfig =	             /*mBlk configuration table */
    {
    0, 0, NULL, 0
    };

/*network cluster pool configuration */
CL_DESC SnClDescTbl []= 
    {
    /*
    ClusterSize                num	memArea	memsize
    -----------                ---      ------- -------
    */
    { OPTIMAL_CLUSTER_SIZE,	0,	NULL,	  0  },
    { MTU_CLUSTER_SIZE,         0,      NULL,     0  }

    };

/*Number of cluster pools */
int SnClDescTblNumEnt = (NELEMENTS(SnClDescTbl));

/* Initialization Parameters within InitString */
typedef struct 
    {
    int     unit;
    int	    ivec;
    u_long  devaddr;
    } INIT_PARM;

/* GLOBALS */
unsigned long snEndDcr = WAIT0 | DW_32 | LBR;
unsigned long snEndDcr2 = 0;

/* External function prototypes not defined in any header files.
 * Some of these are the functions required of the BSP modules.
 */
IMPORT STATUS sysEnetAddrGet ();
IMPORT STATUS sysEnetInit ();
IMPORT STATUS sysEnetIntEnable ();
IMPORT STATUS sysEnetIntDisable ();
IMPORT STATUS sysEnetIntAck ();

#if defined(LED_DEBUG) || defined(DEBUG)
#undef  LOCAL
#define LOCAL 
#endif

/* END EXTERNAL INTERFACE ROUTINES */
END_OBJ*      sn83932EndLoad (char* initString);

/* REQUIRED END ENTRY POINTS */
LOCAL STATUS	sn83932Start 		(DRV_CTRL *);
LOCAL STATUS	sn83932Stop 		(DRV_CTRL *);
LOCAL STATUS	sn83932Unload 		(DRV_CTRL *);
LOCAL int	sn83932Ioctl 		(DRV_CTRL *, int, caddr_t);
LOCAL STATUS	sn83932Send 		(DRV_CTRL *, M_BLK_ID pBuf);
LOCAL STATUS	sn83932MCastAddrAdd 	(DRV_CTRL *, char*);
LOCAL STATUS	sn83932MCastAddrDel 	(DRV_CTRL *, char*);
LOCAL STATUS	sn83932MCastAddrGet 	(DRV_CTRL *, MULTI_TABLE*);
LOCAL STATUS	sn83932PollSend 	(DRV_CTRL *, M_BLK_ID pBuf);
LOCAL STATUS	sn83932PollRecv 	(DRV_CTRL *, M_BLK_ID pBuf);

/* Interrupt Handler */
LOCAL void	sn83932Int		(DRV_CTRL *);

/* INTERNAL ROUTINES */
LOCAL STATUS	snInitParse 	(INIT_PARM *, char *);
LOCAL STATUS	snInitMem 	(DRV_CTRL *);
LOCAL void	snChipReset	(DRV_CTRL *);
LOCAL void	snChipInit	(DRV_CTRL *);
LOCAL void	snChipStart	(DRV_CTRL *);
LOCAL void	snCamMacLoad	(DRV_CTRL *);
LOCAL void	snCamMcastLoad	(DRV_CTRL *);
LOCAL void	snPollStart	(DRV_CTRL *);
LOCAL void	snPollStop	(DRV_CTRL *);
LOCAL void	snIfConfig	(DRV_CTRL *, int);
LOCAL void	snTxReclaim	(DRV_CTRL *, int);
LOCAL int	snProcessRxPkts	(DRV_CTRL *, int, M_BLK_ID pBuf);
LOCAL void	snEventHandler	(DRV_CTRL *);

/* SONIC Clock TIMER ENTRY POINTS */
void  snEndClkEnable	(int, int, FUNCPTR);
void  snEndClkDisable	(int);

/*
 * Declare our function table.  This is static across all driver
 * instances.
 */
LOCAL NET_FUNCS snFuncTable =
    {
    (FUNCPTR)sn83932Start,	/* Function to start the device. */
    (FUNCPTR)sn83932Stop,	/* Function to stop the device. */
    (FUNCPTR)sn83932Unload,	/* Unloading function for the driver. */
    (FUNCPTR)sn83932Ioctl,	/* Ioctl function for the driver. */
    (FUNCPTR)sn83932Send,	/* Send function for the driver. */
    (FUNCPTR)sn83932MCastAddrAdd,/* Multicast address add function */
    (FUNCPTR)sn83932MCastAddrDel,/* Multicast address delete function. */
    (FUNCPTR)sn83932MCastAddrGet,/* Multicast table retrieve function. */
    (FUNCPTR)sn83932PollSend,	/* Polling send function. */
    (FUNCPTR)sn83932PollRecv,	/* Polling receive function. */
    endEtherAddressForm,        /* Put address info into a NET_BUFFER */
    endEtherPacketDataGet,      /* Get pointer to data in NET_BUFFER. */
    endEtherPacketAddrGet
    };

/* SECTION: Initialization and Start-Up Routines */


/*******************************************************************************
*
* sn83932EndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device specific parameters are passed in the <initString> 
* parameter.  This string must be of the format:
*
*     <unit_number>:<device_reg_addr>:<ivec>
*
* These parameters are all individually described in the sn83932End man
* page.
*
* RETURNS: An END object pointer or NULL on error.
*/


END_OBJ * sn83932EndLoad
    (
    char * initString		/* String to be parse by the driver. */
    )
    {
    INIT_PARM	initParm;
    DRV_CTRL * pDrvCtrl;

#ifdef DEBUG
    SN_LOGMSG ("sn83932End Loading...\n", 0,0,0,0,0,0);
#endif 

    /* Parse the initString Parameters */

    if (initString == NULL)
        return(NULL);

    if (initString[0] == 0)
        {
        bcopy ((char *)SN_DEV_NAME, initString, SN_DEV_NAME_LEN);
        return (0);
        }

    if (snInitParse (&initParm, initString) == ERROR)
	return NULL; 

    /* Sanity check the unit number */
    if (initParm.unit < 0 || initParm.unit >= MAX_UNITS)
        return NULL;

    /* allocate the device structure */
    pDrvCtrl = (DRV_CTRL *)calloc (sizeof(DRV_CTRL), 1);

    if (pDrvCtrl == NULL)
        return (NULL);

    if (pDrvCtrl->endData.attached == TRUE)
        return (&pDrvCtrl->endData);

    /* save the inputted parameters */
    pDrvCtrl->unit = initParm.unit;
    pDrvCtrl->ivec = initParm.ivec;
    pDrvCtrl->pDev = (SONIC *) initParm.devaddr;

    /* Initialize the default interrupt mask configuration in control block */
    pDrvCtrl->imr = SN_IMR_INIT;

    /* EndData Initialization */
    if (END_OBJ_INIT (&pDrvCtrl->endData, (DEV_OBJ *)pDrvCtrl, "sn",
                      pDrvCtrl->unit,	&snFuncTable,
                      "Sonic 83932 Enhanced Network Driver") == ERROR)
		      
         return (NULL);

    /* get an Ethernet addr from BSP and intialize the MIB2 */
    if (sysEnetAddrGet (initParm.unit, pDrvCtrl->enetAddr) == ERROR)
        return (NULL);

    if (END_MIB_INIT (&pDrvCtrl->endData, M2_ifType_ethernet_csmacd,
		      (UCHAR *)pDrvCtrl->enetAddr,6,ETHERMTU, SN_SPEED) 
		      == ERROR)
	return (NULL);

    /* Allocate, initialize and set up shared memory region */
    if (snInitMem (pDrvCtrl) == ERROR)
	return (NULL);

    /***** Perform device initialization *****/
    snChipReset (pDrvCtrl);                 /* reset device */
    sysEnetInit (pDrvCtrl->unit);           /* do any board specific set up */
    sysEnetIntDisable (pDrvCtrl->unit);     /* board specific int disable */
    snChipInit (pDrvCtrl);		    /* init the device */

    /* Set flags */
    END_OBJ_READY (&pDrvCtrl->endData,
		   (IFF_NOTRAILERS | IFF_MULTICAST | IFF_BROADCAST));

    return (&pDrvCtrl->endData);
    }

/*******************************************************************************
*
* snInitParse - parse parameter values from initString according to the
*	following format:
*
*    int    unit,       * unit number *
*    char * pDevRegs,   * addr of device's regs *
*    int    ivec        * vector number *
*
*
* RETURNS: OK or ERROR
*/


LOCAL STATUS snInitParse
    (
    INIT_PARM * pParms,      /* pointer contains the data in the initstring */
    char *      pinitString
    )
    {
    char *	ptok;			/* token for the initString */
    char *	pHolder=NULL;           /* temp string holder */

    ptok = strtok_r(pinitString, ":", &pHolder);
    if (ptok == NULL)
        return ERROR;
    pParms->unit = atoi(ptok);

    ptok=strtok_r(NULL, ":", &pHolder);
    if (ptok == NULL)
        return ERROR; 
    pParms->devaddr = strtoul (ptok, NULL, 16);

    ptok=strtok_r(NULL, ":", &pHolder);
    if (ptok == NULL)
        return ERROR;
    pParms->ivec = atoi (ptok);
    
    return OK;
    }


/*******************************************************************************
*
* sn83932Start - start the device
*
* This function connects interrupt, calls BSP to enable them,  and starts the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR
* NOMANUAL
*/

LOCAL STATUS sn83932Start
    (
    DRV_CTRL * pDrvCtrl
    )
    {

#ifdef DEBUG
    SN_LOGMSG ("sn83932Start...\n",0,0,0,0,0,0);
#endif

    /* Sanity Check */
    if (pDrvCtrl == NULL)
	return EINVAL;

    if (pDrvCtrl->online)
	return OK;

    /* Connect the interrupt vector */
    if (intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC(pDrvCtrl->ivec),
		       sn83932Int, (int)pDrvCtrl) != OK)
	return ERROR;

    /* Enable device interrupts at system level */
    sysEnetIntEnable (pDrvCtrl->unit);

    /* Initialize the physical address in the CAM */
    snCamMacLoad (pDrvCtrl);

    /* Bring up the device */
    snChipStart (pDrvCtrl);

    /* Denote it as online */
    pDrvCtrl->online = TRUE;

    /* mark the interface as up */

    END_FLAGS_SET (&pDrvCtrl->endData, (IFF_UP | IFF_RUNNING));

#ifdef DEBUG
    SN_LOGMSG ("end of sn83932Start...\n",0,0,0,0,0,0);
#endif

    return (OK);

    } /* End of sn83932Start() */

/*******************************************************************************
*
* sn83932Stop - Stop the device
*
* This routine disables interrupts and resets the device.
*
* RETURNS: OK or ERROR
* NOMANUAL
*/

LOCAL STATUS sn83932Stop
    (
    DRV_CTRL * pDrvCtrl
    )
    {

    /* Sanity Check */
    if (pDrvCtrl == NULL)
	return EINVAL;

    sysEnetIntDisable (pDrvCtrl->unit);         /* board specific int */
						/* disable */
    pDrvCtrl->pDev->imr = 0;	                /* Turn off interrupts */
    pDrvCtrl->online = FALSE;			/* denote unit as offline */

    /* mark the interface as down */

    END_FLAGS_CLR (&pDrvCtrl->endData, (IFF_UP | IFF_RUNNING));

    return OK;
    }

/*******************************************************************************
*
* sn83932Unload - unload the device by freeing any allocated memory and
*	          releasing the EndData objects from the MUX.
*
*
* RETURNS:  OK or ERROR
* NOMANUAL
*
*/

LOCAL STATUS sn83932Unload
    (
    DRV_CTRL *pDrvCtrl
    )
    {

    /* Sanity Check */
    if (pDrvCtrl == NULL)
	return EINVAL;

    if (pDrvCtrl->endData.attached == TRUE)
        {
	/* Stop the device if still on line */
	if (pDrvCtrl->online)
	    sn83932Stop (pDrvCtrl);

        /* Reset the Chip to pre-empt any further action */
	snChipReset (pDrvCtrl);

        /* release EndData objects */
        END_OBJECT_UNLOAD (&pDrvCtrl->endData);

        /* Free up malloc'd memory */
        cacheDmaFree (pDrvCtrl->pMem);

	/* Denote device as detached */
	pDrvCtrl->endData.attached = FALSE;

	}

    return OK;

    }



/******************************************************************************
*
* snInitMem - establish shared memory region for sonic chip
*
* The driver and the device share areas of memory.  Each area has a
* specific use.  The device imposes a nasty restriction, in that
* an area's bounds must lie in the same 64k page.  The entire amount of
* shared memory that is needed is much less than 64k.  Therefore, we
* will group all the individual areas into one area, and attempt to
* ensure that this total area lies in a single 64k page.
*
* We must also consider data cache coherency of this shared memory.
* We therefore use the special system functions that allow us to
* obtain cache-safe memory, or at least allow us to maintain the
* coherency of this memory ourselves.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS snInitMem
    (
    DRV_CTRL * pDrvCtrl
    )
    {
    int cnt;
    u_long temp;				/* Temporary address holder */
    char *pCur;					/* Block local variable */
    CAM_DESC * pCam;				/* CAM working descriptor */
    TX_DESC * pTXD;
    RX_DESC * pRXD;                              /* Rx working descriptor */
    RRA_DESC * pRRAD;                            /* RRA working descriptor */

    /* The first step is to calculate the size of this shared region. */ 

    pDrvCtrl->shMemSize =
                    CAM_SIZE +      /* the area used for Ethernet addrs   */
                    RRA_SIZE +      /* the area that describes Rx buffers */
                    RDA_SIZE +      /* the area that holds Rx descriptors */
                    TDA_SIZE +      /* the area that holds Tx descriptors */
                    RBA_SIZE;       /* the area that holds all Rx buffers */

    /*
     * That was easy.  Now we request some memory from the system.
     * For now, we are taking a simple approach.  We simply request one
     * big region, large enough to ensure that we can position our shared
     * area in a single 64k page.  This approach will waste memory, but
     * blame the board designers who picked the SONIC without realizing
     * that the device has this weird restriction.
     */

    if (!CACHE_DMA_IS_WRITE_COHERENT ())
        return (ERROR);

    pDrvCtrl->pMem = cacheDmaMalloc (pDrvCtrl->shMemSize + 0x00010000);

    if (pDrvCtrl->pMem == NULL)                 /* no memory, so abort */
        return (ERROR);

    /*
     * Find start of 64k page in the region.  This becomes start of our
     * region.
     */

    pDrvCtrl->pShMem = (char *)
                       (((u_long)pDrvCtrl->pMem & 0xffff0000) + 0x00010000);

    /***** Carve up the shared area into specific areas *****/
    pCur = pDrvCtrl->pShMem;                /* start of shared area */
    pDrvCtrl->CDA = (u_long) pCur;          /* CAM area */
    pCur += CAM_SIZE;                       /* advance the pointer */

    pDrvCtrl->RSA = (u_long) pCur;          /* receive resource table */
    pCur += RRA_SIZE;                       /* advance the pointer */

    pDrvCtrl->REA = (u_long) pCur;          /* save end pointer */
    pDrvCtrl->RDA = (u_long) pCur;          /* receive descriptor area */
    pCur += RDA_SIZE;                       /* advance the pointer */

    pDrvCtrl->pTDA = (TX_DESC *)pCur;       /* transmit descriptor area */
    pCur += TDA_SIZE;                       /* advance the pointer */

    pDrvCtrl->RBA1 = (u_long) pCur;         /* Receive Buffer 1 */
    pCur += RX_BUF_SIZE + RX_BUF_EXTRA;     /* advance the pointer */

    pDrvCtrl->RBA2 = (u_long) pCur;         /* Receive Buffer 2 */
    pCur += RX_BUF_SIZE + RX_BUF_EXTRA;     /* advance the pointer */

    pDrvCtrl->RBA3 = (u_long) pCur;         /* Receive Buffer 3 */
    pCur += RX_BUF_SIZE + RX_BUF_EXTRA;     /* advance the pointer */

    pDrvCtrl->RBA4 = (u_long) pCur;         /* Receive Buffer 4 */

    /* Init the CAM Descriptors to all broadcasts */
    pCam = (CAM_DESC *)pDrvCtrl->CDA;

    /* Program the descriptor entry for each CAM register */
    for (cnt = 0; cnt < CAM_COUNT; cnt++, pCam++)
	{
    	pCam->cep  = cnt; 
    	pCam->cap0 = 0xffff;
    	pCam->cap1 = 0xffff;
    	pCam->cap2 = 0xffff;
	}

    pDrvCtrl->pCamEnableMask = (u_long *)pCam;
    *pDrvCtrl->pCamEnableMask = 0;		/* All Addresses Disabled */

    /*** init the Tx descriptors ***/
    pTXD = pDrvCtrl->pTDA;                      /* get initial ptr */
    bzero ((char *)pTXD, TDA_SIZE);             /* zero the whole enchilada */

    for (cnt = 0; cnt < NUM_TX_DESC; cnt++)     /* loop thru each */
        {
	pTXD->number = cnt;
        pTXD->pLink = pTXD + 1;                 /* link to next */
        pTXD++;                                 /* bump to next */
        }

    /*
     * The link field of the last desc is special; it needs to point
     * back to the first desc.  So, we fix it here.
     */

    (--pTXD)->pLink = pDrvCtrl->pTDA;

    pDrvCtrl->pTXDFree = pDrvCtrl->pTDA;              /* initial value */
    pDrvCtrl->pTXDReclaim = pDrvCtrl->pTDA;           /* initial value */
    pDrvCtrl->pTXDLast = pDrvCtrl->pTDA;              /* initial value */

    /*** init the Rx buffer descriptors ***/
    /* Build the RRA */
    pRRAD = (RRA_DESC *)pDrvCtrl->RSA;           /* get ptr to first entry */

    /* Stuff buffer ptr; least significant 16 bits, then most significant 16 */
    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RBA1);
    pRRAD->buff_ptr0 = temp & UMASK;
    pRRAD->buff_ptr1 = temp >> 16;

    /* Stuff word count; least significant 16 bits, then most significant 16 */
    pRRAD->buff_wc0 = (RX_BUF_SIZE >> 1) & UMASK;
    pRRAD->buff_wc1 = (RX_BUF_SIZE >> 1) >> 16;
    pRRAD++;                                    /* bump to next entry */

    /* Stuff buffer ptr; least significant 16 bits, then most significant 16 */
    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RBA2);
    pRRAD->buff_ptr0 = (u_long) temp & UMASK;
    pRRAD->buff_ptr1 = (u_long) temp >> 16;

    /* Stuff word count; least significant 16 bits, then most significant 16 */
    pRRAD->buff_wc0 = RX_BUF_SIZE >> 1 & UMASK;
    pRRAD->buff_wc1 = RX_BUF_SIZE >> 1 >> 16;
    pRRAD++;                                    /* bump to next entry */

    /* Stuff buffer ptr; least significant 16 bits, then most significant 16 */
    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RBA3);
    pRRAD->buff_ptr0 = (u_long) temp & UMASK;
    pRRAD->buff_ptr1 = (u_long) temp >> 16;

    /* Stuff word count; least significant 16 bits, then most significant 16 */
    pRRAD->buff_wc0 = RX_BUF_SIZE >> 1 & UMASK;
    pRRAD->buff_wc1 = RX_BUF_SIZE >> 1 >> 16;
    pRRAD++;                                    /* bump to next entry */

    /* Stuff buffer ptr; least significant 16 bits, then most significant 16 */
    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RBA4);
    pRRAD->buff_ptr0 = (u_long) temp & UMASK;
    pRRAD->buff_ptr1 = (u_long) temp >> 16;

    /* Stuff word count; least significant 16 bits, then most significant 16 */
    pRRAD->buff_wc0 = RX_BUF_SIZE >> 1 & UMASK;
    pRRAD->buff_wc1 = RX_BUF_SIZE >> 1 >> 16;

    /* Setup the Rx frame descriptors */
    pRXD = (RX_DESC *) pDrvCtrl->RDA;           /* get initial ptr */
    bzero ((char *)pRXD, RDA_SIZE);             /* zero the whole tomato */
    pDrvCtrl->pRXDNext = pRXD;                  /* start from the start! */
    pDrvCtrl->pRXDLast =                        /* stuff ptr to last */
                    (RX_DESC *) ((u_long)pRXD + RDA_SIZE - RX_DESC_SIZ);

    while (pRXD <= pDrvCtrl->pRXDLast)          /* loop thru each */
        {
        pRXD->in_use = IN_USE;                  /* set IN_USE by device */
        pRXD->link =                            /* set link */
            (RX_DESC *) CACHE_DMA_VIRT_TO_PHYS (pRXD + 1);
        pRXD++;                                 /* bump to next */
        }

    /*
     * The link field of the last desc is special; it points nowhere,
     * but must mark the end-of-list.  So, we fix it here.
     */

    pDrvCtrl->pRXDLast->link = RX_EOL;


    /* memory allocation for NetPool */

    if((pDrvCtrl->endData.pNetPool = malloc (sizeof(NET_POOL))) == NULL)
        return (ERROR);

    /* number of clusters in each cluster pool */
    SnClDescTbl[0].clNum = NUM_OPTIMAL_CLUSTER;
    SnClDescTbl[1].clNum = NUM_MTU_CLUSTER;

    /* number of cluster blocks */
    SnMclConfig.clBlkNum = NUM_CL_BLK;

    /* number of mblks */
    SnMclConfig.mBlkNum = NUM_MBLK;

    /* memory requirement for cluster blks and mblks */
    SnMclConfig.memSize = ((SnMclConfig.mBlkNum * (MSIZE + sizeof (long)))
                           + (SnMclConfig.clBlkNum * (CL_BLK_SZ  + 
			   sizeof (long))));

    if ((SnMclConfig.memArea = (char *) memalign (sizeof(long),
                                SnMclConfig.memSize)) == NULL)
        return (ERROR);

    /* memory requirement for cluster pools */
    SnClDescTbl[0].memSize =((SnClDescTbl[0].clNum * 
                             (OPTIMAL_CLUSTER_SIZE +8))+ sizeof(int));

    SnClDescTbl[1].memSize =((SnClDescTbl[1].clNum * (MTU_CLUSTER_SIZE +8))
                             + sizeof(int)); 

    if((SnClDescTbl[0].memArea =(char *) memalign (sizeof(long),
                                 SnClDescTbl[0].memSize))==NULL) 
        return(ERROR); 

    if((SnClDescTbl[1].memArea = (char *) memalign (sizeof(long),
                                 SnClDescTbl[1].memSize))==NULL) 
        return(ERROR); 

    if (netPoolInit(pDrvCtrl->endData.pNetPool, &SnMclConfig,
                    &SnClDescTbl[0],SnClDescTblNumEnt, NULL) == ERROR) 
        {
#ifdef DEBUG
    SN_LOGMSG("could not init buffering\n",0,0,0,0,0,0);
#endif
        return(ERROR);
        }

    /*
     * obtain the poolId for each cluster pool, needed when obtaining *
     * a cluster 
     */
    pDrvCtrl->pClPoolId[0] = clPoolIdGet(pDrvCtrl->endData.pNetPool,
                                         OPTIMAL_CLUSTER_SIZE,FALSE);

    pDrvCtrl->pClPoolId[1] = clPoolIdGet(pDrvCtrl->endData.pNetPool,
                                         MTU_CLUSTER_SIZE,FALSE);

#ifdef DEBUG
    SN_LOGMSG("init mem ok\n",0,0,0,0,0,0);
#endif
    
    return OK;
    } 


/*======================================================================*
 *	D E V I C E   C O N T R O L   R O U T I N E S	
 *======================================================================*/

/*******************************************************************************
*
* snChipReset - Place chip in Reset mode
*/

LOCAL void snChipReset
    (
    DRV_CTRL * pDrvCtrl
    )

    {
    pDrvCtrl->pDev->cr = RST;
    pDrvCtrl->pDev->rsc = 0;  /* set the sequence counter to zero */
    }


/*******************************************************************************
*
* snChipInit -  Initialize the device registers
*
* Returns: N/A
*
*/

LOCAL void snChipInit 
    (
    DRV_CTRL * pDrvCtrl
    )

    {
    SONIC * pDev;                       /* ptr to the device regs */
    u_long temp;			/* holder of physical addresses */

    pDrvCtrl->txBlocked = FALSE;
    pDev = pDrvCtrl->pDev;

    pDev->cr = RST;                     /* Turn ON RESET */
                                        /* Data Control */
    pDev->dcr = snEndDcr;
    if (snEndDcr2)
	pDev->dcr2 = snEndDcr2;
    pDev->imr = 0;                      /* All Interrupts off */
    pDev->isr = 0x7fff;                 /* Clear ISR */

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RSA);
    pDev->urra = temp >> 16;            /* Upper RSA */
    pDev->rsa = temp & UMASK;           /* Lower RSA */

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->REA);
    pDev->rea = temp & UMASK;           /* Lower REA */

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RSA);
    pDev->rrp = temp & UMASK;           /* point to first desc */
    pDev->rwp = temp & UMASK;           /* point to first desc */
    pDev->rsc = 0;

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->RDA);
    pDev->urda = temp >> 16;            /* Upper RDA */
    pDev->crda = temp & UMASK;          /* Lower RDA */

    temp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pDrvCtrl->pTDA);
    pDev->utda = temp >> 16;            /* first TXD */
    pDev->ctda = temp & UMASK;

    pDev->cr = RST_OFF;                 /* Turn OFF RESET */
    pDev->cr = RRRA;                    /* prime with RRA read */

    pDev->rcr = BRD;			/* Accept broadcasts */
					/* Multicasts (up to 15) are 	*
					 * programmed directly into the *
					 * CAM registers.		*
					 */
    }

/*******************************************************************************
*
* snChipStart - hardware startup  of chip - sets up interupt masks and turns
*	on the receiver. 
*
* RETURNS N/A
*
*/

LOCAL void snChipStart 
    (
    DRV_CTRL * pDrvCtrl
    )

    {
    SONIC *pDev;                        /* ptr to the device regs */

    /* Point to device registers */
    pDev = pDrvCtrl->pDev;

    /* Enable transmit, receive, transmit error, & receive buffer exhaustion */
    /*SL* RDE cannot occur since there is space for 512 64 byte packets */

    /* Enable appropriate interrupts (those configured to be enabled) */
    pDev->imr = (pDrvCtrl->imr & SN_IMR_INIT);

    /* Turn on receiver */
    pDev->cr = RXEN; 

    }

/*******************************************************************************
*
* snCamMacLoad - put the local Ethernet address in the CAM
*
* First we set up a data area with the enet addr, then we tell the device
* where it is and command the device to read it.  When the device is done,
* it will interrupt us.  We wait for this interrupt with a semaphore that
* will be given by the interrupt handler.
*
* We now load all 16 CAM registers.  At initialization, all CAM descriptors
* were programmed to be broadcasts.  However, none were enabled.  In this
* routine, we program the hardware MAC address of the device unit.  The
* first CAM entry is reserved for the physical address of the unit.
* The remaining 15 are allotted to multicast addresses.  Nonewithstanding,
* whenever we load the CAM, all 16 descriptors are loaded.
*
*/

LOCAL void snCamMacLoad
    (
    DRV_CTRL * pDrvCtrl
    )

    {
     CAM_DESC * pCam;
     SONIC * pDev;


    /* Initialize pointers */
    pDev = pDrvCtrl->pDev;
    pCam = (CAM_DESC *)pDrvCtrl->CDA;


    /* Turn off load-Cam isr indication if set */
    if (pDev->isr & LCD)
	pDev->isr = LCD;


    /* Program the descriptor entry for CAM register 0 */
    pCam->cep  = 0; 
    pCam->cap0 = (pDrvCtrl->enetAddr[1] << 8) | pDrvCtrl->enetAddr[0];
    pCam->cap1 = (pDrvCtrl->enetAddr[3] << 8) | pDrvCtrl->enetAddr[2];
    pCam->cap2 = (pDrvCtrl->enetAddr[5] << 8) | pDrvCtrl->enetAddr[4];


    /* Enable the filtering for this address */
    *pDrvCtrl->pCamEnableMask |= 1;


    /* Set up Load registers */
    pDev->cdp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pCam) & UMASK;
    pDev->cdc = CAM_COUNT;                         /* Number of Entries */
    pDev->cr = LCAM;                               /* issue "load CAM" cmd */


    /*
     * Wait for operation to complete since we can't do anything until
     * the MAC address is registered.  A "dead time" loop here is deemed
     * OK, since we are in initialization phase, and will only do this
     * once.
     */

    while (!(pDev->isr & LCD))
        ;

    pDev->isr = LCD;                        /* clear the event flag */

    }


/*******************************************************************************
*
* snCamMcastLoad - Load all configured Multicast addresses in the CAM
*
* This routine differs from the previous one in that only the
* entries that were set aside for multicast addresses are updated.
*
*/

LOCAL void snCamMcastLoad
    (
    DRV_CTRL * pDrvCtrl
    )
    {
    MULTI_TABLE tmpTable;
    CAM_DESC *pCam;
    SONIC *pDev;
    int num;
    int intLevel;


    pCam = (CAM_DESC *)pDrvCtrl->CDA;
    pDev = pDrvCtrl->pDev;

    /* Update our Multicast table */
    tmpTable.pTable = (char *)pDrvCtrl->mcastTable;
    tmpTable.len = sizeof (pDrvCtrl->mcastTable);
    etherMultiGet (&pDrvCtrl->endData.multiList, &tmpTable);

    /* Record the first multicast descriptor before we increment the pointer*/
    pCam++;
    pDev->cdp = (u_long) CACHE_DMA_VIRT_TO_PHYS (pCam) & UMASK;

    for (num = 1; num <= pDrvCtrl->endData.nMulti; num++, pCam++)
	{ 
    	/* Program each descriptor entry for the appropriate CAM entry */
    	pCam->cep  = num; 
    	pCam->cap0 = (tmpTable.pTable[1] << 8) | tmpTable.pTable[0];
    	pCam->cap1 = (tmpTable.pTable[3] << 8) | tmpTable.pTable[2];
    	pCam->cap2 = (tmpTable.pTable[5] << 8) | tmpTable.pTable[4];

	/* Increment to next address */
	tmpTable.pTable += 6;

        /* Enable the filtering for this address */
        *pDrvCtrl->pCamEnableMask |= (1 << num);
	}
        

    /* Set up Load registers */
    pDev->cdc = CAM_COUNT - 1;                     /* 15 MCAST entries */


    /* Make sure the TXP bit is not set in the CR, wait if it is, then load CAM    */
    intLevel = intLock();

    while (pDev->cr & TXP)
	continue;


    /* safe to issue "load CAM" cmd */
    pDev->cr = LCAM;

    intUnlock(intLevel);


    /* Make sure load command has finished before continuing */
    while (!(pDev->isr & LCD))
	continue;

    /* Turn off LCD indication */
    pDev->isr = LCD;
    }



/*============================================================================*
 * 		D R I V E R	C O N T R O L     R O U T I N E S
 *============================================================================*/

/*******************************************************************************
*
* sn83932Ioctl - the driver I/O control function
*
* Process an ioctl request.
* This is one of the routines that can be called from "outside" via
* the interface structure.
* Cannot be called before the attach, since DRV_CTRL ptr would be NULL.
*
*  Returns:  	0 - successful
*		EINVAL - invalid argument(s)
* NOMANUAL
*
*/

LOCAL int sn83932Ioctl
    (
     DRV_CTRL *pDrvCtrl,		/* ptr to device control info */
     int cmd,				/* ioctl command */
     caddr_t data			/* argument data */
    )
    {
     int value;				/* Value of data when *
					 * passed-by-value *
					 */

    /* Sanity Check */
    if (pDrvCtrl == NULL)
	return EINVAL;

    switch (cmd)
        {
        case EIOCSADDR:
	    /* Set physical address - only valid if device is off line */
	    /* Copy address into the MIB table as well */
	    /* New address takes affect after device is started */

#ifdef DEBUG
    SN_LOGMSG ("Set Address...\n",0,0,0,0,0,0);
#endif

    	    if (data == NULL)
		return EINVAL;

	    if (pDrvCtrl->online)
		return EINVAL;

	    /* Copy the address into the device control block */
	    bcopy ((char *)data,
		   (char *)pDrvCtrl->enetAddr,
		   sizeof(pDrvCtrl->enetAddr)); 

	    /* Copy the address to the MIB table */
    	    bcopy ((char *)data,
		   (char *)pDrvCtrl->endData.mib2Tbl.ifPhysAddress.phyAddress,
		   pDrvCtrl->endData.mib2Tbl.ifPhysAddress.addrLength);

	    /* Program the new ethernet address */
	    snCamMacLoad (pDrvCtrl);
            break;


        case EIOCGADDR:
	    /* Copy address in device control area to user space */

#ifdef DEBUG
    SN_LOGMSG ("Get Address...\n",0,0,0,0,0,0);
#endif

    	    if (data == NULL)
		return EINVAL;

	    bcopy ((char *)pDrvCtrl->enetAddr,
		   (char *)data,
		   sizeof (pDrvCtrl->enetAddr));
	    break;

	case EIOCSFLAGS:
	    /* Set/Clear interface flags */

#ifdef DEBUG
    SN_LOGMSG ("Set Flags - %x...\n",(int)data,0,0,0,0,0);
#endif

	    value = (int)data;
	    if (value < 0)
		{/* Clear denoted flags */
		value = -value;
		value--;
		pDrvCtrl->endData.flags &= ~value;
		}
	    else
		/* Set flags */
		pDrvCtrl->endData.flags |= value;

	    snIfConfig (pDrvCtrl, value);
	    break;

	case EIOCGFLAGS:
	    /* Get interface flags */

#ifdef DEBUG
    SN_LOGMSG ("Get Flags...\n",0,0,0,0,0,0);
#endif

    	    if (data == NULL)
		return EINVAL;

	    *(int *)data = END_FLAGS_GET (&pDrvCtrl->endData);
	    break;

	case EIOCPOLLSTART:
	    /* Put device in poll mode */

	    snPollStart (pDrvCtrl);
	    break;

	case EIOCPOLLSTOP:
	    /* Put device into interrupt mode */

	    snPollStop (pDrvCtrl);
	    break;

/*SL*/	case EIOCGMIB2:
	    /* Get the MIB2 table */

#ifdef DEBUG
    SN_LOGMSG ("Get MIBS...\n",0,0,0,0,0,0);
#endif

    	    if (data == NULL)
		return EINVAL;

	    bcopy ((char *)&pDrvCtrl->endData.mib2Tbl, (char *)data,
		   sizeof (pDrvCtrl->endData.mib2Tbl));
	    break;

	case EIOCGFBUF:
	    /* Get some minimum buf size of first buffer in scatter xmit */
	    
#ifdef DEBUG
    SN_LOGMSG ("Get MIN_BUF_SIZE...\n",0,0,0,0,0,0);
#endif

	    if (data == NULL)
		return EINVAL;

	    *(int *)data = MIN_1STXMTBUF_SIZE;
	    break;

	default:

#ifdef DEBUG
    SN_LOGMSG ("Undefined Command - Return\n",
               0, 0, 0, 0, 0, 0);
#endif
	    return EINVAL;
        }

#ifdef DEBUG
    SN_LOGMSG ("Done\n", 0, 0, 0, 0, 0, 0);
#endif 

    return 0;
    }


/*******************************************************************************
*
* sn83932MCastAddrAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  
* NOMANUAL
*
*/

LOCAL STATUS sn83932MCastAddrAdd
    (
    DRV_CTRL *	pDrvCtrl,
    char *	pAddr
    )
    {
    int		retVal;
    

    /* Sanity Check */
    if (pDrvCtrl == NULL)
	return EINVAL;

#ifdef DEBUG
    SN_LOGMSG  ("sn83932McastAddrAdd - %x:%x:%x:%x:%x:%x\n",
                 pAddr[0], pAddr[1], pAddr[2], pAddr[3], pAddr[4], pAddr[5]);
#endif

    if (pDrvCtrl->endData.nMulti == MAX_MCASTS)
	return ERROR;

    retVal = etherMultiAdd (&pDrvCtrl->endData.multiList, pAddr);


    if (retVal == ENETRESET)
        {
        pDrvCtrl->endData.nMulti++;
        snCamMcastLoad (pDrvCtrl);
	retVal = OK;
        }

#ifdef DEBUG
    SN_LOGMSG ("Mcast Done\n", 0,0,0,0,0,0);
#endif 

    return ((retVal == OK) ? OK : ERROR);
    }


/*******************************************************************************
*
* sn83932MCastAddrDel - delete a multicast address for the device
*
* This routine deletes a multicast address from the current list of
* multicast addresses.
*
* NOMANUAL
*/

LOCAL STATUS sn83932MCastAddrDel
    (
    DRV_CTRL *	pDrvCtrl,
    char * 	pAddr
    )
    {
    int		retVal;
    

    /* Sanity Check */
    if (pDrvCtrl == NULL)
	return EINVAL;

#ifdef DEBUG
    SN_LOGMSG ("sn83932astAddrDel%x:%x:%x:%x:%x:%x\n",pAddr[0], 
	       pAddr[1], pAddr[2], pAddr[3], pAddr[4], pAddr[5]);
#endif
                  

    retVal = etherMultiDel (&pDrvCtrl->endData.multiList, pAddr);

    if (retVal == ENETRESET)
        {
        pDrvCtrl->endData.nMulti--;
        snCamMcastLoad (pDrvCtrl);
	retVal = OK;
        }

    return ((retVal == OK) ? OK : ERROR);
    }



/*******************************************************************************
*
* sn83932MCastAddrGet - get the current multicast address list
*
* This routine returns the current multicast address list in <pTable>
*
* NOMANUAL
*/
LOCAL STATUS sn83932MCastAddrGet
    (
    DRV_CTRL *	pDrvCtrl,
    MULTI_TABLE * pTable
    )
    {
    
    /* Sanity Check */
    if (pDrvCtrl == NULL)
	return EINVAL;

#ifdef DEBUG
    SN_LOGMSG ("sn83932astAddrGet\n", 0, 0, 0, 0, 0, 0);
#endif


    return (etherMultiGet (&pDrvCtrl->endData.multiList, pTable));
    }



/*******************************************************************************
*
* snPollStart - Put the device into polling mode.  We first wait until any
* 	       outstanding interrupt requests are processed normally.
*
* RETURNS:  N/A
*
* NOMANUAL
*/

LOCAL void snPollStart
    (
     DRV_CTRL * pDrvCtrl
    )
    {
    int intLevel;

    /* if offline, then only set flag */
    if (!pDrvCtrl->online)
	{
	pDrvCtrl->flags |= SN_POLLING;
	return;
	}


    if (pDrvCtrl->flags & SN_POLLING)
	return;

    intLevel = intLock();

    pDrvCtrl->pDev->imr = 0;      /*SL* &= ~SN_IMR_DATA; */

    /* Mark device as polling */
    pDrvCtrl->flags |= SN_POLLING;

    intUnlock (intLevel);


    } 


/*******************************************************************************
*
* snPollStop - Put the device into interrupt mode.  This is the default mode 
*	for the device.   Nothing is done if the device is not in polling mode. 
*
* RETURNS:  N/A
*
*/

LOCAL void snPollStop
    (
     DRV_CTRL *pDrvCtrl
    )
    {
    int  intLevel;

    intLevel = intLock();

    /* Mark device as polling */
    pDrvCtrl->flags &= ~SN_POLLING;

    /* Mask in data interrupts */
    pDrvCtrl->pDev->imr = pDrvCtrl->imr;

#ifdef LED_DEBUG
    PutLED ('I', 0);
#endif
    intUnlock (intLevel);
    } 


/*******************************************************************************
*
* snIfConfig - Config the device according to allowable interface flag
*	revisions by the application.  This is called from within the
*	Ioctl call to register IF flag changes.
*
* RETURNS:  N/A
*
*/

LOCAL void snIfConfig
    (
     DRV_CTRL * pDrvCtrl,
     int flags
    )
    {
    u_char *pDrvFlags;			/* ptr to the device's local flags */


    pDrvFlags = &pDrvCtrl->flags;

    if (flags & IFF_PROMISC)
	{
	if (!(*pDrvFlags & SN_PROMISC))
	    {/* Put device into promiscuous mode */

	    pDrvCtrl->pDev->rcr |= PRO;
	    *pDrvFlags |= IFF_PROMISC;
	    }
	}
    else
	{
	if (flags & SN_PROMISC)
	    {/* Turn off promiscuous mode */

	    pDrvCtrl->pDev->rcr &= ~PRO;
	    *pDrvFlags &= ~IFF_PROMISC;
	    }
	}


    if (flags & IFF_ALLMULTI)
	{
	if (!(*pDrvFlags & SN_ALLMULTI))
	    {/* Accept ALL multicast packets */

	    pDrvCtrl->pDev->rcr |= AMC;
	    *pDrvFlags |= SN_ALLMULTI;
	    }
	}
    else
	{
    	if (flags & SN_ALLMULTI)
	    {/* Filter multicasts according to multicast entries */

	    pDrvCtrl->pDev->rcr &= ~AMC;
	    *pDrvFlags &= ~SN_ALLMULTI;
	    }
	}
    }




/*============================================================================*
 * 			T R A N S M I T     R O U T I N E S
 *============================================================================*/

/*******************************************************************************
*
* sn83932Send - the driver send routine
*
* This routine takes a NET_BUFFER and sends off the data within the NET_BUFFER.
* The data must already have the addressing information properly installed
* in it.  This is done by a higher layer.  The last arguments are a free
* routine to be called when the device is done with the buffer and a pointer
* to the argument to pass to the free routine.  Supports NET_BUFFER chaining. 
*
* RETURNS: OK or ERROR.
* NOMANUAL
*/

LOCAL STATUS sn83932Send
    (
    DRV_CTRL * pDrvCtrl,		       /* device record ptr */
    M_BLK_ID pMblk 	         	       /* data to be sent */
    )
    {


    TX_DESC * pTXD;                            /* Tx descriptor to be used */
    u_long addr;
    int totalLength = 0;                       /* keep track of total length
						* of the mblk chain 
						*/

    M_BLK_ID pChunk;                           /* tmp mblk to travel 
						* the chain 
						*/
    int count =0;                     
    BOOL No_Mblk =FALSE;                       /* set in case all the data 
						* is contained in the first 
						* mblk of a chain 
						*/

    BOOL Exceed = FALSE;                       /* set if num of mblks in a 
						* chain exceed max fragments 
						* a tx desc can habdle 
						*/
    char * pBuf = NULL;


    /* Sanity Check */
    if (pDrvCtrl == NULL)
        {
        netMblkClChainFree(pMblk); /* free the given mBlk chain */
        errno = EINVAL;
        return (ERROR);
        }

#ifdef LED_DEBUG
    PutLED ('x',2)
#endif

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */

    END_TX_SEM_TAKE (&pDrvCtrl->endData, WAIT_FOREVER);

    if (pDrvCtrl->flags & SN_POLLING)
        {
        netMblkClChainFree(pMblk); /* free the given mBlk chain */
        errno = EINVAL;

        END_TX_SEM_GIVE (&pDrvCtrl->endData);
        return (ERROR);
        }

    if (pDrvCtrl->txBlocked)
        {
        END_TX_SEM_GIVE (&pDrvCtrl->endData);
        return (END_ERR_BLOCK); 
        }

    /* count the number of cluster in the chain */

    pChunk=pMblk;

    while(pChunk!= NULL)
        {
        if(pChunk->mBlkHdr.mLen > 0)
            count++;
        pChunk=pChunk->mBlkHdr.mNext;
        }

    if(count >= MAX_TX_FRAGS)
        {   
	/* allocate a cluster from the cluster pool of MTU size */
        pBuf = netClusterGet(pDrvCtrl->endData.pNetPool,
			     pDrvCtrl->pClPoolId[1]);
        if(pBuf == NULL)
            {
            END_M2_OUTDISCARDS (&pDrvCtrl->endData);

            pDrvCtrl->txBlocked = TRUE;

            END_TX_SEM_GIVE (&pDrvCtrl->endData);
            return (END_ERR_BLOCK);
            }
#ifdef PULI_LED_DEBUG
  PutLED('E',3);
#endif
        Exceed=TRUE;
        }   /*free mblk*/

    /* See if transmit descriptor is available */
    pTXD = pDrvCtrl->pTXDFree;

    if (pTXD->flag == IN_USE)
    	{  /* No room  - clean-up & return */

#ifdef DEBUG
    SN_LOGMSG("transmit descriptor in use\n",0,0,0,0,0,0); 
#endif
	if(Exceed)
	    netClFree(pDrvCtrl->endData.pNetPool,(UINT8 *)pBuf);

        END_M2_OUTDISCARDS (&pDrvCtrl->endData);

        pDrvCtrl->txBlocked = TRUE;

        /* Release the TX semaphore */
	END_TX_SEM_GIVE (&pDrvCtrl->endData);

	return (END_ERR_BLOCK);
	}  /* No room */

    /* Mark this descriptor as "in use" */
    pTXD->flag   = IN_USE;
    pTXD->status = 0;

    /*
     * Build a transmit descriptor.
     * Loop thru each fragment in the mbuf chain and stuff our info.
     */

    /*
     * loop through the chain until last mblk is pointing to NULL, 
     * consider clusters only if their length is positive number and if not, 
     * get the next mblk 
     */

    if(Exceed)
        {
        count=0;
        if(pMblk->mBlkHdr.mData[5] & 0x01)
            END_M2_OUTNUCAST (&pDrvCtrl->endData);
        else
            END_M2_OUTUCAST (&pDrvCtrl->endData);
		       
        /*
         * copy all the data in each mblk of the chain to the newly 
	 * allocate cluster
	 */
        totalLength = netMblkToBufCopy(pMblk,pBuf,NULL);

	/*Free mblk chain after copying */
        netMblkClChainFree(pMblk);

        CACHE_USER_FLUSH(pBuf,totalLength);

        /* set the data pointers and the correponding size of each fragment */
        addr = (u_long) CACHE_DMA_VIRT_TO_PHYS (pBuf);
        pTXD->frag [count].frag_ptr0 = (u_long) addr & UMASK;
        pTXD->frag [count].frag_ptr1 = (u_long) addr >> 16;
        pTXD->frag [count].frag_size = totalLength;
        count++;

        /* Record the buffer's free routine for later invocation */
        pDrvCtrl->freeBuf[pTXD->number].pfreeRtn  = (FUNCPTR)netClFree;
        pDrvCtrl->freeBuf[pTXD->number].pSpare1= pDrvCtrl->endData.pNetPool;
        pDrvCtrl->freeBuf[pTXD->number].pSpare2=pBuf;
        }

    else
        /* 
         * loop through the chain until last mblk is pointing to NULL,
         * consider clusters only if their length is positive number 
	 * and if not,
	 * get the next mblk
	 */

        {
        pChunk =pMblk;

        for(count=0;pChunk != NULL;) 
            {
            /* if all the data is contained in the first mblk */
            if((pChunk->mBlkHdr.mLen == pMblk->mBlkPktHdr.len) 
	       && pChunk==pMblk)
                No_Mblk=TRUE;

            if (pChunk->mBlkHdr.mLen <= 0)
	        {
                pChunk =pChunk->mBlkHdr.mNext;
                continue;
	        }  


	    /*
	     * Check if frame contains a broadcast/multicast destination
             * address.  Update appropriate MIB variable accordingly.
	     */

            if ((count == 0)  &&  (pChunk->mBlkHdr.mData[5] & 0x01))
	        END_M2_OUTNUCAST (&pDrvCtrl->endData);
            else
                END_M2_OUTUCAST (&pDrvCtrl->endData);

            /* ensure data is flushed */
            CACHE_USER_FLUSH (pChunk->mBlkHdr.mData,pChunk->mBlkHdr.mLen);

            /*
             * set the data pointers and the correponding size of 
	     * each fragment 
	     */
            addr = (u_long) CACHE_DMA_VIRT_TO_PHYS (pChunk->mBlkHdr.mData);
            pTXD->frag [count].frag_ptr0 = (u_long) addr & UMASK;
            pTXD->frag [count].frag_ptr1 = (u_long) addr >> 16;
            pTXD->frag [count].frag_size = pChunk->mBlkHdr.mLen;


            /* keep track of total packet length */
            totalLength += pChunk->mBlkHdr.mLen;

            if(No_Mblk)
                {
                count=1;
                break;
                }

             /* loop through */
             pChunk =pChunk->mBlkHdr.mNext;
             count++;
             }

        /* Record the buffer's free routine for later invocation */
        pDrvCtrl->freeBuf[pTXD->number].pfreeRtn = (FUNCPTR)netMblkClChainFree;
        pDrvCtrl->freeBuf[pTXD->number].pSpare1= pMblk;
        pDrvCtrl->freeBuf[pTXD->number].pSpare2=NULL;

        }

    /* Set total packet size & fragment count */
    pTXD->pkt_size = max (ETHERSMALL, totalLength);   /* set packet size */
    pTXD->frag_count = count;                     /* set fragment count */

    if (totalLength < ETHERSMALL)
        {
        pTXD->frag[count-1].frag_size += (ETHERSMALL - totalLength);
	totalLength = ETHERSMALL;
	}

    /* Update MIB variable accordingly */
    END_M2_OUTOCTETS (&pDrvCtrl->endData, totalLength);



    /* Reset the actual pLink field of the last transmitted descriptor */
    pDrvCtrl->pTXDLast->frag[pDrvCtrl->pTXDLast->frag_count].frag_ptr0
			&= ~TX_EOL;


    /* copy link field to where device will expect it  & set the EOL bit */
    pTXD->frag [count].frag_ptr0 =
        (u_long) CACHE_DMA_VIRT_TO_PHYS (pTXD->pLink) | TX_EOL;


    /* Adjust pointer to next TXD. */
    pDrvCtrl->pTXDFree = (TX_DESC *)((u_long)pTXD->pLink & ~TX_EOL);
    pDrvCtrl->pTXDLast = pTXD;


    /* start the transmitter but make sure the load-cam bit is not set */
    while (pDrvCtrl->pDev->cr & LCAM)
        continue;

    pDrvCtrl->pDev->cr = TXP;

    /* Release Transmit semaphore */
    END_TX_SEM_GIVE (&pDrvCtrl->endData);

    return OK;
    }


/*******************************************************************************
*
* sn83932PollSend - the driver send routine
*
*	This routine is valid only when called while the device is in Polling
*	mode.  The NetBuffer passed in must not be in a chained configuration.
*	If either of these rules are broken, the driver returns EINVAL.
*	The transmit descriptor is filled with the relevant data and the
*	transmitter started.  The transmit descriptor status is then polled for
*	completion.  The transmit descriptor is cleared and the data buffer
*	is freed.
*	Note:  The same descriptor is used for each invocation of this function.
*
*	RETURNS:  OK,
*		  EINVAL - invalid usage
*		  EAGAIN - error in transmission or busy
* NOMANUAL
*/

LOCAL STATUS sn83932PollSend
    (
    DRV_CTRL * pDrvCtrl,
    M_BLK_ID pMblk
    )
    {

    TX_DESC *pTXD;
    u_long  addr;
    int retval;
    volatile u_short stat = 0;

    /* Sanity Check */
    if (pDrvCtrl == NULL)
	return EINVAL;

    /* Buffer chaining not supported in Poll Mode */
    if (!(pDrvCtrl->flags & SN_POLLING) || (pMblk->mBlkHdr.mNext!=NULL))
	return (EINVAL);

    if (pDrvCtrl->txBlocked)
        return (END_ERR_BLOCK);
    
    /* See if transmit descriptor is available  */
    pTXD = pDrvCtrl->pTXDFree;

    if (pTXD->flag == IN_USE)
    	{/* No Room */
	return (EAGAIN);
    	}

    /* Mark the descriptor as "in use" to reserve it */
    pTXD->flag   = IN_USE;
    pTXD->status = 0;

    /* ensure data is flushed */
    CACHE_USER_FLUSH (pMblk->mBlkHdr.mData, pMblk->mBlkHdr.mLen);

    /* Set up descriptor */
    pTXD->pkt_size = max (ETHERSMALL, pMblk->mBlkHdr.mLen);
    pTXD->frag_count = 1;

    /* Assign to a fragment */
    addr = (u_long) CACHE_DMA_VIRT_TO_PHYS (pMblk->mBlkHdr.mData);
    pTXD->frag [0].frag_ptr0 = (u_long) addr & UMASK;
    pTXD->frag [0].frag_ptr1 = (u_long) addr >> 16;
    pTXD->frag [0].frag_size = pTXD->pkt_size; 

    /* copy link field to where device will expect it  & set the EOL bit */
    pTXD->frag [pTXD->frag_count].frag_ptr0 =
        (u_long) CACHE_DMA_VIRT_TO_PHYS (pTXD->pLink) | TX_EOL;

    /* Set the transmitter on */
    pDrvCtrl->pDev->cr = TXP;

    /* Poll for transmit completion */
    do  {
        CACHE_DMA_INVALIDATE (&pTXD->status, sizeof (pTXD->status));
	stat = pTXD->status;
    }while (!stat); 

    /* Update MIB variables */
    END_M2_OUTOCTETS (&pDrvCtrl->endData, pTXD->pkt_size);

    /* Determine if unicast or not */
    if (pMblk->mBlkHdr.mData[5] & 0x01)
	END_M2_OUTNUCAST (&pDrvCtrl->endData);
    else
	END_M2_OUTUCAST (&pDrvCtrl->endData);

    /* Check for successful transmission */
    if (stat & PTX)
	retval = OK; /* Successful Send */

    else
	{  /* Error in Sending */

	END_M2_OUTERRORS (&pDrvCtrl->endData);
	retval = EAGAIN;
	}

    /* Free this Descriptor  for another poll request */
    pTXD->flag = NOT_IN_USE;
    pTXD->status = 0;

    /* Adjust pointer to next TXD. */
    pDrvCtrl->pTXDFree = (TX_DESC *)((u_long)pTXD->pLink & ~TX_EOL);
    pDrvCtrl->pTXDLast = pTXD;
    pDrvCtrl->pTXDReclaim = pDrvCtrl->pTXDFree;

    return (retval);
    }



/*****************************************************************************
*
* snTxReclaim - reclaims transmit descriptors that the device has serviced
*
*	 - Only valid if device is not in Polling Mode -
*
*/

LOCAL void snTxReclaim 
    (
    DRV_CTRL *pDrvCtrl,
    int maxReclaims
    )
    {
    TX_DESC *pTXD;
    int  intLevel;
    int numReclaims=0;

    /* No business here if we're in Poll Mode */

    if (pDrvCtrl->flags & SN_POLLING)
	return;

    /* Check if someone else is in the midst of reclaiming */
    if (pDrvCtrl->flags & SN_RECLAIMING)
        {
        SN_LOGMSG("someone else is busy reclaiming\n",0,0,0,0,0,0);
        return;
        }

    /* Close reclaiming to all others */
    pDrvCtrl->flags |= SN_RECLAIMING;

    pTXD = pDrvCtrl->pTXDReclaim;                  /* get ptr to desc */

    CACHE_DMA_INVALIDATE (&pTXD->status, sizeof (u_long));


    /*
     * The device is deemed to be done with the descriptor if the
     * the descriptor had been flagged as "given" to the device,
     * and the descriptor status field is set.
     */

    /*
     * Check if descriptor was used status may return 0 if xmit error 
     * occurred 
     */

    while ((pTXD->flag == IN_USE) &&
	   ((pTXD->status & PTX) || !(pDrvCtrl->pDev->cr & TXP)))
        {

        if ( pTXD->status & PTX )
            {
            /* Packet was transmitted successfully. *
	    * Release the data buffer(s) */

	    /* Release buffer associated with each fragment in the TXD */
	    (pDrvCtrl->freeBuf[pTXD->number].pfreeRtn) 
	                      (pDrvCtrl->freeBuf[pTXD->number].pSpare1,
	                       pDrvCtrl->freeBuf[pTXD->number].pSpare2);

            /* done with the desc, so clean it up */
            pTXD->flag    = NOT_IN_USE;
            pTXD->status  = 0;
            }

        else 
            {     /* ERROR! */

	    END_M2_OUTERRORS (&pDrvCtrl->endData);

	    intLevel = intLock();

	    pTXD->status = 0;

	    /* Make sure LCAM bit is not set or we'll lock up */
	    while (pDrvCtrl->pDev->cr & LCAM)
	        continue;

            pDrvCtrl->pDev->cr = TXP;           /* restart transmitter */

	    intUnlock (intLevel);

	    /* Get out now! */
            break;	

	    }/* ERROR */

        /* Advance to the next TXD. */
        pTXD = (TX_DESC *)((u_long)pTXD->pLink & ~TX_EOL);

        /* Update the reclaim pointer for next time. */
        pDrvCtrl->pTXDReclaim = pTXD;

	/* Check if we've taken too long */
	if (numReclaims++ ==  maxReclaims)
	    break;

        /* invalidate the cache for this TXD */
        CACHE_DMA_INVALIDATE (&pTXD->status, sizeof (pTXD->status));

        } /* while */

    pDrvCtrl->flags &= ~SN_RECLAIMING;

    }



/*============================================================================*
 *		R E C E I V E   R O U T I N E S
 *============================================================================*/

/****************************************************************************
*
* sn83932PollRecv - Returns the next packet waiting to be processed.  If no
*	packet exists, EAGAIN indication is returned instead.
*
* RETURNS:  OK or EAGAIN
*
*/

LOCAL STATUS sn83932PollRecv
    (
    DRV_CTRL *pDrvCtrl,
    M_BLK_ID pMblk 
    )

    {
     

    /* Sanity Check */
    if (pDrvCtrl == NULL || !(pDrvCtrl->flags & SN_POLLING))
	return EINVAL;


    if (pMblk == NULL || pMblk->mBlkHdr.mData == NULL)
	return (EINVAL);
     

    /* Process the received packet and give it to the mux */
    if (snProcessRxPkts (pDrvCtrl, 1, pMblk) == 0)
	return EAGAIN;

    else
	return OK;

    }




/*******************************************************************************
*
* snProcessRxPkts - Process the received packets up to the maximum
*	as indicated by the supplied argument.  For each recieved
*	indication, the packet is extracted, the appropriate MIB variables
*	are updated and the packet is then sent up to the Mux.  This
*	device does not support buffer loaning and therefore is not
*	utilized.  Once the packet is sent to the mux, the receive
*	descriptor pointers are adjusted appropriately.
*
*	RETURNS	-  Number of packets processed exclusive of errors 
*		  
*/	

LOCAL int snProcessRxPkts 
    (
    DRV_CTRL * pDrvCtrl,	        /* ptr to device's control info */
    int maxPackets,		/* Max number of packets to process */
    M_BLK_ID pMblk 		/* Optional net buffer for packet */
    )

    {
    END_OBJ *pEnd;			/* ptr to our End structure */
    RX_DESC *pRXD;			/* ptr to current descriptor */
    SONIC *pDev;                       	/* ptr to the device regs */
    char *pPktHdr;			/* ptr to packet */
    int recvLen;			/* size of packet received */
    int ndx;                           	/* index into RRA desc array */
    int packetCount;			/* num of packets processed */
    int POLL_MODE=1;                    /* if in Poll mode */
    CL_BLK_ID pClBlk;                   /* cluster block pointer */
    UCHAR *    pCluster;                 /* cluster pointer */
    BOOL error=FALSE;                   /* set in case of bad packet or out of
					mblks, cluster blks or cluster */

    /* Note the devices's End address */
    pEnd = &pDrvCtrl->endData;

    /* check if NOT Poll mode */
    if(pMblk==NULL)
    POLL_MODE=0;

    /* If a buffer is provided, then only one packet may be processed  */
    if (pMblk != NULL)
	maxPackets = 1; 

#ifdef LED_DEBUG
    if (!(pDrvCtrl->flags & SN_POLLING))
        PutLED ('r',1);
#endif

    /* Process all received packets up to the supplied MAX */
    for (packetCount = 0; packetCount < maxPackets; )
        {
        recvLen = 0;
        /* get the next receive descriptor */
    	pRXD = pDrvCtrl->pRXDNext;

    	CACHE_DMA_INVALIDATE (pRXD, RX_DESC_SIZ);

        /* Check if next slot has a packet */
        if (pRXD->in_use == IN_USE)
	    {
#ifdef DEBUG
   SN_LOGMSG("descriptor in use\n",0,0,0,0,0,0); 
#endif
            break; 
            }

    	/* Check for any error in reception */
    	if (!(pRXD->status & PRX))
    	    { /* BAD PACKET */

	    /*SL* Update MIB error count */
	    END_M2_INERRORS (pEnd);
#ifdef DEBUG
    SN_LOGMSG("bad packet\n",0,0,0,0,0,0);
#endif
            }/* BAD PACKET */

        else
    	    {    /* GOOD PACKET */

    	    /* Get packet length */
   	    recvLen  = (pRXD->byte_count & UMASK) - 4;
     
            pPktHdr = (char *)
                   (((pRXD->pkt_ptr1 & UMASK)<<16) | (pRXD->pkt_ptr0 & UMASK));

    	    CACHE_DMA_INVALIDATE (pPktHdr, recvLen);

	    if (!POLL_MODE)
	        { /* NOT POLL MODE */

                /* Obtain the appropriate cluster depending on size of 
		 * received packet 
		 */
		    
		if(recvLen <= OPTIMAL_CLUSTER_SIZE)
		    {
  	            if((pCluster= (UCHAR *)netClusterGet(pEnd->pNetPool,
		                             pDrvCtrl->pClPoolId[0])) == NULL)
		        {
#ifdef DEBUG
    SN_LOGMSG("out of clusters in the first pool\n",0,0,0,0,0,0);
#endif
                        }
                    }

                /* if packet size greater than optimal size or out of 
	         * optimal size cluster 
		 */
	     	if(recvLen > OPTIMAL_CLUSTER_SIZE || pCluster == NULL)
		    {
		    pCluster= (UCHAR *) netClusterGet(pEnd->pNetPool,
		    pDrvCtrl->pClPoolId[1]);
                    }

	        if(pCluster==NULL)
	            {
#ifdef DEBUG
    SN_LOGMSG("cannot allocate cluster\n",0,0,0,0,0,0);
#endif
	            error=TRUE;
                    }

                /* obtain a cluster blk */
	        if((!error) && (pClBlk= netClBlkGet(pEnd->pNetPool,
		                M_DONTWAIT))==NULL)
	            {
		    netClFree(pEnd->pNetPool,pCluster);
#ifdef DEBUG
    SN_LOGMSG("Out of Cluster Blocks\n",0,0,0,0,0,0);
#endif
                    error=TRUE;
	            }

                /* obtain a mblk */
	        if((!error) && (pMblk = mBlkGet(pEnd->pNetPool,M_DONTWAIT,
		                                MT_DATA))==NULL)
	            {
	            netClBlkFree(pEnd->pNetPool,pClBlk);
	            netClFree(pEnd->pNetPool,pCluster);
#ifdef DEBUG
    SN_LOGMSG("Out of M blocks\n",0,0,0,0,0,0);
#endif
	            error=TRUE;
	            }

                if(!error)
		    {
                    /* Joining the cluster to the MBlock */
	            netClBlkJoin(pClBlk,(char *)pCluster,recvLen,NULL,0,0,0);
	            netMblkClJoin(pMblk,pClBlk);
                    pMblk->m_data=pMblk->m_data+2;
#ifdef DEBUG
    SN_LOGMSG("length of packet received =%d\n",recvLen,0,0,0,0,0); 
#endif
		    }

            	} /* NOT POLL MODE */
 
	    else
	        {
                pMblk->mBlkHdr.mData = pMblk->pClBlk->clNode.pClBuf;
		pMblk->mBlkHdr.mData +=2;
	        }  

	    if(!error)
	        {
                pMblk->mBlkHdr.mLen= recvLen;
	        pMblk->mBlkHdr.mFlags |= M_PKTHDR;
	        pMblk->mBlkPktHdr.len =recvLen;
	        bcopy((char *) CACHE_DMA_PHYS_TO_VIRT (pPktHdr),
		      (char *) pMblk->m_data,recvLen);
                /*SL*  Update MIB2 stats here */
                END_M2_INOCTETS (pEnd, recvLen);

                if (pRXD->status & (MC | BC))
                    END_M2_INNUCAST (pEnd);
                else
                    END_M2_INUCAST (pEnd);
	        } 

            } /* GOOD PACKET */

        /* Update the next receive slot */
	pDrvCtrl->pRXDNext = (RX_DESC *) CACHE_DMA_PHYS_TO_VIRT (pRXD->link);

        /* Make it the last descriptor in the chain */
	pDrvCtrl->pRXDLast->link = (RX_DESC *) CACHE_DMA_VIRT_TO_PHYS (pRXD);
	pDrvCtrl->pRXDLast = pRXD;

        /* Check if this was the last pkt in this buffer*/
        if (pRXD->status & LPKT)
            {/* form index to RRA descriptor this packet associates with */

#ifdef DEBUG	    
    SN_LOGMSG("last packet in the buffer\n",0,0,0,0,0,0); 
#endif
	    ndx = (((pRXD->seq_no & 0x0000ff00) >> 8) + 1) % NUM_RRA_DESC;
       	    pDev = pDrvCtrl->pDev;
	    pDev->rwp = (u_long)(&((RRA_DESC *)pDrvCtrl->RSA)[ndx]) & UMASK;
	    if (pDev->isr & RBE)                /* deal with RBE condition */
		{
                pDev->isr = RBE;                /* clear the RBE condition */
#ifdef DEBUG
    SN_LOGMSG ("RBE\n", 0,0,0,0,0,0);
#endif 
	        }
            }

        /* Give control of this descriptor back to the device */
        pRXD->status = 0;
        pRXD->in_use = IN_USE;                   /* Set to IN_USE by device */
        pRXD->link = RX_EOL;                     /* Last one */

      
        /* in case of bad packet or out of clusters or cluster blks or mblks */
        if(recvLen==0 || error)
	    break;

        /* send the packet up the stack if NOT Poll mode */
        if(!POLL_MODE)
            END_RCV_RTN_CALL (pEnd, pMblk);

        /* increment the packet count */
        packetCount++;

        }/* for */

        return (packetCount);
    }


 /*====================================================================*
  *			E V E N T   R O U T I N E S
  *====================================================================*/

/*******************************************************************************
*
* sn83932Int - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*
* NOMANUAL
* RETURNS: N/A.
*/

LOCAL void sn83932Int
    (
     DRV_CTRL  * pDrvCtrl
    )
    {
    SONIC *pDev;				/* ptr to the device regs */

#ifdef LED_DEBUG
    if (pDrvCtrl->flags & SN_POLLING)
	PutLED ('I', 0);
#endif

    /* Point to the SONIC device registers */
    pDev = pDrvCtrl->pDev;

    /*** Check for bus retry ***/

    if (pDev->isr & BR)
        {
	pDev->isr = BR;

#ifdef DEBUG
	logMsg ("Bus Retry (int)\n", 0,0,0,0,0,0);
#endif
	}

    /* Check for a timer interrupt */
    if ((pDev->isr & pDev->imr) & TCEN)
	{/* Timer Interrupt */

#ifdef LED_DEBUG
	PutLED ('T', 3);
#endif

	if (pDrvCtrl->timerIntHandler)

	    {/* Call handler if it exists */
	    pDev->cr = STP;
	    pDev->isr = TCEN;
	    (*pDrvCtrl->timerIntHandler)();
	    pDev->wt0 = pDrvCtrl->ticks & 0xffff;
	    pDev->wt1 = pDrvCtrl->ticks >> 16;
	    pDev->cr = ST;
	    }
	}


    /*----------------------------------------*/
    /* Check for any packet (data) interrupts */
    /*----------------------------------------*/

    if ((pDev->isr & pDev->imr) & SN_IMR_DATA)
	
	{

        /* disable further data interrupts */
	pDev->imr = (pDrvCtrl->imr & ~SN_IMR_DATA);

        /* Queue event handler for the netTask */
        (void) netJobAdd ((FUNCPTR)snEventHandler,
                          (int)pDrvCtrl, 0, 0, 0, 0);
	}



    sysEnetIntAck (pDrvCtrl->unit);                /* do system int ack */

    }


/*******************************************************************************
*
* snEventHandler - device event handler, a netTask job
*/

LOCAL void snEventHandler
    (
     DRV_CTRL * pDrvCtrl
    )

    {
     SONIC *pDev;                         /* ptr to the device regs */

    pDev = pDrvCtrl->pDev;

#ifdef LED_DEBUG
    PutLED ('v',3);
#endif

    /* Loop, processing events */
    while ( pDev->isr & SN_IMR_DATA )
        {

        /*** Process all received packets ***/

        if (pDev->isr & PKTRX)
            {
	    /* clear this interrupt */
            pDev->isr = PKTRX;                

#ifdef LED_DEBUG
    PutLED ('r',3);
#endif
	    /* Process all received packets */
            snProcessRxPkts (pDrvCtrl, RX_PROCESS_FLOW, 0);
            }

        /*** Process transmitter events ***/

        if ( pDev->isr & (TXDN | TXER) )
            {
	    /* Clear transmit interrupts */
            pDev->isr = (TXDN | TXER);

#ifdef LED_DEBUG
    PutLED ('x',3);
#endif
	    /* Process used transmit descriptor */
            snTxReclaim (pDrvCtrl, TX_PROCESS_FLOW);
            if (pDrvCtrl->txBlocked)
                {
                netJobAdd ((FUNCPTR)muxTxRestart, (int)&pDrvCtrl->endData,
		0, 0, 0, 0);
                pDrvCtrl->txBlocked = FALSE;
                }
            }

        }

#ifdef LED_DEBUG
    PutLED ('V',3);
#endif


    /* Enable the interrupts */

    pDev->imr = pDrvCtrl->imr;

    }



/*===========================================================================*
 *			T I M E R   R O U T I N E S
 *===========================================================================*/

/*******************************************************************************
*
* snEndClkEnable - enable the SONIC timer interrupt
*
* This routine enables the SONIC timer to give a periodic interrupt.
* The <unit> parameter selects the SONIC device.  The <ticksPerSecond>
* parameter specifies the number of interrupts to generate per second.
* When an interrupt occurs, the function in <intHandler> is called.
*
* NOTE:
* The SONIC clock interrupt is dependent on the SONIC interface being
* configured and enabled.  For this reason, this interrupt is unsuitable
* for use as the main system clock.  It is suitable for use as an auxiliary
* clock source for boards that have no other source of clock.
*
* RETURNS: N/A
*
* SEE ALSO: snEndClkDisable()
*
* NOMANUAL
*/

void snEndClkEnable
    (
    int unit,			/* unit number */
    int ticksPerSecond,		/* interrupt frequency */
    FUNCPTR intHandler		/* interrupt handler */
    )
    {
    DRV_CTRL *pDrvCtrl;
    SONIC *pDev;
    int key;

    pDrvCtrl = (DRV_CTRL *)endFindByName (SN_DEV_NAME, unit);
    if (pDrvCtrl == NULL)
        {
        SN_LOGMSG ("Device entry not found for unit %d \n",unit,0,0,0,0,0);
        }
    else
        {
        pDev = pDrvCtrl->pDev;

        key = intLock ();

        pDrvCtrl->timerIntHandler = intHandler;
        pDrvCtrl->ticks = 5000000 / ticksPerSecond;
        pDrvCtrl->imr |= TCEN;

        if (pDev)
	    {
	    pDev->cr = STP;
	    pDev->wt0 = pDrvCtrl->ticks & 0xffff;
	    pDev->wt1 = pDrvCtrl->ticks >> 16;
	    pDev->imr |= TCEN;
	    pDev->cr = ST;
	    }
        intUnlock (key);
        }
    }

/*******************************************************************************
*
* snEndClkDisable - disable the SONIC timer interrupt
*
* This routine disables the SONIC timer interrupt.  The <unit> parameter
* selects the SONIC device.
*
* RETURNS: N/A
*
* SEE ALSO: snEndClkEnable()
*
* NOMANUAL
*/

void snEndClkDisable
    (
    int unit			/* unit number */
    )
    {
    DRV_CTRL *pDrvCtrl;
    SONIC *pDev;
    int key;


    pDrvCtrl = (DRV_CTRL *)endFindByName (SN_DEV_NAME, unit);
    if (pDrvCtrl == NULL)
        {
        SN_LOGMSG ("Device entry not found for unit %d \n",unit,0,0,0,0,0);
        }
    else
        {
        pDev = pDrvCtrl->pDev;
        if (pDev)
	    {
	    key = intLock ();
	    pDev->imr &= ~TCEN; 
	    pDrvCtrl->imr &= ~TCEN;
	    pDev->cr = STP;
	    intUnlock (key);
	    }
        }
    }



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * SECTION: NOTES
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*

NOTES:
    There are some things unsupported in this version:
    a) buffer loaning on receive
    b) output hooks
    c) trailer protocol
    d) promiscuous mode		*SL*  Supported in END Model *SL*

    Also, the receive setup needs work so that the number of RRA descriptors
    is not fixed at four.  It would be a nice addition to allow all the
    sizes of the shared memory structures to be specified by the runtime
    functions that call our init routines.

*/

/* END OF FILE */
