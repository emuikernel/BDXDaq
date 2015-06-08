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
//	 CODA Menu Window Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaMenuWindow.cc,v $
//   Revision 1.2  1998/04/08 17:29:10  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:25:01  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <XcodaUi.h>
#include <XcodaMenuWindow.h>
#include <XcodaMenuBar.h>
#include <Xm/Form.h>

XcodaMenuWindow::XcodaMenuWindow(Widget parent, char *name):XcodaUi(name)
{
  Arg arg[20];
  int ac = 0;
#ifdef _TRACE_OBJECTS
  printf("Create XcodaMenuWindow Object \n");
#endif
  _w = XtCreateWidget(name, xmFormWidgetClass, parent,
		      NULL, 0);
  MenuBar = new XcodaMenuBar (_w, "menu_bar");

  _menu_bar = (Widget)MenuBar->baseWidget();
  
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues (_menu_bar, arg, ac);
  ac = 0;
  
  installDestroyHandler ();
}

void XcodaMenuWindow::init()
{
  Arg arg[20];
  int ac = 0;

  // create a manager widget to hold everything below the menu bar
  _holder_widget = createMenuWindow (_w);

  createMenuPanes();
  MenuBar->manage();

  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, _menu_bar); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetValues (_holder_widget, arg, ac);
  ac = 0;
}

XcodaMenuWindow::~XcodaMenuWindow()
{
#ifdef _TRACE_OBJECTS
  printf("Destroy XcodaMenuWindow Object \n");
#endif  
  // no need to delete menu bar
  // XcodaMenuBar will delete itself upon widget destroy
  //  delete MenuBar;
}

