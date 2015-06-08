//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      X11 Window Display Device Abstraction
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgDispDev.cc,v $
//   Revision 1.5  1998/05/28 17:46:14  heyes
//   new GUI look
//
//   Revision 1.4  1998/04/08 18:31:01  heyes
//   new look and feel GUI
//
//   Revision 1.3  1997/08/25 16:01:50  heyes
//   fix some display problems
//
//   Revision 1.2  1997/08/21 19:28:50  heyes
//   fix borders and color problems
//
//   Revision 1.1.1.1  1996/10/10 19:38:05  chen
//   CODA OO-graphics library for Xt
//
//
#include "cgDispDev.h"
#include <cgColorCxt.h>
#include <cgScene.h>
#include <Xm/DrawingA.h>

//========================================================================
//            Xt Intrinsic Translation Stuff
//========================================================================
extern 
void cgSelectProc (Widget w, XEvent* event, String* params, Cardinal* num);

extern
void cgActionProc (Widget w, XEvent* event, String* params, Cardinal* num);

extern
void cgButton3Proc (Widget w, XEvent* event, String* params, Cardinal* num);

extern
void cgButton2Proc (Widget w, XEvent* event, String* params, Cardinal* num);


static XtActionsRec actions[]=
{
  {"Select", cgSelectProc},
  {"Action", cgActionProc},
  {"GlobalAction3", cgButton3Proc},
  {"GlobalAction2", cgButton2Proc},
};

// translations of pick or action
static char pickBehaviour[]="\
<Btn1Down>: Select()\n\
<Btn1Down>(2): Action()\n\
<Btn3Down>: GlobalAction3()\n\
<Btn2Down>: GlobalAction2()\n\
";

//==========================================================================
//           Implementation of cgDispDev Class
//=========================================================================

cgDispDev** cgDispDev::devList = 0;
int         cgDispDev::numDevices = 0;
int         cgDispDev::actionRegistered = 0;

cgDispDev::cgDispDev (Widget parent, char *name, 
		      Dimension width, Dimension height, 
		      int mode, int colormode)
:mode_ (mode), colormode_ (colormode), parent_ (parent), 
 w_ (0), shell_ (0), colorCxt_ (0), firstTime_ (1),
 scenes_ (0), numScenes_ (0), 
 btn1action_ (0), btn2action_ (0), btn3action_ (0),
 btn1arg_ (0), btn2arg_ (0), btn3arg_ (0),
 window_ (0), drawable_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create cgDispDev Class Object\n");
#endif
  // first update device list
  cgDispDev::addDevice (this);
  if (cgDispDev::actionRegistered == 0) {
    cgDispDev::actionRegistered = 1;
    XtAppAddActions (XtWidgetToApplicationContext (parent),
		     actions, 4);
  }
    
  name_ = new char[::strlen (name) + 1];
  ::strcpy (name_, name);

  createWidget (width, height);
  installDestroyHandler ();

  display_ = ::XtDisplay (w_);
  screen_ = XDefaultScreenOfDisplay (display_);
  root_ = XDefaultRootWindow (display_);
  depth_ = DefaultDepth (display_, DefaultScreen (display_));  
}

