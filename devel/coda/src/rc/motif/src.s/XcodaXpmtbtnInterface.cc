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
//	 CODA Xpm Toggle Button X Interface
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaXpmtbtnInterface.cc,v $
//   Revision 1.2  1998/04/08 17:29:48  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:25:00  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <XcodaXpmtbtnInterface.h>
#include <XcodaXpm.h>
#include <Xm/ToggleB.h>

XcodaXpmtbtnInterface::XcodaXpmtbtnInterface(Widget parent,
					     char **pix,
					     char **sel_pix,
					     codaComd *cmd,
					     int      notoggle,
					     int      state)
:XcodaTbtnInterface (parent, cmd, state)
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaXpmtbtnInterface Objects \n");
#endif
  _parent = parent;
  _initState = state;
  _notoggle = notoggle;
  nxpm = new XcodaXpm(parent, pix);
  if (sel_pix)
    selxpm = new XcodaXpm (parent, sel_pix);
  else
    selxpm = 0;
}

XcodaXpmtbtnInterface::XcodaXpmtbtnInterface(Widget parent,
					     char **pix,
					     char **sel_pix,
					     codaComd *cmd,
					     int notoggle,
					     int state,
					     char *symbol)
:XcodaTbtnInterface (parent, cmd, state)
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaXpmtbtnInterface Objects \n");
#endif
  _parent = parent;
  _notoggle = notoggle;
  _initState = state;
  nxpm = new XcodaXpm(parent, pix, symbol);
  if (sel_pix)
    selxpm = new XcodaXpm (parent, sel_pix, symbol);
  else
    selxpm = 0;
}

XcodaXpmtbtnInterface::~XcodaXpmtbtnInterface()
{
#ifdef _TARCE_OBJECTS
  printf("Destroy XcodaXpmtbtnInterface Objects \n");
#endif
  delete nxpm;
  if (selxpm)
    delete selxpm;
}

void XcodaXpmtbtnInterface::init ()
{
  Arg           arg[10];
  int           ac = 0;
  Pixmap        npixmap=0, selpixmap=0;

  // from XcodaXpm class
  npixmap = nxpm->createPixmapFromXpm();
  if (selxpm)
    selpixmap = selxpm->createPixmapFromXpm();

  if(npixmap){
    XtSetArg (arg[ac], XmNlabelType, XmPIXMAP); ac++;
    XtSetArg (arg[ac], XmNlabelPixmap, npixmap); ac++;
    if (selpixmap){
      XtSetArg (arg[ac], XmNselectPixmap, selpixmap); ac++;
    }
    else{
      XtSetArg (arg[ac], XmNselectPixmap, npixmap); ac++;
    }
  }
  if(_notoggle){
    XtSetArg (arg[ac], XmNindicatorOn, False); ac++;
    XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  }
  if(_initState){
    XtSetArg(arg[ac], XmNset, True); ac++;
  }
  else{
    XtSetArg(arg[ac], XmNset, False); ac++;
  }
  _w = XtCreateWidget (_name,
		       xmToggleButtonWidgetClass,
		       _parent, 
		       arg, ac);
  ac = 0;
  installDestroyHandler();

  if(_active)
    activate();
  else
    deactivate();
  
  XtAddCallback (_w, XmNvalueChangedCallback,
		 (XtCallbackProc)&codaComdXInterface::executeComdCallback,
		 (XtPointer) this);
  XtManageChild (_w);
}

XcodaXpm*
XcodaXpmtbtnInterface::pixmap (void)
{
  return nxpm;
}

XcodaXpm*
XcodaXpmtbtnInterface::selectionPixmap (void)
{
  return selxpm;
}

void
XcodaXpmtbtnInterface::setPixmap (XcodaXpm* xpm)
{
  Arg arg[10];
  int ac = 0;
  Pixmap pix = 0;

  if ((pix = xpm->getPixmap ()) == 0) 
    pix = xpm->createPixmapFromXpm ();
  if (pix) {
    XtSetArg (arg[ac], XmNlabelPixmap, pix); ac++;
    XtSetValues (_w, arg, ac);
  }
}

void
XcodaXpmtbtnInterface::setSelectionPixmap (XcodaXpm* xpm)
{
  Arg arg[10];
  int ac = 0;
  Pixmap pix = 0;

  if ((pix = xpm->getPixmap ()) == 0) 
    pix = xpm->createPixmapFromXpm ();
  if (pix) {
    XtSetArg (arg[ac], XmNselectPixmap, pix); ac++;
    XtSetValues (_w, arg, ac);
  }
}

