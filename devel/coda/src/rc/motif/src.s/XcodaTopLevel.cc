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
//	CODA Toplevel Shell Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaTopLevel.cc,v $
//   Revision 1.4  1998/04/08 17:29:39  heyes
//   get in there
//
//   Revision 1.3  1997/06/06 19:00:42  heyes
//   new RC
//
//   Revision 1.2  1997/04/16 18:12:21  chen
//   add multilist and extensions
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include "XcodaApp.h"
#include "XcodaTopLevel.h"
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <assert.h>

XcodaTopLevel::XcodaTopLevel( char *name )
: XcodaUi ( name )
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaTopLevel Objects \n");
#endif
  _base_widget = (Widget)NULL;
  assert ( theApplication ); // Application object must exist
  // before any MainWindow object
  theApplication->registerWindow ( this );
}

void XcodaTopLevel::initialize (void)
{
  // All toplevel windows in the MotifApp framework are 
  // implemented as a popup shell off the Application's
  // base widget.
    
  _w = XtCreatePopupShell ( _name, 
			   applicationShellWidgetClass,
			   theApplication->baseWidget(),
			   NULL, 0 );

  installDestroyHandler();

  _base_widget = CreateBaseWidget(_w);
  assert(_base_widget);

  // Manage Base widget if the derived class hasn't done so
  if(!XtIsManaged (_base_widget))
    XtManageChild (_base_widget);
}

XcodaTopLevel::~XcodaTopLevel( )
{
#ifdef _TRACE_OBJECTS
  printf("Destroy XcodaTopLevel Objects \n");
#endif
  // Unregister this window with the Application object
  // if the list inside theApplication is not locked
  if (!theApplication->_listLocked)
    theApplication->unregisterWindow ( this );
}

void
XcodaTopLevel::manage (void)
{
  assert ( _w );
  XtPopup ( _w, XtGrabNone );
  
  // Map the window, in case the window is iconified
    
  if ( XtIsRealized ( _w ) )
    XMapRaised ( XtDisplay ( _w ), XtWindow ( _w ) );
}

void
XcodaTopLevel::unmanage (void)
{
  assert ( _w );
  XtPopdown ( _w );
}

void 
XcodaTopLevel::iconify(void)
{
  assert ( _w );
    
  // Set the widget to have an initial iconic state
  // in case the base widget has not yet been realized
    
  XtVaSetValues ( _w, XmNiconic, TRUE, NULL );
    
  // If the widget has already been realized,
  // iconify the window
    
  if ( XtIsRealized ( _w ) )
    XIconifyWindow ( XtDisplay ( _w ), XtWindow ( _w ), 0 );
}

void
XcodaTopLevel::defineCursor (unsigned int cursor)
{
  Cursor rc = XCreateFontCursor (XtDisplay (_w), cursor);
  XDefineCursor (XtDisplay (_w), XtWindow (_w), rc);
  XmUpdateDisplay (_w);
}

void
XcodaTopLevel::undefineCursor (void)
{
  XUndefineCursor (XtDisplay (_w), XtWindow (_w));
}



