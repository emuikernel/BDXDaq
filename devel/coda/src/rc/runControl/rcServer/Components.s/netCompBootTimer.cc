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
//   $Log: netCompBootTimer.cc,v $
//   Revision 1.1  1997/01/24 15:22:07  chen
//   component boot timer
//
//
#ifndef _CODA_USE_THREADS

#include <netComponent.h>
#include "netCompBootTimer.h"

netCompBootTimer::netCompBootTimer (void)
:reactorTimer (*glbReactor), comp_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create netCompBootTimer Class Object\n");
#endif
}

netCompBootTimer::~netCompBootTimer (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete netCompBootTimer Class Object\n");
#endif
  // don't free pointer of transitioner
  comp_ = 0;
}

void
netCompBootTimer::timer_callback (void)
{
  if (comp_) 
    comp_->timerCallback ();
}

void
netCompBootTimer::component (netComponent* comp)
{
  comp_ = comp;
}


#endif
