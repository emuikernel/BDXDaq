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
//      Generic State Transition Class. This Abstract class acts as the
//      parent for specific State Transition Classes (eg. DOWNLOADER)
//      which perform state transitions on the subsystems within the DAQ
//      system
//
// NOTICE: subsystem are transitioned in decreasing priority. Subsystems
//         having the same priority are transitioned in parallel, but
//         they must complete the transitions prior to the nexr lower
//         priority subsystems being initiated.
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: transitioner.h,v $
//   Revision 1.3  1997/05/16 16:04:23  chen
//   add global script capability
//
//   Revision 1.2  1996/10/22 17:17:22  chen
//   fix bugs of boot stage different state among components
//
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#ifndef _CODA_TRANSITIONER_H
#define _CODA_TRANSITIONER_H

#include <daqCommon.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <transitionTimer.h>
#include <codaSlist.h>
#include <daqActions.h>

#include "subSysSortedList.h"

class daqSystem;
class daqSubSystem;

class transitioner
{
public:
  // operations
  // execute transition
  virtual void execute (void);
  // cancel transition: CODA_ERROR == if there is an error
  //                    CODA_IGNORED == canceled on purpose
  void cancel (int type = CODA_ERROR);
  // add a child transitioner to the list
  void child (transitioner* child);
  // return child transitioner
  transitioner* child (void) const;

  // return timeout value
  virtual int  transitionTimeout (void);
  // set new timeout value (in second)
  virtual void setTransitionTimeout (int sec);
  // polling frequency
  int        tickInterval (void) const;
  // timer_callback function
  virtual void timerCallback (void);
  // transition title
  const char *title (void);

  // turn on/off wait for script flag
  void waitForScript   (void);
  void noWaitForScript (void);

  virtual const char* className (void) const {return "transitioner";}

  // destructor
  virtual ~transitioner (void);

protected:
  // constructor
  transitioner (daqSystem* system);
  
  // build transition sequence list
  void build (void);
  // action associated with this transition
  // subclass must define this function
  virtual int action (void) = 0;
  // confirm transition failure : type = CODA_ERROR: realy cares
  // type = CODA_IGNORED: canceled transition, do not care
  virtual void confirmFailure (int type = CODA_ERROR);
  // confirm transition success
  virtual void confirmTransition (void);
  // doing transition here
  virtual void doTransition (void);
  // check transition finished or not
  virtual int  transitionFinished (int fstate, int desiredState);
  // execute item on subsystem
  virtual void executeItem (daqSubSystem* subsystem) = 0;
  // some extra system wide setup upon state transition. default is empty
  virtual void extraRunParmSetup (void);

  // setup proper failure state
  virtual void setupFailure (void);
  // setup success state
  virtual void setupSuccess (void);
  // transition busy flag
  virtual int transitionBusy (void);
  // success state
  virtual int  successState (void) = 0;
  // failure state
  virtual int  failureState (void) = 0;
  // transition state
  virtual int transitionState (void) = 0;
  // check subsystem state
  virtual int subSystemState  (daqSubSystem* subsys);

  // send transition result back to the run object
  virtual void sendTransitionResult (int success);

  virtual void runUserSuccessScript (void);

  // clean up all children transitioner list in the case of failure
  void    cleanupChildren (void);

  // data area

  // pointer to daq system
  daqSystem* system_;
  // transition list which holds subsystem according to their priorities
  subSysSortedList transitionList_;
  // transition list iterator during the whole transition
  // ***** Order Dependent ************** //
  codaSlistIterator tranListIte_;
  // active daqSubSystem list which holds all active subSystems
  // all active subSystems have the same priority
  codaSlist activeList_;
  // child of this transitioner
  transitioner* child_;
  // timer to do polling on transition
  transitionTimer    timer_;
  // names of the busy subsystems
  char      *names_;
  // status of the transition
  int       status_;
  // frequency of checking status of transition
  static int tickInterval_;
  // transition timeout
  static int timeout_;
  // timer counter
  int        timeoutCount_;
  // transitioner operation mode: waiting for a user script to finish
  int        waitScript_;
};
#endif
  
  
