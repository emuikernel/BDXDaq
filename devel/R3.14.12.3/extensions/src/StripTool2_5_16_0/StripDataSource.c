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

/* Notes
 *
 *      This module performs several functions:
 *
 *      (1) acquires real-time data and stores it in a ring
 *          buffer.
 *      (2) passes off requests for older data to the archive
 *          service.
 *      (3) on a per-curve basis, renders data into XSegments,
 *          given the appropriate transformation parameters.
 *
 *      So, how does it work?  Basically, the user must first
 *      call init_range() in order to specify what data will
 *      be rendered.  This routine takes a begin time, bin size,
 *      and number of bins.  With this information, it first
 *      scans through the ring buffer to determine the indexes
 *      of the samples corresponding to the endpoints lying
 *      within the requested range, and saves this in the
 *      DataSource structure.  Next, each curve is scanned
 *      in order to determine whether it contains any data
 *      on the requested range.  If not, and if a request has
 *      not already been sent to the archive service, one
 *      such request is issued, and a status variable is toggled
 *      indicating that a history request for the given time
 *      is outstanding.
 *
 *      Once the range has been initialized, render() may be
 *      called to generate XSegment structures describing the
 *      rasterized data on the time range.  If only new data
 *      is requested, then all data greater than the global
 *      start time and less than the already-rendered start
 *      time are built into segments, and connected to the
 *      end point of the already rendered data.  This is
 *      accomplished by first checking the buffer for older
 *      data, and then the history result structure.  Similarly,
 *      the rendered endpoint is compared with the global end
 *      time.  If the latter is greater, then the newer points
 *      are built into segments and connected to the endpoint.
 *
 *      In order to facilitate this building of segments, the
 *      routine called "segmentify()" takes, among other parameters,
 *      time, value, and status pointers which indicate the location
 *      in memory of some contiguous array of sequential samples.
 *      Depending on the specified direction parameter, these pointers
 *      are either incremented or decremented until either the maximum
 *      number of points have been processed or the referenced time
 *      falls past the specified stop_time (meaning "before" in the case
 *      of a backward direction, or "after" in the case of a forward
 *      direction).  The caller may also specify connecting endpoints
 *      to which the generated line segments must be attached.
 */     

#define DEBUG1 0
#define DEBUG_SDS_TIMES 0
#define DEBUG_SDDS 0

#ifdef USE_SDDS
#include "SDDS.h"
#endif

#include "StripDataSource.h"
#include "StripDefines.h"
#include "StripMisc.h"
#include "StripGraph.h" /* Albert */

#include <X11/cursorfont.h>
extern Widget history_topShell;
extern int auto_scaleTriger;
extern long radioChange;
static Cursor cursor = 0; 


#ifndef SIZE_MAX
#  define SIZE_MAX      ((size_t)-1)
#endif

#define CURVE_DATA(C)   ((CurveData *)((StripCurveInfo *)C)->id)

#define SDS_LTE                 0
#define SDS_GTE                 1

#define SDS_DUMP_FIELDWIDTH     33 /* Albert -- was 30 */
#define SDS_DUMP_NUMWIDTH       23 /* Albert -- was 20 */
#define SDS_DUMP_BADVALUESTR    "BadVal"

#define SDS_BUFFERED_DATA       (1 << 0)
#define SDS_HISTORY_DATA        (1 << 1)
#define SDS_BOTH_DATA           (SDS_BUFFERED_DATA | SDS_HISTORY_DATA)


#define DUMP_SDDS_TIME_COL           "Time"
#define DUMP_SDDS_TIME_COL_UNITS     "seconds"
#define DUMP_SDDS_CONTENTS           "StripTool data"
#define DUMP_SDDS_DESCRIPTION        ""

static RenderBuffer     render_buffer = {0, 0, 0};

/* These are used as parameter types for segmentify() */
typedef struct          _TimeBuffer
{
  struct timeval        *base;
  struct timeval        *ptr;
  size_t                count;
} TimeBuffer;

typedef struct          _ValueBuffer
{
  double                *base;
  double                *ptr;
  size_t                count;
} ValueBuffer;

typedef struct          _StatusBuffer
{
  StatusType            *base;
  StatusType            *ptr;
  size_t                count;
} StatusBuffer;

typedef enum _SegmentifyDirection
{
  SDS_INCREASING, SDS_DECREASING
}
SegmentifyDirection;

static size_t   segmentify      (StripDataSourceInfo *,
  RenderBuffer *,
  SegmentifyDirection,
  TimeBuffer *,
  ValueBuffer *,
  StatusBuffer *,
  int,
  struct timeval *,
  DataPoint *,
  DataPoint *,
  DataPoint *,
  DataPoint *,
  sdsTransform,
  void *,
  sdsTransform,
  void *);

static int      resize          (StripDataSourceInfo    *sds,
  size_t                 buf_size);



static long     find_date_idx   (struct timeval         *t,
  struct timeval         *times,
  size_t                 n_times,
  size_t                 max_times,
  size_t                 idx_latest,
  int                    mode);

static int      pack_array      (void **, size_t,
  int, int, int,
  int, int *, int *);

static int      verify_render_buffer    (RenderBuffer   *, int);

static int printData(struct timeval *t,CurveData *c,char *v); /*Albert */
static int findNextTime(struct timeval *tv,struct timeval *res,StripDataSourceInfo *s) ; /*Albert */

/*
 * StripDataSource_init
 */
StripDataSource
StripDataSource_init    (StripHistory history)
{
  StripDataSourceInfo   *sds;

  /* allocate and zero the DataSource structure */
#if DEBUG_SDS_TIMES
  printf("StripDataSource_init: malloc StripDataSourceInfo\n");
#endif
  if ((sds = (StripDataSourceInfo *)malloc (sizeof(StripDataSourceInfo))))
  {
    sds->history        = history;
    sds->buf_size       = 0;
    sds->cur_idx        = 0;
    sds->count          = 0;
    sds->times          = 0;
    sds->idx_t0         = 0;
    sds->idx_t1         = 0;
    sds->bin_size       = 0;
    sds->n_bins         = 0;

    /* clear the buffers */
    memset (sds->buffers, 0, STRIP_MAX_CURVES * sizeof(CurveData));
  }

  return sds;
}



/*
 * StripDataSource_delete
 */
void
StripDataSource_delete  (StripDataSource the_sds)
{
  StripDataSourceInfo   *sds = (StripDataSourceInfo *)the_sds;
  int                   i;

  for (i = 0; i < STRIP_MAX_CURVES; i++)
  {
    if (sds->buffers[i].val)
      free (sds->buffers[i].val);
    if (sds->buffers[i].stat)
      free (sds->buffers[i].stat);
  }

  free (sds);
}



/*
 * StripDataSource_setattr
 */
int
StripDataSource_setattr (StripDataSource the_sds, ...)
{
  va_list               ap;
  StripDataSourceInfo   *sds = (StripDataSourceInfo *)the_sds;
  int                   attrib;
  size_t                tmp;
  int                   ret_val = 1;

  
  va_start (ap, the_sds);
  for (attrib = va_arg (ap, SDSAttribute);
       (attrib != 0);
       attrib = va_arg (ap, SDSAttribute))
  {
    if ((ret_val = ((attrib > 0) && (attrib < SDS_LAST_ATTRIBUTE))))
      switch (attrib)
      {
	case SDS_NUMSAMPLES:
	  tmp = va_arg (ap, size_t);
#if DEBUG_SDS_TIMES
	  printf("StripDataSource_setattr: tmp=%u buf_size=%u resize=%s\n",
	    tmp,sds->buf_size,tmp>sds->buf_size?"True":"False");
#endif
	  if (tmp > sds->buf_size) resize (sds, tmp);
#if DEBUG_SDS_TIMES
	  printf("StripDataSource_setattr: After resize buf_size=%u\n",
	    sds->buf_size);
#endif
	  break;
      }
  }

  va_end (ap);
  return ret_val;
}



/*
 * StripDataSource_getattr
 */
int
StripDataSource_getattr (StripDataSource the_sds, ...)
{
  va_list               ap;
  StripDataSourceInfo   *sds = (StripDataSourceInfo *)the_sds;
  int                   attrib;
  int                   ret_val = 1;
  size_t                index;

  
  va_start (ap, the_sds);
  for (attrib = va_arg (ap, SDSAttribute);
       (attrib != 0);
       attrib = va_arg (ap, SDSAttribute))
  {
    if ((ret_val = ((attrib > 0) && (attrib < SDS_LAST_ATTRIBUTE))))
      switch (attrib)
      {
	case SDS_NUMSAMPLES:
	  *(va_arg (ap, size_t *)) = sds->buf_size;
	  break;

	case SDS_BEGIN_TIME:
	  if (sds->count == sds->buf_size) index = (sds->cur_idx + 1) % sds->buf_size; 
	  else index = 1;
	  *(va_arg (ap, struct timeval *)) = sds->times[index];
	  break;

      }
  }

  va_end (ap);
  return ret_val;
}



