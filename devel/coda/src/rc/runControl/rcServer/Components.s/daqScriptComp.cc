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
//      CODA script component implmentation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqScriptComp.cc,v $
//   Revision 1.10  1999/07/28 19:17:39  rwm
//   Bunch of hacks to support Pause script button for Hall B.
//
//   Revision 1.9  1998/11/05 20:11:44  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.8  1997/12/15 16:14:41  abbottd
//   Changes to allow global transition scripts to work
//
//   Revision 1.7  1997/12/12 14:13:13  heyes
//   fix status from waitpid
//
//   Revision 1.6  1997/06/17 19:29:12  heyes
//   for larry
//
//   Revision 1.5  1997/03/05 20:24:51  chen
//   setenv DD_NAME to session name
//
//   Revision 1.4  1996/12/04 18:32:43  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.3  1996/11/13 18:42:55  chen
//   Let main thread to giveup CPU for script thread
//
//   Revision 1.2  1996/10/14 20:02:46  heyes
//   changed message system
//
//   Revision 1.1.1.1  1996/10/11 13:39:20  chen
//   run control source
//
//
#include "daqScriptComp.h"
#include <rcMsgReporter.h>
#include <daqRun.h>
#include <transitioner.h>

#undef _CODA_USE_THREADS

#ifdef _CODA_USE_THREADS
void *
daqScriptComp::scriptThread (void* arg)
{
  char *user_env;

  user_env = ::getenv("USER_ENV");

  if (user_env == NULL)
    user_env = "none";

  // signal mask set
  sigset_t sig_set, oldset;
  sigemptyset (&sig_set);
  // add SIGINT and SIGQUIT to the sigset
  sigaddset (&sig_set, SIGINT);
  sigaddset (&sig_set, SIGQUIT);

  daqScriptComp* comp = (daqScriptComp *)arg;
  daqSystem& system = comp->subsys_.system ();

  // construct a user script which has X window Display information
  char     realscript[1024];  // no way to exceed 1024 chars
  ::sprintf (realscript, "setenv USER_ENV %s;setenv DISPLAY %s; setenv DD_NAME %s; ",
	     user_env, system.run()->controlDisplay(), system.run()->exptname () );
  ::strcat  (realscript, comp->script_);

//#ifdef _CODA_DEBUG
  printf ("real script is: %s\n", realscript);
//#endif

  // start up a child process 
  ::fflush (stdout);
  int tty = ::open ("/dev/tty", 2);
  if (tty == -1) {
    fprintf (stderr, "%s: Cannot open /dev/tty \n", realscript);
    exit (1);
  }
  pid_t pid = ::vfork ();
  // child process will not duplicate parent address space, so
  // in the child process one has to call exec () system call
  // quickly
  if (pid == -1) { // unable to fork
    // disable canceling
    pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, 0);

    pthread_mutex_lock (&comp->lock_);
    comp->setState (comp->failureState_);
    comp->thrCreated_ = 0;
    comp->tellTransitionerToMove ();
    pthread_mutex_unlock (&comp->lock_);

    reporter->cmsglog (CMSGLOG_ERROR,"Thread %d: unable to fork a child process\n",
			     pthread_self ());

    // enable canceling
    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, 0);
    return 0;
  }
  else if (pid == 0) { // child process
    close (0); dup (tty);
    close (1); dup (tty);
    close (2); dup (tty);
    close (tty);
    execlp ("csh", "csh", "-c", realscript, (char *)0);
    exit (127);
  }
  // parent process
  close (tty);

  // let the child catch signal
  pthread_sigmask (SIG_SETMASK, &sig_set, &oldset);

  // set cancel handler for this thread
  int w, status;
  // waitpid is a cancel point for this thread
  while ((w = ::waitpid (pid, &status, 0)) != pid && w != -1)
    ;
  // disable canceling
  pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, 0);  
  if (w == -1) {
    pthread_mutex_lock (&comp->lock_);
    comp->setState (comp->failureState_);
    comp->thrCreated_ = 0;
    comp->tellTransitionerToMove ();
    pthread_mutex_unlock (&comp->lock_);

    reporter->cmsglog (CMSGLOG_ERROR,"Thread %d: Child process died ......\n");
  }
  else {
    pthread_mutex_lock (&comp->lock_);
    comp->setState (comp->successState () );
    comp->thrCreated_ = 0;
    comp->tellTransitionerToMove ();
    pthread_mutex_unlock (&comp->lock_);

    reporter->cmsglog (CMSGLOG_INFO1,"Thread %d: finished running script\n",
			     pthread_self ());
  }
  pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, 0);

  // reset signal handler
  pthread_sigmask (SIG_SETMASK, &oldset, 0);

  return 0;
}  
#endif

