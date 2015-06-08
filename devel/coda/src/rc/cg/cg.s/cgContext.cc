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
//      Graphics Context for all components
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgContext.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:05  chen
//   CODA OO-graphics library for Xt
//
//
#include <cgColorCxt.h>
#include "cgContext.h"

#define CG_DEFAULT_FONT "-*-fixed-*-*-*-*-*-140-*-*-*-*-*-*"

//==========================================================================
//         Implementation of cgContextRep
//==========================================================================
cgContextRep::cgContextRep (cgDispDev& disp)
:disp_ (disp), refcount_ (1)
{
#ifdef _TRACE_OBJECTS
  printf ("Create cgContextRep Class Object\n");
#endif

  XFontStruct *font = XLoadQueryFont (disp_.display (), CG_DEFAULT_FONT);

  values_.function = GXcopy;
  values_.foreground = disp.fg();
  values_.background = disp.bg();
  values_.line_width = 0;
  values_.line_style = LineSolid;
  values_.cap_style = CapButt;
  values_.join_style = JoinMiter;
  values_.font = font->fid;

  int mask = CG_CXT_FUNCTION | CG_CXT_FOREGROUND 
    | CG_CXT_BACKGROUND | CG_CXT_LINE_WIDTH
    | CG_CXT_LINE_STYLE | CG_CXT_CAP_STYLE
    | CG_CXT_JOIN_STYLE | CG_CXT_FONT;

  xgc_ = XCreateGC (disp_.display (), disp_.rootW (), mask, &values_);

  // free XFontStruct without unloading the font: Important
  XFreeFontInfo (0, font, 1);
  
  // set X window display pointer
  xdisplay_ = disp_.display ();
  
}

cgContextRep::cgContextRep (cgDispDev& disp, XGCValues* val)
:disp_ (disp), refcount_ (1)
{
#ifdef _TRACE_OBJECTS
  printf ("Create cgContextRep Class Object\n");
#endif
  int mask;

  values_.function = val->function;
  values_.foreground = val->foreground;
  values_.background = val->background;
  values_.line_width = val->line_width;
  values_.line_style = val->line_style;
  values_.cap_style = val->cap_style;
  values_.join_style = val->join_style;
  values_.font = val->font;
  mask = CG_CXT_FUNCTION | CG_CXT_FOREGROUND 
    | CG_CXT_BACKGROUND | CG_CXT_LINE_WIDTH
      | CG_CXT_LINE_STYLE | CG_CXT_CAP_STYLE
	| CG_CXT_JOIN_STYLE | CG_CXT_FONT;
  xgc_ = XCreateGC (disp_.display (), disp_.rootW (), mask, &values_);
  // setup x window display pointer
  xdisplay_ = disp_.display ();
}


cgContextRep::~cgContextRep (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete cgContextRep Class Object \n");
#endif
  // use X window display pointer
  XFreeGC (xdisplay_, xgc_);
}

//========================================================================
//           Implementation of cgContext
//========================================================================
cgContext::cgContext (cgDispDev& disp)
{
#ifdef _TRACE_OBJECTS
  printf ("Create cgContext Class Object \n");
#endif
  rep_ = new cgContextRep (disp);
}

cgContext::cgContext (cgDispDev& disp, XGCValues* val)
{
#ifdef _TRACE_OBJECTS
  printf ("Create cgContext Class Object \n");
#endif
  rep_ = new cgContextRep (disp, val);
}

cgContext::~cgContext (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete cgContext Class Object \n");
#endif
  if (--rep_->refcount_ <= 0)
    delete rep_;
}

cgContext::cgContext (const cgContext& cxt)
{
  rep_ = cxt.rep_;
  (rep_->refcount_)++;
}

cgContext&
cgContext::operator = (const cgContext& cxt)
{
  if (this != &cxt) {
    if (--rep_->refcount_ <= 0)
      delete rep_;
    rep_ = cxt.rep_;
    rep_->refcount_ ++;
  }
  return *this;
}

cgContext*
cgContext::copy (void) const
{
  cgContext* cxt = new cgContext (rep_->disp_, &(rep_->values_));
  return cxt;
}

void
cgContext::reset (void)
{
  // get default font
  XFontStruct* font;
  font = XLoadQueryFont (rep_->disp_.display (),CG_DEFAULT_FONT);

  // set all value elements to default value
  rep_->values_.function = GXcopy;
  rep_->values_.foreground = rep_->disp_.fg ();
  rep_->values_.background = rep_->disp_.bg ();
  rep_->values_.line_width = 0;
  rep_->values_.line_style = LineSolid;
  rep_->values_.cap_style = CapButt;
  rep_->values_.join_style = JoinMiter;
  rep_->values_.font = font->fid;
  

  int mask = CG_CXT_FUNCTION | CG_CXT_FOREGROUND 
    | CG_CXT_BACKGROUND | CG_CXT_LINE_WIDTH
    | CG_CXT_LINE_STYLE | CG_CXT_CAP_STYLE
    | CG_CXT_JOIN_STYLE | CG_CXT_FONT;

  XChangeGC (rep_->disp_.display(), rep_->xgc_, mask, &(rep_->values_));

  // free XFontStruct without unloading the font: Important
  XFreeFontInfo (0, font, 1);
}

