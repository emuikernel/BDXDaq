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

/* -------------------------------------------------------------------------
 *
 * getArchiveRecord.c - get Data from archive record.
 *
 *      Author:  Albert Kagarmanov (kagarman@mail.desy.de)
 *      Original idea from V.Romanowski   
 *      Date:   1998 - 1999
 * 
 *      Experimental Physics and Industrial Control System (EPICS)
 *
 *-------------------------------------------------------------------------*/

/*
 *	getArchiveRecord.c		
 *     
 *     getArchiveData () routine prepares some arrays of value and time
 *      existed in archive record.
 *
 *      getRawDataFromRecord() 
 *          1. get IDs of 'record_name.NVAL'
 *                        'record_name.TIM'
 *                        'record_name.VAL'
 *          2. get arrays of time and data
 *      
 *      orderArrays()
 *          sort the arrays of time and data according to time
 */

#include <dirent.h>
#include <stdio.h>
#if defined(SYSV) || defined(SVR4) || defined(SOLARIS)
#include <string.h>
#else
#include <strings.h>
#endif
#include <cadef.h>
#include <caerr.h>
#include <db_access.h>
#include <tsDefs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <memory.h>
#define TS_EPOCH_SEC_PAST_1970   7305*86400
/*#define TS_EPOCH_SEC_PAST_1970 6940*86400  * new Epics realese do not support it */
#define DEBUG 0
#define DEBUG1 0 
#include "StripHistory.h"
#include "getArchiveRecord.h"
#define INFORM_INTERNAL_ERROR() fprintf(stderr,"internal error(%s:%d)\n",__FILE__, __LINE__)
#define READ_VALUE_DELAY        2.0 /* 0.7 */
#define NAME_SIZE               64

/*----------------------------------------------------------*/ 
#define CA_PEND(messg)                                       \
  {                                                          \
    int my_status = ca_pend_io(READ_VALUE_DELAY);            \
    if (my_status == ECA_TIMEOUT) {                          \
       if (ca_pend_io(READ_VALUE_DELAY) != ECA_NORMAL) {     \
         SEVCHK (my_status,messg)                            \
         RETURN_ERROR;                                       \
       }                                                     \
    }                                                        \
    else if (my_status != ECA_NORMAL) {                      \
    /*if(my_status != 210) */  {RETURN_ERROR;}               \
    }                                                        \
  }                                                          \
/*----------------------------------------------------------*/


/*-------------------------------------------------------*/ 
#define _CA_PEND(messg)                                    \
  {                                                       \
    int my_status = ca_pend_io(READ_VALUE_DELAY);         \
    if (my_status != ECA_NORMAL) {                        \
       if(my_status <50) {SEVCHK (my_status,messg)}       \
       else fprintf(stderr,"my_staus =%d\n",my_status); \
       /*RETURN_ERROR;*/                                  \
    }                                                     \
  }                                                       \
/*-------------------------------------------------------*/

#define ARCHIVE_RECORD_TAG__NUMBER_OF_ELEMENTS ".NVAL"
#define ARCHIVE_RECORD_TAG__TIME_ARRAY         ".TIM"
#define ARCHIVE_RECORD_TAG__VAL_DATA           ".VAL"
#define ARCHIVE_RECORD_TAG__FLUSH_DATA         ".FLSH"

#define PRINTF(messg) fprintf(stderr,"%s:%d\n%s\n",__FILE__, __LINE__, messg)
 
typedef struct _channelIds {
    struct _channelIds *next;
    char               *name;
    chid               nvalId;
    chid               timId;
    chid               valId;   
    chid               flushId; 
}ChannelIds;

static ChannelIds * listChannelIds;

/* forward declaration
 */

static int getChannelIds(
      char        *name,       /* record name */  
      long        requestMode,
      ChannelIds  **IDs);      /* record IDs */

static int getRawDataFromRecord (
      ChannelIds  *IDs,    
      long        requestMode,
      long        **timeData, /* returned time data pointer */
      double       **valData,  /* returned val data pointer */
      long        **statusData,
      long        *count);    /* how much elements are in array */

