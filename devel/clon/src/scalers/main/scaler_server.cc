//  scaler_server
//
//  gets scaler events from ET and ships scaler information to info server
//  checks livetime, calculates TRGS differences
//  If no scaler events reads hardware directly
//
//  still to do:
//    enable live_or test
//
//  ejw, 3-feb-00


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for smartsockets
#include <rtworks/cxxipc.hxx>


// for i/o

using namespace std;
#include <strstream>

#include <fstream.h>
#include <iomanip.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>


// for tcl
extern "C"{
#include "tcl.h"
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


// for et
extern "C" {
#include <et.h>
}

// for tcpClientCmd
#include "libtcp.h"


// for CLAS ipc
#include <clas_ipc_prototypes.h>

#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )

// misc variables
Tcl_Interp *interp;  
static char *application      	= (char*)"clastest";
static char *session          	= NULL;
static char *uniq_name        	= (char*)"scaler_server";
static char *init_tcl_script  	= NULL;
static char *unique_id        	= (char *) malloc(512);
static char *host             	= getenv("HOST");
static double check_time      	= 2.0;
static int scaler_readout_time  = 25;        // seconds
static int done               	= 0;
static int debug              	= 0;
static int no_file            	= 0;
static int no_info            	= 0;
static int no_fcup            	= 0;
static double live_tol        	= 5.0;       // in percent
static int min_evt_live       	= 800000;    // about 10 minutes
static time_t last_time       	= time(NULL);
static time_t last_scaler    	= time(NULL);
static int current_run        	= 0;
static int nevt_run        	= 0;
static int total_bad_bank_count = 0;
static int bad_bank_count       = 0;
static int livetime_err_count   = 0;

static time_t delta;
static int scaler_err_this_run = 0;
static double live_trig,live_clock,live_fcup,live_or;


// lengths of scaler banks defined here
static unsigned long trgs[96];
static unsigned long last_trgs[96];
static unsigned long delta_trgs[96];
static unsigned long ecs[96];
static unsigned long scs[192];
static unsigned long sts[16];
static unsigned long last_s1st[35];
static unsigned long delta_s1st[35];


static char filename[132];
static char temp[10000];

static int nrec                 = 0;
static int nevent             	= 0;
static int last_rec          	= 0;
static int last_proc          	= 0;
static double rec_rate       	= 0.;
static double proc_rate       	= 0.;


// for et
static int et_ok              = 0;
static char *et_station_name        = (char*)"SCALER";
static et_openconfig openconfig;
static et_sys_id et_system_id;
static char et_filename[128];
static et_stat_id et_station_id;
static et_statconfig et_station_config;
static et_att_id et_attach_id;
static et_event *et_event_ptr;


// prototypes
void decode_command_line(int argc, char **argv);
void analyze_event(long *evt);
void init_et();
void connect_et();
void get_hardware_scalers(void);
void init_tcl(void);
void scaler_evt_to_file(long event[]);
void hardware_scaler_to_file();
long *find_next_bank(long*, char *, int &, int &, int &);
int find_tag_line(istrstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(istrstream &file, char buffer[], int buflen);
extern "C" {
void T_ENTRY control_message_callback(T_IPC_CONN conn,
				      T_IPC_CONN_PROCESS_CB_DATA data,
				      T_CB_ARG arg);
void *process_events(void *param);
void *control_thread(void *param);
void *hardware_thread(void *param);
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
int get_run_status(char *session);
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	      int severity, const char *status, int code, const char *message);
}


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//--------------------------------------------------------------------------


main(int argc,char **argv) {

  int i,status;
  pthread_t t1,t2,t3;
  int r1=0,r2=0,r3=0;


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line
  decode_command_line(argc,argv);
    
    
  // get session name
  if(session==NULL)session=(char*)"clasprod";


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
  server.SubjectSubscribe((T_STR)"evt_system",TRUE);


  // enable Tcl ipc control (i.e. create tcl_request callback)
  tclipc_init(interp);


  // init et structures, etc;
  init_et();
  for(i=0; i<sizeof(last_trgs)/sizeof(unsigned long); i++) {last_trgs[i]=0;}
  for(i=0; i<sizeof(last_s1st)/sizeof(unsigned long); i++) {last_s1st[i]=0;}

  // post startup message
  sprintf(temp,"Process startup:    %15s  in application:  %s",unique_id,application);
  status=insert_msg("scaler_server",unique_id,unique_id,"status",0,"START",0,temp);


  // launch control thread
  if(pthread_create(&t1,NULL,control_thread,(void *)&r1)!=0) {
    cerr << "\nUnable to create control thread" << endl << endl;
    exit(EXIT_FAILURE);
  }


  // launch event thread
  if(pthread_create(&t2,NULL,process_events,(void *)&r2)!=0) {
    cerr << "\nUnable to create event thread" << endl << endl;
    exit(EXIT_FAILURE);
  }


  /* Sergey: no hardware enqueries
  // launch hardware scaler readout thread
  if(pthread_create(&t3,NULL,hardware_thread,(void *)&r3)!=0) {
    cerr << "\nUnable to create hardware readout thread" << endl << endl;
    exit(EXIT_FAILURE);
  }
  */

  // flush everything
  server.Flush();
  cout << flush;


  // wait for control thread to terminate
  pthread_join(t1,NULL);

  
  // done
  et_forcedclose(et_system_id);
  ipc_close();
  sprintf(temp,"Process shutdown:  %15s",unique_id);
  status=insert_msg("scaler_server",unique_id,unique_id,"status",0,"STOP",0,temp);
  exit(EXIT_SUCCESS);
}
       

//--------------------------------------------------------------------------


void *control_thread(void *param) {


  // handle IPC messages...read scalers directly from hardware if no scaler events arrive
  while(done==0) {
    
    server.MainLoop(check_time);
    

    // calc rates every 30 seconds
    delta=time(NULL)-last_time;
    if(delta>30) {
      rec_rate=(double)(nrec-last_rec)/delta;
      proc_rate=(double)(nevent-last_proc)/delta;
      last_time=time(NULL);
      last_rec=nrec;
      last_proc=nevent;
    }
    
  }
  
  return (void *)0;
}


//------------------------------------------------------------------------


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
    status=et_event_get(et_system_id,et_attach_id,&et_event_ptr,ET_SLEEP,NULL);
    

    if (status==ET_OK) {
      nrec++;
      et_event_getdata(et_event_ptr,(void**)&evt);
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


void init_et() {
  

  et_ok=0;


  // create et file name, etc.
  sprintf(et_filename,"/tmp/et_sys_%s",session);

  //et_open_config_init(&openconfig);

  // get et library name
  //char *et_user_library     = (char *) malloc(512);
  //strcpy(et_user_library,getenv("CLON_LIB"));
  //strcat(et_user_library,(char*)"/et_user_library.so");


  // station created in clas_et_start
  // create station config in case no station exists
//    et_station_config_init(&et_station_config);
//    et_station_config_setblock(et_station_config,ET_STATION_BLOCKING);
//    et_station_config_setselect(et_station_config,ET_STATION_SELECT_USER);
//    et_station_config_setuser(et_station_config,ET_STATION_USER_SINGLE);
//    et_station_config_setrestore(et_station_config,ET_STATION_RESTORE_OUT);
//    et_station_config_setcue(et_station_config,100);
//    et_station_config_setprescale(et_station_config,1);
//    et_station_config_setlib(et_station_config,et_user_library);
//    et_station_config_setfunction(et_station_config,(char*)"et_scaler_function");


  return;
}


//--------------------------------------------------------------------------


void connect_et() {
  
  int status;
  sigset_t sigblock;
  

  et_ok=0;

  
  // open et system
  if(et_open(&et_system_id,et_filename,openconfig)!=ET_OK)return;
  

  // never create station, just attach to existing station
  // create station if not already created
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
      cout << status << endl;
      et_forcedclose(et_system_id);
      cerr << "?station " << et_station_name << " does not exist" << endl;
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
      cerr << "?unable to attach to station " << et_station_name << endl;
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
    
    
    // reset bad bank flags,counters,run,etc.
    run=evt[5];
    if(run!=current_run) {
      current_run=run;
      bad_bank_count=0;
      scaler_err_this_run=0;
      for(i=0; i<sizeof(last_trgs)/sizeof(unsigned long); i++) {last_trgs[i]=0;}
      for(i=0; i<sizeof(last_s1st)/sizeof(unsigned long); i++) {last_s1st[i]=0;}
    }
    found_bad_bank=0;
    
    
    // create info_server message
    if(no_info==0) {
      msg = new TipcMsg((T_STR)"info_server");
      msg->Dest((T_STR)"info_server/in/scaler_server/data");
      *msg << (T_STR)"scaler_server";
    }      
    
    
    // loop over all banks (datap>0), fill msg and/or dump 
    // Note:  do NOT change nwords!
    if(debug!=0) cout << "\n\n------------------ new scaler event ----------------------" << endl;
    position = 0;
    nwords = 0;
    while ((datap=find_next_bank(evt,bankname,banknum,position,nwords))>0) {
      
      
      // check for illegal banks in scaler event
      if(
	 (strncasecmp(bankname,"DC0",3)==0) ||
	 (strncasecmp(bankname,"EC ",3)==0) ||
	 (strncasecmp(bankname,"SC ",3)==0) ||
	 (strncasecmp(bankname,"CC ",3)==0) ||
	 (strncasecmp(bankname,"EC1",3)==0) ) {
	found_bad_bank=1;
      }
      
      
      // store software scaler diffs
      if(strncasecmp(bankname,"S1ST",4)==0) {
	for(i=0; i<sizeof(last_s1st)/sizeof(unsigned long); i++) {
	  delta_s1st[i]=(unsigned long)datap[i]-last_s1st[i];
	  last_s1st[i]=datap[i];
	}
      }
	
	
      // special handling for TRGS bank 0
      if((strncasecmp(bankname,"TRGS",4)==0)&&(banknum==0)) {
	
	// store trigger scaler diffs
	for(i=0; i<sizeof(last_trgs)/sizeof(unsigned long); i++) {
	  delta_trgs[i]=(unsigned long)datap[i]-last_trgs[i];
	  last_trgs[i]=datap[i];
	}
	
	
	// check for livetime agreement after enough events taken
	nevt_run=datap[61];
	live_trig  = (datap[60]>0)?(float)datap[61]/datap[60]:0.0;
	live_clock = (datap[16]>0)?(float)datap[32]/datap[16]:0.0;
	live_fcup  = (datap[17]>0)?(float)datap[33]/datap[17]:0.0;
	live_or    = (datap[34]>0)?(float)datap[61]/datap[34]:0.0;
	
	if( (nevt_run>min_evt_live) && (scaler_err_this_run==0) && 
	    (
	     (100.*fabs(live_trig  - live_clock)>live_tol) || 
//  	     (100.*fabs(live_trig  - live_or   )>live_tol) || 
//  	     (100.*fabs(live_clock - live_or   )>live_tol) || 
	     ((no_fcup==0) &&
	      ( (100.*fabs(live_fcup-live_clock)>live_tol) ||
//  	        ((100.*fabs(live_fcup-live_or)>live_tol))  ||
	        ((100.*fabs(live_fcup-live_trig)>live_tol)) ))
	     )
	    ) {
	  scaler_err_this_run=1;
	  livetime_err_count++;
	  if(no_fcup==0) {
	    sprintf(temp,"Livetime disagreement run %d, trig,clock,fcup,or:  %f,  %f,  %f,  %f",
		    current_run,live_trig,live_clock,live_fcup,live_or);
	  } else {
	    sprintf(temp,"Livetime disagreement run %d, trig,clock,or:  %f,  %f,  %f",
		    current_run,live_trig,live_clock,live_or);
	  }
	  insert_msg("online","scaler_server",unique_id,"status",1,"WARN",0,temp);
	  cout << temp << endl;;
	}
      }
      
      
      // fill ipc msg with encoded bank name and bank contents except DCST and HLS banks
      if(no_info==0) {
	if((strncasecmp(bankname,"DCST",4)!=0)&&(strncasecmp(bankname,"HLS",3)!=0))  {
	  sprintf(scal_bankname,"%-4s%02d",bankname,banknum);
	  for (int m=0; m<strlen(scal_bankname); m++) if(scal_bankname[m]==' ')scal_bankname[m]='_';
	  *msg << scal_bankname;
	  *msg << SetSize(nwords) << (T_INT4*)datap;
	  if((strncasecmp(bankname,"TRGS",4)==0)&&(banknum==0)) {
	    *msg << (T_STR)"TRGD00";
	    *msg << SetSize(nwords) << (T_INT4*)delta_trgs;
	  }
	  if(strncasecmp(bankname,"S1ST",4)==0) {
	    *msg << (T_STR)"S1SD00";
	    *msg << SetSize(nwords) << (T_INT4*)delta_s1st;
	  }
	}
      }
      
      
      // dump banks
      if(debug!=0) {
	cout << "\n\nScaler data for bank: " << bankname << " number " << banknum
	     << " Words = " << nwords << endl << endl;
	for (int k = 0; k < nwords; k++) {
	  cout << setw(10) << datap[k] << ' ';
	  if((k+1)%8 == 0) 	cout << endl;
	}
	cout << endl << endl;
	
	if((strncasecmp(bankname,"TRGS",4)==0)&&(banknum==0)) {
	  cout << "\n\nTRGS differences in bank TRGD number 0 Words = " 
	       << sizeof(delta_trgs)/sizeof(unsigned long) << endl << endl;
	  for (int k = 0; k < sizeof(delta_trgs)/sizeof(unsigned long); k++) {
	    cout << setw(10) << delta_trgs[k] << ' ';
	    if((k+1)%8 == 0) 	cout << endl;
	  }
	}

	if(strncasecmp(bankname,"S1ST",4)==0) {
	  cout << "\n\nS1ST differences in bank S1SD number 0 Words = " 
	       << sizeof(delta_s1st)/sizeof(unsigned long) << endl << endl;
	  for (int k = 0; k < sizeof(delta_s1st)/sizeof(unsigned long); k++) {
	    cout << setw(10) << delta_s1st[k] << ' ';
	    if((k+1)%8 == 0) 	cout << endl;
	  }
	}

	cout << endl << endl;
      }
      
      
    } // loop over banks
    
    
    // send, flush, delete message
    if(no_info==0) {
      server.Send(*msg,TRUE);
      server.Flush();
      delete msg;
    }
    
    
    // post warning about bad banks (every 1 million events, count reset each run)
    if(found_bad_bank!=0) {
      total_bad_bank_count++;
      bad_bank_count++;
      if((bad_bank_count%1000000)==1) {
	insert_msg("online","CODA",unique_id,"status",2,"CLAS",0,
		   "SYNC ERROR:  scaler_server found illegal bank in scaler event");
      }
    }
    
    
    // write event to file
    if(no_file==0)scaler_evt_to_file(evt);
    
    
  }
  
  return;
}


//-------------------------------------------------------------------


void scaler_evt_to_file(long event[]) {
  
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
  fname << getenv("CLON_PARMS") << "/scalers/archive/scalers_" << session << "_" 
	<< setw(6) << run << ".txt" << ends;
  ofstream file(fname.str());
  if(!file.is_open())return;
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
      for(int j=i; j<MIN(i+8,nwrds); j++) {
	file << setw(10) << event[ind+j] << setw(1) << " ";
      }
      file << endl;
    }
    file << "*EOD*" << endl;


    // write out TRGD info if current bank is TRGS
    if((strncasecmp(name,"TRGS",4)==0)&&(nbnk==0)) {
      file << "\n\n*TRGD00*" << endl;
      for(int i=0; i<nwrds; i+=8) {
	for(int j=i; j<MIN(i+8,nwrds); j++) {
	  file << setw(10) << delta_trgs[j] << setw(1) << " ";
	}
	file << endl;
      }
      file << "*EOD*" << endl;
    }


    // write out S1SD info if current bank is S1ST
    if(strncasecmp(name,"S1ST",4)==0) {
      file << "\n\n*S1SD00*" << endl;
      for(int i=0; i<nwrds; i+=8) {
	for(int j=i; j<MIN(i+8,nwrds); j++) {
	  file << setw(10) << delta_s1st[j] << setw(1) << " ";
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
  TipcMsg msg((T_STR)"info_server");
  msg.Dest((T_STR)"info_server/in/scaler_server/hardware");
  msg << (T_STR)"scaler_server";


  // scaler1 (trgs)
  tcpClientCmd("scaler1","read_scaler1",temp);
  istrstream sc1(temp,sizeof(temp));
  if(find_tag_line(sc1,"*TRGS*",line,sizeof(line))==0) {
    for(i=0; i<sizeof(trgs)/sizeof(unsigned long); i+=8) {
      get_next_line(sc1,line,sizeof(line));
      l=0; ind=0; for(k=0; k<8; k++) {l+=ind; sscanf(&line[l],"%lu %n",&trgs[k+i],&ind);}
    }
  }
  msg << (T_STR)"TRGS00" << SetSize(sizeof(trgs)/sizeof(unsigned long)) << (T_INT4*)trgs;


  // scaler2 (ecs)
  tcpClientCmd("scaler2","read_scaler2",temp);
  istrstream sc2(temp,sizeof(temp));
  if(find_tag_line(sc2,"*EC*",line,sizeof(line))==0) {
    for(i=0; i<sizeof(ecs)/sizeof(long); i+=8) {
      get_next_line(sc2,line,sizeof(line));
      l=0; ind=0; for(k=0; k<8; k++) {l+=ind; sscanf(&line[l],"%lu %n",&ecs[k+i],&ind);}
    }
  }
  msg << (T_STR)"ECS_00" << SetSize(sizeof(ecs)/sizeof(long)) << (T_INT4*)ecs;


  // scaler2 (scs)
  if(find_tag_line(sc2,"*SC*",line,sizeof(line))==0) {
    for(i=0; i<sizeof(scs)/sizeof(long); i+=8) {
      get_next_line(sc2,line,sizeof(line));
      l=0; ind=0; for(k=0; k<8; k++) {l+=ind; sscanf(&line[l],"%lu %n",&scs[k+i],&ind);}
    }
  }
  msg << (T_STR)"SCS_00" << SetSize(sizeof(scs)/sizeof(long)) << (T_INT4*)scs;


  // scaler3 (sts)
  tcpClientCmd("scaler3","read_scaler3",temp);
  istrstream sc3(temp,sizeof(temp));
  if(find_tag_line(sc3,"*ST*",line,sizeof(line))==0) {
    for(i=0; i<sizeof(sts)/sizeof(long); i+=8) {
      get_next_line(sc3,line,sizeof(line));
      l=0; ind=0; for(k=0; k<8; k++) {l+=ind; sscanf(&line[l],"%lu %n",&sts[k+i],&ind);}
    }
  }
  msg << (T_STR)"STS_00" << SetSize(sizeof(sts)/sizeof(long)) << (T_INT4*)sts;


  // send and flush message
  server.Send(msg,TRUE);
  server.Flush();
  

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
  if(!file.is_open())return;
  file << "Hardware scaler dump by scaler_server at " << ctime(&now) << endl << endl;


  file << "\n\n*TRGS00*" << endl;
  for(i=0; i<sizeof(trgs)/sizeof(long); i+=8) {
    for(j=i; j<MIN(i+8,sizeof(trgs)/sizeof(long)); j++) {
      file << setw(10) << trgs[j] << setw(1) << " ";
    }
    file << endl;
  }
  file << "*EOD*" << endl;
  
  
  file << "\n\n*ECS 00*" << endl;
  for(i=0; i<sizeof(ecs)/sizeof(long); i+=8) {
    for(j=i; j<MIN(i+8,sizeof(ecs)/sizeof(long)); j++) {
      file << setw(10) << ecs[j] << setw(1) << " ";
    }
    file << endl;
  }
  file << "*EOD*" << endl;
  
  
  file << "\n\n*SCS 00*" << endl;
  for(i=0; i<sizeof(scs)/sizeof(long); i+=8) {
    for(j=i; j<MIN(i+8,sizeof(scs)/sizeof(long)); j++) {
      file << setw(10) << scs[j] << setw(1) << " ";
    }
    file << endl;
  }
  file << "*EOD*" << endl;
  

  file << "\n\n*STS 00*" << endl;
  for(i=0; i<sizeof(sts)/sizeof(long); i+=8) {
    for(j=i; j<MIN(i+8,sizeof(sts)/sizeof(long)); j++) {
      file << setw(10) << sts[j] << setw(1) << " ";
    }
    file << endl;
  }
  file << "*EOD*" << endl;


  file << endl << endl;
  file.close();
  return;
}


//-------------------------------------------------------------------


void status_poll_callback(T_IPC_MSG msg) {

  char *t;


  TipcMsgAppendStr(msg,(T_STR)"Session");
  TipcMsgAppendStr(msg,session);

  TipcMsgAppendStr(msg,(T_STR)"Number of events received");
  TipcMsgAppendInt4(msg,nrec);

  TipcMsgAppendStr(msg,(T_STR)"Number of scaler events processed");
  TipcMsgAppendInt4(msg,nevent);

  TipcMsgAppendStr(msg,(T_STR)"Current run");
  TipcMsgAppendInt4(msg,current_run);

  TipcMsgAppendStr(msg,(T_STR)"Number events this run");
  TipcMsgAppendInt4(msg,nevt_run);

  TipcMsgAppendStr(msg,(T_STR)"Total bad bank count");
  TipcMsgAppendInt4(msg,total_bad_bank_count);

  TipcMsgAppendStr(msg,(T_STR)"Run bad bank count");
  TipcMsgAppendInt4(msg,bad_bank_count);

  TipcMsgAppendStr(msg,(T_STR)"Livetime by triggers");
  TipcMsgAppendReal8(msg,live_trig);

  TipcMsgAppendStr(msg,(T_STR)"Livetime by clock");
  TipcMsgAppendReal8(msg,live_clock);

  TipcMsgAppendStr(msg,(T_STR)"Livetime by fcup");
  TipcMsgAppendReal8(msg,live_fcup);

  TipcMsgAppendStr(msg,(T_STR)"Livetime by Level1 OR");
  TipcMsgAppendReal8(msg,live_or);

  TipcMsgAppendStr(msg,(T_STR)"Livetime error count");
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

    TipcMsg status((T_STR)"evt_status");
    status.Dest((T_STR)"/evt_system/status");
    status.Sender(unique_id);
    sprintf(temp,"et:%s",et_station_name);
    status << unique_id << host << session << srvnode 
	   << (T_INT4) nrec << rec_rate << (T_INT4) nevent << proc_rate
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

  const char *help = 
    "\nusage:\n\n  scaler_server [-a application] [-s session] [-u uniq_name] [-live live_tol]\n"
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










