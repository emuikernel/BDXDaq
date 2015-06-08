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
//      Simple 2D Trash Can Symbol
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgTrashCan.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:05  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_TRASH_CAN_H
#define _CG_TRASH_CAN_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <cgPrim.h>
#include <cgRec.h>
#include <cgPolygon.h>
#include <cgLine.h>

class cgTrashCan: public cgPrim
{
public:
  // constructor and destructor
  // constructoed by bottom left coordinate and height (width = 3*height/4)
  cgTrashCan         (double blx, double bly,
		      double height);
  virtual ~cgTrashCan (void);

  // inherited operations
  virtual cgPrim*     copy   (void) const;
  virtual void        draw   (const cgScene* s) const;
  virtual void        erase  (const cgScene* s) const;
  virtual cgPrim&     translate (const cgPoint& point);
  virtual void        createCgCxt (cgDispDev& dev);
  virtual void        setCgCxt    (cgContext& cxt);
  virtual void        copyCgCxt   (cgContext& cxt);

  // set trashcan body color
  void                setCanColor (Pixel color);
  void                setCanColor (char* name);
  // set forground color
  void                setCanFgColor (Pixel color);
  void                setCanFgColor (char* name);

  // open and close
  void                close (const cgScene* s);
  void                open  (const cgScene* s);

  // class name
  virtual const char* className (void) const {return "cgTrashCan";}

protected:
  // create all internal components
  void createInternalPrims (void);
  // propagate cgContext change to all components
  void propagateCgCxt (void);

  // real internal components
  cgRec*     body_;
  cgLine**   shades_;
  cgPolygon* cover_;
  cgPolygon* handle_;
  cgPolygon* tiltedCover_;
  cgPolygon* tiltedHandle_;

  // internal coordinates
  double x_, y_;
  double width_, height_;

  // color
  Pixel color_, fg_;
  
  // flag to tell open or closed
  int closed_;

private:
  // deny access to copy and assignment
  cgTrashCan (const cgTrashCan& can);
  cgTrashCan& operator = (const cgTrashCan& can);
};
#endif


  

