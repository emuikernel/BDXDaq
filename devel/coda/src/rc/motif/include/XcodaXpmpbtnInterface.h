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
//	CODA Xpm Button X interface
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaXpmpbtnInterface.h,v $
//   Revision 1.4  1998/06/18 12:21:53  heyes
//   new GUI ready I think
//
//   Revision 1.3  1998/04/08 17:29:47  heyes
//   get in there
//
//   Revision 1.2  1997/07/30 14:12:19  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/10 19:24:59  chen
//   coda motif C++ library
//
//
#ifndef _X_CODA_XPMBTN_INTERFACE_H
#define _X_CODA_XPMBTN_INTERFACE_H

#include <XcodaPbtnInterface.h>
class XcodaXpm;

class XcodaXpmpbtnInterface: public XcodaPbtnInterface
{
 public: 
  XcodaXpmpbtnInterface (Widget, char *, char **, codaComd *);
  XcodaXpmpbtnInterface (Widget, char *, char **, codaComd *, char *);
  virtual ~XcodaXpmpbtnInterface();
  
  virtual void init();

  virtual const char *className() const {return "XcodaXpmpbtnInterface";}

  Pixmap        npixmap;
  Pixmap        apixmap;
  Pixmap        upixmap;
  int           height_;
  GC            xgc;

protected:
  static void crossEventHandler (Widget, XtPointer clientData,
				 XEvent* event);
  static void xEventHandler (Widget wid, XtPointer clientData,
			     XtPointer event);
private:
  char *_name;
  Widget   _parent;
  XcodaXpm *nxpm;
  XcodaXpm *axpm;
  XcodaXpm *uxpm;
  int armed;
};
#endif


