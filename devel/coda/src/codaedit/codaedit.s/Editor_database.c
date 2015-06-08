/*-----------------------------------------------------------------------------
 * Copyright (c) 1991,1992 Southeastern Universities Research Association,
 *                         Continuous Electron Beam Accelerator Facility
 *
 * This software was developed under a United States Government license
 * described in the NOTICE file included as part of this distribution.
 *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
 * Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
 *-----------------------------------------------------------------------------
 * 
 * Description:
 *	Implementation of CODA editor and database interface
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Mar 2008: Sergey Boyarinov: migrate to mysql
 *
 * Revision History:
 *   $Log: Editor_database.c,v $
 *   Revision 1.10  1998/06/18 12:28:26  heyes
 *   tidy up a lot of thingscd ../cedit
 *
 *   Revision 1.9  1997/12/03 16:56:08  heyes
 *   increase array bounds to pervent core dumps in graph etc
 *
 *   Revision 1.8  1997/09/08 15:19:15  heyes
 *   fix dd icon etc
 *
 *   Revision 1.7  1997/07/09 13:03:33  heyes
 *   back to normal
 *
 *   Revision 1.6  1997/07/08 15:30:32  heyes
 *   add stuff
 *
 *   Revision 1.5  1997/06/20 16:59:52  heyes
 *   clean up GUI!
 *
 *   Revision 1.4  1997/06/16 12:23:39  heyes
 *   various fixes and nicities!
 *
 *   Revision 1.3  1997/06/04 10:31:48  heyes
 *   tune makefile
 *
 *   Revision 1.2  1996/11/27 15:05:07  chen
 *   change configuration and remove RCS from icon list
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:21  chen
 *   coda source
 *
 *	  
 */

#include <assert.h>
#include "Editor_graph.h"
#include "Editor_database.h"

#define _CODA_DEBUG

#define PRIORITY_TABLE_NAME "priority"
#define RUNTYPE_TABLE_NAME  "runTypes"
#define EXPINFO_TABLE_NAME  "sessions"
#define PROCESS_TABLE_NAME  "process"

static MYSQL* mysql = NULL;       /* connection socket to the database */
static char   dbaseServerHost[128];   /* database server host name         */
static char*  dbasename = 0;          /* current database name             */
static char*  runType = 0;            /* current configuration name        */

/* component type string */
static char* compTypeString[] = {
  "TS", "ROC", "EB", "ANA",
  "EBANA", "UT", "ER", "LOG", "SC", 
  "UC", "RCS", "FILE", "FILE","DEBUG","UNKNOWN","MON","NONE","UNKNOWN"};

static char*
configName (char* fullname)
{
  char temp[255];
  char *p, *q;

  q = temp;
  p = fullname;
  while (p != strstr (fullname, "_config")) {
    *q = *p;
    q++; p++;
  }
  *q = '\0';

  p = (char *)malloc ((strlen (temp) + 1)*sizeof(char));
  strcpy (p, temp);
  return p;
}


/**********************************************************/
/****************** database operations *******************/
/**********************************************************/

int
connectToDatabase (char *host)
{
  const char *user = "clasrun";
  const char *passwd = "";
  const char db[128];
  unsigned int port = 0;
  const char *unix_socket = "";
  unsigned long client_flag = 0;

#ifdef _CODA_DEBUG
  printf("connectToDatabase() reached\n");
#endif

  if (host == 0)
  {
	/*
    if (gethostname (dbaseServerHost, sizeof (dbaseServerHost)) != 0)
    {
      fprintf (stderr, "Cannot find this workstation hostname\n");
      exit(1);
    }
	*/
    strncpy (dbaseServerHost, "clondb1", sizeof (dbaseServerHost));
  }
  else
  {
    strncpy (dbaseServerHost, host, sizeof (dbaseServerHost));
  }


  /*initialize MYSQL structure; use 'mysql_options'*/
  mysql = dbConnect(dbaseServerHost, getenv("EXPID"));

  /*sergey: use our wrapper
  sprintf(db,"daq_%s",getenv("EXPID"));
  mysql = mysql_init(mysql);
#ifdef _CODA_DEBUG
  printf("connectToDatabase: after mysql_init mysql=0x%08x\n",mysql);
#endif
  if(mysql == NULL) return(NULL);

#ifdef _CODA_DEBUG
  printf("connectToDatabase: connecting: mysql=0x%08x dbaseServerHost=>%s< user=>%s< passwd=>%s<\n",
                              mysql,
                              dbaseServerHost,
                              user,
                              passwd);
  printf("connectToDatabase: connecting: db=>%s< port=%d unix_socket=>%s< flag=%d\n",
                              db,
                              port,
                              unix_socket,
                              client_flag);
#endif

  mysql = mysql_real_connect( mysql,
                              dbaseServerHost,
                              user,
                              passwd,
                              db,
                              port,
                              unix_socket,
                              client_flag);

#ifdef _CODA_DEBUG
  printf("connectToDatabase: after mysql_real_connect mysql=0x%08x\n",mysql);
#endif
  */

  return(mysql);
}

