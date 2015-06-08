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
//	Basic Component header file
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaBasic.h,v $
//   Revision 1.3  1998/04/08 17:28:49  heyes
//   get in there
//
//   Revision 1.2  1997/04/16 18:12:19  chen
//   add multilist and extensions
//
//   Revision 1.1.1.1  1996/10/10 19:24:57  chen
//   coda motif C++ library
//
//
#ifndef _XCODABASIC_H
#define _XCODABASIC_H

#include <X11/cursorfont.h>
#include <Xm/Xm.h>

class XcodaBasic {

public:
  virtual ~XcodaBasic();
  virtual void manage();   
  // Manage and unmanage widget tree
  virtual void unmanage();
  const Widget baseWidget() { return _w; }
    
protected:
  char    *_name;
  Widget  _w;    
  // Protected constructor to prevent instantiation
  XcodaBasic ( const char * );   
};
#endif

