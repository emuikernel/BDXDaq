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
//      Simple 2D graphical representation of a tape drive
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgTapeDrive.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:05  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_TAPE_DRIVE_H
#define _CG_TAPE_DRIVE_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <cgPrim.h>
#include <cgCircle.h>
#include <cgRec.h>
#include <cgPieArc.h>
#include <cgLine.h>

class cgTapeDrive: public cgPrim
{
public:
  // constructor and destructor
  // construct a tape drive through bottom left x and y + height 
  // width = 2* height
  cgTapeDrive (double blx, double bly, double height);
  virtual ~cgTapeDrive (void);

  // inherited operations
  virtual cgPrim*   copy        (void) const;
  virtual void      draw        (const cgScene* s) const;
  virtual void      erase       (const cgScene* s) const;
  virtual cgPrim&   translate   (const cgPoint& p);
  virtual void      createCgCxt (cgDispDev& disp);
  virtual void      setCgCxt    (cgContext& cxt);
  virtual void      copyCgCxt   (cgContext& cxt);

  // set pixel for tape drive body color
  void              setTapeDriveColor (Pixel color);
  void              setTapeDriveColor (char* name);

  // set pixel for tape drive body foreground color
  void              setTapeDriveFgColor (Pixel color);
  void              setTapeDriveFgColor (char* name);

  // simulate tape rotation
  void              rotateTape (const cgScene* s);
  
  // class name
  virtual const char* className (void) const {return "cgTapeDrive";}

protected:
  // creation of all internal components
  void createInternalPrims (void);
  // propagate cgContext change to all components
  void propagateCgCxt      (void);

  // real components
  cgRec*        frame_;
  cgLine*       conn_;
  cgPieArc**    left_;
  cgPieArc**    right_;

  // coordinates
  double x_, y_;
  double height_, width_;
  // Pixel color of tape drive
  Pixel color_;
  // Pixel for foreground
  Pixel fg_;
  // integet value indicating alternating filled pieArc
  int  move_;

private:
  // deny access to copy and assignment operator
  cgTapeDrive (const cgTapeDrive& drive);
  cgTapeDrive& operator = (const cgTapeDrive& drive);
};
#endif





  