void
closeDatabase (void)
{
  dbDisconnect(mysql);
  /*sergey: use our wrapper
  if (mysql != NULL) mysql_close (mysql);
  */
  mysql = NULL;
}

int
databaseIsOpen (void)
{
  if (mysql != NULL)
    return 1;
  return 0;
}

void
cleanDatabaseMiscInfo (void)
{
  if (dbasename) free (dbasename);
  dbasename = 0;
  if (runType) free (runType);
  runType = 0;
}

int
createNewDatabase (char *name)
{
  char queryString[1024];

  if (mysql == NULL)
    return -1;

  sprintf (queryString, "CREATE DATABASE %s\n", name);
  if (mysql_query(mysql, queryString) != 0)
    return -1;
  if (selectDatabase (name) < 0)
    return -1;
  /* create all tables */
  if (createExpInfoTable () < 0) {
#ifdef _CODA_DEBUG
    printf ("Cannot create expinfo table\n");
#endif
    return -1;
  }
  if (createProcessTable () < 0) {
#ifdef _CODA_DEBUG
    printf ("Cannot create process table\n");
#endif
    return -1;
  } 
  if (createRunTypeTable () < 0) {
#ifdef _CODA_DEBUG
    printf ("Cannot create runtype table\n");
#endif
    return -1;
  }
  if (createPriorityTable () < 0) {
#ifdef _CODA_DEBUG
    printf ("Cannot create priority table\n");
#endif
    return -1;
  }
  return 0;
}

int
listAllDatabases (char* dbase[], int* num)
{
  MYSQL_RES *res = 0;
  int       i = 0;
  MYSQL_ROW     row;

  if (mysql == NULL)
    return -1;
  res = mysql_list_dbs(mysql, NULL);

  if (!res)
    return -1;

  while ((row = mysql_fetch_row (res)))
  {
    dbase[i] = (char *)malloc ((strlen (row[0]) + 1)*sizeof(char));
    strcpy (dbase[i], row[0]);
    i++;
  }
  mysql_free_result(res);

  *num = i;
  return 0;
}

int
selectDatabase (char *name)
{
  int status;

  if (dbasename) free (dbasename);
  dbasename = 0;

  status = mysql_select_db(mysql, name);
  if (status >= 0)
  {
    dbasename = (char *)malloc ((strlen (name) + 1)*sizeof (char));
    strcpy (dbasename, name);
  }
  if (dbasename) {
#ifdef _CODA_DEBUG
    printf ("Selected database is %s\n", dbasename);
#endif
  }
  return status;
}

int
databaseSelected (void)
{
  if (dbasename) return(1);
  return(0);
}


char*
currentDatabase (void)
{
  return(dbasename);
}

int
removeDatabase (char *name)
{
  char queryString[1024];

  if (!databaseIsOpen()) return(-1);

  sprintf (queryString, "DROP DATABASE %s\n", name);
  if (mysql_query(mysql, queryString) != 0)
  {
#ifdef _CODA_DEBUG
	  printf ("Drop database error: %s\n", mysql_error(mysql));
#endif
    return(-1);
  }

  return(0);
}







/**********************************************************/
/***************** create table operations ****************/
/**********************************************************/


int
createConfigTable (char* config)
{
  char queryString[1024];
  char valString[256];
  int  num;

  if (!databaseIsOpen()) return(-1);

  if (!databaseSelected()) return(-1);

  sprintf (queryString, "create table %s(\n", config);
  strcat  (queryString, "name varchar(32) binary not null primary key,\n");

/* sergey: problem on Linux, maybe newer mysql ???
  strcat  (queryString, "code char(512) not null,\n");
  strcat  (queryString, "inputs char(400) not null,\n");
  strcat  (queryString, "outputs char(400) not null,\n");
*/
  strcat  (queryString, "code text not null,\n");
  strcat  (queryString, "inputs text not null,\n");
  strcat  (queryString, "outputs text not null,\n");

  strcat  (queryString, "first char(32) not null,\n");
  strcat  (queryString, "next char(32) not null,\n");
  strcat  (queryString, "inuse char(32) not null\n");
  strcat  (queryString,")");
  if (mysql_query (mysql, queryString) != 0)
  {
#ifdef _CODA_DEBUG
    printf ("Create %s config table Error: %s\n",config, mysql_error(mysql));
#endif
    return(-1);
  }
  else
  {
#ifdef _CODA_DEBUG
    printf ("Create %s config table\n",config);
#endif
  }

  /* insert configuration name into the runtype table */
  num = numberConfigs();
  if (num != -1)
  {
    sprintf (queryString, "insert into %s\n", RUNTYPE_TABLE_NAME);
    sprintf (valString, "values ('%s', %d, 'no','')",
	     config, num);
    strcat  (queryString, valString);
    if (mysql_query(mysql, queryString) != 0)
    {
#ifdef _CODA_DEBUG
      printf ("Insert into runtype error: %s\n", mysql_error(mysql));
#endif
      return(-1);
    }
    else
    {
#ifdef _CODA_DEBUG
      printf ("Insert into runtype\n");
#endif
    }
  }  
  return(0);
}

