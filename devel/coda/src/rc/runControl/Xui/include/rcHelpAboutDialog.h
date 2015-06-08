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
//      RunControl Help Menu About Dialog
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcHelpAboutDialog.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#ifndef _RC_HELP_ABOUT_DIALOG_H
#define _RC_HELP_ABOUT_DIALOG_H

#include <stdio.h>
#include <string.h>
#include <XcodaFormDialog.h>


class XcodaXpmLabel;
class rcHelpAbout;

class rcHelpAboutDialog: public XcodaFormDialog
{
public:
  //constructor and destructor
  rcHelpAboutDialog  (rcHelpAbout* base, char* name, char* title);
  ~rcHelpAboutDialog (void);

  // class name
  virtual const char* className (void) const {return "rcHelpAboutDialog";}

protected:
  // inherited operations
  void    createFormChildren (void);
  // callback for pushbuttons
  static void okCallback (Widget, XtPointer, XmAnyCallbackStruct* );

private:
  rcHelpAbout* base_;
  XcodaXpmLabel* log_;
};
#endif

		     
