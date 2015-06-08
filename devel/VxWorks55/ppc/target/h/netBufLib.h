/* netBufLib.h - network buffers header header file */

/* Copyright 1984 - 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01p,13jan03,rae  Merged from velocecp branch (SPR 73393)
01o,05jun02,vvv  added M_EOB flag (SPR #72213)
01n,26mar02,vvv  added M_PROXY flag (SPR #74518)
01m,10oct01,rae  merge from truestack ver 01q base 01l
01l,24mar99,ann  protected some #defines by #ifndefs due to conflicts
                 in project facility (all the ones defined in 00network.cdf)
01k,16mar99,spm  recovered orphaned code from tor1_0_1.sens1_1 (SPR #25770)
01j,08oct98,gnn	 updated system network buffer allocations
01i,11dec97,vin  added prototype for netMblkOffsetToBufCopy part of SPR 9563.
01h,08dec97,vin  added prototype for netMblkChainDup SPR 9966.
01g,03dec97,vin  added prototype for netTupleGet(), added NUM_SYS_xxx defines
01f,13nov97,vin  added error codes and cleanup.
01e,08oct97,vin  corrected clBlkFree()
01d,30sep96,vin  added M_BLK_SZ.
01c,26sep97,vin  added NUM_XXX_MIN macros for bootroms.
01b,19sep97,vin  added cluster Blk typedef, optimized mBlk structure.
		 moved mExt structure out of the mBlk structure.
01a,08aug97,vin  written.
*/

#ifndef __INCnetBufLibh
#define __INCnetBufLibh

#ifdef __cplusplus
extern "C" {
#endif

#if CPU_FAMILY==I960
#pragma align 1			/* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/* status codes */

#define	S_netBufLib_MEMSIZE_INVALID	(M_netBufLib | 1)
#define	S_netBufLib_CLSIZE_INVALID	(M_netBufLib | 2)
#define	S_netBufLib_NO_SYSTEM_MEMORY	(M_netBufLib | 3)
#define	S_netBufLib_MEM_UNALIGNED	(M_netBufLib | 4)
#define	S_netBufLib_MEMSIZE_UNALIGNED	(M_netBufLib | 5)
#define	S_netBufLib_MEMAREA_INVALID	(M_netBufLib | 6)
#define S_netBufLib_MBLK_INVALID	(M_netBufLib | 7)
#define S_netBufLib_NETPOOL_INVALID	(M_netBufLib | 8)
#define S_netBufLib_INVALID_ARGUMENT	(M_netBufLib | 9)
#define S_netBufLib_NO_POOL_MEMORY	(M_netBufLib | 10)

#ifndef NUM_NET_MBLKS
#define NUM_NET_MBLKS			400	/* no. mBlks to initialize */
#endif  /* NUM_NET_MBLKS */

#define MAX_MBLK_TYPES			256	/* max number of mBlk types */

/* data clusters used by the network stack */

#ifndef NUM_64
#define NUM_64				200	/* no. 64 byte clusters */
#endif  /* NUM_64 */

#ifndef NUM_128
#define NUM_128				200	/* no. 128 byte clusters */
#endif  /* NUM_128 */

#ifndef NUM_256
#define NUM_256				80	/* no. 256 byte clusters */
#endif  /* NUM_256 */

#ifndef NUM_512
#define NUM_512				80	/* no. 512 byte clusters */
#endif  /* NUM_512 */

#ifndef NUM_1024
#define NUM_1024			50	/* no. 1024 byte clusters */
#endif  /* NUM_1024 */

#ifndef NUM_2048
#define NUM_2048			50	/* no. 2048 byte clusters */
#endif  /* NUM_2048 */

#ifndef NUM_CL_BLKS
#define NUM_CL_BLKS			(NUM_64 + NUM_128 + NUM_256 +      \
					 NUM_512 + NUM_1024 + NUM_2048)
#endif  /* NUM_CL_BLKS */

/* data clusters for a minimal configuration */

#define NUM_NET_MBLKS_MIN		200	/* no. mBlks to initialize */

