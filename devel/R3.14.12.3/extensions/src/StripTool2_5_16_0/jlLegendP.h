/*************************************************************************\
* Copyright (c) 1994-2004 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 1997-2003 Southeastern Universities Research Association,
* as Operator of Thomas Jefferson National Accelerator Facility.
* Copyright (c) 1997-2002 Deutches Elektronen-Synchrotron in der Helmholtz-
* Gemelnschaft (DESY).
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#ifndef _jlLegendP
#define _jlLegendP

#include <Xm/PrimitiveP.h>   
#include "jlLegend.h"

#define LEGEND_MAX_STRLEN       64

enum
{
  LGITEM_NAME = 0,
  LGITEM_RANGE,
  LGITEM_UNITS,
  LGITEM_COMMENT,
  NUM_LGITEMS
};

enum
{
  LGITEM_MASK_NAME      = (1 << LGITEM_NAME),
  LGITEM_MASK_RANGE     = (1 << LGITEM_RANGE),
  LGITEM_MASK_UNITS     = (1 << LGITEM_UNITS),
  LGITEM_MASK_COMMENT   = (1 << LGITEM_COMMENT)
};

#define LGITEM_MASK_ALL ((1 << NUM_LGITEMS)-1)


typedef struct _LegendItemInfo
{
  char                          info[NUM_LGITEMS][LEGEND_MAX_STRLEN];
  XCharStruct                   extents[NUM_LGITEMS];
  Pixel                         color;
  XRectangle                    box;
  struct _LegendItemInfo        *prev, *next;
} LegendItemInfo;


/* ====== New fields for the Legend widget class record ====== */
typedef struct _LegendClassPart
{
  int   make_compiler_happy;    /* keep compiler happy */
}
LegendClassPart;


/* ====== Full class record declaration ====== */
typedef struct _LegendClassRec
{
  CoreClassPart         core_class;
  XmPrimitiveClassPart  primitive_class;
  LegendClassPart       legend_class;
}
LegendClassRec;


extern LegendClassRec   legendClassRec;


/* ====== New fields for the Legend widget record ====== */
typedef struct _LegendPart
{
  /* resources */
  XFontStruct           *font;
  Boolean               use_pixmap;
  XtCallbackList        choose_callback;
  
  /* private state */
  GC                    gc;
  Pixmap                pixmap;
  Boolean               need_refresh;
  LegendItemInfo        *items;
  Dimension             min_width;
  XPoint                dims[NUM_LGITEMS];      /* x = width, y = height */
}
LegendPart;

/* ====== Full instance record declaration ====== */
typedef struct _LegendRec
{
  CorePart              core;
  XmPrimitivePart       primitive;
  LegendPart            legend;
}
LegendRec;

#endif
