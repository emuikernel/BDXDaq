h37263
s 00000/00001/00323
d D 1.2 07/10/12 10:09:15 boiarino 2 1
c *** empty log message ***
e
s 00324/00000/00000
d D 1.1 07/10/12 09:51:41 boiarino 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
/*
*  dbrsql.cc
*
*  Performs SQL database access for dbrouter
*
*  ejw, 28-jun-96
*  ejw, 6-jun-06  switched from ingres to mysql
*                 not 100% match between ingres and mysql concepts!
*                 sql_process_data() never used so just returns error
*
*/

// CC -c dbrsql.cc -I$RTHOME/include -I/usr/include/mysql -I.


#define _POSIX_SOURCE 1
#define __EXTENSIONS__


/* include files */

using namespace std;
#include <strstream>

#include <rtworks/ipc.h>
#include <fstream.h>
#include <stdio.h>
D 2
#include <macros.h>
E 2
#include <time.h>
#include <iomanip.h>

#include <dbrouter.h>


// for mysql
static MYSQL *dbhandle = NULL;


// static storage for Int4Ptr's
static int int4ptr_count=-1;
static T_INT4 counts[500][2];


/*-------------------------------------------------------------------*/


sql_code sql_connect(char *dbhost, char *dbuser, char *dbname) {
  

  /* connect to database */
  dbhandle=mysql_init(NULL);
  if(!mysql_real_connect(dbhandle,dbhost,dbuser,NULL,dbname,0,NULL,0)) {
    time_t now = time(NULL);
    cerr << "Unable to connect to database (error: " << mysql_errno(dbhandle) 
	 << ", " << mysql_error(dbhandle) << ") on " 
	 << ctime(&now) << endl;
    return(SQL_ERROR);
  }
  
  /* no table locks on read */
  //  exec sql set lockmode session where readlock=nolock; ???
  

  /* no autocommit */
  /*Sergey: need mysql5.0!!! mysql_autocommit(dbhandle,0);*/
  

  return(SQL_OK);
}


/*-------------------------------------------------------------------*/


sql_code sql_connect_check() {

  int stat=mysql_ping(dbhandle);
  if(stat!=0) {
    cerr << "?lost connect to database, stat is " << stat << endl;
    return(SQL_ERROR);
  } else {
    return(SQL_OK);
  }
}


/*-------------------------------------------------------------------*/


sql_code sql_process_sql(int maxrow, T_STR sql_statement, T_IPC_MSG reply) {
  
  int i;
  int rows_affected;
  int rows_selected;
  T_PTR msg_ptr;              /* holds pointer to beginning of message data area */
  T_INT4 msg_size;            /* size of message so far, in bytes */
  int *row_count_ptr;         /* points to start of 1st row counter */
  MYSQL_RES *res;
  MYSQL_ROW row;
  

  /* not a select statement */
  if(strncasecmp(sql_statement+strspn(sql_statement," "),"select",6)!=0) {

    // execute query
    mysql_query(dbhandle, sql_statement);


    /* get number of rows affected */
    rows_affected=mysql_affected_rows(dbhandle);


    /* check for errors, append status codes to reply message, then return */
    if(mysql_errno(dbhandle)!=0) {
      errprintf("\n%s\n",mysql_error(dbhandle));
      TipcMsgAppendInt4(reply,SQL_ROLLBACK);
      TipcMsgAppendInt4(reply,rows_affected);
      TipcMsgAppendInt4(reply,0);
      TipcMsgAppendStr(reply,(T_STR)mysql_error(dbhandle));
      return(SQL_ROLLBACK);
    } else{
      row_mod_count += rows_affected;
      TipcMsgAppendInt4(reply,SQL_COMMIT);
      TipcMsgAppendInt4(reply,rows_affected);
      TipcMsgAppendInt4(reply,0);
      return(SQL_COMMIT);
    } 
    
    
  } else {                /* select------------------------------------------------*/
    

    // execute query, get result set and process rows

 cout << "executing " << sql_statement << endl;

    mysql_query(dbhandle, sql_statement);
    res = mysql_use_result(dbhandle);
    if(res!=NULL) {

 cout << "got result" << endl;

      /* fill in reply message header words */
      TipcMsgAppendInt4(reply,SQL_COMMIT);
      
      // int4 array ptr for row counts, contents filled later
      int4ptr_count++;
      if(int4ptr_count>500) {
	cerr << "\n??? int4ptr count exceeds 500...impossible!\n" << endl;
	exit(EXIT_FAILURE);
      }
      TipcMsgAppendInt4ArrayPtr(reply,counts[int4ptr_count],2,NULL);

      /* fetch and process rows */
      rows_selected=0;
      while(row = mysql_fetch_row(res)) {
	if(rows_selected>=maxrow)break;
	rows_selected++;
	if(rows_selected==1)sql_add_header_to_message(reply,res);
	sql_add_row_to_message(reply,res,row);
      }
      
      /* count total number of rows returned */
      row_return_count += rows_selected;
      
      /* fill in row counts...must skip 4-byte field-types preceeding each int4 data word */
      counts[int4ptr_count][0]=rows_selected;
      counts[int4ptr_count][1]=rows_selected;

      mysql_free_result(res);
      return(SQL_COMMIT);
    }
  }
  
}


