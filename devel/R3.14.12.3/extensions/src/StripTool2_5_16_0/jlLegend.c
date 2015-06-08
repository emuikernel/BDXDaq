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

#include <Xm/XmP.h>
#include "jlLegendP.h"

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_WIDTH           75
#define DEFAULT_HEIGHT          100
#define ITEM_INFO_PAD           2
#define COLOR_NPIXELS_TOP       1
#define COLOR_NPIXELS_BOTTOM    1
#define COLOR_NPIXELS_SIDE      5
#define MIN_WIDTH_NUM_CHARS     16

#define offset(field) XtOffsetOf(LegendRec, field)

/* resources
 */
static XtResource       resources[] =
{
  /* font */
  {
    XmNfont,
    XmCFont,
    XmRFontStruct,
    sizeof (XFontStruct),
    offset (legend.font),
    XmRString,
    (XtPointer)XtDefaultFont
  },

  /* use_pixmap */
  {
    XjNusePixmap,
    XjCUsePixmap,
    XmRBoolean,
    sizeof (Boolean),
    offset (legend.use_pixmap),
    XmRImmediate,
    (XtPointer)False
  },

  /* choose callback list */
  {
    XjNchooseCallback,
    XmCCallback,
    XmRCallback,
    sizeof (XtCallbackList),
    offset (legend.choose_callback),
    XmRCallback,
    (XtPointer)0
  },

  /* min width */
  {
    XmNminWidth,
    XmCMinWidth,
    XmRDimension,
    sizeof (Dimension),
    offset (legend.min_width),
    XmRImmediate,
    (XtPointer)DEFAULT_WIDTH
  }
};


/* static variables
 */
 


/* method prototypes
 */
static void     ClassInitialize (void);

static void     Initialize      (Widget,        /* request */
                                 Widget,        /* new */
                                 ArgList,       /* args */
                                 Cardinal *);   /* num_args */

static void     Redisplay       (Widget,
                                 XEvent *,      /* event */
                                 Region);       /* region */

static void     Destroy         (Widget);

static void     Resize          (Widget);

static Boolean  SetValues       (Widget,        /* old */
                                 Widget,        /* request */
                                 Widget,        /* new */
                                 ArgList,       /* args */
                                 Cardinal *);   /* num_args */

static XtGeometryResult QueryGeometry   (Widget,                
                                         XtWidgetGeometry *,    /* request */
                                         XtWidgetGeometry *);   /* reply */

/* action function prototypes
 */
static void     Arm             (Widget,
                                 XEvent *,
                                 String *,      /* params */
                                 Cardinal *);   /* num_params */

/* private function prototypes
 */
static void     GetPixmap               (LegendWidget);
static void     FreePixmap              (LegendWidget);
#if 0
/* KE: unused */
static void     Update                  (LegendWidget);
#endif
static void     Draw                    (LegendWidget, Drawable);
static void     CalculateDims           (LegendWidget);
static void     GetDimsForMask          (LegendWidget, int, XPoint *);
static int      BestMaskForHeight       (LegendWidget, int, int);


/* converter prototypes
 */


/* default translations
 */
static char             defaultTranslations[] =
        "<Btn1Down>:    Arm(1)  \n"
        "<Btn2Down>:    Arm(2)  \n"
        "<Btn3Down>:    Arm(3)  \n";

/* actions
 */
static XtActionsRec     actions[] =
{
  {"Arm",       Arm}
};


/* class record
 */
