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
//      Graphics Context for All Components
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgContext.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:06  chen
//   CODA OO-graphics library for Xt
//
//

#ifndef _CG_CONTEXT_H
#define _CG_CONTEXT_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <cgDispDev.h>

#define CG_CXT_FUNCTION       GCFunction
#define CG_CXT_PLANE_MASK     GCPlaneMask
#define CG_CXT_FOREGROUND     GCForeground
#define CG_CXT_BACKGROUND     GCBackground
#define CG_CXT_LINE_WIDTH     GCLineWidth
#define CG_CXT_LINE_STYLE     GCLineStyle
#define CG_CXT_CAP_STYLE      GCCapStyle
#define CG_CXT_JOIN_STYLE     GCJoinStyle
#define CG_CXT_FONT           GCFont

// all functions
#define CG_CXT_FUNC_CLEAR     GXclear
#define CG_CXT_FUNC_AND       GXand
#define CG_CXT_FUNC_COPY      GXcopy
#define CG_CXT_FUNC_NOOP      GXnoop
#define CG_CXT_FUNC_XOR       GXxor
#define CG_CXT_FUNC_SET       GXset

// all line style
#define CG_CXT_LINE_SOLID     LineSolid
#define CG_CXT_LINE_DASH      LineOnOffDash


// all cap style
#define CG_CXT_CAP_NOTLAST    CapNotLast
#define CG_CXT_CAP_BUTT       CapButt
#define CG_CXT_CAP_ROUND      CapRound
#define CG_CXT_CAP_PROJECTING CapProjecting

// all join style
#define CG_CXT_JOIN_MITER     JoinMiter
#define CG_CXT_JOIN_ROUND     JoinRound
#define CG_CXT_JOIN_BEVEL     JoinBevel

class cgContextRep
{
public:
  // constructors and destructor
  cgContextRep       (cgDispDev& disp);
  cgContextRep       (cgDispDev& disp, XGCValues* val);
  ~cgContextRep      (void);

  XGCValues          values_;
  GC                 xgc_;
  int                refcount_;

  cgDispDev&         disp_;
  // keep xwindow display around in case we need the display but
  // disp_ is already deleted
  Display*           xdisplay_;
};


class cgContext
{
public:
  // constructors
  cgContext          (cgDispDev& disp);
  cgContext          (const cgContext& cxt);
  virtual ~cgContext (void);

  cgContext& operator = (const cgContext& cxt);

  // really duplicate context instead of using reference counting
  cgContext* copy (void) const;

  // reset all context to default values
  virtual void reset (void);
  virtual void setForeground (Pixel color);
  virtual void setForeground (unsigned short r,
			      unsigned short g,
			      unsigned short b);
  virtual void setForeground (char* colorname);
  virtual void setBackground (Pixel color);
  virtual void setBackground (unsigned short r,
			      unsigned short g,
			      unsigned short b);
  virtual void setBackground (char* colorname);
  virtual void setFunction   (int func);
  virtual void setFont       (char *fontname);
  virtual void setLineWidth  (int width);
  virtual void setLineStyle  (int style);
  virtual void setCapStyle   (int style);
  virtual void setJoinStyle  (int style);

  // set shadow color
  virtual Pixel liteShade    (Pixel color);
  virtual Pixel darkShade    (Pixel color);

  // retrieve values
  virtual void getForeground (Pixel& fg);
  virtual void getBackground (Pixel& bg);
  virtual void getFunction   (int& func);
  virtual void getLineWidth  (int& width);
  // called has to free font structure using XFreeFontInfo
  // which just free font structure but not the font
  virtual XFontStruct* getFont (void) const;

  // return device width and height
  virtual Dimension deviceWidth   (void) const;
  virtual Dimension deviceHeight  (void) const;

  // return display device
  cgDispDev& dispDev (void) const;

  // return X window graphics context, internal use only
  GC           xgc       (void) const;
  Display*     xdisplay  (void) const;
  Drawable     xdrawable (void) const;

  virtual const char* className (void) const {return "cgContext";}

protected:
  // protected constructor
  cgContext          (cgDispDev& disp, XGCValues* val);
  // real stuff
  cgContextRep*      rep_;
};
#endif


