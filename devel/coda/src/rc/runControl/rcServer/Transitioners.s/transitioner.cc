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
//      Generic State Transitioner Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: transitioner.cc,v $
//   Revision 1.14  2000/01/20 16:12:06  rwm
//   Change severity of cmlog message for failing a transition.
//   Since the user [or his/er bot ;-)  ] is going to know that
//   the transition failed, the severity if ERROR, not SEVERE.
//   Glory be to Hall-B!
//
//   Revision 1.13  1999/07/28 19:46:42  rwm
//   Wrap #define of _TRACE_OBJECTS
//
//   Revision 1.12  1999/02/17 18:12:24  rwm
//   AUTOEND behaviour.
//
//   Revision 1.11  1998/06/02 19:51:49  heyes
//   fixed rcServer
//
//   Revision 1.10  1997/12/15 16:15:05  abbottd
//   Changes to allow global transition scripts to work
//
//   Revision 1.9  1997/09/19 17:57:21  heyes
//   longer timeouts
//
//   Revision 1.8  1997/08/25 15:57:37  heyes
//   use dplite.h
//
//   Revision 1.7  1997/07/22 19:39:07  heyes
//   cleaned up lots of things
//
//   Revision 1.6  1997/05/16 16:04:23  chen
//   add global script capability
//
//   Revision 1.5  1996/10/31 15:56:15  chen
//   Fixing boot stage bug + reorganize code
//
//   Revision 1.4  1996/10/22 17:17:21  chen
//   fix bugs of boot stage different state among components
//
//   Revision 1.3  1996/10/14 20:02:55  heyes
//   changed message system
//
//   Revision 1.2  1996/10/14 13:29:41  heyes
//   fix timeouts
//
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#include <daqSystem.h>
#include <daqSubSystem.h>
#include <daqState.h>
#include <daqActions.h>
#include <daqScriptSystem.h>
#include <rcMsgReporter.h>
#include <subSysTransitioner.h>
#include "transitioner.h"
/*sergey
#include <dplite.h>
*/
#define _CODA_DEBUG
#ifndef _TRACE_OBJECTS
# define _TRACE_OBJECTS
#endif

// timer interval for ping transitioning components, 1000 msec
int transitioner::tickInterval_ = 1000;
// transition timeout value 15 seconds
int transitioner::timeout_ = 45;

transitioner::transitioner (daqSystem* system)
:system_ (system), child_ (0), activeList_ (), 
 transitionList_ (), tranListIte_ (transitionList_), timer_ (), names_ (0), 
 status_ (CODA_SUCCESS), timeoutCount_ (0), waitScript_ (0)
{
  //printf (OBJECT_DBG,"Create Transitioner Class Object\n");

  timer_.transitionerPtr (this);
}

transitioner::~transitioner (void)
{

  //printf (OBJECT_DBG,"Delete Transitioner Class Object\n");

  if (names_)
    delete []names_;
}

// build transitione sequence according to the priority of each subsystem
void
transitioner::build (void)
{
  // clean up transition list first
  // The transition list is a sorted list according to the priority of
  // the subsystem in the stage of "action"
  transitionList_.deleteAllValues ();
  // transitioner is a friend of daqSystem
  codaSlistIterator ite (system_->subsystems_);
  daqSubSystem* subsys = 0;

  //printf("transitioner 1\n");
  
  for (ite.init (); !ite; ++ite) {
    subsys = (daqSubSystem *)ite ();
    if (subsys->enabled ()) {
      subsys->priorityByAction (action ());
      transitionList_.add ((void *)subsys);
    }
  }
}

// check whether a transition is finished by comparing
// final state with the sucess state
int
transitioner::transitionFinished (int fstate, int successState)
{
  //printf("transitioner::transitionFinished reached 2\n");
  return(fstate == successState);
}

