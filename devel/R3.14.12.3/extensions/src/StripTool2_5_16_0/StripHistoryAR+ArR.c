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
#include "getHistory.h"
#include <time.h>

#define DEBUG 0

/*#ifdef USE_AAPI TODO */
extern char **algorithmString;
extern int algorithmLength;

/* #endif */

/* StripHistory_init
 */
StripHistory    StripHistory_init       (Strip strip)
{
  StripHistoryInfo      *shi = 0;

  if ((shi = (StripHistoryInfo *)malloc (sizeof(StripHistoryInfo))))
  {
    shi->strip = strip;
  } 
  else
    {
      perror("StripHistory_init: can't allocate memory");
      exit(1);
    }
#ifdef USE_AAPI
  if(AAPI_init() !=0 ) 
    {
     fprintf(stderr,"StripHistory_init: can't init AAPI\n");
     exit(1);
    }

  if(extractAAPIfilterList(&algorithmString,&algorithmLength) != 0) 
    {
      algorithmLength=0;
      fprintf(stderr,"StripHistory_init: can't extractAAPIfilterList\n"); 
      exit(1);
    }
#endif
#ifdef USE_CAR
  if(CAR_init(&(shi->archiverInfo)) !=0 ) 
    {
     fprintf(stderr,"StripHistory_init: can't init CAR\n");
     exit(1);
    }
#endif
  return (StripHistory)shi;  
}

/* StripHistory_delete
 */
void    StripHistory_delete     (StripHistory the_shi)
{
  StripHistoryInfo      *shi = (StripHistoryInfo *)the_shi;

#ifdef USE_AAPI
  AAPI_free();
#endif
#ifdef USE_CAR
  CAR_delete(shi);
#endif
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

  unsigned long err;

  struct timeval *times=NULL;
  short  *status=NULL;
  double *data=NULL;
  unsigned long count=0;
  
  result->t0 = *begin;
  result->t1 = *end;
  result->n_points = 0;
  result->fetch_stat=FETCH_NODATA;
  
  if((err=getHistory(the_shi,name,begin,end,&times,&status,&data,&count)) != 0) 
    {
      fprintf(stderr,"err=%ld:bad getHistory; no goodData \n",err);
      return (FETCH_NODATA);
    }
  if(count < 1)
    {
      if(DEBUG) fprintf(stderr,"getHistory; no goodData count=%lu\n",count);
      return (FETCH_NODATA);
    }
  
  if ((compare_times (begin, &times[count-1]) <= 0) &&
      (compare_times (end, &times[0]) >= 0) &&
      (compare_times (begin, end) <= 0))
    {
      result->times      = times;
      result->data       = data;
      result->status     = status;
      result->n_points   = count;
      result->fetch_stat = FETCH_DONE;
    }
  else 
    {
      if(DEBUG) 
	{
	  printf("StripHistory_fetch: Compare problem \n");
	}
      result->fetch_stat = FETCH_NODATA;
    }
  if(DEBUG) printf("%s: StripHistory_fetch: OK\n",name);
  return result->fetch_stat;
}

 /* StripHistory_cancel
 */
void    StripHistory_cancel     (StripHistory           the_shi,
                                 StripHistoryResult     *result)
{
}
/* StripHistoryResult_release
 */
void  StripHistoryResult_release    (StripHistory           BOGUS(the_shi),
                                     StripHistoryResult     *result)
{
#ifdef USE_AAPI
AAPI_Result_release(result);
#endif
#ifdef USE_CAR
CAR_Result_release(result);
#endif
}
