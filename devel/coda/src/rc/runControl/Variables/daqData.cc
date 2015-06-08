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
//      Generic Data Acquisition Data
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqData.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:30  chen
//   run control source
//
//
#include <daqDataWriter.h>
#include <daqDataTrigger.h>
#include "daqData.h"

daqData::daqData (char* compname, char* attrname, int data)
:data_ (compname, attrname, data), active_ (0), updater_ (0), trigger_ (0),
 doTrigger_ (0),  writer_ (0), channels_ (), getCbkList_ (), setCbkList_ (),
 offCbkList_ (), locked_ (0), writable_ (0), historyKept_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqData Class Object\n");
#endif
  constructKey (compname, attrname);
}

daqData::daqData (char* compname, char* attrname, float data)
:data_ (compname, attrname, data), active_ (0), updater_ (0), trigger_ (0),
 doTrigger_ (0), writer_ (0), channels_(),  getCbkList_ (), setCbkList_ (),
 offCbkList_ (), locked_ (0), writable_ (0), historyKept_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqData Class Object\n");
#endif
  constructKey (compname, attrname);
}

daqData::daqData (char* compname, char* attrname, double data)
:data_ (compname, attrname, data), active_ (0),updater_ (0), trigger_ (0),
 doTrigger_ (0), writer_ (0), channels_(), getCbkList_ (), setCbkList_ (),
 offCbkList_ (), locked_ (0), writable_ (0), historyKept_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqData Class Object\n");
#endif
  constructKey (compname, attrname);
}

daqData::daqData (char* compname, char* attrname, char* data)
:data_ (compname, attrname, data), active_ (0), updater_ (0), trigger_ (0),
 doTrigger_ (0), writer_ (0), channels_(), getCbkList_ (), setCbkList_ (),
 offCbkList_ (), locked_ (0), writable_ (0), historyKept_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqData Class Object\n");
#endif
  constructKey (compname, attrname);
}

daqData::daqData (char* compname, char* attrname, daqArbStruct* data)
:data_ (compname, attrname, data), active_ (0), updater_ (0), trigger_ (0),
 doTrigger_ (0), writer_ (0), channels_(), getCbkList_ (), setCbkList_ (),
 offCbkList_ (), locked_ (0), writable_ (0), historyKept_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqData Class Object\n");
#endif
  constructKey (compname, attrname);
}

daqData::daqData (char* compname, char* attrname, int* data, int count)
:data_ (compname, attrname, data, count), active_ (0), updater_ (0), 
 trigger_ (0), doTrigger_ (0), writer_ (0),channels_(),  getCbkList_ (), 
 setCbkList_ (), offCbkList_ (), locked_ (0), writable_ (0), historyKept_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqData Class Object\n");
#endif
  constructKey (compname, attrname);
}

daqData::daqData (char* compname, char* attrname, float* data, int count)
:data_ (compname, attrname, data, count), active_ (0), 
 updater_ (0), trigger_ (0), doTrigger_ (0), writer_ (0), 
 channels_(),  getCbkList_ (), setCbkList_ (), offCbkList_ (), 
 locked_ (0), writable_ (0), historyKept_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqData Class Object\n");
#endif
  constructKey (compname, attrname);
}

daqData::daqData (char* compname, char* attrname, double* data, int count)
:data_ (compname, attrname, data, count), active_ (0), 
 updater_ (0), trigger_ (0), doTrigger_ (0), writer_ (0), channels_(),  
 getCbkList_ (), setCbkList_ (), offCbkList_ (), 
 locked_ (0), writable_ (0), historyKept_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqData Class Object\n");
#endif
  constructKey (compname, attrname);
}

daqData::daqData (char* compname, char* attrname, char** data, int count)
:data_ (compname, attrname, data, count), active_ (0), 
 updater_ (0), trigger_ (0), doTrigger_ (0), writer_ (0), channels_(), 
 getCbkList_ (), setCbkList_ (), offCbkList_ (), locked_ (0), writable_ (0),
 historyKept_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqData Class Object\n");
#endif
  constructKey (compname, attrname);
}

daqData::~daqData (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete daqData Class Object\n");
#endif
  delete []key_;
  if (writer_)
    delete writer_;
  if (trigger_)
    delete trigger_;
  // all these elements inside list have to be freed by caller
  // derived class handles free all the stuff inside callback list
}

