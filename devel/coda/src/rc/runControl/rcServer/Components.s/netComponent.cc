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
//      Data Acquisition Network Component Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: netComponent.cc,v $
//   Revision 1.27  1999/11/29 19:46:52  rwm
//   status() used to return state_ !
//
//   Revision 1.26  1999/07/28 19:17:40  rwm
//   Bunch of hacks to support Pause script button for Hall B.
//
//   Revision 1.25  1999/02/17 18:11:26  rwm
//   AUTOEND behaviour.
//
//   Revision 1.24  1999/02/04 16:24:20  rwm
//   Some reformatting and debug_printf.
//
//   Revision 1.23  1999/02/02 19:02:13  heyes
//   AUTOEND feature
//
//   Revision 1.22  1999/01/28 18:49:25  heyes
//   run end error message
//
//   Revision 1.21  1999/01/28 14:13:35  heyes
//   end run 60 secs after fail
//
//   Revision 1.20  1998/11/09 16:18:43  timmer
//   Linux port
//
//   Revision 1.19  1998/11/05 20:11:50  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.18  1998/09/18 15:05:06  heyes
//    fixed an end related problem for Dave.
//
//   Revision 1.17  1998/02/24 16:27:42  rwm
//   download and prestart async by not serially asking for status.
//
//   Revision 1.16  1997/08/25 15:57:34  heyes
//   use dplite.h
//
//   Revision 1.15  1997/06/13 21:30:47  heyes
//   for marki
//
//   Revision 1.13  1997/05/23 16:45:08  heyes
//   add SESSION env variable, remove coda_activate
//
//   Revision 1.12  1997/05/15 13:34:32  chen
//   change previoud modification back
//
//   Revision 1.11  1997/05/14 18:36:15  chen
//   Fix bug on prestart arguments ordering
//
//   Revision 1.10  1997/02/26 16:20:40  heyes
//   fix exit and zap
//
//   Revision 1.9  1996/12/04 18:32:46  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.8  1996/11/27 15:06:10  chen
//   fix CDEV and Set internal itcl component state before doing state transition
//
//   Revision 1.7  1996/11/05 17:37:49  chen
//   bootable flag is added to daqComponent
//
//   Revision 1.6  1996/10/31 15:56:10  chen
//   Fixing boot stage bug + reorganize code
//
//   Revision 1.5  1996/10/28 20:32:08  heyes
//   timeout on DP_ask
//
//   Revision 1.4  1996/10/28 14:23:04  heyes
//   communication with ROCs changed
//
//   Revision 1.3  1996/10/22 17:17:18  chen
//   fix bugs of boot stage different state among components
//
//   Revision 1.2  1996/10/14 20:02:50  heyes
//   changed message system
//
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#include "netComponent.h"
#include <daqGlobal.h>
#include <daqSystem.h>
#include <rcMsgReporter.h>
#include <daqRun.h>
#include <codaCompClnt.h>

#ifdef USE_TK
#include <rcTclInterface.h>
#endif

/* Sergey: status_ described in daqTarget.h ? */

//===========================================================================
//        Implementation of class netComponent
//===========================================================================

// timer tick interval 250 mesec
int netComponent::tickInterval = 250;
// timeout limit is 10 seconds
int netComponent::timeoutLimit = 10;
// maximum number of retries to connect to remote object
int netComponent::numConn_ = 10;

int netComponent::IsPaused = FALSE;

#ifdef _CODA_USE_THREADS
#define DISABLE_THREAD_CANCELLING (pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, 0))
#define ENABLE_THREAD_CANCELLING  (pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, 0))
#define SET_CANCEL_POINT          (pthread_testcancel ())


