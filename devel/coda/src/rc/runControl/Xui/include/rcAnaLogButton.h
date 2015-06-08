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
//      RunControl Ana Log Option Command
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcAnaLogButton.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#ifndef _RC_ANALOG_BUTTON_H
#define _RC_ANALOG_BUTTON_H

#include <rcMenuComd.h>
#include <rcClientHandler.h>

class rcNetStatus;
class rcInfoPanel;
class rcAnaLogDialog;

class rcAnaLogButton: public rcMenuComd
{
public:
  rcAnaLogButton (char* name, int active,
		  char* acc, char* acc_text, rcClientHandler& handler);
  virtual ~rcAnaLogButton (void);

  // class name
  virtual const char *className (void) const {return "rcAnaLogButton";}

protected:
  virtual void doit   (void);
  virtual void undoit (void);

private:
  // dialog
  rcAnaLogDialog* dialog_;
};
#endif
