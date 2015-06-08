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
//   $Log: cgTrashCan.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:05  chen
//   CODA OO-graphics library for Xt
//
//
#include <cgColorCxt.h>
#include "cgTrashCan.h"

const int CG_NUM_CAN_SHADES = 4;

cgTrashCan::cgTrashCan (double blx, double bly, double height)
:cgPrim (), x_ (blx), y_ (bly), width_ (3*height/4.0), height_ (height),
 color_ (0), fg_ (1), closed_ (1)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create cgTrashCan Class Object\n");
#endif
  createInternalPrims ();
}

cgTrashCan::~cgTrashCan (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete cgTrashCan Class Object\n");
#endif
  delete body_;
  delete cover_;
  delete tiltedCover_;
  delete handle_;
  delete tiltedHandle_;
  for (int i = 0; i < CG_NUM_CAN_SHADES; i++) {
    delete shades_[i];
  }

  delete []shades_;
}

void
cgTrashCan::createInternalPrims (void)
{
  // reduce width a little
  double realWidth = width_ - width_/10.0;

  double wd = realWidth/10.0;
  double canht = height_ - realWidth*0.30;

  body_ = new cgRec (x_ + wd, y_, realWidth - 2*wd, canht);
  // draw all lines
  shades_ = new cgLine*[CG_NUM_CAN_SHADES];
  double space = (realWidth - 2*wd)/(CG_NUM_CAN_SHADES + 1);
  for (int i = 0; i < CG_NUM_CAN_SHADES; i++) {
    shades_[i] = new cgLine (x_ + wd + space*(i+1), y_ + height_/10.0,
			     x_ + wd + space*(i+1), 
			     y_ + canht - height_/10.0);
  }

  // create cover
  cgPoint p[4];
  p[0].setx (x_);
  p[0].sety (y_ + canht);
  p[1].setx (x_);
  p[1].sety (y_ + canht + height_/10.0);
  p[2].setx (x_ + realWidth);
  p[2].sety (y_ + canht + height_/10.0);
  p[3].setx (x_ + realWidth);
  p[3].sety (y_ + canht);
  cover_ = new cgPolygon (p, 4);

  // create handle
  p[0].setx (x_ + realWidth/2.0 - realWidth/5.0);
  p[0].sety (y_ + canht + height_/10.0);
  p[1].setx (x_ + realWidth/2.0 - realWidth/5.0);
  p[1].sety (y_ + canht + 3*height_/20.0);
  p[2].setx (x_ + realWidth/2.0 + realWidth/5.0);
  p[2].sety (y_ + canht + 3*height_/20.0);
  p[3].setx (x_ + realWidth/2.0 + realWidth/5.0);
  p[3].sety (y_ + canht + height_/10.0); 
  handle_ = new cgPolygon (p, 4);

  // create tiltled cover
  tiltedCover_ = (cgPolygon *)cover_->copy ();
  tiltedCover_->rot (-10.0);


  tiltedHandle_ = (cgPolygon *)handle_->copy ();
  tiltedHandle_->rot (-10.0);
}

cgPrim*
cgTrashCan::copy (void) const
{
  cgTrashCan* can = new cgTrashCan (x_, y_, height_);
  if (cxt_)
    can->copyCgCxt (*cxt_);
  can->setCanColor (color_);
  can->setCanFgColor (fg_);
  return can;
}

void
cgTrashCan::draw (const cgScene* s) const
{
  int i = 0;
  assert (cxt_);

  if (cxt_->xdrawable () == 0)
    return;

  // get foreground and background color
  Pixel fg, bg;
  cxt_->getForeground (fg);
  cxt_->getBackground (bg);

  // erase previous drawing
  if (closed_) { 
      cxt_->setForeground (bg);
      tiltedCover_->solid (s);
      cxt_->setForeground (fg);
      tiltedCover_->erase (s);
      tiltedHandle_->erase (s);
    }
  else {
    cxt_->setForeground (bg);
    cover_->solid (s);
    cxt_->setForeground (fg);
    cover_->erase (s);
    handle_->erase (s);
  }

  cxt_->setForeground (color_);
  body_->solid (s);

  // reset foreground color
  cxt_->setForeground (fg_);
  body_->draw (s);
  for (i = 0; i < CG_NUM_CAN_SHADES; i++)
    shades_[i]->draw (s);

  if (closed_) {
    cxt_->setForeground (color_);
    cover_->solid (s);
    cxt_->setForeground (fg_);
    cover_->draw (s);
    handle_->draw (s);
  }
  else {
    cxt_->setForeground (color_);
    tiltedCover_->solid (s);
    cxt_->setForeground (fg_);
    tiltedCover_->draw (s);
    tiltedHandle_->draw (s);
  }
  cxt_->setForeground (fg);
}

void
cgTrashCan::erase (const cgScene* s) const
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
cgTrashCan::translate (const cgPoint& p)
{
  x_ += p.givx();
  y_ += p.givy();
  return *this;
}

void
cgTrashCan::propagateCgCxt (void)
{
  body_->setCgCxt (*cxt_);
  cover_->setCgCxt  (*cxt_);
  tiltedCover_->setCgCxt  (*cxt_);
  for (int i = 0; i < CG_NUM_CAN_SHADES; i++) 
    shades_[i]->setCgCxt (*cxt_);
  handle_->setCgCxt (*cxt_);
  tiltedHandle_->setCgCxt (*cxt_);
}

void
cgTrashCan::createCgCxt (cgDispDev& disp)
{
  cgPrim::createCgCxt (disp);
  propagateCgCxt ();
  cxt_->getBackground (color_);
  cxt_->getForeground (fg_);
}

void
cgTrashCan::copyCgCxt (cgContext& cxt)
{
  cgPrim::copyCgCxt (cxt);
  propagateCgCxt ();
  cxt_->getBackground (color_);
  cxt_->getForeground (fg_);
}

void
cgTrashCan::setCgCxt (cgContext& cxt)
{
  cgPrim::setCgCxt (cxt);
  propagateCgCxt ();
  cxt_->getBackground (color_);
  cxt_->getForeground (fg_);
}

void
cgTrashCan::setCanColor (Pixel color)
{
  color_ = color;
}

void
cgTrashCan::setCanColor (char* name)
{
  assert (cxt_);
  cgColorCxt* cxt = cxt_->dispDev().colorCxt ();
  color_ = cxt->pixel (name);
}

void
cgTrashCan::setCanFgColor (Pixel color)
{
  fg_ = color;
}

void
cgTrashCan::setCanFgColor (char* name)
{
  assert (cxt_);
  cgColorCxt* cxt = cxt_->dispDev().colorCxt ();
  fg_ = cxt->pixel (name);
}

void
cgTrashCan::open (const cgScene* s)
{
  closed_ = 0;
  if (cxt_->xdrawable () != 0)
    draw (s);
}

void
cgTrashCan::close (const cgScene* s)
{
  closed_ = 1;
  if (cxt_->xdrawable () != 0)
    draw (s);
}
