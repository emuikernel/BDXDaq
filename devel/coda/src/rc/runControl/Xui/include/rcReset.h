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
//      RunControl Reset Command Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcReset.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#ifndef _RC_RESET_H
#define _RC_RESET_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcComdButton.h>

class rcButtonPanel;

class rcResetDialog;

class rcReset: public rcComdButton
{
public:
  // constructor and destructor
  rcReset  (Widget parent, rcButtonPanel* panel, rcClientHandler& handler);
  ~rcReset (void);

  // operation of reset
  void reset (void);

  // class name
  virtual const char* className (void) const {return "rcReset";}

protected:
  // inherited functions
  virtual void doit   (void);
  virtual void undoit (void);

  // callback for prestart action
  static void resetCallback (int statuc, void* arg, daqNetData* data);

private:
  // dialog
  rcResetDialog* dialog_;
};
#endif


