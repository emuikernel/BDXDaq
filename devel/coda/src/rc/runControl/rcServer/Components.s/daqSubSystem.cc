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
//      Data Acquisition Subsystem
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqSubSystem.cc,v $
//   Revision 1.4  1997/07/22 19:38:57  heyes
//   cleaned up lots of things
//
//   Revision 1.3  1996/10/22 17:17:17  chen
//   fix bugs of boot stage different state among components
//
//   Revision 1.2  1996/10/14 20:02:47  heyes
//   changed message system
//
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#include <subSysActivater.h>
#include <subSysAborter.h>
#include <subSysBooter.h>
#include <subSysConfigurer.h>
#include <subSysDownloader.h>
#include <subSysEnder.h>
#include <subSysPrestarter.h>
#include <subSysPauser.h>
#include <subSysResetter.h>
#include <subSysTerminater.h>
#include <subSysVerifier.h>
#include <rcMsgReporter.h>
#include <daqState.h>
#include <daqRun.h>
#include "daqSubSystem.h"

daqSubSystem::daqSubSystem (char *className, daqSystem& system)
:daqComponent (className), system_ (system), compList_ ()
{
#ifdef _TRACE_OBJECTS
  printf ("              Create daqSubSystem Class Object\n");
#endif
  // add this subsystem into system
  system_.addSubSystem (this);
  // initialize all those transitioners
  aborter_ = 0;
  activater_ = 0;
  booter_ = 0;
  configurer_ = 0;
  downloader_ = 0;
  ender_ = 0;
  prestarter_ = 0;
  pauser_ = 0;
  resetter_ = 0;
  terminater_ = 0;
  verifier_ = 0;
  currTransitioner_ = 0;
}

daqSubSystem::~daqSubSystem (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete daqSubSystem Class Object\n");
#endif
  // destroy all daqComponents here
  codaSlistIterator ite (compList_);
  daqComponent *comp = 0;
  
  for (ite.init (); !ite; ++ite) {
    comp = (daqComponent *)ite ();
    delete comp;
  }
  // remove this subSystem from the system
  if (!system_.subsysLocked_)
    system_.removeSubSystem (this);
  // remove all those transitioners
  if (aborter_)
    delete aborter_;
  if (activater_)
    delete activater_;
  if (booter_)
    delete booter_;
  if (configurer_)
    delete configurer_;
  if (downloader_)
    delete downloader_;
  if (ender_)
    delete ender_;
  if (prestarter_)
    delete prestarter_;
  if (pauser_)
    delete pauser_;
  if (resetter_)
    delete resetter_;
  if (terminater_)
    delete terminater_;
  if (verifier_)
    delete verifier_;
  currTransitioner_ = 0;
}

int
daqSubSystem::addComponent (daqComponent* comp)
{
  if (compList_.includes ((void *)comp))
    return CODA_WARNING;
  else 
    compList_.add ( (void *)comp);
  return CODA_SUCCESS;
}

int
daqSubSystem::removeComponent (daqComponent* comp)
{
  if (compList_.remove ((void *)comp) == 0)
    return CODA_WARNING;
  else
    return CODA_SUCCESS;
}

daqComponent*
daqSubSystem::firstComponent (void)
{
  if (compList_.isEmpty ())
    return 0;
  else
    return (daqComponent *)(compList_.firstElement ());
}

daqComponent*
daqSubSystem::firstEnabledComp (void)
{
  codaSlistIterator ite (compList_);
  daqComponent *comp = 0;

  int found = 0;
  for (ite.init (); !ite; ++ite) {
    comp = (daqComponent *)ite ();
    if (comp->enabled ())
      return comp;
  }
  return 0;
}

int
daqSubSystem::removeComponent (char *compName)
{
  codaSlistIterator ite (compList_);
  daqComponent *comp = 0;

  int found = 0;
  for (ite.init (); !ite; ++ite) {
    comp = (daqComponent *)ite ();
    if (::strcmp (comp->title(), compName) == 0) {
      found = 1;
      break;
    }
  }
  if (found) {
    compList_.remove ((void *)comp);
    return CODA_SUCCESS;
  }
  return CODA_WARNING;
}

int
daqSubSystem::numComponents (void)
{
  return compList_.count ();
}

int
daqSubSystem::removeAllComponents (void)
{
  // free memory of all components
  codaSlistIterator ite (compList_);
  daqComponent* comp = 0;

  for (ite.init (); !ite; ++ite) {
    comp = (daqComponent *)ite ();
    delete comp;
  }
  compList_.deleteAllValues ();

  return CODA_SUCCESS;
}