/*
 * StripDataSource_addcurve
 */
int
StripDataSource_addcurve        (StripDataSource        the_sds,
  StripCurve             the_curve)
{
  StripDataSourceInfo   *sds = (StripDataSourceInfo *)the_sds;
  int                   i;
  int                   ret = 0;
  
  for (i = 0; i < STRIP_MAX_CURVES; i++)
    if (!sds->buffers[i].curve)         /* it's available */
      break;

  if (i < STRIP_MAX_CURVES)
  {
    sds->buffers[i].first = SIZE_MAX;
    sds->buffers[i].val = (double *)malloc (sds->buf_size * sizeof (double));
    sds->buffers[i].stat = (StatusType *)calloc
      (sds->buf_size, sizeof (StatusType));
    if (sds->buffers[i].val && sds->buffers[i].stat)
    {
      sds->buffers[i].curve = (StripCurveInfo *)the_curve;
      memset (sds->buffers[i].endpoints, 0, 2*sizeof(DataPoint));
      
      /* use the id field of the strip curve to reference the buffer */
      ((StripCurveInfo *)the_curve)->id = &sds->buffers[i];
	
      sds->buffers[i].history.fetch_stat = FETCH_IDLE;
      ret = 1;
    }
    else
    {
      if (sds->buffers[i].val) free (sds->buffers[i].val);
      if (sds->buffers[i].stat) free (sds->buffers[i].stat);
    }
  }
  
  return ret;
}
#if 0
if(sds->cur_idx > 1) {
  CurveData *cd = CURVE_DATA(the_curve);
  struct timeval h0, h_end;
  int n,m,k; 
  int status;

  h_end=sds->times[sds->cur_idx];
  for (m=0;m<sds->cur_idx; m++) {
    if ( sds->times[m].tv_sec != 0 ) { 
	h0=sds->times[m];
	break;
    }
  }
  printf("m=%d cur_idx=%d\n",m,sds->cur_idx );
  if (m < sds->cur_idx)
  { 
		    
    printf("sds->cur_idx=%ld\n",sds->cur_idx);
    k=0;
    do 
    {
	k++;
	status = StripHistory_fetch
	  (sds->history, cd->curve->details->name, &h0, &h_end,
	    &cd->history, 0, 0);
	printf("%d %d %s:cd->history.n_points=%d\n",
	  k,status,cd->curve->details->name,cd->history.n_points);
		
    } 
    while ( (cd->history.n_points == 0 ) && ( k < 2)  ) ;
 
    if(cd->history.n_points > 0) {
	for(n=0;n<cd->history.n_points;n++)
	  printf("%s",ctime(&cd->history.times[n].tv_sec));
    }   

    /* History problem  */
	    
  }
  else printf(" only zerro time\n");
}
#endif

void  StripDataSource_refresh (StripDataSource        the_sds)
{
  StripDataSourceInfo   *sds = (StripDataSourceInfo *)the_sds;
  int i;

/* Albert */
  for(i=0;i<STRIP_MAX_CURVES;i++)
    sds->buffers[i].history.fetch_stat = FETCH_IDLE;
}


/*
 * StripDataSource_min_max
 */
int
StripDataSource_min_max (StripDataSourceInfo *sds, struct timeval h0,
  struct timeval h_end)
{
  StripCurveInfo                *c;
  int                           m,i;
  int                           some_data;
  int need_refresh=0;
  CurveData *cd;
  
  double min=0.0;
  double max=0.0;
  
  int first,last;
  
  double width;
  double alpha;
  
  int local_precision;
  
  for (m = 0; m < STRIP_MAX_CURVES; m++)
  {
    if ((c = sds->buffers[m].curve) != NULL)
    {
      cd = &sds->buffers[m];
      some_data = 0;
	
      first=find_date_idx
	  (&h0, sds->times, sds->count, sds->buf_size, sds->cur_idx, SDS_GTE);
      last=find_date_idx
	  (&h_end, sds->times, sds->count, sds->buf_size, sds->cur_idx, SDS_LTE);
	
      if ((first > -1) && (last > -1) )
	{
	  some_data=1;
	  min=cd->val[first]; 
	  max=cd->val[first];
	  if (first <= last)
	  {
	    for(i=first; i <= last; i++) 
	    {
		if(cd->val[i] < min) min=cd->val[i]; 
		if(cd->val[i] > max) max=cd->val[i]; 
	    }
	  }
	  else
	  {
#if 0
	    /* perror Albert problem */
	    for(i=first; i <= last; i--)
	    {
		printf("\n\n!!!!%f;\n",cd->val[i]); 
		if(cd->val[i]<min) min=cd->val[i]; 
		if(cd->val[i]>max) max=cd->val[i]; 
	    }
#else
	    /* First part is first to end */
	    for(i=first; i < (int)sds->buf_size; i++)
	    {
		if(cd->val[i] < min) min=cd->val[i]; 
		if(cd->val[i] > max) max=cd->val[i]; 
	    }
	    /* Second part is 0 to last */
	    for(i=0; i <= last; i++)
	    {
		if(cd->val[i] < min) min=cd->val[i]; 
		if(cd->val[i] > max) max=cd->val[i]; 
	    }
#endif	  
	  }
	}
#ifdef STRIP_HISTORY
	if(!cursor) cursor = XCreateFontCursor(XtDisplay(history_topShell),XC_watch);
	XDefineCursor(XtDisplay(history_topShell),
	  XtWindow(history_topShell), cursor);
	XFlush(XtDisplay(history_topShell));

      StripHistory_fetch
	  (sds->history, cd->curve->details->name, &h0, &h_end,
	    &cd->history, 0, 0);

	XUndefineCursor(XtDisplay(history_topShell),
	  XtWindow(history_topShell));

	if((cd->history.n_points>0)&&(cd->history.fetch_stat==FETCH_DONE))
	{
	  first = find_date_idx
	    (&h0, cd->history.times, cd->history.n_points,
		cd->history.n_points, cd->history.n_points - 1, SDS_GTE);
	  last = find_date_idx
	    (&h_end, cd->history.times, cd->history.n_points,
		cd->history.n_points, cd->history.n_points - 1, SDS_LTE);
	  
	  if ((first > -1) && (last > -1) && (first<=last ) )
	  {
	    if (some_data == 0) 
	    {
		min=cd->history.data[first];
		max=cd->history.data[first];
		some_data =1;
	    }
	    for(i=first;i<= last; i++)  
	    {
		if (cd->history.data[i]< min) min=cd->history.data[i]; 
		if (cd->history.data[i]> max) max=cd->history.data[i]; 
	    }
	  }
	}

#endif /* STRIP_HISTORY */

      if ((min < max) && (some_data) )
	{
	  if((c->details->min != min)||(c->details->max != max)) 
	  {
	    need_refresh=1;
	    width = max-min;
	    c->details->max= max + (double) (width/100.0);
	    c->details->min= min - (double) (width/100.0);
	  }
	}
      else if ((min == max) && (some_data) )
	{
	  if( !((c->details->min < min)&& (c->details->max > min)) && 
	    (c->details->max - c->details->min > 0 ) )
	  {
	    /* constant-curve is unvisible - put it in visible area
	       using psevdo-random  algorithm:*/
	    need_refresh=1;
	    width= c->details->max - c->details->min;
	    alpha = (double) ( (double) (time(NULL)%25) +25.0) /100.0;
	    c->details->min = min - width*(  alpha);
	    c->details->max = min + width*(1-alpha);
	  }
	}
      else
	{
	  printf ("min>max:%g>%g or isData=%d\n",min,max,some_data);
	  continue;
	}

	/****Reasonable shift of lim i.e. min=0.123098712345 -> min=0.123 *******/
	local_precision=c->details->precision;
	if (local_precision < 2) local_precision =1;
	for(i=0;i<local_precision;i++)
	{
	  c->details->min= c->details->min*10.0;
	  c->details->max= c->details->max*10.0;
	}
	
	c->details->min=floor(c->details->min);
	c->details->max=ceil (c->details->max);
	
	for(i=0;i<local_precision;i++)
	{
	  c->details->min= c->details->min/10.0;
	  c->details->max= c->details->max/10.0;
	}
	/* End of Reasonable shift of limits ********************* */
	
    }
  }
  
  return (need_refresh);
}



/*
 * StripDataSource_removecurve
 */
int
StripDataSource_removecurve     (StripDataSource the_sds, StripCurve the_curve)
{
  StripDataSourceInfo   *sds = (StripDataSourceInfo *)the_sds;
  CurveData             *cd;
  int                   ret_val = 1;

  if ((cd = CURVE_DATA(the_curve)) != NULL)
  {
    StripHistoryResult_release (sds->history, &cd->history);
    cd->curve = NULL;
    free (cd->val);
    free (cd->stat);
    cd->val = NULL;
    cd->stat = NULL;
    ((StripCurveInfo *)the_curve)->id = NULL;
  }

  return ret_val;
}

