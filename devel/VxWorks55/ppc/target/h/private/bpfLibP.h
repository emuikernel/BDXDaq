/* bpfLibP.h - Berkeley Packet Filter (BPF) private include file */

/* Copyright 1999 - 2000 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*-
 * Copyright (c) 1990, 1991, 1992, 1994, 1995, 1996
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from the Stanford/CMU enet packet filter,
 * (net/enet.c) distributed as part of 4.3BSD, and code contributed
 * to Berkeley by Steven McCanne and Van Jacobson both of Lawrence 
 * Berkeley Laboratory.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)bpfdesc.h	7.1 (Berkeley) 5/7/91
 *
 * @(#) $Header: bpfdesc.h,v 1.14 96/06/16 22:28:07 leres Exp $ (LBL)
 */

/*
modification history
--------------------
01b,17nov00,spm  added support for BSD network devices
01a,24nov99,spm  created from BSD bpfdesc.h,v 1.14 96/06/16 22:28:07
*/

#ifndef __INCbpfLibPh
#define __INCbpfLibPh

#ifdef __cplusplus
extern "C" {
#endif


/* includes */

#include "vxWorks.h"
#include "lstLib.h"
#include "iosLib.h"
#include "selectLib.h"
#include "endLib.h"

#include "net/bpf.h"

/* typedefs */

/* Descriptor associated with each attached network interface. */

typedef struct bpf_proto_ctrl {      /* BPF_PROTO_CTRL */
    NODE  bpfProtoLstNode;               /* list of interface attachments */
    BOOL  bsdFlag;                       /* BSD or END network device? */
    void  * pCookie;                     /* identifies net device attachment */
    LIST  bpfUnitList;                   /* list of attached BPF units */
    u_int numShared;                     /* number of shared BPF devices */
    LIST  bpfDevList;                    /* shared BPF device attachments */
    u_int numPromisc;                    /* number of promiscuous listeners */
    BOOL  bpfPromiscEnabled;             /* TRUE if BPF turned on promisc */
    u_int dataLinkType;                  /* data link layer type */
    int   mtuSize;                       /* Max device frame size */
    char  devName [END_NAME_MAX + 1];    /* name of network interface */
    int   devUnit;                       /* unit number of network interface */
} BPF_PROTO_CTRL;

/* Descriptor associated with each open bpf file. */

typedef struct bpf_dev_ctrl {      /* BPF_DEV_CTRL */
    NODE bpfDevLstNode; /* node in list of devices attached to an interface */
    UINT32 bpfDevId;       /* unique identifier for all units of BPF device */
    BOOL selectFlag; 	/* tasks pending on select for device? */

    BPF_PROTO_CTRL * pNetIfCtrl;  /* used for first attachment of BPF device */
    BPF_PROTO_CTRL * pNetDev;     /* attachment for this unit, if any */

    SEM_ID 	lockSem; 	  /* provides mutual exclusion */
    /*
     * Buffer slots: two character buffers buffer the incoming packets.
     *   The model has three slots. recvStoreBuf is always occupied.
     *   recvStoreBuf (store) - Receive interrupt puts packets here.
     *   readHoldBuf (hold) - When recvStoreBuf is full, put cluster here
     *   and wakeup read (replace recvStoreBuf with freeBuf).
     *   freeBuf (free) - When read is done, put cluster here.
     * On receiving, if recvStoreBuf is full and freeBuf is NULL, 
     * packet is dropped.
     */

    caddr_t  recvStoreBuf;	/* store slot */
    caddr_t  readHoldBuf;       /* hold slot */
    caddr_t  freeBuf;	        /* free slot */
    int      recvStoreLen;	/* current length of store buffer */
    int      readHoldLen;	/* current length of hold buffer */
    u_int    bufSize;	        /* absolute length of buffers */

    u_long    readTimeout;      /* Read timeout in 'ticks' */
    BPF_INSN  * pBpfInsnFilter; /* filter code */
    int       filterLen; 	/* number of instructions in filter */
    u_long    pktRecvCount;	/* number of packets received */
    u_long    pktDropCount;	/* number of packets dropped */
    BOOL      promiscMode;	/* true if listening promiscuously */
    BOOL      immediateMode;	/* true to return on packet arrival */
    BOOL      nonBlockMode;     /* true if read is non-blocking */
    SEM_ID    readWkupSem;      /* pended read sem. */
    int	      usrFlags;         /* file open flags */
    BOOL      inUse;            /* TRUE if being used by a file desc */
    SEL_WAKEUP_LIST selWkupList;/* select wakeup list */
} BPF_DEV_CTRL;

typedef struct {        /* BPF_DEV_HDR */
    DEV_HDR      devHdr;        /* BPF device hdr */
    BPF_DEV_CTRL * pBpfDevTbl;  /* Table of BPF device entries */
    BPF_PROTO_CTRL * pBpfNetIfTbl;    /* Attachments to network interfaces */
    int          numDevs;       /* number of BPF devices */
    int          bufSize;       /* default BPF device buffer size */
    SEM_ID 	 lockSem; 	/* provides mutual exclusion */
    } BPF_DEV_HDR;

/* defines */

#define BPF_LOCK_KEY   1   /* Key to release BPF lock */
#define BPF_DUMMY_KEY  0   /* dummy key will not release lock */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

IMPORT int  _bpfLock ();
IMPORT BOOL _bpfUnlock (int lockKey); 
IMPORT void _bpfPacketTap (LIST * pListBpfDev, long type, M_BLK_ID pMBlk, 
			   int netDataOffset);
IMPORT STATUS _bpfDevDetach (BPF_DEV_CTRL * pBpfDev);
IMPORT void _bpfProtoInit (void);
IMPORT STATUS _bpfProtoAttach (BPF_DEV_CTRL *, char *, int, struct ifnet *);
IMPORT STATUS _bpfProtoDetach (BPF_DEV_CTRL * pBpfDev);
IMPORT STATUS _bpfProtoPromisc (BPF_PROTO_CTRL * pBpfProto, BOOL on); 
IMPORT void _bpfTimeStamp (struct timeval * pTimeval);
IMPORT STATUS _bpfProtoSend (BPF_PROTO_CTRL * pBpfProto, char * pBuf, 
			     int nBytes, BOOL nbIo);
#else   /* __STDC__ */

IMPORT int  _bpfLock ();
IMPORT BOOL _bpfUnlock (); 
IMPORT STATUS _bpfDevDetach ();
IMPORT void _bpfProtoInit ();
IMPORT BPF_PROTO_CTRL * _bpfProtoAttach ();
IMPORT BPF_PROTO_CTRL * _bpfProtoDetach ();
IMPORT STATUS _bpfProtoPromisc ();
IMPORT void _bpfTimeStamp ();
IMPORT STATUS _bpfProtoSend ();
IMPORT void _bpfPacketTap ();

#endif /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCbpfLibPh */
