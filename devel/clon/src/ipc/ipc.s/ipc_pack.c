/* ------------------------------------------------------------
 *  Pack various data into smart sockets message buffers.
 *
 *  Contains:
 *  ipc_PackInit 
 *  ipc_PackForget
 *  ipc_PackError
 *  ipc_VaPack
 *  dbrt_VaSendRow
 *  dbrt_VaSendSql
 *  dbrt_VaSendWRSql
 *
 *  This code currently uses a smartsockets define NUMERIC data
 *  messsage type to transfer data in.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 * JAM - initial version.
 * JAM - add in dbrt_VaPackCmd
 * JAM - change to Elliot's message types. I ONLY lookup 
 *       by name. The ipc_init call created the needed structures
 *       in the server.
 */
#include <stdarg.h>
#include <stdio.h>
#include <rtworks/ipc.h>
/*  include <clas_ipc_mt.h>  */

T_IPC_MT mt;

typedef struct ipc_ERR {
  char  *ms, *ts;
  int   number_packed;
  int   num_errors ;
  int   error_index [16] ;
} ipc_ERR ;

T_IPC_MT  rpc_mt1=0;
T_IPC_MT  rpc_mt2;
T_IPC_MSG rpc_msg;
T_IPC_MSG rpc_rmsg;
T_IPC_MSG rpc_msgH;

static ipc_ERR ie ;
static char *er_msg_table[30] = 
{
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
 
} ;

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
void dbrt_answer_error ()
{
  return ;
}
/* -------------------------------------------------------------- */
void error_out (char *s)
{
  printf ("%s\n", s);
  exit (0);
}
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
/* 
 * init the packer. 
 * 1. get a smartsockets msgtype
 * 2. get a new smartsockets msg structure
 *    and return it.
 *
 */
T_IPC_MSG ipc_PackInit (msg)
T_IPC_MSG msg;
{
  mt = (T_IPC_MT)TipcMtLookup ("dbr_request");
  if (mt == 0) {
    printf ("create failed\n");
    exit (0) ;
  }
  /*  printf ("create the message\n"); */
  
  msg = TipcMsgCreate (mt);
  if (msg == 0) {
    printf ("create failed\n");
    exit (0) ;
  }

  return msg ;
}
/* ------------------------------------------------------------ */
void ipc_PackForget (msg)
T_IPC_MSG msg;
{
  TipcMsgDestroy (msg);
}
/* ------------------------------------------------------------ */
void clear_error ()
{
  int j;

  for (j=0; j<16; j++)
    ie.error_index[j] = -1;

  ie.number_packed = ie.num_errors = 0;
  ie.ms = 0; ie.ts = 0;
}
/* ------------------------------------------------------------ */
/* 
 * provide verbose feedback about a
 * packing error.
 *
 * makes use of the static error struct
 * carried in this file.
 *
 */
void ipc_PackError ()
{
  int j;

  printf ("\n\n------------ Error Messages ---------------\n");
  printf ("number packed = %d\nnumber of errors = %d\n", 
	  ie.number_packed, ie.num_errors);
  for (j=0; j<16; j++)
    if (ie.error_index[j] > 0)
      printf ("error message = %s\n", er_msg_table[ie.error_index[j]]);

  printf ("-------------------------------------------\n");
}
/* --------------------------------------------------------------- */
/* 
 * Pack a table row - var arguments
 * 
 * takes a triple of 
 *  column, dataType, data
 *
 * returns:
 *  0  = failed - use ipc_packerror
 *  >0 = success.
 */
