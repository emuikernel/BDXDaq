
/* event_monitor.cc */


/*
event_monitor -a clasprod -s clasprod -r -g MON -adir /hist/monitor -no_photon
*/
//  event_monitor

//  analyzes events, creates hist

//  still to do
//    timed wait not working?

//  ejw, 21-dec-99
//  rhf, 19 Sept 00 fixing concurrancy issues


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

#ifdef SSIPC
// for smartsockets
#include <rtworks/cxxipc.hxx>
#endif

// for et
extern "C" {
#include <et.h>
}


// system stuff
#include <fstream.h>
#include <iomanip.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>

#ifdef SunOS
#include <thread.h> /*just for 'thr_setconcurrency'*/
#endif

#ifdef SSIPC
// for CLAS ipc
#include <clas_ipc_prototypes.h>
#endif

// for bos
#define NBCS 800000
extern "C"{
#include <bosio.h>
#include <bosfun.h>
#include <bos.h>
#include <bcs.h>
}



/* ??? 
typedef struct hbookcommon
{
  int mem[6000000];
} HBOOKcommon;
HBOOKcommon	pawc_;
*/


// fixes hbook reset in different threads
char *css[1000];
int css0 = 0;
int maxcss = sizeof(css)/sizeof(char*);


// misc variables
static char *application      = (char*)"clastest";
static char *session          = NULL;
#ifdef SunOS_i86pc
static char *uniq_name        = (char*)"evmon";
#else
static char *uniq_name        = (char*)"mon";
#endif
static char *unique_id        = (char *) malloc(64);
static int evt_from_file      = 0;
static char *input_file       = NULL;
static int max_event          = -1;
static int wait_time          = 100;    // msec
static int et_ok              = 0;
static int debug              = 0;
static int done               = 0;
static time_t start_time      = time(NULL);
static BOSIO *descriptor;
static char temp[256];
static char filename[256];


// et stuff
static char *et_station_name        = (char*)"MON";
static et_openconfig openconfig;
static et_sys_id et_system_id;
static char et_filename[128];
static et_stat_id et_station_id;
static et_statconfig et_station_config;
static et_att_id et_attach_id;
static et_event *et_event_ptr;


// for hbook
static int no_global          = -1;
static char *global_sect      = (char*)"none";
static int reset_hist_bor     = 0;
static int no_archive         = 0;
static int no_snapshot        = 0;
static char *archive_dir      = (char*)".";
static time_t last_archive    = time(NULL);
static time_t last_reset      = 0;
static int archive_time       = 600;       // secs
static int archive_evt        = 1000000;   // events


// event counts, rates, etc.
static int last_run           = 0;
static double et_rate         = 0.;
static double proc_rate       = 0.;
static time_t last_time       = time(NULL);
static int last_nevet         = 0;
static int last_nevproc       = 0;
static time_t delta;


// fortran common block
extern "C" {
struct {
  long nevet;
  long nevproc;
  long nev_this_run;
  long nrun_proc;

  long current_run;
  long current_event;

  long no_cc;
  long no_dc;
  long no_ec;
  long no_sc;
  long no_st;

  long no_ic;
  long no_lac;
  long no_trig;
  long no_tg;
  long no_photon;
  long no_call;
  long no_l2;

  long no_scaler;
  long no_hist;
  long no_timeline;

  long event_length;

  long no_rast;

  long no_tpc;

  long no_tpe;

} event_monitor_ = {0,0,0,0, 0,0, 0,0,0,0,0,  0,0,0,0,0,0,0,  0,0,0, 0, 0, 0,0};
}


// prototypes
void decode_command_line(int argc, char **argv);
void init_et();
void connect_et();
void process_events(void);
void analyze_event(void);
void decode_hbook_id(const char *cid, long *id, char *dir);