#if 0
/* KE: This is not used */
/*
 * StripDataSource_removecurveAll
 * for final clean all garbage at data-buffer  Albert
 * This very much like StripDataSource_init except not everything
 *   is zeroed
 */
int
StripDataSource_removecurveAll(StripDataSource the_sds)
{
  StripDataSourceInfo   *sds = (StripDataSourceInfo *)the_sds;
  int                   ret_val = 1;

#if DEBUG_SDS_TIMES
  /* KE: Expect you should free things before setting the pointers to
     zero, e.g. sds->times */
  printf("StripDataSource_removecurveAll: times=%x\n",sds->times);
#endif  
  
  /*sds->history        = NULL; Albert*/
  sds->buf_size       = 0;
  sds->cur_idx        = 0;
  sds->count          = 0;
  sds->times          = 0;
  sds->idx_t0         = 0;
  sds->idx_t1         = 0;
  sds->bin_size       = 0;
  sds->n_bins         = 0;
  
  /* clear the buffers */
  memset (sds->buffers, 0, STRIP_MAX_CURVES * sizeof(CurveData));
  
  return ret_val;
}
#endif

/*
 * StripDataSource_sample
 */
void
StripDataSource_sample  (StripDataSource the_sds, char *sgP)
{
  StripGraph sg = (StripGraph) sgP;
  StripDataSourceInfo           *sds = (StripDataSourceInfo *)the_sds;
  StripCurveInfo                *c;
  int                           i;
  int                           need_time = 1;
  double a; /*Albert*/
  
  for (i = 0; i < STRIP_MAX_CURVES; i++)
  {
    
    if ((c = sds->buffers[i].curve) != NULL)
    {
	a=c->get_value (c->func_data);
      if (need_time)
      {
	  if (a != sds->buffers[i].val[sds->cur_idx])
	  {
	    /*printf("name=%s;old=%f,new=%f\n",
		c->details->name,a,
		sds->buffers[i].val[sds->cur_idx]);*/
	    CurveLegendRefresh(c,sg,a); 
	  }
	  
	  
	  
        sds->cur_idx = (sds->cur_idx + 1) % sds->buf_size;
        get_current_time (&sds->times[sds->cur_idx]);
        sds->count = min ((sds->count+1), sds->buf_size);
        need_time = 0;
      }       
      else {
	  if (a != sds->buffers[i].val[sds->cur_idx -1 ])
	  {
	    CurveLegendRefresh(c,sg,a);
	  }
      }
	
      if ((c->status & STRIPCURVE_CONNECTED) &&
	  !(c->status & STRIPCURVE_WAITING))
      {
	  sds->buffers[i].val[sds->cur_idx] = a;
	  /*c->get_value (c->func_data); */
        sds->buffers[i].stat[sds->cur_idx] = DATASTAT_PLOTABLE;
	  
        /* first sample for this curve? */
        if (sds->buffers[i].first == SIZE_MAX)
          sds->buffers[i].first = sds->cur_idx;
	  
        /* otherwise, have we just overwritten what was previously
         * the first data point? If so, then increment the first data
         * point index */
        else if (sds->cur_idx == sds->buffers[i].first)
          sds->buffers[i].first = (sds->buffers[i].first + 1) % sds->buf_size;
      }
      else sds->buffers[i].stat[sds->cur_idx] &= ~DATASTAT_PLOTABLE;
    }
  }
}
/*
  Line 844
  some HACK here if delta time for History request < 2% from range interval
  do not send History request!  Albert Kagarmanov 23.11.2000
*/



/*
 * StripDataSource_init_range
 */
int
StripDataSource_init_range      (StripDataSource        the_sds,
  struct timeval         *t0,
  double                 bin_size,
  int                    n_bins,
  sdsRenderTechnique     method)
{
  StripDataSourceInfo   *sds = (StripDataSourceInfo *)the_sds;
  CurveData             *cd;
  struct timeval        t1, t_tmp;
  struct timeval        h0, h1, *h_end;
  long                  r0, r1 = 0;
  int                   have_data = 0;
  int                   i;

  long deltaHistoryTime;

  /* make t1 */
  dbl2time (&t_tmp, n_bins * bin_size);
  add_times (&t1, t0, &t_tmp);

  /* initial history request range */
  h0 = *t0;
  h1 = t1;
  
  /* find earliest timestamp in ring buffer which is greater than
   * or equal to the desired begin time */
  r0 = ((sds->count > 0)?
    find_date_idx
    (t0, sds->times, sds->count, sds->buf_size, sds->cur_idx, SDS_GTE)
    : -1);

  /* look for last date only if the first one was ok,
   * set up history request range */
  if (r0 >= 0)
  {
    r1 = find_date_idx
      (&t1, sds->times, sds->count, sds->buf_size, sds->cur_idx, SDS_LTE);
    if (compare_times (&sds->times[r0], &t1) <= 0)
      h1 = sds->times[r0];
  }
  
  /* set the ring buffer date pointers */
  if ((r0 >= 0) && (r1 >= 0))
  {
    sds->idx_t0 = (size_t)r0;
    sds->idx_t1 = (size_t)r1;
    
    if (sds->idx_t0 <= sds->idx_t1)
      r0 = sds->idx_t1 - sds->idx_t0 + 1;
    else r0 = sds->buf_size - sds->idx_t0 + sds->idx_t1 + 1;

    have_data = 1;
  }
  else sds->idx_t0 = sds->idx_t1;
  
  /* check each curve for fast-update plausibility, and send off
   * any requisite history fetches */
  for (i = 0; i < STRIP_MAX_CURVES; i++)
    if (sds->buffers[i].curve)
    {
      cd = &sds->buffers[i];

      /* verify endpoints
       *
       *  If there is already some data rendered (in which case
       *  the endpoints for the curve will have been set to valid
       *  time values), then check whether either endpoint falls
       *  into the current range.  If so, then we'll be able to
       *  do a fast update by connecting to the already rendered
       *  data.  We also can avoid calling out to the history
       *  service if the timespan for which we don't have live
       *  data lies entirely within the interval which the
       *  already-rendered data spans (described by extents).
       */
      cd->connectable = False;
      if (compare_times (&cd->endpoints[0].t, &cd->endpoints[1].t) < 0)
        if ((compare_times (t0, &cd->endpoints[0].t) < 0) ||
	    (compare_times (&t1, &cd->endpoints[1].t) > 0))
          cd->connectable = (method == SDS_JOIN_NEW);

      
      /* history request range
       *
       * case 1: no live data                           (first = ??)
       *        request entire range
       *
       * case 2: live data starts on or before t0       (first <= t0)
       *        don't need history data
       *        
       * case 3: live data starts on or after t1        (first >= t1)
       *        request entire range
       *
       * case 4: live data starts within the interval   (t0 < first < t1)
       *   case a: no rendered data
       *        request range ends at first
       *   case b: have connectable rendered data
       *     case 1: live data starts before or after rendered extent
       *     (first < extents[0] OR first > extents[1])
       *        request range ends at first
       *     case 2: live data starts within rendered extent
       *     (extents[0] <= first <= extents[1])
       *        request range ends at extents[0]
       */

      /* case 1 */
      if (cd->first == SIZE_MAX)
        h_end = &h1;

      /* case 2 */
      else if (compare_times (&sds->times[cd->first], t0) <= 0)
        h_end = &h0;

      /* case 3 */
      else if (compare_times (&sds->times[cd->first], &t1) >= 0)
        h_end = &h1;

      /* case 4-a */
      else if (!cd->connectable)
        h_end = &sds->times[cd->first];

      /* case 4-b-1 */
      else if ((compare_times (&sds->times[cd->first], &cd->extents[0]) < 0) ||
	  (compare_times (&sds->times[cd->first], &cd->extents[1]) > 0))
        h_end = &sds->times[cd->first];

      /* case 4-b-2 */
      else h_end = &cd->extents[0];
      

      deltaHistoryTime = h_end->tv_sec - h0.tv_sec;

      /*      printf("deltaHistoryTime=%ld n_bins=%d bin_size=%g \n",deltaHistoryTime,n_bins,bin_size); */

      /* get the history data? */
      if ( (compare_times (&h0, h_end) < 0) &&
	  ((cd->history.fetch_stat == FETCH_IDLE) ||
	    (compare_times (&cd->history.t0, &h0) > 0) ||
	    (compare_times (&cd->history.t1, h_end) < 0)) && 
	  ((auto_scaleTriger!=1)||((auto_scaleTriger==1)&&(radioChange))) 
	  && (n_bins*bin_size > 0) && (deltaHistoryTime > 1) &&
	  (deltaHistoryTime > ((5.0*n_bins*bin_size)/100.0) )
	  )
      {
        /* cancel preceding request if necessary */
        if (cd->history.fetch_stat == FETCH_PENDING)
          StripHistory_cancel (sds->history, &cd->history);

        /* send off new request */
	  if(!cursor) cursor=XCreateFontCursor(XtDisplay(history_topShell),XC_watch);
	  XDefineCursor(XtDisplay(history_topShell),
	    XtWindow(history_topShell), cursor);
	  XFlush(XtDisplay(history_topShell));

        StripHistory_fetch
          (sds->history, cd->curve->details->name, &h0, h_end,
		&cd->history, 0, 0);
	  XUndefineCursor(XtDisplay(history_topShell),
	    XtWindow(history_topShell));
      }

      /* if we have history data, we now need to find the
       * begin and end locations for the current history range */
      if ((compare_times (&h0, h_end) < 0) &&
	  (cd->history.fetch_stat == FETCH_DONE))
      {
        cd->hidx_t0 = find_date_idx
          (&h0, cd->history.times, cd->history.n_points,
		cd->history.n_points, cd->history.n_points - 1, SDS_GTE);
        cd->hidx_t1 = find_date_idx
          (h_end, cd->history.times, cd->history.n_points,
		cd->history.n_points, cd->history.n_points - 1, SDS_LTE);

        have_data |= ((cd->hidx_t0 >= 0) && (cd->hidx_t1 >= cd->hidx_t0));
      }
    }
  if (radioChange)  radioChange=0;
  sds->req_t0 = *t0;
  sds->req_t1 = t1;
  sds->bin_size = bin_size;
  sds->n_bins = n_bins;

  return have_data;
}



