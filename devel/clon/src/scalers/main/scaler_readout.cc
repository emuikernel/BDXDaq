// 
//  scaler_readout
//
//  reads scalers directly out of VME
//  NOTE:  does NOT reset scalers at readout!
//
//
//  still to do:
//     run in progress vs go state?
//
//
//  ejw, 5-feb-98


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

using namespace std;
#include <strstream>


// for tcl
extern "C" {
#include <tcl.h>
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}

// for tcpClientCmd
#include "libtcp.h"

// misc variables
static char *application       = (char*)"clastest";
static char *unique_id         = (char*)"scaler_readout";
static char *script_name       = (char*)"bin/get_scalers";
static char *scaler_file_name  = (char*)"scalers/get_scalers.txt";
static int wait_time           = 10;
static int fork_time           = 10;
static int dp_time             = 3;
static Tcl_Interp *interp;   
static char *init_tcl_script   = NULL;
static int done                = 0;
static int dump                = 0;
static int debug               = 0;
static int no_ipc              = 0;
static int fail_count          = 0;
static time_t last_readout     = 0;
static time_t now;
static char filename[200];


// prototypes
void decode_command_line(int argc, char **argv);
void init_tcl(void);
void process_loop(void);
void append_scalers(ifstream &file, TipcMsg &msg, const char *findtag, const char *msgtag);
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
extern "C" {
int tcl_help(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
int tcl_quit(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
int get_run_status(const char *session);
int fork_and_wait(const char *command, int timeout, ...);  
}


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


// session name
static char *session          = NULL;


//--------------------------------------------------------------------------


main(int argc,char **argv) {


  int status;
  strstream temp;


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line...flags may be overridden in Tcl startup script
  decode_command_line(argc,argv);


  // set session name if not specified via env variable or on command line
  if(session==NULL)session=(char*)"clasprod";


  // get Tcl interp, create tcl commands, link vars, process Tcl startup script, etc.
  interp=tclinterp_init();
  init_tcl();
  if(init_tcl_script!=NULL)Tcl_EvalFile(interp,init_tcl_script);


  // set ipc parameters and connect to ipc system
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
  status=ipc_init(unique_id,"VME scaler readout");
  if(status<0){
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another scaler_readout  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }


  // enable Tcl ipc control (i.e. create tcl_request callback)
  tclipc_init(interp);


  // post startup message
  temp << "Process startup:    scaler_readout starting in " << application << ends;


  // main loop
  process_loop();


  // done
  ipc_close();
  temp.seekp(0,ios::beg);
  temp << "Process shutdown:  scaler_readout" << ends;

  exit(EXIT_SUCCESS);
}
       

//--------------------------------------------------------------------------


void process_loop(void) {

  int status;
  char line[256];

  while (done==0) {

    // pause between readouts
    server.MainLoop((double)wait_time);
    if(done==1)return;


    // read scalers if run NOT in progress...check l1 enable bit
    //    if(get_run_status(session)!=11) { ???
    line[0]='\0';
    tcpClientCmd("clastrig2","ts_l1_bit()",line);
    if(line[0]=='0') {

      // read scalers into file
      sprintf(filename,"%s/%s",getenv("CLON_PARMS"),script_name);
      status=fork_and_wait(filename,fork_time,NULL);
      if(status!=0){
	fail_count++;
	continue;
      }
      last_readout=time(NULL);
      sprintf(filename,"%s/%s",getenv("CLON_PARMS"),scaler_file_name);


      // dump file to screen
      if(dump!=0) {
	strstream cmd;
	cmd << "cat " << filename << ends;
	cout << endl << "Contents of " << filename << " --------------------" << endl << endl;
	system(cmd.str());
	cout << endl << endl << "-----------------------End of file--------------------------"
	     << endl << endl;
      }
      

      // create info server message 
      if(no_ipc==0){
	TipcMsg message((T_STR)"info_server");
	message.Sender((T_STR)"scaler_readout");
	message.Dest((T_STR)"info_server");
	message << (T_STR)"scaler_readout";

	ifstream file(filename);
	if(!file.is_open()){
	  cerr << "unable to read scaler file " << filename << endl;
	  continue;
	}

	// append scalers to message
        append_scalers(file,message,"*TG*"  ,"sr_TGS_00");
	append_scalers(file,message,"*EC*"  ,"sr_ECS_00");
	append_scalers(file,message,"*CC*"  ,"sr_CCS_00");
	append_scalers(file,message,"*SC*"  ,"sr_SCS_00");
	append_scalers(file,message,"*TRGS*","sr_TRGS00");
	if(debug!=0)message.Print(TutOut);

	// send, flush message and close file
	server.Send(message);
	server.Flush();
	file.close();
      }

    }
    
  }
}


//--------------------------------------------------------------------------


void append_scalers(ifstream &file, TipcMsg &msg, const char *findtag, const char *msgtag) {

  char buf[120];
  int nscaler=0;
  unsigned long scaler[2048];


  if(find_tag_line(file,findtag,buf,sizeof(buf))==0){

    while( (nscaler<((sizeof(scaler)/sizeof(long))-8)) && (get_next_line(file,buf,sizeof(buf))==0) ){
      istrstream s(buf,sizeof(buf));
      for(int i=0; i<8; i++) s >> scaler[nscaler+i];
      nscaler+=8;
    }

    if(nscaler>0){
      msg << (T_STR) msgtag;
      msg << SetSize(nscaler) << (T_INT4*) scaler;
    }

  }

}


//--------------------------------------------------------------------------


void status_poll_callback(T_IPC_MSG msg){

  TipcMsgAppendStr(msg,(T_STR)"last_readout");
  TipcMsgAppendStr(msg,ctime(&last_readout));

  TipcMsgAppendStr(msg,(T_STR)"fail_count");
  TipcMsgAppendInt4(msg,fail_count);

  TipcMsgAppendStr(msg,(T_STR)"wait_time");
  TipcMsgAppendInt4(msg,wait_time);

  TipcMsgAppendStr(msg,(T_STR)"fork_time");
  TipcMsgAppendInt4(msg,fork_time);

  TipcMsgAppendStr(msg,(T_STR)"dp_time");
  TipcMsgAppendInt4(msg,dp_time);

  TipcMsgAppendStr(msg,(T_STR)"no_ipc");
  TipcMsgAppendInt4(msg,no_ipc);

  return;
}


//-------------------------------------------------------------------


void quit_callback(int sig) {

  done=1;

  return;
}


//-------------------------------------------------------------------


void init_tcl() {

  // link c and Tcl variables
  Tcl_LinkVar(interp,(char*)"application",  (char *)&application,  TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"unique_id",    (char *)&unique_id,    TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"session",      (char *)&session,      TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"wait_time",    (char *)&wait_time,    TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"fork_time",    (char *)&fork_time,    TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"dp_time",      (char *)&dp_time,      TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"dump",         (char *)&dump,         TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"debug",        (char *)&debug,        TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"no_ipc",       (char *)&no_ipc,       TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"fail_count",   (char *)&fail_count,   TCL_LINK_INT||TCL_LINK_READ_ONLY);


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


void decode_command_line(int argc, char**argv) {

  const char *help = "\nusage:\n\n  scaler_readout [-a application] [-u unique_id]\n"
    "        [-fork fork_time] [-t init_tcl_script] [-w wait_time] [-dp dp_time]\n"
    "        [-no_ipc] [-dump]\n";


  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-dump",5)==0){
      dump=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-fork",5)==0){
      fork_time=atoi(argv[i+1]);
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-dp",3)==0){
      dp_time=atoi(argv[i+1]);
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_ipc",7)==0){
      no_ipc=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0){
      unique_id=strdup(argv[i+1]);
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
    else if (strncasecmp(argv[i],"-",1)==0) {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
      i=i+2;
    }
  }

  return;
}

  
//----------------------------------------------------------------


int tcl_help(ClientData clientdata, Tcl_Interp *interp,
	     int argc, char **argv) {

    const char *help =
    "\nTcl commands available in the scaler_readout program:\n\n"
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
	     int argc, char **argv){
  
  done=1;
  
  return (TCL_OK);
}


//---------------------------------------------------------------------