extern "C" {

#ifdef SSIPC
void control_message_callback(T_IPC_CONN conn,
			      T_IPC_CONN_PROCESS_CB_DATA data,
			      T_CB_ARG arg);
void status_poll_callback(T_IPC_MSG msg);
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *status, int code, const char *message);
#endif

void *control_thread(void *param);
void quit_callback(int sig);
/*int et2bos(long *evt, int *jw, const char *list);*/
char *strdupf(const char *fstring, int len);

}


// fortran prototypes
extern "C" {
void clas_init_(int *no_global, const char *global_sect_name, int len);
void clas_book_(void);
void clas_bor_(void);
void clas_process_(void);
void clas_eor_(void);
void clas_done_(void);
void reset_hist_(const char *flag, int len);
void hcdir_(const char *dir, const char *flag, int l1, int l2);
void hrput_(long *id, const char *filename, const char *flag, int len1, int len2);
void hbook1_(long*,const char*,int*,float*,float*,float*,int);
void hbook2_(long*,const char*,int*,float*,float*,int*,float*,float*,float*,int);
void hidall_(long*,int*);
void hgive_(long*,const char*,int*,float*,float*,int*,float*,float*,int*,int*,int);
void hrdir_(int*,const char*,int*,int);
void cltou_(const char *flag, int len);

}

#ifdef SSIPC
// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();
#endif

//--------------------------------------------------------------------------


