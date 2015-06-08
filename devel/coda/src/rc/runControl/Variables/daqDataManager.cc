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
//      CODA Data Manager Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqDataManager.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:30  chen
//   run control source
//
//
#include <daqData.h>
#include "daqDataManager.h"

const int CODA_DATA_TABLE_SIZE = 107;
const int FULLNAME_LEN         = 256;

daqDataManager::daqDataManager (void)
:dataTable_ (CODA_DATA_TABLE_SIZE, codaStrHashFunc)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqDataManager Class Object\n");
#endif
}

daqDataManager::~daqDataManager (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete daqDataManager Class Object\n");
#endif
}

void
daqDataManager::cleanAll (void)
{
  dataTable_.deleteAllValues ();
}

void
daqDataManager::freeAll (void)
{
  codaStrHashIterator ite (dataTable_);
  daqData* data = 0;

  for (ite.init (); !ite; ++ite){
    data = (daqData *)ite ();
#ifdef _CODA_DEBUG
    printf ("Remove data Variables %s %s\n",data->compname(), 
	    data->attrname ());
#endif
    delete data;
  }
}

int
daqDataManager::hasData (char *compname, char *attrname)
{
  char fullname[FULLNAME_LEN];

  ::strcpy (fullname, compname);
  ::strcat (fullname, "+");
  ::strcat (fullname, attrname);

  codaSlist& list = dataTable_.bucketRef (fullname);
  codaSlistIterator ite (list);
  daqData *data = 0;

  for (ite.init(); !ite; ++ite){
    data = (daqData *)ite ();
    if (::strcmp (fullname, data->key ()) == 0)
      return 1;
  }
  return 0;
}

int
daqDataManager::addData (daqData *data)
{
  if (hasData (data->compname (), data->attrname ()))
    return CODA_ERROR;
  dataTable_.add (data->key (), (void *)data);
  return CODA_SUCCESS;
}

int
daqDataManager::removeData (daqData *data)
{
  if (!hasData (data->compname (), data->attrname ()))
    return CODA_ERROR;
  dataTable_.remove (data->key (), (void *)data);
  return CODA_SUCCESS;
}

int
daqDataManager::removeData (char *compname, char *attrname)
{
  char fullname[FULLNAME_LEN];

  ::strcpy (fullname, compname);
  ::strcat (fullname, "+");
  ::strcat (fullname, attrname);

  codaSlist& list = dataTable_.bucketRef (fullname);
  codaSlistIterator ite (list);
  daqData *data = 0;

  for (ite.init(); !ite; ++ite){
    data = (daqData *)ite ();
    if (::strcmp (fullname, data->key ()) == 0){
      list.remove ((void *)data);
      delete data;
      return CODA_SUCCESS;
    }
  }
  return CODA_ERROR;
}

int
daqDataManager::findData (char *compname, char *attrname, daqData* &data)
{
  char fullname[FULLNAME_LEN];

  ::strcpy (fullname, compname);
  ::strcat (fullname, "+");
  ::strcat (fullname, attrname);

  codaSlist& list = dataTable_.bucketRef (fullname);
  codaSlistIterator ite (list);
  daqData *tdata = 0;

  for (ite.init(); !ite; ++ite){
    tdata = (daqData *)ite ();
    if (::strcmp (fullname, tdata->key ()) == 0){
      data = tdata;
      return CODA_SUCCESS;
    }
  }
  data = 0;
  return CODA_ERROR;
}

void
daqDataManager::dumpAll (void)
{
  codaStrHashIterator ite (dataTable_);
  daqData* data = 0;

  for (ite.init (); !ite; ++ite){
    data = (daqData *)ite ();
    data->dumpAll ();
  }  
}

void
daqDataManager::update (void)
{
  codaStrHashIterator ite (dataTable_);
  daqData* data = 0;

  for (ite.init (); !ite; ++ite){
    data = (daqData *)ite ();
    if (data->active())
      data->update ();
  }  
}

//=========================================================================
//         Implementation of iterator
//=========================================================================
daqDataManagerIterator::daqDataManagerIterator (daqDataManager& manager)
:ite_ (manager.dataTable_)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqDataManager Iterator Class Obejct\n");
#endif
  // empty
}

daqDataManagerIterator::~daqDataManagerIterator (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete daqDataManager Iterator Class Obejct\n");
#endif
  // empty
}

int
daqDataManagerIterator::init (void)
{
  return ite_.init ();
}

daqData*
daqDataManagerIterator::operator () (void)
{
  return (daqData *)ite_ ();
}

int
daqDataManagerIterator::operator ! (void)
{
  return !ite_;
}

int
daqDataManagerIterator::operator ++ (void)
{
  return ++ite_;
}
