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
//      Sorted List according to priority of the daqComponent in
//      decreasing order
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqCompSortedList.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#include <daqComponent.h>
#include "daqCompSortedList.h"

daqCompSortedList::daqCompSortedList (void)
:codaSlist ()
{
#ifdef _TRACE_OBJECTS
  printf ("    Create daqCopSortedList Class Object\n");
#endif
  // empty
}

daqCompSortedList::~daqCompSortedList (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete daqCopSortedList Class Object\n");
#endif
  // empty
}

void
daqCompSortedList::add (void *comp)
{
  codaSlistIterator itr (*this);
  daqComponent *tcomp = 0;
  daqComponent *val = (daqComponent *)comp;

  for (itr.init (); !itr; ++itr){
    tcomp = (daqComponent *)itr ();
    if (*val > *tcomp) { 
      // found a right position
      itr.addBefore (comp);
      return;
    }
  } 
  // add to the end of the list
  itr.addBefore (comp);
  //codaSlist::add (comp);
}
