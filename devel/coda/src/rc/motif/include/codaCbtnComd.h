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
//	CODA cascade button Command class header file
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaCbtnComd.h,v $
//   Revision 1.2  1998/04/08 17:29:49  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:59  chen
//   coda motif C++ library
//
//
#ifndef _CODA_CBTN_COMD
#define _CODA_CBTN_COMD

#include <codaComd.h>
#include <codaComdList.h>

class XcodaCbtnInterface;

class codaCbtnComd:public codaComd{

 public:
  codaCbtnComd (char *, int);
  virtual ~codaCbtnComd();
  virtual void createXInterface(Widget);
  virtual const char *className(void) const {return "codaCbtnComd";};

 protected:
  virtual void doit();
  virtual void undoit();
  void         registerCommand(codaComd *);

 private:
  codaComdList *_comd_list;
  XcodaCbtnInterface *cb;
};
#endif
