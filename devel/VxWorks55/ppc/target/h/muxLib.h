/* muxLib.h - definitions for the MUX library */

/* Copyright 1984 - 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
02c,13jan03,rae  Merged from velocecp branch (SPR #83258)
02b,10oct01,rae  merge from truestack ver 02g, base 01z (SPRs 32626, 64406)
02a,16oct00,spm  merged version 02b from tor3_0_x branch (adds support for
                 link-level broadcasts and fixes reboot mechanism)
01z,29apr99,pul  Upgraded NPT phase3 code to tor2.0.0
01y,02mar99,pul  added muxShow() declaration, removed muxDevNameGet: SPR# 24242
01x,13nov98.n_s  add muxDevStopAll. spr #23229
01w,19mar99,sj   removed nptHook structure
01v,27oct98,pul  changed nptHookRtn to return STATUS 
01u,12oct98,sj   added TK hooks and errors
01t,08dec97,gnn  END code review fixes.
01s,17oct97,vin  changed prototypes.
01r,09sep98,ham  corrected the comment for MUX_MAX_IFTYPE.
01q,09sep98,ham  associate MUX_MAX_TYPE with IFT_MAX_TYPE,SPR#22298.
01p,08sep98,ham  moved MUX_MAX_TYPE from muxLib.c,SPR#22298.
01o,06oct97,gnn  added include of end.h which we need for prototypes
01n,03oct97,gnn  fixed SPR 8986, added function prototypes
01m,25sep97,gnn  SENS beta feedback fixes
01l,22aug97,gnn  update of polled mode.
01k,19aug97,gnn  changes due to new buffering scheme.
01j,12aug97,gnn  changes necessitated by MUX/END update.
01i,15may97,gnn  added new protocol type MUX_PROTO_OUTPUT
01h,17apr97,gnn  added muxDevNameGet
                 changed muxDevLoad to return void* to device
01g,07apr97,gnn  added new error defines.
                 modified muxDevLoad to follow new paradigm.
01f,12mar97,gnn  fixed a bug in muxReceive's calling API.
01e,03feb97,gnn  Modified muxBuf code to be more generic and support other,
                 non-TMD systems.
01d,21jan97,gnn  Changed interfaces for muxSend.
                 Removed TxBuf stuff.
                 Added buffer management stuff.
01c,23oct96,gnn  name changes to follow coding standards.
01b,22oct96,gnn  added MUX_PROTO_PROMISC define to hold definition
                 of a pomiscuous protocol
                 replaced netVectors with netBuffers
01a,05May96,gnn	 written.

*/
 
/*
DESCRIPTION
This file includes function prototypes for the MUX.

INCLUDE FILES:
*/

#ifndef __INCmuxLibh
#define __INCmuxLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */
#include "net/if_types.h"
#include "end.h"
/* defints */

/* MUX_MAX_IFTYPE associates with IFT_MAX_TYPE as default. If the user wants
 * to add a resource function, verify it's smaller than MUX_MAX_IFTYPE.
 */
#define MUX_MAX_IFTYPE IFT_MAX_TYPE

/* Error defines. */
#define S_muxLib_LOAD_FAILED                  (M_muxLib | 1)
#define S_muxLib_NO_DEVICE                    (M_muxLib | 2)
#define S_muxLib_INVALID_ARGS                 (M_muxLib | 3)
#define S_muxLib_ALLOC_FAILED                 (M_muxLib | 4)
#define S_muxLib_ALREADY_BOUND                (M_muxLib | 5)
#define S_muxLib_UNLOAD_FAILED                (M_muxLib | 6)
#define S_muxLib_NOT_A_TK_DEVICE              (M_muxLib | 7)
#define S_muxLib_NO_TK_DEVICE                 (M_muxLib | 8)
#define S_muxLib_END_BIND_FAILED              (M_muxLib | 9)


/* These are above all SAPs but still in the ethernet size range */
/* so won't interfere with RFC 1700 */
#define MUX_PROTO_PROMISC	0x100	
#define MUX_PROTO_SNARF         0x101	
#define MUX_PROTO_OUTPUT        0x102

/* The modes in which the MUX can operate.  Part of WRS 1152. */
enum MUX_MODE {MUX_MODE_NORM, MUX_MODE_POLL};
#define MUX_POLL_TASK_PRIORITY 255
#define MUX_POLL_TASK_DELAY 0

/* Defines for the MUX in polled mode. */

/* typedefs */

/* Module Object Definition */

typedef struct _muxLib
    {
    int debug;
    SEM_ID lock;                  /* To lock on muxDevLoads. */
    int mode;
    int priority;
    int taskDelay;
    int taskID;
    } muxLib;