#define NUM_64_MIN			50	/* no. 64 byte clusters */
#define NUM_128_MIN			50	/* no. 128 byte clusters */
#define NUM_256_MIN			10	/* no. 256 byte clusters */
#define NUM_512_MIN			10	/* no. 512 byte clusters */
#define NUM_1024_MIN			20	/* no. 1024 byte clusters */

#define NUM_CL_BLKS_MIN			(NUM_64_MIN + NUM_128_MIN + 	\
					 NUM_256_MIN + NUM_512_MIN + 	\
                                         NUM_1024_MIN)

/* system clusters for routes, interface addresses, pcbs, sockets, etc. */
/* This is 61440 bytes and should allow us to open the default number */
/* of sockets which is 50 (NUM_FILES) */

  /* DJA - double number of Sys clusters to 128 */
 
#ifndef NUM_SYS_64
#define NUM_SYS_64		       128	/*  64 byte system clusters */
#endif /* NUM_SYS_64 */

#ifndef NUM_SYS_128
#define NUM_SYS_128		       128	/* 128 byte system clusters */
#endif /* NUM_SYS_128 */

#ifndef NUM_SYS_256
#define NUM_SYS_256		       128	/* 256 byte system clusters */
#endif  /* NUM_SYS_256 */

#ifndef NUM_SYS_512
#define NUM_SYS_512		       128	/* 512 byte system clusters */
#endif  /* NUM_SYS_512 */

#ifndef NUM_SYS_CL_BLKS
#define NUM_SYS_CL_BLKS			(NUM_SYS_64  + NUM_SYS_128 + \
					 NUM_SYS_256 + NUM_SYS_512)
#endif  /* NUM_SYS_CL_BLKS */

#ifndef NUM_SYS_MBLKS
#define NUM_SYS_MBLKS			(2 * NUM_SYS_CL_BLKS)
#endif  /* NUM_SYS_MBLKS */

/* system clusters for a minimal configuration */

#define NUM_SYS_64_MIN			20	/*  64 byte system clusters */
#define NUM_SYS_128_MIN			15	/* 128 byte system clusters */
#define NUM_SYS_256_MIN			15	/* 256 byte system clusters */
#define NUM_SYS_512_MIN			10	/* 512 byte system clusters */

#define NUM_SYS_CL_BLKS_MIN		(NUM_SYS_64_MIN  + NUM_SYS_128_MIN + \
					 NUM_SYS_256_MIN + NUM_SYS_512_MIN)

#define NUM_SYS_MBLKS_MIN		NUM_SYS_CL_BLKS_MIN

/* cluster defines */

#define CL_LOG2_64			6	
#define CL_LOG2_128			7
#define CL_LOG2_256			8
#define CL_LOG2_512			9
#define CL_LOG2_1024			10
#define CL_LOG2_2048			11
#define CL_LOG2_4096	 		12
#define CL_LOG2_8192 			13
#define CL_LOG2_16384			14
#define CL_LOG2_32768			15
#define CL_LOG2_65536			16

#define CL_SIZE_64			64
#define CL_SIZE_128			128
#define CL_SIZE_256			256
#define CL_SIZE_512			512
#define CL_SIZE_1024			1024
#define CL_SIZE_2048			2048
#define CL_SIZE_4096			4096
#define CL_SIZE_8192			8192
#define CL_SIZE_16384			16384
#define CL_SIZE_32768			32768
#define CL_SIZE_65536			65536

#define CL_LOG2_MIN			CL_LOG2_64
#define CL_LOG2_MAX			CL_LOG2_65536
#define CL_SIZE_MAX			(1 << CL_LOG2_MAX)
#define CL_SIZE_MIN			(1 << CL_LOG2_MIN)
#define CL_INDX_MIN			0
#define CL_INDX_MAX			(CL_LOG2_MAX - CL_LOG2_MIN)
#define CL_TBL_SIZE			(CL_INDX_MAX + 1)


#define CL_LOG2_TO_CL_INDEX(x)		((x) - CL_LOG2_MIN)
#define CL_LOG2_TO_CL_SIZE(x)		(1 << (x))
#define SIZE_TO_LOG2(size)		(ffsMsb((size)) - 1)
#define CL_SIZE_TO_CL_INDEX(clSize) 	(ffsMsb((clSize)) - (1 + CL_LOG2_MIN))