LegendClassRec          legendClassRec = {
  /* core_class fields */
  {
    /* superclass               */      (WidgetClass) &xmPrimitiveClassRec,
    /* class_name               */      "Legend",
    /* widget_size              */      sizeof (LegendRec),
    /* class_initialize         */      ClassInitialize,
    /* class_part_initialize    */      NULL,
    /* class_inited             */      FALSE,
    /* initialize               */      Initialize,
    /* initialize_hook          */      NULL,
    /* realize                  */      XtInheritRealize,
    /* actions                  */      actions,
    /* num_actions              */      XtNumber (actions),
    /* resources                */      resources,
    /* num_resources            */      XtNumber (resources),
    /* xrm_class                */      NULLQUARK,
    /* compress_motion          */      TRUE,
    /* compress_exposure        */      XtExposeCompressMultiple,
    /* compress_enterleave      */      FALSE,
    /* visible_interest         */      FALSE,
    /* destroy                  */      Destroy,
    /* resize                   */      Resize,
    /* expose                   */      Redisplay,
    /* set_values               */      SetValues,
    /* set_values_hook          */      NULL,
    /* set_values_almost        */      XtInheritSetValuesAlmost,
    /* get_values_hook          */      NULL,
    /* accept_focus             */      NULL,
    /* version                  */      XtVersion,
    /* callback_private         */      NULL,
    /* tm_table                 */      defaultTranslations,
    /* query_geometry           */      QueryGeometry,
    /* display_accelerator      */      XtInheritDisplayAccelerator,
    /* extension                */      NULL
    },

  /* Primitive class fields */  
  {
    /* border_highlight         */      XmInheritWidgetProc,       
    /* border_unhighlight       */      XmInheritWidgetProc,    
    /* translations             */      XmInheritTranslations,      
    /* arm_and_activate         */      NULL,             
    /* syn resources            */      NULL,           
    /* num_syn_resources        */      0, 
    /* extension                */      NULL,
    },

  /* Legend */
  {
    /* placeholder              */      0,
  },
};

WidgetClass legendWidgetClass = (WidgetClass) &legendClassRec;


/* ========================================================================
 *
 *         >>>>>>>>>>   U S E F U L   M A C R O S   <<<<<<<<<<
 *
 * ========================================================================
 */
#ifndef MAX
#define MAX(a,b)        ((a) > (b)? (a) : (b))
#endif


/* ========================================================================
 *
 *         >>>>>>>>>>   W I D G E T   M E T H O D S   <<<<<<<<<<
 *
 * ========================================================================
 */

/* ClassInitialize
 */
static void
ClassInitialize (void)
{
}


/* Initialize
 */
static void
Initialize      (Widget         treq,
                 Widget         tnew,
                 ArgList        args,
                 Cardinal       *num_args)
{
  LegendWidget new = (LegendWidget) tnew;

  new->legend.items = 0;
  new->legend.pixmap = 0;
  new->legend.need_refresh = True;

  new->legend.gc = XtAllocateGC
    ((Widget)new, new->core.depth, 0, 0, GCForeground | GCFont, 0);
  XSetFont (XtDisplay(new), new->legend.gc, new->legend.font->fid);

  /* Motif Primitive sets width and height to provide enough room for
   * the highlight and shadow around a widget.  A widget that uses these
   * features would *add* its desired dimensions to those set by Primitive.
   */
  new->core.width = MAX (new->core.width, new->legend.min_width);
  new->core.height = MAX (new->core.height, DEFAULT_HEIGHT);

  /* don't want any border */
  new->core.border_width = 0;

  /* tell Primitive not to allow tabbing to this widget,
   * set shadow thickness to 0, etc. */ 
  XtVaSetValues
    ((Widget)new,
     XmNtraversalOn,            False,
     XmNshadowThickness,        (Dimension)0,
     XmNhighlightThickness,     (Dimension)0,
     NULL);
}


/* Redisplay
 *
 *      If the event is null, then the function was called
 *      by the widget itself.
 */
