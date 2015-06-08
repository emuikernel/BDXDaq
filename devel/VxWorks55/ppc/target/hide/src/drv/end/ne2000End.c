/* ne2000End.c - NE2000 END network interface driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01o,24jan02,rcs  added support for user setting the "Configuration Register A"
                 or "Configuration Register B" from the load end string.
01n,14jan02,dat  Removing warnings from Diab compiler
01m,05dec00,dat  merge from sustaining to tor2_0_x
01l,30nov00,pai  corrected the handling of EIOCSFLAGS ioctl (SPR #29423).
01k,30nov00,pai  IFF_UP flag set in ne2000Start routine (SPR #32034, 34433).
01j,10may00,pai  Discontinued use of splnet() (SPR #28366).  The ne2000Send()
                 routine now returns END_ERR_BLOCK instead of ERROR so that
                 upper-level protocols will clean up passed in mBlk.
01i,28jul99,jkf  cleaned up warnings, fixed multicast support. (SPR#27921)
01h,31mar99,sbs  changed atoi to strtoul for offset parameter parsing.
                 (SPR #26208)
01g,29mar99,dat  SPR 26119, documentation, usage of .bS/.bE
01f,09feb99,fle  doc : fixed a problem with end of library description
01e,27jan99,dat  removed most of the lint. (more work needed)
01d,11aug98,mem  rewrote part of the overwrite recovery handling to
		 avoid locking up the driver.
01c,20may98,mem  added additional debug tracing, removed free of
		 pMblk in ne2000PollSend().
01b,06may98,mem  integrated changes from Brian Nash.
01a,16mar98,mem	 written, based on drv/netif/if_ene.c.
*/

/*
DESCRIPTION

This module implements the NE2000 Ethernet network interface driver.

EXTERNAL INTERFACE
The only external interface is the ne2000EndLoad() routine, which expects
the <initString> parameter as input.  This parameter passes in a
colon-delimited string of the format:

<unit>:<adrs>:<vecNum>:<intLvl>:<byteAccess>:<usePromEnetAddr>:<offset>

The ne2000EndLoad() function uses strtok() to parse the string.

TARGET-SPECIFIC PARAMETERS
.IP <unit>
A convenient holdover from the former model.  This parameter is used only
in the string name for the driver.
.IP <adrs>
Tells the driver where to find the ne2000.
.IP <vecNum>
Configures the ne2000 device to generate hardware interrupts
for various events within the device. Thus, it contains
an interrupt handler routine.  The driver calls sysIntConnect() to connect
its interrupt handler to the interrupt vector generated as a result of
the ne2000 interrupt.
.IP <intLvl>
This parameter is passed to an external support routine, sysLanIntEnable(),
which is described below in "External Support Requirements." This routine
is called during as part of driver's initialization.  It handles any
board-specific operations required to allow the servicing of a ne2000
interrupt on targets that use additional interrupt controller devices to
help organize and service the various interrupt sources.  This parameter
makes it possible for this driver to avoid all board-specific knowledge of
such devices.
.IP <byteAccess>
Tells the driver the NE2000 is jumpered to operate in 8-bit mode.
Requires that SYS_IN_WORD_STRING() and SYS_OUT_WORD_STRING() be
written to properly access the device in this mode.
.IP <usePromEnetAddr>
Attempt to get the ethernet address for the device from the on-chip
(board) PROM attached to the NE2000.  Will fall back to using the
BSP-supplied ethernet address if this parameter is 0 or if unable to
read the ethernet address.
.IP <offset>
Specifies the memory alignment offset.

EXTERNAL SUPPORT REQUIREMENTS
This driver requires several external support functions, defined as macros:
.CS
    SYS_INT_CONNECT(pDrvCtrl, routine, arg)
    SYS_INT_DISCONNECT (pDrvCtrl, routine, arg)
    SYS_INT_ENABLE(pDrvCtrl)
    SYS_IN_CHAR(pDrvCtrl, reg, pData)
    SYS_OUT_CHAR(pDrvCtrl, reg, pData)
    SYS_IN_WORD_STRING(pDrvCtrl, reg, pData)
    SYS_OUT_WORD_STRING(pDrvCtrl, reg, pData)
.CE

These macros allow the driver to be customized for BSPs that use
special versions of these routines.

The macro SYS_INT_CONNECT is used to connect the interrupt handler to
the appropriate vector.  By default it is the routine intConnect().

The macro SYS_INT_DISCONNECT is used to disconnect the interrupt handler prior
to unloading the module.  By default this is a dummy routine that
returns OK.

The macro SYS_INT_ENABLE is used to enable the interrupt level for the
end device.  It is called once during initialization.  By default this is
the routine sysLanIntEnable(), defined in the module sysLib.o.

The macro SYS_ENET_ADDR_GET is used to get the ethernet address (MAC)
for the device.  The single argument to this routine is the END_DEVICE
pointer.  By default this routine copies the ethernet address stored in
the global variable ne2000EndEnetAddr into the END_DEVICE structure.

The macros SYS_IN_CHAR, SYS_OUT_CHAR, SYS_IN_WORD_STRING and
SYS_OUT_WORD_STRING are used for accessing the ne2000 device.  The
default macros map these operations onto sysInByte(), sysOutByte(),
sysInWordString() and sysOutWordString().

INCLUDES:
end.h endLib.h etherMultiLib.h

SEE ALSO: muxLib, endLib
.I "Writing and Enhanced Network Driver"

INTERNAL: We are using the DP83905 manual as our reference. It is 
NE2000 compatible.

*/

#include "vxWorks.h"
#include "endLib.h"			/* Common END structures. */
#include "etherMultiLib.h"
#include "netLib.h"
#include "cacheLib.h"
#include "lstLib.h"			/* Needed to maintain protocol list. */
#include "iv.h"
#include "stdlib.h"
#include "sysLib.h"
#include "intLib.h"
#include "taskLib.h"
#include "drv/end/ne2000End.h"		/* Common defines. */


/***** LOCAL DEFINITIONS *****/

#define NE2000_ALL_INTS    (IM_OVWE | IM_TXEE | IM_RXEE | IM_PTXE | IM_PRXE)


/*
 * AUTODIN-II, FDDI, ethernet polynomial for 32 bit CRC.
 *
 * The polynomial is expressed:
 *
 *   ( x^32 + x^26 + x^23 + x^22 + x^16 +
 *     x^12 + x^11 + x^10 + x^8  + x^7  +
 *            x^5  + x^4  + x^2  + x^1  + 1 ) = 0x04c11db7
 *
 * Where x = base 2 (binary) and x^32 is ignored for CRC.
 * (x^32, is not in 32 bit value (ends at x^31)
 */

#define CRC32_POLYNOMIAL		0x04c11db7


/* Configuration items */

#define END_SPEED		10000000

/* Naming items */
#define NE2000_DEV_NAME		"ene"
#define NE2000_DEV_NAME_LEN	4

/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */

/* Macro to connect interrupt handler to vector */

#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult) \
	{ \
	IMPORT STATUS sysIntConnect(); \
	*pResult = intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (pDrvCtrl->ivec), \
			     rtn, (int)arg); \
	}
#endif

/* Macro to disconnect interrupt handler from vector */

#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl,rtn,arg,pResult) \
	{ \
	*pResult = OK; /* HELP: need a real routine */ \
	}
#endif

/* Macro to enable the appropriate interrupt level */

#ifndef SYS_INT_ENABLE
#define SYS_INT_ENABLE(pDrvCtrl) \
	{ \
	IMPORT void sysLanIntEnable(); \
	sysLanIntEnable (pDrvCtrl->ilevel); \
	}
#endif

/* Macro to get the ethernet address from the BSP */

#ifndef SYS_ENET_ADDR_GET
#define SYS_ENET_ADDR_GET(pDevice) \
	ne2000EnetAddrGet(pDevice)
#endif

/*
 * Macros to do a short (UINT16) access to the chip. Default
 * assumes a normal memory mapped device.
 * CPU_FAMILY is used to avoid conflict between x86 and non
 * x86 defintitions (proper) of these macro's.  This is
 * to avoid some compiler warnings.
 */

#if     (CPU_FAMILY == I80X86)

/* sysLib.h uses two declarations of sys<Blah>WordString */

#ifndef SYS_OUT_CHAR
#define SYS_OUT_CHAR(pDrvCtrl,addr,value) \
	sysOutByte((pDrvCtrl)->base + (int) (addr),(char) (value))
#endif

#ifndef SYS_IN_CHAR
#define SYS_IN_CHAR(pDrvCtrl,addr,pData) \
	(*(pData) = (UCHAR) sysInByte((pDrvCtrl)->base + (int) (addr)))
#endif

/* sysLib.h head setsup to declarations of sys<Blah>WordString */
#ifndef SYS_IN_WORD_STRING
#define SYS_IN_WORD_STRING(pDrvCtrl,addr,pData,len) \
	(sysInWordString ((pDrvCtrl)->base + (int) (addr), (short *)(pData), \
			  (len)))
#endif

#ifndef SYS_OUT_WORD_STRING
#define SYS_OUT_WORD_STRING(pDrvCtrl,addr,pData,len) \
	(sysOutWordString ((pDrvCtrl)->base + (int) (addr), (short *)(pData), \
			   (len)))
#endif

#else /* #if     (CPU_FAMILY == I80X86) */

#ifndef SYS_OUT_CHAR
#define SYS_OUT_CHAR(pDrvCtrl,addr,value) \
    	sysOutByte((pDrvCtrl)->base + (UINT) (addr), (value))
#endif

#ifndef SYS_IN_CHAR
#define SYS_IN_CHAR(pDrvCtrl,addr,pData) \
    	(*(pData) = (UCHAR) sysInByte((pDrvCtrl)->base + (UINT) (addr)))
#endif

