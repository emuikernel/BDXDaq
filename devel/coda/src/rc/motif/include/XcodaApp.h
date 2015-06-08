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
//	CODA Motif Application Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaApp.h,v $
//   Revision 1.3  1998/04/08 17:28:49  heyes
//   get in there
//
//   Revision 1.2  1997/06/06 19:00:39  heyes
//   new RC
//
//   Revision 1.1.1.1  1996/10/10 19:24:57  chen
//   coda motif C++ library
//
//
#ifndef _XCODAAPP_H
#define _XCODAAPP_H

#include <XcodaUi.h>

class XcodaTopLevel;

class XcodaApp : public XcodaUi {
public:
  // constructors 
  XcodaApp (char *name );
  XcodaApp (char *name, char **fallback_resources);
 
  // Function to clean up all X widgets and their C++ counterpart
  // Any X program which uses XcodaApp should call this before exit
  void         cleanMemory (void);
  // Open connection to X display, return 1 success, return 0 failure
  int          open        (int*, char **);
  // Functions to handle Xt interface
#if (XlibSpecificationRelease>=5)   
  virtual void initialize ( int *, char ** );  
#else
  virtual void initialize ( unsigned int *, char ** );  
#endif
  virtual void execute (void);
  virtual void handleone (void);
    
  // Functions to manipulate application's top-level windows
  void manage   (void);
  void unmanage (void);
  void iconify  (void);

  // Functions to register toplevel shell
  void registerWindow   ( XcodaTopLevel * );
  void unregisterWindow ( XcodaTopLevel * );
  int  numTopWindows()  {return _numWindows;}
    
  // Convenient access functions
  Display      *display()     { return _display; }
  XtAppContext  appContext()  { return _appContext; }
  const char   *applicationClass()  { return _applicationClass; }
    
  virtual const char *className() const { return "XcodaApp"; }

protected:
  // hide destructor and force programmer to call cleanMemory
  virtual ~XcodaApp (void);     

  // Support commonly needed data structures as a convenience
  Display     *_display;
  XtAppContext _appContext;
    
  char   *_applicationClass;      // Class name of this application
  XcodaTopLevel **_windows;       // top-level windows in the program
  int            _numWindows;
  int            _listLocked;     // window list lock

  // flag to tell whether a X connection is established
  int            _opened;

 private:
  // exit timer callback function
  static void exitTimer(XtPointer, XtIntervalId *);
  // remove all windows
  void removeAllWindows ( void );
  // fallback resources
  char **_fallback_resources;
  // friend class
  friend class XcodaTopLevel;
};

// Pointer to single global instance
extern XcodaApp *theApplication; 
#endif