// initiate transition for all subsystems having
// the next encountered priority
void
transitioner::doTransition (void)
{
  int priority;
  int gotAll = 0;
  int busy = 0;
  daqSubSystem* subsys = 0;

  //printf("transitioner::doTransition 3\n");
  status_ = CODA_SUCCESS;
  while(!busy)
  {
    gotAll = 0;

    // clean up active list first
    activeList_.deleteAllValues ();

    // construct the active list
    if(!tranListIte_)
    {
      subsys = (daqSubSystem *)tranListIte_();
      priority = subsys->priority();
      activeList_.add((void *)subsys);
      ++tranListIte_;
    }

    while (!tranListIte_ && !gotAll)
    {
      subsys = (daqSubSystem *)tranListIte_();
      if(priority == subsys->priority ())
      {
        activeList_.add((void *)subsys);
        ++tranListIte_;
      }
      else
        gotAll = 1;
    }

    // end of construction of active list
    if( !activeList_.isEmpty () )
    {
      codaSlistIterator ite (activeList_);
      
      for(ite.init (); !ite; ++ite)
      {
        subsys = (daqSubSystem *)ite ();

        //Sergey: error on Linux: printf("==> Do transition %s on subsystem %s\n",className,subsys->title ());
        executeItem(subsys);

#ifdef DEBUG_MSGS
        printf("==> Executed\n");
#endif

      }

      // check whether this transition is busy
      if(transitionBusy() )
      {
        timer_.auto_arm (transitioner::tickInterval_);
        timeoutCount_  = 0;
        // get out the loop to wait
        busy = 1;
      }
    }
    else
    { 
      // there is nothing to do in transition
      // all are finished, so confirm to successful state
      confirmTransition ();
      busy = 1;
    }
  }
}

void
transitioner::cancel (int type)
{
  //printf("transitioner 4\n");
  timer_.dis_arm ();
  codaSlistIterator ite (activeList_);
  daqSubSystem* subsys = 0;

  for (ite.init (); !ite; ++ite) {
    subsys = (daqSubSystem *)ite ();
    subsys->transitioner()->cancel ();
  }
  confirmFailure (type);
}

void
transitioner::confirmFailure (int type)
{
  codaSlistIterator ite (system_->subsystems_);

  //printf("transitioner 5\n");
  //printf (TRANS_DBG,"confirmFailure Entered\n");

  // put an arbitrary large state to final state
  int finalState = 1000;
  daqSubSystem* subsys = 0;
  int state;
  int failed = 0;

  for (ite.init (); !ite; ++ite) {
    subsys = (daqSubSystem *)ite();
    if (subsys->enabled ()) {
      state = subSystemState (subsys);
      if (!transitionFinished (state, successState ()) ) { // failed
	reporter->cmsglog (CMSGLOG_ERROR,"%s : subsystem failed in %s state\n",
			   subsys->title (), 
			   codaDaqState->stateString (state));
	if (state >= CODA_LOW_STATE && state <= CODA_HIGH_STATE) {
	  if (state < finalState)
	    finalState = state;
	}
	else 
	  finalState = failureState ();
	failed = 1;
      }
    }
  }
  if (failed) {
    system_->setState (finalState);
    reporter->cmsglog (CMSGLOG_ERROR,"%s failed !!!\n", title ());

    // clean up the list of transitioners that are possible chained 
    // together by auto transition
    cleanupChildren ();

    status_ = CODA_ERROR;
    // send failure information to daq run
    sendTransitionResult (type);
  }
}

// confirm that transition succeeded
void
transitioner::confirmTransition (void)
{
  daqSubSystem* subsys = 0;
  int           state, failed = 0;
  codaSlistIterator ite (system_->subsystems_);
  
  //printf("transitioner 6\n");
  //printf (TRANS_DBG,"Confirm Transition Entered\n");

  for (ite.init (); !ite; ++ite) {
    subsys = (daqSubSystem *)ite ();
    if (subsys->enabled ()) {
      state = subSystemState (subsys);
      if (!transitionFinished (state, successState ()) ) { // failed
	reporter->cmsglog (CMSGLOG_ERROR,"%s : subsystem failed in %s state\n",
			   subsys->title (), 
			   codaDaqState->stateString (state));
	failed = 1;
      }
    }
  }
  if (!failed) {
    setupSuccess ();
    
    // run a script here in blocked mode so all scripts should be short
    // This is a global script to be executed at the end of a Transition

	//printf("11111111111\n");
	//sleep(10);
	//printf("22222222222\n");
    runUserSuccessScript ();
	//printf("33333333333\n");
	//sleep(10);
	//printf("44444444444\n");
    
    //printf(TRANS_DBG,"run transition script %08x\n",child_);
	//printf("transitioner: run transition script %08x\n",child_);

    if(child_)
    {
	  //printf("exec script 1\n");
      child_->execute();
	  //printf("exec script 2\n");
      child_ = 0;
    }
    else
    {
      // send success to daq run
	  //printf("exec script 3\n");
      sendTransitionResult (CODA_SUCCESS);
	  //printf("exec script 4\n");
    }
  }
  else
  {
    setupFailure();
    // send failure to daq run
    sendTransitionResult (CODA_ERROR);
  }
}

