h43243
s 00106/00072/01010
d D 1.41 07/11/06 14:15:08 boiarino 43 42
c change '>=' to '>' in 'if(i /*>=*/> NumFile)'
c 
e
s 00007/00000/01075
d D 1.40 07/10/12 12:04:09 boiarino 42 41
c *** empty log message ***
e
s 00075/00069/01000
d D 1.39 07/10/12 10:10:57 boiarino 41 40
c *** empty log message ***
e
s 00001/00000/01068
d D 1.38 05/11/18 10:05:01 wolin 40 39
c Memory leak in process_backlog fixed
e
s 00002/00002/01066
d D 1.37 01/01/03 10:43:35 wolin 39 38
c is_open()
e
s 00004/00004/01064
d D 1.36 00/12/05 18:15:10 wolin 38 37
c New CC
e
s 00075/00071/00993
d D 1.35 00/10/27 12:39:35 wolin 37 36
c const
e
s 00013/00001/01051
d D 1.34 98/11/05 12:16:40 wolin 36 35
c Seems to work...must look at this again
e
s 00049/00035/01003
d D 1.33 98/03/12 16:39:32 wolin 35 34
c Many bombproofings and minor improvements
c 
e
s 00118/00105/00920
d D 1.32 98/03/11 16:49:45 wolin 34 33
c More file i/o bomproofing, other minor cleanups
c 
e
s 00021/00011/01004
d D 1.31 98/01/06 16:25:59 wolin 33 32
c Now working with ss40 and int4ArrayPtr
c 
e
s 00000/00000/01015
d D 1.30 97/06/26 09:13:23 clasrun 32 31
c no changes
e
s 00005/00003/01010
d D 1.29 97/06/24 09:23:08 wolin 31 30
c Process backlog after all msg stored on disk
e
s 00003/00002/01010
d D 1.28 97/06/23 12:07:21 wolin 30 29
c Fixed bug for empty sql string
e
s 00010/00008/01002
d D 1.27 97/06/11 13:05:19 wolin 29 28
c New backlog dir name
e
s 00034/00016/00976
d D 1.26 97/06/06 12:24:23 wolin 28 27
c Uniq dgrp and subscribed datagroup now different
e
s 00002/00002/00990
d D 1.25 97/06/02 16:52:39 wolin 27 26
c Fixed help 
e
s 00004/00004/00988
d D 1.24 97/05/23 12:03:57 wolin 26 25
c Using new cmlog
e
s 00004/00004/00988
d D 1.23 97/05/19 16:00:42 wolin 25 24
c Removed insert_msg, default database is db3::clastest
e
s 00009/00069/00983
d D 1.22 97/04/02 12:10:33 wolin 24 23
c Using new cmlog message system
e
s 00006/00006/01046
d D 1.21 97/03/10 14:49:42 wolin 23 22
c Removed msql stuff
e
s 00001/00001/01051
d D 1.20 97/02/18 12:33:23 wolin 22 21
c Typo
e
s 00002/00002/01050
d D 1.19 97/02/18 12:31:49 wolin 21 20
c Typo
e
s 00001/00001/01051
d D 1.18 97/02/18 11:05:30 wolin 20 19
c Typo
e
s 00083/00003/00969
d D 1.17 97/02/18 09:35:21 wolin 19 18
c Added start/stop message to clasmsg database
e
s 00047/00083/00925
d D 1.16 97/01/17 14:24:39 wolin 18 17
c Improved exit failure code
e
s 00137/00009/00871
d D 1.15 97/01/15 14:04:31 wolin 17 16
c Added tcl control
e
s 00001/00001/00879
d D 1.14 97/01/14 10:25:51 wolin 16 15
c Fixed bug in binary message processing
e
s 00048/00033/00832
d D 1.13 97/01/07 16:15:49 wolin 15 14
c Now sends error message in dbr_reply, other minor mods
e
s 00001/00001/00864
d D 1.12 96/11/26 17:34:00 wolin 14 13
c Added +x, g+x to backlog dir createion
e
s 00006/00013/00859
d D 1.11 96/09/05 13:34:27 wolin 13 12
c New scheme for mt initialization 
e
s 00001/00001/00871
d D 1.10 96/09/04 13:12:21 wolin 12 11
c Switched to posix-compliant mkdir
e
s 00003/00006/00869
d D 1.9 96/09/04 09:47:55 wolin 11 10
c Minor bug in length of Fullname variable caused problems on HP but not Sun!?
e
s 00001/00001/00874
d D 1.8 96/08/28 13:30:47 wolin 10 9
c Typo concerning sql_trans_count
e
s 00001/00003/00874
d D 1.7 96/08/27 17:15:04 wolin 9 8
c Bug in -debug flag
e
s 00000/00000/00877
d D 1.6 96/08/27 14:49:42 wolin 8 7
c Got pointer to message structure too early!
e
s 00044/00088/00833
d D 1.5 96/08/09 15:34:23 aswin 7 6
c Bombproofing.  Added new Exit_Error function for failed calls.
e
s 00000/00000/00921
d D 1.4 96/08/09 11:19:31 aswin 6 4
i 5
c Merged changes between child workspace "/home/aswin/dbr2" and
c  parent workspace "/usr/local/clas/work_src/dbrouter".
c 
e
s 00272/00087/00642
d D 1.2.1.1 96/08/09 11:08:30 aswin 5 3
c Added sort for backlog files, added packed binary message stuff,
c other minor mods and bombproofing
e
s 00007/00000/00729
d D 1.3 96/08/02 13:44:47 wolin 4 3
c Added database to status_poll_result message 
e
s 00000/00000/00729
d D 1.2 96/07/30 14:23:22 wolin 3 1
c Now working on SUN with new Ingres installation
e
s 00000/00000/00000
d R 1.2 96/07/26 17:10:07 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 dbrouter/s/dbrouter.cc
c Name history : 1 0 s/dbrouter.cc
e
s 00729/00000/00000
d D 1.1 96/07/26 17:10:06 wolin 1 0
c CLAS database router
e
u
U
f e 0
t
T
I 41
// CC -c dbrouter.cc -I. -I$RTHOME/include -I/usr/local/clas/devel_new/include -I/usr/local/coda/2.2.1/common/include -I/usr/include/mysql
// rtlink -cxx -o dbrouter dbrouter.o dbrsql.o -L/usr/lib/mysql/ -lmysqlclient -L/usr/local/clas/devel_new/SunOS_sun4u/lib -lipc -lutil -L/usr/local/coda/2.2.1/SunOS/lib -ltcl


E 41
I 1
//  dbrouter
//
D 41
//  routes database requests to ingres
E 41
I 41
//  routes database requests
E 41
//
D 34
//
//  Usage:
//
D 17
//     dbrouter [-u uniq_dgrp] [-a application] [-d database] [-b backup_dir] [-debug]
E 17
I 17
D 28
//       dbrouter [-a application] [-u uniq_dgrp] [-d database] [-b backlog_dir] 
E 28
I 28
D 29
//       dbrouter [-a application] [-u uniq_dgrp] [-d database] [-dgrp datagroup] [-b backlog_dir] 
E 29
I 29
//       dbrouter [-a application] [-u uniq_dgrp] [-d database] [-dgrp datagroup] [-b backlog_name] 
E 29
E 28
D 27
//                [-l logfile] [-e errorfile] [-t tcl_script] [-debug]
E 27
I 27
//                [-t tcl_script] [-debug]
E 27
E 17
//
E 34
//       default application     = "clastest"
D 28
//       default uniq_dgrp       = "dbrouter"
E 28
I 28
//       default uniq_name       = "dbrouter"
E 28
D 25
//       default database        = "clastest"
E 25
I 25
D 34
//       default database        = "db3::clastest"
I 28
//       default datagroup       = "dbrouter"
E 34
I 34
D 41
//       default database        = "db5::clastest"
E 41
I 41
//       default dbname          = "test"
E 41
E 34
E 28
E 25
I 17
D 29
//       default backlog_dir     = "./backlog"
E 29
I 29
//       default backlog_dir     = "./backlog_<application>"
E 29
D 34
//       default logfile         = "dbrouter.log"
//       default errfile         = "dbrouter.err"
E 34
I 34
//       default logfile         = "dbrouter_<application>.log"
//       default errfile         = "dbrouter_<application>.err"
E 34
//       default tcl_script      = NULL
D 34
//       debug causes all incoming and outgoing messages to be logged to special file 
E 34
I 34
//       debug causes all incoming and outgoing messages to be logged to file 
E 34
E 17
//
//
D 17
//  Debug flag causes all incoming and outgoing messages to be logged to file 
E 17
D 34
//
E 34
D 17
//
E 17
//  Still to do:
D 17
// 
D 5
//     must sort backlog files
//     create backlog dir if it doesn't exist
E 5
I 5
D 7


E 5
//     *** support Jay's packed-data format ***
E 7
//     Format startup/shutdown messages
E 17
I 17
D 18
//     this file needs to be broken up!
E 18
I 18
//     this file needs to be cleaned up
E 18
E 17
D 11
//     Check all error returns
//     some cleanup and streamlining
E 11
//     use const
I 35
//     combine with dbrsql.scc into dbrouter.scc
E 35
D 18
//     control messages in quit handler not working
E 18
//
//
//  Possible enhancements:
//     datahandlers for long varchar
//     does loss-of-database scheme work?
D 18
//     alarm if database down, log to error file
E 18
I 18
//     alarm if database down, log to error file?
E 18
//     rethink GMD strategy and backlog dir strategy?
D 11
//     not sure why can't pass msg objects to process_msg?
E 11
//
//
I 11
D 16
//  ejw, 4-sep-96
E 16
I 16
//  ejw, 14-jan-96
I 33
D 35
//  began upgrade to ss40 6-jan-1998, ejw
E 35
I 35
//  upgrade to ss40 6-jan-1998, ejw
I 41
//  upgrade to mysql 13-jun-2006, ejw
E 41
E 35
E 33
E 16
E 11
I 5
//
E 5
D 11
//  ejw, 28-jun-96
E 11
D 17
//
E 17
D 11
//
E 11


//-------------------------------------------------------------------


I 34
// for posix
E 34
#define _POSIX_SOURCE 1
#define __EXTENSIONS__

I 11

E 11
D 34
// include files, define prototypes, global variables, etc.

// Smartsockets include file
E 34
I 34
// for smartsockets
E 34
#include <rtworks/cxxipc.hxx>

I 17
D 34
// tcl header files
E 34
I 34

// for tcl 
E 34
extern "C"{
#include <tcl.h>
}

E 17
D 34
// misc system headers
E 34
I 34

// misc 
I 42

/* sergey: add following*/
using namespace std;
#include <strstream>
E 42
E 34
#include <fstream.h>
I 42
#include <iomanip.h>

#include <fstream.h>
E 42
#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
D 41
#include <macros.h>
E 41
I 36
#include <sys/types.h>
#include <sys/stat.h>
E 36

I 34

E 34
// dbrouter parameters, globals, prototypes, etc.
#include <dbrouter.h>

