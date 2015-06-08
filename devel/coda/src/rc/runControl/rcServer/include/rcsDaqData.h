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
//      CODA RunControl Server Data Variable Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcsDaqData.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:19  chen
//   run control source
//
//
#ifndef _CODA_RCS_DAQ_DATA_H
#define _CODA_RCS_DAQ_DATA_H

#include <daqData.h>
#include <rccCmdBuffer.h>

class rcsDaqData: public daqData
{
public:
  // constructors and destructor
  rcsDaqData (char* compname, char* attrname, int    data);
  rcsDaqData (char* compname, char* attrname, float  data);
  rcsDaqData (char* compname, char* attrname, double data);
  rcsDaqData (char* compname, char* attrname, char*  data);
  rcsDaqData (char* compname, char* attrname, daqArbStruct*  data);
  // constructors for array of elements
  rcsDaqData (char* compname, char* attrname, int* data, int count);
  rcsDaqData (char* compname, char* attrname, float* data, int count);
  rcsDaqData (char* compname, char* attrname, double* data, int count);
  rcsDaqData (char* compname, char* attrname, char** data, int count);

  // remove a monitor on callback by callback id
  // return CODA_SUCCESS on succsss, CODA_ERROR for failure
  int removeMonCallback (long cbkid);
  // remove different callbacks by client id
  // remove CODA_SUCCESS on success, CODA_ERROR for failure
  int removeMonCallbacks (rccIO* cid);
  int removeGetCallbacks (rccIO* cid);
  int removeSetCallbacks (rccIO* cid);

  // in case of any change in value, send new value to monitors
  virtual void notifyChannels (void);

  virtual ~rcsDaqData (void);

  // class name
  virtual const char *className(void) const {return "rcsDaqData";}
};
#endif
 
   
