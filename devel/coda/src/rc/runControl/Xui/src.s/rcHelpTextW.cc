//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      Implementation of rcHelpTextW class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcHelpTextW.cc,v $
//   Revision 1.3  1998/05/28 17:46:56  heyes
//   new GUI look
//
//   Revision 1.2  1998/04/08 18:31:17  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#include "rcHelpTextW.h"

extern "C" void loadAndOrJump(char *file, char *loc, Boolean store);
#ifndef Darwin
extern "C" char *getenv(char *env);
#endif

rcHelpTextW::rcHelpTextW (Widget parent, char* name,
			  char* title)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcHelpTextW Class Object\n");
#endif
  // empty
}

rcHelpTextW::~rcHelpTextW (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcHelpTextW Class Object\n");
#endif
  // empty
}

void
rcHelpTextW::setHelpText (char* text)
{
  char *dollar_coda, file[1024];

  dollar_coda = getenv("CODA");

  if (dollar_coda) {
    sprintf(file,"%s/common/html/rc/contextHelp.html", dollar_coda);
  }
  loadAndOrJump(file,NULL,1);
  loadAndOrJump(NULL,text,1);
}