static void
Redisplay       (Widget w, XEvent *event, Region region)
{
  LegendWidget  cw = (LegendWidget)w;
  XExposeEvent  *expose = 0;

  if (!XtIsRealized ((Widget)cw)) return;
  if (!cw->legend.pixmap && cw->legend.use_pixmap) GetPixmap (cw);

  /* determine event type */
  if (event) if (event->type == Expose) expose = (XExposeEvent *)event;
    
  /* if no pixmap, draw directly into window */
  if (!cw->legend.pixmap)
  {
    /* If this wasn't an expose event, then the window hasn't
     * been cleared for us */
    if (!expose) XClearWindow (XtDisplay (cw), XtWindow (cw));
    Draw (cw, XtWindow (cw));
    cw->legend.need_refresh = False;
  }

  /* use pixmap */
  else
  {
    /* if the pixmap needs to be updated, do that */
    if (cw->legend.need_refresh) Draw (cw, cw->legend.pixmap);
    cw->legend.need_refresh = False;

    /* if called internally, redraw the whole area */
    if (!expose)
      XCopyArea
        (XtDisplay (cw), cw->legend.pixmap, XtWindow (cw), cw->legend.gc,
         0, 0, cw->core.width, cw->core.height, 0, 0);
                  
    /* otherwise, copy the bounding box for the exposed region
     * from the pixmap */
    else
      XCopyArea
        (XtDisplay (cw), cw->legend.pixmap, XtWindow (cw), cw->legend.gc,
         expose->x, expose->y, expose->width, expose->height,
         expose->x, expose->y);
  }
}



static Boolean
SetValues       (Widget         cur_w,
                 Widget         req_w,
                 Widget         new_w,
                 ArgList        args,
                 Cardinal       *num_args)
{
  LegendWidget  cur = (LegendWidget) cur_w;
  LegendWidget  new = (LegendWidget) new_w;
  Boolean       do_redisplay = False;
  Boolean       do_recompute = False;

  if ((cur->primitive.foreground != new->primitive.foreground) ||
      (cur->core.background_pixel != new->core.background_pixel) ||
      (cur->legend.font != new->legend.font))
      

  {
    new->legend.need_refresh = True;
  }

  if (cur->legend.font != new->legend.font)
  {
    XSetFont (XtDisplay(new), new->legend.gc, new->legend.font->fid);
    do_recompute = True;
  }

  /* start or stop using a pixmap? */
  if (cur->legend.use_pixmap != new->legend.use_pixmap)
  {
    if (!new->legend.use_pixmap && cur->legend.pixmap)
      FreePixmap (new);
    new->legend.pixmap = 0;
    new->legend.need_refresh = True;
  }

  /* if the dimensions have changed, then we need to get a new
   * pixmap and redraw */
  if ((cur->core.width != new->core.width) ||
      (cur->core.height != new->core.height))
  {
    if (new->legend.pixmap) FreePixmap (new);
    do_recompute = True;
    new->legend.need_refresh = True;
  }

  if (do_recompute) CalculateDims (new);

  /* if we return True, then the X Toolkit will clear the widget
   * and invoke Redisplay.  We only want this to happen if we
   * aren't using a pixmap and the widget needs to be refreshed. */
  do_redisplay |= (new->legend.need_refresh && !new->legend.use_pixmap);
  if (new->legend.need_refresh && !do_redisplay)
    Redisplay ((Widget)new, 0, 0);
  
  return do_redisplay;
}


static void
Destroy         (Widget w)
{
  LegendWidget          cw = (LegendWidget)w;
  LegendItemInfo        *item, *tmp;

  XtReleaseGC ((Widget)cw, cw->legend.gc);
  FreePixmap (cw);

  item = cw->legend.items;
  while (item)
  {
    tmp = item;
    item = item->next;
    free (tmp);
  }
}


static void
Resize          (Widget w)
{
  LegendWidget cw = (LegendWidget)w;

  if (cw->legend.pixmap) FreePixmap (cw);
   cw->legend.need_refresh = 1;
}