/* StripDataSource_render
 */
size_t
StripDataSource_render  (StripDataSource        the_sds,
  StripCurve             curve,
  sdsTransform           x_transform,
  void                   *x_data,
  sdsTransform           y_transform,
  void                   *y_data,
  XSegment               **segs)
{
  StripDataSourceInfo   *sds = (StripDataSourceInfo *)the_sds;
  CurveData             *cd = CURVE_DATA(curve);
  int                   max_points = 0;
  DataPoint             ring_first, hist_first, ring_last, hist_last;
  TimeBuffer            ring_times, hist_times;
  ValueBuffer           ring_values, hist_values;
  StatusBuffer          ring_status, hist_status;
  int                   data_state = 0;

  render_buffer.n_segs = 0;

  /* ring buffer pointers & initializations */
  if (sds->idx_t0 != sds->idx_t1)
  {
    data_state |= SDS_BUFFERED_DATA;

    if (sds->idx_t0 > sds->idx_t1)
      max_points = sds->buf_size - sds->idx_t0 + sds->idx_t1 + 1;
    else max_points = sds->idx_t1 - sds->idx_t0 + 1;
      
    ring_times.base = sds->times; 
    ring_times.count = sds->buf_size;
    ring_values.base = cd->val;
    ring_values.count = sds->buf_size;
    ring_status.base = cd->stat;
    ring_status.count = sds->buf_size;
  }

  /* history buffer pointers & initializations */
  if (cd->history.fetch_stat == FETCH_DONE)
  {
    data_state |= SDS_HISTORY_DATA;
    
    hist_times.base = cd->history.times;
    hist_times.count = cd->history.n_points;
    hist_values.base = cd->history.data;
    hist_values.count = cd->history.n_points;
    hist_status.base = cd->history.status;
    hist_status.count = cd->history.n_points;
  }

  if (!(data_state & SDS_BOTH_DATA))    /* no data at all? */
  {
    cd->endpoints[0].t.tv_sec = 1;
    cd->endpoints[1].t.tv_sec = 0;
    return 0;
  }
      
  /* fast update? */
  if (cd->connectable)
  {
    /* any data in the ring buffer ahead of currently rendered? */
    if (data_state & SDS_BUFFERED_DATA)
    {
      if (compare_times
	  (&ring_times.base[sds->idx_t0], &cd->endpoints[0].t) < 0)
      {
        ring_times.ptr = ring_times.base + sds->idx_t0;
        ring_values.ptr = ring_values.base + sds->idx_t0;
        ring_status.ptr = ring_status.base + sds->idx_t0;
        
        segmentify
          (sds, &render_buffer, SDS_INCREASING,
		&ring_times, &ring_values, &ring_status,
		max_points, &cd->endpoints[0].t,
		0, &cd->endpoints[0],
		&ring_first, 0, x_transform, x_data, y_transform, y_data);
        
        /* remember new beginning endpoint */
        cd->endpoints[0] = ring_first;
      }
      else
      {
        ring_first.t = ring_times.base[sds->idx_t0];
        ring_first.v = ring_values.base[sds->idx_t0];
        ring_first.s = ring_status.base[sds->idx_t0];
      }
    }

    /* any history data before currently rendered? */
    if (data_state & SDS_HISTORY_DATA)
    {
      if (compare_times
	  (&hist_times.base[cd->hidx_t0], &cd->endpoints[0].t) < 0)
      {
        hist_times.ptr = hist_times.base + cd->hidx_t0;
        hist_values.ptr = hist_values.base + cd->hidx_t0;
        hist_status.ptr = hist_status.base + cd->hidx_t0;

        segmentify
          (sds, &render_buffer, SDS_INCREASING,
		&hist_times, &hist_values, &hist_status,
		cd->hidx_t1 - cd->hidx_t0 + 1, &cd->endpoints[0].t,
		0, &cd->endpoints[0],
		&hist_first, 0, x_transform, x_data, y_transform, y_data);
        
        /* new beginning endpoint */
        cd->endpoints[0] = hist_first;
      }
      else
      {
        hist_first.t = hist_times.base[cd->hidx_t0];
        hist_first.v = hist_values.base[cd->hidx_t0];
        hist_first.s = hist_status.base[cd->hidx_t0];
      }
    }

    /* any history data following currently rendered? */
    if (data_state & SDS_HISTORY_DATA)
    {
      if (compare_times
	  (&hist_times.base[cd->hidx_t1], &cd->endpoints[1].t) > 0)
      {
        hist_times.ptr = hist_times.base + cd->hidx_t1;
        hist_values.ptr = hist_values.base + cd->hidx_t1;
        hist_status.ptr = hist_status.base + cd->hidx_t1;
        
        segmentify
          (sds, &render_buffer, SDS_DECREASING,
		&hist_times, &hist_values, &hist_status,
		cd->hidx_t1 - cd->hidx_t0 + 1, &cd->endpoints[1].t,
		0, &cd->endpoints[1],
		&hist_last, 0, x_transform, x_data, y_transform, y_data);
        
        /* new finishing endpoint */
        cd->endpoints[1] = hist_last;
      }
      else
      {
        hist_last.t = hist_times.base[cd->hidx_t1];
        hist_last.v = hist_values.base[cd->hidx_t1];
        hist_last.s = hist_status.base[cd->hidx_t1];
      }
    }
     
    /* any data in the ring buffer following currently rendered? */
    if (data_state & SDS_BUFFERED_DATA)
    {
      if (compare_times
	  (&ring_times.base[sds->idx_t1], &cd->endpoints[1].t) > 0)
      {
        ring_times.ptr = ring_times.base + sds->idx_t1;
        ring_values.ptr = ring_values.base + sds->idx_t1;
        ring_status.ptr = ring_status.base + sds->idx_t1;

        segmentify
          (sds, &render_buffer, SDS_DECREASING,
		&ring_times, &ring_values, &ring_status,
		max_points, &cd->endpoints[1].t,
		0, &cd->endpoints[1],
		&ring_last, 0, x_transform, x_data, y_transform, y_data);
        
        /* remember new finishing endpoint */
        cd->endpoints[1] = ring_last;
      }
      else
      {
        ring_last.t = ring_times.base[sds->idx_t1];
        ring_last.v = ring_values.base[sds->idx_t1];
        ring_last.s = ring_status.base[sds->idx_t1];
      }
    }

    /* verify that the endpoints are within the current range.
     * If not, choose the closest history or buffer point. */
    if ((compare_times (&cd->endpoints[0].t, &sds->req_t0) < 0))
    {
      if (data_state == SDS_BUFFERED_DATA)
        cd->endpoints[0] = ring_first;
      else if (data_state == SDS_HISTORY_DATA)
        cd->endpoints[0] = hist_first;
      else if (compare_times (&ring_first.t, &hist_first.t) <= 0)
        cd->endpoints[0] = ring_first;
      else cd->endpoints[0] = hist_first;
    }
    
    if ((compare_times (&cd->endpoints[1].t, &sds->req_t1) > 0))
    {
      if (data_state == SDS_BUFFERED_DATA)
        cd->endpoints[1] = ring_last;
      else if (data_state == SDS_HISTORY_DATA)
        cd->endpoints[1] = hist_last;
      else if (compare_times (&ring_last.t, &hist_last.t) >= 0)
        cd->endpoints[1] = ring_last;
      else cd->endpoints[1] = hist_last;
    }

    /* Finally, set the rendered data extents.  We are guaranteed that
     * the extents have already been initialized because they are set
     * when a curve is first rendered, and a fast update can not be
     * initiated if no data is rendered.  So, we just need to
     *
     *  (a) expand the extents if the corresponding endpoints have expanded
     *  (b) clip the extents to the current range if they overextend
     */
    if (compare_times (&cd->endpoints[0].t, &cd->extents[0]) < 0)
      cd->extents[0] = cd->endpoints[0].t;
    if (compare_times (&cd->endpoints[1].t, &cd->extents[1]) > 0)
      cd->extents[1] = cd->endpoints[1].t;
    
    if (compare_times (&cd->extents[0], &sds->req_t0) < 0)
      cd->extents[0] = sds->req_t0;
    if (compare_times (&cd->extents[1], &sds->req_t1) > 0)
      cd->extents[1] = sds->req_t1;
  }

  /* complete refresh? */
  else
  {
    /* ====== ring buffer ====== */
    if (data_state & SDS_BUFFERED_DATA) /* any buffered data on range? */
    {
      ring_times.ptr = ring_times.base + sds->idx_t0;
      ring_values.ptr = ring_values.base + sds->idx_t0;
      ring_status.ptr = ring_status.base + sds->idx_t0;
      
      segmentify
        (sds, &render_buffer, SDS_INCREASING,
	    &ring_times, &ring_values, &ring_status,
	    max_points, &ring_times.base[sds->idx_t1],
	    0, 0,
	    &cd->endpoints[0], &cd->endpoints[1],  /* new endpoints */
	    x_transform, x_data, y_transform, y_data);
    }
      
    /* ====== history data ====== */
    if (data_state & SDS_HISTORY_DATA)
    {
      hist_times.ptr = hist_times.base + cd->hidx_t0;
      hist_values.ptr = hist_values.base + cd->hidx_t0;
      hist_status.ptr = hist_status.base + cd->hidx_t0;

      if (data_state & SDS_BUFFERED_DATA)
      {
        /* any history data ahead of currently rendered buffer data? */
        if (compare_times
	    (&hist_times.base[cd->hidx_t0], &cd->endpoints[0].t) < 0)
        {
          segmentify
            (sds, &render_buffer, SDS_INCREASING,
		  &hist_times, &hist_values, &hist_status,
		  cd->hidx_t1 - cd->hidx_t0 + 1, &cd->endpoints[0].t,
		  0, &cd->endpoints[0],
		  &hist_first, 0, x_transform, x_data, y_transform, y_data);
          
          /* new beginning endpoint */
          cd->endpoints[0] = hist_first;
        }
      }
      else
      {
        segmentify
          (sds, &render_buffer, SDS_INCREASING,
		&hist_times, &hist_values, &hist_status,
		cd->hidx_t1 - cd->hidx_t0 + 1, &hist_times.base[cd->hidx_t1],
		0, 0,
		&cd->endpoints[0], &cd->endpoints[1],        /* new endpoints */
		x_transform, x_data, y_transform, y_data);
      }
    }

    /* Record the data extents.  Since we have just done a complete
     * refresh, then the extents are the same as the endpoints */
    cd->extents[0] = cd->endpoints[0].t;
    cd->extents[1] = cd->endpoints[1].t;
  }

  *segs = render_buffer.segs;
  return render_buffer.n_segs;
}


