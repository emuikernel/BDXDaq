h48736
s 00004/00011/00228
d D 1.11 01/11/16 15:43:04 boiarino 13 12
c add 'iw' as first argument in 'get_et_event' function
c as soon as online recsis is using it already like that
c (and it is right think to do)
c 
c 
e
s 00004/00004/00235
d D 1.10 00/10/19 13:42:23 wolin 12 10
c For new cc
e
s 00004/00005/00236
d D 1.8.1.1 00/09/05 14:39:53 wolin 11 9
c New CC
e
s 00005/00007/00234
d D 1.9 00/10/19 13:25:06 wolin 10 9
c Added forcedclose
c 
e
s 00000/00001/00241
d D 1.8 00/02/07 12:05:42 wolin 9 8
c Seems to work, according to Gerry
e
s 00000/00000/00242
d D 1.7 00/02/02 16:17:23 wolin 8 7
c updated for new et
e
s 00008/00006/00234
d D 1.6 00/01/04 15:14:22 wolin 7 6
c Updated for new et
e
s 00044/00000/00196
d D 1.5 99/07/06 13:59:50 wolin 6 5
c Added evt_status_callback
c 
e
s 00012/00005/00184
d D 1.4 99/06/29 10:48:56 wolin 5 4
c Now using et2bos
c 
e
s 00018/00006/00171
d D 1.3 99/05/25 11:27:47 wolin 4 3
c Added timout, other mods for Gerry
c 
e
s 00006/00002/00171
d D 1.2 99/05/24 11:41:54 wolin 3 1
c Removed control word stuff
c 
e
s 00000/00000/00000
d R 1.2 99/05/24 11:38:04 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 etutil/s/et_.cc
e
s 00173/00000/00000
d D 1.1 99/05/24 11:38:03 wolin 1 0
c 
e
u
U
f e 0
t
T
I 1
//  et_.cc

//  fortran callable interface to et system

I 3
//  still to do:
//     control words?


E 3
//  ejw, 21-may-99


D 11
I 12
// for smartsockets
#include <rtworks/cxxipc.hxx>


