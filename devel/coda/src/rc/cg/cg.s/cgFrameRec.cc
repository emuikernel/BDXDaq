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
//      Implementation of 2D frame rectangle
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgFrameRec.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include <cgContext.h>
#include "cgFrameRec.h"

cgFrameRec::cgFrameRec (double blx, double bly,
			double width, double height,
			double frameWidth, int frameType)
: cgPrim (), bleft_ (blx, bly), width_ (width), height_ (height),
  frameWd_ (frameWidth), frameType_ (frameType), center_ ()
{
  center_.setx (blx + width_/2);
  center_.sety (bly + height_/2);

  createInternalPrims (blx, bly);
}

cgFrameRec::cgFrameRec (const cgPoint& bl, double width, double height,
			double frameWidth, int frameType)
:cgPrim (), bleft_ (bl), width_ (width), height_ (height),
 frameWd_ (frameWidth), frameType_ (frameType), center_ ()
{
  center_.setx (bleft_.givx () + width_/2);
  center_.sety (bleft_.givy () + height_/2);

  createInternalPrims (bleft_.givx (), bleft_.givy ());
}

void
cgFrameRec::createInternalPrims (double blx, double bly)
{
  irec_ = new cgRec (blx + frameWd_, bly + frameWd_,
		     width_ - 2*frameWd_, height_ - 2*frameWd_);
  // create all polygons
  cgPoint pl[4];
  // bottom polygon
  pl[0].setx (blx);
  pl[0].sety (bly);
  pl[1].setx (blx + frameWd_);
  pl[1].sety (bly + frameWd_);
  pl[2].setx (blx + width_ - frameWd_);
  pl[2].sety (bly + frameWd_);
  pl[3].setx (blx + width_);
  pl[3].sety (bly);
  bot_ = new cgPolygon (pl, 4);

  // right polygon
  pl[0].setx (blx + width_);
  pl[0].sety (bly);
  pl[1].setx (blx + width_ - frameWd_);
  pl[1].sety (bly + frameWd_);
  pl[2].setx (blx + width_ - frameWd_);
  pl[2].sety (bly + height_ - frameWd_);
  pl[3].setx (blx + width_);
  pl[3].sety (bly + height_);
  rite_ = new cgPolygon (pl, 4);

  // top polygon
  pl[0].setx (blx);
  pl[0].sety (bly + height_);
  pl[1].setx (blx + width_);
  pl[1].sety (bly + height_);
  pl[2].setx (blx + width_ - frameWd_);
  pl[2].sety (bly + height_ - frameWd_);
  pl[3].setx (blx + frameWd_);
  pl[3].sety (bly + height_ - frameWd_);
  top_ = new cgPolygon (pl, 4);

  // left polygon
  pl[0].setx (blx);
  pl[0].sety (bly);
  pl[1].setx (blx);
  pl[1].sety (bly + height_);
  pl[2].setx (blx + frameWd_);
  pl[2].sety (bly + height_ - frameWd_);
  pl[3].setx (blx + frameWd_);
  pl[3].sety (bly + frameWd_);
  left_ = new cgPolygon (pl, 4);
}

cgFrameRec::~cgFrameRec (void)
{
  delete irec_;
  delete top_;
  delete bot_;
  delete left_;
  delete rite_;
}

cgPrim*
cgFrameRec::copy (void) const
{
  cgFrameRec *newrec = new cgFrameRec (bleft_, width_, height_,
				       frameWd_, frameType_);
  if (cxt_)
    newrec->copyCgCxt (*cxt_);
  return newrec;
}

void
cgFrameRec::draw (const cgScene* s) const
{
  assert (cxt_);
  cxt_->setLineWidth (0);
  // no need to draw rectangle
  //  irec_->draw (s);
  
  // fill all polygons
  Pixel bg, fg;
  cxt_->getBackground (bg);
  cxt_->getForeground (fg);
  Pixel lite, dark;
  lite = cxt_->liteShade (bg);
  dark = cxt_->darkShade (bg);

  if (frameType_ == CG_SHADOW_IN) {
    cxt_->setForeground (dark);
    top_->solid (s);
    left_->solid (s);
    cxt_->setForeground (lite);
    bot_->solid (s);
    rite_->solid (s);
  }
  else {
    cxt_->setForeground (lite);
    top_->solid (s);
    left_->solid (s);
    cxt_->setForeground (dark);
    bot_->solid (s);
    rite_->solid (s);
  }
  cxt_->setForeground (fg);
}

void
cgFrameRec::erase (const cgScene* s) const
{
  cgRec eraseRec (bleft_, width_, height_);
  assert (cxt_);
  eraseRec.setCgCxt (*cxt_);
  // set cxt background and foreground with the smae color
  Pixel bg, fg;
  cxt_->getBackground (bg);
  cxt_->getForeground (fg);
  cxt_->setForeground (bg);
  eraseRec.solid (s);
  cxt_->setForeground (fg);
}

cgPrim&
cgFrameRec::rot (double deg)
{
  cgPoint cen (center_);
  translate (-cen);

  irec_->rot (deg);
  top_->rot (deg);
  bot_->rot (deg);
  left_->rot (deg);
  rite_->rot (deg);

  translate (cen);
  return *this;
}
  
cgPrim&
cgFrameRec::translate (const cgPoint& point)
{
  center_ += point;
  irec_->translate (point);
  left_->translate (point);
  rite_->translate (point);
  bot_->translate (point);
  top_->translate (point);
  return *this;
}

void
cgFrameRec::createCgCxt (cgDispDev& disp)
{
  cgPrim::createCgCxt (disp);
  if (irec_)
    irec_->setCgCxt (*cxt_);
  if (top_)
    top_->setCgCxt (*cxt_);
  if (bot_)
    bot_->setCgCxt (*cxt_);
  if (left_)
    left_->setCgCxt (*cxt_);
  if (rite_)
    rite_->setCgCxt (*cxt_);
}

void
cgFrameRec::copyCgCxt (cgContext& cxt)
{
  cgPrim::copyCgCxt (cxt);
  if (irec_)
    irec_->setCgCxt (*cxt_);
  if (top_)
    top_->setCgCxt (*cxt_);
  if (bot_)
    bot_->setCgCxt (*cxt_);
  if (left_)
    left_->setCgCxt (*cxt_);
  if (rite_)
    rite_->setCgCxt (*cxt_);
}

void
cgFrameRec::setCgCxt (cgContext& cxt)
{
  cgPrim::setCgCxt (cxt);
  if (irec_)
    irec_->setCgCxt (*cxt_);
  if (top_)
    top_->setCgCxt (*cxt_);
  if (bot_)
    bot_->setCgCxt (*cxt_);
  if (left_)
    left_->setCgCxt (*cxt_);
  if (rite_)
    rite_->setCgCxt (*cxt_);
}

int
cgFrameRec::contains (const cgPoint& p)
{
  if (p.givx() >= bleft_.givx() && p.givx() <= bleft_.givx() + width_ &&
      p.givy() >= bleft_.givy() && p.givy() <= bleft_.givy() + height_)
    return 1;
  else
    return 0;
}

void
cgFrameRec::select (const cgScene* s)
{
  static int selected = 0;

  if (!selected) {
    selected = 1;
  }
  else {
    selected = 0;
  }
}