#ifndef SYS_IN_WORD_STRING
#define SYS_IN_WORD_STRING(pDrvCtrl,addr,pData,len) \
    	(sysInWordString ((pDrvCtrl)->base + (UINT) (addr),  \
			  (UINT16 *)(pData), (len)))
#endif

#ifndef SYS_OUT_WORD_STRING
#define SYS_OUT_WORD_STRING(pDrvCtrl,addr,pData,len) \
    	(sysOutWordString ((pDrvCtrl)->base + (UINT) (addr), \
			   (UINT16 *) (pData), (len)))
#endif

#endif /* #if     (CPU_FAMILY == I80X86) */

/* A shortcut for getting the hardware address from the MIB II stuff. */

#define END_HADDR(pEnd)	\
		((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
		((pEnd)->mib2Tbl.ifPhysAddress.addrLength)


/* Statistics we gather from the NE2000 */

typedef struct
    {
    UINT collisions;
    UINT crcs;
    UINT aligns;
    UINT missed;
    UINT overruns;
    UINT disabled;
    UINT deferring;
    UINT underruns;
    UINT aborts;
    UINT outofwindow;
    UINT heartbeats;
    UINT badPacket;
    UINT shortPacket;
    UINT tnoerror;
    UINT rnoerror;
    UINT terror;
    UINT rerror;
    UINT overwrite;
    UINT wrapped;
    UINT interrupts;
    UINT reset;
    UINT strayint;
    UINT jabber;
    } NE2000_STAT;

/*
 * The definition of the driver control structure.  The packetBuf[]
 * array needs to be kept at least word-aligned as it gets passed to
 * SYS_OUT_WORD_STRING() and some architectures cannot handle
 * misaligned accesses.
 */

typedef struct ne2000_device
    {
    END_OBJ		endObj;		/* The class we inherit from. */
    END_ERR		lastError;	/* Last error passed to muxError */
    int			lastIntError;	/* last interrupt signalled error */
    int			unit;		/* unit number */
    int			ivec;		/* interrupt vector */
    int			ilevel;		/* interrupt level */
    int			byteAccess;	/* 8-bit access mode */
    int			usePromEnetAddr;/* enet addr from PROM */
    ULONG		base;		/* base address */
    int			offset;		/* offset for memory alignment */
    char		packetBuf [NE2000_BUFSIZ];	/* long-aligned */
    volatile long	flags;		/* Our local flags. */
    volatile UCHAR	current;	/* current-page reg at interrupt */
    volatile ULONG	imask;		/* interrupt mask */
    volatile NE2000_STAT stats;		/* NE2000 stats */
    UCHAR		enetAddr[6];	/* ethernet address */
    UCHAR		mcastFilter[8];	/* multicast filter */
    UCHAR		nextPacket;	/* where the next received packet is */
    CL_POOL_ID		clPoolId;
    int                 configRegA;     /* configuration register A */
    int                 configRegB;     /* configuration register B */
} NE2000END_DEVICE;

/* Definitions for the flags field */

#define END_PROMISCUOUS_FLAG	0x01
#define END_RECV_HANDLING_FLAG	0x02
#define END_TX_IN_PROGRESS	0x04
#define END_TX_BLOCKED		0x08
#define END_POLLING		0x10
#define END_OVERWRITE		0x20
#define END_OVERWRITE2		0x40

/***** DEBUG MACROS *****/

#undef DEBUG

#ifdef DEBUG
#   include "stdio.h"
#   include "logLib.h"
    int endDebug = 0;
#   define ENDLOGMSG(x) \
	do { \
	    if (endDebug) \
		logMsg x; \
	   } while (0)
#else
#   define ENDLOGMSG(x)
#endif /* ENDDEBUG */

/***** LOCALS *****/

/* imports */

IMPORT char ne2000EnetAddr[];
IMPORT int endMultiLstCnt (END_OBJ *);

/* forward static functions */

LOCAL void	ne2000Int (NE2000END_DEVICE* pDrvCtrl);
LOCAL void	ne2000HandleRcvInt (NE2000END_DEVICE* pDrvCtrl);
LOCAL void	ne2000Config (NE2000END_DEVICE* pDrvCtrl,
			      int intEnable);
LOCAL void	ne2000OverwriteRecover (NE2000END_DEVICE* pDrvCtrl,
					UCHAR cmdStatus);
LOCAL int	ne2000PacketGet (NE2000END_DEVICE* pDrvCtrl, char *pData);
LOCAL void	ne2000AddrFilterSet (NE2000END_DEVICE* pDrvCtrl);
LOCAL UINT32 	ne2000CrcWork (UINT8 inChar, UINT32 inCrc);
LOCAL void	ne2000EnetAddrGet (NE2000END_DEVICE* pDrvCtrl);
LOCAL UCHAR	ne2000GetCurr (NE2000END_DEVICE* pDrvCtrl);
LOCAL void	ne2000DataIn (NE2000END_DEVICE* pDrvCtrl, int eneAddress,
			      int len, char* pData);
LOCAL void	ne2000DataOut (NE2000END_DEVICE* pDrvCtrl, char* pData,
			       int len, int eneAddress);
LOCAL STATUS	ne2000Parse (NE2000END_DEVICE* pDrvCtrl, char* initString);
LOCAL STATUS	ne2000MemInit (NE2000END_DEVICE* pDrvCtrl);
LOCAL STATUS	ne2000PollStart (NE2000END_DEVICE* pDrvCtrl);
LOCAL STATUS	ne2000PollStop (NE2000END_DEVICE* pDrvCtrl);


/* END Specific interfaces. */

/* This is the only externally visible interface. */

END_OBJ * 	ne2000EndLoad (char* initString, void *pBSP);

LOCAL STATUS	ne2000Start (void* pCookie);
LOCAL STATUS	ne2000Stop (void* pCookie);
LOCAL STATUS	ne2000Unload (void *pCookie);
LOCAL int	ne2000Ioctl (void *pCookie, int cmd, caddr_t data);
LOCAL STATUS	ne2000Send (void *pCookie, M_BLK_ID pBuf);
LOCAL STATUS	ne2000MCastAdd (void *pCookie, char* pAddress);
LOCAL STATUS	ne2000MCastDel (void *pCookie, char* pAddress);
LOCAL STATUS	ne2000MCastGet (void *pCookie, MULTI_TABLE* pTable);
LOCAL STATUS	ne2000PollSend (void *pCookie, M_BLK_ID pBuf);
LOCAL STATUS	ne2000PollRecv (void *pCookie, M_BLK_ID pBuf);

/*
 * Declare our function table.  This is static across all driver
 * instances.
 */
LOCAL NET_FUNCS ne2000FuncTable =
    {
    (FUNCPTR) ne2000Start,	/* Function to start the device. */
    (FUNCPTR) ne2000Stop,	/* Function to stop the device. */
    (FUNCPTR) ne2000Unload,	/* Unloading function for the driver. */
    (FUNCPTR) ne2000Ioctl,	/* Ioctl function for the driver. */
    (FUNCPTR) ne2000Send,	/* Send function for the driver. */
    (FUNCPTR) ne2000MCastAdd,	/* Multicast address add function for the */
				/* driver. */
    (FUNCPTR) ne2000MCastDel,	/* Multicast address delete function for */
				/* the driver. */
    (FUNCPTR) ne2000MCastGet,	/* Multicast table retrieve function for */
				/* the driver. */
    (FUNCPTR) ne2000PollSend,	/* Polling send function for the driver. */
    (FUNCPTR) ne2000PollRecv,	/* Polling receive function for the driver. */
    endEtherAddressForm, /* put address info into a NET_BUFFER */
    (FUNCPTR) endEtherPacketDataGet, /* get pointer to data in NET_BUFFER */
    (FUNCPTR) endEtherPacketAddrGet  /* Get packet addresses. */
    };

/******************************************************************************
*
* ne2000EndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device specific parameters are passed in the initString.
*
* The string contains the target specific parameters like this:
*
* "unit:register addr:int vector:int level:shmem addr:shmem size:shmem width"
*
* RETURNS: An END object pointer or NULL on error.
*/

END_OBJ* ne2000EndLoad
    (
    char* initString,		/* String to be parsed by the driver. */
    void* pBSP			/* for BSP group */
    )
    {
    NE2000END_DEVICE 	*pDrvCtrl;
    int 		level;
    UCHAR		regVal;

#ifdef DEBUG
    printf ("ne2000EndLoad(%s)\n", initString);
    printf ("\tinitString '%s'\n", initString ? initString : "");
    printf ("\tpBSP       '%s'\n", pBSP ? pBSP : "");
#endif
    if (initString == NULL)
        return (NULL);

    if (initString[0] == '\0')
        {
        bcopy((char *)NE2000_DEV_NAME, initString, NE2000_DEV_NAME_LEN);
        return (NULL);
        }

    /* allocate the device structure */

    pDrvCtrl = (NE2000END_DEVICE *) calloc (sizeof(NE2000END_DEVICE), 1);
    if (pDrvCtrl == NULL)
	goto errorExit;

    /* parse the init string, filling in the device structure */

    if (ne2000Parse (pDrvCtrl, initString) == ERROR)
	goto errorExit;


    /*
     *  If the optional load string parameters configRegA or configRegB
     *  are passed in with the load string then load the passed in value
     *  into the appropriate register.  
     */

    if (pDrvCtrl->configRegA != 0)
        {
        /* This must be an atomic transaction, so it must be intLocked */

        level = intLock ();

        SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_PAGE0);
        SYS_IN_CHAR (pDrvCtrl, ENE_RBCR0, &regVal);
        SYS_OUT_CHAR (pDrvCtrl, ENE_RBCR0, pDrvCtrl->configRegA);

        intUnlock (level);
        }                 

    if (pDrvCtrl->configRegB != 0)
        {
        /* This must be an atomic transaction, so it must be intLocked */

        level = intLock ();

        SYS_IN_CHAR (pDrvCtrl, ENE_RBCR1, &regVal);
        SYS_OUT_CHAR (pDrvCtrl, ENE_RBCR1, pDrvCtrl->configRegB);

        intUnlock (level);
        }         

    /* stop device */

    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_NODMA | CMD_PAGE0 | CMD_STOP);

    /* 
     * HELP:
     * In the generic driver it always uses BUS16 here. I agree that this
     * code looks better, but if the device is inactive why are we doing this
     * here at all.  Why not call ne2000Config from ne2000Start and let it set
     * up the chip when we actually start it ???
     */

    if (pDrvCtrl->byteAccess)
	SYS_OUT_CHAR (pDrvCtrl, ENE_DCON, DCON_BSIZE1 | DCON_BUS_8
			| DCON_LOOPBK_OFF);
    else
	SYS_OUT_CHAR (pDrvCtrl, ENE_DCON, DCON_BSIZE1 | DCON_BUS16 
			| DCON_LOOPBK_OFF);

    /* Ask the BSP to provide the ethernet address. */

    SYS_ENET_ADDR_GET (pDrvCtrl);

    /* initialize the END parts of the structure */

    if (END_OBJ_INIT (&pDrvCtrl->endObj, (DEV_OBJ *)pDrvCtrl, NE2000_DEV_NAME,
		      pDrvCtrl->unit, &ne2000FuncTable,
		      "ne2000 Enhanced Network Driver") == ERROR)
	goto errorExit;

    /* initialize the MIB2 parts of the structure */

    if (END_MIB_INIT (&pDrvCtrl->endObj, M2_ifType_ethernet_csmacd,
		      &pDrvCtrl->enetAddr[0], 6, ETHERMTU,
                      END_SPEED) == ERROR)
	goto errorExit;

    /* Perform memory allocation/distribution */

    if (ne2000MemInit (pDrvCtrl) == ERROR)
	goto errorExit;

    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->endObj,
		    IFF_NOTRAILERS | IFF_MULTICAST | IFF_BROADCAST);

    return (&pDrvCtrl->endObj);

