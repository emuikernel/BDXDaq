//-----------------------------------------------------------------------------
// Copyright (c) 1991,1992 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
// Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
//-----------------------------------------------------------------------------
// 
// Description:
//	 Generic Class for XPM Supporing routines
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgXpm.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:06  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_XPM_H
#define _CG_XPM_H

#include <stdio.h>
#include <string.h>

#include <cgDispDev.h>

class cgXpm
{
public:
  // constructors and destructors

  // cgXpm will not copy pix into internal memory, so pix
  // must be static
  cgXpm (char **pix);
  cgXpm (char **pix, char *bgsymbol);
  virtual ~cgXpm (void);

  // set up display device pointer
  void dispDev (cgDispDev* dev);

  // return pixmap, this call must be after calling dispDev
  Pixmap  getPixmap   (void);
  // return geometry information
  unsigned int width  (void) const;
  unsigned int height (void) const;

  // get string representation of xpm etc
  char** xpmData  (void) const;
  char*  bgSymbol (void) const;
  // class name
  virtual const char *className() const {return "cgXpm";}

protected:
  // this class will handle memory release of this pixmap
  Pixmap    pixmap_;

private:
  // cgDispDev pointer
  cgDispDev* disp_;
  // keep display pointer around which may be needed after cgDispDev
  // is destroyed
  Display*   xdisplay_;
  char       **pix_;
  char       *bgsymbol_;
  // width and height information
  unsigned int width_;
  unsigned int height_;
};
#endif

