h35144
s 00001/00001/00085
d D 1.6 01/01/19 15:12:45 jhardie 7 6
c remove typedef for bool
c 
e
s 00028/00000/00058
d D 1.5 01/01/19 14:18:26 jhardie 6 5
c current version (but not 5.6 ready)
e
s 00000/00000/00058
d D 1.4 97/06/05 11:58:12 jhardie 5 4
c fixed some small problems with smartsockets arrays
e
s 00006/00005/00052
d D 1.3 97/06/05 10:40:37 jhardie 4 3
c Version 1.0 of info_server and support programs
e
s 00021/00000/00036
d D 1.2 97/06/02 18:37:15 jhardie 3 1
c Rewrite parser for full C++.  Info messages input ok,
c output still broken
e
s 00000/00000/00000
d R 1.2 97/03/25 09:45:00 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 3 2 info_server/server/info_globals.hh
c Name history : 2 1 server/info_globals.hh
c Name history : 1 0 s/server/info_globals.hh
e
s 00036/00000/00000
d D 1.1 97/03/25 09:44:59 jhardie 1 0
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
#ifndef INFO_GLOBALS_H_J
#define INFO_GLOBALS_H_J
//**********************************************************************
// Defines the global variables for the info_server program.  If the
// symbol RESERVE_SPACE is defined when this file is read, it will
// reserve the actual memory space for the global variables.  Otherwise,
// it will simply provide an external reference.
//
// Version: %A% %I% on %D% at %T%
//
// Programmer: J.G. Hardie
//**********************************************************************

#ifdef RESERVE_SPACE
#define SPACE
#else
#define SPACE extern
#endif RESERVE_SPACE

I 6
D 7
typedef int bool;
E 7
I 7
//typedef int bool;
E 7
// const bool true=1;
// const bool false=0;

E 6

SPACE Tcl_Interp *interp;	/* TCL interpreter */
SPACE int program_exit;		/* flag that program should quit */

I 3
// Lists for the parsed configuration file.  The space for these is
// reserved in the parser itself so that the parser can be independent
// of this (or any other info_server file).
extern llist inputlist;
extern llist outputlist;
extern llist aliaslist;

// Last time that *any* info server input was processed.
SPACE time_t last_input_time;

// Last time that *any* info server output was built
SPACE time_t last_output_time;

D 4

// The Smartsockets server object reference
// SPACE TipcSrv &ss_server;

E 4
E 3
// We want everyone to have access to the command line arguments
D 4
// but have to be careful about 
E 4
I 4
// but have to be careful about initialization.
E 4
#ifndef RESERVE_SPACE
I 4
extern TipcSrv &ss_serv;
E 4
extern char *app_name;
extern char *config_file;
extern char *tcl_config_file;
extern int debug_flag;
extern int no_tcl;
extern int monitor_evts;
extern char *unique_name;
I 6
extern char *state_file;
extern int not_loading_state;
extern int stateinterval;

//
// These next three are added at the request of E. Wolin
// dump_file is the name of the info server dump file.
// dump_time is the number of seconds between dumps of the data
// dump_flag determines if dumping is done.
// jgh - 6/15/99
//
extern char *dump_file;
extern int dump_time;
extern int dump_flag;
//
// The next three were added at the request of E. Wolin.
// log_any - flag to print a message whenever an input is seen.
// log_unknown - flag to print a message whenever an unknown input is
//               seen.
// log_bad - flag to print a message whenever an input doesn't
//           match it's definition.
extern int log_any;
extern int log_unknown;
extern int log_bad;
E 6
#endif RESERVE_SPACE

I 4
#ifdef RESERVE_SPACE
TipcSrv &ss_serv = TipcSrv::Instance();
#endif

E 4
#endif INFO_GLOBALS_H_J
I 3




E 3
E 1
