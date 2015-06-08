h24287
s 00003/00002/00082
d D 1.2 06/07/02 14:16:13 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 03/12/22 17:19:45 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 timeline/update_tables.c
e
s 00084/00000/00000
d D 1.1 03/12/22 17:19:44 boiarino 1 0
c date and time created 03/12/22 17:19:44 by boiarino
e
u
U
f e 0
t
T
I 1
/*
 * function to update the database
 */

#include <stdio.h>
D 3
#include "mysql.h"
E 3
I 3
#include <mysql/mysql.h>
E 3
#include "timeline.h"

D 3
int update_tables(int run, int event, char name[100], char group[100], float value, float error)
E 3
I 3
int
update_tables(int run, int event, char name[100], char group[100], float value, float error)
E 3
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
E 1
