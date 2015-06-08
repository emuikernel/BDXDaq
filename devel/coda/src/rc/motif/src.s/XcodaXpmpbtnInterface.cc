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
//	 CODA Xpm Push Button X Interface
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaXpmpbtnInterface.cc,v $
//   Revision 1.9  1998/06/18 12:21:52  heyes
//   new GUI ready I think
//
//   Revision 1.8  1998/04/08 17:29:46  heyes
//   get in there
//
//   Revision 1.5  1997/08/01 18:39:03  heyes
//   add background pixmap support
//
//   Revision 1.4  1997/07/30 15:43:42  heyes
//   add more xpm support
//
//   Revision 1.3  1997/07/30 14:31:30  heyes
//   resolve conflicts
//
//   Revision 1.2  1997/07/30 14:12:18  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <XcodaXpmpbtnInterface.h>
#include <XcodaXpm.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/DrawnB.h>

#define BUTTON_WIDTH 97
#define BUTTON_WIDTH2 8
#define BUTTON_HEIGHT 24
  
static XFontStruct *tfont = 0; 

extern "C" Pixmap XcodaCreatePixmapFromXpm (Widget parent,
				        char** data,
				        int    type);
extern "C" void exit(int);
extern "C" Pixel get_pixel(Display*, Colormap,int,int,int);

XcodaXpmpbtnInterface::XcodaXpmpbtnInterface(Widget parent,
					     char *name,
					     char **pix,
					     codaComd *cmd)
:XcodaPbtnInterface (parent, cmd)
{

  Arg           arg[10];
  int           ac = 0;
  int height;

  ac = 0;
  XtSetArg(arg[ac], XmNheight, &height); ac++;
  XtGetValues(parent, arg, ac); 

#ifdef _TRACE_OBJECTS
  printf("Create XcodaXpmpbtnInterface Objects \n");
#endif
  _parent = parent;
  _name = name;

  npixmap = 0;
  apixmap = 0;
  upixmap = 0;

  height_ = 24;

  if (pix) {
    npixmap = XcodaCreatePixmapFromXpm(parent,pix,1);
  } else {
    {
#include "Button1.xpm"
      npixmap = XcodaCreatePixmapFromXpm(parent,magick,1);
    }
    {
#include "Button2.xpm"
      apixmap = XcodaCreatePixmapFromXpm(parent,magick,1);
    }
    {
#include "Button3.xpm"
      upixmap = XcodaCreatePixmapFromXpm(parent,magick,1);
    }
  }
}

XcodaXpmpbtnInterface::XcodaXpmpbtnInterface(Widget parent,
					     char *name,
					     char **pix,
					     codaComd *cmd,
					     char *symbol)
:XcodaPbtnInterface (parent, cmd)
{
  Arg           arg[10];
  int           ac = 0;
  int           height = 0;

  ac = 0;
  XtSetArg(arg[ac], XmNheight, &height); ac++;
  XtGetValues(parent, arg, ac); 

#ifdef _TRACE_OBJECTS
  printf("Create XcodaXpmpbtnInterface Objects \n");
#endif
  _parent = parent;
  _name = name;

  height_ = 72;
  npixmap = 0;
  apixmap = 0;
  upixmap = 0;

  if (pix) {
    npixmap = XcodaCreatePixmapFromXpm(parent,pix,1);
  } else {
    {
#include "Button4.xpm"
      npixmap = XcodaCreatePixmapFromXpm(parent,Button4_xpm,1);
    }
    {
#include "Button5.xpm"
      apixmap = XcodaCreatePixmapFromXpm(parent,Button5_xpm,1);
    }
    {
#include "Button4.xpm"
      upixmap = XcodaCreatePixmapFromXpm(parent,Button4_xpm,1);
    }
  }
}

XcodaXpmpbtnInterface::~XcodaXpmpbtnInterface()
{
#ifdef _TARCE_OBJECTS
  printf("Destroy XcodaXpmpbtnInterface Objects \n");
#endif
}

