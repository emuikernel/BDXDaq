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
//      CODA runControl daqArbStruct factory Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqArbStructFactory.h,v $
//   Revision 1.2  1996/11/04 16:14:51  chen
//   add options for monitoring components
//
//   Revision 1.1.1.1  1996/10/11 13:39:30  chen
//   run control source
//
//
#ifndef _CODA_DAQARB_STRUCT_FAC_H
#define _CODA_DAQARB_STRUCT_FAC_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <daqArbStruct.h>

class daqArbStructFactory
{
public:
  enum structId {BOOT_INFO = 100, RUNTYPE_INFO, MONITOR_INFO};

  // constructor and destructor
  daqArbStructFactory  (void);
  ~daqArbStructFactory (void);


  daqArbStruct* create (long id);

};
#endif
