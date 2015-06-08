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
//   $Log: rccThrPipe.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#include "rccThrPipe.h"

rccThrPipe::rccThrPipe (Reactor& r)
:reactor_ (r)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create rccThrPipe Class Object\n");
#endif
  ::pipe (pipe_);
  
  if (reactor_.register_handler (pipe_[0], this, 
				 Event_Handler::READ_MASK) == -1) 
    fprintf (stderr, "Cannot register event handler to thread pipe\n");
}

rccThrPipe::~rccThrPipe (void)
{
#ifdef _TRACE_OBJECTS
  printf ("      Delete rccThrPipe Class Object\n");
#endif
  // empty
}

int
rccThrPipe::get_handle (void) const
{
  return pipe_[0];
}

int
rccThrPipe::handle_close (int, Reactor_Mask )
{
  close (pipe_[0]);
  close (pipe_[1]);
  return 0;
}

int
rccThrPipe::handle_input (int)
{
  int   n, status = 0;
  long  type, size;
  daqNetData ndata;
  rcMsg *recver = new rcMsg (DAUNKNOWN, ndata);

  n = pipe_[0] >> *recver;
#ifdef _CODA_DEBUG
  printf ("%d bytes is received from pipe\n", n);
  printf ("recver type is %d\n", recver->type ());
#endif
  switch (n) {
  case -1:
  case 0:
    printf ("Pipe is broken from thread\n");
    delete recver;
    status = -1;
    break;
  default:
    break;
  }
  return status;
}

int
rccThrPipe::writeFd (void) const
{
  return pipe_[1];
}

