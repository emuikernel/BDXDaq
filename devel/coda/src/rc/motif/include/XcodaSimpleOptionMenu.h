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
//	Interface for simple Option Menu with push buttons
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaSimpleOptionMenu.h,v $
//   Revision 1.2  1998/04/08 17:29:34  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:25:01  chen
//   coda motif C++ library
//
//
#ifndef _CODA_OPTION_MENU_H
#define _CODA_OPTION_MENU_H

#include <XcodaUi.h>

class XcodaSimpleOptionMenu: public XcodaUi
{
public:
  // constructor and destructor
  XcodaSimpleOptionMenu (Widget parent, char* name, char* title, 
		   int maxentries = 50);
  virtual ~XcodaSimpleOptionMenu (void);

  // initialize all widgets
  virtual void   init (void);
  // add list of entries
  virtual void   addEntries (char **names, int num,
			     int selection = 0);
  // add lisrt of entries with active/not active information
  // active[] = 1: active, 0: inactive
  // put item selection to the current selection
  virtual void   addEntries (char **names, int* active, 
			     int num, int selection = 0);
  // remove all entries
  virtual void   removeAll  (void);

  // class name
  virtual const char *className (void) const {return "XcodaSimpleOptionMenu";}

protected:
  // real action after a selection, subclass should implement this
  virtual void  doit (void);
  // real action after menu entries have been changed
  // subclass provide implementation
  virtual void  entriesChangedAction (void);

  // current selection
  int           currentSel_;
  // previous selection
  int           prevSel_;
  // All unmanaged widgets
  Widget*       widgets_;
  int           numWidgets_;

  // callback for all pushbuttons
  static void comdCallback (Widget w, XtPointer clientData, 
			    XmAnyCallbackStruct *);

private:
  Widget        parent_;
  Widget        menu_;
  char*         title_;
  // flag to check whether one has all pushbutton mapped
  int           pbmapped_;
};
#endif
  
