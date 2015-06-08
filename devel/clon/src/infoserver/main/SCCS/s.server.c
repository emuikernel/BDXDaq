h06463
s 00000/00000/00288
d D 1.9 97/06/05 11:58:09 jhardie 10 9
c fixed some small problems with smartsockets arrays
e
s 00000/00000/00288
d D 1.8 97/06/05 10:40:34 jhardie 9 8
c Version 1.0 of info_server and support programs
e
s 00001/00001/00287
d D 1.7 97/06/02 18:37:11 jhardie 8 7
c Rewrite parser for full C++.  Info messages input ok,
c output still broken
e
s 00001/00001/00287
d D 1.6 96/12/09 17:06:19 jhardie 7 6
c fixed bug in event modulo.  -m 1 would always be false so the server was throwing away all the events.  Change the test to == 0 rather than == 1 and things work fine now.
e
s 00014/00000/00274
d D 1.5 96/12/07 20:03:28 jhardie 6 5
c changed to add event modulo - drops scaler events on the floor if the rate is too high.
e
s 00028/00001/00246
d D 1.4 96/12/04 12:53:47 jhardie 5 4
c Made more robust with respect to multiple display processes running at the same time.
e
s 00000/00000/00247
d D 1.3 96/11/26 12:18:02 jhardie 4 3
c prototype version for dec test run
e
s 00017/00001/00230
d D 1.2 96/11/20 10:33:42 jhardie 3 1
c Added command line processing
c 
e
s 00000/00000/00000
d R 1.2 96/10/30 13:07:51 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 4 3 info_server/server/server.c
c Name history : 3 2 server/server.c
c Name history : 2 1 s/server/server.c
c Name history : 1 0 s/server.c
e
s 00231/00000/00000
d D 1.1 96/10/30 13:07:50 jhardie 1 0
c Source code for server program.
c Scaler specifig, non clas-ipc compliant
e
u
U
f e 0
t
T
I 1
/********
 * server.c
 *
I 5
 * Info_Server program.  Serves as an intermediate buffer between information
 * sources and information consumers.  
 * First Version: Scalers only, simple event repackaging and redistribution.
 * Future Versions:  Should use ipc and be reconfigurable.  Should also
 *                   accept broadcast commands.
 * 
 * Author: J. Hardie
 *
E 5
*********/

#include <stdio.h>
#include <unistd.h>

#include <rtworks/ipc.h>
#include "servmsg.h"
I 3
#include "argproc.h"
E 3

#define MAXSCALERS 1024


/*
 * Definitions of callback functions.  These functions will process the
 * individual messages as they come in.
 */


/* Default Callback.  If we get a message that we don't understand, we
 * want to just ignore it.  The message gets dropped on the floor.
 */
static void process_default(T_IPC_CONN conn,
			    T_IPC_CONN_PROCESS_CB_DATA data,
			    T_CB_ARG arg)
{
   return;
} 


/* Scaler Events:  If we get an incoming scaler event, then we will need
 * to reprocess it.
 */
I 6
static int numdumped=0;

E 6
static void process_scaler_evt(T_IPC_CONN conn,
			       T_IPC_CONN_PROCESS_CB_DATA data,
			       T_CB_ARG arg)
{
    T_IPC_MSG evt;

    /* these are for outgoing messages */
    T_IPC_MSG msg;
    T_IPC_MT mt;
    T_STR type;


    /* these are for internal use.  We have a maximum number of scalers */
    /* that we can send in a single message.  If the total number of */
    /* scalers exceeds this, the message gets truncated. */

    /* message timestamps are the number of hundretdths of a second */
    /* since the beginning of the run.  This allows for a maximum */
    /* single data run of just over 49 days. */
    long timestamp;

    /* the scaler array */
    long *scalers, numscalers, *newscal;

    int i,j;			/* some misc variables */

    printf("Got One ----> ");

    /* Initialize the incoming event message */
    evt = data->msg;
    if (!TipcMsgSetCurrent(evt,0)) 
    {
	fprintf(stdout,"Error: could not set message field to zero\n");
	return;
    }

    /* Process the message.  We expect the incoming event format */
    /* to have the following form: */
    /* string -> event type.  This should be either "scaler" or "tagger" */
    /* int4 -> timestamp      The time that the event occurred. */
    /* *int4 -> array of integer4 elements containing the scalers */

    if (!TipcMsgRead(evt, T_IPC_FT_STR, &type,
		     T_IPC_FT_INT4, &timestamp,
		     T_IPC_FT_INT4_ARRAY, &scalers, &numscalers,
		     NULL))
    {
	fprintf(stderr,"Error: Could not read incoming message\n");
	fprintf(stderr,"Got name(%s) and time(%ld)\n",type,timestamp);
	fprintf(stderr,"NumScalers(%ld)\n",numscalers);
	if (numscalers <= MAXSCALERS)
	{
	    for (i=0; i< numscalers; i++)
		fprintf(stderr,"Scaler(%d) = (%ld)\n",i,scalers[i]);
	}
	return;
    }
    
I 6
    printf("Scaler Event Stream: %s\n",type);
    printf("Scaler Time Stamp: %ld \n",timestamp);
    printf("Number of Scalers: %ld \n",numscalers);

    numdumped++;

D 7
    if (numdumped%arg_mod_events != 1)
E 7
I 7
    if (numdumped%arg_mod_events != 0)
E 7
      return;
    
       
E 6
    newscal = (long *)malloc(numscalers*sizeof(long));

    for (i=0; i< numscalers; i++)
    {
        newscal[i] = scalers[i];
        printf("Scaler(%d) = %ld(%ld)\n",i,scalers[i],newscal[i]);
    }
        

    /* Now that we have the needed information, we construct an outgoing */
    /* message and send the data along. */

    mt = TipcMtLookup(SCA_MSG_NAME);
    if (mt == NULL)
    {
	fprintf(stdout,"Error: Cant find scaler message type\n");
	return;
    }

    msg = TipcMsgCreate(mt);
    if (msg == NULL)
    {
	fprintf(stdout,"Error: Couldnt create scaler message\n");
	return;
    }

    if (!TipcMsgWrite(msg,
		      T_IPC_PROP_DEST, "infoscaler",
		      T_IPC_FT_STR, type,
		      T_IPC_FT_INT4, timestamp,
		      T_IPC_FT_INT4_ARRAY, newscal, numscalers,
		      NULL))
    {
	fprintf(stderr,"Error: could not construct the scaler message\n");
	return;
    }

    /* At this point, we should have a properly constructed message */
    /* so we send it out the pipeline. */
    if (!TipcSrvMsgSend(msg, TRUE))
    {
	fprintf(stderr,"Error: Could not send the scaler message\n");
	TipcMsgDestroy(msg);
	return;
    }

    if (!TipcSrvFlush())
    {
	fprintf(stderr,"Error: Unable to flush outgoing queue\n");
	TipcMsgDestroy(msg);
	return;
    }

    TipcMsgDestroy(msg);
    free((char *)newscal);

    printf("Sent One!\n");

    return;
}


