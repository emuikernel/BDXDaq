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

#ifndef _StripDataSource
#define _StripDataSource

#include "StripCurve.h"
#include "StripHistory.h"


/* ======= Data Types ======= */

typedef short   StatusType;

typedef struct          _DataPoint
{
  struct timeval        t;
  double                v;
  StatusType            s;
} DataPoint;

typedef struct          _RenderBuffer
{
  XSegment              *segs;
  int                   max_segs;
  int                   n_segs;
} RenderBuffer;

typedef struct          _CurveData
{
  StripCurveInfo        *curve;

  /* === ring buffers === */
  size_t                first;  /* index of first live data point */
  double                *val;
  StatusType            *stat;

  /* === rendered data info === */
  Boolean               connectable;    /* can new data be connected to old? */
  DataPoint             endpoints[2];   /* from most recent render */
  struct timeval        extents[2];     /* extent of *visible* rendered data */

  /* === history buffer === */
  StripHistoryResult    history;
  size_t                hidx_t0, hidx_t1;
} CurveData;

typedef struct          _StripDataSourceInfo
{
  StripHistory          history;
  CurveData             buffers[STRIP_MAX_CURVES];

  /* ring buffer of sample times */
  size_t                buf_size;
  size_t                cur_idx;
  size_t                count;
  struct timeval        *times;

  /* info for currently initialized time range */
  size_t                idx_t0, idx_t1;
  struct timeval        req_t0, req_t1;
  double                bin_size;
  int                   n_bins;
}
StripDataSourceInfo;

typedef void *  StripDataSource;

typedef enum
{
  SDS_REFRESH_ALL, SDS_JOIN_NEW
}
sdsRenderTechnique;

/* sdsTransform
 *
 *      Function to transform a set of real values into a set
 *      of plottable values.
 *
 *      The first parameter references some client-specific
 *      data used to compute the transform.  The second and
 *      third parameters are, respectively, the input and
 *      output arrays.  The transform routine must guarantee
 *      that these can both point to the same memory without
 *      ill effects (allowing an in-place transformation).
 *      The last parameter indicates the length of the arrays.
 */
typedef void    (*sdsTransform) (void *,        /* transform data */
                                 double *,      /* before */
                                 double *,      /* after */
                                 int);          /* num points */

typedef enum
{
  DATASTAT_PLOTABLE     = 1     /* the point is plotable */
} DataStatus;

/* ======= Attributes ======= */
typedef enum
{
  SDS_NUMSAMPLES = 1,   /* (size_t)     number of samples to keep       rw */
  SDS_BEGIN_TIME = 2,   /* (struct timeval *) */
  SDS_LAST_ATTRIBUTE
} SDSAttribute;



/* ======= Functions ======= */

/*
 * StripDataSource_init
 *
 *      Creates a new strip data structure, setting all values to defaults.
 */
StripDataSource         StripDataSource_init    (StripHistory);


/*
 * StripDataSource_delete
 *
 *      Destroys the specified data buffer.
 */
void    StripDataSource_delete  (StripDataSource);


/*
 * StripDataSource_set/getattr
 *
 *      Sets or gets the specified attribute, returning true on success.
 */
int     StripDataSource_setattr (StripDataSource, ...);
int     StripDataSource_getattr (StripDataSource, ...);


/*
 * StripDataSource_addcurve
 *
 *      Tells the DataSource to acquire data for the given curve whenever
 *      a sample is requested.
 */
int     StripDataSource_addcurve        (StripDataSource, StripCurve);


/*
 * StripDataSource_removecurve
 *
 *      Removes the given curve from those the DataSource knows.
 */
int     StripDataSource_removecurve     (StripDataSource, StripCurve);


/*
 * StripDataSource_sample
 *
 *      Tells the buffer to sample the data for all curves it knows about.
 */
void    StripDataSource_sample  (StripDataSource, char *); /* Albert */


/*
 * StripDataSource_init_range
 *
 *      Initializes DataSource for subsequent retrievals.  After this routine
 *      is called, and until it is called again, all requests for data or
 *      time stamps will only return data inside the range specified here.
 *      (The endpoints are included).  Returns true iff some data is available
 *      for plotting.
 *
 *      technique:              refresh all, join new
 *
 *      This specifies the technique to be used in subsequent render calls.
 */
int     StripDataSource_init_range      (StripDataSource,
                                         struct timeval *,      /* begin */
                                         double,                /* bin size */
                                         int,                   /* n bins */
                                         sdsRenderTechnique);
 

/* StripDataSource_render
 *
 *      Bins the data on the current range for the specified curve, given
 *      transform functions for both the x and y axes.
 *
 *      The resulting data is stored as a series of line segments,
 *      the starting address of which will be written into the supplied
 *      pointer location.  The number of generated segments is returned.
 *
 *      Note that the referenced XSegment array is a static buffer,
 *      so its contents are only good until the next call to render(),
 *      at which point they will be overwritten.
 *
 *      If the prevailing technique (as specified in previous call to
 *      init_range()) is JOIN_NEW, then only that data which has
 *      accumulated since the last call will be rendered, and it
 *      will be joined to the previous endpoints if appropriate.
 *
 *      In order to accomplish this, the endpoints from the resulting
 *      (joined) range are remembered at the end of the routine.
 *
 *      Assumes init_range() has already been called.
 */
#ifndef NO_X11_HERE /* Albert */
size_t  StripDataSource_render  (StripDataSource,
                                 StripCurve,
                                 sdsTransform,          /* x transform */
                                 void *,                /* x transform data */
                                 sdsTransform,          /* y transform */
                                 void *,                /* y transform data */
                                 XSegment **);          /* result */

#endif /* Albert */
/*
 * StripDataSource_dump
 *
 *      Causes all ring buffer data for the current range to be dumped out to
 *      the specified file.
 */
int     StripDataSource_dump            (StripDataSource, FILE *,char *sgi);/* Albert */

/*
 * StripDataSource_dump_csv
 *
 *      Causes all ring buffer data for the current range to be dumped out to
 *      the specified comma separated values file.
 */
int     StripDataSource_dump_csv        (StripDataSource, FILE *,char *sgi);

int   Strip_auto_scale     (Strip the_strip);  /* Albert */

int   StripDataSource_removecurveAll(StripDataSource the_sds);

void  StripDataSource_refresh (StripDataSource        the_sds);

int StripDataSource_min_max (StripDataSourceInfo *sds, struct timeval h0,
  struct timeval h_end);

#ifdef USE_SDDS
/*
 * StripDataSource_dump_sdds
 *
 *      Causes all ring buffer data for the current range to be output in SDDS 
 *      format to the specified filename.
 */
int     StripDataSource_dump_sdds            (StripDataSource, char *);
#endif /* SDDS */

#endif  /* #ifndef _StripDataSource */
