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
//      Implementation of rcPanel Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcPanel.cc,v $
//   Revision 1.3  1998/05/28 17:47:01  heyes
//   new GUI look
//
//   Revision 1.2  1996/10/31 15:57:31  chen
//   Add server message to database option
//
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#include <daqNetData.h>
#include "rcPanel.h"

rcPanel::rcPanel (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Create rcPanel Class Object\n");
#endif
  // empty
}

rcPanel::~rcPanel (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete rcPanel Class Object\n");
#endif
  // empty
}

void
rcPanel::configMastership (int type)
{
  // empty
}

void
rcPanel::configOnlineOption (int option)
{
  // empty
}

void
rcPanel::configBoot ()
{
  // empty
}

void
rcPanel::configMonParms ()
{
  // empty
}

void
rcPanel::configUpdateInterval (int interval)
{
  // empty
}

void
rcPanel::anaLogChanged (daqNetData* info, int added)
{
  // empty
}

void
rcPanel::clientsConnectionConfig (char** clients, int numClients)
{
  // empty
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
void
rcPanel::configRcsMsgToDbase (int)
{
  // empty
}

void
rcPanel::configTokenInterval (int )
{
  // empty
}
#endif