int
main(int argc, char **argv)
{
  int status;
  pthread_t t1;
  int r1=0;
  

  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line
  decode_command_line(argc,argv);
  if(evt_from_file!=0)event_monitor_.no_timeline=-1;


  // get session name
  if(session==NULL)session=(char*)"clasprod";

printf("1\n");fflush(stdout);

  // create unique_id from uniq_name and session name
  strcpy(unique_id,uniq_name);
  strcat(unique_id,"_");
  strcat(unique_id,session);
  
  // init ipc thread package
#ifdef SunOS
  thr_setconcurrency(thr_getconcurrency()+2);
#endif

#ifdef SSIPC
  if(!TipcInitThreads()) {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }

  // set ipc parameters, connect to server, etc.
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
  status=ipc_init(unique_id,"event monitor");
  if(status<0) {
    cerr << "\n?Unable to connect to server..."
	 << "probably duplicate unique id\n"
	 << "   ...check for another event_monitor connected to "
	 << session << " using ipc_info\n"
	 << "   ...only one such process allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }
  server.SubjectSubscribe((T_STR)"evt_system",TRUE);
  server.Flush();


  // launch ipc thread
  if(pthread_create(&t1,NULL,control_thread,(void *)&r1)!=0) {
    cerr << "\nUnable to create ipc thread" << endl << endl;
    exit(EXIT_FAILURE);
  }
#endif

  // initialize bos
  bosInit(bcs_.iw,NBCS);

  // init et structures, etc;
  init_et();
  
  // initialize hbook, packages
  clas_init_(&no_global,global_sect,strlen(global_sect));


  // book hist
cerr << "\n Will book all histos now" << endl << endl;
 fflush(stderr); 
  clas_book_();

cerr << "\n *main* done with booking" << endl << endl;
 fflush(stderr); 

  // init ipc control callback, or open file
  if(evt_from_file==0)
  {
    ;
#ifdef SSIPC
    ipc_set_control_message_callback(control_message_callback);
    server.Flush();
#endif
  }
  else
  {
    status=bosOpen(input_file,(char*)"r",&descriptor);
  }

#ifdef SSIPC
  // post startup message
  sprintf(temp,"Process startup:    %15s  in application:  %s",unique_id,application);
  status=insert_msg("event_monitor",unique_id,unique_id,"status",0,"START",0,temp);
#endif

  // flush output
  fflush(NULL);


  // process events - loop
  process_events();


  // last eor
  if(event_monitor_.nev_this_run>0)
  {
    clas_eor_();
    cout << "End of run       " << event_monitor_.current_run << endl;
  }


  // final package cleanup
  clas_done_();


  // close data source
  if(evt_from_file==0)
  {
    et_forcedclose(et_system_id);
  }
  else
  {
    bosClose(descriptor);
  }
  

  // last archive hist
  if(no_archive==0)
  {
    hcdir_("//PAWC"," ",6,1);
    sprintf(filename,"%s/%s_%s_%06d.hbook",archive_dir,session,uniq_name,
	    event_monitor_.current_run);
    long id=0;
    hrput_(&id, filename, "T", strlen(filename), 1);
  }


  // print stats
  if(evt_from_file!=0)
  {
    cout << "\n\n   Event monitor read events from file:  " << input_file << endl << endl;
  }
  else
  {
    cout << "\n\n   Event monitor received " << event_monitor_.nevet 
	 << " events from et" << endl << endl;
  }
  cout << "   Events processed:   " << event_monitor_.nevproc << endl
       << "   Runs processed:     " << event_monitor_.nrun_proc << endl << endl;
  
#ifdef SSIPC
  // post shutdown message
  sprintf(temp,"Process shutdown:  %15s",unique_id);
  status=insert_msg("event_monitor",unique_id,unique_id,"status",0,"STOP",0,temp);

  // close ipc and exit
  ipc_close();
#endif

  exit(EXIT_SUCCESS);
}


//--------------------------------------------------------------------------


void *control_thread(void *param) {

  while(done==0) {

#ifdef SSIPC
    server.MainLoop(2.0);
#endif

    // calc event rates every 10 seconds
    delta=time(NULL)-last_time;
    if(delta>10) {
      et_rate =(double)(event_monitor_.nevet-last_nevet)/delta;
      proc_rate=(double)(event_monitor_.nevproc-last_nevproc)/delta;
      last_time=time(NULL);
      last_nevet=event_monitor_.nevet;
      last_nevproc=event_monitor_.nevproc;   
    }      
  }


  return (void *)0;
}


//------------------------------------------------------------------------


void init_et() {
  

  et_ok=0;


  // create et file name, etc.
  sprintf(et_filename,"/tmp/et_sys_%s",session);

  et_open_config_init(&openconfig);

  // get et library name

  char *et_user_library;

  et_user_library = (char *) malloc(128);
  
  strcpy(et_user_library,getenv("CLON_LIB"));
  strcat(et_user_library,"/et_user_library.so");
  


  // create station config in case no station exists
  et_station_config_init(&et_station_config);
  et_station_config_setblock(et_station_config,ET_STATION_BLOCKING);
  et_station_config_setselect(et_station_config,ET_STATION_SELECT_USER);
  et_station_config_setuser(et_station_config,ET_STATION_USER_SINGLE);
  et_station_config_setrestore(et_station_config,ET_STATION_RESTORE_OUT);
  et_station_config_setcue(et_station_config,100);
  et_station_config_setprescale(et_station_config,1);
  et_station_config_setlib(et_station_config,et_user_library);
  et_station_config_setfunction(et_station_config,"et_mon_function");


  return;
}


//--------------------------------------------------------------------------


void connect_et() {
  
  int status;
  sigset_t sigblock;
  

  et_ok=0;

  
  // open et system
  if(et_open(&et_system_id,et_filename,openconfig)!=ET_OK)return;
  
printf("11\n");fflush(stdout);
  // create station if not already created
  status=et_station_create(et_system_id,&et_station_id,et_station_name,et_station_config);
printf("22\n");fflush(stdout);

  if((status!=ET_OK)&&(status!=ET_ERROR_EXISTS)) { 
      cout << status << endl;
      et_forcedclose(et_system_id);
      cerr << "Unable to create station " << et_station_name << endl;
      done=1;
      return;
  }
  
  
  // block signals to THIS thread and any thread created by this thread
  // needed to keep signals from et threads
  sigfillset(&sigblock);
  pthread_sigmask(SIG_BLOCK,&sigblock,NULL);
  
  
  // attach to station
  status=et_station_attach(et_system_id,et_station_id,&et_attach_id);
  if(status!=ET_OK) {
      et_forcedclose(et_system_id);
      cerr << "Unable to attach to station " << et_station_name << endl;
      done=1;
      return;
  }
  

  // unblock signals
  pthread_sigmask(SIG_UNBLOCK,&sigblock,NULL);


  // success
  et_ok=1; 
  cout << "...now connected to ET system: " << et_filename 
       << ",   station: " << et_station_name << endl;

  return;
}


//--------------------------------------------------------------------------


void
process_events()
{
  long *evt;
  int status;
  unsigned int len;
  timespec wait_spec;

  while(done==0)
  {
    // clean bos common
    bosLdrop(bcs_.iw,(char*)"E");
    bosNgarbage(bcs_.iw);

    // get an event from et or file
    if(evt_from_file==0)
    {   
      // check et system
      if(et_ok==0)
      {
        connect_et();
        if(et_ok==0)
        {
          sleep(1);
          continue;
        }
      }
      else if(et_alive(et_system_id)==0)
      {
        done=1;
        return;
      }

      // et system ok...try to get event...sleep if none available
      wait_spec.tv_sec =(time_t)(wait_time/1000);
      wait_spec.tv_nsec=(long)((wait_time%1000)*1000000);
      //status=et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_TIMED,&wait_spec);
      status=et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_ASYNC,NULL);
      if(status==ET_OK)
      {   
        event_monitor_.nevet++;
        et_event_getdata(et_event_ptr,(void**)&evt);
        et2bos((int *)evt, (int *)bcs_.iw, (char*)"E");
	 
        et_event_getlength(et_event_ptr,&len);
        event_monitor_.event_length=len/4;
        et_event_put(et_system_id,et_attach_id,et_event_ptr);

        analyze_event();
      }
      else if((status==ET_ERROR_TIMEOUT)||(status==ET_ERROR_EMPTY)||
              (status==ET_ERROR_BUSY))
      {
        nanosleep(&wait_spec,NULL);
        continue;
      }
      else
      {
        cerr << "?error return from et_event_get: " << status << endl;
        done=1;
        return;
      }
    }
    else
    {
      if(bosRead(descriptor,bcs_.iw,(char*)"E")!=0)
      {
        done=1;
        break;
      }
      event_monitor_.event_length=0.;
      analyze_event();
    }
  }

  return;
}


