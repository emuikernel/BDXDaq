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
//	Command X-window Interface header file
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaComdXInterface.h,v $
//   Revision 1.3  1998/04/08 17:29:52  heyes
//   get in there
//
//   Revision 1.2  1997/06/06 19:00:44  heyes
//   new RC
//
//   Revision 1.1.1.1  1996/10/10 19:25:00  chen
//   coda motif C++ library
//
//
#ifndef _CODA_COMD_X_INTERFACE
#define _CODA_COMD_X_INTERFACE
#include <XcodaUi.h>

class codaComd;

class codaComdXInterface : public XcodaUi {

 public:    
  virtual ~codaComdXInterface();
  virtual void activate();
  virtual void deactivate();
  virtual void init () = 0;
  virtual const char *className() const;
  Widget  realBaseWidget();
 protected:    

  codaComd   *_cmd;
  Widget     _parent;
  static void executeComdCallback ( Widget, 
				   XtPointer, 
				   XtPointer );
  int _active;
  // children may be diffrent class of widget
  codaComdXInterface ( Widget, codaComd * );
  // redefine widgetDestroyed function
  virtual void widgetDestroyed();    
};
#endif