/* segmentify
 */
static size_t
segmentify      (StripDataSourceInfo    *sds,
  RenderBuffer           *rbuf,
  SegmentifyDirection    direction,
  TimeBuffer             *times,
  ValueBuffer            *values,
  StatusBuffer           *status,
  int                    max_points,
  struct timeval         *stop_t,
  DataPoint              *connect_first,
  DataPoint              *connect_last,
  DataPoint              *first,
  DataPoint              *last,
  sdsTransform           x_transform,
  void                   *x_data,
  sdsTransform           y_transform,
  void                   *y_data)
{
  XPoint                p1, p2;         /* previous point, current point */
  XSegment              *s;             /* current line segment */
  Boolean               empty_seg;
  Boolean               done;
  int                   d1x, d1y;       /* dx, dy for current line (s) */
  int                   d2x, d2y;       /* dx, dy for new line (p1, p2) */
  struct timeval        *t = NULL;
  double                *v = NULL;
  double                z;
  StatusType            *stat = NULL;
  int                   offset;
  int                   n_processed;
  Boolean               zero_points;

  p1.x=0;
  p1.y=0;

  /* NB: For this algorithm, we'll adopt the convention that
   * the first point in a segment will always hold the min
   * point with respect to x.  So for XSegment s, s.x1 <= s.x2
   * 
   * first make sure we have enough memory to hold a reasonable
   * number of sements: 2 * number of horizontal bins
   */
  if (!verify_render_buffer (rbuf, sds->n_bins * 2))
    fprintf (stderr, "StripDataSource_segmentify(): memory exhausted\n");

  /* process 'em */
  s = &rbuf->segs[rbuf->n_segs];
  empty_seg = True;
  n_processed = 0;
  done = False;
  zero_points = True;
  
  while (!done || connect_last)
  {
    if (connect_first)
    {
      /* connect first point */
      t = &connect_first->t;
      v = &connect_first->v;
      stat = &connect_first->s;
      connect_first = 0;
    }
    
    else if ((n_processed < max_points) && !done)
    {
      if ((direction == SDS_INCREASING) &&
	  (compare_times (times->ptr, stop_t) <= 0))
      {
        t = times->ptr++;
        v = values->ptr++;
        stat = status->ptr++;
        n_processed++;

        /* check buffers for wrap around */
        if (times->ptr >= times->base + times->count)
          times->ptr -= times->count;
        if (values->ptr >= values->base + values->count)
          values->ptr -= values->count;
        if (status->ptr >= status->base + status->count)
          status->ptr -= status->count;
      }
      else if ((direction == SDS_DECREASING) &&
	  (compare_times (times->ptr, stop_t) >= 0))
      {
        t = times->ptr--;
        v = values->ptr--;
        stat = status->ptr--;
        n_processed++;

        /* check buffers for wrap around */
        if (times->ptr < times->base)
          times->ptr += times->count;
        if (values->ptr < values->base)
          values->ptr += values->count;
        if (status->ptr < status->base)
          status->ptr += status->count;
      }
      else
      {
        done = True;
        continue;
      }
    }
    
    else if (connect_last)
    {
      /* connect last point */
      t = &connect_last->t;
      v = &connect_last->v;
      stat = &connect_last->s;
      connect_last = 0;
    }
    
    else break;

    /* remeber first point */
    if (zero_points)
    {
      if (first)
      {
        first->t = *t;
        first->v = *v;
        first->s = *stat;
      }
      zero_points = False;
    }

    /* if this point is not plotable, then we have a hole
     * in the data.  Must finish current segment if not
     * already finished, and flag that we need to start
     * afresh. */
    if (!(*stat & DATASTAT_PLOTABLE))
    {
      /* If the previous point was valid, then we need to 
       * stop goofing with its line segment, and start working
       * on a new one. */
      if (!empty_seg) s++;

      /* make sure we have enough memory.  This isn't very efficient,
       * but hopefully will only happen in rare situations */
      offset = s - rbuf->segs;
      if (!verify_render_buffer (rbuf, rbuf->n_segs + 128))
      {
        fprintf
          (stderr,
		"StripDataSource_segmentify():\n"
		"  memory exhausted, unable to render all data\n");
        return rbuf->n_segs;
      }
      s = rbuf->segs + offset;
      
      empty_seg = True;
      continue;
    }

    /* transform the data point to a raster location.
     *
     *  For the time being, we just transform one point at a time.
     *  Once I've got this debugged, we can do bigger chunks in
     *  order to eliminate some of the function call overhead.
     */
    z = time2dbl (t);
    x_transform (x_data, &z, &z, 1);
    p2.x = (short)z;
    y_transform (y_data, v, &z, 1);
    p2.y = (short)z;

    if (empty_seg)      /* initialize empty segment? */
    {
      s->x1 = s->x2 = p1.x = p2.x;
      s->y1 = s->y2 = p1.y = p2.y;
      empty_seg = False;
      rbuf->n_segs++;
    }
    else
    {
      /* the slope of the current line */
      d1x = s->x2 - s->x1;
      d1y = s->y2 - s->y1;

      /* the slope of the new line */
      d2x = p2.x - p1.x;
      d2y = p2.y - p1.y;

      /* now we have the slope of the original line in d1x, and,
       * in d2x, the slope of the line generated by connecting
       * the previous point, p1, with the new point, p2.
       * 
       * there are four possibilities for each slope (note that
       * x is assumed to be monotonically increasing or decreasing,
       * because it is a function of reverse or forward time).
       * 
       * (a) dx == 0, dy == 0   : no change
       * (b) dx == 0, dy != 0   : vertical +/-.
       * (c) dx != 0,  dy == 0  : horizontal +
       * (d) dx != 0,  dy != 0  : horizontal +, vertical +/-
       * 
       * Note that if either slope falls into category (a), then
       * the lines may be collapsed.
       * 
       * If both slopes fall into category (b) or both fall into
       * category (c), then the lines can be collapsed.
       * 
       * If both slopes fall into category (d), then the lines
       * can be collapsed iff d1y/d1x = d2y/d2x
       */
      
      /* category a     --one or both lines are actually just points */
      if (!d1x && !d1y)
      {
        s->x2 = p2.x;
        s->y2 = p2.y;
      }
      else if (!d2x && !d2y)
      {
        /* new point overlaps old --do nothing */
      }

      
      /* category b     --changing vertical component */
      else if (!d1x && !d2x)
      {
        if (s->y1 >= s->y2)
        {
          if (p2.y > s->y1)
            s->y1 = p2.y;
          else if (p2.y < s->y2)
            s->y2 = p2.y;
        }
        else    /* s->y1 < s->y2 */
        {
          if (p2.y > s->y2)
            s->y2 = p2.y;
          else if (p2.y < s->y1)
            s->y1 = p2.y;
        }
      }

      
      /* category c     --changing horizontal component */
      else if (!d1y && !d2y)
      {
        if (p2.x != s->x2) s->x2 = p2.x;
      }

      
      /* category d     --changing horizontal, changing vertical */
      else
      {
        /* if d1x/d1y = d2x/d2y then new point is on the same line */
        if (d1x*d2y == d2x*d1y)
        {
          s->x2 = p2.x;
          s->y2 = p2.y;
        }

        /* oh well, can't win 'em all.  Have to make a new line */
        else
        {
          if (s->x2 != p1.x)
          {
            p2.x--;
            p2.x++;
          }
            
          rbuf->n_segs++;
          s++;

          /* make sure we have enough memory */
          offset = s - rbuf->segs;
          if (!verify_render_buffer (rbuf, rbuf->n_segs + 8))
          {
            fprintf
              (stderr,
		    "StripDataSource_segmentify(): memory exhausted, unable to\n"
		    "  render all data\n");
            return rbuf->n_segs;
          }
          s = rbuf->segs + offset;
          
          s->x1 = p1.x;
          s->y1 = p1.y;
          s->x2 = p2.x;
          s->y2 = p2.y;
        }
      }
    }

    p1 = p2;
  }

  if (last && !zero_points)
  {
    last->t = *t;
    last->v = *v;
    last->s = *stat;
  }
  
  return (size_t)n_processed;
}


