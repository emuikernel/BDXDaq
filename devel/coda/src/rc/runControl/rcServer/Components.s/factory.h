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
//      A Factory Producing all DAQ SubSystems and Daq Components
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: factory.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:19  chen
//   run control source
//
//
#ifndef _CODA_FACTORY_H
#define _CODA_FACTORY_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <netComponent.h>
#include <daqSubSystem.h>
#include <daqSystem.h>

class factory
{
public:
  // constructor and destructor
  factory  (int exptid, daqSystem& system);
  ~factory (void);

  // create subsystem with class name 'className'
  daqSubSystem* createSubSystem (char* className);
  // create network component
  daqComponent* createComponent (char* title, int number, char* type,
				 char* node, char* bootString);
  // create user script component associated with one network component
  daqComponent* createComponent (daqComponent* comp, 
				 char* action, char* script);
				 
  // set subSystem priority
  void subSystemPriority (char* className, int priority);

private:
  int exptid_;
  daqSystem& system_;
  // deny access to copy and assignment
  factory (const factory& );
  factory& operator = (const factory&);
};
#endif
