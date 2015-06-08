/* end.h - Structure for the MUX and END API*/

/* Copyright 1984 - 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
03h,13jan03,rae  Merged from velocecp branch
03g,23oct01,rae  merge from truestack (fix some ioctl values)
03f,16oct01,rcs  added END_ERR_NO_BUF flag to notify mux of driver cluster
                 exhaustion. SPR# 70545
01z,13jul01,ann  added a flag for RFC2233 driver support to eliminate the
                 global mibStyle variable
03e,09nov00,spm  removed pNptCookie from END_OBJ for binary compatibility
03d,07nov00,spm  removed nptFlag from END_OBJ for T2 END binary compatibility
03d,25oct00,niq  Merge RFC2233 changes from open_stack-p1-r1 branch
                 03c,10mar00,ead  Modifed the END object to support RFC 
                 variables
03c,16oct00,spm  merged version 01v from tor3_0_x branch (base version 01r):
                 adds backward compatibility fixes and support for multiple
                 snarf protocols and link-level broadcasts
03b,29apr99,pul  Upgraded NPT phase3 code to tor2.0.0
03a,29mar99,pul  altered END_QUERY struct to conform to WRS coding standard
02z,24mar99,sj   NET_PROTOCOL flags field is used by MUX
02y,05mar99,sj   added END_QUERY; added END_ERR_FLAGS
02x,02mar99,sj   fixed IOCTL values so that last byte of IOCTL cmd is <255
02w,22feb99,sj   added EIOCGBCASTADDR, EIOCGADDRLEN, EIOCGBIND ioctls
01v,15oct98,pul  changed the call back routines not to specify argument list 
01u,08oct98,sj   EIOCGNPT ioctl command for NPT
01t,29sep98,fle  doc : made it refgen parsable
01s,10sep98,pul  added endBind  and EIOCGNPT command
01r,25aug98,n_s  update comments for DEV_OBJ. spr 21546.
01q,17jan98,gnn  fixed a bug in output filtering.
01p,08dec97,gnn  END code review fixes.
01o,17oct97,vin  added llHdrInfo, changed prototypes.
01n,03oct97,gnn  fixed SPR 8986, added proper function prototypes
01m,25sep97,gnn  SENS beta feedback fixes
01l,19aug97,gnn  changes due to new buffering scheme.
01k,12aug97,gnn  changes necessitated by MUX/END update.
01j,02jun97,gnn  added EIOCGMWIDTH to get memory width from driver.
01i,15may97,gnn  added outputFilter element to device structure.
01h,07apr97,gnn  added END_TBL_ENTRY for new boot loading code.
01g,26feb97,gnn  Added END_BUF_MUX and END_BUF_DRIVER defines.
01f,21jan97,gnn  removed all buffer loaning stuff.
                 added buffer pool variables and calls.
                 added a IOCTL to get the minimumb first buffer in a
                 scatter/gather system.
01e,27nov96,gnn  added MIB 2 Ioctl.
01d,07nov96,gnn  fixed the flags fields to be int.
01c,23oct96,gnn  name changes to follow coding standards.
01b,22oct96,gnn  added pragmas for the i960.
                 removed netVector structure and added a pNext pointer
                 to the NET_BUFFER.
                 added function pointers for the start and stop routines.
                 changed the ioctls to be noun->verb
01a,18apr96,gnn	 written.

*/
 
/*
DESCRIPTION

This file defines all the structures uses by the MUX and any END.  These
structures are either passed by functions in either the MUX or END
or stored by the MUX.
*/

#ifndef __INCendh
#define __INCendh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "semLib.h"
#include "m2Lib.h"	/* For MIBII counters. */
#include "lstLib.h"	/* List library for linked structures. */
#include "memLib.h"
#include "netBufLib.h"
#include "net/if_llc.h"

#if CPU_FAMILY==I960
#pragma align 1			/* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/*
 * 
 * Flags defined for the END.
 *
 */

#define END_DEBUG 0x1000
/*
 * Flags for the type of a receive buffer that we've got.
 */

