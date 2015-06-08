h32630
s 00002/00002/00313
d D 1.4 00/01/20 12:52:11 wolin 5 4
c dd2scaler switched to scaler_server
e
s 00001/00001/00314
d D 1.3 00/01/14 13:49:31 wolin 4 3
c No more DD_NAME
e
s 00016/00005/00299
d D 1.2 98/02/25 18:08:19 wolin 3 1
c Now unpacks scaler messages
c 
e
s 00000/00000/00000
d R 1.2 98/02/25 17:18:57 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 scalers/s/scaler_analysis.cc
e
s 00304/00000/00000
d D 1.1 98/02/25 17:18:56 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
//  scaler_analysis

//  analyzed scaler data from info_server message

//  E.Wolin, 25-feb-98


#define _POSIX_SOURCE 1
#define __EXTENSIONS__


// system stuff
#include <iostream.h>


// smartsockets
#include <rtworks/cxxipc.hxx>


// for clas
#include <clas_ipc_prototypes.h>


// for tcl
extern "C" {
#include <tcl.h>
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


// program variables
static char *application      = "clastest";
D 4
static char *session          = getenv("DD_NAME");
E 4
I 4
static char *session          = NULL;
E 4
D 3
static char *uniq_name        = "scalana";
E 3
I 3
static char *uniq_name        = "scaler_analysis";
E 3
static char *init_tcl_script  = NULL;
static int ipc_pend_time      = 10;
static int done               = 0;
static int debug              = 0;


//  tcl/tk variables
extern "C" {
static Tcl_Interp *interp; 
}


// prototypes
void init_tcl(void);
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
void run_control_callback(T_IPC_CONN conn, T_IPC_CONN_PROCESS_CB_DATA data, T_CB_ARG arg);
void info_server_callback(T_IPC_CONN conn, T_IPC_CONN_PROCESS_CB_DATA data, T_CB_ARG arg);
void decode_command_line(int argc, char **argv);
int tcl_help(ClientData clientdata, Tcl_Interp *interp,
		int argc, char **argv);
int tcl_quit(ClientData clientdata, Tcl_Interp *interp,
		int argc, char **argv);


// ref to ipc_server, connection created later
TipcSrv &server=TipcSrv::Instance();


//-----------------------------------------------------------------------


main(int argc, char **argv) {
  

  // synch with c i/o
  ios::sync_with_stdio();


  // decode command-line arguments
  decode_command_line(argc,argv);


  // get session name
  if(session==NULL)session="clasprod";


  // initialize IPC
  ipc_set_application(application);
  ipc_set_quit_callback(quit_callback);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_init(uniq_name,"Scaler analysis");
  fflush(NULL);


  // get Tcl interp, create tcl commands, link vars, process Tcl startup script, etc.
  interp=tclinterp_init();
  init_tcl();
  if(init_tcl_script!=NULL)Tcl_EvalFile(interp,init_tcl_script);
  tclipc_init(interp);


  // create run_control callback and receive run_control subject
  TipcMt mt_run("run_control");
  server.ProcessCbCreate(mt_run,run_control_callback,NULL);
  server.DgRecv("run_control",TRUE);
  server.Flush();


  // create info_server callback and receive run_control subject
  TipcMt mt_info("info_server");
  server.ProcessCbCreate(mt_info,info_server_callback,NULL);
  server.DgRecv("info_server",TRUE);
  server.Flush();


  // analyze scaler events
  while (done==0) {
    server.MainLoop((double)ipc_pend_time);
  }


  // done
  ipc_close();
  exit(EXIT_SUCCESS);

}


//------------------------------------------------------------------


void run_control_callback(T_IPC_CONN conn, T_IPC_CONN_PROCESS_CB_DATA data, T_CB_ARG arg) {

  T_IPC_MSG msg=data->msg;

  char *msg_transition;
  T_INT4 msg_run;
  char *msg_session;
  char *msg_config;


  // decode run control message
  TipcMsgSetCurrent(msg,0);
  TipcMsgNextStr(msg,&msg_transition);
  TipcMsgNextInt4(msg,&msg_run);
  TipcMsgNextStr(msg,&msg_session);
  TipcMsgNextStr(msg,&msg_config);


  if(debug==1) cout << "received run_control message for session,run,transition:  " 
		    << msg_session << ", " << msg_run << ", " << msg_transition << endl;


  return;
}


//-------------------------------------------------------------------


void info_server_callback(T_IPC_CONN conn, T_IPC_CONN_PROCESS_CB_DATA data, T_CB_ARG arg) {

  T_IPC_MSG msg=data->msg;
D 3
  char *info_type;
E 3
I 3
  char *info_type, *bname;
  T_INT4 *scalers;
  T_INT4 size;
E 3


  // decode info server message
  TipcMsgSetCurrent(msg,0);
  TipcMsgNextStr(msg,&info_type);
D 3
  

  if(debug==1) cout << "received info_server message type " 
E 3
I 3
  if(debug==1) cout << "Received info_server message type " 
E 3
		    << info_type << endl; 
  

I 3
D 5
  // just dd2scaler messages
E 5
I 5
  // just scaler_server messages
E 5
  // format is bank_name,array pairs
D 5
  if(strcmp(info_type,"dd2scaler")==0) {
E 5
I 5
  if(strcmp(info_type,"scaler_server")==0) {
E 5
    while(1==1) {
      if(!TipcMsgNextStr(msg,&bname))break;
      TipcMsgNextInt4Array(msg,&scalers,&size);
      if(debug==1) cout << "Found bank " << bname << " with size " << size << endl;
    }
  }


E 3
  return;
}


//-------------------------------------------------------------------


void init_tcl(){


  // program variables
  Tcl_LinkVar(interp,"application",    	      (char *)&application,           TCL_LINK_STRING);
  Tcl_LinkVar(interp,"session",               (char *)&session,               TCL_LINK_STRING);
  Tcl_LinkVar(interp,"uniq_name",     	      (char *)&uniq_name,             TCL_LINK_STRING);
  Tcl_LinkVar(interp,"ipc_pend_time",         (char *)&ipc_pend_time,         TCL_LINK_INT);


  // create Tcl commands
  Tcl_CreateCommand(interp,"help",tcl_help,
  		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,"quit",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,"stop",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,"exit",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  return;

}


//--------------------------------------------------------------------------


void status_poll_callback(T_IPC_MSG msg) {

  TipcMsgAppendStr(msg,"session");
  TipcMsgAppendStr(msg,session);

}


//------------------------------------------------------------------


void decode_command_line(int argc, char **argv){

  int i;

  i=1;
  while(i<argc) {

    if( (strcasecmp(argv[i],"-h")==0) || (strcasecmp(argv[i],"-help")==0) ){
      cout << "\n\n  scaler_readout [-a application] [-s session] [-t init_tcl_script] \n"
	"     [-u uniq_name] [-p ipc_pend_time [-debug]" << endl << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0){
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0){
      uniq_name=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-t",2)==0){
      init_tcl_script=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-p",2)==0){
      ipc_pend_time=atoi(argv[i+1]);
      i=i+2;
    }
    else { // ignore unknown command line option
      printf("\nUnknown command line arg: %s\n\n",argv[i]);
      i=i+1;
    }
  }
}

//------------------------------------------------------------------


int tcl_help(ClientData clientdata, Tcl_Interp *interp,
		int argc, char **argv){

    char *help =
    "\nTcl commands available in the ipc_monitor program:\n\n"
    " help                  print this message\n"
    " stop                  stop program\n"
    " quit                  stop program\n"
    " exit                  stop program\n"
    "\n\n Type command that require args with NO args for more information\n"
    "\n";

    Tcl_SetResult(interp,help,TCL_STATIC);

  return (TCL_OK);

}


//---------------------------------------------------------------------


void quit_callback(int sig){

  done=1;
}


//-------------------------------------------------------------------


int tcl_quit(ClientData clientdata, Tcl_Interp *interp,
	     int argc, char **argv){
  
  done=1;
  return (TCL_OK);
}


//---------------------------------------------------------------------

E 1
