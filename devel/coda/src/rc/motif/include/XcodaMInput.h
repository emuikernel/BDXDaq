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
//	Xt Multiple Input Channels (Abstract Class)
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaMInput.h,v $
//   Revision 1.2  1998/04/08 17:29:05  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:25:00  chen
//   coda motif C++ library
//
//
#ifndef _XCODA_M_INPUT_H
#define _XCODA_M_INPUT_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#define XCODA_MAX_FD 256

typedef struct _input_xfd
{
  int fd;
  XtInputId id;
}XcodaInputXfd;

class XcodaMInput
{
public:
  // destructor
  virtual ~XcodaMInput (void);

  // operations
  // add a single file descriptor 
  void addInput    (int fd, XtPointer mask);
  void removeInput (int fd);

protected:
  // constrcutor
  XcodaMInput (Widget);
  // input callback function: derived classes implement
  virtual void input_callback (int fd) = 0;

  // internal file descriptors
  XcodaInputXfd xfds_[XCODA_MAX_FD];
  
private:
  static void inputCallback (XtPointer, int*, XtInputId*);
  XtAppContext context_;
};
#endif
