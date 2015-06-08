/******************/
/* from libutil/s */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "msql.h"

void
get_run_config(char *msql_database, char *session, int *run, char **config)
{
  int connNum;
  m_result *result;
  m_row row_out;
  char query[1024];

  // connect to msql database
  connNum = msqlConnect(getenv("MSQL_TCP_HOST"));
  msqlSelectDB(connNum, msql_database);
  

  // form msql query, execute, then close msql connection
  sprintf(query,"SELECT runNumber,config FROM sessions WHERE name='%s'",
    session);
  msqlQuery(connNum, query);
  result = msqlStoreResult();
  row_out = msqlFetchRow(result);
  msqlClose(connNum);
  

  // check for null row_out
  if(row_out==NULL)
  {
    *run=0;
    *config=NULL;
    return;
  }


  // run number 
  if(row_out[0]==NULL)
  {
    *run=0;
  }
  else
  {
    *run=atoi(row_out[0]);
  }


  // config
  *config=strdup(row_out[1]);

  return;
}


/******************/
/******************/
