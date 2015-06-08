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
//	Pushbutton Command Interface Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaPbtnInterface.cc,v $
//   Revision 1.4  1998/04/08 17:29:16  heyes
//   get in there
//
//   Revision 1.3  1997/07/30 14:12:14  heyes
//   add more xpm support
//
//   Revision 1.2  1997/06/06 19:00:41  heyes
//   new RC
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <XcodaPbtnInterface.h>
#include <Xm/PushB.h>

XcodaPbtnInterface::XcodaPbtnInterface(Widget parent,
				       codaComd *cmd)
:codaComdXInterface (parent,cmd)
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaPbtnInterface Object \n");
#endif
  _acc = 0;
  _acc_text = 0;
}

XcodaPbtnInterface::XcodaPbtnInterface(Widget parent,
				       codaComd *cmd,
				       char *acc,
				       char *acc_text)
:codaComdXInterface (parent,cmd)
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaPbtnInterface Object \n");
#endif
  _acc = new char[::strlen(acc) + 1];
  ::strcpy(_acc, acc);

  _acc_text = new char[::strlen(acc_text) + 1];
  ::strcpy(_acc_text, acc_text);
}

void XcodaPbtnInterface::init ()
{
  Arg arg[5];
  int ac = 0;
  Pixmap map;

  XtSetArg (arg[ac], XmNbackgroundPixmap, &map); ac++;
  XtGetValues (_parent, arg, ac); 

  ac = 0;  
  if (map) {
    XtSetArg (arg[ac], XmNbackgroundPixmap, map); ac++;
  }

  if(_acc && _acc_text){
    XmString t = XmStringCreateSimple(_acc_text);
    XtSetArg(arg[ac], XmNaccelerator, _acc); ac++;
    XtSetArg(arg[ac], XmNacceleratorText, t); ac++;
  
    _w = XtCreateWidget (_name, xmPushButtonWidgetClass,
			 _parent, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
  else {
    _w = XtCreateWidget (_name, xmPushButtonWidgetClass,
			 _parent, arg, ac);
  }
  installDestroyHandler();
  
  if (_active)
    activate();
  else
    deactivate();

  XtAddCallback (_w, 
		 XmNactivateCallback,
		 (XtCallbackProc)&codaComdXInterface::executeComdCallback,
		 (XtPointer)this);
  XtManageChild (_w);
}

XcodaPbtnInterface::~XcodaPbtnInterface()
{
#ifdef _TRACE_OBJECTS
  printf("Destroy XcodaPbtnInterface Object \n");
#endif
  if (_acc)
    delete []_acc;
  if (_acc_text)
    delete []_acc_text;
}

const char *XcodaPbtnInterface::className() const
{
  return "XcodaPbtnInterface";
}



