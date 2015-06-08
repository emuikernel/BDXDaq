h41036
s 00001/00001/00434
d D 1.12 01/01/03 10:51:57 wolin 13 12
c is_open()
e
s 00001/00001/00434
d D 1.11 00/10/26 17:04:37 wolin 12 11
c const
e
s 00044/00043/00391
d D 1.10 00/10/26 17:03:41 wolin 11 10
c const
e
s 00007/00012/00427
d D 1.9 00/10/26 16:53:07 wolin 10 9
c const
e
s 00003/00003/00436
d D 1.8 00/09/08 13:18:30 wolin 9 8
c New clon_root
e
s 00002/00002/00437
d D 1.7 00/01/14 13:50:43 wolin 8 7
c No more DD_NAME
e
s 00026/00009/00413
d D 1.6 98/02/05 17:44:56 wolin 7 6
c Now checks TS l1 enable bit
c 
e
s 00027/00037/00395
d D 1.5 98/02/05 16:55:18 wolin 6 5
c New file format, new info server format
c 
e
s 00003/00002/00429
d D 1.4 97/11/03 17:07:58 wolin 5 4
c Minor mods
c 
e
s 00035/00017/00396
d D 1.3 97/10/23 13:24:11 wolin 4 3
c Fixed fork_and_wait
c 
e
s 00041/00007/00372
d D 1.2 97/07/10 16:23:42 wolin 3 1
c Now sends out info server message (new format)
e
s 00000/00000/00000
d R 1.2 97/07/08 16:22:41 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 scalers/s/scaler_readout.cc
c Name history : 1 0 s/scaler_readout.cc
e
s 00379/00000/00000
d D 1.1 97/07/08 16:22:40 wolin 1 0
c Periodically reads scalers from VME and ships to info_server
e
u
U
f e 0
t
T
I 1
// 
//  scaler_readout
//
D 4
//  reads scalers directly out of VME
E 4
I 4
D 6
//  reads scalers directly out of VME via dpsh script
E 6
I 6
//  reads scalers directly out of VME
E 6
//  NOTE:  does NOT reset scalers at readout!
E 4
//
D 4
//  ejw, 8-jul-97
E 4
I 4
//
//  still to do:
//     run in progress vs go state?
//
//
D 6
//  ejw, 20-aug-97
E 6
I 6
//  ejw, 5-feb-98
E 6
E 4


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
#include <strstream.h>


