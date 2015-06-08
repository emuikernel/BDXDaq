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
//      A null terminated string with specified font
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgString.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_STRING_H
#define _CG_STRING_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <cgConst.h>
#include <cgPrim.h>

class cgString: public cgPrim
{
public:
  // constructor and destructor
  cgString (char* str, char* font, int alignment = CG_ALIGNMENT_BEGINNING,
	    int yalignment = CG_ALIGNMENT_CENTER);
  // CG_ALIGNMENT_BEGINNING: x, y are the bottom left corner
  // CG_ALIGNMENT_CENTER: x, y are the center of the string
  // CG_ALIGNMENT_END:    x, y are the bottom right corner
  cgString (char* str, char* font, double x, double y,
	    int alignment = CG_ALIGNMENT_BEGINNING,
	    int yalignment = CG_ALIGNMENT_CENTER);

  virtual ~cgString (void);

  // inherited operations
  virtual cgPrim* copy (void) const;
  // if any part of string is out of viewing part, do not show it
  virtual void draw (const cgScene* s) const;
  // no support for rotation and scale yet
  virtual cgPrim& translate (const cgPoint& point);

  // graphical context
  virtual void createCgCxt   (cgDispDev& disp);
  virtual void copyCgCxt     (cgContext& cxt);
  virtual void setCgCxt      (cgContext& cxt);

  // set x and y coordinates
  void    setx (double x);
  void    sety (double y);
  double  givx (void) const;
  double  givy (void) const;
  double  height (const cgScene* s) const;
  double  width  (const cgScene* s) const;

  // return string associated with this class
  char*   string (void) const;

  // class name
  virtual const char* className (void) const {return "cgString";}

protected:
  // drawing string
  char *str_;
  // font name
  char *fontname_;
  // position
  double x_, y_;
  // flag of alignment
  int alignment_;
  // flag of y alignment
  int yalignment_;

private:
  // deny access to copy and assigment operations
  cgString (const cgString& str);
  cgString& operator = (const cgString& str);
};
#endif