I 34

E 34
// ipc prototypes, message types, etc.
D 34
#include <clas_ipc.h>
E 34
D 13
#include <clas_ipc_mt.h>
E 13
#include <clas_ipc_prototypes.h>

I 41
#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
E 41
D 34
/* misc variables */
E 34
I 34

// misc variables 
E 34
D 37
char *application        = "clastest";
D 28
char *uniq_dgrp          = "dbrouter";
E 28
I 28
char *uniq_name          = "dbrouter";
E 28
D 25
char *database           = "clastest";
E 25
I 25
D 34
char *database           = "db3::clastest";
I 28
char *datagroup          = "dbrouter";
E 34
I 34
char *database           = "db5::clastest";
E 34
E 28
E 25
D 29
char *backlog_dir        = "./backlog";
D 28
char *backlog_count_file = "msgcount.dat";
E 28
I 28
char *backlog_count_name = "msgcount.dat";
E 29
I 29
char *backlog_dir_name   = "./backlog_%s";
char *backlog_file       = "msgcount.dat";
E 37
I 37
char *application        = (char*)"clastest";
char *uniq_name          = (char*)"dbrouter";
D 41
char *database           = (char*)"db5::clastest";
E 41
I 41
char *dbhost             = (char*)"clondb1";
char *dbuser             = (char*)"clasrun";
char *database           = (char*)"clasprod";
E 41
char *backlog_dir_name   = (char*)"./backlog_%s";
char *backlog_file       = (char*)"msgcount.dat";
E 37
E 29
D 34
char uniq_dgrp[100];
E 34
I 34
int debug            	 = 0;
int done             	 = 0;
D 35
int destroy_ok           = 0;
E 35
char temp[1000];
E 34
I 29
D 43
char backlog_dir[100];
I 34
char uniq_string[64];
E 43
I 43
char backlog_dir[256];
char uniq_string[256];
E 43
E 34
E 29
E 28

I 34

// counts
E 34
int msg_count        = 0;
int proc_count       = 0;
int err_count        = 0;
int sql_count        = 0;
int sql_trans_count  = 0;
int data_count       = 0;
int insert_count     = 0;
int select_count     = 0;
int update_count     = 0;
int delete_count     = 0;
int other_count      = 0;
int commit_count     = 0;
int rollback_count   = 0;
int row_mod_count    = 0;
int row_return_count = 0;

D 33
int debug_flag       = 0;
E 33
I 33
D 34
int debug            = 0;
E 33
D 18
int quit_flag        = 0;
E 18
I 18
int done             = 0;
E 18
char temp[1000];
E 34

I 19
D 24
char *msqlhost               = "clon00";
char *msqldb                 = "clasmsg";
char *msqlinfotbl            = "clasinfo";
char *msqlerrtbl             = "claserr";
char *msqlstattbl            = "classtat";
char *msqlmhost              = "clon";
char *msqlfacility           = "online";
int msqldaemon               = 2;
E 19
I 17

E 24
D 34
// tcl variables
E 34
I 34
// for tcl 
E 34
char *init_tcl_script    = NULL;
Tcl_Interp *interp;  
I 34
extern "C" {
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}
E 34

E 17
D 34
// ref to server (connection created later)
TipcSrv &server=TipcSrv::Instance();
E 34

// output log and error log files
ofstream logfile;
ofstream errfile;

I 17
D 24
/* tcl stuff */
E 24
I 24
D 34
// tcl stuff
E 24
extern "C" {
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}
E 34

