h55485
s 00002/00002/00040
d D 1.9 01/01/19 15:47:42 jhardie 11 10
c fixed version problems and const B.S.
e
s 00001/00000/00041
d D 1.8 01/01/19 15:12:24 jhardie 10 9
c Fixed const problem
c 
e
s 00000/00000/00041
d D 1.7 01/01/19 14:59:13 jhardie 9 8
i 7
c Merged changes between child workspace "/home/jhardie/scalers/info_server" and
c  parent workspace "/usr/local/clas/devel/source".
c 
e
s 00002/00002/00038
d D 1.6 01/01/09 16:13:54 jhardie 8 6
c Fixed const problem in parameters for read_config, config_info_tcl
c 
e
s 00001/00000/00040
d D 1.5.1.1 99/06/15 13:30:06 jhardie 7 6
c added prototype for dumpthelist() function.  Used in dumping the data
c to the specified file.
c 
e
s 00001/00002/00039
d D 1.5 98/01/22 14:48:49 jhardie 6 5
c added prototype for handle_timers() for dealing with timeouts in
c messages.
c Note:  Timer resolution is ticks of one second only.
c 
e
s 00000/00000/00041
d D 1.4 97/06/05 11:58:15 jhardie 5 4
c fixed some small problems with smartsockets arrays
e
s 00012/00001/00029
d D 1.3 97/06/05 10:40:40 jhardie 4 3
c Version 1.0 of info_server and support programs
e
s 00008/00001/00022
d D 1.2 97/06/02 18:37:17 jhardie 3 1
c Rewrite parser for full C++.  Info messages input ok,
c output still broken
e
s 00000/00000/00000
d R 1.2 97/03/25 09:45:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 3 2 info_server/server/info_proto.hh
c Name history : 2 1 server/info_proto.hh
c Name history : 1 0 s/server/info_proto.hh
e
s 00023/00000/00000
d D 1.1 97/03/25 09:45:01 jhardie 1 0
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
#ifndef INFO_PROTO_H_J
#define INFO_PROTO_H_J
//**********************************************************************
// Prototypes for the Info_Server program
//
// Version: %A% %I% on %D% at %T%
//
// Programmer: J.G. Hardie
//**********************************************************************

// TCL related prototypes
D 8
void config_info_tcl(char *config);
E 8
I 8
void config_info_tcl(const char *config);
E 8


// Configuration related prototypes
D 3
void config_info_c(char *config);
E 3
I 3
D 8
int read_config(char *f);
E 8
I 8
int read_config(const char *f);
E 8
E 3
char *data_stream_status();
I 3
void printlist(llist &l);
I 7
void dumpthelist(llist &l, ostream &out);
I 10
D 11
void process_info_state(const char *filename)
E 10
E 7
E 3

E 11
I 11
void process_info_state(const char *filename);
void save_state_now(const char *filename);
E 11
I 3

E 3
// Smartsockets message related prototypes
void setup_ss_calbacks();
I 3
void ipc_open_input(llist &l);
void process_info_serv(T_IPC_CONN conn, T_IPC_CONN_DEFAULT_CB_DATA data,
		       T_CB_ARG arg);
D 4
void process_input_msg(TipcMsg &m, input *in);
E 4
I 4
int process_input_msg(TipcMsg &m, input *in);
E 4
void reconcile_output();
I 4
void construct_output(output *o);
E 4
E 3
D 6


E 6
I 6
void handle_timers();
E 6
I 4

// A simple inline function to make the system time() routines
// behave in a more rational manner.  Also makes it clearer what is
// going on.
inline time_t timestamp() 
{
  time_t t;
  return time(&t);
}

E 4
#endif INFO_PROTO_H_J
E 1
