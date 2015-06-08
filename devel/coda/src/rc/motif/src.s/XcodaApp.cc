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
//	CODA Motif Application Frame Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaApp.cc,v $
//   Revision 1.9  2000/01/28 16:39:10  rwm
//   include should be X11/xpm.h
//
//   Revision 1.8  1998/04/08 17:28:48  heyes
//   get in there
//
//   Revision 1.7  1997/10/20 12:45:51  heyes
//   first tag for B
//
//   Revision 1.6  1997/08/20 18:38:44  heyes
//   fix up for SunOS
//
//   Revision 1.5  1997/08/18 13:26:35  heyes
//   pixmap import
//
//   Revision 1.4  1997/08/01 18:38:58  heyes
//   add background pixmap support
//
//   Revision 1.3  1997/05/30 12:44:23  heyes
//   add handleone
//
//   Revision 1.1.1.1  1996/10/10 19:25:00  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include "XcodaApp.h"
#include "XcodaTopLevel.h"
#include <assert.h>
#include <X11/xpm.h>
#include <stone.xpm>

extern "C" void exit(int);

extern "C" Pixmap XcodaCreatePixmapFromXpm(Widget,char**,int);

Pixmap bg_pixmap;

XcodaApp *theApplication = NULL;

XcodaApp::XcodaApp ( char *appClassName )
:XcodaUi ( appClassName ), _opened (0), _listLocked (0)
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaApp Object\n");
#endif
  // Set the global Application pointer
  theApplication = this;  
  // Initialize data members
    
  _display    = NULL;
  _appContext = NULL;
  _windows    = NULL;
  _numWindows = 0;

  _applicationClass = new char[strlen(appClassName) + 1];
  ::strcpy(_applicationClass, appClassName);
    
  _fallback_resources = 0;
}

XcodaApp::XcodaApp ( char *appClassName, char **res)
:XcodaUi ( appClassName ), _listLocked (0)
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaApp Object\n");
#endif
  // Set the global Application pointer
    
  theApplication = this;  
    
  // Initialize data members
    
  _display    = NULL;
  _appContext = NULL;
  _windows    = NULL;
  _numWindows = 0;

  _applicationClass = new char[strlen(appClassName) + 1];
  ::strcpy(_applicationClass, appClassName);

  // Do not need to duplicate fallback resources
  _fallback_resources = res;
}

int
XcodaApp::open (int* argcp, char** argv)
{
  if(_fallback_resources){
    _w = XtAppInitialize ( &_appContext, 
			  _applicationClass, NULL, 0, 
			  argcp, argv, 
			  _fallback_resources, NULL, 0 );
  }
  else
    _w = XtAppInitialize ( &_appContext, 
			  _applicationClass, NULL, 0, 
			  argcp, argv, 
			  NULL, NULL, 0 );
  if (_w != 0) {
    _opened = 1;
    return 1;
  }
  else
    return 0;
}

int Xhandler(Display *d, XErrorEvent *e)
{
  return 0;
}

#if (XlibSpecificationRelease>=5)
void XcodaApp::initialize ( int *argcp, char **argv )
#else
void XcodaApp::initialize ( unsigned int *argcp, char **argv )
#endif
{
  Arg  arg[20];
  int  ac = 0;

  if (!_opened)
    open (argcp, argv);
  
  if (!_opened) {
    fprintf (stderr, "Cannot connect to X display\n");
    exit (1);
  }
  // Extract and save a pointer to the X display structure
    
  _display = XtDisplay ( _w );
    
  // The Application class is less likely to need to handle
  // "surprise" widget destruction than other classes, but
  // we might as well install a callback to be safe and consistent
    
  installDestroyHandler();
    
  // Center the shell, and make sure it isn't visible

  XtSetArg (arg[ac], XmNmappedWhenManaged, FALSE); ac++;
  XtSetArg (arg[ac], XmNx, DisplayWidth ( _display, 0 ) / 2); ac++;
  XtSetArg (arg[ac], XmNy, DisplayHeight ( _display, 0 ) / 2); ac++;
  XtSetArg (arg[ac], XmNwidth,  1); ac++;
  XtSetArg (arg[ac], XmNheight, 1); ac++;
  XtSetValues ( _w, arg, ac);
  ac = 0;
    
  // The instance name of this object was set in the XcodaUi 
  // constructor, before the name of the program was available
  // Free the old name and reset it to argv[0]
  
  delete []_name;
  
  _name = new char[strlen(argv[0])+1];
  ::strcpy (_name, argv[0]);

  // Force the shell window to exist so dialogs popped up from
  // this shell behave correctly
    
  XtRealizeWidget ( _w );
    
  // Install background pixmaps
  bg_pixmap =  XcodaCreatePixmapFromXpm(_w,magick, 1);
  XImage *image = XGetImage(XtDisplay(_w), bg_pixmap, 0, 0, 159, 160, AllPlanes, XYPixmap);
  // image->depth = 1;
  XmInstallImage (image, "bg_pixmap1");
  
  XSetErrorHandler (Xhandler);

  // Initialize and manage any windows registered
  // with this application.
    
  for ( int i = 0; i < _numWindows; i++ ) {
    _windows[i]->initialize();
    _windows[i]->manage();
  }

}

