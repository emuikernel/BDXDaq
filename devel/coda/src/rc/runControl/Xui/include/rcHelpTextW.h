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
//      RunControl Simple Help Text Window
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcHelpTextW.h,v $
//   Revision 1.2  1998/04/08 18:31:18  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#ifndef _RC_HELP_TEXT_W_H
#define _RC_HELP_TEXT_W_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <XcodaFormDialog.h>

class rcHelpTextW
{
public:
  // constructor and destructor
  rcHelpTextW (Widget parent, char* name, char* title);
  ~rcHelpTextW (void);

  // set text
  void setHelpText (char* text);

private:
  Widget textw_;
};
#endif