#define END_BUF_COPY 	0x1	/* Protocol must copy data. */
#define END_BUF_LOAN 	0x2	/* We're loaning this buf. */

/*
 * Flags for buffers that are to be sent.
 */
#define END_BUF_VECTOR 0x4      /* The NET_BUFFER contains a pointer to an */
                                /* array of NET_BUFFERS and not just one. */
#define END_BUF_MUX    0x8      /* The NET_BUFFER was allocated by the MUX */
#define END_BUF_DRIVER 0x10     /* The NET_BUFFER was allocated by the */
                                /* driver. */

/* Flag denoting the support for RFC 2233 MIB variables */

#define END_MIB_2233   0x10000  /* RFC2233 supported - value chosen to 
                                 * prevent conflict with IFF_* flags in if.h 
                                 */

#define END_INIT_STR_MAX 255
#define END_PROTO_NAME_MAX 32
#define END_NAME_MAX 8          /* The maximum length of a device name */
#define END_DESC_MAX 80         /* The maximum length of device desription. */
/*
 * ERROR Return for device blocked on transmit.
 */

#define END_ERR_BLOCK -2

/*
 * ERROR protocol defines.  These are stored in a 32 bit word in the
 * end_err structure.  User apps can have the upper 16 bits, we get the
 * lower 16.
 */
#define END_ERR_INFO     1   /* Information only */
#define END_ERR_WARN     2   /* Warning */
#define END_ERR_RESET    3   /* Device has reset. */
#define END_ERR_DOWN     4   /* Device has gone down. */
#define END_ERR_UP       5   /* Device has come back on line. */
#define END_ERR_FLAGS	 6   /* Device flags changed */
#define END_ERR_NO_BUF   7   /* Device's cluster pool exhausted. */ 

typedef struct end_err
    {
    INT32 errCode;           /* Error code, see above. */
    char* pMesg;             /* NULL terminated error message */
    void* pSpare;            /* Pointer to user defined data. */
    } END_ERR;
    
/* A specific instance of an Ethernet multicast address. */
typedef struct 
    {
    NODE node;		     /* How we stay in a list. */
    char  addr[6];	     /* address */
    long   refcount;	     /* no. claims to this addr*/
    } ETHER_MULTI; 

typedef struct 
    {
    long len;		/* Length of table in bytes. */
    char *pTable;		/* Pointer to entries. */
    } MULTI_TABLE;

/* structure used as a vehicle to move data during a EIOCQUERY ioctl */

typedef struct
    {
    int query;		/* the query */
    int queryLen;	/* length of data expected/returned */
    char queryData[4]; /* allocate a minimum of 4 bytes; expandable upto 120 */
    } END_QUERY;

/*
 *
 * DEV_OBJ - device specific control object
 *
 * The DEV_OBJ structure is the glue linking the device generic END_OBJ 
 * structure with the device specific data object referenced by pDevice.
 */

typedef struct dev_obj 
    {
    char name[END_NAME_MAX];
    int unit;
    char description[END_DESC_MAX];
    void* pDevice;		/* Pointer back to the device data. */
    } DEV_OBJ;
	
/*
* 
* LL_HDR_INFO - link level Header info 
*
* This data structure defines information that is only relevant to
* a stack receive routine.
*
*/

typedef struct llHdrInfo
    {
    int		destAddrOffset;		/* destination addr offset in mBlk */
    int		destSize; 		/* destination address size */
    int		srcAddrOffset;		/* source address offset in mBlk */
    int		srcSize;		/* source address size */
    int		ctrlAddrOffset;		/* control info offset in mBlk */
    int		ctrlSize;		/* control info size */
    int		pktType;		/* type of the packet */
    int		dataOffset;		/* data offset in the mBlk */
    } LL_HDR_INFO;

/*
 *
 * endObject - the basic end object that everyone derives from
 *
 * This data structure defines a device independant amount of state
 * that is maintained by all drivers/devices.
 * Each specific device (Lance, Intel, etc.) derives from this object
 * first and then incorporates it's own data structures after it.
 *
 */
