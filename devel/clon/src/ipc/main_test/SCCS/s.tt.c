h44831
s 00004/00004/00112
d D 1.4 96/08/29 17:38:22 muguira 5 4
c 
e
s 00002/00053/00114
d D 1.3 96/07/23 22:19:07 muguira 4 3
c JAM - change the names 
e
s 00003/00003/00164
d D 1.2 96/07/23 22:04:02 muguira 3 1
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
d R 1.2 96/07/23 21:05:14 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/test/tt.c
c Name history : 1 0 test/tt.c
e
s 00167/00000/00000
d D 1.1 96/07/23 21:05:13 muguira 1 0
c 
e
u
U
f e 0
t
T
I 1
#include <rtworks/ipc.h>
#include <stdlib.h>
#include <sys/wait.h>


D 3
#define NUMBER_TO_FORK 5
E 3
I 3
D 5
#define NUMBER_TO_FORK 15
E 5
I 5
#define NUMBER_TO_FORK 1
E 5
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
  T_OPTION  option;
  int       r, stat_location, mp, j, cnt;
  float     aFloat[5]; 
  pid_t     pids[512];
  char      id_str[20];

I 4
D 5
  ipc_PackInit ();
E 4

E 5
  for (cnt=0, j=0; j<NUMBER_TO_FORK; j++) {
    if ((mp=fork()) == 0) {
      sprintf (id_str, "stupid%d", j);
      r = ipc_init (id_str, "MrMagoo") ;
      if (r != 0) {
	printf ("ipc_init failed with: %d\n", r);
	exit (0);
      }

      for (r = 0; r<5; r++)
	aFloat[r] = drand48();
      
I 5
      r = dbrt_VaPackCmd (12, "hello Elliott", NULL);

E 5
      r = dbrt_VaPackTable ("XYZ_TABLE",
			    
			    "XYZ_comment", 
			    "str", 
			    "A stupid comment", 
			    
			    "XYZ_col1",
D 3
			    "flt9",
E 3
I 3
			    "flt8",
E 3
			    aFloat[0],
			    
			    "XYZ_col2",
			    "flt4",
			    aFloat[1],
			    
			    "XYZ_col3",
			    "flt4",
			    aFloat[2],
			    
			    "XYZ_col4",
			    "flt4",
			    aFloat[3],
			    
			    "XYZ_col5",
			    "flt4",
			    aFloat[4],
			    
			    NULL) ;
D 5
      
E 5
I 5

E 5
D 3
      if (r != 0)
E 3
I 3
      if (r == 0)
E 3
D 4
	ipc_packerror () ;
E 4
I 4
	ipc_PackError () ;
E 4
      return 0;
    }
    else {
      printf ("parent: proc id = %d\n", mp);
      if (mp != -1)
	cnt++;
    }
  }
  for (j=0; j<NUMBER_TO_FORK; j++)
    wait (&stat_location) ;

  printf ("actual number of process forked: %d\n", cnt);
}
D 4
/* =========================================================== */
/* =========================================================== */
/* =========================================================== */
/*
  r = TutCommandParseStdLicense () ;
  printf ("r = %d\n", r);

  option = TutOptionLookup ("application");
  if (option == 0)
    error_out ("option look up failed\n");

  if (!TutOptionSetEnum (option, "clastest")) 
    error_out ("set option failed");

  printf ("create the message type\n");

  msg = ipc_packinit (msg) ;

  r = TipcSrvCreate (T_IPC_SRV_CONN_FULL) ;
  if (r == 0) {
    printf ("not server connection\n");
    exit (0);
  }
  printf ("done...Now appending messages...\n");

  r = ipc_packVa (msg, 
		  "flt4", 102.67, 
		  "str", "hello world", 
		  "int4", 89,
		  "flt4", 123.4321,
		  NULL) ;

  printf ("packing return: %d\n", r);
*/
/*
  if (!TipcMsgSetDest (msg, "james"))
    error_out ("destination could not be set\n");

  printf ("\n\nDone appending... reset and read back\n");

  parse_it (msg);

  printf ("\n... done now send it\n");
  
  if (!TipcSrvMsgSend (msg, TRUE))
    error_out ("send failed\n");

  if (!TipcSrvFlush ()) 
    error_out ("flush failed\n");

  ipc_packforget (msg);
*/
E 4
E 1
