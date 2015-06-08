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
//	 Implementation of Motif Error Dialog
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaErrorDialog.cc,v $
//   Revision 1.2  1998/04/08 17:28:52  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:25:01  chen
//   coda motif C++ library
//
//
#include <XcodaErrorDialog.h>

XcodaErrorDialog::XcodaErrorDialog(Widget parent,
				   char   *name,
				   char   *title)
:XcodaMsgDialog(parent, name, title)
{
  // empty
}

Widget XcodaErrorDialog::createDialog(Widget parent, char *name)
{
  Widget res = XmCreateErrorDialog(parent, name, NULL, 0);
  return res;
}
