//  event_statistics_monitor

//  collects events stats and sends them to info_server and file

//  still to do:
//     don't need structure, other minor mods

//  ejw, 20-may-99


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for smartsockets
#include <rtworks/cxxipc.hxx>


// system stuff
#include <fstream.h>
#include <iomanip.h>
#include <time.h>
#include <pthread.h>


// for et
extern "C" {
#include <et.h>
}


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
static char *application      = (char*)"clastest";
static char *session          = NULL;
static char *path             = getenv("CLON_PARMS");
static char *uniq_name        = (char*)"evstatmon";
static char *unique_id        = (char *) malloc(64);
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
static time_t delta;
static int status,i,bank,evlen,ind,nwrds;
static char *name;
static char event_source[128];
static char file_name[200];
static char temp[200];
FILE *trigout;


// et stuff
static char *et_station_name        = (char*)"EVSTATMON";
static et_openconfig openconfig;
static et_sys_id et_system_id;
static char et_filename[128];
static et_stat_id et_station_id;
static et_statconfig et_station_config;
static et_att_id et_attach_id;
static et_event *et_event_ptr;
static int et_control[ET_STATION_SELECT_INTS];


//  means, word counts, etc.
static T_INT4 event_num          = 0;
static T_INT4 dc[6],ec[6],ec1[6],sc[6],cc[6];
static T_INT4 ndc[6],nec[6],nec1[6],nsc[6],ncc[6];
static T_REAL8 dc_sec_sum,ec_sec_sum,sc_sec_sum,cc_sec_sum,ec1_sec_sum;
static T_REAL8 sec1_sum,sec2_sum,sec3_sum,sec4_sum,sec5_sum,sec6_sum;
static T_REAL8 tot;
struct {
  T_REAL8 dc[6];
  T_REAL8 ec[6];
  T_REAL8 sc[6];
  T_REAL8 cc[6];
  T_REAL8 ec1[6];
  T_REAL8 dcb[6];
  T_REAL8 ecb[6];
  T_REAL8 scb[6];
  T_REAL8 ccb[6];
  T_REAL8 ec1b[6];
} wrdct;
static T_INT4 dcr[6][3];


// prototypes
void decode_command_line(int argc, char **argv);
void init_et();
void connect_et();
void process_events(void);
void extract_stats(long *p2ctl, long *p2da);
void output_stats();
void print_results(FILE *fp);
void init_tcl(void);
void sum_stats(void);
void reset_stats(void);
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
  int r1=0,r2=0;


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line
  decode_command_line(argc,argv);


  // get status file_name
  sprintf(file_name,"%s/status/evstatmon.txt",path);
  

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
#ifdef SunOS
  thr_setconcurrency(thr_getconcurrency()+4);
#endif
  if(!TipcInitThreads()) {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }


  // set ipc parameters, connect to server, etc.
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
  ipc_set_control_message_callback(control_message_callback);
  status=ipc_init(unique_id,"event statistics monitor");
  if(status<0) {
    cerr << "\n?Unable to connect to server..."
	 << "probably duplicate unique id\n"
	 << "   ...check for another event_statistics_monitor connected to "
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
  sprintf(temp,"Process startup:    %15s  in application:  %s",unique_id,application);
  status=insert_msg("event_statistics_monitor",unique_id,unique_id,"status",0,"START",0,temp);


  // reset stats
  reset_stats();
	    

  // flush outgoing messages and pending output
  server.Flush();
  cout << flush;


  // launch ipc thread
  if(pthread_create(&t1,NULL,control_thread,(void *)&r1)!=0) {
    cerr << "\nUnable to create ipc thread" << endl << endl;
    exit(EXIT_FAILURE);
  }


  // launch event thread
  if(pthread_create(&t2,NULL,process_events,(void *)&r2)!=0) {
    cerr << "\nUnable to create control thread" << endl << endl;
    exit(EXIT_FAILURE);
  }


  // wait for control thread to terminate
  pthread_join(t1,NULL);

  
  // post shutdown message
  sprintf(temp,"Process shutdown:  %15s",unique_id);
  status=insert_msg("event_statistics_monitor",unique_id,unique_id,"status",0,"STOP",0,temp);


  // done
  et_forcedclose(et_system_id);
  ipc_close();
  exit(EXIT_SUCCESS);

}


