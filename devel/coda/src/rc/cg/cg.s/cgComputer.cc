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
//      A Simple 2D graphical Representation of a Computer
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgComputer.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include <cgColorCxt.h>
#include "cgComputer.h"

const int CG_NUM_AIR_DUCTS = 5;

cgComputer::cgComputer (double blx, double bly, double width)
:cgPrim (), x_ (blx), y_ (bly), width_ (width), height_ (width), color_ (0),
 fg_ (1)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create cgComputer Class Object\n");
#endif
  createInternalPrims ();
}

cgComputer::~cgComputer (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete cgComputer Class Object\n");
#endif
  delete monitor_;
  delete screen_;
  delete adjuster_;
  delete supporter_;
  delete console_;
  delete discDr_;
  for (int i = 0; i < CG_NUM_AIR_DUCTS; i++)
    delete ducts_[i];
  delete []ducts_;
}

void
cgComputer::createInternalPrims (void)
{
  // monitor width is smaller than width of console by '2*wd'
  double wd = width_/20.0;
  // divide total height into 6 pieces
  double ht = height_/6.0;
  
  // create monitor
  monitor_ = new cgRec (x_ + wd, y_ + 2*ht, width_ - 2*wd, 4*ht);
  // create screen
  screen_ = new cgRec (x_ + 2*wd, y_ + 2*ht + wd, width_ - 4*wd,
		       4*ht - 2*wd);
  // create adjuster
  adjuster_ = new cgRec (x_ + 5*wd, y_ + 2*ht - ht/2.0,width_ - 10*wd,
			 ht/2.0);
  // create supporter
  supporter_ = new cgRec (x_ + 4*wd, y_ + ht, width_ - 8*wd, ht/2.0);

  // create console
  console_ = new cgRec (x_, y_, width_, ht);
  
  // create floppy drive
  discDr_ = new cgLine (x_ + width_ - 6*wd, y_ + ht/2.0,
			x_ + width_ - 2*wd, y_ + ht/2.0);
  // create air ducts
  ducts_ = new cgLine*[CG_NUM_AIR_DUCTS];
  for (int i = 0; i < CG_NUM_AIR_DUCTS; i++)
    ducts_[i] = new cgLine (x_ + wd*(i+1), y_ + ht/4.0,
			 x_ + wd*(i+1), y_ + ht*3/4.0);
}

cgPrim*
cgComputer::copy (void) const
{
  cgComputer* comp = new cgComputer (x_, y_, width_);
  if (cxt_)
    comp->copyCgCxt (*cxt_);
  comp->setComputerColor (color_);
  comp->setComputerFgColor (fg_);
  return comp;
}

void
cgComputer::draw (const cgScene* s) const
{
  assert (cxt_);

  if (cxt_->xdrawable () == 0)
    return;

  // get foreground and background color
  Pixel fg, bg;
  cxt_->getForeground (fg);
  cxt_->getBackground (bg);

  cxt_->setForeground (color_);
  monitor_->solid (s);
  console_->solid (s);
  supporter_->solid (s);
  adjuster_->solid (s);
  
  // set screen color to background
  screen_->solid (s);

  // reset foreground
  cxt_->setForeground (fg_);
  monitor_->draw (s);
  screen_->draw (s);
  adjuster_->draw (s);
  supporter_->draw (s);
  discDr_->draw (s);
  for (int i = 0; i < CG_NUM_AIR_DUCTS; i++)
    ducts_[i]->draw (s);
  // reset foreground
  cxt_->setForeground (fg);
}

void
cgComputer::erase (const cgScene* s) const
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
cgComputer::translate (const cgPoint& p)
{
  x_ += p.givx();
  y_ += p.givy();
  return *this;
}

void
cgComputer::propagateCgCxt (void)
{
  monitor_->setCgCxt (*cxt_);
  screen_->setCgCxt (*cxt_);
  adjuster_->setCgCxt (*cxt_);
  supporter_->setCgCxt (*cxt_);
  console_->setCgCxt (*cxt_);
  discDr_->setCgCxt (*cxt_);
  for (int i = 0; i < CG_NUM_AIR_DUCTS; i++)
    ducts_[i]->setCgCxt (*cxt_);
}

void
cgComputer::createCgCxt (cgDispDev& disp)
{
  cgPrim::createCgCxt (disp);
  propagateCgCxt ();
  cxt_->getBackground (color_);
  cxt_->getForeground (fg_);
}

void
cgComputer::copyCgCxt (cgContext& cxt)
{
  cgPrim::copyCgCxt (cxt);
  propagateCgCxt ();
  cxt_->getBackground (color_);
  cxt_->getForeground (fg_);
}

void
cgComputer::setCgCxt (cgContext& cxt)
{
  cgPrim::setCgCxt (cxt);
  propagateCgCxt ();
  cxt_->getBackground (color_);
  cxt_->getForeground (fg_);
}

void
cgComputer::setComputerColor (Pixel color)
{
  color_ = color;
}

void
cgComputer::setComputerColor (char* name)
{
  assert (cxt_);
  cgColorCxt* cxt = cxt_->dispDev().colorCxt ();
  color_ = cxt->pixel (name);
}


void
cgComputer::setComputerFgColor (Pixel color)
{
  fg_ = color;
}

void
cgComputer::setComputerFgColor (char* name)
{
  assert (cxt_);
  cgColorCxt* cxt = cxt_->dispDev().colorCxt ();
  fg_ = cxt->pixel (name);
}