static int orderArrays (
      long  *timeData,
      double *valData,
      long  *statusData,
      long  count,
      char *name);   

/*
 *********************************************************************
 * let's start hard coding routines in C..
 *********************************************************************
 */

static void destroyChannelId (ChannelIds  *IDs)
{
  if (IDs) {
    free (IDs->name);
    ca_clear_channel(IDs->nvalId);
    ca_clear_channel(IDs->timId);
    ca_clear_channel(IDs->valId);
    ca_clear_channel(IDs->flushId); 
    free((char*)IDs);
  }    
}

int getArchiveRecord(
      char   *name,
      struct timeval   *from,
      struct timeval   *to,
      long   requestMode,
      struct timeval  **returnedTime,
      double **returnedData,
      short  **returnedStatus,
      long   *returnedCount,
      short *needMoreData)
{

  long   fromTime =from->tv_sec ;
  long   toTime   =  to->tv_sec ;

  u_long          count;
  long        * timeData;
  double       * valData;
  long         *statusData;
  ChannelIds  * IDs;
  int           i;
  char        * archName;
  int           len;
  
  long          tmp_time;
  struct tm *tm;

  u_long tmpCount;

  u_long  leftFit;

  *needMoreData=1;

  if (name == NULL) {
    PRINTF("invalid parameters");
    return ERROR;
  }
  archName = name;

  /* let's check whether cnannel name has ``.VAL'' like sufix
   */
  for (len = strlen(name); len >=0; len--)
    if (name[len] == '.')
      break;
  if (len <= 0) {
    /* if not, than check for ``_h'' sufix 
     */
    len = strlen(name);    
    if (name[len-2] != '_' || name[len-1] != 'h')
      {
	archName = (char*) malloc (sizeof(char) *(len + 3));
	if (archName == NULL) {
	  PRINTF("cannot allocate memory");
	  return ERROR;
	}	
	strncpy(archName, name,len);
	archName[len++] = '_';
	archName[len++] = 'h';
	archName[len] = '\0';
      }
  } 
  else {
    /* if ``.VAL'' sufix exists than do not take it in to acount
     */
    if (name[len-2] == '_' && name[len-1] == 'h')
      len -= 2;    
    archName = (char*) malloc (sizeof(char) * (len + 3));
    if (archName == NULL) {
      PRINTF("cannot allocate memory");
      return ERROR;
    }
    strncpy(archName, name,len);
    archName[len++] = '_';
    archName[len++] = 'h';
    archName[len]   = '\0';
  }
  if (getChannelIds(archName, requestMode, &IDs)) {
    if (archName != name) free(archName);
    if(DEBUG1) printf("%s: getChannelIds error\n",name); 
    *needMoreData=0;
    return ERROR;
  }
  
  
  if(getRawDataFromRecord(IDs,requestMode,
			  &timeData,&valData,
			  &statusData,&count)) 
    {
      if (requestMode == REQUEST_MODE_ONE_SHOT) {
	destroyChannelId (IDs);
      }
      if (archName != name) free(archName);
      if(DEBUG1) printf("%s: getRawDataFromRecord error\n",name); 
      return ERROR;
    } 
  
  if (orderArrays (timeData, valData,statusData, count,name)== ERROR) {
    if (requestMode == REQUEST_MODE_ONE_SHOT) {
      destroyChannelId (IDs);
    }    
    if(DEBUG1) printf("%s:  orderArrays  error\n",name); 
    if (archName != name) free(archName);
    if (timeData)   free ((char*)timeData);    
    if (valData)    free ((char*)valData);     
    if (statusData) free ((char*)statusData); 
    return ERROR;
  }
  
  for(i=0;i<count;i++)  timeData[i]+=TS_EPOCH_SEC_PAST_1970; 
  
#ifdef IOC_SUMMER_WINTER_TIME 
  /*
    VME doesn't change your time in summer but
    Unix  does ...
    it's means that 
    in  summer ---- VME_time = Unix_time-3600 
    in winter       VME_time = Unix_time
    */

    for(i=0;i<count;i++) 
      {
	tmp_time=timeData[i];
	tm = localtime( &tmp_time );
	if(tm->tm_isdst > 0)  /* It's mean Summer */
	  {
	    timeData[i] =  timeData[i] + 3600;
	  }
      }
    if(DEBUG) if(count>0) {
      printf("SummerFlag is %d(positive means Summer)\n",tm->tm_isdst);
     for(i=0;i<count;i++) 
      {
       printf("%s",ctime(&(timeData[i])));
      }
       printf("FROM=%s",ctime(&(fromTime)));
       printf("TO  =%s",ctime(&(toTime)));
    }
#endif

    leftFit=0;

    for (i=0;i<count;i++)   
      { 
	if(timeData[i]<fromTime) {
	 leftFit=1; 
	 *needMoreData=0;
	 break;
	}
      }

    tmpCount=0;
    for (i=0;i<count;i++)   
      { 
	if ( (timeData[i]>fromTime) && (timeData[i]<toTime) )
	  tmpCount++;
      }

    tmpCount += leftFit ;

   if(DEBUG) printf("tmpCount=%ld\n",tmpCount);

    if(tmpCount==0) {
    if (archName != name) free(archName);
    if (timeData)   free ((char*)timeData);    
    if (valData)    free ((char*)valData);     
    if (statusData) free ((char*)statusData);
    *returnedCount=0;
    return(OK);
    }

    if( (*returnedData=(double *)calloc(tmpCount,sizeof(double)))==NULL) 
      {
      fprintf(stderr,"can't alloc %ld double\n",tmpCount);
      if (archName != name) free(archName);
      if (timeData)   free ((char*)timeData);    
      if (valData)    free ((char*)valData);     
      if (statusData) free ((char*)statusData);
      return (ERROR);
    }

    if( (*returnedTime=(struct timeval *)calloc(tmpCount,
			    sizeof(struct timeval)))==NULL) 
      {
      fprintf(stderr,"can't alloc %ld timeval\n",tmpCount);
      if(*returnedData) free((char*) *returnedData);
      if (archName != name) free(archName);
      if (timeData)   free ((char*)timeData);    
      if (valData)    free ((char*)valData);     
      if (statusData) free ((char*)statusData);
      return (ERROR);
    }

    if( (*returnedStatus=(short *)calloc(tmpCount,sizeof(short)))==NULL) 
      {
      fprintf(stderr,"can't alloc %ld long\n",tmpCount);
      if(*returnedData) free((char*) *returnedData);
      if(*returnedTime) free((char*) *returnedTime);
      if (archName != name) free(archName);
      if (timeData)   free ((char*)timeData);    
      if (valData)    free ((char*)valData);     
      if (statusData) free ((char*)statusData);
      return (ERROR);
    }

    *returnedCount=tmpCount;
    tmpCount=0;

    if(leftFit) {
      for (i=0;i<count;i++)   
	{ 
	  if(timeData[i]<fromTime) {
	    (*returnedData)  [tmpCount]=valData[i];
	    (*returnedStatus)[tmpCount]=statusData[i];
	    ((*returnedTime)  [tmpCount]).tv_sec= from->tv_sec;
	    ((*returnedTime)  [tmpCount]).tv_usec=from->tv_usec;; 
	  }
	  else break;
	}
      tmpCount=1;
    }

    for (i=0;i<count;i++)   
      {
	if ( (timeData[i]>fromTime) && (timeData[i]<toTime) ) 
	  {
	    (*returnedData)  [tmpCount]=valData[i];
	    (*returnedStatus)[tmpCount]=statusData[i];
	   ((*returnedTime)  [tmpCount]).tv_sec=timeData[i];
	   ((*returnedTime)  [tmpCount]).tv_usec=0;
	    tmpCount++;
	  }
      }

   if (archName != name) free(archName);
   if (timeData)   free ((char*)timeData);    
   if (valData)    free ((char*)valData);     
   if (statusData) free ((char*)statusData);
   return OK;
}