daqScriptComp::daqScriptComp (char* title, int action, int pri,
			      char* script,
			      daqSubSystem& subsys,
			      daqComponent& comp)
:daqComponent (title), action_ (action), successState_ (CODA_DORMANT),
 failureState_ (CODA_DORMANT), subsys_ (subsys), comp_ (comp)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create daqScriptComp class object\n");
#endif
  script_ = new char[::strlen (script) + 1];
  ::strcpy (script_, script);
  priority (pri);

  subsys_.addComponent (this);
  daqSystem& system = subsys_.system ();
  system.addComponent (this);

#ifdef _CODA_USE_THREADS
  pthread_mutex_init (&lock_, 0);
  thrCreated_ = 0;
#endif
}

daqScriptComp::~daqScriptComp (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete daqScriptComp class object\n");
#endif
  delete []script_;

#ifdef _CODA_USE_THREADS
  pthread_mutex_destroy (&lock_);
  thrCreated_ = 0;
#endif
}

int
daqScriptComp::attach (void)
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
daqScriptComp::detach (void)
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

daqSubSystem&
daqScriptComp::subSystem (void) const
{
  return subsys_;
}

void
daqScriptComp::askTransitionerToWait (void)
{
#ifdef _CODA_USE_THREADS
  daqSystem& system = subsys_.system ();
  
  transitioner* tr = system.currTransitioner ();

  if (tr) {
    reporter->cmsglog (CMSGLOG_INFO,"Transitioner will wait for script %s\n", title_);
    tr->waitForScript ();
  }
#endif
}

void
daqScriptComp::tellTransitionerToMove (void)
{
#ifdef _CODA_USE_THREADS
  daqSystem& system = subsys_.system ();
  
  transitioner* tr = system.currTransitioner ();

  if (tr) {
    reporter->cmsglog (CMSGLOG_INFO,"Transitioner finished waiting for script %s\n",
			     title_);  
    tr->noWaitForScript ();
  }
#endif
}

void
daqScriptComp::cancelTransition (void)
{
  daqComponent::cancelTransition ();
#ifdef _CODA_USE_THREADS
  if (thrCreated_) {
    pthread_mutex_lock (&lock_);
    pthread_cancel (thr_);
    thrCreated_ = 0;
    setState (failureState_);
    pthread_mutex_unlock (&lock_);
  }
#endif
}

int
daqScriptComp::boot (void)
{
  status_ = CODA_SUCCESS;

  if (action_ == CODA_BOOT_ACTION) {
    setState (CODA_DORMANT);
    setupStateInfo (action_);
    reporter->cmsglog (CMSGLOG_INFO,"%s boot underway......\n", title_);
    askTransitionerToWait ();
    status_ = daqScriptComp::doScript (subsys_.system(), script_, this);
  }
#ifdef _CODA_USE_THREADS
  else {
    reporter->cmsglog (CMSGLOG_INFO,"%s booted ok\n", title_);
    setState (CODA_BOOTED);
  }
  return CODA_SUCCESS;
#else
  if (status_ == CODA_SUCCESS){
    reporter->cmsglog (CMSGLOG_INFO,"%s booted ok\n", title_);

    setState (CODA_BOOTED);
  }
  else {
    reporter->cmsglog (CMSGLOG_ERROR,"%s boot failed\n", title_);
    setState (CODA_DORMANT);
  }
  return CODA_SUCCESS;
#endif
}

