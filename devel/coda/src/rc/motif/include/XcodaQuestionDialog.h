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
//	 Motif Question Dialog 
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaQuestionDialog.h,v $
//   Revision 1.2  1998/04/08 17:29:24  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:58  chen
//   coda motif C++ library
//
//
#ifndef _XCODA_QUESTION_DIALOG_H
#define _XCODA_QUESTION_DIALOG_H

#include <XcodaMsgDialog.h>

class XcodaQuestionDialog: public XcodaMsgDialog
{
 public:
  // constructor
  XcodaQuestionDialog(Widget, char *, char *);
  virtual const char *className() const {return "XcodaQuestionDialog";}

 protected:
  virtual Widget createDialog(Widget, char *);
};
#endif
  
    

