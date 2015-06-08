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
//      RunControl Update Interval Command Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcUpdateInterval.h,v $
//   Revision 1.2  1998/05/28 17:47:06  heyes
//   new GUI look
//
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#ifndef _RC_UPDATE_INTERVAL_H
#define _RC_UPDATE_INTERVAL_H

#include <rcMenuComd.h>
#include <rcClientHandler.h>

class rcNetStatus;
class rcInfoPanel;
class rcUpdateIntervalDialog;

class rcUpdateInterval: public rcMenuComd
{
public:
  rcUpdateInterval (char* name, int active,
		    char* acc, char* acc_text, rcClientHandler& handler);
  virtual ~rcUpdateInterval (void);

  // set update interval value
  void setUpdateInterval (int interval);
  int  updateInterval    (void) const;
  void sendUpdateInterval (int interval);

  // class name
  virtual const char *className (void) const {return "rcUpdateInterval";}
  virtual void doit   (void);

protected:
  virtual void undoit (void);

  // callbacks for setting new update interval
  static void simpleCallback (int status, void* arg, daqNetData* data);

private:
  // dialog
  rcUpdateIntervalDialog* dialog_;
  // update interval cached value
  int interval_;
};
#endif
