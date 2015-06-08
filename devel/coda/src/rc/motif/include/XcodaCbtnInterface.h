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
//	Cascade Button Interface header file
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaCbtnInterface.h,v $
//   Revision 1.2  1998/04/08 17:28:50  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:57  chen
//   coda motif C++ library
//
//
#ifndef _X_CODA_CBTN_INTERFACE_H
#define _X_CODA_CBTN_INTERFACE_H

#include <codaComdXInterface.h>

class codaComdList;

class XcodaCbtnInterface: public codaComdXInterface{

 public:
// sole purpose of cascade button is to popup another menu  
  XcodaCbtnInterface (Widget, codaComd *);  
  virtual ~XcodaCbtnInterface();
  virtual void addCommands (codaComdList *);
  virtual void init ();
  virtual const char *className() const;

 private:
  Widget _pulldown;
};

#endif
