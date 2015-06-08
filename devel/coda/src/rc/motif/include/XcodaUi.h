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
//	CODA Motif User Interface component Header 
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaUi.h,v $
//   Revision 1.2  1998/04/08 17:29:40  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:58  chen
//   coda motif C++ library
//
//
#ifndef _XCODAUI_H
#define _XCODAUI_H

#include <XcodaBasic.h>

class XcodaUi : public XcodaBasic
{
public:
  virtual ~XcodaUi();  // Destructor
    
  // Manage the entire widget subtree represented
  // by this component. Overrides XcodaBasic method
  virtual void manage();
    
  // Public access functions
  virtual const char *className() const { return "XcodaUi"; }
    
protected:
  // Protect constructor to prevent direct instantiation
  XcodaUi ( const char * );
    
  // Easy hook for derived classes
  void installDestroyHandler(); 
    
  // Called by widgetDestroyedCallback() if base widget is destroyed
  virtual void widgetDestroyed(); 
    
  // Loads component's default resources into database
  void setDefaultResources ( const Widget , const String *);
    
  // Retrieve resources for this clsss from the resource manager
  void getResources ( const XtResourceList, const int );

private:
// Interface between XmNdestroyCallback and this class
  static void widgetDestroyedCallback ( Widget, 
				       XtPointer, 
				       XtPointer );
};
#endif
