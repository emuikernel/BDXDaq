h26027
s 00012/00000/00094
d D 1.3 96/11/01 15:31:15 muguira 4 3
c JAM - add some comments
e
s 00008/00002/00086
d D 1.2 96/09/30 17:06:41 muguira 3 1
c JAM - change to adapt to Elliott's latest change.
e
s 00000/00000/00000
d R 1.2 96/08/29 17:38:09 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/test/rcv2.c
c Name history : 1 0 test/rcv2.c
e
s 00088/00000/00000
d D 1.1 96/08/29 17:38:08 muguira 1 0
c JAM build test tools for libipc
e
u
U
f e 0
t
T
I 4
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

E 4
I 1
#include <rtworks/ipc.h>
D 3
#include <clas_ipc_mt.h>
E 3
I 3

E 3
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
D 3
  mt = TipcMtLookupByNum (DBR_REPLY_MTNUM_RPC ) ;
E 3
I 3
  mt = TipcMtLookup ("dbr_reply_rpc");
E 3
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

I 3
  /*
E 3
  mt1 = TipcMtCreate ("DBR_REPLY_MTNAME_RPC", DBR_REPLY_MTNUM_RPC, "verbose");
  if (mt1 == 0)
    error_out ("rcv: mt lookup 1  failed\n");

  mt2 = TipcMtCreate ("DBR_REQUEST_MTNAME_RPC", DBR_REQUEST_MTNUM_RPC, "verbose");
  if (mt2 == 0)
    error_out ("rcv: mt lookup 2 failed\n");
I 3
    */
E 3

I 3
  mt2 = TipcMtLookup ("dbr_request_rpc");
  if (mt2 == 0)
    error_out ("where is the request rpc message type");

E 3
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
E 1
