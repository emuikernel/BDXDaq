h13782
s 00011/00004/00371
d D 1.20 03/12/16 22:48:37 boiarino 21 20
c .
e
s 00037/00005/00338
d D 1.19 02/07/09 11:23:13 wolin 20 19
c May be working...
e
s 00032/00022/00311
d D 1.18 00/08/29 16:57:56 wolin 19 18
c New CC
e
s 00018/00012/00315
d D 1.17 00/01/12 17:21:20 wolin 18 17
c Ignoring transition failed coda messages
e
s 00071/00393/00256
d D 1.16 99/05/19 16:08:16 wolin 17 16
c Updated to use ipc alarm system
c 
e
s 00007/00001/00642
d D 1.15 99/04/29 14:09:15 wolin 16 15
c Now dies if cmlog server dies
c 
e
s 00001/00001/00642
d D 1.14 99/03/01 16:26:22 wolin 15 14
c Forgot to set DISPLAY
c 
e
s 00011/00009/00632
d D 1.13 99/02/24 12:36:27 wolin 14 13
c Minor mods, some debug code added
c 
e
s 00005/00016/00636
d D 1.12 99/02/05 09:57:05 wolin 13 12
c Modified alarm delay, removed SEVERE SYNC ERROR
c 
e
s 00003/00003/00649
d D 1.11 99/02/04 13:50:09 wolin 12 11
c Added RCS severe error
c 
e
s 00021/00012/00631
d D 1.10 99/01/28 14:53:06 wolin 11 10
c Added 3 min delay between alarm_windows
c 
e
s 00010/00001/00633
d D 1.9 99/01/28 14:10:49 wolin 10 9
c Added RCS alarm, switched alarm_window to clon03
c 
e
s 00014/00009/00620
d D 1.8 98/10/28 09:26:18 wolin 9 8
c Redid sync alarm
c 
e
s 00179/00181/00450
d D 1.7 98/10/21 09:44:07 wolin 8 7
c Removed log file
c 
e
s 00015/00008/00616
d D 1.6 98/10/20 09:29:12 wolin 7 6
c Ready for deployment
c 
e
s 00198/00116/00426
d D 1.5 98/10/19 15:23:12 wolin 6 5
c Almost done adding code sync error alarm_window
c 
e
s 00143/00133/00399
d D 1.4 98/04/23 10:09:51 wolin 5 4
c Many improvements, updates in preparation for audio alarms
c 
e
s 00124/00150/00408
d D 1.3 97/04/08 16:51:09 wolin 4 3
c Now cmlog and cmlogBrowser enabled
e
s 00000/00000/00558
d D 1.2 97/02/19 12:02:07 wolin 3 1
c Minor mods
e
s 00000/00000/00000
d R 1.2 97/02/19 09:04:43 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 alarm_monitor/s/alarm_handler.cc
c Name history : 1 0 s/alarm_monitor.cc
e
s 00558/00000/00000
d D 1.1 97/02/19 09:04:42 wolin 1 0
c Monitors alarm conditions and takes corrective action
e
u
U
f e 0
t
T
I 1
// 
D 6
//  alarm_monitor
E 6
I 6
//  alarm_handler
I 17
// 
//  handles ipc alarms
E 17
E 6
//
D 17
//  steering program for responding to clas, coda, and epics alarms
I 8
//  alarm handling functions at end of this file
E 8
//
D 4
//  NOTE:
//     must run on clon01 in order to use channel access to talk to epics
//     
E 4
D 6
//
E 6
//  Still to do:
D 4
//     add real epics alarms (may need new epics record to manage many channels at once)
//     add real coda alarms...need callback for RCS shutdown,startup
//     add real msql alarms
E 4
I 4
D 9
//     add real alarms
E 9
I 9
//     add real alarms, count alarms
E 9
E 4
//
D 4
//  ejw, 18-feb-97
E 4
I 4
D 6
//
//  ejw, 12-mar-97
E 6
I 6
//  ejw, 19-oct-98
E 17
I 17
//  ejw, 18-may-99
I 20
//
//  still to do:
//    revamp error handling in conjunction with Dave A.
E 20
E 17
E 6
E 4


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for smartsockets
#include <rtworks/cxxipc.hxx>


D 8
// for i/o
E 8
I 8
// system
E 8
#include <fstream.h>
#include <iomanip.h>
D 8


D 4
// system stuff
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <macros.h>
#include <stdarg.h>
#include <stropts.h>
#include <poll.h>
E 4
I 4
// for strings
E 8
D 17
#include <strstream.h>
E 17
E 4

I 4

