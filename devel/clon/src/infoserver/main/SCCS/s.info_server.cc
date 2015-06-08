h41795
s 00003/00002/00233
d D 1.13 01/01/19 15:03:09 jhardie 15 13
i 14
c Merged changes between child workspace "/home/jhardie/scalers/info_server" and
c  parent workspace "/usr/local/clas/devel/source".
c 
e
s 00037/00008/00198
d D 1.11.1.1 01/01/19 14:18:59 jhardie 14 12
c current version (but not 5.6 ready)
e
s 00009/00009/00197
d D 1.12 01/01/09 16:15:39 jhardie 13 12
c Fixed const problems with SS and TCL functions
c 
e
s 00001/00001/00205
d D 1.11 99/01/06 19:21:26 jhardie 12 11
c Modified for verions of infoserver which
c mangle names, new ss groups, and timestamping.
e
s 00005/00001/00201
d D 1.10 98/06/03 14:28:18 jhardie 11 10
c added sync_with_stdio() to synchronize C and C++ io.  Also forced a
c flush of the C output streams before entering the main loop
c 
e
s 00007/00000/00195
d D 1.9 98/01/27 11:16:14 jhardie 10 9
c Added debug code to dump input and output lists.
c 
e
s 00003/00002/00192
d D 1.8 98/01/22 17:39:49 jhardie 9 8
c assorted bugfixes including a bus error in the ipc_quit_callback
c function.
c 
e
s 00007/00001/00187
d D 1.7 98/01/22 14:52:16 jhardie 8 7
c added code to main loop to check timeouts every 1 second.  Note that
c this is not a precision timer, an output message with a timeout of 10
c seconds will get sent out approximately every 10 seconds, plus some
c relatively small overhead.
c 
e
s 00002/00002/00186
d D 1.6 97/06/06 11:33:03 jhardie 7 6
c made the defaults more reasonable
e
s 00003/00000/00185
d D 1.5 97/06/05 11:58:15 jhardie 6 5
c fixed some small problems with smartsockets arrays
e
s 00000/00000/00185
d D 1.4 97/06/05 10:46:24 jhardie 5 4
c Added versioning variables for SCCS and added install: target for makefile
e
s 00002/00003/00183
d D 1.3 97/06/05 10:40:40 jhardie 4 3
c Version 1.0 of info_server and support programs
e
s 00059/00003/00127
d D 1.2 97/06/02 18:37:18 jhardie 3 1
c Rewrite parser for full C++.  Info messages input ok,
c output still broken
e
s 00000/00000/00000
d R 1.2 97/03/25 09:45:03 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 3 2 info_server/server/info_server.cc
c Name history : 2 1 server/info_server.cc
c Name history : 1 0 s/server/info_server.cc
e
s 00130/00000/00000
d D 1.1 97/03/25 09:45:02 jhardie 1 0
c New C++ version of info-server program.  Translation
c from C because of complexity of internal structures.
c These are better handled with classes.
e
u
U
f e 0
t
T
I 1
//*************************************************************************
//
// Info_Server
//
// Program to accept data from various sources, repackage it as desired
// and send it out in SmartSockets format to various data collectors or
// display programs
//
// Version: %A% %I% on %D% at %T%
//
// Programmer: J.G. Hardie
//
//*************************************************************************

// Include all the standard stuff
#include "info_includes.hh"


// Info server class definitions and prototypes.  We reserve
// space here.  Do not define RESERVE_SPACE in any other files.
#define RESERVE_SPACE
D 3
#include "info_proto.hh"
E 3
#include "info_classes.hh"
#include "info_globals.hh"
I 3
#include "info_proto.hh"
E 3
#undef RESERVE_SPACE


// Command line arguments
extern "C" {
#include "procargs.h"
}

// Command line variables
D 7
char *app_name = "testing";
char *config_file = NULL;
E 7
I 7
D 13
char *app_name = "clasprod";
D 12
char *config_file = "/usr/local/clas/parms/info_server.cfg";
E 12
I 12
char *config_file = "/usr/local/clas/parms/info_server/info_server.cfg";
E 12
E 7
char *tcl_config_file = NULL;
E 13
I 13
const char *app_name = "clasprod";
const char *config_file = "/usr/local/clas/parms/info_server/info_server.cfg";
const char *tcl_config_file = NULL;
E 13
int debug_flag = 0;
int no_tcl = 0;
int monitor_evts = 0;
I 15

