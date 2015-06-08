// 
//  alarm_handler
// 
//  handles ipc alarms
//
//  ejw, 18-may-99
//
//  still to do:
//    revamp error handling in conjunction with Dave A.


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


// misc variables
static char *application      = (char *)"clastest";
static char *unique_id        = (char *)"alarm_handler";
static Tcl_Interp *interp;  
static char *init_tcl_script  = NULL;
static int rcs_deadtime       = 120;   // seconds
static int eb1_deadtime       = 120;  // seconds
static int nmsg               = 0;
static int nmsg_ignored       = 0;
static int nmsg_handled       = 0;
static int done               = 0;
static int debug              = 0;
static char temp[512];
static time_t last_rcs        = 0;
static time_t last_eb1        = 0;



// prototypes
void decode_command_line(int argc, char **argv);
void init_tcl();
void cmlog_callback(T_IPC_CONN,
		    T_IPC_CONN_PROCESS_CB_DATA,
		    T_CB_ARG);
extern "C" {
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
int tcl_help(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
int tcl_quit(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
	      int severity, char *msgstatus, int code, char *text);
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
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
  status=ipc_init(unique_id,(char *)"alarm_handler");
  if(status<0) {
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another alarm_handler using ipc_info\n"
	 << "   ...only one instance allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }
  TipcMt mt((char*)"cmlog");
  server.ProcessCbCreate(mt,cmlog_callback,0);
  server.SubjectSubscribe((char*)"cmlog",TRUE);


  // enable tcl callback
  tclipc_init(interp);


  // post startup message
  sprintf(temp,"Process startup:  %15s in application %s",unique_id,application);
  status=insert_msg((char*)"alarm_handler",(char*)"online",unique_id,(char*)"status",0,
		    (char*)"START",0,temp);


  //  handle alarms, ipc messages, etc
  while (done==0) {
    server.MainLoop(1.0);
  }


  // done
  ipc_close();
  sprintf(temp,"Process shutdown:  %15s",unique_id);
  status=insert_msg((char*)"alarm_handler",(char*)"online",unique_id,(char*)"status",0,
		    (char*)"STOP",0,temp);

  exit(EXIT_SUCCESS);
}
       

//--------------------------------------------------------------------------


void cmlog_callback(T_IPC_CONN conn,
		    T_IPC_CONN_PROCESS_CB_DATA data,
		    T_CB_ARG arg) {

  int status;
  T_STR domain;
  T_STR host;
  T_STR user;
  T_INT4 msgtime;

  T_STR name;
  T_STR facility;
  T_STR process;
  T_STR msgclass;
  T_INT4 severity;
  T_STR msgstatus;
  T_INT4 code;
  T_STR text;

  TipcMsg msg(data->msg);
  time_t now = time(NULL);


  nmsg++;


  // unpack message
  msg >> domain >> host >> user >> msgtime
      >> name >> facility >> process >> msgclass 
      >> severity >> msgstatus >> code >> text;


  // ignore non-clas user
  if((strncasecmp(user,"clas",4)!=0)) {
    nmsg_ignored++;
    return;
  }


  // remove newlines from text
  for(int i=0; i<strlen(text); i++) if(text[i]=='\n')text[i]='\0';


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
    sprintf(temp,"/bin/csh -c \'(setenv DISPLAY clon03:0.0; $CLON_BIN/alarm_window %s:  %s)\'&",
	    "SEVERE error in run control server, run must be stopped",text);
    cout << temp << endl;
    system(temp);
    last_rcs=now;


  // notify operator about any scaler1 error...EJW, 22-may-2002
  } else if((severity>=2)&&(strcasecmp(process,"scaler1")==0)) {
    nmsg_handled++;
    cout << "Handling ERROR from SCALER1 " << ctime(&now);
    sprintf(temp,"/bin/csh -c \'(setenv DISPLAY clon03:0.0; $CLON_BIN/alarm_window %s:  %s)\'&",
	    "SCALER1 ERROR, crate should be rebooted",text);
    cout << temp << endl;
    system(temp);


  // notify operator about any EB error...EJW, 22-may-2002
  } else if( (severity>=2)                   &&
	     (strcasecmp(process,"EB1")==0)  &&
	     ((now-last_eb1)>eb1_deadtime)
	     ) {
    nmsg_handled++;
    cout << "Handling ERROR from EB1 " << ctime(&now);
    sprintf(temp,"/bin/csh -c \'(setenv DISPLAY clon03:0.0; $CLON_BIN/alarm_window %s:  %s)\'&",
	    "EB1 ERROR, run must be stopped",text);
    cout << temp << endl;
    system(temp);
    last_eb1=now;

  } else {
    nmsg_ignored++;
  }
  

  return;
}

  
//----------------------------------------------------------------


extern "C" {
void status_poll_callback(T_IPC_MSG msg) {

  TipcMsgAppendStr(msg,(char*)"Number of messages received");
  TipcMsgAppendInt4(msg,nmsg);
  
  TipcMsgAppendStr(msg,(char*)"Number of messages ignored");
  TipcMsgAppendInt4(msg,nmsg_ignored);
  
  TipcMsgAppendStr(msg,(char*)"Number of messages handled");
  TipcMsgAppendInt4(msg,nmsg_handled);
  
  return;
}
}


//-------------------------------------------------------------------


extern "C" {
void quit_callback(int sig) {


  cout << "...stopping...received signal " << sig << endl;
  done=1;
  return;
}
}


//-------------------------------------------------------------------


void decode_command_line(int argc, char**argv) {

  const char *help = "\nusage:\n\n  alarm_handler [-a application] [-u unique_id]"
    " [-t init_tcl_script] [-debug]\n";


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
    else if (strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0) {
      unique_id=strdup(argv[i+1]);
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
  Tcl_LinkVar(interp,(char*)"application", (char *)&application,  TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"unique_id",   (char *)&unique_id,    TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"nmsg",        (char *)&nmsg,         TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nmsg_ignored",(char *)&nmsg_ignored, TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nmsg_handled",(char *)&nmsg_handled, TCL_LINK_INT|TCL_LINK_READ_ONLY);


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


extern "C" {
int tcl_help(ClientData clientdata, Tcl_Interp *interp,
		int argc, char **argv) {

  char *help = (char*)
    "\nTcl commands available in the alarm_handler program:\n\n"
    " help                  print this message\n"
    " stop                  stop program\n"
    " quit                  stop program\n"
    " exit                  stop program\n"
    "\n\n Type command that require args with NO args for more information\n"
    "\n";

    Tcl_SetResult(interp,help,TCL_STATIC);

  return (TCL_OK);

}
}


//---------------------------------------------------------------------


extern "C" {
int tcl_quit(ClientData clientdata, Tcl_Interp *interp,
	     int argc, char **argv) {
  
  cout << "...received Tcl quit message..." << endl;
  done=1;
  
  return (TCL_OK);
}
}


//---------------------------------------------------------------------
