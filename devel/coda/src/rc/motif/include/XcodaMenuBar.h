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
//	 CODA Motif Menu Bar Class Header
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaMenuBar.h,v $
//   Revision 1.2  1998/04/08 17:29:08  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:58  chen
//   coda motif C++ library
//
//
#ifndef _CODA_MENU_BAR
#define _CODA_MENU_BAR

// just for those who wants to know mwnuBar name when
// one wants to put in the resource file. The menubar has
// name "menu_bar"

#include <XcodaUi.h>

class codaComd;
class codaComdList;

class XcodaMenuBar: public XcodaUi{

 public:
  XcodaMenuBar (Widget, char *);
  virtual ~XcodaMenuBar();
  virtual void addRegMenuEntry(codaComdList *, char *, char mnemonic = 0);
  virtual void addHelpMenuEntry(codaComdList *, char *, char mnemonic = 0);
  virtual const char *className() const { return "XcodaMenuBar";};
};
#endif