/* mBlk types */
#define	MT_FREE		0	/* should be on free list */
#define	MT_DATA		1	/* dynamic (data) allocation */
#define	MT_HEADER	2	/* packet header */
#define	MT_SOCKET	3	/* socket structure */
#define	MT_PCB		4	/* protocol control block */
#define	MT_RTABLE	5	/* routing tables */
#define	MT_HTABLE	6	/* IMP host tables */
#define	MT_ATABLE	7	/* address resolution tables */
#define	MT_SONAME	8	/* socket name */
#define MT_ZOMBIE       9       /* zombie proc status */
#define	MT_SOOPTS	10	/* socket options */
#define	MT_FTABLE	11	/* fragment reassembly header */
#define	MT_RIGHTS	12	/* access rights */
#define	MT_IFADDR	13	/* interface address */
#define MT_CONTROL	14	/* extra-data protocol message */
#define MT_OOBDATA	15	/* expedited data  */
#define	MT_IPMOPTS	16	/* internet multicast options */
#define	MT_IPMADDR	17	/* internet multicast address */
#define	MT_IFMADDR	18	/* link-level multicast address */
#define	MT_MRTABLE	19	/* multicast routing tables */

#define NUM_MBLK_TYPES	20	/* number of mBlk types defined */

/* mBlk flags */

#define	M_EXT		0x01	/* has an associated cluster */
#define	M_PKTHDR	0x02	/* start of record */
#define	M_EOR		0x04	/* end of record */
#define M_FORWARD       0x08    /* packet to be forwarded */

/* mBlk pkthdr flags, also in mFlags */

#define	M_BCAST		0x10	/* send/received as link-level broadcast */
#define	M_MCAST		0x20	/* send/received as link-level multicast */
#define M_PROXY         0x40    /* broadcast forwarded through proxy */
#define M_SECURE_PKT    0x80   /* already gone through security check */ 

/*
 * M_EOB is set when the mblk contains the last part of a large block of
 * data sent by an application using TCP (a large block of data is one
 * which causes sosend to issue multiple protocol send requests).
 * M_EOB and M_EOR can be used together since M_EOB is only used for TCP
 * and M_EOR is only used for datagram protocols (UDP, raw IP).
 */

#define M_EOB           M_EOR

/* flags to mClGet/mBlkGet */

#define	M_DONTWAIT	1	/* dont wait if buffers not available */
#define	M_WAIT		0	/* wait if buffers not available */

/* length to copy to copy all data from the mBlk chain*/

#define	M_COPYALL	1000000000

/* check to see if an mBlk is associated with a cluster */

#define M_HASCL(pMblk)	((pMblk)->mBlkHdr.mFlags & M_EXT)

#define	M_BLK_SZ	sizeof(struct mBlk) 	/* size of an mBlk */
#define	MSIZE		M_BLK_SZ	/* size of an mBlk */

#define CL_BLK_SZ	sizeof(struct clBlk)	/* size of cluster block */

/* macro to get to the net pool ptr from the mBlk */

#define MBLK_TO_NET_POOL(pMblk) (*(NET_POOL_ID *)((int)pMblk - sizeof(int)))

/* macro to get to the cluster pool ptr from the cluster buffer */

#define CL_BUF_TO_CL_POOL(pClBuf) (*(CL_POOL_ID *)((int)pClBuf - sizeof(int)))

/* macro to get to the cluster reference count from the cluster buffer */

#define CL_BUF_TO_REFCNT(pClBuf)  (UCHAR *)((int)pClBuf - (2 * sizeof(int)))

/* macros for accessing the functions of net pool directly */

#define poolInit(poolId,pMclBlkConfig,pClDescTbl,tblNumEnt,fromKheap)	\
    		(*(((NET_POOL_ID)(poolId))->pFuncTbl->pInitRtn)) 	\
                 ((poolId), (pMclBlkConfig), (pClDescTbl), (tblNumEnt), \
		  (fromKheap))
    
#define mBlkFree(poolId,pMblk)						\
    		(*(((NET_POOL_ID)(poolId))->pFuncTbl->pMblkFreeRtn)) 	\
                 ((poolId), (pMblk))

