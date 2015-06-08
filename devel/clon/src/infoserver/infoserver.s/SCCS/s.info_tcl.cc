h60769
s 00001/00001/00024
d D 1.6 01/01/09 16:25:02 jhardie 7 6
c Fixed ANOTHER const problem - sheesh
c 
e
s 00001/00001/00024
d D 1.5 01/01/09 16:12:23 jhardie 6 5
c Fixed const problem with config_info_tcl
c 
e
s 00000/00000/00025
d D 1.4 97/06/05 11:58:18 jhardie 5 4
c fixed some small problems with smartsockets arrays
e
s 00000/00000/00025
d D 1.3 97/06/05 10:40:43 jhardie 4 3
c Version 1.0 of info_server and support programs
e
s 00001/00001/00024
d D 1.2 97/06/02 18:37:20 jhardie 3 1
c Rewrite parser for full C++.  Info messages input ok,
c output still broken
e
s 00000/00000/00000
d R 1.2 97/03/25 09:45:05 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 3 2 info_server/server/info_tcl.cc
c Name history : 2 1 server/info_tcl.cc
c Name history : 1 0 s/server/info_tcl.cc
e
s 00025/00000/00000
d D 1.1 97/03/25 09:45:04 jhardie 1 0
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
//**********************************************************************
// info_tcl.cc
//
// TCL related service routines, variables and commands for the Info_server
// program.
//
// Version: %A% %I%
//
// Programmer: J.G. Hardie
//**********************************************************************

#include "info_includes.hh"

D 3
#include "info_proto.hh"
E 3
#include "info_classes.hh"
#include "info_globals.hh"
I 3
#include "info_proto.hh"
E 3


D 6
void config_info_tcl(char *config)
E 6
I 6
void config_info_tcl(const char *config)
E 6
{

  if (config)
D 7
    Tcl_EvalFile(interp,config);
E 7
I 7
    Tcl_EvalFile(interp,CAST config);
E 7

}
E 1