void *
netComponent::bootThread (void *arg)
{
  netComponent* comp = (netComponent *)arg;

  daqRun* run = comp->subsys_.system().run();
  int updateI = run->dataUpdateInterval();

  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = (netComponent::tickInterval)*1000;
  int counterLimit = (netComponent::timeoutLimit*1000)/(netComponent::tickInterval);
  int status, timeoutCount = 0;
  printf("Boot thread!!!\n");

  while (timeoutCount < counterLimit) {
    // always test cancel thread to enable cancel point right here
    SET_CANCEL_POINT;

    // disable canceling
    DISABLE_THREAD_CANCELLING;
    {
      locker guard (comp);
      printf("dacreate from boot thread\n");
      comp->status_ = ::codaDaCreate (comp->title_, comp->number_, comp->expid_, 
				      comp->type_, comp->node_,updateI);

      comp->transformState (CODA_BOOTED, CODA_BOOTED, comp->status_);

      if (comp->status_ == CODA_SUCCESS) {
	comp->established_ = 1;

	reporter->cmsglog (CMSGLOG_INFO1,"Thread %d: %s booted ok\n", pthread_self (),
			   comp->title_);
	return 0;
      }
      else if ((timeoutCount % 8) == 0) {
	reporter->cmsglog (CMSGLOG_INFO1,"Thread %d: %s still booting......\n", 
			   pthread_self(), comp->title_);
	timeoutCount ++;
      }
      else 
	timeoutCount ++;
      ::select (0, 0, 0, 0, &tv);
      // enable canceling
      ENABLE_THREAD_CANCELLING;
    }
  }
  // disable canceling
  DISABLE_THREAD_CANCELLING;

  reporter->cmsglog (CMSGLOG_ERROR,"Thread %d: %s boot timeout !!!\n", 
		     pthread_self (), comp->title_);

  {
    locker guard (comp);
    comp->status_ = CODA_ERROR;
    comp->thrCreated_ = 0;
    comp->prevState_ = CODA_DORMANT;
    comp->state_ = CODA_DORMANT;

    reporter->cmsglog (CMSGLOG_INFO1,"Thread %d: finished.......\n", pthread_self ());
  }

  ENABLE_THREAD_CANCELLING;
  // set cancel point
  SET_CANCEL_POINT;
  return 0;
}

void
netComponent::cancelTransitionThread_i (void)
{
#ifdef DEBUG_MSGS
  printf("netComponent::cancelTransitionThread_i reached\n");
#endif
  if(thrCreated_)
  {
    pthread_cancel (thr_);
    thrCreated_ = 0;
  }
}

void
netComponent::cancelTransitionThread (void)
{
  locker guard (this);
  cancelTransitionThread_i ();
}

int
netComponent::createDetachedThread (void *(*func)(void *), pthread_t* thr)
{
  // create a detached thread
  int status;
#ifdef DEBUG_MSGS
  printf("netComponent::createDetachedThread reached\n");
#endif
  ::pthread_attr_t attr;
  ::pthread_attr_init (&attr);
  ::pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
  status = ::pthread_create (thr, &attr, func, (void *)this);
  ::pthread_attr_destroy (&attr);
  return status;
}

void
netComponent::lock (void)
{
  ::pthread_mutex_lock (&lock_);
}

void
netComponent::unlock (void)
{
  ::pthread_mutex_unlock (&lock_);
}


locker::locker (netComponent* comp)
  :comp_ (comp)
{
  comp_->lock ();
}

locker::~locker (void)
{
  comp_->unlock ();
}

#endif
  

netComponent::netComponent (char *title, int number, int expid,
			    char *type, char *node, char *bootString,
			    daqSubSystem& subsys)
  :daqComponent (title), subsys_ (subsys), config_ (0),
   number_ (number), expid_ (expid), 
   established_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create netComponent Class Object %s\n", title);
#endif
  type_ = new char[::strlen (type) + 1];
  ::strcpy (type_, type);

  node_ = new char[::strlen (node) + 1];
  ::strcpy (node_, node);

  if (bootString && *bootString) {
    bootString_ = new char[::strlen (bootString) + 1];
    ::strcpy (bootString_, bootString);
  }
  else {
    bootString_ = 0;
    bootable_ = 0;
    autoBoot_ = 0;
  }

  // set default priority of this component
  priority (number);

  //  Autoend feature
  char *monitor = ::getenv("AUTOEND");

  if (monitor) {
    if ((::strcmp(monitor,type_) == 0) ||
	(::strcmp(monitor,"ALL") == 0) ||
	((::strcmp(monitor,"UNIX") == 0) && ::strcmp(type_,"EB")) ||
	((::strcmp(monitor,"UNIX") == 0) && ::strcmp(type_,"ER"))  ) {
      printf("Turning monitor for autoend on: monitor = %s, type_ = %s.\n",
	     monitor, type_);

      monitorOn();
    }
  }
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)  
  // generate an Itcl object to map onto the C++ object
  codaCreateDaComp (title);
