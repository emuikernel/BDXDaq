//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      RunControl Thread/Main Thread communication channel
//      This class only exists in the main thread
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rccStdinPipe.cc,v $
//   Revision 1.1  1998/11/09 20:40:58  heyes
//   merge with Linux port
//
//
//
#include "rccStdinPipe.h"
#ifdef USE_TK
#include "../Components.s/rcTclInterface.h"
#endif

rccStdinPipe::rccStdinPipe (Reactor& r)
:reactor_ (r)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create rccStdinPipe Class Object\n");
#endif

#ifdef USE_TK
    initTcl();
#endif

    if (reactor_.register_handler (0, this, 
				 Event_Handler::READ_MASK) == -1) 
    fprintf (stderr, "Cannot register event handler to thread pipe\n");
}

rccStdinPipe::~rccStdinPipe (void)
{
#ifdef _TRACE_OBJECTS
  printf ("      Delete rccStdinPipe Class Object\n");
#endif
  // empty
}

int
rccStdinPipe::get_handle (void) const
{
  return 0;
}

int
rccStdinPipe::handle_close (int, Reactor_Mask )
{
  //close (pipe_[0]);
  //close (pipe_[1]);
  return 0;
}

int
rccStdinPipe::handle_input (int)
{
#ifdef USE_TK
   tkOneEvent();
#endif
   return 0;
}

int
rccStdinPipe::writeFd (void) const
{
  return 0;
}

