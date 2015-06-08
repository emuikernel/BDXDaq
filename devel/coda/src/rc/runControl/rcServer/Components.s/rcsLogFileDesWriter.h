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
//      CODA RunControl data variable RCS logFileDescriptor writer
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcsLogFileDesWriter.h,v $
//   Revision 1.2  1996/12/04 18:32:48  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:20  chen
//   run control source
//
//
#ifndef _CODA_RCS_LOGFDES_WRITER_H
#define _CODA_RCS_LOGFDES_WRITER_H

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)

#include <daqDataWriter.h>

class daqRun;

class rcsLogFileDesWriter: public daqDataWriter
{
public:
  // constructor and destructor
  rcsLogFileDesWriter  (daqRun* run);
  ~rcsLogFileDesWriter (void);

  virtual void write   (daqData* data);

private:
  daqRun* run_;
};

#endif

#endif
