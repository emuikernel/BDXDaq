/*
 * function to update the database
 */

#include <stdio.h>
#include <mysql/mysql.h>
#include "timeline.h"

int
update_tables(int run, int event, char name[100], char group[100], float value, float error)
{
  MYSQL mysql;
  MYSQL_ROW mysql_row;
  MYSQL_RES *mysql_res;

  char sql_cmd_local[10000];
  char shrt_name[20], shrt_group[20], table_name[50];
  char host_name[20]=HOST,user_name[40]=NAME, db_name[20]=DB_NAME;
  int status, len, nrows;
  static int nerrors=0, init=0;

  status = 2;
  
#if MYSQL_VERSION_ID >= 32200
  if (init==0) {
    mysql_init(&mysql);
    printf("Initializing mysql\n");
    init=1;
  } 
#endif	

  
#if MYSQL_VERSION_ID >= 32200
  if (!(mysql_real_connect(&mysql,host_name,user_name,NULL,db_name,0,NULL,0))) {
#else
  if (!(mysql_real_connect(&mysql,host_name,user_name,NULL,0,NULL,0))) {
#endif	
    if (nerrors < 10 ) {
      printf("update_tables SQL_ERR %i: connection error%s\n", 
	   nerrors, mysql_error(&mysql));
    }
    status = 0;
    nerrors++;
  } else if (mysql_select_db(&mysql,db_name)) {
    if (nerrors < 10) {
      printf("update_tables SQL_ERR %i: db select error%s\n", 
	     nerrors,mysql_error(&mysql));
    }
    status = 0;
    nerrors++;
  } else {

    nerrors = 0;

    sql_cmd_local[0] = '\0';

    strncpy(shrt_name,name,19);
    strncpy(shrt_group,group,19);
    strcpy(table_name,shrt_name);
    strcat(table_name,"DOT");
    strcat(table_name,shrt_group);

    sprintf(sql_cmd_local,"INSERT INTO %s (run, event,value, error) VALUES (%i, %i, %f,%f)", table_name, run, event, value, error);
    len = strlen(sql_cmd_local);

    if (mysql_real_query(&mysql, sql_cmd_local, len)) {
      status=0;
      printf("update_tables SQL_ERR: %s\n%s\n", mysql_error(&mysql), sql_cmd_local);
    }
    
    sprintf(sql_cmd_local,"UPDATE latest_value SET run=%i, event=%i,value=%f, error=%f WHERE var_name=\'%s\' and var_group=\'%s\'", run, event, value, error, shrt_name, shrt_group);
    
    len = strlen(sql_cmd_local);
    if (mysql_real_query(&mysql, sql_cmd_local, len)) {
      status=0;
      printf("update_tables SQL_ERR: %s\n%s\n", mysql_error(&mysql), sql_cmd_local);
    }

    mysql_close(&mysql);
  }
  /* update epics records  */
  /*  update_epics(name, group, value);  */

  return(status);
}