static XtGeometryResult
QueryGeometry   (Widget                 w,
                 XtWidgetGeometry       *proposed,
                 XtWidgetGeometry       *answer)
{
  LegendWidget          cw = (LegendWidget)w;
  XtGeometryResult      response;
  XPoint                most, least;
  int                   mask;

  answer->request_mode = CWWidth | CWHeight;
  
  GetDimsForMask (cw, LGITEM_MASK_ALL, &most);
  GetDimsForMask (cw, LGITEM_MASK_NAME, &least);

  /* height takes precedence over width. */
  if (proposed->request_mode & CWHeight)
  {
    if (proposed->height > most.y)
    {
      answer->height = most.y;
      answer->width = most.x;
    }
    else if (proposed->height < least.y)
    {
      answer->height = least.y;
      answer->width = least.x;
    }
    else
    {
      mask = BestMaskForHeight (cw, LGITEM_MASK_ALL, answer->height);
      GetDimsForMask (cw, mask, &most);
      answer->width = most.x;
      answer->height = proposed->height;
    }
  }

  /* just width? */
  else if (proposed->request_mode & CWWidth)
  {
    mask = BestMaskForHeight (cw, LGITEM_MASK_ALL, cw->core.height);
    GetDimsForMask (cw, mask, &most);
    answer->width = most.x;
    answer->height = cw->core.height;
  }

  /* neither --let's give our opinion about good width and height */
  else
  {
    answer->width = MAX (most.x, DEFAULT_WIDTH);
    answer->height = MAX (most.y, DEFAULT_HEIGHT);
  }

  if ((proposed->request_mode & CWWidth) &&
      (proposed->request_mode & CWHeight) &&
      (proposed->width == answer->width) &&
      (proposed->height == answer->height))
    response = XtGeometryYes;

  else if ((answer->width == cw->core.width) &&
           (answer->height == cw->core.height))
    response = XtGeometryNo;

  else response = XtGeometryAlmost;

  return response;
}


/* ========================================================================
 *
 *         >>>>>>>>>>   P R I V A T E   F U N C T I O N S   <<<<<<<<<<
 *
 * ========================================================================
 */


/* GetPixmap
 */
static void
GetPixmap       (LegendWidget cw)
{
  if (!XtIsRealized ((Widget)cw)) return;

  cw->legend.pixmap = XCreatePixmap
    (XtDisplay (cw), XtWindow (cw),
     cw->core.width, cw->core.height, cw->core.depth);

  if (!cw->legend.pixmap)
    XtWarningMsg
      ("Resource Unavailable", "GetPixmap",
       "XtToolkitError", "Legend widget is unable to allocate server memory",
       (String *)0, (Cardinal *)0);
    
}


/* FreePixmap
 */
static void
FreePixmap      (LegendWidget cw)
{
  if (cw->legend.pixmap) XFreePixmap (XtDisplay (cw), cw->legend.pixmap);
  cw->legend.pixmap = 0;
}

/* Albert */

void LegendRefresh(LegendWidget cw)
{
  register int                  mask, m;
  register LegendItemInfo       *item;
  register int                  x, y;
  register char                 *s, *p;
  XPoint                        dims;
  XRectangle                    rect;
  Drawable                      canvas;
  
  /* which canvas? */
  if (cw->legend.use_pixmap)
  {
    if (!cw->legend.pixmap) GetPixmap (cw);
    canvas = cw->legend.pixmap;
    if (!canvas) canvas = XtWindow(cw);
  }
  else {
    canvas = XtWindow(cw);
  }
  
  XSetForeground (XtDisplay (cw), cw->legend.gc, cw->core.background_pixel);
  XFillRectangle
    (XtDisplay (cw), canvas, cw->legend.gc,
	0, 0, cw->core.width + 1, cw->core.height + 1);
  
  /* which elements to draw? */
  mask = BestMaskForHeight (cw, LGITEM_MASK_ALL, cw->core.height);
  GetDimsForMask (cw, mask, &dims);
  
  /* make the width equal to the widget with */
  dims.x = cw->core.width;
  
  /* for each legend item */
  x = y = 0;
  for (item = cw->legend.items; item; item = item->next)
  {
    /* remember legend entry top left coords */
    item->box.x = x; item->box.y = y;

    /* determine the location and dimensions of the text box */
    rect.width = 2 + item->extents[LGITEM_NAME].width;
    rect.height = 2 +
      item->extents[LGITEM_NAME].ascent + item->extents[LGITEM_NAME].descent;
    rect.x = x + (dims.x - rect.width) / 2;
    rect.y = y + COLOR_NPIXELS_TOP;

    /* draw the title area */
    XSetForeground (XtDisplay (cw), cw->legend.gc, item->color);
    XFillRectangle
      (XtDisplay (cw), canvas, cw->legend.gc,
       x, y,
       dims.x + 1, rect.height + COLOR_NPIXELS_TOP + COLOR_NPIXELS_BOTTOM + 1);

    /* clear inside the title area */
    XSetForeground (XtDisplay (cw), cw->legend.gc, cw->core.background_pixel);
    XFillRectangle
      (XtDisplay (cw), canvas, cw->legend.gc,
       x + COLOR_NPIXELS_SIDE, rect.y,
       dims.x + 1 - (2*COLOR_NPIXELS_SIDE), rect.height+1);

    /* draw the text */
    XSetForeground (XtDisplay (cw), cw->legend.gc, cw->primitive.foreground);
    for (p = s = item->info[LGITEM_NAME]; *p; p++);
    XDrawString
      (XtDisplay (cw), canvas, cw->legend.gc,
       rect.x+1 , rect.y+1 + item->extents[LGITEM_NAME].ascent, s, p-s);

    /* advance current vertical location, remeber item area */
    y += rect.height + COLOR_NPIXELS_TOP + COLOR_NPIXELS_BOTTOM;
    y += ITEM_INFO_PAD;

    /* draw the other portions of this item */
    for (m = LGITEM_NAME+1; m < NUM_LGITEMS; m++)
      if ((1 << m) & mask)
      {
        for (p = s = item->info[m]; *p; p++);
        if (p > s)
        {
          /* left hand indent: COLOR_NPIXELS_SIDE */
          XDrawString
            (XtDisplay (cw), canvas, cw->legend.gc,
             x + COLOR_NPIXELS_SIDE, y + item->extents[m].ascent, s, p-s);

          /* advance vertical position */
          y += item->extents[m].ascent + item->extents[m].descent;
          y += ITEM_INFO_PAD;
        }
      }
    
    item->box.width = dims.x;
    item->box.height = y - item->box.y;
  }

  /* copy the pixmap if used */
  if (canvas != XtWindow(cw))
  {
    XCopyArea
	(XtDisplay (cw), cw->legend.pixmap, XtWindow (cw), cw->legend.gc,
	  0, 0, cw->core.width, cw->core.height, 0, 0);
  }
  
  return;
}