int
daqScriptComp::terminate (void)
{
  status_ = CODA_SUCCESS;

  if (action_ == CODA_TERMINATE_ACTION) {
    setupStateInfo (action_);

    reporter->cmsglog (CMSGLOG_INFO,"%s terminate underway......\n", title_); 
    askTransitionerToWait ();
    status_ = daqScriptComp::doScript (subsys_.system(), script_, this);
  }
#ifdef _CODA_USE_THREADS
  else {
    reporter->cmsglog (CMSGLOG_INFO,"%s terminated ok\n", title_);
    setState (CODA_DORMANT);
  }
  return CODA_SUCCESS;
#else
  if (status_ == CODA_SUCCESS){
    reporter->cmsglog (CMSGLOG_INFO,"%s terminated ok\n", title_);
    setState (CODA_DORMANT);
  }
  else {
    reporter->cmsglog (CMSGLOG_WARN,"%s terminate failed\n", title_);
    setState (CODA_DORMANT);
  }
  status_ = CODA_SUCCESS;
  return CODA_SUCCESS;
#endif
}

int
daqScriptComp::status (void) const
{
  return status_;
}

int
daqScriptComp::configure (void)
{
  status_ = CODA_SUCCESS;

  if (action_ == CODA_CONFIGURE_ACTION) {
    setState (CODA_BOOTED);
    setupStateInfo (action_);
    reporter->cmsglog (CMSGLOG_INFO,"%s configure underway......\n", title_);    
    askTransitionerToWait ();
    status_ = daqScriptComp::doScript (subsys_.system(), script_, this);
  }
#ifdef _CODA_USE_THREADS
  else {
    reporter->cmsglog (CMSGLOG_INFO,"%s configured ok\n", title_);

    setState (CODA_CONFIGURED);
  }
  return CODA_SUCCESS;
#else
  if (status_ == CODA_SUCCESS){
    reporter->cmsglog (CMSGLOG_INFO,"%s configured ok\n", title_);

    setState (CODA_CONFIGURED);
  }
  else {
    reporter->cmsglog (CMSGLOG_WARN,"%s configure failed\n", title_);
    setState (CODA_DORMANT);
  }
  return CODA_SUCCESS;
#endif
}

int
daqScriptComp::download (void)
{
  status_ = CODA_SUCCESS;

  if (action_ == CODA_DOWNLOAD_ACTION) {
    setState (CODA_CONFIGURED);
    setupStateInfo (action_);
    reporter->cmsglog (CMSGLOG_INFO,"%s download underway......\n", title_);    
    askTransitionerToWait ();
    status_ = daqScriptComp::doScript (subsys_.system(), script_, this);
  }
#ifdef _CODA_USE_THREADS
  else {
    reporter->cmsglog (CMSGLOG_INFO,"%s downloaded ok\n", title_);
    setState (CODA_DOWNLOADED);
  }
  return CODA_SUCCESS;
#else
  if (status_ == CODA_SUCCESS){
    reporter->cmsglog (CMSGLOG_INFO,"%s downloaded ok\n", title_);
    setState (CODA_DOWNLOADED);
  }
  else {
    reporter->cmsglog (CMSGLOG_WARN,"%s download failed\n", title_);
    setState (CODA_CONFIGURED);
  }
  return CODA_SUCCESS;
#endif
}

int
daqScriptComp::go (void)
{
  status_ = CODA_SUCCESS;

  if (action_ == CODA_GO_ACTION) {
    setupStateInfo (action_);
    reporter->cmsglog (CMSGLOG_INFO,"%s go underway......\n", title_);
    askTransitionerToWait ();
    status_ = daqScriptComp::doScript (subsys_.system(), script_, this);
  }
#ifdef _CODA_USE_THREADS
  else {
    reporter->cmsglog (CMSGLOG_INFO,"%s activated ok\n", title_);
    setState (CODA_ACTIVE);
  }
  return CODA_SUCCESS;
#else
  if (status_ == CODA_SUCCESS){
    reporter->cmsglog (CMSGLOG_INFO,"%s activated ok\n", title_);
    setState (CODA_ACTIVE);
  }
  else {
    reporter->cmsglog (CMSGLOG_WARN,"%s active failed\n", title_);
    setState (CODA_PAUSED);
  }
  return CODA_SUCCESS;
#endif
}

