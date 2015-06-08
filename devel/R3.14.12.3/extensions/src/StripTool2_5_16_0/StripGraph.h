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

#ifndef _StripGraph
#define _StripGraph

#include "StripConfig.h"
#include "StripDataSource.h"
#include "jlAxis.h"



/* ======= Data Types ======= */
typedef void *  StripGraph;


typedef enum
{
  SGCOMP_XAXIS = 0,
  SGCOMP_YAXIS,
  SGCOMP_LEGEND,
  SGCOMP_LEGEND_DATA, /* Albert */
  SGCOMP_DATA,
  SGCOMP_TITLE,
  SGCOMP_GRID,
  SGCOMP_COUNT
}
SGComponent;

typedef enum
{
  SGCOMPMASK_XAXIS              = (1 << SGCOMP_XAXIS),
  SGCOMPMASK_YAXIS              = (1 << SGCOMP_YAXIS),
  SGCOMPMASK_LEGEND             = (1 << SGCOMP_LEGEND),
  SGCOMPMASK_LEGEND_DATA        = (1 << SGCOMP_LEGEND_DATA), /* Albert */
  SGCOMPMASK_DATA               = (1 << SGCOMP_DATA),
  SGCOMPMASK_TITLE              = (1 << SGCOMP_TITLE),
  SGCOMPMASK_GRID               = (1 << SGCOMP_GRID)
}
SGComponentMask;

#define SGCOMPMASK_ALL \
(SGCOMPMASK_XAXIS       |\
 SGCOMPMASK_YAXIS       |\
 SGCOMPMASK_LEGEND      |\
 SGCOMPMASK_DATA        |\
 SGCOMPMASK_TITLE       |\
 SGCOMPMASK_GRID)


/* ======= Status bits ======= */
typedef enum
{
  SGSTAT_GRAPH_REFRESH  = (1 << 0),     /* do not scroll graph: replot */
  SGSTAT_LEGEND_REFRESH = (1 << 1)      /* recalculate the legend */
} StripGraphStatus;

/* ======= Attributes ======= */
typedef enum
{
  STRIPGRAPH_WIDGET = 1,        /* (Widget)  window in which to draw    rw */
  STRIPGRAPH_HEADING,           /* (char *)  graph title                rw */
  STRIPGRAPH_DATA_SOURCE,       /* (StripDataSource)                    rw */
  STRIPGRAPH_BEGIN_TIME,        /* (struct timeval *)                   rw */
  STRIPGRAPH_END_TIME,          /* (struct timeval *)                   rw */
  STRIPGRAPH_USER_DATA,         /* (void *)  miscellaneous client data  rw */
  STRIPGRAPH_ANNOTATION_INFO,   /* (void *)  miscellaneous client data  rw */
  STRIPGRAPH_SELECTED_CURVE,    /* (StripCurveInfo *)                   rw */
  STRIPGRAPH_LAST_ATTRIBUTE
} StripGraphAttribute;




/* ======= Functions ======= */
/*
 * StripGraph_init
 *
 *      Creates a new StripGraph data structure, then creates
 *      all necessary widgets as children of the given XmForm
 *      manager.
 */
StripGraph      StripGraph_init         (Widget, StripConfig *);


/*
 * StripGraph_delete
 *
 *      Destroys the specified StripGraph, and all of its associated
 *      widgets.
 */
void    StripGraph_delete       (StripGraph);


/*
 * Strip_set/getattr
 *
 *      Sets or gets the specified attribute, returning true on success.
 */
int     StripGraph_setattr      (StripGraph, ...);
int     StripGraph_getattr      (StripGraph, ...);


/*
 * StripGraph_addcurve
 *
 *      Tells the Graph to plot the given curve.
 */
int     StripGraph_addcurve     (StripGraph, StripCurve);


/*
 * StripGraph_removecurve
 *
 *      Removes the given curve from those the Graph plots.
 */
int     StripGraph_removecurve  (StripGraph, StripCurve);


/*
 * StripGraph_draw
 *
 *      Draws the components of the strip chart specified by the mask.  If
 *      the region is non-null, then output is restricted to that region
 *      of the window.
 */
void    StripGraph_draw         (StripGraph,
                                 unsigned,      /* mask */
                                 Region *);


/*
 * StripGraph_dumpdata
 *
 *      Causes all data for the curves on the current time range, to be
 *      dumped out to the specified file.
 */
int     StripGraph_dumpdata     (StripGraph, FILE *);


/*
 * StripGraph_dumpdata_csv
 *
 *      Causes all data for the curves on the current time range, to be
 *      dumped out to the specified comma separated values file.
 */
int     StripGraph_dumpdata_csv     (StripGraph, FILE *);


#ifdef USE_SDDS
/*
 * StripGraph_dumpdata_sdds
 *
 *      Causes all data for the curves on the current time range, to be
 *      dumped out ind SDDS format to the specified filename.
 */
int     StripGraph_dumpdata_sdds     (StripGraph, char *);
#endif


/*
 * StripGraph_set/get/clearstat
 *
 *      Set:    sets the specified status bit high.
 *      Get:    returns true iff the specified status bit is high.
 *      Clear:  sets the specified status bit low.
 */
unsigned        StripGraph_setstat      (StripGraph, unsigned);
unsigned        StripGraph_getstat      (StripGraph, unsigned);
unsigned        StripGraph_clearstat    (StripGraph, unsigned);
 
int StripAuto_min_max (StripDataSource sds, char *sgi) ; /* Albert */
void CurveLegendRefresh(StripCurveInfo *c, StripGraph sg, double a);  /* Albert */
#endif


jlaTransformInfo* StripGraph_getTransform(StripGraph sgi, StripCurveInfo *curve);
