/* wdbMbufLib.h - header file for WDB I/O buffer handling */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,25apr02,jhw  Added C++ support (SPR 76304).
01c,19sep97,vin modified for cluster Blk logic, removed hardcoded assumption
		of the location of the refcnt.
01b,24jul96,vin upgraded to BSD4.4 mbufs
01a,04may95,ms  written.
*/

#ifndef __INCwdbMbufLibh
#define __INCwdbMbufLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "net/mbuf.h"

/* macros */

#define wdbMbufClusterInit(pMbuf, addr, nBytes, callBackRtn, callBackArg) \
    {									\
    (pMbuf)->m_next       = NULL;					\
    (pMbuf)->m_nextpkt    = NULL;					\
    (pMbuf)->m_data       = (caddr_t)(addr);				\
    (pMbuf)->m_len        = nBytes;					\
    (pMbuf)->m_type       = MT_DATA;					\
    (pMbuf)->m_flags      |= M_EXT;					\
    (pMbuf)->m_extBuf     = (caddr_t)(addr);				\
    (pMbuf)->m_extSize    = nBytes;					\
    (pMbuf)->m_extFreeRtn = callBackRtn;				\
    (pMbuf)->m_extRefCnt = 1;						\
    (pMbuf)->m_extArg1    = callBackArg;				\
    (pMbuf)->m_extArg2    = 0;						\
    (pMbuf)->m_extArg3    = 0;						\
    }

#define wdbMbufChainFree(pMbuf)						\
    {									\
    struct mbuf *__pNextMbuf;						\
    struct mbuf *__pThisMbuf;						\
    __pThisMbuf = pMbuf;						\
    while (__pThisMbuf != NULL)						\
	{								\
	__pNextMbuf = __pThisMbuf->m_next;				\
	wdbMbufFree (__pThisMbuf);					\
	__pThisMbuf = __pNextMbuf;					\
	}								\
    }

#define wdbMbufDataGet(__pMbuf, __pDest, __len, __pLen)			\
    {									\
    int __nBytes;							\
    struct mbuf * __pThisMbuf;						\
									\
    __nBytes = 0;							\
    for (__pThisMbuf = __pMbuf;						\
         __pThisMbuf != NULL;						\
         __pThisMbuf = __pThisMbuf->m_next)				\
        {								\
        int __moreBytes = MIN (__pThisMbuf->m_len, __len - __nBytes);	\
        bcopy (mtod (__pThisMbuf, char *), __pDest + __nBytes, __moreBytes); \
        __nBytes += __moreBytes;					\
        if (__nBytes >= __len)						\
            break;							\
        }								\
									\
    *(__pLen) = __nBytes;						\
    }

/* function prototypes */

#ifdef  __STDC__

extern struct mbuf *	wdbMbufAlloc     (void);
extern void             wdbMbufFree      (struct mbuf * segId);

#else   /* __STDC__ */

extern struct mbuf *	wdbMbufAlloc     ();
extern void             wdbMbufFree      ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif	/* __INCwdbMbufLibh */