errorExit:
    if (pDrvCtrl != NULL)
	free ((char *)pDrvCtrl);

    return (NULL);
    }

/******************************************************************************
*
* ne2000Parse - parse the init string
*
* Parse the input string.  Fill in values in the driver control structure.
*
* The initialization string format is:
* .CS
*   "unit:adrs:vecnum:intLvl:byteAccess:usePromEnetAddr:offset"
* .CE
*
* .IP <unit>
* Device unit number, a small integer.
* .IP <adrs>
* Base address
* .IP <vecNum>
* Interrupt vector number (used with sysIntConnect)
* .IP <intLvl>
* Interrupt level (used with sysLanIntEnable)
* .IP <byteAccess>
* Use 8-bit access mode.
* .IP <usePromEnetAddr>
* get ethernet address from PROM.
* .IP <offset>
* offset for memory alignment
* Set Configuration Register A. Defaults to reset value. 
* .IP <configRegA>
* Set Configuration Register B. Defaults to reset value. 
* .IP <configRegB>
* .LP
*
* RETURNS: OK or ERROR for invalid arguments.
*/

LOCAL STATUS ne2000Parse
    (
    NE2000END_DEVICE * pDrvCtrl,
    char * initString
    )
    {
    char*	tok;
    char*	holder = NULL;

    /* Parse the initString */

    /* Unit number. */

    tok = strtok_r (initString, ":", &holder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->unit = atoi (tok);

    /* Base address. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->base = strtoul (tok, NULL, 16);

    /* Interrupt vector. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->ivec = strtoul (tok, NULL, 16);

    /* Interrupt level. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->ilevel = strtoul (tok, NULL, 16);

    /* 8-bit access. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->byteAccess = atoi (tok);

    /* ethernet address from PROM. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->usePromEnetAddr = atoi (tok);

    /* memory alignment offset */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->offset = strtoul (tok, NULL, 16);

    /* Set configuration register A - optional parameter */
   
    pDrvCtrl->configRegA = 0;

    tok = strtok_r (NULL, ":", &holder);
    if (tok != NULL)
        pDrvCtrl->configRegA = strtoul (tok, NULL, 16);

    /* Set configuration register B - optional parameter */
   
    pDrvCtrl->configRegB = 0;

    tok = strtok_r (NULL, ":", &holder);
    if (tok != NULL)
        pDrvCtrl->configRegB = strtoul (tok, NULL, 16);

    return (OK);
    }

/*******************************************************************************
*
* ne2000MemInit - initialize memory for the chip
*
* This routine is highly specific to the device.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ne2000MemInit
    (
    NE2000END_DEVICE * pDrvCtrl	/* device to be initialized */
    )
    {
    M_CL_CONFIG	eneMclBlkConfig;
    CL_DESC	clDesc;                      /* cluster description */

    bzero ((char *)&eneMclBlkConfig, sizeof(eneMclBlkConfig));
    bzero ((char *)&clDesc, sizeof(clDesc));

    clDesc.clNum   = 32;
    clDesc.clSize  = NE2000_BUFSIZ;	/* allow for alignment */
    clDesc.memSize = ((clDesc.clNum * (clDesc.clSize + 8)) + 4);

    eneMclBlkConfig.mBlkNum  = 16 * 4;
    eneMclBlkConfig.clBlkNum = clDesc.clNum;

    /*
     * mBlk and cluster configuration memory size initialization
     * memory size adjusted to hold the netPool pointer at the head.
     */
    eneMclBlkConfig.memSize =
      (eneMclBlkConfig.mBlkNum * (MSIZE + sizeof (long)))
      + (eneMclBlkConfig.clBlkNum * (CL_BLK_SZ + sizeof (long)));
    eneMclBlkConfig.memArea = (char *) memalign(sizeof (long),
						eneMclBlkConfig.memSize);
    if (eneMclBlkConfig.memArea == NULL)
        return (ERROR);

    clDesc.memArea = (char *) malloc (clDesc.memSize);
    if (clDesc.memArea == NULL)
	return (ERROR);

    pDrvCtrl->endObj.pNetPool = (NET_POOL_ID) malloc (sizeof(NET_POOL));
    if (pDrvCtrl->endObj.pNetPool == NULL)
        return (ERROR);

    /* Initialize the net buffer pool with transmit buffers */
    if (netPoolInit (pDrvCtrl->endObj.pNetPool, &eneMclBlkConfig,
                     &clDesc, 1, NULL) == ERROR)
        return (ERROR);

    /* Save the cluster pool id */
    pDrvCtrl->clPoolId = clPoolIdGet (pDrvCtrl->endObj.pNetPool,
				      NE2000_BUFSIZ, FALSE);

    return (OK);
    }

/*******************************************************************************
*
* ne2000Ioctl - the driver I/O control routine
*
* Process an ioctl request.
*/

LOCAL int ne2000Ioctl
    (
    void*	pCookie,	/* device ptr */
    int		cmd,
    caddr_t	data
    )
    {
    int error = 0;
    long value;
    NE2000END_DEVICE* pDrvCtrl = (NE2000END_DEVICE *) pCookie;

    switch ((UINT) cmd)
        {
        case EIOCSADDR:
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)data, (char *)END_HADDR(&pDrvCtrl->endObj),
		   END_HADDR_LEN(&pDrvCtrl->endObj));
            break;

        case EIOCGADDR:
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)END_HADDR(&pDrvCtrl->endObj), (char *)data,
		   END_HADDR_LEN(&pDrvCtrl->endObj));
            break;

        case EIOCSFLAGS:
	    value = (long)data;
	    if (value < 0)
		{
		value = -value;
		value--;
		END_FLAGS_CLR (&pDrvCtrl->endObj, value);
		}
	    else
		END_FLAGS_SET (&pDrvCtrl->endObj, value);

	    ne2000Config (pDrvCtrl, TRUE);
            break;

        case EIOCGFLAGS:
	    *(int *)data = END_FLAGS_GET(&pDrvCtrl->endObj);
            break;

	case EIOCPOLLSTART:
	    ne2000PollStart (pDrvCtrl);
	    break;

	case EIOCPOLLSTOP:
	    ne2000PollStop (pDrvCtrl);
	    break;

        case EIOCGMIB2:
            if (data == NULL)
                return (EINVAL);
            bcopy((char *)&pDrvCtrl->endObj.mib2Tbl, (char *)data,
                  sizeof(pDrvCtrl->endObj.mib2Tbl));
            break;

        case EIOCGFBUF:
            if (data == NULL)
                return (EINVAL);
#if 0
	    /* XXX */
            *(int *)data = END_MIN_FBUF;
#endif
            break;

        default:
            error = EINVAL;
        }

    return (error);
    }

/*******************************************************************************
*
* ne2000Start - start the device
*
* This function calls BSP functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR
*
*/

LOCAL STATUS ne2000Start
    (
    void*	pCookie
    )
    {
    STATUS result;
    NE2000END_DEVICE* pDrvCtrl = (NE2000END_DEVICE *) pCookie;

    ENDLOGMSG (("ne2000Start\n", 0, 0, 0, 0, 0, 0));

    SYS_INT_CONNECT (pDrvCtrl, ne2000Int, (int)pDrvCtrl, &result);
    if (result == ERROR)
	return (ERROR);


    /* mark the interface as up (SPR #32034) */

    END_FLAGS_SET (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));


    /* Enable interrupts */

    pDrvCtrl->imask = (pDrvCtrl->flags & END_POLLING) ? 0 : NE2000_ALL_INTS;
    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);
    SYS_INT_ENABLE (pDrvCtrl);

    return (OK);
    }

