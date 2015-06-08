h03295
s 00041/00005/00676
d D 1.16 07/10/22 00:02:06 boiarino 17 16
c *** empty log message ***
e
s 00033/00006/00648
d D 1.15 06/02/15 15:29:23 boiarino 16 15
c will attach to ET system on host 'CLON_EB', usually remote one
e
s 00000/00002/00654
d D 1.14 01/05/14 17:06:19 wolin 15 14
c et.h does not need extern c
e
s 00044/00044/00612
d D 1.13 01/04/02 14:49:32 wolin 14 13
c Fixes for new compiler
e
s 00001/00001/00655
d D 1.12 01/04/02 14:32:34 wolin 13 12
c Bombproofed .eof()
e
s 00001/00001/00655
d D 1.11 01/01/03 10:47:08 wolin 12 11
c is_open()
e
s 00028/00018/00628
d D 1.10 00/10/25 14:00:34 wolin 11 10
c Added check on prestart,go,pause
e
s 00006/00014/00640
d D 1.9 00/10/19 13:16:18 wolin 10 9
c Added forcedclose
e
s 00000/00001/00654
d D 1.8 00/09/08 13:08:53 wolin 9 8
c Minor mods
e
s 00002/00001/00653
d D 1.7 00/02/17 14:26:34 wolin 8 7
c Made control[0]=104, same as 5th head word
e
s 00009/00007/00645
d D 1.6 00/01/12 14:57:34 wolin 7 6
c Updated for new et
e
s 00004/00004/00648
d D 1.5 99/05/20 16:26:29 wolin 6 5
c Seems to be working with new et
c 
e
s 00017/00019/00635
d D 1.4 99/03/16 13:18:51 wolin 5 4
c No longer checking run status
c 
e
s 00002/00002/00652
d D 1.3 99/03/09 13:58:34 wolin 4 3
c Seems to be working
e
s 00028/00012/00626
d D 1.2 99/02/16 16:25:38 wolin 3 1
c New scheme for subjects and message types
c 
e
s 00000/00000/00000
d R 1.2 99/02/12 11:25:28 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 et2ipc/s/ipcbank2et.cc
e
s 00638/00000/00000
d D 1.1 99/02/12 11:25:27 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 16

#define REMOTE_ET

E 16
I 1
//  ipcbank2et
//
//  inserts bos bank events into data stream 
//
D 10
//  ejw, 11-feb-99
E 10
I 10
//  ejw, 19-oct-00
E 10


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for smartsockets
#include <rtworks/cxxipc.hxx>


// CLAS ipc
#include <clas_ipc_prototypes.h>


// for i/o
#include <fstream.h>
#include <iomanip.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


I 17
/* BOS swap */
/*sergey: temporary*/
#include "/usr/local/clas/devel/coda/src/et/main/cinclude/bosRecordSwap.c"