void
cgDispDev::createWidget (Dimension width, Dimension height)
{
  Arg args[10];
  int ac = 0;

  // find parent background and foreground
  Pixel fg, bg;
  XtSetArg (args[ac], XtNbackground, &bg); ac++;
  XtGetValues (parent_, args, ac);
  ac = 0;

  if (mode_ == CG_POPUP_MODE) {
    if (colormode_ == CG_PRIVATE_CMAP) {
      XtSetArg (args[ac], XtNusePrivateColormap, TRUE);
      ac++;
    }
    shell_ = XtCreatePopupShell (name_, appPlusShellWidgetClass, parent_,
				 args, ac);
    ac = 0;
    // create color context
    //xcc_ = AppPlusShellGetXCC (shell_);
    
    XtSetArg (args[ac], XtNwidth, width); ac++;
    XtSetArg (args[ac], XtNheight, height); ac++;
    w_ = XtCreateManagedWidget ("cgDrawingPanel", xmDrawingAreaWidgetClass, shell_,
				args, ac);
    ac = 0;
    // add event handlers
    XtAddEventHandler (w_, ExposureMask, FALSE,
		       (XtEventHandler)&(cgDispDev::redisplay), (void *)this);
    XtAddEventHandler (w_, StructureNotifyMask, FALSE,
		       (XtEventHandler)&(cgDispDev::resize), (void *)this);
    // add translation manager
    XtOverrideTranslations (w_, XtParseTranslationTable (pickBehaviour));
  }
  else {
    Screen* scr = XDefaultScreenOfDisplay (XtDisplay (parent_));
    Colormap cmp = XDefaultColormapOfScreen (scr);

    //xcc_ = XCCCreate (parent_, XDefaultVisualOfScreen (scr), cmp);

    XtSetArg (args[ac], XtNborderWidth, 0); ac++;
    XtSetArg (args[ac], XtNwidth, width); ac++;
    XtSetArg (args[ac], XtNheight, height); ac++;
    w_ = XtCreateManagedWidget (name_, xmDrawingAreaWidgetClass, parent_,
				args, ac);
    ac = 0;    
    shell_ = 0;

    // add event handlers
    XtAddEventHandler (w_, ExposureMask, FALSE,
		       (XtEventHandler)&(cgDispDev::redisplay), (void *)this);
    XtAddEventHandler (w_, StructureNotifyMask, FALSE,
		       (XtEventHandler)&(cgDispDev::resize), (void *)this);
    // add translation manager
    XtOverrideTranslations (w_, XtParseTranslationTable (pickBehaviour));
  }
}

Dimension
cgDispDev::width (void) const
{
  assert (w_);
  Arg args[10];
  int ac = 0;
  Dimension width = 0;

  XtSetArg (args[ac], XtNwidth, &width); ac++;
  XtGetValues (w_, args, ac);
  ac = 0;
  return width;
}

Dimension
cgDispDev::height (void) const
{
  assert (w_);
  Arg args[10];
  int ac = 0;
  Dimension height = 0;

  XtSetArg (args[ac], XtNheight, &height); ac++;
  XtGetValues (w_, args, ac);
  ac = 0;
  return height;
}

Pixel
cgDispDev::fg (void) const
{
  assert (w_);
  return BlackPixelOfScreen (XDefaultScreenOfDisplay (XtDisplay (w_)));
}

Pixel
cgDispDev::bg (void) const
{
  assert (w_);
  Arg args[10];
  int ac = 0;
  Pixel pix;

  XtSetArg (args[ac], XtNbackground, &pix); ac++;
  XtGetValues (w_, args, ac);
  ac = 0;
  return pix;
}

void
cgDispDev::widgetDestroyedCallback (Widget,
				    XtPointer clientData,
				    XtPointer)
{
  cgDispDev* obj = (cgDispDev *)clientData;
  obj->widgetDestroyed ();
}

void
cgDispDev::widgetDestroyed (void)
{
  w_ = 0;
  drawable_ = 0;
  window_ = 0;
  delete this;
}

void
cgDispDev::installDestroyHandler (void)
{
  assert (w_);
  XtAddCallback (w_, XtNdestroyCallback,
		 (XtCallbackProc)&cgDispDev::widgetDestroyedCallback,
		 (XtPointer)this);
}

void
cgDispDev::manage (void)
{
  assert (XtHasCallbacks (w_, XtNdestroyCallback) == XtCallbackHasSome);
  if (mode_ == CG_POPUP_MODE) {
    assert (shell_);
    XtPopup (shell_, XtGrabNone);
  }
  else 
    XtManageChild (w_);
}

void
cgDispDev::unmanage (void)
{
  if (mode_ == CG_POPUP_MODE) {
    assert (shell_);
    XtPopdown (shell_);
  }
  else
    XtUnmanageChild (w_);
}

void
cgDispDev::createColorCxt (void)
{
  //assert (xcc_);
  colorCxt_ = new cgColorCxt (*this);
}

cgColorCxt*
cgDispDev::colorCxt (void) const
{
  return colorCxt_;
}


