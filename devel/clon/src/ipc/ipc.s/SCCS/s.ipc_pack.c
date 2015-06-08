h36937
s 00001/00001/00527
d D 1.7 96/12/05 17:53:24 muguira 8 7
c JAM - fix debug problem
e
s 00029/00026/00499
d D 1.6 96/11/15 16:25:49 muguira 7 6
c JAM - fix a bug.
e
s 00013/00017/00512
d D 1.5 96/09/30 17:08:32 muguira 6 5
c JAM - use Elliott's new mt number and names.
e
s 00288/00079/00241
d D 1.4 96/08/29 17:39:20 muguira 5 4
c JAM - add more functions
e
s 00002/00002/00318
d D 1.3 96/07/23 22:18:34 muguira 4 3
c JAM - change the names 
e
s 00003/00003/00317
d D 1.2 96/07/23 22:08:29 muguira 3 1
c JAM - change the names 
e
s 00000/00000/00000
d R 1.2 96/07/23 22:05:34 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/s/ipc_pack.c
c Name history : 1 0 s/ipc_pack.c
e
s 00320/00000/00000
d D 1.1 96/07/23 22:05:33 muguira 1 0
c JAM - add to the lib:
c dbrt_VaPackTable
c ipc_VaPack
e
u
U
f e 0
t
T
I 1
/* ------------------------------------------------------------
 *  Pack various data into smart sockets message buffers.
 *
 *  Contains:
 *  ipc_PackInit 
 *  ipc_PackForget
 *  ipc_PackError
 *  ipc_VaPack
D 5
 *  dbrt_VaPackTable
E 5
I 5
 *  dbrt_VaSendRow
 *  dbrt_VaSendSql
 *  dbrt_VaSendWRSql
E 5
 *
 *  This code currently uses a smartsockets define NUMERIC data
 *  messsage type to transfer data in.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 * JAM - initial version.
I 5
 * JAM - add in dbrt_VaPackCmd
I 6
 * JAM - change to Elliot's message types. I ONLY lookup 
 *       by name. The ipc_init call created the needed structures
 *       in the server.
E 6
E 5
 */
#include <stdarg.h>
#include <stdio.h>
#include <rtworks/ipc.h>
I 5
D 6
#include <clas_ipc_mt.h>
E 6
I 6
/*  include <clas_ipc_mt.h>  */
E 6
E 5

T_IPC_MT mt;

typedef struct ipc_ERR {
  char  *ms, *ts;
  int   number_packed;
  int   num_errors ;
D 5
  int   error_index ;
E 5
I 5
  int   error_index [16] ;
E 5
} ipc_ERR ;

I 7
T_IPC_MT  rpc_mt1=0;
T_IPC_MT  rpc_mt2;
T_IPC_MSG rpc_msg;
T_IPC_MSG rpc_rmsg;
T_IPC_MSG rpc_msgH;

