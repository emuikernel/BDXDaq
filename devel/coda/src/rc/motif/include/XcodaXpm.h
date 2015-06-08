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
//   $Log: XcodaXpm.h,v $
//   Revision 1.2  1998/04/08 17:29:43  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:59  chen
//   coda motif C++ library
//
//
#ifndef _XCODA_XPM_H
#define _XCODA_XPM_H

class XcodaXpm
{
public:
  // constructors and destructors
  XcodaXpm (Widget parent, char **pix);
  XcodaXpm (Widget parent, char **pix, char *bgsymbol);
  virtual ~XcodaXpm();
  Pixmap  createPixmapFromXpm();
  Pixmap  getPixmap (void);
  virtual const char *className() const {return "XcodaXpm";}

protected:
  // this class will handle memory release of this pixmap
  Pixmap    _pixmap;

private:
  Widget   _parent;
  Display  *_display;
  Colormap _cmap;
  Screen   *_scr;
  Window   _win;
  int      _depth;
  char     **_pix;
  char     *_bgsymbol;
};
#endif

