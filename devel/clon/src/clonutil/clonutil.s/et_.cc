//  et_.cc

//  fortran callable interface to et system

//  still to do:
//     control words?


//  ejw, 21-may-99


// for smartsockets
#include <rtworks/cxxipc.hxx>


extern "C" {


// system stuff
#include <signal.h>

#include "bosio.h"

// for et
#include <et.h>


// local variables
static int et_ok = 0;
static char *session;
static char et_filename[128];
static char *et_station_name;
et_openconfig openconfig;
static et_sys_id et_system_id;
static et_stat_id et_station_id;
static et_statconfig et_station_config;
static et_att_id et_attach_id;
static et_event *et_event_ptr;


// prototypes
char *strdupf(char *fname, int flen);
void connect_et();


//------------------------------------------------------------------------


int *init_et_(char *fsession, char *fstation, int *qsize, int *prescale,
	      int l1, int l2) {
  

  // copy session and station name
  session         = strdupf(fsession,l1);
  et_station_name = strdupf(fstation,l2);


  // create et file name
  sprintf(et_filename,"/tmp/et_sys_%s",session);
  et_open_config_init(&openconfig);


  // create and fill station config object
  et_station_config_init(&et_station_config);
  et_station_config_setblock(et_station_config,ET_STATION_NONBLOCKING);
  et_station_config_setselect(et_station_config,ET_STATION_SELECT_ALL);
  et_station_config_setuser(et_station_config,ET_STATION_USER_MULTI);
  et_station_config_setrestore(et_station_config,ET_STATION_RESTORE_OUT);
  et_station_config_setcue(et_station_config,*qsize);
  et_station_config_setprescale(et_station_config,*prescale);


  return (int*)-1;
}


//--------------------------------------------------------------------------


int *get_et_event_(int *iw, int *timeout) {

  // timeout in msec

  long len;
  int *evt;
  timespec stime;


  // attach to et system if it exists, return error if it doesn't
  if(et_ok==0) {
    connect_et();
    if(et_ok==0) {
      return(int *)0;
    }

  } else if(et_alive(et_system_id)==0) {
    return(int *)1;
  }	
  
  
  // et system ok...get event
  if(*timeout<=0) {
     if(et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_ASYNC,NULL)!=ET_OK) {
      return(int *)2;
    }
  } else {
    stime.tv_sec  = *timeout/1000;
    stime.tv_nsec = (*timeout-stime.tv_sec*1000)*1000;
    if(et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_TIMED,&stime)!=ET_OK) {
      return(int *)3;
    }
  }
  
  
  // extract event data and convert to bos
  et_event_getdata(et_event_ptr,(void **)&evt);
  et2bos(evt, iw, (char *)"E");


  // put event back
  et_event_put(et_system_id,et_attach_id,et_event_ptr);


  return(int *)-1;
}


//-------------------------------------------------------------------


void connect_et() {
  
  int status;
  sigset_t sigblock;
  

  et_ok=0;


  // open et system
  if(et_open(&et_system_id,et_filename,openconfig)!=ET_OK)return;


  // create station if not already created
  status=et_station_create(et_system_id,&et_station_id,et_station_name,et_station_config);
  if((status!=ET_OK)&&(status!=ET_ERROR_EXISTS)) { 
    et_forcedclose(et_system_id);
    return;
  }


  // block signals to THIS thread and any thread created by this thread
  // needed to keep signals from being delivered to et system threads
  sigfillset(&sigblock);
  pthread_sigmask(SIG_BLOCK,&sigblock,NULL);


  // attach to station
  status=et_station_attach(et_system_id,et_station_id,&et_attach_id);
  if(status!=ET_OK) {
    et_forcedclose(et_system_id);
    return;
  }
  

  // unblock signals
  pthread_sigmask(SIG_UNBLOCK,&sigblock,NULL);


  // success
  et_ok=1; 

  return;
}


//--------------------------------------------------------------------------


int *close_et_() {

  et_forcedclose(et_system_id);
  return(int *)-1;
}


//--------------------------------------------------------------------------


void evt_status_callback_(char *unique_idf, char *sessionf, int *et_ok, char *station_namef,
			  int *nevrec, double *rec_rate, int *nevproc, double *proc_rate,
			  int len1, int len2, int len3) {


  char *unique_id    = strdupf(unique_idf,   len1);
  char *session      = strdupf(sessionf,     len2);
  char *station_name = strdupf(station_namef,len3);
  char temp[100];


  // get server and server node
  TipcSrv &server=TipcSrv::Instance();
  T_STR srvnode = server.Node();


  TipcMsg status("evt_status");
  status.Dest("/evt_system/status");
  status.Sender(unique_id);
  sprintf(temp,"et:%s",station_name);
  status << unique_id << getenv("HOST") << session << srvnode 
	 << (T_INT4) *nevrec << *rec_rate << (T_INT4) *nevproc << *proc_rate
	 << (T_INT4) *et_ok << temp;
  server.Send(status,TRUE);
  server.Flush();


  // clean memory
  free(unique_id);
  free(session);
  free(station_name);


  return;
}


//---------------------------------------------------------------------


} // extern "C"


