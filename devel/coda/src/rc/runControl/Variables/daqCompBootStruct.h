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
//      CODA run control : component auto boot values
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqCompBootStruct.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:30  chen
//   run control source
//
//
#ifndef _CODA_DAQ_COMPBOOT_STRUCT_H
#define _CODA_DAQ_COMPBOOT_STRUCT_H

#include <daqArbStruct.h>

class daqCompBootStruct: public daqArbStruct
{
public:
  daqCompBootStruct  (void);
  ~daqCompBootStruct (void);

  // return duplicated object
  daqArbStruct* dup (void);

  // return size information
  size_t size (void);

  // return run time id information
  long   id   (void);

  // encode information
  void   encode (char* buffer, size_t &bufsize);
  
  // decode information
  void   decode (char* buffer, size_t size);

  // clean up
  void   cleanUp (void);

  // add new components and auto boot info to the struct
  void   insertInfo (char* component, int autoboot);

  // get components and auto boot information
  long   compBootInfo (char** &components, long* &autoboot);

  // print out all information
  void   dumpAll (void);

  // class name
  const char* className (void) const {return "daqCompBootStruct";}

protected:

  // encode data area
  void   encodeData (void);
  // restore data
  void   restoreData (void);

private:
  // data area
  static int maxNumComps;
  static int maxCompNameLen;

  // all components name
  long    id_;             // run time decoding id
  long    numComponents_;
  long*   autoboot_;
  char**  components_;
  // one has to count virtual function table pointer size
  // to align in 8 byte boundary
};
#endif
