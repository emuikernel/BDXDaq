h17764
s 00000/00000/00000
d R 1.2 98/01/22 12:45:06 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 info_server/misc/killer.c
c Name history : 1 0 misc/killer.c
e
s 00072/00000/00000
d D 1.1 98/01/22 12:45:05 jhardie 1 0
c setup
e
u
U
f e 0
t
T
I 1
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

E 1
