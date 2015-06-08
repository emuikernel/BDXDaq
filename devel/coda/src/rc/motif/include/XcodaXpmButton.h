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
//	XPM Based Color PushButton
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaXpmButton.h,v $
//   Revision 1.3  2000/01/28 16:42:29  rwm
//   include should be X11/xpm.h
//
//   Revision 1.2  1998/04/08 17:29:44  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:59  chen
//   coda motif C++ library
//
//
#ifndef XCODAXPMBUTTON_H
#define XCODAXPMBUTTON_H
#include <XcodaUi.h>
#include <Xm/PushB.h>
#include <X11/xpm.h>

class XcodaXpmButton: public XcodaUi{

 public:
  virtual ~XcodaXpmButton();
  virtual const char *const className() { return ("XcodaXpmButton");}

 protected:
// abstract class put constructor under protection
  XcodaXpmButton (Widget parent, const char *, char **);
  XcodaXpmButton (Widget parent, const char *, char *, char **);
// real callback function, derived class must provide real callback
  virtual void callback ( ) = 0;
  virtual void widgetDestroyed();

 private:
  Display  *_display;
  Pixmap   _pixmap;
  Pixel    _fg, _bg;
  Colormap _cmap;
  Screen   *_scr;
  Window   _win;
  int      _depth;
  char     *_bgsymbol;
  void     createPixmap(Widget parent, char **);
  static void widgetCallback (Widget, XtPointer, XtPointer);
};
#endif