//-------------------------------------------------------------------


void
analyze_event(void)
{
  int head;

  /* reset histograms if required */
#ifdef SSIPC
  // check for tcl reset command before current event processed
  while(css0 > 0)
  {   
    // clear histogram
    reset_hist_(css[css0],strlen(css[css0]));

    // free memory and decrement count
    free(css[css0]);
    css0--;
  }
#else
  if(0)
  {
    reset_hist_("ALL",3);
  }
#endif


  // ignore event if no head bank
  head=bosNlink(bcs_.iw,(char*)"HEAD",0);
  if(head<=0)
  {
    if(debug!=0)cout << "missing head bank event " << event_monitor_.nevet+1 << endl;
    return;
  }

  // ignore events from previous runs
  if(bcs_.iw[head+1]<=last_run)
  {
    if(debug!=0)cout << "Received event from old run " << bcs_.iw[head+1] << endl;
    return;
  }
 
  // check for end of run...end run event or run number increases
  if((bcs_.iw[head+6]==20)||(bcs_.iw[head+1]>event_monitor_.current_run))
  {
    if(event_monitor_.nev_this_run>0)
    {
      event_monitor_.nev_this_run=0;
      event_monitor_.nrun_proc++;
      last_run=event_monitor_.current_run;
      cout << "End of run       " << event_monitor_.current_run << endl << endl;
      clas_eor_();
      if(no_archive==0)
      {
        hcdir_("//PAWC"," ",6,1);
        sprintf(filename,"%s/%s_%s_%06d.hbook",archive_dir,session,uniq_name,last_run);
        long id=0;
        hrput_(&id, filename, "T", strlen(filename), 1);
        last_archive=time(NULL);
      }
    }
  }

  // check for bor...run number change
  if(bcs_.iw[head+1]>event_monitor_.current_run)
  {
    event_monitor_.current_run=bcs_.iw[head+1];
    if((event_monitor_.no_hist==0)&&(reset_hist_bor==1))
    {
      reset_hist_("ALL",3);
      last_reset=time(NULL);
    }
    cout << "\nBeginning of run " << event_monitor_.current_run << endl;
    clas_bor_();
    event_monitor_.nev_this_run=0;
  }

  // ignore process control events
  if(bcs_.iw[head+6]>15)
  {
    if(debug!=0)cout << "Ignoreing control event " << event_monitor_.nevet << " evtype is " 
		     << bcs_.iw[head+6]<< endl;
    return;
  }

  // process event
  event_monitor_.nevproc++;
  event_monitor_.nev_this_run++;
  event_monitor_.current_run=bcs_.iw[head+1];
  event_monitor_.current_event=bcs_.iw[head+2];
  clas_process_();

  // check if snapshot time
  if((no_snapshot==0)&&(no_archive==0)&&
     ((time(NULL)-last_archive)>archive_time)&&
     (event_monitor_.nev_this_run>archive_evt))
  {
    hcdir_("//PAWC"," ",6,1);
    sprintf(filename,"%s/%s_%s_%06d.hbook",archive_dir,session,uniq_name,
	    event_monitor_.current_run);
    long id=0;
    hrput_(&id, filename, "T", strlen(filename), 1);
    last_archive=time(NULL);
  }

  // check if processed enough events
  if((max_event>0)&&(event_monitor_.nevproc>=max_event)) done=1;

  return;
}