#endif

  // register this component into subsystem and system
  subsys_.addComponent (this);
  daqSystem& system = subsys_.system ();
  system.addComponent (this);

#ifdef _CODA_USE_THREADS
  pthread_mutex_init (&lock_, 0);
  thrCreated_ = 0;
#else
  bootTimer_ = new netCompBootTimer ();
  bootTimer_->component (this);
  timerCount_ = 0;
#endif
}

netComponent::~netComponent (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete netComponent Class Object\n");
#endif

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)  
  // delete the Itcl object that mapped onto the C++ object
  codaDeleteDaComp (title_);
#endif

  delete []type_;
  delete []node_;
  if (bootString_)
    delete []bootString_;
  config_ = 0;

#ifdef _CODA_USE_THREADS
  cancelTransitionThread ();
#else
  bootTimer_->dis_arm ();
  delete bootTimer_;
#endif
}

int
netComponent::attach (void)
{
  int err = 0;
  if (subsys_.addComponent (this) != CODA_SUCCESS)
    err = 1;
  daqSystem& system = subsys_.system ();
  if (system.addComponent (this) != CODA_SUCCESS)
    err = 1;
  if (err)
    return CODA_WARNING;

  return CODA_SUCCESS;
}

int
netComponent::detach (void)
{
  int err = 0;
  if (subsys_.removeComponent (this) != CODA_SUCCESS)
    err = 1;
  daqSystem& system = subsys_.system ();
  if (system.removeComponent (this) != CODA_SUCCESS)
    err = 1;
  if (err)
    return CODA_WARNING;
  return CODA_SUCCESS;
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
void
netComponent::buildBootInfo (void)
{
  // empty
}
#else
void
netComponent::buildBootInfo (void)
{
  daqSubSystem* subsys = 0;
  netComponent* comp = 0;
  daqSystem& sys = subsys_.system ();

  ::strcpy (bootinfo_, title_);
  if (sys.locateSystem (CODA_EB_CLASS, subsys) == CODA_SUCCESS) {
    // assume they are all netComponent
    comp = (netComponent *) (subsys->firstEnabledComp ());
    if (comp) {
      ::strcat (bootinfo_," -e=");
      ::strcat (bootinfo_,comp->node_);
    }
  }
  if (sys.locateSystem (CODA_ANA_CLASS, subsys) == CODA_SUCCESS) {
    // assume they are all netComponent
    comp = (netComponent *) (subsys->firstEnabledComp ());
    if (comp) {
      ::strcat (bootinfo_," -a=");
      ::strcat (bootinfo_,comp->node_);  
    }
  }
  if (sys.locateSystem (CODA_ER_CLASS, subsys) == CODA_SUCCESS) {
    // assume they are all netComponent
    comp = (netComponent *) (subsys->firstEnabledComp ());
    if (comp) {
      ::strcat (bootinfo_," -l=");
      ::strcat (bootinfo_,comp->node_);  
    }
  }
  if (sys.locateSystem (CODA_LOG_CLASS, subsys) == CODA_SUCCESS) {
    // assume they are all netComponent
    comp = (netComponent *) (subsys->firstEnabledComp ());
    if (comp) {
      ::strcat (bootinfo_," -m=");
      ::strcat (bootinfo_,comp->node_);  
    }
  }
}
#endif      
  
int
netComponent::boot (void)
{
  status_ = CODA_SUCCESS;
  daqRun* run = subsys_.system().run();
  int updateI = run->dataUpdateInterval();

  if (!established_) {
#ifdef _CODA_DEBUG
    if (bootString_) 
      printf ("Component number %d type %s node %s and bootString %s\n",
	      number_, type_, node_, bootString_);
    else
      printf ("Component number %d type %s node %s\n",
	      number_, type_, node_);
#endif

    reporter->cmsglog (CMSGLOG_INFO,"Connecting to %s on host %s\n",title_,node_);

    buildBootInfo ();

    status_ = ::codaDaCreate (title_, number_, expid_, type_, node_,updateI);

    transformState (CODA_BOOTED, CODA_BOOTED, status_);

    if (autoBoot_) {
      if (status_ != CODA_SUCCESS && bootString_) {
				// create bootString
	char msg[256];
	char *rshCmd = "rsh";

#ifdef HP_UX
	rshCmd = "remsh";
#endif
	char *bootCmd = "echo";

	if (::strcmp(type_,"RCS") == 0) {
	  bootCmd = "rcServer";
	}
	else if  (::strcmp(type_,"EB") == 0) {
	  bootCmd = "coda_eb";
	}
	else if (::strcmp(type_,"ROC") == 0) {
	  bootCmd = "coda_roc";
	}
	else if (::strcmp(type_,"ER") == 0) {
	  bootCmd = "coda_er";
	}
	else if (::strcmp(type_,"TS") == 0) {
	  bootCmd = "coda_ts";
	} 

	char *realType = type_;
	if (::strcmp(type_,"EB") == 0) {
	  realType = "CDEB";
	}
	daqRun* run = subsys_.system().run();

	reporter->cmsglog (CMSGLOG_WARN,"attempt to boot %s\n",
			   title_);      
	::sprintf(msg,"coda_activate.tcl %s %s %s %s %s %s\n",
		  run->msqlhost(),
		  node_,
		  run->database (),
		  run->exptname (),
		  realType,
		  title_);

	::printf("coda_activate.tcl %s %s %s %s %s %s\n",
		 run->msqlhost(),
		 node_,
		 run->database (),
		 run->exptname (),
		 realType,
		 title_);

	status_ = system(msg);

				/*if (!bootString_) 
				  ::sprintf (msg, "coda_activate -m %s %s %d %s %s %s %s", 
				  run->msqlhost(),
				  title_, number_, type_, 
				  node_, run->database (), run->exptname () );
				  else 
				  ::sprintf (msg, "coda_activate -p %s -m %s %s %d %s %s %s %s",
				  bootString_, 
				  run->msqlhost(),
				  title_, number_, type_, node_, 
				  run->database (), run->exptname () );*/

      }
      else if (bootString_ == 0 && status_ != CODA_SUCCESS)  {
	printf("here with boot failure %d\n",status_);
	status_ = CODA_ERROR;
      }
      if ((status_ == CODA_SUCCESS) || (status_ == CODA_WARNING)) {
	setState (CODA_DORMANT);
	reporter->cmsglog (CMSGLOG_INFO1,"boot underway...\n");
	waitForConnection ();
      }
      else {
	setState (CODA_DORMANT);
	reporter->cmsglog (CMSGLOG_ERROR,"boot failed !!!\n");
	setOverrideState (CODA_DORMANT);
      }
    } // autoboot
    else {
      if (status_ == CODA_SUCCESS) {
	established_ = 1;
	setState (CODA_BOOTED);
	reporter->cmsglog (CMSGLOG_INFO,"booted ok\n");      
      }
      else {
	setState (CODA_DISCONNECTED);
	reporter->cmsglog (CMSGLOG_ERROR,"boot failed !!!\n");
	setOverrideState (CODA_DISCONNECTED);
      }

    }
  }
  else // if not connected
    setState (CODA_BOOTED);

  return CODA_SUCCESS;
}

int
netComponent::terminate (void)
{
  if (established_) {
    status_ = CODA_SUCCESS;
    reporter->cmsglog (CMSGLOG_INFO,"%s terminate ......\n", title_);
    // real zap function
    status_ = ::codaDaZap (title_);
    if (status_ == CODA_SUCCESS) 
      reporter->cmsglog (CMSGLOG_INFO,"%s terminated ok\n", title_);
    else
      reporter->cmsglog (CMSGLOG_ERROR,"%s terminated failed !!!\n", title_);
  }
  setState (CODA_DORMANT);
  established_ = 0;
  status_ = CODA_SUCCESS;
  return CODA_SUCCESS;
}

void
netComponent::cancelTransition (void)
{
  if (established_)
    daqComponent::cancelTransition ();
  else {
    prevState_ = CODA_DORMANT;
    state_ = CODA_DORMANT;
  }
#ifdef _CODA_USE_THREADS
  cancelTransitionThread ();
#else
  bootTimer_->dis_arm ();
  timerCount_ = 0;
#endif
}

int
netComponent::configure (void)
{
  status_ = CODA_SUCCESS;
  // remove all the old information

  config_ = 0;
  // get configuration information
  daqRun* run = subsys_.system().run();
  if (run->getNetConfigInfo (title_, config_) != CODA_SUCCESS)
    fprintf (stderr, "Something is fishy..........\n");

  setState (CODA_CONFIGURED);

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)  
  // get information on the Itcl object that map onto the C++ object
  if (enabled_) // only get state information for enable components
    codaDaCompConfigure (title_);
#endif

  return status_;
}