//--------------------------------------------------------------------------


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

      // sum,output,reset stats 
      sum_stats();
      output_stats();
      reset_stats();
    }

  }

  return (void *)0;
}


//------------------------------------------------------------------------


void init_et() {
  

  et_ok=0;


  // create et file name
  sprintf(et_filename,"/tmp/et_sys_%s",session);
  et_open_config_init(&openconfig);


  // create station config in case no station exists
  et_station_config_init(&et_station_config);
  et_station_config_setblock(et_station_config,ET_STATION_NONBLOCKING);
  et_station_config_setselect(et_station_config,ET_STATION_SELECT_ALL);
  et_station_config_setuser(et_station_config,ET_STATION_USER_SINGLE);
  et_station_config_setrestore(et_station_config,ET_STATION_RESTORE_OUT);
  et_station_config_setcue(et_station_config,100);
  et_station_config_setprescale(et_station_config,1);


  return;
}


//--------------------------------------------------------------------------


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


  // success
  et_ok=1; 
  cout << "...now connected to ET system: " << et_filename 
       << ",   station: " << et_station_name << endl;

  return;
}


//--------------------------------------------------------------------------


void *process_events(void *param) {

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
      return (void *)0;
    }	
    
    
    // et system ok...get event
    if(et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_SLEEP,NULL)!=ET_OK) {
      cerr << "?error return from et_event_get" << endl;
      done=1;
      return (void *)0;
    }


    // get pointers
    et_event_getcontrol(et_event_ptr,et_control);
    et_event_getdata(et_event_ptr,(void**)&evt);
    nevet++;
    event_num++;
    
    
    // get stats
    nevproc++;
    extract_stats((long*)et_control,evt);
    

    // put event back
    et_event_put(et_system_id,et_attach_id,et_event_ptr);
    
  }


  return (void *)0;
}


//-------------------------------------------------------------------


void extract_stats(long *p2ctl, long *p2da) {

    int sector,layer,region;
    short *s;


  // ignore scaler and control events
  if((*p2ctl<1)||(*p2ctl>15))return;


  // loop over all banks
  ind = 11;
  evlen = p2da[10] + 11;
  while (ind<evlen) {
    
    name   = (char *)&p2da[ind+1];
    bank   = p2da[ind+3];
    nwrds  = p2da[ind+8];

    // bombproof against empty events, check for unusual bank size
    if(nwrds<=0)break;
    if((debug!=0)&&(nwrds>2000)) {
      char cname[5]; cname[4]='\0'; strncpy(cname,name,4);
      cerr << "found " << nwrds << " in bank " << cname << " number " << bank << endl;
    }
    

    sector = bank;


    // collect stats
    if (strncmp(name,"HEAD",4)==0) {
      run=p2da[ind+11];
    } else if (strncmp(name,"DC0 ",4)==0) {
      ndc[sector-1]++;
      dc[sector-1] += nwrds;
    } else if(strncmp(name,"EC  ",4)==0) {
      nec[sector-1]++;
      ec[sector-1] += nwrds;
    } else if(strncmp(name,"SC  ",4)==0) {
      nsc[sector-1]++;
      sc[sector-1] += nwrds;
    } else if(strncmp(name,"CC  ",4)==0) {
      ncc[sector-1]++;
      cc[sector-1] += nwrds;
    } else if(strncmp(name,"EC1 ",4)==0) {
      nec1[sector-1]++;
      ec1[sector-1] += nwrds;
    }	  
    
    // index of 1st data word
    ind  += p2da[ind];


    // count DC his (16 bit) by region
    if (strncmp(name,"DC0 ",4)==0) {
	s=(short *)&p2da[ind];
	for(i=0; i<2*nwrds; i++) {
	    layer=s[i]>>8;
	    region=layer/12+1;
	    if((region>0)&&(region<4))dcr[sector-1][region-1]++;
	}
    }


    // next bank
    ind += nwrds;
  }
  
  return;
}