static int getChannelIds(
      char        * name,
      long          requestMode,
      ChannelIds ** IDs)
{
  register ChannelIds * ids = NULL;
  char                  string[NAME_SIZE];
  int                   status;

  if (strlen(name) > NAME_SIZE -6) {
    PRINTF("name is very long");
    return ERROR;
  }

  /* check static list of preexeisted ChannelIds for the name
   */
  if (requestMode == REQUEST_MODE_ONE_SHOT && listChannelIds) 
  {
    /* remove node from static list
     */
   if (strcmp(name, listChannelIds->name) == 0) 
   {
     *IDs = listChannelIds;
     listChannelIds = listChannelIds->next;
     return OK;
   }
   else 
   {
     register ChannelIds *p, *n;
     for (p = listChannelIds, n = p->next; n != NULL; p = n, n = n->next) {
       if (strcmp(name, n->name) == 0) {
	 p->next = n->next;
	 *IDs = n;
	 return OK;
       }
     }
   }
  }
  /* if the node is exists in the list, return it.
   */
  for (ids = listChannelIds; ids != NULL; ids = ids->next) {
    if (strcmp(name, ids->name) == 0){
     *IDs = ids;
      return OK;
    }
  }

  /* make a new ChannelIds
   */
  ids = (ChannelIds *) malloc (sizeof (ChannelIds));
  if (ids == NULL) {
    PRINTF("cannot allocate memory");
    return ERROR;
  }
  
  /* copy name
   */
  ids->name = (char*) malloc (sizeof(char) * (strlen(name)+1));
  if (ids->name == NULL) {
    free ((char*)ids);
    PRINTF("cannot allocate memory");
    return ERROR;
  }
  
  strcpy (ids->name, name);
  
  /* get number of element in time&data arrays
   */
  strcpy(string, name);
  strcat(string, ARCHIVE_RECORD_TAG__NUMBER_OF_ELEMENTS); 

  status = ca_search(string, &ids->nvalId);
  if (status != ECA_NORMAL) {
    if (ids->name) free((char*)ids->name); 
    if (ids) free((char*)ids);             
    fprintf(stderr,"%s:bad ca_search\n",string);
    return ERROR;
  }
  
#define RETURN_ERROR                     \
  if(DEBUG1) fprintf(stderr,"NUMBER_OF_ELEMENTS pend_io error\n");\
 ca_clear_channel(ids->nvalId);          \
 if (ids->name) free((char*)ids->name);  \
 if (ids) free((char*)ids);              \
 return ERROR

  CA_PEND("ca_pend_io failure");

#undef RETURN_ERROR

  /* array of time
   */
  strcpy(string, name);
  strcat(string, ARCHIVE_RECORD_TAG__TIME_ARRAY);

  status = ca_search(string, &ids->timId);
  if (status != ECA_NORMAL) {
    ca_clear_channel(ids->nvalId);
    if (ids->name) free((char*)ids->name); 
    if (ids) free((char*)ids);             
    fprintf(stderr,"%s:bad ca_search\n",string);
    return ERROR;
  }

#define RETURN_ERROR                     \
  if(DEBUG1) fprintf(stderr,"TIME_ARRAY pend_io error\n");\
 ca_clear_channel(ids->nvalId);          \
 ca_clear_channel(ids->timId);           \
 if (ids->name) free((char*)ids->name);  \
 if (ids) free((char*)ids);              \
 return ERROR

  CA_PEND("ca_pend_io failure");
 
#undef RETURN_ERROR

  /* array of data
   */
  strcpy(string, name);
  strcat(string, ARCHIVE_RECORD_TAG__VAL_DATA);
  
  status = ca_search(string, &ids->valId);
  if (status != ECA_NORMAL) {
    ca_clear_channel(ids->nvalId);
    ca_clear_channel(ids->timId);
    if (ids->name) free((char*)ids->name); 
    if (ids) free((char*)ids);             
    fprintf(stderr,"%s:bad ca_search\n",string);
    return ERROR;
  }
  
#define RETURN_ERROR                     \
  if(DEBUG1) fprintf(stderr," VAL_DATA pend_io error\n");\
 ca_clear_channel(ids->nvalId);          \
 ca_clear_channel(ids->timId);           \
 ca_clear_channel(ids->valId);	         \
 if (ids->name) free((char*)ids->name);  \
 if (ids) free((char*)ids);              \
 return ERROR

  CA_PEND("ca_pend_io failure");

#undef RETURN_ERROR  

 /* Flush 
  */
 strcpy(string, name);
  strcat(string, ARCHIVE_RECORD_TAG__FLUSH_DATA);

  status = ca_search(string, &ids->flushId);
  if (status != ECA_NORMAL) {
    ca_clear_channel(ids->nvalId);
    ca_clear_channel(ids->timId);
    ca_clear_channel(ids->valId);
    if (ids->name) free((char*)ids->name); 
    if (ids) free((char*)ids);             
    fprintf(stderr,"%s:bad ca_search\n",string);
    return ERROR;
  }

  
#define RETURN_ERROR                     \
  if(DEBUG1) fprintf(stderr,"FLUSH_DATA pend_io error\n"); \
 ca_clear_channel(ids->nvalId);          \
 ca_clear_channel(ids->timId);           \
 ca_clear_channel(ids->valId);	         \
 ca_clear_channel(ids->flushId);	 \
 if (ids->name) free((char*)ids->name);  \
 if (ids) free((char*)ids);              \
 return ERROR

  CA_PEND("ca_pend_io failure");

#undef RETURN_ERROR  

  if (requestMode == REQUEST_MODE_CONTINUE) {
    ids->next = listChannelIds;
    listChannelIds = ids;
  }

  if (IDs) *IDs = ids;

  return OK;
}