/******************************************************************************
*
* ne2000Config - reconfigure the interface under us.
*
* Reconfigure the interface setting promiscuous mode, and changing the
* multicast interface list.
*
* NOMANUAL
*/
LOCAL void ne2000Config
    (
    NE2000END_DEVICE *pDrvCtrl,
    BOOL	      intEnable		/* TRUE to enable interrupts */
    )
    {
    UCHAR	rxFilter;		/* receiver configuration */
#ifdef DEBUG
    static buf [256];
#endif

    ENDLOGMSG (("ne2000Config: enter (intEnable=%d)\n",
		intEnable, 0, 0, 0, 0, 0));

    /* Disable device interrupts */

    pDrvCtrl->imask = 0;
    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);

    /* Set up address filter for multicasting. */

    if (END_MULTI_LST_CNT(&pDrvCtrl->endObj) > 0)
	ne2000AddrFilterSet (pDrvCtrl);

    /* preserve END_POLLING and OVERWRITE flags */
    pDrvCtrl->flags &= (END_POLLING | END_OVERWRITE | END_OVERWRITE2);

    /* 1. program Command Register for page 0 and for no DMA */

    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_NODMA | CMD_PAGE0 | CMD_STOP);

    /* 2. initialize Data Configuration Register:
     *    16-bit bus, burst mode, 8-deep FIFO.
     * or
     *    8-bit bus, burst mode, 8-deep FIFO.
     */

    if (pDrvCtrl->byteAccess)
	SYS_OUT_CHAR (pDrvCtrl, ENE_DCON, DCON_BSIZE1 | DCON_BUS_8 
			| DCON_LOOPBK_OFF);
    else
	SYS_OUT_CHAR (pDrvCtrl, ENE_DCON, DCON_BSIZE1 | DCON_BUS16 
			| DCON_LOOPBK_OFF);

    SYS_ENET_ADDR_GET (pDrvCtrl);

    /* 3. clear Remote Byte Count Register */

    SYS_OUT_CHAR (pDrvCtrl, ENE_RBCR0, 0x00);
    SYS_OUT_CHAR (pDrvCtrl, ENE_RBCR1, 0x00);

    /* 4. initialize Receive Configuration Register */

    /* Always accept broadcast packets. */

    rxFilter = RCON_BROAD;

    /* Set multicast mode if it's asked for. */

    if (END_MULTI_LST_CNT(&pDrvCtrl->endObj) > 0)
	{
	rxFilter |= RCON_GROUP;
	}
    ENDLOGMSG (("\tMulticast mode %s\n",
	    (rxFilter & RCON_GROUP) ? "on" : "off",
	    0, 0, 0, 0, 0));

    /* Set promiscuous mode if it's asked for. */

    if (END_FLAGS_GET(&pDrvCtrl->endObj) & IFF_PROMISC)
	{
	rxFilter |= RCON_PROM;
	}
    ENDLOGMSG (("\tPromiscuous mode %s\n",
	    (rxFilter & RCON_PROM) ? "on" : "off",
	    0, 0, 0, 0, 0));

    ENDLOGMSG (("\tsetting rxFilter = 0x%x\n", rxFilter,2,3,4,5,6));

    SYS_OUT_CHAR (pDrvCtrl, ENE_RCON, rxFilter);

    /* 5. place the ENE in LOOPBACK mode 1 or 2 */

    SYS_OUT_CHAR (pDrvCtrl, ENE_TCON, TCON_LB1);

    /* 6. initialize Receive Buffer Ring */

    SYS_OUT_CHAR (pDrvCtrl, ENE_RSTART, NE2000_PSTART);
    SYS_OUT_CHAR (pDrvCtrl, ENE_RSTOP, NE2000_PSTOP);
    SYS_OUT_CHAR (pDrvCtrl, ENE_BOUND, NE2000_PSTART);

    /* 7. clear Interrupt Status Register */

    SYS_OUT_CHAR (pDrvCtrl, ENE_INTSTAT, (char)0xff);

    /* 8. initialize Interrupt Mask Register */

    pDrvCtrl->imask = 0;
    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);

    /* 9. program Command Register for page 1 */

    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_PAGE1 | CMD_STOP);

    /* i) initialize physical address registers */

    SYS_OUT_CHAR (pDrvCtrl, ENE_STA0, pDrvCtrl->enetAddr[0]);
    SYS_OUT_CHAR (pDrvCtrl, ENE_STA1, pDrvCtrl->enetAddr[1]);
    SYS_OUT_CHAR (pDrvCtrl, ENE_STA2, pDrvCtrl->enetAddr[2]);
    SYS_OUT_CHAR (pDrvCtrl, ENE_STA3, pDrvCtrl->enetAddr[3]);
    SYS_OUT_CHAR (pDrvCtrl, ENE_STA4, pDrvCtrl->enetAddr[4]);
    SYS_OUT_CHAR (pDrvCtrl, ENE_STA5, pDrvCtrl->enetAddr[5]);

    ENDLOGMSG (("enet addr %02x:%02x:%02x:%02x:%02x:%02x\n",
		pDrvCtrl->enetAddr[0] & 0xff,
		pDrvCtrl->enetAddr[1] & 0xff,
		pDrvCtrl->enetAddr[2] & 0xff,
		pDrvCtrl->enetAddr[3] & 0xff,
		pDrvCtrl->enetAddr[4] & 0xff,
		pDrvCtrl->enetAddr[5] & 0xff));

    /* ii) initialize multicast address registers */

    ne2000AddrFilterSet (pDrvCtrl);
    SYS_OUT_CHAR (pDrvCtrl, ENE_MAR0, pDrvCtrl->mcastFilter[0]);
    SYS_OUT_CHAR (pDrvCtrl, ENE_MAR1, pDrvCtrl->mcastFilter[1]);
    SYS_OUT_CHAR (pDrvCtrl, ENE_MAR2, pDrvCtrl->mcastFilter[2]);
    SYS_OUT_CHAR (pDrvCtrl, ENE_MAR3, pDrvCtrl->mcastFilter[3]);
    SYS_OUT_CHAR (pDrvCtrl, ENE_MAR4, pDrvCtrl->mcastFilter[4]);
    SYS_OUT_CHAR (pDrvCtrl, ENE_MAR5, pDrvCtrl->mcastFilter[5]);
    SYS_OUT_CHAR (pDrvCtrl, ENE_MAR6, pDrvCtrl->mcastFilter[6]);
    SYS_OUT_CHAR (pDrvCtrl, ENE_MAR7, pDrvCtrl->mcastFilter[7]);


#ifdef DEBUG
   ENDLOGMSG ((("Setting multicast addresses to:\n"),1,2,3,4,5,6));
    sprintf(buf, "enet mcast %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
	    pDrvCtrl->mcastFilter[0] & 0xff,
	    pDrvCtrl->mcastFilter[1] & 0xff,
	    pDrvCtrl->mcastFilter[2] & 0xff,
	    pDrvCtrl->mcastFilter[3] & 0xff,
	    pDrvCtrl->mcastFilter[4] & 0xff,
	    pDrvCtrl->mcastFilter[5] & 0xff,
	    pDrvCtrl->mcastFilter[6] & 0xff,
	    pDrvCtrl->mcastFilter[7] & 0xff);
    ENDLOGMSG (((buf),1,2,3,4,5,6));
#endif /* DEBUG */

    /* iii) initialize current page pointer */

    SYS_OUT_CHAR (pDrvCtrl, ENE_CURR, NE2000_PSTART + 1);
    pDrvCtrl->nextPacket = NE2000_PSTART + 1;

    /* set memory parameters */

    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_NODMA | CMD_PAGE2 | CMD_STOP);

    /*
     * HELP: DP 83905 manual says thas ENE_ENH is address counter lower,
     * ENE_BLOCK is address counter upper.  not wait states, or anything
     * else. What is with these comments?
     */

    SYS_OUT_CHAR (pDrvCtrl, ENE_ENH, 0x00);       /* 0 wait states */
    SYS_OUT_CHAR (pDrvCtrl, ENE_BLOCK, 0x00);     /* 0x00xxxx */

    /* 10. put the ENE in START mode */

    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_NODMA | CMD_PAGE0 | CMD_START);

    /* 11. initialize Transmit Configuration Register */

    SYS_OUT_CHAR (pDrvCtrl, ENE_TCON, TCON_NORMAL);

    ENDLOGMSG (("ne2000Config: done\n", 0, 0, 0, 0, 0, 0));

    pDrvCtrl->imask = ((pDrvCtrl->flags & END_POLLING) ? 0 : NE2000_ALL_INTS);

    /* Only actually enable interrupts if not polling */
    if (intEnable)
	SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);
    }

/*******************************************************************************
*
* ne2000OverwriteRecover - recover from receive buffer ring overflow
*
* This procedure is mandated by National Semiconductor as the only safe
* way to recover from an Overwrite Warning Error.  The first two steps
* of their procedure, reading the Command Register and stopping the NIC,
* were accomplished in the interrupt handler.  The rest occurs here.
*
* This routine is scheduled to run in the net task since it must delay
* to allow the STOP command to take effect.
*/

