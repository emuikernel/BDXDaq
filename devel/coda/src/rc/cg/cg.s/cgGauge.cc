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
//      Implementation of cgGauge Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgGauge.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include "cgGauge.h"

cgGauge::cgGauge (double xmin, double xmax, double ymin, double ymax,
		  int showBorder)
:cgPrim (), xmin_ (xmin), xmax_ (xmax), 
 ymin_ (ymin), ymax_ (ymax), value_ (xmin),
 mapped_ (1), showBorder_ (showBorder)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create cgGaugeClass Object\n");
#endif
  createInternalPrims ();
}

cgGauge::~cgGauge (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete cgGaugeClass Object\n");
#endif
  delete holder_;
  delete mover_;
}

void
cgGauge::createInternalPrims (void)
{
  holder_ = new cgRec (xmin_, ymin_, (xmax_ - xmin_), (ymax_ - ymin_));
  mover_ = new cgRec (value_, ymin_, (xmax_ - xmin_)/10.0, (ymax_ - ymin_));
}

cgPrim*
cgGauge::copy (void) const
{
  cgGauge* gauge = new cgGauge (xmin_, xmax_, ymin_, ymax_);
  if (cxt_)
    gauge->copyCgCxt (*cxt_);
  return gauge;
}

void
cgGauge::draw (const cgScene* s) const
{
  assert (cxt_);

  if (cxt_->xdrawable () == 0)
    return;

  if (showBorder_)
    holder_->draw (s);

  if (mapped_) {
    mover_->draw (s);
    mover_->solid (s);
  }
}

void
cgGauge::erase (const cgScene* s) const
{
  if (cxt_->xdrawable () == 0)
    return;

  if (showBorder_)
    holder_->erase (s);

  mover_->erase (s);

  Pixel fg, bg;
  cxt_->getBackground (bg);
  cxt_->getForeground (fg);
  cxt_->setForeground (bg);
  mover_->solid (s);
  cxt_->setForeground (fg);  
}

void
cgGauge::setValue (const cgScene* s, double value)
{
  double mvWd = (xmax_ - xmin_)/10.0;
  if (value > xmax_ - mvWd)
    value_ = xmax_- mvWd;
  else if (value < xmin_)
    value_ = xmin_;
  else
    value_ = value;
  // erase old mover rectangle
  mover_->erase (s);
  Pixel bg, fg;
  cxt_->getBackground (bg);
  cxt_->getForeground (fg);
  cxt_->setForeground (bg);
  mover_->solid (s);
  cxt_->setForeground (fg);
  // set new mover position
  cgPoint bl (value_, ymin_);
  mover_->setbl (bl);
  mover_->draw (s);
  mover_->solid (s);
}

double
cgGauge::getValue (void) const
{
  return value_;
}

void
cgGauge::createCgCxt (cgDispDev& disp)
{
  cgPrim::createCgCxt (disp);
  if (holder_)
    holder_->setCgCxt (*cxt_);
  if (mover_)
    mover_->setCgCxt (*cxt_);
}

void
cgGauge::copyCgCxt (cgContext& cxt)
{
  cgPrim::copyCgCxt (cxt);
  if (holder_)
    holder_->setCgCxt (*cxt_);
  if (mover_)
    mover_->setCgCxt (*cxt_);
}

void
cgGauge::setCgCxt (cgContext& cxt)
{
  cgPrim::setCgCxt (cxt);
  if (holder_)
    holder_->setCgCxt (*cxt_);
  if (mover_)
    mover_->setCgCxt (*cxt_);
}

void
cgGauge::mapIndicator (const cgScene* s)
{
  if (!mapped_) {
    assert (cxt_);
    mover_->draw (s);
    mover_->solid (s);
    mapped_ = 1;
  }
}

void
cgGauge::unmapIndicator (const cgScene* s)
{
  if (mapped_) {
    Pixel fg, bg;
    cxt_->getBackground (bg);
    cxt_->getForeground (fg);
    cxt_->setForeground (bg);
    mover_->draw (s);
    mover_->solid (s);
    cxt_->setForeground (fg);  
    mapped_ = 0;
  }
}
