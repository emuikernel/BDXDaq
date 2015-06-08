//-----------------------------------------------------------------------------
// Copyright (c) 1991,1992 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
// Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
//-----------------------------------------------------------------------------
// 
// Description:
//	 X Reactor For Input/Output
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: X_Reactor.h,v $
//   Revision 1.1.1.1  1996/10/10 20:04:47  chen
//   simple ACE version 3.1
//
//
#ifndef _X_REACTOR_H
#define _X_REACTOR_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Event_Handler.h>
#include <sysincludes.h>

class X_Reactor
{
 public:
  enum {DEFAULT_SIZE = 256};
  // constructor and destructor
  
  X_Reactor (XtAppContext app);
  X_Reactor (XtAppContext app, int size, int restart = 0);
  virtual ~X_Reactor (void);

  virtual int open (int size = DEFAULT_SIZE, int restart = 0);
  virtual void close (void);

  virtual int register_handler (Event_Handler *, Reactor_Mask);
  virtual int remove_handler (Event_Handler *, Reactor_Mask);

  virtual void notify_handle(HANDLE handle);

 protected:
  virtual int attach (HANDLE handle, Event_Handler *, Reactor_Mask);
  virtual int detach (HANDLE handle, Reactor_Mask);

  static void inputCallback (XtPointer, int *, XtInputId *);
  int   max_size_;
  int   max_handle_;
  Event_Handler **event_handlers_;
  Reactor_Mask *mask_;
  XtInputId *xinput_id_;

 private:
  XtAppContext app_context_;
};

#ifdef __INLINE__
#define INLINE inline
#include "X_Reactor.h"
#else
#define INLINE
#endif
#endif

