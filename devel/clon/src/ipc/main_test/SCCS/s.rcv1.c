h19897
s 00000/00000/00110
d D 1.2 96/07/23 22:03:40 muguira 3 1
c JAM - first time on the sun. MUST source the rtlink.csh in
c /apps/smartsockets35/bin/ and the have the following environment
c variables defined:
c CC
c LNK_LIB
c INCLUDE
c CFLAGS
c LDFLAGS
e
s 00000/00000/00000
d R 1.2 96/07/23 21:05:15 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/test/rcv1.c
c Name history : 1 0 test/rcv1.c
e
s 00110/00000/00000
d D 1.1 96/07/23 21:05:14 muguira 1 0
c 
e
u
U
f e 0
t
T
I 1
#include <rtworks/ipc.h>
/* -------------------------------- */
void error_out (s)
char *s;
{
  printf ("%s", s);
  exit (0);
}
/* -------------------------------- */
void parse_it (msg)
T_IPC_MSG msg;
{
  T_IPC_FT  ft;
  T_STR     str_val;
  T_REAL4   r4;
  T_INT4    i4;
  T_REAL8   r8;

  printf ("================== start ==================================\n");
  if (!TipcMsgSetCurrent (msg, 0))
    error_out ("set current failed\n");
  while (1) {
    if (!TipcMsgNextType (msg, &ft))
      break;

    printf ("t ipc ft = %d\n", ft);
    
    switch (ft) {
    case T_IPC_FT_STR:
      if (!TipcMsgNextStr (msg, &str_val))
	error_out ("could not get next string\n");
      printf ("--> %s\n\n", str_val);
      break;
    case T_IPC_FT_INT4:
      if (!TipcMsgNextInt4 (msg, &i4))
	error_out ("could not get next int4\n");
      printf ("--> %d\n", i4);
      break;
    case T_IPC_FT_REAL4:
      if (!TipcMsgNextReal4 (msg, &r4))
	error_out ("could not get next float4\n");
      printf ("--> %f\n", r4);
      break;
    case T_IPC_FT_REAL8:
      if (!TipcMsgNextReal8 (msg, &r8))
	error_out ("could not get next float8\n");
      printf ("--> %f\n", r8);
      break;
    }
  }
  printf ("==================== end ================================\n");
}
/* -------------------------------- */
void cb_process (conn, data, arg)
T_IPC_CONN                 conn;
T_IPC_CONN_PROCESS_CB_DATA data;
T_CB_ARG                   arg;
{

  parse_it (data->msg);

}
/* -------------------------------- */
void main (argc, argv)
int argc;
char *argv[];
{
  T_IPC_MSG msg;
  T_OPTION  option;
  T_IPC_MT  mt;
  T_BOOL    rt, mr;
  int       r;

  r = ipc_init ("james", "MrMagoo") ;
  if (r != 0) {
    printf ("ipc_init failed with: %d\n", r);
    exit (0);
  }
  printf ("have server connection...\n");

  mt = TipcMtLookupByNum (T_MT_NUMERIC_DATA);
  if (mt == 0)
    error_out ("rcv: mt lookup failed\n");

  if (!TipcSrvProcessCbCreate (mt, cb_process, NULL))
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
