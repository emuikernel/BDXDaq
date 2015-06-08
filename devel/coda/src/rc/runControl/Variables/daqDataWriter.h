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
//      daqDataWriter that writes a daqData to a database or file
//      Abstract Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqDataWriter.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:30  chen
//   run control source
//
//
#ifndef _CODA_DAQDATA_WRITER_H
#define _CODA_DAQDATA_WRITER_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

class daqData;

class daqDataWriter
{
public:
  // write function, default is empty
  virtual void write (daqData *data);

  virtual ~daqDataWriter (void);
  
protected:
  daqDataWriter (void);
};
#endif
