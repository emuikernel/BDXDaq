h34634
s 00007/00005/00901
d D 1.8 07/10/12 15:15:57 boiarino 9 8
c *** empty log message ***
e
s 00002/00000/00904
d D 1.7 03/12/16 23:35:03 boiarino 8 7
c .
e
s 00002/00002/00902
d D 1.6 02/10/23 13:05:51 wolin 7 6
c Minor
e
s 00021/00022/00883
d D 1.5 02/10/11 16:18:45 wolin 6 5
c Minor
e
s 00005/00005/00900
d D 1.4 02/10/11 13:45:53 wolin 5 4
c Typo, other minor mods
e
s 00178/00130/00727
d D 1.3 02/10/11 10:24:57 wolin 4 3
c Basically working, need more checks, need alarms
e
s 00307/00070/00550
d D 1.2 02/08/29 15:33:06 wolin 3 1
c Still working on it...
e
s 00000/00000/00000
d R 1.2 02/08/28 15:40:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 scalers/s/scaler_statistics_monitor.cc
e
s 00620/00000/00000
d D 1.1 02/08/28 15:40:01 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
//  scaler_statistics_monitor

D 4
//  analyzes scaler stats
E 4
I 4
//  analyzes scalers, prints archive file
E 4

I 4
D 6
// msg if count non-zero
// new scaler bank


E 6
E 4
//  ejw, 28-aug-2002


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for smartsockets
#include <rtworks/cxxipc.hxx>


// system stuff
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <time.h>
#include <pthread.h>


// for et
extern "C" {
#include <et.h>
I 3
#include <etbosio.h>
E 3
}


I 3
// for bos in et
static ETSYS *etsysptr;
static int trgs00,s1st00,rcst30;


E 3
// for tcl
extern "C"{
#include "tcl.h"
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


// for CLAS ipc
#include <clas_ipc_prototypes.h>


// misc variables
Tcl_Interp *interp;  
static char *init_tcl_script  = NULL;
D 6
static char *application      = (char*)"clastest";
E 6
I 6
static char *project          = (char*)"clastest";
E 6
static char *session          = NULL;
D 3
static char *path             = getenv("CLON_PARMS");
E 3
I 3
static char *clon_parms       = getenv("CLON_PARMS");
E 3
static char *uniq_name        = (char*)"scstatmon";
D 9
static char *unique_id        = (char *) malloc(64);
E 9
I 9
static char *unique_id        = (char *) malloc(512);
E 9
I 6
static int no_alarm           = 0;
E 6
static int no_file            = 0;
static int no_info            = 0;
static int debug              = 0;
static int dump               = 0;
static int done               = 0;
static int et_ok              = 0;
static time_t time_start      = time(NULL);

static int run                = 0;
static int nevet              = 0;
static int nevproc            = 0;
static double et_rate         = 0.;
static double proc_rate       = 0.;
static time_t last_time       = time(NULL);
static int last_et            = 0;
static int last_proc          = 0;
I 6
static int nalarm             = 0;
E 6
static time_t delta;
static char file_name[200];
static char temp[200];


// et stuff
static char *et_station_name        = (char*)"SCSTATMON";
static et_openconfig openconfig;
static et_sys_id et_system_id;
static char et_filename[128];
static et_stat_id et_station_id;
static et_statconfig et_station_config;
static et_att_id et_attach_id;
static et_event *et_event_ptr;
D 3
static int et_control[ET_STATION_SELECT_INTS];
E 3
I 3
static long *evt;
E 3


I 3
// scaler values
static int clock_rate         = 100000;
static int tbits_on,s1bits_on;
static unsigned long rc_tot,rc_phys,rc_scal,rc_ill,rc_psyn;
static unsigned long s1_phys,s1l1_zero,s1l1_empty,s1l1_notempty,s1l1_ok,s1_sum;
static unsigned long s1l2_pass,s1l2_latefail,s1l2_zero,s1l2_empty,s1l2_notempty,s1l2_ok,s113_count,s115_count,s1l1l2_zero_count,s113_l1_zero_count,s113_l2_zero_count,s113_l1l2_zero_count;
unsigned long trgs_tot,trgs_or_l1_live,trgs_l1accept,trgs_l2fail,trgs_l2pass,trgs_l2start,trgs_l2clear,trgs_l2accept,trgs_l3accept,trgs_sum;
static double deltat;
static double trgd_tot,trgd_l1accept,trgd_l2fail,trgd_l2pass,trgd_l2start,trgd_l2clear,trgd_l2accept,trgd_l3accept;
static double live_clock,live_fcup,live_trig,live_or,live_random;
I 4
static double live_trig_bit[12];
E 4


E 3
// prototypes
void decode_command_line(int argc, char **argv);
void init_et();
void connect_et();
void process_events(void);
D 3
void analyze_scalers(long *ctl, long *evt);
E 3
I 3
void analyze_scaler_event();
void get_scalers();
D 4
void check_scalers();
E 3
void output_stats();
E 4
I 4
void output_results();
E 4
void print_results(ostream &o);
I 4
void print_diagnostics(ostream &o);
E 4
void init_tcl(void);
extern "C"{
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
void *control_thread(void *param);
void *process_events(void *param);
void control_message_callback(T_IPC_CONN conn,
			      T_IPC_CONN_PROCESS_CB_DATA data,
			      T_CB_ARG arg);
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass,
	      int severity, const char *status, int code, const char *message);
}


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//--------------------------------------------------------------------------


