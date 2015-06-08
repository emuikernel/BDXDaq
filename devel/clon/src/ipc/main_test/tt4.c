/*
 *  Test all of the "upperlevel" calls to
 *  the variable argument packer.
 *
 * JAM - 11/01/96 - init version
 *
 */
#include <rtworks/ipc.h>
#include <stdlib.h>
#include <sys/wait.h>

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
  char      uni_dgrp[20];
  char      id_str[20];


  strcpy (id_str, "stupid");
  strcpy (uni_dgrp, "MrMagoo");
  /*
   * the clas ipc lib needs 2 strings, a unique data group 
   *      and the application name
   * id_str - the id of this process
   * uni_dgrp - the unique data group.
   */
  r = ipc_init (uni_dgrp, id_str) ;
  if (r != 0) {
    /* 
     * must have failed!
     * print out the return code and die
     */
    printf ("ipc_init failed with: %d\n", r);
    exit (0);
  }

  /* 
   * working so far...
   * lets now set the destination data group
   */
  dbrt_SetDest ("dbrouter");

  /*
   * ok lets try to send some data around...
   *
   * 1st test: send many "things" to the dest data group
   *     VaSendSql is documented else where...
   */      
  r = dbrt_VaSendSql (1234, "Hello Elliott", 
		      4321, "Hello James", 
		      987,"killer dude", NULL);

  /* did we error? */

  if (r <= 0)
    ipc_PackError ();

  /*
   * now for a new test ...
   * send stuff to a data base row. Assumes u know the row schema
   */
  for (r = 0; r<5; r++)
    aFloat[r] = drand48();

  r = dbrt_VaSendRow ("XYZ_TABLE",
			
			"XYZ_comment", 
			"str", 
			"A stupid comment", 
			
			"XYZ_col1",
			"flt8",
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
  
  if (r <= 0)
    ipc_PackError () ;

  /* 
   *  lastly, Send sql and wait for a reply...
   */
  rmsg = (T_IPC_MSG)dbrt_VaSendWRSql (123, "select * from run_log_begin", 0);
  parse_it (rmsg) ;
  TipcMsgDestroy (rmsg);

  printf ("\n\n\n======================================\n\n\n\n");
  /* 
   *  lastly, Send sql and wait for a reply...
   */
  rmsg = (T_IPC_MSG)dbrt_VaSendWRSql (123, "select * from run_log_begin", NULL);
  parse_it (rmsg) ;
  TipcMsgDestroy (rmsg);

  printf ("\n\n\n======================================\n\n\n\n");
  /* 
   *  lastly, Send sql and wait for a reply...
   */
  rmsg = (T_IPC_MSG)dbrt_VaSendWRSql (123, "select * from run_log_begin", NULL);
  parse_it (rmsg) ;
  TipcMsgDestroy (rmsg);

  printf ("\n\n\n======================================\n\n\n\n");
  /* 
   *  lastly, Send sql and wait for a reply...
   */
  rmsg = (T_IPC_MSG)dbrt_VaSendWRSql (123, "select * from run_log_begin", NULL);
  parse_it (rmsg) ;
  TipcMsgDestroy (rmsg);

  printf ("\n\n\n======================================\n\n\n\n");
  /* 
   *  lastly, Send sql and wait for a reply...
   */
  rmsg = (T_IPC_MSG)dbrt_VaSendWRSql (123, "select * from run_log_begin", NULL);
  parse_it (rmsg) ;
  TipcMsgDestroy (rmsg);

  printf ("\n\n\n======================================\n\n\n\n");
  /* 
   *  lastly, Send sql and wait for a reply...
   */
  rmsg = (T_IPC_MSG)dbrt_VaSendWRSql (123, "select * from run_log_begin", NULL);
  parse_it (rmsg) ;
  TipcMsgDestroy (rmsg);

  printf ("\n\n\n======================================\n\n\n\n");

  printf ("done ...\n");

}
