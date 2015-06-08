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
//      Window Image created by XPM
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgXpmImage.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_XPM_IMAGE_H
#define _CG_XPM_IMAGE_H

#include <cgConst.h>
#include <cgXpm.h>
#include <cgPrim.h>
#include <cgString.h>

class cgXpmImage: public cgPrim
{
public:
  // constructor and destructor
  cgXpmImage (char* name, char** pix, char* bgsymbol = 0, 
	      int alignment = CG_ALIGNMENT_BEGINNING);
  // X and Y are bottom left corner of the image
  cgXpmImage (char* name, double x, double y,
	      char** pix, char* bgsymbol = 0,
	      int alignment = CG_ALIGNMENT_BEGINNING);
  virtual ~cgXpmImage (void);

  // new operation
  // realy init pixmap, note: must be done after cxt_ has been created
  virtual void init (void);

  // inherited operations
  virtual cgPrim* copy  (void) const;
  virtual void    draw  (const cgScene* s) const;
  virtual void    erase (const cgScene* s) const;
  // no support for scale and rotation yet
  virtual cgPrim& translate (const cgPoint& point);

  // set and retrieve x and y
  void    setx (double x);
  void    sety (double y);
  double  givx (void) const;
  double  givy (void) const;
  double  width (const cgScene* s) const;
  double  height (const cgScene* s) const;

  // context routines
  virtual void createCgCxt (cgDispDev& disp);
  virtual void copyCgCxt   (cgContext& cxt);
  virtual void setCgCxt    (cgContext& cxt);

  // class name
  virtual const char* className (void) const {return "cgXpmImage";}

protected:
  // real drawing primitive
  cgString* string_;
  cgXpm*    xpm_;
  // position
  double x_, y_;
  // alignment flag
  int alignment_;

private:
  // deny access to copy and assignment
  cgXpmImage (const cgXpmImage&);
  cgXpmImage& operator = (const cgXpmImage&);
};
#endif

