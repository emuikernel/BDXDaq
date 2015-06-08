//*************************************************************************
//
// Info_Server
//
// Program to accept data from various sources, repackage it as desired
// and send it out in SmartSockets format to various data collectors or
// display programs
//
// Version: @(#) info_server.cc 1.13@(#) 1.13 on 05/02/25 at 16:25:44
//
// Programmer: J.G. Hardie
//
//*************************************************************************

// Include all the standard stuff
#include "info_includes.hh"


// Info server class definitions and prototypes.  We reserve
// space here.  Do not define RESERVE_SPACE in any other files.
#define RESERVE_SPACE
#include "info_classes.hh"
#include "info_globals.hh"
#include "info_proto.hh"
#undef RESERVE_SPACE


// Command line arguments
extern "C" {
#include "procargs.h"
}

// Command line variables
const char *app_name = "clasprod";
const char *config_file = "/usr/local/clas/parms/info_server/info_server.cfg";
const char *tcl_config_file = NULL;
int debug_flag = 0;
int no_tcl = 0;
int monitor_evts = 0;

const char *unique_name = "info_server";
const char *dump_file = "info_server_dump.dat";
int dump_time = 120;
int dump_flag = 0;
int log_any = 0;
int log_unknown = 0;
int log_bad = 0;
int not_loading_state=1;
const char *state_file = "/usr/local/clas/parms/info_server/info_server.sta";
int stateinterval = 300;


ARGTABLE
ARGENTRY("app",STRING,&app_name,"is the application name"),
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
ENDTABLE

extern "C" void status_poll_callback(T_IPC_MSG msg);
void quit_callback(int sig);

// Escape button.  This gets set to 1 when we are supposed to quit.

int main(int argc, char **argv)
{
    int status;
    TipcMt mt;

    // Force synchronization between C and C++ I/O systems.
    ios::sync_with_stdio();
    
    
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
    last_input_time = timestamp();
    last_output_time = timestamp();
    
    cout << "@(#) info_server.cc 1.13@(#) 1.13 Starting" << endl;
    program_exit = 0;

    // process the command line arguments
    GETARGS(argc,argv);

    // If we are logging, make sure that log_any and log_bad/log_unknown
    // are consistent
    if (log_any)
      log_bad = log_unknown = log_any;
    
    
    // initialize the TCL interpreter
    if (!no_tcl)
    {
	cout << "Starting TCL interpreter" << endl;
	interp = tclinterp_init();
	config_info_tcl(tcl_config_file);
    }
    

    // read the configuration file and build the event streams
    cout << "Reading the configuration file" << endl;
    if (!read_config(config_file))
    {
      cerr << "InfoServer:  Error in configuration file."
	   << "Info server cannot start.  Goodbye." << endl;
      exit(0);
    }
    
    DEBUG(
      cout << "DEBUG: Input List" << endl;
      printlist(inputlist);
      cout << "DEBUG: Output List" << endl;
      printlist(outputlist);
      );
    
    

    // initialize smartsockets
    // Not sure if this works, but we'll find out... we attempt
    // to get a reference to the server object.  Hopefully, we
    // can then go ahead and use the server object, but the ipc
    // calls might screw things up.
    ipc_set_application(app_name);
    ipc_set_user_status_poll_callback(status_poll_callback);
//    ipc_set_quit_callback(quit_callback);
    status = ipc_init(unique_name,"Info_Server");
    if (status < 0)
    {
	cout << "Info_Server: Cannot start IPC connection" << endl;
	cout << "Probably a duplicate name (" << unique_name << ")" << endl;
	exit(EXIT_FAILURE);
    }

    // If TCL is enabled, create the tcl_ipc callbacks
    if (!no_tcl)
	tclipc_init(interp);

    // Next, set a callback for the incoming message type "info_server"
    mt.Lookup( CAST "info_server");
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

    // Here, we open all the required smartsockets data groups for
    // the input streams.  The output streams don't require any 
    // additional data groups since each outgoing message carries
    // it's own data group along with it.
    ipc_open_input(inputlist);

    // For now, we just loop here waiting for something to happen.
    DEBUG(cout << "InfoServer: Entering wait loop" << endl;);
    
    ss_serv.Flush();
    fflush(NULL);

    // At this point, we open the state file and process all the messages
    // contained in it.  If the state interval is set to zero, we don't
    // read the state (or write it).
    if (stateinterval)
      process_info_state(state_file);
    
    // The following loop deals with both incoming asynchronous
    // smartsockets messages and the synchronous timeouts that might
    // be present on various output messages.
    while (!program_exit)
    {
      ss_serv.MainLoop(1.0);
      handle_timers();
    }
    
    return 0;
}

//----------------------------------------------------------------------
// status_poll_callback
// Provides a status message for the IPC monitor program.
// Argument is a semi-constructed message to which we add
// some further useless information.
//----------------------------------------------------------------------
extern "C" void status_poll_callback(T_IPC_MSG msg)
{
    char *t;

    TipcMsgAppendStr(msg, CAST "Stream Status");
    t = data_stream_status();
    if (t)
	TipcMsgAppendStr(msg,t);
    else
	TipcMsgAppendStr(msg, CAST "No Status Available");

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
    cout << "Quit Callback was called with sig = " << sig << endl;
    return;
}


