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
//      Prestart State Transition Class. Perform Prestart State Transition On
//      DAQ Sub System
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysPrestarter.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#ifndef _CODA_SUBSYS_PRESTARTER_H
#define _CODA_SUBSYS_PRESTARTER_H

#include <subSysTransitioner.h>

class subSysPrestarter: public subSysTransitioner
{
public:
  // constructor and destructor
  subSysPrestarter  (daqSubSystem* subsys);
  ~subSysPrestarter (void);

  virtual const char* className (void) const {return "subSysPrestarter";}

protected:
  // all inheried operations
  virtual int  action (void) const;
  virtual void executeItem (daqComponent* comp);
  virtual int  failureState (void);
  virtual int  successState (void);

private:
  //deny access to copy and assignment operations
  subSysPrestarter (const subSysPrestarter&);
  subSysPrestarter& operator = (const subSysPrestarter&);
};
#endif
