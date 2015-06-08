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
//	 CODA String Prompt Dialog Box
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaStringPromptDialog.h,v $
//   Revision 1.2  1998/04/08 17:29:37  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:59  chen
//   coda motif C++ library
//
//
#ifndef _XCODA_STR_PROMPT_DIA_H
#define _XCODA_STR_PROMPT_DIA_H

#include <XcodaPromptDialog.h>

class XcodaStringPromptDialog: public XcodaPromptDialog
{
 public:
  // constructor and destrcutor
  XcodaStringPromptDialog(Widget parent,
		       char *name, 
		       char *title);
  virtual ~XcodaStringPromptDialog();
  char    *outputResult(); // user need to free memory
  virtual const char *className() const {return "XcodaStringPromptDialog";}
  
 protected:
  virtual void ok();
  virtual void cancel();
  virtual int  checkSyntax();
};
#endif
