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
//	Simple Motif XPM Label
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaXpmLabel.cc,v $
//   Revision 1.2  1998/04/08 17:29:45  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:25:01  chen
//   coda motif C++ library
//
//
#include <Xm/Label.h>
#include "XcodaXpmLabel.h"

XcodaXpmLabel::XcodaXpmLabel (Widget parent, char* name, char** pix,
			      char* bgsymbol)
:XcodaUi (name), parent_ (parent), pix_ (pix), pixmap_ (0), bgsymbol_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create XcodaXpmLabel Class Object\n");
#endif
  if (bgsymbol) {
    bgsymbol_ = new char[::strlen (bgsymbol) + 1];
    ::strcpy (bgsymbol_, bgsymbol);
  }
  if (pix_) {
    if (bgsymbol_)
      pixmap_ = new XcodaXpm (parent, pix_, bgsymbol_);
    else
      pixmap_ = new XcodaXpm (parent, pix_);
  }
}

XcodaXpmLabel::~XcodaXpmLabel (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete XcodaXpmLabel Class Object\n");
#endif
  // pix_ is just a pointer

  delete pixmap_;

  if (bgsymbol_)
    delete []bgsymbol_;
}

void
XcodaXpmLabel::init (void)
{
  Arg arg[20];
  int ac = 0;
  Pixmap pix = 0;

  if (pixmap_) 
    pix = pixmap_->createPixmapFromXpm ();
  if (pix) {
    XtSetArg (arg[ac], XmNlabelType, XmPIXMAP); ac++;
    XtSetArg (arg[ac], XmNlabelPixmap, pix); ac++;
  }
  _w = XtCreateManagedWidget (_name, xmLabelWidgetClass,
			      parent_, arg, ac);
  ac = 0;
  
  // install destroy handler
  installDestroyHandler ();
}

void
XcodaXpmLabel::setPixmap (XcodaXpm* xpm)
{
  Arg arg[20];
  int ac = 0;
  Pixmap pix = 0;

  if ((pix = xpm->getPixmap ()) == 0)
    pix = xpm->createPixmapFromXpm();
  if (pix) {
    XtSetArg (arg[ac], XmNlabelType, XmPIXMAP); ac++;
    XtSetArg (arg[ac], XmNlabelPixmap, pix); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
  }
}

XcodaXpm*
XcodaXpmLabel::defaultPixmap (void)
{
  return pixmap_;
}
