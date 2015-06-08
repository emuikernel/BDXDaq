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
//      Composite Transitioner Class (Implementation)
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: compTransitioner.cc,v $
//   Revision 1.3  1998/06/02 19:51:48  heyes
//   fixed rcServer
//
//   Revision 1.2  1997/08/25 15:57:36  heyes
//   use dplite.h
//
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#include "transitioner.h"
#include "compTransitioner.h"

compTransitioner::compTransitioner (void)
:tranList_ (), trIte_ (tranList_)
{
#ifdef _TRACE_OBJECTS
  printf ("Create compTransitioner Class Object\n");
#endif
  trIte_.init ();
}

compTransitioner::~compTransitioner (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete compTransitioner Class Object\n");
#endif
  // empty
}

void
compTransitioner::pendTransitioner (transitioner* tr)
{
  if (!tranList_.isEmpty ()) {
    transitioner* current = (transitioner *) trIte_ ();
    trIte_.addAfter ((void *)tr);
    current->child (tr);
  }
  else
    trIte_.addAfter ((void *)tr);
  ++trIte_;
}

void
compTransitioner::addTransitioner (transitioner* tr)
{

  if (!tranList_.isEmpty ()) {
    transitioner* next = (transitioner *)tranList_.firstElement ();
    tr->child (next);
    tranList_.add ((void *)tr);
  }
  else 
    {
      tranList_.add ((void *)tr);
    }
}

void
compTransitioner::execute (void)
{
  if (!tranList_.isEmpty ()) {
    transitioner* first = (transitioner *)tranList_.firstElement ();
    first->execute();
  }
}

  
