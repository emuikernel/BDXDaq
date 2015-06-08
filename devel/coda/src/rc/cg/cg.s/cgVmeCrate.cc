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
//      Simple VME Crate
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgVmeCrate.cc,v $
//   Revision 1.3  1999/12/01 16:41:26  rwm
//   Nix compiler warnings via casts.
//
//   Revision 1.2  1997/08/25 16:01:52  heyes
//   fix some display problems
//
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include <cgPoint.h>
#include <cgColorCxt.h>
#include "cgVmeCrate.h"

cgVmeCrate::cgVmeCrate (double blx, double bly, 
			double height,
			int numSlots)
:cgPrim (), blx_ (blx), bly_ (bly), 
 width_ (2*height), height_ (height), numSlots_ (numSlots), crateColor_ (0),
 crateFg_ (1)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create cgVmeCrate Class Object\n");
#endif
  createInternalPrims ();
}

cgVmeCrate::~cgVmeCrate (void)
{
  int i;
#ifdef _TRACE_OBJECTS
  printf ("    Create cgVmeCrate Class Object\n");
#endif
  delete frame_;
  delete fan_;
  delete switch_;
  for (i = 0; i < numSlots_ - 1; i++)
    delete lines_[i];
  delete []lines_;

  for (i = 0; i < numLights_; i++)
    delete lights_[i];
  delete []lights_;
}

void
cgVmeCrate::createInternalPrims (void)
{
  int i;
  frame_ = new cgRec (blx_, bly_,  width_,  height_);
  fan_ = new cgRec (blx_, bly_, width_, height_/5.0);
  
  // create all lines which simulate all the slots
  double slotwd = (width_)/numSlots_;

  lines_ = new cgLine*[numSlots_ - 1];
  for (i = 0; i < numSlots_ - 1; i++) {
    lines_[i] = new cgLine (blx_ + slotwd*(i + 1), bly_ + height_/5.0,
			    blx_ + slotwd*(i + 1), bly_ + height_);
  }

  // create switch
  switch_ = new cgRec (blx_ + slotwd, bly_ + height_/10.0 - slotwd/2.0,
		       slotwd, slotwd);

  // create led lights
  double ld = slotwd/2.0;
  numLights_ = (int) ( (4*height_/5.0) / (2*ld) );
  
  lights_ = new cgCircle*[numLights_];
  cgPoint cen;
  for (i = 0; i < numLights_; i++) {
    cen.setx (blx_ + width_ - slotwd/2.0);
    cen.sety (bly_ + height_/5.0 + ld*(2*i+1));
    lights_[i] = new cgCircle (cen, ld/2.0);
  }
}

cgPrim*
cgVmeCrate::copy (void) const
{
  cgVmeCrate *newcr = new cgVmeCrate (blx_, bly_, height_,
				      numSlots_);
  if (cxt_)
    newcr->copyCgCxt (*cxt_);
  newcr->setCrateColor (crateColor_);
  newcr->setCrateFgColor (crateFg_);
  return newcr;
}

void
cgVmeCrate::draw (const cgScene* s) const
{
  int i;
  assert (cxt_);
  // get foreground and background color
  Pixel fg, bg;
  cxt_->getForeground (fg);
  cxt_->getBackground (bg);


  cxt_->setForeground (crateColor_);
  frame_->solid (s);

  cxt_->setForeground (crateFg_);
  frame_->draw (s);

  for (i = 0; i < numSlots_ - 1; i++) 
    lines_[i]->draw (s);
  
  // draw fan part
  fan_->draw (s);
  switch_->solid (s);

  // always set color of lights to red
  cxt_->setForeground ("red");
  for (i = 0; i < numLights_; i++)
    lights_[i]->solid (s);
  
  // reset foreground
  cxt_->setForeground (fg);
}

void
cgVmeCrate::erase (const cgScene* s) const
{
  assert (cxt_);
  // get foreground and background color
  Pixel fg, bg;
  cxt_->getForeground (fg);
  cxt_->getBackground (bg);

  cgRec eraseRec (blx_, bly_, width_, height_);
  eraseRec.setCgCxt (*cxt_);
  cxt_->setForeground (bg);
  eraseRec.draw (s);
  eraseRec.solid (s);

  // reset foreground
  cxt_->setForeground (fg);
}

cgPrim&
cgVmeCrate::translate (const cgPoint& p)
{
  blx_ += p.givx();
  bly_ += p.givy();
  return *this;
}

void
cgVmeCrate::propagateCgCxt (void)
{
  int i;
  frame_->setCgCxt (*cxt_);
  for (i = 0; i < numSlots_ - 1; i++)
    lines_[i]->setCgCxt (*cxt_);
  for (i = 0; i < numLights_; i++)
    lights_[i]->setCgCxt (*cxt_);
  fan_->setCgCxt (*cxt_);
  switch_->setCgCxt (*cxt_);
}

void
cgVmeCrate::createCgCxt (cgDispDev& disp)
{
  cgPrim::createCgCxt (disp);
  propagateCgCxt ();
  cxt_->getBackground (crateColor_);
  cxt_->getForeground (crateFg_);
}

void
cgVmeCrate::copyCgCxt (cgContext& cxt)
{
  cgPrim::copyCgCxt (cxt);
  propagateCgCxt ();
  cxt_->getBackground (crateColor_);
  cxt_->getForeground (crateFg_);
}

void
cgVmeCrate::setCgCxt (cgContext& cxt)
{
  cgPrim::setCgCxt (cxt);
  propagateCgCxt ();
  cxt_->getBackground (crateColor_);
  cxt_->getForeground (crateFg_);
}

void
cgVmeCrate::setCrateColor (Pixel color)
{
  crateColor_ = color;
}

void
cgVmeCrate::setCrateColor (char* name)
{
  assert (cxt_);
  cgColorCxt* cxt = cxt_->dispDev().colorCxt ();
  crateColor_ = cxt->pixel (name);
}

void
cgVmeCrate::setCrateFgColor (Pixel color)
{
  crateFg_ = color;
}

void
cgVmeCrate::setCrateFgColor (char* name)
{
  assert (cxt_);
  cgColorCxt* cxt = cxt_->dispDev().colorCxt ();
  crateFg_ = cxt->pixel (name);
}
