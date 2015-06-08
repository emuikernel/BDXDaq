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
//	 X Window Reactor Class for I/O
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: X_Reactor.C,v $
//   Revision 1.1.1.1  1996/10/10 20:04:47  chen
//   simple ACE version 3.1
//
//
#include "X_Reactor.h"

void 
X_Reactor::inputCallback (XtPointer client_data, int *source, XtInputId *id)
{
  X_Reactor *obj = (X_Reactor *)client_data;

  obj->notify_handle (*source);
}

#ifndef __INLINE__
#include "X_Reactor.i"
#endif