int
netComponent::download(void)
{
  status_ = CODA_SUCCESS;
  if(established_)
  {
    int oldstate = state();

#ifdef NEVER_DEFINED
    /* It used to be that we could only call download from one of
       these three states. Now it is safe to call download from
       anywhere.
    */
    if(oldstate == CODA_CONFIGURED || oldstate == CODA_BOOTED ||
       oldstate == CODA_DOWNLOADED) 

#endif
    {
      reporter->cmsglog (CMSGLOG_INFO,"download component %s ......\n", title_);
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
      // set internal itcl components state to downloading
      codaDaCompSetState(title_, CODA_DOWNLOADING);

      daqRun* run = subsys_.system().run();
      status_ = ::codaDaDownload (title_, run->runtype ());
#else
      status_ = ::codaDaDownload (title_, config_);
#endif

#ifdef DEBUG_MSGS
      printf("befor transformState +++++++++++++++++++++\n");
#endif
      transformState(CODA_DOWNLOADING, CODA_DOWNLOADED, status_);
#ifdef DEBUG_MSGS
      printf("after transformState =====================\n");
#endif
      if(status_ == CODA_SUCCESS)
      {
#ifdef DEBUG_MSGS
        printf("%s download underway.....\n", title_);
#endif
        reporter->cmsglog(CMSGLOG_INFO1,"%s download underway.....\n", title_);
      }
      else
      { 
        printf("%s downloaded failed\n", title_);
        reporter->cmsglog(CMSGLOG_ERROR,"%s downloaded failed\n", title_);
      }
    }
  }
  return CODA_SUCCESS;
}

