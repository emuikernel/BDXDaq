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

#include "StripHistory.h"
#include "StripDataSource.h"
#include <math.h>

/* StripHistoryInfo
 *
 *      Contains instance data for the archive service.
 */
typedef struct _StripHistoryInfo
{
  Strip         strip;
}
StripHistoryInfo;


/* StripHistory_init
 */
StripHistory    StripHistory_init       (Strip strip)
{
  StripHistoryInfo      *shi = 0;

  if (shi = (StripHistoryInfo *)malloc (sizeof(StripHistoryInfo)))
  {
    shi->strip = strip;
  }

  return (StripHistory)shi;
}


/* StripHistory_delete
 */
void    StripHistory_delete     (StripHistory the_shi)
{
  StripHistoryInfo      *shi = (StripHistoryInfo *)the_shi;

  free (shi);
}


/* StripHistory_fetch
 */
FetchStatus     StripHistory_fetch      (StripHistory           the_shi,
                                         char                   *name,
                                         struct timeval         *begin,
                                         struct timeval         *end,
                                         StripHistoryResult     *result,
                                         StripHistoryCallback   callback,
                                         void                   *call_data)
{
#define MAX_ITEMS       1024
#define MY_PI           3.14159265358979323846

  static struct timeval times[MAX_ITEMS];
  static double         data[MAX_ITEMS];
  static short          status[MAX_ITEMS];
  static int            init = 0;

  struct timeval        tv;
  double                x;
  int                   i;

  gettimeofday (&tv, 0);
  fprintf (stdout, "sec: %d, usec: %u\n", tv.tv_sec, tv.tv_usec);

  if (!init)
  {
    for (i = 0; i < MAX_ITEMS; i++)
    {
      x = i / (double)MAX_ITEMS;
      x *= 10 * 2 * MY_PI;              /* 10 complete sine waves over range */
      data[i] = sin (x) * 10;
      status[i] = DATASTAT_PLOTABLE;

      x = subtract_times (&times[i], begin, end);
      x = i * (x / MAX_ITEMS);
      x += time2dbl (begin);
      dbl2time (&times[i], x);
    }

    init = 1;
  }

  /* remember the request range */
  result->t0 = *begin;
  result->t1 = *end;
    
  if ((compare_times (begin, &times[MAX_ITEMS-1]) <= 0) &&
      (compare_times (end, &times[0]) >= 0) &&
      (compare_times (begin, end) <= 0))
  {
    result->times = times;
    result->data = data;
    result->status = status;
    result->n_points = MAX_ITEMS;
    result->fetch_stat = FETCH_DONE;
  }
  else result->fetch_stat = FETCH_NODATA;

  return result->fetch_stat;
#undef MAX_ITEMS
}
 

/* StripHistory_cancel
 */
void    StripHistory_cancel     (StripHistory           the_shi,
                                 StripHistoryResult     *result)
{
}



/* StripHistoryResult_release
 */
void  StripHistoryResult_release    (StripHistory           the_shi,
                                     StripHistoryResult     *result)
{
}
