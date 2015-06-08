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
//      Data Acquisition Sub system class
//      
//      Subsystem are transitioned in decreasing priority. Subsystems
//      having same priority are transitioned in parallel, but
//      they must complete the transition prior to the next lower
//      priority subsystems being initiated
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqSubSystem.h,v $
//   Revision 1.2  1996/10/22 17:17:17  chen
//   fix bugs of boot stage different state among components
//
//   Revision 1.1.1.1  1996/10/11 13:39:17  chen
//   run control source
//
//
#ifndef _CODA_DAQ_SUBSYSTEM_H
#define _CODA_DAQ_SUBSYSTEM_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <daqConst.h>
#include <daqSystem.h>
#include "daqComponent.h"
#include "daqCompSortedList.h"

class subSysTransitioner;

class daqSubSystem: public daqComponent
{
public:
  // constructor and destructor
  daqSubSystem            (char *className, daqSystem& system);
  ~daqSubSystem           (void);

  // operations

  // add a component to this sub_system
  // return CODA_SUCCESS and CODA_WARNING
  int addComponent        (daqComponent* comp);
  // remove a component by component name
  // return CODA_SUCCESS and CODA_WARNING
  int removeComponent     (char* compName);
  // remove a component
  // return CODA_SUCCESS or CODA_WARNING
  int removeComponent     (daqComponent* comp);
  //  number of components
  int numComponents       (void);
  // remove all components
  int removeAllComponents (void);
  // first component in the system
  daqComponent* firstComponent (void);
  // first enabled component in the system: for coda 1.4 only
  daqComponent* firstEnabledComp (void);

  // set default priority for all transition stage
  void setDefaultPriority (int pri);
  // set priority for a particular action
  void setPriority        (int action, int pri);
  // get priority according to the action
  void  priorityByAction  (int action);

  // redefine attach and detach to system
  virtual int  attach     (void);
  virtual int  detach     (void);

  // check configuration information to enable right components
  virtual void enableRun  (void);
  virtual void enableAll  (void);
  virtual void disableAll (void);

  // all those inherited actions
  virtual int  boot       (void);
  virtual int  terminate  (void);
  virtual int  status     (void) const;
  
  virtual void abort      (int wanted);
  virtual int  configure  (void);
  virtual int  download   (void);
  virtual int  go         (void);
  virtual int  pause      (void);
  virtual int  prestart   (void);
  virtual int  end        (void);
  virtual int  verify     (void);
  virtual int  reset      (void);
  // redefine  state 
  virtual int  state      (void);
  
  // return state information at boot stage
  int          bootState  (void);

  // do state transition
  int         doTransition (subSysTransitioner* tran);

  // return current subSysTransitioner
  subSysTransitioner* transitioner (void);
  // return system reference
  daqSystem& system       (void) const;

  // return component list
  codaSlist& compList (void);
  
  // class name
  virtual const char* className (void) const {return "daqSubSystem";}

private:
  // daq system
  daqSystem& system_;
  // state value in case of transition failure
  int   summaryState_;
  
  // priorities in different transation stage
  int priorities_[CODA_NUM_ACTIONS];
  // sorted list containes or daqComponets
  daqCompSortedList compList_;
  // all those transitioners
  subSysTransitioner *aborter_;
  subSysTransitioner *activater_;
  subSysTransitioner *booter_;
  subSysTransitioner *configurer_;
  subSysTransitioner *downloader_;
  subSysTransitioner *ender_;
  subSysTransitioner *prestarter_;
  subSysTransitioner *pauser_;
  subSysTransitioner *resetter_;
  subSysTransitioner *terminater_;
  subSysTransitioner *verifier_;
  // current transitioner
  subSysTransitioner *currTransitioner_;
  // friend class
  friend class subSysTransitioner;
};
#endif
