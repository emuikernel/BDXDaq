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
//	 CODA Motif Form Dialog Class Header
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaForm.h,v $
//   Revision 1.1  1998/04/08 18:40:45  heyes
//   add
//
//   Revision 1.1.1.1  1996/10/10 19:24:58  chen
//   coda motif C++ library
//
//
#ifndef _XCODA_FORM
#define _XCODA_FORM

#include <XcodaUi.h>

class XcodaForm: public XcodaUi
{
public:
  //destructor
  virtual ~XcodaForm (void);
  void    init             (void);
  void    popup            (void);
  void    popdown          (void);
  int     isMapped         (void);
  void    setModal         (void);
  void    unsetModal       (void);
  virtual const char *className(void) const {return "XcodaFormDialog";}

protected:
  // constructor
  XcodaForm(Widget parent, char *name, char *title);
  // Children who inherit from this must provide real routine here
  virtual void createFormChildren() = 0;
  // set default button
  virtual void defaultButton (Widget w);

private:
  Widget _parent;
  char   *_title;
  int    alreadyManaged;
  int    modalDialog;
};
#endif
