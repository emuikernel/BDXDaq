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
//      Implementation of daqDataTrigger Class (Abstract Class)      
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqDataTrigger.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:30  chen
//   run control source
//
//
#include <daqData.h>
#include "daqDataTrigger.h"

daqDataTrigger::daqDataTrigger (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqDataTrigger Class Object\n");
#endif
  // empty
}

daqDataTrigger::~daqDataTrigger (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete daqDataTrigger Class Object\n");
#endif
}

void
daqDataTrigger::trigger (daqData* )
{
  // empty
}
