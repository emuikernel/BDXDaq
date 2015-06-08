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
//      daqDataUpdater Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqDataUpdater.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:19  chen
//   run control source
//
//
#include <daqDataManager.h>
#include "daqDataUpdater.h"

daqDataUpdater::daqDataUpdater (void)
:reactorTimer (*glbReactor), manager_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Crete daqDataUpdater Class Object\n");
#endif
  // empty
}

daqDataUpdater::~daqDataUpdater (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete daqDataUpdater Class Object\n");
#endif
  // empty
}

void
daqDataUpdater::dataManager (daqDataManager* manager)
{
  manager_ = manager;
  assert (manager_);
}

void
daqDataUpdater::timer_callback (void)
{
  manager_->update ();
}

int
daqDataUpdater::updateInterval (void) const
{
  if (auto_armed ())
    return interval ();
  else
    return 0;
}

void
daqDataUpdater::updateInterval (int msec)
{
  interval (msec);
}

void
daqDataUpdater::startUpdating (void)
{
  auto_arm ();
}

void
daqDataUpdater::stopUpdating (void)
{
  dis_arm ();
}
