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
//      Generic Data Acquisition Data Type
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqData.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:30  chen
//   run control source
//
//
#ifndef _CODA_DAQ_DATA_H
#define _CODA_DAQ_DATA_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <codaConst.h>
#include <codaSlist.h>
#include <daqDataManager.h>
#include <daqNetData.h>

class daqDataWriter;
class daqDataTrigger;

typedef void (*daqUpdater)(char *compname, 
			   char *attrname, 
			   void* buffer,
			   int   count);

class daqData
{
public:
  // constructors and destructor
  daqData (char* compname, char* attrname, int    data);
  daqData (char* compname, char* attrname, float  data);
  daqData (char* compname, char* attrname, double data);
  daqData (char* compname, char* attrname, char*  data);
  daqData (char* compname, char* attrname, daqArbStruct*  data);
  // constructors for array of elements
  daqData (char* compname, char* attrname, int* data, int count);
  daqData (char* compname, char* attrname, float* data, int count);
  daqData (char* compname, char* attrname, double* data, int count);
  daqData (char* compname, char* attrname, char** data, int count);

  virtual ~daqData (void);
  
  // operations: return CODA_SUCCESS for success, CODA_ERROR for failure
  // connect/disconnect this data to data manager
  int connect            (daqDataManager& manager);
  int disconnect         (daqDataManager& manager);

  // setup updating functions
  void updater           (daqUpdater updater);
  daqUpdater updater     (void) const;
  // setup triggering functions
  void trigger           (daqDataTrigger* trig);
  void enableTrigger     (void);
  void disableTrigger    (void);
  // setup writer
  void writer            (daqDataWriter* writer);

  // setup active or non active flag
  void activate          (void);
  void deactivate        (void);
  int  active            (void) const;

  // setup monitorOn and MonitorOff method
  virtual void monitorOn         (void* channel);
  virtual void monitorOff        (void* channel);
  // return whether this variables is currently being monitored
  virtual int  monitored         (void);

  // retrieve values all conversion operators
  operator int           (void);
  operator long          (void);
  operator float         (void);
  operator double        (void);
  operator char*         (void); // no content copy, just a pointer
  operator daqArbStruct* (void); // content copy
  operator daqNetData&   (void);

  // set value
  daqData& operator =    (int val);
  daqData& operator =    (long val);
  daqData& operator =    (float val);
  daqData& operator =    (double val);
  daqData& operator =    (char* val);
  daqData& operator =    (daqArbStruct* val);
  // assign value from data without changing component name and attribute
  daqData& operator =    (const daqNetData& data);

  // assignment for array of elements
  void assignData        (int* data, int count);
  void assignData        (float* data, int count);
  void assignData        (double* data, int count);
  void assignData        (char** data, int count);

  // retrieve array of elements
  // callers provide memory buffer and size of buffer 'count'
  // upon return count will be real number of elements
  // return CODA_SUCCESS on complete match, return CODA_WARNING for
  // mismatch. return CODA_ERROR if initial count == 0
  int getData            (int data[], int& count);
  int getData            (float data[], int& count);
  int getData            (double data[], int& count);
  int getData            (char* data[], int& count);

  // retrive name informations
  char *attrname         (void) const;
  char *compname         (void) const;
  long count             (void) const;
  // key string used in the hash
  char *key              (void) const;

  // update data value
  void update            (void);
  // in case of any change in value, send new value to monitors
  virtual void notifyChannels (void);
  // printout data
  void dumpAll           (void);

  // operations on these callback list
  virtual void registerGetCbk    (void *id);
  virtual void registerSetCbk    (void *id);
  virtual void registerMonOffCbk (void *id);
  virtual int  hasGetCbk         (void *id);
  virtual int  hasSetCbk         (void *id);
  virtual int  hasMonitorCbk     (void *id);
  virtual int  hasMonOffCbk      (void *id);
  virtual void removeGetCbk      (void *id);
  virtual void removeSetCbk      (void *id);
  virtual void removeMonOffCbk   (void *id);
  virtual int  numGetCbks        (void) const;
  virtual int  numSetCbks        (void) const;
  virtual int  numMonitorCbks    (void) const;
  virtual int  numMonOffCbks     (void) const;
  // callback list
  codaSlist& getCbkList          (void);
  codaSlist& setCbkList          (void);
  codaSlist& monitorCbkList      (void);
  codaSlist& monOffCbkList       (void);

  // data locking during writing
  int  locked (void) const;
  void lock   (void);
  void unlock (void);

  // read/write flag
  int  writable     (void) const;
  void enableWrite  (void);
  void disableWrite (void);

  // keep history
  int   historyKept     (void) const;
  void  keepHistory     (void);
  void  discardHistory  (void);

  
  // class name
  virtual const char *className(void) const {return "daqData";}

protected:
  void         constructKey (char *name, char *attr);

  // real network data
  daqNetData data_;
  // Monitor Channels
  // use void* to represent TCP/Callbacks
  codaSlist channels_;
  // get callback list
  codaSlist getCbkList_;
  // set callback list
  codaSlist setCbkList_;
  // monitorOff callback list
  codaSlist offCbkList_;

private:
  // data area
  char       *key_;
  int        active_;
  // data locking mechanism
  int        locked_;
  // r/w flag
  int        writable_;
  // keep history flag
  int        historyKept_;
  // updating functions
  daqUpdater updater_;
  // trigger which will do something other than updating
  daqDataTrigger* trigger_;
  int             doTrigger_;  // allow on/off trigger
  // writer
  daqDataWriter* writer_;

  // deny access to copy and assignment since it makes no sense to
  // have two data variables to have same callbacks attached to them
  daqData (const daqData& data);
  daqData& operator = (const daqData& data);
};
#endif
