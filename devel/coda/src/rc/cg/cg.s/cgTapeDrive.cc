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
//      Simple 2D graphical tape drive
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgTapeDrive.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:05  chen
//   CODA OO-graphics library for Xt
//
//
#include <cgColorCxt.h>
#include "cgTapeDrive.h"

const int CG_NUM_PIE_ARCS = 8;

cgTapeDrive::cgTapeDrive (double blx, double bly, double height)
:cgPrim (), x_ (blx), y_ (bly), width_ (2*height), height_ (height),
 move_ (0), color_ (0), fg_ (1)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create cgTapeDrive Class Object\n");
#endif
  createInternalPrims ();
}

cgTapeDrive::~cgTapeDrive (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete cgTapeDrive Class Object\n");
#endif
  delete frame_;
  delete conn_;
  for (int i = 0; i < CG_NUM_PIE_ARCS; i++) {
    delete left_[i];
    delete right_[i];
  }
  delete []left_;
  delete []right_;
}

void
cgTapeDrive::createInternalPrims (void)
{
  double wd = width_/4.0;

  frame_ = new cgRec (x_, y_, width_, height_);
  conn_ = new cgLine (x_ + wd, y_ + (height_ + wd)/2.0,
		      x_ + 3*wd, y_ + (height_ + wd)/2.0);
  // create left and right tape drive
  left_ = new cgPieArc*[CG_NUM_PIE_ARCS];
  right_ = new cgPieArc*[CG_NUM_PIE_ARCS];
  
  for (int i = 0; i < CG_NUM_PIE_ARCS; i++) {
    left_[i] = new cgPieArc (x_ + wd, y_ + height_/2.0, wd/2.0,
			     i*45.0, (i+1)*45.0);
    right_[i] = new cgPieArc (x_ + 3*wd, y_ + height_/2.0, wd/2.0,
			     i*45.0, (i+1)*45.0);
  }
}

cgPrim*
cgTapeDrive::copy (void) const
{
  cgTapeDrive* dr = new cgTapeDrive (x_, y_, height_);
  if (cxt_)
    dr->copyCgCxt (*cxt_);
  dr->setTapeDriveColor (color_);
  dr->setTapeDriveFgColor (fg_);
  return dr;
}

void
cgTapeDrive::draw (const cgScene* s) const
{
  int i = 0;
  assert (cxt_);

  if (cxt_->xdrawable () == 0)
    return;

  // get foreground and background color
  Pixel fg, bg;
  cxt_->getForeground (fg);
  cxt_->getBackground (bg);

  cxt_->setForeground (color_);
  frame_->solid (s);
  
  // reset foreground color
  cxt_->setForeground (fg_);
  frame_->draw (s);
  conn_->draw (s);

  for (i = 0; i < CG_NUM_PIE_ARCS; i++) {
    if (move_ == 0) {
      if (i%2 == 0) {
	left_[i]->solid (s);
	right_[i]->solid (s);
      }
    }
    else {
      if (i%2 != 0) {
	left_[i]->solid (s);
	right_[i]->solid (s);
      }
    }
    left_[i]->draw (s);
    right_[i]->draw (s);
  }
  // reset foreground color
  cxt_->setForeground (fg);
}

void
cgTapeDrive::erase (const cgScene* s) const
{
  assert (cxt_);

  if (cxt_->xdrawable () == 0)
    return;

  // get foreground and background color
  Pixel fg, bg;
  cxt_->getForeground (fg);
  cxt_->getBackground (bg);

  cgRec eraseRec (x_, y_, width_, height_);
  eraseRec.setCgCxt (*cxt_);
  cxt_->setForeground (bg);
  eraseRec.draw (s);
  eraseRec.solid (s);

  // reset foreground
  cxt_->setForeground (fg);
}

cgPrim&
cgTapeDrive::translate (const cgPoint& p)
{
  x_ += p.givx();
  y_ += p.givy();
  return *this;
}

void
cgTapeDrive::rotateTape (const cgScene* s)
{
  assert (cxt_);
  if (cxt_->xdrawable () != 0) {
    int i = 0;
    Pixel fg, bg;
    cxt_->getForeground (fg);
    cxt_->getBackground (bg);

    // erase old filled pie arc
    cxt_->setForeground (color_);
    for (i = 0; i < CG_NUM_PIE_ARCS; i++) {
      if (move_ == 0) {
	if (i%2 == 0) {
	  right_[i]->solid (s);
	  left_[i]->solid (s);
	}
      }
      else {
	if (i%2 != 0) {
	  right_[i]->solid (s);
	  left_[i]->solid (s);
	}
      }
    }
    if (move_ == 0)
      move_ = 1;
    else
      move_ = 0;
    // refill new pie arcs
    cxt_->setForeground (fg_);
    for (i = 0; i < CG_NUM_PIE_ARCS; i++) {
      if (move_ == 0) {
	if (i%2 == 0) {
	  right_[i]->solid (s);
	  left_[i]->solid (s);
	}
      }
      else {
	if (i%2 != 0) {
	  right_[i]->solid (s);
	  left_[i]->solid (s);
	}
      }
    }
    // reset foreground color
    cxt_->setForeground (fg);
  }
}  

void
cgTapeDrive::propagateCgCxt (void)
{
  frame_->setCgCxt (*cxt_);
  conn_->setCgCxt  (*cxt_);
  for (int i = 0; i < CG_NUM_PIE_ARCS; i++) {
    left_[i]->setCgCxt (*cxt_);
    right_[i]->setCgCxt (*cxt_);
  }
}

void
cgTapeDrive::createCgCxt (cgDispDev& disp)
{
  cgPrim::createCgCxt (disp);
  propagateCgCxt ();
  cxt_->getBackground (color_);
  cxt_->getForeground (fg_);
}

void
cgTapeDrive::copyCgCxt (cgContext& cxt)
{
  cgPrim::copyCgCxt (cxt);
  propagateCgCxt ();
  cxt_->getBackground (color_);
  cxt_->getForeground (fg_);
}

void
cgTapeDrive::setCgCxt (cgContext& cxt)
{
  cgPrim::setCgCxt (cxt);
  propagateCgCxt ();
  cxt_->getBackground (color_);
  cxt_->getForeground (fg_);
}

void
cgTapeDrive::setTapeDriveColor (Pixel color)
{
  color_ = color;
}

void
cgTapeDrive::setTapeDriveColor (char* name)
{
  assert (cxt_);
  cgColorCxt* cxt = cxt_->dispDev().colorCxt ();
  color_ = cxt->pixel (name);
}

void
cgTapeDrive::setTapeDriveFgColor (Pixel color)
{
  fg_ = color;
}

void
cgTapeDrive::setTapeDriveFgColor (char* name)
{
  assert (cxt_);
  cgColorCxt* cxt = cxt_->dispDev().colorCxt ();
  fg_ = cxt->pixel (name);
}

      
	
  
