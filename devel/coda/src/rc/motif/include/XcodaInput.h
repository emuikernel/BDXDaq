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
//	 XWindow Input Channel which allow X to monitor
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaInput.h,v $
//   Revision 1.2  1998/04/08 17:28:59  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:58  chen
//   coda motif C++ library
//
//
#ifndef _XCODA_INPUT_H
#define _XCODA_INPUT_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

class XcodaInput
{
public:
  // constructor and destrctor
  virtual ~XcodaInput();

  void addInput(int fd, XtPointer mask);
  void removeInput();
  
protected:
  // derived class has to provide this
  XcodaInput  (Widget);
  XcodaInput  (XtAppContext context);

  int         _fd;
  XtInputId   _inputId;
  virtual void input_callback() = 0;
  
private:
  static void inputCallback(XtPointer, int *, XtInputId *);
  int         _hasInput;
  XtAppContext _context;
};
#endif