int
daqScriptComp::pause (void)
{
  status_ = CODA_SUCCESS;

  if (action_ == CODA_PAUSE_ACTION) {
    setupStateInfo (action_);
    reporter->cmsglog (CMSGLOG_INFO,"%s pause underway......\n", title_);
    askTransitionerToWait ();
    status_ = daqScriptComp::doScript (subsys_.system (), script_, this);
  }
#ifdef _CODA_USE_THREADS
  else {
    reporter->cmsglog (CMSGLOG_INFO,"%s paused ok\n", title_);
    setState (CODA_PAUSED);
  }
  return CODA_SUCCESS;
#else
  if (status_ == CODA_SUCCESS){
    reporter->cmsglog (CMSGLOG_INFO,"%s paused ok\n", title_);
    setState (CODA_PAUSED);
  }
  else {
    reporter->cmsglog (CMSGLOG_WARN,"%s pause failed\n", title_);
    setState (CODA_ACTIVE);
  }
  return CODA_SUCCESS;
#endif
}  

int
daqScriptComp::prestart (void)
{
  status_ = CODA_SUCCESS;

  if (action_ == CODA_PRESTART_ACTION) {
    setupStateInfo (action_);
    reporter->cmsglog (CMSGLOG_INFO,"%s prestart underway......\n", title_);
    askTransitionerToWait ();

    status_ = daqScriptComp::doScript (subsys_.system(), script_, this);
  }
#ifdef _CODA_USE_THREADS
  else {
    reporter->cmsglog (CMSGLOG_INFO,"%s prestarted ok\n", title_);
    setState (CODA_PAUSED);
  }
  return CODA_SUCCESS;
#else
  if (status_ == CODA_SUCCESS){
    reporter->cmsglog (CMSGLOG_INFO,"%s prestarted ok\n", title_);
    setState (CODA_PAUSED);
  }
  else {
    reporter->cmsglog (CMSGLOG_WARN,"%s prestart failed\n", title_);
    setState (CODA_DOWNLOADED);
  }
  return CODA_SUCCESS;
#endif
}

int
daqScriptComp::reset (void)
{
  return terminate ();
}

int
daqScriptComp::end (void)
{
  status_ = CODA_SUCCESS;

  if (action_ == CODA_END_ACTION) {
    setupStateInfo (action_);
    reporter->cmsglog (CMSGLOG_INFO,"%s end underway......\n", title_);
    askTransitionerToWait ();
    status_ = daqScriptComp::doScript (subsys_.system (), script_, this);
  }
#ifdef _CODA_USE_THREADS
  else {
    reporter->cmsglog (CMSGLOG_INFO,"%s ended ok\n", title_);
    setState (CODA_DOWNLOADED);
  }
  return CODA_SUCCESS;
#else
  if (status_ == CODA_SUCCESS){
    reporter->cmsglog (CMSGLOG_INFO,"%s ended ok\n", title_);
    setState (CODA_DOWNLOADED);
  }
  else {
    reporter->cmsglog (CMSGLOG_WARN,"%s end failed\n", title_);
    setState (prevState_);
  }
  return CODA_SUCCESS;
#endif
}

int
daqScriptComp::verify (void)
{
  return CODA_SUCCESS;
}

int
daqScriptComp::state (void)
{
#ifdef _CODA_DEBUG
  printf ("script component %s has state %d\n", title_, state_);
#endif
  return state_;
}