/* Draw
 *
 *      Draws current widget state into supplied drawable
 */
static void
Draw            (LegendWidget cw, Drawable canvas)
{
  register int                  mask, m;
  register LegendItemInfo       *item;
  register int                  x, y;
  register char                 *s, *p;
  XPoint                        dims;
  XRectangle                    rect;

  /* clear the drawable */
  XSetForeground (XtDisplay (cw), cw->legend.gc, cw->core.background_pixel);
  XFillRectangle
    (XtDisplay (cw), canvas, cw->legend.gc,
     0, 0, cw->core.width + 1, cw->core.height + 1);

  /* which elements to draw? */
  mask = BestMaskForHeight (cw, LGITEM_MASK_ALL, cw->core.height);
  GetDimsForMask (cw, mask, &dims);

  /* make the width equal to the widget with */
  dims.x = cw->core.width;

  /* for each legend item */
  x = y = 0;
  for (item = cw->legend.items; item; item = item->next)
  {
    /* remember legend entry top left coords */
    item->box.x = x; item->box.y = y;

    /* determine the location and dimensions of the text box */
    rect.width = 2 + item->extents[LGITEM_NAME].width;
    rect.height = 2 +
      item->extents[LGITEM_NAME].ascent + item->extents[LGITEM_NAME].descent;
    rect.x = x + (dims.x - rect.width) / 2;
    rect.y = y + COLOR_NPIXELS_TOP;

    /* draw the title area */
    XSetForeground (XtDisplay (cw), cw->legend.gc, item->color);
    XFillRectangle
      (XtDisplay (cw), canvas, cw->legend.gc,
       x, y,
       dims.x + 1, rect.height + COLOR_NPIXELS_TOP + COLOR_NPIXELS_BOTTOM + 1);

    /* clear inside the title area */
    XSetForeground (XtDisplay (cw), cw->legend.gc, cw->core.background_pixel);
    XFillRectangle
      (XtDisplay (cw), canvas, cw->legend.gc,
       x + COLOR_NPIXELS_SIDE, rect.y,
       dims.x + 1 - (2*COLOR_NPIXELS_SIDE), rect.height+1);

    /* draw the text */
    XSetForeground (XtDisplay (cw), cw->legend.gc, cw->primitive.foreground);
    for (p = s = item->info[LGITEM_NAME]; *p; p++);
    XDrawString
      (XtDisplay (cw), canvas, cw->legend.gc,
       rect.x+1 , rect.y+1 + item->extents[LGITEM_NAME].ascent, s, p-s);

    /* advance current vertical location, remeber item area */
    y += rect.height + COLOR_NPIXELS_TOP + COLOR_NPIXELS_BOTTOM;
    y += ITEM_INFO_PAD;

    /* draw the other portions of this item */
    for (m = LGITEM_NAME+1; m < NUM_LGITEMS; m++)
      if ((1 << m) & mask)
      {
        for (p = s = item->info[m]; *p; p++);
        if (p > s)
        {
          /* left hand indent: COLOR_NPIXELS_SIDE */
          XDrawString
            (XtDisplay (cw), canvas, cw->legend.gc,
             x + COLOR_NPIXELS_SIDE, y + item->extents[m].ascent, s, p-s);

          /* advance vertical position */
          y += item->extents[m].ascent + item->extents[m].descent;
          y += ITEM_INFO_PAD;
        }
      }
    
    item->box.width = dims.x;
    item->box.height = y - item->box.y;
  }
}


