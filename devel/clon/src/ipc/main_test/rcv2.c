/*
 *  1 version of a receive function.
 *  I do a lot of the ipc_init functions by hand
 *  I should use ipc_init but this came before and 
 *  I don't have time to change it.
 *  This is not a bad thing because this file may
 *  be the only perment place examples of 
 *  some of this code lives.
 *
 *  JAM 11/01/1996 - initial version...
 */

#include <rtworks/ipc.h>

static double kk; 

/* -------------------------------- */
void error_out (s)
char *s;
{
  printf ("%s", s);
  exit (0);
}
/* -------------------------------- */
void cb_process (conn, data, arg)
T_IPC_CONN                 conn;
T_IPC_CONN_PROCESS_CB_DATA data;
T_CB_ARG                   arg;
{
  T_STR caller;
  T_IPC_MT  mt;

  printf ("working now\n");
  kk += 1.0 ;
  mt = TipcMtLookup ("dbr_reply_rpc");
  if (mt == 0)
    printf ("cb error 1\n");

  if (!TipcMsgGetSender (data->msg, &caller))
    printf ("cb error 2\n");

  if (!TipcSrvMsgWrite (caller, mt, FALSE, T_IPC_FT_REAL8, kk, NULL))
    printf ("cb error 3\n");

  if (!TipcSrvFlush ())
    printf ("cb error 4\n");
}
/* -------------------------------- */
void main (argc, argv)
int argc;
char *argv[];
{
  T_IPC_MSG msg;
  T_OPTION  option;
  T_IPC_MT  mt1, mt2;
  T_BOOL    rt, mr;
  int       r;

  kk = 1.0;
  r = ipc_init ("james", "MrMagoo") ;
  if (r != 0) {
    printf ("ipc_init failed with: %d\n", r);
    exit (0);
  }
  printf ("have server connection...\n");

  /*
  mt1 = TipcMtCreate ("DBR_REPLY_MTNAME_RPC", DBR_REPLY_MTNUM_RPC, "verbose");
  if (mt1 == 0)
    error_out ("rcv: mt lookup 1  failed\n");

  mt2 = TipcMtCreate ("DBR_REQUEST_MTNAME_RPC", DBR_REQUEST_MTNUM_RPC, "verbose");
  if (mt2 == 0)
    error_out ("rcv: mt lookup 2 failed\n");
    */

  mt2 = TipcMtLookup ("dbr_request_rpc");
  if (mt2 == 0)
    error_out ("where is the request rpc message type");

  if (!TipcSrvProcessCbCreate (mt2, cb_process, NULL))
    error_out ("process callback create failed\n");

  if (!TipcSrvDgSetRecv ("james1", TRUE))
    error_out ("set rcv data group failed\n");

  for (;;) {
    mr = TipcSrvMainLoop (T_TIMEOUT_FOREVER) ;
    if (mr == 0) {
      rt = TutErrNumGet () ;
      switch (rt) {
      case T_ERR_VAL_TOO_SMALL:
	printf ("value too small: %d\n", rt);
	break;
      case T_ERR_SRV_NOT_CONNECTED:
	printf ("not connected: %d\n", rt);
	break;
      default:
	printf ("took an unknown error: %d %d\n", rt, mr);
	perror ("well");
	break;
      }
    }
  }

}
