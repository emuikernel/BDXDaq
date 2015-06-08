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
//      Implementation of a simple 2D real time strip plot
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cg2DRtPlot.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:05  chen
//   CODA OO-graphics library for Xt
//
//
#include <cgColorCxt.h>
#include "cg2DRtPlot.h"

#define CG_LABEL_FONT "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*"

cg2DRtPlot::cg2DRtPlot (double blx, double bly, 
			double height, int bufsize,
			int history)
:cgPrim (), x_ (blx), ymin_ (bly), ymax_ (bly + height), height_ (height),
 width_ (bufsize), bufsize_ (bufsize), history_ (history), cursor_ (0),
 top_ (0), fgColor_ (1), pymin_ (0.0), pymax_ (0.0), yscaleFac_ (4.0)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create cg2DRtPlot Class Object\n");
#endif
  createInternalPrims ();
}

cg2DRtPlot::~cg2DRtPlot (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete cg2DRtPlot Class Object\n");
#endif

  delete xaxis_;
  delete yaxis_;
  delete topLabel_;

  int numLines;
  if (cursor_ >= bufsize_)
    numLines = bufsize_ - 1;
  else
    numLines = cursor_ - 1;
  for (int i = 0; i < numLines; i++) 
    delete lines_[i];

  delete []lines_;
  delete []top_;
  cursor_ = 0;
}

void
cg2DRtPlot::createInternalPrims (void)
{
  // create all line pointers
  lines_ = new cgLine*[bufsize_ - 1];
  // history
  top_ = new double[bufsize_];
  
  xaxis_ = new cgLine (x_, ymin_, x_ + width_, ymin_);
  yaxis_ = new cgLine (x_, ymin_, x_, ymax_);
  
  // create top label
  char temp[32];
  double  value = ceiling (ymax_);
  sprintf (temp, "%.1lf", value);

  topLabel_ = new cgString (temp, CG_LABEL_FONT, x_, ymax_,
			    CG_ALIGNMENT_BEGINNING, CG_ALIGNMENT_TOP);
}

double
cg2DRtPlot::ceiling (double val)
{
  if (val*val > 1.0)
    return (int)val + 1.0;
  else
    return val;
}

void
cg2DRtPlot::rewriteTopLabel (const cgScene* scene)
{
  // delete old label
  topLabel_->erase (scene);
  delete topLabel_;

  // create new label
  double value = ceiling (ymax_);
  char temp[32];

  sprintf (temp, "%.1lf", value);
  topLabel_ = new cgString (temp, CG_LABEL_FONT, x_, ymax_,
			    CG_ALIGNMENT_BEGINNING, CG_ALIGNMENT_TOP);
  if (cxt_)
    topLabel_->setCgCxt (*cxt_);
  topLabel_->draw (scene);
}
  

