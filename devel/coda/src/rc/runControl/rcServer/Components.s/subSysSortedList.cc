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
//      Sorted List according to priority of the daqSubSystem in
//      decreasing order
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysSortedList.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#include <daqSubSystem.h>
#include "subSysSortedList.h"

subSysSortedList::subSysSortedList (void)
:codaSlist ()
{
#ifdef _TRACE_OBJECTS
  printf ("    Create subSysSortedList Class Object\n");
#endif
  // empty
}

subSysSortedList::~subSysSortedList (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete subSysSortedList Class Object\n");
#endif
  // empty
  // one cannot delete these pointers. They will be used somewhere else
}

void
subSysSortedList::add (void *sys)
{
  codaSlistIterator itr (*this);
  daqSubSystem *tsys = 0;
  daqSubSystem *val = (daqSubSystem *)sys;

  for (itr.init (); !itr; ++itr){
    tsys = (daqSubSystem *)itr ();
    if (*val > *tsys) { 
      // found a right position
      itr.addBefore (sys);
      return;
    }
  }
  // add to the end of the list
  itr.addBefore (sys);
}