void
cgContext::setForeground (Pixel col)
{
  rep_->values_.foreground = col;
  XSetForeground (rep_->disp_.display(), rep_->xgc_, col);
}

void
cgContext::getForeground (Pixel& fg)
{
  unsigned long mask = CG_CXT_FOREGROUND;

  if (XGetGCValues (rep_->disp_.display(), rep_->xgc_, mask, 
		    &(rep_->values_)))
    fg = rep_->values_.foreground;
  else
    fg = 0;
}

void
cgContext::setForeground (unsigned short r,
			  unsigned short g,
			  unsigned short b)
{
  assert (rep_->disp_.colorCxt());
  rep_->values_.foreground  = rep_->disp_.colorCxt()->pixel (r, g, b);
  setForeground (rep_->values_.foreground);
}

void
cgContext::setForeground (char* colorname)
{
  assert (rep_->disp_.colorCxt());
  rep_->values_.foreground = rep_->disp_.colorCxt()->pixel (colorname);
  setForeground (rep_->values_.foreground);
}


void
cgContext::setBackground (Pixel col)
{
  rep_->values_.background = col;
  XSetBackground (rep_->disp_.display(), rep_->xgc_, col);
}

void
cgContext::getBackground (Pixel& bg)
{
  unsigned long mask = CG_CXT_BACKGROUND;

  if (XGetGCValues (rep_->disp_.display(), rep_->xgc_, mask, 
		    &(rep_->values_)))
    bg = rep_->values_.background;
  else
    bg = 0;
}

void
cgContext::setBackground (unsigned short r,
			  unsigned short g,
			  unsigned short b)
{
  assert (rep_->disp_.colorCxt());
  rep_->values_.background = rep_->disp_.colorCxt()->pixel (r, g, b);
  setBackground (rep_->values_.background);
}

void
cgContext::setBackground (char* colorname)
{
  assert (rep_->disp_.colorCxt());
  rep_->values_.background = rep_->disp_.colorCxt()->pixel (colorname);
  setBackground (rep_->values_.background);
}


void
cgContext::setFunction (int func)
{
  rep_->values_.function = func;
  XSetFunction (rep_->disp_.display (), rep_->xgc_, func);
}

void
cgContext::getFunction (int& func)
{
  unsigned long mask = CG_CXT_FUNCTION;

  if (XGetGCValues (rep_->disp_.display(), rep_->xgc_, mask, 
		    &(rep_->values_)))
    func = rep_->values_.function;
  else
    func = CG_CXT_FUNC_COPY;
}  

void
cgContext::setFont (char* fontname)
{
  XFontStruct* font;
  font = XLoadQueryFont (rep_->disp_.display (), fontname);
  if (font == 0) { // not found, using default
    font = XLoadQueryFont (rep_->disp_.display (), CG_DEFAULT_FONT);
  }
  rep_->values_.font = font->fid;
  XSetFont (rep_->disp_.display (), rep_->xgc_, font->fid);

  // free XFontStruct without unloading the font: Important
  XFreeFontInfo (0, font, 1);
}

XFontStruct*
cgContext::getFont (void) const
{
  XFontStruct* f = XQueryFont (rep_->disp_.display (), rep_->values_.font);
  return f;
}

void
cgContext::setLineWidth (int width)
{
  rep_->values_.line_width = width;
  int mask = CG_CXT_LINE_WIDTH;

  XChangeGC (rep_->disp_.display (), rep_->xgc_, mask, &(rep_->values_));
}

void
cgContext::getLineWidth (int& width)
{
  unsigned long mask = CG_CXT_LINE_WIDTH;
  if (XGetGCValues (rep_->disp_.display (), rep_->xgc_, mask, 
		    &(rep_->values_)))
    width = rep_->values_.line_width;
  else
    width = 0;
}

void
cgContext::setLineStyle (int style)
{
  rep_->values_.line_style = style;
  int mask = CG_CXT_LINE_STYLE;

  XChangeGC (rep_->disp_.display (), rep_->xgc_, mask, &(rep_->values_));  
}

void
cgContext::setCapStyle (int style)
{
  rep_->values_.cap_style = style;
  int mask = CG_CXT_CAP_STYLE;

  XChangeGC (rep_->disp_.display (), rep_->xgc_, mask, &(rep_->values_));
}

void
cgContext::setJoinStyle (int style)
{
  rep_->values_.join_style = style;
  int mask = CG_CXT_JOIN_STYLE;

  XChangeGC (rep_->disp_.display (), rep_->xgc_, mask, &(rep_->values_));
}

GC
cgContext::xgc (void) const
{
  return rep_->xgc_;
}

Display*
cgContext::xdisplay (void) const
{
  return rep_->disp_.display ();
}

Drawable
cgContext::xdrawable (void) const
{
  return rep_->disp_.drawable ();
}

Pixel
cgContext::liteShade (Pixel color)
{
  return rep_->disp_.colorCxt()->liteShadePixel (color);
}


Pixel
cgContext::darkShade (Pixel color)
{
  return rep_->disp_.colorCxt()->darkShadePixel (color);
}

Dimension
cgContext::deviceWidth (void) const
{
  return rep_->disp_.width ();
}

Dimension
cgContext::deviceHeight (void) const
{
  return rep_->disp_.height ();
}

cgDispDev&
cgContext::dispDev (void) const
{
  return rep_->disp_;
}