int
daqData::connect (daqDataManager& manager)
{
  return manager.addData (this);
}

int
daqData::disconnect (daqDataManager& manager)
{
  return manager.removeData (this);
}

void
daqData::updater (daqUpdater updater)
{
  updater_ = updater;
}

daqUpdater
daqData::updater (void) const
{
  return updater_;
}

void
daqData::trigger (daqDataTrigger* trig)
{
  trigger_ = trig;
}

void
daqData::disableTrigger (void)
{
  doTrigger_ = 0;
}

void
daqData::enableTrigger (void)
{
  doTrigger_ = 1;
}

void
daqData::writer (daqDataWriter* writer)
{
  writer_ = writer;
}

daqData::operator int (void)
{
  return (int)data_;
}

daqData::operator long (void)
{
  return (long)data_;
}

daqData::operator float (void)
{
  return (float)data_;
}

daqData::operator double (void)
{
  return (double)data_;
}

daqData::operator char* (void)
{
  return (char *)data_;
}

daqData::operator daqArbStruct* (void)
{
  return (daqArbStruct *)data_;
}

daqData::operator daqNetData& (void)
{
  return data_;
}

daqData&
daqData::operator = (int val)
{
  data_ = val;
  notifyChannels ();
  if (writable_ && writer_)
    writer_->write (this);
  return *this;
}

daqData&
daqData::operator = (long val)
{
  data_ = val;
  notifyChannels ();
  if (writable_ && writer_)
    writer_->write (this);
  return *this;
}

daqData&
daqData::operator = (float val)
{
  data_ = val;
  notifyChannels ();
  if (writable_ && writer_)
    writer_->write (this);
  return *this;
}

daqData&
daqData::operator = (double val)
{
  data_ = val;
  notifyChannels ();
  if (writable_ && writer_)
    writer_->write (this);
  return *this;
}

daqData&
daqData::operator = (char* val)
{
  data_ = val;
  notifyChannels ();
  if (writable_ && writer_)
    writer_->write (this);
  return *this;
}

daqData&
daqData::operator = (daqArbStruct* val)
{
  data_ = val;
  notifyChannels ();
  if (writable_ && writer_)
    writer_->write (this);
  return *this;
}


daqData&
daqData::operator = (const daqNetData& data)
{
  data_.assignData (data);
  notifyChannels ();
  if (writable_ && writer_)
    writer_->write (this);
  return *this;
}

void
daqData::assignData (int* data, int count)
{
  data_.assignData (data, count);
  notifyChannels ();
  if (writable_ && writer_)
    writer_->write (this);
}

void
daqData::assignData (float* data, int count)
{
  data_.assignData (data, count);
  notifyChannels ();
  if (writable_ && writer_)
    writer_->write (this);
}

void
daqData::assignData (double* data, int count)
{
  data_.assignData (data, count);
  notifyChannels ();
  if (writable_ && writer_)
    writer_->write (this);
}

void
daqData::assignData (char** data, int count)
{
  data_.assignData (data, count);
  notifyChannels ();
  if (writable_ && writer_)
    writer_->write (this);
}

int
daqData::getData (int data[], int& count)
{
  return data_.getData (data, count);
}

int
daqData::getData (float data[], int& count)
{
  return data_.getData (data, count);
}

int
daqData::getData (double data[], int& count)
{
  return data_.getData (data, count);
}

int
daqData::getData (char* data[], int& count)
{
  return data_.getData (data, count);
}
      
char*
daqData::attrname (void) const
{
  return data_.attribute ();
}

char*
daqData::compname (void) const
{
  return data_.name ();
}

char*
daqData::key (void) const
{
  return key_;
}

void
daqData::activate (void)
{
  active_ = 1;
}

void
daqData::deactivate (void)
{
  active_ = 0;
}

int
daqData::active (void) const
{
  return active_;
}

