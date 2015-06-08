h39402
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/netincl.h
e
s 00148/00000/00000
d D 1.1 00/08/10 11:09:57 boiarino 1 0
c date and time created 00/08/10 11:09:57 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef _NETINCL_
#ifdef MACII
/* for MACII only */
#pragma segment NETCSTAFF
#endif
#include "ioincl.h"
#include <string.h>
 
#ifdef IBMMVS
#include <netdb.h>
#include "rpc.h"
#include <socket.h>
#endif
#ifdef IBMVM
#include <stdlib.h>
#include <netdb.h>
#include "rpc.h"
#include <socket.h>
#endif
#ifdef MACII
#include <StdLib.h>
#include "XZlist.h"
#include "MACsocket.h"
#include "rpc.h"
#endif
 
#if defined(ALLIANT) || defined(SunOS) || defined(ULTRIX) || defined(ALPHA) \
                     || defined(IRIX) || defined(HPUX) || defined(AIX) \
                     || defined(Linux)
#define UNIX	/* s.b. */
 
#ifdef SunOS /* s.b. */
#define PORTMAP
#ifdef __STDC__
#undef __STDC__
#endif
#endif
 
#include <netdb.h>
#include "rpc/rpc.h"
#include <sys/socket.h>
#include <time.h>
#endif
 
#ifdef APOLLO
#define UNIX	/* s.b. */
#include <netdb.h>
#include "rpc.h"
#include <sys/socket.h>
#endif
 
#ifdef VAXVMS
#include <netdb.h>
#include <socket.h>
#define IamVAX
#include "rpc.h"
#endif
 
#define MAXSERV 10
#ifndef OPENC
extern CLIENT * clntpointer[MAXSERV];
#endif
 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   The following definition is instalation dependent. It defines the
   host on which the FP-server for the local APOLLO ring is beig run */
 
#define  APOSERV   "rc01"
 
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef SunOS
#define  MAXNAMELEN    256
#endif
 
#ifdef VAXVMS
#define  SENDSIZE    NULL  /* NULL default size of TCP output buffer */
#define  RECESIZE    NULL  /* NULL default size of TCP input buffer  */
#else
#define  SENDSIZE   30000
#define  RECESIZE   30000
#endif
#define  PERMNUM  0x20805901       /* Prognum of permanent server */
#define  VERSNUM        1
#define  MAINPORT     1710
#define  OPEN           1
#define  CLOSE          2
#define  READ           3
#define  WRITE          4
#define  REWIND         5
#define  LOCK           6
#define  CLOSE_SERVER  10
#define  CHECK_PORT    15
#define  SUBMIT        20
#define  KEY_DELETE    21
#define  KEY_REWRITE   22
#define  KEY_WRITE     23
#define  KEY_READ      24
#define  KEY_BACKSPACE 25
 
struct data_t                      /* input structure for xdr_raw    */
{ int           nbytes;            /* #bytes transmitted             */
  char         *buffer;            /* pointer to the first byte      */
};
 
struct req_t                      /* input structure for xdr_request */
{ int    stream;                  /* stream_id for I/O               */
  int    nbskip;                  /* #bytes to be skipped            */
  int    bufsize;                 /* #bytes to be read/written       */
};
 
struct write_t                    /* input structure for xdr_write   */
{ int           stream;
  int           ibyte;            /* where to start writing          */
  int           nbwrite;          /* #bytes to be written            */
  char         *buffer;
};
 
struct open_t                     /* input structure for xdr_open    */
{ char         *name;
  char         *options;
  int           ispc;             /* code of space units:1=TRK,2=CYL,
                                     anything else='average blksize' */
  int           nr1;              /* primary space                   */
  int           nr2;              /* secondary space                 */
  int           keyunit;          /* zero for non-keyed access,
                                     positive integer for keyed access
                                     files, must be file specific    */
};
 
struct auth_t                     /* input structure for xdr_auth    */
{ char *User, *Password;
  char *Class, *Time_M, *Time_S;
};
#if defined(VAXVMS) || defined(MACII)
static FILE *log = (FILE *)NULL;

int xdr_open(XDR *xdrsp, struct open_t *openp);
int xdr_auth(XDR *xdrsp, struct auth_t *authp);
int get_auth(char *machine, char *login, char *password);
int xdr_put(XDR *xdrsp, struct write_t *writep);


#endif
#define _NETINCL_
#endif
 
 
E 1
