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
//      CODA run control : all run type data structure
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqRunTypeStruct.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//
#ifndef _CODA_DAQ_RUNTYPE_STRUCT_H
#define _CODA_DAQ_RUNTYPE_STRUCT_H

#include <daqArbStruct.h>

class daqRunTypeStruct;

class daqRunTypeItem
{
public:
  // size information
  size_t size (void);

  // encode information
  void   encode (char* buffer, size_t &bufsize);
  
  // decode information and returnning size of this object
  void   decode (char* buffer, size_t& size);

  // specific operation
  void   cleanUp ();

  long   runtypeId (void) const;
  long   inuse     (void) const;
  char*  name      (void) const;
  char*  category  (void) const;

  // class name
  const char* className (void) const {return "daqRunTypeItem";}

protected:
  // encode data area
  void   encodeData (void);
  // restore data
  void   restoreData (void);

private:
  // run type id
  long    runid_;          
  // in use flag
  long    inuse_;
  // size of name
  long    namelen_;
  // size of category
  long    catlen_;
  // name of the run
  char*   name_;
  // name of the category
  char*   cat_;

  // only daqRunTypeStruct can access this class
  daqRunTypeItem  (void);
  daqRunTypeItem  (char* name, long runid, 
		   long inuse, char* category = 0);
  daqRunTypeItem  (const daqRunTypeItem& rt);
  ~daqRunTypeItem (void);

  // friend class
  friend class daqRunTypeStruct;
};

class daqRunTypeStruct: public daqArbStruct
{
public:
  daqRunTypeStruct  (void);
  ~daqRunTypeStruct (void);

  // return duplicated object
  daqArbStruct* dup (void);

  // return size information
  size_t size (void);

  // return run time id information
  long   id   (void);

  // encode information
  void   encode (char* buffer, size_t& bufsize);
  // decode information
  void   decode (char* buffer, size_t bufsize);

  // clean up
  void   cleanUp (void);

  // add new run type
  void   insertRunType (char* run, long runid, long inuse, char* cat = 0);

  // print all information
  void   dumpAll (void);

  // class name
  const char* className (void) const {return "daqRunTypeStruct";}

protected:
  // encode data area
  void encodeData (void);
  // restore data value
  void restoreData (void);

  // maximum number of run types
  static int maxNumRunTypes;

private:
  // run time id
  long id_;
  // number of single run type items
  long numRunTypes_;
  // padding for alignment of 8 byte boundary
  long unused_;
  // run type to follow
  daqRunTypeItem** runtypes_;

  // deny access to copy and assignment operator
  daqRunTypeStruct (const daqRunTypeStruct& );
  daqRunTypeStruct& operator = (const daqRunTypeStruct& );
};

#endif