int
createPositionTable (char* config)
{
  char queryString[1024];

  if (!databaseIsOpen()) return(-1);

  if (!databaseSelected()) return(-1);

  sprintf (queryString, "create table %s_pos(\n", config);
  strcat  (queryString, "name varchar(32) binary not null primary key,\n");
  strcat  (queryString, "row  int not null,\n");
  strcat  (queryString, "col  int not null\n");
  strcat  (queryString,")");
  if (mysql_query (mysql, queryString) != 0)
  {
#ifdef _CODA_DEBUG
    printf ("Create %s pos table Error: %s\n",config, mysql_error(mysql));
#endif
    return(-1);
  }
  else
  {
#ifdef _CODA_DEBUG
    printf ("Create %s pos table\n",config);
#endif
  }

  return(0);
}

int
createExpInfoTable (void)
{
  char queryString[1024];

  if (!databaseIsOpen ())
    return -1;

  if (!databaseSelected ())
    return -1;

  sprintf (queryString, "create table %s(\n", EXPINFO_TABLE_NAME);
  strcat  (queryString, "name varchar(64) binary not null primary key,\n");
  strcat  (queryString, "id int not null,\n");
  strcat  (queryString, "owner char(32) not null,\n");
  strcat  (queryString, "inuse char(32) not null,\n");
  strcat  (queryString, "log_name char(32) not null,\n");
  strcat  (queryString, "rc_name char(32) not null,\n");
  strcat  (queryString, "runNumber int not null,\n");
  strcat  (queryString, "config char(32) not null\n");
  strcat  (queryString,")");
  if (mysql_query (mysql, queryString) != 0)
  {
    fprintf (stderr, "Command failed: %s\n", mysql_error(mysql));
    return(-1);
  }
  return(0);
}

int
createProcessTable (void)
{
  char queryString[1024];

  if (!databaseIsOpen ())
    return -1;

  if (!databaseSelected ())
    return -1;

  sprintf (queryString, "create table %s(\n", PROCESS_TABLE_NAME);
  strcat  (queryString, "name varchar(32) binary not null primary key,\n");
  strcat  (queryString, "id int not null,\n");
  strcat  (queryString, "cmd char(128) not null,\n");
  strcat  (queryString, "type char(32) not null,\n");
  strcat  (queryString, "host char(32) not null,\n");
  strcat  (queryString, "port int not null,\n");
  strcat  (queryString, "state char(32) not null,\n");
  strcat  (queryString, "pid int not null,\n");
  strcat  (queryString, "inuse char(32) not null,\n");
  strcat  (queryString, "clone char(32) not null\n");
  strcat  (queryString,")");

  if(mysql_query (mysql, queryString) != 0) return(-1);
  return(0);
}

int 
createOptionTable (char* config)
{
  char queryString[1024];
  char valString  [256];
  MYSQL_RES *res;

  if (!databaseIsOpen ())
    return -1;

  if (!databaseSelected ())
    return -1;

  sprintf (queryString, "select * from %s_option", config);
  if (mysql_query (mysql, queryString) != 0)
  {
#ifdef _CODA_DEBUG
    printf ("Select %s option table error: %s\n", config, mysql_error(mysql));
#endif
    sprintf (queryString, "create table %s_option(\n", config);
    strcat  (queryString, "name char(32) not null,\n");
    strcat  (queryString, "value  char(80) not null\n");
    strcat  (queryString, ")");
    if (mysql_query (mysql, queryString) != 0)
    {
#ifdef _CODA_DEBUG
      printf ("Create %s option table error: %s\n", config, mysql_error(mysql));
#endif
      return(-1);
    }


    /* insert default data limit */
    printf("insert defaults\n");
    sprintf (queryString, "insert into %s_option\n",config);
    sprintf (valString, "values ('dataLimit', '0')");
    strcat  (queryString, valString);
    if (mysql_query (mysql, queryString) != 0)
    {
#ifdef _CODA_DEBUG
      printf ("insert %s to option table failed: %s\n", config, mysql_error(mysql));
#endif
    }
    
    /* insert event limit */
    sprintf (queryString, "insert into %s_option\n",config);
    sprintf (valString, "values ('eventLimit', '0')");
    strcat  (queryString, valString);
    if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
      printf ("insert %s to option table failed: %s\n", config, mysql_error(mysql));
#endif
    }
    
    /* insert token interval*/
    sprintf (queryString, "insert into %s_option\n",config);
    sprintf (valString, "values ('tokenInterval', '64')");
    strcat  (queryString, valString);
    if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
      printf ("insert %s to option table failed: %s\n", config, mysql_error(mysql));
#endif
    }
  }
  else
  {
#ifdef _CODA_DEBUG
    printf ("Select %s option table successful, clear 'res'\n", config);
#endif
    res = mysql_store_result (mysql);
    mysql_free_result(res);
  }

  return(0);
}

