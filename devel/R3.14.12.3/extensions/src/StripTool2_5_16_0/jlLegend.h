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

#ifndef _jlLegend
#define _jlLegend

#include <Xm/Xm.h>

/* LegendItem
 *
 *      A legend entry handle.
 */
typedef void *  LegendItem;

/* ====== New resource names ======
 */
#define XjNusePixmap            "usePixmap"
#define XjNchooseCallback       "chooseCallback"


/* ====== New resource classes ======
 */
#define XjCUsePixmap            "UsePixmap"
#define XjCChooseCallback       "ChooseCallback"


/* ====== New resource types ======
 */


/* ====== Callback stuff ======
 */
typedef struct _XjLegendCallbackStruct
{
  int           reason;
  XEvent        *event;
  LegendItem    item;
} XjLegendCallbackStruct;
 

/* ====== Class record constants ======
 */
extern WidgetClass                      legendWidgetClass;

typedef struct _LegendClassRec *        LegendWidgetClass;
typedef struct _LegendRec *             LegendWidget;


/* ====== some useful public functions ====== */

/* XjLegendNewItem
 *
 *      Create a new item with the specified info.  Returns (LegendItem)0
 *      on failure.
 */
LegendItem      XjLegendNewItem         (Widget,
                                         char *,        /* name */
                                         char *,        /* units */
                                         char *,        /* range */
                                         char *,        /* comment */
                                         Pixel);        /* color */

/* XjLegendDeleteItem
 *
 *      Removes the specified entry from the legend.
 */
void            XjLegendDeleteItem      (Widget, LegendItem);


/* XjLegendUpdateItem
 *
 *      Updates the legend entry for the given legend item with
 *      the specified parameters.
 */
void            XjLegendUpdateItem      (Widget,
                                         LegendItem,
                                         char *,
                                         char *,
                                         char *,
                                         char *,
                                         Pixel);


/* XjLegendResize
 *
 *      Attempts to resize the widget to its desired dimensions.
 *      If successful, it will be redrawn.
 */
void            XjLegendResize  (Widget w);


/* XjLegendUpdate
 *
 *      Causes the widget to be re-calculated and re-rendered.
 */
void            XjLegendUpdate          (Widget w);


void

/* Should put description here */
XjLegendValueUpdateItem    (Widget         w,
                         LegendItem     the_item,
                         char           *nameStart,
                         char           *units,
                         char           *range,
                         char           *comment,
                         Pixel          color);

/* Should put description here */
void LegendRefresh(LegendWidget cw);

#endif  /* #ifndef _jlLegend */
