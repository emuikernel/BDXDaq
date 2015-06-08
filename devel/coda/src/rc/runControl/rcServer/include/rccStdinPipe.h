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
//   $Log: rccStdinPipe.h,v $
//   Revision 1.1  1998/11/09 20:40:59  heyes
//   merge with Linux port
//
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#ifndef _RC_THR_PIPE_H
#define _RC_THR_PIPE_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <Event_Handler.h>
#include <Reactor.h>
#include <rcMsg.h>

class rccStdinPipe: public Event_Handler
{
public:
  // constructor and destructor
  rccStdinPipe  (Reactor& r);
  ~rccStdinPipe (void);
  
  // operations
  
  // return the file descriptor for writing
  int writeFd (void) const;

protected:
  // inherited operations
  virtual int get_handle   (void) const;
  virtual int handle_close (int, Reactor_Mask);
  virtual int handle_input (int);

private:
  // reactor
  Reactor& reactor_;
};
#endif
