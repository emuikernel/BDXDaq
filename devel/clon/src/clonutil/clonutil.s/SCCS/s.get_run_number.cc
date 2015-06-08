h01942
s 00000/00004/00059
d D 1.4 07/10/12 09:49:42 boiarino 5 4
c *** empty log message ***
e
s 00006/00000/00057
d D 1.3 06/07/11 21:56:35 boiarino 4 3
c Linux port
e
s 00005/00001/00052
d D 1.2 97/06/21 03:32:38 clasrun 3 1
c Bombproofed run number
e
s 00000/00000/00000
d R 1.2 97/05/19 11:30:47 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libutil/s/get_run_number.cc
c Name history : 1 0 s/get_run_number.cc
e
s 00053/00000/00000
d D 1.1 97/05/19 11:30:46 wolin 1 0
c Gets run number from msql database given session name
e
u
U
f e 0
t
T
I 1
//  get_run_number
//
//  gets run number for this session
//
//  ejw, 19-may-97


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

#include <stdlib.h>
I 4

D 5
#ifdef Linux
E 5
using namespace std;
#include <strstream>
D 5
#else
E 4
#include <strstream.h>
I 4
#endif
E 5
E 4

// online and coda stuff
extern "C"{
#include <msql.h>
}


//--------------------------------------------------------------------------

extern "C" {
int get_run_number(char *msql_database, char *session){


  m_result *result;
  m_row row_out;
  ostrstream query;
  

  // connect to msql database
  int connNum = msqlConnect(getenv("MSQL_TCP_HOST"));
  msqlSelectDB(connNum,msql_database);
  

  // form msql query, execute, then close msql connection
  query << "select runNumber from sessions where name='"
	<< session << "'" << ends;
  msqlQuery(connNum,query.str());
  result = msqlStoreResult();
  row_out = msqlFetchRow(result);
  msqlClose(connNum);
  

  // get run number 
D 3
  return(atoi(row_out[0]));
E 3
I 3
  if(row_out[0]==NULL){
    return(0);
  } else {
    return(atoi(row_out[0]));
  }
E 3

}
} // extern "C"


//--------------------------------------------------------------------------
E 1
