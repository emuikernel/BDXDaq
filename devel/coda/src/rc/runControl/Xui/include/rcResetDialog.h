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
//      RunControl Reset Dialog Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcResetDialog.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#ifndef _RC_RESET_DIALOG_H
#define _RC_RESET_DIALOG_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcClientHandler.h>
#include <XcodaWarningDialog.h>

class rcReset;

class rcResetDialog: public XcodaWarningDialog
{
public:
  // constructor and destructor
  rcResetDialog (rcReset* reset, char* name,
		 char* title,
		 rcClientHandler& handler);
  virtual ~rcResetDialog (void);

  // class name
  virtual const char* className (void) const {return "rcResetDialog";}

protected:
  // define ok
  virtual void ok (void);
  
private:
  // reset comamnd button
  rcReset* reset_;
  // netWork handler
  rcClientHandler& netHandler_;
};
#endif
