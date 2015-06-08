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
//      Target class. This abstract class defines interface for all
//                    components inside coda svc
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: target.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:17  chen
//   run control source
//
//
#ifndef _CODA_TAGGET_H
#define _CODA_TARGET_H

#include <daqCommon.h>

class target
{
 public:
  // constructor and destructor
  virtual ~target  (void);

  virtual int boot (void) = 0;
  virtual int terminate (void) = 0;
  virtual int status (void) const = 0;

  // operations
  void             title  (char *title);
  char             *title (void) const;

  virtual const char *className (void) const {return "target";}

 protected:
  target           (char *title);
  // data area
  char*            title_;
};
#endif