static int getRawDataFromRecord (
      ChannelIds   *IDs,        /* records' names */
      long         requestMode,
      long         **timeData,  /* returned time data pointer */
      double       **valData,   /* returned val data pointer */
      long         **statusData,
      long         *count)      /* how much elements are in array */
{
  int         i, status;
  long true=1; 
#define RETURN_ERROR return ERROR

  if((timeData==NULL)||(valData==NULL)||(statusData==NULL)||(count==NULL)){
    INFORM_INTERNAL_ERROR();
    return ERROR;
  }

  *timeData = NULL;
  *valData  = NULL;
  *statusData  = NULL;
  status = ca_put (DBF_LONG, IDs->flushId, &true);
  if (status != ECA_NORMAL) {
    RETURN_ERROR;
  }
 
  CA_PEND("ca_pend_io failure"); /* need ca_pend before ca_get Albert. */

  status = ca_get (DBF_LONG, IDs->nvalId, count);
  if (status != ECA_NORMAL) {
    RETURN_ERROR;
  }
  CA_PEND("ca_pend_io failure");
 
  /* if number is not proper
   */

  if (*count < 1) {
    *count = 0;
    ca_clear_channel(IDs->nvalId);
    PRINTF("archive record has 0 size arraries");
    return ERROR;
  }

  if (*count > 10240){
    fprintf(stderr,"size=%ld(dec) %lx (hex) %d\n", *count,*count,*count);
    PRINTF("archive record has so big size arraries");
    *count=0;
    return ERROR;
  }

  
  if(DEBUG) fprintf(stderr,"count=%ld\n",*count);

  /* allocate memory for arrays from record
   */

  *timeData =   (long*)    calloc (*count, sizeof (long));
  *valData =    (double*)  calloc (*count, sizeof (double));
  *statusData = (long*)    calloc (*count, sizeof (long));

  /* if there is no more memory
   */
  if ( (*valData == NULL) || (*timeData == NULL) ||(*statusData ==NULL)  ) 
    {
      ca_clear_channel(IDs->nvalId);
 
      if (*valData)    free ((char*)valData);
      if (*timeData)   free ((char*)timeData);
      if (*statusData) free ((char*)statusData);
      *timeData    = NULL;
      *valData     = NULL;
      *statusData  = NULL;
      PRINTF("cannot allocate memory");
      fprintf(stderr,"count=%d\n",*count); 
      return ERROR;
    }
  
  /* read the arrays
   */
  ca_array_get(DBF_LONG, (unsigned long)*count, IDs->timId, *timeData);
  CA_PEND("ca_pend_io failure");
  ca_array_get(DBF_DOUBLE,(unsigned long)*count, IDs->valId, *valData);
  CA_PEND("ca_pend_io failure");
  
  for(i=0;i < *count; i++) {
    (*statusData)[i]=0L;  /* Now we don't handle status. Albert. */
  }

  /* For new record we must read only first elements (until 
   *timeData[i] > 0)  - after that we have garbage & *timeData[i]==0. Albert.
   */
  for(i=0;i < *count; i++) 
    {
      if ( (*timeData)[i] == 0) 
        {
	  if(DEBUG)fprintf(stderr,
		    "new record has wrong position =%d (count=%ld)\n",
		    i,*count);
	  *count=i-1;
	  break;
        } 
    }
  /* end of new record checking */
  
  return OK;
#undef RETURN_ERROR
}


