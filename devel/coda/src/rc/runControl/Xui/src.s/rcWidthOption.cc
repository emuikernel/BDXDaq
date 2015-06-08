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
//      Implementation of audio option toggle button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcWidthOption.cc,v $
//   Revision 1.4  1998/05/28 17:47:08  heyes
//   new GUI look
//
//   Revision 1.3  1998/04/08 18:31:34  heyes
//   new look and feel GUI
//
//   Revision 1.2  1997/07/30 14:32:54  heyes
//   add more xpm support
//
//   Revision 1.1  1997/07/22 19:39:05  heyes
//   cleaned up lots of things
//
//   Revision 1.2  1996/12/04 18:32:24  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
#include <rcComdOption.h>
#include "rcWidthOption.h"

rcWidthOption::rcWidthOption (char* name, int active,
			      char* acc, char* acc_text,
			      int state,
			      rcClientHandler& handler)
:rcMenuTogComd (name, active, acc, acc_text, state, handler)
{
#ifdef _TRACE_OBJECTS
  printf ("               Create rcWidthOption class object\n");
#endif
}

rcWidthOption::~rcWidthOption (void)
{
#ifdef _TRACE_OBJECTS
  printf ("               Delete rcWidthOption class object\n");
#endif
  // mshWin_ is just a pointer
}

void
rcWidthOption::doit (void)
{
  int x,y;
  unsigned int w,h,bw,dp,ac;
  Arg arg[20];
  Window root;
  extern Widget toplevel;

  char tmp[200];

  XGetGeometry(XtDisplay(toplevel),XtWindow(toplevel),&root,&x,&y,&w,&h,&bw,&dp);

  if (w != 490) {
    XResizeWindow(XtDisplay(toplevel),XtWindow(toplevel),
		  490,h);
  } else {
    XResizeWindow(XtDisplay(toplevel),XtWindow(toplevel),
		  DisplayWidth(XtDisplay(toplevel),0),h);
  }
}

void
rcWidthOption::undoit (void)
{
  // empty
}
#endif
