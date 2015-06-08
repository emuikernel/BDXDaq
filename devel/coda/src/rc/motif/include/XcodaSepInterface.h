//-----------------------------------------------------------------------------
// Copyright (c) 1991,1992 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
// Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
//-----------------------------------------------------------------------------
// 
// Description:
//	 Separator Command X Interface
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaSepInterface.h,v $
//   Revision 1.2  1998/04/08 17:29:30  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:59  chen
//   coda motif C++ library
//
//
#ifndef _XCODA_SEP_INTERFACE_H
#define _XCODA_SEP_INTERFACE_H

#include <codaComdXInterface.h>

class XcodaSepInterface: public codaComdXInterface{
 public:
  XcodaSepInterface(Widget, codaComd *);
  virtual ~XcodaSepInterface();

  virtual void init();
  virtual const char *className() const {return "XcodaSepInterface";}
};

#endif