E 7
D 5
static ipc_ERR ie;
E 5
I 5
static ipc_ERR ie ;
E 5
static char *er_msg_table[30] = 
{
D 5
  "pack table name failed",         /* 0 */
  "pack column name failed",        /* 1 */
  "pack msg string failed (sN)",    /* 2 */
  "pack msg integer failed",        /* 3 */
  "pack msg float4 failed",         /* 4 */
  "pack msg float4 failed",         /* 5 */
  "pack: unknown field type"        /* 6 */
E 5
I 5
  "dbrt_VaPackTable: table name failed",         /* 0 */
  "dbrt_VaPackTable: column name failed",        /* 1 */
  "dbrt_VaPackTable: msg string failed (sN)",    /* 2 */
  "dbrt_VaPackTable: msg integer failed",        /* 3 */
  "dbrt_VaPackTable: msg float4 failed",         /* 4 */
  "dbrt_VaPackTable: msg float4 failed",         /* 5 */
  "dbrt_VaPackTable: unknown field type",       /* 6 */
  "dbrt_VaPackTable: message send failed",      /* 7 */
  "dbrt_VaPackTable: send flush failed",        /* 8 */
  "dbrt_VaPackSql: could not pack 1st arg",
  "dbrt_VaPackSql: could not retrieve an integer",   /* 10 */
  "dbrt_VaPackSql: could not pack int arg\n",
  "dbrt_VaPackSql: could not retrieve a string",     /* 12 */
  "dbrt_VaPackSql: could not pack a string",
  "dbrt_VaPackSql: destination could not be set",    /* 14 */
  "dbrt_VaPackSql: send failed",                     /* 15 */
  "dbrt_VaPackSql: flush failed"                     /* 16 */
  "dbrt_VaPackTable: destination could not be set",  
  "dbrt_VaSendWRSql: mt lookup 1 failed",            /* 18 */
  "dbrt_VaSendWRSql: mt lookup 2 failed",            /* 19 */
  "dbrt_VaSendWRSql: could not pack 1st arg",
  "dbrt_VaSendWRSql: could not retrieve an integer",   /* 21 */
  "dbrt_VaSendWRSql: could not pack int arg\n",
  "dbrt_VaSendWRSql: could not retrieve a string",     /* 23 */
  "dbrt_VaSendWRSql: could not pack a string",
  "dbrt_VaSendWRSql: destination could not be set",    /* 25 */
  "dbrt_VaSendWRSql: send failed",                     /* 26 */
  "dbrt_VaSendWRSql: flush failed"                     /* 27 */
 
E 5
} ;
D 5
/* -------------------------------- */
E 5
I 5

static char theDest[64] ;

/* -------------------------------------------------------------- */
void reg_error (int msg_no)
{
  int iz = 0;

  iz = (ie.num_errors & 15);
  ie.error_index[iz] = msg_no;
  ie.num_errors +=1 ;
}
/* -------------------------------------------------------------- */
E 5
void dbrt_answer_error ()
{
  return ;
}
D 5
/* -------------------------------- */
E 5
I 5
/* -------------------------------------------------------------- */
E 5
void error_out (char *s)
{
  printf ("%s\n", s);
  exit (0);
}
D 5
/* -------------------------------- */
E 5
I 5
/* -------------------------------------------------------------- */
void dbrt_SetDest (s)
char *s;
{
  if (s == 0)
    strcpy (theDest, "unknown") ;
  else
    strcpy (theDest, s) ;
}
/* -------------------------------------------------------------- */
E 5
/* 
 * init the packer. 
 * 1. get a smartsockets msgtype
 * 2. get a new smartsockets msg structure
 *    and return it.
 *
 */
D 3
T_IPC_MSG ipc_packinit (msg)
E 3
I 3
T_IPC_MSG ipc_PackInit (msg)
E 3
T_IPC_MSG msg;
{
D 6
  mt = TipcMtLookupByNum (T_MT_NUMERIC_DATA) ;
E 6
I 6
  mt = (T_IPC_MT)TipcMtLookup ("dbr_request");
E 6
  if (mt == 0) {
    printf ("create failed\n");
    exit (0) ;
  }
D 5
  printf ("create the message\n");
E 5
I 5
  /*  printf ("create the message\n"); */
E 5
  
  msg = TipcMsgCreate (mt);
  if (msg == 0) {
    printf ("create failed\n");
    exit (0) ;
  }
I 5

E 5
  return msg ;
}
D 5
/* -------------------------------- */
E 5
I 5
/* ------------------------------------------------------------ */
E 5
D 3
void ipc_packforget (msg)
E 3
I 3
void ipc_PackForget (msg)
E 3
T_IPC_MSG msg;
{
  TipcMsgDestroy (msg);
}
D 5
/* -------------------------------- */
E 5
I 5
/* ------------------------------------------------------------ */
E 5
void clear_error ()
{
I 5
  int j;

  for (j=0; j<16; j++)
    ie.error_index[j] = -1;

E 5
  ie.number_packed = ie.num_errors = 0;
  ie.ms = 0; ie.ts = 0;
D 5
  ie.error_index = -1;
E 5
}
D 5
/* -------------------------------- */
E 5
I 5
/* ------------------------------------------------------------ */
E 5
/* 
 * provide verbose feedback about a
 * packing error.
 *
 * makes use of the static error struct
 * carried in this file.
 *
 */
