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
//      RunControl Close Command
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcClose.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#ifndef _RC_CLOSE_H
#define _RC_CLOSE_H

#include <rcMenuComd.h>
#include <rcClientHandler.h>

class rcNetStatus;
class rcInfoPanel;
class rcCloseDialog;

class rcClose: public rcMenuComd
{
public:
  rcClose (char* name, int active,
	   char* acc, char* acc_text, rcClientHandler& handler);
  virtual ~rcClose (void);

  // really command to close runcontrol
  void   close (void);

  virtual const char *className (void) const {return "rcClose";}

protected:
  virtual void doit   (void);
  virtual void undoit (void);

private:
  // dialog
  rcCloseDialog* dialog_;
};
#endif