E 17
// for tcl
extern "C" {
#include <tcl.h>
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


// for et
D 15
extern "C"{
E 15
#include <et.h>
D 15
}
E 15


// misc variables
D 14
static char *project       	 = "clastest";
static char *session   		 = NULL;
E 14
I 14
static char *project       	 = (char*)"clastest";
static char *session   		 = (char*)NULL;
E 14
static char *host         	 = getenv("HOST");
D 9
static char *clon_root 		 = getenv("CLON_ROOT");
E 9
D 14
static char *unique_name       	 = "ipcbank2et";
E 14
I 14
static char *unique_name       	 = (char*)"ipcbank2et";
E 14
static char *unique_id           = (char *) malloc(64);
static int wait_time           	 = 5;
D 11
static int restart_time       	 = 30;
E 11
static Tcl_Interp *interp;     
static char *init_tcl_script   	 = NULL;
D 14
static char *msql_database     	 = "clasrun";
E 14
I 14
static char *msql_database     	 = (char*)"clasrun";
E 14
static int done                	 = 0;
D 11
static int force_et            	 = 0;
E 11
static int nev_rec           	 = 0;
static int nev_to_et           	 = 0;
static int nev_no_run            = 0;
static int nev_no_et           	 = 0;
static int run                   = 0;
D 5
static int run_status            = 0;
E 5
static int debug                 = 0;
static int et_ok                 = 0;
static int lost_connection       = 0;
static time_t last_time          = time(NULL);
static int last_rec              = 0;
static int last_proc             = 0; 
static double rec_rate;
static double proc_rate;
static time_t now;
static char bank_source[132];
I 11
static char rcstate_file[128];
E 11
static char temp[256];


// et stuff
static et_sys_id et_system_id;
I 7
static et_openconfig openconfig;
E 7
static char et_filename[128];
D 6
static et_proc_id et_process_id;
E 6
I 6
static et_att_id et_attach_id;
E 6
static et_event *et_event_ptr;
static int et_control[ET_STATION_SELECT_INTS];
D 17
static int etbuffersize;
E 17
I 17
static unsigned int etbuffersize;
E 17


// other prototypes
void decode_command_line(int argc, char **argv);
void init_tcl(void);
void init_et(void);
void connect_et(void);
D 14
void status_poll_callback(T_IPC_MSG msg);
E 14
void bosbank_callback(T_IPC_CONN,
		      T_IPC_CONN_PROCESS_CB_DATA,
		      T_CB_ARG);
I 14
extern "C" {
void status_poll_callback(T_IPC_MSG msg);
E 14
void control_message_callback(T_IPC_CONN,
			      T_IPC_CONN_PROCESS_CB_DATA,
			      T_CB_ARG);
void quit_callback(int sig);
D 14
extern "C" {
E 14
int get_run_number(char *msql_database, char *session);
D 5
int get_run_status(char *session);
E 5
D 3
int create_header(int *p, int fevlen, int &banksize,
E 3
I 3
int create_header(int *p, int evlen, int &banksize,
E 3
                  int name1, int name2, int nrun, int nevnt, int nphys, int trig);
D 3
int add_bank(int *p2fev, int fevlen, 
E 3
I 3
int add_bank(int *p2ev, int evlen, 
E 3
D 14
      char *name, int num, char *format, int ncol, int nrow, int ndata, int &banksize, int *data);
E 14
I 14
      const char *name, int num, const char *format, int ncol, int nrow, int ndata, int &banksize, int *data);
E 14
D 3
int va_add_bank(int *p2fev, int fevlen, 
E 3
I 3
int va_add_bank(int *p2ev, int evlen, 
E 3
D 14
      char *name, int num, char *format, int ncol, int nrow, int ndata, int &banksize, ...);
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
	       int severity, char *status, int code, char *text);
}
E 14
I 14
      const char *name, int num, const char *format, int ncol, int nrow, int ndata, int &banksize, ...);
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *status, int code, const char *text);
E 14
int tcl_help(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
int tcl_quit(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
I 14
}
E 14


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//--------------------------------------------------------------------------


main(int argc,char **argv){


  int status;


  // synch with stdio
  ios::sync_with_stdio();


  // decode command line
  decode_command_line(argc,argv);


  // set session name
D 14
  if(session==NULL)session="clasprod";
E 14
I 14
  if(session==NULL)session=(char*)"clasprod";
E 14


  // get Tcl interp, create tcl commands, link vars, process Tcl startup script, etc.
  interp=tclinterp_init();
  init_tcl();
  if(init_tcl_script!=NULL)Tcl_EvalFile(interp,init_tcl_script);


  // create unique_id from uniq_name, node, and et system name
  char *p;
  strcpy(unique_id,unique_name);
  strcat(unique_id,"_");
  p=host+strlen(host)-2;
  strcat(unique_id,p);
  strcat(unique_id,"_");
  strcat(unique_id,session);
  

  // set ipc parameters and connect to ipc system
  ipc_set_application(project);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_control_message_callback(control_message_callback);
  ipc_set_quit_callback(quit_callback);
  status=ipc_init(unique_id,"ipcbank2et");
  if(status<0){
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another ipcbank2et  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }
D 14
  server.SubjectSubscribe("evt_system",TRUE);
D 3
  TipcMt mt("bosbank");
E 3
I 3
  TipcMt mt("evt_bosbank");
E 14
I 14
  server.SubjectSubscribe((T_STR)"evt_system",TRUE);
  TipcMt mt((T_STR)"evt_bosbank");
E 14
E 3
  server.ProcessCbCreate(mt,bosbank_callback,0);
D 3
  sprintf(bank_source,"bosbank/%s",session);
E 3
I 3
  sprintf(bank_source,"evt_bosbank/%s",session);
E 3
  server.SubjectSubscribe(bank_source,TRUE);


  //  create tcl_request callback
  tclipc_init(interp);


  // initialize et system
  init_et();


I 11
  // get rcstate file name
  sprintf(rcstate_file,"%s/run_log/rcstate_%s.txt",getenv("CLON_PARMS"),session);


E 11
  // post startup message
  sprintf(temp,"Process startup:  %s  in project:  %s, session: %s",unique_id,project,session);
  status=insert_msg("ipcbank2et","online",unique_id,"status",0,"START",0,temp);


  // flush messages and output
  server.Flush();
  fflush(NULL);

D 16

E 16
  // main loop
  while(done==0) {

    server.MainLoop((double)wait_time);

I 5
D 16

E 16
    // check et
    if((et_ok==1)&&(et_alive(et_system_id)==0)) {
      lost_connection=1;
      done=1;
    }

D 16

E 16
E 5
    // calc rates every 60 seconds...banks come very slowly!
    now=time(NULL);
    if((now-last_time)>60) {
      int delta=now-last_time;
      rec_rate=(double)(nev_rec-last_rec)/delta;
      proc_rate=(double)(nev_to_et-last_proc)/delta;
      last_time=now;
      last_rec=nev_rec;
      last_proc=nev_to_et;
    }
I 16

E 16
  }


I 16

E 16
D 10
  // restart if lost connection, otherwise close
  if(lost_connection==1) {
    insert_msg("ipcbank2et","online",unique_id,"status",1,"WARN",0,
D 5
	       "ipcbank2et...lost connection to ET system...restarting");
D 4
    sprintf(temp,"%s/start_ipcbank2et %d %s %s &",getenv("CLON_BIN"),restart_time,session,project);
E 4
I 4
    sprintf(temp,"%s/start_ipcbank2et %d %s %s &",getenv("CLON_BIN"),restart_time,project,session);
E 4
    cerr << "Lost connection...restarting: " << temp << endl;
    system(temp);
E 5
I 5
	       "ipcbank2et...lost connection to ET system");
    //    sprintf(temp,"%s/start_ipcbank2et %d %s %s &",getenv("CLON_BIN"),restart_time,project,session);
    //    cerr << "Lost connection...restarting: " << temp << endl;
    //    system(temp);
E 5
  }


  // shutdown message
E 10
I 10
  // shutdown messages
  if(lost_connection==1)insert_msg("ipcbank2et","online",unique_id,"status",1,"WARN",0,
				   "ipcbank2et...lost connection to ET system");
E 10
  sprintf(temp,"Process shutdown:  %s",unique_id);
  status=insert_msg("ipcbank2et","online",unique_id,"status",0,"STOP",0,temp);


  // done
D 10
  if(et_ok==1)et_close(et_system_id);
E 10
I 10
  if(et_ok==1)et_forcedclose(et_system_id);
E 10
  ipc_close();
  exit(EXIT_SUCCESS);
}
       

//--------------------------------------------------------------------------


void init_et() {
  
D 16

E 16
I 16
  char ch[256], *chptr;
E 16
  et_ok=0;


  // create et file name
  sprintf(et_filename,"/tmp/et_sys_%s",session);
D 7

E 7
I 7
  et_open_config_init(&openconfig);
E 7

I 16
#ifdef REMOTE_ET

D 17
  chptr = getenv("CLON_EB");
E 17
I 17
  chptr = getenv("CLON_ER");
E 17
  if(chptr==NULL)
  {
D 17
    printf("ERROR: env var 'CLON_EB' is not set - exit\n");
E 17
I 17
    printf("ERROR: env var 'CLON_ER' is not set - exit\n");
E 17
    exit(0);
  }

  et_open_config_sethost(openconfig, chptr);
  et_open_config_gethost(openconfig, ch);

D 17
  /*need it ?*/
  /*et_open_config_setmode(openconfig, ET_HOST_AS_REMOTE);*/
E 17
I 17
  /* do not need it: Carl fixed problem
E 17
  et_open_config_setmode(openconfig, ET_DIRECT);
I 17
  */
E 17

  printf("remote host >%s< et system >%s<\n",ch,et_filename);

#endif

E 16
D 3
  // get max normal event size
  et_system_geteventsize(et_system_id,&etbuffersize);


E 3
  return;
}


//---------------------------------------------------------------------


void connect_et() {
  
  int status;
I 3
  sigset_t sigblock;
E 3


  et_ok=0;

I 16
  printf("trying to connect to ET system\n");
E 16

  // open et system
D 7
  if(et_open(&et_system_id,et_filename,0,NULL)!=ET_OK)return;
E 7
I 7
D 16
  if(et_open(&et_system_id,et_filename,openconfig)!=ET_OK)return;
E 16
I 16
  if(et_open(&et_system_id,et_filename,openconfig)!=ET_OK)
  {
    printf("ERROR: Cannot connect to ET - return\n");
    return;
  }
E 16
E 7

D 16

E 16
I 3
  // get max normal event size
  et_system_geteventsize(et_system_id,&etbuffersize);
I 16
  printf("INFO: event size = %d\n",etbuffersize);
E 16


  // block signals to THIS thread and any thread created by this thread
  // needed to keep signals from et threads
  sigfillset(&sigblock);
  pthread_sigmask(SIG_BLOCK,&sigblock,NULL);


E 3
  // attach to existing station
D 6
  status=et_attach_to_station(et_system_id,&et_process_id,ET_GRANDCENTRAL);
E 6
I 6
D 7
  status=et_attach_to_station(et_system_id,&et_attach_id,ET_GRANDCENTRAL);
E 7
I 7
  status=et_station_attach(et_system_id,ET_GRANDCENTRAL,&et_attach_id);
E 7
E 6
  if(status!=ET_OK) {
D 10
    et_close(et_system_id);
E 10
I 10
    et_forcedclose(et_system_id);
E 10
    cerr << "Unable to attach to grandcentral station" << endl;
    done=1;
    return;
  }
  

I 3
  // unblock signals
  pthread_sigmask(SIG_UNBLOCK,&sigblock,NULL);


E 3
  // success
  et_ok=1; 
  cout << "...now connected to ET system: " << et_filename 
       << ",   station: grandcentral" << endl;


  return;
}


//---------------------------------------------------------------------


void bosbank_callback(T_IPC_CONN conn,
		      T_IPC_CONN_PROCESS_CB_DATA data,
		      T_CB_ARG arg) {
  
  int status;
D 7
  int *p,i,nused,banksize,nhead,buflen;
E 7
I 7
  int *p,*pstart,i,nused,banksize,nhead,buflen;
E 7
  T_STR msgtype;
  T_STR bankname,bankformat;
  T_INT4 banknumber,ncol,nrow,nwrds,ndatawords;
  T_INT4* datawords;
I 11
  char line[128];
E 11
  
  TipcMsg msg(data->msg);


  // for record segment header
  int nevnt  = 0;
  int nphys  = 0;
  int trig   = 0;

  // constants for head bank
  int nvers  = 0;
  int type   = 104;
  int rocst  = 0;
  int evcls  = 0;
  int presc  = 0;


  // total events received
  nev_rec++;


D 5
  // ignore if no run in progress
  run_status=get_run_status(session);
  if((force_et!=1)&&(run_status!=11)) {
    nev_no_run++;
    return;
  }


E 5
  // check et
  if(et_ok==0) {
    connect_et();
    if(et_ok==0) {
      nev_no_et++;
      return;
    }
  } else if(et_alive(et_system_id)==0) {
I 5
    nev_no_et++;
E 5
    lost_connection=1;
    done=1;
    return;
  }


I 11
  // no entry unless in prestart, go, or pause state
  ifstream file(rcstate_file);
D 12
  if(file.bad()||file.eof()) {
E 12
I 12
D 13
  if(!file.is_open()||file.eof()) {
E 13
I 13
  if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad()) {
E 13
E 12
    cerr << "\n?Unable to read " << rcstate_file << endl;
    nev_no_run++;
    return;
  }
  file.getline(line,sizeof(line));
  file.close();
  if(
     (strncasecmp(line,"prestart",8)!=0) &&
     (strncasecmp(line,"go",2)!=0)       &&
     (strncasecmp(line,"pause",5)!=0)    
     ) {
    nev_no_run++;
    return;
  }
  

E 11
I 5
  // get run number
  run=get_run_number(msql_database,session);


E 5
  // get free event 
D 6
  status=et_event_new(et_system_id,et_process_id,&et_event_ptr,ET_WAIT_ASYNC,NULL,etbuffersize);
E 6
I 6
D 7
  status=et_event_new(et_system_id,et_attach_id,&et_event_ptr,ET_WAIT_ASYNC,NULL,etbuffersize);
E 7
I 7
  status=et_event_new(et_system_id,et_attach_id,&et_event_ptr,ET_ASYNC,NULL,etbuffersize);
E 7
E 6
  if(status!=ET_OK) {
I 17
	printf("error in et_event_new - return\n");
E 17
    if(debug!=0) cerr << "?unable to get event, status is: " << status << endl;
    nev_no_et++;
    return;
  }


D 3
  // set pointer, reset counts, fill ctl words, etc.
E 3
I 3
D 8
  // set control words
E 8
I 8
  // set control words...must set 1st word > 32 to not fool CODA
E 8
  for (int ii=0; ii<ET_STATION_SELECT_INTS; ii++) et_control[ii]=0;
I 8
  et_control[0]=type;
E 8
  et_event_setcontrol(et_event_ptr,et_control,ET_STATION_SELECT_INTS);


  // set pointer, reset counts, etc.
E 3
D 7
  p=(int *)et_event_getdata(et_event_ptr);
E 7
I 7
  et_event_getdata(et_event_ptr,(void**)&p);
  pstart=p;
E 7
  nused=0;
  nhead=0;


D 5
  // get run number
  run=get_run_number(msql_database,session);


E 5
  // create segment header, then update pointer and counters
  status=create_header(p,etbuffersize-nused,nhead,'RUNP','ARMS',run,nevnt,nphys,trig);
  if(status==0){
    p+=nhead;
    nused+=nhead;
  }  

I 17
  /* 'pstart' points to data area */
E 17

  // head bank
  status=va_add_bank(p,etbuffersize-nused,"HEAD",0,"I",8,1,8,banksize,
	      nvers,run,nevnt,(int)time(NULL),type,rocst,evcls,presc);
  if(status==0) {
    p+=banksize;
    nused+=banksize;
  }


  // extract banks from message and insert into event
  int fptr = 1;
  msg.Current(0);
  while(fptr<msg.NumFields()) {
    msg >> bankname >> banknumber >> bankformat >> ncol >> nrow >> nwrds
	>> datawords >> GetSize(&ndatawords) >> Check;
    if((debug==1)&&(nwrds!=ndatawords)) {
      cerr << "Bank data inconsistent...nwrds,ndatawords are: " << nwrds << ", " << ndatawords << endl;
    }
    status=add_bank(p,etbuffersize-nused,bankname,banknumber,bankformat,ncol,nrow,nwrds,banksize,
		    (int*)&datawords[0]);
    if(status==0) {
      p+=banksize;
      nused+=banksize;
    }
    fptr+=7;
  }
  

  // all banks added...set overall word and byte counts
D 7
  *((int *)et_event_getdata(et_event_ptr)+10)=nused-nhead;
E 7
I 7
  *(pstart+10)=nused-nhead;
E 7
  et_event_setlength(et_event_ptr,nused*4);


I 17
#ifdef REMOTE_ET


  /*****************************************************************/
  /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
  /* sergey: we assume that local host is sparc and destination is */
  /* intel, so we'll swap BOS record and set ET endian equal to    */
  /* ET_ENDIAN_LITTLE=1; in future it must be done such way so it  */
  /* will check the necessity of swapping                          */
  /*****************************************************************/


  /****************** do it only if necessary !!!!!!!!!!!!!!!!!!!! */
  BOSrecordSwap((unsigned int *)pstart, (unsigned int *)pstart);
  /**************************/

{
  int endian;
  et_event_getendian(et_event_ptr,&endian);
  /*printf("before endian=0x%08x\n",endian);*/
  et_event_setendian(et_event_ptr,ET_ENDIAN_LITTLE);
  et_event_getendian(et_event_ptr,&endian);
  /*printf("after endian=0x%08x\n",endian);*/
}

#endif


E 17
  // insert event into ET system
  if(et_alive(et_system_id)==1) {
D 6
    status=et_event_put(et_system_id,et_process_id,et_event_ptr);
E 6
I 6
    status=et_event_put(et_system_id,et_attach_id,et_event_ptr);
E 6
    if(status==ET_OK) {
      nev_to_et++;
    } else {
D 3
      if(debug==1) cerr << "?unable to put fev, status is: " << status << endl;
E 3
I 3
      if(debug==1) cerr << "?unable to put event, status is: " << status << endl;
E 3
      nev_no_et++;
    }
  }

  return;
}

  
//----------------------------------------------------------------


void status_poll_callback(T_IPC_MSG msg){

  
D 14
  TipcMsgAppendStr(msg,"Bank source");
E 14
I 14
  TipcMsgAppendStr(msg,(T_STR)"Bank source");
E 14
  TipcMsgAppendStr(msg,bank_source);

D 14
  TipcMsgAppendStr(msg,"nevent received");
E 14
I 14
  TipcMsgAppendStr(msg,(T_STR)"nevent received");
E 14
  TipcMsgAppendInt4(msg,nev_rec);
  
D 14
  TipcMsgAppendStr(msg,"nevent sent to et");
E 14
I 14
  TipcMsgAppendStr(msg,(T_STR)"nevent sent to et");
E 14
  TipcMsgAppendInt4(msg,nev_to_et);
  
D 14
  TipcMsgAppendStr(msg,"nevent no et");
E 14
I 14
  TipcMsgAppendStr(msg,(T_STR)"nevent no et");
E 14
  TipcMsgAppendInt4(msg,nev_no_et);
  
D 14
  TipcMsgAppendStr(msg,"nevent no run");
E 14
I 14
  TipcMsgAppendStr(msg,(T_STR)"nevent no run");
E 14
  TipcMsgAppendInt4(msg,nev_no_run);
  
D 14
  TipcMsgAppendStr(msg,"wait_time");
E 14
I 14
  TipcMsgAppendStr(msg,(T_STR)"wait_time");
E 14
  TipcMsgAppendInt4(msg,wait_time);

D 11
  TipcMsgAppendStr(msg,"restart_time");
  TipcMsgAppendInt4(msg,restart_time);

E 11
  return;
}


//-------------------------------------------------------------------


void quit_callback(int sig){

  done=1;

  return;
}


//-------------------------------------------------------------------


void control_message_callback(T_IPC_CONN conn,
			      T_IPC_CONN_PROCESS_CB_DATA data,
			      T_CB_ARG arg) {

  T_STR string;


  // get first string
  TipcMsgSetCurrent(data->msg,0);
  TipcMsgNextStr(data->msg,&string);
  
  
D 3
  //  et status request
E 3
I 3
  //  event transfer system status request
E 3
  if(strcasecmp(string,"evt_status_poll")==0) {

    T_STR srvnode = server.Node();

D 14
    TipcMsg status("evt_status");
    status.Dest("/evt_system/status");
E 14
I 14
    TipcMsg status((T_STR)"evt_status");
    status.Dest((T_STR)"/evt_system/status");
E 14
    status.Sender(unique_id);
    status << unique_id << host << session << srvnode 
	   << (T_INT4) nev_rec << rec_rate << (T_INT4) nev_to_et << proc_rate
D 4
	   << (T_INT4) et_ok << "et:GRANDCENTRAL";
E 4
I 4
D 14
	   << (T_INT4) et_ok << "IPC->et:GRANDCENTRAL";
E 14
I 14
	   << (T_INT4) et_ok << (T_STR)"IPC->et:GRANDCENTRAL";
E 14
E 4
    server.Send(status,TRUE);
    server.Flush();


  //  don't understand message...ship to smartsockets interpreter
  } else {
    TutCommandParseStr(string);
  }

  return;
}


//----------------------------------------------------------------------


void decode_command_line(int argc, char**argv){

D 11
  char *help = "\nusage:\n\n  ipcbank2et [-a project] [-u unique_name]\n"
    "        [-s session] [-m msql_database] [-t init_tcl_script] [-w wait_time] [-r restart_time]\n"
    "        [-force] [-debug]\n";
E 11
I 11
D 14
  char *help = "\nusage:\n\n  ipcbank2et [-a project] [-u unique_name] [-s session]\n"
E 14
I 14
  const char *help = "\nusage:\n\n  ipcbank2et [-a project] [-u unique_name] [-s session]\n"
E 14
    "             [-m msql_database] [-t init_tcl_script] [-w wait_time]\n"
    "             [-debug]\n";
E 11


  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
D 11
    else if (strncasecmp(argv[i],"-force",6)==0){
      force_et=1;
      i=i+1;
    }
E 11
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      project=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0){
      unique_name=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0){
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-m",2)==0){
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-t",2)==0){
      init_tcl_script=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-w",2)==0){
      wait_time=atoi(argv[i+1]);
      i=i+2;
    }
D 11
    else if (strncasecmp(argv[i],"-r",2)==0){
      restart_time=atoi(argv[i+1]);
      i=i+2;
    }
E 11
    else if (strncasecmp(argv[i],"-",1)==0) {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
      i=i+1;
    }
  }

  return;
}

  
//----------------------------------------------------------------


