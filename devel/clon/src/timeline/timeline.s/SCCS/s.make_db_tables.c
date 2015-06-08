h58686
s 00001/00001/00138
d D 1.3 06/10/30 16:44:43 boiarino 4 3
c *** empty log message ***
e
s 00005/00003/00134
d D 1.2 06/07/02 14:16:56 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 03/12/22 17:19:40 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 timeline/make_db_tables.c
e
s 00137/00000/00000
d D 1.1 03/12/22 17:19:39 boiarino 1 0
c date and time created 03/12/22 17:19:39 by boiarino
e
u
U
f e 0
t
T
I 1
/*
 * function to create the database tables if they do not exist
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
int make_db_tables(char name[100], char group[100]) 
E 3
I 3
int
make_db_tables(char name[100], char group[100]) 
E 3
{
  MYSQL mysql;
  MYSQL_ROW mysql_row;
  MYSQL_RES *mysql_res;

  char sql_cmd[10000];
  char shrt_name[20], shrt_group[20], table_name[50];
  int status, len, nrows;
D 4
  char host_name[20]=HOST,user_name[40]=NAME, db_name[20]=DB_NAME;
E 4
I 4
  char host_name[20]=HOST, user_name[40]=NAME, db_name[20]=DB_NAME;
E 4
  static init=0;

  status = 0;
#if MYSQL_VERSION_ID >= 32200
  if (init==0) {
    mysql_init(&mysql);
    printf("Initializind mysql\n");
    init=1;
  } 
#endif	
  
#if MYSQL_VERSION_ID >= 32200
  if (!(mysql_real_connect(&mysql,host_name,user_name,NULL,db_name,0,NULL,0))) {
#else
  if (!(mysql_real_connect(&mysql,host_name,user_name,NULL,0,NULL,0))) {
#endif	
    printf("make_db_tables SQL_ERR: connection error%s\n", mysql_error(&mysql));
	/*	fprintf(stderr, "%s\n", mysql_error(&mysql)); */
  }
  else if (mysql_select_db(&mysql,db_name)) {
	printf("make_db_tables SQL_ERR: db select error%s\n", mysql_error(&mysql));
	/*	fprintf(stderr, "%s\n", mysql_error(&mysql)); */
  } else {
  
    
    strncpy(shrt_name,name,19);
    strncpy(shrt_group,group,19);
    strcpy(table_name,shrt_name);
    strcat(table_name,"DOT");
    strcat(table_name,shrt_group);
    
    /* first make sure the table doesnot exist */
    
    sprintf(sql_cmd,"SHOW TABLES FROM clas_online LIKE \'%s\'",table_name);
    len = strlen(sql_cmd);
    if (mysql_real_query(&mysql, sql_cmd, len)) {
      printf("make_db_tables SQL_ERR: %s\n", mysql_error(&mysql));
      printf("make_db_tables SQL_ERR: query error%s\n", sql_cmd);
    }
    mysql_res = mysql_store_result(&mysql);
    nrows = mysql_num_rows(mysql_res);

    if (nrows == 0) {  /* table doesnt exist so create it */

      sprintf(sql_cmd,"CREATE TABLE %s (tou TIMESTAMP, run INT UNSIGNED NOT NULL, event INT UNSIGNED NOT NULL, value FLOAT, error FLOAT, PRIMARY KEY (tou), KEY run (run))",table_name);

      len = strlen(sql_cmd);
      if (mysql_real_query(&mysql, sql_cmd, len)) {
	printf("make_db_tables SQL_ERR: %s\n", mysql_error(&mysql));
	printf("make_db_tables SQL_ERR: query error%s\n", sql_cmd);
	/*	fprintf(stderr, "%s\n", mysql_error(&mysql)); */
      }
    } else {
      /*    printf("make_db_tables W: Table %s already exists...not created\n",table_name); */
    }

    /* now add the row to the latest_value table if it doesn't exist */

    sprintf(sql_cmd,"SELECT var_name FROM latest_value WHERE var_name=\'%s\' and var_group=\'%s\'",shrt_name, shrt_group);
    len = strlen(sql_cmd);
    if (mysql_real_query(&mysql, sql_cmd, len)) {
      printf("make_db_tables SQL_ERR: %s\n", mysql_error(&mysql));
      printf("make_db_tables SQL_ERR: query error%s\n", sql_cmd);
      /*	fprintf(stderr, "%s\n", mysql_error(&mysql)); */
    }
    mysql_res = mysql_store_result(&mysql);
    nrows = mysql_num_rows(mysql_res);

    if (nrows == 0) { /* add an entry to the latest values table */
      sprintf(sql_cmd,"INSERT INTO latest_value (var_name, var_group) VALUES (\'%s\',\'%s\')",shrt_name, shrt_group);
      len = strlen(sql_cmd);
      if (mysql_real_query(&mysql, sql_cmd, len)) {
	  printf("make_db_tables SQL_ERR: %s\n", mysql_error(&mysql));
	  printf("make_db_tables SQL_ERR: query error%s\n", sql_cmd);
	/*	fprintf(stderr, "%s\n", mysql_error(&mysql)); */
      }
    }
    /* remember to close the connection.... */
    mysql_close(&mysql);

    status = 1;
  }
  return(status);
}

D 3
void make_db_tables_(name, group, len_name, len_group)
E 3
I 3
void
make_db_tables_(name, group, len_name, len_group)
E 3
     char *name, *group;
     int len_name, len_group;
{
  int i, end_name, end_group;
  char *cname, *cgroup;

  end_name=0;
  for (i=1; i<=len_name && end_name==0; i++) {
    if ((isspace(name[i]))) {
      end_name=i;
    }
  }

  cname = (char *)malloc(end_name+1);
  strncpy(cname, name, end_name);
  cname[end_name]=0;

  end_group=0;
  for (i=1; i<=len_group && end_group==0; i++) {
    if ((isspace(group[i]))) {
      end_group=i;
    }
  }
  cgroup = (char *)malloc(end_group+1);
  strncpy(cgroup, group, end_group);
  cgroup[end_group]=0;

  make_db_tables(cname, cgroup);
  return;
  
}


E 1