int 
createScriptTable (char* config)
{
  char queryString[1024];

  if (!databaseIsOpen ()) return(-1);

  if (!databaseSelected ()) return(-1);

  sprintf (queryString, "create table %s_script(\n", config);
  strcat  (queryString, "name char(32) not null,\n");
  strcat  (queryString, "state char(32) not null,\n");
  strcat  (queryString, "script char(128) not null\n");
  strcat  (queryString, ")");
  if (mysql_query (mysql, queryString) != 0)
  {
#ifdef _CODA_DEBUG
    printf ("Create %s script table error: %s\n", config, mysql_error(mysql));
#endif
    return(-1);
  }
  else
  {
#ifdef _CODA_DEBUG
    printf ("Create %s script table\n", config);
#endif
  }

  return(0);
}


int
createPriorityTable (void)
{
  char queryString[1024];
  char valString[256];
  int  status;

  if (!databaseIsOpen ())
    return -1;

  if (!databaseSelected ())
    return -1;

  sprintf (queryString, "create table %s(\n", PRIORITY_TABLE_NAME);
  strcat  (queryString, "class char(32) not null,\n");
  strcat  (queryString, "priority int not null\n");
  strcat  (queryString,")");
  if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
    printf ("create priority table error: %s\n", mysql_error(mysql));
#endif
    return -1;
  }
  /* insert value to table */
  sprintf (queryString, "insert into %s\n", PRIORITY_TABLE_NAME);
  /* roc class */
  strcat  (queryString, "values ('ROC', 11)");
  if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
    printf ("Insert priority value error: %s\n", mysql_error(mysql));
#endif
    return -1;
  }
  /* EB class */
  sprintf (queryString, "insert into %s\n", PRIORITY_TABLE_NAME);  
  strcat  (queryString, "values ('EB',  15)"); 
  if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
    printf ("Insert priority value error: %s\n", mysql_error(mysql));
#endif
    return -1;
  }
  /* ANA class */
  sprintf (queryString, "insert into %s\n", PRIORITY_TABLE_NAME);  
  strcat  (queryString, "values ('ANA', 19)");
  if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
    printf ("Insert priority value error: %s\n", mysql_error(mysql));
#endif
    return -1;
  }
  /* ER class */
  sprintf (queryString, "insert into %s\n", PRIORITY_TABLE_NAME);  
  strcat  (queryString, "values ('ER',  23)"); 
  if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
    printf ("Insert priority value error: %s\n", mysql_error(mysql));
#endif
    return -1;
  }
  /* LOG class */
  sprintf (queryString, "insert into %s\n", PRIORITY_TABLE_NAME);  
  strcat  (queryString, "values ('LOG', 27)"); 
  if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
    printf ("Insert priority value error: %s\n", mysql_error(mysql));
#endif
    return -1;
  }
  /* TS class */
  sprintf (queryString, "insert into %s\n", PRIORITY_TABLE_NAME);  
  strcat  (queryString, "values ('TS', -27)"); 
  if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
    printf ("Insert priority value error: %s\n", mysql_error(mysql));
#endif
    return -1;
  }
  return 0;
}
  

int
createRunTypeTable (void)
{
  char queryString[1024];

  if (!databaseIsOpen ())
    return(-1);

  if (!databaseSelected ())
    return(-1);

  sprintf (queryString, "create table %s(\n", RUNTYPE_TABLE_NAME);
  strcat  (queryString, "name varchar(32) binary not null primary key,\n");
  strcat  (queryString, "id int not null,\n");
  strcat  (queryString, "inuse char(32) not null,\n");
  strcat  (queryString, "category char(32) not null\n");
  strcat  (queryString,")");

  if(mysql_query (mysql, queryString) != 0) return(-1);
  return(0);
}









int
listAllTables (char* tables[], int* num)
{
  MYSQL_RES *res;
  MYSQL_ROW row;
  int       i = 0;
  
  if (databaseSelected ()) {
    res = mysql_list_tables(mysql, NULL);
    if (!res)
    {
      *num = 0;
      return -1;
    }

    while ((row = mysql_fetch_row (res)))
    {
      tables[i] = (char *)malloc ((strlen (row[0]) + 1)*sizeof (char));
      strcpy (tables[i], row[0]);
      i++;
    }
    *num = i;
    mysql_free_result (res);
    return 0;
  }
  *num = 0;
  return -1;
}

int
listAllConfigs (char* configs[], int* num)
{
  char queryString[1024];
  MYSQL_RES *res;
  MYSQL_ROW row;
  int      i = 0;

  *num = 0;
  if (databaseSelected ())
  {
    sprintf (queryString, "select * from %s", RUNTYPE_TABLE_NAME);
    if (mysql_query (mysql, queryString) != 0)
    {
#ifdef _CODA_DEBUG
      printf ("listAllConfigs error: %s\n", mysql_error(mysql));
#endif
      return(-1);
    }
    res = mysql_store_result (mysql);
    if (!res)
    {
#ifdef _CODA_DEBUG
      printf ("Query in listAllConfigs failed: %s \n", mysql_error(mysql));
#endif
      return(-1);
    }
    i = 0;
    while ((row = mysql_fetch_row (res)))
    {
      configs[i] = (char *)malloc ((strlen (row[0]) + 1) * sizeof (char));
      strcpy (configs[i], row[0]);
      i++;
    }
    *num = i;
    mysql_free_result (res);
    return(0);
  }
  return(-1);
}

