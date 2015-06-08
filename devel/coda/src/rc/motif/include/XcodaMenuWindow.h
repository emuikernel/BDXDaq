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
//	Coda Menu Window Class Header File
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaMenuWindow.h,v $
//   Revision 1.2  1998/04/08 17:29:11  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:58  chen
//   coda motif C++ library
//
//
#ifndef _CODA_MENU_WINDOW
#define _CODA_MENU_WINDOW

#include <XcodaUi.h>

class XcodaMenuBar;

class XcodaMenuWindow: public XcodaUi{

 public:
  XcodaMenuWindow (Widget, char *);
  virtual ~XcodaMenuWindow ();
  
  virtual void init();
  const char *className() const {return "XcodaMenuWindow";};

 protected:
  Widget _holder_widget;
  // derived class must define this feature to
  // create any manager widget
  virtual Widget createMenuWindow (Widget) = 0;
  // derived class must provide real menu entries
  XcodaMenuBar *MenuBar;
  virtual void createMenuPanes() = 0;

 private:
  Widget _menu_bar;
  
};
#endif

  
