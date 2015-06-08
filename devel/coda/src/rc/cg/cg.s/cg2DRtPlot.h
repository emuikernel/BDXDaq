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
//      2 dimensional real time plot (like strip chart)
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cg2DRtPlot.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:05  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_2DRT_PLOT_H
#define _CG_2DRT_PLOT_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <cgPrim.h>
#include <cgRec.h>
#include <cgLine.h>
#include <cgPolygon.h>
#include <cgString.h>

class cg2DRtPlot: public cgPrim
{
public:
  // constructor and destructor
  // constructed by bottom left coordinate and height,
  // plus number of points to remember as history
  cg2DRtPlot    (double blx, double bly, 
		 double height,
		 int bufsize = 100, int history = 50);
  virtual ~cg2DRtPlot (void);

  // inherited operations
  virtual cgPrim*   copy       (void) const;
  virtual void      draw       (const cgScene* s) const;
  virtual void      erase      (const cgScene* s) const;
  virtual cgPrim&   translate  (const cgPoint& point);
  virtual void      createCgCxt (cgDispDev& disp);
  virtual void      setCgCxt    (cgContext& cxt);
  virtual void      copyCgCxt   (cgContext& cxt);

  // set foreground color
  void              foregroundColor (Pixel color);
  void              foregroundColor (char* colorname);

  // add new point to the buffer
  void              addPoint (double value, const cgScene* s);

  // reset the plot
  void              reset (const cgScene* s);

  // set y axis scale factor
  // default is ymax = valuemax + (ymax - ymin)/4.0
  void              yscaleFactor  (double factor);

  // class name
  virtual const char* className (void) const {return "cg2DRtPlot";}
  
protected:
  // create all internal components
  void createInternalPrims (void);
  // propage cgContext change to all components
  void propagateCgCxt (void);

  // rescale the plot
  void rescale (double value, const cgScene* scene);

  // draw top label
  void rewriteTopLabel (const cgScene* scene);

  // round ymax value
  double ceiling (double yval);

  // all internal components
  cgLine* xaxis_;
  cgLine* yaxis_;
  cgString* topLabel_;
  cgLine** lines_;
  
  // internal coordinates
  double x_;
  double ymin_, ymax_;
  double width_, height_;

  // foreground color
  Pixel fgColor_;

  // buffer size and history
  int bufsize_, history_;
  double *top_;

  // current cursor
  int cursor_;

  // current ymax and ymin value of points
  double pymax_, pymin_;

  // y value scale factor
  double yscaleFac_;

private:
  // deny access to copy and assignment
  cg2DRtPlot (const cg2DRtPlot& plot);
  cg2DRtPlot& operator = (const cg2DRtPlot& plot);
};
#endif

  