int
netComponent::go (void)
{
  char scriptname[1024];
  status_ = CODA_SUCCESS;


  if (established_) {
    if (state () == CODA_PAUSED) {
      reporter->cmsglog (CMSGLOG_INFO,"%s go.....\n", title_);

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
      codaDaCompSetState (title_, CODA_ACTIVATING);
#endif

      // hack to allow pause/resume script. RWM April 99
      if(strcmp(type_, "TS") == 0)
      {
        if(netComponent::IsPaused)
        {
          sprintf(scriptname,"coda_resume %s",node_);
          printf("scriptname1: >%s<\n",scriptname);
          status_ = runScript(scriptname);
          printf("Status of script1 = %d\n",status_);
        }
        else
        {
          status_ = ::codaDaGo (title_, 0);
        }
        netComponent::IsPaused = FALSE;
      }
      else
      {
        status_ = ::codaDaGo (title_, 0);
      }

      transformState(CODA_ACTIVATING, CODA_ACTIVE, status_);

      if(status_ == CODA_SUCCESS)
      {
        if(state_ == CODA_ACTIVE)
          reporter->cmsglog (CMSGLOG_INFO,"%s go ok!?!?!\n", title_);
        else
          reporter->cmsglog (CMSGLOG_INFO1,"%s go underway.....\n", title_);
      }
      else
      {
        reporter->cmsglog (CMSGLOG_ERROR,"%s go failed\n", title_);
      }

    } else {
      
      // hack to allow pause/resume script. RWM April 99
      if(strcmp(type_, "TS") == 0)
      {
        if(netComponent::IsPaused)
        {
          sprintf(scriptname,"coda_resume %s",node_);
          printf("scriptname2: >%s<\n",scriptname);
          status_ = runScript(scriptname);
          printf("Status of script2 = %d\n",status_);
        }
        else
        {
          printf("REPORT A BUG: netComponent::Go(void): This should never happen.\n");
        }

        if(status_ == 0)
        {
          netComponent::IsPaused = FALSE;
        }
        else
        {
          // bugbug: Since resuming is not a full member of the federation ;-\
          // all I can do here is report an ERROR.
          reporter->cmsglog (CMSGLOG_ERROR,"%s resume failed\n", title_);
          return CODA_ERROR;
        }
      }

    }
  }
  return CODA_SUCCESS;
}