LOCAL void ne2000OverwriteRecover
    (
    NE2000END_DEVICE  *pDrvCtrl,
    UCHAR cmdStatus
    )
    {
    UCHAR stat;
    BOOL  reSend;

    ENDLOGMSG (("ne2000OverwriteRecover: enter (flags=%x, imask=%x)\n",
		pDrvCtrl->flags,pDrvCtrl->imask,0,0,0,0));

    /* We shouldn't get here without the OVERWRITE flags set */
    if (!(pDrvCtrl->flags & END_OVERWRITE2))
	{
	ENDLOGMSG (("ne2000OverwriteRecover: bad flags\n",
		    0,0,0,0,0,0));
#ifdef DEBUG
	SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, 0);
	taskSuspend (0);
#else
	return;
#endif
	}

    /* 1. read the TXP bit in the command register (already in cmdStatus) */

    /* 2. issue the STOP command (done in the interrupt handler) */

    /* 3. delay at least 1.6 milliseconds (1/625 second) */

    taskDelay ((sysClkRateGet() + 624)/ 625);

    /* we are now supposedly sure the NIC is stopped */

    /* 4. clear Remote Byte Count Register */

    SYS_OUT_CHAR (pDrvCtrl, ENE_RBCR0, 0x00);
    SYS_OUT_CHAR (pDrvCtrl, ENE_RBCR1, 0x00);

    /* 5. read the stored value of the TXP bit (in cmdStatus) */

    if ((cmdStatus & CMD_TXP) == 0)
        reSend = FALSE;
    else
        {
	SYS_IN_CHAR (pDrvCtrl, ENE_INTSTAT, &stat);
        if ((stat & (ISTAT_PTX | ISTAT_TXE)) == 0)
	    {
	    /* transmit was in progress but probably deferring */
            reSend = TRUE;
	    }
        else
	    {
            /* transmit was completed (probably), don't repeat it */
            reSend = FALSE;
	    }
        }

    /* 6. place the NIC in loopback mode */

    SYS_OUT_CHAR (pDrvCtrl, ENE_TCON, TCON_LB1);

    /* 7. issue the START command to the NIC */

    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_NODMA | CMD_PAGE0 | CMD_START);

    /* 8. remove one or more packets from the receive buffer ring */

    /* clear overwrite flag */
    pDrvCtrl->flags &= ~END_OVERWRITE;
    pDrvCtrl->current = ne2000GetCurr (pDrvCtrl);
    ne2000HandleRcvInt (pDrvCtrl);

    /* 9. reset the overwrite warning bit in the ISR */

    /* reset all active interrupt conditions */

    SYS_IN_CHAR (pDrvCtrl, ENE_INTSTAT, &stat);
    SYS_OUT_CHAR (pDrvCtrl, ENE_INTSTAT, stat);

    /* 10. take the NIC out of loopback */

    SYS_OUT_CHAR (pDrvCtrl, ENE_TCON, 0x00);

    /* 10a. re-enable all device interrupts */

    pDrvCtrl->imask = NE2000_ALL_INTS;
    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);

    /* 11. if need to resend, do so, otherwise if anything queued, send that */

    pDrvCtrl->flags &= ~END_OVERWRITE2;
    if (reSend)
        SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_TXP);
    else
	muxTxRestart (&pDrvCtrl->endObj);
    ENDLOGMSG (("ne2000OverwriteRecover: done (flags=%x, imask=%x)\n",
		pDrvCtrl->flags,pDrvCtrl->imask,0,0,0,0));
    }

/*******************************************************************************
*
* ne2000Int - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*
* RETURNS: N/A.
*/

LOCAL void ne2000Int
    (
    NE2000END_DEVICE  *pDrvCtrl
    )
    {
    UCHAR val;
    UCHAR intStat;
    UCHAR txStat;
    UCHAR rxStat;

    pDrvCtrl->stats.interrupts++;

    /* Read status and Acknowlegde interrupt */

    SYS_IN_CHAR (pDrvCtrl, ENE_INTSTAT, &intStat);
    intStat &= pDrvCtrl->imask;
    SYS_OUT_CHAR (pDrvCtrl, ENE_INTSTAT, intStat);

    /* Get TX and RX status */

    SYS_IN_CHAR (pDrvCtrl, ENE_TSTAT, &txStat);
    SYS_IN_CHAR (pDrvCtrl, ENE_RSTAT, &rxStat);

    ENDLOGMSG (("ne2000Int: intStat=%02x imask=%02x txStat=%02x rxStat=%02x\n",
		intStat, pDrvCtrl->imask, txStat, rxStat, 0, 0));

    /* Update count registers */
    SYS_IN_CHAR (pDrvCtrl, ENE_COLCNT, &val);
    pDrvCtrl->stats.collisions += val;
    SYS_IN_CHAR (pDrvCtrl, ENE_ALICNT, &val);
    pDrvCtrl->stats.aligns += val;
    SYS_IN_CHAR (pDrvCtrl, ENE_CRCCNT, &val);
    pDrvCtrl->stats.crcs += val;
    SYS_IN_CHAR (pDrvCtrl, ENE_MPCNT, &val);
    pDrvCtrl->stats.missed += val;

    /*
     * enable interrupts, clear receive and/or transmit interrupts, and clear
     * any errors that may be set.
     */

    /* Check for errors */

    if (intStat & ISTAT_OVW) /* Overwrite */
        {
	UCHAR cmdStat;

	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
	if (!(pDrvCtrl->flags & END_OVERWRITE2))
	    {
	    pDrvCtrl->flags |= (END_OVERWRITE | END_OVERWRITE2);
	    pDrvCtrl->lastError.errCode = END_ERR_WARN;
	    pDrvCtrl->lastError.pMesg = "Overwrite";
	    netJobAdd ((FUNCPTR) muxError, (int) &pDrvCtrl->endObj,
		       (int) &pDrvCtrl->lastError,
		       0, 0, 0);
	    pDrvCtrl->stats.overwrite++;
	    SYS_IN_CHAR (pDrvCtrl, ENE_COLCNT, &cmdStat);
	    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_NODMA | CMD_PAGE0 | CMD_STOP);
	    ENDLOGMSG (("ne2000Int: overwrite detected\n",
			0, 0, 0, 0, 0, 0));
	    netJobAdd ((FUNCPTR) ne2000OverwriteRecover, (int) pDrvCtrl,
			      cmdStat, 0, 0, 0);
	    }
	/* no further interrupt processing */
	pDrvCtrl->imask = 0;
	SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);
	return;
        }

    /* Receive handling */

    if (intStat & ISTAT_RXE) /* Receive-error */
        {
	if (!pDrvCtrl->lastIntError)
	    {
	    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
	    pDrvCtrl->lastError.errCode = END_ERR_WARN;
	    pDrvCtrl->lastError.pMesg = "receive error";
	    netJobAdd ((FUNCPTR) muxError, (int) &pDrvCtrl->endObj,
		       (int) &pDrvCtrl->lastError,
		       0, 0, 0);
	    }
	++pDrvCtrl->lastIntError;
        pDrvCtrl->stats.rerror++;
        if (rxStat & RSTAT_OVER)
            pDrvCtrl->stats.overruns++;
        if (rxStat & RSTAT_DIS)
            pDrvCtrl->stats.disabled++;
        if (rxStat & RSTAT_DFR)
            pDrvCtrl->stats.deferring++;
        }
    if (intStat & ISTAT_TXE)    /* Transmit error-packet not sent */
        {
	if (!pDrvCtrl->lastIntError)
	    {
	    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS,  +1);
	    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, -1);
	    pDrvCtrl->lastError.errCode = END_ERR_WARN;
	    pDrvCtrl->lastError.pMesg = "transmit error";
	    netJobAdd ((FUNCPTR) muxError, (int) &pDrvCtrl->endObj,
		       (int) &pDrvCtrl->lastError,
		       0, 0, 0);
	    }
	++pDrvCtrl->lastIntError;
        pDrvCtrl->stats.terror++;
        if (txStat & TSTAT_ABORT)
            {
            pDrvCtrl->stats.aborts++;
            pDrvCtrl->stats.collisions += 16;
            }
        if (txStat & TSTAT_UNDER)
            pDrvCtrl->stats.underruns++;
        }

    /* Transmit handling */

    if (intStat & ISTAT_PTX)    /* Transmit-packet sent */
        {
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
        if (txStat & TSTAT_CDH)
            pDrvCtrl->stats.heartbeats++;
        if (txStat & TSTAT_OWC)
            pDrvCtrl->stats.outofwindow++;
        if (txStat & TSTAT_PTX)
            pDrvCtrl->stats.tnoerror++;
        }

    /* Check for lack of errors */
    if (!(intStat & (ISTAT_RXE | ISTAT_TXE)))
	pDrvCtrl->lastIntError = 0;

    /* Have netTask handle any input packets */

    if (intStat & ISTAT_PRX)
        {
        pDrvCtrl->current = ne2000GetCurr (pDrvCtrl);
	ENDLOGMSG(("ne2000Int: input packet (flags=%x, current=%d)\n",
		   pDrvCtrl->flags, pDrvCtrl->current, 0, 0, 0, 0));
        if (!(pDrvCtrl->flags & END_RECV_HANDLING_FLAG))
            {
	    /* Disable RX int */
	    pDrvCtrl->imask &= ~IM_PRXE;
	    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);

            pDrvCtrl->flags |= END_RECV_HANDLING_FLAG;
            (void)netJobAdd ((FUNCPTR)ne2000HandleRcvInt, (int)pDrvCtrl,
                              0,0,0,0);
            }
        }

    /* Check for transmit complete */

    if ((intStat & (ISTAT_TXE | ISTAT_PTX)) != 0)
	{
	pDrvCtrl->flags &= ~END_TX_IN_PROGRESS;
	ENDLOGMSG (("ne2000Int: Tx complete, blocked=%d\n",
		    (pDrvCtrl->flags & END_TX_BLOCKED) ? 1 : 0,0,0,0,0,0));
	if (pDrvCtrl->flags & END_TX_BLOCKED)
	    {
	    /* Disable TX ints */
	    pDrvCtrl->imask &= ~(IM_TXEE | IM_PTXE);
	    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);

	    pDrvCtrl->flags &= ~END_TX_BLOCKED;
	    netJobAdd ((FUNCPTR)muxTxRestart, (int)&pDrvCtrl->endObj,
		       0, 0, 0, 0);
	    }
	}

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();
    }


/*******************************************************************************
*
* ne2000HandleRcvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*
* RETURNS: N/A.
*/