/*-------------------------------------------------------------------*/


void sql_add_header_to_message(T_IPC_MSG reply, MYSQL_RES *res) {
  
  int ncol = mysql_num_fields(res);


  /* append number of columns */
  TipcMsgAppendInt4(reply,ncol);

  /* append column names */
  for (int i=0; i<ncol; i++) TipcMsgAppendStr(reply,mysql_fetch_field_direct(res,i)->name);
}


/*-------------------------------------------------------------------*/


void sql_add_row_to_message(T_IPC_MSG reply, MYSQL_RES *res, MYSQL_ROW row) {
  
  int ncol               = mysql_num_fields(res);
  MYSQL_FIELD *fields    = mysql_fetch_fields(res);
  unsigned long *lengths = mysql_fetch_lengths(res);


  for (int i=0; i<ncol; i++) {
    
    switch(fields[i].type) {
	
    case MYSQL_TYPE_TINY:
      if(lengths[i]!=0) {
	TipcMsgAppendChar(reply,atoi(row[i]));
      } else {
	TipcMsgAppendChar(reply,'\0');
      }
      break;

    case MYSQL_TYPE_SHORT:
      if(lengths[i]!=0) {
	TipcMsgAppendInt2(reply,atoi(row[i]));
      } else {
	TipcMsgAppendInt2(reply,0xDEAD);
      }
      break;

    case MYSQL_TYPE_LONG:
    case MYSQL_TYPE_INT24:
      if(lengths[i]!=0) {
	TipcMsgAppendInt4(reply,atol(row[i]));
      } else {
	TipcMsgAppendInt4(reply,0xDEADBEEF);
      }
      break;
      
    case MYSQL_TYPE_LONGLONG:
      if(lengths[i]!=0) {
	TipcMsgAppendInt8(reply,atoll(row[i]));
      } else {
	TipcMsgAppendInt8(reply,0xDEADBEEF);
      }
      break;
      
    case MYSQL_TYPE_FLOAT:
      if(lengths[i]!=0) {
	TipcMsgAppendReal4(reply,atof(row[i]));
      } else {
	TipcMsgAppendReal4(reply,0xDEADBEEF);
      }
      break;
      
    case MYSQL_TYPE_DOUBLE:
      if(lengths[i]!=0) {
	TipcMsgAppendReal8(reply,atof(row[i]));
      } else {
	TipcMsgAppendReal8(reply,0xDEADBEEF);
      }
      break;
      
    default:
      if(lengths[i]!=0) {
	TipcMsgAppendStr(reply,row[i]);
      } else {
	TipcMsgAppendStr(reply,(T_STR)"\0");
      }
      break;
    }

  }

}

/*-------------------------------------------------------------------*/


// never used...ejw, 12-jun-2006
sql_code sql_process_data(T_IPC_MSG msg,T_IPC_MSG reply) {
  return(SQL_BAD_FORMAT);
}
  

/*-------------------------------------------------------------------*/


void sql_commit(){

  printf("sql_commit: not supported yet!!!\n");
  /*sergey: need mysql5.0!!! mysql_commit(dbhandle);*/

  return;
}


/*-------------------------------------------------------------------*/


void sql_rollback(){

  printf("sql_rollback: not supported yet!!!\n");
  /*sergey: need mysql5.0!!! mysql_rollback(dbhandle);*/

  return;
}


/*-------------------------------------------------------------------*/


sql_code sql_disconnect(void){
  mysql_close(dbhandle);
  return(SQL_OK);
}


/*-------------------------------------------------------------------*/


void reset_int4ptr_count(){

  int4ptr_count=-1;
  return;
}


/*-------------------------------------------------------------------*/


E 1
