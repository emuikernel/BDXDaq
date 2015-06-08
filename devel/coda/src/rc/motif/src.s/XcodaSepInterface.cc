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
//	 Coda Separator Comd X Interface
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaSepInterface.cc,v $
//   Revision 1.2  1998/04/08 17:29:29  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <XcodaSepInterface.h>
#include <Xm/Separator.h>

XcodaSepInterface::XcodaSepInterface(Widget parent,
				     codaComd *cmd)
:codaComdXInterface(parent, cmd)
{
#ifdef _TRACE_OBJECTS
  printf("       Create XcodaSepInterface Object \n");
#endif
  // empty
}

void XcodaSepInterface::init()
{
  _w = XtCreateWidget(_name, xmSeparatorWidgetClass,
		      _parent, NULL, 0);
  if(_active)
    activate();
  else
    deactivate();

  installDestroyHandler();
  manage();
}

XcodaSepInterface::~XcodaSepInterface()
{
#ifdef _TRACE_OBJECTS
  printf("          Destroy XcodaSepInterface Object \n");
#endif
}  