/* CalculateDims
 *
 *      Calculates the dimensions needed to display each of the legend
 *      item information components.
 */
static void
CalculateDims   (LegendWidget cw)
{
  register LegendItemInfo       *item;
  XCharStruct                   overall;
  register int                  i;
  int                                 direction, font_ascent, font_descent;
  register char                 *p, *s;

  /* zero out the info dimensions */
  for (i = 0; i < NUM_LGITEMS; i++)
    cw->legend.dims[i].x = cw->legend.dims[i].y = 0;

  /* for each item in legend */
  for (item = cw->legend.items; item; item = item->next)
  {
    for (i = 0; i < NUM_LGITEMS; i++)
    {
      s = item->info[i];
      for (p = s; *p; p++);
      
      /* if info item for entry is not empty, then get its text extents */
      if (p > s)
      {
        XTextExtents
          (cw->legend.font, s, p - s,
           &direction, &font_ascent, &font_descent, &overall);

        /* remember text extents */
        item->extents[i] = overall;

        /* if this is the name, we need to also include pixels for
         * the color info.  We do this by first allowing for one
         * pixel's width buffer around the bounding box, adding
         * COLOR_NPIXELS_TOP + COLOR_NPIXELS_BOTTOM to the height
         * and 2 * COLOR_NPIXELS_SIDE to the width. */
        if (i == LGITEM_NAME)
        {
          overall.ascent += 2 + COLOR_NPIXELS_TOP + COLOR_NPIXELS_BOTTOM;
          overall.width += 2 + (2 * COLOR_NPIXELS_SIDE);
        }

        /* otherwise, be sure to include the left hand indent */
        else overall.width += COLOR_NPIXELS_SIDE;

        cw->legend.dims[i].y += overall.ascent + overall.descent;
        cw->legend.dims[i].x = MAX (overall.width, cw->legend.dims[i].x);

        /* every info item is offset from the previous by ITEM_INFO_PAD
         * pixels */
        cw->legend.dims[i].y += ITEM_INFO_PAD;
      }
    }
  }

  /* if at least one item, subtract ITEM_INFO_PAD from total height, because
   * first item is not offset */
  if (cw->legend.dims[LGITEM_NAME].y > 0)
    cw->legend.dims[LGITEM_NAME].y -= ITEM_INFO_PAD;
}


/* GetDimsForMask
 *
 *      Calculates the minimum number of pixels needed to draw the
 *      legend with the items specified by mask.  Regardless of wether
 *      or not it is included in mask, LGITEM_NAME is always
 *      considered in this computation.  The generated width and
 *      height are stored in the supplied XPoint structure.  Width
 *      as x, height as y.
 */
