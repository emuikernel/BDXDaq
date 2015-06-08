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
//	 Motif Error Dialog 
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaErrorDialog.h,v $
//   Revision 1.2  1998/04/08 17:28:53  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:57  chen
//   coda motif C++ library
//
//
#ifndef _XCODA_ERROR_DIALOG
#define _XCODA_ERROR_DIALOG

#include <XcodaMsgDialog.h>

class XcodaErrorDialog: public XcodaMsgDialog
{
 public:
  // constructor
  XcodaErrorDialog(Widget, char *, char *);
  virtual const char *className() const {return "XcodaErrorDialog";}

 protected:
  virtual Widget createDialog(Widget, char *);
};
#endif
  
    

