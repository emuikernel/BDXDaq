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
//      RunControl Mastership Toggle Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMastershipButton.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#ifndef _RC_MASTERSHIP_BUTTON_H
#define _RC_MASTERSHIP_BUTTON_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <codaXpmtbtnComd.h>
#include <rcClientHandler.h>
#include <rcPanel.h>

class XcodaXpm;

class rcMastershipButton: public codaXpmtbtnComd
{
public:
  // constructor and destructor
  rcMastershipButton (Widget parent, char* name,
		      rcClientHandler& handler);
  virtual ~rcMastershipButton (void);

  // create all widgets
  void    init            (void);
  const Widget baseWidget (void);

  // manage and unmanage
  void    manage          (void);
  void    unmanage        (void);

  // class name
  virtual const char* className (void) const {return "rcMastershipButton";}

protected:
  // inherited operation
  virtual void doit   (void);
  virtual void undoit (void);

private:
  // Widgets
  Widget parent_;
  Widget w_;
  // network handler
  rcClientHandler& netHandler_;
};
#endif
