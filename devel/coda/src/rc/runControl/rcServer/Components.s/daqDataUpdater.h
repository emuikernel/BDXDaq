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
//      Timer to update all values inside a data manager
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqDataUpdater.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:19  chen
//   run control source
//
//
#ifndef _CODA_DATA_UPDATER_H
#define _CODA_DATA_UPDATER_H

#include <daqCommon.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <daqGlobal.h>
#include <reactorTimer.h>

class daqDataManager;

class daqDataUpdater : public reactorTimer
{
public:
  // constructor and destructor
  daqDataUpdater  (void);
  ~daqDataUpdater (void);

  void dataManager (daqDataManager* manager);

  // operations
  int  updateInterval (void) const;
  void updateInterval (int msec);
  void startUpdating  (void);
  void stopUpdating   (void);

  virtual void timer_callback (void);

  virtual const char *className (void) const {return "daqDataUpdater";}

private:
  daqDataManager* manager_;
};
#endif
