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
//      RunControl 2D Strip Chart Diaplay
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rc2DRtDisp.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#ifndef _RC_2DRT_DISP_H
#define _RC_2DRT_DISP_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <cgDispDev.h>
#include <cg2DRtPlot.h>

class rc2DRtDisp
{
public:
  // constructor and destructor
  rc2DRtDisp  (Widget parent, char* name, double xmin, double ymin,
	       double ymax, int bufsize, int history,
	       Dimension width = 100,
	       Dimension height = 100);
  ~rc2DRtDisp (void);

  // return widget associated with this
  const Widget baseWidget (void);

  // manage and unmanage
  void manage    (void);
  void unmanage  (void);

  // add one more point to the display
  void         addPoint (double value);
  // reset display
  void         reset    (void);

  // class name
  const char* className (void) const {return "rc2DRtDisp";}


private:
  cgDispDev*    disp_;
  cg2DRtPlot*   plot_;
  cgScene*      scene_;
  // flag of mapped plot or not
  int           mapped_;
};
#endif