#define clFree(poolId,pClBuf)						\
    		(*(((NET_POOL_ID)(poolId))->pFuncTbl->pClFreeRtn)) 	\
                 ((poolId), (pClBuf))

#define clBlkFree(poolId,pClBlk)					\
    		(*(((NET_POOL_ID)(poolId))->pFuncTbl->pClBlkFreeRtn)) 	\
                 ((pClBlk))
                    
#define mBlkClFree(poolId,pMblk)					\
    		(*(((NET_POOL_ID)(poolId))->pFuncTbl->pMblkClFreeRtn)) 	\
                 ((poolId), (pMblk))

#define mBlkGet(poolId,canWait,type)					\
                 (*(((NET_POOL_ID)(poolId))->pFuncTbl->pMblkGetRtn)) 	\
                 ((poolId), (canWait), (type))

#define clBlkGet(poolId,canWait)					\
                 (*(((NET_POOL_ID)(poolId))->pFuncTbl->pClBlkGetRtn)) 	\
                 ((poolId), (canWait))
                    
#define clusterGet(poolId,pClPool)					\
    		(*(((NET_POOL_ID)(poolId))->pFuncTbl->pClGetRtn)) 	\
                 ((poolId), (pClPool))
                    
#define mClGet(poolId,pMblk,bufSize,canWait,bestFit)			\
    		(*(((NET_POOL_ID)(poolId))->pFuncTbl->pMblkClGetRtn)) 	\
                 ((poolId), (pMblk), (bufSize), (canWait), (bestFit))

#define clPoolIdGet(poolId,bufSize,bestFit)				\
    		(*(((NET_POOL_ID)(poolId))->pFuncTbl->pClPoolIdGetRtn))	\
                 ((poolId), (bufSize), (bestFit))

/*
 * This structure is used to configure the number of mBlks and cluster blocks
 * that are allocated in a netpool. Each cluster has a corresponding cluster
 * block which manages it. An mBlk manages a cluster block and the cluster.
 * When a netpool is configured there should as many cluster blocks as the
 * cumulative number of clusters. The defaults are defined above, but the
 * structure is defined and fileed in usrConfig.c. For TCP applications it is
 * good to have two mBlks for every cluster block.
 */

typedef struct
    {
    int		mBlkNum;		/* number of mBlks */
    int		clBlkNum;		/* number of cluster Blocks */
    char * 	memArea;		/* pre allocated memory area */
    int		memSize;		/* pre allocated memory size */
    } M_CL_CONFIG;

/*
 *  This structure defines the way the networking code utilizes clusters.
 *  A table of type CL_DESC is filled in usrNetwork.c. 
 *  Clusters are also used to hold network data structures other than the 
 *  regular data. The network data structures are routing table entries,
 *  interface address structures, multicast address structures etc.
 */

typedef struct clDesc
    {
    int		clSize;			/* cluster type */
    int		clNum;			/* number of clusters */
    char *	memArea;		/* pre allocated memory area */
    int		memSize;		/* pre allocated memory size */
    } CL_DESC; 

/* this structure defines the cluster */

typedef struct clBuff
    {
    struct clBuff *	pClNext; 	/* pointer to the next clBuff */
    } CL_BUF;

typedef CL_BUF * CL_BUF_ID; 

/* this structure defines the each cluster pool */

typedef struct clPool
    {
    int			clSize;		/* cluster size */
    int			clLg2;		/* cluster log 2 size */
    int			clNum; 		/* number of clusters */
    int			clNumFree; 	/* number of clusters free */
    int			clUsage;	/* number of times used */
    CL_BUF_ID		pClHead;	/* pointer to the cluster head */
    struct netPool *	pNetPool;	/* pointer to the netPool */
    } CL_POOL; 

typedef CL_POOL * CL_POOL_ID; 

/* header at beginning of each mBlk */

typedef struct mHdr
    {
    struct mBlk *	mNext;		/* next buffer in chain */
    struct mBlk *	mNextPkt;	/* next chain in queue/record */
    char *		mData;		/* location of data */
    int			mLen;		/* amount of data in this mBlk */
    UCHAR		mType;		/* type of data in this mBlk */
    UCHAR		mFlags;		/* flags; see below */
    USHORT		reserved; 
    } M_BLK_HDR;

