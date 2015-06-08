h43197
s 00005/00000/00051
d D 1.5 01/01/09 16:02:47 jhardie 6 5
c Added definition of CAST for ANSI compliance.
c 
e
s 00000/00000/00051
d D 1.4 97/06/05 11:58:13 jhardie 5 4
c fixed some small problems with smartsockets arrays
e
s 00001/00000/00050
d D 1.3 97/06/05 10:40:38 jhardie 4 3
c Version 1.0 of info_server and support programs
e
s 00013/00000/00037
d D 1.2 97/06/02 18:37:15 jhardie 3 1
c Rewrite parser for full C++.  Info messages input ok,
c output still broken
e
s 00000/00000/00000
d R 1.2 97/03/25 09:45:01 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 3 2 info_server/server/info_includes.hh
c Name history : 2 1 server/info_includes.hh
c Name history : 1 0 s/server/info_includes.hh
e
s 00037/00000/00000
d D 1.1 97/03/25 09:45:00 jhardie 1 0
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
#ifndef INFO_INCLUDES_H_J
#define INFO_INCLUDES_H_J
//**********************************************************************
// Standard include files for the info-server
//**********************************************************************

I 6
// KLUDGE for broken C++ API
// Uncomment when talarian fixes their header files.
//#define CAST
#define CAST (char *)

E 6
// We wanna be POSIX compliant if possible
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

// System related stuff
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
I 3
#include <ctype.h>
I 4
#include <time.h>
E 4
E 3

// C++ streams and other stuff
#include <fstream.h>
#include <iostream.h>
#include <iomanip.h>

I 3
// A definition for a useful debugging macro.  If COMPDEBUG
// is defined then the code enclosed by this macro is compiled
// and kept in the program, otherwise the code is omitted.
#ifndef DEBUG
#ifdef DEBUG_CODE
#define DEBUG(x) x
#else
#define DEBUG(x)
#endif
#endif


E 3
// TCL definitions
extern "C" {
#include <tcl.h>
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


// SmartSockets API
#include <rtworks/cxxipc.hxx>

// CLAS ipc definitions
#include <clas_ipc_prototypes.h>

#endif INFO_INCLUDES_H_J
E 1
