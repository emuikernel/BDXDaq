/***********************************************************************
 * evtsender.c
 *
 * Generates events which it sends to the info_server process
 ***********************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include <rtworks/ipc.h>
#include "servmsg.h"
#include "argproc.h"

#define MAXSCALERS 2600

/* Default Callback.  If we get a message that we don't understand, we
 * want to just ignore it.  The message gets dropped on the floor.
 */
static void process_default(T_IPC_CONN conn,
			    T_IPC_CONN_PROCESS_CB_DATA data,
			    T_CB_ARG arg)
{
   return;
} 


/* very simple LCG */
long seed = 0;
long im = 139968;
long ia = 3877;
long ic = 29573;
long getrand(long lo, long hi)
{
    long foo;
    time_t timer;

    if (seed == 0)		/* initialize */
    {
	timer = time(&timer);
	seed = timer/1000000;
    }
    seed = (seed*ia+ic)%im;
    return((long)(lo + (hi-lo+1)*(double)(seed)/(double)(im)));
}


void main(int argc, char **argv)
{
  T_IPC_MT mt;
  T_IPC_MSG msg;
  T_OPTION appname;

  long scalers[MAXSCALERS],numscalers,ctr,timestamp;
  char *type="scaler";
  char stuff[128];
  int i;
  long val,vlow,vhi;

  printf("Event Generator - Starting up.  Please wait\n");
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
  if (!TutOptionSetEnum(appname,"jghtest"))
  {
      fprintf(stderr,"Error - could not set application name\n");
      exit(T_EXIT_FAILURE);
  }


  /* define the incoming and outgoing message types */
  create_new_types(SCA_EVT_NAME,INFO_SCA_EVT,SCA_EVT_GR);
  create_new_types(SCA_MSG_NAME,INFO_SCA_MSG,SCA_MSG_GR);

  /* make connection to smartsockets server */
  if (!TipcSrvCreate(T_IPC_SRV_CONN_FULL))
  {
      fprintf(stderr,"Error: Could not connect to server\n");
      exit (T_EXIT_FAILURE);
  }

  printf("Got connection.  starting to send\n");
  
  /* Now, with things set up correctly, we can just enter the main */
  /* processing loop and see what happens */
  ctr = 0;
  for (;;)
  {
/*    printf("Hit <RETURN> to send a message\n");
    gets(stuff); */
    printf("Going to sleep\n");
    
      sleep(10);

    printf("Sending: \n");
    for (i = 0; i < MAXSCALERS; i++)
    {
	vlow = getrand(i/8,MAXSCALERS/4);
	vhi = getrand(MAXSCALERS/8+i/8,MAXSCALERS/2);
	val = abs(getrand(vlow,vhi));
	scalers[i] = val;
    }
    ctr = getrand(50,75);
    numscalers = MAXSCALERS;
    timestamp = ctr;

    /* Now that we have the needed information, we construct an outgoing */
    /* message and send the data along. */

    mt = TipcMtLookup(SCA_EVT_NAME);
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
		      T_IPC_PROP_DEST, "fnort",
		      T_IPC_FT_STR, type,
		      T_IPC_FT_INT4, timestamp,
		      T_IPC_FT_INT4_ARRAY, scalers, numscalers,
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
  }

}