void init_tcl(){

  // link c and Tcl variables
D 14
  Tcl_LinkVar(interp,"project",    	   (char *)&project,      	TCL_LINK_STRING);
  Tcl_LinkVar(interp,"unique_name",    	   (char *)&unique_name,        TCL_LINK_STRING);
  Tcl_LinkVar(interp,"session",     	   (char *)&session,            TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msql_database",  	   (char *)&msql_database,      TCL_LINK_STRING);
  Tcl_LinkVar(interp,"wait_time",     	   (char *)&wait_time,          TCL_LINK_INT);
D 11
  Tcl_LinkVar(interp,"restart_time",  	   (char *)&restart_time,       TCL_LINK_INT);
  Tcl_LinkVar(interp,"force_et",     	   (char *)&force_et,           TCL_LINK_INT);
E 11
  Tcl_LinkVar(interp,"debug",     	   (char *)&debug,              TCL_LINK_INT);
  Tcl_LinkVar(interp,"nev_rec",     	   (char *)&nev_rec,            TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"nev_to_et",     	   (char *)&nev_to_et,          TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"nev_no_et",     	   (char *)&nev_no_et,          TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"nev_no_run",     	   (char *)&nev_no_run,         TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"rec_rate",     	   (char *)&rec_rate,           TCL_LINK_DOUBLE|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"proc_rate",     	   (char *)&proc_rate,          TCL_LINK_DOUBLE|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"et_ok",     	   (char *)&et_ok,              TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"run",     	   (char *)&run,                TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 14
I 14
  Tcl_LinkVar(interp,(char*)"project",       (char *)&project,       TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"unique_name",   (char *)&unique_name,   TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"session",       (char *)&session,       TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"msql_database", (char *)&msql_database, TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"wait_time",     (char *)&wait_time,     TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"debug",         (char *)&debug,         TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"nev_rec",       (char *)&nev_rec,       TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nev_to_et",     (char *)&nev_to_et,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nev_no_et",     (char *)&nev_no_et,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nev_no_run",    (char *)&nev_no_run,    TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"rec_rate",      (char *)&rec_rate,      TCL_LINK_DOUBLE|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"proc_rate",     (char *)&proc_rate,     TCL_LINK_DOUBLE|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"et_ok",         (char *)&et_ok,         TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"run",     	     (char *)&run,           TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 14
D 5
  Tcl_LinkVar(interp,"run_status",     	   (char *)&run_status,         TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 5


  // create Tcl commands
D 14
  Tcl_CreateCommand(interp,"help",tcl_help,
E 14
I 14
  Tcl_CreateCommand(interp,(char*)"help",tcl_help,
E 14
  		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 14
  Tcl_CreateCommand(interp,"quit",tcl_quit,
E 14
I 14
  Tcl_CreateCommand(interp,(char*)"quit",tcl_quit,
E 14
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 14
  Tcl_CreateCommand(interp,"stop",tcl_quit,
E 14
I 14
  Tcl_CreateCommand(interp,(char*)"stop",tcl_quit,
E 14
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 14
  Tcl_CreateCommand(interp,"exit",tcl_quit,
E 14
I 14
  Tcl_CreateCommand(interp,(char*)"exit",tcl_quit,
E 14
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  return;

}


//--------------------------------------------------------------------------


int tcl_help(ClientData clientdata, Tcl_Interp *interp,
		int argc, char **argv){

D 14
    char *help =
E 14
I 14
    const char *help =
E 14
    "\nTcl commands available in the ipcbank2et program:\n\n"
    " help                  print this message\n"
    " stop                  stop program\n"
    " quit                  stop program\n"
    " exit                  stop program\n"
    "\n\n Type command that require args with NO args for more information\n"
    "\n";

D 14
    Tcl_SetResult(interp,help,TCL_STATIC);
E 14
I 14
    Tcl_SetResult(interp,(char*)help,TCL_STATIC);
E 14

  return (TCL_OK);

}


//---------------------------------------------------------------------


int tcl_quit(ClientData clientdata, Tcl_Interp *interp,
	     int argc, char **argv){
  
  done=1;
  
  return (TCL_OK);
}


//---------------------------------------------------------------------
E 1