E 12
extern "C" {
E 11
I 11
// for smartsockets
#include <rtworks/cxxipc.hxx>
E 11


// system stuff
#include <signal.h>


I 11
extern "C" {

E 11
// for et
#include <et.h>
D 11
D 12


I 6
// for smartsockets
#include <rtworks/cxxipc.hxx>
E 11
E 12


E 6
I 5
D 13
// for bos
#define NBCS 800000
#include <bosfun.h>
#include <bcs.h>



E 13
E 5
// local variables
D 10
static int et_ok                    = 0;
E 10
I 10
static int et_ok = 0;
E 10
static char *session;
static char et_filename[128];
static char *et_station_name;
I 7
et_openconfig openconfig;
E 7
static et_sys_id et_system_id;
static et_stat_id et_station_id;
static et_statconfig et_station_config;
static et_att_id et_attach_id;
static et_event *et_event_ptr;
D 3
static int et_control[ET_STATION_SELECT_INTS];
E 3
I 3
D 10
// static int et_control[ET_STATION_SELECT_INTS];
E 10
E 3


// prototypes
char *strdupf(char *fname, int flen);
void connect_et();
D 5
void dd2bos_(long *evlen, long *pdata, char *rt, int len);
E 5
I 5
D 13
int et2bos(long *evt, int *jw, char *list);
E 13
I 13
int et2bos(int *evt, int *jw, char *list);
E 13
E 5


//------------------------------------------------------------------------


D 4
int *init_et_(char *fsession, char *fstation, int l1, int l2) {
E 4
I 4
int *init_et_(char *fsession, char *fstation, int *qsize, int *prescale,
	      int l1, int l2) {
E 4
  

  // copy session and station name
  session         = strdupf(fsession,l1);
  et_station_name = strdupf(fstation,l2);


  // create et file name
  sprintf(et_filename,"/tmp/et_sys_%s",session);
I 7
  et_open_config_init(&openconfig);
E 7


  // create and fill station config object
  et_station_config_init(&et_station_config);
  et_station_config_setblock(et_station_config,ET_STATION_NONBLOCKING);
  et_station_config_setselect(et_station_config,ET_STATION_SELECT_ALL);
D 5
  et_station_config_setuser(et_station_config,ET_STATION_USER_SINGLE);
E 5
I 5
  et_station_config_setuser(et_station_config,ET_STATION_USER_MULTI);
E 5
  et_station_config_setrestore(et_station_config,ET_STATION_RESTORE_OUT);
D 4
  et_station_config_setcue(et_station_config,10);
  et_station_config_setprescale(et_station_config,1);
E 4
I 4
  et_station_config_setcue(et_station_config,*qsize);
  et_station_config_setprescale(et_station_config,*prescale);
E 4


  return (int*)-1;
}


//--------------------------------------------------------------------------


D 4
int *get_et_event_() {
E 4
I 4
D 13
int *get_et_event_(int *timeout) {
E 13
I 13
int *get_et_event_(int *iw, int *timeout) {
E 13

  // timeout in msec
E 4

D 9
  char rt[16];
E 9
  long len;
D 13
  long *evt;
E 13
I 13
  int *evt;
E 13
I 4
  timespec stime;
E 4


  // attach to et system if it exists, return error if it doesn't
  if(et_ok==0) {
    connect_et();
    if(et_ok==0) {
      return(int *)0;
    }

  } else if(et_alive(et_system_id)==0) {
    return(int *)1;
  }	
  
  
D 10
  // et system ok...get event, sleep until event appears
E 10
I 10
  // et system ok...get event
E 10
D 4
  if(et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_WAIT_SLEEP,NULL)!=ET_OK) {
    return(int *)2;
E 4
I 4
  if(*timeout<=0) {
D 7
     if(et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_WAIT_ASYNC,NULL)!=ET_OK) {
E 7
I 7
     if(et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_ASYNC,NULL)!=ET_OK) {
E 7
      return(int *)2;
    }
  } else {
    stime.tv_sec  = *timeout/1000;
    stime.tv_nsec = (*timeout-stime.tv_sec*1000)*1000;
D 7
    if(et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_WAIT_TIMED,&stime)!=ET_OK) {
E 7
I 7
    if(et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_TIMED,&stime)!=ET_OK) {
E 7
      return(int *)3;
    }
E 4
  }
  
  
  // extract event data and convert to bos
D 3
  et_event_getcontrol(et_event_ptr,et_control);
E 3
I 3
D 10
  //  et_event_getcontrol(et_event_ptr,et_control);
E 10
E 3
D 7
  evt=(long*)et_event_getdata(et_event_ptr);
E 7
I 7
  et_event_getdata(et_event_ptr,(void **)&evt);
E 7
D 5
  dd2bos_(&len,evt,rt,16);
  
  
E 5
I 5
D 13
  et2bos(evt, bcs_.iw, "E");
E 13
I 13
  et2bos(evt, iw, "E");
E 13


E 5
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
D 7
  if(et_open(&et_system_id,et_filename,0,NULL)!=ET_OK)return;
E 7
I 7
  if(et_open(&et_system_id,et_filename,openconfig)!=ET_OK)return;
E 7


  // create station if not already created
D 7
  status=et_station_create(et_system_id,et_station_name,et_station_config,&et_station_id);
E 7
I 7
  status=et_station_create(et_system_id,&et_station_id,et_station_name,et_station_config);
E 7
  if((status!=ET_OK)&&(status!=ET_ERROR_EXISTS)) { 
D 10
    et_close(et_system_id);
E 10
I 10
    et_forcedclose(et_system_id);
E 10
    return;
  }


  // block signals to THIS thread and any thread created by this thread
  // needed to keep signals from being delivered to et system threads
  sigfillset(&sigblock);
  pthread_sigmask(SIG_BLOCK,&sigblock,NULL);


  // attach to station
D 7
  status=et_attach_to_station(et_system_id,&et_attach_id,et_station_id);
E 7
I 7
  status=et_station_attach(et_system_id,et_station_id,&et_attach_id);
E 7
  if(status!=ET_OK) {
D 10
    et_close(et_system_id);
E 10
I 10
    et_forcedclose(et_system_id);
E 10
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

D 10
  et_close(et_system_id);
E 10
I 10
  et_forcedclose(et_system_id);
E 10
  return(int *)-1;
}


//--------------------------------------------------------------------------
I 6


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
E 6


} // extern "C"


E 1