//-------------------------------------------------------------------


void quit_callback(int sig) {
  
  // received signal or quit control command
  cout << "...stopping, received signal: " << sig << endl;
  done=1;
  
  return;
}


//----------------------------------------------------------------------

#ifdef SSIPC

void status_poll_callback(T_IPC_MSG msg) {
  
  char p[27];

  
  TipcMsgAppendStr(msg,(T_STR)"Session");
  TipcMsgAppendStr(msg,session);
  TipcMsgAppendStr(msg,(T_STR)"application");
  TipcMsgAppendStr(msg,application);
  TipcMsgAppendStr(msg,(T_STR)"unique_id");
  TipcMsgAppendStr(msg,unique_id);

  TipcMsgAppendStr(msg,(T_STR)"evt_from_file");
  TipcMsgAppendInt4(msg,evt_from_file);
  if(evt_from_file==1) {
    TipcMsgAppendStr(msg,(T_STR)"input_file");
    TipcMsgAppendStr(msg,input_file);
  }

  TipcMsgAppendStr(msg,(T_STR)"");
  TipcMsgAppendStr(msg,(T_STR)"");

  TipcMsgAppendStr(msg,(T_STR)"start_time");
  strcpy(p,ctime(&start_time)); *strchr(p,'\n')='\0';
  TipcMsgAppendStr(msg,p);
  TipcMsgAppendStr(msg,(T_STR)"current_run");
  TipcMsgAppendInt4(msg,event_monitor_.current_run);
  TipcMsgAppendStr(msg,(T_STR)"current_event");
  TipcMsgAppendInt4(msg,event_monitor_.current_event);
  TipcMsgAppendStr(msg,(T_STR)"last_run");
  TipcMsgAppendInt4(msg,last_run);
  TipcMsgAppendStr(msg,(T_STR)"last_archive");
  strcpy(p,ctime(&last_archive)); *strchr(p,'\n')='\0';
  TipcMsgAppendStr(msg,p);
  TipcMsgAppendStr(msg,(T_STR)"last_reset");
  strcpy(p,ctime(&last_reset)); *strchr(p,'\n')='\0';
  TipcMsgAppendStr(msg,p);

  TipcMsgAppendStr(msg,(T_STR)"");
  TipcMsgAppendStr(msg,(T_STR)"");

  TipcMsgAppendStr(msg,(T_STR)"nevet");
  TipcMsgAppendInt4(msg,event_monitor_.nevet);
  TipcMsgAppendStr(msg,(T_STR)"nevproc");
  TipcMsgAppendInt4(msg,event_monitor_.nevproc);
  TipcMsgAppendStr(msg,(T_STR)"nev_this_run");
  TipcMsgAppendInt4(msg,event_monitor_.nev_this_run);
  TipcMsgAppendStr(msg,(T_STR)"nrun_proc");
  TipcMsgAppendInt4(msg,event_monitor_.nrun_proc);

  TipcMsgAppendStr(msg,(T_STR)"");
  TipcMsgAppendStr(msg,(T_STR)"");

  /*sergey
  if(init_tcl_script!=NULL) {
    TipcMsgAppendStr(msg,(T_STR)"init_tcl_script");
    TipcMsgAppendStr(msg,init_tcl_script);
  }
  */

  TipcMsgAppendStr(msg,(T_STR)"no_global");
  TipcMsgAppendInt4(msg,no_global);
  TipcMsgAppendStr(msg,(T_STR)"global_sect");
  TipcMsgAppendStr(msg,global_sect);
  TipcMsgAppendStr(msg,(T_STR)"archive_time");
  TipcMsgAppendInt4(msg,archive_time);
  TipcMsgAppendStr(msg,(T_STR)"archive_evt");
  TipcMsgAppendInt4(msg,archive_evt);
  TipcMsgAppendStr(msg,(T_STR)"max_event");
  TipcMsgAppendInt4(msg,max_event);
  TipcMsgAppendStr(msg,(T_STR)"wait_time");
  TipcMsgAppendInt4(msg,wait_time);
  TipcMsgAppendStr(msg,(T_STR)"reset_hist_bor");
  TipcMsgAppendInt4(msg,reset_hist_bor);
  TipcMsgAppendStr(msg,(T_STR)"no_archive");
  TipcMsgAppendInt4(msg,no_archive);
  TipcMsgAppendStr(msg,(T_STR)"archive_dir");
  TipcMsgAppendStr(msg,archive_dir);
  TipcMsgAppendStr(msg,(T_STR)"no_snapshot");
  TipcMsgAppendInt4(msg,no_snapshot);
  TipcMsgAppendStr(msg,p);
  TipcMsgAppendStr(msg,(T_STR)"debug");
  TipcMsgAppendInt4(msg,debug);

  TipcMsgAppendStr(msg,(T_STR)"");
  TipcMsgAppendStr(msg,(T_STR)"");

  TipcMsgAppendStr(msg,(T_STR)"no_cc");
  TipcMsgAppendInt4(msg,event_monitor_.no_cc);
  TipcMsgAppendStr(msg,(T_STR)"no_ec");
  TipcMsgAppendInt4(msg,event_monitor_.no_ec);
  TipcMsgAppendStr(msg,(T_STR)"no_sc");
  TipcMsgAppendInt4(msg,event_monitor_.no_sc);
  TipcMsgAppendStr(msg,(T_STR)"no_dc");
  TipcMsgAppendInt4(msg,event_monitor_.no_dc);
  TipcMsgAppendStr(msg,(T_STR)"no_lac");
  TipcMsgAppendInt4(msg,event_monitor_.no_lac);
  TipcMsgAppendStr(msg,(T_STR)"no_st");
  TipcMsgAppendInt4(msg,event_monitor_.no_st);
  TipcMsgAppendStr(msg,(T_STR)"no_ic");
  TipcMsgAppendInt4(msg,event_monitor_.no_ic);
  TipcMsgAppendStr(msg,(T_STR)"no_tg");
  TipcMsgAppendInt4(msg,event_monitor_.no_tg);
  TipcMsgAppendStr(msg,(T_STR)"no_trig");
  TipcMsgAppendInt4(msg,event_monitor_.no_trig);
  TipcMsgAppendStr(msg,(T_STR)"no_photon");
  TipcMsgAppendInt4(msg,event_monitor_.no_photon);
  TipcMsgAppendStr(msg,(T_STR)"no_call");
  TipcMsgAppendInt4(msg,event_monitor_.no_call);
  TipcMsgAppendStr(msg,(T_STR)"no_l2");
  TipcMsgAppendInt4(msg,event_monitor_.no_l2);
  TipcMsgAppendStr(msg,(T_STR)"no_scaler");
  TipcMsgAppendInt4(msg,event_monitor_.no_scaler);
  TipcMsgAppendStr(msg,(T_STR)"no_timeline");
  TipcMsgAppendInt4(msg,event_monitor_.no_timeline);
  TipcMsgAppendStr(msg,(T_STR)"no_hist");
  TipcMsgAppendInt4(msg,event_monitor_.no_hist);

  TipcMsgAppendStr(msg,(T_STR)"no_rast");
  TipcMsgAppendInt4(msg,event_monitor_.no_rast);

  TipcMsgAppendStr(msg,(T_STR)"no_tpc");
  TipcMsgAppendInt4(msg,event_monitor_.no_tpc);

  return;
}