//-------------------------------------------------------------------


void sum_stats() {

    int i;
    
    
    for(i=0;i<6;i++) {
	wrdct.dc[i]   = (event_num>0)?(((double)dc[i])/event_num):0;
	wrdct.sc[i]   = (event_num>0)?(((double)sc[i])/event_num):0;
	wrdct.cc[i]   = (event_num>0)?(((double)cc[i])/event_num):0;
	wrdct.ec[i]   = (event_num>0)?(((double)ec[i])/event_num):0;
	wrdct.ec1[i]  = (event_num>0)?(((double)ec1[i])/event_num):0;
	wrdct.dcb[i]  = (ndc[i]>0)?(((double)dc[i])/ndc[i]):0.;
	wrdct.scb[i]  = (nsc[i]>0)?(((double)sc[i])/nsc[i]):0.;
	wrdct.ccb[i]  = (ncc[i]>0)?(((double)cc[i])/ncc[i]):0.;
	wrdct.ecb[i]  = (nec[i]>0)?(((double)ec[i])/nec[i]):0.;
	wrdct.ec1b[i] = (nec1[i]>0)?(((double)ec1[i])/nec1[i]):0.;
    }
    
    
    for(i=0;i<6;i++) {
	dc_sec_sum +=wrdct.dc[i];
	ec_sec_sum +=wrdct.ec[i];
	sc_sec_sum +=wrdct.sc[i];
	cc_sec_sum +=wrdct.cc[i];
	ec1_sec_sum +=wrdct.ec1[i];
    }


    sec1_sum =wrdct.dc[1]+wrdct.ec[1]+wrdct.sc[1]+wrdct.cc[1]+wrdct.ec1[1];
    sec2_sum =wrdct.dc[2]+wrdct.ec[2]+wrdct.sc[2]+wrdct.cc[2]+wrdct.ec1[2];
    sec3_sum =wrdct.dc[3]+wrdct.ec[3]+wrdct.sc[3]+wrdct.cc[3]+wrdct.ec1[3];
    sec4_sum =wrdct.dc[4]+wrdct.ec[4]+wrdct.sc[4]+wrdct.cc[4]+wrdct.ec1[4];
    sec5_sum =wrdct.dc[5]+wrdct.ec[5]+wrdct.sc[5]+wrdct.cc[5]+wrdct.ec1[5];
    sec6_sum =wrdct.dc[6]+wrdct.ec[6]+wrdct.sc[6]+wrdct.cc[6]+wrdct.ec1[6];

    tot = dc_sec_sum + ec_sec_sum + sc_sec_sum + cc_sec_sum + ec1_sec_sum; 

    
    return;
}


//----------------------------------------------------------------


void output_stats() {

  int i;


  // info server message
  if(no_info==0) {
    TipcMsg msg = TipcMsg((T_STR)"info_server");
    msg.Dest((T_STR)"info_server/in/event_statistics_monitor");
    msg << (T_STR)"event_statistics_monitor";
    msg << (T_INT4)run << (T_INT4)time(NULL) << (T_INT4)event_num;
    msg 
      << SetSize(6) << ndc  << SetSize(6) << nec << SetSize(6) << nsc
      << SetSize(6) << ncc  << SetSize(6) << nec1;
    msg 
      << SetSize(6) << dc  << SetSize(6) << ec  << SetSize(6) << sc
      << SetSize(6) << cc  << SetSize(6) << ec1;
    msg 
      << SetSize(6) << wrdct.dc  << SetSize(6) << wrdct.ec  << SetSize(6) << wrdct.sc
      << SetSize(6) << wrdct.cc  << SetSize(6) << wrdct.ec1;
    msg 
      << SetSize(6) << wrdct.dcb  << SetSize(6) << wrdct.ecb  << SetSize(6) << wrdct.scb
      << SetSize(6) << wrdct.ccb  << SetSize(6) << wrdct.ec1b;
    msg << SetSize(18) << (T_INT4*)dcr;
    msg << dc_sec_sum << ec_sec_sum << sc_sec_sum << cc_sec_sum << ec1_sec_sum;
    msg << sec1_sum << sec2_sum << sec3_sum << sec4_sum << sec5_sum << sec6_sum;
    msg << tot;
    server.Send(msg,TRUE);
    server.Flush();
  }      
  
  
  // update file
  if(no_file==0) {
    trigout = fopen(file_name,"w+");
    print_results(trigout);
    fclose(trigout);
  }

  
  // dump results if requested
  if(dump==1) {
    print_results(stdout);
  }
  
  
  return;
}


