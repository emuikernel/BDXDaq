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
//	 CODA Integer Prompt Dialog Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaIntPromptDialog.cc,v $
//   Revision 1.2  1998/04/08 17:29:01  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <XcodaIntPromptDialog.h>

XcodaIntPromptDialog::XcodaIntPromptDialog(Widget parent,
					   char *name,
					   char *title)
:XcodaPromptDialog(parent, name, title)
{
#ifdef _TARCE_OBJECTS
  printf("              Create XcodaIntPromptDialog Object\n");
#endif
}

XcodaIntPromptDialog::~XcodaIntPromptDialog()
{
#ifdef _TARCE_OBJECTS
  printf("              Delete XcodaIntPromptDialog Object\n");
#endif  
}

void XcodaIntPromptDialog::ok()
{
  //empty
}

void XcodaIntPromptDialog::cancel()
{
  //empty
}

int XcodaIntPromptDialog::checkSyntax()
{
  int value = 0;
  int st;

  if((st = sscanf(_result,"%d",&value)) < 1)
    return 0;
  else
    return 1;
}

int XcodaIntPromptDialog::outputResult()
{
  int value;

  if(checkSyntax()){
    sscanf(_result, "%d", &value);
    return value;
  }
  else
    return 0;
}

