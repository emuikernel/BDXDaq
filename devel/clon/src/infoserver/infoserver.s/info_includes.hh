#ifndef INFO_INCLUDES_H_J
#define INFO_INCLUDES_H_J
//**********************************************************************
// Standard include files for the info-server
//**********************************************************************

// KLUDGE for broken C++ API
// Uncomment when talarian fixes their header files.
//#define CAST
#define CAST (char *)

// We wanna be POSIX compliant if possible
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

// System related stuff
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// C++ streams and other stuff
#include <fstream.h>
#include <iostream.h>
#include <iomanip.h>

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
