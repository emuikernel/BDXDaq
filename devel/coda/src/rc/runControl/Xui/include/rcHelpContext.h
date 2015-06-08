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
//      RunControl Help Context
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcHelpContext.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#ifndef _RC_HELP_CONTEXT_H
#define _RC_HELP_CONTEXT_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <codaPbtnComd.h>

class rcHelpOverview;
class rcHelpTextW;

class rcHelpContext: public codaPbtnComd
{
public:
  // constructor and destructor
  rcHelpContext (char* name, int active, int index,
		 rcHelpOverview* parent);
  ~rcHelpContext (void);

  // class name
  virtual const char* className (void) const {return "rcHelpContext";}

protected:
  virtual void doit (void);
  virtual void undoit (void);

  // display help text
  void displayHelpText (char* text);
  // process text
  static void processText (char* helptext, char* disptext);

private:
  // parent button
  rcHelpOverview* parent_;
  // index to help documents
  int index_;
  // help documents
  static char* helpdoc_[];
  // help text display window
  static rcHelpTextW*  helpDisp_;
};
#endif