D 3
void ipc_packerror ()
E 3
I 3
void ipc_PackError ()
E 3
{
D 5
  printf ("------------ Error Messages ---------------\n");
  printf ("col name = %s\nformat string = %s\n", ie.ms, ie.ts);
E 5
I 5
  int j;

  printf ("\n\n------------ Error Messages ---------------\n");
E 5
  printf ("number packed = %d\nnumber of errors = %d\n", 
	  ie.number_packed, ie.num_errors);
D 5
  printf ("error message = %s\n", er_msg_table[ie.error_index]);
E 5
I 5
  for (j=0; j<16; j++)
    if (ie.error_index[j] > 0)
      printf ("error message = %s\n", er_msg_table[ie.error_index[j]]);

E 5
  printf ("-------------------------------------------\n");
}
D 5
/* -------------------------------- */
E 5
I 5
/* --------------------------------------------------------------- */
E 5
/* 
D 5
 * Pack a table - var arguments
E 5
I 5
 * Pack a table row - var arguments
E 5
 * 
 * takes a triple of 
 *  column, dataType, data
 *
 * returns:
D 5
 *  0 = failed - use ipc_packerror
 *  1 = success.
E 5
I 5
 *  0  = failed - use ipc_packerror
 *  >0 = success.
E 5
 */