XcodaApp::~XcodaApp (void)
{
#ifdef _TRACE_OBJECTS
  printf ("XcodaApp Destroy Itself\n");
#endif
  delete []_applicationClass;
  removeAllWindows ();
}

void XcodaApp::cleanMemory()
{
  if (_w){
    // printf("Clean up C++ objects ...\n");
    XtIntervalId timer_id;
    timer_id = XtAppAddTimeOut (_appContext, 3000,
				&XcodaApp::exitTimer, (XtPointer)this);
    delete this;
    theApplication = NULL;
  }
}

void
XcodaApp::execute (void)
{
  // Just loop forever
  XtAppMainLoop ( _appContext );
}

void
XcodaApp::handleone (void)
{
  // Just one X event
  XtInputMask mask;
  mask = XtAppPending(_appContext);
  if (mask) {
    XtAppProcessEvent ( _appContext,mask );
  }
}

void 
XcodaApp::registerWindow ( XcodaTopLevel *window )
{
  int i;
  XcodaTopLevel **newList;
    
  // Allocate a new list large enough to hold the new
  // object, and copy the contents of the current list 
  // to the new list
    
  newList = new XcodaTopLevel*[_numWindows + 1];
    
  for ( i = 0; i < _numWindows; i++ )
    newList[i] = _windows[i];
    
  // Install the new list and add the window to the list
    
  delete []_windows;
  _windows =  newList;
  _windows[_numWindows] = window;
    
  _numWindows++;
}

void
XcodaApp::unregisterWindow ( XcodaTopLevel *window )
{
  int i, index;
  XcodaTopLevel **newList;

  if(_numWindows <= 1){
    _windows[0] = 0;
    delete []_windows;
    _numWindows = 0;
    _windows = 0;
    return ;
  }

  // Allocate a new, smaller list
  
  newList = new XcodaTopLevel*[_numWindows - 1];
    
  // Copy all objects, except the one to be 
  // removed, to the new list
    
  index = 0;
  for ( i = 0; i < _numWindows; i++ )
    if ( _windows[i] != window )
      newList[index++] = _windows[i];
    
  // Install the new list
  delete []_windows;
  _windows =  newList;
    
  _numWindows--;
}

void
XcodaApp::removeAllWindows()
{
  // lock this list first
  _listLocked = 1;
  // delete all windows
  for(int i = 0; i < _numWindows; i++) {
    delete _windows[i];
  }
  _listLocked = 0;
  if(_numWindows)
    delete []_windows;
}

void
XcodaApp::manage (void)
{
  // Manage all application windows. This will pop up
  // iconified windows as well.
    
  for ( int i = 0; i < _numWindows; i++ )
    _windows[i]->manage();
}

void 
XcodaApp::unmanage (void)
{
  // Unmanage all application windows
    
  for ( int i = 0; i < _numWindows; i++ )
    _windows[i]->unmanage();
}

void
XcodaApp::iconify(void)
{
  // Iconify all top-level windows.
    
  for ( int i = 0; i < _numWindows; i++ )
    _windows[i]->iconify();
}

void 
XcodaApp::exitTimer (XtPointer client_data, XtIntervalId *id)
{
  exit (0);
}
