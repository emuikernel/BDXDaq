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
//      Target class 
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: target.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:17  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "target.h"

target::target (char *title)
{
#ifdef _TRACE_OBJECTS
  printf ("Create target class object\n");
#endif
  title_ = new char [::strlen (title) + 1];
  ::strcpy (title_, title);
}

target::~target (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete target class object\n");
#endif
  if (title_)
    delete []title_;
  title_ = 0;
}

void
target::title (char *title)
{
  if (title_)
    delete []title_;
  title_ = new char[::strlen (title) + 1];
  ::strcpy (title_, title);
}

char *
target::title (void) const
{
  return title_;
}
