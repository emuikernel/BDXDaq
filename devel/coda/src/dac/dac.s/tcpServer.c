
/* tcpServer.c - TCP server by Sergey Boyarinov, last revision May 2011 */ 
/* DESCRIPTION This file contains the server-side of the VxWorks TCP example code. 
   The example code demonstrates the usage of several BSD 4.4-style socket routine calls. */

#ifdef VXWORKS

#include <vxWorks.h>
#include <sockLib.h>
#include <inetLib.h>
#include <taskLib.h>
#include <stdioLib.h>
#include <strLib.h>
#include <ioLib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <sysSymTbl.h>
#include <symLib.h>

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

#ifdef Linux
#include <linux/prctl.h>
#endif

#endif

#include "libtcp.h" 
#include "libdb.h" 

/* readiness flag */
static int request_in_progress;

/* task flag */
static int iTaskTCP;

/* function declarations */ 
extern char *targetName(); /* from roc_component.c */

static void tcpServerWorkTask(TWORK *targ); 
static int TcpServer(void);


#ifdef VXWORKS
/*
extern int execute();
*/
#else
#include <dlfcn.h>
#endif


#ifdef __cplusplus
typedef int 		(*FPTR) (...);     /* ptr to function returning int */
typedef void 		(*VOIDFPTR) (...); /* ptr to function returning void */
#else
typedef int 		(*FPTR) ();	   /* ptr to function returning int */
typedef void 		(*VOIDFPTR) (); /* ptr to function returning void */
#endif			/* _cplusplus */


#ifndef VXWORKS
/* currently processed message */
static char current_message[REQUEST_MSG_SIZE];
static char localname[128];
#endif

/*
the maximum number of parameters is 'NARGS', currently 8, extra parameters will be ignored;
'NARGS' can be bigger for UNIX but on VXWORKS the table in 'FUNCALLVXWORKS' becomes huge;
hopefully 8 will be enough

'my_execute' accepts strings in "" quotes, 0x, floats and ints; in contrast with
vxWorks's 'execute' it treats single quotes '' as string, not as a character;
since it treats '' as string, to send a single character use its digital code,
for example instead of 'A' use 65

examples:

tcpClient tage "test3(0x55,66.336,44)"
(prints: test3: a=85, b=66.335999, c=44)

if want to send a string:

tcpClient tage 'test4(0x55,66.3366666,"sss",44)'
 or
tcpClient tage "test4(0x55,66.3366666,'sss',44)"
(printf: test4: a=85, b=66.336670, c=>sss<, d=44)

NOTE: because of different representation of 'float' in parameters list on UNIX and VXWORKS (or probably Intel and PPC)
ugly macros FUNCALLVXWORKS was introduced for VXWORKS on PPC

*/