// for tcl
extern "C" {
#include <tcl.h>
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


// misc variables
D 10
static char *application       = "clastest";
static char *unique_id         = "scaler_readout";
I 4
D 6
static char *script_name       = "/usr/local/clas/bin/get_scalers";
E 4
static char *scaler_file_name  = "/usr/local/clas/parms/scalers/scalers.txt";
static int wait_time           = 20;
D 4
static int fork_time           = 30;
E 4
I 4
static int fork_time           = 15;
E 6
I 6
static char *script_name       = "bin/get_scalers";
D 9
static char *scaler_file_name  = "parms/scalers/get_scalers.txt";
E 9
I 9
static char *scaler_file_name  = "scalers/get_scalers.txt";
E 10
I 10
static char *application       = (char*)"clastest";
static char *unique_id         = (char*)"scaler_readout";
static char *script_name       = (char*)"bin/get_scalers";
static char *scaler_file_name  = (char*)"scalers/get_scalers.txt";
E 10
E 9
static int wait_time           = 10;
static int fork_time           = 10;
I 7
static int dp_time             = 3;
E 7
E 6
E 4
static Tcl_Interp *interp;   
static char *init_tcl_script   = NULL;
static int done                = 0;
static int dump                = 0;
I 3
D 6
static debug                   = 0;
E 6
I 6
static int debug               = 0;
E 6
E 3
static int no_ipc              = 0;
static int fail_count          = 0;
static time_t last_readout     = 0;
static time_t now;
I 6
static char filename[200];
E 6


D 6

E 6
D 3

E 3
D 4
// other prototypes
E 4
I 4
// prototypes
E 4
void decode_command_line(int argc, char **argv);
void init_tcl(void);
void process_loop(void);
I 3
D 11
void append_scalers(ifstream &file, TipcMsg &msg, char *findtag, char *msgtag);
E 3
D 10
int find_tag_line(ifstream &file, char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
E 10
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
E 11
I 11
void append_scalers(ifstream &file, TipcMsg &msg, const char *findtag, const char *msgtag);
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
extern "C" {
E 11
int tcl_help(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
int tcl_quit(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
I 10
D 11
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
E 10
extern "C" {
D 4
int fork_and_wait(char *command, int timeout);  
E 4
I 4
int get_run_status(char *session);
E 11
I 11
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
int get_run_status(const char *session);
E 11
I 6
void DP_cmd_init(char *msql_tcp_host);
void DP_cmd(char *roc, char *cmd, char *buf, int timeout);
E 6
D 11
int fork_and_wait(char *command, int timeout, ...);  
E 11
I 11
int fork_and_wait(const char *command, int timeout, ...);  
}
E 11
E 4
D 10
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
	       int severity, char *status, int code, char *text);
}
E 10


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


D 8
// session name is same as dd name
char *session          = getenv("DD_NAME");
E 8
I 8
// session name
D 11
char *session          = NULL;
E 11
I 11
static char *session          = NULL;
E 11
E 8


//--------------------------------------------------------------------------


D 11
main(int argc,char **argv){
E 11
I 11
main(int argc,char **argv) {
E 11


  int status;
  strstream temp;


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line...flags may be overridden in Tcl startup script
  decode_command_line(argc,argv);


  // set session name if not specified via env variable or on command line
D 10
  if(session==NULL)session="clasprod";
E 10
I 10
  if(session==NULL)session=(char*)"clasprod";
E 10


  // get Tcl interp, create tcl commands, link vars, process Tcl startup script, etc.
  interp=tclinterp_init();
  init_tcl();
  if(init_tcl_script!=NULL)Tcl_EvalFile(interp,init_tcl_script);


  // set ipc parameters and connect to ipc system
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
D 4
  status=ipc_init(unique_id,"VME scaler readou7t");
E 4
I 4
  status=ipc_init(unique_id,"VME scaler readout");
E 4
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
D 10
  // status=insert_msg("scaler_readout","online",unique_id,"status",0,"START",0,temp.str());
E 10


  // main loop
  process_loop();


  // done
  ipc_close();
  temp.seekp(0,ios::beg);
  temp << "Process shutdown:  scaler_readout" << ends;
D 10
  // status=insert_msg("scaler_readout","online",unique_id,"status",0,"STOP",0,temp.str());
E 10

  exit(EXIT_SUCCESS);
D 6

E 6
}
       

//--------------------------------------------------------------------------


D 11
void process_loop(void){
E 11
I 11
void process_loop(void) {
E 11

D 3
  char buf[120];
E 3
D 4
  int status;
E 4
I 4
D 6
  int status,taking_data;
E 6
I 6
  int status;
I 7
  char line[256];
E 7
E 6
E 4


I 7
  // init DP communication to rocs
  DP_cmd_init(getenv("MSQL_TCP_HOST"));


E 7
  while (done==0) {

    // pause between readouts
    server.MainLoop((double)wait_time);
I 6
    if(done==1)return;
E 6


D 4
    // read and process scalers
    if(done==0) {
E 4
I 4
D 6
    // check run status (only check for "go" for the moment...)
    taking_data=(get_run_status(session)==11)?1:0;
    // taking_data=(fork_and_wait("get_ts_go_bit",fork_time,NULL)==1)?1:0;
    
E 6
I 6
D 7
    // read scalers if run NOT in progress (only check for "go" for the moment...) ???
    if(get_run_status(session)!=11) {
E 7
I 7
    // read scalers if run NOT in progress...check l1 enable bit
    //    if(get_run_status(session)!=11) { ???
    line[0]='\0';
D 11
    DP_cmd("clastrig2","exec ts_l1_bit()",line,dp_time);
E 11
I 11
    DP_cmd((char*)"clastrig2",(char*)"exec ts_l1_bit()",line,dp_time);
E 11
    if(line[0]=='0') {
E 7
E 6
E 4

I 4
D 6
    // read scalers and process only if not taking data
    if((done==0)&&(taking_data==0)) {

E 6
E 4
      // read scalers into file
D 4
      status=fork_and_wait("get_scalers",fork_time);
E 4
I 4
D 6
      status=fork_and_wait(script_name,fork_time,NULL);
E 6
I 6
D 9
      sprintf(filename,"%s/%s",getenv("CLON_ROOT"),script_name);
E 9
I 9
      sprintf(filename,"%s/%s",getenv("CLON_PARMS"),script_name);
E 9
      status=fork_and_wait(filename,fork_time,NULL);
E 6
E 4
      if(status!=0){
	fail_count++;
	continue;
      }
      last_readout=time(NULL);
I 6
D 9
      sprintf(filename,"%s/%s",getenv("CLON_ROOT"),scaler_file_name);
E 9
I 9
      sprintf(filename,"%s/%s",getenv("CLON_PARMS"),scaler_file_name);
E 9
E 6


      // dump file to screen
      if(dump!=0) {
	strstream cmd;
D 6
	cmd << "cat " << scaler_file_name << ends;
	cout << endl << "Contents of " << scaler_file_name << " --------------------" << endl << endl;
E 6
I 6
	cmd << "cat " << filename << ends;
	cout << endl << "Contents of " << filename << " --------------------" << endl << endl;
E 6
	system(cmd.str());
	cout << endl << endl << "-----------------------End of file--------------------------"
	     << endl << endl;
      }
      

      // create info server message 
      if(no_ipc==0){
D 11
	TipcMsg message("info_server");
	message.Sender("scaler_readout");
	message.Dest("info_server");
	message << "scaler_readout";
E 11
I 11
	TipcMsg message((T_STR)"info_server");
	message.Sender((T_STR)"scaler_readout");
	message.Dest((T_STR)"info_server");
	message << (T_STR)"scaler_readout";
E 11

D 6
	ifstream file(scaler_file_name);
E 6
I 6
	ifstream file(filename);
E 6
D 13
	if(file.bad()){
E 13
I 13
	if(!file.is_open()){
E 13
D 6
	  cerr << "unable to read scaler file " << scaler_file_name << endl;
E 6
I 6
	  cerr << "unable to read scaler file " << filename << endl;
E 6
	  continue;
	}

I 3
	// append scalers to message
D 4
        append_scalers(file,message,"*TG*","TG");
	append_scalers(file,message,"*CC*","CC");
	append_scalers(file,message,"*EC*","EC");
	append_scalers(file,message,"*SC*","SC");
E 4
I 4
D 6
        append_scalers(file,message,"*TG*"  ,"TGS");
	append_scalers(file,message,"*EC*"  ,"ECS");
D 5
	append_scalers(file,message,"*SC*"  ,"SCS");
	append_scalers(file,message,"*TRIG*","TRIG");
E 5
	append_scalers(file,message,"*CC*"  ,"CCS");
I 5
	append_scalers(file,message,"*SC*"  ,"SCS");
	append_scalers(file,message,"*TRGS*","TRGS");
	append_scalers(file,message,"*L1S*","L1S");
E 6
I 6
D 7
        append_scalers(file,message,"*TG*"  ,"TGS_00");
	append_scalers(file,message,"*EC*"  ,"ECS_00");
	append_scalers(file,message,"*CC*"  ,"CCS_00");
	append_scalers(file,message,"*SC*"  ,"SCS_00");
	append_scalers(file,message,"*TRGS*","TRGS00");
E 7
I 7
        append_scalers(file,message,"*TG*"  ,"sr_TGS_00");
	append_scalers(file,message,"*EC*"  ,"sr_ECS_00");
	append_scalers(file,message,"*CC*"  ,"sr_CCS_00");
	append_scalers(file,message,"*SC*"  ,"sr_SCS_00");
	append_scalers(file,message,"*TRGS*","sr_TRGS00");
E 7
E 6
E 5
E 4
	if(debug!=0)message.Print(TutOut);
E 3

D 3
	// process sections of file...
	status=find_tag_line(file,"*SCALTAG*",buf,sizeof(buf));
	status=find_tag_line(file,"*CROC05*",buf,sizeof(buf));


E 3
	// send, flush message and close file
	server.Send(message);
	server.Flush();
	file.close();
      }

    }
    
  }
}


//--------------------------------------------------------------------------


I 3
D 12
void append_scalers(ifstream &file, TipcMsg &msg, char *findtag, char *msgtag){
E 12
I 12
void append_scalers(ifstream &file, TipcMsg &msg, const char *findtag, const char *msgtag) {
E 12

  char buf[120];
  int nscaler=0;
D 4
  unsigned int scaler[2048];
E 4
I 4
  unsigned long scaler[2048];
E 4


  if(find_tag_line(file,findtag,buf,sizeof(buf))==0){

D 4
    while( (nscaler<((sizeof(scaler)/sizeof(int))-8)) && (get_next_line(file,buf,sizeof(buf))==0) ){
E 4
I 4
    while( (nscaler<((sizeof(scaler)/sizeof(long))-8)) && (get_next_line(file,buf,sizeof(buf))==0) ){
E 4
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


E 3
void status_poll_callback(T_IPC_MSG msg){

D 11
  TipcMsgAppendStr(msg,"last_readout");
E 11
I 11
  TipcMsgAppendStr(msg,(T_STR)"last_readout");
E 11
  TipcMsgAppendStr(msg,ctime(&last_readout));

D 11
  TipcMsgAppendStr(msg,"fail_count");
E 11
I 11
  TipcMsgAppendStr(msg,(T_STR)"fail_count");
E 11
  TipcMsgAppendInt4(msg,fail_count);

D 11
  TipcMsgAppendStr(msg,"wait_time");
E 11
I 11
  TipcMsgAppendStr(msg,(T_STR)"wait_time");
E 11
  TipcMsgAppendInt4(msg,wait_time);

D 11
  TipcMsgAppendStr(msg,"fork_time");
E 11
I 11
  TipcMsgAppendStr(msg,(T_STR)"fork_time");
E 11
  TipcMsgAppendInt4(msg,fork_time);

I 7
D 11
  TipcMsgAppendStr(msg,"dp_time");
E 11
I 11
  TipcMsgAppendStr(msg,(T_STR)"dp_time");
E 11
  TipcMsgAppendInt4(msg,dp_time);

E 7
D 11
  TipcMsgAppendStr(msg,"no_ipc");
E 11
I 11
  TipcMsgAppendStr(msg,(T_STR)"no_ipc");
E 11
  TipcMsgAppendInt4(msg,no_ipc);

  return;
}


//-------------------------------------------------------------------


D 11
void quit_callback(int sig){
E 11
I 11
void quit_callback(int sig) {
E 11

  done=1;

  return;
}


//-------------------------------------------------------------------


D 11
void init_tcl(){
E 11
I 11
void init_tcl() {
E 11

  // link c and Tcl variables
D 11
  Tcl_LinkVar(interp,"application",  (char *)&application,  TCL_LINK_STRING);
  Tcl_LinkVar(interp,"unique_id",    (char *)&unique_id,    TCL_LINK_STRING);
  Tcl_LinkVar(interp,"session",      (char *)&session,      TCL_LINK_STRING);
  Tcl_LinkVar(interp,"wait_time",    (char *)&wait_time,    TCL_LINK_INT);
  Tcl_LinkVar(interp,"fork_time",    (char *)&fork_time,    TCL_LINK_INT);
I 7
  Tcl_LinkVar(interp,"dp_time",      (char *)&dp_time,      TCL_LINK_INT);
E 7
  Tcl_LinkVar(interp,"dump",         (char *)&dump,         TCL_LINK_INT);
I 3
  Tcl_LinkVar(interp,"debug",        (char *)&debug,        TCL_LINK_INT);
E 3
  Tcl_LinkVar(interp,"no_ipc",       (char *)&no_ipc,       TCL_LINK_INT);
  Tcl_LinkVar(interp,"fail_count",   (char *)&fail_count,   TCL_LINK_INT||TCL_LINK_READ_ONLY);
E 11
I 11
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
E 11


  // create Tcl commands
D 11
  Tcl_CreateCommand(interp,"help",tcl_help,
E 11
I 11
  Tcl_CreateCommand(interp,(char*)"help",tcl_help,
E 11
  		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 11
  Tcl_CreateCommand(interp,"quit",tcl_quit,
E 11
I 11
  Tcl_CreateCommand(interp,(char*)"quit",tcl_quit,
E 11
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 11
  Tcl_CreateCommand(interp,"stop",tcl_quit,
E 11
I 11
  Tcl_CreateCommand(interp,(char*)"stop",tcl_quit,
E 11
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 11
  Tcl_CreateCommand(interp,"exit",tcl_quit,
E 11
I 11
  Tcl_CreateCommand(interp,(char*)"exit",tcl_quit,
E 11
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  return;

}


//--------------------------------------------------------------------------


D 11
void decode_command_line(int argc, char**argv){
E 11
I 11
void decode_command_line(int argc, char**argv) {
E 11

D 11
  char *help = "\nusage:\n\n  scaler_readout [-a application] [-u unique_id]\n"
E 11
I 11
  const char *help = "\nusage:\n\n  scaler_readout [-a application] [-u unique_id]\n"
E 11
D 4
    "        [-fork fork_time] [-f scaler_file_name]\n" 
E 4
I 4
D 6
    "        [-fork fork_time] [-s script_name] [-f scaler_file_name]\n" 
E 6
I 6
D 7
    "        [-fork fork_time] \n" 
E 6
E 4
    "        [-t init_tcl_script] [-w wait_time] [-no_ipc] [-dump]\n";
E 7
I 7
    "        [-fork fork_time] [-t init_tcl_script] [-w wait_time] [-dp dp_time]\n"
    "        [-no_ipc] [-dump]\n";
E 7


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
I 3
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
E 3
    else if (strncasecmp(argv[i],"-fork",5)==0){
      fork_time=atoi(argv[i+1]);
      i=i+1;
    }
I 7
    else if (strncasecmp(argv[i],"-dp",3)==0){
      dp_time=atoi(argv[i+1]);
      i=i+1;
    }
E 7
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
I 4
D 6
    else if (strncasecmp(argv[i],"-s",2)==0){
      script_name=strdup(argv[i+1]);
      i=i+2;
    }
E 4
    else if (strncasecmp(argv[i],"-f",2)==0){
      scaler_file_name=strdup(argv[i+1]);
      i=i+2;
    }
E 6
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
D 11
		int argc, char **argv){
E 11
I 11
	     int argc, char **argv) {
E 11

D 11
    char *help =
E 11
I 11
    const char *help =
E 11
    "\nTcl commands available in the scaler_readout program:\n\n"
    " help                  print this message\n"
    " stop                  stop program\n"
    " quit                  stop program\n"
    " exit                  stop program\n"
    "\n\n Type command that require args with NO args for more information\n"
    "\n";

D 11
    Tcl_SetResult(interp,help,TCL_STATIC);
E 11
I 11
    Tcl_SetResult(interp,(char*)help,TCL_STATIC);
E 11

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
