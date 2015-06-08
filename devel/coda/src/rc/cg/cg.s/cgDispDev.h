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
//      X11 Window Display Device (Abstract Class)
//      Based on XtWidget (AppPlusShell)
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgDispDev.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:06  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_DISP_DEV_H
#define _CG_DISP_DEV_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <AppPlusS.h>

// standard colormap
#define CG_STD_CMAP 0
// private colormap
#define CG_PRIVATE_CMAP 1

// creation mode
#define CG_POPUP_MODE 1
#define CG_SIMPLE_MODE 0

class cgDispDev;

typedef void (*cgAction)(cgDispDev* disp, void *arg);

class cgColorCxt;
class cgContext;

class cgScene;

class cgDispDev
{
public:
  // mode = 1 for popup, mode = 0 for simple widget only (std colormap)
  cgDispDev          (Widget parent, char* name, 
		      Dimension width = 300,
		      Dimension height = 300,
		      int       mode = CG_POPUP_MODE,
		      int colormode = CG_STD_CMAP);
  virtual ~cgDispDev (void);

  // size information
  virtual Dimension width  (void) const;
  virtual Dimension height (void) const;
  // Forground and Background 
  virtual Pixel     fg     (void) const;
  virtual Pixel     bg     (void) const;
  // manage and unmanage
  virtual void manage      (void);
  virtual void unmanage    (void);

  // register action function
  void         btn1action  (cgAction action, void* arg);
  void         btn2action  (cgAction action, void* arg);
  void         btn3action  (cgAction action, void* arg);
  
  // create color context
  void         createColorCxt (void);
  cgColorCxt*  colorCxt       (void) const;
  // return base widget
  const Widget baseWidget (void)  {return w_;}
  // return shell widget
  const Widget shell      (void)  {return shell_;}

  // register drawing scene
  virtual void registerScene (cgScene* scene);

  // Some X window information
  Display*  display  (void) const;
  Colormap  cmap     (void) const;
  Drawable  drawable (void) const;
  Window    window   (void) const;
  Window    rootW    (void) const;
  int       depth    (void) const;

  // use widget w as index to get cgDispDev
  static cgDispDev* disp    (Widget w);


  virtual const char* className (void) const {return "cgDispDev";}

protected:
  void  createWidget          (Dimension width, Dimension height);
  void  installDestroyHandler (void); // easy hook for derived class
  // destroy callback
  virtual void widgetDestroyed (void);
  static  void widgetDestroyedCallback (Widget,
					XtPointer,
					XtPointer);
  static void  redisplay   (Widget, XtPointer, XEvent *, Boolean);
  static void  resize      (Widget, XtPointer, XEvent *, Boolean);

  // device list routines
  static void  addDevice    (cgDispDev* dev);
  static void  removeDevice (cgDispDev* dev);

  // data area
  int                mode_;
  int                colormode_;
  char*              name_;
  Widget             parent_;
  Widget             w_;
  Widget             shell_;
  Display            *display_;
  Screen             *screen_;
  int                depth_;
  Window             window_;
  Window             root_;
  Drawable           drawable_;
  // X window Color context
  XCC                xcc_;
  // CG class Clolr context
  cgColorCxt*        colorCxt_;

  // list of drawing scenes
  cgScene**          scenes_;
  int                numScenes_;

  // list of actions for button press
  cgAction           btn1action_;
  void*              btn1arg_;
  cgAction           btn2action_;
  void*              btn2arg_;
  cgAction           btn3action_;
  void*              btn3arg_;

  // flag to tell the first time exposed event
  int                firstTime_;

  // list of all cgDisplay devices
  static             cgDispDev** devList;
  static             int         numDevices;
  static             int         actionRegistered;

  // friend class
  friend class       cgColorCxt;
  friend class       cgContext;
  friend void        cgSelectProc (Widget, XEvent*, String*, Cardinal*);
  friend void        cgActionProc (Widget, XEvent*, String*, Cardinal*);
  friend void        cgButton3Proc (Widget, XEvent*, String*, Cardinal*);
  friend void        cgButton2Proc (Widget, XEvent*, String*, Cardinal*);
  

  // deny access to assignment and copy operations
  cgDispDev         (const cgDispDev&);
  cgDispDev& operator = (const cgDispDev&);
};
#endif
