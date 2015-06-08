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

#include "AAPI_client.h"
#include "StripHistory.h"
#include "get_AAPI_data.h"

extern unsigned int historySize;

extern long radioBoxAlgorithm; 
#define DEBUG 0
#define SERVER_ERROR_BUF_SIZE 256

static void tryFreeData(answerData *ans_data,char *serverErrorString);

u_long get_AAPI_data(char           *name,
		     struct timeval *begin,
		     struct timeval *end,
		     struct timeval **times,
		     short           **status,
		     double         **data,
		     u_long *count)
{

int i;
static  recRequest req;
static answerData *ans_data;
char *dataP;
u_long cmd;
u_long err;
u_long serverError, serverVersion;
char *serverErrorString;
static AAPI_connection_establish=1;
static char buf[SERVER_ERROR_BUF_SIZE];
	
  cmd=DATA_REQUEST_CMD;
  req.from_sec=begin->tv_sec;
  req.from_usec=(begin->tv_usec)*nSecPerUSec;
  req.to_sec=end->tv_sec;
  req.to_usec=(end->tv_usec)*nSecPerUSec;
  req.maxNum= historySize;
  req.convers=1 + radioBoxAlgorithm;
  req.conversPar=0.0;
  req.PV_name_size=1;
  req.name=name;

  if ( (err=AAPI_get(cmd,(char *)&req,(char **)&dataP,&serverError,
		     &serverErrorString,&serverVersion)) != 0) 
    {
      fprintf(stderr,"err=%ld: bad AAPI_getData call;no goodData\n",
	      err);
      if (AAPI_connection_establish == 1) 
	{
	  AAPI_connection_establish = 0;
	  History_MessageBox_popup
	    ( "AAPI_SERVER PROBLEM", 
	     "OK",
	     "AAPI-server problem. \ncould be you need restart it.\n");
	}
     return (err);
    }
   else { AAPI_connection_establish = 1; }

  /* FatalError = NoData. sereverString like popUp menu: */

  if (serverError) {
    memset(buf,0,SERVER_ERROR_BUF_SIZE);
    sprintf(buf,"serverError=%ld\n",serverError);
    if(serverErrorString) 
      sprintf(buf,"which means %s\n",serverErrorString); 
    else sprintf(buf,"-undefine error\n"); 
    History_MessageBox_popup("BIG REQUEST", "OK",buf);
    tryFreeData(ans_data,serverErrorString);
    return (-1);
  }

  ans_data = (answerData *) dataP;
  
  if(ans_data->PV_name_size != 1) 
    {
      fprintf(stderr,"bad num=%ld != 1\n",ans_data->PV_name_size);
      tryFreeData(ans_data,serverErrorString);
      return (-1);
    }
  
  if(ans_data->single[0].error != 0 )
    {
      fprintf(stderr,"Server data error=%ld; no goodData\n",
	      ans_data->single[0].error);
      tryFreeData(ans_data,serverErrorString);
      return (-1);
    }
  
  *count=ans_data->single[0].count;
  
  if(*count < 1 )
    {
      fprintf(stderr,"Server count data=%ld is no good; no goodData\n",*count);
      tryFreeData(ans_data,serverErrorString);
      return (-1);
    }

   if(*count > historySize)
    {
      *count = historySize-1; /* show rest of big buffer */
      fprintf(stderr,"Server count data=%ld is big no goodData\n",*count);
      History_MessageBox_popup("BIG REQUEST", "OK",
             "So many Data from Archiver\nPlease, decrease interval or increase # historyPoints.\n");	
      
    }

  if( (*data=(double *) calloc(*count,sizeof(double))) == NULL) 
    {
      fprintf(stderr,"can't alloc %ld double\n",*count);
      tryFreeData(ans_data,serverErrorString);
      return (-1);
    }
  
  if( (*status=(short *) calloc(*count,sizeof(short))) == NULL) 
    {
      fprintf(stderr,"can't alloc %ld long\n",*count);
      if(*data) free(*data);
      tryFreeData(ans_data,serverErrorString);
      return (-1);
    }
  
  if( (*times=(struct timeval *)calloc(*count,sizeof(struct timeval)))
      == NULL) 
    {
      fprintf(stderr,"can't alloc %ld times\n",*count);
      if(*data) free(*data);
      if(*status) free(*status);
      tryFreeData(ans_data,serverErrorString);
      return (-1);
    }
  
  
  for(i=0;i< (*count);i++) 
    {
      (*times)[i].tv_sec = ans_data->single[0].sample[i].time_sec;
      (*times)[i].tv_usec=(ans_data->single[0].sample[i].time_usec)
	/ (nSecPerUSec);
      (*data)[i]         = ans_data->single[0].sample[i].value;
      (*status)[i]       =(short) ans_data->single[0].sample[i].status;  
    }

  tryFreeData(ans_data,serverErrorString);
  return (0);
}


static void tryFreeData(answerData *ans_data,char *serverErrorString)	
{
  int i;
  for(i=0;i<ans_data->PV_name_size;i++){
    if(ans_data->single[i].sample)     free(ans_data->single[i].sample);
    if(ans_data->single[i].ctrl.Units) free(ans_data->single[i].ctrl.Units);
    }
  if(ans_data->single)  free(ans_data->single);
  if(serverErrorString) free (serverErrorString);
}

void  AAPI_Result_release (StripHistoryResult     *result)
{

 if(result->data)    free (result->data);
 if(result->times)   free (result->times);
 if(result->status)  free (result->status);
}

#define MAX_FILTER_LIST 100

int extractAAPIfilterList(char ***list, int *len)
{
  u_long err;
  u_long cmd;
  char *dataP;
  int i;
  filterListData *filterListDataP;
  char *arr;
  u_long serverError, serverVersion;
  char *serverErrorString;

  cmd=FILTER_LIST_CMD;

  if ( (err=AAPI_get(cmd,NULL,(char **)&dataP,&serverError,&serverErrorString,
		     &serverVersion)) != 0) {
    fprintf(stderr,"err=%ld: bad AAPI_getData call \n",err);
    return (1);
  } 
   if (serverError) {
    printf("serverError=%ld",serverError);
    if(serverErrorString) 
      printf("which means %s!!!\n",serverErrorString); 
    else printf("but ErrorString is empty!\n"); 

    free(serverErrorString);
    return (-1);
  } 

  filterListDataP = (filterListData *) dataP;

  /*  printf("filterListData.num=%ld\n",filterListDataP->num);*/
  *len = filterListDataP->num;
  if(( *len <= 0)||( *len > MAX_FILTER_LIST ) ) 
    {
      printf("filterListData.num=%ld is bad\n",filterListDataP->num);
      if(filterListDataP->list) free(filterListDataP->list);
      return (1);
    }
  if( (*list= (char **) calloc(sizeof(char *),*len)) ==NULL ) 
    {
      perror("extractAAPIfilterList: no memory");
      if(filterListDataP->list) free(filterListDataP->list);
      return(1);
    }

  arr=filterListDataP->list;

  for(i=0;i<filterListDataP->num;i++) {
    if( ((*list)[i]= calloc(sizeof(char),strlen(arr) +1)) == NULL)
      {
	perror("extractAAPIfilterList: no memory");
	if(filterListDataP->list) free(filterListDataP->list);
	return(1);
      }
    strcpy((*list)[i],arr);
    /*    printf("%s;\n",(*list)[i]);*/

    arr+=(strlen(arr) +1 );
  }

  if(filterListDataP->list) free(filterListDataP->list);
  
  return (0);  
}
