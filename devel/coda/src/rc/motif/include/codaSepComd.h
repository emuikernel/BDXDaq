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
//	 Separator Widget class for menubar
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaSepComd.h,v $
//   Revision 1.2  1998/04/08 17:29:54  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:25:00  chen
//   coda motif C++ library
//
//
#ifndef _CODA_SEP_COMD_H
#define _CODA_SEP_COMD_H

#include <codaComd.h>

class XcodaSepInterface;

class codaSepComd: public codaComd
{
public:
  codaSepComd(char *, int = 1);
  virtual ~codaSepComd (void);
  virtual void createXInterface (Widget);
  virtual const char *className (void) const {return "codaSepComd";}

protected:
  virtual void doit();
  virtual void undoit();
  
private:
  XcodaSepInterface *sep;
};
#endif
