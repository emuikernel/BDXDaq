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
//	XcodaToplevel Window Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaTopLevel.h,v $
//   Revision 1.3  1998/04/08 17:29:39  heyes
//   get in there
//
//   Revision 1.2  1997/04/16 18:12:22  chen
//   add multilist and extensions
//
//   Revision 1.1.1.1  1996/10/10 19:24:59  chen
//   coda motif C++ library
//
//
#ifndef _XCODATOPLEVEL_H
#define _XCODATOPLEVEL_H

#include <XcodaUi.h>

class XcodaTopLevel: public XcodaUi
{
public:
  virtual ~XcodaTopLevel();
    
  // The Application class automatically calls initialize() 
  // for all registered main window objects
    
  virtual void initialize (void);
  // popup the window
  virtual void manage     (void);   
  // pop down the window
  virtual void unmanage   (void); 
  virtual void iconify    (void);
  // change cursor
  virtual void defineCursor (unsigned int cursor);
  virtual void undefineCursor (void);
  
protected:
  // Constructor requires only a name.  protect from direct instance
  XcodaTopLevel ( char *name );   
  Widget _base_widget;
  // Derived classes must define this function to
  // create form or rowcolumn...
  virtual Widget CreateBaseWidget (Widget parent) = 0;
};
#endif
