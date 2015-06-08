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
//      RunControl Token Interval Command Button (CODA_2_0)
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcTokenIButton.h,v $
//   Revision 1.2  1996/12/04 18:32:33  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#ifndef _RC_TOKEN_IBUTTON_H
#define _RC_TOKEN_IBUTTON_H

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)

#include <rcMenuComd.h>
#include <rcClientHandler.h>

class rcNetStatus;
class rcInfoPanel;
class rcTokenIDialog;

class rcTokenIButton: public rcMenuComd
{
public:
  rcTokenIButton (char* name, int active,
		  char* acc, char* acc_text, rcClientHandler& handler);
  virtual ~rcTokenIButton (void);

  // set update interval value
  void setTokenInterval   (int interval);
  int  tokenInterval      (void) const;
  void sendTokenInterval  (int interval);

  // class name
  virtual const char *className (void) const {return "rcTokenIButton";}

protected:
  virtual void doit   (void);
  virtual void undoit (void);

  // callbacks for setting new token interval
  static void setTICallback (int status, void* arg, daqNetData* data);

private:
  // dialog
  rcTokenIDialog* dialog_;
  // token interval cached value
  int interval_;
};

#endif /* coda_2_0 */

#endif
