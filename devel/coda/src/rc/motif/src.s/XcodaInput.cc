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
//	 X window Input class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaInput.cc,v $
//   Revision 1.2  1998/04/08 17:28:58  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <assert.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <XcodaInput.h>

XcodaInput::XcodaInput(Widget parent)
{
#ifdef _TARCE_OBJECTS
  printf("    Create XcodaInput Object\n");
#endif
  _hasInput = 0;
  _fd = 0;
  _context = XtWidgetToApplicationContext(parent);
  assert(_context);
}

XcodaInput::XcodaInput(XtAppContext context)
:_context (context)
{
#ifdef _TARCE_OBJECTS
  printf("    Create XcodaInput Object\n");
#endif
  _hasInput = 0;
  _fd = 0;
}

XcodaInput::~XcodaInput()
{
#ifdef _TARCE_OBJECTS
  printf("    Delete XcodaInput Object\n");
#endif
  if(_hasInput)
    removeInput();
}

void XcodaInput::addInput(int fd, XtPointer mask)
{
  _hasInput = 1;
  _fd = fd;
  _inputId = XtAppAddInput(_context, fd, mask,
			   &XcodaInput::inputCallback,
			   (XtPointer)this);
}
    
void XcodaInput::removeInput()
{
  _hasInput = 0;
  XtRemoveInput(_inputId);
}

void XcodaInput::inputCallback(XtPointer client_data,
			       int       *fd,
			       XtInputId *id)
{
  XcodaInput *obj = (XcodaInput *)client_data;
  obj->input_callback();
}