static int orderArrays (
      long   *timeData,
      double  *valData,
      long    *statusData,
      long    count,
      char *name)     
{
  register int  i;
  int           start;
  long    timeTmp; 
  double  valTmp;  
  long    statusTmp;
  
  /* search for element less then next one, that will be last added one
   */
  for (i = 0; i < count-1; i++) {
    if (timeData[i] > timeData[i+1])
      break;
  }
  /* ****** for DESY archive record "one permutation" error ************ */
#if 1  
  if (i<count-2) 
    {
      if( timeData[i+2] > timeData[0] )   
	{  
	  /* permutation happen!!!             */
	  /* We'll permutate a[i+1] and a[i+2] */
	  
	  fprintf(stderr,"Warning:%s-permutation\n",name);/* for DEBUG */ 
	  timeTmp=timeData[i+1];
	  timeData[i+1]=timeData[i+2];
	  timeData[i+2]=timeTmp;
	  
	  valTmp=valData[i+1];
	  valData[i+1]=valData[i+2];
	  valData[i+2]=valTmp;
	  
	  statusTmp=statusData[i+1];
	  statusData[i+1]=statusData[i+2];
	  statusData[i+2]=statusTmp;
	  
	  
	  /* ***** try find start again ***************** */
	  
	  for (i = 0; i < count-1; i++) 
	    {
	      if (timeData[i] > timeData[i+1])
		break;
	    }
	  
	}
    }
#endif
/* ****** end for DESY archive record "one permutation" error ************ */
  
  
  /* element next for last one added whould be start element(IMHO)
   */
  start = i < count-1 ? i+1 : 0;
  
  if(start != 0) fprintf(stderr,
		 "Waring:%s-history buffer is really ring buffer\n",name); 

  /* if start element is not the first in array, reordering arrays
   */
  if (start) 
    {
      /* allocate tmp arrays 
       */
      long  * tmpTime = (long*) calloc (count, sizeof(long));
      double * tmpVal = (double*) calloc (count, sizeof(double));
      long  * tmpStatus = (long*) calloc (count, sizeof(long));
      if((tmpTime == NULL)||(tmpVal == NULL)|| (tmpStatus == NULL)) 
	{
	  if (tmpTime) free ((char*)tmpTime);
	  if (tmpVal)  free ((char*)tmpVal);
	  if (tmpStatus)  free ((char*)tmpStatus);
	  PRINTF("cannot allocate memory");
	  return ERROR;
	}
      
      /* copy parts from original to tmp
       */
      memcpy ((char*)tmpTime, (char*)&timeData[start], 
	      (count - start) * sizeof (long));
      memcpy ((char*)tmpVal,  (char*)&valData[start], 
	      (count - start) * sizeof (double));
      memcpy ((char*)tmpStatus, (char*)&statusData[start], 
	      (count - start) * sizeof (long));
      
      memcpy ((char*)&(tmpTime[count - start]), (char*)timeData, 
	      start * sizeof (long)); 
      memcpy ((char*)&(tmpVal[count - start]),  (char*)valData, 
	      start * sizeof (double)); 
      memcpy ((char*)&(tmpStatus[count - start]), (char*)statusData, 
	      start * sizeof (long)); 
      
      /* copy back
       */
      memcpy((char*)timeData, (char*)tmpTime, count * sizeof (long));
      memcpy((char*)valData, (char*)tmpVal, count * sizeof (double));
      memcpy((char*)statusData, (char*)tmpStatus, count * sizeof (long));
      
      /* free and bye
       */
      free ((char*) tmpVal);
      free ((char*) tmpTime);
      free ((char*) tmpStatus);
    }
  
  return OK;
}


