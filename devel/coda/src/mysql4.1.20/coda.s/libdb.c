
/* libdb.c - interface library to the database (currently mysql) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "libdb.h"

#define ROC_OK     0
#define ROC_ERROR  1


/* we will make sure that dbDisconnect always called before next dbConnect
#ifdef VXWORKS
static SEM_ID udp_lock;
#else
static pthread_mutex_t udp_lock;
#endif
*/

/* UNIX function(s) missing in VXWORKS */
#ifdef VXWORKS

#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <hostLib.h>
#include <in.h>
#include "mynetdb.h"

  static struct hostent h;
  static struct sockaddr_in sin;
  static char *addrlist[2];

struct hostent *
gethostbyname(const char *hostName) 
{
  int host;

  if ((host = hostGetByName((char *)hostName))==ERROR)
  {
    printf("netdb: unknown host >%s<\n",hostName);
    return(NULL);
  }
  h.h_name = (char *)hostName;
  h.h_addr_list = addrlist;
  addrlist[0] = (char *) &sin.sin_addr;
  h.h_length = sizeof(sin.sin_addr);
  bzero((char *)&sin, sizeof(sin));
  sin.sin_addr.s_addr = host;

  return(&h);
}

#endif




MYSQL *
dbConnect(const char *host, const char *database)
{
  MYSQL *mysql;
  int iattempts, nattempts = 5;

  const char *user = "clasrun";
  const char *passwd = "";
  unsigned int port = 0;
  const char *unix_socket = "";
  unsigned long client_flag = 0;

  char dbaseServerHost[128];   /* database server host name */
  char db[128]; /* normally comes from EXPID, add 'daq_' */

  /*
#ifdef VXWORKS
  udp_lock = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if(udp_lock == NULL)
  {
    logMsg("UDP_establish: ERROR: could not allocate a semaphore\n",1,2,3,4,5,6);
    return(0);
  }
#else
  pthread_mutex_init(&udp_lock, NULL);
#endif
#ifdef VXWORKS
  semTake(udp_lock, WAIT_FOREVER);
#else
  pthread_mutex_lock(&udp_lock);
#endif
  */

  iattempts=0;

tryagain:

  if(iattempts > nattempts)
  {
    printf("dbConnect: cannot connect after %d attempts - severe ERROR\n",iattempts);
    return(NULL);
  }

  iattempts ++;

#ifdef DEBUG
printf("INFO(dbConnect1): errno=%d\n",errno);
perror("INFO(dbConnect1)");
#endif

  if(strlen(host) > 0)
  {
    strncpy (dbaseServerHost, host, sizeof (dbaseServerHost));
  }
  else
  {
    printf("dbConnect WARN: host is not specified, use 'clondb1'\n");
    strncpy (dbaseServerHost, "clondb1", sizeof (dbaseServerHost));
  }
  sprintf (db, "daq_%s",database);
#ifdef DEBUG
  printf("dbConnect: host >%s<, database >%s<\n",dbaseServerHost,db);
#endif
  /*initialize MYSQL structure; use 'mysql_options'*/
  mysql = mysql_init(NULL);
#ifdef DEBUG
  printf("dbConnect: after mysql_init mysql=0x%08x\n",mysql);
#endif
  if(mysql == NULL) return(NULL);

  /* connect to database */
  /*
  printf("dbConnect: connecting: mysql=0x%08x dbaseServerHost=>%s< user=>%s< passwd=>%s<\n",
                              mysql,
                              dbaseServerHost,
                              user,
                              passwd);
  printf("dbConnect: connecting: db=>%s< port=%d unix_socket=>%s< flag=%d\n",
                              db,
                              port,
                              unix_socket,
                              client_flag);
  */
  mysql = mysql_real_connect( mysql,
                              dbaseServerHost,
                              user,
                              passwd,
                              db,
                              port,
                              unix_socket,
                              client_flag);
#ifdef DEBUG
  printf("dbConnect: after mysql_real_connect mysql=0x%08x\n",mysql);
printf("INFO(dbConnect9): errno=%d\n",errno);
perror("INFO(dbConnect9)");
#endif

  if(mysql==NULL)
  {
    printf("dbConnect: attempt %d failed, trying again\n",iattempts);
#ifdef VXWORKS
    taskDelay(sysClkRateGet()*3);
#else
    sleep(3);
#endif
    goto tryagain;
  }

  return(mysql);
}



