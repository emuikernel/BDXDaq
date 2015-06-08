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
//	Coda Motif User Interface Component
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaUi.cc,v $
//   Revision 1.2  1998/04/08 17:29:40  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include "XcodaUi.h"
#include <assert.h>
#include <stdio.h>

XcodaUi::XcodaUi ( const char *name ) : XcodaBasic ( name )
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaUi Object \n");
#endif
    // Empty
}

void XcodaUi::widgetDestroyedCallback ( Widget, 
				       XtPointer clientData, 
				       XtPointer )
{
  XcodaUi * obj = (XcodaUi *) clientData;	
    
  obj->widgetDestroyed();
}

void XcodaUi::widgetDestroyed()
{
  _w = NULL;
  delete this;
}

void XcodaUi::installDestroyHandler()
{
  assert ( _w != NULL );
  XtAddCallback ( _w, 
		 XmNdestroyCallback,
		 (XtCallbackProc)&XcodaUi::widgetDestroyedCallback, 
		 (XtPointer) this );
}

void XcodaUi::manage()
{
  assert ( _w != NULL );
  assert ( XtHasCallbacks ( _w, XmNdestroyCallback ) ==
	  XtCallbackHasSome );
  XtManageChild ( _w );
}

XcodaUi::~XcodaUi()
{
#ifdef _TRACE_OBJECTS
  printf("Delete  XcodaUi Object \n");
#endif
  // Make sure the widget hasn't already been destroyed
    
  if ( _w ) 
    {
      // Remove destroy callback so Xt can't call the callback
      // with a pointer to an object that has already been freed
	
      XtRemoveCallback ( _w, 
			XmNdestroyCallback,
			&XcodaUi::widgetDestroyedCallback,
			(XtPointer) this );	
    }
}

void XcodaUi::getResources ( const XtResourceList resources, 
			    const int numResources )
{
    // Check for errors
    
  assert ( _w != NULL );
  assert ( resources != NULL );
    
  // Retrieve the requested resources relative to the 
  // parent of this object's base widget
    
  XtGetSubresources ( XtParent( _w ), 
		     (XtPointer) this, 
		     _name,
		     className(),
		     resources, 
		     numResources,
		     NULL, 
		     0 );
}

void XcodaUi::setDefaultResources ( const Widget w, 
				   const String *resourceSpec )
{
  int         i;	
  Display    *dpy = XtDisplay ( w );	  // Retrieve the display pointer
  XrmDatabase rdb = NULL;             // A resource data base

  // Create an empty resource database

  rdb = XrmGetStringDatabase ( "" );

  // Add the Component resources, prepending the name of the component

  i = 0;
  while ( resourceSpec[i] != NULL )
    {
      char buf[1000];
      
      sprintf(buf, "*%s%s", _name, resourceSpec[i++]);
      XrmPutLineResource( &rdb, buf );
    }

  // Merge them into the Xt database, with lowest precendence

  if ( rdb )
    {
#if (XlibSpecificationRelease>=5)
      XrmDatabase db = XtDatabase(dpy);
      XrmCombineDatabase(rdb, &db, FALSE);
#else
      XrmMergeDatabases ( dpy->db, &rdb );
      dpy->db = rdb;
#endif
    }
}


