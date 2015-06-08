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
//      RunControl 2D Data Display Strip Chart
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rc2DRtDisp.cc,v $
//   Revision 1.2  1998/04/08 18:31:09  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#include "rc2DRtDisp.h"

rc2DRtDisp::rc2DRtDisp (Widget parent, char* name, double xmin, double ymin,
			double ymax, int bufsize,int history,
			Dimension width, Dimension height)
:mapped_ (0)
{
  
  return;
#ifdef _TRACE_OBJECTS
  printf ("Create rc2DRtDisp Class Object\n");
#endif
  disp_ = new cgDispDev (parent, name, width, height, CG_SIMPLE_MODE);
  disp_->manage ();
  disp_->createColorCxt ();
  scene_ = new cgScene (xmin, xmin + bufsize,
			ymin - 2, ymax + 2,
			0.05, 0.95, 0.05, 0.95, *disp_);
  disp_->registerScene (scene_);
  // crate 2D plot
  plot_ = new cg2DRtPlot (xmin, ymin,
			  ymax - ymin, bufsize, history);
  plot_->createCgCxt (*disp_);
  plot_->foregroundColor ("red");
}

rc2DRtDisp::~rc2DRtDisp (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete rc2DRtDisp Class Object\n");
#endif
  // cgDispDev will be automatically destroyed by Xt Widget
  // Destroy Callback

  // cgDispDev also will delete every scene, so no need to delete scene_
  // if not registered one has to delete plot :note: important
  if (!mapped_)
    delete plot_;
}

const Widget
rc2DRtDisp::baseWidget (void)
{
  return disp_->baseWidget ();
}

void
rc2DRtDisp::manage (void)
{
  if (!mapped_) {
    plot_->attach (scene_);
    mapped_ = 1;
  }
}

void
rc2DRtDisp::unmanage (void)
{
  if (mapped_) {
    plot_->erase (scene_);
    plot_->detach (scene_);
    mapped_ = 0;
  }
}

void
rc2DRtDisp::addPoint (double value)
{
  if (mapped_)
    plot_->addPoint (value, scene_);
}

void
rc2DRtDisp::reset (void)
{
  if (mapped_) 
    plot_->reset (scene_);
}

