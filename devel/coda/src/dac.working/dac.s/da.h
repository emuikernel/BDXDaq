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
 *
 * Revision History:<
 *      $Log: da.h,v $
 *      Revision 1.19  1998/11/17 19:17:40  abbottd
 *      added some variables/defines for Byte swapping
 *
 *      Revision 1.18  1998/08/21 15:44:13  rwm
 *      Fixed for ET.
 *
 *      Revision 1.17  1998/06/05 04:33:24  rwm
 *      Added ifndef guards around whole file.
 *
 *      Revision 1.16  1998/05/27 13:45:04  heyes
 *      add message Q to ROC, improve EB stability on EBD transition
 *
 *      Revision 1.14  1998/03/09 14:23:55  heyes
 *      add debug level stuff
 *
 *      Revision 1.13  1998/03/03 19:52:58  heyes
 *      fix requeue for vlad
 *
 *      Revision 1.11  1997/06/10 18:58:00  heyes
 *      add DD to ROC
 *
 *      Revision 1.10  1997/06/06 14:59:30  heyes
 *      fooling with linux
 *
 *      Revision 1.9  1997/06/05 14:38:05  heyes
 *      tune for linux
 *
 *      Revision 1.8  1997/06/04 01:01:14  heyes
 *      fix Make
 *
 *      Revision 1.7  1997/05/19 19:39:47  heyes
 *      fixed ER multi restart problem
 *
 *      Revision 1.6  1997/02/28 19:40:12  heyes
 *      roc_mask added to BE
 *
 *      Revision 1.5  1997/02/25 18:49:15  heyes
 *      add error recovery to ROC and EB ./coda_roc -s ghtest -t ROC -n ROC2 -i -r
 *
 *      Revision 1.4  1997/01/16 15:30:38  heyes
 *      Increase speed of EB, inc. changes after Dec run.
 *
 *      Revision 1.3  1996/10/08 17:59:00  heyes
 *      working threaded eb
 *
 *      Revision 1.1.1.1  1996/08/21 19:18:53  heyes
 *      Imported sources
 *
*	  Revision 1.1  1994/05/04  13:23:40  heyes
*	  Initial revision
*
*	  Revision 1.1  94/03/16  07:55:32  07:55:32  heyes (Graham Heyes)
*	  Initial revision
*	  
*	  Revision 1.1  94/03/15  11:52:53  11:52:53  heyes (Graham Heyes)
*	  Initial revision
*	  
*	  Revision 1.1  93/10/27  08:46:27  08:46:27  heyes (Graham Heyes)
*	  Initial revision
*	  
 *	  Revision 1.9  93/04/29  14:32:13  14:32:13  heyes (Graham Heyes)
 *	  fix alpha test problems
 *	  
 *	  Revision 1.8  1993/03/29  14:43:44  heyes
 *	  remove referneces to VxWorks.h
 *
 *	  Revision 1.7  1993/03/24  15:31:55  heyes
 *	  remove roc id from event headers
 *
 *	  Revision 1.6  1993/03/16  14:05:32  heyes
 *	  reinsert for coda 1.2
 *
 *	  Revision 1.5  1992/08/14  15:30:54  heyes
 *	  1.1.2 release- support for VME
 *
 *	  Revision 1.4  1992/07/21  17:52:00  heyes
 *	  run time debug on vxworks
 *
 *	  Revision 1.3  1992/06/26  12:44:20  heyes
 *	  removed uio.h from Vxworks, no longer needed
 *
 *	  Revision 1.2  1992/06/25  19:01:07  heyes
 *	  insert into RCS for first time
 *
 *	  Revision 1.1  1992/06/25  18:29:25  heyes
 *	  Initial revision
 *
 *
 *----------------------------------------------------------------------------*/

#ifndef _CODA_DA_H
#define	_CODA_DA_H

#ifndef FALSE
# define FALSE 0
#endif

#ifndef TRUE
# define TRUE !FALSE
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

#include <tcl.h>
#include <tclInt.h>
#include <tclPort.h>

/* Sergey: our itcl.h from 'itcl2.0' directory */
#include "itcl_v2.h"
/* Sergey: use modified tk.h from 'tklite' directory installed as tk_nox.h */
#include "tk_nox.h"


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

static int dtswap[] = {
  0,2,2,0,1,1,0,0, 3,2,3,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
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

#define TCL_PROC(name) int name (objClass object, Tcl_Interp *interp, int argc, char **argv)

#define TCL_SPROC(name) static int name (objClass object, Tcl_Interp *interp, int argc, char **argv)


/* Linux does not have hrtime */
#ifdef Linux

typedef	long long	hrtime_t; /* or uint64_t ??? */

#endif


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

/* _CODA_DA_H */
#endif
