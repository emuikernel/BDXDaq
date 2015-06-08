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
//      Pause State Transition Class. Perform Pause State Transition On
//      DAQ Sub System
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysPauser.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#ifndef _CODA_SUBSYS_PAUSER_H
#define _CODA_SUBSYS_PAUSER_H

#include <subSysTransitioner.h>

class subSysPauser: public subSysTransitioner
{
public:
  // constructor and destructor
  subSysPauser  (daqSubSystem* subsys);
  ~subSysPauser (void);

  virtual const char* className (void) const {return "subSysPauser";}

protected:
  // all inheried operations
  virtual int  action (void) const;
  virtual void executeItem (daqComponent* comp);
  virtual int  failureState (void);
  virtual int  successState (void);

private:
  //deny access to copy and assignment operations
  subSysPauser (const subSysPauser&);
  subSysPauser& operator = (const subSysPauser&);
};
#endif
