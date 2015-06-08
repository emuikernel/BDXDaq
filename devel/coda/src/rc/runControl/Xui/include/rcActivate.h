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
//      RunControl Go Command Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcActivate.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#ifndef _RC_ACTIVATE_H
#define _RC_ACTIVATE_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcComdButton.h>

class rcButtonPanel;

class rcActivate: public rcComdButton
{
public:
  // constructor and destructor
  rcActivate  (Widget parent, rcButtonPanel* panel, rcClientHandler& handler);
  ~rcActivate (void);

  // class name
  virtual const char* className (void) const {return "rcActivate";}

protected:
  // inherited functions
  virtual void doit   (void);
  virtual void undoit (void);

  // callback for prestart action
  static void activateCallback (int statuc, void* arg, daqNetData* data);

};
#endif


