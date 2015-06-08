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
//      Implementation of rcsDaqData Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcsDaqData.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:20  chen
//   run control source
//
//
#include "rcsDaqData.h"

rcsDaqData::rcsDaqData (char* compname, char* attrname, int data)
:daqData (compname, attrname, data)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcsDaqData Class Object\n");
#endif
  // empty
}

rcsDaqData::rcsDaqData (char* compname, char* attrname, float data)
:daqData (compname, attrname, data)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcsDaqData Class Object\n");
#endif
  // empty
}

rcsDaqData::rcsDaqData (char* compname, char* attrname, double data)
:daqData (compname, attrname, data)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcsDaqData Class Object\n");
#endif
  // empty
}

rcsDaqData::rcsDaqData (char* compname, char* attrname, char* data)
:daqData (compname, attrname, data)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcsDaqData Class Object\n");
#endif
  // empty
}

rcsDaqData::rcsDaqData (char* compname, char* attrname, daqArbStruct* data)
:daqData (compname, attrname, data)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcsDaqData Class Object\n");
#endif
  // empty
}

rcsDaqData::rcsDaqData (char* compname, char* attrname, int* data, int count)
:daqData (compname, attrname, data, count)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcsDaqData Class Object\n");
#endif
  // empty
}

rcsDaqData::rcsDaqData (char* compname, char* attrname, float* data, int count)
:daqData (compname, attrname, data, count)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcsDaqData Class Object\n");
#endif
  // empty
}

rcsDaqData::rcsDaqData (char* compname, char* attrname, 
			double* data, int count)
:daqData (compname, attrname, data, count)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcsDaqData Class Object\n");
#endif
  // empty
}

rcsDaqData::rcsDaqData (char* compname, char* attrname, char** data, int count)
:daqData (compname, attrname, data, count)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcsDaqData Class Object\n");
#endif
  // empty
}

rcsDaqData::~rcsDaqData (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcsDaqData Class Object\n");
#endif
  codaSlistIterator ite (channels_);
  rcCmdBufItem *item = 0;

  for (ite.init(); !ite; ++ite) {
    item = (rcCmdBufItem *)ite ();
    delete item->cmsg;
    delete item;
  }
  
  codaSlistIterator ite1 (getCbkList_);
  for (ite1.init(); !ite1; ++ite1) {
    item = (rcCmdBufItem *)ite1 ();
    delete item->cmsg;
    delete item;
  }

  codaSlistIterator ite2 (setCbkList_);
  for (ite2.init(); !ite2; ++ite2) {
    item = (rcCmdBufItem *)ite2 ();
    delete item->cmsg;
    delete item;
  }
}

void
rcsDaqData::notifyChannels (void)
{
  codaSlistIterator ite (channels_);
  rcCmdBufItem *item = 0;

  for (ite.init(); !ite; ++ite) {
    item = (rcCmdBufItem *)ite ();
    item->channel->sendResult (item->cmsg->type (), 
			       data_, item->cmsg->reqId());
  }
}

int
rcsDaqData::removeMonCallback (long cbkid)
{
  codaSlistIterator ite (channels_);
  rcCmdBufItem *item = 0;
  int found = 0;

  for (ite.init(); !ite; ++ite) {
    item = (rcCmdBufItem *)ite ();
    if (item->cmsg->reqId() == cbkid) {
      ite.removeCurrent ();
      delete item->cmsg;
      delete item;
      found = 1;
      break;
    }
  }
  if (found) {
    // check whether this data is being monitored, if not, deavtivate it
    if (channels_.isEmpty ()) {
      deactivate ();
    }
    // return success status
    return CODA_SUCCESS;
  }
  else
    return CODA_ERROR;
}

int
rcsDaqData::removeGetCallbacks (rccIO *cid)
{
  int found = 0;
  rcCmdBufItem *item = 0;
  // go through all get callback list to check channel id
  codaSlistIterator site (getCbkList_);
  for (site.init (); !site; ++site) {
    item = (rcCmdBufItem *)site ();
    if (item->channel == (rccIO *)cid) {
      found = 1;
      site.removeCurrent ();
      delete item->cmsg;
      delete item;
    }
  }
  if (found)
    return CODA_SUCCESS;
  else
    return CODA_ERROR;
}

int
rcsDaqData::removeSetCallbacks (rccIO* cid)
{
  int found = 0;
  rcCmdBufItem *item = 0;
  // go through all set callback list to check channel id
  codaSlistIterator site1 (setCbkList_);
  for (site1.init (); !site1; ++site1) {
    item = (rcCmdBufItem *)site1 ();
    if (item->channel == (rccIO *)cid) {
      found = 1;
      site1.removeCurrent ();
      delete item->cmsg;
      delete item;
    }
  }
  if (found)
    return CODA_SUCCESS;
  else
    return CODA_ERROR;
}

int
rcsDaqData::removeMonCallbacks (rccIO *cid)
{
  int found = 0;
  rcCmdBufItem *item = 0;
  // go through all monitor callback list to check channel id
  codaSlistIterator site2 (channels_);
  for (site2.init (); !site2; ++site2) {
    item = (rcCmdBufItem *)site2 ();
    if (item->channel == (rccIO *)cid) {
      found = 1;
      site2.removeCurrent ();
      delete item->cmsg;
      delete item;
    }
  }
  if (found)
    return CODA_SUCCESS;
  else
    return CODA_ERROR;
}  