#ifdef USE_SDDS
/*
 * StripDataSource_dump_sdds
 */
int
StripDataSource_dump_sdds    (StripDataSource        the_sds,
  char                    *fName)
{
  StripDataSourceInfo   *sds = (StripDataSourceInfo *)the_sds;
  char                  buf[SDS_DUMP_FIELDWIDTH+1];
  time_t                tt;
  double                time;
  int                   msec;
  int                   i, i0, j, row;
  SDDS_TABLE            Table;
  long                  rowIndex;
  long                  numRows;

#if DEBUG_SDDS
  printf("StripDataSource_dump_sdds:\n");
#endif  

  /* if range is not initialized, return failure */
  if (sds->idx_t0 == sds->idx_t1) return 0;

  /* if no curves, return failure */
  for (i = 0; i < STRIP_MAX_CURVES; i++) if (sds->buffers[i].curve) break;
  if (i >= STRIP_MAX_CURVES) return 0;

  /* Initializes a SDDS_TABLE structure for use writing data to a SDDS file */
  if (!SDDS_InitializeOutput(&Table,SDDS_BINARY,1L,DUMP_SDDS_DESCRIPTION,
	  DUMP_SDDS_CONTENTS,fName))
    SDDS_PrintErrors(stderr,SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  /* Processes definition of the time column */
  if (SDDS_DefineColumn(&Table,DUMP_SDDS_TIME_COL,NULL,DUMP_SDDS_TIME_COL_UNITS,
    NULL,NULL,SDDS_DOUBLE,0) == -1)
    SDDS_PrintErrors(stderr,SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  /* Processes definitions of the data columns */
  for (i = 0; i < STRIP_MAX_CURVES; i++)
    if (sds->buffers[i].curve)
    {
      if(SDS_DUMP_NUMWIDTH>sds->buffers[i].curve->details->precision)
        sprintf(buf,"%%%d.%dg",SDS_DUMP_NUMWIDTH,
	    sds->buffers[i].curve->details->precision);
      else
        sprintf(buf,"%%%dg",SDS_DUMP_NUMWIDTH);
      if (SDDS_DefineColumn(&Table, sds->buffers[i].curve->details->name,NULL,
		sds->buffers[i].curve->details->egu,
		sds->buffers[i].curve->details->comment,
		buf, SDDS_DOUBLE, 0) == -1)
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    }

  /* Writes the SDDS header describing the layout of the data tables */
  if (!SDDS_WriteLayout(&Table))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  /* Initializes a SDDS_TABLE structure */
  numRows = sds->count;
  if (!SDDS_StartTable(&Table, numRows))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

#if DEBUG_SDDS
  printf("  sds->idx_t0=%ld sds->idx_t1=%ld sds->cur_idx=%ld sds->buf_size=%ld\n",
    (long)sds->idx_t0,(long)sds->idx_t1,(long)sds->cur_idx,(long)sds->buf_size);
  printf("  sds->count=%ld sds->bin_size=%g sds->n_bins=%ld\n",
    (long)sds->count,sds->bin_size,(long)sds->n_bins);
  printf("  numRows=%ld\n",numRows);
#if 0
  for(i=0; i < sds->buf_size; i++) {
    printf("%4d",i);
    for(j=0; j < STRIP_MAX_CURVES; j++) {
      if (sds->buffers[j].curve) {
	  printf(" %2d %10.4f",j,sds->buffers[j].val[i]);
	}
    }
    printf("\n");
  }
#endif  
#endif  

  /* Set SDDS table values */
  rowIndex = 0;
  /* Data is from 1 to cur_idx until it wraps, then it is from
   * cur_idx+1 to buf_size-1 and from 0 to cur_idx. */
  if(sds->count < sds->buf_size) i0 = 1;
  else i0 = sds->cur_idx + 1;
  for (row = 0; row < numRows; row++)
  {
    /* Determine the index in the circular buffer */
    i=(i0 + row) % sds->buf_size;
    
    /* Format sample time column value */
    tt = (time_t)sds->times[i].tv_sec;
    msec = (int)(sds->times[i].tv_usec / ONE_THOUSAND);
    time = (double)tt + ((double)msec / (double)ONE_THOUSAND);

    /* Set time value */
    if (SDDS_SetRowValues(&Table, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, rowIndex,
      DUMP_SDDS_TIME_COL , time, NULL) != 1)
	SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

    /* Format and set data column values */
    for (j = 0; j < STRIP_MAX_CURVES; j++)
      if (sds->buffers[j].curve)
      {

        if (sds->buffers[j].stat[i] & DATASTAT_PLOTABLE)
        {
          if (SDDS_SetRowValues(&Table, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
		    rowIndex, sds->buffers[j].curve->details->name,
		    sds->buffers[j].val[i], NULL) != 1)
            SDDS_PrintErrors(stderr,
		  SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
#if 0
        else
        {
          if (SDDS_SetRowValues(&Table, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
            rowIndex, sds->buffers[j].curve->details->name, "NaN", NULL) != 1)
            SDDS_PrintErrors(stderr,
		  SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
        }
#endif
      }
    ++rowIndex;
  }
#if DEBUG_SDDS
  printf("  numIndex=%ld numRows=%ld\n",rowIndex,numRows);
#endif  

  /* Writes out the current data table */
  if (!SDDS_WriteTable(&Table))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  /* Erases the data set description and frees all memory being used
   * for a data set. */
  if (!SDDS_Terminate(&Table))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  return 1;
}
#endif /* SDDS */


/*
 * StripDataSource_dump
 */
int
StripDataSource_dump    (StripDataSource        the_sds,
  FILE                   *outfile,char * cgi) /* Albert */
{
  StripDataSourceInfo   *sds = (StripDataSourceInfo *)the_sds;
  char                  buf[SDS_DUMP_FIELDWIDTH+1];
  int                   i, j;
  struct timeval Start,End;
  struct timeval StartCopy,EndCopy;
  CurveData *cd;

  struct timeval *timeP=0;
  StripGraph sg = (StripGraph) cgi; /* Albert */

  /* if range is not initialized, return failure  Albert not nessary for hist
     if (sds->idx_t0 == sds->idx_t1) return 0; */

  /* if no curves, return failure */
  for (i = 0; i < STRIP_MAX_CURVES; i++) if (sds->buffers[i].curve) break;
  if (i >= STRIP_MAX_CURVES) { if(DEBUG1)perror("No one curvers");return 0; }

  StripGraph_getattr (sg, STRIPGRAPH_BEGIN_TIME, &Start, 0);
  StripGraph_getattr (sg, STRIPGRAPH_END_TIME,   &End,   0);

  memcpy(&StartCopy,&Start,sizeof(struct timeval));
  memcpy(  &EndCopy,&End,  sizeof(struct timeval));

  if(DEBUG1)printf("Start=%s",ctime((const time_t *)&(Start.tv_sec)));
  if(DEBUG1)printf("End=%s",ctime((const time_t *)&(End.tv_sec)));

  if(!cursor) cursor = XCreateFontCursor(XtDisplay(history_topShell),XC_watch);
  XDefineCursor(XtDisplay(history_topShell),
    XtWindow(history_topShell), cursor);
  XFlush(XtDisplay(history_topShell));

  for (i = 0; i < STRIP_MAX_CURVES; i++) {
    if (!sds->buffers[i].curve) continue; 
    cd = &sds->buffers[i];
    StripHistory_fetch
      (sds->history, cd->curve->details->name, &StartCopy, &EndCopy,
	  &cd->history, 0, 0);
  }

  /* this is very straightforward:
   * (a) for every curve, print out its name across the top
   * (b) for every time on the range
   *     (1) print out the time
   *     (2) for every curve, print out its value
   */

  /* (a) */
  fprintf (outfile, "%s\t", "Time");
  for (i = 0; i < STRIP_MAX_CURVES; i++)
    if (sds->buffers[i].curve) fprintf(outfile, "%s [%s]\t", 
	sds->buffers[i].curve->details->name,sds->buffers[i].curve->details->egu);
  fprintf (outfile, "\n");
  
  /* (b) */
  timeP=&StartCopy;

  while(findNextTime(timeP,timeP,sds) ==0) 
  {
    if(compare_times(timeP,&Start)<0) continue;
    if(compare_times(timeP,&End)>0) break;

    /* (b-1) */
    memset(buf,0,SDS_DUMP_FIELDWIDTH+1);
    strftime(buf, SDS_DUMP_FIELDWIDTH, "%m/%d/%Y %H:%M:%S",
	localtime ((const time_t *)&(timeP->tv_sec)));
    fprintf (outfile, "%s.%06d\t",buf,(int)timeP->tv_usec);
      
    /* (b-2) */
    for (j = 0; j < STRIP_MAX_CURVES; j++)
	if (sds->buffers[j].curve)
	{
	  cd = &sds->buffers[j];
	  memset(buf,0,SDS_DUMP_FIELDWIDTH+1);
	  printData(timeP,cd,buf);
	  fprintf (outfile, "%s\t",buf);
	}
      
    /* finally, the end-line */
    fprintf (outfile, "\n");
  } /* end of history while() handeling */

  
  if(DEBUG1)printf("Start=%s",ctime((const time_t *)&(Start.tv_sec)));
  if(DEBUG1)printf("End=%s",ctime((const time_t *)&(End.tv_sec)));

  if (sds->idx_t0 != sds->idx_t1) 
  {
    for (i = sds->idx_t0; i != sds->idx_t1; i = (i+1) % sds->buf_size)
    {
	if(compare_times(&(sds->times[i]),&End)>0) 
	{if(DEBUG1)printf("T[%d]>End   break\n",i); break;}
	if(compare_times(&(sds->times[i]),&Start)<0) 
	{if(DEBUG1)
	  printf("Start > T[%d]=%s",i,ctime((const time_t *)&(sds->times[i].tv_sec))); 
	continue;}
	if(DEBUG1)printf("Good i=%d\n",i);
	
	/* (b-1) */
	memset(buf,0,SDS_DUMP_FIELDWIDTH+1);
	strftime(buf, SDS_DUMP_FIELDWIDTH, "%m/%d/%Y %H:%M:%S",
	  localtime ((const time_t *)&(sds->times[i].tv_sec)));
	fprintf (outfile, "%s.%06d\t",buf,(int)sds->times[i].tv_usec); 
	/* (b-2) */
	for (j = 0; j < STRIP_MAX_CURVES; j++)
	  if (sds->buffers[j].curve)
	  {
	    if (sds->buffers[j].stat[i] & DATASTAT_PLOTABLE)
		fprintf (outfile, "%g\t",sds->buffers[j].val[i]);
	    else fprintf (outfile, "%s\t",SDS_DUMP_BADVALUESTR);
	  }
	
	/* finally, the end-line */
	fprintf (outfile, "\n");
    }
  } else {if(DEBUG1) perror("DUMP:NO CURRENT DATA");}


  if(DEBUG1)printf("Last i=%d\n",i);

  fflush (outfile);
  XUndefineCursor(XtDisplay(history_topShell),
    XtWindow(history_topShell));
  return 1;
}


/*
 * StripDataSource_dump_csv
 */
int
StripDataSource_dump_csv    (StripDataSource        the_sds,
  FILE                   *outfile,char * cgi)
{
  StripDataSourceInfo   *sds = (StripDataSourceInfo *)the_sds;
  char                  buf[SDS_DUMP_FIELDWIDTH+1];
  int                   i, j;
  struct timeval Start,End;
  struct timeval StartCopy,EndCopy;
  CurveData *cd;

  struct timeval *timeP=0;
  StripGraph sg = (StripGraph) cgi;

  /* if range is not initialized, return failure 
     if (sds->idx_t0 == sds->idx_t1) return 0; */

  /* if no curves, return failure */
  for (i = 0; i < STRIP_MAX_CURVES; i++) if (sds->buffers[i].curve) break;
  if (i >= STRIP_MAX_CURVES) { if(DEBUG1)perror("No one curvers");return 0; }

  StripGraph_getattr (sg, STRIPGRAPH_BEGIN_TIME, &Start, 0);
  StripGraph_getattr (sg, STRIPGRAPH_END_TIME,   &End,   0);

  memcpy(&StartCopy,&Start,sizeof(struct timeval));
  memcpy(  &EndCopy,&End,  sizeof(struct timeval));

  if(DEBUG1)printf("Start=%s",ctime(&(Start.tv_sec)));
  if(DEBUG1)printf("End=%s",ctime(&(End.tv_sec)));

  if(!cursor) cursor = XCreateFontCursor(XtDisplay(history_topShell),XC_watch);
  XDefineCursor(XtDisplay(history_topShell),
    XtWindow(history_topShell), cursor);
  XFlush(XtDisplay(history_topShell));

  for (i = 0; i < STRIP_MAX_CURVES; i++) {
    if (!sds->buffers[i].curve) continue; 
    cd = &sds->buffers[i];
    StripHistory_fetch
      (sds->history, cd->curve->details->name, &StartCopy, &EndCopy,
	  &cd->history, 0, 0);
  }

  /* this is very straightforward:
   * (a) for every curve, print out its name across the top
   * (b) for every time on the range
   *     (1) print out the time
   *     (2) for every curve, print out its value
   */

  /* (a) */
  /* (a) */
  fprintf (outfile, "%s", "Time");
  for (i = 0; i < STRIP_MAX_CURVES; i++)
    if (sds->buffers[i].curve) fprintf(outfile, ",%s [%s]", 
	sds->buffers[i].curve->details->name,sds->buffers[i].curve->details->egu);
  fprintf (outfile, "\n");
  
  /* (b) */
  timeP=&StartCopy;

  while(findNextTime(timeP,timeP,sds) ==0) 
  {
    if(compare_times(timeP,&Start)<0) continue;
    if(compare_times(timeP,&End)>0) break;

    /* (b-1) */
    memset(buf,0,SDS_DUMP_FIELDWIDTH+1);
    strftime(buf, SDS_DUMP_FIELDWIDTH, "%m/%d/%Y,%H:%M:%S",
	localtime (&(timeP->tv_sec)));
    fprintf (outfile, "%s.%06d",buf,(int)timeP->tv_usec);
      
    /* (b-2) */
    for (j = 0; j < STRIP_MAX_CURVES; j++)
	if (sds->buffers[j].curve)
	{
	  cd = &sds->buffers[j];
	  memset(buf,0,SDS_DUMP_FIELDWIDTH+1);
	  printData(timeP,cd,buf);
	  fprintf (outfile, ",%s",buf);
	}
      
    /* finally, the end-line */
    fprintf (outfile, "\n");
  } /* end of history while() handeling */

  
  if(DEBUG1)printf("Start=%s",ctime(&(Start.tv_sec)));
  if(DEBUG1)printf("End=%s",ctime(&(End.tv_sec)));

  if (sds->idx_t0 != sds->idx_t1) 
  {
    for (i = sds->idx_t0; i != sds->idx_t1; i = (i+1) % sds->buf_size)
    {
	if(compare_times(&(sds->times[i]),&End)>0) 
	{if(DEBUG1)printf("T[%d]>End   break\n",i); break;}
	if(compare_times(&(sds->times[i]),&Start)<0) 
	{if(DEBUG1)
	  printf("Start > T[%d]=%s",i,ctime(&(sds->times[i].tv_sec))); 
	continue;}
	if(DEBUG1)printf("Good i=%d\n",i);
	
	/* (b-1) */
	memset(buf,0,SDS_DUMP_FIELDWIDTH+1);
	strftime(buf, SDS_DUMP_FIELDWIDTH, "%m/%d/%Y %H:%M:%S",
	  localtime (&(sds->times[i].tv_sec)));
	fprintf (outfile, "%s.%06d",buf,(int)sds->times[i].tv_usec); 
	/* (b-2) */
	for (j = 0; j < STRIP_MAX_CURVES; j++)
	  if (sds->buffers[j].curve)
	  {
	    if (sds->buffers[j].stat[i] & DATASTAT_PLOTABLE)
		fprintf (outfile, ",%g",sds->buffers[j].val[i]);
	    else fprintf (outfile, ",%s",SDS_DUMP_BADVALUESTR);
	  }
	
	/* finally, the end-line */
	fprintf (outfile, "\n");
    }
  } else {if(DEBUG1) perror("DUMP:NO CURRENT DATA");}


  if(DEBUG1)printf("Last i=%d\n",i);

  fflush (outfile);
  XUndefineCursor(XtDisplay(history_topShell),
    XtWindow(history_topShell));
  return 1;
}


/* ====== Static Functions ====== */
static long
find_date_idx   (struct timeval         *t,
  struct timeval         *times,
  size_t                 n_times,
  size_t                 max_times,
  size_t                 idx_latest,
  int                    mode)
{
  long  a, b, i;
  long  x;


  x = ((long)idx_latest - (long)n_times + 1);
  if (x < 0) x += max_times;

  a = x;
  b = (long)idx_latest;

  /* first check boundary conditions */
  if (mode == SDS_LTE)
  {
    x = compare_times (&times[a], t);
    if (x > 0)
      return -1;
    else if (x == 0)    /* hey, we found it! */
      return a;
  }
  else if (mode == SDS_GTE)
  {
    x = compare_times (&times[b], t);
    if (x < 0)
      return -1;
    else if (x == 0)    /* hey, we found it! */
      return b;
  }

  /* break the buffer into two parts if the begin index is greater than
   * the end index */
  if (a > b)
  {
    /* the buffer wraps around --determine whether t lies btw a and n,
     * or 0 and b */
    x = compare_times (t, &times[max_times-1]);
    if (x < 0)
      b = (long)max_times-1;
    else if (x > 0)
      a = 0;
    else        /* hey, we found it! */
      return (long)max_times-1;
  }
  
  /* now do a binary search */
  do {
    i = a + ((b-a)/2);
    x = compare_times (&times[i], t);

    if (x > 0)
      b = i-1;
    else if (x < 0)
      a = i+1;
    else        /* found it */
      return i;
  } while (a <= b);

  if (x < 0)
  {
    if (mode == SDS_LTE)
      return i;
    if (mode == SDS_GTE)
      return i+1;
  }
  else if (x > 0)
  {
    if (mode == SDS_LTE)
      return i-1;
    if (mode == SDS_GTE)
      return i;
  }

  return -1;    /* never executed */
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * change data buffer size routine
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static int
resize  (StripDataSourceInfo *sds, size_t buf_size)
{
  int                   i;
  int                   new_count = sds->count;
  int                   ret_val = 0;
  
  int                   new_index;

    
#if DEBUG_SDS_TIMES
  printf("resize: times=%u buf_size=%u\n",
    sds->times,buf_size);
#endif
  
  if (sds->buf_size == 0)
  {
    sds->times = (struct timeval *)malloc
      (buf_size * sizeof(struct timeval));
    new_index = new_count = 0;
    ret_val = (sds->times != NULL);
  }
  else
  {
    ret_val = pack_array
      ((void **)&sds->times, sizeof(struct timeval),
	  sds->buf_size, sds->cur_idx, sds->count,
	  buf_size, &new_index, &new_count);
    if (ret_val)
      for (i = 0; i < STRIP_MAX_CURVES; i++)
        if (sds->buffers[i].val)
        {
          ret_val = pack_array
            ((void **)&sds->buffers[i].val, sizeof(double),
		  sds->buf_size, sds->cur_idx, sds->count,
		  buf_size, 0, 0);
          if (ret_val)
            ret_val = pack_array
              ((void **)&sds->buffers[i].stat, sizeof (StatusType),
		    sds->buf_size, sds->cur_idx, sds->count,
		    buf_size, 0, 0);
          if (!ret_val) break;
        }
  }
  if (ret_val)
  {
    sds->buf_size = buf_size;
    sds->cur_idx = new_index;
    sds->count = new_count;
  }
  return ret_val;
}



static int
pack_array      (void   **p,    /* address of pointer */
  size_t nbytes, /* array element size */
  int    n0,     /* old size */
  int    i0,     /* old index */
  int    s0,     /* old count */
  int    n1,     /* new size */
  int    *i1,    /* new index */
  int    *s1)    /* new count */
{
  int   x, y;
  char  *q;

  if ((q = (char *)*p) != NULL)
  {
    if (n1 > n0)        /* new size is greater than old */
    {
      if ((q = (char *)realloc (q, n1*nbytes)))
      {
        /* how many to push to the end? */
        x = s0-i0-1;
        if (x > 0)
          memmove (q+((n1-x)*nbytes), q+((n0-x)*nbytes), x*nbytes);
        if (i1) *i1 = i0;
        if (s1) *s1 = s0;
      }
    }
    else                /* new size is less than old */
    {
      x = (i0+1) - n1; /* num at front of old array which won't now fit */

      if (x > 0)        /* not all elements on [0, i0] will fit on [0, n1] */
      {
        memmove (q, q+(x*nbytes), n1*nbytes);
        if (i1) *i1 = i0 - x;
        if (s1) *s1 = n1;
      }
      else      /* x <= 0 */
      {
        y = i0 + 1;             /* number at front of array */
        x = min (-x, s0-y);     /* x <-- num elem. to pack at end */
              
        if (x > 0)
          memmove (q+((n1-x)*nbytes), q+((n0-x)*nbytes), x*nbytes);
              
        if (i1) *i1 = i0;
        if (s1) *s1 = y + x;
      }
          
      q = (char *)realloc (q, n1*nbytes);
    }
  }
  
  if (q) *p = q;
  return (q != NULL);
}


static int
verify_render_buffer    (RenderBuffer *rbuf, int n)
{
  XSegment      *s;
  int           ret = 1;
  
  if (rbuf->max_segs < n)
  {
    if (!rbuf->segs)
      s = (XSegment *)malloc (n * sizeof (XSegment));
    else s = (XSegment *)realloc (rbuf->segs, n * sizeof (XSegment));

    if (s)
    {
      rbuf->segs = s;
      rbuf->max_segs = n;
    }
    else ret = 0;
  }

  return ret;
}


/* static function for HistoryDump: Albert */
static int findNextTime (struct timeval *tv,struct timeval *result,StripDataSourceInfo *sds)
{
  CurveData *cd;
  int m,i;
  int first=1;

  for (m = 0; m < STRIP_MAX_CURVES; m++)
  {
    if (sds->buffers[m].curve == NULL) continue;
    cd = &sds->buffers[m];
    if(cd->history.n_points<1) continue;
    if(cd->history.fetch_stat!=FETCH_DONE){perror("FETCH_DONE!"); continue;}

    for(i=0;i<cd->history.n_points;i++) 
    {
	if( compare_times(&(cd->history.times[i]),tv) >0) {
	  if (first) {
	    first=0;
	    result->tv_sec= (cd->history.times[i]).tv_sec;
	    result->tv_usec=(cd->history.times[i]).tv_usec;
	    continue;
	  }
	  if(compare_times(&(cd->history.times[i]),result)<0) { 
	    result->tv_sec= (cd->history.times[i]).tv_sec;
	    result->tv_usec=(cd->history.times[i]).tv_usec;
	  }
	}
    }
  }

  return(first);
}

static int printData(struct timeval *time, CurveData *cd, char *val)
{
  int i;
  short first=1;
  for(i=0;i<cd->history.n_points;i++) 
  {
    if(!compare_times(&(cd->history.times[i]),time)) {
	first=0;
	sprintf(val,"%g",cd->history.data[i]);
    }
  }
  if(first) sprintf(val,"%s","N/A");
  return(first);    
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
