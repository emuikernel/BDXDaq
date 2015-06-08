h46089
s 00004/00001/00488
d D 1.4 07/10/12 09:56:03 boiarino 4 3
c *** empty log message ***
e
s 00002/00004/00487
d D 1.3 06/10/31 10:13:03 wolin 3 2
c Forgot to reinitialize mysql memory after connection close/reopen
c 
e
s 00009/00007/00482
d D 1.2 06/08/22 22:23:58 boiarino 2 1
c *** empty log message ***
e
s 00489/00000/00000
d D 1.1 06/07/01 10:27:06 boiarino 1 0
c date and time created 06/07/01 10:27:06 by boiarino
e
u
U
f e 0
t
T
I 1
// to do:   need mysql_autocommit
// 
//  alarm_server
//
//  monitors ipc alarm system and updates mysql database
//
//  still to do:
//     reenable alh alarm processing (probably never will...jun-2006)
//
//  ejw, 18-may-99
//  ejw, 20jun-2006 switched to mysql from ingres


D 2
// CC -c alarm_server.cc -I$RTHOME/include -I/usr/include/mysql/ -I/usr/local/clas/devel_new/include -I/usr/local/coda/2.2.1/common/include
//rtlink -cxx -o alarm_server alarm_server.o -L/usr/lib/mysql/ -lmysqlclient -L/usr/local/clas/devel_new/SunOS_sun4u/lib -lipc -lutil -L/usr/local/coda/2.2.1/SunOS/lib -ltcl
E 2
I 2
// CC -c alarm_server.cc -I$RTHOME/include -I/usr/include/mysql/ 
//         -I/usr/local/clas/devel_new/include -I/usr/local/coda/2.2.1/common/include
//rtlink -cxx -o alarm_server alarm_server.o -L/usr/lib/mysql/ -lmysqlclient \
//         -L/usr/local/clas/devel_new/SunOS_sun4u/lib -lipc -lutil -L/usr/local/coda/2.2.1/SunOS/lib -ltcl
E 2

// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for smartsockets
#include <rtworks/cxxipc.hxx>


// for mysql
D 2
#include <mysql.h>
E 2
I 2
#include <mysql/mysql.h>
E 2


// system
D 4
#include <strstream.h>
E 4
I 4

using namespace std;
#include <strstream>

E 4
#include <fstream.h>
#include <iomanip.h>
#include <time.h>
#include <sys/types.h>


