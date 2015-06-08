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
//   $Log: rcOnLine.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#ifndef _RC_RCONLINE_H
#define _RC_RCONLINE_H

#include <rcMenuTogComd.h>

class rcOnLine: public rcMenuTogComd
{
public:
  // constructor and destructor
  rcOnLine (char* name, int active,
	    char* acc, char* acc_text,
	    int state,
	    rcClientHandler& handler);
  ~rcOnLine (void);

  // class name
  virtual const char* className (void) const {return "rcOnLine";}

protected:
  // inherited operations
  virtual void doit    (void);
  virtual void undoit  (void);
  // callbacks for commands
  static void onlineCallback  (int status, void* arg, daqNetData* data);
  static void offlineCallback (int status, void* arg, daqNetData* data);
};
#endif




  