E 15
D 13
char *unique_name = "info_server";
E 13
I 13
const char *unique_name = "info_server";
E 13
I 14
D 15
char *dump_file = "info_server_dump.dat";
E 15
I 15
const char *dump_file = "info_server_dump.dat";
E 15
int dump_time = 120;
int dump_flag = 0;
int log_any = 0;
int log_unknown = 0;
int log_bad = 0;
int not_loading_state=1;
D 15
char *state_file = "/usr/local/clas/parms/info_server/info_server.sta";
E 15
I 15
const char *state_file = "/usr/local/clas/parms/info_server/info_server.sta";
E 15
int stateinterval = 300;

E 14

ARGTABLE
ARGENTRY("app",STRING,&app_name,"is the application name"),
D 14
ARGENTRY("config",FILENAME,&config_file,"is the configuration file name"),
ARGENTRY("tclscp",FILENAME,&tcl_config_file,"is the TCL configuration script"),
ARGENTRY("uniq",STRING,&unique_name,"is the programs unique name"),
ARGENTRY("notcl",TOGGLE,&no_tcl,"turns off the TCL interpreter"),
ARGENTRY("debug",TOGGLE,&debug_flag,"turns on debugging output"),
ARGENTRY("monitor",TOGGLE,&monitor_evts,"turns on event monitoring"),
ARGENTRY("help",USAGE,NULL,NULL),
E 14
I 14
 ARGENTRY("config",FILENAME,&config_file,"is the configuration file name"),
 ARGENTRY("tclscp",FILENAME,&tcl_config_file,"is the TCL configuration script"),
  ARGENTRY("state",FILENAME,&state_file,"is the name of the file to which the info_server saves its internal state"),
  ARGENTRY("savetime",INTEGER,&stateinterval,"is the interval between state saves"),
 ARGENTRY("uniq",STRING,&unique_name,"is the programs unique name"),
 ARGENTRY("notcl",TOGGLE,&no_tcl,"turns off the TCL interpreter"),
 ARGENTRY("debug",TOGGLE,&debug_flag,"turns on debugging output"),
// ARGENTRY("monitor",TOGGLE,&monitor_evts,"turns on event monitoring"),
 ARGENTRY("dump",TOGGLE,&dump_flag,"turns on data dumping to dumpfile"),
 ARGENTRY("dumpfile",FILENAME,&dump_file,"is the name of file in which data are dumped."),
 ARGENTRY("dumptime",INTEGER,&dump_time,"is the number of seconds between dumps of data"),
 ARGENTRY("logany",TOGGLE,&log_any,"enables notification when any input is seen"),
 ARGENTRY("logunknown",TOGGLE,&log_unknown,"enables notification when unknown input is seen"),
 ARGENTRY("logbad",TOGGLE,&log_bad,"enables notification when undefined input is seen"), 
 ARGENTRY("help",USAGE,NULL,NULL),
E 14
ENDTABLE

D 13
void status_poll_callback(T_IPC_MSG msg);
E 13
I 13
extern "C" void status_poll_callback(T_IPC_MSG msg);
E 13
void quit_callback(int sig);
I 3
D 4
TipcSrv &ss_serv = TipcSrv::Instance();
E 4
E 3

I 3
// Escape button.  This gets set to 1 when we are supposed to quit.
E 3

