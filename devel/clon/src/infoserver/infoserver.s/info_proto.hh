#ifndef INFO_PROTO_H_J
#define INFO_PROTO_H_J
//**********************************************************************
// Prototypes for the Info_Server program
//
// Version: @(#) info_proto.hh 1.9@(#) 1.9 on 05/02/25 at 16:25:44
//
// Programmer: J.G. Hardie
//**********************************************************************

// TCL related prototypes
void config_info_tcl(const char *config);


// Configuration related prototypes
int read_config(const char *f);
char *data_stream_status();
void printlist(llist &l);
void dumpthelist(llist &l, ostream &out);
void process_info_state(const char *filename);
void save_state_now(const char *filename);

// Smartsockets message related prototypes
void setup_ss_calbacks();
void ipc_open_input(llist &l);
void process_info_serv(T_IPC_CONN conn, T_IPC_CONN_DEFAULT_CB_DATA data,
		       T_CB_ARG arg);
int process_input_msg(TipcMsg &m, input *in);
void reconcile_output();
void construct_output(output *o);
void handle_timers();

// A simple inline function to make the system time() routines
// behave in a more rational manner.  Also makes it clearer what is
// going on.
inline time_t timestamp() 
{
  time_t t;
  return time(&t);
}

#endif INFO_PROTO_H_J