int
numberConfigs(void)
{
  char queryString[1024];
  MYSQL_RES *res;
  MYSQL_ROW row;
  int       i = 0;

  if (databaseSelected ())
  {
    sprintf (queryString, "select * from %s", RUNTYPE_TABLE_NAME);
    if (mysql_query (mysql, queryString) != 0)
    {
#ifdef _CODA_DEBUG
      printf ("select * from %s error: %s\n", RUNTYPE_TABLE_NAME,
                                                 mysql_error(mysql));
#endif
      return(-1);
    }
    else
    {
#ifdef _CODA_DEBUG
      printf ("select * from %s\n", RUNTYPE_TABLE_NAME);
#endif
    }

    if( (res = mysql_store_result (mysql)) == NULL )
    {
#ifdef _CODA_DEBUG
      printf ("Query in numberConfigs failed: %s \n", mysql_error(mysql));
#endif
      return(-1);
    }
    i = 0;
    while ( (row = mysql_fetch_row (res)) != NULL ) i++;
    mysql_free_result (res);
    return(i);
  }
  return(-1);
}

int
isConfigCreated (char* config)
{
  char *configs[200]; /* no way to exceed 200 configuration */
  int  num, i;
  int  found = 0;

  if (listAllConfigs (configs, &num) < 0) {
    return -1;
  }

  for (i = 0; i < num; i++) {
    if (strcmp (config, configs[i]) == 0) 
      found = 1;
    free (configs[i]);
  }
  return found;
}

int
isTableCreated (char* name)
{
  char *names[200]; /* no way to exceed 200 configuration */
  int  num, i;
  int  found = 0;

  if (listAllConfigs (names, &num) < 0) {
    return -1;
  }

  for (i = 0; i < num; i++) {
    if (strcmp (name, names[i]) == 0) 
      found = 1;
    free (names[i]);
  }
  return found;
}


/**********************************************************/
/***************** remove table operations ****************/
/**********************************************************/

int
removePositionTable (char* config)
{
  char queryString[1024];
  
  if (databaseSelected ()) {
    sprintf (queryString, "drop table %s_pos", config);
    if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
      printf ("Cannot remove %s position table: %s\n", config, mysql_error(mysql));
#endif
      return -1;
    }
    return 0;
  }
  return -1;
}

int
removeScriptTable (char* config)
{
  char queryString[1024];
  
  if (databaseSelected ()) {
    sprintf (queryString, "drop table %s_script", config);
    if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
      printf ("Cannot remove %s script table: %s\n", config, mysql_error(mysql));
#endif
      return -1;
    }
    return 0;
  }
  return -1;
}


int
removeOptionTable (char* config)
{
  char queryString[1024];
  
  if (databaseSelected ()) {
    sprintf (queryString, "drop table %s_option", config);
    if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
      printf ("Cannot remove %s option table: %s\n", config, mysql_error(mysql));
#endif
      return -1;
    }
    return 0;
  }
  return -1;
}

int
removeConfigTable (char* config)
{
  char queryString[1024];
  char valString[256];
  char* configs[200];
  int   num, i, j;
  char* tables[200];
  int   numtables = 0;
  int   found = 0;
  
  if (databaseSelected ())
  {

    sprintf (queryString, "drop table %s", config);
    if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
      printf ("Cannot remove %s config table: %s\n", config, mysql_error(mysql));
#endif
    }
    sprintf (queryString, "drop table %s_script", config);
    if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
      printf ("Cannot remove %s_script table: %s\n", config, mysql_error(mysql));
#endif
    }
    sprintf (queryString, "drop table %s_pos", config);
    if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
      printf ("Cannot remove %s_pos table: %s\n", config, mysql_error(mysql));
#endif
    }

    /* delete this config from run type table */
    sprintf (queryString, "delete from %s\n", RUNTYPE_TABLE_NAME);
    sprintf (valString, "where name = '%s'",config);
    strcat  (queryString, valString);
    if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
      printf ("Cannot remove %s from runtype: %s\n", config, mysql_error(mysql));
#endif
    }

    if (listAllConfigs(configs, &num) == 0)
    { 
      for (i = 0; i < num; i++)
      {
        sprintf (queryString, "update %s set id = %d where name = '%s'",
		  RUNTYPE_TABLE_NAME, i, configs[i]);
        if (mysql_query (mysql, queryString) != 0)
        {
#ifdef _CODA_DEBUG
          printf ("Cannot update %s to runtype: %s\n", mysql_error(mysql));
#endif
        }
      }
	}
  }
  return(0);
}


/**********************************************************/
/***************** update table operations ****************/
/**********************************************************/

