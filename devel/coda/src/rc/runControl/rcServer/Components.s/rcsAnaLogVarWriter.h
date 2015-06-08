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
//      CODA RunControl data variable ANA Log Writer
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcsAnaLogVarWriter.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:20  chen
//   run control source
//
//
#ifndef _CODA_RCS_ANALOG_WRITER_H
#define _CODA_RCS_ANALOG_WRITER_H

#include <daqDataWriter.h>

class daqRun;

class rcsAnaLogVarWriter: public daqDataWriter
{
public:
  // constructor and destructor
  rcsAnaLogVarWriter  (daqRun* run);
  ~rcsAnaLogVarWriter (void);

  virtual void write (daqData* data);

private:
  daqRun* run_;
};
#endif
