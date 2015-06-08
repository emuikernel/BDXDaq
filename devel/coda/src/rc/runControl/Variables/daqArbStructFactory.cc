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
//      Implementation of daqArbStructFactory Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqArbStructFactory.cc,v $
//   Revision 1.2  1996/11/04 16:14:50  chen
//   add options for monitoring components
//
//   Revision 1.1.1.1  1996/10/11 13:39:30  chen
//   run control source
//
//
#include <daqCompBootStruct.h>
#include <daqRunTypeStruct.h>
#include <daqMonitorStruct.h>

#include "daqArbStructFactory.h"

daqArbStructFactory::daqArbStructFactory (void)
{
#ifdef _TRACE_OBJECTS
  printf ("create daqArbStructFactory Class Object\n");
#endif
}

daqArbStructFactory::~daqArbStructFactory (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete daqArbStructFactory Class Object\n");
#endif
}

daqArbStruct *
daqArbStructFactory::create (long id)
{
  if (id == daqArbStructFactory::BOOT_INFO)
    return new daqCompBootStruct ();
  else if (id == daqArbStructFactory::RUNTYPE_INFO)
    return new daqRunTypeStruct ();
  else if (id == daqArbStructFactory::MONITOR_INFO)
    return new daqMonitorStruct ();
  else
    return 0;
}
