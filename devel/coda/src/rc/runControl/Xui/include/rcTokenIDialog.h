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
//      RunControl Token Interval Dialog
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcTokenIDialog.h,v $
//   Revision 1.2  1996/12/04 18:32:34  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#ifndef _RC_TOKEN_INTERVAL_DIALOG_H
#define _RC_TOKEN_INTERVAL_DIALOG_H

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcClientHandler.h>
#include <XcodaIntPromptDialog.h>

class rcTokenIButton;

class rcTokenIDialog: public XcodaIntPromptDialog
{
public:
  // constructor and destructor
  rcTokenIDialog (rcTokenIButton* button, char* name, char* title,
		  rcClientHandler& handler);
  virtual ~rcTokenIDialog (void);

  // set update interval
  void    setTokenInterval (int interval);
  
  // class name
  virtual const char* className (void) const {return "rcTokenIDialog";}

protected:
  // inherited operation
  virtual void ok     (void);
  virtual void cancel (void);
  virtual int  checkSyntax (void);

private:
  rcTokenIButton* button_;
  // network handler
  rcClientHandler& netHandler_;
};

#endif /* coda_2_0 */

#endif
