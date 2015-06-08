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
//      Simple 2D rectangle
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgRec.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_REC_T
#define _CG_REC_T

#include <cgPrim.h>
#include <cgPolygon.h>
#include <cgScene.h>

class cgRec: public cgPolygon
{
public:
  cgRec (void);
  cgRec (double blx, double bly, double width, double height);
  cgRec (const cgPoint& bl, double width, double height);
  virtual ~cgRec (void);

  // return attribute
  cgPoint         givbl     (void) const;
  double          width     (void) const;
  double          height    (void) const;

  // set attributes
  void            setbl     (const cgPoint& cgbl);
  void            width     (double w);
  void            height    (double h);

protected:
  cgPoint bleft_;
  double  width_;
  double  height_;

private:
  cgRec (const cgRec &);
};
#endif
