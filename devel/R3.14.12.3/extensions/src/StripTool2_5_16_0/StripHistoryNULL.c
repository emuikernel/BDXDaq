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

/* StripHistoryInfo
 *
 *      Contains instance data for the archive service.
 */
typedef struct _StripHistoryInfo
{
  Strip         strip;
}
StripHistoryInfo;

static StripHistoryInfo shi;


/* StripHistory_init
 */
StripHistory    StripHistory_init       (Strip BOGUS(1))
{
  return (StripHistory)&shi;
}


/* StripHistory_delete
 */
void    StripHistory_delete             (StripHistory BOGUS(1))
{
}


/* StripHistory_fetch
 */
FetchStatus     StripHistory_fetch      (StripHistory           BOGUS(1),
                                         char                   *BOGUS(2),
                                         struct timeval         *t0,
                                         struct timeval         *t1,
                                         StripHistoryResult     *result,
                                         StripHistoryCallback   BOGUS(3),
                                         void                   *BOGUS(4))
{
  result->t0 = *t0;
  result->t1 = *t1;
  result->n_points = 0;
  result->fetch_stat = FETCH_NODATA;

  return result->fetch_stat;
}
 

/* StripHistory_cancel
 */
void    StripHistory_cancel     (StripHistory           BOGUS(1),
                                 StripHistoryResult     *BOGUS(2))
{
}



/* StripHistoryResult_release
 */
void  StripHistoryResult_release    (StripHistory           BOGUS(1),
                                     StripHistoryResult     *BOGUS(2))
{
}