/* record/packet header in first mBlk of chain; valid if M_PKTHDR set */

typedef struct	pktHdr
    {
    struct ifnet *	rcvif;		/* rcv interface */
    int			len;		/* total packet length */
    } M_PKT_HDR;

typedef union clBlkList
    {
    struct clBlk * 	pClBlkNext;	/* pointer to the next clBlk */
    char * 		pClBuf;		/* pointer to the buffer */
    } CL_BLK_LIST;
    
/* description of external storage mapped into mBlk, valid if M_EXT set */

typedef struct clBlk
    {
    CL_BLK_LIST 	clNode;		/* union of next clBlk, buffer ptr */
    UINT		clSize;		/* cluster size */
    int			clRefCnt;	/* reference count of the cluster */
    FUNCPTR		pClFreeRtn;	/* pointer to cluster free routine */
    int			clFreeArg1;	/* free routine arg 1 */
    int			clFreeArg2;	/* free routine arg 2 */
    int			clFreeArg3;	/* free routine arg 3 */
    struct netPool *	pNetPool;	/* pointer to the netPool */
    } CL_BLK;

/* mBlk structure */

typedef struct mBlk
    {
    M_BLK_HDR 	mBlkHdr; 		/* header */
    M_PKT_HDR	mBlkPktHdr;		/* pkthdr */
    CL_BLK *	pClBlk;			/* pointer to cluster blk */
    } M_BLK;

/* mBlk statistics used to show data pool by show routines */

typedef struct mbstat
    {
    ULONG	mNum;			/* mBlks obtained from page pool */
    ULONG	mDrops;			/* times failed to find space */
    ULONG	mWait;			/* times waited for space */
    ULONG	mDrain;			/* times drained protocols for space */
    ULONG	mTypes[256];		/* type specific mBlk allocations */
    } M_STAT;

typedef M_BLK * 	M_BLK_ID;
typedef CL_BLK *	CL_BLK_ID;
typedef struct netPool 	NET_POOL;
typedef struct poolFunc POOL_FUNC;
typedef NET_POOL * 	NET_POOL_ID;

struct	poolFunc			/* POOL_FUNC */
    {
    /* pointer to the pool initialization routine */
    STATUS	(*pInitRtn) (NET_POOL_ID pNetPool, M_CL_CONFIG * pMclBlkConfig,
                             CL_DESC * pClDescTbl, int clDescTblNumEnt,
			     BOOL fromKheap);

    /* pointer to mBlk free routine */
    void	(*pMblkFreeRtn) (NET_POOL_ID pNetPool, M_BLK_ID pMblk);

    /* pointer to cluster Blk free routine */
    void	(*pClBlkFreeRtn) (CL_BLK_ID pClBlk);

    /* pointer to cluster free routine */
    void	(*pClFreeRtn) (NET_POOL_ID pNetPool, char * pClBuf);

    /* pointer to mBlk/cluster pair free routine */
    M_BLK_ID 	(*pMblkClFreeRtn) (NET_POOL_ID pNetPool, M_BLK_ID pMblk);

    /* pointer to mBlk get routine */
    M_BLK_ID	(*pMblkGetRtn) (NET_POOL_ID pNetPool, int canWait, UCHAR type);

    /* pointer to cluster Blk get routine */
    CL_BLK_ID	(*pClBlkGetRtn) (NET_POOL_ID pNetPool, int canWait);
    
    /* pointer to a cluster buffer get routine */
    char *	(*pClGetRtn) (NET_POOL_ID pNetPool, CL_POOL_ID pClPool);

    /* pointer to mBlk/cluster pair get routine */
    STATUS	(*pMblkClGetRtn) (NET_POOL_ID pNetPool, M_BLK_ID pMblk,
                                  int bufSize, int canWait, BOOL bestFit);

    /* pointer to cluster pool Id get routine */
    CL_POOL_ID	(*pClPoolIdGetRtn) (NET_POOL_ID pNetPool, int	bufSize,
                                    BOOL bestFit);
    };

