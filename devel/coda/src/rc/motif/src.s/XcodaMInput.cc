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
//	 XcodaMInput Class Implementation (Abstract Class)
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaMInput.cc,v $
//   Revision 1.3  2000/08/21 18:11:27  abbottd
//   Sun 5.0 C++ compiler fix
//
//   Revision 1.2  1998/04/08 17:29:04  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include "XcodaMInput.h"

XcodaMInput::XcodaMInput (Widget base)
{
  int i;

#ifdef _TRACE_OBJECT
  printf ("Create XcodaMInput Class Object\n");
#endif
  // initialize fd set
  for (i = 0; i < XCODA_MAX_FD; i++) {
    xfds_[i].fd = -1;
    xfds_[i].id = 0;
  }
  context_ = XtWidgetToApplicationContext (base);
  assert (context_);
}

XcodaMInput::~XcodaMInput (void)
{
  int i;

#ifdef _TRACE_OBJECT
  printf ("Create XcodaMInput Class Object\n");
#endif
  // remove all input
  for (i = 0; i < XCODA_MAX_FD; i++) {
    if (xfds_[i].fd != -1) {
      XtRemoveInput (xfds_[i].id);
      xfds_[i].fd = -1;
    }
  }
}

void
XcodaMInput::addInput (int fd, XtPointer mask)
{
  int i;

  if (fd >= 0) {
    // first check whether this fd is already here
    for (i = 0; i < XCODA_MAX_FD; i++) {
      if (xfds_[i].fd == fd)
	return;
    }
    // get first empty slot
    for (i = 0; i < XCODA_MAX_FD; i++) {
      if (xfds_[i].fd == -1)
	break;
    }
    xfds_[i].fd = fd;
    xfds_[i].id = XtAppAddInput (context_, fd, mask,
				 &XcodaMInput::inputCallback,
				 (XtPointer)this);
  }
}

void
XcodaMInput::removeInput (int fd)
{
  int i;

  if (fd >= 0){
    // find right slot
    for (i = 0; i < XCODA_MAX_FD; i++) {
      if (xfds_[i].fd == fd) {
	XtRemoveInput (xfds_[i].id);
	xfds_[i].fd = -1;
	return;
      }
    }
  }
}

void
XcodaMInput::inputCallback (XtPointer  clientData,
			    int*       fd,
			    XtInputId* id)
{
  XcodaMInput *obj = (XcodaMInput *)clientData;
  obj->input_callback (*fd);
}

