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
//	 CODA Motif Menu Bar Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaMenuBar.cc,v $
//   Revision 1.3  1998/04/08 17:29:06  heyes
//   get in there
//
//   Revision 1.2  1997/07/30 14:12:14  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <XcodaMenuBar.h>
#include <codaComd.h>
#include <codaComdList.h>
#include <codaComdXInterface.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>

XcodaMenuBar::XcodaMenuBar(Widget parent, char *name)
:XcodaUi(name)
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaMenuBar \n");
#endif
  _w = XmCreateMenuBar (parent, _name, NULL, 0);
  installDestroyHandler();
}

XcodaMenuBar::~XcodaMenuBar()
{
#ifdef _TRACE_OBJECTS
  printf("Destroy XcodaMenuBar \n");
#endif
}

void XcodaMenuBar::addRegMenuEntry(codaComdList *list, 
				   char *name,
				   char mnemonic)
{
  int i;
  Widget pulldown, cascade;
  Arg    arg[10];
  int    ac = 0;
  XmString t;
  Pixmap map;

  XtSetArg (arg[ac], XmNbackgroundPixmap, &map); ac++;
  XtGetValues (_w, arg, ac); 

  ac = 0;
  // create a pulldown menu first
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_OUT); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;

  if (map) {
    XtSetArg (arg[ac], XmNbackgroundPixmap, map); ac++;
  }
  pulldown = XmCreatePulldownMenu(_w, name, arg, ac);
  // create cascade button
  ac = 0;
  t = XmStringCreateSimple(name);
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNsubMenuId, pulldown); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_OUT); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;

  if (map) {
    XtSetArg (arg[ac], XmNbackgroundPixmap, map); ac++;
  }

  if(mnemonic)
    XtSetArg (arg[ac], XmNmnemonic, mnemonic); ac++;
  cascade = XtCreateManagedWidget ("cascade",
				   xmCascadeButtonWidgetClass,
				   _w, arg, ac);
  ac = 0;
  XmStringFree (t);
  
  for (i=0; i < list->size(); i++){
    codaComd *cmd;
    cmd = (*list)[i];
    cmd->createXInterface(pulldown);
  }
}

void XcodaMenuBar::addHelpMenuEntry(codaComdList *list, 
				    char *name,
				    char mnemonic)
{
  int i;
  Widget pulldown, cascade;
  Arg    arg[10];
  int    ac = 0;
  XmString t;
  Pixmap map;

  XtSetArg (arg[ac], XmNbackgroundPixmap, &map); ac++;
  XtGetValues (_w, arg, ac); 
  ac = 0;

  // create a pulldown menu first
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_OUT); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;

  if (map) {
    XtSetArg (arg[ac], XmNbackgroundPixmap, map); ac++;
  }
  pulldown = XmCreatePulldownMenu(_w, name, arg, ac);
  ac = 0;
  // create cascade button
  t = XmStringCreateSimple(name);
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNsubMenuId, pulldown); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_OUT); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;

  if(mnemonic)
    XtSetArg (arg[ac], XmNmnemonic, mnemonic); ac++;

  if (map) {
    XtSetArg (arg[ac], XmNbackgroundPixmap, map); ac++;
  }
  cascade = XtCreateManagedWidget ("cascade",
				   xmCascadeButtonWidgetClass,
				   _w, arg, ac);
  ac = 0;
  XmStringFree (t);
  
  for (i=0; i < list->size(); i++){
    codaComd *cmd;
    cmd = (*list)[i];
    cmd->createXInterface(pulldown);
  }  
  /* Set help menu entry */
  XtSetArg (arg[ac], XmNmenuHelpWidget, cascade); ac++;
  XtSetValues (_w, arg, ac);
  ac = 0;
}