static void
GetDimsForMask          (LegendWidget           cw,
                         register int           mask,
                         register XPoint        *dims)
{
  register int  i;

  dims->x = dims->y = 0;
  
  mask |= LGITEM_NAME;
  for (i = 0; i < NUM_LGITEMS; i++)
  {
    if (mask & (1 << i))
    {
      dims->y += cw->legend.dims[i].y;
      dims->x = MAX (dims->x, cw->legend.dims[i].x);
    }
  }
}


/* BestMaskForHeight
 *
 *      Iteratively subtracts items from the given mask in order of
 *      increasing importance, until the generated legend will lose
 *      the least amount of important information when rendered with
 *      the specified height.  The resulting mask is returned.
 */
static int
BestMaskForHeight       (LegendWidget   cw,
                         register int   mask,
                         register int   height)
{
  register int  i;
  XPoint        dims;
  
  mask  |= LGITEM_NAME;
  i = 1 << (NUM_LGITEMS - 1);

  GetDimsForMask (cw, mask, &dims);
  while ((mask != LGITEM_MASK_NAME) && (dims.y > height))
  {
    mask &= ~i;
    i >>= 1;
    GetDimsForMask (cw, mask, &dims);
  }

  return mask;
}


/* ========================================================================
 *
 *        >>>>>>>>>>  H E L P E R    F U N C T I O N S  <<<<<<<<<<
 *
 * ========================================================================
 */



/* ========================================================================
 *
 *                   >>>>>>>>>>  A C T I O N S  <<<<<<<<<<
 *
 * ========================================================================
 */

/* Arm
 */
static void
Arm             (Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  LegendWidget                  cw = (LegendWidget)w;
  LegendItemInfo                *item;
  XjLegendCallbackStruct        call_data;

  /* determine which item owns this location */
  for (item = cw->legend.items; item; item = item->next)
    if ((event->xbutton.x >= item->box.x) &&
        (event->xbutton.x < (item->box.x + item->box.width)) &&
        (event->xbutton.y >= item->box.y) &&
        (event->xbutton.y < (item->box.y + item->box.height)))
    {
      if (cw->legend.choose_callback)
      {
        call_data.reason = XmCR_ACTIVATE;
        call_data.event = event;
        call_data.item = (LegendItem)item;
        XtCallCallbackList
          ((Widget)cw, cw->legend.choose_callback, (XtPointer) &call_data);
      }
      break;
    }
}


/* ========================================================================
 *
 *                >>>>>>>>>>  C O N V E R T E R S  <<<<<<<<<<
 *
 * ========================================================================
 */



/* ========================================================================
 *
 *  >>>>>>>>>>  P U B L I C   U T I L I T Y  F U N C T I O N S  <<<<<<<<<<
 *
 * ========================================================================
 */
LegendItem
XjLegendNewItem         (Widget w,
                         char   *name,
                         char   *units,
                         char   *range,
                         char   *comment,
                         Pixel  color)
{       
  LegendWidget          cw = (LegendWidget)w;
  LegendItemInfo        *item, *p;

  if ((item = (LegendItemInfo *)malloc (sizeof (LegendItemInfo))))
  {
    /* append to list */
    if (cw->legend.items)
    {
      /* find end of list */
      for (p = cw->legend.items; p->next; p = p->next);
      
      /* append */
      p->next = item;
      item->prev = p;
      item->next = 0;
    }
    else
    {
      cw->legend.items = item;
      item->prev = item->next = 0;
    }

    XjLegendUpdateItem
      ((Widget)cw, (LegendItem)item, name, units, range, comment, color);
  }

  return (LegendItem)item;
}


void
XjLegendDeleteItem      (Widget w, LegendItem the_item)
{
  LegendWidget          cw = (LegendWidget)w;
  LegendItemInfo        *item = (LegendItemInfo *)the_item;

  if (item->next) item->next->prev = item->prev;
  if (item->prev) item->prev->next = item->next;

  if (cw->legend.items == item) cw->legend.items = item->next;

  free (item);
  cw->legend.need_refresh = True;
  XjLegendResize (w);
}


