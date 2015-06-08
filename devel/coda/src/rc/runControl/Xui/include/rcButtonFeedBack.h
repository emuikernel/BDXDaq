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
//      RunControl Preference Button Feedback 
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcButtonFeedBack.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#ifndef _RC_BUTTON_FEEDBACK_H
#define _RC_BUTTON_FEEDBACK_H

#include <rcMenuTogComd.h>

class rcHelpMsgWin;

class rcButtonFeedBack: public rcMenuTogComd
{
public:
  // constructor and destructor
  rcButtonFeedBack (char* name, int active,
		    char* acc, char* acc_text,
		    int state,
		    rcClientHandler& handler);
  ~rcButtonFeedBack (void);

  // setup pointer to rcHelpMsgWin
  void helpMsgWindow (rcHelpMsgWin* win);

  // class name
  virtual const char* className (void) const {return "rcButtonFeedBack";}

protected:
  // inherited operations
  virtual void doit    (void);
  virtual void undoit  (void);

private:
  // message window
  rcHelpMsgWin* msgWin_;
};
#endif




  
