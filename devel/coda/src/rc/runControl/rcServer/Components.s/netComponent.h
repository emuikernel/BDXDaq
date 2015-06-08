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
//      Data Acquisition Network Components
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: netComponent.h,v $
//   Revision 1.6  1999/07/28 19:17:40  rwm
//   Bunch of hacks to support Pause script button for Hall B.
//
//   Revision 1.5  1998/11/05 20:11:51  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.4  1997/01/24 16:36:05  chen
//   change/add Log Component for 1.4
//
//   Revision 1.3  1996/12/04 18:32:46  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.2  1996/10/28 14:23:05  heyes
//   communication with ROCs changed
//
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#ifndef _CODA_NET_COMPONENT_H
#define _CODA_NET_COMPONENT_H

#ifdef _CODA_USE_THREADS
#include <pthread.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include <daqSubSystem.h>
#include <daqComponent.h>
#include <netConfig.h>
#include <netCompBootTimer.h>

class netConfig;

#ifdef _CODA_USE_THREADS
class locker;
#endif

class netComponent: public daqComponent
{
public:
  // constructors and destructor
  // will be changed to Config_Info Structure
  netComponent  (char* title, int number, int expid, char* type, 
		 char* node, char* bootString, daqSubSystem& subsys);
  virtual ~netComponent (void);

  // inherited operations
  virtual int  attach    (void);
  virtual int  detach    (void);

  virtual int  boot      (void);
  virtual int  terminate (void);
  virtual int  status    (void) const;

  virtual int  configure (void);
  virtual int  download  (void);
  virtual int  go        (void);
  virtual int  pause     (void);
  virtual int  prestart  (void);
  virtual int  end       (void);
  virtual int  verify    (void);
  virtual int  reset     (void);
  // get state from Itcl object
  virtual int  state     (void);
  // get real state off network
  virtual int  state2    (void);
  // waiting for connection from the component
  // return 1: connected
  // return 0: still trying
  // return -1: timedout
  virtual int  waitForConnection (void);
  // redefine cancel transition.
  virtual void cancelTransition  (void);

  // return subsystem
  daqSubSystem& subSystem (void) const;

  // set component state to data variable
  virtual void setStatusVariable (int st);

  // class name
  virtual const char* className (void) const {return "netComponent";}
  int runScript(char *scriptName);


protected:
  // deny access to copy and assignment operator
  netComponent (const netComponent& comp);
  netComponent& operator = (const netComponent& comp);

  // check state information after state transition
  void transformState (int state1, int state2, int& status);
  // build boot information associated with this component
  void buildBootInfo (void);

  // data areas

  // daqSubSystem
  daqSubSystem& subsys_;
  // unique number
  int  number_;
  // expeirment id
  int  expid_;
  // network established flag
  int  established_;
  // class type
  char *type_;
  // host name for this component
  char *node_;
  // bootstring for this component
  char *bootString_;

  char* config_; // just a pointer
  char  bootinfo_[256];

  // timer tick interval
  static int tickInterval;
  // timeout limit
  static int timeoutLimit;
  // True if rcServer is Paused. bugbug: Quick hack.
  static int IsPaused;
  // connection number retries
  static int numConn_;

  int oldUpdateI_;
  // update interval

#ifdef _CODA_USE_THREADS
  // lock/unlock function 
  void         lock   (void);
  void         unlock (void);

  // creation and cancellation of threads
  int          createDetachedThread   (void * (*)(void *), pthread_t*);
  void         cancelTransitionThread (void);

  void         cancelTransitionThread_i (void);
  
  // state transition thread
  static void* bootThread       (void *);

  pthread_t    thr_;
  int          thrCreated_;
  pthread_mutex_t lock_;

  // friend class
  friend class locker;
#else
  virtual void              timerCallback (void);
  netCompBootTimer*         bootTimer_;
  int                       timerCount_;

  friend class              netCompBootTimer;
#endif
};

#ifdef _CODA_USE_THREADS
class locker
{
public:
  locker (netComponent* comp);
  ~locker (void);

private:
  netComponent* comp_;
};
#endif


#endif