int
netComponent::pause (void)
{
  char scriptname[1024];
  int script_st = 0;
  status_ = CODA_SUCCESS;
  if (established_) {
    if (state () == CODA_ACTIVE) {
      reporter->cmsglog (CMSGLOG_INFO,"%s pause......\n", title_);

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
      codaDaCompSetState (title_, CODA_PRESTARTING);
#endif

      // Hack to allow pause/resume script. RWM April 99

      // Instead or really telling all components to pause,
      // just call a script called "coda_pause" so that
      // the proper pause/resume behaviour for an experiment
      // can be carried out.

      // Hack: only pause the Trigger Supervisor.
      if(strcmp(type_, "TS") == 0)
      {
        sprintf(scriptname,"coda_pause %s",node_);
        printf("scriptname: >%s<\n",scriptname);
        status_ = runScript(scriptname);
        printf("Status of script = %d\n",status_);
        // If script ran ok then we really are paused.
        if(status_ == 0)
        {
          netComponent::IsPaused = TRUE;
          status_ = CODA_PAUSED;
        }
      }
      else
      {
	    // non-hack behaviour
	    //status_ = ::codaDaPause (title_, 0);
        status_ = CODA_PAUSED;
      }

      transformState (CODA_PAUSING, CODA_PAUSED, status_);

      if(status_ == CODA_SUCCESS)
      {
        if(state () == CODA_PAUSED)
        {
          reporter->cmsglog (CMSGLOG_INFO,"%s paused ok\n", title_);
        }
        else
        {
          reporter->cmsglog (CMSGLOG_INFO1,"%s pause underway......\n", title_);
        }
      }
      else
      {
        reporter->cmsglog (CMSGLOG_ERROR,"%s pause failed\n", title_);
      }
    }
  }
  return CODA_SUCCESS;
}

int
netComponent::prestart (void)
{
  status_ = CODA_SUCCESS;
  if (established_)
  {
    if(state () == CODA_DOWNLOADED )
    {
      reporter->cmsglog (CMSGLOG_INFO,"%s prestart......\n", title_);

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
      codaDaCompSetState (title_, CODA_PRESTARTING);
#endif

      daqRun* run = subsys_.system().run();
      status_ = ::codaDaPrestart(title_, run->runtypeNum(), run->runNumber());
      transformState (CODA_PRESTARTING, CODA_PAUSED, status_);
      if(status_ == CODA_SUCCESS)
      {
        reporter->cmsglog (CMSGLOG_INFO1,"%s prestart underway......\n", title_);

        // hack to allow pause/resume script. RWM April 99
        if(strcmp(type_, "TS") == 0)
        {
          netComponent::IsPaused = FALSE;
        }
      }
      else
      {
        reporter->cmsglog (CMSGLOG_ERROR,"%s prestart failed\n", title_);
        return CODA_ERROR;
      }
    }
  }
  return CODA_SUCCESS;
}

