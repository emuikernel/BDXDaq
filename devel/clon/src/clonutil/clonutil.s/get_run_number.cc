//  get_run_number
//
//  gets run number for this session
//
//  ejw, 19-may-97


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

#include <stdlib.h>

using namespace std;
#include <strstream>

// online and coda stuff
extern "C"{
#include "libdb.h"
}


//--------------------------------------------------------------------------

extern "C" {
int get_run_number(char *mysql_database, char *session){

  MYSQL *connNum;
  MYSQL_RES *result;
  MYSQL_ROW row_out;
  ostrstream query;
  

  // connect to mysql database
  connNum = dbConnect(getenv("MYSQL_HOST"), mysql_database);
  

  // form mysql query, execute, then close mysql connection
  query << "select runNumber from sessions where name='"
	<< session << "'" << ends;
  mysql_query(connNum,query.str());
  result = mysql_store_result(connNum);
  row_out = mysql_fetch_row(result);
  

  // get run number 
  if(row_out[0]==NULL){

    mysql_free_result(result);
    dbDisconnect(connNum);
    return(0);
  } else {
    mysql_free_result(result);
    dbDisconnect(connNum);
    return(atoi(row_out[0]));
  }

}
} // extern "C"


//--------------------------------------------------------------------------
