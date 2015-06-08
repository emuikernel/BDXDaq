
/* mysql client test program */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql.h>



static MYSQL* mysql = NULL;
static char   dbaseServerHost[128];

#ifdef VXWORKS
int
mysql_client_test_simple()
#else
int
main()
#endif
{
  MYSQL_RES *res;
  MYSQL_ROW row;

  const char *host = "clondb1"/*"129.57.167.67"*/;
  const char *user = "clasrun";
  const char *passwd = "";
  const char *db = "daq"; /* database name */
  unsigned int port = 0;
  const char *unix_socket = "";
  unsigned long client_flag = 0;

  printf("connectToDatabase() reached\n");

  strncpy (dbaseServerHost, host, sizeof (dbaseServerHost));

  /*initialize MYSQL structure; use 'mysql_options'*/
  mysql = mysql_init(mysql);
  printf("connectToDatabase: after mysql_init mysql=0x%08x\n",mysql);
  if(mysql == NULL) return(0);

  /* connect to database */
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

  mysql = mysql_real_connect( mysql,
                              dbaseServerHost,
                              user,
                              passwd,
                              db,
                              port,
                              unix_socket,
                              client_flag);

  printf("connectToDatabase: after mysql_real_connect mysql=0x%08x\n",mysql);
  if(mysql == NULL) return(0);

  /* */




  if (mysql_query(mysql, "SHOW TABLES") ||
        !((res= mysql_store_result(mysql)))) return(1);






  printf("closing connection to database\n");

  /* close database */
  if (mysql != NULL) mysql_close (mysql);
  mysql = NULL;


  return(0);
}

