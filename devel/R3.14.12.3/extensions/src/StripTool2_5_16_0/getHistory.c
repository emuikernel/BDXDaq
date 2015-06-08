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
#ifdef USE_AAPI
#include "AAPI_client.h"
#include "get_AAPI_data.h"
#endif
#ifdef USE_ARCHIVE_RECORD
#include "getArchiveRecord.h"
#endif
#include "getHistory.h"
#define DEBUG  0
#define DEBUG1 0
#define DEBUG2 0

unsigned long getHistory(StripHistory     the_shi,
		  char*            name,
		  struct timeval*  begin,
		  struct timeval*  end,
		  struct timeval** times,
		  short**          status,
		  double**         data,
		  unsigned long *  count)
{
  struct timeval right_endpoint;   /* right end for AAPI request */
  unsigned long commonCount=0;
  int i;

  double *returnedDataIOC =NULL;
  struct timeval *returnedTimeIOC=NULL;
  short *returnedStatusIOC=NULL;
  long returnedCountIOC=0;

  double *returnedDataAAPI=NULL;
  struct timeval *returnedTimeAAPI=NULL;
  short *returnedStatusAAPI=NULL;
  long returnedCountAAPI=0;

  *count=0;
  right_endpoint.tv_sec = end->tv_sec;
  right_endpoint.tv_usec = end->tv_usec;

#ifdef   USE_ARCHIVE_RECORD
  if(getArchiveRecord(name,begin,end,
		     REQUEST_MODE_CONTINUE ,&returnedTimeIOC, 
		     &returnedDataIOC, &returnedStatusIOC,
		     &returnedCountIOC,&needMoreData) != 0)
    { 
      if(DEBUG1) fprintf(stderr,"%s:getArchiveRecord Error\n",name);
      returnedCountIOC=0;/*????      return (-1);  */
    }

  if(returnedCountIOC>0) 
    {
      if(compare_times(&returnedTimeIOC[0],&right_endpoint) <=0) 
	{
	  right_endpoint.tv_sec = returnedTimeIOC[0].tv_sec;
	  right_endpoint.tv_usec= returnedTimeIOC[0].tv_usec;
	  commonCount=returnedCountIOC;
	}
    }

  if(DEBUG) { 
    printf("name=%s;returnedCountIOC=%ld\n",name,returnedCountIOC);
    printf("IOC FROM=%s",ctime(&(begin->tv_sec)));
    printf("IOC TO  =%s",ctime(&(  end->tv_sec)));    
    if(DEBUG2) for(i=0;i<returnedCountIOC;i++)  
      printf("IOC=%s",ctime(&((returnedTimeIOC)[i].tv_sec)));
  }
#endif
#if defined(USE_AAPI) || defined(USE_CAR)
  if((needMoreData)&&(begin->tv_sec < right_endpoint.tv_sec)) 
    { 
if(DEBUG1) printf("history req is here\n");
#ifdef USE_AAPI
      if(get_AAPI_data(name,begin ,&right_endpoint,
		       &returnedTimeAAPI, 
		       &returnedStatusAAPI, &returnedDataAAPI,
		       &returnedCountAAPI) != 0)
#endif
#ifdef USE_CAR
      if(get_CAR_data(the_shi,name,begin ,&right_endpoint,
		       &returnedTimeAAPI, 
		       &returnedStatusAAPI, &returnedDataAAPI,
		       &returnedCountAAPI) != 0)
#endif
	{ 
	  if(DEBUG) fprintf(stderr,"%s:getArchiveAPI Error\n",name);
	  returnedCountAAPI=0; /* ???? return (-1); */
	}
      
      if(DEBUG1) {
	printf("returnedCountAAPI=%ld\n",returnedCountAAPI);
	printf("AAPI FROM=%s",ctime(&(begin->tv_sec)));
	printf("AAPI TO  =%s",ctime(&(right_endpoint.tv_sec)));    
	if(DEBUG2) for(i=0;i<returnedCountAAPI;i++)  
	  printf("AAPI=%s",ctime(&((returnedTimeAAPI)[i].tv_sec)));
      }
      
      commonCount += returnedCountAAPI;
    }
  else { if(DEBUG1) printf("don't need history req\n");}
#endif  /* USE_AAPI || USE_CAR */

  if(commonCount>0) 
    {
      *data  =(double *)calloc(commonCount,sizeof(double));
      *status=(short *)  calloc(commonCount,sizeof(short));
      *times =(struct timeval *)calloc(commonCount,sizeof(struct timeval));
      if((*data == NULL) ||(*times == NULL) ||  (*status == NULL))
      {
	fprintf(stderr,"can't alloc %ld answers\n",commonCount);
	if(*data)   free (*data);
	if(*times)   free (*times);
	if(*status) free (*status);
	if (returnedDataAAPI)   free (returnedDataAAPI);
	if (returnedStatusAAPI) free (returnedStatusAAPI);
	if (returnedTimeAAPI)   free (returnedTimeAAPI);
	if (returnedDataIOC)    free (returnedDataIOC);
	if (returnedStatusIOC)  free (returnedStatusIOC);
	if (returnedTimeIOC)    free (returnedTimeIOC);
	return (-1);
      }

      if(returnedCountAAPI>0) 
	{
	  memcpy(*data,  returnedDataAAPI,  returnedCountAAPI*(sizeof(double)));
	  memcpy(*status,returnedStatusAAPI,returnedCountAAPI*(sizeof(short)));
	  memcpy(*times,  returnedTimeAAPI,  returnedCountAAPI*(sizeof(struct timeval)));
	}

      if(returnedCountIOC>0) {
	memcpy(&((*data)  [returnedCountAAPI]),  returnedDataIOC,
	       returnedCountIOC*(sizeof(double)));
	memcpy(&((*status)[returnedCountAAPI]),returnedStatusIOC,
	       returnedCountIOC*(sizeof(short)));
	memcpy(&((*times)  [returnedCountAAPI]),  returnedTimeIOC,
	       returnedCountIOC*(sizeof(struct timeval)));

      }
      /* ATTENTION: Strip status is no CA status! */
      for(i=0;i<(int)commonCount;i++) (*status)[i] |= DATASTAT_PLOTABLE;

    }

  if(DEBUG1) {
    printf("commonCount=%ld\n",commonCount);
    printf("COM FROM=%s",ctime((const time_t *)&(begin->tv_sec)));
    printf("COM TO  =%s",ctime((const time_t *)&(end->tv_sec)));    
    if(DEBUG2) for(i=0;i<(int)commonCount;i++)  
      printf("TIME=%s",ctime((const time_t *)&((*times)[i].tv_sec)));
  }
  
  if (returnedDataAAPI)   free (returnedDataAAPI);
  if (returnedStatusAAPI) free (returnedStatusAAPI);
  if (returnedTimeAAPI)   free (returnedTimeAAPI);
  if (returnedDataIOC)    free (returnedDataIOC);
  if (returnedStatusIOC)  free (returnedStatusIOC);
  if (returnedTimeIOC)    free (returnedTimeIOC);
  *count=commonCount;
  
  return (0);
}



