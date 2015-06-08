//**********************************************************************
// info_tcl.cc
//
// TCL related service routines, variables and commands for the Info_server
// program.
//
// Version: @(#) info_tcl.cc 1.6@(#) 1.6
//
// Programmer: J.G. Hardie
//**********************************************************************

#include "info_includes.hh"

#include "info_classes.hh"
#include "info_globals.hh"
#include "info_proto.hh"


void config_info_tcl(const char *config)
{

  if (config)
    Tcl_EvalFile(interp,CAST config);

}