D 5
int dbrt_VaPackTable (char *t_name, ...)
E 5
I 5
int dbrt_VaSendRow (char *t_name, ...)
E 5
{
  int       r;
  char      *tab_name, *ms, *ts, *sN ;
  int       i4;
  float     r4;
  double    r8;
  T_IPC_MSG msg;
  T_OPTION  option;

  va_list arg;
  va_start(arg, t_name);

  clear_error ();

D 4
  msg = ipc_packinit (msg) ;
E 4
I 4
  msg = ipc_PackInit (msg) ;
E 4

  if (t_name == 0)
    return 0;
  if (!TipcMsgAppendStr (msg, t_name)) {
D 5
    ie.error_index = 0;
    ie.num_errors += 1;
E 5
I 5
    reg_error (0) ;
E 5
    return 0 ;
  }
  else
    ie.number_packed += 1;

  while (1) {
    ie.ms = va_arg (arg, char *) ;
    if (ie.ms == 0)
      break;
    if (!TipcMsgAppendStr (msg, ie.ms)) {
D 5
      ie.error_index = 1;
      ie.num_errors += 1 ;
E 5
I 5
      reg_error (1) ;
E 5
      break;
    } else
      ie.number_packed += 1;

    ie.ts = va_arg (arg, char *);
    if ((strcmp (ie.ts, "str")) == 0) {
      sN = va_arg (arg, char *);
      if (!TipcMsgAppendStr (msg, sN)) {
D 5
	ie.error_index = 2;
	ie.num_errors += 1;
E 5
I 5
	reg_error (2) ;
E 5
	break ;  
      }
      else
	ie.number_packed += 1;
    }
    else
      if ((strcmp (ie.ts, "int4")) == 0) {
	i4 = va_arg (arg, int);
	if (!TipcMsgAppendInt4 (msg, i4)) {
D 5
	  ie.error_index = 3;
	  ie.num_errors += 1;
E 5
I 5
	  reg_error (3) ;
E 5
	  break; 
	}
	else
	  ie.number_packed += 1;
      }
      else
	if ((strcmp (ie.ts, "flt4")) == 0) {
	  r4 = (float)va_arg (arg, double);
	  if (!TipcMsgAppendReal4 (msg, r4)) { 
D 5
	    ie.error_index = 4;
	    ie.num_errors += 1;
E 5
I 5
	    reg_error (4) ;
E 5
	    break;
	  }
	  else
	    ie.number_packed += 1;
	}
	else
	  if ((strcmp (ie.ts, "flt8")) == 0) {
	    r8 = va_arg (arg, double);
	    if (!TipcMsgAppendReal8 (msg, r8)) {
D 5
	      ie.error_index = 5;
	      ie.num_errors += 1;
E 5
I 5
	      reg_error (5) ;
E 5
	      break;
	    } 
	    else
	      ie.number_packed += 1;
	  }
	  else {
D 5
	    ie.error_index = 6;
	    ie.num_errors += 1;
E 5
I 5
	    reg_error (6) ;
E 5
	    break;
	  }
  }
  va_end (arg);
D 5
  
  if (!TipcMsgSetDest (msg, "james"))
    error_out ("destination could not be set\n");
  
  if (!TipcSrvMsgSend (msg, TRUE))
E 5
I 5

  if (ie.num_errors > 0) {
    ipc_PackForget (msg);
E 5
    return 0;
D 5
  
  if (!TipcSrvFlush ()) 
E 5
I 5
  }
D 8
  if (!TipcMsgSetDest (msg, "james")) {
E 8
I 8
  if (!TipcMsgSetDest (msg, theDest)) {
E 8
    reg_error (17) ;
E 5
    return 0;
I 5
  }
E 5
  
I 5
  if (!TipcSrvMsgSend (msg, TRUE)) {
    reg_error (7);
    ipc_PackForget (msg);
    return 0;
  }
  if (!TipcSrvFlush ()) {
    reg_error (8); 
    ipc_PackForget (msg);
    return 0;
  }
E 5
D 4
  ipc_packforget (msg);
E 4
I 4
  ipc_PackForget (msg);
E 4
D 5
  return 1;
E 5
I 5
  return ie.number_packed;
E 5
}
D 5
/* -------------------------------- */
E 5
I 5
/* --------------------------------------------------------------- */
E 5
/*
 * Pack a variable number of items
 * into a smartsockets msg buff.
 *
 * take a double of
 *  dataType, data
 *
 * return the number of message
 * fragments "packed"
 */
int ipc_VaPack (T_IPC_MSG msg, ...) 
{
  int     r, number_packed;
  char    *ms, *ts; 
  T_STR   sN ;
  T_INT4  i4;
  T_REAL4 r4;
  T_REAL8 r8;

  va_list arg;

  number_packed = 0;
  va_start(arg, msg);

  while (1) {
    ts = va_arg (arg, char *);
    if (ts == 0)
      break ;
    if ((strcmp (ts, "str")) == 0) {
      sN = va_arg (arg, char *);
      if (!TipcMsgAppendStr (msg, sN)) 
	break ;  /* error */
      else
	number_packed += 1;
    }
    else
      if ((strcmp (ts, "int4")) == 0) {
	i4 = va_arg (arg, int);
	if (!TipcMsgAppendInt4 (msg, i4)) 
	  break;  /* error */
	else
	  number_packed += 1;
      }
      else
	if ((strcmp (ts, "flt4")) == 0) {
	  r4 = (float)va_arg (arg, double);
	  if (!TipcMsgAppendReal4 (msg, r4)) 
	    break;  /* error */
	  else
	    number_packed += 1;
	}
	else
	  if ((strcmp (ts, "flt8")) == 0) {
	    r8 = va_arg (arg, double);
	    if (!TipcMsgAppendReal8 (msg, r8)) 
	      break;  /* error */
	    else
	      number_packed += 1;
	  }
	  else
	    break;
  }
  va_end (arg);
  return number_packed;
}
D 5
/* -======================================================- 
main ()
E 5
I 5
/* ------------------------------------------------------------------ */
/*
 * Pack a variable number of pairs,
 * where each pair is made up of:
 * 1. an integer numberRows/Number2Return.
 * 2. the sql command to execute.
 *
 * inputs:  a double of
 *  int, string
 *
 * return the number of message
 * fragments "packed"
 *
 * if any error return -1 and set global error
 * info.
 */
int dbrt_VaSendSql (int in, ...) 
E 5
{
D 5
  msg.i = 1;
  msg.j = 2;
  msg.k = 3;
E 5
I 5
  int       r ;
  T_STR     sN ;
  T_INT4    i4, FirstTime;
  T_IPC_MSG msgH;
  va_list   arg;
E 5

D 5
  ipc_packVa (&msg, "flt4", 102.67, 
	      "str", "hello world", 
	      "int4", 89,
	      "flt4", 43.123,
	      NULL) ;
E 5
I 5
  va_start(arg, in);
E 5

D 5
  dbrt_packTableVa (&msg, "XYZ_TABLE",
	      
	      "XYZ_comment", 
	      "int4", 
	      42, 
	      
	      "XYZ_Integer",
	      "str",
	      "hello world",
	      
	      "XYZ_Float 1",
	      "flt4",
	      42.06,
	      
	      "XYZ_Float 2",
	      "flt8",
	      42.43,
	      
	      NULL) ;
E 5
I 5
  clear_error () ;
  FirstTime = 1;
E 5

I 5
  msgH = ipc_PackInit (msgH) ;

  while (1) {
    if (FirstTime) {
      FirstTime = 0;
      if ((TipcMsgAppendInt4 (msgH, in)) == 0) {
	reg_error (9) ;
	break;
      }
    }
    else {
      i4 = va_arg (arg, int);
      if (i4 <= 0) {
	break;  /* this is how I break out */
      }         /* NOT an error case */

      if ((TipcMsgAppendInt4 (msgH, i4)) == 0) {
	reg_error (11) ;
	break ;
      }
    }
    sN = va_arg (arg, char *);
D 6
    if (sN <= NULL) {
E 6
I 6
    if (sN <= (T_STR)0) {
E 6
      reg_error (12) ;
      break ;
    }
    if ((TipcMsgAppendStr (msgH, sN)) == 0) {
      reg_error (13) ;
      break;
    }
    ie.number_packed += 1;
  }
  va_end (arg);
  
  if (ie.num_errors) {
    ipc_PackForget (msgH);
    return -1 ;
  }

  if (!TipcMsgSetDest (msgH, theDest)) {
    reg_error (14);
    ipc_PackForget (msgH);
    return -1;
  }
  
  if (!TipcSrvMsgSend (msgH, TRUE)) {
    reg_error (15) ;
    ipc_PackForget (msgH);
    return -1;
  }
  
  if (!TipcSrvFlush ()) {
    reg_error (16) ;
    ipc_PackForget (msgH);
    return -1;
  }

  ipc_PackForget (msgH);

  return ie.number_packed;
E 5
}
D 5
-==========================================================- */
E 5
I 5
/* ------------------------------------------------------------------ */
/*
 */
T_IPC_MSG dbrt_VaSendWRSql (int in, ...)
{
D 7
  T_IPC_MT  mt1;
  T_IPC_MT  mt2;
  T_IPC_MSG msg;
  T_IPC_MSG rmsg;
  T_IPC_MSG msgH;
E 7
  int       FirstTime ;
D 7
  int       i4;
E 7
I 7
  int       i4, r;
E 7
  char     *sN;

  va_list   arg;

  va_start(arg, in);

  clear_error () ;
  FirstTime = 1;

D 6
  if ((mt1 = TipcMtLookup (DBR_REPLY_MTNAME_RPC)) == 0) {
    mt1 = TipcMtCreate (DBR_REPLY_MTNAME_RPC, DBR_REPLY_MTNUM_RPC, "verbose");
    if (mt1 == 0) {
      reg_error (18);
      return 0;
    }
E 6
I 6
D 7
  if ((mt1 = TipcMtLookup ("dbr_reply_rpc")) == 0) {
    reg_error (18);
E 7
I 7
  if (rpc_mt1 == 0) {
    if ((rpc_mt1 = TipcMtLookup ("dbr_reply_rpc")) == 0) {
      reg_error (18);
    }
E 7
E 6
  }
D 7
  rmsg = TipcMsgCreate (mt1); 

D 6
  if ((mt2 = TipcMtLookup (DBR_REQUEST_MTNAME_RPC)) == 0) {
    mt2 = TipcMtCreate (DBR_REQUEST_MTNAME_RPC, DBR_REQUEST_MTNUM_RPC, "verbose");
    if (mt2 == 0) {
      reg_error (19);
      return 0;
    }
E 6
I 6
  if ((mt2 = TipcMtLookup ("dbr_request_rpc")) == 0) {
    reg_error (19);
    return 0;
E 7
I 7
  rpc_rmsg = TipcMsgCreate (rpc_mt1); 
  if (rpc_mt2 == 0) {
    if ((rpc_mt2 = TipcMtLookup ("dbr_request_rpc")) == 0) {
      reg_error (19);
      return 0;
    }
E 7
E 6
  }
D 7
   
  msgH = TipcMsgCreate (mt2);
E 7
I 7
  rpc_msgH = TipcMsgCreate (rpc_mt2);
E 7

  while (1) {
    if (FirstTime) {
      FirstTime = 0;
D 7
      if ((TipcMsgAppendInt4 (msgH, in)) == 0) {
E 7
I 7
      if ((TipcMsgAppendInt4 (rpc_msgH, in)) == 0) {
E 7
	reg_error (20) ;
	break;
      }
    }
    else {
      i4 = va_arg (arg, int);
      if (i4 <= 0) {
	break;  /* this is how I break out */
      }         /* NOT an error case */

D 7
      if ((TipcMsgAppendInt4 (msgH, i4)) == 0) {
E 7
I 7
      if ((TipcMsgAppendInt4 (rpc_msgH, i4)) == 0) {
E 7
	reg_error (22) ;
	break ;
      }
    }
    sN = va_arg (arg, char *);
D 6
    if (sN <= NULL) {
E 6
I 6
    if (sN <= (T_STR)0) {
E 6
      reg_error (23) ;
      break ;
    }
D 7
    if ((TipcMsgAppendStr (msgH, sN)) == 0) {
E 7
I 7
    if ((TipcMsgAppendStr (rpc_msgH, sN)) == 0) {
E 7
      reg_error (24) ;
      break;
    }
    ie.number_packed += 1;
  }
  va_end (arg);
  
  if (ie.num_errors) {
D 7
    ipc_PackForget (msgH);
E 7
I 7
    ipc_PackForget (rpc_msgH);
E 7
D 6
    return -1 ;
E 6
I 6
    return 0 ;
E 6
  }

D 7
  if (!(TipcMsgSetDest (msgH, theDest))) {
E 7
I 7
  printf ("theDest [%s]\n", theDest);
  if (!(TipcMsgSetDest (rpc_msgH, theDest))) {
E 7
      reg_error (25) ;
      return 0;
  }

  printf ("sending ... \n");
D 7
  rmsg = TipcSrvMsgSendRpc (msgH, 24.0) ;
E 7
I 7
  rpc_rmsg = TipcSrvMsgSendRpc (rpc_msgH, 24.0) ; 
E 7

D 7
  if (rmsg == 0) {
E 7
I 7
  if (rpc_rmsg == 0) {
E 7
    reg_error (26);
    return 0;
  }

D 7
  TipcMsgDestroy (msgH);
  TipcMtDestroy (mt2);
  TipcMtDestroy (mt1);
  return rmsg ;
E 7
I 7
  TipcMsgDestroy (rpc_msgH);

  return rpc_rmsg ;
E 7
}
E 5
E 1
