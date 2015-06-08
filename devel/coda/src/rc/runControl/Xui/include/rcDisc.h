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
//      RunControl Dis-Connect Command
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcDisc.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#ifndef _RC_DISCONNECT_H
#define _RC_DISCONNECT_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcComdButton.h>

class rcButtonPanel;

class rcDiscDialog;

class rcDisc: public rcComdButton
{
public:
  rcDisc  (Widget parent, rcButtonPanel* panel, rcClientHandler& handler);
  ~rcDisc (void);

  // class name
  virtual const char* className (void) const {return "rcDisc";}
  
protected:
  // inherited functions
  virtual void doit   (void);
  virtual void undoit (void);

private:
  // dialog assoiciated with this command
  rcDiscDialog* dialog_;
};
#endif

