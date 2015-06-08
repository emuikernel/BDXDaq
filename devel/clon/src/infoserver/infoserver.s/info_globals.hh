#ifndef INFO_GLOBALS_H_J
#define INFO_GLOBALS_H_J
//**********************************************************************
// Defines the global variables for the info_server program.  If the
// symbol RESERVE_SPACE is defined when this file is read, it will
// reserve the actual memory space for the global variables.  Otherwise,
// it will simply provide an external reference.
//
// Version: @(#) info_globals.hh 1.6@(#) 1.6 on 05/02/25 at 16:25:43
//
// Programmer: J.G. Hardie
//**********************************************************************

#ifdef RESERVE_SPACE
#define SPACE
#else
#define SPACE extern
#endif RESERVE_SPACE

//typedef int bool;
// const bool true=1;
// const bool false=0;


SPACE Tcl_Interp *interp;	/* TCL interpreter */
SPACE int program_exit;		/* flag that program should quit */

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

// We want everyone to have access to the command line arguments
// but have to be careful about initialization.
#ifndef RESERVE_SPACE
extern TipcSrv &ss_serv;
extern char *app_name;
extern char *config_file;
extern char *tcl_config_file;
extern int debug_flag;
extern int no_tcl;
extern int monitor_evts;
extern char *unique_name;
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
#endif RESERVE_SPACE

#ifdef RESERVE_SPACE
TipcSrv &ss_serv = TipcSrv::Instance();
#endif

#endif INFO_GLOBALS_H_J