void
dbDisconnect(MYSQL *mysql)
{
#ifdef DEBUG
  printf("dbDisconnect: mysql=0x%08x\n",mysql);
printf("INFO(dbDisconnect1): errno=%d\n",errno);
perror("INFO(dbDisconnect1)");
#endif
  if (mysql != NULL) mysql_close (mysql);
#ifdef DEBUG
printf("INFO(dbDisconnect9): errno=%d\n",errno);
perror("INFO(dbDisconnect9)");
#endif
  mysql = NULL;


  /*
#ifdef VXWORKS
  semGive(udp_lock);
#else
  pthread_mutex_unlock(&udp_lock);
#endif
#ifdef VXWORKS
  semGive(udp_lock);
  semDelete(udp_lock);
#else
  pthread_mutex_unlock(&udp_lock);
  pthread_mutex_destroy(&udp_lock);
#endif
  */
}


int
dbGetInt(MYSQL *mysql, char *str, int *value)
{
  MYSQL_RES *res = 0;
  MYSQL_ROW row;

  if(mysql_query(mysql,str) != 0)
  {
    printf("dbGetInt  ERROR: mysql=0x%08x, query >%s<\n",mysql,str);
    return(ROC_ERROR);
  }
  else
  {
    /*printf("id selected\n")*/;
  }

  /* gets results from previous query */
  if(!(res = mysql_store_result (mysql) ))
  {
    printf("dbGetInt: ERROR in mysql_store_result()\n");
    return(ROC_ERROR);
  }
  else
  {
	/*
    printf("nrow=%d nfields=%d\n",tableRes->numRows,tableRes->numFields);
	*/
    if(mysql_num_rows(res) == 1)
    {
	  /*
      printf("dbGetInt: value >%s< %d\n",*(tableRes->queryData->data),
        atoi(*(tableRes->queryData->data)));
	  */
      row = mysql_fetch_row (res);
      *value = atoi(row[0]);
    }
    else
    {
      printf("dbGetInt: ERROR: unknown nrow\n");
      return(ROC_ERROR);
    }

    mysql_free_result(res);
  }

  return(ROC_OK);
}

int
dbGetStr(MYSQL *mysql, char *str, char *strout)
{
  MYSQL_RES *res = 0;
  MYSQL_ROW row;
  int ret;

  if((ret=mysql_query(mysql,str)) != 0)
  {
    printf("dbGetStr ERROR: mysql=0x%08x, query >%s<, ret=%d\n",mysql,str,ret);
    return(ROC_ERROR);
  }
  else
  {
    /*printf("id selected\n")*/;
  }

  /* gets results from previous query */
  if(!(res = mysql_store_result (mysql) ))
  {
    printf("dbGetStr: ERROR in mysql_store_result()\n");
    return(ROC_ERROR);
  }
  else
  {
	/*
    printf("nrow=%d nfields=%d\n",tableRes->numRows,tableRes->numFields);
	*/
    if(mysql_num_rows(res) == 1)
    {
	  /*
      printf("dbGetStr: strout >%s<\n",tableRes->queryData->data[0]);
	  */
      row = mysql_fetch_row (res);
      strcpy(strout, row[0]);
    }
    else
    {
      printf("dbGetStr: ERROR: unknown nrow\n");
      return(ROC_ERROR);
    }

    mysql_free_result(res);
  }

  return(ROC_OK);
}