I 24
D 34
// misc prototypes
E 34
I 34
// prototypes
E 34
E 24
void init_tcl();
I 38
extern "C"{
E 38
int tcl_help(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
int tcl_quit(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
I 19
D 38
extern "C"{
E 38
D 24
int eMsgLog(char *, char *, char *, char *, char *, char *,
	char *, char *, int, int);
E 24
I 24
D 26
int insert_msg(char *name, char *facility, char *process, char *msgclass, char *severity, 
                   int code, char *message);
E 26
I 26
D 37
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
	       int severity, char *status, int code, char *text);
E 37
I 37
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *status, int code, const char *text);
E 37
E 26
E 24
}	
I 33
void reset_int4ptr_count();
E 33
E 19


E 17
// debug message log file
TipcMsgFile *debug_msg_file;

D 13
// database message types
TipcMt request_mt     (DBR_REQUEST_MTNAME,     DBR_REQUEST_MTNUM,     "verbose");
TipcMt reply_mt       (DBR_REPLY_MTNAME,       DBR_REPLY_MTNUM,       "verbose");
TipcMt request_mt_rpc (DBR_REQUEST_MTNAME_RPC, DBR_REQUEST_MTNUM_RPC, "verbose");
TipcMt reply_mt_rpc   (DBR_REPLY_MTNAME_RPC,   DBR_REPLY_MTNUM_RPC,"   verbose");
  
E 13

I 34
// ref to server (connection created later)
TipcSrv &server=TipcSrv::Instance();


E 34
//-------------------------------------------------------------------


D 34
main(int argc, char **argv)
{
E 34
I 34
main(int argc, char **argv) {
E 34

  time_t start=time(NULL);
  char filename[132];
I 18
  TipcMsg msg;
I 19
D 24
  int err;
E 24
I 24
  int status;
E 24
E 19
E 18

  
  // decode command line arguments
  decode_command_line(argc,argv);
	
I 34

E 34
I 17
  // get Tcl interp, create tcl commands, link vars, process Tcl startup script, etc.
  interp=tclinterp_init();
  init_tcl();
  if(init_tcl_script!=NULL)Tcl_EvalFile(interp,init_tcl_script);

I 28
D 34
  // create unique datagroup from unique name and application
  sprintf(uniq_dgrp,"%s_%s",uniq_name,application);
E 34

I 34
  // create unique string  from unique name and application
  sprintf(uniq_string,"%s_%s",uniq_name,application);


E 34
E 28
E 17
  // open log file and print startup information
D 34
  sprintf(filename,"%s.log",uniq_dgrp);
E 34
I 34
  sprintf(filename,"%s.log",uniq_string);
E 34
  logfile.open(filename,ios::out|ios::app);
D 28
  logfile << uniq_dgrp << " starting in application: " << application 
E 28
I 28
  logfile << uniq_name << " starting in application: " << application 
E 28
          << "  using database: " << database << "  on " << ctime(&start) << flush;
D 5
	
E 5
I 5
		
I 34

E 34
E 5
  // open error log file
D 34
  sprintf(filename,"%s.err",uniq_dgrp);
E 34
I 34
  sprintf(filename,"%s.err",uniq_string);
E 34
  errfile.open(filename,ios::out|ios::app);
  errfile << endl;
D 28
  errfile << uniq_dgrp << " starting in application: " << application 
E 28
I 28
  errfile << uniq_name << " starting in application: " << application 
E 28
D 5
          << "  using database: " << database << "  on " << ctime(&start) << flush;
E 5
I 5
D 15
          << "  using database: " << database << "  on " <<  ctime(&start) << flush;
E 15
I 15
          << "  using database: " << database << "  on " <<  ctime(&start) << endl;
E 15
E 5

I 34

E 34
  // open message log file if in debug mode
D 33
  if(debug_flag!=0){
E 33
I 33
D 41
  if(debug!=0){
E 41
I 41
  if(debug!=0) {
E 41
E 33
D 34
    sprintf(filename,"%s.debug",uniq_dgrp);
E 34
I 34
    sprintf(filename,"%s.debug",uniq_string);
E 34
    debug_msg_file = new TipcMsgFile(filename,T_IPC_MSG_FILE_CREATE_WRITE);
  }

D 34
  // set application
  ipc_set_application(application);
E 34

D 34
  // disable alarm polling
  ipc_set_alarm_poll_time(0);

  // declare quit handler
  ipc_set_quit_callback(dbrouter_quit_callback);

D 13
  // register message types in DATA category before connecting to server
D 5
  server.LogAddMt(T_IPC_SRV_LOG_DATA,request_mt);
  server.LogAddMt(T_IPC_SRV_LOG_DATA,reply_mt);
  server.LogAddMt(T_IPC_SRV_LOG_DATA,request_mt_rpc);
  server.LogAddMt(T_IPC_SRV_LOG_DATA,reply_mt_rpc);
E 5
I 5
  if ((!server.LogAddMt(T_IPC_SRV_LOG_DATA,request_mt)) ||
      (!server.LogAddMt(T_IPC_SRV_LOG_DATA,reply_mt))  ||
      (!server.LogAddMt(T_IPC_SRV_LOG_DATA,request_mt_rpc))  ||
D 7
      (!server.LogAddMt(T_IPC_SRV_LOG_DATA,reply_mt_rpc))) {
    errfile  << "Unable to register message types"  << endl;
    dbrouter_done();
    exit(EXIT_FAILURE);
  }
E 7
I 7
      (!server.LogAddMt(T_IPC_SRV_LOG_DATA,reply_mt_rpc))) 
    exit (Exit_Error("Unable to register message types"));
E 13
I 13
  // set disconnect mode to "warm" for GMD
E 34
I 34
  // init ipc
  ipc_set_application(application);
  ipc_set_quit_callback(quit_callback);
E 34
  ipc_set_disconnect_mode("warm");
E 13
E 7
E 5
D 34

  // initialize IPC and create connection to server
  ipc_init(uniq_dgrp,"Database Router");
  
I 28
  // subscribe to database message datagroup (default "dbrouter")
  server.DgRecv(datagroup,TRUE);
E 34
I 34
  ipc_set_user_status_poll_callback(status_poll_data);
  ipc_init(uniq_name,"Database Router");
E 34
  server.Flush();

E 28
I 13
D 15
  // create objects from pre-existing message types (created by ipc_init)
E 15
I 15
D 34
  // create mt objects from message types previously created by ipc_init
E 34
I 34

  // create mt objects
E 34
E 15
D 37
  TipcMt request_mt("dbr_request");
  TipcMt request_mt_rpc("dbr_request_rpc");
E 37
I 37
  TipcMt request_mt((char*)"dbr_request");
  TipcMt request_mt_rpc((char*)"dbr_request_rpc");
E 37

I 34

E 34
E 13
  // create callback for database request message types
  if( (server.ProcessCbCreate(request_mt,     receive_database_request, NULL)==NULL) || 
D 5
      (server.ProcessCbCreate(request_mt_rpc, receive_database_request, NULL)==NULL) ){ 
E 5
I 5
D 7
      (server.ProcessCbCreate(request_mt_rpc, receive_database_request, NULL)==NULL) ){
E 5
    errfile << "Unable to create process callback for request messages" << endl;
    dbrouter_done();
    exit(EXIT_FAILURE);
  }
E 7
I 7
      (server.ProcessCbCreate(request_mt_rpc, receive_database_request, NULL)==NULL) )
    exit (Exit_Error("Unable to create process callback for request messages"));
E 7

D 34
  // register function to fill user data for status_poll result messages
  ipc_set_user_status_poll_callback(status_poll_data);
E 34

I 17
  // enable Tcl ipc control by creating tcl_request callback
  tclipc_init(interp);

I 34

E 34
E 17
I 15
D 35
  // create backlog dir if it doesn't exist
E 35
I 35
  // create backlog dir and file if they don't exist
E 35
I 29
  sprintf(backlog_dir,backlog_dir_name,application);
E 29
D 35
  if (chdir(backlog_dir)==0)chdir ("..");
  else mkdir (backlog_dir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP);
E 35
I 35
D 43
  if (chdir(backlog_dir)==0) {
E 43
I 43
  if (chdir(backlog_dir)==0)
  {
E 43
    chdir ("..");
D 43
  } else {
E 43
I 43
  }
  else
  {
E 43
    status = mkdir (backlog_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(status!=0)exit (Exit_Error("Unable to create backlog dir."));
    sprintf(filename,"%s/%s",backlog_dir,backlog_file);
    ofstream msgout(filename,ios::out);
    if(!msgout)exit (Exit_Error("Unable to create backlog msgcount file."));
    msgout << 0;
    msgout.close();
  }
E 35

I 34

E 34
I 19
D 23
  /* post startup message */
E 23
I 23
  // post startup message
E 23
D 28
  sprintf(temp,"Process startup:   %15s  in application:  %s",uniq_dgrp,application);
E 28
I 28
  sprintf(temp,"Process startup:   %15s  in application:  %s",uniq_name,application);
E 28
D 21
  err=eMsgLog(msqlhost,msqldb,msqlstattbl,uniq_dgrp,"START",temp,
E 21
I 21
D 23
  err=eMsgLog(msqlhost,msqldb,msqlstattbl,"dbrouter","START",temp,
E 21
	      msqlmhost,msqlfacility,0,msqldaemon);
E 23
I 23
D 24
  //  err=eMsgLog(msqlhost,msqldb,msqlstattbl,"dbrouter","START",temp,
  //	      msqlmhost,msqlfacility,0,msqldaemon);
E 24
I 24
D 25
  status=insert_msg("dbrouter","dbrouter",uniq_dgrp,"status","START",0,temp);
E 25
I 25
D 26
  //   status=insert_msg("dbrouter","dbrouter",uniq_dgrp,"status","START",0,temp);
E 26
I 26
D 34
  status=insert_msg("dbrouter","dbrouter",uniq_dgrp,"status",0,"START",0,temp);
E 34
I 34
  status=insert_msg("dbrouter","dbrouter",uniq_name,"status",0,"START",0,temp);
E 34
E 26
E 25
E 24
E 23


E 19
D 31
  // process any existing messages in backlog dir
E 31
I 31
  // process prexisting messages in backlog dir
E 31
  process_backlog();

E 15
D 18
  // wait forever for incoming messages
E 18
I 18

D 31
  //  process incoming messages until quit signal or command arrives
E 31
I 31
  //  process incoming messages until quit command or signal arrives
  //  only process messages after incoming queue is empty (i.e. all msg safely stored on disk)
E 31
E 18
D 34
  while(1==1){
E 34
I 34
D 43
  while(1==1) {
E 43
I 43
  while(1==1)
  {
E 43
E 34
D 18
    server.MainLoop(T_TIMEOUT_FOREVER);
    if(quit_flag!=0)break;
E 18
I 18
    msg=server.Next(T_TIMEOUT_FOREVER);
D 34
    if(msg!=NULL){
E 34
I 34
D 43
    if(msg!=NULL) {
E 43
I 43
    if(msg!=NULL)
    {
E 43
E 34
      server.Process(msg);
D 34
      msg.Destroy();
E 34
I 34
D 35
      if(destroy_ok==0){ 
	msg.Destroy(); 
	server.Flush();
      }
E 35
I 35
      msg.Destroy(); 
      server.Flush();
E 35
E 34
    }
D 34
    if(done!=0)break;
E 34
I 31
    if(server.NumQueued()<=0)process_backlog();   
I 34
    if(done!=0)break;
E 34
E 31
E 18
  }

D 18
  // received quit signal
E 18
I 18

D 34
  // received quit command or signal
E 34
I 34
  // done
E 34
E 18
  dbrouter_done();
  exit(EXIT_SUCCESS);
I 18
D 34

E 34
E 18
}


//--------------------------------------------------------------------------------


D 43
void receive_database_request(
E 43
I 43
void
receive_database_request(
E 43
	T_IPC_CONN                 conn,
	T_IPC_CONN_DEFAULT_CB_DATA data,
	T_CB_ARG                   arg)
{
D 43

E 43
  TipcMsg msg(data->msg);
  int num = 0;
  char filename[200];
I 33
  char sender[200];
E 33
  
I 34

E 34
  // count messages received
  msg_count++;

I 34

E 34
  // dump message to log file if in debug mode
D 33
  if(debug_flag!=0)*debug_msg_file << msg;
E 33
I 33
  if(debug!=0)*debug_msg_file << msg;
E 33

I 34

E 34
I 5
D 15
  //Check for back-log directory, create it if needed
  if (chdir (backlog_dir) == 0)  chdir ("..");
D 12
  else mkdir (backlog_dir, -1);
E 12
I 12
D 14
  else mkdir (backlog_dir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP);
E 14
I 14
  else mkdir (backlog_dir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP);
E 14
E 12

E 15
E 5
  // get next sequence number from file, increment, then write back out
D 28
  sprintf(filename,"%s/%s",backlog_dir,backlog_count_file);
E 28
I 28
D 29
  sprintf(filename,"%s/%s_%s",backlog_dir,application,backlog_count_name);
E 29
I 29
  sprintf(filename,"%s/%s",backlog_dir,backlog_file);
E 29
E 28
  ifstream msgin(filename,ios::in);
D 34
  msgin >> num;
  msgin.close();
  num++;
  ofstream msgout(filename,ios::out);
  msgout << num << endl;
  msgout.close();
E 34
I 34
D 39
  if(!msgin.bad()) {
E 39
I 39
  if(msgin.is_open()) {
E 39
    msgin >> num;
    msgin.close();
    num++;
    ofstream msgout(filename,ios::out);
D 39
    if(!msgout.bad()) {
E 39
I 39
    if(msgout.is_open()) {
E 39
      msgout << num << endl;
      msgout.close();
    } else {
D 35
      exit(Exit_Error("Unable to write to msgcount file."));
E 35
I 35
      sprintf(temp,"Unable to write to msgcount file %s",filename);
      exit(Exit_Error(temp));
E 35
    }
  } else {
D 35
    exit(Exit_Error("Unable to read msgcount file."));
E 35
I 35
    sprintf(temp,"Unable to read msgcount file %s",filename);
    exit(Exit_Error(temp));
E 35
  }
E 34

I 34

E 34
  // form full message file name, including seq number, userprop, and sender
D 33
  sprintf(filename,"%s/dbrouter.%06d:%d:%s",backlog_dir,num,msg.UserProp(),msg.Sender());
E 33
I 33
D 34
  // must get rid of "/" in sender
E 34
I 34
  // change / in sender to \, then invert later
E 34
  strcpy(sender,msg.Sender());
D 34
  //   for (int i=0; i<strlen(sender); i++) if(sender[i]=='/')sender[i]='_';
E 34
I 34
  for (int i=0; i<strlen(sender); i++) if(sender[i]=='/')sender[i]='\\';
E 34
  sprintf(filename,"%s/dbrouter.%06d:%d:%s",backlog_dir,num,msg.UserProp(),&(sender[1]));
E 33

I 34

E 34
  // copy message to backlog area in its own file
D 34
  TipcMsgFile *msgfile = new TipcMsgFile(filename,T_IPC_MSG_FILE_CREATE_WRITE);
  *msgfile << msg;
  delete msgfile;
E 34
I 34
  TipcMsgFile msgfile(filename,T_IPC_MSG_FILE_CREATE_WRITE);
  if(!msgfile) {
D 35
    exit(Exit_Error("Unable to open message file."));
E 35
I 35
    sprintf(temp,"Unable to open message file %s",filename);
    exit(Exit_Error(temp));
E 35
  } else {
    msgfile << msg;
  }
E 34

D 34
  // process files in backlog area
D 31
  process_backlog();
E 31
I 31
  //  process_backlog();
E 31

E 34
I 34
  return;
E 34
}


//--------------------------------------------------------------------------------

  
D 34
void process_backlog(void){
E 34
I 34
D 43
void process_backlog(void) {
E 34

E 43
I 43
void
process_backlog(void)
{
E 43
  DIR *dir;
  struct dirent *file;
I 36
  struct stat statbuf;
E 36
  T_IPC_MSG_FILE msgfile;
  T_IPC_MSG msg;
  T_STR ptr;
  T_STR sender;
  T_INT4 userprop;
I 34
  int NumFile = 0;
E 34
D 5
  char filename[200];

E 5
  
D 15
  // open database
  if(sql_connect(database)!=SQL_OK)return;
E 15
I 15

D 34
  // open database...return if unable
  if(sql_connect(database)!=SQL_OK){
    return;
  }
E 15
  
E 34
  // open backlog directory
D 41
  if((dir=opendir(backlog_dir))==NULL){
E 41
I 41
D 43
  if((dir=opendir(backlog_dir))==NULL) {
E 43
I 43
  if((dir=opendir(backlog_dir))==NULL)
  {
E 43
E 41
D 18
    errfile << "Unable to open backlog directory " << backlog_dir << endl;
E 18
D 34
    sql_disconnect();
E 34
D 18
    return;
E 18
I 18
D 35
    exit(Exit_Error("Unable to open backlog directory."));
E 35
I 35
D 38
    sprintf("Unable to open backlog directory %s.",backlog_dir);
E 38
I 38
    sprintf((char*)"Unable to open backlog directory %s.",backlog_dir);
E 38
    exit(Exit_Error(temp));
E 35
E 18
  }
D 5
  
  // process files in backlog directory
  while((file=readdir(dir))!=NULL){
E 5
I 5

I 34

E 34
  // Determine number of files in backlog directory
D 34
  int NumFile = 0;
E 34
D 18
  while((file=readdir(dir))!=NULL)
    if(strncasecmp(file->d_name,"dbrouter.",9)==0)
      NumFile++;
E 18
I 18
D 41
  while((file=readdir(dir))!=NULL){
E 41
I 41
D 43
  while((file=readdir(dir))!=NULL) {
E 41
    if(strncasecmp(file->d_name,"dbrouter.",9)==0)NumFile++;
E 43
I 43
  while((file=readdir(dir))!=NULL)
  {
    if(strncasecmp(file->d_name,"dbrouter.",9)==0) NumFile++;
E 43
  }
E 18

I 34

E 34
D 15
  // Dynamically create corresponding array
E 15
I 15
  // return if no backlog files
D 41
  if(NumFile<=0){
E 41
I 41
D 43
  if(NumFile<=0) {
E 43
I 43
  if(NumFile<=0)
  {
E 43
E 41
D 34
    sql_disconnect();
E 34
    closedir(dir);
    return;
  }

I 34

  // open database...return if unable
D 41
  if(sql_connect(database)!=SQL_OK){
E 41
I 41
D 43
  if(sql_connect(dbhost,dbuser,database)!=SQL_OK) {
E 43
I 43
  if(sql_connect(dbhost,dbuser,database)!=SQL_OK)
  {
E 43
E 41
I 35
    closedir(dir);
E 35
    return;
  }
  

E 34
  // create array to hold backlog file names
E 15
  char **filename = new char *[NumFile];

I 34

E 34
D 15
  // create path name to file
E 15
I 15
D 43
  // fill array with file names
E 43
I 43
  /* fill array with file names */
E 43
E 15
  rewinddir(dir);
  int i = 0;
D 43
  while ((file = readdir(dir))!=NULL)  {
    if (i >= NumFile)  {
E 43
I 43
  while ((file = readdir(dir)) != NULL)
  {
    //printf("[%3d] file >%s<\n",i,file);
    if(i /*>=*/> NumFile) /* sergey: ??? */
    {
      //printf("i=%d, NumFile=%d\n",i,NumFile);
E 43
D 15
      errfile << "A dbrouter file was created during backlog process." << endl;
E 15
I 15
      errfile << "dbrouter file created while in process_backlog...processing interrupted" << endl;
E 15
      sql_disconnect();
I 15
      closedir(dir);
I 40
      delete filename;
E 40
E 15
      return;
    }
E 5
D 41
    if(strncasecmp(file->d_name,"dbrouter.",9)==0){
E 41
I 41
D 43
    if(strncasecmp(file->d_name,"dbrouter.",9)==0) {
E 43
I 43
    if(strncasecmp(file->d_name,"dbrouter.",9)==0)
    {
E 43
E 41
D 5
      
      // create full path name to file
      sprintf(filename,"%s/%s",backlog_dir,file->d_name);
      
      // open file...bug in c++ stuff, must use c api instead
      msgfile=TipcMsgFileCreate(filename,T_IPC_MSG_FILE_CREATE_READ);
      if(msgfile==NULL) {   // garbage in file
	unlink(filename);
E 5
I 5
      filename[i] = strdup(file->d_name);
I 43
	  //printf("file [%3d] from backlog: >%s<\n",i,filename[i]);
E 43
      i++;
    }
  }
I 43
  //printf("start sorting, NumFile=%d, i=%d\n",NumFile,i);
E 43

I 34

E 34
D 15

E 15
D 43
  // Sort backlog files to process in order using Insertion Sort
  for (i = 1; i < NumFile; i++)  {
    if (strcmp (filename[i], filename[i-1]) == -1)  {
E 43
I 43
  /* Sort backlog files to process in order using Insertion Sort */
  for(i = 1; i < NumFile; i++)
  {
    if (strcmp (filename[i], filename[i-1]) == -1)
    {
E 43
      char *v = strdup (filename[i]);
      int j = i;
D 43
      do  {
	filename[j] = filename[j-1];
	j--;
E 43
I 43
      do
      {
	    filename[j] = filename[j-1];
	    j--;
E 43
      } while ((j>0) && (strcmp (filename[j-1], v) == 1));
D 43
      free (filename[j]);      // free up old filename first
E 43
I 43

      free (filename[j]);      /* free up old filename first */
E 43
      filename[j] = strdup (v);
      free (v);
    }
  }
I 43
  //printf("done sorting, NumFile=%d\n",NumFile);
E 43

I 34

E 34
D 15
  // Process each backlog file
E 15
I 15
D 43
  // Process backlog files in sorted order
E 15
D 34
  for (i = 0; i < NumFile; i++)  {
E 34
I 34
  for (i=0; i<NumFile; i++) {
E 34

D 15
    // Create Full path name
E 15
I 15
    // create full path name
E 43
I 43
  /* Process backlog files in sorted order */
  for (i=0; i<NumFile; i++)
  {
    /* create full path name */
E 43
E 15
D 11
    char *Fullfilename = new char [strlen(filename[i])+strlen(backlog_dir) +1];
E 11
I 11
    char *Fullfilename = new char [strlen(filename[i])+strlen(backlog_dir) +10];
E 11
    sprintf(Fullfilename,"%s/%s", backlog_dir, filename[i]); 
I 36

I 43
	//printf("processing file >%s<\n",Fullfilename);
E 43

D 43
    // check that file exists
    if((filename[i]==NULL)||(strlen(filename[i])<8)||(stat(Fullfilename,&statbuf)!=0)) {
E 43
I 43
    /* check that file exists */
    if((filename[i]==NULL)||(strlen(filename[i])<8)||(stat(Fullfilename,&statbuf)!=0))
    {
E 43
      errfile << "Ignoring non-existent file: " << Fullfilename << endl;
      continue;
    }
E 36
 
I 36

E 36
D 43
    // open file...bug in c++ stuff, must use c api instead
E 43
I 43
    /* open file...bug in c++ stuff, must use c api instead */
E 43
    msgfile=TipcMsgFileCreate(Fullfilename,T_IPC_MSG_FILE_CREATE_READ);
D 43
    if(msgfile==NULL) {   // garbage in file
E 43
I 43
    if(msgfile==NULL)   /* garbage in file */
	{
      //printf("garbage in file ..\n");
E 43
      char *dead = new char [strlen(backlog_dir) + 15];
      sprintf (dead,"%s/dead_dbrouter_%d",backlog_dir,i);
      errfile << "Garbage in file...file renamed as " << dead << endl;
      rename(Fullfilename,dead);
      delete dead;
D 33
    }
    else {
E 33
I 33
D 43

    } else {

E 33
      // read message from file and close
      if ((!TipcMsgFileRead(msgfile,&msg)) || (!TipcMsgFileDestroy(msgfile))) {
D 18
	errfile << "Error in reading msg from file." << endl;
	delete Fullfilename;
E 18
	sql_disconnect();
D 18
	closedir(dir);
	for (i = 0; i < NumFile; i ++)
	  free (filename[i]);
	delete filename;
	dbrouter_done();
	exit (EXIT_FAILURE);
E 18
I 18
D 36
	exit(Exit_Error("Error in reading msg from file."));
E 36
I 36
	sprintf(temp,"Error in reading msg file: %s",Fullfilename);
	//	exit(Exit_Error(temp));
E 43
I 43
    }
    else
    {
      //printf("reading message from file ..\n");
      /* read message from file and close */
      if ((!TipcMsgFileRead(msgfile,&msg)) || (!TipcMsgFileDestroy(msgfile)))
      {
	    sql_disconnect();
	    sprintf(temp,"Error in reading msg file: %s",Fullfilename);
		/*exit(Exit_Error(temp));*/
E 43
E 36
E 18
E 5
      }
D 5
      else {
	// read message (not object!) from file and close
	TipcMsgFileRead(msgfile,&msg);
	TipcMsgFileDestroy(msgfile);
E 5

D 5
	// get userprop and sender from encoded filename
	(ptr=strchr(file->d_name,':'))++;
	sscanf(ptr,"%d",&userprop);
	TipcMsgSetUserProp(msg,userprop);
	(sender=strchr(ptr,':'))++;
	TipcMsgSetSender(msg,sender);
E 5
D 18

D 5
	// process message, then delete file if successful, otherwise break out of loop
	// always delete message
	if(process_message(msg)==DBR_OK){
	  TipcMsgDestroy(msg);
	  unlink(filename);
	} else {
	  TipcMsgDestroy(msg);
	  break;
E 5
I 5
      // create msg object from msg and set sender (stored after 1st ':' in file name)
E 18
I 18
D 43
      // create msg object from msg
E 43
I 43
      /* create msg object from msg */
E 43
E 18
      TipcMsg msg_obj(msg);

D 18
      //      msg_obj.Sender((sender=strchr(filename[i],':'))++);

      // get userprop and sender from encoded filename
E 18
I 18
D 34
      // get userprop from encoded filename and set
E 34
I 34
D 43
      // get userprop from encoded filename
E 34
E 18
D 37
      (ptr=strchr(filename[i],':'))++;
E 37
I 37
      //   (ptr=strchr(filename[i],':'))++;
E 43
I 43
      /* get userprop from encoded filename */
	  /*   (ptr=strchr(filename[i],':'))++; */
E 43
      ptr=strchr(filename[i],':');
      ptr++;
E 37
      sscanf(ptr,"%d",&userprop);
D 18

E 18
D 43
      if (!TipcMsgSetUserProp(msg_obj,userprop)) {
D 18
	errfile << "User prop set failed on msg obj." << endl;
	delete Fullfilename;
E 18
	sql_disconnect();
D 18
	closedir(dir);
	for (i = 0; i < NumFile; i ++)
	  free (filename[i]);
	delete filename;
	dbrouter_done();
	exit (EXIT_FAILURE);
E 18
I 18
	exit(Exit_Error("User prop set failed on msg obj."));
E 43
I 43
      if (!TipcMsgSetUserProp(msg_obj,userprop))
      {
	    sql_disconnect();
	    exit(Exit_Error("User prop set failed on msg obj."));
E 43
E 18
      }
I 18

D 34
      // get sender from encoded filename and set
E 34
I 34
D 43
      // get sender from encoded filename
E 34
E 18
D 37
      (sender=strchr(ptr,':'))++;
E 37
I 37
      //  (sender=strchr(ptr,':'))++;
E 43
I 43
      /* get sender from encoded filename */
      /*  (sender=strchr(ptr,':'))++; */
E 43
      sender=strchr(ptr,':');
      sender++;
E 37
I 34
      for (int i=0; i<strlen(sender); i++) if(sender[i]=='\\')sender[i]='/';
E 34
D 43
      if (!TipcMsgSetSender(msg_obj,sender)) {
D 18
	errfile << "Sender set failed on msg obj."  << endl;
	delete Fullfilename;
E 18
	sql_disconnect();
D 18
	closedir(dir);
	for (i = 0; i < NumFile; i ++)
	  free (filename[i]);
	delete filename;
	dbrouter_done();
	exit (EXIT_FAILURE);
E 18
I 18
	exit(Exit_Error("Sender set failed on msg obj."));
E 43
I 43
      if (!TipcMsgSetSender(msg_obj,sender))
      {
	    sql_disconnect();
	    exit(Exit_Error("Sender set failed on msg obj."));
E 43
E 18
      }
     
D 43
      // process obj, then delete file if successful, otherwise break out of loop..always delete message
D 41
      if(process_message(msg_obj)==DBR_OK){
E 41
I 41
      if(process_message(msg_obj)==DBR_OK) {
E 41
	if (!TipcMsgDestroy(msg)) {
D 18
	  errfile << "Error in Destroying msg."  << endl;
	  delete Fullfilename;
E 18
	  sql_disconnect();
I 33
	  reset_int4ptr_count();  // kludge...
E 33
D 18
	  closedir(dir);
	  for (i = 0; i < NumFile; i ++)
	    free (filename[i]);
	  delete filename;
	  dbrouter_done();
	  exit (EXIT_FAILURE);
E 18
I 18
	  exit(Exit_Error("Error in Destroying msg."));
E 18
E 5
	}
I 5
	unlink(Fullfilename);
E 43
I 43
      /* process obj, then delete file if successful, otherwise break out of loop..always delete message */
      if(process_message(msg_obj)==DBR_OK)
      {
	    if (!TipcMsgDestroy(msg))
        {
	      sql_disconnect();
	      reset_int4ptr_count();  /* kludge... */
	      exit(Exit_Error("Error in Destroying msg."));
	    }
	    unlink(Fullfilename);
E 43
      } 
D 43
      else {
	if (!TipcMsgDestroy(msg)) {
D 18
	  errfile << "Error in Destroying msg."  << endl;
	  delete Fullfilename;
E 18
	  sql_disconnect();
I 33
	  reset_int4ptr_count();  // kludge...
E 33
D 18
	  closedir(dir);
	  for (i = 0; i < NumFile; i ++)
	    free (filename[i]);
	  delete filename;
	  dbrouter_done();
	  exit (EXIT_FAILURE);
E 18
I 18
	  exit(Exit_Error("Error in Destroying msg."));
E 18
	}
	break;
E 43
I 43
      else
      {
	    if (!TipcMsgDestroy(msg))
        {
	      sql_disconnect();
	      reset_int4ptr_count();  /* kludge... */
	      exit(Exit_Error("Error in Destroying msg."));
	    }
	    break;
E 43
E 5
      }

I 33
D 43
      reset_int4ptr_count();  // kludge
E 43
I 43
      reset_int4ptr_count();  /* kludge */
E 43
E 33
    }
I 5
    delete Fullfilename;
E 5
  }

I 34

E 34
D 35
  // disconnect from database
  sql_disconnect();

I 34

E 35
E 34
D 43
  // close backlog dir
E 43
I 43
  /* close backlog dir */
E 43
  closedir(dir);

I 34

I 35
D 43
  // disconnect from database
E 43
I 43
  /* disconnect from database */
E 43
  sql_disconnect();


E 35
E 34
D 5
}
E 5
I 5
D 43
  // clear allocated array memory
E 43
I 43
  /* clear allocated array memory */
E 43
D 15
  for (i = 0; i < NumFile; i ++)
    free (filename[i]);
E 15
I 15
  for (i = 0; i < NumFile; i ++)  free (filename[i]);
E 15
  delete filename;
E 5

I 35

  return;
E 35
D 34

E 34
D 5
//-------------------------------------------------------------------
E 5
I 5
}
E 5

I 15

E 15
D 5

E 5
I 5
//-------------------------------------------------------------------
E 5
D 15
dbr_code process_message(T_IPC_MSG msg)
{
E 15

I 5

I 15
D 41
dbr_code process_message(T_IPC_MSG msg){
E 41
I 41
dbr_code process_message(T_IPC_MSG msg) {
E 41

E 15
E 5
  T_STR sqlstring;
  T_INT4 id,maxrow,userprop,nfield,msgmtnum;
  T_STR sender;
  T_IPC_MT msgtype;
  T_IPC_FT fieldtype;
  int i;
  sql_code status;
  time_t now;


I 5
D 15

E 15
E 5
  // count processed messages
  proc_count++;

I 5
D 15

E 15
E 5
  // get incoming transaction id
D 5
  TipcMsgGetUserProp(msg,&userprop);
E 5
I 5
D 7
  if (!TipcMsgGetUserProp(msg,&userprop)) {
    errfile << "Can't get transaction id." << endl;
    dbrouter_done();
    exit (EXIT_FAILURE);
  }
E 7
I 7
D 18
  if (!TipcMsgGetUserProp(msg,&userprop))
    exit (Exit_Error("Can't get transaction id."));
E 18
I 18
  if (!TipcMsgGetUserProp(msg,&userprop))exit(Exit_Error("GetUserProp failure."));
E 18
E 7
E 5
  id=userprop&0xFF;

  // get incoming msg type, number, and sender
D 5
  TipcMsgGetType(msg,&msgtype);
  TipcMtGetNum(msgtype,&msgmtnum);
  TipcMsgGetSender(msg,&sender);
E 5
I 5
D 7
  if (!TipcMsgGetType(msg,&msgtype)) {
    errfile << "Can't get msg type." << endl;
    dbrouter_done();
    exit (EXIT_FAILURE);
  }
  if (!TipcMtGetNum(msgtype,&msgmtnum)) {
    errfile << "Can't get msg number."  << endl;
    dbrouter_done();
    exit (EXIT_FAILURE);
  }
  if (!TipcMsgGetSender(msg,&sender)) {
    errfile << "Can't get msg sender."  << endl;
    dbrouter_done();
    exit (EXIT_FAILURE);
  }
E 7
I 7
D 15
  if (!TipcMsgGetType(msg,&msgtype))
    exit (Exit_Error("Can't get msg type."));
E 15
I 15
  if (!TipcMsgGetType(msg,&msgtype))    exit(Exit_Error("Can't get msg type."));
  if (!TipcMtGetNum(msgtype,&msgmtnum)) exit(Exit_Error("Can't get msg number."));
  if (!TipcMsgGetSender(msg,&sender))   exit(Exit_Error("Can't get msg sender."));
E 15
E 7
E 5

I 7
D 15
  if (!TipcMtGetNum(msgtype,&msgmtnum))
    exit (Exit_Error("Can't get msg number."));

  if (!TipcMsgGetSender(msg,&sender))
    exit (Exit_Error("Can't get msg sender."));

E 15
E 7
  // create reply message...reply message type is 1 more than request type
  TipcMt mtrep(msgmtnum+1);
  TipcMsg reply(mtrep);
D 5
  reply.Sender(uniq_dgrp);
  reply.Dest(sender);
E 5
I 5
D 7
  if ((!reply.Sender(uniq_dgrp)) || (!reply.Dest(sender))) {
    errfile << "Unsuccessful in creating reply message." << endl;
    dbrouter_done();
    exit (EXIT_FAILURE);
  }
E 7
I 7
D 34
  if ((!reply.Sender(uniq_dgrp)) || (!reply.Dest(sender)))
E 34
I 34
  if ((!reply.Sender(uniq_name)) || (!reply.Dest(sender)))
E 34
    exit (Exit_Error("Unsuccessful in creating reply message."));
E 7
E 5

D 5

E 5
  // check message format and process
D 5
  TipcMsgNextType(msg,&fieldtype);
E 5
I 5
D 7
  if (!TipcMsgNextType(msg,&fieldtype)) {
    errfile << "Msg format type determination failure." << endl;
    dbrouter_done();
    exit (EXIT_FAILURE);
  }
E 7
I 7
  if (!TipcMsgNextType(msg,&fieldtype)) 
D 18
    exit (Exit_Error("Msg format type determination failure."));
E 18
I 18
    exit (Exit_Error("Msg field type determination failure."));
E 18
E 7
E 5

  // first field int4 means ascii format
D 41
  if(fieldtype==T_IPC_FT_INT4){
E 41
I 41
  if(fieldtype==T_IPC_FT_INT4) {
E 41
D 5
    
E 5
    sql_count++;
D 5
    
E 5
I 5

E 5
    // extract maxrow and SQL statement, count type, execute, check status, repeat
    // stop and rollback on any error
D 5
    TipcMsgGetNumFields(msg,&nfield);
E 5
I 5
D 7
    if (!TipcMsgGetNumFields(msg,&nfield)) {
      errfile << "Number of field determination failure...rollback failed." << endl;
      dbrouter_done();
      exit (EXIT_FAILURE);
    }
E 7
I 7
    if (!TipcMsgGetNumFields(msg,&nfield)) 
D 18
      exit (Exit_Error("Number of field determination failure...rollback failed."));
E 18
I 18
      exit (Exit_Error("GetNumFields failure."));
E 18

I 30
    // ignore if can't decode message
E 30
E 7
E 5
D 41
    for (i=1; i<=nfield; i+=2){
E 41
I 41
    for (i=1; i<=nfield; i+=2) {
E 41
I 30
      status=SQL_BAD_FORMAT;
E 30
      sql_trans_count++;
D 5
      TipcMsgNextInt4(msg,&maxrow);
      TipcMsgNextStr(msg,&sqlstring);
E 5
I 5
D 7
      if ((!TipcMsgNextInt4(msg,&maxrow)) || (!TipcMsgNextStr(msg,&sqlstring))) {
	errfile << "Msg retrievial failure...rollback failed." << endl;
	dbrouter_done();
	exit (EXIT_FAILURE);
      }
E 7
I 7
D 30
      if ((!TipcMsgNextInt4(msg,&maxrow)) || (!TipcMsgNextStr(msg,&sqlstring)))
D 18
	exit (Exit_Error("Msg retrievial failure...rollback failed."));
E 18
I 18
	exit (Exit_Error("MsgNext failure."));
E 30
I 30
      if ((!TipcMsgNextInt4(msg,&maxrow)) || (!TipcMsgNextStr(msg,&sqlstring))) continue;
E 30
E 18
E 7
E 5
D 41
      maxrow=min(maxrow,DBR_MAX_ROWS_TO_RETURN);
E 41
I 41
      maxrow=MIN(maxrow,DBR_MAX_ROWS_TO_RETURN);
E 41
      count_sqltype(sqlstring);
      status=sql_process_sql(int(maxrow),sqlstring,reply.Message());
      if(status!=SQL_COMMIT)break;
    }
    if((i>3)&&(status!=SQL_COMMIT))status=SQL_ROLLBACK;  // just in case...
    check_status(status,msg);
  }

I 41
  //  should no longer be used...ejw, 14-jun-2006
E 41
  // first field char string means packed database entry, insert only
D 41
  else if(fieldtype==T_IPC_FT_STR){
E 41
I 41
  else if(fieldtype==T_IPC_FT_STR) {
E 41
    data_count++;
    insert_count++;
D 5
//    status=sql_process_data(msg,reply.Message());
E 5
I 5
    status=sql_process_data(msg,reply.Message());
E 5
    check_status(status,msg);
  }
D 5
    
E 5
I 5

E 5
  // unknown format
  else {
    status=SQL_BAD_FORMAT;
    check_status(status,msg);
    reply << T_INT4(SQL_BAD_FORMAT) << T_INT4(0);
I 5

E 5
  }

D 5

E 5
  // dump reply into debug file if requested
D 5
  if(debug_flag!=0)reply.Print(T_OUT_FUNC(msgerrprint));
E 5
I 5
D 9
  if(debug_flag!=0)
D 7
    if (!reply.Print(T_OUT_FUNC(msgerrprint))) {
      errfile << "Reply transfer to debug file failed." << endl;
      dbrouter_done();
      exit (EXIT_FAILURE);
    }
E 7
I 7
    if (!reply.Print(T_OUT_FUNC(msgerrprint)))
      exit (Exit_Error("Reply transfer to debug file failed."));
E 9
I 9
D 33
  if(debug_flag!=0)*debug_msg_file << reply;
E 33
I 33
  if(debug!=0)*debug_msg_file << reply;
E 33
E 9
E 7
E 5

D 5

E 5
  // transaction fails and no reply sent if connection to database was lost
  // otherwise pack user property, send msg, and flush
D 41
  if(sql_connect_check()==SQL_OK){
E 41
I 41
  if(sql_connect_check()==SQL_OK) {
E 41
D 5
    reply.UserProp(T_INT4(((status&0xFF)<<8)|id));
    server.Send(reply);
    server.Flush();
E 5
I 5
D 7
    if (!reply.UserProp(T_INT4(((status&0xFF)<<8)|id))) {
      errfile << "Error in packing userprop to reply msg." << endl;
      dbrouter_done();
      exit (EXIT_FAILURE);
    }
    if ((!server.Send(reply)) || (!server.Flush())) {
      errfile << "Reply msg sent to server failed." << endl;
      dbrouter_done();
      exit (EXIT_FAILURE);
    }
E 7
I 7
    if (!reply.UserProp(T_INT4(((status&0xFF)<<8)|id)))
      exit (Exit_Error("Error in packing userprop to reply msg."));

    if ((!server.Send(reply)) || (!server.Flush()))
D 18
      exit (Exit_Error("Reply msg sent to server failed."));
E 18
I 18
      exit (Exit_Error("Msg send failure."));
E 18

E 7
E 5
    return(DBR_OK);
I 15

E 15
  } else {
    now=time(NULL);
    errfile << "Discovered connection to database lost on: " << ctime(&now) << flush;
    return(DBR_FAIL);
  }

  // reply mt and msg objects destroyed by going out of scope
}


//-------------------------------------------------------------------


D 41
void check_status(int status, T_IPC_MSG msg){
E 41
I 41
void check_status(int status, T_IPC_MSG msg) {
E 41

  switch(status) {
  case SQL_COMMIT:    // transaction committed
    commit_count++;
    sql_commit();
    break;

  case SQL_ROLLBACK:  // rollback performed
    rollback_count++;
    sql_rollback();
    errfile << "\nRollback performed on msg: " << endl;
D 5
    TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint));
E 5
I 5
D 7
    if (!TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint))) {
      errfile << "Error in Printing message." << endl;
      dbrouter_done();
      exit (EXIT_FAILURE);
    }
E 7
I 7
    if (!TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint))) 
      exit (Exit_Error("Error in Printing message."));
E 7
E 5
    break;

  case SQL_BAD_FORMAT: // illegal format
    err_count++; 
    errfile << "\nReceived bad message format:" << endl;
D 5
    TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint));
E 5
I 5
D 7
    if (!TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint))) {
      errfile << "Error in Printing message." << endl;
      dbrouter_done();
      exit (EXIT_FAILURE);
    }
E 7
I 7
    if (!TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint)))
      exit (Exit_Error("Error in Printing message."));
E 7
E 5
    break;

D 15
  case SQL_ERROR:  // general error
E 15
I 15
  case SQL_ERROR:  // general SQL error
E 15
    err_count++;
D 15
    errfile << "\nError found in msg: " << endl;
E 15
I 15
    errfile << "Error found in msg: " << endl;
E 15
D 5
    TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint));
E 5
I 5
D 7
    if (!TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint))) {
      errfile << "Error in Printing message." << endl;
      dbrouter_done();
      exit (EXIT_FAILURE);
    }
E 7
I 7
    if (!TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint)))
      exit (Exit_Error("Error in Printing message."));
I 15
    errfile << endl << endl << endl;
E 15
E 7
E 5
    break;

  case SQL_OK:  // shouldn't happen
    errfile << "\nIllegal status SQL_OK received by check_status" << endl;
    break;

  default: // unknown status
    errfile << "\nUnknown status return from sql_process_xxx: " << status << endl;
D 5
    TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint));
E 5
I 5
D 7
    if (!TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint))) {
      errfile << "Error in Printing message." << endl;
      dbrouter_done();
      exit (EXIT_FAILURE);
    }
E 7
I 7
    if (!TipcMsgPrint(msg,T_OUT_FUNC(msgerrprint)))
      exit (Exit_Error("Error in Printing message."));
E 7
E 5
    break;
  }
}


//-------------------------------------------------------------------


D 34
void dbrouter_quit_callback(int sig){
E 34
I 34
D 41
void quit_callback(int sig){
E 41
I 41
void quit_callback(int sig) {
E 41
E 34

I 18
  done=1;
E 18
  errfile << "\nQuit callback received sig " << sig << endl;

D 18
  // received QUIT control command...in between processing database messages
E 18
I 18
  // QUIT control command
E 18
D 41
  if(sig<=0){
E 41
I 41
  if(sig<=0) {
E 41
D 18
     errfile  << "   ...received QUIT control command...stopping immediately..." << endl;
     dbrouter_done();
     exit(EXIT_SUCCESS);
E 18
I 18
     errfile  << "   ...received QUIT control command...stopping..." << endl;
E 18

D 18
   // SIGHUP or SIGTERM...stop after processing backlog
E 18
I 18
   // SIGHUP or SIGTERM
E 18
D 41
   } else if((sig==SIGHUP)||(sig==SIGTERM)){
E 41
I 41
   } else if((sig==SIGHUP)||(sig==SIGTERM)) {
E 41
D 18
     quit_flag=1;
     errfile  << "   ...received SIGHUP or SIGTERM...will stop after processing backlog...\n" << endl;
    return;
E 18
I 18
     errfile  << "   ...received SIGHUP or SIGTERM...stopping...\n" << endl;
E 18

D 18
  // stop immediately on SIGQUIT or SIGINT
E 18
I 18
  // SIGQUIT or SIGINT
E 18
  } else {
D 18
    errfile << "    ...received SIGINT or SIGQUIT...stopping immediately..." << endl;
    dbrouter_done();
    exit(EXIT_SUCCESS);
E 18
I 18
    errfile << "    ...received SIGINT or SIGQUIT...stopping..." << endl;
E 18
  }
I 18

  return;
E 18
}


//-------------------------------------------------------------------


D 41
void dbrouter_done(void){
E 41
I 41
void dbrouter_done(void) {
E 41

  time_t endtime=time(NULL);
I 19
D 24
  int err;
E 24
I 24
  int status;
E 24
E 19

I 19

E 19
  // print shutdown message and close log file
D 28
  logfile << uniq_dgrp << " stopping on " << ctime(&endtime);
E 28
I 28
  logfile << uniq_name << " stopping on " << ctime(&endtime);
E 28
  logfile <<   "  Processed " << msg_count        << " total database messages:\n";
  logfile << "              " << sql_count        << " sql type\n";
  logfile << "              " << data_count       << " packed data\n";
  logfile << "              " << err_count        << " errors\n";
  logfile << "              " << sql_trans_count  << " sql transactions in sql-type messages\n";
  logfile << "              " << insert_count 	  << " inserts\n";
  logfile << "              " << select_count 	  << " selects\n";
  logfile << "              " << update_count 	  << " updates\n";
  logfile << "              " << delete_count 	  << " deletes\n";
  logfile << "              " << other_count  	  << " other transactions\n";
  logfile << "              " << commit_count     << " commits\n";
  logfile << "              " << rollback_count   << " rollbacks\n";
  logfile << "              " << row_mod_count    << " total rows modified \n";
  logfile << "              " << row_return_count << " total rows returned \n\n";
  logfile << endl << endl;
  logfile.close();


  // print shutdown message and close error log file
  errfile << endl;
D 28
  errfile << uniq_dgrp << " stopping on " << ctime(&endtime);
E 28
I 28
  errfile << uniq_name << " stopping on " << ctime(&endtime);
E 28
  errfile << "   Processed " << msg_count << " database messages, " 
          << commit_count   << " commits, "
          << rollback_count << " rollbacks, "
          << err_count << " errors\n\n\n" << flush;
  errfile.close();


  // close debug message log file if open
D 33
  if(debug_flag!=0)delete debug_msg_file;
E 33
I 33
  if(debug!=0)delete debug_msg_file;
E 33

  // close ipc connection
  ipc_close();
I 19

D 23
  /* post shutdown message */
E 23
I 23
  // post shutdown message
E 23
D 20
  sprintf(temp,"Process shutdown:  %s",uniq_dgrp);
E 20
I 20
D 28
  sprintf(temp,"Process shutdown:  %15s",uniq_dgrp);
E 28
I 28
  sprintf(temp,"Process shutdown:  %15s  in application %s",uniq_name, application);
E 28
E 20
D 21
  err=eMsgLog(msqlhost,msqldb,msqlstattbl,uniq_dgrp,"STOP",temp,
E 21
I 21
D 23
  err=eMsgLog(msqlhost,msqldb,msqlstattbl,"dbrouter","STOP",temp,
E 21
	      msqlmhost,msqlfacility,0,msqldaemon);
E 23
I 23
D 24
  // err=eMsgLog(msqlhost,msqldb,msqlstattbl,"dbrouter","STOP",temp,
  //	      msqlmhost,msqlfacility,0,msqldaemon);
E 24
I 24
D 25
  status=insert_msg("dbrouter","dbrouter",uniq_dgrp,"status","STOP",0,temp);
E 25
I 25
D 26
  //   status=insert_msg("dbrouter","dbrouter",uniq_dgrp,"status","STOP",0,temp);
E 26
I 26
D 34
  status=insert_msg("dbrouter","dbrouter",uniq_dgrp,"status",0,"STOP",0,temp);
E 34
I 34
  status=insert_msg("dbrouter","dbrouter",uniq_name,"status",0,"STOP",0,temp);
E 34
E 26
E 25
E 24
E 23
E 19
}


//-------------------------------------------------------------------


D 41
void status_poll_data(T_IPC_MSG msg){
E 41
I 41
void status_poll_data(T_IPC_MSG msg) {
E 41

I 4
D 37
  TipcMsgAppendStr(msg,"Database");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Database");
E 37
  TipcMsgAppendStr(msg,database);

I 28
D 34
  TipcMsgAppendStr(msg,"Datagroup");
  TipcMsgAppendStr(msg,datagroup);
E 34
I 34
D 37
  TipcMsgAppendStr(msg,"Unique name");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Unique name");
E 37
  TipcMsgAppendStr(msg,uniq_name);
E 34

E 28
I 17
D 41
  if(init_tcl_script!=NULL){
E 41
I 41
  if(init_tcl_script!=NULL) {
E 41
D 37
    TipcMsgAppendStr(msg,"Init tcl script");
E 37
I 37
    TipcMsgAppendStr(msg,(char*)"Init tcl script");
E 37
    TipcMsgAppendStr(msg,init_tcl_script);
  }

E 17
  /* this makes a blank line */
D 37
  TipcMsgAppendStr(msg,"");
  TipcMsgAppendStr(msg,"");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");
E 37
 
E 4
D 37
  TipcMsgAppendStr(msg,"Number of messages received");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of messages received");
E 37
  TipcMsgAppendInt4(msg,msg_count);
D 37
  TipcMsgAppendStr(msg,"Number of sql messages");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of sql messages");
E 37
  TipcMsgAppendInt4(msg,sql_count);
D 37
  TipcMsgAppendStr(msg,"Number of data messages");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of data messages");
E 37
  TipcMsgAppendInt4(msg,data_count);
D 37
  TipcMsgAppendStr(msg,"Number of errors");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of errors");
E 37
  TipcMsgAppendInt4(msg,err_count);

D 17
  /* this makes a blank line */
E 17
D 37
  TipcMsgAppendStr(msg,"");
  TipcMsgAppendStr(msg,"");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");
E 37

D 37
  TipcMsgAppendStr(msg,"Number of sql trans in sql-type messages");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of sql trans in sql-type messages");
E 37
D 10
  TipcMsgAppendInt4(msg,insert_count);
E 10
I 10
  TipcMsgAppendInt4(msg,sql_trans_count);
E 10

D 37
  TipcMsgAppendStr(msg,"");
  TipcMsgAppendStr(msg,"");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");
E 37

D 37
  TipcMsgAppendStr(msg,"Number of inserts");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of inserts");
E 37
  TipcMsgAppendInt4(msg,insert_count);
D 37
  TipcMsgAppendStr(msg,"Number of selects");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of selects");
E 37
  TipcMsgAppendInt4(msg,select_count);
D 37
  TipcMsgAppendStr(msg,"Number of updates");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of updates");
E 37
  TipcMsgAppendInt4(msg,update_count);
D 37
  TipcMsgAppendStr(msg,"Number of deletes");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of deletes");
E 37
  TipcMsgAppendInt4(msg,delete_count);
D 37
  TipcMsgAppendStr(msg,"Number of others");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of others");
E 37
  TipcMsgAppendInt4(msg,other_count);
D 37
  TipcMsgAppendStr(msg,"Number of rows modified");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of rows modified");
E 37
  TipcMsgAppendInt4(msg,row_mod_count);
D 37
  TipcMsgAppendStr(msg,"Number of rows returned");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of rows returned");
E 37
  TipcMsgAppendInt4(msg,row_return_count);

D 37
  TipcMsgAppendStr(msg,"");
  TipcMsgAppendStr(msg,"");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");
E 37

D 37
  TipcMsgAppendStr(msg,"Number of commits");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of commits");
E 37
  TipcMsgAppendInt4(msg,commit_count);
D 37
  TipcMsgAppendStr(msg,"Number of rollbacks");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"Number of rollbacks");
E 37
  TipcMsgAppendInt4(msg,rollback_count);

I 19
D 37
  TipcMsgAppendStr(msg,"");
  TipcMsgAppendStr(msg,"");
E 37
I 37
  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");
E 37

D 24
  TipcMsgAppendStr(msg,"Msql host");
  TipcMsgAppendStr(msg,msqlhost);
  TipcMsgAppendStr(msg,"Msql daemon number");
  TipcMsgAppendInt4(msg,msqldaemon);
  TipcMsgAppendStr(msg,"Msql database");
  TipcMsgAppendStr(msg,msqldb);
  TipcMsgAppendStr(msg,"Msql info table");
  TipcMsgAppendStr(msg,msqlinfotbl);
  TipcMsgAppendStr(msg,"Msql error table");
  TipcMsgAppendStr(msg,msqlerrtbl);
  TipcMsgAppendStr(msg,"Msql status table");
  TipcMsgAppendStr(msg,msqlstattbl);
  TipcMsgAppendStr(msg,"Msql machine host");
  TipcMsgAppendStr(msg,msqlmhost);

E 24
E 19
}


//-------------------------------------------------------------------


void decode_command_line(int argc, char **argv)
{

  int i=1;
I 5
D 15
  char *help = "\nusage:\n\n   dbrouter [-a application] [-u uniq_dgrp] [-d database ] [-l logfile] [-b backlog_dir] [-e errorfile]\n\n\n";
E 15
I 15
D 19
  char *help = "\nusage:\n\n   dbrouter [-a application] [-u uniq_dgrp] [-d database ]"
D 17
               " [-l logfile] [-b backlog_dir] [-e errorfile]\n\n\n";
E 17
I 17
               " [-l logfile] [-b backlog_dir] [-e errorfile] [-t tcl_script]\n\n\n";
E 19
I 19
D 28
  char *help = "\nusage:\n\n   dbrouter [-a application] [-u uniq_dgrp] [-d database]\n"
D 24
               " [-l logfile] [-b backlog_dir] [-e errorfile] [-t tcl_script]\n"
D 22
               " [-mhost msqlhost] [-mdaemone msqldaemon] [-mdb msqldb]\n"
E 22
I 22
               " [-mhost msqlhost] [-mdaemon msqldaemon] [-mdb msqldb]\n"
E 22
               " [-minfo msqinfotbl] [-merr msqlerrtbl] [-mstat msqlstattbl]\n\n\n";
E 24
I 24
D 27
               " [-l logfile] [-b backlog_dir] [-e errorfile] [-t tcl_script]\n\n";
E 27
I 27
               " [-b backlog_dir] [-t tcl_script]\n\n";
E 28
I 28
D 38
  char *help = "\nusage:\n\n   dbrouter [-a application] [-u uniq_name] [-d database]\n"
D 29
               "          [-dgrp datagroup] [-b backlog_dir] [-t tcl_script]\n\n";
E 29
I 29
D 33
               "          [-dgrp datagroup] [-b backlog_dir_name] [-t tcl_script]\n\n";
E 33
I 33
D 34
               "          [-dgrp datagroup] [-b backlog_dir_name] [-t tcl_script] [-debug]\n\n";
E 34
I 34
               "          [-b backlog_dir_name] [-t tcl_script] [-debug]\n\n";
E 38
I 38
D 41
  const char *help = "\nusage:\n\n   dbrouter [-a application] [-u uniq_name] [-d database]\n"
    "          [-b backlog_dir_name] [-t tcl_script] [-debug]\n\n";
E 41
I 41
  const char *help = "\nusage:\n\n   dbrouter [-a application] [-u uniq_name] [-host dbhost] [-user dbuser] [-d database]\n"
    "            [-b backlog_dir_name] [-t tcl_script] [-debug]\n\n";
E 41
E 38
E 34
E 33
E 29
E 28
E 27
E 24
E 19
E 17


E 15
E 5
  while(i<argc) {
D 5

E 5
I 5
    
E 5
D 41
    if(strncasecmp(argv[i],"-h",2)==0){
D 5
      cout << "\nusage:\n\n   dbrouter [-a application] [-u uniq_dgrp] [-d database ] [-l logfile] [-b backlog_dir] [-e errorfile]\n\n\n";
E 5
I 5
      cout << help;
E 5
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-deb",4)==0){
E 41
I 41
    if (strncasecmp(argv[i],"-debug",6)==0) {
E 41
D 33
      debug_flag=1;
E 33
I 33
      debug=1;
E 33
      i=i+1;
    }
I 19
D 24
    else if (strcasecmp(argv[i],"-mhost")==0){
      msqlhost=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strcasecmp(argv[i],"-mdaemon")==0){
      sscanf(argv[i+1],"%d",&msqldaemon);
      i=i+2;
    }
    else if (strcasecmp(argv[i],"-mdb")==0){
      msqldb=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strcasecmp(argv[i],"-minfo")==0){
      msqlinfotbl=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strcasecmp(argv[i],"-merr")==0){
      msqlerrtbl=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strcasecmp(argv[i],"-mstat")==0){
      msqlstattbl=strdup(argv[i+1]);
      i=i+2;
    }
E 24
E 19
D 5
    else if (strncasecmp(argv[i],"-a",2)==0){
      application=strdup(argv[i+1]);
      i=i+2;
E 5
I 5
D 41
    else if (argc!=(i+1)) { 
D 28
      if (strncasecmp(argv[i],"-a",2)==0){
E 28
I 28
D 34
      if (strncasecmp(argv[i],"-dgrp",5)==0){
	datagroup=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-a",2)==0){
E 34
I 34
      if (strncasecmp(argv[i],"-a",2)==0){
E 34
E 28
	application=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-u",2)==0){
D 28
	uniq_dgrp=strdup(argv[i+1]);
E 28
I 28
	uniq_name=strdup(argv[i+1]);
E 28
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-b",2)==0){
D 29
	backlog_dir=strdup(argv[i+1]);
E 29
I 29
	backlog_dir_name=strdup(argv[i+1]);
E 29
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-d",2)==0){
	database=strdup(argv[i+1]);
	i=i+2;
      }
I 17
      else if (strncasecmp(argv[i],"-t",2)==0){
	init_tcl_script=strdup(argv[i+1]);
	i=i+2;
      }
E 17
      else {
	cerr << "Unknown command line arg: " << argv[i] << endl;
	i=i+1;
      }
E 41
I 41
    else if (strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;
E 41
E 5
    }
I 41
    else if (strncasecmp(argv[i],"-host",5)==0) {
      dbhost=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-user",5)==0) {
      dbuser=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0) {
      uniq_name=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-b",2)==0) {
      backlog_dir_name=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-d",2)==0) {
      database=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-t",2)==0) {
      init_tcl_script=strdup(argv[i+1]);
      i=i+2;
    }
E 41
D 5
    else if (strncasecmp(argv[i],"-u",2)==0){
      uniq_dgrp=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-b",2)==0){
      backlog_dir=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-d",2)==0){
      database=strdup(argv[i+1]);
      i=i+2;
    }
E 5
    else {
D 5
      cerr << "Unknown command line arg: " << argv[i] << endl;
      i=i+1;
E 5
I 5
      cout << help;
      exit(EXIT_SUCCESS);
E 5
    }
  }
}

//-------------------------------------------------------------------


// counts type of sql transaction in sql string
D 41
void count_sqltype(T_STR sqlstring){
E 41
I 41
void count_sqltype(T_STR sqlstring) {
E 41

  char *ptr;

  // get pointer to first token
  ptr=&sqlstring[strspn(sqlstring," ")];

  // check token and count transaction types
D 41
  if(strncasecmp(ptr,"insert",6)==0){
E 41
I 41
  if(strncasecmp(ptr,"insert",6)==0) {
E 41
    insert_count++;
  }
D 41
  else if(strncasecmp(ptr,"select",6)==0){
E 41
I 41
  else if(strncasecmp(ptr,"select",6)==0) {
E 41
    select_count++;
  }
D 41
  else if(strncasecmp(ptr,"update",6)==0){
E 41
I 41
  else if(strncasecmp(ptr,"update",6)==0) {
E 41
    update_count++;
  }
D 41
  else if(strncasecmp(ptr,"delete",6)==0){
E 41
I 41
  else if(strncasecmp(ptr,"delete",6)==0) {
E 41
    delete_count++;
  }
  else {
    other_count++;
  }

}

//-------------------------------------------------------------------


I 17
D 41
void init_tcl(){
E 41
I 41
void init_tcl() {
E 41

  // link c and Tcl variables
D 37
  Tcl_LinkVar(interp,"application",    	  (char *)&application,      TCL_LINK_STRING);
D 28
  Tcl_LinkVar(interp,"uniq_dgrp",     	  (char *)&uniq_dgrp,        TCL_LINK_STRING);
E 28
I 28
  Tcl_LinkVar(interp,"uniq_name",     	  (char *)&uniq_name,        TCL_LINK_STRING);
E 28
  Tcl_LinkVar(interp,"database",     	  (char *)&database,         TCL_LINK_STRING);
I 28
D 34
  Tcl_LinkVar(interp,"datagroup",     	  (char *)&datagroup,        TCL_LINK_STRING);
E 34
E 28
D 29
  Tcl_LinkVar(interp,"backlog_dir",    	  (char *)&backlog_dir,      TCL_LINK_STRING);
E 29
I 29
  Tcl_LinkVar(interp,"backlog_dir_name",  (char *)&backlog_dir_name, TCL_LINK_STRING);
E 29
D 33
  Tcl_LinkVar(interp,"debug_flag",    	  (char *)&debug_flag,       TCL_LINK_INT);
E 33
I 33
  Tcl_LinkVar(interp,"debug",    	  (char *)&debug,            TCL_LINK_INT);
E 33
  Tcl_LinkVar(interp,"init_tcl_script",	  (char *)&init_tcl_script,  TCL_LINK_STRING);

D 18
  Tcl_LinkVar(interp,"msg_count",         (char *)&msg_count,         TCL_LINK_INT);       
  Tcl_LinkVar(interp,"proc_count",        (char *)&proc_count,        TCL_LINK_INT);      
  Tcl_LinkVar(interp,"err_count",         (char *)&err_count,         TCL_LINK_INT);       
E 18
I 18
D 35
  Tcl_LinkVar(interp,"msg_count",         (char *)&msg_count,         TCL_LINK_INT);      
  Tcl_LinkVar(interp,"proc_count",        (char *)&proc_count,        TCL_LINK_INT);
  Tcl_LinkVar(interp,"err_count",         (char *)&err_count,         TCL_LINK_INT);
E 18
  Tcl_LinkVar(interp,"sql_count",         (char *)&sql_count,         TCL_LINK_INT);       
  Tcl_LinkVar(interp,"sql_trans_count",   (char *)&sql_trans_count,   TCL_LINK_INT); 
  Tcl_LinkVar(interp,"data_count",        (char *)&data_count,        TCL_LINK_INT);      
  Tcl_LinkVar(interp,"insert_count",      (char *)&insert_count,      TCL_LINK_INT);    
  Tcl_LinkVar(interp,"select_count",      (char *)&select_count,      TCL_LINK_INT);    
  Tcl_LinkVar(interp,"update_count",      (char *)&update_count,      TCL_LINK_INT);    
  Tcl_LinkVar(interp,"delete_count",      (char *)&delete_count,      TCL_LINK_INT);    
  Tcl_LinkVar(interp,"other_count",       (char *)&other_count,       TCL_LINK_INT);     
  Tcl_LinkVar(interp,"commit_count",      (char *)&commit_count,      TCL_LINK_INT);    
  Tcl_LinkVar(interp,"rollback_count",    (char *)&rollback_count,    TCL_LINK_INT);  
  Tcl_LinkVar(interp,"row_mod_count",     (char *)&row_mod_count,     TCL_LINK_INT);   
  Tcl_LinkVar(interp,"row_return_count",  (char *)&row_return_count,  TCL_LINK_INT);
E 35
I 35
  Tcl_LinkVar(interp,"msg_count",         (char *)&msg_count,        TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"proc_count",        (char *)&proc_count,       TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"err_count",         (char *)&err_count,        TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"sql_count",         (char *)&sql_count,        TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"sql_trans_count",   (char *)&sql_trans_count,  TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"data_count",        (char *)&data_count,       TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"insert_count",      (char *)&insert_count,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"select_count",      (char *)&select_count,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"update_count",      (char *)&update_count,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"delete_count",      (char *)&delete_count,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"other_count",       (char *)&other_count,      TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"commit_count",      (char *)&commit_count,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"rollback_count",    (char *)&rollback_count,   TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"row_mod_count",     (char *)&row_mod_count,    TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"row_return_count",  (char *)&row_return_count, TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 37
I 37
  Tcl_LinkVar(interp,(char*)"application",    	  (char *)&application,      TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"uniq_name",     	  (char *)&uniq_name,        TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"database",     	  (char *)&database,         TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"backlog_dir_name",  (char *)&backlog_dir_name, TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"debug",    	  (char *)&debug,            TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"init_tcl_script",	  (char *)&init_tcl_script,  TCL_LINK_STRING);

  Tcl_LinkVar(interp,(char*)"msg_count",         (char *)&msg_count,        TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"proc_count",        (char *)&proc_count,       TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"err_count",         (char *)&err_count,        TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"sql_count",         (char *)&sql_count,        TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"sql_trans_count",   (char *)&sql_trans_count,  TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"data_count",        (char *)&data_count,       TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"insert_count",      (char *)&insert_count,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"select_count",      (char *)&select_count,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"update_count",      (char *)&update_count,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"delete_count",      (char *)&delete_count,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"other_count",       (char *)&other_count,      TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"commit_count",      (char *)&commit_count,     TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"rollback_count",    (char *)&rollback_count,   TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"row_mod_count",     (char *)&row_mod_count,    TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"row_return_count",  (char *)&row_return_count, TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 37
E 35

I 19
D 24
  Tcl_LinkVar(interp,"msqlhost",              (char *)&msqlhost,              TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqldb",                (char *)&msqldb,                TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqlinfotbl",           (char *)&msqlinfotbl,           TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqlerrtbl",            (char *)&msqlerrtbl,            TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqlstattbl",           (char *)&msqlstattbl,           TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqlmhost",             (char *)&msqlmhost,             TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqlfacility",          (char *)&msqlfacility,          TCL_LINK_STRING);
  Tcl_LinkVar(interp,"msqldaemon",            (char *)&msqldaemon,            TCL_LINK_INT);
E 24
E 19

D 24

E 24
  // create Tcl commands
D 37
  Tcl_CreateCommand(interp,"help",tcl_help,
E 37
I 37
  Tcl_CreateCommand(interp,(char*)"help",tcl_help,
E 37
  		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 37
  Tcl_CreateCommand(interp,"quit",tcl_quit,
E 37
I 37
  Tcl_CreateCommand(interp,(char*)"quit",tcl_quit,
E 37
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 37
  Tcl_CreateCommand(interp,"stop",tcl_quit,
E 37
I 37
  Tcl_CreateCommand(interp,(char*)"stop",tcl_quit,
E 37
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 37
  Tcl_CreateCommand(interp,"exit",tcl_quit,
E 37
I 37
  Tcl_CreateCommand(interp,(char*)"exit",tcl_quit,
E 37
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  return;

}


//--------------------------------------------------------------------------


int tcl_help(ClientData clientdata, Tcl_Interp *interp,
D 41
		int argc, char **argv){
E 41
I 41
		int argc, char **argv) {
E 41

D 37
    char *help =
E 37
I 37
    const char *help =
E 37
    "\nTcl commands available in the dbrouter program:\n\n"
    " help                  print this message\n"
    " stop                  stop program\n"
    " quit                  stop program\n"
    " exit                  stop program\n"
    "\n\n Type command that require args with NO args for more information\n"
    "\n";

D 37
    Tcl_SetResult(interp,help,TCL_STATIC);
E 37
I 37
    Tcl_SetResult(interp,(char*)help,TCL_STATIC);
E 37

  return (TCL_OK);

}


/*---------------------------------------------------------------------*/


int tcl_quit(ClientData clientdata, Tcl_Interp *interp,
D 41
	     int argc, char **argv){
E 41
I 41
	     int argc, char **argv) {
E 41
  
I 19
  int err;

E 19
D 18
  logfile << "     ...received Tcl quit command..." << endl;
  dbrouter_done();
  exit(EXIT_SUCCESS);
E 18
I 18
  errfile << "     ...received Tcl quit command...stopping..." << endl;
D 19
  done=1;
E 19
E 18

I 19
  done=1;
E 19
  return(0);
}



//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------


E 17
D 15
// the following routines are called from dbrsql, a c program
E 15
I 15
// the following allow dbrsql (c routines) to access cout, cerr, etc.
E 15

extern "C" {



// allows sql routines to print to cout

D 37
void coutprintf(char *fmt, ...){
E 37
I 37
D 41
void coutprintf(const char *fmt, ...){
E 41
I 41
void coutprintf(const char *fmt, ...) {
E 41
E 37

  va_list args;

  va_start(args,fmt);

  vsprintf(temp,fmt,args);
  cout << temp;

  va_end(args);
}

//-------------------------------------------------------------------


// allows sql routines to print to cerr

D 37
void cerrprintf(char *fmt, ...){
E 37
I 37
D 41
void cerrprintf(const char *fmt, ...){
E 41
I 41
void cerrprintf(const char *fmt, ...) {
E 41
E 37

  va_list args;

  va_start(args,fmt);

  vsprintf(temp,fmt,args);
  cerr << temp;

  va_end(args);
}

//-------------------------------------------------------------------


// allows sql routines to print to log file

D 37
void logprintf(char *fmt, ...){
E 37
I 37
D 41
void logprintf(const char *fmt, ...){
E 41
I 41
void logprintf(const char *fmt, ...) {
E 41
E 37

  va_list args;

  va_start(args,fmt);

  vsprintf(temp,fmt,args);
  logfile << temp;

  va_end(args);
}

//-------------------------------------------------------------------


// allows sql routines to print to error file

D 37
void errprintf(char *fmt, ...){
E 37
I 37
D 41
void errprintf(const char *fmt, ...){
E 41
I 41
void errprintf(const char *fmt, ...) {
E 41
E 37

  va_list args;

  va_start(args,fmt);

  vsprintf(temp,fmt,args);
  errfile << temp;

  va_end(args);
}

//-------------------------------------------------------------------


// allows TipcMsgPrint to print to errfile

D 41
void msgerrprint(T_STR *fmt, ...){
E 41
I 41
void msgerrprint(T_STR *fmt, ...) {
E 41

  va_list args;

  va_start(args,fmt);

  vsprintf(temp,(char *)fmt,args);
  errfile << temp << flush;

  va_end(args);
}

I 15

E 15
//-------------------------------------------------------------------

I 15

E 15
I 7
D 37
int Exit_Error (char *MsgErr) {
E 37
I 37
int Exit_Error (const char *MsgErr) {
E 37
D 35
      errfile << MsgErr << endl;
      dbrouter_done();
      return (EXIT_FAILURE);
E 35
I 35
  errfile << endl << MsgErr << endl;
  dbrouter_done();
  return (EXIT_FAILURE);
E 35
}
E 7

D 15
}  // end of extern "C"
E 15
I 5

I 15
//-------------------------------------------------------------------
E 15


I 15
}  // end of extern "C"
E 15

I 15

I 17
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
E 17
E 15


D 17

E 17



E 5
E 1
