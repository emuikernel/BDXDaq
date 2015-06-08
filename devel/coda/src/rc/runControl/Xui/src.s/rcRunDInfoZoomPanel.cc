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
//      RunControl Dynamic Information Zoomed Panel
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRunDInfoZoomPanel.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#include "rcRunDInfoZoomPanel.h"

double rcRunDInfoZoomPanel::xmin = 0.0;
double rcRunDInfoZoomPanel::xmax = 100.0;
double rcRunDInfoZoomPanel::ymin = 0.0;
double rcRunDInfoZoomPanel::ymax = 100.0;
char*  rcRunDInfoZoomPanel::textfont = "-*-helvetica-medium-r-*-*-20-*-*-*-*-*-*-*";

static void dismissDisplay (cgDispDev* disp, void *arg)
{
  rcRunDInfoZoomPanel* panel = (rcRunDInfoZoomPanel *)arg;
  panel->unmanage ();
}


rcRunDInfoZoomPanel::rcRunDInfoZoomPanel (Widget parent,
					  char* name,
					  float   zoom,
					  Dimension width,
					  Dimension height)
{
#ifdef _TRACE_OBJECTS
  printf ("Create rcRunDInfoZoomPanel Class Object\n");
#endif
  // create display device
  disp_ = new cgDispDev (parent, name, width, height,
			 CG_POPUP_MODE);
  disp_->manage ();

  // create graphical context
  disp_->createColorCxt ();

  // create new graphical scene
  scene_ = new cgScene (rcRunDInfoZoomPanel::xmin,
			rcRunDInfoZoomPanel::xmax,
			rcRunDInfoZoomPanel::ymin,
			rcRunDInfoZoomPanel::ymax,
			0.1, 0.9, 0.1, 0.9, *disp_);
  disp_->registerScene (scene_);

  // create all components

  // create event number display
  cgFrameRec*  envframe = new cgFrameRec  (0.0, 66.0, 100.0, 33.0,
					   2.0);
  evnum_ = new cgRotString ("     0      ",
			    rcRunDInfoZoomPanel::textfont,
			    5.0, 70.0,
			    0.0, zoom);

  envframe->createCgCxt (*disp_);
  evnum_->createCgCxt (*disp_);
  envframe->attach (scene_);
  evnum_->attach (scene_);

  // create event integrated display
  cgFrameRec* evirateframe = new cgFrameRec (0.0, 33.0, 80.0, 33.0,
					     2.0);

  evirate_ = new cgRotString ("    0.0    ",
			      rcRunDInfoZoomPanel::textfont,
			      5.0, 36.0,
			      0.0, zoom);
  cgRotString* hzl0 = new cgRotString ("Hz",
				       rcRunDInfoZoomPanel::textfont,
				       81.0, 36.0,
				       0.0, zoom);      
  evirateframe->createCgCxt (*disp_);
  evirate_->createCgCxt (*disp_);
  hzl0->createCgCxt (*disp_);
  evirateframe->attach (scene_);
  evirate_->attach (scene_);
  hzl0->attach (scene_);

  // create event differential display
  cgFrameRec* evdrateframe = new cgFrameRec (0.0, 1.0, 80.0, 33.0,
					     2.0);
  evdrate_ = new cgRotString ("    0.0    ",
			      rcRunDInfoZoomPanel::textfont,
			      5.0, 3.0,
			      0.0, zoom);
  cgRotString* hzl1 = new cgRotString ("Hz",
				       rcRunDInfoZoomPanel::textfont,
				       81.0, 3.0,
				       0.0, zoom);      

  evdrateframe->createCgCxt (*disp_);
  evdrate_->createCgCxt (*disp_);
  hzl1->createCgCxt (*disp_);
  evdrateframe->attach (scene_);
  evdrate_->attach (scene_);
  hzl1->attach (scene_);

  // register button3 action which is dismiss
  disp_->btn3action (dismissDisplay, (void *)this);
  mapped_ = 1;
}

rcRunDInfoZoomPanel::~rcRunDInfoZoomPanel (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete rcRunDInfoZoomPanel Class Object\n");
#endif
  // Xt Destroy Mechnisam will destroy cgDispDev and etc..
  // everything will be destroyed by disp
}

const Widget
rcRunDInfoZoomPanel::baseWidget (void)
{
  return disp_->baseWidget ();
}

void
rcRunDInfoZoomPanel::unmanage (void)
{
  disp_->unmanage ();
  mapped_ = 0;
}

void
rcRunDInfoZoomPanel::manage (void)
{
  disp_->manage ();
  mapped_ = 1;
}

int
rcRunDInfoZoomPanel::mapped (void) const
{
  return mapped_;
}

void
rcRunDInfoZoomPanel::updateEventNumber (char* event)
{
  evnum_->string (scene_, event);
}

void
rcRunDInfoZoomPanel::updateEventRate (char* irate, char* drate)
{
  evirate_->string (scene_, irate);
  evdrate_->string (scene_, drate);
}