LOCAL void ne2000HandleRcvInt
    (
    NE2000END_DEVICE *pDrvCtrl
    )
    {
    int oldLevel;
    char *pBuf;

    /* END_RECV_HANDLING_FLAG set by ISR */

    ENDLOGMSG (("ne2000HandleRcvInt(%x): enter (flags=%x)\n",
		pDrvCtrl,pDrvCtrl->flags,0,0,0,0));
    pBuf = NULL;
    while (pDrvCtrl->flags & END_RECV_HANDLING_FLAG)
	{
	int	len;
	CL_BLK_ID	pClBlk;
	M_BLK_ID	pMblk;      /* MBLK to send upstream */

	ENDLOGMSG (("ne2000HandleRcvInt: flags=%x imask=%x cur=%d next=%d\n",
		    pDrvCtrl->flags, pDrvCtrl->imask,
		    pDrvCtrl->current, pDrvCtrl->nextPacket,
		    0, 0));
	/* check if all packets removed */
	if (pDrvCtrl->nextPacket == pDrvCtrl->current)
	    break;

	/* Break out if we get an overwrite condition */
	if (pDrvCtrl->flags & END_OVERWRITE)
	    break;

	/* Allocate an MBLK, and a replacement buffer */
	if (!pBuf)
	    {
	    pBuf = netClusterGet (pDrvCtrl->endObj.pNetPool,
				  pDrvCtrl->clPoolId);
	    if (!pBuf)
		{
		ENDLOGMSG (("ne2000HandleRcvInt: Out of clusters!\n",
			    0, 0, 0, 0, 0, 0));
		break;
		}
	    }
	/* Read packet in offset so IP header is long-aligned */
	len = ne2000PacketGet (pDrvCtrl, pBuf + pDrvCtrl->offset);
	if (len <= 0)
	    {
	    ENDLOGMSG (("ne2000HandleRcvInt: bad packet! (len=%d)\n",
			len, 0, 0, 0, 0, 0));
	    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
	    break;
	    }
	pMblk = mBlkGet (pDrvCtrl->endObj.pNetPool,
			 M_DONTWAIT, MT_DATA);
	if (!pMblk)
	    {
	    ENDLOGMSG (("ne2000HandleRcvInt: Out of M Blocks!\n",
			0, 0, 0, 0, 0, 0));
	    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
	    break;
	    }
	pClBlk = netClBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT);
	if (!pClBlk)
	    {
	    ENDLOGMSG (("ne2000HandleRcvInt: Out of CL Blocks!\n",
			0, 0, 0, 0, 0, 0));
	    netMblkFree (pDrvCtrl->endObj.pNetPool, (M_BLK_ID)pMblk);
	    break;
	    }
	/* Associate the data pointer with the MBLK */
	netClBlkJoin (pClBlk, pBuf, NE2000_BUFSIZ, NULL, 0, 0, 0);

	/* Associate the data pointer with the MBLK */
	netMblkClJoin (pMblk, pClBlk);

	pMblk->mBlkHdr.mFlags |= M_PKTHDR;
	pMblk->mBlkHdr.mLen    = len;
	pMblk->mBlkPktHdr.len  = len;
	/* Adjust mData to match n23000PacketGet() above */
	pMblk->mBlkHdr.mData   += pDrvCtrl->offset;

	/* record received packet */
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);


	/* Call the upper layer's receive routine. */
	END_RCV_RTN_CALL (&pDrvCtrl->endObj, pMblk);

	/* buffer was used, will need another next time around */
	pBuf = NULL;
	}

    /* If we still have an unused buffer, free it */
    if (pBuf)
	netClFree (pDrvCtrl->endObj.pNetPool, (UCHAR *) pBuf);

    /* Re-enable the receive interrupt */
    oldLevel = intLock ();
    pDrvCtrl->flags &= ~END_RECV_HANDLING_FLAG;
    pDrvCtrl->imask |= IM_PRXE;
    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);

    ENDLOGMSG (("ne2000HandleRcvInt: done (flags=%x, imask=%x)\n",
		pDrvCtrl->flags,pDrvCtrl->imask,0,0,0,0));

    intUnlock (oldLevel);
    }

/*******************************************************************************
*
* ne2000DataIn - input bytes from NE2000 memory
*
* NOMANUAL
*/

LOCAL void ne2000DataIn
    (
    NE2000END_DEVICE *	pDrvCtrl,
    int			eneAddress,
    int			len,
    char *		pData
    )
    {
    if (len == 0)
	return;

    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, 0);
    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_NODMA | CMD_PAGE0 | CMD_START);
    SYS_OUT_CHAR (pDrvCtrl, ENE_RSAR0, eneAddress & 0xff);
    SYS_OUT_CHAR (pDrvCtrl, ENE_RSAR1, eneAddress >> 8);
    SYS_OUT_CHAR (pDrvCtrl, ENE_RBCR0, len & 0xff);
    SYS_OUT_CHAR (pDrvCtrl, ENE_RBCR1, len >> 8);
    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_RREAD | CMD_START);

    SYS_IN_WORD_STRING (pDrvCtrl, ENE_DATA, pData, len/2);
    if (len & 1)
	SYS_IN_CHAR (pDrvCtrl, ENE_DATA, (pData + (len - 1)));
    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);
    }

/*******************************************************************************
*
* ne2000DataOut - output bytes to NE2000 memory
*
* NOMANUAL
*/

LOCAL void ne2000DataOut
    (
    NE2000END_DEVICE *	pDrvCtrl,
    char *		pData,
    int			len,
    int			eneAddress
    )
    {
    if (len == 0)
        return;

    SYS_OUT_CHAR (pDrvCtrl, ENE_RBCR0, (char)0xff);
    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_NODMA | CMD_PAGE0 | CMD_START);
    SYS_OUT_CHAR (pDrvCtrl, ENE_RSAR0, eneAddress & 0xff);
    SYS_OUT_CHAR (pDrvCtrl, ENE_RSAR1, eneAddress >> 8);
    SYS_OUT_CHAR (pDrvCtrl, ENE_RBCR0, len & 0xff);
    SYS_OUT_CHAR (pDrvCtrl, ENE_RBCR1, len >> 8);
    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_RWRITE | CMD_START);

    SYS_OUT_WORD_STRING (pDrvCtrl, ENE_DATA, pData, len/2);
    if (len & 1)
        SYS_OUT_CHAR (pDrvCtrl, ENE_DATA, *(pData + (len - 1)));
    }

/*******************************************************************************
*
* ne2000Send - the driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.  The last arguments are a free
* routine to be called when the device is done with the buffer and a pointer
* to the argument to pass to the free routine.  
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ne2000Send
    (
    void*	pCookie,	/* device ptr */
    M_BLK_ID	pMblk		/* data to send */
    )
    {
    int		len;
    UCHAR	cmdStat;
    NE2000END_DEVICE* pDrvCtrl = (NE2000END_DEVICE *) pCookie;
    
    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */


    if (pDrvCtrl->flags & (END_OVERWRITE | END_OVERWRITE2))
	{
	return (END_ERR_BLOCK);
	}

    ENDLOGMSG (("ne2000Send: enter: (flags=%x, imask=%x)\n",
		pDrvCtrl->flags, pDrvCtrl->imask, 0, 0, 0, 0));

    /* Note device receive interrupts may still be enabled */

    END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);
    pDrvCtrl->flags |= END_TX_BLOCKED;

    if (pDrvCtrl->flags & END_TX_IN_PROGRESS)
	{
	int cnt;

	ENDLOGMSG (("ne2000Send: waiting for TX_IN_PROGRESS\n",
		    0,0,0,0,0,0));

	/* Wait up to 1 second */
	cnt = sysClkRateGet ();
	while ((pDrvCtrl->flags & END_TX_IN_PROGRESS) && (cnt-- > 0))
	    taskDelay (1);
	}

    /* Disable device interrupts and check for overwrite detected */
    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, 0);
    if (pDrvCtrl->flags & (END_OVERWRITE | END_OVERWRITE2))
	{
	ENDLOGMSG (("ne2000Send: overwrite detected (timeout)\n",
		    0, 0, 0, 0, 0, 0));
	pDrvCtrl->flags &= ~END_TX_BLOCKED;
	END_TX_SEM_GIVE (&pDrvCtrl->endObj);
	return (END_ERR_BLOCK);
	}

    /* If TX still in progress, re-configure chip */
    if (pDrvCtrl->flags & END_TX_IN_PROGRESS)
	{
	UCHAR tstat;

	SYS_IN_CHAR (pDrvCtrl, ENE_TSTAT, &tstat);
	SYS_IN_CHAR (pDrvCtrl, ENE_CMD, &cmdStat);
	ENDLOGMSG (("ne2000Send: timeout: flags=%x cmd=%02x stat=%02x\n",
		    pDrvCtrl->flags, cmdStat, tstat, 0, 0, 0));
	ne2000Config (pDrvCtrl, FALSE);
	}

    /* Copy and free the MBLK */

    len = netMblkToBufCopy (pMblk, pDrvCtrl->packetBuf, NULL);
    netMblkClChainFree (pMblk);
    len = max (len, ETHERSMALL);

    /* Transfer to the device */

    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, 0x00);
    ne2000DataOut (pDrvCtrl, pDrvCtrl->packetBuf, len, (NE2000_TSTART << 8));

    /* Flush the write pipe */
    
    CACHE_PIPE_FLUSH ();
    
    /* kick Transmitter */

    SYS_OUT_CHAR (pDrvCtrl, ENE_TSTART, NE2000_TSTART);
    SYS_OUT_CHAR (pDrvCtrl, ENE_TCNTH, len >> 8);
    SYS_OUT_CHAR (pDrvCtrl, ENE_TCNTL, len & 0xff);
    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_TXP | CMD_START);

    /* mark tx as in progress */

    pDrvCtrl->flags |= END_TX_IN_PROGRESS;

    /* Re-enable device TX interrupts */

    pDrvCtrl->imask |= (IM_TXEE | IM_PTXE);
    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    /* update statistics */
    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);


    ENDLOGMSG (("ne2000Send: done: imask=%02x\n",
		pDrvCtrl->imask, 0, 0, 0, 0, 0));
    return (OK);
    }

/*******************************************************************************
*
* ne2000PacketGet - get next received message
*
* Get next received message.  Returns NULL if none are
* ready.
*
* RETURNS: ptr to next packet, or NULL if none ready.
*/

