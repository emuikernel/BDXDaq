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
//      Network Component Boot Timer
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: netCompBootTimer.h,v $
//   Revision 1.1  1997/01/24 15:22:28  chen
//   component boot timer
//
//
//
#ifndef _CODA_NETCOMP_BOOT_TIMER_H
#define _CODA_NETCOMP_BOOT_TIMER_H

#ifndef _CODA_USE_THREADS

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <daqGlobal.h>
#include <reactorTimer.h>

class netComponent;

class netCompBootTimer : public reactorTimer
{
public:
  // constructor and destructor
  netCompBootTimer  (void);
  ~netCompBootTimer (void);
  
  void component (netComponent* comp);

  virtual void timer_callback (void);

private:
  netComponent*  comp_;
};

#endif

#endif