void
transitioner::child (transitioner *tr)
{
  //printf("transitioner 7\n");
  child_ = tr;
}

transitioner*
transitioner::child (void) const
{
  printf("transitioner 8\n");
  return child_;
}

void
transitioner::cleanupChildren (void)
{
  // clean up the list of transitioners that are possible chained 
  // together by auto transition
  transitioner *p = child_;
  transitioner *q = 0;
  //printf("transitioner 9\n");
  while(p)
  {
    q = p->child ();
    p->child (0);
    p = q;
  }
}


void
transitioner::extraRunParmSetup (void)
{
  //printf("transitioner 10\n");
  // empty
}

const char*
transitioner::title (void)
{
  //printf("transitioner 11\n");
  return codaDaqActions->actionString (action ());
}

void
transitioner::execute (void)
{
  //printf("transitioner 12\n");
  // set current transitioner to the system
  system_->currTransitioner (this);
  // first build transition list
  build ();
  // set state information
  system_->setState (transitionState ());
  
  status_ = CODA_SUCCESS;
  // extra system wide parameter set up
  extraRunParmSetup ();
  // move cursor to the begginning of the list
  tranListIte_.init();
  // now start engine
  doTransition ();
}

void
transitioner::setupFailure (void)
{
  //printf("transitioner 13\n");
  system_->setState (failureState ());
  // 
  reporter->cmsglog (CMSGLOG_ERROR,"%s failed !!!\n",title());

  // clean up the list of transitioners that are possible chained 
  // together by auto transition
  cleanupChildren ();
  // finished cleaning

  status_ = CODA_ERROR;
}

void
transitioner::setupSuccess (void)
{
  //printf("transitioner 14\n");
  system_->setState (successState ());
  reporter->cmsglog (CMSGLOG_INFO,"transition %s succeeded !\n",title());
  status_ = CODA_SUCCESS;
}

int
transitioner::subSystemState(daqSubSystem* subsys)
{
  int tmp;
  //printf("transitioner::subSystemState reached 15\n");
  tmp = subsys->state();
  //printf("transitioner::subSystemState reached 151\n");
  return(tmp);
}


// that method called on timer when we are waiting for components
// to finish transition
void
transitioner::timerCallback(void)
{
  //printf("transitioner::timerCallback 16\n");
  if(transitionBusy())
  {
    if(!waitScript_) timeoutCount_ ++;
    if(timeoutCount_ >=
       (transitionTimeout()*1000)/transitioner::tickInterval_)
    {
      timer_.dis_arm();
      printf("%s : %s subsystem(s) timeout !!!(%d %d %d)\n",
			 title(),
			 names_,timeoutCount_,transitionTimeout(),
			 transitioner::tickInterval_);
      reporter->cmsglog(CMSGLOG_ERROR,"%s : %s subsystem(s) timeout !!!(%d %d %d)\n",
			 title(),
			 names_,timeoutCount_,transitionTimeout(),
			 transitioner::tickInterval_);
      // cancel transition
      cancel(CODA_ERROR);
    }
    else if(timeoutCount_ % 4 == 0)
    {
#ifdef DEBUG_MSGS
      printf("%s : waiting for %s subsystem(s)...\n",
			title (), names_);
#endif
      reporter->cmsglog (CMSGLOG_INFO1,"%s : waiting for %s subsystem(s)...\n",
			title (), names_);
    }
    else if(timeoutCount_ % 8 == 0)
    {
      printf("%s : %s subsystem(s) still busy...\n",
			title (), names_);
      reporter->cmsglog (CMSGLOG_INFO1,"%s : %s subsystem(s) still busy...\n",
			title (), names_);
    }
  }
  else
  {
    //printf("doTransition\n");
    timer_.dis_arm ();
    doTransition();
  }
}