int
insertValToPosTable (char* config, char* name, int row, int col)
{
  char queryString[1024];
  char valString[256];

  if (databaseSelected ())
  {
    sprintf (queryString, "insert into %s_pos (name, row, col) ",config);
    sprintf (valString, "values ('%s', %d, %d)",name,row,col);
    strcat  (queryString, valString);
    if (mysql_query (mysql, queryString) != 0)
    {
#ifdef _CODA_DEBUG
      printf ("insert %s to position table failed: %s\n", config, mysql_error(mysql));
#endif
      return(-1);
    }
	else
	{
#ifdef _CODA_DEBUG
      printf ("insert %s to position table\n", config);
#endif
	}
    return(0);
  }
  return(-1);
}

int
insertValToOptionTable (char* config, char* name, char* value)
{
  char queryString[1024];
  char valString[256];

  if (databaseSelected ())
  {
    /* delete old entry */
    sprintf (queryString, "delete from %s_option where name = '%s'",
	     config, name);
    if(mysql_query (mysql, queryString) != 0)
    {
#ifdef _CODA_DEBUG
      printf ("delete from %s_option table failed: %s\n", config, mysql_error(mysql));
#endif
    }

    /* insert the new one */
    sprintf (queryString, "insert into %s_option\n",config);
    if (value != 0) 
      sprintf (valString, "values ('%s', '%s')", name, value);
    else
      sprintf (valString, "values ('%s', '')", name, value);
    strcat  (queryString, valString);
    if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
      printf ("insert %s to option table failed: %s\n", config, mysql_error(mysql));
#endif
    }
  }
  return 0;
}

int
insertValToScriptTable (char* config, char* name, codaScript* list)
{
  char queryString[1024];
  char valString[256];
  codaScript* p;

  if (list == 0)
    return -1;

  if (databaseSelected ()) {
    /* MSQL is stupid: not to allow one to insert multiple values */
    /* at the same time                                           */
    for (p = list; p != 0; p = p->next) {
      sprintf (queryString, "insert into %s_script\n",config);
      sprintf (valString, "values ('%s', '%s', '%s')", 
	       name, p->state, p->script);
      strcat  (queryString, valString);
      if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
	printf ("insert %s to script table failed: %s\n", config, mysql_error(mysql));
#endif
	return -1;
      }
    }
    return 0;
  }
  return -1;
}

int 
insertValToConfigTable (char* config, char* name, char* code,
			char* inputs, char* outputs, char* next, int first)
{
  char queryString[1024];
  char valString[512];  

  if (databaseSelected ()) {
    sprintf (queryString, "insert into %s\n",config);
    if (next) {
      if (first) 
	sprintf (valString, "values ('%s','%s','%s','%s','yes','%s','no')",
		 name, code, inputs, outputs, next);
      else
	sprintf (valString, "values ('%s','%s','%s','%s','no','%s','no')",
		 name, code, inputs, outputs, next);
    }
    else {
      if (first)
	sprintf (valString, "values ('%s','%s','%s','%s','yes','','no')",
		 name, code, inputs, outputs);
      else
	sprintf (valString, "values ('%s','%s','%s','%s','no','','no')",
		 name, code, inputs, outputs);
    }
    strcat (queryString, valString);
    if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
      printf ("insert %s to config table failed: %s\n", config, mysql_error(mysql));
#endif
      return -1;
    }
    return 0;
  }
  return -1;
}

int
removeDaqCompFromProcTable (char* name)
{
  char queryString[1024];
  char keyString[256];


  if (databaseSelected ()) {
    sprintf (queryString, "delete from %s\n",PROCESS_TABLE_NAME);
    sprintf (keyString, "where name='%s'",name);
    strcat (queryString, keyString);
    if (mysql_query (mysql, queryString) != 0) 
      return -1;
    return 0;
  }
  return -1;
}

int
isDaqCompInProcTable (char* name)
{
  char queryString[1024];
  char keyString[256];
  MYSQL_RES *res;
  MYSQL_ROW row;

  if (!databaseSelected ()) 
    return -1;

  sprintf (queryString, "select * from %s\n",PROCESS_TABLE_NAME);
  sprintf (keyString, "where name='%s'",name);
  strcat (queryString, keyString);
  if (mysql_query (mysql, queryString) != 0)
  {
#ifdef _CODA_DEBUG
    printf ("Search component in the proc table error: %s\n", mysql_error(mysql));
#endif
    return -1;
  }
  res = mysql_store_result (mysql);
  if (!res)
    return 0;
  row = mysql_fetch_row (res);
  if (!row) {
    mysql_free_result (res);
    return 0;
  }
  if (strcmp (row[0], name) != 0) {
    mysql_free_result (res);
    return 0;
  }
  mysql_free_result (res);
  return 1;
}