typedef struct end_object
    {
    NODE node;
#ifndef _WRS_VXWORKS_5_X
    BOOL nptFlagSpace;          /* Had indicated type of interface: END or NPT. Now: UNUSED */
#endif /* _WRS_VXWORKS_5_X */
    DEV_OBJ devObject;		/* Root of the device heirarchy. */
    STATUS (*receiveRtn) ();
                                /* Routine to call on reception. */
    struct net_protocol *outputFilter;
                                /* Optional output filter routine. */
    void* pOutputFilterSpare;   /* Output filter's spare pointer */
    BOOL attached;		/* Indicates unit is attached. */
    SEM_ID txSem;		/* Transmitter semaphore. */
    long flags;			/* Various flags. */
    struct net_funcs *pFuncTable;	/* Function table. */
    M2_INTERFACETBL mib2Tbl;	/* MIBII counters. */
    LIST multiList;		/* Head of the multicast address list */
    int	nMulti;			/* Number of elements in the list. */
    LIST protocols;		/* Protocol node list. */
    int snarfCount;             /* Number of snarf protocols at head of list */
    NET_POOL_ID pNetPool;       /* Memory cookie used by MUX buffering. */
#ifndef _WRS_VXWORKS_5_X
    void * pNptCookie; 		/* Access to NPT BIB (for unloading). UNUSED */
#endif /* _WRS_VXWORKS_5_X */
    M2_ID * pMib2Tbl;           /* RFC 2233 MIB objects */
    } END_OBJ;

    
/*
 * 
 * NET_PROTOCOL - protocol structure
 *
 * This data structure defines information that is only relevant to
 * a protocol.  This structure hangs from a singly linked list whose
 * head is stored in a netDevice.
 *
 */
typedef struct net_protocol 
    {
    NODE node;			/* How we stay in a list. */
    char name[END_PROTO_NAME_MAX];     /* String name for this protocol. */
    long type;			/* Protocol type from RFC 1700 */
    BOOL nptFlag; 		/* NPT or original MUX prototype? */
    BOOL (*stackRcvRtn) ();
                                /* The routine to call when we get */
				/* a packet. */
    STATUS (*stackShutdownRtn) ();
                                /* The routine to call to shutdown */
				/* the protocol stack. */
    STATUS (*stackTxRestartRtn) ();
                                /* Callback for restarting on blocked tx. */
    void (*stackErrorRtn) ();
                                /* Callback for device errors. */
    END_OBJ * pEnd; 		/* Device bound to protocol. */
    void* pSpare;               /* Spare pointer that can be passed to */
                                /* the protocol. */
    void * pNptCookie; 		/* Access to NPT BIB (for unbinding). */
    } NET_PROTOCOL;



/*
 * 
 * NET_FUNCS - driver function table
 *
 * This is a table that is created, one per driver, to hold all the
 * function pointers for that driver.  In this way we can have only one
 * instance to this structer, but one pointer per netDevice structure.
 */

typedef struct net_funcs
    {
    STATUS (*start) (END_OBJ*);		   /* Driver's start func. */
    STATUS (*stop) (END_OBJ*);		   /* Driver's stop func. */
    STATUS (*unload) (END_OBJ*);		   /* Driver's unload func. */
    int (*ioctl) (END_OBJ*, int, caddr_t);    /* Driver's ioctl func. */
    STATUS (*send) (END_OBJ* , M_BLK_ID);	   /* Driver's send func. */
    STATUS (*mCastAddrAdd) (END_OBJ*, char*); /* Driver's mcast add func. */
    STATUS (*mCastAddrDel) (END_OBJ*, char*); /* Driver's mcast delete func. */
    STATUS (*mCastAddrGet) (END_OBJ*, MULTI_TABLE*);
                                           /* Driver's mcast get fun. */
    STATUS (*pollSend) (END_OBJ*, M_BLK_ID);  /* Driver's polling send func. */
    STATUS (*pollRcv) (END_OBJ*, M_BLK_ID);   /* Driver's polling receive func. */
    M_BLK_ID (*formAddress) (M_BLK_ID, M_BLK_ID, M_BLK_ID, BOOL);
                                           /* Driver's addr formation func. */
    STATUS (*packetDataGet) (M_BLK_ID, LL_HDR_INFO *);
                                           /* Driver's packet data get func. */
    STATUS (*addrGet) (M_BLK_ID, M_BLK_ID, M_BLK_ID, M_BLK_ID, M_BLK_ID);
                                           /* Driver's packet addr get func. */
    int (*endBind) (void*, void*, void*, long type); 
					/* information exchange between */
					/* network service and network driver */ 
    } NET_FUNCS; 