void
daqScriptComp::setupStateInfo (int action)
{
  switch (action){
  case CODA_BOOT_ACTION:
    successState_ = CODA_BOOTED;
    failureState_ = CODA_DORMANT;
    break;
  case CODA_TERMINATE_ACTION:
    successState_ = CODA_DORMANT;
    failureState_ = CODA_DORMANT;
    break;
  case CODA_CONFIGURE_ACTION:
    successState_ = CODA_CONFIGURED;
    failureState_ = CODA_DORMANT;
    break;
  case CODA_DOWNLOAD_ACTION:
    successState_ = CODA_DOWNLOADED;
    failureState_ = CODA_CONFIGURED;
    break;
  case CODA_GO_ACTION:
    successState_ = CODA_ACTIVE;
    failureState_ = CODA_PAUSED;
    break;
  case CODA_PAUSE_ACTION:
    successState_ = CODA_PAUSED;
    failureState_ = CODA_ACTIVE;
    break;
  case CODA_PRESTART_ACTION:
    successState_ = CODA_PAUSED;
    failureState_ = CODA_DOWNLOADED;
    break;
  case CODA_END_ACTION:
    successState_ = CODA_DOWNLOADED;
    failureState_ = prevState_;
    break;
  default:
    break;
  }
}

int
daqScriptComp::successState (void)
{
  if (action_ > CODA_DOWNLOAD_ACTION)
    return successState_;
  else {
    int cstate = comp_.state ();
    if (cstate != CODA_DORMANT)
      return comp_.state ();
    else
      return successState_;
  }
}

int
daqScriptComp::doScript (daqSystem& system, char *script, daqScriptComp* comp)
{
#ifdef _CODA_USE_THREADS
  comp->thrCreated_ = 1;
  puts("RWM: Using threads");

  // create a detached thread
  ::pthread_attr_t attr;
  ::pthread_attr_init (&attr);
  ::pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
  if (::pthread_create (&(comp->thr_), &attr, &(daqScriptComp::scriptThread),
			(void *)comp) == 0) {
    reporter->cmsglog (CMSGLOG_INFO,"Create a new thread %d for script\n", 
		       comp->thr_);
    ::pthread_attr_destroy (&attr);
  }
  else
    reporter->cmsglog (CMSGLOG_ERROR,"Unable to create a new thread \n");

  // RWM: see if sleeping for shorter time make transitions faster.
  //sleep (1);  // give up CPU to let thread to run for a while
  sleep (10);  // give up CPU to let thread to run for a while

  return CODA_SUCCESS;
#else
  int st = CODA_SUCCESS;
  static void (*istat)(int);
  static void (*qstat)(int);

  // construct a user script which has X window Display information
  char     realscript[1024];  // no way to exceed 1024 chars

  ::sprintf (realscript, "setenv DISPLAY %s; setenv DD_NAME %s; ",
	     system.run()->controlDisplay(),
	     system.run()->exptname () );
  ::strcat  (realscript, script);
  //#ifdef _CODA_DEBUG
  printf ("rreal script is: %s\n", realscript);
  //#endif

  // start up a child process 
  ::fflush (stdout);
  int tty = ::open ("/dev/tty", 2);
  if (tty == -1) {
    fprintf (stderr, "%s: Cannot open /dev/tty \n", realscript);
    exit (1);
  }
  pid_t pid = ::vfork ();
  // child process will not duplicate parent address space, so
  // in the child process one has to call exec () system call
  // quickly
  if (pid == -1) // unable to fork
    return CODA_ERROR;
  else if (pid == 0) { // child process
    close (0); dup (tty);
    close (1); dup (tty);
    close (2); dup (tty);
    close (tty);
    execlp ("csh", "csh", "-c", realscript, (char *)0);
    exit (127);
  }
  // parent process
  close (tty);

  // let the child catch signal
#ifndef __ultrix
  istat = ::signal (SIGINT, SIG_IGN);
  qstat = ::signal (SIGQUIT, SIG_IGN);
#endif

  int w, status;
  signed char estatus;
  while ((w = ::waitpid (pid, &status, 0)) != pid && w != -1)
    ;

  estatus = (signed char) (status>>8)&0xff;
  printf("Script terminated with status %x estatus = %d\n",status,estatus);

  if ((w == -1) || (estatus == -2))
    st = CODA_ERROR;

#ifndef __ultrix
  ::signal (SIGINT, istat);
  ::signal (SIGQUIT, qstat);
#endif
  return st;
#endif
}
