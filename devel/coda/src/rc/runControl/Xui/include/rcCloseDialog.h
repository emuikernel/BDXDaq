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
//      RunControl Close Dialog
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcCloseDialog.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#ifndef _RC_CLOSE_DIALOG_H
#define _RC_CLOSE_DIALOG_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcClientHandler.h>
#include <XcodaWarningDialog.h>

class rcClose;

class rcCloseDialog: public XcodaWarningDialog
{
public:
  // constructor and destructor
  rcCloseDialog (rcClose* close, char* name,
                 char* title,
                 rcClientHandler& handler);
  virtual ~rcCloseDialog (void);

  // class name
  virtual const char* className (void) const {return "rcCloseDialog";}

protected:
  // define ok function
  virtual void ok (void);

private:
  // close menu button
  rcClose* close_;
  // network handler
  rcClientHandler& netHandler_;
};
#endif

  
