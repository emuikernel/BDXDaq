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
//      CODA RunControl Client Data Variable Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rccDaqData.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:32  chen
//   run control source
//
//
#ifndef _CODA_RCC_DAQ_DATA_H
#define _CODA_RCC_DAQ_DATA_H

#include <daqData.h>
#include <codaRcCallback.h>

class rccDaqData: public daqData
{
public:
  // constructors and destructor
  rccDaqData (char* compname, char* attrname, int    data);
  rccDaqData (char* compname, char* attrname, float  data);
  rccDaqData (char* compname, char* attrname, double data);
  rccDaqData (char* compname, char* attrname, char*  data);
  rccDaqData (char* compname, char* attrname, daqArbStruct*  data);
  // constructors for array of elements
  rccDaqData (char* compname, char* attrname, int* data, int count);
  rccDaqData (char* compname, char* attrname, float* data, int count);
  rccDaqData (char* compname, char* attrname, double* data, int count);
  rccDaqData (char* compname, char* attrname, char** data, int count);

  virtual ~rccDaqData (void);

  // check whether this data has this callback
  int hasMonCallback (codaRcCallback& callback);
  // return first  monitor callback pointer stored inside the data
  codaRcCallback* monCallback (codaRcCallback& callback);
  // notify disconnection of this data to all monitored callback
  void notifyDisconnection (void);

  // redefine remove callbacks : call hasCallbacks before calling the following
  virtual void removeGetCbk    (void *id);
  virtual void removeSetCbk    (void *id);
  virtual void monitorOff      (void *id);
  virtual void removeMonOffCbk (void *id);

  // remove all callbacks
  void         removeCallbacks (void);

  // class name
  virtual const char *className(void) const {return "rccDaqData";}
};
#endif