int main(int argc, char **argv) {

  pthread_t t1,t2;
  int status;


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line
  decode_command_line(argc,argv);


D 3
  // get status file_name
  sprintf(file_name,"%s/status/scstatmon.txt",path);
  

E 3
  // get session name
  if(session==NULL)session=(char*)"clasprod";

  
  // create unique_id from uniq_name and session name
  strcpy(unique_id,uniq_name);
  strcat(unique_id,"_");
  strcat(unique_id,session);
  

  // get Tcl interp, create tcl commands, link vars, process Tcl startup script, etc.
  interp=tclinterp_init();
  init_tcl();
  if(init_tcl_script!=NULL)Tcl_EvalFile(interp,init_tcl_script);
  
  
  // init ipc thread package
I 9
#ifdef SunOS
E 9
  thr_setconcurrency(thr_getconcurrency()+4);
I 9
#endif
E 9
  if(!TipcInitThreads()) {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }


  // set ipc parameters, connect to server, etc.
D 6
  ipc_set_application(application);
E 6
I 6
  ipc_set_application(project);
E 6
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
  ipc_set_control_message_callback(control_message_callback);
  status=ipc_init(unique_id,"scaler statistics monitor");
  if(status<0) {
    cerr << "\n?Unable to connect to server..."
	 << "probably duplicate unique id\n"
	 << "   ...check for another scaler_statistics_monitor connected to "
	 << session << " using ipc_info\n"
	 << "   ...only one such process allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }
  server.SubjectSubscribe((T_STR)"evt_system",TRUE);


  // enable Tcl ipc control (i.e. create tcl_request callback)
  tclipc_init(interp);
  
  
  // init et structures, etc;
  init_et();


  // post startup message
D 6
  sprintf(temp,"Process startup:    %15s  in application:  %s",unique_id,application);
E 6
I 6
  sprintf(temp,"Process startup:    %15s  in project:  %s",unique_id,project);
E 6
  status=insert_msg("scaler_statistics_monitor",unique_id,unique_id,"status",0,"START",0,temp);


D 4
  // flush outgoing messages and pending output
E 4
I 4
  // flush pending outputs
E 4
  server.Flush();
  cout << flush;


  // launch ipc thread
  if(pthread_create(&t1,NULL,control_thread,(void *)0)!=0) {
D 4
    cerr << "\nUnable to create ipc thread" << endl << endl;
E 4
I 4
    cerr << "\nUnable to create control thread" << endl << endl;
E 4
    exit(EXIT_FAILURE);
  }


  // launch event thread
  if(pthread_create(&t2,NULL,process_events,(void *)0)!=0) {
D 4
    cerr << "\nUnable to create control thread" << endl << endl;
E 4
I 4
    cerr << "\nUnable to create event thread" << endl << endl;
E 4
    exit(EXIT_FAILURE);
  }


  // wait for control thread to terminate
  pthread_join(t1,NULL);

  
  // post shutdown message
  sprintf(temp,"Process shutdown:  %15s",unique_id);
  status=insert_msg("scaler_statistics_monitor",unique_id,unique_id,"status",0,"STOP",0,temp);


  // done
I 3
  etSysfree(etsysptr);
E 3
  et_forcedclose(et_system_id);
  ipc_close();
  exit(EXIT_SUCCESS);

}


//--------------------------------------------------------------------------


I 4
void init_et() {
  

  // get et library name
D 9
  char *et_user_library     = (char *) malloc(64);
  strcpy(et_user_library,getenv("CLON_LIB"));
  strcat(et_user_library,(char*)"/et_user_library.so");
E 9
I 9
  char *et_user_library     = (char *) malloc(512);
  strcpy(et_user_library,getenv("CODA_LIB"));
  strcat(et_user_library,(char*)"/libet_user.so");
E 9


  // create et file name
  sprintf(et_filename,"/tmp/et_sys_%s",session);
  et_open_config_init(&openconfig);


  // create station config in case no station exists
  et_station_config_init(&et_station_config);
  et_station_config_setblock(et_station_config,ET_STATION_BLOCKING);
  et_station_config_setselect(et_station_config,ET_STATION_SELECT_USER);
  et_station_config_setuser(et_station_config,ET_STATION_USER_SINGLE);
  et_station_config_setrestore(et_station_config,ET_STATION_RESTORE_OUT);
  et_station_config_setcue(et_station_config,100);
  et_station_config_setprescale(et_station_config,1);
  et_station_config_setlib(et_station_config,et_user_library);
  et_station_config_setfunction(et_station_config,(char*)"et_scaler_function");


  return;
}


//--------------------------------------------------------------------------


E 4
void *control_thread(void *param) {

  while(done==0) {

    server.MainLoop(1.0);


    // calc rates and output stats every 10 seconds
    delta=time(NULL)-last_time;
    if(delta>10) {
      et_rate =(double)(nevet-last_et)/delta;
      proc_rate=(double)(nevproc-last_proc)/delta;
      last_time=time(NULL);
      last_et=nevet;
      last_proc=nevproc;
D 3

      // sum,output,reset stats 
      output_stats();
E 3
    }

  }

  return (void *)0;
}


//------------------------------------------------------------------------


I 3
void *process_events(void *param) {

D 9
  int evsize;
E 9
I 9
  unsigned int evsize;
E 9


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
      return (void *)0;
    }	
    
    
    // et system ok...get event
    if(et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_SLEEP,NULL)!=ET_OK) {
      cerr << "?error return from et_event_get" << endl;
      done=1;
      return (void *)0;
    }


    // get pointers, etc.
    et_event_getdata(et_event_ptr,(void**)&evt);
    et_event_getlength(et_event_ptr,&evsize);
    nevet++;
    
    
    // give et buffer to bos
    etOpen((int*)evt,evsize/4-10,etsysptr);


    // analyze scaler event
    analyze_scaler_event();
    

    // put event back
    etClose((int*)evt);
    et_event_put(et_system_id,et_attach_id,et_event_ptr);