void
daqSubSystem::setDefaultPriority (int pri)
{
  // clean up all elements first
  for (int i = 0; i < CODA_NUM_ACTIONS; i++)
    priorities_[i] = 0;
  
  priorities_[CODA_BOOT_ACTION] = pri;
  priorities_[CODA_DOWNLOAD_ACTION] = pri;
  priorities_[CODA_PRESTART_ACTION] = pri;
  priorities_[CODA_GO_ACTION] = pri;

  priorities_[CODA_PAUSE_ACTION] = -pri;
  priorities_[CODA_END_ACTION] = -pri;
}

void
daqSubSystem::setPriority (int action, int pri)
{
  if (action < CODA_CONNECT_ACTION && action > CODA_TERMINATE_ACTION)
    status_ = CODA_ERROR;
  else {
    priorities_[action] = pri;
    status_ = CODA_SUCCESS;
  }
}

void
daqSubSystem::priorityByAction (int action)
{
  if (action < CODA_CONNECT_ACTION && action > CODA_TERMINATE_ACTION) {
    status_ = CODA_ERROR;
    priority_ = 0;
  }
  else {
    priority_ = priorities_[action];
    status_ = CODA_SUCCESS;
  }
}

int
daqSubSystem::attach (void)
{
  return system_.addSubSystem (this);
}

int
daqSubSystem::detach (void)
{
  return system_.removeSubSystem (this);
}

void
daqSubSystem::enableRun (void)
{
  codaSlistIterator ite (compList_);
  daqComponent* comp = 0;
  
  daqRun* run = system_.run ();
  int found = 0;
  for (ite.init(); !ite; ++ite) {
    comp = (daqComponent *)ite();
    if (run->configured (comp->title()) == CODA_SUCCESS) {
      comp->enable ();
      found = 1;
    }
    else
      comp->disable ();
  }
  if (found) // at least one component in the subsystem is active
    enable ();
  else
    disable ();
}

void
daqSubSystem::enableAll (void)
{
  codaSlistIterator ite (compList_);
  daqComponent* comp = 0;

  for (ite.init(); !ite; ++ite) {
    comp = (daqComponent *)ite();
    comp->enable ();
  }
}

void
daqSubSystem::disableAll (void)
{
  codaSlistIterator ite (compList_);
  daqComponent* comp = 0;

  for (ite.init(); !ite; ++ite) {
    comp = (daqComponent *)ite();
    comp->disable ();
  }  
}

int
daqSubSystem::boot (void)
{
  summaryState_ = CODA_DORMANT;
  if (booter_ == 0)
    booter_ = new subSysBooter (this);
  return doTransition (booter_);
}

int
daqSubSystem::terminate (void)
{
  summaryState_ = CODA_TERMINATING;
  if (terminater_ == 0)
    terminater_ = new subSysTerminater (this);
  return doTransition (terminater_);
}

int
daqSubSystem::status (void) const
{
  return status_;
}

void
daqSubSystem::abort (int wanted)
{
  if (aborter_ == 0)
    aborter_ = new subSysAborter (this);
  ((subSysAborter *)aborter_)->abortedState (wanted);
  doTransition (aborter_);
}

int
daqSubSystem::configure (void)
{
  summaryState_ = CODA_BOOTED;
  if (configurer_ == 0)
    configurer_ = new subSysConfigurer (this);
  return doTransition (configurer_);
}

int
daqSubSystem::download (void)
{
  summaryState_ = CODA_CONFIGURED;
  if (downloader_ == 0)
    downloader_ = new subSysDownloader (this);
  return doTransition (downloader_);
}

int
daqSubSystem::go (void)
{
  summaryState_ = CODA_PAUSED;
  if (activater_ == 0)
    activater_ = new subSysActivater (this);
  return doTransition (activater_);
}

int
daqSubSystem::pause (void)
{
  summaryState_ = CODA_ACTIVE;
  if (pauser_ == 0)
    pauser_ = new subSysPauser (this);
  return doTransition (pauser_);
}

int
daqSubSystem::prestart (void)
{
  summaryState_ = CODA_DOWNLOADED;
  if (prestarter_ == 0)
    prestarter_ = new subSysPrestarter (this);
  return doTransition (prestarter_);
}

int
daqSubSystem::end (void)
{
  summaryState_ = state_;
  if (ender_ == 0)
    ender_ = new subSysEnder (this);
  return doTransition (ender_);
}

