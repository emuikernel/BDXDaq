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
//	 CODA String Prompt Dialog Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaStringPromptDialog.cc,v $
//   Revision 1.2  1998/04/08 17:29:36  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <XcodaStringPromptDialog.h>

XcodaStringPromptDialog::XcodaStringPromptDialog(Widget parent,
						 char *name,
						 char *title)
:XcodaPromptDialog(parent, name, title)
{
#ifdef _TARCE_OBJECTS
  printf("              Create XcodaStringPromptDialog Object\n");
#endif
}

XcodaStringPromptDialog::~XcodaStringPromptDialog()
{
#ifdef _TARCE_OBJECTS
  printf("              Delete XcodaStringPromptDialog Object\n");
#endif  
}

void XcodaStringPromptDialog::ok()
{
  //empty
}

void XcodaStringPromptDialog::cancel()
{
  //empty
}

int XcodaStringPromptDialog::checkSyntax()
{
  return 1;
}

char *XcodaStringPromptDialog::outputResult()
{
  char *value;
  
  value = new char[::strlen(_result) + 1];
  ::strcpy(value, _result);
  return value;
}