int
insertDaqCompToProcTable (daqComp* comp)
{
  char queryString[1024];
  char keyString[256];
  char valString[512];

  if (databaseSelected ()) {
    sprintf (queryString, "insert into %s\n", PROCESS_TABLE_NAME);
    if (comp->boot_string != 0) 
      sprintf (valString, "values ('%s',%d,'%s','%s','%s',0,'dormant',0,'no','no')",
	       comp->comp_name, comp->id_num, comp->boot_string, 
	       compTypeString [comp->type], comp->node_name);
    else
      sprintf (valString, "values ('%s',%d,'','%s','%s',0,'dormant',0,'no','no')",
	       comp->comp_name, comp->id_num, 
	       compTypeString [comp->type], comp->node_name);
    strcat (queryString, valString);
    if (mysql_query (mysql, queryString) != 0)
      return -1;
    return 0;
  }
  return -1;
}

int
updateDaqCompToProcTable (daqComp* comp)
{
  char queryString[1024];

  if (databaseSelected ()) {
    if (comp->boot_string != 0) 
      sprintf (queryString, "update %s set id = %d, cmd = '%s', type = '%s', host = '%s' where name = '%s'", 
	       PROCESS_TABLE_NAME, comp->id_num, comp->boot_string, 
	       compTypeString [comp->type], comp->node_name,
	       comp->comp_name);
    else
      sprintf (queryString, "update %s set id = %d, cmd = '', type = '%s', host = '%s' where name = '%s'", 
	       PROCESS_TABLE_NAME, comp->id_num, 
	       compTypeString [comp->type], comp->node_name,
	       comp->comp_name);
    if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
      printf ("Update %s component to process table error: %s\n", comp->comp_name,
	      mysql_error(mysql));
#endif
      return(-1);
    }
    return(0);
  }

  return(-1);
}

int
selectConfigTable (char* config)
{
  if (runType) {
    free (runType);
    runType = 0;
  }
  runType = (char *)malloc ((strlen(config) + 1)*sizeof(config));
  strcpy (runType, config);
  return 0;
}

char*
currentConfigTable (void)
{
  return runType;
}

void
removeMiscConfigInfo (void)
{
  if (runType) free (runType);
  runType = 0;
}

int
createRcNetCompsFromDbase (rcNetComp** comp, int *num)
{
  char queryString [1024];
  int  i = 0;
  MYSQL_RES *res = 0;
  MYSQL_ROW row;
  char      errmsg[256];

  *num = 0;
  if (!databaseSelected ()) return(-1);

  sprintf (queryString, "select * from %s", PROCESS_TABLE_NAME);
  if (mysql_query (mysql, queryString) != 0)
  {
#ifdef _CODA_DEBUG
    printf ("Get all from process table error: %s\n", mysql_error(mysql));
#endif
    sprintf (errmsg, "Get all components failed: %s", mysql_error(mysql));
    pop_error_message (errmsg, sw_geometry.draw_area);
    return -1;
  }
  res = mysql_store_result (mysql);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("Query get all from process table error: %s\n", mysql_error(mysql));
#endif
    sprintf (errmsg, "Query process table failed: %s", mysql_error(mysql));
    pop_error_message (errmsg, sw_geometry.draw_area);
    return -1;
  }
  i = 0;
  while ((row = mysql_fetch_row (res)))
  {
#ifdef _CODA_DEBUG
    printf ("Construct comp %s %s %s %s %s \n",row[0], row[1], row[2], row[3], row[4]);
#endif
    if (strcasecmp (row[3], "RCS") != 0 && strcasecmp (row[3], "USER") != 0)
    {
      comp[i] = newRcNetComp ();
      setRcNetComp (comp[i], row[0], atoi (row[1]), row[2],
		    row[3], row[4]);
      i++;
    }
  }
  mysql_free_result (res);
  *num = i;
  return 0;
}

int
retrieveConfigInfoFromDbase (char* config, ConfigInfo** cinfo, int* num)
{
  char queryString[1024];
  MYSQL_RES *res = 0;
  MYSQL_ROW row;
  int  i = 0, j = 0;
  char      errmsg[256];

  *num = 0;
  if (!databaseSelected ()) return(-1);
  
  sprintf (queryString, "select * from %s", config);
  if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
    printf ("get all from %s table error: %s\n", mysql_error(mysql));
#endif
    sprintf (errmsg, "Query %s table failed: %s", config, mysql_error(mysql));
    pop_error_message (errmsg, sw_geometry.draw_area);
    return -1;
  }
  res = mysql_store_result (mysql);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("Query get all from %s table error: %s\n", mysql_error(mysql));