/*
 * 
 * End Specific IOCTL commands.
 *
 * The following are the end specific IOCTL commands.  This is a place
 * holder for them until we places them into the system ioctl.h file.
 * 
 * EIOC stands for END ioctl, just as SIOC stands for socket ioctl.
 *
 * The original set of END IOCTL commands started with number 260 and continued
 * upwards. The value 260 or higher cannot be accomodated within the LSByte
 * reserved for it by the _IO[RW] macros(sys/ioctl.h).
 *
 * However the numbers, 260 through 274, that were used did not
 * alter the value of the command byte('i') in the IOCTL command and
 * are equivalent to the numbers 4 through 18 used below. This preserves
 * binary compatibility with END drivers that were built using the old defines.
 *
 * This list of IOCTLS with command byte 'i' SHOULD NOT BE EXTENDED BEYOND 18
 *
 * User defined IOCTLS should use the command byte 'e' (for END) and should
 * start at 128. Numbers 0 through 127 are reserved by Wind River Systems.
 */

#include "ioctl.h"

/* Device commands */
#define EIOCSFLAGS _IOW('i', 4, int) /* Set device flag */
#define EIOCGFLAGS _IOR('i', 5, int)	/* Get device flag */
#define EIOCSADDR _IOW('i', 6, char *)	/* Set device address */
#define EIOCGADDR _IOR('i', 7, char *)	/* Get device address */
#define EIOCGFBUF _IOR('i', 8, int)   /* Get min 1st buf for chaining. */
#define EIOCGMWIDTH _IOR('i', 9, int)   /* Get device memory width. */

/* Multicast commands */
#define EIOCMULTIADD _IOW('i', 10, char *)	/* add m'cast addr */
#define EIOCMULTIDEL _IOW('i', 11, char *)	/* del m'cast addr */
#define EIOCMULTIGET _IOWR('i', 12, MULTI_TABLE)/* get m'cast addr */

/* Miscellaneous functions */
#define EIOCSRCVCB _IOW('i', 13, FUNCPTR)	/* Set receive callback */
#define EIOCPOLLSTART _IOW('i', 14, NULL)	/* Start polling. */
#define EIOCPOLLSTOP _IOW('i', 15, NULL)	/* Stop polling. */

/* MibII functions */
#define EIOCGMIB2 _IOR('i', 16, M2_INTERFACETBL *)

/* Name function. */
#define EIOCGNAME _IOR('i', 17, char *)     /* get the device name */

/* Header length fucntion. */
#define EIOCGHDRLEN _IOR('i', 18, int)     /* get the header length */

/*
 * The IOCTL commands below were introduced by the Network Protocol Toolkit
 * The command byte is set to 'e' for END and numbers 1 thro' 127 are reserved
 * for WRS.
 * Below we start at 20 to allow room for the above 'i' based IOCTLS to be
 * converted to 'e' based IOCTLS sometime in the future.
 */

/* Toolkit device or not */
#define EIOCGNPT _IOR('e', 20, NULL) /* return true in case of toolkit device*/

/* Broadcast address */
#define EIOCGBCASTADDR _IOR('e', 21, char *) /* return Broadcast addr */

/* Query IOCTL command */
#define EIOCQUERY _IOWR('e', 22, END_QUERY)