LOCAL int ne2000PacketGet
    (
    NE2000END_DEVICE	*pDrvCtrl,
    char		*pData
    )
    {
    UINT        packetSize;
    UCHAR	uppByteCnt;
    UINT8       tempPage;               /* used in buffer validation */
    UINT8       pageCount;              /* used in buffer validation */
    UINT        packetLen = 0;
    NE2000_HEADER  h;

    if (pDrvCtrl->nextPacket == pDrvCtrl->current)
	return (0);

    ne2000DataIn (pDrvCtrl,
		  (((UINT)pDrvCtrl->nextPacket << 8) & 0x0000ffff),
		  sizeof (NE2000_HEADER), (char *) &h);

    /*
     * Calculate upper byte count in case it's corrupted,
     * though this supposedly happens only in StarLAN applications
     * with bus clock frequence greater than 4 mHz.
     */
    if (h.next > pDrvCtrl->nextPacket)
	uppByteCnt = (UCHAR) (h.next - pDrvCtrl->nextPacket);
    else
	uppByteCnt = (UCHAR) ((NE2000_PSTOP - pDrvCtrl->nextPacket)
			      + (h.next - NE2000_PSTART));
    if (h.lowByteCnt > 0xfc)
	uppByteCnt -= 2;
    else
	uppByteCnt -= 1;
    h.uppByteCnt = uppByteCnt;

    /* compute packet size excluding Ethernet checksum bytes */

    packetSize = (((UINT)h.uppByteCnt << 8) + h.lowByteCnt) - 4;

    /* Check for packet (and header) shifted in memory (by heavy load).
     * The method and solution are recommended by 3Com in their
     * EtherLink II Adapter Technical Manual, with the addition of
     * a reasonableness check on the next-page link.
     */
    pageCount = (UCHAR) ((packetSize + 4 + sizeof (NE2000_HEADER)
			  + (ENE_PAGESIZE - 1)) / ENE_PAGESIZE);
    tempPage = (UCHAR) (pDrvCtrl->nextPacket + pageCount);
    if (tempPage >= NE2000_PSTOP)
	tempPage -= (NE2000_PSTOP - NE2000_PSTART);
    if ((h.next != tempPage) ||
	(h.next < NE2000_PSTART) || (h.next >= NE2000_PSTOP))
	{
	/* can't trust anything now */
	pDrvCtrl->stats.badPacket++;
	pDrvCtrl->stats.rerror++;
	/* stop and restart the interface */
	if (!(pDrvCtrl->flags & (END_OVERWRITE|END_OVERWRITE2)))
	    ne2000Config (pDrvCtrl, TRUE);
	return (-1);
	}

    /* reject runts, although we are configured to reject them */
    if (packetSize < 60)
	{
	pDrvCtrl->stats.shortPacket++;
	goto doneGet;
	}

    /* reject packets larger than our scratch buffer */
    if (packetSize > NE2000_BUFSIZ)
	goto doneGet;

    if (h.rstat & RSTAT_PRX)
	{
	/* 3Com says this status marks a packet bit-shifted in memory;
	 * the data cannot be trusted but the NIC header is OK.
	 */
	if (h.rstat & (RSTAT_DFR | RSTAT_DIS))
	    {
	    pDrvCtrl->stats.badPacket++;
	    pDrvCtrl->stats.rerror++;
	    goto doneGet;
	    }
	pDrvCtrl->stats.rnoerror++;
	}
    else
	{
	if (h.rstat & RSTAT_DFR)
	    pDrvCtrl->stats.jabber++;
	pDrvCtrl->stats.rerror++;
	goto doneGet;
	}

    /* Signal that we received a good packet */
    packetLen = packetSize;

    /* copy separated frame to a temporary buffer */
    ne2000DataIn (pDrvCtrl,
		  ((UINT)pDrvCtrl->nextPacket << 8) + sizeof (NE2000_HEADER),
		  packetLen,
		  pData);

doneGet:
    pDrvCtrl->nextPacket = h.next;
    SYS_OUT_CHAR (pDrvCtrl, ENE_BOUND,
		  (pDrvCtrl->nextPacket != NE2000_PSTART ?
		   pDrvCtrl->nextPacket - 1 : NE2000_PSTOP - 1));

    return (packetLen);
    }


/******************************************************************************
*
* ne2000PollRecv - routine to receive a packet in polled mode.
*
* This routine is called by a user to try and get a packet from the
* device.
*/

LOCAL STATUS ne2000PollRecv
    (
    void*	pCookie,	/* device ptr */
    M_BLK_ID	pMblk
    )
    {
    int len;
    NE2000END_DEVICE* pDrvCtrl = (NE2000END_DEVICE *) pCookie;

    /* If no packet is available return immediately */
    pDrvCtrl->current = ne2000GetCurr (pDrvCtrl);
    if (pDrvCtrl->nextPacket == pDrvCtrl->current)
        return ((STATUS) (EAGAIN));

    /* Upper layer provides the buffer, make sure it's big enough. */
    if ((pMblk->mBlkHdr.mLen < NE2000_BUFSIZ)
	|| !(pMblk->mBlkHdr.mFlags & M_EXT))
	{
        return ((STATUS) (EAGAIN));
	}

    ENDLOGMSG (("ne2000PollRecv: enter: imask=%02x\n",
		pDrvCtrl->imask, 0, 0, 0, 0, 0));

    len = ne2000PacketGet (pDrvCtrl, pMblk->mBlkHdr.mData + pDrvCtrl->offset);
    if (len <= 0)
        return ((STATUS) (EAGAIN));
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkHdr.mLen   = len;
    pMblk->mBlkPktHdr.len = len;
    /* Adjust mData to match n23000PacketGet() above */
    pMblk->mBlkHdr.mData += pDrvCtrl->offset;

    ENDLOGMSG (("ne2000PollRecv: done: imask=%02x\n",
		pDrvCtrl->imask, 0, 0, 0, 0, 0));
    return (OK);
    }

/*******************************************************************************
*
* ne2000PollSend - routine to send a packet in polled mode.
*
* This routine is called by a user to try and send a packet on the
* device.
*/

static STATUS ne2000PollSend
    (
    void*	pCookie,	/* device ptr */
    M_BLK_ID	pMblk
    )
    {
    char *	pBuf;
    UCHAR	cmdStat;
    int		len;
    NE2000END_DEVICE* pDrvCtrl = (NE2000END_DEVICE *) pCookie;

    ENDLOGMSG (("ne2000PollSend: enter: imask=%x flags=%x\n",
		pDrvCtrl->imask, pDrvCtrl->flags, 0, 0, 0, 0));

    SYS_IN_CHAR (pDrvCtrl, ENE_CMD, &cmdStat);
    while (cmdStat & CMD_TXP)
	SYS_IN_CHAR (pDrvCtrl, ENE_DATA, &cmdStat);

    /* Get the next TXD */

    pBuf = pDrvCtrl->packetBuf;
    len = netMblkToBufCopy (pMblk, pBuf, NULL);
    len = max (len, ETHERSMALL);

    /* Transfer to the device */

    ne2000DataOut (pDrvCtrl, pDrvCtrl->packetBuf, len, (NE2000_TSTART << 8));

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* kick Transmitter */

    SYS_OUT_CHAR (pDrvCtrl, ENE_TSTART, NE2000_TSTART);
    SYS_OUT_CHAR (pDrvCtrl, ENE_TCNTH, len >> 8);
    SYS_OUT_CHAR (pDrvCtrl, ENE_TCNTL, len & 0xff);
    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_TXP | CMD_START);

    /* update statistics */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Spin until the packet has been sent */

    SYS_IN_CHAR (pDrvCtrl, ENE_CMD, &cmdStat);
    while (cmdStat & CMD_TXP)
	SYS_IN_CHAR (pDrvCtrl, ENE_CMD, &cmdStat);

    ENDLOGMSG (("ne2000PollSend: done: imask=%x flags=%x\n",
		pDrvCtrl->imask, pDrvCtrl->flags, 0, 0, 0, 0));

    return (OK);
    }

/*******************************************************************************
*
* ne2000PollStart - start polled mode operations
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ne2000PollStart
    (
    NE2000END_DEVICE* pDrvCtrl
    )
    {
    int oldLevel;

    oldLevel = intLock ();
    ENDLOGMSG (("ne2000PollStart: imask=%x flags=%x\n",
		pDrvCtrl->imask, pDrvCtrl->flags, 0, 0, 0, 0));
    pDrvCtrl->flags |= END_POLLING;
    pDrvCtrl->imask = 0;
    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);
    intUnlock (oldLevel);

    return (OK);
    }

/*******************************************************************************
*
* ne2000PollStop - stop polled mode operations
*
* This function terminates polled mode operation.  The device returns to
* interrupt mode.
*
* The device interrupts are enabled, the current mode flag is switched
* to indicate interrupt mode and the device is then reconfigured for
* interrupt operation.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ne2000PollStop
    (
    NE2000END_DEVICE* pDrvCtrl
    )
    {
    int oldLevel;

    oldLevel = intLock ();
    pDrvCtrl->flags &= ~END_POLLING;
    pDrvCtrl->imask = NE2000_ALL_INTS;
    /* leave recv int disabled if in receive loop. */
    if (pDrvCtrl->flags & END_RECV_HANDLING_FLAG)
	pDrvCtrl->imask &= ~IM_PRXE;
    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);

    ENDLOGMSG (("ne2000PollStop: imask=%x flags=%x\n",
		pDrvCtrl->imask, pDrvCtrl->flags, 0, 0, 0, 0));
    intUnlock (oldLevel);
    return (OK);
    }

