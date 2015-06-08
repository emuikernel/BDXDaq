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
//      Simple 2D Rectangle with frame
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgFrameRec.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_FRAMEREC_T
#define _CG_FRAMEREC_T

#include <cgRec.h>

#define CG_SHADOW_IN  0
#define CG_SHADOW_OUT 1

class cgFrameRec: public cgPrim
{
public:
  // constructor and destructor
  cgFrameRec (double blx, double bly, double width, double height,
	      double frameWidth, int frameType = CG_SHADOW_IN);
  cgFrameRec (const cgPoint& bl, double width, double height,
	      double frameWidth, int frameType = CG_SHADOW_IN);
  virtual ~cgFrameRec (void);

  virtual cgPrim*  copy      (void) const;
  virtual void     draw      (const cgScene* s) const;
  virtual void     erase     (const cgScene* s) const;
  virtual cgPrim&  rot       (double angle);
  virtual cgPrim&  translate (const cgPoint& distance);

  // color context manipulation
  virtual void createCgCxt (cgDispDev& disp);
  virtual void copyCgCxt   (cgContext& cxt);
  virtual void setCgCxt    (cgContext& cxt);

  // contains point?
  virtual int  contains (const cgPoint& p);
  // action and selection
  virtual void select (const cgScene* s);

protected:
  // create all followings
  void createInternalPrims (double blx, double bly);

  cgPoint bleft_;
  double  width_;
  double  height_;
  double  frameWd_;
  int     frameType_;
  cgPoint center_;

private:
  // inside rectangle
  cgRec*       irec_;
  cgPolygon*   top_;
  cgPolygon*   bot_;
  cgPolygon*   left_;
  cgPolygon*   rite_;
};
#endif