void
cg2DRtPlot::rescale (double point, const cgScene* s)
{
  int redraw = 0;
  int erased = 0;
  int redrawTopLabel = 0;

  int i = 0;
  cgWinView& wv = (cgWinView& )s->givwv ();


  // check x coordinates
  if (cursor_ >= bufsize_) {
    for (i = 0; i < bufsize_ - 1; i++)
      delete lines_[i];

    // move history part to the beginning
    int j = 0;
    for (i = bufsize_ - history_; i < bufsize_; i++) {
      top_[j++] = top_[i];
    }

    cursor_ = j;

    // create all new line segments
    for (i = 0; i < cursor_ - 1; i++) {
      lines_[i] = new cgLine (i + x_, top_[i], i + 1 + x_, top_[i + 1]);
      lines_[i]->setCgCxt (*cxt_);      
    }

    // recheck current maximum and minimum value
    pymin_ = top_[0];
    pymax_ = top_[0];
    for (i = 1; i < cursor_; i++) {
      if (top_[i] > pymax_)
	pymax_ = top_[i];
      if (top_[i] < pymin_)
	pymin_ = top_[i];
    }
    
    redraw = 1;
  }
  
  // check y direction
  if (point > ymax_ - topLabel_->height (s)) {
    ymax_ = point + (ymax_ - ymin_)/yscaleFac_ + topLabel_->height (s);
    height_ = ymax_ - ymin_;
    // reset new axis end points
    yaxis_->setEndPoints (x_, ymin_, x_, ymax_);

    // rescale if ymax is greater than the window boundary
    if (ymax_ > wv.ymax() ) {
      // should erase old everything in the old window coordinate
      erase (s);
      erased = 1;
      // change window boundary
      wv.setYmax (ymax_);
      redraw = 1;
    }
    else {
      if (cxt_) {
	yaxis_->erase (s);
	// draw new axis
	yaxis_->draw (s);
      }
    }
    redrawTopLabel = 1;
  }
  else if (point < ymin_) {
    ymin_ = point - (ymax_ - ymin_)/yscaleFac_;
    // update height information
    height_ = ymax_ - ymin_;
    // create new yaxis
    yaxis_->setEndPoints (x_, ymin_, x_, ymax_);
    xaxis_->setEndPoints (x_, ymin_, x_ + width_, ymin_);

    // check y value compared to ymin_
    if (ymin_ < wv.ymin ()) {
      // erase everything in the old window boundary
      erase (s);
      erased = 1;

      wv.setYmin (ymin_ - (ymax_ - ymin_)/yscaleFac_);
    }
    redraw = 1;
  }

  // check whether we need to rescale the plot if window max >> pymax 
  // or window ymin << pymin_
  /* if ((wv.ymax () - wv.ymin()) > 
      yscaleFac_*(pymax_ - pymin_ - topLabel_->height (s) )) {
    // should erase old everything in the old window coordinate
    erase (s);
    erased = 1;

    ymax_ = pymax_ + (ymax_ - ymin_)/yscaleFac_ + topLabel_->height (s);
    ymin_ = pymin_; 
    */

  if ((wv.ymax () - wv.ymin()) > 
      yscaleFac_*(pymax_ - ymin_ - topLabel_->height (s) )) {
    // should erase old everything in the old window coordinate
    erase (s);
    erased = 1;

    ymax_ = pymax_ + (ymax_ - ymin_)/yscaleFac_ + topLabel_->height (s);

    height_ = ymax_ - ymin_;
    // reset end points for x y axis
    yaxis_->setEndPoints (x_, ymin_, x_, ymax_);
    xaxis_->setEndPoints (x_, ymin_, x_ + width_, ymin_);

    // change window boundary
    wv.setYmax (ymax_);
    wv.setYmin (ymin_);

    // set redraw flag
    redraw = 1;
    
    // set redraw top label flag
    redrawTopLabel = 1;
  }


  // check redraw top label flag
  if (redrawTopLabel) 
    rewriteTopLabel (s);

  // check redraw flag
  if (redraw) {
    if (!erased)
      erase (s);
    draw (s);
  }
}

void
cg2DRtPlot::addPoint (double point, const cgScene* s)
{
  int i = 0;
  cgPoint p[4];

  // update minimum and maximum value of all points
  if (point > pymax_)
    pymax_ = point;
  if (point < pymin_)
    pymin_ = point;

  // rescale the plot
  rescale (point, s);

  if (cursor_ == 0) { // the first plot
    top_[cursor_] = point;

    // advance cursor position
    cursor_ ++;
    return;
  }

  lines_ [cursor_ - 1]  = new cgLine (x_ + cursor_ - 1, top_[cursor_ - 1],
				      x_ + cursor_, point);
  if (cxt_)
    lines_[cursor_ - 1]->setCgCxt (*cxt_);
  // fill this new polygon
  lines_[cursor_ - 1]->draw (s);
  // update previous value array
  top_[cursor_] = point;

  // advance cursor position
  cursor_ ++;
}