/******************************************************************************
*
* ne2000AddrFilterSet - set the address filter for multicast addresses
*
* This routine loops through all of the multicast addresses on the list
* of addresses (added with the endAddrAdd() routine) and sets the
* device's filter appropriately.
*
* NOMANUAL
*/
LOCAL void ne2000AddrFilterSet
    (
    NE2000END_DEVICE *pDrvCtrl
    )
    {
    int i;
    int eAddr;
    u_char enetChar;
    u_char *pCp;
    u_long crc;
    ETHER_MULTI* pCurr;

    ENDLOGMSG (("ne2000AddrFilterSet\n", 0, 0, 0, 0, 0, 0));

    /*
     * There are 8 multicast address registers (MAR0-7) which
     * decode the multicast addresses to be received.  These
     * registers provide filtering of multicast addresses hashed
     * by the CRC hardware logic.  All destination addresses are
     * fed through the CRC hardware logic and when the last bit
     * of the destination address enters the CRC hardware, the
     * 6 MSB's of the CRC generator are latched.  These six bits
     * are then used to index a unique filter bit (FB0-63, 64 since
     * there are eight-8bit registers, and 2^6 = 64) in the multicast
     * address registers.   If the index filter bit is set,
     * the packet is accepted.
     *
     * To configure MAR0-7 to accept a specific multicast address,
     * the above sequence must be also be duplicated in software to
     * determine which filter bit in the multicast registers should
     * be set for a given multicast address.   Several bits can be
     * set to accept several multicast addresses.
     *
     * The standard AUTODIN-II polynomial is used for the 32-bit CRC
     * hash index calculation.  The AUTODIN-II, FDDI, ethernet
     * polynomial for 32 bit CRC is 0x04c11db7, as deemed kosher
     * by the mighty polynomial gods of error awareness.
     *
     * The polynomial is expressed:
     *
     *   ( x^32 + x^26 + x^23 + x^22 + x^16 +
     *     x^12 + x^11 + x^10 + x^8  + x^7  +
     *            x^5  + x^4  + x^2  + x^1  + 1 ) = 0x04c11db7
     *
     * Where x = base 2 (binary) and x^32 is ignored for CRC.
     *
     */

    /* initialize (clear) all filter bits */

    for (i = 0; i < 8; ++i)
	pDrvCtrl->mcastFilter[i] = 0;


    /* set the proper MAR0-7 filter bit(s) for every address */

    for (pCurr = END_MULTI_LST_FIRST (&pDrvCtrl->endObj);
	 pCurr != NULL;
	 pCurr = END_MULTI_LST_NEXT(pCurr))
	{
	pCp = (unsigned char *)&pCurr->addr;

	crc = 0xffffffff;  /* initial CRC seed */

	/* build the CRC for the 6 byte adrs */

	for (eAddr = 0; eAddr < 6; eAddr++)
	    {
	    enetChar = *pCp++;
            crc = ne2000CrcWork (enetChar, crc);
	    }

	/*
	 * We now have the crc for the current address and we are
	 * interested in the 6 most significant bits of the crc
	 * for indexing because that is whats latched by the CRC
	 * hardware logic.
	 *
	 * Which of the eight MAR registers, 0-7, is indexed with
	 * the upper three bits of the six.  Which of the eight bits
	 * to set in that MAR register is indexed by the lower three
	 * bits of the six.
	 */

	/* get six msb */

	crc >>= 26;
	crc &= 0x3f;


	/* high 3 bit index MAR reg, low three bits index reg bit */

	pDrvCtrl->mcastFilter[crc >> 3] |= (1 << (crc & 7));
	}
    }

/*****************************************************************************
*
* ne2000CrcWork - return CRC using a 32 bit polynomial
*
* This is a work routine used by ne2000AddrFilterSet
*
* NOMANUAL
*
*/

LOCAL UINT32 ne2000CrcWork
    (
    UINT8 inChar,
    UINT32 inCrc
    )
    {
    UINT8 work = 0;
    UINT8 wrap;
    int i;

    /*
     * standard CRC algorithm, 8 bit stream.
     */

    for (i = 0; i < 8; i++)
        {
        wrap = (inCrc >> 31);
        inCrc <<= 1;
        work  = ((work << 1) | wrap) ^ inChar;
        if (work & 1)
            {
            inCrc ^= CRC32_POLYNOMIAL;
            }
         work >>= 1;
         inChar >>= 1;
         }
     return (inCrc);
     }


/*****************************************************************************
*
* ne2000MCastAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*/

LOCAL STATUS ne2000MCastAdd
    (
    void*	pCookie,	/* device ptr */
    char*	pAddress
    )
    {
    NE2000END_DEVICE* pDrvCtrl = (NE2000END_DEVICE *) pCookie;

    ENDLOGMSG (("ne2000MCastAdd - %02x:%02x:%02x:%02x:%02x:%02x\n",
		pAddress[0]&0xff, pAddress[1]&0xff, pAddress[2]&0xff,
		pAddress[3]&0xff, pAddress[4]&0xff, pAddress[5]&0xff));

    if (etherMultiAdd (&pDrvCtrl->endObj.multiList, pAddress) == ENETRESET)
	{
	ne2000Config (pDrvCtrl, TRUE);
	}

    return (OK);
    }

/*****************************************************************************
*
* ne2000MCastDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*/

LOCAL STATUS ne2000MCastDel
    (
    void*	pCookie,	/* device ptr */
    char*	pAddress
    )
    {
    NE2000END_DEVICE* pDrvCtrl = (NE2000END_DEVICE *) pCookie;

    ENDLOGMSG (("ne2000MCastDel - %02x:%02x:%02x:%02x:%02x:%02x\n",
		pAddress[0]&0xff, pAddress[1]&0xff, pAddress[2]&0xff,
		pAddress[3]&0xff, pAddress[4]&0xff, pAddress[5]&0xff));

    if (etherMultiDel (&pDrvCtrl->endObj.multiList,
		       (char *)pAddress) == ENETRESET)
	{
	ne2000Config (pDrvCtrl, TRUE);
	}

    return (OK);
    }

/*****************************************************************************
*
* ne2000MCastGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*/

LOCAL STATUS ne2000MCastGet
    (
    void*		pCookie,	/* device ptr */
    MULTI_TABLE*	pTable
    )
    {
    int error;
    NE2000END_DEVICE* pDrvCtrl = (NE2000END_DEVICE *) pCookie;

    ENDLOGMSG (("ne2000MCastGet\n", 0, 0, 0, 0, 0, 0));
    error = etherMultiGet (&pDrvCtrl->endObj.multiList, pTable);

    return (error);
    }

/******************************************************************************
*
* ne2000Stop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS ne2000Stop
    (
    void *pCookie
    )
    {
    STATUS result = OK;
    NE2000END_DEVICE* pDrvCtrl;

    pDrvCtrl = (NE2000END_DEVICE *) pCookie;

    /* mark the interface -- down */

    END_FLAGS_CLR (&pDrvCtrl->endObj, IFF_UP | IFF_RUNNING);

    /* Disable device interrupts */

    pDrvCtrl->imask = 0;

    SYS_OUT_CHAR (pDrvCtrl, ENE_INTMASK, pDrvCtrl->imask);

    /* stop device */

    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_NODMA | CMD_PAGE0 | CMD_STOP);

    ENDLOGMSG (("ne2000Stop\n", 0, 0, 0, 0, 0, 0));

    SYS_INT_DISCONNECT (pDrvCtrl, ne2000Int, (int)pDrvCtrl, &result);

    if (result == ERROR)
	ENDLOGMSG (("Could not disconnect interrupt!\n", 1, 2, 3, 4, 5, 6));

    return (result);
    }

/******************************************************************************
*
* ne2000Unload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*/

LOCAL STATUS ne2000Unload
    (
    void *pCookie
    )
    {
    NE2000END_DEVICE* pDrvCtrl = (NE2000END_DEVICE *) pCookie;

    ENDLOGMSG (("ne2000Unload\n", 0, 0, 0, 0, 0, 0));

    END_OBJECT_UNLOAD (&pDrvCtrl->endObj); /* generic end unload functions */

    return (OK);
    }

/******************************************************************************
*
* ne2000EnetAddrGet - get enet address
*
* NOMANUAL
*/

LOCAL void ne2000EnetAddrGet
    (
    NE2000END_DEVICE* pDrvCtrl
    )
    {
    int i;

    if (pDrvCtrl->usePromEnetAddr)
	{
	SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_NODMA | CMD_PAGE0 | CMD_START);
	SYS_OUT_CHAR (pDrvCtrl, ENE_RSAR0, NE2000_EADDR_LOC);
	SYS_OUT_CHAR (pDrvCtrl, ENE_RSAR1, NE2000_CONFIG_PAGE);
	SYS_OUT_CHAR (pDrvCtrl, ENE_RBCR0, (EADDR_LEN * 2) & 0xff);
	SYS_OUT_CHAR (pDrvCtrl, ENE_RBCR1, (EADDR_LEN * 2) >> 8);
	SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_RREAD | CMD_START);

	for (i = 0; i < 6; ++i)
	    {
	    char ch;

	    SYS_IN_CHAR (pDrvCtrl, ENE_DATA, &pDrvCtrl->enetAddr[i]);
	    /* Must consume "duplicate" high bytes in 8-bit mode */
	    if (pDrvCtrl->byteAccess)
		SYS_IN_CHAR (pDrvCtrl, ENE_DATA, &ch);
	    }

	/* check for enet addr of all zeros (unprogrammed) */
	for (i = 0; i < 6; ++i)
	    if (pDrvCtrl->enetAddr[i])
		return;
	}

    /* get the enet addr from the BSP. */
    for (i = 0; i < 6; ++i)
	pDrvCtrl->enetAddr[i] = ne2000EnetAddr[i];

    }

/******************************************************************************
*
* ne2000GetCurr - get current page
*
* RETURNS: current page that ENE is working on
*
* NOMANUAL
*/

LOCAL UCHAR ne2000GetCurr
    (
    NE2000END_DEVICE* pDrvCtrl
    )
    {
    UCHAR curr;

    /* get CURR register */
    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_PAGE1);
    SYS_IN_CHAR (pDrvCtrl, ENE_CURR, &curr);
    SYS_OUT_CHAR (pDrvCtrl, ENE_CMD, CMD_PAGE0);

    return (curr);
    }
