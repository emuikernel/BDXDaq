h28438
s 00007/00006/01171
d D 1.30 07/10/12 15:15:06 boiarino 32 31
c abs -> fabs
c 
e
s 00009/00008/01168
d D 1.29 07/10/12 10:24:20 boiarino 31 30
c *** empty log message ***
e
s 00004/00001/01172
d D 1.28 07/10/12 10:01:05 boiarino 30 29
c *** empty log message ***
e
s 00017/00020/01156
d D 1.27 07/06/18 14:25:36 boiarino 29 28
c *** empty log message ***
e
s 00003/00003/01173
d D 1.26 02/06/12 13:23:36 wolin 28 27
c 96 scalers in scaler1
e
s 00003/00003/01173
d D 1.25 02/02/04 12:20:02 wolin 27 26
c Bug due to multiple TRGD banks
e
s 00024/00014/01152
d D 1.24 01/11/06 11:44:46 wolin 26 25
c Station created in clas_et_start
e
s 00035/00011/01131
d D 1.23 01/09/05 16:46:18 wolin 25 24
c Split hardware readout into another thread
e
s 00002/00002/01140
d D 1.22 01/08/20 11:11:16 wolin 24 23
c Only TRGS bank 0 triggers TRGD
e
s 00001/00001/01141
d D 1.21 01/08/15 17:08:39 wolin 23 22
c 25 sec scaler readout time
e
s 00002/00002/01140
d D 1.20 01/01/03 10:51:33 wolin 22 21
c is_open()
e
s 00006/00008/01136
d D 1.19 00/12/05 16:55:59 wolin 21 19
i 20
c Accepted child's version in workspace "/usr/local/clas/devel/source".
c 
e
s 00067/00067/01075
d D 1.17.1.1 00/10/26 17:11:45 wolin 20 18
c const
e
s 00010/00007/01135
d D 1.18 00/12/01 14:26:29 wolin 19 18
c Minor bombproofing of rec and proc rates
e
s 00003/00003/01139
d D 1.17 00/10/19 13:21:56 wolin 18 17
c Added forcedclose
e
s 00043/00004/01099
d D 1.16 00/02/29 12:33:35 wolin 17 16
c Added S1SD
e
s 00001/00001/01102
d D 1.15 00/02/08 12:28:58 wolin 16 15
c Increased hardware readout to 20 sec to handle slow daq
e
s 00002/00002/01101
d D 1.14 00/02/04 12:38:31 wolin 15 14
c TRGD now working
e
s 00030/00016/01073
d D 1.13 00/02/03 16:11:56 wolin 14 13
c Fixed trgd, still working on new scaler channels
e
s 00002/00002/01087
d D 1.12 00/01/18 12:57:29 wolin 13 12
c Fixed file name
e
s 00009/00007/01080
d D 1.11 00/01/07 16:40:24 wolin 12 11
c Gets events from ET instead of ipc
e
s 00001/00005/01086
d D 1.10 00/01/04 16:29:54 wolin 11 10
c Still working
e
s 00074/00064/01017
d D 1.9 99/12/16 10:36:24 wolin 10 9
c Minor reorganization
e
s 00241/00090/00840
d D 1.8 99/12/16 09:59:01 wolin 9 8
c Switched to ET, needs testing
e
s 00001/00002/00929
d D 1.7 99/11/02 11:42:33 wolin 8 7
c Default no_fcup=0, using init script
e
s 00009/00003/00922
d D 1.6 99/11/01 12:43:11 wolin 7 6
c no_fcup=1, other minor mods
e
s 00010/00010/00915
d D 1.5 99/09/10 15:26:28 wolin 6 5
c Added another scaler to trgs bank
c 
e
s 00003/00003/00922
d D 1.4 99/07/07 11:31:46 wolin 5 4
c Typos in scaler labels
c 
e
s 00006/00005/00919
d D 1.3 99/05/12 11:26:44 wolin 4 3
c Hardware scalers now shipped in int4, not binary
e
s 00110/00012/00814
d D 1.2 99/03/18 14:21:34 wolin 3 1
c Added hardware scalers and file
c 
e
s 00000/00000/00000
d R 1.2 99/03/18 13:48:31 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 scalers/s/scaler_server.cc
e
s 00826/00000/00000
d D 1.1 99/03/18 13:48:30 wolin 1 0
c 
e
u
U
f e 0
t
T
I 1
//  scaler_server
//
D 9
//  gets scaler events from ipc and ships scaler information to info server
E 9
I 9
//  gets scaler events from ET and ships scaler information to info server
E 9
//  checks livetime, calculates TRGS differences
D 9
//  If no scaler event in 10 seconds reads hardware directly
E 9
I 9
//  If no scaler events reads hardware directly
E 9
//
I 10
D 11
//  still to do:
//     get all scaler events
//
E 10
D 9
//  ejw, 19-mar-99
E 9
I 9
//  ejw, 15-dec-99
E 9
//
E 11
I 11
D 12
//  ejw, 16-dec-99
E 12
I 12
D 14
//  ejw, 6-jan-00
E 14
I 14
//  still to do:
//    enable live_or test
//
//  ejw, 3-feb-00
E 14
E 12
E 11
D 9
//   still to do:
I 7
D 8
//       no_fcup=1
E 8
E 7
//       switch to UINT4
E 9


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for smartsockets
#include <rtworks/cxxipc.hxx>


// for i/o
I 30

using namespace std;
#include <strstream>

E 30
#include <fstream.h>
D 30
#include <strstream.h>
E 30
#include <iomanip.h>
#include <stdio.h>
D 31
#include <macros.h>
E 31
I 9
#include <pthread.h>
I 32
#include <math.h>
E 32
E 9


