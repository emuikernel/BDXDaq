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
//      CODA RunControl data variable RCS event limit writer
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcsEvLimitWriter.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:20  chen
//   run control source
//
//
#include <daqRun.h>
#include "rcsEvLimitWriter.h"

rcsEvLimitWriter::rcsEvLimitWriter (daqRun* r)
:daqDataWriter (), run_ (r)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcsEvLimitWriter Class Object\n");
#endif
  // empty
}

rcsEvLimitWriter::~rcsEvLimitWriter (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete rcsEvLimitWriter Class Object\n");
#endif
  // empty
}

void
rcsEvLimitWriter::write (daqData* data)
{
  run_->updateEventLimit ((int)(*data));
}