//-------------------------------------------------------------------


void reset_stats() {

  int i,j;
  time_start = time(NULL);
  event_num   = 0;
  
  dc_sec_sum  = 0.;
  ec_sec_sum  = 0.;
  sc_sec_sum  = 0.;
  cc_sec_sum  = 0.;
  ec1_sec_sum = 0.;
  tot         = 0.;
  
  sec1_sum    = 0.;
  sec2_sum    = 0.;
  sec3_sum    = 0.;
  sec4_sum    = 0.;
  sec5_sum    = 0.;
  sec6_sum    = 0.;
  
  for(i=0;i<6;i++) {
    ndc[i]    = 0;
    dc[i]     = 0;
    nsc[i]    = 0;
    sc[i]     = 0;
    ncc[i]    = 0;
    cc[i]     = 0;
    nec[i]    = 0;
    ec[i]     = 0;
    nec1[i]   = 0;
    ec1[i]    = 0;
    for(j=0; j<3; j++)dcr[i][j]=0;
  }

  return;
}


//----------------------------------------------------------------


void print_results(FILE *fp) {

  int i;
  time_t now = time(NULL);             

 
  if(fp==stdout)fprintf(fp,"\n\n#-------------------------------------------------------------\n");
  fprintf(fp,"\n# Created by event_statistics_monitor on %s",ctime(&now));
  fprintf(fp,"#     EJW, VHG, 4-mar-1998\n\n");


  fprintf(fp,"\n*TIME* --- unix time\n");
  fprintf(fp,"%d\n\n",time(NULL));
 

  fprintf(fp,"\n*RUN* --- run number\n");
  fprintf(fp,"%d\n\n",run);


  fprintf(fp,"\n*TOTALS* --- Total number of data words for %d events\n",event_num);
  fprintf(fp,"#   DC     EC     SC     CC    EC1\n");
  fprintf(fp,"#   --     --     --     --    ---\n");
  for(i=0;i<6;i++) fprintf(fp,"%6d %6d %6d %6d %6d\n",
			   dc[i],ec[i],sc[i],cc[i],ec1[i]);
  

  fprintf(fp,"\n*BANKS* --- Total number of banks\n");
  fprintf(fp,"#   DC     EC     SC     CC    EC1\n");
  fprintf(fp,"#   --     --     --     --    ---\n");
  for(i=0;i<6;i++) fprintf(fp,"%6d %6d %6d %6d %6d \n",
			   ndc[i],nec[i],nsc[i],ncc[i],nec1[i]);
  

  fprintf(fp,"\n*BOCCUPS* --- Fractional bank occupancy per event\n");
  fprintf(fp,"#    DC      EC      SC      CC     EC1\n");
  fprintf(fp,"#    --      --      --      --     ---\n");
  for(i=0;i<6;i++) fprintf(fp,"%7.2f %7.2f %7.2f %7.2f %7.2f \n",
			   (event_num>0)?(((double)ndc[i])/event_num):0,
			   (event_num>0)?(((double)nec[i])/event_num):0,
			   (event_num>0)?(((double)nsc[i])/event_num):0,
			   (event_num>0)?(((double)ncc[i])/event_num):0,
			   (event_num>0)?(((double)nec1[i])/event_num):0);
  

  fprintf(fp,"\n\n*EMEANS* --- Mean number of data words per event\n");
  fprintf(fp,"#    DC      EC      SC      CC     EC1\n");
  fprintf(fp,"#    --      --      --      --     ---\n");
  for(i=0;i<6;i++) fprintf(fp,"%7.2f %7.2f %7.2f %7.2f %7.2f \n",
			   wrdct.dc[i],wrdct.ec[i],wrdct.sc[i],wrdct.cc[i],wrdct.ec1[i]);

  
  fprintf(fp,"\n*BMEANS* --- Mean number of data words per bank\n");
  fprintf(fp,"#    DC      EC      SC      CC     EC1\n");
  fprintf(fp,"#    --      --      --      --     ---\n");
  for(i=0;i<6;i++) fprintf(fp,"%7.2f %7.2f %7.2f %7.2f %7.2f \n",
			   wrdct.dcb[i],wrdct.ecb[i],wrdct.scb[i],wrdct.ccb[i],wrdct.ec1b[i]);
  

  fprintf(fp,"\n*MDET* --- Mean number of data words per event summed over sectors\n");
  fprintf(fp,"#    DC      EC      SC      CC     EC1\n");
  fprintf(fp,"#    --      --      --      --     ---\n");
  fprintf(fp,"%7.2f %7.2f %7.2f %7.2f %7.2f \n",
	  dc_sec_sum,ec_sec_sum,sc_sec_sum,cc_sec_sum,ec1_sec_sum); 
  

  fprintf(fp,"\n*MSEC* --- Mean number of data words per event summed over detectors\n");
  fprintf(fp,"#    S1      S2      S3      S4     S5     S6\n");
  fprintf(fp,"#    --      --      --      --     --     --\n");
  fprintf(fp,"%7.2f %7.2f %7.2f %7.2f %7.2f %7.2f\n",
	  sec1_sum,sec2_sum,sec3_sum,sec4_sum,sec5_sum,sec6_sum); 
  

  fprintf(fp,"\n\n*MTOT* --- Mean number of data words per event for entire detector\n");
  fprintf(fp,"%7.2f\n",tot);


  fprintf(fp,"\n\n*DCREGIONS* --- Mean number of DC hits per region for %d events\n",event_num);
  fprintf(fp,"#    R1      R2      R3\n");
  fprintf(fp,"#    --      --      --\n");
  for(i=0;i<6;i++) fprintf(fp,"%7.2f %7.2f %7.2f\n",
			   (event_num>0)?(((double)dcr[i][0])/event_num):0,
			   (event_num>0)?(((double)dcr[i][1])/event_num):0,
			   (event_num>0)?(((double)dcr[i][2])/event_num):0  );

  
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


void status_poll_callback(T_IPC_MSG msg) {
  
  
  TipcMsgAppendStr(msg,(T_STR)"Session");
  TipcMsgAppendStr(msg,session);
  
  TipcMsgAppendStr(msg,(T_STR)"Total number of events received");
  TipcMsgAppendInt4(msg,nevet);
  
  TipcMsgAppendStr(msg,(T_STR)"Total number of events processed");
  TipcMsgAppendInt4(msg,nevproc);
  
  TipcMsgAppendStr(msg,(T_STR)"Number of events this interval");
  TipcMsgAppendInt4(msg,event_num);
  
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
  Tcl_LinkVar(interp,(char*)"application",  (char *)&application, TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"unique_id",    (char *)&unique_id,   TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"session",      (char *)&session,     TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"debug",        (char *)&debug,       TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"dump",         (char *)&dump,        TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"no_file",      (char *)&no_file, 	  TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"no_info",      (char *)&no_info, 	  TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"event_num",    (char *)&event_num,   TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nevet",        (char *)&nevet,   	  TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nevproc",      (char *)&nevproc,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  
  return;
}


//--------------------------------------------------------------------------


void decode_command_line(int argc, char**argv) {
  
  const char *help = 
    "\nusage:\n\n  event_statistics_monitor [-a application] [-s session] [-u uniq_name] \n"
    "          [-debug] [-no_file] [-dump] [-no_info]\n";
  
  
  
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
      else if (strncasecmp(argv[i],"-no_file",6)==0) {
	no_file=1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-no_info",6)==0) {
	no_info=1;
	i=i+1;
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
      else if (strncasecmp(argv[i],"-",1)==0) {
	cout << "Unknown command line arg: " << argv[i] 
	     << argv[i+1] << endl << endl;
	i=i+2;
      }
    }
  
  return;
}


//----------------------------------------------------------------