// for tcl
extern "C"{
#include "tcl.h"
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


I 9
// for et
extern "C" {
#include <et.h>
}

I 29
// for tcpClientCmd
#include "libtcp.h"
E 29

I 29

E 29
E 9
// for CLAS ipc
#include <clas_ipc_prototypes.h>

I 31
#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )
E 31

// misc variables
Tcl_Interp *interp;  
D 20
static char *application      	= "clastest";
E 20
I 20
static char *application      	= (char*)"clastest";
E 20
static char *session          	= NULL;
D 20
static char *uniq_name        	= "scaler_server";
E 20
I 20
static char *uniq_name        	= (char*)"scaler_server";
E 20
static char *init_tcl_script  	= NULL;
D 29
static char *unique_id        	= (char *) malloc(64);
E 29
I 29
static char *unique_id        	= (char *) malloc(512);
E 29
static char *host             	= getenv("HOST");
static double check_time      	= 2.0;
D 16
static int scaler_readout_time  = 11;        // seconds
E 16
I 16
D 23
static int scaler_readout_time  = 20;        // seconds
E 23
I 23
static int scaler_readout_time  = 25;        // seconds
E 23
E 16
static int done               	= 0;
static int debug              	= 0;
static int no_file            	= 0;
static int no_info            	= 0;
D 7
static int no_fcup            	= 0;
E 7
I 7
D 8
static int no_fcup            	= 1;
E 8
I 8
static int no_fcup            	= 0;
E 8
E 7
static double live_tol        	= 5.0;       // in percent
static int min_evt_live       	= 800000;    // about 10 minutes
static time_t last_time       	= time(NULL);
static time_t last_scaler    	= time(NULL);
static int current_run        	= 0;
static int nevt_run        	= 0;
static int total_bad_bank_count = 0;
static int bad_bank_count       = 0;
static int livetime_err_count   = 0;
I 9

I 21
static time_t delta;
E 21
E 9
D 19
static time_t delta;
E 19
D 9
static char temp[10000];
E 9
static int scaler_err_this_run = 0;
D 14
static double live_trig,live_clock,live_fcup;
E 14
I 14
static double live_trig,live_clock,live_fcup,live_or;
E 14
I 10


// lengths of scaler banks defined here
E 10
D 6
static unsigned long trgs[64];
static unsigned long last_trgs[64];
E 6
I 6
D 28
static unsigned long trgs[80];
static unsigned long last_trgs[80];
I 10
D 14
static unsigned long delta_trgs[80];
E 14
I 14
D 15
static long delta_trgs[80];
E 15
I 15
static unsigned long delta_trgs[80];
E 28
I 28
static unsigned long trgs[96];
static unsigned long last_trgs[96];
static unsigned long delta_trgs[96];
E 28
E 15
E 14
E 10
E 6
I 3
static unsigned long ecs[96];
static unsigned long scs[192];
static unsigned long sts[16];
I 17
static unsigned long last_s1st[35];
static unsigned long delta_s1st[35];
E 17
E 3
D 6
static T_INT4 delta_trgs[64];
E 6
I 6
D 10
static T_INT4 delta_trgs[80];
E 10
I 10

E 10
I 9

E 9
E 6
static char filename[132];
I 9
static char temp[10000];
E 9

static int nrec                 = 0;
static int nevent             	= 0;
static int last_rec          	= 0;
static int last_proc          	= 0;
static double rec_rate       	= 0.;
static double proc_rate       	= 0.;


I 9
// for et
static int et_ok              = 0;
D 20
static char *et_station_name        = "SCALER";
E 20
I 20
static char *et_station_name        = (char*)"SCALER";
E 20
I 12
static et_openconfig openconfig;
E 12
static et_sys_id et_system_id;
static char et_filename[128];
static et_stat_id et_station_id;
static et_statconfig et_station_config;
static et_att_id et_attach_id;
static et_event *et_event_ptr;

E 9

// prototypes
void decode_command_line(int argc, char **argv);
D 9
void T_ENTRY scaler_event_callback(T_IPC_CONN conn,
				   T_IPC_CONN_PROCESS_CB_DATA data,
				   T_CB_ARG arg);
E 9
I 9
D 20
void *process_events(void *param);
void *control_thread(void *param);
E 20
void analyze_event(long *evt);
void init_et();
void connect_et();
E 9
void get_hardware_scalers(void);
D 20
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
E 20
void init_tcl(void);
D 3
void scaler_to_file(long event[]);
E 3
I 3
void scaler_evt_to_file(long event[]);
void hardware_scaler_to_file();
E 3
long *find_next_bank(long*, char *, int &, int &, int &);
I 20
int find_tag_line(istrstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(istrstream &file, char buffer[], int buflen);
extern "C" {
E 20
void T_ENTRY control_message_callback(T_IPC_CONN conn,
				      T_IPC_CONN_PROCESS_CB_DATA data,
				      T_CB_ARG arg);
D 20
int find_tag_line(istrstream &file, char *tag, char buffer[], int buflen);
int get_next_line(istrstream &file, char buffer[], int buflen);
extern "C" {
E 20
I 20
void *process_events(void *param);
void *control_thread(void *param);
I 25
void *hardware_thread(void *param);
E 25
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
E 20
int get_run_status(char *session);
D 20
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
	      int severity, char *status, int code, char *message);
E 20
I 20
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	      int severity, const char *status, int code, const char *message);
E 20
D 29
void DP_cmd_init(char *msql_tcp_host);
void DP_cmd(char *roc, char *cmd, char *buf, int timeout);
E 29
}


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//--------------------------------------------------------------------------


main(int argc,char **argv) {

D 17
  int status;
E 17
I 17
  int i,status;
E 17
I 9
D 25
  pthread_t t1,t2;
  int r1=0,r2=0;
E 25
I 25
  pthread_t t1,t2,t3;
  int r1=0,r2=0,r3=0;
E 25
E 9


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line
  decode_command_line(argc,argv);
    
    
  // get session name
D 20
  if(session==NULL)session="clasprod";
E 20
I 20
  if(session==NULL)session=(char*)"clasprod";
E 20


  // create unique_id from uniq_name and session name
  if (debug==0) {
    strcpy(unique_id,uniq_name);
    strcat(unique_id,"_");
    strcat(unique_id,session);
  }


  // get Tcl interp, create tcl commands, link vars, process Tcl startup
  // script, etc.
  interp=tclinterp_init();
  init_tcl();
  if(init_tcl_script!=NULL)Tcl_EvalFile(interp,init_tcl_script);


  // set ipc parameters, connect to server, etc.
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
  ipc_set_control_message_callback(control_message_callback);
  status=ipc_init(unique_id,"scaler monitor");
  if(status<0) {
    cerr << "\n?Unable to connect to server..."
	 << "probably duplicate unique id\n"
	 << "   ...check for another scaler_server on " 
	 << host << " connected to " 
	 << session << " using ipc_info\n"
	 << "   ...only one such process allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }
D 20
  server.SubjectSubscribe("evt_system",TRUE);
E 20
I 20
  server.SubjectSubscribe((T_STR)"evt_system",TRUE);
E 20


I 29
  // enable Tcl ipc control (i.e. create tcl_request callback)
  tclipc_init(interp);


E 29
D 9
  // create scaler event callback 
  TipcMt mt("evt_event");
  server.ProcessCbCreate(mt,scaler_event_callback,0);


  // subscribe to scaler events
  sprintf(temp,"/evt_event/%s/scaler",session);
  server.SubjectSubscribe(temp,TRUE);
E 9
I 9
  // init et structures, etc;
  init_et();
I 10
D 17
  for(int i=0; i<sizeof(last_trgs)/sizeof(unsigned long); i++) {last_trgs[i]=0;}
E 17
I 17
  for(i=0; i<sizeof(last_trgs)/sizeof(unsigned long); i++) {last_trgs[i]=0;}
  for(i=0; i<sizeof(last_s1st)/sizeof(unsigned long); i++) {last_s1st[i]=0;}
E 17
E 10
E 9

D 29

  // enable Tcl ipc control (i.e. create tcl_request callback)
  tclipc_init(interp);


  // init dp_cmd
D 20
  DP_cmd_init(getenv("MSQL_TCP_HOST"));
E 20
I 20
  DP_cmd_init(getenv((char*)"MSQL_TCP_HOST"));
E 20


E 29
  // post startup message
  sprintf(temp,"Process startup:    %15s  in application:  %s",unique_id,application);
  status=insert_msg("scaler_server",unique_id,unique_id,"status",0,"START",0,temp);


D 9
  // flush pending messages and output
E 9
I 9
D 10
  // launch event thread
  if(pthread_create(&t1,NULL,process_events,(void *)&r1)!=0) {
    cerr << "\nUnable to create event thread" << endl << endl;
E 10
I 10
  // launch control thread
  if(pthread_create(&t1,NULL,control_thread,(void *)&r1)!=0) {
    cerr << "\nUnable to create control thread" << endl << endl;
E 10
    exit(EXIT_FAILURE);
  }


D 10
  // launch ipc thread
  if(pthread_create(&t2,NULL,control_thread,(void *)&r2)!=0) {
    cerr << "\nUnable to create control thread" << endl << endl;
E 10
I 10
  // launch event thread
  if(pthread_create(&t2,NULL,process_events,(void *)&r2)!=0) {
    cerr << "\nUnable to create event thread" << endl << endl;
E 10
    exit(EXIT_FAILURE);
  }


I 29
  /* Sergey: no hardware enqueries
E 29
I 25
  // launch hardware scaler readout thread
  if(pthread_create(&t3,NULL,hardware_thread,(void *)&r3)!=0) {
    cerr << "\nUnable to create hardware readout thread" << endl << endl;
    exit(EXIT_FAILURE);
  }
I 29
  */
E 29

D 29

E 29
E 25
  // flush everything
E 9
  server.Flush();
  cout << flush;


D 9
  // process scaler events, handle IPC messages, etc.
  // read scalers directly from hardware if no run in progress
  while(done==0) {
E 9
I 9
  // wait for control thread to terminate
  pthread_join(t1,NULL);
E 9

D 9
    server.MainLoop(check_time);
E 9
I 9
  
  // done
D 18
  et_close(et_system_id);
E 18
I 18
  et_forcedclose(et_system_id);
E 18
  ipc_close();
  sprintf(temp,"Process shutdown:  %15s",unique_id);
  status=insert_msg("scaler_server",unique_id,unique_id,"status",0,"STOP",0,temp);
  exit(EXIT_SUCCESS);
}
       

//--------------------------------------------------------------------------
E 9


I 9
void *control_thread(void *param) {

I 19
D 21
  time_t now;
  int delta;

E 21
E 19

  // handle IPC messages...read scalers directly from hardware if no scaler events arrive
  while(done==0) {
    
    server.MainLoop(check_time);
D 19
    
E 19
I 19
D 21
    now=time(NULL);
E 21
I 21
    
E 21
E 19
D 25
    
E 9
    // read scalers from hardware if no scalers in a while
D 19
    delta=time(NULL)-last_scaler;
E 19
I 19
D 21
    delta=now-last_scaler;
E 21
I 21
    delta=time(NULL)-last_scaler;
E 21
E 19
    if(delta>scaler_readout_time) {
      get_hardware_scalers();
D 19
      last_scaler=time(NULL);
E 19
I 19
D 21
      last_scaler=now;
E 21
I 21
      last_scaler=time(NULL);
E 21
E 19
    }
D 9


E 9
I 9
    
    
E 25
I 25

E 25
E 9
    // calc rates every 30 seconds
D 19
    delta=time(NULL)-last_time;
E 19
I 19
D 21
    delta=now-last_time;
E 21
I 21
    delta=time(NULL)-last_time;
E 21
E 19
    if(delta>30) {
      rec_rate=(double)(nrec-last_rec)/delta;
      proc_rate=(double)(nevent-last_proc)/delta;
D 19
      last_time=time(NULL);
E 19
I 19
D 21
      last_time=now;
E 21
I 21
      last_time=time(NULL);
E 21
E 19
      last_rec=nrec;
      last_proc=nevent;
    }
D 9

E 9
I 9
    
E 9
  }
  
I 9
  return (void *)0;
}
E 9

D 9
  // done
  ipc_close();
  sprintf(temp,"Process shutdown:  %15s",unique_id);
  status=insert_msg("scaler_server",unique_id,unique_id,"status",0,"STOP",0,temp);
  exit(EXIT_SUCCESS);
E 9
I 9

//------------------------------------------------------------------------


I 10
void *process_events(void *param) {

  int status;
  long *evt;


  while(done==0) {

    // check et system
    if(et_ok==0) {
      connect_et();
      if(et_ok==0) {
	sleep(1);
	continue;
      }
    } else if(et_alive(et_system_id)==0) {
      done=1;
      cerr << "?ET system died" << endl;
      return (void *)0;
    }	
    
    
    // get event...sleep if none available
D 12
    status=et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_WAIT_SLEEP,NULL);
E 12
I 12
    status=et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_SLEEP,NULL);
E 12
    

    if (status==ET_OK) {
      nrec++;
D 12
      evt=(long*)et_event_getdata(et_event_ptr);
E 12
I 12
      et_event_getdata(et_event_ptr,(void**)&evt);
E 12
      analyze_event(evt);

    } else {
      done=1;
      cerr << "?error return from et_event_get: " << status << endl;
      return (void *)0;
    }

    
    // put event back
    et_event_put(et_system_id,et_attach_id,et_event_ptr);
  }
  

  return (void *)0;
}


//-------------------------------------------------------------------
I 25


void *hardware_thread(void *param) {


  // read scalers directly from hardware if no scaler events arrive
  while(done==0) {
    
    sleep((int)check_time);
    
    // read scalers from hardware if no scalers read in a while
    delta=time(NULL)-last_scaler;
    if(delta>scaler_readout_time) {
      get_hardware_scalers();
      last_scaler=time(NULL);
    }

  }
  
  return (void *)0;
}


//------------------------------------------------------------------------
E 25


E 10
void init_et() {
  

  et_ok=0;


D 12
  // create et file name
E 12
I 12
  // create et file name, etc.
E 12
  sprintf(et_filename,"/tmp/et_sys_%s",session);
I 12
D 29
  et_open_config_init(&openconfig);
E 29
E 12

I 29
  //et_open_config_init(&openconfig);
E 29

  // get et library name
D 29
  char *et_user_library     = (char *) malloc(64);
  strcpy(et_user_library,getenv("CLON_LIB"));
D 20
  strcat(et_user_library,"/et_user_library.so");
E 20
I 20
  strcat(et_user_library,(char*)"/et_user_library.so");
E 29
I 29
  //char *et_user_library     = (char *) malloc(512);
  //strcpy(et_user_library,getenv("CLON_LIB"));
  //strcat(et_user_library,(char*)"/et_user_library.so");
E 29
E 20


I 26
  // station created in clas_et_start
E 26
  // create station config in case no station exists
D 26
  et_station_config_init(&et_station_config);
  et_station_config_setblock(et_station_config,ET_STATION_BLOCKING);
  et_station_config_setselect(et_station_config,ET_STATION_SELECT_USER);
  et_station_config_setuser(et_station_config,ET_STATION_USER_SINGLE);
  et_station_config_setrestore(et_station_config,ET_STATION_RESTORE_OUT);
  et_station_config_setcue(et_station_config,100);
  et_station_config_setprescale(et_station_config,1);
  et_station_config_setlib(et_station_config,et_user_library);
D 10
  et_station_config_setfunction(et_station_config,"et_mon_function");
E 10
I 10
D 20
  et_station_config_setfunction(et_station_config,"et_scaler_function");
E 20
I 20
  et_station_config_setfunction(et_station_config,(char*)"et_scaler_function");
E 26
I 26
//    et_station_config_init(&et_station_config);
//    et_station_config_setblock(et_station_config,ET_STATION_BLOCKING);
//    et_station_config_setselect(et_station_config,ET_STATION_SELECT_USER);
//    et_station_config_setuser(et_station_config,ET_STATION_USER_SINGLE);
//    et_station_config_setrestore(et_station_config,ET_STATION_RESTORE_OUT);
//    et_station_config_setcue(et_station_config,100);
//    et_station_config_setprescale(et_station_config,1);
//    et_station_config_setlib(et_station_config,et_user_library);
//    et_station_config_setfunction(et_station_config,(char*)"et_scaler_function");
E 26
E 20
E 10


  return;
E 9
}
D 9
       
E 9
I 9

E 9

//--------------------------------------------------------------------------


D 9
void T_ENTRY scaler_event_callback(T_IPC_CONN conn,
				   T_IPC_CONN_PROCESS_CB_DATA data,
				   T_CB_ARG arg) {
E 9
I 9
void connect_et() {
  
  int status;
  sigset_t sigblock;
  
E 9

D 9
  int i,banknum;
  T_INT4 *datap;
  TipcMsg *msg;
  char bankname[10];		// name of the current bank
  char scal_bankname[12];       // name of bank for scaler info server message
  int position;			// current position in event
  int nwords;			// the number of data words in the bank
  int found_bad_bank;
  int run;
  T_STR evt_src,ctl_fmt,evt_fmt;
  T_INT4 nctl;
E 9
I 9
  et_ok=0;
E 9

I 9
  
  // open et system
D 12
  if(et_open(&et_system_id,et_filename,0,NULL)!=ET_OK)return;
E 12
I 12
  if(et_open(&et_system_id,et_filename,openconfig)!=ET_OK)return;
E 12
  
E 9

I 26
  // never create station, just attach to existing station
E 26
I 9
  // create station if not already created
D 12
  status=et_station_create(et_system_id,et_station_name,et_station_config,&et_station_id);
E 12
I 12
D 26
  status=et_station_create(et_system_id,&et_station_id,et_station_name,et_station_config);
E 12
  if((status!=ET_OK)&&(status!=ET_ERROR_EXISTS)) { 
E 26
I 26
//    status=et_station_create(et_system_id,&et_station_id,et_station_name,et_station_config);
//    if((status!=ET_OK)&&(status!=ET_ERROR_EXISTS)) { 
//        cout << status << endl;
//        et_forcedclose(et_system_id);
//        cerr << "Unable to create station " << et_station_name << endl;
//        done=1;
//        return;
//    }
  status=et_station_name_to_id(et_system_id,&et_station_id,et_station_name);
  if(status!=ET_OK) {
E 26
      cout << status << endl;
D 18
      et_close(et_system_id);
E 18
I 18
      et_forcedclose(et_system_id);
E 18
D 26
      cerr << "Unable to create station " << et_station_name << endl;
E 26
I 26
      cerr << "?station " << et_station_name << " does not exist" << endl;
E 26
      done=1;
      return;
  }
D 26
  
E 26
I 26

E 26
  
  // block signals to THIS thread and any thread created by this thread
  // needed to keep signals from et threads
  sigfillset(&sigblock);
  pthread_sigmask(SIG_BLOCK,&sigblock,NULL);
  
  
  // attach to station
D 12
  status=et_attach_to_station(et_system_id,&et_attach_id,et_station_id);
E 12
I 12
  status=et_station_attach(et_system_id,et_station_id,&et_attach_id);
E 12
  if(status!=ET_OK) {
D 18
      et_close(et_system_id);
E 18
I 18
      et_forcedclose(et_system_id);
E 18
D 26
      cerr << "Unable to attach to station " << et_station_name << endl;
E 26
I 26
      cerr << "?unable to attach to station " << et_station_name << endl;
E 26
      done=1;
      return;
  }
  
E 9

D 9
  // scaler event variables
  TipcMsg evt(data->msg);
  T_INT4 len;
  T_INT4 ctlsize,datasize;
  T_INT4 *pctl, *pdata;
E 9
I 9
  // unblock signals
  pthread_sigmask(SIG_UNBLOCK,&sigblock,NULL);
E 9


D 9
  nrec++;
E 9
I 9
  // success
  et_ok=1; 
  cout << "...now connected to ET system: " << et_filename 
       << ",   station: " << et_station_name << endl;
E 9

I 9
  return;
}
E 9

D 9
  // check event source
  evt >> evt_src;
  if(strcasecmp(evt_src,"et")!=0)return;
E 9

I 9
//--------------------------------------------------------------------------
E 9

D 9
  // get ctl words
  evt >> ctl_fmt >> nctl >> pctl >> GetSize(&ctlsize);
E 9

I 9
D 10
void *process_events(void *param) {
E 9

D 9
  // get event
  evt >> evt_fmt;
  if(strcasecmp(evt_fmt,"fpack")!=0)return;
  evt >> len >> pdata >> GetSize(&datasize);
E 9
I 9
  int status;
  long *evt;
E 9


D 9
  // check if this is a scaler event
  if(strncasecmp((char*)(pdata+3),"SCAL",4)==0) {
E 9
I 9
  while(done==0) {
E 9

D 9
    nevent++;
    last_scaler=time(NULL);
E 9
I 9
    // check et system
    if(et_ok==0) {
      connect_et();
      if(et_ok==0) {
	sleep(1);
	continue;
      }
    } else if(et_alive(et_system_id)==0) {
      done=1;
      cerr << "?ET system died" << endl;
      return (void *)0;
    }	
    
    
    // get event...sleep if none available
    status=et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_WAIT_SLEEP,NULL);
    
    if (status==ET_OK) {
      nrec++;
      evt=(long*)et_event_getdata(et_event_ptr);
      analyze_event(evt);

    } else {
      done=1;
      cerr << "?error return from et_event_get: " << status << endl;
      return (void *)0;
    }
E 9

I 9
    
    // put event back
    et_event_put(et_system_id,et_attach_id,et_event_ptr);
  }
  
E 9

I 9
  return (void *)0;
}


//-------------------------------------------------------------------


E 10
void analyze_event(long *evt) {

  int i,banknum;
  char bankname[10];		// name of the current bank
  char scal_bankname[12];       // name of bank for scaler info server message
  int position;			// current position in event
  int nwords;			// the number of data words in the bank
  int found_bad_bank;
  int run;
  TipcMsg *msg;
  long *datap;


  // check if this is a scaler event
  if(strncasecmp((char*)(evt+3),"SCAL",4)==0) {
    
    nevent++;
    last_scaler=time(NULL);
    
    
E 9
    // reset bad bank flags,counters,run,etc.
D 9
    run=pdata[5];
E 9
I 9
    run=evt[5];
E 9
    if(run!=current_run) {
      current_run=run;
      bad_bank_count=0;
      scaler_err_this_run=0;
I 14
D 17
      for(int i=0; i<sizeof(last_trgs)/sizeof(unsigned long); i++) {last_trgs[i]=0;}
E 17
I 17
      for(i=0; i<sizeof(last_trgs)/sizeof(unsigned long); i++) {last_trgs[i]=0;}
      for(i=0; i<sizeof(last_s1st)/sizeof(unsigned long); i++) {last_s1st[i]=0;}
E 17
E 14
    }
    found_bad_bank=0;
D 9


E 9
I 9
    
    
E 9
    // create info_server message
    if(no_info==0) {
D 20
      msg = new TipcMsg("info_server");
D 14
      msg->Dest("info_server/in/scaler_server");
E 14
I 14
      msg->Dest("info_server/in/scaler_server/data");
E 14
      *msg << "scaler_server";
E 20
I 20
      msg = new TipcMsg((T_STR)"info_server");
      msg->Dest((T_STR)"info_server/in/scaler_server/data");
      *msg << (T_STR)"scaler_server";
E 20
    }      
    
    
    // loop over all banks (datap>0), fill msg and/or dump 
    // Note:  do NOT change nwords!
    if(debug!=0) cout << "\n\n------------------ new scaler event ----------------------" << endl;
    position = 0;
    nwords = 0;
D 9
    while ((datap=find_next_bank(pdata,bankname,banknum,position,nwords))>0) {
E 9
I 9
    while ((datap=find_next_bank(evt,bankname,banknum,position,nwords))>0) {
      
E 9
      
D 9

E 9
      // check for illegal banks in scaler event
      if(
	 (strncasecmp(bankname,"DC0",3)==0) ||
	 (strncasecmp(bankname,"EC ",3)==0) ||
	 (strncasecmp(bankname,"SC ",3)==0) ||
	 (strncasecmp(bankname,"CC ",3)==0) ||
	 (strncasecmp(bankname,"EC1",3)==0) ) {
	found_bad_bank=1;
      }
D 9


E 9
I 9
      
      
I 17
      // store software scaler diffs
      if(strncasecmp(bankname,"S1ST",4)==0) {
	for(i=0; i<sizeof(last_s1st)/sizeof(unsigned long); i++) {
	  delta_s1st[i]=(unsigned long)datap[i]-last_s1st[i];
	  last_s1st[i]=datap[i];
	}
      }
	
	
E 17
E 9
D 24
      // special handling for TRGS bank
      if(strncasecmp(bankname,"TRGS",4)==0) {
E 24
I 24
      // special handling for TRGS bank 0
      if((strncasecmp(bankname,"TRGS",4)==0)&&(banknum==0)) {
E 24
	
	// store trigger scaler diffs
D 6
	for(i=0; i<64; i++) {
E 6
I 6
D 10
	for(i=0; i<80; i++) {
E 10
I 10
	for(i=0; i<sizeof(last_trgs)/sizeof(unsigned long); i++) {
E 10
E 6
D 15
	  delta_trgs[i]=datap[i]-last_trgs[i];
E 15
I 15
	  delta_trgs[i]=(unsigned long)datap[i]-last_trgs[i];
E 15
	  last_trgs[i]=datap[i];
	}
D 9


E 9
I 9
	
	
E 9
	// check for livetime agreement after enough events taken
D 9
        nevt_run=datap[61];
E 9
I 9
	nevt_run=datap[61];
E 9
	live_trig  = (datap[60]>0)?(float)datap[61]/datap[60]:0.0;
	live_clock = (datap[16]>0)?(float)datap[32]/datap[16]:0.0;
	live_fcup  = (datap[17]>0)?(float)datap[33]/datap[17]:0.0;
I 14
	live_or    = (datap[34]>0)?(float)datap[61]/datap[34]:0.0;
E 14
	
	if( (nevt_run>min_evt_live) && (scaler_err_this_run==0) && 
	    (
D 14
	     (100.*abs(live_trig-live_clock)>live_tol) || 
	     ((no_fcup==0)&&
	      ((100.*abs(live_fcup-live_clock)>live_tol)||((100.*abs(live_fcup-live_trig)>live_tol))))
E 14
I 14
D 32
	     (100.*abs(live_trig  - live_clock)>live_tol) || 
//  	     (100.*abs(live_trig  - live_or   )>live_tol) || 
//  	     (100.*abs(live_clock - live_or   )>live_tol) || 
E 32
I 32
	     (100.*fabs(live_trig  - live_clock)>live_tol) || 
//  	     (100.*fabs(live_trig  - live_or   )>live_tol) || 
//  	     (100.*fabs(live_clock - live_or   )>live_tol) || 
E 32
	     ((no_fcup==0) &&
D 32
	      ( (100.*abs(live_fcup-live_clock)>live_tol) ||
//  	        ((100.*abs(live_fcup-live_or)>live_tol))  ||
	        ((100.*abs(live_fcup-live_trig)>live_tol)) ))
E 32
I 32
	      ( (100.*fabs(live_fcup-live_clock)>live_tol) ||
//  	        ((100.*fabs(live_fcup-live_or)>live_tol))  ||
	        ((100.*fabs(live_fcup-live_trig)>live_tol)) ))
E 32
E 14
D 9
	    )
	  ) {
E 9
I 9
	     )
	    ) {
E 9
	  scaler_err_this_run=1;
	  livetime_err_count++;
D 7
	  sprintf(temp,"Livetime disagreement run %d, trig,clock,fcup:  %f,  %f,  %f",
		  current_run,live_trig,live_clock,live_fcup);
E 7
I 7
	  if(no_fcup==0) {
D 14
	    sprintf(temp,"Livetime disagreement run %d, trig,clock,fcup:  %f,  %f,  %f",
		    current_run,live_trig,live_clock,live_fcup);
E 14
I 14
	    sprintf(temp,"Livetime disagreement run %d, trig,clock,fcup,or:  %f,  %f,  %f,  %f",
		    current_run,live_trig,live_clock,live_fcup,live_or);
E 14
	  } else {
D 14
	    sprintf(temp,"Livetime disagreement run %d, trig,clock:  %f,  %f",
		    current_run,live_trig,live_clock);
E 14
I 14
	    sprintf(temp,"Livetime disagreement run %d, trig,clock,or:  %f,  %f,  %f",
		    current_run,live_trig,live_clock,live_or);
E 14
	  }
E 7
	  insert_msg("online","scaler_server",unique_id,"status",1,"WARN",0,temp);
	  cout << temp << endl;;
	}
      }
D 9


E 9
I 9
      
      
E 9
      // fill ipc msg with encoded bank name and bank contents except DCST and HLS banks
D 17
      // ship out TRGD (diffs) if current bank is TRGS
E 17
      if(no_info==0) {
	if((strncasecmp(bankname,"DCST",4)!=0)&&(strncasecmp(bankname,"HLS",3)!=0))  {
	  sprintf(scal_bankname,"%-4s%02d",bankname,banknum);
	  for (int m=0; m<strlen(scal_bankname); m++) if(scal_bankname[m]==' ')scal_bankname[m]='_';
	  *msg << scal_bankname;
	  *msg << SetSize(nwords) << (T_INT4*)datap;
D 27
	  if(strncasecmp(bankname,"TRGS",4)==0) {
E 27
I 27
	  if((strncasecmp(bankname,"TRGS",4)==0)&&(banknum==0)) {
E 27
D 4
	    *msg << "TRGD00" << SetSize(64) << (T_INT4*)delta_trgs;
E 4
I 4
D 20
	    *msg << "TRGD00";
E 20
I 20
	    *msg << (T_STR)"TRGD00";
E 20
	    *msg << SetSize(nwords) << (T_INT4*)delta_trgs;
E 4
	  }
I 17
	  if(strncasecmp(bankname,"S1ST",4)==0) {
D 20
	    *msg << "S1SD00";
E 20
I 20
	    *msg << (T_STR)"S1SD00";
E 20
	    *msg << SetSize(nwords) << (T_INT4*)delta_s1st;
	  }
E 17
	}
      }
      
D 9

E 9
I 9
      
E 9
      // dump banks
      if(debug!=0) {
	cout << "\n\nScaler data for bank: " << bankname << " number " << banknum
	     << " Words = " << nwords << endl << endl;
	for (int k = 0; k < nwords; k++) {
	  cout << setw(10) << datap[k] << ' ';
	  if((k+1)%8 == 0) 	cout << endl;
	}
	cout << endl << endl;
D 9

E 9
I 9
	
E 9
D 27
	if(strncasecmp(bankname,"TRGS",4)==0) {
E 27
I 27
	if((strncasecmp(bankname,"TRGS",4)==0)&&(banknum==0)) {
E 27
D 6
	  cout << "\n\nTRGS differences in bank TRGD number 0 Words = 64" << endl << endl;
	  for (int k = 0; k < 64; k++) {
E 6
I 6
D 10
	  cout << "\n\nTRGS differences in bank TRGD number 0 Words = 80" << endl << endl;
	  for (int k = 0; k < 80; k++) {
E 10
I 10
	  cout << "\n\nTRGS differences in bank TRGD number 0 Words = " 
	       << sizeof(delta_trgs)/sizeof(unsigned long) << endl << endl;
	  for (int k = 0; k < sizeof(delta_trgs)/sizeof(unsigned long); k++) {
E 10
E 6
	    cout << setw(10) << delta_trgs[k] << ' ';
	    if((k+1)%8 == 0) 	cout << endl;
	  }
	}
I 17

	if(strncasecmp(bankname,"S1ST",4)==0) {
	  cout << "\n\nS1ST differences in bank S1SD number 0 Words = " 
	       << sizeof(delta_s1st)/sizeof(unsigned long) << endl << endl;
	  for (int k = 0; k < sizeof(delta_s1st)/sizeof(unsigned long); k++) {
	    cout << setw(10) << delta_s1st[k] << ' ';
	    if((k+1)%8 == 0) 	cout << endl;
	  }
	}

E 17
	cout << endl << endl;
      }
      
D 9

E 9
I 9
      
E 9
    } // loop over banks
    
D 9

E 9
I 9
    
E 9
    // send, flush, delete message
    if(no_info==0) {
      server.Send(*msg,TRUE);
      server.Flush();
      delete msg;
    }
    
D 9

E 9
I 9
    
E 9
    // post warning about bad banks (every 1 million events, count reset each run)
    if(found_bad_bank!=0) {
      total_bad_bank_count++;
      bad_bank_count++;
      if((bad_bank_count%1000000)==1) {
	insert_msg("online","CODA",unique_id,"status",2,"CLAS",0,
		   "SYNC ERROR:  scaler_server found illegal bank in scaler event");
      }
    }
D 9


E 9
I 9
    
    
E 9
    // write event to file
D 3
    scaler_to_file(pdata);
E 3
I 3
D 9
    scaler_evt_to_file(pdata);
E 9
I 9
D 13
    scaler_evt_to_file(evt);
E 13
I 13
    if(no_file==0)scaler_evt_to_file(evt);
E 13
E 9
E 3
    
    
D 9
  } // scaler event


E 9
I 9
  }
  
E 9
  return;
}


//-------------------------------------------------------------------


D 3
void scaler_to_file(long event[]) {
E 3
I 3
void scaler_evt_to_file(long event[]) {
E 3
D 9

E 9
I 9
  
E 9
  char *name, *p;
  char localname[20];
  int nbnk,ncols,nrows,nwrds;
  time_t now=time(NULL);
  int cnt;

  int ind     = 11;
  int evlen   = event[10] + 11;
  int run     = event[5];
  strstream fname;


  // open output file
  fname.fill('0');
D 13
  fname << getenv("CLON_PARMS") << "/scalers/archive/dd2scaler_" << session << "_" 
E 13
I 13
  fname << getenv("CLON_PARMS") << "/scalers/archive/scalers_" << session << "_" 
E 13
	<< setw(6) << run << ".txt" << ends;
  ofstream file(fname.str());
D 22
  if(file.bad())return;
E 22
I 22
  if(!file.is_open())return;
E 22
  file << "Scaler event dump by scaler_server at " << ctime(&now) << endl << endl;


  // dump all banks in event
  while (ind<evlen) {
    
    name  = (char *)&event[ind+1];
    nbnk  = event[ind+3];
    ncols = event[ind+4];
    nrows = event[ind+5];
    nwrds = event[ind+8];


    // bombproof against empty event
    if(nwrds<=0)break;


    // index of 1st data word
    ind  += event[ind];
    

    // print bank assuming all words are B32
    sprintf(localname,"%c%c%c%c%02d",name[0],name[1],name[2],name[3],nbnk);
    file << "\n\n*" << localname << "*" << endl;
    for(int i=0; i<nwrds; i+=8) {
D 31
      for(int j=i; j<min(i+8,nwrds); j++) {
E 31
I 31
      for(int j=i; j<MIN(i+8,nwrds); j++) {
E 31
	file << setw(10) << event[ind+j] << setw(1) << " ";
      }
      file << endl;
    }
    file << "*EOD*" << endl;


    // write out TRGD info if current bank is TRGS
D 27
    if(strncasecmp(name,"TRGS",4)==0) {
E 27
I 27
    if((strncasecmp(name,"TRGS",4)==0)&&(nbnk==0)) {
E 27
      file << "\n\n*TRGD00*" << endl;
D 6
      for(int i=0; i<64; i+=8) {
	for(int j=i; j<min(i+8,64); j++) {
E 6
I 6
D 10
      for(int i=0; i<80; i+=8) {
	for(int j=i; j<min(i+8,80); j++) {
E 10
I 10
D 14
      for(int i=0; i<sizeof(delta_trgs)/sizeof(unsigned long); i+=8) {
	for(int j=i; j<min(i+8,sizeof(delta_trgs)/sizeof(unsigned long)); j++) {
E 10
E 6
	  file << setw(10) << delta_trgs[ind+j] << setw(1) << " ";
E 14
I 14
      for(int i=0; i<nwrds; i+=8) {
D 31
	for(int j=i; j<min(i+8,nwrds); j++) {
E 31
I 31
	for(int j=i; j<MIN(i+8,nwrds); j++) {
E 31
	  file << setw(10) << delta_trgs[j] << setw(1) << " ";
I 17
	}
	file << endl;
      }
      file << "*EOD*" << endl;
    }


    // write out S1SD info if current bank is S1ST
    if(strncasecmp(name,"S1ST",4)==0) {
      file << "\n\n*S1SD00*" << endl;
      for(int i=0; i<nwrds; i+=8) {
D 31
	for(int j=i; j<min(i+8,nwrds); j++) {
E 31
I 31
	for(int j=i; j<MIN(i+8,nwrds); j++) {
E 31
	  file << setw(10) << delta_s1st[j] << setw(1) << " ";
E 17
E 14
	}
	file << endl;
      }
      file << "*EOD*" << endl;
    }


    // next bank
    ind += nwrds;
  }
  

  file << endl << endl;
  file.close();
  return;
}


//-------------------------------------------------------------------


void get_hardware_scalers(void) {

  int i,j,k,l,ind;
  char line[256];


  if(no_info==1)return;


  // create scaler message
D 20
  TipcMsg msg("info_server");
D 14
  msg.Dest("info_server/in/scaler_server");
E 14
I 14
  msg.Dest("info_server/in/scaler_server/hardware");
E 14
  msg << "scaler_server";
E 20
I 20
  TipcMsg msg((T_STR)"info_server");
  msg.Dest((T_STR)"info_server/in/scaler_server/hardware");
  msg << (T_STR)"scaler_server";
E 20


D 3
  // scaler1
E 3
I 3
  // scaler1 (trgs)
E 3
D 20
  DP_cmd("scaler1","exec read_scaler1",temp,4);
E 20
I 20
D 29
  DP_cmd((char*)"scaler1",(char*)"exec read_scaler1",temp,4);
E 29
I 29
  tcpClientCmd("scaler1","read_scaler1",temp);
E 29
E 20
  istrstream sc1(temp,sizeof(temp));
  if(find_tag_line(sc1,"*TRGS*",line,sizeof(line))==0) {
D 6
    for(i=0; i<64; i+=8) {
E 6
I 6
D 10
    for(i=0; i<80; i+=8) {
E 10
I 10
    for(i=0; i<sizeof(trgs)/sizeof(unsigned long); i+=8) {
E 10
E 6
      get_next_line(sc1,line,sizeof(line));
      l=0; ind=0; for(k=0; k<8; k++) {l+=ind; sscanf(&line[l],"%lu %n",&trgs[k+i],&ind);}
D 3
      for(j=i; j<i+8; j++) {
	delta_trgs[j]=trgs[j]-last_trgs[j];
	last_trgs[j]=trgs[j];
      }
E 3
    }
  }
D 4
  msg << "TRGS00" << SetSize(64) << trgs;
E 4
I 4
D 6
  msg << "TRGS00" << SetSize(64) << (T_INT4*)trgs;
E 6
I 6
D 10
  msg << "TRGS00" << SetSize(80) << (T_INT4*)trgs;
E 10
I 10
D 20
  msg << "TRGS00" << SetSize(sizeof(trgs)/sizeof(unsigned long)) << (T_INT4*)trgs;
E 20
I 20
  msg << (T_STR)"TRGS00" << SetSize(sizeof(trgs)/sizeof(unsigned long)) << (T_INT4*)trgs;
E 20
E 10
E 6
E 4
D 3
  msg << "TRGD00" << SetSize(64) << delta_trgs;
E 3


D 3
  // scaler2
  // scaler3
E 3
I 3
  // scaler2 (ecs)
D 20
  DP_cmd("scaler2","exec read_scaler2",temp,4);
E 20
I 20
D 29
  DP_cmd((char*)"scaler2",(char*)"exec read_scaler2",temp,4);
E 29
I 29
  tcpClientCmd("scaler2","read_scaler2",temp);
E 29
E 20
  istrstream sc2(temp,sizeof(temp));
  if(find_tag_line(sc2,"*EC*",line,sizeof(line))==0) {
    for(i=0; i<sizeof(ecs)/sizeof(long); i+=8) {
      get_next_line(sc2,line,sizeof(line));
      l=0; ind=0; for(k=0; k<8; k++) {l+=ind; sscanf(&line[l],"%lu %n",&ecs[k+i],&ind);}
    }
  }
D 4
  msg << "ECS 00" << SetSize(sizeof(ecs)/sizeof(long)) << ecs;
E 4
I 4
D 5
  msg << "ECS 00" << SetSize(sizeof(ecs)/sizeof(long)) << (T_INT4*)ecs;
E 5
I 5
D 20
  msg << "ECS_00" << SetSize(sizeof(ecs)/sizeof(long)) << (T_INT4*)ecs;
E 20
I 20
  msg << (T_STR)"ECS_00" << SetSize(sizeof(ecs)/sizeof(long)) << (T_INT4*)ecs;
E 20
E 5
E 4


  // scaler2 (scs)
  if(find_tag_line(sc2,"*SC*",line,sizeof(line))==0) {
    for(i=0; i<sizeof(scs)/sizeof(long); i+=8) {
      get_next_line(sc2,line,sizeof(line));
      l=0; ind=0; for(k=0; k<8; k++) {l+=ind; sscanf(&line[l],"%lu %n",&scs[k+i],&ind);}
    }
  }
D 4
  msg << "SCS 00" << SetSize(sizeof(scs)/sizeof(long)) << scs;
E 4
I 4
D 5
  msg << "SCS 00" << SetSize(sizeof(scs)/sizeof(long)) << (T_INT4*)scs;
E 5
I 5
D 20
  msg << "SCS_00" << SetSize(sizeof(scs)/sizeof(long)) << (T_INT4*)scs;
E 20
I 20
  msg << (T_STR)"SCS_00" << SetSize(sizeof(scs)/sizeof(long)) << (T_INT4*)scs;
E 20
E 5
E 4


  // scaler3 (sts)
D 20
  DP_cmd("scaler3","exec read_scaler3",temp,4);
E 20
I 20
D 29
  DP_cmd((char*)"scaler3",(char*)"exec read_scaler3",temp,4);
E 29
I 29
  tcpClientCmd("scaler3","read_scaler3",temp);
E 29
E 20
  istrstream sc3(temp,sizeof(temp));
  if(find_tag_line(sc3,"*ST*",line,sizeof(line))==0) {
    for(i=0; i<sizeof(sts)/sizeof(long); i+=8) {
      get_next_line(sc3,line,sizeof(line));
      l=0; ind=0; for(k=0; k<8; k++) {l+=ind; sscanf(&line[l],"%lu %n",&sts[k+i],&ind);}
    }
  }
D 4
  msg << "STS 00" << SetSize(sizeof(sts)/sizeof(long)) << ecs;
E 4
I 4
D 5
  msg << "STS 00" << SetSize(sizeof(sts)/sizeof(long)) << (T_INT4*)sts;
E 5
I 5
D 20
  msg << "STS_00" << SetSize(sizeof(sts)/sizeof(long)) << (T_INT4*)sts;
E 20
I 20
  msg << (T_STR)"STS_00" << SetSize(sizeof(sts)/sizeof(long)) << (T_INT4*)sts;
E 20
E 5
E 4
E 3


  // send and flush message
  server.Send(msg,TRUE);
  server.Flush();
  
I 3

  // write to file
  hardware_scaler_to_file();

  return;
}


//-------------------------------------------------------------------


void hardware_scaler_to_file() {


  time_t now=time(NULL);
  strstream fname;
  int i,j;


  // open output file
  fname.fill('0');
  fname << getenv("CLON_PARMS") << "/scalers/scaler_server_" << session	<< ".txt" << ends;
  ofstream file(fname.str());
D 22
  if(file.bad())return;
E 22
I 22
  if(!file.is_open())return;
E 22
  file << "Hardware scaler dump by scaler_server at " << ctime(&now) << endl << endl;


  file << "\n\n*TRGS00*" << endl;
  for(i=0; i<sizeof(trgs)/sizeof(long); i+=8) {
D 31
    for(j=i; j<min(i+8,sizeof(trgs)/sizeof(long)); j++) {
E 31
I 31
    for(j=i; j<MIN(i+8,sizeof(trgs)/sizeof(long)); j++) {
E 31
      file << setw(10) << trgs[j] << setw(1) << " ";
    }
    file << endl;
  }
  file << "*EOD*" << endl;
  
  
  file << "\n\n*ECS 00*" << endl;
  for(i=0; i<sizeof(ecs)/sizeof(long); i+=8) {
D 31
    for(j=i; j<min(i+8,sizeof(ecs)/sizeof(long)); j++) {
E 31
I 31
    for(j=i; j<MIN(i+8,sizeof(ecs)/sizeof(long)); j++) {
E 31
      file << setw(10) << ecs[j] << setw(1) << " ";
    }
    file << endl;
  }
  file << "*EOD*" << endl;
  
  
  file << "\n\n*SCS 00*" << endl;
  for(i=0; i<sizeof(scs)/sizeof(long); i+=8) {
D 31
    for(j=i; j<min(i+8,sizeof(scs)/sizeof(long)); j++) {
E 31
I 31
    for(j=i; j<MIN(i+8,sizeof(scs)/sizeof(long)); j++) {
E 31
      file << setw(10) << scs[j] << setw(1) << " ";
    }
    file << endl;
  }
  file << "*EOD*" << endl;
  

  file << "\n\n*STS 00*" << endl;
  for(i=0; i<sizeof(sts)/sizeof(long); i+=8) {
D 31
    for(j=i; j<min(i+8,sizeof(sts)/sizeof(long)); j++) {
E 31
I 31
    for(j=i; j<MIN(i+8,sizeof(sts)/sizeof(long)); j++) {
E 31
      file << setw(10) << sts[j] << setw(1) << " ";
    }
    file << endl;
  }
  file << "*EOD*" << endl;


  file << endl << endl;
  file.close();
E 3
  return;
}


//-------------------------------------------------------------------


void status_poll_callback(T_IPC_MSG msg) {

  char *t;


D 20
  TipcMsgAppendStr(msg,"Session");
E 20
I 20
  TipcMsgAppendStr(msg,(T_STR)"Session");
E 20
  TipcMsgAppendStr(msg,session);

D 20
  TipcMsgAppendStr(msg,"Number of events received");
E 20
I 20
  TipcMsgAppendStr(msg,(T_STR)"Number of events received");
E 20
  TipcMsgAppendInt4(msg,nrec);

D 20
  TipcMsgAppendStr(msg,"Number of scaler events processed");
E 20
I 20
  TipcMsgAppendStr(msg,(T_STR)"Number of scaler events processed");
E 20
  TipcMsgAppendInt4(msg,nevent);

D 20
  TipcMsgAppendStr(msg,"Current run");
E 20
I 20
  TipcMsgAppendStr(msg,(T_STR)"Current run");
E 20
  TipcMsgAppendInt4(msg,current_run);

D 20
  TipcMsgAppendStr(msg,"Number events this run");
E 20
I 20
  TipcMsgAppendStr(msg,(T_STR)"Number events this run");
E 20
  TipcMsgAppendInt4(msg,nevt_run);

D 20
  TipcMsgAppendStr(msg,"Total bad bank count");
E 20
I 20
  TipcMsgAppendStr(msg,(T_STR)"Total bad bank count");
E 20
  TipcMsgAppendInt4(msg,total_bad_bank_count);

D 20
  TipcMsgAppendStr(msg,"Run bad bank count");
E 20
I 20
  TipcMsgAppendStr(msg,(T_STR)"Run bad bank count");
E 20
  TipcMsgAppendInt4(msg,bad_bank_count);

D 20
  TipcMsgAppendStr(msg,"Livetime by triggers");
E 20
I 20
  TipcMsgAppendStr(msg,(T_STR)"Livetime by triggers");
E 20
  TipcMsgAppendReal8(msg,live_trig);

D 20
  TipcMsgAppendStr(msg,"Livetime by clock");
E 20
I 20
  TipcMsgAppendStr(msg,(T_STR)"Livetime by clock");
E 20
  TipcMsgAppendReal8(msg,live_clock);

D 20
  TipcMsgAppendStr(msg,"Livetime by fcup");
E 20
I 20
  TipcMsgAppendStr(msg,(T_STR)"Livetime by fcup");
E 20
  TipcMsgAppendReal8(msg,live_fcup);

I 14
D 20
  TipcMsgAppendStr(msg,"Livetime by Level1 OR");
E 20
I 20
  TipcMsgAppendStr(msg,(T_STR)"Livetime by Level1 OR");
E 20
  TipcMsgAppendReal8(msg,live_or);

E 14
D 20
  TipcMsgAppendStr(msg,"Livetime error count");
E 20
I 20
  TipcMsgAppendStr(msg,(T_STR)"Livetime error count");
E 20
  TipcMsgAppendInt4(msg,livetime_err_count);

  return;
}


//-------------------------------------------------------------------


void quit_callback(int sig) {

  done=1;
  return;
}


//----------------------------------------------------------------------


void T_ENTRY control_message_callback(T_IPC_CONN conn,
				      T_IPC_CONN_PROCESS_CB_DATA data,
				      T_CB_ARG arg) {

  T_STR string;


  // get first string
  TipcMsgSetCurrent(data->msg,0);
  TipcMsgNextStr(data->msg,&string);
  
  
  //  evt status request
  if(strcasecmp(string,"evt_status_poll")==0) {

    T_STR srvnode = server.Node();

D 20
    TipcMsg status("evt_status");
    status.Dest("/evt_system/status");
E 20
I 20
    TipcMsg status((T_STR)"evt_status");
    status.Dest((T_STR)"/evt_system/status");
E 20
    status.Sender(unique_id);
I 14
    sprintf(temp,"et:%s",et_station_name);
E 14
    status << unique_id << host << session << srvnode 
	   << (T_INT4) nrec << rec_rate << (T_INT4) nevent << proc_rate
D 14
	   << (T_INT4) -1 << "IPC";
E 14
I 14
	   << (T_INT4) et_ok << temp;
E 14
    server.Send(status,TRUE);
    server.Flush();


  //  don't understand message...ship to smartsockets interpreter
  } else {
    TutCommandParseStr(string);
  }

  return;
}


//----------------------------------------------------------------------


void init_tcl() {


  // link c and Tcl variables
D 20
  Tcl_LinkVar(interp,"application",     (char *)&application,     TCL_LINK_STRING);
  Tcl_LinkVar(interp,"unique_id",       (char *)&unique_id,       TCL_LINK_STRING);
  Tcl_LinkVar(interp,"session",         (char *)&session,         TCL_LINK_STRING);
D 19
  Tcl_LinkVar(interp,"scaler_reaout_time",(char *)&scaler_readout_time,TCL_LINK_INT);
E 19
I 19
  Tcl_LinkVar(interp,"scaler_readout_time",(char *)&scaler_readout_time,TCL_LINK_INT);
E 19
  Tcl_LinkVar(interp,"check_time",      (char *)&check_time,      TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp,"debug",           (char *)&debug, 	  TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,"no_file",         (char *)&no_file, 	  TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,"no_info",         (char *)&no_info, 	  TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,"no_fcup",         (char *)&no_fcup, 	  TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,"min_evt_live",    (char *)&min_evt_live, 	  TCL_LINK_INT);
  Tcl_LinkVar(interp,"live_tol",        (char *)&live_tol, 	  TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp,"nevent", 		(char *)&nevent,   	  TCL_LINK_INT|TCL_LINK_READ_ONLY);
I 19
  Tcl_LinkVar(interp,"nrec", 		(char *)&nrec,   	  TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 19
  Tcl_LinkVar(interp,"nevt_run",	(char *)&nevt_run,   	  TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"total_bad_bank_count",(char *)&total_bad_bank_count,TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"bad_bank_count",  (char *)&bad_bank_count,  TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"live_trig",       (char *)&live_trig,   	  TCL_LINK_DOUBLE|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"live_clock",      (char *)&live_clock,   	  TCL_LINK_DOUBLE|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"live_fcup",       (char *)&live_fcup,   	  TCL_LINK_DOUBLE|TCL_LINK_READ_ONLY);
I 14
  Tcl_LinkVar(interp,"live_or",         (char *)&live_or,   	  TCL_LINK_DOUBLE|TCL_LINK_READ_ONLY);
E 14
  Tcl_LinkVar(interp,"livetime_err_count",(char *)&livetime_err_count,TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 20
I 20
  Tcl_LinkVar(interp,(char*)"application",     (char *)&application,     TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"unique_id",       (char *)&unique_id,       TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"session",         (char *)&session,         TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"scaler_reaout_time",(char *)&scaler_readout_time,TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"check_time",      (char *)&check_time,      TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp,(char*)"debug",           (char *)&debug, 	  TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"no_file",         (char *)&no_file, 	  TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"no_info",         (char *)&no_info, 	  TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"no_fcup",         (char *)&no_fcup, 	  TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"min_evt_live",    (char *)&min_evt_live, 	  TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"live_tol",        (char *)&live_tol, 	  TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp,(char*)"nevent", 	       (char *)&nevent,   	  TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nevt_run",	       (char *)&nevt_run,         TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"total_bad_bank_count",(char *)&total_bad_bank_count,TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"bad_bank_count",  (char *)&bad_bank_count,   TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"live_trig",       (char *)&live_trig,   	  TCL_LINK_DOUBLE|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"live_clock",      (char *)&live_clock,   	  TCL_LINK_DOUBLE|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"live_fcup",       (char *)&live_fcup,   	  TCL_LINK_DOUBLE|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"live_or",         (char *)&live_or,   	  TCL_LINK_DOUBLE|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"livetime_err_count",(char *)&livetime_err_count,TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 20
    
  return;
}


//--------------------------------------------------------------------------


long *find_next_bank(long event[], char *bank, int &banknum, 
			      int &pos, int &nwrds) {

  // This searches through the incoming event from the current position,
  // looking for the next non-HEAD bank in the event.  If it finds one,
  // it returns the name of the bank as a 4 element string, a pointer
  // to the data in the bank and the size of the data.
  //
  // The function will return a pointer to the data if it finds a bank, 
  // 0 if it reaches the end of the event without finding another bank.
  //
  // SPECIAL INPUT:  If the position (pos) is zero, this signals the
  // beginning of processing for a new event and the event size variable
  // will be updated.
  //
  // Note: nwrds is assumed not to change between invocations of this
  // function!
  

  int ncols,nrows,ind,evlen;
  char *name;
  

  // reset pointers and counters if new event
  evlen=event[10]+11;
  if(pos<=0) ind=11; else ind=pos+nwrds;
  

  // loop through remaining banks
  while (ind<evlen) {
    name    = (char *)&event[ind+1];
    banknum = event[ind+3];
    ncols   = event[ind+4];
    nrows   = event[ind+5];
    nwrds   = event[ind+8];

    // bombproof against empty events
    if(nwrds<=0)break;

    // index of 1st data word in bank
    ind+=event[ind];
    
    // skip HEAD bank, otherwise form bank name, set pointers, and return
    if (strncmp(name,"HEAD",4)==0) {
      ind += nwrds;
    } else {
      while(*name!=' ') *bank++ = *name++; 
      *bank = '\0';
      pos = ind;		// keep the position
      return (long *)&event[ind];
    }

  }


  // no more banks
  return (long *)0;
}


//-------------------------------------------------------------------


void decode_command_line(int argc, char**argv) {

D 20
  char *help = 
E 20
I 20
  const char *help = 
E 20
D 3
    "\nusage:\n\n  scaler_server [-a application] [-s session] [-u uniq_name] [-live live_tol]"
E 3
I 3
    "\nusage:\n\n  scaler_server [-a application] [-s session] [-u uniq_name] [-live live_tol]\n"
E 3
    "[-scr scaler_readout_time] [-c check_time] [-debug] [-no_file] [-no_info] [-no_fcup] [-min min_evt_live]\n";
    
    
    
  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while (i<argc) 
  {
    if (strncasecmp(argv[i],"-h",2)==0)
    {
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-live",5)==0) {
      live_tol=atof(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-no_file",8)==0) {
      no_file=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_info",8)==0) {
      no_info=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_fcup",8)==0) {
      no_fcup=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-min",4)==0) {
      min_evt_live=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-scr",4)==0) {
      scaler_readout_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0) {
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0) {
      uniq_name=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-t",2)==0) {
      init_tcl_script=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-c",2)==0) {
      sscanf(argv[i+1],"%f",check_time);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-",1)==0) {
      cout << "Unknown command line arg: " << argv[i] 
	   << argv[i+1] << endl << endl;
      i=i+2;
    }
  }

  return;
}

  
//----------------------------------------------------------------










E 1
