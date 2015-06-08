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
//      RunControl Preference Display Server Message
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcDisplayMsg.h,v $
//   Revision 1.1  1996/10/14 20:12:32  chen
//   init inport
//
//
//
#ifndef _RC_DISPLAY_MSG_H
#define _RC_DISPLAY_MSG_H

#include <rcMenuTogComd.h>

class rcMsgWindow;

class rcDisplayMsg: public rcMenuTogComd
{
public:
  // constructor and destructor
  rcDisplayMsg (char* name, int active,
		    char* acc, char* acc_text,
		    int state,
		    rcClientHandler& handler);
  ~rcDisplayMsg (void);

  // setup pointer to rcMsgWindow
  void msgWindow (rcMsgWindow* win);

  // class name
  virtual const char* className (void) const {return "rcDisplayMsg";}

protected:
  // inherited operations
  virtual void doit    (void);
  virtual void undoit  (void);

private:
  // message window
  rcMsgWindow* msgWin_;
};
#endif




  