void main(int argc, char **argv)
{
  T_IPC_MT mt;
I 3
D 5
  T_OPTION appname;
E 5
I 5
  T_OPTION appname,uniqdg;
  T_STR uniqname = "infoserver";
E 5
E 3

D 3

E 3
  printf("Event Server - Starting up.  Please wait\n");
I 3

  argproc(argc,argv);
I 6

  printf("Event Modulo: %d \n",arg_mod_events);
E 6
E 3
  
  /* start SmartSockets */

  if (!TutCommandParseStdLicense())
  {
      fprintf(stderr,"Error: Could not load license\n");
      exit(T_EXIT_FAILURE);
  }

  TutCommandParseStr("setopt server_names jlabs2.cebaf.gov");

I 3
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
E 3

I 3

I 5
  /* Look up the unique name and set it to the default value.  This
   * is done so that only one version of the info server will be started.
   * If someone attempts to start multiple servers, the connect to the
   * SmartSockets server will fail and the program will exit.
   */
  uniqdg = TutOptionLookup("Unique_Datagroup");
  if (uniqdg == NULL)
  {
      fprintf(stderr,"Error - could not find unique DG option\n");
      exit(T_EXIT_FAILURE);
  }
  if (!TutOptionSetEnum(uniqdg,uniqname))
  {
      fprintf(stderr,"Error - could not set unique name\n");
      exit(T_EXIT_FAILURE);
  }
  
E 5
E 3
  /* define the incoming and outgoing message types */
  create_new_types(SCA_EVT_NAME,INFO_SCA_EVT,SCA_EVT_GR);
  create_new_types(SCA_MSG_NAME,INFO_SCA_MSG,SCA_MSG_GR);

  /* make connection to smartsockets server */
  if (!TipcSrvCreate(T_IPC_SRV_CONN_FULL))
  {
      fprintf(stderr,"Error: Could not connect to server\n");
I 5
      fprintf(stderr,"Check for server already running\n");
E 5
      exit (T_EXIT_FAILURE);
  }

  /* define callbacks for the default message type and the scaler_evt */
  /* message type. */
  mt = TipcMtLookup(SCA_EVT_NAME);
  if (mt == NULL)
  {
      fprintf(stdout,"Error: Could not look up scaler event type\n");
      exit(T_EXIT_FAILURE);
  }
  if (TipcSrvProcessCbCreate(mt,process_scaler_evt,NULL) == NULL)
  {
      fprintf(stdout,"Error: could not create scaler event callback\n");
      exit(T_EXIT_FAILURE);
  }

  if (TipcSrvDefaultCbCreate(process_default,NULL) == NULL)
  {
      fprintf(stderr,"Error: Could not create default callback\n");
      exit(T_EXIT_FAILURE);
  }


  /* Tell the server that we want to receive the event stream "scalevts" */
  if (!TipcSrvStdDgSetRecv(TRUE,FALSE))
  {
      fprintf(stderr,"Error could not recv std data groups\n");
      exit(T_EXIT_FAILURE);
  }

D 8
  if (!TipcSrvDgSetRecv("scalevts",TRUE))
E 8
I 8
  if (!TipcSrvDgSetRecv("info_server",TRUE))
E 8
  {
      fprintf(stderr,"Error could not recv SCALEVTS data group\n");
      exit(T_EXIT_FAILURE);
  }


  /* Now, with things set up correctly, we can just enter the main */
  /* processing loop and see what happens */
  for (;;)
  {
      if (!TipcSrvMainLoop(T_TIMEOUT_FOREVER))
      {
	  fprintf(stderr,"Error: Main Loop ailed with error %d",
		  TutErrNumGet());
      }
  }

}

E 1