int main(int argc, char **argv)
{
    int status;
I 3
    TipcMt mt;
E 3

I 11
    // Force synchronization between C and C++ I/O systems.
    ios::sync_with_stdio();
    
    
E 11
I 3
    // Initialize the last times.  These are used to avoid unnecessary
    // list searches to find out if we have to build a new message or
    // not.  The handshake works as follows:  When an input message
    // is received, the callback tries to parse it.  If the parse fails
    // because the message is from an unknown source, or for any other
    // reason, then last_input_time will not be changed.  When the
    // parse callback exits, the main loop will try to see if any output
    // needs to be built.  If last_output_time > last_input_time, then
    // no output is needed and the search can stop.  OTOH, if there is
    // no problem with the parse, then the last_input_time will be changed
    // and the output routines will have to check to see if any of the
    // output streams are due for construction.
D 4
    last_input_time = 0;
    last_output_time = 1;
E 4
I 4
    last_input_time = timestamp();
    last_output_time = timestamp();
E 4
    
E 3
    cout << "%A% %I% Starting" << endl;
    program_exit = 0;

    // process the command line arguments
    GETARGS(argc,argv);

I 14
    // If we are logging, make sure that log_any and log_bad/log_unknown
    // are consistent
    if (log_any)
      log_bad = log_unknown = log_any;
    
    
E 14
    // initialize the TCL interpreter
    if (!no_tcl)
    {
	cout << "Starting TCL interpreter" << endl;
	interp = tclinterp_init();
	config_info_tcl(tcl_config_file);
    }
    

    // read the configuration file and build the event streams
    cout << "Reading the configuration file" << endl;
D 3
    config_info_c(config_file);
E 3
I 3
    if (!read_config(config_file))
    {
      cerr << "InfoServer:  Error in configuration file."
	   << "Info server cannot start.  Goodbye." << endl;
      exit(0);
    }
E 3
    
I 10
    DEBUG(
      cout << "DEBUG: Input List" << endl;
      printlist(inputlist);
      cout << "DEBUG: Output List" << endl;
      printlist(outputlist);
      );
    
E 10
I 3
    
E 3

    // initialize smartsockets
I 3
    // Not sure if this works, but we'll find out... we attempt
    // to get a reference to the server object.  Hopefully, we
    // can then go ahead and use the server object, but the ipc
    // calls might screw things up.
E 3
    ipc_set_application(app_name);
    ipc_set_user_status_poll_callback(status_poll_callback);
D 9
    ipc_set_quit_callback(quit_callback);
E 9
I 9
//    ipc_set_quit_callback(quit_callback);
E 9
D 3
    status = ipc_init(unique_name,"Info Server");
E 3
I 3
    status = ipc_init(unique_name,"Info_Server");
E 3
    if (status < 0)
    {
	cout << "Info_Server: Cannot start IPC connection" << endl;
	cout << "Probably a duplicate name (" << unique_name << ")" << endl;
	exit(EXIT_FAILURE);
    }

    // If TCL is enabled, create the tcl_ipc callbacks
    if (!no_tcl)
	tclipc_init(interp);

I 3
    // Next, set a callback for the incoming message type "info_server"
D 13
    mt.Lookup("info_server");
E 13
I 13
    mt.Lookup( CAST "info_server");
E 13
    if (!mt)
    {
      cout << "Info_Server: could not find info_server mt" << endl;
      return T_EXIT_FAILURE;
    }
    
    if (!ss_serv.ProcessCbCreate(mt,process_info_serv,0))
    {
      cout << "Info_Server: could not set info_server cb" << endl;
      return T_EXIT_FAILURE;
    }
E 3

I 3
    // Here, we open all the required smartsockets data groups for
    // the input streams.  The output streams don't require any 
    // additional data groups since each outgoing message carries
    // it's own data group along with it.
    ipc_open_input(inputlist);

    // For now, we just loop here waiting for something to happen.
    DEBUG(cout << "InfoServer: Entering wait loop" << endl;);
    
I 6
    ss_serv.Flush();
D 11
    
E 11
I 11
    fflush(NULL);
E 11
D 8
    
E 8
I 8

I 14
    // At this point, we open the state file and process all the messages
    // contained in it.  If the state interval is set to zero, we don't
    // read the state (or write it).
    if (stateinterval)
      process_info_state(state_file);
    
E 14
    // The following loop deals with both incoming asynchronous
    // smartsockets messages and the synchronous timeouts that might
    // be present on various output messages.
E 8
E 6
    while (!program_exit)
I 8
    {
E 8
      ss_serv.MainLoop(1.0);
I 8
      handle_timers();
    }
E 8
    
E 3
D 9
    exit(EXIT_SUCCESS);
E 9
I 9
    return 0;
E 9
}

//----------------------------------------------------------------------
// status_poll_callback
// Provides a status message for the IPC monitor program.
// Argument is a semi-constructed message to which we add
// some further useless information.
//----------------------------------------------------------------------
D 13
void status_poll_callback(T_IPC_MSG msg)
E 13
I 13
extern "C" void status_poll_callback(T_IPC_MSG msg)
E 13
{
    char *t;

D 13
    TipcMsgAppendStr(msg,"Stream Status");
E 13
I 13
    TipcMsgAppendStr(msg, CAST "Stream Status");
E 13
    t = data_stream_status();
    if (t)
	TipcMsgAppendStr(msg,t);
    else
D 13
	TipcMsgAppendStr(msg,"No Status Available");
E 13
I 13
	TipcMsgAppendStr(msg, CAST "No Status Available");
E 13

    return;
}

//----------------------------------------------------------------------
// quit_callback
// Processes the quit event.  Doesn't do much really - sets a flag
// which causes the program to exit from it's event loop.
//----------------------------------------------------------------------
void quit_callback(int sig)
{
    program_exit = 1;
I 9
    cout << "Quit Callback was called with sig = " << sig << endl;
E 9
    return;
}
I 3
D 14

E 14


E 3
E 1