void XcodaXpmpbtnInterface::init ()
{
  Arg           arg[10];
  int           ac = 0;
  XGCValues gcv;

  Window root_w;
  Display   *dpy;
  Screen    *scr;
  Colormap  cmap;
  Pixel     temp,temp2, temp3, fg , bg;
  int height;
  armed = 0;

  dpy = XtDisplay(_parent);
  scr = XtScreen(_parent);
  root_w = DefaultRootWindow(dpy);

  cmap = DefaultColormapOfScreen(scr);
  ac = 0;
  XtSetArg(arg[ac], XmNbackground, &bg); ac++;
  XtSetArg(arg[ac], XmNforeground, &fg); ac++;
  XtGetValues(_parent, arg, ac); 

  xgc = XCreateGC(dpy, root_w,GCForeground|GCBackground,
		      &gcv);
  if (tfont == 0) {
    if ((tfont = XLoadQueryFont(dpy,"-*-times-bold-r-*-*-14-*-*-*-*-*-*-*"))
	== NULL ){
      perror("cannot find font:-*-times-bold-r-*-*-14-*-*-*-*-*-*-*");
      exit(1);
    }  
  }

  XSetFont(dpy, xgc, tfont->fid);
  // from XcodaXpm class

  int text_wd;
  int text_h;

  if (_name) {
    text_wd = XTextWidth(tfont, _name, 
			 strlen(_name));
    text_h = tfont->max_bounds.ascent / 2 + tfont->max_bounds.descent;
  }

  temp = get_pixel(dpy, cmap,0,0,0);  
  temp2 = get_pixel(dpy, cmap, 65535, 65535, 66635);  
  temp3 = get_pixel(dpy, cmap, 65535, 0, 0);
  
  if (npixmap && apixmap) { 
    if (_name) {
      XSetForeground(dpy, xgc, temp);
      XDrawString(dpy, npixmap, xgc,(BUTTON_WIDTH2)/2+2, (height_/2 + text_h/2)+2, _name, strlen(_name));
      XSetForeground(dpy, xgc, temp2);
      XDrawString(dpy, npixmap, xgc, (BUTTON_WIDTH2)/2, (height_/2 + text_h/2), _name, strlen(_name));
    }
  }

  if (apixmap) { 
    if (_name) {
      XSetForeground(dpy, xgc, temp);
      XDrawString(dpy, apixmap, xgc, (BUTTON_WIDTH2)/2+2, (height_/2 + text_h/2)+2, _name, strlen(_name));
      XSetForeground(dpy, xgc, temp2);
      XDrawString(dpy, apixmap, xgc, (BUTTON_WIDTH2)/2, (height_/2 + text_h/2), _name, strlen(_name));
    } 
  }
  if (upixmap) {
    if (_name) {
      XSetForeground(dpy, xgc, temp);
      XDrawString(dpy, upixmap, xgc, (BUTTON_WIDTH2)/2+2, (height_/2 + text_h/2)+2, _name, strlen(_name));
      XSetForeground(dpy, xgc, temp3);
      XDrawString(dpy, upixmap, xgc, (BUTTON_WIDTH2)/2, (height_/2 + text_h/2), _name, strlen(_name));
    }
  }
  ac = 0;

  if(apixmap){
    _w = XtCreateWidget (_name, xmDrawnButtonWidgetClass, _parent,
			 arg, ac);

    XtVaSetValues(_w, XmNwidth,BUTTON_WIDTH,XmNheight,height_,NULL );

    XtAddEventHandler (_w, EnterWindowMask | LeaveWindowMask, FALSE, 
		       (XtEventHandler)&XcodaXpmpbtnInterface::crossEventHandler,
		       (XtPointer)this);

    XtAddCallback (_w, XmNactivateCallback, 
		       &XcodaXpmpbtnInterface::xEventHandler,
		       (XtPointer)this);
    
    XtAddCallback (_w, XmNexposeCallback, 
		       &XcodaXpmpbtnInterface::xEventHandler,
		       (XtPointer)this);
    
    XtAddCallback (_w, XmNarmCallback, 
		       &XcodaXpmpbtnInterface::xEventHandler,
		       (XtPointer)this);
    
    XtAddCallback (_w, XmNdisarmCallback, 
		       &XcodaXpmpbtnInterface::xEventHandler,
		       (XtPointer)this);
    
    ac = 0;
  }
  else {
    ac = 0;
    if (npixmap) {
      XtSetArg (arg[ac], XmNlabelType, XmPIXMAP); ac++;
      XtSetArg (arg[ac], XmNlabelPixmap, npixmap); ac++;      
    }
    _w = XtCreateWidget (_name, xmPushButtonWidgetClass, _parent,
			 arg, ac);
  }

  installDestroyHandler();

  if(_active)
    activate();
  else
    deactivate();
  
  XtAddCallback (_w,
		 XmNactivateCallback,
		 (XtCallbackProc)&codaComdXInterface::executeComdCallback,
		 (XtPointer)this);

  XtManageChild (_w);
}

void
XcodaXpmpbtnInterface::crossEventHandler (Widget wid, XtPointer clientData,
					  XEvent* event)
{
  XcodaXpmpbtnInterface* obj = (XcodaXpmpbtnInterface *)clientData;
  XCrossingEvent* cev = (XCrossingEvent *)event;
  if ((obj->armed)||(obj->_active == 0)) 
    return;

  switch (cev->type) {
  case EnterNotify :
    XCopyArea(XtDisplay(wid),obj->upixmap,XtWindow(wid),obj->xgc,0,0,BUTTON_WIDTH,obj->height_,0,0);
    break;
  case LeaveNotify :
    XCopyArea(XtDisplay(wid),obj->npixmap,XtWindow(wid),obj->xgc,0,0,BUTTON_WIDTH,obj->height_,0,0);
    break;
  }
}


void
XcodaXpmpbtnInterface::xEventHandler (Widget wid, XtPointer clientData,
				 XtPointer event)
{
  XcodaXpmpbtnInterface* obj = (XcodaXpmpbtnInterface *)clientData;
  XmDrawnButtonCallbackStruct* cbs = (XmDrawnButtonCallbackStruct *)event;
  if (!XtIsRealized(wid)||(obj->_active == 0)) 
    return;

  switch (cbs->reason) {
  case XmCR_ACTIVATE :
    XCopyArea(XtDisplay(wid),obj->apixmap,XtWindow(wid),obj->xgc,0,0,BUTTON_WIDTH,obj->height_,0,0);
    break;
  case XmCR_DISARM :
    XCopyArea(XtDisplay(wid),obj->npixmap,XtWindow(wid),obj->xgc,0,0,BUTTON_WIDTH,obj->height_,0,0);
    obj->armed = 0;
    break;
  case XmCR_EXPOSE :
    XCopyArea(XtDisplay(wid),obj->npixmap,XtWindow(wid),obj->xgc,0,0,BUTTON_WIDTH,obj->height_,0,0);
    break;
  case XmCR_ARM :
    XCopyArea(XtDisplay(wid),obj->apixmap,XtWindow(wid),obj->xgc,0,0,BUTTON_WIDTH,obj->height_,0,0);
    obj->armed = 1;
    break;
  }
}









