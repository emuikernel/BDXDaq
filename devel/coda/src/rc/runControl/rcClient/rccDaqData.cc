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
//      Implementation of rccDaqData Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rccDaqData.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:32  chen
//   run control source
//
//
#include "rccDaqData.h"

rccDaqData::rccDaqData (char* compname, char* attrname, int data)
:daqData (compname, attrname, data)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rccDaqData Class Object\n");
#endif
  // empty
}

rccDaqData::rccDaqData (char* compname, char* attrname, float data)
:daqData (compname, attrname, data)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rccDaqData Class Object\n");
#endif
  // empty
}

rccDaqData::rccDaqData (char* compname, char* attrname, double data)
:daqData (compname, attrname, data)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rccDaqData Class Object\n");
#endif
  // empty
}

rccDaqData::rccDaqData (char* compname, char* attrname, char* data)
:daqData (compname, attrname, data)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rccDaqData Class Object\n");
#endif
  // empty
}

rccDaqData::rccDaqData (char* compname, char* attrname, daqArbStruct* data)
:daqData (compname, attrname, data)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rccDaqData Class Object\n");
#endif
  // empty
}

rccDaqData::rccDaqData (char* compname, char* attrname, int* data, int count)
:daqData (compname, attrname, data, count)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rccDaqData Class Object\n");
#endif
  // empty
}

rccDaqData::rccDaqData (char* compname, char* attrname, float* data, int count)
:daqData (compname, attrname, data, count)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rccDaqData Class Object\n");
#endif
  // empty
}

rccDaqData::rccDaqData (char* compname, char* attrname, 
			double* data, int count)
:daqData (compname, attrname, data, count)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rccDaqData Class Object\n");
#endif
  // empty
}

rccDaqData::rccDaqData (char* compname, char* attrname, char** data, int count)
:daqData (compname, attrname, data, count)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rccDaqData Class Object\n");
#endif
  // empty
}

rccDaqData::~rccDaqData (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rccDaqData Class Object\n");
#endif
  removeCallbacks ();
}

int
rccDaqData::hasMonCallback (codaRcCallback& callback)
{
  codaSlistIterator ite (channels_);
  codaRcCallback* cbk = 0;

  for (ite.init (); !ite; ++ite) {
    cbk = (codaRcCallback *)ite ();
    if (callback == *cbk)
      return 1;
  }
  return 0;
}

codaRcCallback*
rccDaqData::monCallback (codaRcCallback& callback)
{
  codaSlistIterator ite (channels_);
  codaRcCallback* cbk = 0;

  for (ite.init (); !ite; ++ite) {
    cbk = (codaRcCallback *)ite ();
    if (callback == *cbk)
      return cbk;
  }
  return 0;  
}

void
rccDaqData::notifyDisconnection (void)
{
  codaSlistIterator ite (channels_);
  codaRcCallback* cbk = 0;
  rcCallback func = 0;
  void* arg = 0;

  for (ite.init (); !ite; ++ite) {
    cbk = (codaRcCallback *)ite ();
    func = cbk->callbackFunction ();
    arg = cbk->userarg ();
    /*(*func)(CODA_ERROR, arg, &(daqNetData)(*this));*/
    (*func)(CODA_ERROR, arg, (daqNetData *)this);
  }
}

void
rccDaqData::removeGetCbk (void *id)
{
  daqData::removeGetCbk (id);
  codaRcCallback* cbk = (codaRcCallback *)id;
  delete cbk;
}

void
rccDaqData::removeSetCbk (void *id)
{
  daqData::removeSetCbk (id);
  codaRcCallback* cbk = (codaRcCallback *)id;
  delete cbk;
}

void
rccDaqData::removeMonOffCbk (void *id)
{
  daqData::removeMonOffCbk (id);
  codaRcCallback* cbk = (codaRcCallback *)id;
  delete cbk;
}

void
rccDaqData::monitorOff (void *id)
{
  daqData::monitorOff (id);
  codaRcCallback* cbk = (codaRcCallback *)id;
  delete cbk;
}

void
rccDaqData::removeCallbacks (void)
{
  codaSlistIterator ite (channels_);
  codaRcCallback *item = 0;

  for (ite.init(); !ite; ++ite) {
    item = (codaRcCallback *)ite ();
    delete item;
  }
  
  codaSlistIterator ite1 (getCbkList_);
  for (ite1.init(); !ite1; ++ite1) {
    item = (codaRcCallback *)ite1 ();
    delete item;
  }

  codaSlistIterator ite2 (setCbkList_);
  for (ite2.init(); !ite2; ++ite2) {
    item = (codaRcCallback *)ite2 ();
    delete item;
  }

  codaSlistIterator ite3 (offCbkList_);
  for (ite3.init(); !ite3; ++ite3) {
    item = (codaRcCallback *)ite3 ();
    delete item;
  }
  // empty all list
  channels_.deleteAllValues ();
  getCbkList_.deleteAllValues ();
  setCbkList_.deleteAllValues ();
  offCbkList_.deleteAllValues ();
}
