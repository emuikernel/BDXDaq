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
//      Data Acquition Component (Abstract Class)
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqComponent.cc,v $
//   Revision 1.5  1997/09/05 12:04:00  heyes
//   almost final
//
//   Revision 1.4  1997/07/22 19:38:53  heyes
//   cleaned up lots of things
//
//   Revision 1.3  1996/11/05 17:37:46  chen
//   bootable flag is added to daqComponent
//
//   Revision 1.2  1996/11/04 16:13:46  chen
//   add options for monitoring components
//
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#include "daqComponent.h"
#ifndef Darwin
extern "C" char *getenv(char *);
#endif

daqComponent::daqComponent (char *title)
:daqTarget (title), priority_ (0), enabled_ (0), autoBoot_ (0), 
 monitored_ (1), bootable_ (1)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create daqComponent Class Object\n");
#endif
  // empty

  if (getenv("AUTOBOOT")) {
    autoBoot_ = 1;
    printf("autoboot enabled for %s\n",title);
  }
}

daqComponent::~daqComponent (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete daqComponent Class Object\n");
#endif
  // empty
}

void
daqComponent::abort (int wanted)
{
  // inherited from daqTarget
  status_ = CODA_SUCCESS;
  setState (wanted);
}

void
daqComponent::enable (void)
{
  status_ = CODA_SUCCESS;
  enabled_ = 1;
}

void
daqComponent::disable (void)
{

  status_ = CODA_SUCCESS;
  enabled_ = 0;
}

int
daqComponent::enabled (void) const
{
  return enabled_;
}

void
daqComponent::cancelTransition (void)
{
  setState (prevState_);
}

void
daqComponent::priority (int pri)
{
  priority_ = pri;
  status_ = CODA_SUCCESS;
}

int
daqComponent::priority (void) const
{
  return priority_;
}

void
daqComponent::enableAutoBoot (void)
{
  autoBoot_ = 1;
}

void
daqComponent::disableAutoBoot (void)
{
  autoBoot_ = 0;
}

int
daqComponent::autoBoot (void) const
{
  return autoBoot_;
}

int
daqComponent::bootable (void) const
{
  return bootable_;
}

void
daqComponent::monitorOn (void)
{
  monitored_ = 1;
}

void
daqComponent::monitorOff (void)
{
  monitored_ = 0;
}

int
daqComponent::monitored (void) const
{
  return monitored_;
}

int
daqComponent::compare (const daqComponent& comp) const
{
  if (priority_ == comp.priority_)
    return 0;
  else if (priority_ < comp.priority_)
    return -1;
  else
    return 1;
}

int operator < (const daqComponent& left, const daqComponent& right)
{
  return left.compare (right) < 0;
}

int operator <= (const daqComponent& left, const daqComponent& right)
{
  return left.compare (right) <= 0;
}

int operator > (const daqComponent& left, const daqComponent& right)
{
  return left.compare (right) > 0;
}

int operator >= (const daqComponent& left, const daqComponent& right)
{
  return left.compare (right) >= 0;
}

int operator != (const daqComponent& left, const daqComponent& right)
{
  return left.compare (right) != 0;
}

int operator == (const daqComponent& left, const daqComponent& right)
{
  return left.compare (right) == 0;
}
