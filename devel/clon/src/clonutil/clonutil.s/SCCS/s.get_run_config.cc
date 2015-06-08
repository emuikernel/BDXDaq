h42365
s 00000/00004/00073
d D 1.7 07/10/12 09:49:05 boiarino 8 7
c *** empty log message ***
e
s 00006/00000/00071
d D 1.6 06/07/11 21:56:46 boiarino 7 6
c Linux port
e
s 00001/00000/00070
d D 1.5 98/02/25 15:00:20 wolin 6 5
c Typo
c 
e
s 00007/00000/00063
d D 1.4 98/02/25 14:58:00 wolin 5 4
c Checking for null row_out
c 
e
s 00006/00001/00057
d D 1.3 97/06/21 03:32:45 clasrun 4 3
c Bombproofed run number
e
s 00002/00002/00056
d D 1.2 97/05/20 16:48:43 wolin 3 1
c Minor bug
e
s 00000/00000/00000
d R 1.2 97/05/20 16:37:47 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libutil/s/get_run_config.cc
c Name history : 1 0 s/get_run_config.cc
e
s 00058/00000/00000
d D 1.1 97/05/20 16:37:46 wolin 1 0
c Gets run and config from msql database
e
u
U
f e 0
t
T
I 1
//  get_run_config
//
//  gets run number and config for this session
//
//  ejw, 20-may-97


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

#include <strings.h>
#include <stdlib.h>
I 7

D 8
#ifdef Linux
E 8
using namespace std;
#include <strstream>
D 8
#else
E 7
#include <strstream.h>
I 7
#endif
E 8
E 7

// online and coda stuff
extern "C"{
#include <msql.h>
}


//--------------------------------------------------------------------------

extern "C" {
D 3
void get_run_config(char *msql_database, char *session, int *run, char *config){
E 3
I 3
void get_run_config(char *msql_database, char *session, int *run, char **config){
E 3


  m_result *result;
  m_row row_out;
  ostrstream query;
  

  // connect to msql database
  int connNum = msqlConnect(getenv("MSQL_TCP_HOST"));
  msqlSelectDB(connNum,msql_database);
  

  // form msql query, execute, then close msql connection
  query << "select runNumber,config from sessions where name='"
	<< session << "'" << ends;
  msqlQuery(connNum,query.str());
  result = msqlStoreResult();
  row_out = msqlFetchRow(result);
  msqlClose(connNum);
  

I 5
  // check for null row_out
  if(row_out==NULL) {
    *run=0;
    *config=NULL;
I 6
    return;
E 6
  }


E 5
  // run number 
D 4
  *run=atoi(row_out[0]);
E 4
I 4
  if(row_out[0]==NULL){
    *run=0;
  } else {
    *run=atoi(row_out[0]);
  }
E 4

I 4

E 4
  // config
D 3
  config=strdup(row_out[1]);
E 3
I 3
  *config=strdup(row_out[1]);
E 3

  return;
}
} // extern "C"


//--------------------------------------------------------------------------
E 1