int
netComponent::reset (void)
{
  if (established_) {
    status_ = CODA_SUCCESS;
    reporter->cmsglog (CMSGLOG_INFO,"%s reset ......\n", title_);
    // real zap function
    status_ = ::codaDaReset (title_);

    // hack to allow pause/resume script. RWM April 99
    if (strcmp(type_, "TS") == 0) {
      netComponent::IsPaused = FALSE;
    }

    if (status_ == CODA_SUCCESS) {
      reporter->cmsglog (CMSGLOG_INFO,"%s reset ok\n", title_);
    } else {
      reporter->cmsglog (CMSGLOG_ERROR,"%s reset failed !!!\n", title_);
    }
  }
  setState (CODA_DORMANT);
  established_ = 0;
  status_ = CODA_SUCCESS;
  return CODA_SUCCESS;
}

int
netComponent::end (void)
{
  status_ = CODA_SUCCESS;
  if (established_) {
    if (state () >= CODA_DOWNLOADED) {
      reporter->cmsglog (CMSGLOG_INFO,"%s end......\n", title_);

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
      codaDaCompSetState (title_, CODA_ENDING);
#endif

      status_ = ::codaDaEnd (title_, 0);
      transformState (CODA_ENDING, CODA_DOWNLOADED, status_);
      if (status_ == CODA_SUCCESS) {

	reporter->cmsglog (CMSGLOG_INFO1,"%s end underway......\n", title_);

	// hack to allow pause/resume script. RWM April 99
	if (strcmp(type_, "TS") == 0) {
	  netComponent::IsPaused = FALSE;
	}
      } else {
	reporter->cmsglog (CMSGLOG_ERROR,"%s end failed\n", title_);
      }
    }
  }
  return CODA_SUCCESS;
}

int
netComponent::verify (void)
{
  // empty for now
  return CODA_SUCCESS;
}

int 
netComponent::status (void) const
{
  return status_;
}

int
netComponent::state (void)
{
  int st = state_;

  daqRun* run = subsys_.system().run();
  int updateI = run->dataUpdateInterval();

  /*Sergey
  printf ("netComponent::state: Component %s at state %d\n",title_, state_);
  */

  if(time(0) > prevtime_ + 6 * updateI)
  {
    reporter->cmsglog (CMSGLOG_WARN,
		       "%s has not reported status for %d seconds\n", 
		       title_, time(0) - prevtime_);

    if(time(0) > prevtime_ + 6 * updateI + 60)
    {
      if(st == DA_ACTIVE)
      {
        st = overrideState_;
        prevState_ = state_;
        state_ = st;
        // set state dynamic variable associated with this component
        if(::getenv("AUTOEND"))
        {
          // Warn for now, will be an error later.
          reporter->cmsglog (CMSGLOG_WARN,
			     "%s no status for %d seconds\n", 
			     title_, time(0) - prevtime_ + 60); 

          reporter->cmsglog (CMSGLOG_WARN, 
			     "Runcontrol is about to end the Run\n" ); 
        }
        else
        {
          reporter->cmsglog (CMSGLOG_ERROR,
			     "End Run, %s no status for %d seconds\n", 
			     title_, time(0) - prevtime_ + 60);
        }
        setStatusVariable (st);
      }
    }
  }

  if((state_ == DA_ACTIVE ) && (prevState_ == overrideState_))
  {
    printf("WARNING: %s welcome back!! where've you been??\n",title_);
    reporter->cmsglog (CMSGLOG_WARN,"%s welcome back!! where've you been??\n", title_);
    prevState_ = state_;
  }

  //printf ("netComponent::state: Component %s at state %d\n",title_, state_);

  return(st);
}

int
netComponent::state2 (void)
{
printf("netComponent::state2 reached\n");
  return state();
}

void
netComponent::setStatusVariable (int st)
{
  daqSystem& system = subsys_.system ();  
  system.run()->setStatusVariable (title_, st);
}

