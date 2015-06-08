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
//      Implementation of rcNetStatus Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcNetStatus.cc,v $
//   Revision 1.3  1998/04/08 18:31:25  heyes
//   new look and feel GUI
//
//   Revision 1.2  1997/08/25 16:00:36  heyes
//   fix some display problems
//
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#include <Xm/Frame.h>
#include "rcNetStatus.h"

const double RC_NET_XMIN = 0.0;
const double RC_NET_XMAX = 100.0;
const double RC_NET_YMIN = 0.0;
const double RC_NET_YMAX = 100.0;
// update interval in msec
const int    RC_NET_UPDATE_INTERVAL = 100;

const double RC_NET_DIST = (RC_NET_XMAX - RC_NET_XMIN)/10.0;

rcNetStatus::rcNetStatus (Widget parent, char* name,
			  Dimension width, Dimension height)
:armed_ (0), autoTimerId_ (0), counter_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create rcNetStatus Class Object\n");
#endif
  Arg arg[10];
  int ac = 0;

}

rcNetStatus::~rcNetStatus (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete rcNetStatus Class Object\n");
#endif
}

const Widget
rcNetStatus::baseWidget (void)
{
  return frame_;
}

void
rcNetStatus::start (void)
{

}

void
rcNetStatus::autoTimerCallback (XtPointer clientData, XtIntervalId* )
{
}

void
rcNetStatus::stop (void)
{
}

void
rcNetStatus::startShowingStatus (void)
{
}

void
rcNetStatus::showingInProgress (void)
{
}

void
rcNetStatus::stopShowingStatus (void)
{

}



