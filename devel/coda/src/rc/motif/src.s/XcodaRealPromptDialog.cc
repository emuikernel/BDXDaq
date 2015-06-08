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
//	 CODA Real Prompt Dialog Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaRealPromptDialog.cc,v $
//   Revision 1.2  1998/04/08 17:29:26  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <XcodaRealPromptDialog.h>

XcodaRealPromptDialog::XcodaRealPromptDialog(Widget parent,
					     char *name,
					     char *title)
:XcodaPromptDialog(parent, name, title)
{
#ifdef _TARCE_OBJECTS
  printf("              Create XcodaRealPromptDialog Object\n");
#endif
}

XcodaRealPromptDialog::~XcodaRealPromptDialog()
{
#ifdef _TARCE_OBJECTS
  printf("              Delete XcodaRealPromptDialog Object\n");
#endif  
}

void XcodaRealPromptDialog::ok()
{
  //empty
}

void XcodaRealPromptDialog::cancel()
{
  //empty
}

int XcodaRealPromptDialog::checkSyntax()
{
  char *p = _result;
  int  num_dots = 0;

  while(*p != '\0'){
    if(!isdigit(*p)){
      if(*p == '.'){
	num_dots++;
	if(num_dots > 1) // only allow '.' appear once
	  return 0;
	p++;
      }
      else
	return 0;
    }
    else
      p++;
  }
  return 1;
}

float XcodaRealPromptDialog::outputResult()
{
  float value;

  if(checkSyntax()){
    sscanf(_result, "%f", &value);
    return value;
  }
  else
    return 0.0;
}