/*
 * These data structures define the 2D list of address resolution
 * functions.  This list is ordered by ifType and the protocol.
 */
typedef struct mux_addr_rec
    {
    NODE node;
    long protocol;
    FUNCPTR addrResFunc;
    } MUX_ADDR_REC;

/* globals */

/* locals */

/* forward declarations */

STATUS muxLibInit ();
IMPORT void * muxDevLoad(int unit, END_OBJ* (*endLoad) (char *, void*),
                           char *initString,
                           BOOL loaning, void*pBSP);
STATUS muxDevStart(void* pCookie);
STATUS muxDevStop(void* pCookie);
void muxShow(char* pDevName, int unit);
IMPORT STATUS muxDevStopAll (int timeout);
IMPORT STATUS muxDevUnload(char* pName, int unit);
void * muxBind (char * pName, int unit,
                  BOOL (*stackRcvRtn) (void*, long,M_BLK_ID, LL_HDR_INFO *,
                                       void*),
                  STATUS (*stackShutdownRtn) (void*, void*),
                  STATUS (*stackTxRestartRtn) (void*, void*),
                  void (*stackErrorRtn) (END_OBJ*, END_ERR*, void*),
                  long type, char* pProtoName, void* pSpare);
IMPORT STATUS muxUnbind(void* pCookie, long type, FUNCPTR stackShutdownRtn);
IMPORT STATUS muxSend(void* pCookie, M_BLK_ID pNBuff);
IMPORT STATUS muxReceive(void* pCookie, M_BLK_ID pNBuff);
IMPORT STATUS muxPollSend(void* pCookie, M_BLK_ID pNBuff);
IMPORT STATUS muxPollReceive(void* pCookie, M_BLK_ID pNBuff);
IMPORT STATUS muxIoctl(void* pCookie, int cmd, caddr_t data);
IMPORT STATUS muxMCastAddrAdd(void* pCookie, char* pAddress);
IMPORT STATUS muxMCastAddrDel(void* pCookie, char* pAddress);
IMPORT STATUS muxMCastAddrGet(void* pCookie, MULTI_TABLE* pTable);
IMPORT char* muxLoanGet(void* pCookie);
IMPORT UINT8* muxRefCntGet(void* pCookie);
IMPORT void muxLoanRet(void* pCookie, char* pData, UINT8* pRef);
IMPORT M_BLK_ID muxLinkHeaderCreate(void* pCookie, M_BLK_ID pPacket,
                               M_BLK_ID pSrcAddr, M_BLK_ID pDstAddr,
                               BOOL bcastFlag);
IMPORT M_BLK_ID muxAddressForm(void* pCookie, M_BLK_ID pMblk,
                               M_BLK_ID pSrcAddr, M_BLK_ID pDstAddr);
IMPORT STATUS muxPacketDataGet(void* pCookie, M_BLK_ID pMblk, LL_HDR_INFO *);
IMPORT STATUS muxPacketAddrGet (void* pCookie, M_BLK_ID pMblk,
                                M_BLK_ID pSrc,
                                M_BLK_ID pDst,
                                M_BLK_ID pESrc,
                                M_BLK_ID pEDst);
IMPORT void muxTxRestart(void* pCookie);

IMPORT void muxError ( void* pCookie, END_ERR* pError );


IMPORT END_OBJ* endFindByName(char* pName, int unit);
IMPORT STATUS muxAddrResFuncDel ( long ifType, long protocol );
IMPORT FUNCPTR muxAddrResFuncGet ( long ifType, long protocol );
IMPORT STATUS muxAddrResFuncAdd ( long ifType, long protocol,
                                  FUNCPTR addrResFunc);

IMPORT BOOL muxDevExists (char* pName, int unit);

IMPORT STATUS muxTaskDelaySet ( int delay );
IMPORT STATUS muxTaskDelayGet ( int* pDelay );
IMPORT STATUS muxTaskPrioritySet ( int priority );
IMPORT STATUS muxTaskPriorityGet ( int* pPriority );
IMPORT void * muxProtoPrivDataGet (END_OBJ * pEnd, int proto);
IMPORT STATUS muxIterateByName (char * pName,
				FUNCPTR pCallbackRtn,
                                void * pCallbackArg);

IMPORT STATUS muxPollDevAdd (int, char *);
IMPORT STATUS muxPollDevDel (int, char *);
IMPORT BOOL   muxPollDevStat (int, char *);
IMPORT void   muxPollTask (void);
IMPORT STATUS muxPollStart (int, int, int);
IMPORT STATUS muxPollEnd (void);

#ifdef __cplusplus
}
#endif

#endif /* __INCmuxLibh */
