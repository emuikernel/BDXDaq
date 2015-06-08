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
//	Toggle Button Interface Class Header File
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaTbtnInterface.h,v $
//   Revision 1.2  1998/04/08 17:29:38  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:59  chen
//   coda motif C++ library
//
//
#ifndef _X_CODA_TBTN_INTERFACE_H
#define _X_CODA_TBTN_INTERFACE_H

#include <codaComdXInterface.h>

class XcodaTbtnInterface: public codaComdXInterface{

 public:
  
  XcodaTbtnInterface (Widget parent, codaComd *cmd, int state);
  XcodaTbtnInterface (Widget parent, codaComd *cmd, int notoggle, int state);
  XcodaTbtnInterface (Widget parent, codaComd *cmd, int state, 
		      char *acc, char *acc_text);
  virtual ~XcodaTbtnInterface();
  void setState(int st);
  int state(); // return state of toggle button
  virtual void init ();
  virtual const char *className() const;
 private:
  char *_acc;
  char *_acc_text;
  int initState;
  int _notoggle;
};

#endif