cgDispDev::~cgDispDev (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete cgDispDev Class Object\n");
#endif
  //if (xcc_) {
  //  XCCFree (xcc_);
  //  xcc_ = 0;
  // }

  if (colorCxt_) {
    delete colorCxt_;
    colorCxt_ = 0;
  }

  if (numScenes_ != 0) {
    for (int i = 0; i < numScenes_; i++)
      delete scenes_[i];
    delete []scenes_;
  }
  numScenes_ = 0;
  
  if (w_) {
    XtRemoveCallback (w_,
		      XtNdestroyCallback,
		      &cgDispDev::widgetDestroyedCallback,
		      (XtPointer)this);
  }
  // remove name
  delete []name_;
}

Display*
cgDispDev::display (void) const
{
  return display_;
}

Colormap
cgDispDev::cmap (void) const
{
  assert (colorCxt_);
  //assert (xcc_);
  //return XCCGetColormap (xcc_);
  return NULL;
}

Drawable
cgDispDev::drawable (void) const
{
  return drawable_;
}

Window
cgDispDev::window (void) const
{
  return window_;
}

Window
cgDispDev::rootW (void) const
{
  return root_;
}

int
cgDispDev::depth (void) const
{
  return depth_;
}

void
cgDispDev::registerScene (cgScene* sc)
{
  int i;
  if (numScenes_ == 0) {
    scenes_ = new cgScene*[1];
    scenes_[0] = sc;
    numScenes_ = 1;
  }
  else {
    cgScene** newscenes = new cgScene*[numScenes_ + 1];
    for (i = 0; i < numScenes_; i++)
      newscenes[i] = scenes_[i];
    newscenes[i] = sc;
    delete []scenes_;
    scenes_ = newscenes;
    numScenes_ ++;
  }
}

void
cgDispDev::addDevice (cgDispDev* dev)
{
  int i;
  if (cgDispDev::numDevices == 0) {
    cgDispDev::devList = new cgDispDev*[1];
    cgDispDev::devList[0] = dev;
    cgDispDev::numDevices = 1;
  }
  else {
    cgDispDev** newlist = new cgDispDev*[cgDispDev::numDevices + 1];
    for (i = 0; i < cgDispDev::numDevices; i++)
      newlist[i] = cgDispDev::devList[i];
    newlist[i] = dev;

    delete [](cgDispDev::devList);
    cgDispDev::numDevices += 1;
    cgDispDev::devList = newlist;
  }
}

void
cgDispDev::removeDevice (cgDispDev* dev)
{
  assert (cgDispDev::numDevices > 0);
  if (cgDispDev::numDevices == 1) { // last one
    delete [](cgDispDev::devList);
    cgDispDev::numDevices = 0;
  }
  else {
    cgDispDev** newlist = new cgDispDev*[cgDispDev::numDevices - 1];
    int j = 0;
    for (int i = 0; i < cgDispDev::numDevices; i++) {
      if (dev != cgDispDev::devList[i])
	newlist[j++] = cgDispDev::devList[i];
    }
    delete [](cgDispDev::devList);
    cgDispDev::devList = newlist;
    cgDispDev::numDevices -= 1;
  }
}

cgDispDev*
cgDispDev::disp (Widget w)
{
  for (int i = 0; i < cgDispDev::numDevices; i++) {
    if (w == (cgDispDev::devList[i])->baseWidget ())
      return cgDispDev::devList[i];
  }
  fprintf (stderr, "Cannot find any display device, Fatal!!!!\n");
  return 0;
}

void
cgDispDev::redisplay (Widget w, XtPointer clientData,
		      XEvent*, Boolean)
{
  cgDispDev* obj = (cgDispDev *)clientData;
  static int wd = 0;
  static int ht = 0;
  int i = 0;

  if (obj->firstTime_) {
    // at this time window has been just created
    obj->window_ = XtWindow (w);
    obj->drawable_ = (Drawable)(obj->window_);
    obj->firstTime_ = 0;
  }

  if (wd != obj->width ()) {
    wd = obj->width();
    for (i = 0; i < obj->numScenes_; i++) 
      obj->scenes_[i]->devWidth ((double)wd);
  }

  if (ht != obj->height ()) {
    ht = obj->height ();
    for (i = 0; i < obj->numScenes_; i++) 
      obj->scenes_[i]->devHeight ((double)ht);
  }

  for (i = 0; i < obj->numScenes_; i++) 
    obj->scenes_[i]->draw ();
}

