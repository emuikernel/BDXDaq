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
//	Push Button Interface Class Header
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaPbtnInterface.h,v $
//   Revision 1.3  1998/04/08 17:29:17  heyes
//   get in there
//
//   Revision 1.2  1997/06/06 19:00:41  heyes
//   new RC
//
//   Revision 1.1.1.1  1996/10/10 19:24:58  chen
//   coda motif C++ library
//
//
#ifndef _X_CODA_PBTN_INTERFACE_H
#define _X_CODA_PBTN_INTERFACE_H

#include <codaComdXInterface.h>

class XcodaPbtnInterface: public codaComdXInterface{
 public:
  XcodaPbtnInterface (Widget parent, codaComd *cmd);
  XcodaPbtnInterface (Widget parent, codaComd *cmd, char *acc, char *acc_text);

  virtual ~XcodaPbtnInterface();

  virtual void init ();

  virtual const char *className() const;
  void defaultButton();
 private:
  char *_acc;
  char *_acc_text;
};
#endif