E 4
// for tcl
extern "C" {
#include <tcl.h>
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


// for CLAS ipc
#include <clas_ipc_prototypes.h>


D 4
// for cdev
E 4
I 4
D 17
// for cdev and cmlog
E 4
#include <cdev.h>
#include <cdevData.h>
#include <cdevDevice.h>
#include <cdevRequestObject.h>
#include <cdevSystem.h>
I 4
#include <cmlogBrowser.h>
E 4


E 17
D 4
// for msql
#include <msql.h>


E 4
// misc variables
D 5
char *application      = "clastest";
char *unique_id        = "alarm_monitor";
D 4
char *session          = "ejw";
E 4
Tcl_Interp *interp;  
char *init_tcl_script  = NULL;
int done               = 0;
char temp[256];
D 4
static int msqlindex;
E 4
I 4
strstream temp1;
E 5
I 5
D 19
static char *application      = "clastest";
D 17
static char *session          = getenv("DD_NAME");
E 17
D 6
static char *unique_id        = "alarm_monitor";
E 6
I 6
static char *unique_id        = "alarm_handler";
E 19
I 19
static char *application      = (char *)"clastest";
static char *unique_id        = (char *)"alarm_handler";
E 19
D 14
static char *window           = getenv("DISPLAY");
E 14
D 8
static ofstream logfile;
E 8
E 6
static Tcl_Interp *interp;  
static char *init_tcl_script  = NULL;
I 17
D 20
static int rcs_deadtime       = 90;  // seconds
E 20
I 20
static int rcs_deadtime       = 120;   // seconds
static int eb1_deadtime       = 120;  // seconds
E 20
static int nmsg               = 0;
static int nmsg_ignored       = 0;
static int nmsg_handled       = 0;
E 17
static int done               = 0;
D 6
static time_t start           = time(NULL);
static char temp[256];
E 6
I 6
D 17
int debug                     = 0;
static time_t now             = time(NULL);
E 17
I 17
static int debug              = 0;
E 17
static char temp[512];
I 9
D 17
static int nalarm             = 0;
E 9
E 6
D 8
static strstream temp1;
E 8
E 5
E 4

E 17
I 17
static time_t last_rcs        = 0;
I 20
static time_t last_eb1        = 0;
E 20
E 17

I 13
D 17
// alarm times
static time_t last_rcs=0;


E 13
I 5
D 6
// also used in callback functions
int debug      = 0;


E 6
E 5
// file descriptors for services registered with the cdev system object
static int ipcfd;
D 4
static int msqlfd;
E 4
I 4
static cdevUserFdCbkId ipcid;
static int cmlogfd;
static cdevUserFdCbkId cmlogid;
E 17
E 4

I 5

E 5
D 4

// msql daemon and database variables
static char *msqlhost               = "clon00";
static char *msqldb                 = "clasmsg";
static char *msqlinfotbl            = "clasinfo";
static char *msqlerrtbl             = "claserr";
static char *msqlstattbl            = "classtat";
static char *msqlmhost              = "clon";
static char *msqlfacility           = "online";
static int msqldaemon               = 2;
static int msqlportbase             = 8100;

E 4
D 6
// output log file
D 5
ofstream logfile;
E 5
I 5
static ofstream logfile;
E 5


E 6
D 8
// prototypes for callbacks
E 8
I 8
// prototypes
void decode_command_line(int argc, char **argv);
void init_tcl();
D 17
void init_ipc_callbacks();
void init_cmlog_callbacks();
void init_epics_callbacks();
void init_coda_callbacks();
E 8
D 5
#include <alarm_callback_prototypes.h>
E 5
I 5
void epics_callback_func(int status, void *userarg, cdevRequestObject &myreqobj,
			    cdevData& result);
void coda_callback_func(int status, void *userarg, cdevRequestObject &myreqobj,
			    cdevData& result);
D 6
void cmlog_callback_func (int status, void* arg, cmlogPacket* data);
E 6
I 6
void cmlog_callback_func(int status, void* arg, cmlogPacket* data);
D 7
void handle_cmlog_alarms(long msgtime, char *facility, char *host, char *user, char *process,
			 char *msgclass, int severity, char *msgstatus, int code, char *text);
E 7
I 7
D 8
void handle_cmlog_alarms(long msgtime, char *name, char *facility, char *host, char *user, 
E 8
I 8
void handle_cmlog_alarms(int debug, long msgtime, char *name, char *facility, char *host, char *user, 
E 8
			 char *process, char *msgclass, int severity, char *msgstatus, int code, 
			 char *text);
E 17
I 17
void cmlog_callback(T_IPC_CONN,
		    T_IPC_CONN_PROCESS_CB_DATA,
		    T_CB_ARG);
I 19
extern "C" {
E 19
E 17
E 7
E 6
E 5
D 8


// other prototypes
void decode_command_line(int argc, char **argv);
void init_tcl();
I 4
void init_ipc_callbacks();
void init_cmlog_callbacks();
E 4
void init_epics_callbacks();
D 4
void init_msql_callbacks();
E 4
void init_coda_callbacks();
E 8
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
D 6
void alarm_monitor_done(void);
E 6
D 4
void ipc_mainloop(int poll_time);
E 4
I 4
D 17
int ipc_mainloop(int opened, int fd, void *arg);
int cmlog_mainloop(int opened, int fd, void *arg);
E 17
E 4
int tcl_help(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
int tcl_quit(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
D 4
extern "C"{
int msqlRegister(int fd, char *tbl);
int eMsgLog(char *, char *, char *, char *, char *, char *,
	char *, char *, int, int);
}
E 4
I 4
D 19
extern "C" {
E 19
D 5
int insert_msg(char *name, char *facility, char *process, char *msgclass, char *severity, 
                   int code, char *message);
E 5
I 5
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
D 6
	       int severity, char *status, int code, char *text);
E 6
I 6
D 17
	      int severity, char *status, int code, char *message);
E 17
I 17
	      int severity, char *msgstatus, int code, char *text);
E 17
E 6
E 5
}	
E 4


I 4
D 5

// global objects, etc.

E 5
E 4
// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();

I 5

E 5
D 17
// ref to cdev system object
cdevSystem &cdevsys = cdevSystem::defaultSystem ();

I 5

E 5
I 4
// cmlog browser
cmlogBrowser browser;
E 4

I 4
D 5
// session name is same as dd name
char *session          = getenv("DD_NAME");
E 5

E 17
D 5

E 5
E 4
//--------------------------------------------------------------------------


D 6
main(int argc,char **argv){
E 6
I 6
main(int argc,char **argv) {
E 6

D 4
  int status,err;
E 4
I 4
  int status;
E 4
D 5
  time_t start=time(NULL);
E 5


D 5
  // decode command line...flags may be overridden in Tcl startup script
E 5
I 5
  // synch with c i/o
  ios::sync_with_stdio();


D 17
  // only print cdev error messages
  cdevsys.setThreshold(CDEV_SEVERITY_ERROR);


E 17
  // decode command line flags
E 5
  decode_command_line(argc,argv);


I 5
D 8
  // get session/DD name for talking to rcServer
E 8
I 8
D 17
  // session name
E 8
  if(session==NULL)session="clasprod";

  
E 17
E 5
  // get Tcl interp, create tcl commands, link vars, process Tcl startup script, etc.
  interp=tclinterp_init();
  init_tcl();
  if(init_tcl_script!=NULL)Tcl_EvalFile(interp,init_tcl_script);


  // set ipc parameters and connect to ipc system
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
D 9
  status=ipc_init(unique_id,"alarm monitor");
E 9
I 9
D 17
  status=ipc_init(unique_id,"alarm handler");
E 17
I 17
D 19
  status=ipc_init(unique_id,"alarm_handler");
E 19
I 19
  status=ipc_init(unique_id,(char *)"alarm_handler");
E 19
E 17
E 9
D 6
  if(status<0){
E 6
I 6
  if(status<0) {
E 6
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
D 6
	 << "   ...check for another alarm_monitor  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
E 6
I 6
	 << "   ...check for another alarm_handler using ipc_info\n"
	 << "   ...only one instance allowed!" << endl << endl;
E 6
    exit(EXIT_FAILURE);
  }
I 17
D 19
  TipcMt mt("cmlog");
E 19
I 19
  TipcMt mt((char*)"cmlog");
E 19
  server.ProcessCbCreate(mt,cmlog_callback,0);
D 19
  server.SubjectSubscribe("cmlog",TRUE);
E 19
I 19
  server.SubjectSubscribe((char*)"cmlog",TRUE);
E 19
E 17


D 5
  // enable Tcl ipc control (i.e. create tcl_request callback)
E 5
I 5
D 6
  //  create tcl_request callback
E 6
I 6
D 8
  // tcl_request callback
E 8
I 8
  // enable tcl callback
E 8
E 6
E 5
  tclipc_init(interp);


I 5
D 6
  // set up all callbacks
E 6
I 6
D 8
  // other callbacks
E 8
I 8
D 17
  // create callbacks for alarm handling
E 8
E 6
  init_ipc_callbacks();
  init_cmlog_callbacks();
I 6
  init_epics_callbacks();
E 6
  // init_coda_callbacks();
D 6
  // init_epics_callbacks();
E 6


E 5
D 8
  // open log file and print startup information
D 4
  strcpy(temp,unique_id);
  strcat(temp,".log");
  logfile.open(temp,ios::out|ios::app);
E 4
I 4
D 5
  temp1 << unique_id << ".log" << ends;
E 5
I 5
D 6
  temp1 << getenv("CLON_PARMS") << "/alarm_monitor/" << unique_id << ".log" << ends;
E 6
I 6
  temp1 << getenv("CLON_PARMS") << "/alarm/" << unique_id << ".log" << ends;
E 6
E 5
  logfile.open(temp1.str(),ios::out|ios::app);
E 4
  logfile << endl << endl << unique_id << " starting in application: " << application 
E 8
I 8
  // log file, cmlog startup message
  strstream temps;
  temps << getenv("CLON_PARMS") << "/alarm/" << unique_id << ".log" << ends;
  cout << endl << endl << unique_id << " starting in application: " << application 
E 8
D 5
	  << " on " << ctime(&start);  
E 5
I 5
D 6
	  << " on " << ctime(&start) << flush;  
E 5
D 4
  logfile << "...connecting to coda session " << session << endl;
E 4


  // post startup message
E 6
I 6
	  << " on " << ctime(&now) << flush;  
E 6
  sprintf(temp,"Process startup:   %15s  in application:  %s",unique_id,application);
E 17
I 17
  // post startup message
  sprintf(temp,"Process startup:  %15s in application %s",unique_id,application);
E 17
D 4
  err=eMsgLog(msqlhost,msqldb,msqlstattbl,"alarm_monitor","START",temp,
	      msqlmhost,msqlfacility,0,msqldaemon);
E 4
I 4
D 5
  status=insert_msg("alarm_monitor","alarm_monitor",unique_id,"status","START",0,temp);
E 5
I 5
D 6
  status=insert_msg("alarm_monitor","alarm_monitor",unique_id,"status",0,"START",0,temp);
E 6
I 6
D 19
  status=insert_msg("alarm_handler","online",unique_id,"status",0,"START",0,temp);
E 19
I 19
  status=insert_msg((char*)"alarm_handler",(char*)"online",unique_id,(char*)"status",0,
		    (char*)"START",0,temp);
E 19
E 6
E 5
E 4


I 6
D 17

E 17
E 6
D 4
  // get and register ipc fd with cdev system object
  ipcfd=server.XtSource();
  // cdevsys.addUserFdCallback(ipcfd,ipc_mainloop,(char *)0);
E 4
D 5

I 4
  // only print cdev error messages
  cdevsys.setThreshold(CDEV_SEVERITY_ERROR);
E 4

D 4
  // setup callbacks
  init_msql_callbacks();
E 4
I 4

  // set up all callbacks
  init_ipc_callbacks();
  init_cmlog_callbacks();
E 4
  // init_coda_callbacks();
  // init_epics_callbacks();


D 4
  // debug...set up fds array for poll function
  pollfd *fds = (pollfd*) malloc(sizeof(pollfd));
  fds[0].fd=msqlfd;
  fds[0].events=POLLIN;



  // main loop...handle alarms, ipc messages, etc; stop when done flag set
E 4
I 4
  // main loop...handle alarms, ipc messages, etc;  stop when done flag set
E 4
  while (done==0){
D 4
    // cdevsys.pend(0.5);
    ipc_check(0.1);

    // debug...do poll loop here
     int stat=poll(fds,1,100);
     if(stat>0){
       if(fds[0].revents!=0){
	 msql_callback_func(msqlfd);
       }
     } else if (stat<0){
       cout << "error...poll status = " << stat << endl;
     }

E 4
I 4
    cdevsys.pend(0.5);
E 5
I 5
  //  handle alarms, ipc messages, etc
  while (done==0) {
D 17
    cdevsys.pend(1.0);
E 17
I 17
    server.MainLoop(1.0);
E 17
E 5
E 4
  }


I 6
D 17

E 17
E 6
  // done
D 6
  alarm_monitor_done();
E 6
I 6
D 8
  ipc_close();
E 8
D 17
  now=time(NULL);
D 8
  logfile << "stopping on " << ctime(&now) << endl;  
E 8
I 8
  cout << "stopping on " << ctime(&now) << endl;  
E 17
  ipc_close();
E 8
D 14
  sprintf(temp,"Process shutdown:  %15s",unique_id);
E 14
I 14
D 17
  sprintf(temp,"Process shutdown:  %15s  in: %s",unique_id,application);
E 17
I 17
  sprintf(temp,"Process shutdown:  %15s",unique_id);
E 17
E 14
D 19
  status=insert_msg("alarm_handler","online",unique_id,"status",0,"STOP",0,temp);
E 19
I 19
  status=insert_msg((char*)"alarm_handler",(char*)"online",unique_id,(char*)"status",0,
		    (char*)"STOP",0,temp);
E 19
I 8

E 8
E 6
  exit(EXIT_SUCCESS);
D 5

E 5
}
       

//--------------------------------------------------------------------------


I 4
D 5
void init_ipc_callbacks(){
E 5
I 5
D 17
void init_ipc_callbacks() {
E 5

  ipcfd=server.XtSource();
  cdevsys.addUserFdCallback(ipcfd,ipc_mainloop,(void *)0,ipcid);

  return;
}


//--------------------------------------------------------------------------


E 4
D 5
void init_epics_callbacks(){
E 5
I 5
D 6
int ipc_mainloop(int opened, int fd, void *arg){
E 6
I 6
D 8
int ipc_mainloop(int opened, int fd, void *arg) {
E 6
  
D 6
  if(opened==0){
    cerr << "Cdevsys unable to attach to ipc fd: " << fd << endl;
    return(-1);
E 6
I 6
  if(opened==0) {
    cerr << "cdevsys unable to attach to ipc fd: " << fd << endl;
    exit(EXIT_FAILURE);
E 6
E 5

D 5
  int status;

  // create callback for epics channels
  cdevCallback epics_ch1(epics_callback_func,(void*)1);
  cdevDevice &ch1 = cdevDevice::attachRef("B/hv_06_01_02_11");

  // cdevDevice &ch1 = cdevDevice::attachRef("cdev_ai0");
  status=ch1.sendCallback("monitorOn DVI", NULL, epics_ch1);

  // status=ch1.sendCallback("monitorOn", NULL, epics_ch1);
  if(status!=CDEV_SUCCESS){
    cout << "ch1.send error status is: " << status << endl;
    exit(EXIT_FAILURE);
E 5
I 5
  } else {
    server.MainLoop(0.0);
D 6
    return (0);
E 6
I 6
    return(0);
E 6
E 5
  }
D 5

  return;
E 5
}


D 5
//--------------------------------------------------------------------------
E 5
I 5
//-------------------------------------------------------------------
E 5


E 8
D 4
void init_msql_callbacks(){
E 4
I 4
D 5
void init_cmlog_callbacks(){
E 5
I 5
void init_cmlog_callbacks() {
E 17
I 17
void cmlog_callback(T_IPC_CONN conn,
		    T_IPC_CONN_PROCESS_CB_DATA data,
		    T_CB_ARG arg) {
E 17
E 5
E 4

D 4
  // get fd for activity in error table
  msqlindex=msqlConnectPort(msqlhost,msqlportbase+msqldaemon);
  msqlSelectDB(msqlindex,msqldb);
  msqlfd=msqlRegister(msqlindex,msqlerrtbl);
E 4
I 4
  int status;
D 17
  cdevData data;
E 17
I 17
  T_STR domain;
  T_STR host;
  T_STR user;
  T_INT4 msgtime;
E 17
E 4

I 17
  T_STR name;
  T_STR facility;
  T_STR process;
  T_STR msgclass;
  T_INT4 severity;
  T_STR msgstatus;
  T_INT4 code;
  T_STR text;
E 17
D 4
  // register fd and callback with cdev
  // cdevsys.addUserFdCallback(msqlfd,msql_callback,(char *)msqlfd);
E 4

I 4
D 17
  // connect to cmlog server
  status=browser.connect();
D 6
  if(status!=CMLOG_SUCCESS){
E 6
I 6
  if(status!=CMLOG_SUCCESS) {
E 6
    cerr << "\n  Unable to connect to browser, status is: " << status << endl << endl;
    exit(EXIT_FAILURE);
  }
  
E 17
I 17
  TipcMsg msg(data->msg);
  time_t now = time(NULL);
E 17
I 6

E 6
D 17
  // add cmlog to cdevsys pend loop
  cmlogfd=browser.getFd();
  cdevsys.addUserFdCallback(cmlogfd,cmlog_mainloop,(void *)0,cmlogid);
E 17

I 17
  nmsg++;
E 17

D 17
  // create browser callback
  status=browser.queryCallback ("monitorOn loggingData", data, cmlog_callback_func, 0);
  if(status!=CMLOG_SUCCESS) {
    cerr << "\n  Unable to create cmlog callback, status is: " << status << endl << endl;
    exit(EXIT_FAILURE);
  }
  
E 4
  return;
}


//--------------------------------------------------------------------------
E 17

I 17
  // unpack message
  msg >> domain >> host >> user >> msgtime
      >> name >> facility >> process >> msgclass 
      >> severity >> msgstatus >> code >> text;
E 17

I 5
D 6
int cmlog_mainloop(int opened, int fd, void *arg){
E 6
I 6
D 8
int cmlog_mainloop(int opened, int fd, void *arg) {
E 6
  
D 6
  if(opened==0){
    cerr << "Cdevsys unable to attach to cmlog fd: " << fd << endl;
    return(-1);
E 6
I 6
  if(opened==0) {
    cerr << "cdevsys unable to attach to cmlog fd: " << fd << endl;
    exit(EXIT_FAILURE);
E 8
I 8
D 17
void cmlog_callback_func (int status, void* arg, cmlogPacket* data) {
E 17
E 8
E 6

I 20
  // ignore non-clas user
  if((strncasecmp(user,"clas",4)!=0)) {
    nmsg_ignored++;
    return;
  }


I 21
  // remove newlines from text
  for(int i=0; i<strlen(text); i++) if(text[i]=='\n')text[i]='\0';


E 21
E 20
D 8
  } else {
    browser.pendIO(0.0);
    return (0);
E 8
I 8
D 17
  char name[20];
  char facility[20];
  char host[20];
  char user[20];
  char msgstatus[20];
  char process[20];
  char msgclass[20];
  int severity;
  long code;
  long msgtime;
  char text[1024];
  char atime[20];
  struct tm *tstruct;
  cdevData* res = NULL;


  // extract messages from packet
  if (data) {

    cmlogMsg** msgs = data->messages();
    for (int i=0; i<data->numberOfData(); i++) {

      cmlog_cdevMessage& idata = (cmlog_cdevMessage)(*msgs[i]);
      res=idata.getData();
      if (res) {
	res->get("cmlogTime", &msgtime);  
	res->get("name",      name,        sizeof(name));
	res->get("facility",  facility,    sizeof(facility));
	res->get("host",      host,        sizeof(host));
	res->get("user",      user,        sizeof(user));
	res->get("process",   process,     sizeof(process));
	res->get("class",     msgclass,    sizeof(msgclass));
 	res->get("severity",  &severity);
	res->get("status",    msgstatus,   sizeof(msgstatus));
	res->get("code",      &code);  
	res->get("text",      text,        sizeof(text));
	

	if((debug==1)&&(severity>=2)) {
	  tstruct=localtime(&msgtime);
	  strftime(atime,sizeof(atime),"%d-%h-%Y %H:%M",tstruct);
	  cout << setw(8) << facility << " " << setw(8) << host << " " 
	       << setw(8) << user << " " << setw(17) << atime << " " 
	       << setw(12) << process << " " << setw(8) << msgclass << " " 
	       << setw(1) << severity << " " << setw(6) << msgstatus << " " 
	       << setw(3) << code << " " << setw(0) << text << endl;
	}


	// handle alarm
	handle_cmlog_alarms(debug,msgtime,name,facility,host,user,process,msgclass,severity,
			    msgstatus,code,text);

      }
E 17
I 17
D 18
  // handle alarms
  if( ((severity>2) && (strcmp(msgclass,"RCS")==0)) ) {
    if((now-last_rcs)>rcs_deadtime) {
      nmsg_handled++;
      cout << "Handling SEVERE error from RCS " << ctime(&now);
      sprintf(temp,"/bin/csh -c \"(setenv DISPLAY clon03:0.0; "
	      "$CLON_BIN/alarm_window -w %s %s:  %s )&\"",
      	      "clon03:0.0",
	      "Run control server detected SEVERE error, run should be stopped",text);
      system(temp);
      last_rcs=now;
E 17
    }
E 18
I 18
  // handle alarms...ignore download,prestart,etc. failures
  if( (severity>2)                                  &&
      (strcmp(msgclass,"RCS")==0)                   && 
      (strncasecmp(text,"Boot failed",11)!=0)       &&
      (strncasecmp(text,"Download failed",15)!=0)   && 
      (strncasecmp(text,"Prestart failed",15)!=0)   && 
      (strncasecmp(text,"Go failed",9)!=0)          &&
      (strncasecmp(text,"End failed",10)!=0)        &&
      ((now-last_rcs)>rcs_deadtime) 
    ) {
    nmsg_handled++;
    cout << "Handling SEVERE error from RCS " << ctime(&now);
D 20
    sprintf(temp,"/bin/csh -c \"(setenv DISPLAY clon03:0.0; "
	    "$CLON_BIN/alarm_window -w %s %s:  %s )&\"",
	    "clon03:0.0",
	    "Run control server detected SEVERE error, run should be stopped",text);
E 20
I 20
D 21
    sprintf(temp,"$CLON_BIN/alarm_window -w clon03:0.0 %s:  %s &",
E 21
I 21
    sprintf(temp,"/bin/csh -c \'(setenv DISPLAY clon03:0.0; $CLON_BIN/alarm_window %s:  %s)\'&",
E 21
	    "SEVERE error in run control server, run must be stopped",text);
I 21
    cout << temp << endl;
E 21
E 20
    system(temp);
    last_rcs=now;
I 20


  // notify operator about any scaler1 error...EJW, 22-may-2002
  } else if((severity>=2)&&(strcasecmp(process,"scaler1")==0)) {
    nmsg_handled++;
    cout << "Handling ERROR from SCALER1 " << ctime(&now);
D 21
    sprintf(temp,"$CLON_BIN/alarm_window -w clon03:0.0 %s:  %s &",
	    "SCALER1 ERROR, run may need to be stopped",text);
E 21
I 21
    sprintf(temp,"/bin/csh -c \'(setenv DISPLAY clon03:0.0; $CLON_BIN/alarm_window %s:  %s)\'&",
	    "SCALER1 ERROR, crate should be rebooted",text);
    cout << temp << endl;
E 21
    system(temp);


  // notify operator about any EB error...EJW, 22-may-2002
  } else if( (severity>=2)                   &&
	     (strcasecmp(process,"EB1")==0)  &&
	     ((now-last_eb1)>eb1_deadtime)
	     ) {
    nmsg_handled++;
    cout << "Handling ERROR from EB1 " << ctime(&now);
D 21
    sprintf(temp,"$CLON_BIN/alarm_window -w clon03:0.0 %s:  %s &",
E 21
I 21
    sprintf(temp,"/bin/csh -c \'(setenv DISPLAY clon03:0.0; $CLON_BIN/alarm_window %s:  %s)\'&",
E 21
	    "EB1 ERROR, run must be stopped",text);
I 21
    cout << temp << endl;
E 21
    system(temp);
    last_eb1=now;
E 20
E 18

D 17
    // free memory
    for (i = 0; i < data->numberOfData (); i++) delete msgs[i];
    delete []msgs;
E 8
  }
I 8

  return;
E 8
}


D 8
//-------------------------------------------------------------------
E 8
I 8
//--------------------------------------------------------------------------
E 8


D 6
void init_epics_callbacks(){
E 6
I 6
void init_epics_callbacks() {
E 6

D 6
  int status;

  // create callback for epics channels
  cdevCallback epics_ch1(epics_callback_func,(void*)1);
  cdevDevice &ch1 = cdevDevice::attachRef("B/hv_06_01_02_11");
  status=ch1.sendCallback("monitorOn DVI", NULL, epics_ch1);
  if(status!=CDEV_SUCCESS){
    cout << "ch1.send error status is: " << status << endl;
    exit(EXIT_FAILURE);
  }

E 6
  return;
}


//--------------------------------------------------------------------------


I 8
void epics_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
			    cdevData& result){

  static int count = 0;

  count++;

  cout << endl;
  cout << "epics callback call number: " << setw(20) << count << endl;
  cout << "                    status: " << setw(20) << status << endl;
  cout << "                     myarg: " << setw(20) << (int)userarg<< endl;
  cout << "                    result: " << setw(20) << (double) result << endl;
  cout << endl;

}


//---------------------------------------------------------------------------


E 8
E 5
D 6
void init_coda_callbacks(){
E 6
I 6
void init_coda_callbacks() {
E 6

  int status;
  cdevData rcs,nsresult,result;


  // create directory device object
  cdevDevice& ns = cdevDevice::attachRef("cdevDirectory");

I 5

E 5
  // add run control session to list of directory services
D 4
  strcpy(temp,"RCS : ");
  strcat(temp,session);
  strcat(temp,";");
  cout << "value is: " << temp << endl;
  rcs.insert ("value",temp);
E 4
I 4
D 5
  temp1.seekp(0,ios::beg);
  temp1 << "RCS : " << session<< ";" << ends;
  rcs.insert ("value",temp1.str());
E 5
I 5
  sprintf(temp,"RCS : %s;",session);
  rcs.insert ("value",temp);
E 5
E 4
  status=ns.send("update", rcs, nsresult);
D 6
  if(status!=CDEV_SUCCESS){
E 6
I 6
  if(status!=CDEV_SUCCESS) {
E 6
    cout << "ns.send error status is: " << status << endl;
    exit(EXIT_FAILURE);
I 4
  }
D 8
  logfile << "    ...database now knows about coda session: " << session << endl;
E 8
I 8
  cout << "    ...database now knows about coda session: " << session << endl;
E 8


  // get ref to run control server device
  cdevDevice &rcsd = cdevDevice::attachRef(session);

I 5

E 5
  // talk to RCS via device
  status=rcsd.send("get runNumber",NULL,result);
D 6
  if(status!=CDEV_SUCCESS){
E 6
I 6
  if(status!=CDEV_SUCCESS) {
E 6
    cerr << "rcsd.send error status is: " << status << endl;
    cerr << "couldn't get run number using device: " << rcsd.name() << endl;
E 17
E 4
  } else {
D 4
    cout << "ns.send worked" << endl;
E 4
I 4
D 5
    cout << "run number using device is: " << (int) result << endl;
E 5
I 5
D 17
    cout << "run number is: " << (int) result << endl;
E 17
I 17
    nmsg_ignored++;
E 17
E 5
E 4
  }
D 17

I 4
D 5
  /*
E 5
E 4

D 5
  // create pointer to request object for run number
  cdevRequestObject *req = cdevRequestObject::attachPtr(session,"get runNumber");
  if(!req){
    cout << "...error creating pointer to RCS request object for get runNumber" << endl;
    return;
  } else {
    cout << "...created pointer to RCS request object for get runNumber" << endl;
  }
E 5
I 5
//   // create pointer to request object for run number
//   cdevRequestObject *req = cdevRequestObject::attachPtr(session,"get runNumber");
D 6
//   if(!req){
E 6
I 6
//   if(!req) {
E 6
//     cout << "...error creating pointer to RCS request object for get runNumber" << endl;
//     return;
//   } else {
//     cout << "...created pointer to RCS request object for get runNumber" << endl;
//   }
E 5

D 5
  // request and print current run number
  req->send(0,result);
  if(!req){
    cout << "...error sending to RCS request object" << endl;
    return;
  } else {
    cout << "...req->send to RCS request object worked..." << endl;
    cout << "...current runNumber is: " << (int) result << endl;
  }
E 5
I 5
//   // request and print current run number
//   req->send(0,result);
D 6
//   if(!req){
E 6
I 6
//   if(!req) {
E 6
//     cout << "...error sending to RCS request object" << endl;
//     return;
//   } else {
//     cout << "...req->send to RCS request object worked..." << endl;
//     cout << "...current runNumber is: " << (int) result << endl;
//   }
E 5


D 5
  // set monitor on run number
  cdevRequestObject *reqc = cdevRequestObject::attachPtr(session,"monitorOn runNumber");
  if(!reqc){
    cout << "...error creating pointer to RCS request object for monitorOn runNumber" << endl;
    return;
  } else {
    cout << "...created pointer to RCS request object for monitorOn runNumber" << endl;
  }
E 5
I 5
//   // set monitor on run number
//   cdevRequestObject *reqc = cdevRequestObject::attachPtr(session,"monitorOn runNumber");
D 6
//   if(!reqc){
E 6
I 6
//   if(!reqc) {
E 6
//     cout << "...error creating pointer to RCS request object for monitorOn runNumber" << endl;
//     return;
//   } else {
//     cout << "...created pointer to RCS request object for monitorOn runNumber" << endl;
//   }
E 5

D 5
  cdevCallback coda_runnumber(coda_callback_func,(void*)1);
  status=reqc->sendCallback(NULL, coda_runnumber);
  if(status!=CDEV_SUCCESS){
    cout << "...error creating sendCallback for monitorOn request object" << status << endl;
    exit(EXIT_FAILURE);
  } else {
    cout << "...registered callback for monitorOn request object, status is: " << status << endl;
  }
E 5
I 5
//   cdevCallback coda_runnumber(coda_callback_func,(void*)1);
//   status=reqc->sendCallback(NULL, coda_runnumber);
D 6
//   if(status!=CDEV_SUCCESS){
E 6
I 6
//   if(status!=CDEV_SUCCESS) {
E 6
//     cout << "...error creating sendCallback for monitorOn request object" << status << endl;
//     exit(EXIT_FAILURE);
//   } else {
//     cout << "...registered callback for monitorOn request object, status is: " << status << endl;
//   }
E 17
I 17
  
E 17
E 5

I 4
D 5
  */
E 5
D 6


E 6
E 4
  return;
}

D 17

//--------------------------------------------------------------------------


I 8
void coda_callback_func(int status, void *userarg, cdevRequestObject &coda_req_obj,
			    cdevData& result){

  static int count = 0;

  count++;

  cout << endl;
  cout << "coda callback call number: " << setw(20) << count << endl;
  cout << "                   status: " << setw(20) << status << endl;
  cout << "                    myarg: " << setw(20) << (int)userarg<< endl;
  cout << "                   result: " << setw(20) << (int) result << endl;
  cout << endl;

}


//---------------------------------------------------------------------------
E 17
I 17
  
//----------------------------------------------------------------
E 17


I 19
extern "C" {
E 19
E 8
D 6
void status_poll_callback(T_IPC_MSG msg){
E 6
I 6
void status_poll_callback(T_IPC_MSG msg) {
E 6

I 6
D 17
  TipcMsgAppendStr(msg,"Session");
  TipcMsgAppendStr(msg,session);
E 17
I 17
D 19
  TipcMsgAppendStr(msg,"Number of messages received");
E 19
I 19
  TipcMsgAppendStr(msg,(char*)"Number of messages received");
E 19
  TipcMsgAppendInt4(msg,nmsg);
  
D 19
  TipcMsgAppendStr(msg,"Number of messages ignored");
E 19
I 19
  TipcMsgAppendStr(msg,(char*)"Number of messages ignored");
E 19
  TipcMsgAppendInt4(msg,nmsg_ignored);
E 17
  
I 9
D 17
  TipcMsgAppendStr(msg,"Number of alarms handled");
  TipcMsgAppendInt4(msg,nalarm);
E 17
I 17
D 19
  TipcMsgAppendStr(msg,"Number of messages handled");
E 19
I 19
  TipcMsgAppendStr(msg,(char*)"Number of messages handled");
E 19
  TipcMsgAppendInt4(msg,nmsg_handled);
E 17
  
E 9
E 6
D 4
  TipcMsgAppendStr(msg,"Msql host");
  TipcMsgAppendStr(msg,msqlhost);
  TipcMsgAppendStr(msg,"Msql daemon number");
  TipcMsgAppendInt4(msg,msqldaemon);
  TipcMsgAppendStr(msg,"Msql database");
  TipcMsgAppendStr(msg,msqldb);
  TipcMsgAppendStr(msg,"Msql info table");
  TipcMsgAppendStr(msg,msqlinfotbl);
  TipcMsgAppendStr(msg,"Msql error table");
  TipcMsgAppendStr(msg,msqlerrtbl);
  TipcMsgAppendStr(msg,"Msql status table");
  TipcMsgAppendStr(msg,msqlstattbl);
  TipcMsgAppendStr(msg,"Msql machine host");
  TipcMsgAppendStr(msg,msqlmhost);

E 4
  return;
}
I 19
}
E 19


//-------------------------------------------------------------------


I 19
extern "C" {
E 19
D 6
void quit_callback(int sig){
E 6
I 6
void quit_callback(int sig) {
E 6

D 6
  // received quit control command
  if(sig<=0){
    logfile  << "...received QUIT command..." << endl;
    
    // SIGHUP or SIGTERM
  } else if((sig==SIGHUP)||(sig==SIGTERM)){
    logfile  << "...received SIGHUP or SIGTERM..." << endl;
    
    // SIGQUIT or SIGINT
  } else {
    logfile << "...received SIGINT or SIGQUIT..." << endl;
  }
E 6

D 6
  // set done flag
E 6
I 6
D 8
  logfile << "...stopping...received signal " << sig << endl;
E 8
I 8
  cout << "...stopping...received signal " << sig << endl;
E 8
E 6
  done=1;
D 6

E 6
  return;
}
I 19
}
E 19


//-------------------------------------------------------------------


D 6
void alarm_monitor_done(void){
E 6
I 6
D 8
void init_tcl() {
E 8
I 8
D 17
int ipc_mainloop(int opened, int fd, void *arg) {
  
  if(opened==0) {
    cerr << "cdevsys unable to attach to ipc fd: " << fd << endl;
    exit(EXIT_FAILURE);
E 8
E 6

D 4
  int err;
E 4
I 4
D 6
  int status;
E 4
  time_t endtime=time(NULL);

  // close ipc connection
  ipc_close();

  // enter stop msg ito log file
  logfile << "stopping on " << ctime(&endtime) << endl;  

D 5
  // post startup message to cmsglog system
E 5
I 5
  // post shutdown message
E 5
  sprintf(temp,"Process shutdown:  %15s",unique_id);
D 4
  err=eMsgLog(msqlhost,msqldb,msqlstattbl,"alarm_monitor","STOP",temp,
	      msqlmhost,msqlfacility,0,msqldaemon);
E 4
I 4
D 5
  status=insert_msg("alarm_monitor","alarm_monitor",unique_id,"status","STOP",0,temp);
E 5
I 5
  status=insert_msg("alarm_monitor","alarm_monitor",unique_id,"status",0,"STOP",0,temp);
E 5
E 4

I 4
D 5

E 5
E 4
  return;
}


//-------------------------------------------------------------------


D 4
// process messages in Smartsockets receipt queue
void ipc_mainloop(int poll_time){
E 4
I 4
D 5
int ipc_mainloop(int opened, int fd, void *arg){
  
  if(opened==0){
    cerr << "Cdevsys unable to attach to ipc fd: " << fd << endl;
    return(-1);
E 4

D 4
server.MainLoop( (double)poll_time );
E 4
I 4
  } else {
    server.MainLoop(0.0);
    return (0);
  }
}
E 4

I 4

//-------------------------------------------------------------------


int cmlog_mainloop(int opened, int fd, void *arg){
  
  if(opened==0){
    cerr << "Cdevsys unable to attach to cmlog fd: " << fd << endl;
    return(-1);

  } else {
    browser.pendIO (0.0);
    return (0);
  }
E 4
}


//-------------------------------------------------------------------


E 5
void init_tcl(){

E 6
D 8
  // link c and Tcl variables
  Tcl_LinkVar(interp,"application",    	   (char *)&application,      	TCL_LINK_STRING);
  Tcl_LinkVar(interp,"unique_id",     	   (char *)&unique_id,          TCL_LINK_STRING);
  Tcl_LinkVar(interp,"session",     	   (char *)&session,            TCL_LINK_STRING);
E 8
I 8
  } else {
    server.MainLoop(0.0);
    return(0);
  }
}
E 8

D 4
  Tcl_LinkVar(interp,"msqlhost",           (char *)&msqlhost,           TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqldb",             (char *)&msqldb,             TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqlinfotbl",        (char *)&msqlinfotbl,        TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqlerrtbl",         (char *)&msqlerrtbl,         TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqlstattbl",        (char *)&msqlstattbl,        TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqlmhost",          (char *)&msqlmhost,          TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqlfacility",       (char *)&msqlfacility,       TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqldaemon",         (char *)&msqldaemon,         TCL_LINK_INT);
E 4

D 4

E 4
D 8
  // create Tcl commands
  Tcl_CreateCommand(interp,"help",tcl_help,
  		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,"quit",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,"stop",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,"exit",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
E 8
I 8
//-------------------------------------------------------------------
E 8

D 8
  return;
E 8

I 8
int cmlog_mainloop(int opened, int fd, void *arg) {
  
I 16
  int status;

E 16
  if(opened==0) {
    cerr << "cdevsys unable to attach to cmlog fd: " << fd << endl;
    exit(EXIT_FAILURE);

  } else {
D 16
    browser.pendIO(0.0);
E 16
I 16
    status=browser.pendIO(0.0);
    if(status!=CMLOG_SUCCESS) {
      cerr << "?Error return from pend, status is: " << status << endl << endl;
      done=1;
    }
E 16
    return (0);
  }
E 8
}


D 8
//--------------------------------------------------------------------------
E 8
I 8
//-------------------------------------------------------------------
E 8


E 17
D 6
void decode_command_line(int argc, char**argv){
E 6
I 6
void decode_command_line(int argc, char**argv) {
E 6

D 6
  char *help = "\nusage:\n\n  alarm_monitor [-a application] [-u unique_id]"
D 4
    " [-s session] [-t init_tcl_script]\n"
    "       [-mhost msqlhost]     [-mdaemon msqldaemon] [-mdb msqldb]\n"
    "       [-minfo msqlinfotbl]  [-merr msqlerrtbl]    [-mstat msqlstattbl]\n\n";
E 4
I 4
D 5
    " [-s session] [-t init_tcl_script]\n";
E 5
I 5
    " [-s session] [-t init_tcl_script] [-debug]\n";
E 6
I 6
D 19
  char *help = "\nusage:\n\n  alarm_handler [-a application] [-u unique_id]"
E 19
I 19
  const char *help = "\nusage:\n\n  alarm_handler [-a application] [-u unique_id]"
E 19
D 14
    " [-s session] [-t init_tcl_script] [-w window] [-debug]\n";
E 14
I 14
D 17
    " [-s session] [-t init_tcl_script] [-debug]\n";
E 17
I 17
    " [-t init_tcl_script] [-debug]\n";
E 17
E 14
E 6
E 5
E 4


  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
D 6
    if(strncasecmp(argv[i],"-h",2)==0){
E 6
I 6
    if(strncasecmp(argv[i],"-h",2)==0) {
E 6
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
I 5
D 6
    else if (strncasecmp(argv[i],"-debug",6)==0){
E 6
I 6
    else if (strncasecmp(argv[i],"-debug",6)==0) {
E 6
      debug=1;
      i=i+1;
    }
E 5
D 6
    else if (strncasecmp(argv[i],"-a",2)==0){
E 6
I 6
    else if (strncasecmp(argv[i],"-a",2)==0) {
E 6
      application=strdup(argv[i+1]);
      i=i+2;
    }
D 6
    else if (strncasecmp(argv[i],"-u",2)==0){
E 6
I 6
    else if (strncasecmp(argv[i],"-u",2)==0) {
E 6
      unique_id=strdup(argv[i+1]);
      i=i+2;
    }
D 6
    else if (strncasecmp(argv[i],"-s",2)==0){
E 6
I 6
D 17
    else if (strncasecmp(argv[i],"-s",2)==0) {
E 6
      session=strdup(argv[i+1]);
      i=i+2;
    }
E 17
D 6
    else if (strncasecmp(argv[i],"-t",2)==0){
E 6
I 6
    else if (strncasecmp(argv[i],"-t",2)==0) {
E 6
      init_tcl_script=strdup(argv[i+1]);
      i=i+2;
    }
I 6
D 14
    else if (strncasecmp(argv[i],"-w",2)==0) {
      window=strdup(argv[i+1]);
      i=i+2;
    }
E 14
E 6
    else if (strncasecmp(argv[i],"-",1)==0) {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
      i=i+2;
    }
D 4
    else if (strcasecmp(argv[i],"-mhost")==0){
      msqlhost=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strcasecmp(argv[i],"-mdaemon")==0){
      sscanf(argv[i+1],"%d",&msqldaemon);
      i=i+2;
    }
    else if (strcasecmp(argv[i],"-mdb")==0){
      msqldb=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strcasecmp(argv[i],"-minfo")==0){
      msqlinfotbl=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strcasecmp(argv[i],"-merr")==0){
      msqlerrtbl=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strcasecmp(argv[i],"-mstat")==0){
      msqlstattbl=strdup(argv[i+1]);
      i=i+2;
    }
E 4
  }

  return;
}

  
//----------------------------------------------------------------


I 8
void init_tcl() {

  // link c and Tcl variables
D 9
  Tcl_LinkVar(interp,"application",    	   (char *)&application,      	TCL_LINK_STRING);
  Tcl_LinkVar(interp,"unique_id",     	   (char *)&unique_id,          TCL_LINK_STRING);
  Tcl_LinkVar(interp,"session",     	   (char *)&session,            TCL_LINK_STRING);
E 9
I 9
D 17
  Tcl_LinkVar(interp,"application",    	(char *)&application,        TCL_LINK_STRING);
  Tcl_LinkVar(interp,"unique_id",     	(char *)&unique_id,          TCL_LINK_STRING);
  Tcl_LinkVar(interp,"session",     	(char *)&session,            TCL_LINK_STRING);
  Tcl_LinkVar(interp,"nalarm",     	(char *)&nalarm,             TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 17
I 17
D 19
  Tcl_LinkVar(interp,"application",    	(char *)&application,  TCL_LINK_STRING);
  Tcl_LinkVar(interp,"unique_id",     	(char *)&unique_id,    TCL_LINK_STRING);
  Tcl_LinkVar(interp,"nmsg",     	(char *)&nmsg,         TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"nmsg_ignored",   	(char *)&nmsg_ignored, TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"nmsg_handled",  	(char *)&nmsg_handled, TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 19
I 19
  Tcl_LinkVar(interp,(char*)"application", (char *)&application,  TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"unique_id",   (char *)&unique_id,    TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"nmsg",        (char *)&nmsg,         TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nmsg_ignored",(char *)&nmsg_ignored, TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nmsg_handled",(char *)&nmsg_handled, TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 19
E 17
E 9


  // create Tcl commands
D 19
  Tcl_CreateCommand(interp,"help",tcl_help,
E 19
I 19
  Tcl_CreateCommand(interp,(char*)"help",tcl_help,
E 19
  		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 19
  Tcl_CreateCommand(interp,"quit",tcl_quit,
E 19
I 19
  Tcl_CreateCommand(interp,(char*)"quit",tcl_quit,
E 19
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 19
  Tcl_CreateCommand(interp,"stop",tcl_quit,
E 19
I 19
  Tcl_CreateCommand(interp,(char*)"stop",tcl_quit,
E 19
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 19
  Tcl_CreateCommand(interp,"exit",tcl_quit,
E 19
I 19
  Tcl_CreateCommand(interp,(char*)"exit",tcl_quit,
E 19
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  return;

}


//--------------------------------------------------------------------------


I 19
extern "C" {
E 19
E 8
int tcl_help(ClientData clientdata, Tcl_Interp *interp,
D 6
		int argc, char **argv){
E 6
I 6
		int argc, char **argv) {
E 6

D 19
    char *help =
E 19
I 19
  char *help = (char*)
E 19
D 4
    "\nTcl commands available in the dd_monitor program:\n\n"
E 4
I 4
D 6
    "\nTcl commands available in the alarm_monitor program:\n\n"
E 6
I 6
    "\nTcl commands available in the alarm_handler program:\n\n"
E 6
E 4
    " help                  print this message\n"
    " stop                  stop program\n"
    " quit                  stop program\n"
    " exit                  stop program\n"
    "\n\n Type command that require args with NO args for more information\n"
    "\n";

    Tcl_SetResult(interp,help,TCL_STATIC);

  return (TCL_OK);

}
I 19
}
E 19


D 6
/*---------------------------------------------------------------------*/
E 6
I 6
//---------------------------------------------------------------------
E 6


I 19
extern "C" {
E 19
int tcl_quit(ClientData clientdata, Tcl_Interp *interp,
D 6
	     int argc, char **argv){
E 6
I 6
	     int argc, char **argv) {
E 6
  
D 8
  logfile << "...received Tcl quit message..." << endl;
E 8
I 8
  cout << "...received Tcl quit message..." << endl;
E 8
  done=1;
  
  return (TCL_OK);
I 19
}
E 19
}


D 6
/*---------------------------------------------------------------------*/
E 6
I 6
D 8
//---------------------------------------------------------------------


void epics_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
			    cdevData& result){

  static int count = 0;

  count++;

  cout << endl;
  cout << "epics callback call number: " << setw(20) << count << endl;
  cout << "                    status: " << setw(20) << status << endl;
  cout << "                     myarg: " << setw(20) << (int)userarg<< endl;
  cout << "                    result: " << setw(20) << (double) result << endl;
  cout << endl;

}


//---------------------------------------------------------------------------


void cmlog_callback_func (int status, void* arg, cmlogPacket* data) {

I 7
  char name[20];
E 7
  char facility[20];
  char host[20];
  char user[20];
  char msgstatus[20];
  char process[20];
  char msgclass[20];
  int severity;
  long code;
  long msgtime;
  char text[1024];
  char atime[20];
  struct tm *tstruct;
  cdevData* res = NULL;


  // extract messages from packet
  if (data) {

    cmlogMsg** msgs = data->messages();
    for (int i=0; i<data->numberOfData(); i++) {

      cmlog_cdevMessage& idata = (cmlog_cdevMessage)(*msgs[i]);
      res=idata.getData();
      if (res) {
	res->get("cmlogTime", &msgtime);  
I 7
	res->get("name",      name,        sizeof(name));
E 7
	res->get("facility",  facility,    sizeof(facility));
	res->get("host",      host,        sizeof(host));
	res->get("user",      user,        sizeof(user));
	res->get("process",   process,     sizeof(process));
	res->get("class",     msgclass,    sizeof(msgclass));
 	res->get("severity",  &severity);
	res->get("status",    msgstatus,   sizeof(msgstatus));
	res->get("code",      &code);  
	res->get("text",      text,        sizeof(text));
	

	if((debug==1)&&(severity>=2)) {
	  tstruct=localtime(&msgtime);
	  strftime(atime,sizeof(atime),"%d-%h-%Y %H:%M",tstruct);
	  cout << setw(8) << facility << " " << setw(8) << host << " " 
	       << setw(8) << user << " " << setw(17) << atime << " " 
	       << setw(12) << process << " " << setw(8) << msgclass << " " 
	       << setw(1) << severity << " " << setw(6) << msgstatus << " " 
	       << setw(3) << code << " " << setw(0) << text << endl;
	}


	// handle alarm
D 7
	handle_cmlog_alarms(msgtime,facility,host,user,process,msgclass,severity,msgstatus,code,text);
E 7
I 7
	handle_cmlog_alarms(msgtime,name,facility,host,user,process,msgclass,severity,
			    msgstatus,code,text);
E 7

      }
    }

    // free memory
    for (i = 0; i < data->numberOfData (); i++) delete msgs[i];
    delete []msgs;
  }

  return;
}
E 8
D 17


E 17
D 8
//--------------------------------------------------------------------------
E 8
I 8
//---------------------------------------------------------------------
D 17
//                  alarm handling functions
//---------------------------------------------------------------------
E 8


D 7
void handle_cmlog_alarms(long msgtime, char *facility, char *host, char *user, char *process,
			 char *msgclass, int severity, char *msgstatus, int code, char *text) {
E 7
I 7
D 8
void handle_cmlog_alarms(long msgtime, char *name, char *facility, char *host, char *user, 
E 8
I 8
void handle_cmlog_alarms(int debug, long msgtime, char *name, char *facility, char *host, char *user, 
E 8
			 char *process, char *msgclass, int severity, char *msgstatus, int code, 
			 char *text) {
E 7

I 14
  time_t now=time(NULL);

E 14
I 11
D 13
  time_t last_sync=0;
  time_t last_rcs=0;


E 11
D 7
  char *alarm_window = "/bin/csh -c \"$CLON_BIN/alarm_window -w %s %s >& /dev/null &\"";
E 7

D 8

E 8
D 7
  if(1==0) {
    sprintf(temp,alarm_window,"clon03:0.0","Severe CODA error...run must be stopped!");
E 7
I 7
D 9
  //  if((strcmp(name,"CODA")==0) && (severity==2) && () ) {
  if(0==1) {
E 9
I 9
  if( ((severity==2) && (strncasecmp(text,"SEVERE SYNC ERROR",17)==0)) ) {
D 11
    nalarm++;
E 9
    sprintf(temp,"/bin/csh -c \"$CLON_BIN/alarm_window -w %s %s >& /dev/null &\"",
D 9
	    "$DISPLAY",
D 8
	    "Severe CODA sync error...run must be stopped and ROCs reset!");
E 8
I 8
	    "Severe CODA sync error...run must be stopped and ROCs reset NOW!");
E 9
I 9
D 10
	    "clasxt2:0.0",
E 10
I 10
	    "clon03:0.0",
E 10
	    "Severe CODA sync error...run must be stopped NOW!");
I 10
    system(temp);
  }
  
E 11
I 11
    if((time(NULL)-last_sync)>180) {
      nalarm++;
      sprintf(temp,"/bin/csh -c \"$CLON_BIN/alarm_window -w %s %s >& /dev/null &\"",
	      "clon03:0.0",
	      "Severe CODA sync error...run must be stopped NOW!");
      system(temp);
      last_sync=time(NULL);
    }
  }    
E 13
E 11

D 12
  if( ((severity==2) && (strcmp(msgclass,"RCS")==0)) ) {
E 12
I 12
  if( ((severity>2) && (strcmp(msgclass,"RCS")==0)) ) {
E 12
D 11
    nalarm++;
    sprintf(temp,"/bin/csh -c \"$CLON_BIN/alarm_window -w %s %s >& /dev/null &\"",
	    "clon03:0.0",
	    "RunControl Server detected fatal error...run should be stopped NOW!");
E 10
E 9
E 8
E 7
    system(temp);
E 11
I 11
D 13
    if((time(NULL)-last_rcs)>180) {
E 13
I 13
    if((time(NULL)-last_rcs)>90) {
E 13
      nalarm++;
D 12
      sprintf(temp,"/bin/csh -c \"$CLON_BIN/alarm_window -w %s %s >& /dev/null &\"",
E 12
I 12
D 14
      sprintf(temp,"/bin/csh -c \"$CLON_BIN/alarm_window -w %s %s:  %s >& /dev/null &\"",
E 12
	      "clon03:0.0",
E 14
I 14
      cout << "Handling SEVERE error from RCS " << ctime(&now);
D 15
      sprintf(temp,"/bin/csh -c \"$CLON_BIN/alarm_window -w %s %s:  %s &\"",
E 15
I 15
      sprintf(temp,"/bin/csh -c \"(setenv DISPLAY clon03:0.0; $CLON_BIN/alarm_window -w %s %s:  %s )&\"",
E 15
      	      "clon03:0.0",
E 14
D 12
	      "Run control server detected fatal error...run must be stopped NOW!");
E 12
I 12
	      "Run control server detected SEVERE error, run should be stopped",text);
E 12
      system(temp);
      last_rcs=time(NULL);
    }
E 11
  }
  
  return;
}


//--------------------------------------------------------------------------
I 14




E 14

D 8

void coda_callback_func(int status, void *userarg, cdevRequestObject &coda_req_obj,
			    cdevData& result){

  static int count = 0;

  count++;

  cout << endl;
  cout << "coda callback call number: " << setw(20) << count << endl;
  cout << "                   status: " << setw(20) << status << endl;
  cout << "                    myarg: " << setw(20) << (int)userarg<< endl;
  cout << "                   result: " << setw(20) << (int) result << endl;
  cout << endl;

}


//---------------------------------------------------------------------------
E 8
E 6

E 17
E 1