int
daqSubSystem::verify (void)
{
  summaryState_ = CODA_VERIFYING;
  if (verifier_ == 0)
    verifier_ = new subSysVerifier (this);
  return doTransition (verifier_);
}

int
daqSubSystem::reset (void)
{
  summaryState_ = CODA_BOOTED;
  if (resetter_ == 0)
    resetter_ = new subSysResetter (this);
  return doTransition (resetter_);
}


// Sergey: ...
int
daqSubSystem::state(void)
{
  status_ = CODA_SUCCESS;
  int targetState = 0;
  int someItems = 0;
  int inconsistent = 0;
  int res = 0;
  int compState;

  if(enabled())
  {
    codaSlistIterator ite (compList_);
    daqComponent* comp = 0;
    
    for(ite.init(); !ite; ++ite)
    {
      comp = (daqComponent *) ite();
      if( comp->enabled () )
      {
        compState = comp->state();
        if(compState == CODA_DISCONNECTED && !comp->autoBoot ())
        {
          // not connected and this component does not need to
          // be booted which means it should be up some where.
          status_ = CODA_ERROR;
          state_ = summaryState_;
          reporter->cmsglog(CMSGLOG_WARN,"lost contact with %s, can't read status.\n",
                            comp->title ());
          return compState;
        }
        if(!someItems)
        {
          targetState = compState;
          someItems = 1;
        }
        else
        {
          if(compState != targetState) inconsistent = 1;
        }
      }
    }
    if(!someItems)
    {
      res = state_;
    }
    else if (inconsistent)
    {
      status_ = CODA_ERROR;
      state_ = summaryState_;
      res = summaryState_;
    }
    else
    {
      state_ = targetState;
      res = targetState;
    }
    reporter->cmsglog (CMSGLOG_INFO1,"%s subSystem is in state %s\n",title_, 
		       codaDaqState->stateString (res));
    
    // if subsystem is the state of disconnected, set its state to
    // dormant since the transitioner will look for disconnected
    // state to break from the waiting loop: 8/20/96
    if(res == CODA_DISCONNECTED) res = CODA_DORMANT;
    return(res);
  }
  return(0);
}


daqSystem&
daqSubSystem::system (void) const
{
  return system_;
}

subSysTransitioner*
daqSubSystem::transitioner (void)
{
  return currTransitioner_;
}

int
daqSubSystem::doTransition (subSysTransitioner *tran)
{
  currTransitioner_ = tran;
  status_ = currTransitioner_->execute ();
  return status_;
}

codaSlist&
daqSubSystem::compList (void)
{
  return compList_;
}

int
daqSubSystem::bootState (void)
{
  status_ = CODA_SUCCESS;
  int targetState = 0;
  int someItems = 0;
  int inconsistent = 0;
  int res = 0;
  int compState;

  if (enabled ()) {
    codaSlistIterator ite (compList_);
    daqComponent* comp = 0;
    
    for (ite.init(); !ite; ++ite) {
      comp = (daqComponent *)ite ();
      if (comp->enabled () ) {
	compState = comp->state ();
	if (compState == CODA_DISCONNECTED && !comp->autoBoot ()) {
	  // not connected and this component does not need to
	  // be booted which means it should be up some where.
	  status_ = CODA_ERROR;
	  state_ = summaryState_;
	  reporter->cmsglog (CMSGLOG_WARN,"can't talk to %s but not allowed to autoboot it.\n",
			     comp->title ());
	  return compState;
	}
	if (!someItems) {
	  targetState = compState;
	  someItems = 1;
	}
	else 
	  if (compState < targetState && targetState != CODA_DISCONNECTED) {
	    inconsistent = 1;
	    targetState = compState;
	  }
      }
    }
    if (!someItems) {
      res = state_;
    }
    else if (inconsistent) {
      status_ = CODA_ERROR;
      state_ = targetState;
      res = targetState;
    }
    else {
      state_ = targetState;
      res = targetState;
    }
    reporter->cmsglog (CMSGLOG_INFO1,"%s subSystem is in state %s\n",title_, 
		       codaDaqState->stateString (res));
    
    // if subsystem is the state of disconnected, set its state to
    // dormant since the transitioner will look for disconnected
    // state to break from the waiting loop: 8/20/96
    if (res == CODA_DISCONNECTED)
      res = CODA_DORMANT;
    return res;
  }
  return 0;
}

