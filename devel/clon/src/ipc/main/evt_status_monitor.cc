// evt_status_monitor

//  can broadcast evt_status request message, prints all replies,
//    and create info server message

//  still to do:


// ejw, 22-nov-99



#define _POSIX_SOURCE 1
#define __EXTENSIONS__


//  for smartsockets
#include <rtworks/cxxipc.hxx>


// CLAS ipc
#include <clas_ipc_prototypes.h>


//  system
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include <iomanip.h>
#include <time.h>


#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )


// misc
static char *application = (char*)"clastest";
static int wait_time     = 3;
static int pause_time    = 7;
static int broadcast     = 0;
static int no_print      = 0;
static int info          = 0;
static int file          = 0;
static int done          = 0;
static int once          = 0;
static int nreq          = 0;
static time_t last_time  = 0;
static char *info_file   = (char*)"evt/evt_status_monitor.txt";
static ofstream *ofile;
static char filename[120];


// storage for standard procs
#define MAX_PROC 200
const char *std_id[] = {"ipcbank2et_10_clasprod",
                        "coda_l3_clasprod",
                        "scaler_server_clasprod",
			"et2et_daq_mon_clasprod",
			"coda_pr_clasprod",
                        "coda_tr_clasprod",
			"et2et_daq_mon2_clasprod",
			"evstatmon_clasprod",
                        "mon_clasprod",
                        "recsis_clasprod",
			"scstatmon_clasprod",
			"et2et_eb_clondaq2-daq2_clasprod",
			"et2et_er_clon10-daq1_clasprod",
};
#define CSIZE 256
struct evtinfo {
  char id[CSIZE];
  char node[CSIZE];
  char srvnode[CSIZE];
  char name[CSIZE];
  T_INT4 nevt_rec;
  T_REAL8 rec_rate;
  T_INT4  nevt_proc;
  T_REAL8 proc_rate;
  T_INT4 flag;
  char source[CSIZE];
};
static evtinfo proc_info[MAX_PROC];
static int nextra = 0;
static int nstd   = sizeof(std_id)/sizeof(char*);
static char temp[200];


// prototypes
void broadcast_request(void);
void print_title(void);
void print_results(void);
void send_info_server_msg(void);
void receive_info(
		  T_IPC_CONN conn,
		  T_IPC_CONN_PROCESS_CB_DATA data,
		  T_CB_ARG arg);
extern "C"{
void status_poll_callback(T_IPC_MSG msg);
void quit_callback(int sig);
void decode_command_line(int argc, char **argv);
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	      int severity, const char *status, int code, const char *message);
}


// ref to server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//---------------------------------------------------------------