// for tcl
extern "C" {
#include <tcl.h>
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


// local variables used by mysql
D 2
static char *dbhost                = (char*)"clonpc6";
static char *dbuser                = (char*)"wolin";
static char *database              = (char*)"test";
E 2
I 2
static char *dbhost                = (char*)"clondb1";
static char *dbuser                = (char*)"clasrun";
static char *database              = (char*)"clasprod";
E 2
static MYSQL *dbhandle             = NULL;


// for CLAS ipc
#include <clas_ipc_prototypes.h>


// misc variables
static char *application      = (char*)"clastest";
static char *unique_id        = (char*)"alarm_server";
static Tcl_Interp *interp;  
static char *init_tcl_script  = NULL;
static time_t start           = time(NULL);
static int connected          = 0;
static time_t last_db_entry   = 0;
static int disconnect_timeout = 30;  // seconds
static int nolog              = 0;
static int done               = 0;
static int debug              = 0;
static int ncmlog             = 0;
static int nalarm             = 0;
static int nclear             = 0;
static int db_bad_count       = 0;
char temp[1024];


// other prototypes
void decode_command_line(int argc, char **argv);
void init_tcl(void);
void cmlog_callback(T_IPC_CONN,
		    T_IPC_CONN_PROCESS_CB_DATA,
		    T_CB_ARG);
extern "C" {
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
	       int severity, char *status, int code, char *text);
int tcl_help(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
int tcl_quit(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
}	


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//--------------------------------------------------------------------------


main(int argc,char **argv){

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
  status=ipc_init(unique_id,(char*)"alarm server");
  if(status<0){
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another alarm_server  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }
  TipcMt mt((char*)"cmlog");
  server.ProcessCbCreate(mt,cmlog_callback,0);
  server.SubjectSubscribe((char*)"cmlog",TRUE);


  //  create tcl_request callback
  tclipc_init(interp);


D 3
  // init db connection
  dbhandle=mysql_init(NULL);


E 3
  // post startup message
  sprintf(temp,"Process startup:   %15s  in application:  %s",unique_id,application);
  status=insert_msg((char*)"alarm_server",(char*)"alarm_server",unique_id,(char*)"status",
		    0,(char*)"START",0,temp);
  cout << "\n\nalarm_server startup at " << ctime(&start) << flush;


  // process ipc alarm messages
  while (done==0) {
    server.MainLoop(1.0);

    // disconnect from database if connected and timeout reached
    if((connected==1)&&(time(NULL)-last_db_entry)>disconnect_timeout) {
      mysql_close(dbhandle);
I 3
      dbhandle=NULL;
E 3
      connected=0;
    }
  }


  // done
  sprintf(temp,"Process shutdown:  %15s",unique_id);
  status=insert_msg((char*)"alarm_server",(char*)"alarm_server",unique_id,(char*)"status",
		    0,(char*)"STOP",0,temp);
  time_t now = time(NULL);
  cout << "\n    alarm_server processed " << nalarm << " alarms, " 
       << nclear << " clears" << endl;
  cout << "\nalarm_server stopping at " << ctime(&now) << endl;
  ipc_close();

  exit(EXIT_SUCCESS);
}
       

//--------------------------------------------------------------------------


void cmlog_callback(T_IPC_CONN conn,
		    T_IPC_CONN_PROCESS_CB_DATA data,
		    T_CB_ARG arg) {

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

  int alarm_status,code_db;
  char alarm_time[30];
  char msgclass_db[30];
  char atime[30];
  double interval;

  tm *tstruct;
  char *comma=(char*)",", *prime=(char*)"'";


  ncmlog++;


  // unpack message
  TipcMsg msg(data->msg);
  msg >> domain >> host >> user >> msgtime
      >> name >> facility >> process >> msgclass 
      >> severity >> msgstatus >> code >> text;


  // only process clonalarm messages
  if(strcasecmp(facility,"clonalarm")!=0)return;
  if(code<=0)return;
  
  
  // ignore alh alarms for the moment...28-oct-98, EJW
  if(strcasecmp(process,"alh")==0)return;
  
  
  // connect to database if not connected
  if(connected==0) {
I 3
    if(dbhandle==NULL)dbhandle=mysql_init(NULL);
E 3
    if(mysql_real_connect(dbhandle,dbhost,dbuser,NULL,database,0,NULL,0)) {
      connected=1;
D 2
      mysql_autocommit(dbhandle,1);
E 2
I 2
      // <wait for mysql5.0> mysql_autocommit(dbhandle,1);
E 2
    } else {
      if((db_bad_count%50)==1) {
      if(1==1) {
	time_t now = time(NULL);
	cerr << "Unable to connect to database (error: " << mysql_errno(dbhandle) 
	     << ", " << mysql_error(dbhandle) << ") on " 
	     << ctime(&now) << endl;
	insert_msg((char*)"alarm_server",(char*)"alarm_server",unique_id,(char*)"status",
		   3,(char*)"SEVERE",0,(char*)"Unable to connect to database");
      }
      return;
      }
    }
  }

  // save time of last db access
  last_db_entry=time(NULL);
  
  
// get most recent status,time since last update for this alarm id
//    tstruct = localtime(&msgtime);
//    strftime(atime,sizeof(atime),"%d-%b-%Y %H:%M",tstruct);
//    code_db=code;
//    strcpy(msgclass_db,msgclass);
//    exec sql select alarm_status,interval('secs',date(:atime)-alarm_time)
//      into :alarm_status,:interval from ingres.clonalarm 
//      where system=:msgclass_db and alarm_id=:code_db;
//    if(sqlca.sqlcode!=0) {
//      cerr << "?bad class,code,severity: " << msgclass << "," << code << "," 
//  	 << severity << endl;
//      return;
//    }
  
  
  // get alarm time
  tstruct = localtime(&msgtime);
  strftime(atime,sizeof(atime),"%Y-%m-%d %H:%M",tstruct);
  
  
  // form sql string
  strstream sql;
  sql << "update clonalarm set "
      << "alarm_status=" << severity << comma
      << "alarm_time=" << prime << atime << prime
      << " where alarm_id=" << code 
      << " and system=" << prime << msgclass << prime 
      << " and alarm_status!=" << severity
      << " and alarm_time<=" << prime << atime << prime  
      << ends;
    
    
    // update database...only happens if timestamp more recent and status changed
    // also count alarms, clears, etc.
  if(debug==0) {
    mysql_query(dbhandle,sql.str());
    unsigned int error = mysql_errno(dbhandle);
    if(error!=0) {
      cerr << "Error: " << error << "for: " << endl << sql.str() << endl 
	   << mysql_error(dbhandle) << endl;
    }
    
    int cnt;
    if((cnt=mysql_affected_rows(dbhandle))>0) {
      //	cout << cnt << " rows affected" << endl;
      if(severity>0) nalarm++; else nclear++;
    } else {
      //	cout << "no rows affected" << endl;
    }
    
  } else {
    cout << "sql is: " << sql.str() << endl;
  }
  

  // update log file
  if((debug==0)&&(nolog==0)) {
    cout << atime << ":   " << setw(15) << msgclass << "   " << setw(5) << code 
	 << "   " << severity << "   " << text << endl;
  }
  
  return;
}


//--------------------------------------------------------------------------


extern "C" {
void status_poll_callback(T_IPC_MSG msg){

  TipcMsgAppendStr(msg,(char*)"connected");
  TipcMsgAppendInt4(msg,connected);

  TipcMsgAppendStr(msg,(char*)"ncmlog");
  TipcMsgAppendInt4(msg,ncmlog);

  TipcMsgAppendStr(msg,(char*)"nalarm");
  TipcMsgAppendInt4(msg,nalarm);

  TipcMsgAppendStr(msg,(char*)"nclear");
  TipcMsgAppendInt4(msg,nclear);

  TipcMsgAppendStr(msg,(char*)"db_bad_count");
  TipcMsgAppendInt4(msg,db_bad_count);

  return;
}
}


//-------------------------------------------------------------------


extern "C" {
void quit_callback(int sig){

  done=1;

  return;
}
}


//-------------------------------------------------------------------


void init_tcl(){

  // link c and Tcl variables
  Tcl_LinkVar(interp,(char*)"application", (char *)&application,   TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"unique_id",   (char *)&unique_id,     TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"nolog",       (char *)&nolog,         TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"debug",       (char *)&debug,         TCL_LINK_BOOLEAN);
  Tcl_LinkVar(interp,(char*)"disconnect_timeout",(char *)&disconnect_timeout,TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"ncmlog",      (char *)&ncmlog,        TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nalarm",      (char *)&nalarm,        TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nclear",      (char *)&nclear,        TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"db_bad_count",(char *)&db_bad_count,  TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"connected",   (char *)&connected,     TCL_LINK_BOOLEAN|TCL_LINK_READ_ONLY);


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


void decode_command_line(int argc, char**argv){

  const char *help = "\nusage:\n\n  alarm_server [-a application] [-u unique_id]\n"
    "               [-host dbhost] [-user dbuser] [-db database] [-t init_tcl_script]\n"
    "               [-dtime disconect_timeout] [-nolog] [-debug]\n";


  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-nolog",6)==0){
      nolog=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-host",5)==0){
      dbhost=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-db",3)==0){
      database=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-user",5)==0){
      dbuser=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-dtime",6)==0){
      disconnect_timeout=atoi(argv[i+1]);
      i=i+2;
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
    else if (strncasecmp(argv[i],"-",1)==0) {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
      i=i+2;
    }
  }

  return;
}

  
//----------------------------------------------------------------


extern "C" {
int tcl_help(ClientData clientdata, Tcl_Interp *interp,
		int argc, char **argv){

  char *help = (char*)
    "\nTcl commands available in the alarm_server program:\n\n"
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
	     int argc, char **argv){
  
  done=1;
  
  return (TCL_OK);
}
}


//---------------------------------------------------------------------

E 1
