//  ipc2timeline
//
//  fills timeline hist with info server data
//   
//  Still to do:
//
//  ejw, 11-oct-2001


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for smartsockets
#include <rtworks/cxxipc.hxx>


// system
#include <fstream.h>
#include <iomanip.h>


// for tcl
extern "C" {
#include <tcl.h>
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


// for CLAS ipc
#include <clas_ipc_prototypes.h>


//  for timeline
extern "C" {
#include <timeline.h>
}

#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )


// misc variables
static char *project          = (char*)"clastest";
static char *session          = (char*)"clasprod";
static char *unique_id        = (char*)"ipc2timeline";
static char *msql_database    = (char*)"clasrun";
static Tcl_Interp *interp;
static char *init_tcl_script  = NULL;
static int clock_rate         = 100000;
static int min_evt            = 1000;
static int min_rate           = 100;
static int done               = 0;
static int no_timeline        = 0;
static int debug              = 0;
static char temp[512];


// stats...types MUST be in same order as callback arg
static const char *types[] = {"generic","evstatmon","evtstatus","diskcheck","evmon","scaler","systats00",
			      "systats10","alarm","trigmon"};
static int nmsgtot=0;
static int nentrytot=0;
static int nmsg[sizeof(types)/sizeof(char*)];
static int nentry[sizeof(types)/sizeof(char*)];
static int ntype = sizeof(types)/sizeof(char*);


// evstatmon
static const char *snam[6] = {"s1","s2","s3","s4","s5","s6"};
static const char *dnam[5] = {"dc","ec","sc","cc","ec1"};
static const char *rnam[3] = {"r1","r2","r3"};
static int nevstatmon      = 80;
static char *evstatmon_names[80];
static float evstatmon_vals[80];


// evtstatus
static const char *evtstatus_names[] = {
    "daq_rate","et2et_rate","mon_rate",
    "ipcbank_rate","evtstatmon_rate",
    "scaler_rate","evmon_rate","recsis_rate"
};
static float evtstatus_vals[sizeof(evtstatus_names)/sizeof(char*)];
static int nevtstatus          = sizeof(evtstatus_names)/sizeof(char*);


// diskcheck
static const char *diskcheck_names[] = {
  "clon00_slash","clon00_tmp",
  "clon10_slash","clon10_tmp",
  "ssa","home","usr_local","opt","apps",
  "work","var_mail","usr_clas_msql","usr_clas_cmlog",
  "usr_clas_archives","mnt_admin_clonfs","mnt_admin_hpdb",
  "active_count","empty_count","backed_up_count","not_backed_up_count","moving_count",
  "active_free",
  "RAID0_status","RAID1_status","RAID2_status","RAID3_status",
  "RAID4_status","RAID5_status","RAID6_status","RAID7_status"
};
static float diskcheck_vals[sizeof(diskcheck_names)/sizeof(char*)];
static int ndiskcheck = sizeof(diskcheck_names)/sizeof(char*);


// evmon
static const char *evmon_group="evmon_trig";
static const char *evmon_evt_name="evt_wds";
static const char *evmon_names[7][7] = {
  "tot_wds"   ,"sc_wds"   ,"ec_wds"   ,"cc_wds"   ,"dc_wds"   ,"lac_wds"   ,"tg_wds",
  "tot_wds_s1","sc_wds_s1","ec_wds_s1","cc_wds_s1","dc_wds_s1","lac_wds_s1","tg_wds_s1",
  "tot_wds_s2","sc_wds_s2","ec_wds_s2","cc_wds_s2","dc_wds_s2","lac_wds_s2","tg_wds_s2",
  "tot_wds_s3","sc_wds_s3","ec_wds_s3","cc_wds_s3","dc_wds_s3","lac_wds_s3","tg_wds_s3",
  "tot_wds_s4","sc_wds_s4","ec_wds_s4","cc_wds_s4","dc_wds_s4","lac_wds_s4","tg_wds_s4",
  "tot_wds_s5","sc_wds_s5","ec_wds_s5","cc_wds_s5","dc_wds_s5","lac_wds_s5","tg_wds_s5",
  "tot_wds_s6","sc_wds_s6","ec_wds_s6","cc_wds_s6","dc_wds_s6","lac_wds_s6","tg_wds_s6"
};


// scaler
static time_t scaler_update  = 60;  // seconds
static time_t last_scaler    = 0;
static const char *scaler_names[] = {
  "trig1_ug","trig2_ug","trig3_ug","trig4_ug","trig5_ug","trig6_ug",
  "trig7_ug","trig8_ug","trig9_ug","trig10_ug","trig11_ug","trig12_ug",
  "trig_or_ug","l1accept","l2fail","l2pass","l2start","l2clear",
  "l2accept","l3accept",
  "live_clock","live_fcup","live_trig","fcup_int","fcup_diff",
  "ec_sec1","ec_sec2","ec_sec3","ec_sec4","ec_sec5","ec_sec6",
  "sc_sec1","sc_sec2","sc_sec3","sc_sec4","sc_sec5","sc_sec6",
  "latch1_state_err","latch2_state_err",
  "l2spf","l2fcl","l2apl","l2acc_l3acc",
  "l2_s1","l2_s2","l2_s3","l2_s4","l2_s5","l2_s6","l2_or","l2_ok",
  "s1l1_b1","s1l1_b2","s1l1_b3","s1l1_b4","s1l1_b5","s1l1_b6",
  "s1l1_b7","s1l1_b8","s1l1_b9","s1l1_b10","s1l1_b11","s1l1_b12",
  "s1l1_rate",
  "s1l2_s1","s1l2_s2","s1l2_s3","s1l2_s4","s1l2_s5","s1l2_s6",
  "s1l2_pass","s1l2_fail",
  "live_or","live_random"
};
static float scaler_vals[sizeof(scaler_names)/sizeof(char*)];
static int nscaler = sizeof(scaler_names)/sizeof(char*);


// systats
static int ncpu00 	   = 6;
static int ncpu10 	   = 4;
static const char *systats_names[] = {
    "temp_free","swap_free","net_in","net_in_err","net_out","net_out_err",
    "net_collision","mem_free","cpu_idle_avg",
    "cpu_idle0","cpu_idle1","cpu_idle2","cpu_idle3","cpu_idle4","cpu_idle5"
};
static float systats_vals[sizeof(systats_names)/sizeof(char*)];
static int nsystats = sizeof(systats_names)/sizeof(char*)-MAX(ncpu10,ncpu00);


// alarm_checker
static const char *alarm_names[] = {
    "alarm_count"
};
static float alarm_vals[sizeof(alarm_names)/sizeof(char*)];
static int nalarm= sizeof(alarm_names)/sizeof(char*);



// trigmon
static const char *trigmon_names[] = {
    "s1_l2effic","s2_l2effic","s3_l2effic","s4_l2effic","s5_l2effic","s6_l2effic"
};
static float trigmon_vals[sizeof(trigmon_names)/sizeof(char*)];
static int ntrigmon= sizeof(trigmon_names)/sizeof(char*);


// subject callback prototypes
void generic_callback(T_IPC_CONN,
		      T_IPC_CONN_PROCESS_CB_DATA,
		      T_CB_ARG);
void evstatmon_callback(T_IPC_CONN,
			T_IPC_CONN_PROCESS_CB_DATA,
			T_CB_ARG);
void evtstatus_callback(T_IPC_CONN,
			T_IPC_CONN_PROCESS_CB_DATA,
			T_CB_ARG);
void diskcheck_callback(T_IPC_CONN,
			T_IPC_CONN_PROCESS_CB_DATA,
			T_CB_ARG);
void evmon_callback(T_IPC_CONN,
		    T_IPC_CONN_PROCESS_CB_DATA,
		    T_CB_ARG);
void scaler_callback(T_IPC_CONN,
		     T_IPC_CONN_PROCESS_CB_DATA,
		     T_CB_ARG);
void systats_callback(T_IPC_CONN,
		      T_IPC_CONN_PROCESS_CB_DATA,
		      T_CB_ARG);
void alarm_callback(T_IPC_CONN,
		    T_IPC_CONN_PROCESS_CB_DATA,
		    T_CB_ARG);
void trigmon_callback(T_IPC_CONN,
		      T_IPC_CONN_PROCESS_CB_DATA,
		      T_CB_ARG);


// prototypes
void decode_command_line(int argc, char **argv);
void init_tcl();
void create_callbacks();
void init_timeline();
extern "C" {
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
int tcl_help(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
int tcl_quit(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
int get_run_number(const char *msql_database, const char *session);
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *msgstatus, int code, const char *text);
}	


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//--------------------------------------------------------------------------


main(int argc,char **argv) {

  int status;


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line flags
  decode_command_line(argc,argv);


  // get Tcl interp, create tcl commands, link vars, process Tcl startup script, etc.
  interp=tclinterp_init();
  init_tcl();
  if(init_tcl_script!=NULL)Tcl_EvalFile(interp,init_tcl_script);


  // set ipc parameters and connect to ipc system
  ipc_set_application(project);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
  status=ipc_init(unique_id,"ipc to timeline hist");
  if(status<0) {
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another ipc2timeline using ipc_info\n"
	 << "   ...only one instance allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }


  // create message callbacks
  create_callbacks();


  // create tcl callback
  tclipc_init(interp);


  // init timeline stuff
  init_timeline();


  // post startup message
  sprintf(temp,"Process startup:  %15s in project %s",unique_id,project);
  status=insert_msg("ipc2timeline","online",unique_id,"status",0,"START",0,temp);


  // flush pending output
  fflush(NULL);
  

  // process info server messages
  while (done==0) {
    server.MainLoop(2.0);
  }


  // done
  ipc_close();
  sprintf(temp,"Process shutdown:  %15s",unique_id);
  status=insert_msg("ipc2timeline","online",unique_id,"status",0,"STOP",0,temp);
  exit(EXIT_SUCCESS);
}
       

//--------------------------------------------------------------------------


void create_callbacks() {



  // generic callback
  TipcMt mt_generic((char*)"ipc2timeline");
  server.SubjectSubscribe((char*)"ipc2timeline/generic",TRUE);
  server.SubjectCbCreate((char*)"ipc2timeline/generic",
			 mt_generic,generic_callback,(T_CB_ARG)0);


  // info_server callbacks
  TipcMt  mt((char*)"info_server");

  server.SubjectSubscribe((char*)"info_server/out/ipc2timeline/evstatmon",TRUE);
  server.SubjectCbCreate((char*)"info_server/out/ipc2timeline/evstatmon",
			 mt,evstatmon_callback,(T_CB_ARG)1);

  server.SubjectSubscribe((char*)"info_server/out/ipc2timeline/evtstatus",TRUE);
  server.SubjectCbCreate((char*)"info_server/out/ipc2timeline/evtstatus",
			 mt,evtstatus_callback,(T_CB_ARG)2);

  server.SubjectSubscribe((char*)"info_server/out/ipc2timeline/diskcheck",TRUE);
  server.SubjectCbCreate((char*)"info_server/out/ipc2timeline/diskcheck",
			 mt,diskcheck_callback,(T_CB_ARG)3);

  server.SubjectSubscribe((char*)"info_server/out/ipc2timeline/event_monitor",TRUE);
  server.SubjectCbCreate((char*)"info_server/out/ipc2timeline/event_monitor",
			 mt,evmon_callback,(T_CB_ARG)4);

  server.SubjectSubscribe((char*)"info_server/out/ipc2timeline/scaler",TRUE);
  server.SubjectCbCreate((char*)"info_server/out/ipc2timeline/scaler",
			 mt,scaler_callback,(T_CB_ARG)5);

  server.SubjectSubscribe((char*)"info_server/out/ipc2timeline/systats/clon00",TRUE);
  server.SubjectCbCreate((char*)"info_server/out/ipc2timeline/systats/clon00",
			 mt,systats_callback,(T_CB_ARG)6);

  server.SubjectSubscribe((char*)"info_server/out/ipc2timeline/systats/clon10",TRUE);
  server.SubjectCbCreate((char*)"info_server/out/ipc2timeline/systats/clon10",
			 mt,systats_callback,(T_CB_ARG)7);

  server.SubjectSubscribe((char*)"info_server/out/ipc2timeline/alarm_checker",TRUE);
  server.SubjectCbCreate((char*)"info_server/out/ipc2timeline/alarm_checker",
			 mt,alarm_callback,(T_CB_ARG)8);

  server.SubjectSubscribe((char*)"info_server/out/ipc2timeline/trigmon",TRUE);
  server.SubjectCbCreate((char*)"info_server/out/ipc2timeline/trigmon",
			 mt,trigmon_callback,(T_CB_ARG)9);


  server.Flush();

  return;
}


//--------------------------------------------------------------------------


void init_timeline() {

  int i,j;


  // clear counts
  for(i=0; i<ntype; i++) nmsg[i]=0,nentry[i]=0;


  // evstatmon
  for(i=0; i<5; i++) {
    for(j=0; j<6; j++) {
      sprintf(temp,"%s%s_banks",dnam[i],snam[j]);
      evstatmon_names[6*i+j]=strdup(temp);
      sprintf(temp,"%s%s_words",dnam[i],snam[j]);
      evstatmon_names[30+6*i+j]=strdup(temp);
    }
  }
  evstatmon_names[60]=strdup("event_count");
  evstatmon_names[61]=strdup("clas_words");

  for(i=0; i<6; i++) {
      for(j=0; j<3; j++) {
	  sprintf(temp,"DC%s%s_mean",snam[i],rnam[j]);
	  evstatmon_names[62+3*i+j]=strdup(temp);
      }
  }


  return;
}

  
//----------------------------------------------------------------


void status_poll_callback(T_IPC_MSG msg) {

  int i;


  // get stat totals
  nmsgtot=0;
  nentrytot=0;
  for(i=0; i<ntype; i++) nmsgtot+=nmsg[i],nentrytot+=nentry[i];


  TipcMsgAppendStr(msg,(char*)"Total number of messages received");
  TipcMsgAppendInt4(msg,nmsgtot);
  TipcMsgAppendStr(msg,(char*)"Total number of entries made");
  TipcMsgAppendInt4(msg,nentrytot);

  for(i=0; i<ntype; i++) {
    TipcMsgAppendStr(msg,(char*)"");
    TipcMsgAppendStr(msg,(char*)"");

    strcpy(temp,types[i]);
    strcat(temp," messages");
    TipcMsgAppendStr(msg,temp);
    TipcMsgAppendInt4(msg,nmsg[i]);

    strcpy(temp,types[i]);
    strcat(temp," entries");
    TipcMsgAppendStr(msg,temp);
    TipcMsgAppendInt4(msg,nentry[i]);
  }

  return;
}


//-------------------------------------------------------------------


void quit_callback(int sig) {


  cout << "...stopping...received signal " << sig << endl;
  done=1;
  return;
}


//-------------------------------------------------------------------


void decode_command_line(int argc, char**argv) {

  const char *help = "\nusage:\n\n  ipc2timeline [-a project] [-s session] [-u unique_id]\n"
    "             [-m msql_database] [-t init_tcl_script] [-clock clock_rate] [-min_evt min_evt]\n"
    "             [-min_rate min_rate] [-scal scaler_update] [-no_timeline] [-debug]\n";


  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0) {
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-clock",6)==0) {
      clock_rate=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-min_evt",8)==0) {
      min_evt=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-min_rate",9)==0) {
      min_rate=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-no_timeline",12)==0) {
      no_timeline=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-scal",5)==0) {
      scaler_update=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-a",2)==0) {
      project=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0) {
	session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0) {
      unique_id=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-m",2)==0) {
	msql_database=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-t",2)==0) {
      init_tcl_script=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-",1)==0) {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
      i=i+2;
    }
  }

  return;
}

  
//----------------------------------------------------------------


void init_tcl() {

  // link c and Tcl variables
  Tcl_LinkVar(interp,(char*)"project",       (char *)&project,        TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"unique_id",     (char *)&unique_id,      TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"session",       (char *)&session,        TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"msql_database", (char *)&msql_database,  TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"scaler_update", (char *)&scaler_update,  TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"clock_rate",    (char *)&clock_rate,     TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"min_evt",       (char *)&min_evt,        TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"min_rate",      (char *)&min_rate,       TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"debug",         (char *)&debug,          TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"no_timeline",   (char *)&no_timeline,    TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"nmsgtot",       (char *)&nmsgtot,        TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nentrytot",     (char *)&nentrytot,      TCL_LINK_INT|TCL_LINK_READ_ONLY);


  // create Tcl commands
  Tcl_CreateCommand(interp,(char*)"help",tcl_help,
  		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,(char*)"quit",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,(char*)"stop",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,(char*)"exit",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  return;

}


//--------------------------------------------------------------------------


int tcl_help(ClientData clientdata, Tcl_Interp *interp,
		int argc, char **argv) {

    const char *help =
    "\nTcl commands available in the alarm_handler program:\n\n"
    " help                  print this message\n"
    " stop                  stop program\n"
    " quit                  stop program\n"
    " exit                  stop program\n"
    "\n\n Type command that require args with NO args for more information\n"
    "\n";

    Tcl_SetResult(interp,(char*)help,TCL_STATIC);

  return (TCL_OK);

}


//---------------------------------------------------------------------


int tcl_quit(ClientData clientdata, Tcl_Interp *interp,
	     int argc, char **argv) {
  
  cout << "...received Tcl quit message..." << endl;
  done=1;
  
  return (TCL_OK);
}


//---------------------------------------------------------------------
//---------------------------------------------------------------------
//  info_server message callbacks
//---------------------------------------------------------------------
//---------------------------------------------------------------------


void generic_callback(T_IPC_CONN conn,
		      T_IPC_CONN_PROCESS_CB_DATA data,
		      T_CB_ARG arg) {
  
  T_INT4 run,event;
  T_STR name,group;
  T_REAL4 value,error;
  TipcMsg msg(data->msg);


  nmsg[(int)arg]++;


  //  testing?
  if(no_timeline)return;


  // connect
  if(timeline_connect()==0) {
    time_t now = time(NULL);
    if(debug!=0)cerr << types[(int)arg] << " unable to connect at " << ctime(&now) << flush;
    return;
  }


  // extract data and make entries
  msg.Current(0);
  for(int i=0; i<msg.NumFields(); i+=6) {

    msg >> run >> event >> name >> group >> value >> error;
    nentry[(int)arg]++;
    timeline_put(run,event,name,group,value,error);
  }


  // disconnect
  timeline_disconnect(); 


  return;
}

  
//----------------------------------------------------------------


void evstatmon_callback(T_IPC_CONN conn,
			T_IPC_CONN_PROCESS_CB_DATA data,
			T_CB_ARG arg) {
  
  int i,j;
  T_INT4 run,evtime,event_count;
  T_REAL8 *dvalues,clas_words;
  T_INT4  *ivalues;
  TipcMsg msg(data->msg);


  nmsg[(int)arg]++;


  //  testing?
  if(no_timeline)return;


  // extract data
  msg >> run >> evtime >> event_count >> clas_words;


  // no entry if no data
  if(event_count<=min_evt)return;


  for(j=0; j<30; j+=6) {
    msg >> ivalues; for(i=0; i<6; i++) {evstatmon_vals[j+i]=(float)ivalues[i]/event_count;}
  }
  for(j=30; j<60; j+=6) {
    msg >> dvalues; for(i=0; i<6; i++) {evstatmon_vals[j+i]=(float)dvalues[i];}
  }
  evstatmon_vals[60]=(float)event_count;
  evstatmon_vals[61]=(float)clas_words;
  msg.Current(35);
  msg >> ivalues; for(i=0; i<18; i++) {evstatmon_vals[62+i]=(float)ivalues[i]/event_count;}


  // connect
  if(timeline_connect()==0) {
    time_t now = time(NULL);
    if(debug!=0)cerr << types[(int)arg] << " unable to connect at " << ctime(&now) << flush;
    return;
  }


  // make entry
  nentry[(int)arg]++;
  timeline_put_group(run,0,(char*)"evstatmon",(char**)evstatmon_names,evstatmon_vals,nevstatmon);
  

  // disconnect
  timeline_disconnect(); 


  return;
}

  
//----------------------------------------------------------------


void evtstatus_callback(T_IPC_CONN conn,
			T_IPC_CONN_PROCESS_CB_DATA data,
			T_CB_ARG arg) {
  
  int i,j;
  T_INT4 run;
  T_REAL8 evstatmon_rate,scaler_rate,evmon_rate,recsis_rate,scstatmon_rate;
  T_REAL8 ipcbank_rate,mon_rate,coda_l3_rate,coda_pr_rate,coda_tr_rate;
  T_REAL8 event_rate,et2et_10_00_rate,et2et_10_04_rate,et2et_eb_er_rate,et2et_er_daq_rate;
  TipcMsg msg(data->msg);


  nmsg[(int)arg]++;

  //printf("1\n");
  //  testing?
  if(no_timeline)return;


  //printf("2\n");
  // extract data and convert to float
  // IMPORTANT: the order of fields in following statement
  //            corresponds to the order of fields in
  //            'std_id[]' array in 'evt_status_monitor'
  //            and corresponding input and output records
  //            in info_server.cfg file
  msg >> run
      >> ipcbank_rate
      >> coda_l3_rate
      >> scaler_rate 
      >> et2et_10_00_rate 
      >> coda_pr_rate
      >> coda_tr_rate
      >> et2et_10_04_rate
      >> evstatmon_rate
      >> evmon_rate
      >> recsis_rate
      >> scstatmon_rate
      >> et2et_eb_er_rate 
      >> et2et_er_daq_rate;

  //Sergey: use 'coda_l3_rate' as 'event_rate'
  event_rate = coda_l3_rate;

  //printf("3: rates -> %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
  //  ipcbank_rate,coda_l3_rate,scaler_rate,et2et_10_00_rate,coda_pr_rate,
  //  coda_tr_rate,et2et_10_04_rate,evstatmon_rate,evmon_rate,recsis_rate,scstatmon_rate,
  //  et2et_eb_er_rate,et2et_er_daq_rate);

  evtstatus_vals[0]=(float)event_rate;
  evtstatus_vals[1]=(float)et2et_10_04_rate;
  evtstatus_vals[2]=(float)et2et_10_04_rate;
  evtstatus_vals[3]=(float)ipcbank_rate;
  evtstatus_vals[4]=(float)evstatmon_rate;
  evtstatus_vals[5]=(float)scaler_rate;
  evtstatus_vals[6]=(float)evmon_rate;
  evtstatus_vals[7]=(float)recsis_rate;


  // no entry if data rate too low
  if(event_rate<=(float)min_rate) return;

  //printf("4: %f %f %f %f %f %f %f %f\n",
  // evtstatus_vals[0],evtstatus_vals[1],evtstatus_vals[2],evtstatus_vals[3],
  // evtstatus_vals[4],evtstatus_vals[5],evtstatus_vals[6],evtstatus_vals[7]);

  // connect
  if(timeline_connect()==0)
  {
    time_t now = time(NULL);
    if(debug!=0) cerr << types[(int)arg] << " unable to connect at " << ctime(&now) << flush;
    printf("unable to connect\n");
    return;
  }


  // make entry
  nentry[(int)arg]++;
  timeline_put_group(run,0,(char*)"evtstatus",(char**)evtstatus_names,evtstatus_vals,nevtstatus);
  //printf("5: arg=%d nentry=%d\n",(int)arg,nentry[(int)arg]);


  // disconnect
  timeline_disconnect(); 


  return;
}

  
//----------------------------------------------------------------


void diskcheck_callback(T_IPC_CONN conn,
			T_IPC_CONN_PROCESS_CB_DATA data,
			T_CB_ARG arg) {
  
  int i;
  T_STR status;
  T_INT4 itmp,nraid;
  T_REAL8 active_free;
  TipcMsg msg(data->msg);
  T_INT4 run=get_run_number(msql_database,session);


  nmsg[(int)arg]++;


  //  testing?
  if(no_timeline)return;


  // extract integers
  for(i=0; i<21; i++) { msg >> itmp; diskcheck_vals[i]=(float)itmp; }


  // active_free is double
  msg >> active_free;
  diskcheck_vals[21]=(float)active_free;


  // nraid (not entered into timeline hist)
  msg >> nraid;


  // raid status for 8 disks
  for(i=22; i<30; i++) {
      msg >> status;
      if (strncasecmp(status,"ACTIVE",6)==0) {
	  diskcheck_vals[i]=1.;
      } else if(strncasecmp(status,"EMPTY",5)==0) {
	  diskcheck_vals[i]=2.;
      } else if(strncasecmp(status,"MOVING",6)==0) {
	  diskcheck_vals[i]=3.;
      } else if(strncasecmp(status,"FULL (BA",8)==0) {
	  diskcheck_vals[i]=4.;
      } else if(strncasecmp(status,"FULL (NO",8)==0) {
	  diskcheck_vals[i]=5.;
      } else {
	  diskcheck_vals[i]=0.;
      }
  }


  // connect
  if(timeline_connect()==0) {
    time_t now = time(NULL);
    if(debug!=0)cerr << types[(int)arg] << " unable to connect at " << ctime(&now) << flush;
    return;
  }


  // make entry
  nentry[(int)arg]++;
  timeline_put_group(run,0,(char*)"diskcheck",(char**)diskcheck_names,diskcheck_vals,ndiskcheck);
  

  // disconnect
  timeline_disconnect(); 


  return;
}

  
//----------------------------------------------------------------


void evmon_callback(T_IPC_CONN conn,
		    T_IPC_CONN_PROCESS_CB_DATA data,
		    T_CB_ARG arg) {
  

  // n.b. evmon must send doubles



  TipcMsg msg(data->msg);
  T_INT4 run=get_run_number(msql_database,session);
  T_REAL8 ntrig;
  T_REAL8 trig_evt_mean,trig_evt_mean_err,trig_evt_width,trig_evt_width_err;
  T_REAL8 *trig_mean,*trig_mean_err,*trig_width,*trig_width_err;

  int i,j;
  char var[64];
  char group[64];


  nmsg[(int)arg]++;


  //  testing?
  if(no_timeline)return;


  // extract data
  msg >> ntrig;
  if(ntrig<(double)min_evt)return;

  msg >> trig_evt_mean >> trig_evt_mean_err >> trig_evt_width >> trig_evt_width_err;
  msg >> trig_mean >> trig_mean_err >> trig_width >> trig_width_err;



  // connect
  if(timeline_connect()==0) {
    time_t now = time(NULL);
    if(debug!=0)cerr << types[(int)arg] << " unable to connect at " << ctime(&now) << flush;
    return;
  }


  // make entry
  nentry[(int)arg]++;

  strcpy(var,evmon_evt_name);   strcat(var,"_mean");
  strcpy(group,evmon_group);    strcat(group,"_mean");
  timeline_put(run,0,var,group,trig_evt_mean,trig_evt_mean_err);
  
  strcpy(var,evmon_evt_name);   strcat(var,"_width");
  strcpy(group,evmon_group);    strcat(group,"_width");
  timeline_put(run,0,var,group,trig_evt_width,trig_evt_width_err);
  

  for(i=0; i<7; i++) {
    for(j=0; j<7; j++) {
      strcpy(var,evmon_names[i][j]);  strcat(var,"_mean");
      strcpy(group,evmon_group);      strcat(group,"_mean");
      timeline_put(run,0,var,group,trig_mean[i*7+j],trig_mean_err[i*7+j]);

      strcpy(var,evmon_names[i][j]);  strcat(var,"_width");
      strcpy(group,evmon_group);      strcat(group,"_width");
      timeline_put(run,0,var,group,trig_width[i*7+j],trig_width_err[i*7+j]);
    }
  }
  

  // disconnect
  timeline_disconnect(); 


  return;
}

  
//----------------------------------------------------------------


void scaler_callback(T_IPC_CONN conn,
		     T_IPC_CONN_PROCESS_CB_DATA data,
		     T_CB_ARG arg) {
  

  TipcMsg msg(data->msg);
  T_INT4 run=get_run_number(msql_database,session);
  T_INT4 *trgs=NULL,*trgd=NULL,*scs=NULL,*ecs=NULL,*s1st=NULL,*s1sd=NULL,*rcst30=NULL;
  double deltat;
  int i,j;
  T_STR label;
  time_t now = time(NULL);
  char *p;


  nmsg[(int)arg]++;


  //  testing?
  if(no_timeline)return;


  // too soon?
  if((now-last_scaler)<scaler_update)return;


  // extract data
  while(msg.Next(&label)) {
      p=strstr(label,"::")+2;
      if(strcasecmp(p,"TRGS00")==0) {
	  msg >> trgs;
      } else if(strcasecmp(p,"TRGD00")==0) {
	  msg >> trgd;
      } else if (strcasecmp(p,"SCS_00")==0) {
	  msg >> scs;
      } else if (strcasecmp(p,"ECS_00")==0) {
	  msg >> ecs;
      } else if (strcasecmp(p,"S1ST00")==0) {
	  msg >> s1st;
      } else if (strcasecmp(p,"S1SD00")==0) {
	  msg >> s1sd;
      } else if (strcasecmp(p,"RCST30")==0) {
	  msg >> rcst30;
      }
  }


  //  check that all banks arrived
  if( (trgs==NULL)||(trgd==NULL)||(scs==NULL)||(ecs==NULL)||(s1st==NULL)||
      (s1sd==NULL)||(rcst30==NULL) ) {
    if(debug!=0)cerr << "?missing banks" << endl;
    return;
  }


  // calculate time between scaler events
  deltat=double(trgd[0])/clock_rate;

  // eliminate this check...ejw, 5-dec-2005
  // check for legal deltat (9 to 11 seconds)
//    if((deltat<=9.0)||(deltat>11.0)) {
//        if(debug!=0)cerr << "?illegal deltat: " << deltat << endl;
//        return;
//    }


  // extract data (interval, unless noted)
  for(i=0; i<12; i++) 
      scaler_vals[i]=(double)trgd[48+i]/deltat;              // ungated l1 bits
  scaler_vals[12]=(double)trgd[60]/deltat;     		     // or level1
  scaler_vals[13]=(double)trgd[61]/deltat;     		     // l1accept
  scaler_vals[14]=(double)trgd[64]/deltat;     		     // l2fail
  scaler_vals[15]=(double)trgd[65]/deltat;     		     // l2pass
  scaler_vals[16]=(double)trgd[66]/deltat;     		     // l2start
  scaler_vals[17]=(double)trgd[67]/deltat;     		     // l2clear
  scaler_vals[18]=(double)trgd[68]/deltat;     		     // l2accept
  scaler_vals[19]=(double)trgd[69]/deltat;     		     // l3accept           
  scaler_vals[20]=(trgd[16]>0)?(double)trgd[32]/trgd[16]:0;  // live_clock
  scaler_vals[21]=(trgd[17]>0)?(double)trgd[33]/trgd[17]:0;  // live_fcup
  scaler_vals[22]=(trgd[60]>0)?(double)trgd[61]/trgd[60]:0;  // live_trig
  scaler_vals[23]=trgs[17];            			     // fcup_int  (from begin of run)
  scaler_vals[24]=trgd[17];            			     // fcup_diff          


  // ec total rates (6 channels per sector)
  for(i=0; i<6; i++) {
      scaler_vals[25+i]=0;
      for(j=0; j<6; j++) scaler_vals[25+i]+=(double)ecs[16*i+j]/deltat;
  }


  // sc total rates (32 channels per sector)
  for(i=0; i<6; i++) {
      scaler_vals[31+i]=0;
      for(j=0; j<32; j++) scaler_vals[31+i]+=(double)scs[32*i+j]/deltat;
  }
  

  // trigger problems
  scaler_vals[37]=s1st[13]+s1st[14]+s1st[15];                // latch1 state errors
  scaler_vals[38]=s1st[25]+s1st[26]+s1st[27];                // latch2 state errors
  scaler_vals[39]=trgs[66]-(trgs[65]+trgs[64]);              // l2 start-(pass+fail)
  scaler_vals[40]=trgs[64]-(trgs[67]+s1st[24]);              // l2 fail-(clear+latefail)
  scaler_vals[41]=trgs[68]-(trgs[65]+s1st[24]);              // l2 accept-(pass+latefail)
  scaler_vals[42]=trgs[68]-trgs[69];                         // l2accept-l3accept


  // gated l2 sector rates from trgd
  for(i=0; i<6; i++) 
    scaler_vals[43+i]=(double)trgd[72+i]/deltat;             // sector l2 rates
  scaler_vals[49]=(double)trgd[78]/deltat;                   // l2_or
  scaler_vals[50]=(double)trgd[79]/deltat;                   // l2_ok


  // accepted rates from s1st bank
  for(i=0; i<12; i++) 
      scaler_vals[51+i]=(double)s1sd[i]/deltat;              // bit l1 rates
  scaler_vals[63]=(double)s1sd[12]/deltat;                   // event rate
  for(i=0; i<6; i++) 
      scaler_vals[64+i]=(double)s1sd[17+i]/deltat;           // sector l2 rates
  scaler_vals[70]=(double)s1sd[23]/deltat;                   // l2 pass rate
  scaler_vals[71]=(double)s1sd[24]/deltat;                   // l2 fail rate


  // or and random live times
  scaler_vals[72]=(trgd[60]>0)?(double)trgd[34]/trgd[60]:0;  // live_or
  scaler_vals[73]=(trgd[19]>0)?(double)trgd[35]/trgd[19]:0;  // live_random



  // bombproof livetimes...not sure why this is needed...
  for(i=20; i<23; i++) if(scaler_vals[i]>1.)scaler_vals[i]=0.;
  for(i=72; i<74; i++) if(scaler_vals[i]>1.)scaler_vals[i]=0.;


  // connect
  if(timeline_connect()==0) {
    if(debug!=0)cerr << types[(int)arg] << " unable to connect at " << ctime(&now) << flush;
    return;
  }


  // make entry
  nentry[(int)arg]++;
  timeline_put_group(run,0,(char*)"scalers",(char**)scaler_names,scaler_vals,nscaler);


  // disconnect
  timeline_disconnect(); 


  // record entry time
  last_scaler=now;


  return;
}

  
//----------------------------------------------------------------


void systats_callback(T_IPC_CONN conn,
		      T_IPC_CONN_PROCESS_CB_DATA data,
		      T_CB_ARG arg) {
    

    int i;
    T_INT4 stime,net_in,net_in_err,net_out,net_out_err,net_collision;
    T_REAL8 temp_free,swap_free,mem_free,cpu_idle_avg;
    T_INT4 *cpu_idle;
    T_INT4 asize;
    T_INT4 run=get_run_number(msql_database,session);
    TipcMsg msg(data->msg);
    
    
    nmsg[(int)arg]++;
    
    
    //  testing?
    if(no_timeline)return;
    
    
    // extract data
    msg >> stime >> temp_free >> swap_free >> net_in >> net_in_err >> net_out >> net_out_err
	>> net_collision >> mem_free >> cpu_idle_avg >> cpu_idle >> GetSize(&asize);
    systats_vals[0]=temp_free;
    systats_vals[1]=swap_free;
    systats_vals[2]=net_in;
    systats_vals[3]=net_in_err;
    systats_vals[4]=net_out;
    systats_vals[5]=net_out_err;
    systats_vals[6]=net_collision;
    systats_vals[7]=mem_free;
    systats_vals[8]=cpu_idle_avg;
    for(i=0; i<MAX(ncpu00,ncpu10); i++)systats_vals[9+i]=0;
    for(i=0; i<asize; i++)systats_vals[9+i]=cpu_idle[i];

    
    // connect
    if(timeline_connect()==0) {
	time_t now = time(NULL);
	if(debug!=0)cerr << types[(int)arg] << " unable to connect at " << ctime(&now) << flush;
	return;
    }


    // make entry
    nentry[(int)arg]++;
    if(strcasecmp(types[(int)arg],"systats00")==0) {
	timeline_put_group(run,0,(char*)"systats00",(char**)systats_names,systats_vals,nsystats+ncpu00);
    } else if(strcasecmp(types[(int)arg],"systats10")==0) {
	timeline_put_group(run,0,(char*)"systats10",(char**)systats_names,systats_vals,nsystats+ncpu10);
    } else {
	cerr << "?illegal systats input arg: " << (int)arg 
	     << ",  type: " << types[(int)arg] << endl;
    }
    
    
    // disconnect
    timeline_disconnect(); 
    
    
    return;
}

  
//----------------------------------------------------------------


void alarm_callback(T_IPC_CONN conn,
		    T_IPC_CONN_PROCESS_CB_DATA data,
		    T_CB_ARG arg) {
    

    int i;
    T_INT4 run=get_run_number(msql_database,session);
    T_INT4 alarm_count;
    TipcMsg msg(data->msg);
    
    
    nmsg[(int)arg]++;
    
    
    //  testing?
    if(no_timeline)return;
    
    
    // extract data
    msg >> alarm_count;
    alarm_vals[0]=(float)alarm_count;

    
    // connect
    if(timeline_connect()==0) {
	time_t now = time(NULL);
	if(debug!=0)cerr << types[(int)arg] << " unable to connect at " << ctime(&now) << flush;
	return;
    }


    // make entry
    nentry[(int)arg]++;
    timeline_put_group(run,0,(char*)"alarm_system",(char**)alarm_names,alarm_vals,nalarm);
    
    
    // disconnect
    timeline_disconnect(); 
    
    
    return;
}

  
//----------------------------------------------------------------


void trigmon_callback(T_IPC_CONN conn,
		      T_IPC_CONN_PROCESS_CB_DATA data,
		      T_CB_ARG arg) {
  

    int i;
    T_INT4 run=get_run_number(msql_database,session);
    TipcMsg msg(data->msg);
    T_REAL8 *l2effic;
    
    
    nmsg[(int)arg]++;
    
    
    //  testing?
    if(no_timeline)return;
    
    
    // extract data
    msg >> l2effic;
    for(i=0; i<ntrigmon; i++)trigmon_vals[i]=l2effic[i];

    
    // connect
    if(timeline_connect()==0) {
	time_t now = time(NULL);
	if(debug!=0)cerr << types[(int)arg] << " unable to connect at " << ctime(&now) << flush;
	return;
    }


    // make entry
    nentry[(int)arg]++;
    timeline_put_group(run,0,(char*)"trigmon",(char**)trigmon_names,trigmon_vals,ntrigmon);
    
    
    // disconnect
    timeline_disconnect(); 
    
    
    return;
}

  
  
//----------------------------------------------------------------
//----------------------------------------------------------------