void
cg2DRtPlot::reset (const cgScene* s)
{
  int numLines = 0;
  if (cursor_ >= bufsize_)
    numLines = bufsize_ - 1;
  else
    numLines = cursor_ - 1;

  for (int i = 0; i < numLines; i++)
    delete lines_[i];
  cursor_ = 0;

  if (cxt_->xdrawable ()) {
    erase (s);
    draw (s);
  }
}

cgPrim*
cg2DRtPlot::copy (void) const
{
  cg2DRtPlot* newplot = new cg2DRtPlot (x_, ymin_, height_,
					bufsize_, history_);
  if (cxt_) 
    newplot->copyCgCxt (*cxt_);
  newplot->foregroundColor (fgColor_);
  return newplot;
}

void
cg2DRtPlot::draw (const cgScene* s) const
{
  int i = 0;
  assert (cxt_);

  if (cxt_->xdrawable () == 0)
    return;

  // get foreground and background color
  Pixel fg, bg;
  cxt_->getForeground (fg);
  cxt_->getBackground (bg);
  
  cxt_->setForeground (fgColor_);

  // draw top label
  topLabel_->draw (s);
  // draw all lines
  if (cursor_ >= bufsize_) { // in case of horizontal scaling
    for (i = 0; i < bufsize_ - 1; i++)
      lines_[i]->draw (s);
  }
  else {
    for (i = 0; i < cursor_ - 1; i++)
      lines_[i]->draw (s);
  }
  // draw x amd y axis
  xaxis_->draw (s);
  yaxis_->draw (s);
}

void
cg2DRtPlot::erase (const cgScene* s) const
{
  assert (cxt_);

  if (cxt_->xdrawable () == 0)
    return;

  Pixel fg, bg;
  cxt_->getForeground (fg);
  cxt_->getBackground (bg);

  cgRec eraseRec (x_, ymin_, width_, height_);
  eraseRec.setCgCxt (*cxt_);
  cxt_->setForeground (bg);
  eraseRec.draw (s);
  eraseRec.solid (s);

  // reset foreground
  cxt_->setForeground (fg);
} 
  

cgPrim&
cg2DRtPlot::translate (const cgPoint& p)
{
  x_ += p.givx ();
  ymin_ += p.givy ();
  ymax_ += p.givy ();
  return *this;
}

void
cg2DRtPlot::propagateCgCxt (void)
{
  int i = 0;

  xaxis_->copyCgCxt (*cxt_);
  yaxis_->copyCgCxt (*cxt_);

  topLabel_->setCgCxt (*cxt_);

  if (cursor_ >= bufsize_) { // in case of horizontal scaling
    for (i = 0; i < bufsize_ - 1; i++)
      lines_[i]->setCgCxt (*cxt_);
  }
  else {
    for (i = 0; i < cursor_ - 1; i++)
      lines_[i]->setCgCxt (*cxt_);
  }
}

void
cg2DRtPlot::createCgCxt (cgDispDev& disp)
{
  cgPrim::createCgCxt (disp);
  propagateCgCxt ();
  cxt_->getForeground (fgColor_);
}

void
cg2DRtPlot::copyCgCxt (cgContext& cxt)
{
  cgPrim::copyCgCxt (cxt);
  propagateCgCxt ();
  cxt_->getForeground (fgColor_);
}

void
cg2DRtPlot::setCgCxt (cgContext& cxt)
{
  cgPrim::setCgCxt (cxt);
  propagateCgCxt ();
  cxt_->getForeground (fgColor_);
}

void
cg2DRtPlot::foregroundColor (Pixel color)
{
  fgColor_ = color;
}

void
cg2DRtPlot::foregroundColor (char* name)
{
  assert (cxt_);
  cgColorCxt* cxt = cxt_->dispDev ().colorCxt ();
  fgColor_ = cxt->pixel (name);
}

void
cg2DRtPlot::yscaleFactor (double factor)
{
  yscaleFac_ = factor;
}




