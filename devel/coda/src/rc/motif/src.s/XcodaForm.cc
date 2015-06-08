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
//	 CODA Motif Form Dialog Class (Generic)
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaForm.cc,v $
//   Revision 1.2  1998/05/28 17:46:45  heyes
//   new GUI look
//
//   Revision 1.1  1998/04/08 18:40:13  heyes
//   add
//
//   Revision 1.2  1997/07/30 14:12:13  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <Xm/Form.h>

#include <XcodaForm.h>

//==============================================================================
//           Implementation of XcodaFormDialog Class
//==============================================================================
XcodaForm::XcodaForm(Widget parent, char *name, char *title)
:XcodaUi(name)
{
#ifdef _TRACE_OBJECTS
  printf("    Create XcodaForm Object\n");
#endif
  _parent = parent;
  _title = new char[::strlen(title) + 1];
  ::strcpy (_title, title);
  alreadyManaged = 0;
  modalDialog = 0;
}

XcodaForm::~XcodaForm()
{
#ifdef _TRACE_OBJECTS
  printf("    Delete XcodaForm Object\n");
#endif
  delete []_title;
}

void XcodaForm::init()
{
  Arg arg[10];
  int ac = 0;

  ac = 0;  
  _w = XmCreateForm(_parent, _name, arg, ac);
  ac = 0;
  XtManageChild(_w);
  installDestroyHandler();
  createFormChildren();
}

void XcodaForm::popup()
{
  manage();
  alreadyManaged = 1;
  if(modalDialog)
    XtAddGrab (XtParent(_w), True, False);
}

void XcodaForm::popdown()
{
  unmanage();
  alreadyManaged = 0;
  if(modalDialog)
    XtRemoveGrab (XtParent(_w));
}

int XcodaForm::isMapped()
{
  return alreadyManaged;
}


void XcodaForm::setModal()
{
  modalDialog = 0;
}

void XcodaForm::unsetModal()
{
  modalDialog = 0;
}

void XcodaForm::defaultButton (Widget w)
{
  Arg arg[5];
  int ac = 0;

  XtSetArg (arg[ac], XmNdefaultButton, w); ac++;
  XtSetValues (_w, arg, ac);
  ac = 0;
}

