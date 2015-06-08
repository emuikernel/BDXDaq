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
//	CODA toggle button Command class header file
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaTbtnComd.h,v $
//   Revision 1.2  1998/04/08 17:29:55  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:25:00  chen
//   coda motif C++ library
//
//
#ifndef _CODA_TBTN_COMD
#define _CODA_TBTN_COMD

#include <codaComd.h>

class XcodaTbtnInterface;

class codaTbtnComd:public codaComd
{
public:
  codaTbtnComd (char *name, int active, int state = 0);
  codaTbtnComd (char *name, int active, int notoggle, int state);
  codaTbtnComd (char *name, int active, char *acc, char *acc_text, int state=0);
  virtual ~codaTbtnComd (void);
  
  int     state (void);
  void    setState (int st);
  virtual void createXInterface(Widget);
  virtual const char* className() const {return "codaTbtnComd";}

protected:
  XcodaTbtnInterface *tb;

private:
  char               *_acc;
  char               *_acc_text;
  int                initState;
  int                _notoggle;
};
#endif
