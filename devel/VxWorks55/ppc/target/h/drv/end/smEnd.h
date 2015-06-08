/* smEnd.h - WRS shared memory driver header file */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,28jan99,mas  written based on if_sm.h rev 01f,22sep92.
*/

#ifndef __INCsmEndh
#define __INCsmEndh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "etherLib.h"
#include "netinet/if_ether.h"
#include "smPktLib.h"


/* defines */

/* XXXmas should use the following def only as default in future */

#define	NSM		1

#define SM_PARAMS_MAX     256

#define SM_END_COOKIE     0xfab0fad0

#define SM_MIN_MBLK_NUM   0x10
#define SM_DFLT_MBLK_NUM  0x30

#define SM_MIN_CLBLK_NUM  0x10
#define SM_DFLT_CLBLK_NUM 0x20

#define SM_END_MIN_FBUF	  (1536)	/* min first buffer size */

#define SM_PACKET_ROUNDUP(siz) \
    ((16 - (((siz) + SIZEOF_ETHERHEADER) % 16)) + SIZEOF_ETHERHEADER)

#define SM_END_DEV_NAME   "sm"

#define SM_POLL_TASK_NAME "tsmPollTask"

#define SM_NUM_INT_TYPES  8

#define SM_END_MIB2_SPEED -1

#define SM_END_ATTR_CLR  \
    (IFF_UP | IFF_NOTRAILERS | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST)

/* ethernet address length */

#define EADDR_LEN         (sizeof(struct ether_addr))

#define SM_END_CLST_SIZ	  (ETHERMTU + ENET_HDR_REAL_SIZ + EADDR_LEN)

/*
 * Maximum delay for initial access to sm from slave processor in ticks.
 * At 100 ticks per second, this is about 30 seconds.
 */

#define SM_MAX_WAIT       3000

#define M_BLK_OVERHEAD    4
#define CL_BLK_OVERHEAD   4
#define CL_OVERHEAD       4

#define PKT_TYPE_MULTI	0x1	/* packet with a multicast address */
#define PKT_TYPE_UNI	0x2	/* packet with a unicast address */
#define PKT_TYPE_NONE	0x4	/* address type is not meaningful */


/* Macros */

#undef  deviceValid	/* XXXmas temporary */
#define deviceValid(unit)  (((unit >= 0) && (unit < NSM)) ? TRUE : FALSE)

#define etherAddrPtr(eaddr)	(u_char *)(eaddr)

#define smEndInputCount(pSmEndDev, cpu)  \
    (pSmEndDev)->smPktDesc.cpuLocalAdrs [(cpu)].inputList.count

#define SM_END_ADDR_GET(pEnd)                                          \
    ((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define SM_END_ADDR_LEN_GET(pEnd)                                      \
    ((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

#define SM_END_COPY_FROM_CLUSTER(mBlkId,pData,len)  \
    bcopy ((mBlkId)->mBlkHdr.mData, (pData), (len))
/*    bcopy ((mBlkId)->pClBlk->clNode.pClBuf, (pData), (len))*/

#define SM_END_COPY_TO_CLUSTER(mBlkId,pData,len)  \
    bcopy ((pData), (mBlkId)->mBlkHdr.mData, (len))
/*    bcopy ((pData), (mBlkId)->pClBlk->clNode.pClBuf, (len))*/


/* typedefs */

/* The definition of the shared memory device control structure */

typedef struct sm_end_device	/* SM_END_DEV */
    {
    END_OBJ		end;		/* The class we inherit from. */
    UINT		cookie;		/* sm device validation cookie */
    unsigned		localCpu;	/* local cpu number */
    int			unit;		/* unit number */
    ULONG		busSpace;	/* bus address space for pAnchor/pmem*/
    SM_ANCHOR *		pAnchor;	/* pointer to shared memory anchor */
    char *		pMem;		/* pointer to start of shared memory */
    ULONG		memSize;	/* total shared memory size in bytes */
    unsigned		tasType;	/* test-and-set type (HARD/SOFT) */
    unsigned		maxCpus;	/* max #CPUs supported in 'sm' */
    unsigned		masterCpu;	/* master CPU number */
    ULONG		maxPktBytes;	/* max #packets in shared memory */
    char *		startAddr;	/* start of seq addrs (0 = not seq) */
    char *		ipAddr;		/* non-seq. IP address */
    int			maxPackets;	/* max #packets CPU can receive */
    int			intType;	/* interrupt method */
    int			intArg1;	/* 1st interrupt argument */
    int			intArg2;	/* 2nd interrupt argument */
    int			intArg3;	/* 3rd interrupt argument */
    int			ticksPerBeat;	/* #CPU ticks per heartbeat */
    ULONG		mbNum;		/* number of mBlks to allocate */
    ULONG		cbNum;		/* number of clBlks to allocate */
    BOOL		smAlloc;	/* sm allocated? */
    BOOL		isMaster;	/* we are master CPU? */
    ULONG		flags;		/* Our local flags. */
    SM_PKT_MEM_HDR *	pSmPktHdr;	/* sm packet header */
    CL_POOL_ID		clPoolId;	/* cluster pool ID */
    M_BLK_ID		tupleId;	/* receive-ready tuple (mBlk) ID */
    M_BLK_ID		pollQ;		/* polled tuple queue */
    M_BLK_ID		pollQLast;	/* polled tuple queue last entry */
    void *		pMclBlkCfg;	/* mBlk/clBlk config memory */
    void *		pClustMem;	/* cluster pool memory */

    /* old SM_SOFTC section */

    struct arpcom	arpcom;		/* common ethernet structure*/
    SM_PKT_DESC		smPktDesc;	/* shared mem packet desc */
    u_long		masterAddr;	/* master's IP address */
    } SM_END_DEV;


/* function prototypes */

extern END_OBJ * smEndLoad (char * pParamStr);
extern void      smNetShowInit (void);
extern STATUS    smNetShow (char * ifName, BOOL zero);


#ifdef __cplusplus
}
#endif

#endif /* __INCsmEndh */