#define FUNCALL(A0,A1,A2,A3,A4,A5,A6,A7) \
(*(command_ptr)) (A7##args[0],A6##args[1],A5##args[2],A4##args[3],A3##args[4],A2##args[5],A1##args[6],A0##args[7])

#define FUNCALLVXWORKS \
switch(fmask) \
{ \
  case 0x00: FUNCALL(i,i,i,i,i,i,i,i); break; \
  case 0x01: FUNCALL(i,i,i,i,i,i,i,f); break; \
  case 0x02: FUNCALL(i,i,i,i,i,i,f,i); break; \
  case 0x03: FUNCALL(i,i,i,i,i,i,f,f); break; \
  case 0x04: FUNCALL(i,i,i,i,i,f,i,i); break; \
  case 0x05: FUNCALL(i,i,i,i,i,f,i,f); break; \
  case 0x06: FUNCALL(i,i,i,i,i,f,f,i); break; \
  case 0x07: FUNCALL(i,i,i,i,i,f,f,f); break; \
  case 0x08: FUNCALL(i,i,i,i,f,i,i,i); break; \
  case 0x09: FUNCALL(i,i,i,i,f,i,i,f); break; \
  case 0x0A: FUNCALL(i,i,i,i,f,i,f,i); break; \
  case 0x0B: FUNCALL(i,i,i,i,f,i,f,f); break; \
  case 0x0C: FUNCALL(i,i,i,i,f,f,i,i); break; \
  case 0x0D: FUNCALL(i,i,i,i,f,f,i,f); break; \
  case 0x0E: FUNCALL(i,i,i,i,f,f,f,i); break; \
  case 0x0F: FUNCALL(i,i,i,i,f,f,f,f); break; \
 \
  case 0x10: FUNCALL(i,i,i,f,i,i,i,i); break; \
  case 0x11: FUNCALL(i,i,i,f,i,i,i,f); break; \
  case 0x12: FUNCALL(i,i,i,f,i,i,f,i); break; \
  case 0x13: FUNCALL(i,i,i,f,i,i,f,f); break; \
  case 0x14: FUNCALL(i,i,i,f,i,f,i,i); break; \
  case 0x15: FUNCALL(i,i,i,f,i,f,i,f); break; \
  case 0x16: FUNCALL(i,i,i,f,i,f,f,i); break; \
  case 0x17: FUNCALL(i,i,i,f,i,f,f,f); break; \
  case 0x18: FUNCALL(i,i,i,f,f,i,i,i); break; \
  case 0x19: FUNCALL(i,i,i,f,f,i,i,f); break; \
  case 0x1A: FUNCALL(i,i,i,f,f,i,f,i); break; \
  case 0x1B: FUNCALL(i,i,i,f,f,i,f,f); break; \
  case 0x1C: FUNCALL(i,i,i,f,f,f,i,i); break; \
  case 0x1D: FUNCALL(i,i,i,f,f,f,i,f); break; \
  case 0x1E: FUNCALL(i,i,i,f,f,f,f,i); break; \
  case 0x1F: FUNCALL(i,i,i,f,f,f,f,f); break; \
 \
  case 0x20: FUNCALL(i,i,f,i,i,i,i,i); break; \
  case 0x21: FUNCALL(i,i,f,i,i,i,i,f); break; \
  case 0x22: FUNCALL(i,i,f,i,i,i,f,i); break; \
  case 0x23: FUNCALL(i,i,f,i,i,i,f,f); break; \
  case 0x24: FUNCALL(i,i,f,i,i,f,i,i); break; \
  case 0x25: FUNCALL(i,i,f,i,i,f,i,f); break; \
  case 0x26: FUNCALL(i,i,f,i,i,f,f,i); break; \
  case 0x27: FUNCALL(i,i,f,i,i,f,f,f); break; \
  case 0x28: FUNCALL(i,i,f,i,f,i,i,i); break; \
  case 0x29: FUNCALL(i,i,f,i,f,i,i,f); break; \
  case 0x2A: FUNCALL(i,i,f,i,f,i,f,i); break; \
  case 0x2B: FUNCALL(i,i,f,i,f,i,f,f); break; \
  case 0x2C: FUNCALL(i,i,f,i,f,f,i,i); break; \
  case 0x2D: FUNCALL(i,i,f,i,f,f,i,f); break; \
  case 0x2E: FUNCALL(i,i,f,i,f,f,f,i); break; \
  case 0x2F: FUNCALL(i,i,f,i,f,f,f,f); break; \
 \
  case 0x30: FUNCALL(i,i,f,f,i,i,i,i); break; \
  case 0x31: FUNCALL(i,i,f,f,i,i,i,f); break; \
  case 0x32: FUNCALL(i,i,f,f,i,i,f,i); break; \
  case 0x33: FUNCALL(i,i,f,f,i,i,f,f); break; \
  case 0x34: FUNCALL(i,i,f,f,i,f,i,i); break; \
  case 0x35: FUNCALL(i,i,f,f,i,f,i,f); break; \
  case 0x36: FUNCALL(i,i,f,f,i,f,f,i); break; \
  case 0x37: FUNCALL(i,i,f,f,i,f,f,f); break; \
  case 0x38: FUNCALL(i,i,f,f,f,i,i,i); break; \
  case 0x39: FUNCALL(i,i,f,f,f,i,i,f); break; \
  case 0x3A: FUNCALL(i,i,f,f,f,i,f,i); break; \
  case 0x3B: FUNCALL(i,i,f,f,f,i,f,f); break; \
  case 0x3C: FUNCALL(i,i,f,f,f,f,i,i); break; \
  case 0x3D: FUNCALL(i,i,f,f,f,f,i,f); break; \
  case 0x3E: FUNCALL(i,i,f,f,f,f,f,i); break; \
  case 0x3F: FUNCALL(i,i,f,f,f,f,f,f); break; \
 \
  case 0x40: FUNCALL(i,f,i,i,i,i,i,i); break; \
  case 0x41: FUNCALL(i,f,i,i,i,i,i,f); break; \
  case 0x42: FUNCALL(i,f,i,i,i,i,f,i); break; \
  case 0x43: FUNCALL(i,f,i,i,i,i,f,f); break; \
  case 0x44: FUNCALL(i,f,i,i,i,f,i,i); break; \
  case 0x45: FUNCALL(i,f,i,i,i,f,i,f); break; \
  case 0x46: FUNCALL(i,f,i,i,i,f,f,i); break; \
  case 0x47: FUNCALL(i,f,i,i,i,f,f,f); break; \
  case 0x48: FUNCALL(i,f,i,i,f,i,i,i); break; \
  case 0x49: FUNCALL(i,f,i,i,f,i,i,f); break; \
  case 0x4A: FUNCALL(i,f,i,i,f,i,f,i); break; \
  case 0x4B: FUNCALL(i,f,i,i,f,i,f,f); break; \
  case 0x4C: FUNCALL(i,f,i,i,f,f,i,i); break; \
  case 0x4D: FUNCALL(i,f,i,i,f,f,i,f); break; \
  case 0x4E: FUNCALL(i,f,i,i,f,f,f,i); break; \
  case 0x4F: FUNCALL(i,f,i,i,f,f,f,f); break; \
 \
  case 0x50: FUNCALL(i,f,i,f,i,i,i,i); break; \
  case 0x51: FUNCALL(i,f,i,f,i,i,i,f); break; \
  case 0x52: FUNCALL(i,f,i,f,i,i,f,i); break; \
  case 0x53: FUNCALL(i,f,i,f,i,i,f,f); break; \
  case 0x54: FUNCALL(i,f,i,f,i,f,i,i); break; \
  case 0x55: FUNCALL(i,f,i,f,i,f,i,f); break; \
  case 0x56: FUNCALL(i,f,i,f,i,f,f,i); break; \
  case 0x57: FUNCALL(i,f,i,f,i,f,f,f); break; \
  case 0x58: FUNCALL(i,f,i,f,f,i,i,i); break; \
  case 0x59: FUNCALL(i,f,i,f,f,i,i,f); break; \
  case 0x5A: FUNCALL(i,f,i,f,f,i,f,i); break; \
  case 0x5B: FUNCALL(i,f,i,f,f,i,f,f); break; \
  case 0x5C: FUNCALL(i,f,i,f,f,f,i,i); break; \
  case 0x5D: FUNCALL(i,f,i,f,f,f,i,f); break; \
  case 0x5E: FUNCALL(i,f,i,f,f,f,f,i); break; \
  case 0x5F: FUNCALL(i,f,i,f,f,f,f,f); break; \
 \
  case 0x60: FUNCALL(i,f,f,i,i,i,i,i); break; \
  case 0x61: FUNCALL(i,f,f,i,i,i,i,f); break; \
  case 0x62: FUNCALL(i,f,f,i,i,i,f,i); break; \
  case 0x63: FUNCALL(i,f,f,i,i,i,f,f); break; \
  case 0x64: FUNCALL(i,f,f,i,i,f,i,i); break; \
  case 0x65: FUNCALL(i,f,f,i,i,f,i,f); break; \
  case 0x66: FUNCALL(i,f,f,i,i,f,f,i); break; \
  case 0x67: FUNCALL(i,f,f,i,i,f,f,f); break; \
  case 0x68: FUNCALL(i,f,f,i,f,i,i,i); break; \
  case 0x69: FUNCALL(i,f,f,i,f,i,i,f); break; \
  case 0x6A: FUNCALL(i,f,f,i,f,i,f,i); break; \
  case 0x6B: FUNCALL(i,f,f,i,f,i,f,f); break; \
  case 0x6C: FUNCALL(i,f,f,i,f,f,i,i); break; \
  case 0x6D: FUNCALL(i,f,f,i,f,f,i,f); break; \
  case 0x6E: FUNCALL(i,f,f,i,f,f,f,i); break; \
  case 0x6F: FUNCALL(i,f,f,i,f,f,f,f); break; \
 \
  case 0x70: FUNCALL(i,f,f,f,i,i,i,i); break; \
  case 0x71: FUNCALL(i,f,f,f,i,i,i,f); break; \
  case 0x72: FUNCALL(i,f,f,f,i,i,f,i); break; \
  case 0x73: FUNCALL(i,f,f,f,i,i,f,f); break; \
  case 0x74: FUNCALL(i,f,f,f,i,f,i,i); break; \
  case 0x75: FUNCALL(i,f,f,f,i,f,i,f); break; \
  case 0x76: FUNCALL(i,f,f,f,i,f,f,i); break; \
  case 0x77: FUNCALL(i,f,f,f,i,f,f,f); break; \
  case 0x78: FUNCALL(i,f,f,f,f,i,i,i); break; \
  case 0x79: FUNCALL(i,f,f,f,f,i,i,f); break; \
  case 0x7A: FUNCALL(i,f,f,f,f,i,f,i); break; \
  case 0x7B: FUNCALL(i,f,f,f,f,i,f,f); break; \
  case 0x7C: FUNCALL(i,f,f,f,f,f,i,i); break; \
  case 0x7D: FUNCALL(i,f,f,f,f,f,i,f); break; \
  case 0x7E: FUNCALL(i,f,f,f,f,f,f,i); break; \
  case 0x7F: FUNCALL(i,f,f,f,f,f,f,f); break; \
 \
  case 0x80: FUNCALL(f,i,i,i,i,i,i,i); break; \
  case 0x81: FUNCALL(f,i,i,i,i,i,i,f); break; \
  case 0x82: FUNCALL(f,i,i,i,i,i,f,i); break; \
  case 0x83: FUNCALL(f,i,i,i,i,i,f,f); break; \
  case 0x84: FUNCALL(f,i,i,i,i,f,i,i); break; \
  case 0x85: FUNCALL(f,i,i,i,i,f,i,f); break; \
  case 0x86: FUNCALL(f,i,i,i,i,f,f,i); break; \
  case 0x87: FUNCALL(f,i,i,i,i,f,f,f); break; \
  case 0x88: FUNCALL(f,i,i,i,f,i,i,i); break; \
  case 0x89: FUNCALL(f,i,i,i,f,i,i,f); break; \
  case 0x8A: FUNCALL(f,i,i,i,f,i,f,i); break; \
  case 0x8B: FUNCALL(f,i,i,i,f,i,f,f); break; \
  case 0x8C: FUNCALL(f,i,i,i,f,f,i,i); break; \
  case 0x8D: FUNCALL(f,i,i,i,f,f,i,f); break; \
  case 0x8E: FUNCALL(f,i,i,i,f,f,f,i); break; \
  case 0x8F: FUNCALL(f,i,i,i,f,f,f,f); break; \
 \
  case 0x90: FUNCALL(f,i,i,f,i,i,i,i); break; \
  case 0x91: FUNCALL(f,i,i,f,i,i,i,f); break; \
  case 0x92: FUNCALL(f,i,i,f,i,i,f,i); break; \
  case 0x93: FUNCALL(f,i,i,f,i,i,f,f); break; \
  case 0x94: FUNCALL(f,i,i,f,i,f,i,i); break; \
  case 0x95: FUNCALL(f,i,i,f,i,f,i,f); break; \
  case 0x96: FUNCALL(f,i,i,f,i,f,f,i); break; \
  case 0x97: FUNCALL(f,i,i,f,i,f,f,f); break; \
  case 0x98: FUNCALL(f,i,i,f,f,i,i,i); break; \
  case 0x99: FUNCALL(f,i,i,f,f,i,i,f); break; \
  case 0x9A: FUNCALL(f,i,i,f,f,i,f,i); break; \
  case 0x9B: FUNCALL(f,i,i,f,f,i,f,f); break; \
  case 0x9C: FUNCALL(f,i,i,f,f,f,i,i); break; \
  case 0x9D: FUNCALL(f,i,i,f,f,f,i,f); break; \
  case 0x9E: FUNCALL(f,i,i,f,f,f,f,i); break; \
  case 0x9F: FUNCALL(f,i,i,f,f,f,f,f); break; \
 \
  case 0xA0: FUNCALL(f,i,f,i,i,i,i,i); break; \
  case 0xA1: FUNCALL(f,i,f,i,i,i,i,f); break; \
  case 0xA2: FUNCALL(f,i,f,i,i,i,f,i); break; \
  case 0xA3: FUNCALL(f,i,f,i,i,i,f,f); break; \
  case 0xA4: FUNCALL(f,i,f,i,i,f,i,i); break; \
  case 0xA5: FUNCALL(f,i,f,i,i,f,i,f); break; \
  case 0xA6: FUNCALL(f,i,f,i,i,f,f,i); break; \
  case 0xA7: FUNCALL(f,i,f,i,i,f,f,f); break; \
  case 0xA8: FUNCALL(f,i,f,i,f,i,i,i); break; \
  case 0xA9: FUNCALL(f,i,f,i,f,i,i,f); break; \
  case 0xAA: FUNCALL(f,i,f,i,f,i,f,i); break; \
  case 0xAB: FUNCALL(f,i,f,i,f,i,f,f); break; \
  case 0xAC: FUNCALL(f,i,f,i,f,f,i,i); break; \
  case 0xAD: FUNCALL(f,i,f,i,f,f,i,f); break; \
  case 0xAE: FUNCALL(f,i,f,i,f,f,f,i); break; \
  case 0xAF: FUNCALL(f,i,f,i,f,f,f,f); break; \
 \
  case 0xB0: FUNCALL(f,i,f,f,i,i,i,i); break; \
  case 0xB1: FUNCALL(f,i,f,f,i,i,i,f); break; \
  case 0xB2: FUNCALL(f,i,f,f,i,i,f,i); break; \
  case 0xB3: FUNCALL(f,i,f,f,i,i,f,f); break; \
  case 0xB4: FUNCALL(f,i,f,f,i,f,i,i); break; \
  case 0xB5: FUNCALL(f,i,f,f,i,f,i,f); break; \
  case 0xB6: FUNCALL(f,i,f,f,i,f,f,i); break; \
  case 0xB7: FUNCALL(f,i,f,f,i,f,f,f); break; \
  case 0xB8: FUNCALL(f,i,f,f,f,i,i,i); break; \
  case 0xB9: FUNCALL(f,i,f,f,f,i,i,f); break; \
  case 0xBA: FUNCALL(f,i,f,f,f,i,f,i); break; \
  case 0xBB: FUNCALL(f,i,f,f,f,i,f,f); break; \
  case 0xBC: FUNCALL(f,i,f,f,f,f,i,i); break; \
  case 0xBD: FUNCALL(f,i,f,f,f,f,i,f); break; \
  case 0xBE: FUNCALL(f,i,f,f,f,f,f,i); break; \
  case 0xBF: FUNCALL(f,i,f,f,f,f,f,f); break; \
 \
  case 0xC0: FUNCALL(f,f,i,i,i,i,i,i); break; \
  case 0xC1: FUNCALL(f,f,i,i,i,i,i,f); break; \
  case 0xC2: FUNCALL(f,f,i,i,i,i,f,i); break; \
  case 0xC3: FUNCALL(f,f,i,i,i,i,f,f); break; \
  case 0xC4: FUNCALL(f,f,i,i,i,f,i,i); break; \
  case 0xC5: FUNCALL(f,f,i,i,i,f,i,f); break; \
  case 0xC6: FUNCALL(f,f,i,i,i,f,f,i); break; \
  case 0xC7: FUNCALL(f,f,i,i,i,f,f,f); break; \
  case 0xC8: FUNCALL(f,f,i,i,f,i,i,i); break; \
  case 0xC9: FUNCALL(f,f,i,i,f,i,i,f); break; \
  case 0xCA: FUNCALL(f,f,i,i,f,i,f,i); break; \
  case 0xCB: FUNCALL(f,f,i,i,f,i,f,f); break; \
  case 0xCC: FUNCALL(f,f,i,i,f,f,i,i); break; \
  case 0xCD: FUNCALL(f,f,i,i,f,f,i,f); break; \
  case 0xCE: FUNCALL(f,f,i,i,f,f,f,i); break; \
  case 0xCF: FUNCALL(f,f,i,i,f,f,f,f); break; \
 \
  case 0xD0: FUNCALL(f,f,i,f,i,i,i,i); break; \
  case 0xD1: FUNCALL(f,f,i,f,i,i,i,f); break; \
  case 0xD2: FUNCALL(f,f,i,f,i,i,f,i); break; \
  case 0xD3: FUNCALL(f,f,i,f,i,i,f,f); break; \
  case 0xD4: FUNCALL(f,f,i,f,i,f,i,i); break; \
  case 0xD5: FUNCALL(f,f,i,f,i,f,i,f); break; \
  case 0xD6: FUNCALL(f,f,i,f,i,f,f,i); break; \
  case 0xD7: FUNCALL(f,f,i,f,i,f,f,f); break; \
  case 0xD8: FUNCALL(f,f,i,f,f,i,i,i); break; \
  case 0xD9: FUNCALL(f,f,i,f,f,i,i,f); break; \
  case 0xDA: FUNCALL(f,f,i,f,f,i,f,i); break; \
  case 0xDB: FUNCALL(f,f,i,f,f,i,f,f); break; \
  case 0xDC: FUNCALL(f,f,i,f,f,f,i,i); break; \
  case 0xDD: FUNCALL(f,f,i,f,f,f,i,f); break; \
  case 0xDE: FUNCALL(f,f,i,f,f,f,f,i); break; \
  case 0xDF: FUNCALL(f,f,i,f,f,f,f,f); break; \
 \
  case 0xE0: FUNCALL(f,f,f,i,i,i,i,i); break; \
  case 0xE1: FUNCALL(f,f,f,i,i,i,i,f); break; \
  case 0xE2: FUNCALL(f,f,f,i,i,i,f,i); break; \
  case 0xE3: FUNCALL(f,f,f,i,i,i,f,f); break; \
  case 0xE4: FUNCALL(f,f,f,i,i,f,i,i); break; \
  case 0xE5: FUNCALL(f,f,f,i,i,f,i,f); break; \
  case 0xE6: FUNCALL(f,f,f,i,i,f,f,i); break; \
  case 0xE7: FUNCALL(f,f,f,i,i,f,f,f); break; \
  case 0xE8: FUNCALL(f,f,f,i,f,i,i,i); break; \
  case 0xE9: FUNCALL(f,f,f,i,f,i,i,f); break; \
  case 0xEA: FUNCALL(f,f,f,i,f,i,f,i); break; \
  case 0xEB: FUNCALL(f,f,f,i,f,i,f,f); break; \
  case 0xEC: FUNCALL(f,f,f,i,f,f,i,i); break; \
  case 0xED: FUNCALL(f,f,f,i,f,f,i,f); break; \
  case 0xEE: FUNCALL(f,f,f,i,f,f,f,i); break; \
  case 0xEF: FUNCALL(f,f,f,i,f,f,f,f); break; \
 \
  case 0xF0: FUNCALL(f,f,f,f,i,i,i,i); break; \
  case 0xF1: FUNCALL(f,f,f,f,i,i,i,f); break; \
  case 0xF2: FUNCALL(f,f,f,f,i,i,f,i); break; \
  case 0xF3: FUNCALL(f,f,f,f,i,i,f,f); break; \
  case 0xF4: FUNCALL(f,f,f,f,i,f,i,i); break; \
  case 0xF5: FUNCALL(f,f,f,f,i,f,i,f); break; \
  case 0xF6: FUNCALL(f,f,f,f,i,f,f,i); break; \
  case 0xF7: FUNCALL(f,f,f,f,i,f,f,f); break; \
  case 0xF8: FUNCALL(f,f,f,f,f,i,i,i); break; \
  case 0xF9: FUNCALL(f,f,f,f,f,i,i,f); break; \
  case 0xFA: FUNCALL(f,f,f,f,f,i,f,i); break; \
  case 0xFB: FUNCALL(f,f,f,f,f,i,f,f); break; \
  case 0xFC: FUNCALL(f,f,f,f,f,f,i,i); break; \
  case 0xFD: FUNCALL(f,f,f,f,f,f,i,f); break; \
  case 0xFE: FUNCALL(f,f,f,f,f,f,f,i); break; \
  case 0xFF: FUNCALL(f,f,f,f,f,f,f,f); break; \
 \
  default: FUNCALL(i,i,i,i,i,i,i,i); \
}

#define NARGS 8

static int
my_execute(char *string)
{
  int ii, len, res;
  char *saveptr;
  char *command, *arguments;
  char *args[NARGS], nargs;
  VOIDFPTR command_ptr;
  char str[256];
#ifdef VXWORKS
  char *pValue;
  SYM_TYPE sType;
  int fmask = 0;
  float fargs[NARGS];
  int iargs[NARGS];
  char **sargs = (char **)iargs;
#else
  void *handler;
  float fargs[NARGS];
  int *iargs = (int *)fargs;
  char **sargs = (char **)fargs;
#endif



  /* parsing*/
  strncpy(str,string,255); /*strtok will modify input string, let it be local and non-constant one*/
  /*printf("str >%s<\n",str);*/

  command = strtok_r(str,"(",&saveptr);
  if(command!=NULL)
  {
    /*printf("command >%s<\n",command)*/;
  }
  else
  {
    printf("no command found in >%s<\n",str);
    return(-1);
  }

  arguments = strtok_r(NULL,")",&saveptr);
  if(arguments!=NULL)
  {
    /*printf("arguments >%s<\n",arguments);*/
    args[0] = strtok_r(arguments,",",&saveptr);
    nargs = 1;

    while( (nargs<NARGS) && (args[nargs]=strtok_r(NULL,",",&saveptr)) != NULL ) nargs ++;

    for(ii=0; ii<nargs; ii++)
    {
      if( (strchr(args[ii],'"')!=NULL) || (strchr(args[ii],'\'')!=NULL) ) /*string*/
      {
        sargs[ii] = args[ii];
        while(sargs[ii][0]==' ') sargs[ii] ++; /*remove leading spaces*/
        len = strlen(sargs[ii]);
        while(sargs[ii][len-1]==' ') len--; /*remove trailing spaces*/
        sargs[ii][len] = '\0';
        /*printf("111: sargs[%2d] >%s<\n",ii,sargs[ii]);*/

        sargs[ii] ++; /* remove leading quote */
        len = strlen(sargs[ii]);
        sargs[ii][len-1] = '\0'; /* remove trailing quote */
        /*printf("222: sargs[%2d] >%s<\n",ii,sargs[ii]);*/
	  }
      else if(strchr(args[ii],'.')!=NULL) /*float*/
      {
        sscanf(args[ii],"%f",&fargs[ii]);
        /*printf("flo: args[%2d] >%s< %f\n",ii,args[ii],fargs[ii]);*/
#ifdef VXWORKS
        fmask |= (1<<ii);
#endif
	  }
      else if(strchr(args[ii],'x')!=NULL) /*hex*/
      {
        sscanf(args[ii],"%x",&iargs[ii]);
        /*printf("hex: args[%2d] >%s< %d (0x%x)\n",ii,args[ii],iargs[ii],iargs[ii]);*/
	  }
	  else /*decimal*/
	  {
        sscanf(args[ii],"%i",&iargs[ii]);
        /*printf("dec: args[%2d] >%s< %d\n",ii,args[ii],iargs[ii]);*/
	  }
    }
  }

#ifndef VXWORKS
  /* open symbol table */
  handler = dlopen(NULL, RTLD_NOW | RTLD_GLOBAL);
  if(handler == 0)
  {
	printf("my_execute ERROR: dlopen failed on >%s<\n",dlerror());
    return(-1);
  }
#endif

  /* find symbol */
  command = strtok_r(command," ",&saveptr); /*remove leading and trailing spaces if any*/
  /*printf("command1 >%s<\n",command);*/
  if(command==NULL)
  {
    printf("no command found in >%s<\n",command);    
    return(-1);
  }
#ifdef VXWORKS
  res = symFindByName(sysSymTbl, command, &pValue, &sType);
  if(res == OK)
  {
    /*printf("INFO: >%s()< routine found\n",command)*/;
  }
  else
  {
    printf("ERROR: dlsym returned %d\n",res);
    printf("ERROR: >%s()< routine not found\n",command);
    return(-1);
  }
  command_ptr = (VOIDFPTR) pValue;
#else
  res = dlsym(handler, command);
  if((res != (-1)) && (res != 0))
  {
    /*printf("INFO: >%s()< routine found\n",command)*/;
  }
  else
  {
    printf("ERROR: dlsym returned %d\n",res);
    printf("ERROR: >%s()< routine not found\n",command);
    return(-1);
  }
  command_ptr = (VOIDFPTR) res;
#endif
  /*
  printf("ints-> %d(0x%x) %d(0x%x) %d(0x%x) %d(0x%x)\n",
    iargs[0],iargs[0],iargs[1],iargs[1],iargs[2],iargs[2],iargs[3],iargs[3]);
  printf("floats-> %f %f %f %f\n",fargs[0],fargs[1],fargs[2],fargs[3]);
#ifdef VXWORKS
  printf("fmask=0x%02x\n",fmask);
#endif
  */
  /*printf("my_execute: Executing >%s<\n",command);*/
#ifdef VXWORKS
  FUNCALLVXWORKS;
#else
  FUNCALL(i,i,i,i,i,i,i,i);
#endif

#ifndef VXWORKS
  /* close symbol table */
  if(dlclose((void *)handler) != 0)
  {
    printf("ERROR: failed to unload >%s<\n",command);
    return(-1);
  }
#endif

  return(0);
}


/**************************************************************************** 
* * tcpServer - accept and process requests over a TCP socket 
* * This routine creates a TCP socket, and accepts connections over the socket 
* from clients. Each client connection is handled by spawning a separate 
* task to handle client requests. 
* * This routine may be invoked as follows: 
* -> sp tcpServer
* task spawned: id = 0x3a6f1c, name = t1 
* value = 3829532 = 0x3a6f1c 
* -> MESSAGE FROM CLIENT (Internet Address 150.12.0.10, port 1027): 
* Hello out there 
* * RETURNS: Never, or ERROR if a resources could not be allocated. */ 


#ifdef VXWORKS

int
tcpServer(void)
{
  int iii;

  /* enter infinite loop */
  while(1)
  {
    taskDelay(sysClkRateGet()); /* 1 sec sleep */

    /* check if TCPSERVER task exist; if it does, check it health and kill
     if it is suspended */
    iii = taskIdVerify(iTaskTCP);
    /*printf("taskIdVerify returns %d\n",iii);*/
    if(iii==OK) /* check task status */
    {
      /* if task suspended, delete it */
      iii = taskIsSuspended(iTaskTCP); /* returns TRUE if suspended */
  	  if(iii==TRUE)
      {
        printf("INFO(tcpServer): task is suspended, calling taskDelete(%d)\n",iTaskTCP);
        iii = taskDelete(iTaskTCP);
        printf("taskDelete() returns %d\n",iii);
        iTaskTCP = 0;
        taskDelay(5*sysClkRateGet());
	  }
      else
	  {
        /*printf("INFO(tcpServer): task is Ok\n")*/;
	  }
    }
    else
    {
      iTaskTCP = taskSpawn("TCPSERVER", 251, 0, 100000, TcpServer,
                          0, 0, 0,0,0,0,0,0,0,0);
      printf("taskSpawn(\"TCPSERVER\") returns %d\n",iTaskTCP);
    }
  }
}
#else
int
tcpServer(char *name)
{
  int id;

  strcpy(localname,name);

  pthread_attr_t attr;
  pthread_attr_init(&attr); /* initialize attr with default attributes */
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
  pthread_create(&id, &attr, TcpServer, NULL);
}
#endif

#ifndef VXWORKS
#define TRUE  1
#define FALSE 0
#define OK 0
#define ERROR (-1)
#define STATUS int
#endif


static int
TcpServer(void)
{ 
  struct sockaddr_in serverAddr; 
  struct sockaddr_in clientAddr; 
  int sockAddrSize;              /* size of socket address structure */ 
  int sFd;                       /* socket file descriptor */ 
  int newFd;                     /* socket descriptor from accept */ 
  int ix = 0;                    /* counter for work task names */ 
  int portnum = SERVER_PORT_NUM; /* desired port number; can be changed if that number in use enc */
  char workName[16];             /* name of work task */ 
  int on = TRUE;  /* non-blocking */
  int off = FALSE; /* blocking */
  int status;
  static TWORK targ;
  MYSQL *dbsock = NULL;
  MYSQL_RES *result;
  int numRows;
  char tmp[256], *myname, *hname;
  
  printf("TcpServer reached\n");fflush(stdout);

#ifdef Linux
  prctl(PR_SET_NAME,"tcpServer");
#endif

  /* some cleanup */
  sockAddrSize = sizeof(struct sockaddr_in); 
  memset((char *)&serverAddr, 0, sockAddrSize); 
  memset((char *)&clientAddr, 0, sockAddrSize); 


  /* creates an endpoint for communication and returns a socket file descriptor */
  if((sFd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
  {
    perror("socket"); 
    return(ERROR); 
  } 

  /* set up the local address */ 
  serverAddr.sin_family = AF_INET; 
  serverAddr.sin_port = htons(portnum); 
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* create a TCP-based socket */ 

  /* bind socket to local address */
  while(bind(sFd, (struct sockaddr *)&serverAddr, sockAddrSize) == ERROR)
  {
    printf("TcpServer: trying port %d\n",portnum);
    perror("TcpServer: bind");

    /* try another port (just increment on one) */
    portnum ++;
    if((portnum-SERVER_PORT_NUM) > 50)
    {
      close(sFd); 
      return(ERROR);
    }

    serverAddr.sin_port = htons(portnum);
  }
  printf("TcpServer: bind on port %d\n",portnum);

  /* create queue for client connection requests */ 
  if(listen (sFd, SERVER_MAX_CONNECTIONS) == ERROR)
  {
    perror ("listen"); 
    close (sFd); 
    return (ERROR);
  }

#ifdef VXWORKS
  myname = targetName();
#else
  myname = localname;
#endif
  printf("TcpServer: myname >%s<\n",myname);fflush(stdout);

  /* update daq database 'Ports' table with port number and host name */
  dbsock = dbConnect(getenv("MYSQL_HOST"), "daq");

  /* trying to select our name from 'Ports' table */
  sprintf(tmp,"SELECT Name FROM Ports WHERE Name='%s'",myname);
  if(mysql_query(dbsock, tmp) != 0)
  {
	printf("mysql error (%s)\n",mysql_error(dbsock));
    return(ERROR);
  }

  /* gets results from previous query */
  /* we assume that numRows=0 if our Name does not exist,
     or numRows=1 if it does exist */
  if( !(result = mysql_store_result(dbsock)) )
  {
    printf("ERROR in mysql_store_result (%)\n",mysql_error(dbsock));
    return(ERROR);
  }
  else
  {
    numRows = mysql_num_rows(result);
    mysql_free_result(result);

#ifdef VXWORKS
    hname = myname; /*assume they are the same for vxworks*/
#else
	hname = getenv("HOST");
#endif

    /*printf("nrow=%d\n",numRows);*/
    if(numRows == 0)
    {
      sprintf(tmp,"INSERT INTO Ports (Name,Host,tcpClient_tcp) VALUES ('%s','%s',%d)",
        myname,hname,portnum);
    }
    else if(numRows == 1)
    {
      sprintf(tmp,"UPDATE Ports SET Host='%s',tcpClient_tcp=%d WHERE Name='%s'",hname,portnum,myname);
    }
    else
    {
      printf("ERROR: unknown nrow=%d",numRows);
      return(ERROR);
    }

    if(mysql_query(dbsock, tmp) != 0)
    {
	  printf("ERROR\n");
      return(ERROR);
    }
    else
    {
      printf("Query >%s< succeeded\n",tmp);
    }
  }

  dbDisconnect(dbsock);


  request_in_progress = 0;
  /* accept new connect requests and spawn tasks to process them */ 
  while(1)
  {
    /*printf("inside while loop\n");*/
#ifdef VXWORKS
    /* check for free net buffers */
    usrNetStackSysPoolStatus("tcpServer",0);
    usrNetStackDataPoolStatus("tcpServer",0);
#endif

    /* do not accept new request if current one is not finished yet; too
    many requests may create network buffer shortage */
    if(request_in_progress)
    {
      printf("wait: request in progress\n");
#ifdef VXWORKS
      taskDelay(sysClkRateGet());
#else
      sleep(1);
#endif
      continue;
    }

    /*printf("before accept\n");*/
    if((targ.newFd = accept (sFd, (struct sockaddr *) &clientAddr, &sockAddrSize))
          == ERROR)
    {
      perror ("accept"); 
      close (sFd); 
      return (ERROR); 
    }

    /*printf("accepted request\n");*/
    targ.address = inet_ntoa(clientAddr.sin_addr);
    targ.port = ntohs (clientAddr.sin_port);

    sprintf (workName, "tTcpWork%d", ix++);
	/*
usrNetStackSysPoolStatus("tcpServer",1);
usrNetStackDataPoolStatus("tcpServer",1);
	*/
    request_in_progress = 1;
    /* spawn with floating point flag VX_FP_TASK, just in case if some code needs it */
#ifdef VXWORKS
    if(taskSpawn(workName, SERVER_WORK_PRIORITY, VX_FP_TASK/*0*/, SERVER_STACK_SIZE,
       (FPTR) tcpServerWorkTask, &targ, 0, 0, 0, 0, 0, 0, 0, 0, 0)
          == ERROR)
    {
	  perror("taskSpawn"); 
	  close(newFd);
      request_in_progress = 0;
    }
#else
    /*printf("TcpServer: start work thread\n");*/
	{
      int ret;
	  pthread_t id;
      pthread_attr_t detached_attr;

      pthread_attr_init(&detached_attr);
      pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);
      pthread_attr_setscope(&detached_attr, PTHREAD_SCOPE_SYSTEM);
	  /*
      printf("befor: socket=%d address>%s< port=%d\n",
        targ.newFd, targ.address, targ.port); fflush(stdout);
	  */
      /* block annoying IP address(es) */
      if(!strncmp((int) inet_ntoa (clientAddr.sin_addr),"129.57.71.",10))
	  {
        printf("WARN: ignore request from %s\n",targ.address);
        close(newFd);
        request_in_progress = 0;
	  }
      else
	  {
        ret = pthread_create(&id, &detached_attr, tcpServerWorkTask, &targ);
        if(ret!=0)
        {
          printf("ERROR: pthread_create(CODAtcpServerWorkTask) returned %d\n",
            ret);
          close(targ.newFd);
          request_in_progress = 0;
        }
	  }
	}
#endif
	/*
usrNetStackSysPoolStatus("tcpServer",2);
usrNetStackDataPoolStatus("tcpServer",2);
	*/

    /* sleep 100 msec before processing next request; we do not want to process
    too many requests per minute to avoid network buffers shortage */
#ifdef VXWORKS
    taskDelay(sysClkRateGet()/10);
#endif
  }

} 

/**************************************************************************** 
* * tcpServerWorkTask - process client requests 
* * This routine reads from the server's socket, and processes client 
* requests. If the client requests a reply message, this routine 
* will send a reply to the client. 
* * RETURNS: N/A. */ 

static void
tcpServerWorkTask(TWORK *targ)
	 /*int sFd, char *address, unsigned short port) */
{
  STATUS ret;
  TREQUEST clientRequest;            /* request/message from client */ 
  int nRead;                               /* number of bytes read */ 
  char message[REQUEST_MSG_SIZE];
  int len, oldstdout;
#ifdef Linux
  prctl(PR_SET_NAME,"tcp_server_work");
#endif

#ifdef VXWORKS
  if( (nRead = recv(targ->newFd, (char *) &clientRequest, sizeof (TREQUEST), NULL)) > 0 )
  {
	/*
	printf("MMM 0x%08x 0x%08x\n",nRead,nRead);
    printf ("MESSAGE FROM CLIENT (Internet Address %s, port %d, length %d(%d), replflag %d): Executing %s\n", 
	    targ->address, targ->port, clientRequest.msgLen, nRead, clientRequest.reply, clientRequest.message);
	*/
    if(clientRequest.reply)
    {
    /*
     * Set IO redirection.
     */
      /*printf("redirecting I/O to fd: %d\n",sFd);*/

      ioTaskStdSet(0, STD_ERR, targ->newFd);	/* set std err for execute() */
      ioTaskStdSet(0, STD_OUT, targ->newFd);	/* set std out for execute() */
    }

    /*my_execute() does not print 'value - ..' in the end, but some apps (TOF_laser for example
    need it, see 2 functions in libtcp.c; so will use vxWorks's 'execute', or have to fix my_execute() */
    my_execute(clientRequest.message);   /* try Executing the message */
   
    if(clientRequest.reply)
    {
      fflush(stderr);
      fflush(stdout);
    }

    free(targ->address);                   /* free malloc from inet_ntoa() */ 
	/*
    ret = shutdown(sFd,2);
    if(ret==ERROR) printf("ERROR in shutdown()\n");
	*/
	
    ret = close(targ->newFd);
    if(ret==ERROR) printf("ERROR in close()\n");
	
    request_in_progress = 0;
    return;
  } 
  else if(nRead == 0)
  {
    printf("connection closed, exit thread\n");
  }
  else /* error from recv() */ 
  {
    perror("ERROR (recv)"); 
  }

printf("NEVER HERE ??!!\n");
  close(targ->newFd);    /* close server socket connection */ 
  request_in_progress = 0;
  return;

#else /*UNIX*/

  if( (nRead = recv(targ->newFd, (char *) &clientRequest, sizeof (TREQUEST), 0)) > 0 )
  {
    /* convert integers from network byte order */
    clientRequest.msgLen = ntohl(clientRequest.msgLen);
    clientRequest.reply = ntohl(clientRequest.reply);

	/*
    printf ("MESSAGE (nRead=%d, Address>%s<, port=%d): Executing >%s<\n", 
	    nRead, targ->address, targ->port, clientRequest.message);
	*/
    strcpy(message, clientRequest.message);

    /* store it to be used later for debugging */
    strcpy(current_message, message);

    /* try Executing the message (each component must provide codaExecute() function */
    /*do not print: message may contains bad characters, it will be checked inside codaExecute
           printf("Executing >%s< (len=%d)\n",message,strlen(message));*/


    fflush(stdout);

    oldstdout = dup(STDOUT_FILENO); /*save stdout*/
    dup2(targ->newFd,STDOUT_FILENO); /*redirect stdout*/

	/*close(targ->newFd);*/

	/* check if message makes sence */
    my_execute(message);

	dup2(oldstdout, STDOUT_FILENO); /*restore stdout*/
  }
  else if(nRead == 0)
  {
    printf("connection closed, exit thread\n");
  }
  else
  {
    perror("ERROR (recv)"); 
  }

  /*free(targ->address);-stuck here !!!*/ /* free malloc from inet_ntoa() */ 
  close(targ->newFd);  /* close server socket connection */ 

  request_in_progress = 0;

  /* terminate calling thread */
  pthread_exit(NULL);

#endif
}
