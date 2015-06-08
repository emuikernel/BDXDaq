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
//      CODA data manager
//
//      This Class has one hash table which holds all daqDatas.
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqDataManager.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:30  chen
//   run control source
//
//
#ifndef _CODA_DAQ_DATA_MANAGER_H
#define _CODA_DAQ_DATA_MANAGER_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <codaStrHash.h>

class daqData;
class daqDataManagerIterator;

class daqDataManager
{
public:
  // constructor and destructor
  daqDataManager  (void);
  // all data inside table will be removed and memory not be freed
  ~daqDataManager (void);

  // operations: return CODA_SUCCESS on success, return CODA_ERROR on failure
  // add daqData into the manager
  int  addData     (daqData* data);
  // remove data from the manager but data is still valid
  int  removeData  (daqData* data);
  // remove data by name, data will be invalid (freed)
  int  removeData  (char *compname, char *attrname);
  // find data
  int  hasData     (char *compname, char *attrname);
  // find data
  int  findData    (char *compname, char *attrname, daqData* &data);
  // clean all values, data are still valid
  void cleanAll    (void);
  // remove all values, data are removed, no longer valid
  void freeAll     (void);

  // update all data inside table
  void update  (void);
  // print out all values
  void dumpAll (void);

private:
  // hash table for all variables
  codaStrHash  dataTable_;
  friend class daqDataManagerIterator;
};

class daqDataManagerIterator
{
public:
  daqDataManagerIterator (daqDataManager& manager);
  ~daqDataManagerIterator (void);

  int                init        (void);
  daqData*           operator () (void);
  int                operator !  (void);
  int                operator ++ (void);

protected:
  codaStrHashIterator ite_; 
};
#endif

