h58355
s 00003/00015/00076
d D 1.2 96/09/30 17:06:53 muguira 3 1
c JAM - change to adapt to Elliott's latest change.
e
s 00000/00000/00000
d R 1.2 96/08/29 17:38:09 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/test/tt2.c
c Name history : 1 0 test/tt2.c
e
s 00091/00000/00000
d D 1.1 96/08/29 17:38:08 muguira 1 0
c JAM build test tools for libipc
e
u
U
f e 0
t
T
I 1
/*
 *  Test out dbrt_VaPackSql
 *
 * JAM - 8/29/96 - init version
 *
 */
#include <rtworks/ipc.h>
#include <stdlib.h>
#include <sys/wait.h>
D 3
#include <clas_ipc_mt.h>
E 3
I 3

E 3
/* -------------------------------- */
void parse_it (msg)
T_IPC_MSG msg;
{
  T_IPC_FT  ft;
  T_STR     str_val;
  T_REAL4   r4;
  T_INT4    i4;
  T_REAL8   r8;

  if (!TipcMsgSetCurrent (msg, 0))
    error_out ("set current failed\n");
  while (1) {
    if (!TipcMsgNextType (msg, &ft))
      return;

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
}

/* -------------------------------- */
main ()
{
  T_IPC_MSG msg;
  T_IPC_MSG rmsg;
  T_OPTION  option;
  int       r, stat_location, mp, j, cnt;
  float     aFloat[5]; 
  pid_t     pids[512];
  char      id_str[20];


  strcpy (id_str, "stupid");
  r = ipc_init (id_str, "MrMagoo") ;
  if (r != 0) {
    printf ("ipc_init failed with: %d\n", r);
    exit (0);
  }
D 3
  dbrt_SetDest ("james");
  rmsg = dbrt_VaSendWRSql (123, "Elliott wolin", 123, "look at me", NULL);
E 3
I 3
  dbrt_SetDest ("dbrouter");
  rmsg = dbrt_VaSendWRSql (123, "select * from run_log_begin", NULL);
E 3
  parse_it (rmsg) ;
  TipcMsgDestroy (rmsg);
D 3

  rmsg = dbrt_VaSendWRSql (123, "Elliott wolin", 123, "look at me", NULL);
  parse_it (rmsg) ;
  TipcMsgDestroy (rmsg);

  rmsg = dbrt_VaSendWRSql (123, "Elliott wolin", 123, "look at me", NULL);
  parse_it (rmsg) ;
  TipcMsgDestroy (rmsg);

  rmsg = dbrt_VaSendWRSql (123, "Elliott wolin", 123, "look at me", NULL);
  parse_it (rmsg) ;
  TipcMsgDestroy (rmsg);
E 3

  printf ("done ...\n");

}
E 1
