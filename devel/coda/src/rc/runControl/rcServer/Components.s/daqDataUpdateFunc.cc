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
//      Implementation of various DAQ data updating mechanism
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqDataUpdateFunc.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:19  chen
//   run control source
//
//
#include <daqData.h>
#include "daqDataUpdateFunc.h"
#include <time.h>
#include <codaCompClnt.h>

//==================================================================
//         Update Server Time (currTime)
//==================================================================
void
serverTime (char* compname, char* attrname, void* buffer, int count)
{
  time_t tv = ::time (0);
  ::strncpy ((char *)buffer, ::ctime (&tv), CODA_CONV_LEN);
}

void
serverTimeBin (char* compname, char* attrname, void* buffer, int count)
{
  time_t tv = ::time (0);
  long *temp = (long *)buffer;
  *temp = tv;
}

void
compAttr (char* compname, char* attrname, void* buffer, int count)
{
  codaDaReadInt (compname, attrname, (long *)buffer);
}

  