#if 0

static int intervalChecking(name, host,rpc_port,fromTime,toTime,algorithm,
timeData, valData, statusData,count,
returnedTime,returnedData,returnedStatus,returnedCount, max_count)
char *name;
char *host;
long rpc_port;
long fromTime,toTime, algorithm;
long *timeData;
double *valData;
long *statusData;
int count;
long   *returnedTime;
double *returnedData;
long   *returnedStatus;
long   *returnedCount;
long max_count;
{

  int i, begin,end;
  long current;
  int currentCount;
  int diskCount;
  int zero;
  int tmp;
  
  zero=0;
  diskCount=0;
  
  if(fromTime < timeData[0])                    /*  some data in a disk */ 
    {
      
      if(toTime<timeData[0]) /* no data in history_record => disk data only */
	{
	   if (DEBUG) printf("1)DISK REQ!!!port=%ld\n",rpc_port); 
	  DiskRequest(host,rpc_port,name,fromTime, 
		      toTime, algorithm,&returnedTime[0], &returnedData[0],
		      &returnedStatus[0],&diskCount);
	  
	  *returnedCount=diskCount;
	  return OK;
	}
    
      /* some data in history_record and (also?) some in a disk */

      if (DEBUG) printf("2)DISK REQ!!!port=%ld\n",rpc_port);

      DiskRequest(host,rpc_port,name,fromTime, 
		  timeData[0], algorithm,&returnedTime[0], &returnedData[0],
		  &returnedStatus[0],&diskCount);
      
      if (DEBUG) printf("RPC is OK\n");
    }
  

  currentCount=diskCount;
  tmp=0;            /* if tmp=1 - it's mean left fitting */
  
  for (i=0;i<count;i++)   
    {
      if(timeData[i]>fromTime) break;
      returnedTime[currentCount]=fromTime;
      returnedData[currentCount]=valData[i];
      returnedStatus[currentCount]=statusData[i];
      tmp=1;
    }
  
  
  if (tmp &&(currentCount>0)) 
    {
      if(returnedTime[currentCount-1] == returnedTime[currentCount])  
	tmp=0;  /* else it's means 2 fitting from disk ans history_record */
      if(DEBUG) printf("Double fitting!!!\n"); 
    }
  
  currentCount+=tmp; /* if tmp=1 - left fitting */
  
  
  begin=i;
  for (i=begin;i<count;i++) 
    {
      if((timeData[i]>toTime) && (algorithm != 1L) )
	{ 
	  if(DEBUG)  printf("left fit alg =%ld\n",algorithm);
	  returnedData[currentCount]=valData[i];
	  returnedTime[currentCount]=toTime;
	  returnedStatus[currentCount]=statusData[i];
	  currentCount++;
	  break;
	} 
      returnedData[currentCount]=valData[i];
      returnedTime[currentCount]=timeData[i];
      returnedStatus[currentCount]=statusData[i];
      
      currentCount++;
    }
  
  if(DEBUG) 
    {
      fprintf(stderr,"last  Time=%s",ctime(&returnedTime[currentCount-1]));
    }
  
  if (currentCount > max_count) currentCount = max_count;
  *returnedCount=currentCount;
  
  return OK;
  
}

#endif