void
daqData::update (void)
{
  if (data_.count_ == 1) {
    switch (data_.type_){
    case CODA_INT:
      if (updater_) 
	(*updater_)(data_.compname_, data_.attrname_, 
		    (void *)&(data_.u_.ival), 1);
      break;
    case CODA_FLT:
      if (updater_) 
	(*updater_)(data_.compname_, data_.attrname_, 
		    (void *)&(data_.u_.fval), 1);
      break;
    case CODA_DBL:
      if (updater_) 
	(*updater_)(data_.compname_, data_.attrname_, 
		    (void *)&(data_.u_.dval), 1);
      break;
    case CODA_STR:
      if (updater_) 
	(*updater_)(data_.compname_, data_.attrname_, 
		    (void *)(data_.u_.sval), 1);
      break;
    case CODA_STRUCT:
      if (updater_)
	(*updater_)(data_.compname_, data_.attrname_, 
		    (void *)(data_.u_.arb), 1);	
      break;
    default:
      break;
    }
  }
  else {
    if (updater_) 
      (*updater_)(data_.compname_, data_.attrname_, 
		  (void *)(data_.u_.data), data_.count_);
  }
  if (updater_)
    notifyChannels ();
  if (trigger_ && doTrigger_)
    trigger_->trigger (this);
}

void
daqData::dumpAll (void)
{
  printf ("Data %s %s has value %s\n", compname(), attrname(),
	  (char *)(*this));
}

void
daqData::monitorOn (void* channel)
{
  if (!channels_.includes (channel))
    channels_.add (channel);
}

void
daqData::monitorOff (void* channel)
{
  if (channels_.includes (channel))
    channels_.remove (channel);
}

int
daqData::monitored (void)
{
  if (channels_.isEmpty ())
    return 0;
  else
    return 1;
}

void
daqData::notifyChannels (void)
{
  // empty for now
}

long
daqData::count (void) const
{
  return data_.count ();
}

int
daqData::historyKept (void) const
{
  return historyKept_;
}

void
daqData::keepHistory (void)
{
  historyKept_ = 1;
}

void
daqData::discardHistory (void)
{
  historyKept_ = 0;
}

void
daqData::constructKey (char* compname, char* attrname)
{
  int len = ::strlen (compname) + ::strlen (attrname) + 2;
  key_ = new char[len];
  ::strcpy (key_, compname);
  ::strcat (key_, "+");
  ::strcat (key_, attrname);
}

void
daqData::registerGetCbk (void *id)
{
  getCbkList_.add (id);
}

void
daqData::registerSetCbk (void *id)
{
  setCbkList_.add (id);
}

void
daqData::registerMonOffCbk (void* id)
{
  offCbkList_.add (id);
}

void
daqData::removeGetCbk (void* id)
{
  getCbkList_.remove (id);
}

void
daqData::removeSetCbk (void* id)
{
  setCbkList_.remove (id);
}

void
daqData::removeMonOffCbk (void* id)
{
  offCbkList_.remove (id);
}

int
daqData::hasGetCbk (void *id)
{
  return getCbkList_.includes (id);
}

int
daqData::hasSetCbk (void* id)
{
  return setCbkList_.includes (id);
}

int
daqData::hasMonitorCbk (void *id)
{
  return channels_.includes (id);
}

int
daqData::hasMonOffCbk (void* id)
{
  return offCbkList_.includes (id);
}

int
daqData::numGetCbks (void) const
{
  return getCbkList_.count();
}

int
daqData::numSetCbks (void) const
{
  return setCbkList_.count ();
}

int
daqData::numMonitorCbks (void) const
{
  return channels_.count ();
}

int
daqData::numMonOffCbks (void) const
{
  return offCbkList_.count ();
}

int
daqData::locked (void) const
{
  return locked_;
}

void
daqData::lock (void)
{
  locked_ = 1;
}

void
daqData::unlock (void)
{
  locked_ = 0;
}

int
daqData::writable (void) const
{
  return writable_;
}

void
daqData::enableWrite (void)
{
  writable_ = 1;
}

void
daqData::disableWrite (void)
{
  writable_ = 0;
}

codaSlist&
daqData::getCbkList (void)
{
  return getCbkList_;
}

codaSlist&
daqData::setCbkList (void)
{
  return setCbkList_;
}

codaSlist&
daqData::monitorCbkList (void)
{
  return channels_;
}

codaSlist&
daqData::monOffCbkList (void)
{
  return offCbkList_;
}