//--------------------------------------------------------------------------


void control_message_callback(T_IPC_CONN conn,
			      T_IPC_CONN_PROCESS_CB_DATA data,
			      T_CB_ARG arg) {

  T_STR string;


  // get first string
  TipcMsgSetCurrent(data->msg,0);
  TipcMsgNextStr(data->msg,&string);
  
  
  //  evt status request
  if(strcasecmp(string,"evt_status_poll")==0) {

    T_STR srvnode = server.Node();

    TipcMsg status((T_STR)"evt_status");
    status.Dest((T_STR)"/evt_system/status");
    status.Sender(unique_id);
    sprintf(temp,"et:%s",et_station_name);
    status << unique_id << getenv("HOST") << session << srvnode 
	   << (T_INT4) event_monitor_.nevet << et_rate 
	   << (T_INT4) event_monitor_.nevproc << proc_rate
	   << (T_INT4) et_ok << temp;
    server.Send(status,TRUE);
    server.Flush();


  // reset histograms
  } else if (strcasecmp(string,"reset_hist")==0) {
    reset_hist_("ALL",3);


  //  don't understand message...ship to smartsockets interpreter
  } else {
    TutCommandParseStr(string);
  }

  return;
}
#endif

//----------------------------------------------------------------------


void decode_command_line(int argc, char**argv) {
  
  const char *help = 
    "\nusage:\n\n  event_monitor [-a application] [-s session] [-u uniq_name] [-g global_sect] [-i input_file]\n"
    "          [-stat et_station_name] [-adir archive_dir] [-at archive_time] [-ae archive_evt\n"
    "          [-m max_event] [-r(eset_hist_bor)] [-no_archive] [-no_snapshot] [-wait wait_time] [-debug]\n"
    "          [-no_cc] [-no_dc] [-no_ec] [-no_sc] [-no_st] [-no_ic] [-no_lac] [-no_rast] [-no_tpc]\n"
    "          [-no_trig] [-no_tg] [-no_photon] [-no_call] [-no_l2] [-no_scaler] [-no_timeline] [-no_hist]\n";
  
  
  
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
      else if (strncasecmp(argv[i],"-no_archive",11)==0) {
	no_archive=1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_snapshot",12)==0) {
	no_snapshot=1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-adir",5)==0) {
	archive_dir=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-stat",5)==0) {
	et_station_name=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-no_cc",6)==0) {
	event_monitor_.no_cc=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_dc",6)==0) {
	event_monitor_.no_dc=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_ec",6)==0) {
	event_monitor_.no_ec=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_sc",6)==0) {
	event_monitor_.no_sc=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_st",6)==0) {
	event_monitor_.no_st=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_ic",6)==0) {
	event_monitor_.no_ic=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_lac",7)==0) {
	event_monitor_.no_lac=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_trig",8)==0) {
	event_monitor_.no_trig=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_tg",6)==0) {
	event_monitor_.no_tg=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_photon",10)==0) {
	event_monitor_.no_photon=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_call",8)==0) {
	event_monitor_.no_call=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_l2",6)==0) {
	event_monitor_.no_l2=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_scaler",10)==0) {
	event_monitor_.no_scaler=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_timeline",12)==0) {
	event_monitor_.no_timeline=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_hist",8)==0) {
	event_monitor_.no_hist=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_rast",8)==0) {
	event_monitor_.no_rast=-1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_tpc",7)==0) {
        event_monitor_.no_tpc=-1;
        i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_tpe",7)==0) {
        event_monitor_.no_tpe=-1;
        i=i+1;
      }
      else if (strncasecmp(argv[i],"-wait",5)==0) {
	wait_time=atoi(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-at",3)==0) {
	archive_time=atoi(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-ae",3)==0) {
	archive_evt=atoi(argv[i+1]);
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
      else if (strncasecmp(argv[i],"-m",2)==0) {
	max_event=atoi(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-i",2)==0) {
	evt_from_file=1;
	input_file=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-g",2)==0) {
	no_global=0;
	global_sect=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-r",2)==0) {
	reset_hist_bor=1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-",1)==0) {
	cout << "Unknown command line arg: " << argv[i] 
	     << argv[i+1] << endl << endl;
	i=i+2;
      }
    }
  
  return;
}


//---------------------------------------------------------------------


void decode_hbook_id(const char *cidin, long *id, char *dir) {


  int last_slash=0;
  int id_exists=0;
  char *idp;
  char *dp;
  char *cid = strdup(cidin);


  /* find last slash in cid, set id pointer, and decode id...0 if no id */
  *id=0;
  for(last_slash=strlen(cid); (last_slash>=0)&&((cid+last_slash)[0]!='/'); last_slash--);
  idp=(char*)cid+last_slash+1;
  if( ((strlen(idp)>0)&&(isdigit(*idp)!=0)) || 
      ((strlen(idp)>1)&&(strncmp(idp,"-",1)==0)&&(isdigit(*(idp+1))!=0)) ) {
    id_exists=1;
    sscanf(idp,"%d",id);
  }
  

  /* returned dir always start with //pawc */
  strcat(dir,"//PAWC");


  /* null, //, //pawc, //pawc/, id */
  if(
     (strlen(cid)<=0)         	   	    ||
     (strcmp(cid,"//")==0)    	   	    ||  
     (strcasecmp(cid,"//pawc")==0) 	    ||    
     (strcasecmp(cid,"//pawc/")==0)         || 
     ((last_slash<0)&&(id_exists!=0)) )
    return;


  /* more than just //pawc, add slash */
  strcat(dir,"/");


  /* get pointer to after //pawc stuff and reset last_slash */
  dp=cid;
  if(strncasecmp(dp,"//PAWC",6)==0){
    dp=cid+6;
    last_slash-=6;
  }

  /* move dir pointer if leading slash */
  if(strncmp(dp,"/",1)==0)dp++;


  /* append remainder excluding id */
  if(id_exists==0){
    strcat(dir,dp);
  } else {
    strncat(dir,dp,last_slash);
  }


  /* chop off trailing / if it exists */
  if(dir[strlen(dir)-1]=='/')dir[strlen(dir)-1]='\0';


  /* convert dir to upper case */
  cltou_(dir,strlen(dir));


  return;

}


//------------------------------------------------------------------------

