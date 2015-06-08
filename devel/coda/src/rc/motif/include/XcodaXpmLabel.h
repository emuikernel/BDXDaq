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
//	Simple Motif Label Widget with XPM pixmap
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaXpmLabel.h,v $
//   Revision 1.2  1998/04/08 17:29:45  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:25:01  chen
//   coda motif C++ library
//
//
#ifndef _XCODA_XPM_LABEL_H
#define _XCODA_XPM_LABEL_H

#include <XcodaUi.h>
#include <XcodaXpm.h>

class XcodaXpmLabel: public XcodaUi
{
public:
  // constructor and destructor
  XcodaXpmLabel (Widget parent, char* name, char** pix = 0, 
		 char* bgsymbol = 0);
  virtual ~XcodaXpmLabel (void);

  // init all widgets
  void     init (void);
  
  // setup pixmap
  void      setPixmap (XcodaXpm* xpm);
  // get    default pixmap
  XcodaXpm* defaultPixmap    (void);

  // class name
  virtual const char* className (void) const {return "XcodaXpmLabel";}

private:
  Widget    parent_;
  char**    pix_;
  char*     bgsymbol_;
  XcodaXpm* pixmap_;
};
#endif