#endif
    sprintf (errmsg, "Query %s table failed: %s", config, mysql_error(mysql));
    pop_error_message (errmsg, sw_geometry.draw_area);
    return -1;
  }
  i = 0;
  while ((row = mysql_fetch_row (res)))
  {
    cinfo[i] = newConfigInfo ();
#ifdef _CODA_DEBUG
    printf ("config info %s %s %s %s\n",row[0], row[1], row[2], row[3]);
#endif
    setConfigInfoName     (cinfo[i], row[0]);
    setConfigInfoCode     (cinfo[i], row[1]);
    setConfigInfoInputs   (cinfo[i], row[2]);
    setConfigInfoOutputs  (cinfo[i], row[3]);
    i++;
  }
  mysql_free_result (res);
  *num = i;

  /* get position information */
  sprintf (queryString, "select * from %s_pos", config);
  if (mysql_query (mysql, queryString) != 0)
  {
#ifdef _CODA_DEBUG
    printf ("get all from %s position table error: %s\n", config, mysql_error(mysql));
#endif
    sprintf (errmsg, "Query %s_pos table failed: %s", config, mysql_error(mysql));
    pop_error_message (errmsg, sw_geometry.draw_area);
    return -1;
  }
  res = mysql_store_result (mysql);
  if (!res)
  {
#ifdef _CODA_DEBUG
    printf ("Query get all from %s table error: %s\n", mysql_error(mysql));
#endif
    sprintf (errmsg, "Query %s_pos table failed: %s", config, mysql_error(mysql));
    pop_error_message (errmsg, sw_geometry.draw_area);
    return -1;
  }

  j = *num;
  while ((row = mysql_fetch_row (res)))
  {
    for (i = 0; i < *num; i++) {
      if (matchConfigInfo (cinfo[i], row[0]))
	setConfigInfoPosition (cinfo[i], atoi (row[1]), atoi (row[2]));
    }
  }
  mysql_free_result (res);

  /* get script information */
  sprintf (queryString, "select * from %s_script", config);
  if (mysql_query (mysql, queryString) != 0)
  {
#ifdef _CODA_DEBUG
    printf ("get all from %s script table error: %s\n", config, mysql_error(mysql));
#endif
    return(0);
  }

  res = mysql_store_result (mysql);
  if (!res)
  {
#ifdef _CODA_DEBUG
    printf ("Query get all from %s table error: %s\n", mysql_error(mysql));
#endif
    return 0;
  }

  while ((row = mysql_fetch_row (res)))
  {
    for (i = 0; i < *num; i++)
    {
      if (matchConfigInfo (cinfo[i], row[0]))
      {
        addScriptToConfigInfo (cinfo[i], row[1], row[2]);
      }
    }
  }
  mysql_free_result (res);
  
  return(0);
}

int
getAllOptionInfos (char* config, char*** names, char*** values)
{
  char queryString[256];
  MYSQL_RES *res;
  MYSQL_ROW row;
  int      i, j;
  char **tnames, **tvalues;
  char      errmsg[256];

  /* get position information */
  printf("query\n");

  sprintf (queryString, "select * from %s_option", config);
  if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
    printf ("get all from %s option table error: %s\n", config, mysql_error(mysql));
#endif
    sprintf (errmsg, "Query %s_option table failed: %s", config, mysql_error(mysql));
    pop_error_message (errmsg, sw_geometry.draw_area);
    return -1;
  }
  res = mysql_store_result (mysql);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("Query get all from %s table error: %s\n", mysql_error(mysql));
#endif
    sprintf (errmsg, "Query %s_option table failed: %s", config, mysql_error(mysql));
    pop_error_message (errmsg, sw_geometry.draw_area);
    return -1;
  }

  i = mysql_num_rows(res);
  
  if (i > 0) {
    tnames = (char **)malloc (i*sizeof (char *));
    tvalues = (char **)malloc (i*sizeof (char *));
    
    j = 0;
    while ((row = mysql_fetch_row (res))) {
      tnames[j] = (char *)malloc((strlen(row[0]) + 1)*sizeof (char));
      strcpy (tnames[j], row[0]);
      tvalues[j] = (char *)malloc((strlen(row[1]) + 1)*sizeof (char));
      strcpy (tvalues[j], row[1]);
      j++;
      printf("read %s %s\n",row[0],row[1]);
    }
    assert (j == i);
    *names = tnames;
    *values = tvalues;
  }
  else
  {
    *names = 0;
    *values = 0;
  }
    
  mysql_free_result (res);

  return(i);
}

int
compInConfigTables (char* name)
{
  char* configs[200];
  int  num, i;
  char queryString[1024];
  MYSQL_RES *res;
  MYSQL_ROW row;
  char      errmsg[256];

  if (!databaseSelected ())
    return -1;

  if (listAllConfigs (configs, &num) < 0)
    return -1;

  for (i = 0; i < num; i++) {
    sprintf (queryString, "select * from %s where name = '%s'", configs[i], name);
    if (mysql_query (mysql, queryString) != 0) {
#ifdef _CODA_DEBUG
      printf ("check comp in table error: %s\n", mysql_error(mysql));
#endif
      sprintf (errmsg, "Query %s table failed: %s", configs[i], mysql_error(mysql));
      pop_error_message (errmsg, sw_geometry.draw_area);
      return -1;
    }
    res = mysql_store_result (mysql);
    if (res)
    {
      row = mysql_fetch_row(res);
      if (row)
      {
        if (strcmp (row[0], name) == 0)
        {
          mysql_free_result (res);
          return 1;
        }
      }
      mysql_free_result (res);
    }
  }

  return(0);
}

    
