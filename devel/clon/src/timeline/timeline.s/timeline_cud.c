/*
 * set of three functions to:
 *       connect to the database
 *       make entries to the database
 *       disconnect from the database
 */

#include <stdio.h>
#include <mysql/mysql.h>
#include "timeline.h"

static  MYSQL mysql;

int
timeline_connect()
{
  int status;
  static int nerrors=0, init=0;
  char host_name[20]=HOST,user_name[40]=NAME, db_name[20]=DB_NAME;

#if MYSQL_VERSION_ID >= 32200
  if (init==0) {
    mysql_init(&mysql);
    /*    printf("Initializind mysql\n"); */
    init=1;
  } 
#endif	
  
#if MYSQL_VERSION_ID >= 32200
  if (!(mysql_real_connect(&mysql,host_name,user_name,NULL,db_name,0,NULL,0))) {
#else
  if (!(mysql_real_connect(&mysql,host_name,user_name,NULL,0,NULL,0))) {
#endif	
    printf("timeline_connect  SQL_ERR %i: connection error: %s\n", 
	   nerrors, mysql_error(&mysql));
    status = 0;
    nerrors++;
    
  } else if (mysql_select_db(&mysql,db_name)) {
    printf("timeline_connect SQL_ERR %i: db select error: %s\n", 
	     nerrors,mysql_error(&mysql));
    status = 0;
    nerrors++;
  } else {
    status = 1;
  }
  return(status);
}
int
timeline_disconnect()
{
  int status;
  
    mysql_close(&mysql);
    status = 1;
    return(status);
    
}

int
timeline_put(int run, int event, char name[100], char group[100], float value, float error)
{
  MYSQL_ROW mysql_row;
  MYSQL_RES *mysql_res;

  char sql_cmd[10000];
  char shrt_name[20], shrt_group[20], table_name[50];
  char host_name[20]=HOST,user_name[40]=NAME, db_name[20]=DB_NAME;
  int status, len, nrows;
  static int nerrors=0;

  strncpy(shrt_name,name,19);
  strncpy(shrt_group,group,19);
  strcpy(table_name,shrt_name);
  strcat(table_name,"DOT");
  strcat(table_name,shrt_group);

  status = 2;

  sprintf(sql_cmd,"INSERT INTO %s (run, event,value, error) VALUES (%i, %i, %f,%f)",  
	  table_name, run, event, value, error);
  len = strlen(sql_cmd);

  if (mysql_real_query(&mysql, sql_cmd, len)) {
    status=0;
    printf("timeline_put SQL_ERR: %s\n%s\n", mysql_error(&mysql), sql_cmd);
  }
    
  sprintf(sql_cmd,"UPDATE latest_value SET run=%i, event=%i,value=%f, error=%f WHERE var_name=\'%s\' and var_group=\'%s\'", run, event, value, error, shrt_name, shrt_group); 

  
  len = strlen(sql_cmd);
  if (mysql_real_query(&mysql, sql_cmd, len)) {
    status=0;
    printf("timeline_put SQL_ERR: %s\n%s\n", mysql_error(&mysql), sql_cmd);
  }
  
  /* update epics */
  /*  update_epics(name, group, value); */
  return(status);
}

int
timeline_put_group(int run, int event, char table_name[100], char *col_names[MAX_VAR], float col_values[MAX_VAR], int n_columns)
{
  MYSQL_ROW mysql_row;
  MYSQL_RES *mysql_res;

  char sql_cmd[10000], variable_list[1000], value_list[1000];
  char c_tmp[50];
  char host_name[20]=HOST,user_name[40]=NAME, db_name[20]=DB_NAME;
  int status, len, nrows;
  int i;
  static int nerrors=0;

  status = 2;

  strcpy(variable_list,"run, event");
  sprintf(value_list,"%i,%i",run,event);

  for (i=0; i<n_columns; i++) {
    /*    printf("in loop i: %i col: %s\n",i,col_names[i]);
	  printf("timeline_put_group group:%s name:%s value:%f\n", table_name, col_names[i], col_values[i]);    */
    sprintf(sql_cmd,"UPDATE latest_value SET run=%i, event=%i,value=%f WHERE var_name=\'%s\' and var_group=\'%s\'", run, event, col_values[i], col_names[i], table_name);
    /*    printf("sql_cmd:%s\n",sql_cmd);  */

    len = strlen(sql_cmd);
    if (mysql_real_query(&mysql, sql_cmd, len)) {
      printf("timeline_put_group SQL_ERR: %s\n%s\n", mysql_error(&mysql), sql_cmd);
    }

    strcat(variable_list, ",");
    strcat(variable_list, col_names[i]);
    sprintf(c_tmp,",%f",col_values[i]);
    strcat(value_list, c_tmp);

    /* update epics */
    /*    update_epics(col_names[i], table_name, col_values[i]); */
  }
  sprintf(sql_cmd,"INSERT  INTO %s (%s) VALUES (%s)",table_name, variable_list, value_list);
  /*    printf("sql_cmd:%s\n",sql_cmd); */

    len = strlen(sql_cmd);
    if (mysql_real_query(&mysql, sql_cmd, len)) {
      printf("timeline_put_group SQL_ERR: %s\n%s\n", mysql_error(&mysql), sql_cmd);
    }
  return(status);
  
}

void
timeline_disconnect_()
{
  int status;
  
  status = timeline_disconnect();
}

void
timeline_connect_()
{
  int status;
  
  status = timeline_connect();
}

void
timeline_put_(run, event, name, group, value, error, len_name, len_group)
     int *run, *event, len_name, len_group;
     char *name, *group;
     float *value, *error;
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

  timeline_put(*run, *event, cname, cgroup, *value, *error);
  return;
}

  
