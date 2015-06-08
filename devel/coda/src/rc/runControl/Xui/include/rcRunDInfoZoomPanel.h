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
//      RunControl Zoomed Dynamic Information Panel
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRunDInfoZoomPanel.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#ifndef _RC_RUNDINFO_ZOOM_PANEL_H
#define _RC_RUNDINFO_ZOOM_PANEL_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <cgDispDev.h>
#include <cgFrameRec.h>
#include <cgRotString.h>

class rcRunDInfoZoomPanel
{
public:
  // constructor and destrcutor
  rcRunDInfoZoomPanel (Widget parent, char* name,
		       float zoom,
		       Dimension width = 400,
		       Dimension height = 250);
  ~rcRunDInfoZoomPanel (void);

  // operation
  void unmanage (void);
  void manage   (void);

  int mapped (void) const;

  // update event number and event rate
  void updateEventNumber (char* event);
  void updateEventRate   (char* irate, char* drate);

  // return widget associated with this
  const Widget baseWidget (void);

protected:
  // dimension value
  static double xmin, xmax, ymin, ymax;

  // char font
  static char* textfont;

private:
  cgDispDev*    disp_;
  cgScene*      scene_;
  cgRotString*  evnum_;
  cgRotString*  evirate_;
  cgRotString*  evdrate_;
  // flag of mapped dialog
  int mapped_;
};
#endif
