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
//      RunControl Load Database Dialog Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcLoadDialog.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#include <rcLoad.h>
#include "rcLoadDialog.h"

#if !defined (_CODA_2_0_T) && !defined (_CODA_2_0)
rcLoadDialog::rcLoadDialog (rcLoad* load, char* name, char* title)
:XcodaStringPromptDialog (load->dialogBaseWidget(), name, title), load_ (load)
{
#ifdef _TRACE_OBJECTS
  printf ("                        Create rcLoadDialog Class Object\n");
#endif
  // empty
}

rcLoadDialog::~rcLoadDialog (void)
{
#ifdef _TRACE_OBJECTS
  printf ("                        Delete rcLoadDialog Class Object\n");
#endif
  // empty
}

void
rcLoadDialog::popup (void)
{
  char *dbase = ::getenv ("RCDATABASE");
  if (dbase) 
    setText (dbase);
  XcodaStringPromptDialog::popup ();
}

void
rcLoadDialog::ok (void)
{
  load_->loadRcDbase (_result);
  popdown ();
}

void
rcLoadDialog::cancel (void)
{
  popdown ();
}
#endif