int main(int argc, char **argv) {

  FILE *fp;
  int status;


  // synch with stdio
  ios::sync_with_stdio();


  // decode command line
  decode_command_line(argc,argv);


  // connect to ipc...no info server message if not broadcasting request
  TutSetOutputFunc(ipc_output_dummy);
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
  if(broadcast!=0) {
    status=ipc_init("evt_status_monitor","evt status monitor (master)");
  } else {
    status=ipc_init("","evt status monitor");
    info=0;
    file=0;
  }
  if(status<0) {
    cerr << "\n?Unable to connect to server..."
	 << "probably duplicate unique id\n"
	 << "   ...check for another evt_status_monitor connected to "
	 << application << " using ipc_info\n"
	 << "   ...only one such process allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }


  // reset nstd if not clasprod project
  if(strcasecmp(application,"clasprod")!=0)nstd=0;


  // receive evt_status messages
  server.SubjectSubscribe((T_STR)"/evt_system/status",TRUE);
  TipcMt mt2((T_STR)"evt_status");
  server.ProcessCbCreate(mt2,receive_info,NULL);
  TutSetOutputFunc(NULL);


  // get output file name
  if(file!=0) {
    sprintf(filename,"%s/%s",getenv("CLON_PARMS"),info_file);
  }


  // flush outgoing messages and output
  server.Flush();
  cout << flush;


  // warning message if not in broadcast mode
  if(broadcast==0) {
    cout << "\n  *** NOTE...evt_status_monitor (master) must be running ***" 
	 << endl << endl;
  }


  // post startup message for master
  if(broadcast!=0) {
    sprintf(temp,"Process startup:    %15s  in application:  %s","evt_status_monitor (master)",
	    application);
    status=insert_msg("evt_status_monitor","online","evt_status_monitor","status",0,"START",0,temp);
  }


  // loop forever
  while(done==0) {


    // clear arrays, etc.
    nextra=0;
    for(int i=0; i<nstd; i++) {
      strcpy(proc_info[i].id,std_id[i]);
      strcpy(proc_info[i].node,"unknown");
      strcpy(proc_info[i].srvnode,"unknown");
      strcpy(proc_info[i].name,"unknown");
      proc_info[i].nevt_rec=0;
      proc_info[i].rec_rate=0.0;
      proc_info[i].nevt_proc=0;
      proc_info[i].proc_rate=0.0;
      proc_info[i].flag=0;
      strcpy(proc_info[i].source,"unknown");
    }

    
    server.MainLoop((double)pause_time);


    // broadcast mode
    if(broadcast!=0) {
      broadcast_request();
      if(file!=0)ofile = new ofstream(filename);
      server.MainLoop((double)wait_time);
      print_title();
      print_results();
      if(file!=0) {
	ofile->close();
	delete ofile;
      }
      if(info==1)send_info_server_msg();


    // non-broadcast mode
    } else {
      server.MainLoop((double)wait_time);
      print_title();
      print_results();
    }


    //  repeat or quit
    if(once==1) break;
  }


  // done
  TutSetOutputFunc(ipc_output_dummy);
  if(broadcast!=0) {
    sprintf(temp,"Process shutdown:    %15s  in application:  %s","evt_status_monitor (master)",
	    application);
    status=insert_msg("evt_status_monitor","online","evt_status_monitor","status",0,"STOP",0,temp);
  }
  exit(EXIT_SUCCESS);
}


//------------------------------------------------------------------


void broadcast_request() {

  TipcMsg msg(TipcMtLookupByNum(T_MT_CONTROL));

  nreq++;
  
  msg.Dest((T_STR)"evt_system");
  msg << (T_STR)"evt_status_poll";
  server.Send(msg,TRUE);

  server.Flush();

  return;
}


//-----------------------------------------------------------------------


void print_title() {
  
  
  if(no_print==0) {
    cout << endl << endl
	 << setw(26) << "Program" << " " << setw(12) << "Node" << " " << setw(8) << "Session" << " " 
	 << setw(8) << "Server" << " " << setw(9) << "Evt Rec" << " " << setw(9) << "Rec Rate" << "  "  
	 << setw(8) << "Evt Proc" << " " << setw(9) << "Proc Rate" << " "
	 << setw(4) << "Flag" << " " << setw(4) << "Data Flow" << endl;
    cout << setw(26) << "-------" << " " << setw(12) << "----" << " " << setw(8) << "-------" << " " 
	 << setw(8) << "------" << " " << setw(9) << "-------" << " " << setw(9) << "--------" << "  " 
	 << setw(8) << "--------" << " " << setw(9) << "---------" << " " 
	 << setw(4) << "----" << " " << setw(4) << "---------" << endl;
    cout << endl;
  }


  if(file!=0) {
    time_t now = time(NULL);
    *ofile << "\n  Written by evt_status_monitor on " << ctime(&now) << endl << endl;
    *ofile << setw(26) << "Program" << " " << setw(12) << "Node" << " " << setw(8) << "Session" << " " 
	   << setw(8) << "Server" << " " << setw(9) << "Evt Rec" << " " << setw(9) << "Rec Rate" << " "  
	   << setw(9) << "Evt Proc" << " " << setw(9) << "Proc Rate" << " "
	   << setw(4) << "Flag" << " " << setw(4) << "Data Flow" << endl;
    *ofile << setw(26) << "-------" << " " << setw(12) << "----" << " " << setw(8) << "-------" << " " 
	   << setw(8) << "------" << " " << setw(9) << "-------" << " " << setw(9) << "--------" << " " 
	   << setw(9) << "--------" << " " << setw(9) << "---------" << " " 
	   << setw(4) << "----" << " " << setw(4) << "---------" << endl;
    *ofile << endl;
  }

  return;
}


//------------------------------------------------------------------


void receive_info(T_IPC_CONN conn,
		  T_IPC_CONN_PROCESS_CB_DATA data,
		  T_CB_ARG arg) {
  
  int i;

  T_STR id;
  T_STR node;
  T_STR name;
  T_STR srvnode;
  T_INT4 nevt_rec,nevt_proc;
  T_REAL8 rec_rate,proc_rate;
  T_INT4 flag;
  T_STR source;
  char *p;
    
  T_IPC_MSG msg = data->msg;
  
  
  // unpack message
  TipcMsgSetCurrent(msg,0);
  TipcMsgNextStr(msg,&id);
  TipcMsgNextStr(msg,&node);
  TipcMsgNextStr(msg,&name);
  TipcMsgNextStr(msg,&srvnode);
  TipcMsgNextInt4(msg,&nevt_rec);
  TipcMsgNextReal8(msg,&rec_rate);
  TipcMsgNextInt4(msg,&nevt_proc);
  TipcMsgNextReal8(msg,&proc_rate);
  TipcMsgNextInt4(msg,&flag);
  TipcMsgNextStr(msg,&source);



  // trim node names after first "."
  p=strchr(   node,'.'); if(p!=NULL)*p=NULL;
  p=strchr(srvnode,'.'); if(p!=NULL)*p=NULL;


  // store info...check if this is std proc
  int ptr=-1;
  for(i=0; i<nstd; i++) {
    if(strcasecmp(std_id[i],id)==0) { ptr=i; break; }
  }
  if(ptr<0) {
    nextra++;
    ptr=MIN(nstd+nextra-1,MAX_PROC);
  }

  strcpy(proc_info[ptr].id,id);
  strcpy(proc_info[ptr].node,node);
  strcpy(proc_info[ptr].srvnode,srvnode);
  strcpy(proc_info[ptr].name,name);
  proc_info[ptr].nevt_rec  = nevt_rec;
  proc_info[ptr].rec_rate  = rec_rate;
  proc_info[ptr].nevt_proc = nevt_proc;
  proc_info[ptr].proc_rate = proc_rate;
  proc_info[ptr].flag      = flag;
  strcpy(proc_info[ptr].source,source);
  

  return;
}


//------------------------------------------------------------------


void print_results() {

  int i;


if(no_print==0) {
    cout.clear();
    cout.precision(2);
    cout.setf(ios::fixed);
    for(i=0; i<nstd+nextra; i++) {
      strcpy(temp,proc_info[i].id);
      if(strlen(temp)>30) temp[29]='\0';
      cout << setw(30) << temp
	   << " " << setw(8) << proc_info[i].node << " " 
	   << setw(8) << proc_info[i].name << " " 
	   << setw(8) << proc_info[i].srvnode << " " << setw(9) << proc_info[i].nevt_rec << " " 
	   << setw(9) << proc_info[i].rec_rate << " "
	   << setw(9) << proc_info[i].nevt_proc << " " << setw(9) << proc_info[i].proc_rate << " "
	   << setw(4) << proc_info[i].flag << " " << setw(8) 
	   << setiosflags(ios::left) << proc_info[i].source << resetiosflags(ios::left) << endl;
    }
  }


  if(file!=0) {
    ofile->precision(2);
    ofile->setf(ios::fixed);
    for(i=0; i<nstd+nextra; i++) {
      strcpy(temp,proc_info[i].id);
      if(strlen(temp)>30)temp[29]='\0';
      *ofile << setw(30) << temp
	     << " " << setw(8) << proc_info[i].node << " " 
	     << setw(8) << proc_info[i].name << " " 
	     << setw(8) << proc_info[i].srvnode << " " << setw(9) << proc_info[i].nevt_rec << " " 
	     << setw(9) << proc_info[i].rec_rate << " "
	     << setw(9) << proc_info[i].nevt_proc << " " << setw(9) << proc_info[i].proc_rate << " "
	     << setw(4) << proc_info[i].flag << " " << setw(8) 
	     << setiosflags(ios::left) << proc_info[i].source << resetiosflags(ios::left) << endl;
    }
  }

  return;
}


//-----------------------------------------------------------------------


void send_info_server_msg() {


  // create message
  TipcMsg msg((T_STR)"info_server");
  msg.Dest((T_STR)"info_server/in/evt_status_monitor");


  // fill msg
  msg << (T_STR)"evt_status_monitor" << (T_INT4) nstd;
  for(int i=0; i<nstd; i++) {
    msg	<< proc_info[i].id << proc_info[i].node << proc_info[i].srvnode << proc_info[i].name
	<< proc_info[i].nevt_rec << proc_info[i].rec_rate 
	<< proc_info[i].nevt_proc << proc_info[i].proc_rate
	<< proc_info[i].flag << proc_info[i].source;
  }


  // send and flush
  server.Send(msg,TRUE);
  server.Flush();


  return;
}


//-----------------------------------------------------------------------


void status_poll_callback(T_IPC_MSG msg) {

  
  TipcMsgAppendStr(msg,(char*)"Number of broadcasts");
  TipcMsgAppendInt4(msg,nreq);

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


void decode_command_line(int argc, char **argv) {

  const char *help ="\nusage:\n\n   evt_status_monitor [-a application] [-once] [-p pause_time] [-w wait_time] [-broadcast] [-no_print] [-info] [-file]\n\n";
  int i=1;

  while(i<argc) {
    
    if(strncasecmp(argv[i],"-h",2)==0) {
      printf("%s", help);
      exit(EXIT_SUCCESS);
    } 
    else if (strncasecmp(argv[i],"-broadcast",10)==0) {
      broadcast=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_print",9)==0) {
      no_print=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-info",5)==0) {
      info=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-file",5)==0) {
      file=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-once",5)==0) {
      once=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-w",2)==0) {
      wait_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-p",2)==0) {
      pause_time=atoi(argv[i+1]);
      i=i+2;
    }
    else {
      printf("Unknown command line arg: %s\n",argv[i]);
      i=i+1;
    }
  }
}


//-------------------------------------------------------------------