void
XjLegendUpdateItem      (Widget         w,
                         LegendItem     the_item,
                         char           *name,
                         char           *units,
                         char           *range,
                         char           *comment,
                         Pixel          color)
{
  LegendWidget          cw = (LegendWidget)w;
  LegendItemInfo        *item = (LegendItemInfo *)the_item;
  char                  *p, *s;
  
/* copy args */
  p = s = item->info[LGITEM_NAME];
  if (name) while (*name && ((p-s) < LEGEND_MAX_STRLEN-1)) *p++ = *name++;
  *p = 0;
  
  p = s = item->info[LGITEM_RANGE];
  if (range) while (*range && ((p-s) < LEGEND_MAX_STRLEN-1)) *p++ = *range++;
  *p = 0;
  
  p = s = item->info[LGITEM_UNITS];
  if (units) while (*units && ((p-s) < LEGEND_MAX_STRLEN-1)) *p++ = *units++;
  *p = 0;
  
  p = s = item->info[LGITEM_COMMENT];
  if (comment) while (*comment && ((p-s) < LEGEND_MAX_STRLEN-1))
    *p++ = *comment++;
  *p = 0;
  
  item->color = color;
  cw->legend.need_refresh = True;
  XjLegendResize (w);
}

/* Albert */
void
XjLegendValueUpdateItem    (Widget         w,
                         LegendItem     the_item,
                         char           *nameStart,
                         char           *units,
                         char           *range,
                         char           *comment,
                         Pixel          color)
{
  LegendItemInfo        *item = (LegendItemInfo *)the_item;
  char                  *p, *s;

  static char nameL[128]; /* Albert */
  char * name= nameStart;
  memset(nameL,0,128);
  memset(nameL,' ',40);
  if(strlen(nameStart)<40) {
  strncpy(nameL,nameStart,strlen(nameStart));
  name = (char *) nameL;
  }

  /* copy args */
  p = s = item->info[LGITEM_NAME];
  if (name) while (*name && ((p-s) < LEGEND_MAX_STRLEN-1)) *p++ = *name++;
  *p = 0;
  
  p = s = item->info[LGITEM_RANGE];
  if (range) while (*range && ((p-s) < LEGEND_MAX_STRLEN-1)) *p++ = *range++;
  *p = 0;
  
  p = s = item->info[LGITEM_UNITS];
  if (units) while (*units && ((p-s) < LEGEND_MAX_STRLEN-1)) *p++ = *units++;
  *p = 0;
  
  p = s = item->info[LGITEM_COMMENT];
  if (comment) while (*comment && ((p-s) < LEGEND_MAX_STRLEN-1))
    *p++ = *comment++;
  *p = 0;
  
  item->color = color;
  /*cw->legend.need_refresh = True; perror Albert */
}


/* *************************************************   */

/* *************************************************   */



void
XjLegendUpdate          (Widget w)
{
  LegendWidget  cw = (LegendWidget)w;

  cw->legend.need_refresh = True;
  CalculateDims (cw);
  Redisplay ((Widget)cw, 0, 0);
}


void
XjLegendResize  (Widget w)
{
  LegendWidget  cw = (LegendWidget)w;
  XPoint        dims;
  Dimension     h_req, w_req;

  CalculateDims (cw);
  GetDimsForMask (cw, LGITEM_MASK_ALL, &dims);

  w_req = MAX (dims.x, cw->legend.min_width);
  /* insure it doesn't request an invalid value for the height */
  h_req = MAX (dims.y, ITEM_INFO_PAD);

  if ((w_req != cw->core.width) || (h_req != cw->core.height))
    XtVaSetValues (w, XmNwidth, w_req, XmNheight, h_req, NULL);
}

/* **************************** Emacs Editing Sequences ***************** */
/* Local Variables: */
/* tab-width: 6 */
/* c-basic-offset: 2 */
/* c-comment-only-line-offset: 0 */
/* c-indent-comments-syntactically-p: t */
/* c-label-minimum-indentation: 1 */
/* c-file-offsets: ((substatement-open . 0) (label . 2) */
/* (brace-entry-open . 0) (label .2) (arglist-intro . +) */
/* (arglist-cont-nonempty . c-lineup-arglist) ) */
/* End: */
