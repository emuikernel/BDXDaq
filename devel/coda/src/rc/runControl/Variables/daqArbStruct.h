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
//      CODA run control : generic arbitrary strcuture
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqArbStruct.h,v $
//   Revision 1.2  1996/12/04 18:32:20  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:30  chen
//   run control source
//
//
#ifndef _CODA_DAQ_ARBSTRUCT_H
#define _CODA_DAQ_ARBSTRUCT_H

#include <stdio.h>
#include <string.h>

#define _CODA_ALIGN_WORDB_ 8      /* word boundary alignment            */
                                  /* align a string to word boundary    */
inline long coda_rc_roundup (long len)
{
  return ((len + _CODA_ALIGN_WORDB_ - 1) & ~ (_CODA_ALIGN_WORDB_ - 1));
}

class daqArbStruct
{
public:
  virtual ~daqArbStruct (void);

  // return duplicated object
  virtual daqArbStruct* dup (void);

  // return size information
  virtual size_t size (void);

  // return run time id information
  virtual long   id   (void);

  // encode information
  virtual void   encode (char* buffer, size_t &bufsize);
  
  // decode information
  virtual void   decode (char* buffer, size_t size);

  // class name
  virtual const char* className (void) const {return "daqArbStruct";}

protected:
  // deny direct instantiation
  daqArbStruct  (void);
};
#endif