void
cgDispDev::resize (Widget w, XtPointer clientData,
		   XEvent* event, Boolean)
{
  cgDispDev* obj = (cgDispDev *)clientData;
  static int wd = 0;
  static int ht = 0;
  XConfigureEvent *cev = (XConfigureEvent *)event;
  int    sizeChanged = 0;
  int    i = 0;

  if (!obj->firstTime_) { // already exposed
    for (i = 0; i < obj->numScenes_; i++) 
      obj->scenes_[i]->erase ();
  }

  if (wd != cev->width) {
    wd = cev->width;
    for (i = 0; i < obj->numScenes_; i++) 
      obj->scenes_[i]->devWidth ((double)wd);
    sizeChanged = 1;
  }
  if (ht != cev->height) {
    ht = cev->height;
    for (i = 0; i < obj->numScenes_; i++) 
      obj->scenes_[i]->devHeight ((double)ht);
    sizeChanged = 1;
  }
  if (!obj->firstTime_) { // already exposed
    for (i = 0; i < obj->numScenes_; i++) 
      obj->scenes_[i]->draw ();
  }
}

void
cgDispDev::btn1action (cgAction action, void* arg)
{
  btn1action_ = action;
  btn1arg_ = arg;
}

void
cgDispDev::btn2action (cgAction action, void* arg)
{
  btn2action_ = action;
  btn2arg_ = arg;
}

void
cgDispDev::btn3action (cgAction action, void* arg)
{
  btn3action_ = action;
  btn3arg_ = arg;
}



//==========================================================================
//               Implementation of static action routines
//==========================================================================
void cgSelectProc (Widget w, XEvent* event, String*, Cardinal*)
{
  cgDispDev* obj = cgDispDev::disp (w);
  XButtonEvent *bev = (XButtonEvent *)event;
  cgPoint    devp;
  cgPrim* gobj = 0;

  if (obj->btn1action_)
    (*(obj->btn1action_)) (obj, obj->btn1arg_);

  for (int i = 0; i < obj->numScenes_; i++) {
    const cgWinView& wv = obj->scenes_[i]->givwv ();
    // convert corrdinate from device to window
    devp.setx (bev->x);
    devp.sety (bev->y);
    cgPoint wp = wv.dev_win (devp);
    // the following contain routine only return the first object
    if ((gobj = obj->scenes_[i]->contains (wp)) != 0) 
      gobj->select (obj->scenes_[i]);
  }
}

void cgButton3Proc (Widget w, XEvent* event, String*, Cardinal*)
{
  cgDispDev* obj = cgDispDev::disp (w);

  if (obj->btn3action_) 
    (* (obj->btn3action_)) (obj, obj->btn3arg_);
}

void cgButton2Proc (Widget w, XEvent* event, String*, Cardinal*)
{
  cgDispDev* obj = cgDispDev::disp (w);

  if (obj->btn2action_)
    (* (obj->btn2action_)) (obj, obj->btn2arg_);
}

void cgActionProc (Widget w, XEvent* event, String*, Cardinal*)
{
  cgDispDev* obj = cgDispDev::disp (w);

  XButtonEvent *bev = (XButtonEvent *)event;
  cgPoint    devp;
  cgPrim* gobj = 0;

  for (int i = 0; i < obj->numScenes_; i++) {
    const cgWinView& wv = obj->scenes_[i]->givwv ();
    // convert corrdinate from device to window
    devp.setx (bev->x);
    devp.sety (bev->y);
    cgPoint wp = wv.dev_win (devp);
    // the following contain routine only return the first object
    if ((gobj = obj->scenes_[i]->contains (wp)) != 0)
      gobj->action (obj->scenes_[i]);
  }
}
