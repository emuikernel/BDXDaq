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
//      Implementation of cgPrim Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgPrim.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#include <cgScene.h>
#include "cgPrim.h"

cgPrim::cgPrim (cgPrim* nxt)
:next(nxt), cxt_ (0) 
{
  // empty
}

cgPrim::~cgPrim (void) 
{
  if (next)
    delete next; 
  if (cxt_) 
    delete cxt_;
}

void
cgPrim::attach (cgScene* s)
{
  s->addPrim (this);
}

void
cgPrim::detach (cgScene* s)
{
  s->removePrim (this);
  this->next = 0;  // reset next to 0
}

void 
cgPrim::erase (const cgScene* scene) const
{
  assert (cxt_);
  Pixel fg, bg;

  cxt_->getForeground (fg);
  cxt_->getBackground (bg);
  cxt_->setForeground (bg);
  draw (scene);
  cxt_->setForeground (fg);
}

void
cgPrim::createCgCxt (cgDispDev& disp) 
{
  if (cxt_) delete cxt_;
  cxt_=new cgContext (disp);
}

void
cgPrim::copyCgCxt (cgContext& cxt)
{
  if (cxt_)
    delete cxt_;
  cxt_ = cxt.copy ();
}

void
cgPrim::setCgCxt (cgContext& cxt) 
{
  if (cxt_) delete cxt_;
  cxt_ = new cgContext (cxt);
}

cgContext* 
cgPrim::cgCxt (void) const 
{
  return cxt_;
}
  // check a single point inside the region of this object.