/* More MibII functions */
#define EIOCGMIB2233 _IOR('e', 23, M2_ID **) 
#define EIOCSMIB2233 _IOW('e', 24, IF_SETENTRY *) 
#define EIOCGMCASTLIST _IOR('e', 25, LIST **)

/* MTU for Point to Multipoint */
#define EIOCGMTU _IOR('e', 26, MTU_QUERY *)

/* Access driver's internal 2233 stats counters */
#define EIOCGPOLLCONF _IOR('e', 27, END_IFDRVCONF **)
#define EIOCGPOLLSTATS _IOR('e', 28, END_IFCOUNTERS **)

/* Query system Query commands */

#define END_BIND_QUERY 0x1

/* This structure is a row in our run time END table. 
 * Each row contains all the units started on a particular named device.
 * I.e. all devices that are Lance 7990 Ethernet will be in the list
 * off of the row with the name "ln".
 */

typedef struct end_tbl_row
    {
    NODE node;                  /* Needed by our list processing library. */
    char name[END_NAME_MAX];	/* Name of the devices in row. (ln, etc.) */
    LIST units;                 /* List of devices, i.e. 0, 1, etc..  */
    } END_TBL_ROW;

/* This is the structure that is used by the BSP to build up a table
 * of END devices to be started at boot time.
 */
typedef struct end_tbl_entry
    {
    int unit;                                /* This device's unit # */
    END_OBJ* (*endLoadFunc) (char*, void*);         /* The Load function. */
    char* endLoadString;                     /* The load string. */
    BOOL endLoan;                            /* Do we loan buffers? */
    void* pBSP;                              /* BSP private */
    BOOL processed;                          /* Has this been processed? */
    } END_TBL_ENTRY;
    
#define END_TBL_END NULL

/*
 * Definitions and structures for polled stats counters.
 */

#define END_IFINERRORS_VALID		0x00000001
#define END_IFINDISCARDS_VALID		0x00000002
#define END_IFINUNKNOWNPROTOS_VALID	0x00000004
#define END_IFINOCTETS_VALID		0x00000008
#define END_IFINUCASTPKTS_VALID		0x00000010
#define END_IFINMULTICASTPKTS_VALID	0x00000020
#define END_IFINBROADCASTPKTS_VALID	0x00000040

#define END_IFOUTERRORS_VALID		0x00010000
#define END_IFOUTDISCARDS_VALID		0x00020000
#define END_IFOUTUNKNOWNPROTOS_VALID	0x00040000
#define END_IFOUTOCTETS_VALID		0x00080000
#define END_IFOUTUCASTPKTS_VALID	0x00100000
#define END_IFOUTMULTICASTPKTS_VALID	0x00200000
#define END_IFOUTBROADCASTPKTS_VALID	0x00400000

typedef unsigned long long endCounter;

typedef struct /*endIfCounters*/
    {
    endCounter		ifInErrors;
    endCounter		ifInDiscards;
    endCounter		ifInUnknownProtos;
    endCounter		ifInOctets;
    endCounter		ifInUcastPkts;
    endCounter		ifInMulticastPkts;
    endCounter		ifInBroadcastPkts;

    endCounter		ifOutErrors;
    endCounter		ifOutDiscards;
    endCounter		ifOutOctets;
    endCounter		ifOutUcastPkts;
    endCounter		ifOutMulticastPkts;
    endCounter		ifOutBroadcastPkts;
    } END_IFCOUNTERS;

typedef struct /*endIfDrvConf*/
    {
    int			ifPollInterval;
    UINT32		ifValidCounters;
    void *		ifWatchdog;
    END_OBJ *		ifEndObj;
    void *		ifMuxCookie;
    FUNCPTR		ifPollRtn;
    } END_IFDRVCONF;

#if CPU_FAMILY==I960
#pragma align 0			/* turn off alignment requirement */
#endif	/* CPU_FAMILY==I960 */

#ifdef __cplusplus
}
#endif

#endif /* __INCendh */
