
/* et.c - ET system interface */

#include "ced.h"

#ifndef NO_ET

/* system stuff */
#include <signal.h>

/* for et */
#include "et.h"

/* for bos */
#define NBCS 800000
#include "bos.h"
#include "bosio.h"

/* local variables */
static char *session;
static char et_filename[128];
static et_openconfig openconfig;
static char *et_station_name;
static et_sys_id et_system_id;
static et_stat_id et_station_id;
static et_statconfig et_station_config;
static et_att_id et_attach_id;
static et_event *et_event_ptr;
/* static int et_control[ET_STATION_SELECT_INTS]; */


/* prototypes */
void connect_et();
int et2bos(long *evt, int *jw, char *list);


/*------------------------------------------------------------------------ */

int
init_et_(char *session, char *station, int  qsize, int  prescale)
{
  /* create et file name */
  sprintf(et_filename,"/tmp/et_sys_%s",session);
  Hv_InitCharStr(&et_station_name, station);

  fprintf(stderr, "ET SESSION:  %s\n", session);
  fprintf(stderr, "ET STATION:  %s\n", et_station_name);
  fprintf(stderr, "ET QSIZE:    %d\n", qsize);
  fprintf(stderr, "ET PRESCALE: %d\n", prescale);
  fprintf(stderr, "ET FILENAME: %s\n", et_filename);
  et_open_config_init(&openconfig);

  /* create and fill station config object */
  et_station_config_init(&et_station_config);
  et_station_config_setblock(et_station_config, ET_STATION_NONBLOCKING);
  et_station_config_setselect(et_station_config,ET_STATION_SELECT_ALL);
  et_station_config_setuser(et_station_config,ET_STATION_USER_MULTI);
  et_station_config_setrestore(et_station_config,ET_STATION_RESTORE_OUT);
  et_station_config_setcue(et_station_config, qsize);
  et_station_config_setprescale(et_station_config, prescale);

  return 1;
}


/*-------------------------------------------------------------------------- */
/* timeout in msec */

int
get_et_event_(int timeout)
{
  char rt[16];
  long len;
  long *evt;

  /* attach to et system if it exists, return error if it doesn't */
  if(!etisok) {
      connect_et();
      if(!etisok) {
	  fprintf(stderr, "ET get event failed status: 0\n");
	  return 0; /* failure */
      }
		       
  } else if(et_alive(et_system_id)==0) {
      fprintf(stderr, "ET get event failed status: 1\n");
      return 1; /* another failure */
  }	
  
  
  /* et system ok...get event, sleep until event appears */
  if(timeout <= 0) {
      if(et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_ASYNC,NULL)!=ET_OK) {
	  fprintf(stderr, "ET get event failed status: 2\n");
	  return 2; /* another failure */
    }
  } else {
  }
  
  
  /* extract event data and convert to bos */
  /*  et_event_getcontrol(et_event_ptr,et_control); */

  et_event_getdata(et_event_ptr, (void **)&evt);
  et2bos(evt, bcs_.iw, "E");

  /* put event back */
  et_event_put(et_system_id,et_attach_id,et_event_ptr);

  return -1; /* success! */
}


/*------------------------------------------------------------------- */

void
connect_et()
{  
  int status;
  sigset_t sigblock;

  etisok = False;

  /* open et system */
  if(et_open(&et_system_id,et_filename,openconfig)!=ET_OK) {
      fprintf(stderr, "Connect ET failed status (A)\n");
      return;
  }


  /* create station if not already created */
  status=et_station_create(et_system_id,
			   &et_station_id,
			   et_station_name,
			   et_station_config);

  if((status!=ET_OK)&&(status!=ET_ERROR_EXISTS)) { 
      et_close(et_system_id);
      fprintf(stderr, "Connect ET failed status (B)\n");
      return;
  }


  /* block signals to THIS thread and any thread created by this thread */
  /* needed to keep signals from being delivered to et system threads */
  sigfillset(&sigblock);
  pthread_sigmask(SIG_BLOCK,&sigblock,NULL);


  /* attach to station */
  status=et_station_attach(et_system_id,
			   et_station_id,
			   &et_attach_id);

  if(status!=ET_OK) {
    et_close(et_system_id);
    fprintf(stderr, "Connect ET failed status (C)\n");
    return;
  }
  

  /* unblock signals */
  pthread_sigmask(SIG_UNBLOCK,&sigblock,NULL);


  /* success */
  etisok = True; 

  return;
}


/*-------------------------------------------------------------------------- */

int
close_et_()
{
  et_close(et_system_id);
  return -1;
}

#endif