D 6
    
E 6
  }


  return (void *)0;
}


//-------------------------------------------------------------------


E 3
D 4
void init_et() {
  

  // get et library name
  char *et_user_library     = (char *) malloc(64);
  strcpy(et_user_library,getenv("CLON_LIB"));
  strcat(et_user_library,(char*)"/et_user_library.so");


  // create et file name
  sprintf(et_filename,"/tmp/et_sys_%s",session);
  et_open_config_init(&openconfig);


  // create station config in case no station exists
  et_station_config_init(&et_station_config);
  et_station_config_setblock(et_station_config,ET_STATION_BLOCKING);
  et_station_config_setselect(et_station_config,ET_STATION_SELECT_USER);
  et_station_config_setuser(et_station_config,ET_STATION_USER_SINGLE);
  et_station_config_setrestore(et_station_config,ET_STATION_RESTORE_OUT);
  et_station_config_setcue(et_station_config,100);
  et_station_config_setprescale(et_station_config,1);
  et_station_config_setlib(et_station_config,et_user_library);
  et_station_config_setfunction(et_station_config,(char*)"et_scaler_function");


  return;
}


//--------------------------------------------------------------------------


E 4
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


I 3
  // configure bos for et buffer
  etSysalloc(&etsysptr);



E 3
  // success
  et_ok=1; 
  cout << "...now connected to ET system: " << et_filename 
       << ",   station: " << et_station_name << endl;

  return;
}


//--------------------------------------------------------------------------


