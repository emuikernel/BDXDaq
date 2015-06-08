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
//	Toggle Button Interface Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaTbtnInterface.cc,v $
//   Revision 1.3  1998/04/08 17:29:37  heyes
//   get in there
//
//   Revision 1.2  1997/07/30 14:12:15  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <XcodaTbtnInterface.h>
#include <Xm/ToggleB.h>

XcodaTbtnInterface::XcodaTbtnInterface (Widget parent,
					codaComd *cmd,
					int       st)
:codaComdXInterface(parent,cmd), initState(st)
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaTbtnInterface Object \n");
#endif
  _acc = 0;
  _acc_text = 0;
  _notoggle = 0;
}

XcodaTbtnInterface::XcodaTbtnInterface (Widget parent,
					codaComd *cmd,
					int       notoggle,
					int       st)
:codaComdXInterface(parent,cmd), _notoggle(notoggle),initState(st)
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaTbtnInterface Object \n");
#endif
  _acc = 0;
  _acc_text = 0;
}

XcodaTbtnInterface::XcodaTbtnInterface (Widget parent,
					codaComd *cmd,
					int       st,
					char      *acc,
					char      *acc_text)
:codaComdXInterface(parent,cmd), initState(st)
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaTbtnInterface Object \n");
#endif
  _acc = new char[::strlen(acc) + 1];
  ::strcpy(_acc, acc);
  _acc_text = new char[::strlen(acc_text) + 1];
  ::strcpy(_acc_text, acc_text);
  _notoggle = 0;
}

XcodaTbtnInterface::~XcodaTbtnInterface()
{
#ifdef _TRACE_OBJECTS
  printf("Destroy XcodaTbtnInterface Object \n");
#endif
  if(_acc)
    delete []_acc;
  if(_acc_text)
    delete []_acc_text;
}

void XcodaTbtnInterface::init()
{
  Arg arg[5];
  int ac = 0;
  XmString t;
  Pixmap map;

  XtSetArg (arg[ac], XmNbackgroundPixmap, &map); ac++;
  XtGetValues (_parent, arg, ac); 

  ac = 0;  
  if(_acc && _acc_text){
    t = XmStringCreateSimple(_acc_text);
    if (map) {
      XtSetArg (arg[ac], XmNbackgroundPixmap, map); ac++;
    }
    XtSetArg (arg[ac], XmNaccelerator, _acc); ac++;
    XtSetArg (arg[ac], XmNacceleratorText, t); ac++;
    if (_notoggle){
      XtSetArg (arg[ac], XmNindicatorOn, False); ac++;
      XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
    } else {
      XtSetArg (arg[ac], XmNspacing, 10); ac++;
    }
    if(initState){
      XtSetArg(arg[ac], XmNset, True); ac++;
      _w = XtCreateWidget (_name,
			   xmToggleButtonWidgetClass,
			   _parent, 
			   arg, ac);
    }
    else
      _w = XtCreateWidget (_name,
			   xmToggleButtonWidgetClass,
			   _parent, 
			   arg, ac);
    ac = 0;
    XmStringFree (t);
  }
  else{
    if (_notoggle){
      XtSetArg (arg[ac], XmNindicatorOn, False); ac++;
      XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
    } else {
      XtSetArg (arg[ac], XmNspacing, 10); ac++;
    }
    if(initState){
      XtSetArg(arg[ac], XmNset, True); ac++;
      _w = XtCreateWidget (_name,
			   xmToggleButtonWidgetClass,
			   _parent, 
			   arg, ac);
    }
    else
      _w = XtCreateWidget (_name,
			   xmToggleButtonWidgetClass,
			   _parent, 
			   arg, ac);
    ac = 0;
  }

  installDestroyHandler();

  if (_active)
    activate();
  else
    deactivate();

  XtAddCallback (_w, XmNvalueChangedCallback,
		 (XtCallbackProc)&codaComdXInterface::executeComdCallback,
		 (XtPointer) this);
  XtManageChild (_w);
}

const char *XcodaTbtnInterface::className() const
{
  return "XcodaTbtnInterface";
}

int XcodaTbtnInterface::state()
{
  if (XmToggleButtonGetState(_w)) {
    return 1;
  }
  else {
    return 0;
  }
}

void XcodaTbtnInterface::setState(int st)
{
  if(st)
    XmToggleButtonSetState(_w, True, False);
  else
    XmToggleButtonSetState(_w, False, False);
}
