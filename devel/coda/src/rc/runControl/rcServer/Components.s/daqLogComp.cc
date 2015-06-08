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
//      CODA LOG Component (1.4 Only)
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqLogComp.cc,v $
//   Revision 1.1  1997/01/24 16:36:50  chen
//   add log component for 1.4
//
//
//
#if !defined (_CODA_2_0_T) && !defined (_CODA_2_0)

#include "daqLogComp.h"
#include <codaCompClnt.h>
#include <rcMsgReporter.h>
#include <daqRun.h>

char* daqLogComp::zapmessage = "Zap!!!\n";
char* daqLogComp::resetmessage = "********** System reset **********\n";
char* daqLogComp::initmessage = "CODA System Version 1.4 On line\n";

int   daqLogComp::msglogger = 0;

daqLogComp::daqLogComp (char* title, int number, int expid, char* type,
			char* node, char* bootString,
			daqSubSystem& subsys)
:netComponent (title, number, expid, type, node, bootString, subsys)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create daqLogComp class object\n");
#endif
  // empty
}

daqLogComp::~daqLogComp (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete daqLogComp class object\n");
#endif
  // empty
}

int
daqLogComp::boot (void)
{
  status_ = CODA_SUCCESS;
  if (!daqLogComp::msglogger) {
#ifdef _CODA_DEBUG
    if (bootString_) 
      printf ("Component number %d type %s node %s and bootString %s\n",
	      number_, type_, node_, bootString_);
    else
      printf ("Component number %d type %s node %s\n",
	      number_, type_, node_);
#endif
    reporter->cmsglog (CMSGLOG_INFO,"Connecting to %s on host %s\n",title_,node_);

    // create bootString
    char msg[256];
    ::sprintf (msg, "%s %s %d %s %s",
	       bootString_, title_, number_, type_, node_);

    int errcode = ::system (msg);
    if (errcode == 0) {
      if (daLogOpen (node_, type_, 0) == 0) 
	reporter->cmsglog (CMSGLOG_INFO1,"%s booted ok\n", title_);
      else {
	reporter->cmsglog (CMSGLOG_INFO1, "%s booting underway....\n", title_);
	status_ = CODA_WARNING;
	waitForConnection ();
      }
    }
    else {
      reporter->cmsglog (CMSGLOG_INFO1,"%s boot Shell Script status: %d\n", 
			 title_, errcode);
      setState (CODA_DORMANT);
      status_ = CODA_ERROR;
    }

    if (status_ == CODA_SUCCESS) {
      daqLogComp::msglogger = 1;
      setState (CODA_BOOTED);
      reporter->cmsglog (CMSGLOG_INFO,"booted ok\n");
      //daLogMsg (daqLogComp::initmessage);
    }
    else if (status_ == CODA_WARNING) {
      setState (CODA_DORMANT);
      setOverrideState (CODA_DORMANT);
    }
    else {
      setState (CODA_DORMANT);
      reporter->cmsglog (CMSGLOG_ERROR,"boot failed !!!\n");
      setOverrideState (CODA_DORMANT);
    }
  }
  else // if connected
    setState (CODA_BOOTED);
  
  return CODA_SUCCESS;
}


int
daqLogComp::terminate (void)
{
  if (daqLogComp::msglogger) {
    status_ = CODA_SUCCESS;
    reporter->cmsglog (CMSGLOG_INFO,"%s terminate ......\n", title_);

    // real zap function
    //daLogMsg (daqLogComp::zapmessage);
    
    reporter->cmsglog (CMSGLOG_INFO,"%s terminated ok\n", title_);
  }
  setState (CODA_DORMANT);
  daqLogComp::msglogger = 0;
  status_ = CODA_SUCCESS;
  return CODA_SUCCESS;
}

int
daqLogComp::status (void) const
{
  return status_;
}

int
daqLogComp::configure (void)
{
  status_ = CODA_SUCCESS;
  // remove all the old information

  config_ = 0;
  // get configuration information
  daqRun* run = subsys_.system().run();
  if (run->getNetConfigInfo (title_, config_) != CODA_SUCCESS)
    fprintf (stderr, "Something is fishy..........\n");

  setState (CODA_CONFIGURED);
  
  return status_;
}


int
daqLogComp::download (void)
{
  status_ = CODA_SUCCESS;

  reporter->cmsglog (CMSGLOG_INFO,"%s downloaded ok\n", title_);
  setState (CODA_DOWNLOADED);
  return CODA_SUCCESS;
}

int
daqLogComp::go (void)
{
  status_ = CODA_SUCCESS;
  reporter->cmsglog (CMSGLOG_INFO,"%s activated ok\n", title_);
  setState (CODA_ACTIVE);

  return CODA_SUCCESS;
}

int
daqLogComp::pause (void)
{
  status_ = CODA_SUCCESS;

  reporter->cmsglog (CMSGLOG_INFO,"%s paused ok\n", title_);
  setState (CODA_PAUSED);
  return CODA_SUCCESS;
}  

int
daqLogComp::prestart (void)
{
  status_ = CODA_SUCCESS;

  reporter->cmsglog (CMSGLOG_INFO,"%s prestarted ok\n", title_);
  setState (CODA_PAUSED);
  return CODA_SUCCESS;
}

int
daqLogComp::reset (void)
{
  if (daqLogComp::msglogger)
    /*daLogMsg (daqLogComp::resetmessage)*/;
  setState (CODA_DORMANT);
  status_ = CODA_SUCCESS;
  return CODA_SUCCESS;
}

int
daqLogComp::end (void)
{
  status_ = CODA_SUCCESS;

  reporter->cmsglog (CMSGLOG_INFO,"%s ended ok\n", title_);
  setState (CODA_DOWNLOADED);
  return CODA_SUCCESS;
}

int
daqLogComp::verify (void)
{
  return CODA_SUCCESS;
}

int
daqLogComp::state (void)
{
#ifdef _CODA_DEBUG
  printf ("Log component %s has state %d\n", title_, state_);
#endif
  return state_;
}

int
daqLogComp::state2 (void)
{
#ifdef _CODA_DEBUG
  printf ("Log component %s has state %d\n", title_, state_);
#endif
  return state_;
}

int
daqLogComp::waitForConnection (void)
{
  timerCount_ = 0;
  bootTimer_->auto_arm (netComponent::tickInterval);
  return 1;
}

void
daqLogComp::timerCallback (void)
{
  int openst = 0;
  int counterLimit = (netComponent::timeoutLimit*1000)/(netComponent::tickInterval);

  if (timerCount_ < counterLimit) {
    openst = daLogOpen (node_, type_, 0);
    if (openst == 0)
      status_ = CODA_BOOTED;
    else
      status_ = CODA_DORMANT;

    transformState (CODA_BOOTED, CODA_BOOTED, status_);
    if (status_ == CODA_SUCCESS) {
      daqLogComp::msglogger = 1;
      reporter->cmsglog (CMSGLOG_INFO,"%s booted ok\n", title_);
      bootTimer_->dis_arm ();
      timerCount_ = 0;
      //daLogMsg (daqLogComp::initmessage);
    }
    else if ((timerCount_ % 8) == 0) {
      reporter->cmsglog (CMSGLOG_INFO,"%s still booting......\n", title_);
      timerCount_ ++;
    }
    else 
      timerCount_ ++;
  }
  else {
    reporter->cmsglog (CMSGLOG_ERROR,"%s boot timeout !!!\n", title_);
    status_ = CODA_ERROR;
    // timer is canceld inside cancel transition
    cancelTransition ();
    // change logger flag
    daqLogComp::msglogger = 0;
  }
}  

#endif