int dbrt_VaSendRow (char *t_name, ...)
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

  msg = ipc_PackInit (msg) ;

  if (t_name == 0)
    return 0;
  if (!TipcMsgAppendStr (msg, t_name)) {
    reg_error (0) ;
    return 0 ;
  }
  else
    ie.number_packed += 1;

  while (1) {
    ie.ms = va_arg (arg, char *) ;
    if (ie.ms == 0)
      break;
    if (!TipcMsgAppendStr (msg, ie.ms)) {
      reg_error (1) ;
      break;
    } else
      ie.number_packed += 1;

    ie.ts = va_arg (arg, char *);
    if ((strcmp (ie.ts, "str")) == 0) {
      sN = va_arg (arg, char *);
      if (!TipcMsgAppendStr (msg, sN)) {
	reg_error (2) ;
	break ;  
      }
      else
	ie.number_packed += 1;
    }
    else
      if ((strcmp (ie.ts, "int4")) == 0) {
	i4 = va_arg (arg, int);
	if (!TipcMsgAppendInt4 (msg, i4)) {
	  reg_error (3) ;
	  break; 
	}
	else
	  ie.number_packed += 1;
      }
      else
	if ((strcmp (ie.ts, "flt4")) == 0) {
	  r4 = (float)va_arg (arg, double);
	  if (!TipcMsgAppendReal4 (msg, r4)) { 
	    reg_error (4) ;
	    break;
	  }
	  else
	    ie.number_packed += 1;
	}
	else
	  if ((strcmp (ie.ts, "flt8")) == 0) {
	    r8 = va_arg (arg, double);
	    if (!TipcMsgAppendReal8 (msg, r8)) {
	      reg_error (5) ;
	      break;
	    } 
	    else
	      ie.number_packed += 1;
	  }
	  else {
	    reg_error (6) ;
	    break;
	  }
  }
  va_end (arg);

  if (ie.num_errors > 0) {
    ipc_PackForget (msg);
    return 0;
  }
  if (!TipcMsgSetDest (msg, theDest)) {
    reg_error (17) ;
    return 0;
  }
  
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
  ipc_PackForget (msg);
  return ie.number_packed;
}
/* --------------------------------------------------------------- */
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
{
  int       r ;
  T_STR     sN ;
  T_INT4    i4, FirstTime;
  T_IPC_MSG msgH;
  va_list   arg;

  va_start(arg, in);

  clear_error () ;
  FirstTime = 1;

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
    if (sN <= (T_STR)0) {
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
}
/* ------------------------------------------------------------------ */
/*
 */
T_IPC_MSG dbrt_VaSendWRSql (int in, ...)
{
  int       FirstTime ;
  int       i4, r;
  char     *sN;

  va_list   arg;

  va_start(arg, in);

  clear_error () ;
  FirstTime = 1;

  if (rpc_mt1 == 0) {
    if ((rpc_mt1 = TipcMtLookup ("dbr_reply_rpc")) == 0) {
      reg_error (18);
    }
  }
  rpc_rmsg = TipcMsgCreate (rpc_mt1); 
  if (rpc_mt2 == 0) {
    if ((rpc_mt2 = TipcMtLookup ("dbr_request_rpc")) == 0) {
      reg_error (19);
      return 0;
    }
  }
  rpc_msgH = TipcMsgCreate (rpc_mt2);

  while (1) {
    if (FirstTime) {
      FirstTime = 0;
      if ((TipcMsgAppendInt4 (rpc_msgH, in)) == 0) {
	reg_error (20) ;
	break;
      }
    }
    else {
      i4 = va_arg (arg, int);
      if (i4 <= 0) {
	break;  /* this is how I break out */
      }         /* NOT an error case */

      if ((TipcMsgAppendInt4 (rpc_msgH, i4)) == 0) {
	reg_error (22) ;
	break ;
      }
    }
    sN = va_arg (arg, char *);
    if (sN <= (T_STR)0) {
      reg_error (23) ;
      break ;
    }
    if ((TipcMsgAppendStr (rpc_msgH, sN)) == 0) {
      reg_error (24) ;
      break;
    }
    ie.number_packed += 1;
  }
  va_end (arg);
  
  if (ie.num_errors) {
    ipc_PackForget (rpc_msgH);
    return 0 ;
  }

  printf ("theDest [%s]\n", theDest);
  if (!(TipcMsgSetDest (rpc_msgH, theDest))) {
      reg_error (25) ;
      return 0;
  }

  printf ("sending ... \n");
  rpc_rmsg = TipcSrvMsgSendRpc (rpc_msgH, 24.0) ; 

  if (rpc_rmsg == 0) {
    reg_error (26);
    return 0;
  }

  TipcMsgDestroy (rpc_msgH);

  return rpc_rmsg ;
}