struct netPool				/* NET_POOL */
    {
    M_BLK_ID	pmBlkHead;		/* head of mBlks */
    CL_BLK_ID	pClBlkHead;		/* head of cluster Blocks */
    int		mBlkCnt;		/* number of mblks */
    int		mBlkFree;		/* number of free mblks - deprecated,
                                           use pPoolStat->mTypes[MT_FREE]
                                           instead */
    int		clMask;			/* cluster availability mask */
    int		clLg2Max;		/* cluster log2 maximum size */
    int		clSizeMax;		/* maximum cluster size */
    int		clLg2Min;		/* cluster log2 minimum size */
    int		clSizeMin;		/* minimum cluster size */
    CL_POOL * 	clTbl [CL_TBL_SIZE];	/* pool table */
    M_STAT *	pPoolStat;		/* pool statistics */
    POOL_FUNC *	pFuncTbl;		/* ptr to function ptr table */
    };

/* external declarations */                    
                    
IMPORT STATUS 		netBufLibInit (void);
IMPORT STATUS 		netPoolInit (NET_POOL_ID pNetPool,
                                     M_CL_CONFIG * pMclBlkConfig,
                                     CL_DESC * pClDescTbl, int clDescTblNumEnt,
                                     POOL_FUNC * pFuncTbl);
IMPORT STATUS		netPoolDelete (NET_POOL_ID);
IMPORT void		netMblkFree (NET_POOL_ID pNetPool, M_BLK_ID pMblk);
IMPORT void		netClBlkFree (NET_POOL_ID pNetPool, CL_BLK_ID pClBlk);
IMPORT void 		netClFree (NET_POOL_ID pNetPool, UCHAR * pClBuf);
IMPORT M_BLK_ID 	netMblkClFree (M_BLK_ID pMblk);
IMPORT void		netMblkClChainFree (M_BLK_ID pMblk);
IMPORT M_BLK_ID 	netMblkGet (NET_POOL_ID pNetPool, int canWait,
                                    UCHAR type);
IMPORT CL_BLK_ID	netClBlkGet (NET_POOL_ID pNetPool, int canWait);
IMPORT char *	 	netClusterGet (NET_POOL_ID pNetPool,
                                       CL_POOL_ID pClPool);
IMPORT STATUS 	 	netMblkClGet (NET_POOL_ID pNetPool, M_BLK_ID pMblk,
                                      int bufSize, int canWait, BOOL bestFit);
IMPORT M_BLK_ID 	netTupleGet (NET_POOL_ID pNetPool, int bufSize,
                                     int canWait, UCHAR type, BOOL bestFit);

IMPORT M_BLK_ID 	netTupleGet2 (NET_POOL_ID, int, int);

IMPORT CL_BLK_ID  	netClBlkJoin (CL_BLK_ID pClBlk, char * pClBuf,
                                      int size, FUNCPTR pFreeRtn, int arg1,
                                      int arg2, int arg3);
IMPORT M_BLK_ID  	netMblkClJoin (M_BLK_ID pMblk, CL_BLK_ID pClBlk);
IMPORT CL_POOL_ID 	netClPoolIdGet (NET_POOL_ID pNetPool, int bufSize,
                                        BOOL bestFit);

IMPORT int 		netMblkToBufCopy (M_BLK_ID pMblk, char * pBuf,
                                          FUNCPTR pCopyRtn);
IMPORT int		netMblkOffsetToBufCopy (M_BLK_ID pMblk, int offset,
                                                char * pBuf, int len,
                                                FUNCPTR	pCopyRtn);
IMPORT M_BLK_ID 	netMblkDup (M_BLK_ID pSrcMblk, M_BLK_ID	pDestMblk);
IMPORT M_BLK_ID 	netMblkChainDup (NET_POOL_ID pNetPool, M_BLK_ID	pMblk,
                                         int offset, int len, int canWait);

#if CPU_FAMILY==I960
#pragma align 0			/* turn off alignment requirement */
#endif	/* CPU_FAMILY==I960 */

#ifdef __cplusplus
}
#endif

#endif /* __INCnetBufLibh */