// Test for parallel items to leave transition state.
// Return 1: the transition is still underway
// Return 0: the transition is finished
int
transitioner::transitionBusy (void)
{
  codaSlistIterator ite (activeList_);
  daqSubSystem* subsys = 0;
  int           state = 0;

  //printf("transitioner::transitionBusy 17\n");
  if(names_)
  {
    delete []names_;
    names_ = 0;
  }

  for(ite.init(); !ite; ++ite)
  {
	//printf("111\n");
    subsys = (daqSubSystem *) ite();
    state = subSystemState (subsys);

    // if components are disconnected, no need to wait
    if(state == CODA_DISCONNECTED) return(0);

    if( !transitionFinished(state, successState()) )
    {
      names_ = new char[::strlen (subsys->title()) + 1];
      ::strcpy (names_, subsys->title());
      return(1);
    }
  }
  return(0);
}

int
transitioner::transitionTimeout (void)
{
  //printf("transitioner 18\n");
  return transitioner::timeout_;
}

void
transitioner::setTransitionTimeout (int sec)
{
  //printf("transitioner 19\n");
  transitioner::timeout_ = sec;
}

int
transitioner::tickInterval (void) const
{
  //printf("transitioner 20\n");
  return transitioner::tickInterval_;
}

void
transitioner::waitForScript (void)
{
  //printf("transitioner 21\n");
  waitScript_ = 1;
}

void
transitioner::noWaitForScript (void)
{
  //printf("transitioner 22\n");
  waitScript_ = 0;
}

void
transitioner::sendTransitionResult (int success)
{
  //printf("transitioner 23\n");
  // reset transitioner of the system
  system_->currTransitioner (0);
  // get daqRun Object
  daqRun* run = system_->run ();

  run->cmdFinalResult (success);
}

void
transitioner::runUserSuccessScript (void)
{
  // to avoid 'defunct's
  int rtn_stat;
  static void (*istat)(int);
  static void (*qstat)(int);

  // get daqRun Object
  daqRun* run = system_->run ();

  // get script system
  daqScriptSystem& ssys = run->scriptSystem ();

  char* script = ssys.script (action());

  //printf("transitioner 24\n");
  if (!script)
    return;

  // construct a user script which has X window Display information
  char     realscript[1024];  // no way to exceed 1024 chars
  ::sprintf (realscript, "setenv DISPLAY %s; ", run->controlDisplay());
  ::strcat  (realscript, script);

  printf("State finished real script is: %s\n", realscript);


  /*
  system(realscript);
  */

  
  // start up a child process 
  ::fflush (stdout);
  int tty = ::open ("/dev/tty", 2);
  if (tty == -1) {
    fprintf (stderr, "%s: Cannot open /dev/tty \n", realscript);
    exit (1);
  }

  // child process will not duplicate parent address space, so
  // in the child process one has to call exec () system call
  // quickly
  pid_t pid = ::vfork ();

  //sergey if(pid == -1) // unable to fork
  if(pid < 0) // unable to fork
  {
    //printf("Unable to fork - error\n");
    return;
  }
  else if (pid == 0) // child process
  {
    close(0); dup (tty);
    close(1); dup (tty);
    close(2); dup (tty);
    close(tty);

	//printf("111\n");
	//sleep(10);
	//printf("222\n");

    execlp("csh", "csh", "-c", realscript, (char *)0);

	//printf("333\n");
	//sleep(10);
	//printf("444\n");

	//sergey: need that ???
//printf("555\n");
//waitpid(pid, NULL, 0);
//printf("666\n");
//kill(pid, 9);
//printf("777\n");

    exit(127);
  }

  // parent process
  close (tty);



  // Sergey: following piece copied from Components.s/netComponent.cc
  // to avoid 'defunct's

  // let the child catch signal
  istat = ::signal (SIGINT, SIG_IGN);
  qstat = ::signal (SIGQUIT, SIG_IGN);

  int wpid, status;
  signed char estatus;

  while ((wpid = ::waitpid (pid, &status, 0)) != pid && wpid != -1)
  {
    ;// no-op.
  }

  // Hysterical past - Ask Jie Chen, I don't know - RWM!
  estatus = (signed char) (status>>8)&0xff;

  //#ifdef _CODA_DEBUG
  printf("transitioner.cc: script terminated with status %x estatus = %d\n",
    status,estatus);
  //#endif

  if((wpid == -1) || (estatus == -2))
  {
    rtn_stat = CODA_ERROR;
  }
  else
  {
    if(status == 0)
    {
      rtn_stat = CODA_SUCCESS;
    }
    else
    {
      rtn_stat = CODA_ERROR;
    }
  }

  ::signal (SIGINT, istat);
  ::signal (SIGQUIT, qstat);

  //return(rtn_stat);

  return;
}
