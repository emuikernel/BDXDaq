/*----------------------------------------------------------------------------*
 *  Copyright (c) 1991, 1992  Southeastern Universities Research Association, *
 *                            Continuous Electron Beam Accelerator Facility   *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606 *
 *      heyes@cebaf.gov   Tel: (804) 249-7030    Fax: (804) 249-7363          *
 *----------------------------------------------------------------------------*
 * Discription: follows this header.
 *
 * Author:
 *	Graham Heyes
 *	CEBAF Data Acquisition Group
 *----------------------------------------------------------------------------*/

#ifndef _CODA_DA_H
#define	_CODA_DA_H

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

#define _DADEFINED
#ifdef VXWORKS

#include <vxWorks.h>
#include <types.h>
#include <logLib.h>
#include <memLib.h>
#include <string.h>
#include <semLib.h>
#include <taskLib.h>
#include <socket.h>
#include <sockLib.h>
#include <in.h>
#include <intLib.h>
#include <iosLib.h>
#include <inetLib.h>
#include <hostLib.h>
#include <ioLib.h>
#include <loadLib.h>
#include <errnoLib.h>
#include <sigLib.h>
#include <selectLib.h>
#include <msgQLib.h>
#include <sysSymTbl.h>
#include <symLib.h>
#include <netdb.h>

#else

#include <sys/types.h>
#include <errno.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>

#endif /* ifdef VXWORKS */


#ifndef VXWORKS

#include "et.h"

#ifdef SOLARIS
#include <ucontext.h>
#endif

#endif

#include "obj.h"

#include <setjmp.h>
#ifdef LINUX
#include <stdarg.h>
#endif

#define MAX_FRAG 150		/* event size */
#define MAX_BLOCK 4		/* events per block */
#define TCP_ON 1
#define LOCK_STEP 2

#define EV_SYNC     16
#define EV_PRESTART 17
#define EV_GO       18
#define EV_PAUSE    19
#define EV_END      20

#define EV_BANK_HDR  0x00000100
#define EV_BAD       0x10000000

#define PHYS_BANK_HDR(t,e) (unsigned int)((((t)&0xf)<<16) | \
					     ((e)&0xff) | \
					     EV_BANK_HDR)

#define CTL_BANK_HDR(t) (unsigned int)((((t)&0xffff)<<16) | \
					  0x000001CC)

#define IS_BANK(b) (((unsigned int) (b) && EV_BANK_HDR)==EV_BANK_HDR)

#define DECODE_BANK_HEADER(b,t,e) { t = (b[1]>>16)&0xffff;\
				      e = b[1]&0xff;}


#define EV_BANK_ID 0xc0010100
#define EV_HDR_LEN 4

/* define some things for byte swapping */
#define LSWAP(x)        ((((x) & 0x000000ff) << 24) | \
                         (((x) & 0x0000ff00) <<  8) | \
                         (((x) & 0x00ff0000) >>  8) | \
                         (((x) & 0xff000000) >> 24))

#define SSWAP(x)        ((((x) & 0x00ff) << 8) | \
                         (((x) & 0xff00) >> 8))


#define DT_BANK    0x10
#define DT_LONG    0x01
#define DT_SHORT   0x05
#define DT_BYTE    0x07
#define DT_CHAR    0x03

/* dtswap[1] corresponds type 1, dtswap[2] - type 2 etc; 0 means 'no swap', 1-swap 16bit, 2-swap 32bit, 3-swap 64bit, 4 - composite swap, 5 - bank of banks */
/* EVIO types:
      1    'i'   unsigned int
      2    'F'   floating point
      3    'a'   8-bit char (C++)
      4    'S'   short
      5    's'   unsigned short
      6    'C'   char
      7    'c'   unsigned char
      8    'D'   double (64-bit float)
      9    'L'   long long (64-bit int)
     10    'l'   unsigned long long (64-bit int)
     11    'I'   int
*/
static int dtswap[] = {
  0,2,2,0,1,1,0,0, 3,2,3,0,0,0,5,4, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 };



extern jmp_buf global_env[8][32];
extern int use_recover;

extern int global_env_depth[32];
extern char *global_routine[8][32];

#ifdef HP_UX
#define SIGVECTOR sigvector
#else
#define SIGVECTOR sigvec
#endif

#ifdef VXWORKS
#define pthread_self() 1
#endif

#define recoverContext(name,res) \
{ \
  /*if(use_recover) \
  { \
	int thr = ((unsigned int) pthread_self()) & 31; \
	global_routine[global_env_depth[thr]][thr] = name; \
	res = setjmp(global_env[global_env_depth[thr]][thr]); \
	global_env_depth[thr]++; \
  } \
  else*/ \
  { \
	res = 0; \
  } \
}

#define DATA_DEBUG 1
#define API_DEBUG  2



/* defines for listSplit1() */
#define LISTARGV1 40
#define LISTARGV2 256


/* observed coda_er error:
.................................
CODAtcpServer: start work thread
befor: socket=8 address>129.57.71.15< port=40591
wait: coda request in progress
XXX: msgLen=218759184 nRead=2
wait: coda request in progress
wait: coda request in progress
wait: coda request in progress
wait: coda request in progress
wait: coda request in progress
wait: coda request in progress
Error(new rc): nRead=0, must be 218759185
CODAtcpServer: start work thread
befor: socket=8 address>129.57.71.15< port=40608
wait: coda request in progress
XXX: msgLen=1392574464 nRead=4
Error(new rc): nRead=80, must be 1392574465
CODAtcpServer: start work thread
befor: socket=8 address>129.57.71.15< port=40612
wait: coda request in progress
XXX: msgLen=-11317950 nRead=4
Error(new rc): nRead=-1, must be -11317949
CODAtcpServer: start work thread
befor: socket=8 address>129.57.71.15< port=40614
wait: coda request in progress
XXX: msgLen=0 nRead=4
MESSAGE (nRead=1, Address>129.57.71.15<, port=40614): Executing ><
Executing ><
Segmentation fault
 */

/* Linux and Darwin returns microsecs, Solaris returns nanosecs */
#ifdef Linux
#define NANOMICRO 1
#else
#ifdef Darwin
#define NANOMICRO 1
#else
#define NANOMICRO 1000
#endif
#endif




/* Linux does not have hrtime */
#ifdef Linux

#ifdef  __cplusplus
extern "C" {
#endif

typedef	long long	hrtime_t; /* or uint64_t ??? */
hrtime_t/*uint64_t*/ gethrtime(void);

#ifdef  __cplusplus
}
#endif

#endif


/* _CODA_DA_H */
#endif
