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
//      RunControl Exit Command
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcExit.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#ifndef _RC_EXIT_H
#define _RC_EXIT_H

#include <rcClientHandler.h>
#include <rcMenuComd.h>

class rcExitDialog;

class rcExit: public rcMenuComd
{
public:
  rcExit (char* name, int active,
	  char* acc, char* acc_text, rcClientHandler& handler);
  virtual ~rcExit (void);

  // really command to end runcontrol
  void   exitRunControl (void);

  virtual const char *className (void) const {return "rcExit";}

protected:
  virtual void doit   (void);
  virtual void undoit (void);

  // timer callback for exit function
  static void timerCallback (XtPointer, XtIntervalId *);

private:
  // dialog
  rcExitDialog* dialog_;
  // Timer stuff
  XtIntervalId exitTimerId_;
};
#endif
