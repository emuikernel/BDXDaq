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
//      Data Acquisition System Class. This contains two main entities:
//      
//      1) Global Hash table keyed by string. All daqComponent entites in
//         the system have unique titles by which they may be located
//
//      2) Logical subsystems. These subsystems contain lists of
//         daqComponents which might correspond to physical
//         hardware but which might correspond to logical entities such
//         as user code
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqSystem.h,v $
//   Revision 1.2  1999/07/28 19:17:39  rwm
//   Bunch of hacks to support Pause script button for Hall B.
//
//   Revision 1.1.1.1  1996/10/11 13:39:17  chen
//   run control source
//
//
#ifndef _CODA_DAQ_SYSTEM_H
#define _CODA_DAQ_SYSTEM_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <codaSlist.h>
#include <subSysSortedList.h>
#include <codaStrHash.h>
#include <daqTarget.h>

class daqRun;
class daqComponent;
class daqSubSystem;
class transitioner;
class transitionGraph;

class daqSystem: public daqTarget
{
public:
  // constructor and destructor
  daqSystem          (void);
  ~daqSystem         (void);

  // inherited state transition actions
  int          autostart (void);
  virtual int  boot      (void);
  virtual int  terminate (void);
  virtual int  status    (void) const;
  virtual int  configure (void);
  virtual int  download  (void);
  virtual int  go        (void);
  virtual int  pause     (void);
  virtual int  resume    (void);
  virtual int  prestart  (void);
  virtual int  end       (void);
  virtual int  verify    (void);
  virtual int  reset     (void);
  // auto transition from one state to another state
  int          autoTransition (int istate, int fstate);
  // cancel transition
  int          cancelTransition (void);
  
  // override set state function 
  // need to update daqRun status
  void setState                 (int newst);
  // set override state
  virtual void setOverrideState (int wanted);
  virtual void abort            (int wanted);
  
  // components manipulations
  // return CODA_SUCCESS on success or CODA_ERROR
  int          addComponent     (daqComponent *comp);
  int          removeComponent  (daqComponent *comp);
  int          removeComponent  (char         *title);
  // return all components, caller provide enough buffer
  int          allComponents    (daqComponent* cs[], int bufsize);  
  // caller free memory of cs[i]
  int          allComponents    (char* cs[], int bufsize);
  // return all enabled components, caller provide enough buffer
  // return number of enabled components found
  int          allEnabledComponents    (daqComponent* cs[], int bufsize);
  // caller allocate cs, but caller has to free cs[i]
  int          allEnabledComponents    (char* cs[], int bufsize);
  // enable/disable all components
  void         disableAllComponents    (void);
  void         enableAllComponents     (void);

  void         removeAllScriptComp (void);
  int          addSubSystem     (daqSubSystem* sys);
  int          removeSubSystem  (daqSubSystem* sys);
  int          has              (daqComponent* comp);
  int          has              (char *title, daqComponent* &comp);
  int          locateSystem     (char *className, daqSubSystem* &sys);
  // return pointer back to daqRun object
  daqRun*      run              (void);
  void         run              (daqRun* r);

  // whether the system is inside pause active loop
  int          pauseActiveLoop  (void) const;

  // check state information for all components.
  // return state and check status
  virtual      int checkState (void);

  // clean out everything in the table
  void         cleanAll         (void);

  // setup current transitioner
  void          currTransitioner     (transitioner* tran);
  transitioner* currTransitioner     (void) const;

  // class name
  virtual const char *className (void) const {return "daqSystem";}

private:
  // components pointers hash table
  codaStrHash  components_;
  // subSystem pointers linked list
  codaSlist    subsystems_;
  // flag to lock above list
  int          subsysLocked_;
  // flag of active state before paused state
  int          activeBeforePaused_;
  // pointer points back to daqRun object
  daqRun*      run_;
  // all those transitioners
  transitioner* activater_;
  transitioner* booter_;
  transitioner* configurer_;
  transitioner* downloader_;
  transitioner* ender_;
  transitioner* pauser_;
  transitioner* prestarter_;
  transitioner* resetter_;
  transitioner* terminater_;
  transitioner* verifier_;
  // current transitioner
  transitioner* currTransitioner_;
  // transition graph
  // this class must be created after all these transitioners
  transitionGraph* stateGraph_;
  // friend class
  friend class daqSubSystem;
  friend class transitioner;
  friend class transitionGraph;
};
#endif