int
netComponent::waitForConnection (void)
{
#ifdef _CODA_USE_THREADS
  // create a detached thread
  locker guard (this);

  if (createDetachedThread (&(netComponent::bootThread), &thr_) == 0) {
    thrCreated_ = 1;
    reporter->cmsglog (CMSGLOG_INFO1,"Create a new thread %d \n", thr_);
  }
  else
    reporter->cmsglog (CMSGLOG_ERROR,"Unable to create a new thread\n");
  return -1;
#else
  timerCount_ = 0;
  bootTimer_->auto_arm (netComponent::tickInterval);
  return 1;
#endif
}

#if !defined (_CODA_USE_THREADS)
void
netComponent::timerCallback (void)
{
  int counterLimit = (netComponent::timeoutLimit*1000)/(netComponent::tickInterval);

  daqRun* run = subsys_.system().run();
  int updateI = (int)(*run->updateInterval_);

#ifdef DEBUG_MSGS
  printf("netComponent::timerCallback reached\n");
#endif

  if (timerCount_ < counterLimit) {
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
    status_ = ::codaDaCreate (title_, number_, expid_, type_, node_,updateI);
#else
    status_ = ::codaDaCreate (bootinfo_, number_, expid_, type_, node_,updateI);
#endif

    transformState (CODA_BOOTED, CODA_BOOTED, status_);
    if (status_ == CODA_SUCCESS) {
      established_ = 1;
      reporter->cmsglog (CMSGLOG_INFO,"%s booted ok\n", title_);
      bootTimer_->dis_arm ();
      timerCount_ = 0;
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
  }
}  
#endif


void
netComponent::transformState(int state1, int state2, int& status)
{
  // status is the result of the network call to a remote component
  // status is one of the corrected state if success.
  // status is < 0 if the call failed

#ifdef DEBUG_MSGS
  printf("netComponent::transformState reached\n");
#endif

  if(status == state1 || status == state2)
  {
    setState (status);
    status = CODA_SUCCESS;
  }
  else if(status < 0)
  {
    status = CODA_ERROR;
  }
  else
  {
    setState (status);
    status = CODA_ERROR;
  }
}

daqSubSystem&
netComponent::subSystem (void) const
{
  return subsys_;
}


int 
netComponent::runScript(char *scriptName)
{
  int rtn_stat;
  // construct a user script which has X window Display information
  char realscript[1024];  // no way to exceed 1024 chars

  static void (*istat)(int);
  static void (*qstat)(int);

  daqRun* run = subsys_.system().run();

  ::sprintf (realscript, "setenv DISPLAY %s; ", run->controlDisplay());
  ::strcat  (realscript, scriptName);
  // printf("script is: %s\n", realscript);

  // start up a child process 
  ::fflush (stdout);
  int tty = ::open ("/dev/tty", 2);
  if(tty == -1)
  {
    fprintf (stderr, "%s: Cannot open /dev/tty \n", realscript);
    exit (1);
  }

  pid_t pid = ::vfork ();
  // child process will not duplicate parent address space, so
  // in the child process one has to call exec () system call
  // quickly
  if(pid == -1)
  {
    // unable to fork
    return CODA_ERROR;
  }
  else if(pid == 0)
  { 
    // child process
    close (0); dup (tty);
    close (1); dup (tty);
    close (2); dup (tty);
    close (tty);

    return (execlp ("csh", "csh", "-c", realscript, (char *)0));
  }


  // parent process
  close (tty);

  // let the child catch signal
#ifndef __ultrix
  istat = ::signal (SIGINT, SIG_IGN);
  qstat = ::signal (SIGQUIT, SIG_IGN);
#endif

  int wpid, status;
  signed char estatus;

  while ((wpid = ::waitpid (pid, &status, 0)) != pid && wpid != -1)
  {
    ;// no-op.
  }

  // Hysterical past - Ask Jie Chen, I don't know - RWM!
  estatus = (signed char) (status>>8)&0xff;

#ifdef _CODA_DEBUG
  printf("Script terminated with status %x estatus = %d\n",status,estatus);
#endif

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

#ifndef __ultrix
  ::signal (SIGINT, istat);
  ::signal (SIGQUIT, qstat);
#endif
  return rtn_stat;
}
