/********
 * Killer.  Sends a shutdown message to all processes waiting on the
 * scalevt or infoscaler message streams.
 *
*********/

#include <stdio.h>
#include <unistd.h>

#include <rtworks/ipc.h>
#include "servmsg.h"
#include "argproc.h"

#define MAXSCALERS 1024



void main(int argc, char **argv)
{
  T_IPC_MT mt;
  T_OPTION appname;

  printf("Event Killer - Starting up.  Please wait\n");
  
  argproc(argc,argv);

  /* start SmartSockets */

  if (!TutCommandParseStdLicense())
  {
      fprintf(stderr,"Error: Could not load license\n");
      exit(T_EXIT_FAILURE);
  }

  TutCommandParseStr("setopt server_names jlabs2.cebaf.gov");

  /* Lookup the application name and set it to the supplied value */
  appname = TutOptionLookup("application");
  if (appname == NULL)
  {
      fprintf(stderr,"Error - could not find application option\n");
      exit(T_EXIT_FAILURE);
  }
  if (!TutOptionSetEnum(appname,arg_app_name))
  {
      fprintf(stderr,"Error - could not set application name\n");
      exit(T_EXIT_FAILURE);
  }

  /* make connection to smartsockets server */
  if (!TipcSrvCreate(T_IPC_SRV_CONN_FULL))
  {
      fprintf(stderr,"Error: Could not connect to server\n");
      exit (T_EXIT_FAILURE);
  }

  printf("Sending control messages\n");
  mt = TipcMtLookupByNum(T_MT_CONTROL);
  TipcSrvMsgWrite("scalevts",mt,TRUE,
		  T_IPC_FT_STR, "quit force",
		  NULL);

  printf("Flushing\n");
  TipcSrvFlush();

  mt = TipcMtLookupByNum(T_MT_CONTROL);
  TipcSrvMsgWrite("infoscaler",mt,TRUE,
		  T_IPC_FT_STR, "quit force",
		  NULL);
  TipcSrvDestroy(T_IPC_SRV_CONN_NONE);
}