D 3
void *process_events(void *param) {
E 3
I 3
void analyze_scaler_event() {
E 3

D 3
  long *evt;
E 3

I 3
  // check if this is a scaler event
  if(strncasecmp((char*)(evt+3),"SCAL",4)==0) {
    nevproc++;
    run=evt[5];
    
E 3

D 3
  while(done==0) {
E 3
I 3
D 4
    // find banks
E 4
I 4
D 6
    // find scaler banks
E 6
I 6
    // extract scalers from banks, output results
E 6
E 4
    trgs00=etNlink((int*)evt,(char*)"TRGS",0);
    s1st00=etNlink((int*)evt,(char*)"S1ST",0);
    rcst30=etNlink((int*)evt,(char*)"RCST",30);
E 3
D 6


D 3
    // check et system
    if(et_ok==0) {
      connect_et();
      if(et_ok==0) {
	sleep(1);
	continue;
      }
    } else if(et_alive(et_system_id)==0) {
      done=1;
      return (void *)0;
    }	
    
    
    // et system ok...get event
    if(et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_SLEEP,NULL)!=ET_OK) {
      cerr << "?error return from et_event_get" << endl;
      done=1;
      return (void *)0;
    }
E 3
I 3
D 4
    // extract scalers from banks
    get_scalers();
E 4
I 4
    // extract scalers from banks output results
E 6
    if((trgs00>0)&&(s1st00>0)&&(rcst30>0)) {
	get_scalers();
	output_results();
    } else {
	cerr << "Missing scaler banks run " << run << ", event " << evt[6] << endl;
    }
E 4
E 3
D 6


E 6
D 3
    // get pointers
    et_event_getcontrol(et_event_ptr,et_control);
    et_event_getdata(et_event_ptr,(void**)&evt);
    nevet++;
    
    
    // analyze scalers
    analyze_scalers((long*)et_control,evt);
    
E 3
I 3
D 4
    // perform checks
    check_scalers();
E 3

D 3
    // put event back
    et_event_put(et_system_id,et_attach_id,et_event_ptr);
    
E 3
I 3

    // output summary
    output_stats();
E 4
E 3
  }

I 3
  return;
}
E 3

D 3
  return (void *)0;
E 3
I 3

//-------------------------------------------------------------------


void get_scalers() {

  int i;


  rc_tot  = evt[rcst30+6];
  rc_phys = evt[rcst30+7];
  rc_scal = evt[rcst30+8];
  rc_ill  = evt[rcst30+12];
  rc_psyn = evt[rcst30+14];
  
  s1_phys       = evt[s1st00+12];
  s1l1_zero     = evt[s1st00+13];
  s1l1_empty    = evt[s1st00+14];
  s1l1_notempty = evt[s1st00+15];
  s1l1_ok       = evt[s1st00+16];
D 4
  s1_sum        = 0;
E 4
  
  s1l2_pass           	= evt[s1st00+23];
  s1l2_latefail       	= evt[s1st00+24];
  s1l2_zero           	= evt[s1st00+25];
  s1l2_empty          	= evt[s1st00+26];
  s1l2_notempty       	= evt[s1st00+27];
  s1l2_ok             	= evt[s1st00+28];
  s113_count          	= evt[s1st00+29];
  s115_count          	= evt[s1st00+30];
  s1l1l2_zero_count     = evt[s1st00+31];
  s113_l1_zero_count    = evt[s1st00+32];
  s113_l2_zero_count    = evt[s1st00+33];
  s113_l1l2_zero_count  = evt[s1st00+34];
  
  trgs_tot       	= evt[trgs00+3*16+12];
  trgs_or_l1_live	= evt[trgs00+2*16+2];
  trgs_l1accept  	= evt[trgs00+3*16+13];
  trgs_l2fail    	= evt[trgs00+4*16+0];
  trgs_l2pass    	= evt[trgs00+4*16+1];
  trgs_l2start   	= evt[trgs00+4*16+2];
  trgs_l2clear   	= evt[trgs00+4*16+3];
  trgs_l2accept  	= evt[trgs00+4*16+4];
  trgs_l3accept  	= evt[trgs00+4*16+5];
D 4
  trgs_sum = 0;		
E 4
  			
  deltat                = (double)evt[trgs00+0*16+0]/clock_rate;
  trgd_tot       	= (double)evt[trgs00+3*16+12]/deltat;
  trgd_l1accept  	= (double)evt[trgs00+3*16+13]/deltat;
  trgd_l2fail    	= (double)evt[trgs00+4*16+0]/deltat;
  trgd_l2pass    	= (double)evt[trgs00+4*16+1]/deltat;
  trgd_l2start   	= (double)evt[trgs00+4*16+2]/deltat;
  trgd_l2clear   	= (double)evt[trgs00+4*16+3]/deltat;
  trgd_l2accept  	= (double)evt[trgs00+4*16+4]/deltat;
  trgd_l3accept  	= (double)evt[trgs00+4*16+5]/deltat;
  
D 4
  live_trig=(double)trgs_l1accept/trgs_tot;
  live_clock=(double)evt[trgs00+2*16+0]/evt[trgs00+1*16+0];
  live_random=(double)evt[trgs00+2*16+3]/evt[trgs00+1*16+3];
  live_fcup=(evt[trgs00+1*16+1]>0)?(double)evt[trgs00+2*16+1]/evt[trgs00+1*16+1]:0.;
  live_or=(double)trgs_or_l1_live/trgs_tot;
E 4
I 4
  live_trig   =	(double)trgs_l1accept/trgs_tot;
  live_clock  =	(double)evt[trgs00+2*16+0]/evt[trgs00+1*16+0];
  live_random =	(double)evt[trgs00+2*16+3]/evt[trgs00+1*16+3];
  live_fcup   =	(evt[trgs00+1*16+1]>0)?(double)evt[trgs00+2*16+1]/evt[trgs00+1*16+1]:0.;
  live_or     =	(double)trgs_or_l1_live/trgs_tot;
D 5
  for(i=0; i<12; i++) live_trig_bit[i]=(evt[trgs00+4*16+i]>0) ?
			  (double)evt[trgs00+5*16+i]/evt[trgs00+4*16+i]:0.;
E 5
I 5
  for(i=0; i<12; i++) live_trig_bit[i]=(evt[trgs00+3*16+i]>0) ?
			  (double)evt[trgs00+5*16+i]/evt[trgs00+3*16+i]:0.;
E 5
E 4

D 5

E 5
I 4
  s1_sum=0;
E 4
  s1bits_on=0;
  for(i=0; i<12; i++) {s1_sum+=evt[s1st00+i]; s1bits_on+=(evt[s1st00+i]!=0);}

I 5
D 6

E 6
E 5
I 4
  trgs_sum=0;
E 4
  tbits_on=0;
  for(i=0; i<12; i++) {trgs_sum+=evt[trgs00+3*16+i]; tbits_on+=(evt[trgs00+3*16+i]!=0);}

D 5

E 5
  return;
E 3
}


//-------------------------------------------------------------------


D 3
void analyze_scalers(long *ctl, long *evt) {
E 3
I 3
D 4
void check_scalers() {
E 4
I 4
void output_results() {
E 4
E 3

I 3
D 4
    if((rc_tot-rc_scal-rc_ill)!=(rc_phys+rc_psyn)) {
      cout << " *** ROC tot-scal-ill != phys+psync ***" << endl;
    }
    if((rc_phys+rc_psyn)!=s1_phys) {
      cout << " *** ROC phys+psync != s1 phys+psync ***" << endl;
    }
E 3

D 3
  // check if this is a scaler event
  if(strncasecmp((char*)(evt+3),"SCAL",4)==0) {
    nevproc++;
    run=evt[5];
E 3
I 3
    // s1st internal checks
    if(s1_phys!=(s1l1_ok+s1l1_zero+s1l1_empty)) {
      cout << " *** interrupts not equal ok+zero+empty ***" << endl;
    }
    if((s1bits_on==1)&&((s1_sum+s1l1_zero)!=s1_phys)) {
      cout << " *** s1 trigger bit sum+zero != s1 phys for single trigger ***" << endl;
    }
    if((s1l2_pass+s1l2_latefail)!=s1_phys) {
      cout << " *** s1 pass+latefail != s1 phys count ***" << endl;
    }
    if((s1bits_on==1)&&(s1_sum!=s1l1_ok)) {
      cout << " *** s1 trigger bit sum != s1 l1 ok for single trigger ***" << endl;
    }
  
    // trgs internal checks
    if((trgs_sum!=trgs_tot)&&(tbits_on==1)) {
      cout << " *** sum of l1 bits != or_level1 for single trigger *** " << endl;
    }
    if((trgs_l2fail+trgs_l2pass)!=trgs_l2start) {
      cout << " *** trgs l2pass+l2fail != trgs l2start *** " << endl;
    }
E 3
    
D 3
    // find s1st and trgs banks
E 3
    
D 3
    // perform checks
  }
E 3
I 3
    // compare trgs to s1st
    if(trgs_l2fail!=trgs_l2clear+s1l2_latefail) {
      cout << " *** trgs l2fail != (l2clear+latefail) *** " << endl;
    }
    if((trgs_l2pass+s1l2_latefail)!=s1_phys) {
      cout << " *** trgs (l2pass+latefail) doesn't agree with s1 interrupts *** " << endl;
    }
    if((trgs_l2pass+s1l2_latefail)!=trgs_l2accept) {
      cout << " *** trgs (l2pass+latefail) doesn't agree with l2 accepts *** " << endl;
    }
    if(trgs_l2accept!=trgs_l3accept) {
      cout << " *** trgs l2 accept doesn't agree with l3 accepts *** " << endl;
    }
    if(tbits_on!=s1bits_on) {
      cout << " *** trgs bits on != s1 bits on *** " << endl;
    }
E 3

D 3
  return;
E 3
I 3

    return;
E 3
}


//-------------------------------------------------------------------


void output_stats() {

E 4
  int i;


D 4
  // info server message
D 3
  if(no_info==0) {
    TipcMsg msg = TipcMsg((T_STR)"info_server");
    msg.Dest((T_STR)"info_server/in/scaler_statistics_monitor");
    msg << (T_STR)"scaler_statistics_monitor";
E 3
I 3
//    if(no_info==0) {
//      TipcMsg msg = TipcMsg((T_STR)"info_server");
//      msg.Dest((T_STR)"info_server/in/scaler_statistics_monitor");
//      msg << (T_STR)"scaler_statistics_monitor";
E 4
I 4
  // archive file
  if(no_file==0) {
    sprintf(file_name,"%s/scalers/archive/scstatmon_%s_%06d.txt",clon_parms,session,run);
    ofstream f(file_name);
    print_results(f);
    f.close();
  }
E 4
E 3

D 3
    // ??? 
E 3
I 3
D 4
//      // ??? 
E 3

D 3
    server.Send(msg,TRUE);
    server.Flush();
  }      
E 3
I 3
//      server.Send(msg,TRUE);
//      server.Flush();
//    }      
E 4
E 3
  
D 4
  
D 3
  // update file
E 3
I 3
  // file
E 4
I 4
  // current file
E 4
E 3
  if(no_file==0) {
I 3
D 4
    sprintf(file_name,"%s/scalers/archive/scstatmon_%s_%06d.txt",clon_parms,session,run);
E 4
I 4
    sprintf(file_name,"%s/scalers/scstatmon_%s.txt",clon_parms,session);
E 4
E 3
    ofstream f(file_name);
    print_results(f);
    f.close();
  }

  
D 3
  // dump results if requested
E 3
I 3
D 4
  // dump
E 4
I 4
D 6
  // dump to stdout
E 6
I 6
  // dump
E 6
E 4
E 3
  if(dump==1) {
    print_results(cout);
  }
  
  
I 4
  // info server message
  if(no_info==0) {
    TipcMsg msg = TipcMsg((T_STR)"info_server");
    msg.Dest((T_STR)"info_server/in/scaler_statistics_monitor");
    msg << (T_STR)"scaler_statistics_monitor";

I 8
    //    msg << ???;

E 8
    server.Send(msg,TRUE);
    server.Flush();
  }      
  
  
E 4
  return;
}


//-------------------------------------------------------------------


void print_results(ostream &o) {

I 4
    int i;
I 5
    time_t now=time(NULL);
E 5
E 4
D 3
  int i;
  time_t now = time(NULL);             
 
  o << "hello world" << endl;
E 3

I 4

E 4
I 3
D 5
  o << "\n\nScaler file analysis for run " << run << endl;
E 5
I 5
  o << "\n\nScaler analysis for run " << run << " on " << ctime(&now) << endl;
E 5


  o << "\n\nROC 30\n------" << endl << endl;
  o << "total trigs is:       " << setw(8) << rc_tot << endl;
  o << "illegal trig is:      " << setw(8) << rc_ill << endl;
  o << "scaler events is:     " << setw(8) << rc_scal << endl;
  o << "phys trig is:         " << setw(8) << rc_phys << endl;
  o << "phys sync is:         " << setw(8) << rc_psyn << endl;
  o << "phys + psyn is:       " << setw(8) << rc_phys+rc_psyn << endl;
  o << "tot-scal-illegal is:  " << setw(8) << rc_tot-rc_scal-rc_ill << endl;
  

  o << "\n\nS1ST\n----" << endl << endl;
  o << "s1 bits on is:        " << setw(8) << s1bits_on << endl;
  o << "phys + psync is:      " << setw(8) << s1_phys << endl;
  o << "l1 latch zero is:     " << setw(8) << s1l1_zero << "      ";
  o << "l2 latch zero is:     " << setw(8) << s1l2_zero << endl;
  o << "l1 latch empty is:    " << setw(8) << s1l1_empty << "      ";
  o << "l2 latch empty is:    " << setw(8) << s1l2_empty << endl;
  o << "l1 latch not empty:   " << setw(8) << s1l1_notempty << "      ";
  o << "l2 latch not empty:   " << setw(8) << s1l2_notempty << endl;
  o << "l1 latch ok is:       " << setw(8) << s1l1_ok << "      ";
  o << "l2 latch ok is:       " << setw(8) << s1l2_ok << endl;
  o << "s1 sum is:            " << setw(8) << s1_sum << "      ";
  o << "l2 pass is:           " << setw(8) << s1l2_pass << endl;
  o << "roc 13 is:            " << setw(8) << s113_count << "      ";
  o << "l2 latefail is:       " << setw(8) << s1l2_latefail << endl;
  o << "roc 15 is:            " << setw(8) << s115_count << endl;
  o << "13 l1 zero is:        " << setw(8) << s113_l1_zero_count << "      ";
  o << "13 l2 zero is:        " << setw(8) << s113_l2_zero_count << endl;
  o << "l1l2 zero is:         " << setw(8) << s1l1l2_zero_count << endl;
  o << "13 l1l2 zero is:      " << setw(8) << s113_l1l2_zero_count << endl;


  o << "\n\nTRGS\n----" << endl << endl;
  o << "bits on is:           " << setw(8) << tbits_on << endl;
  o << "or_level1 is:         " << setw(8) << trgs_tot
       << "   (" << trgd_tot << ")" << endl;
  o << "or_level1 live is:    " << setw(8) << trgs_or_l1_live << endl;
  o << "l1 sum is:            " << setw(8) << trgs_sum << endl;
  o << "l1accepts is:         " << setw(8) << trgs_l1accept
       << "   (" << trgd_l1accept << ")" << endl;
  o << "l2start is:           " << setw(8) << trgs_l2start
       << "   (" << trgd_l2start << ")" << endl;
  o << "l2clear is:           " << setw(8) << trgs_l2clear
       << "   (" << trgd_l2clear << ")" << endl;
  o << "l2pass is:            " << setw(8) << trgs_l2pass
       << "   (" << trgd_l2pass << ")" << endl;
  o << "l2fail is:            " << setw(8) << trgs_l2fail
       << "   (" << trgd_l2fail << ")" << endl;
  o << "l2accept is:          " << setw(8) << trgs_l2accept
       << "   (" << trgd_l2accept << ")" << endl;
  o << "l3accept is:          " << setw(8) << trgs_l3accept
       << "   (" << trgd_l3accept << ")" << endl;


  o << "\n\nLivetime\n--------" << endl << endl;
  o << "live trig is:         " << live_trig  
       << "   (" << trgs_l1accept << "/" << trgs_tot << ")" << endl;
  o << "live clock is:        " << live_clock 
       <<  "   (" << evt[trgs00+2*16+0] << "/" << evt[trgs00+1*16+0] << ")" << endl;
  o << "live fcup is:         " << live_fcup 
       <<  "   (" << evt[trgs00+2*16+1]  << "/" << evt[trgs00+1*16+1] << ")" << endl;
  o << "live or is:           " << live_or
       <<  "   (" << trgs_or_l1_live  << "/" << trgs_tot << ")" << endl;
  o << "live random is:       " << live_random
       <<  "   (" << evt[trgs00+2*16+3]  << "/" << evt[trgs00+1*16+3] << ")" << endl;
I 4
  o << endl << endl << endl;
E 4


I 4
D 6
  // live by trigger bits
E 6
I 6
  // live for each trigger bit
E 6
  for(i=0; i<12; i++) {
      sprintf(temp,"%6d  ",i+1);
      o << temp;
  }
  o << endl;
  for(i=0; i<12; i++) o << "------  ";
  o << endl;
  for(i=0; i<12; i++){
      sprintf(temp,"%6.3f  ",live_trig_bit[i]);
      o << temp;
  }
  o << endl;


E 4
  o << endl << endl;
 
I 4

  // print diagnostics
  print_diagnostics(o);


E 4
E 3
  return;
}


//-------------------------------------------------------------------


I 4
void print_diagnostics(ostream &o) {

D 7
    o << "Diagnostics\n" 
	 "-----------" << endl << endl;
E 7

I 7
    o << "Diagnostics\n-----------" << endl << endl;
E 7

I 7

E 7
    // rcst internal checks
    if((rc_tot-rc_scal-rc_ill)!=(rc_phys+rc_psyn)) {
      o << " *** ROC tot-scal-ill != phys+psync ***" << endl;
    }


    // compare rcst to s1st
    if((rc_phys+rc_psyn)!=s1_phys) {
      o << " *** ROC phys+psync != s1 phys+psync ***" << endl;
    }


    // s1st internal checks
    if(s1_phys!=(s1l1_ok+s1l1_zero+s1l1_empty)) {
      o << " *** interrupts not equal ok+zero+empty ***" << endl;
    }
    if((s1bits_on==1)&&((s1_sum+s1l1_zero)!=s1_phys)) {
      o << " *** s1 trigger bit sum+zero != s1 phys for single trigger ***" << endl;
    }
    if((s1l2_pass+s1l2_latefail)!=s1_phys) {
      o << " *** s1 pass+latefail != s1 phys count ***" << endl;
    }
    if((s1bits_on==1)&&(s1_sum!=s1l1_ok)) {
      o << " *** s1 trigger bit sum != s1 l1 ok for single trigger ***" << endl;
    }
    if((s113_count!=0)||(s115_count!=0)) {
      o << " *** s1 evtype 13 and/or evtype 15 count non-zero ***" << endl;
    }

  

    // trgs internal checks
    if((trgs_sum!=trgs_tot)&&(tbits_on==1)) {
      o << " *** sum of l1 bits != or_level1 for single trigger *** " << endl;
    }
    if((trgs_l2fail+trgs_l2pass)!=trgs_l2start) {
      o << " *** trgs l2pass+l2fail != trgs l2start *** " << endl;
    }
    
    
    // compare trgs to s1st
    if(trgs_l2fail!=trgs_l2clear+s1l2_latefail) {
      o << " *** trgs l2fail != (l2clear+latefail) *** " << endl;
    }
    if((trgs_l2pass+s1l2_latefail)!=s1_phys) {
      o << " *** trgs (l2pass+latefail) doesn't agree with s1 interrupts *** " << endl;
    }
    if((trgs_l2pass+s1l2_latefail)!=trgs_l2accept) {
      o << " *** trgs (l2pass+latefail) doesn't agree with l2 accepts *** " << endl;
    }
    if(trgs_l2accept!=trgs_l3accept) {
      o << " *** trgs l2 accept doesn't agree with l3 accepts *** " << endl;
    }
    if(tbits_on!=s1bits_on) {
      o << " *** trgs bits on != s1 bits on *** " << endl;
    }

    o << endl << endl;

    return;
}


//-------------------------------------------------------------------


E 4
void quit_callback(int sig) {
  
  // received signal or quit control command
  cout << "...stopping, received signal: " << sig << endl;
  done=1;
  
  return;
}


//----------------------------------------------------------------------


void status_poll_callback(T_IPC_MSG msg) {
  
  
D 4
  TipcMsgAppendStr(msg,(T_STR)"Session");
  TipcMsgAppendStr(msg,session);
  
E 4
D 3
  TipcMsgAppendStr(msg,(T_STR)"Total number of events received");
E 3
I 3
  TipcMsgAppendStr(msg,(T_STR)"Total number of scaler events received");
E 3
  TipcMsgAppendInt4(msg,nevet);
  
D 3
  TipcMsgAppendStr(msg,(T_STR)"Total number of events processed");
E 3
I 3
  TipcMsgAppendStr(msg,(T_STR)"Total number of scaler events processed");
E 3
  TipcMsgAppendInt4(msg,nevproc);
I 6

  TipcMsgAppendStr(msg,(T_STR)"Total number of alarms sent");
  TipcMsgAppendInt4(msg,nalarm);
E 6
  
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
	   << (T_INT4) nevet << et_rate << (T_INT4) nevproc << proc_rate
	   << (T_INT4) et_ok << temp;
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
D 6
  Tcl_LinkVar(interp,(char*)"application",  (char *)&application, TCL_LINK_STRING);
E 6
I 6
  Tcl_LinkVar(interp,(char*)"project",      (char *)&project,     TCL_LINK_STRING);
E 6
  Tcl_LinkVar(interp,(char*)"unique_id",    (char *)&unique_id,   TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"session",      (char *)&session,     TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"debug",        (char *)&debug,       TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"dump",         (char *)&dump,        TCL_LINK_BOOLEAN);
I 6
  Tcl_LinkVar(interp,(char*)"no_alarm",     (char *)&no_alarm, 	  TCL_LINK_BOOLEAN);
E 6
  Tcl_LinkVar(interp,(char*)"no_file",      (char *)&no_file, 	  TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"no_info",      (char *)&no_info, 	  TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"nevet",        (char *)&nevet,   	  TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nevproc",      (char *)&nevproc,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  
  return;
}


//--------------------------------------------------------------------------


void decode_command_line(int argc, char**argv) {
  
  const char *help = 
D 6
    "\nusage:\n\n  scaler_statistics_monitor [-a application] [-s session] [-u uniq_name] \n"
    "          [-debug] [-no_file] [-dump] [-no_info]\n";
E 6
I 6
    "\nusage:\n\n  scaler_statistics_monitor [-a project] [-s session] [-u uniq_name] \n"
    "          [-debug] [-no_file] [-no_alarm] [-no_info] [-dump]\n";
E 6
  
I 6
 
E 6
  
D 6
  
E 6
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
      else if (strncasecmp(argv[i],"-dump",5)==0) {
	dump=1;
	i=i+1;
      }
I 6
      else if (strncasecmp(argv[i],"-no_alarm",7)==0) {
	no_alarm=1;
	i=i+1;
      }
E 6
      else if (strncasecmp(argv[i],"-no_file",6)==0) {
	no_file=1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_info",6)==0) {
	no_info=1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-a",2)==0) {
D 6
	application=strdup(argv[i+1]);
E 6
I 6
	project=strdup(argv[i+1]);
E 6
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
