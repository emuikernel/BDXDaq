/*
 *	tclSQL.c	- Source for sql commands
 *
 * Copyright (c) 1994   Brad Pepers
 *
 * Permission to use, copy, and distribute for non-commercial purposes,
 * is hereby granted without fee, providing that the above copyright
 * notice appear in all copies and that both the copyright notice and this
 * permission notice appear in supporting documentation.
 *
 * This software is provided "as is" without any expressed or implied
 * warranty.
 *
 * $Id: tclSQL.c,v 1.6 1997/11/20 20:19:26 rwm Exp $
 *
 * $Log: tclSQL.c,v $
 * Revision 1.6  1997/11/20 20:19:26  rwm
 * ifdef'd out unused code to zap cc warning.
 *
 * Revision 1.5  1997/03/28 13:56:33  heyes
 * remove msqlRegister and connectport
 *
 * Revision 1.4  1997/02/10 16:12:02  timmer
 * add Graham's register stuff
 *
 * Revision 1.3  1997/02/07 19:21:46  timmer
 * fixed usage statement for: msql connectPort
 *
 * Revision 1.1.1.1  1996/09/05 14:29:47  heyes
 * Initial Import
 *
 * Revision 1.6  1994/08/25  00:28:07  pepers
 * Changed for mSQL version 0.2
 *
 * Revision 1.5  1994/07/28  23:18:05  pepers
 * Added rows and cols commands
 *
 * Revision 1.4  1994/07/28  22:44:43  pepers
 * Changed around how commands work
 *
 * Revision 1.3  1994/07/28  02:04:54  pepers
 * Changed close command to disconnect
 *
 * Revision 1.2  1994/07/27  22:43:27  pepers
 * Fixed log files
 *
 * Revision 1.1  1994/07/27  22:34:48  pepers
 * First version of mSQL to Tcl/Tk interface
 *
 *
 * Sergey B. June 2008: migrate to MySQL
 *
 */

#include <stdlib.h>
#include <string.h>

#include "tcl.h"
#include "libdb.h"
#include "tclSQL.h"

static mysqlConn *mysqlConnHead = NULL;

int
MYSQL_Init(Tcl_Interp *interp)
{
  Tcl_CreateCommand(interp, "mysql", MYSQL_Cmd, 0, 0);
}

int
MYSQL_Cmd(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  if (argc < 2) {
    Tcl_AppendResult(interp, argv[0], ": missing command name.", 0);
    return TCL_ERROR;
  }

  if (strcmp(argv[1], "connect") == 0) {
    return MYSQL_Cmd_Connect(clientData, interp, argc, argv);
  } else if (strcmp(argv[1], "connectPort") == 0) {
    return MYSQL_Cmd_ConnectPort(clientData, interp, argc, argv);
  } else if (strcmp(argv[1], "connections") == 0) {
    return MYSQL_Cmd_Connections(clientData, interp, argc, argv);
  }

  Tcl_AppendResult(interp, argv[0], ": unsupported command.", 0);
  return TCL_ERROR;
}

int
MYSQL_Cmd_Connect(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  char *name = NULL;
  char *host = NULL;
  char tmp[40];
  MYSQL *socket;
  FILE *fp;
  mysqlConn *c,*cl;

  if (argc > 4 || argc < 3|| *argv[2] == '\0')
  {
    Tcl_AppendResult(interp, "usage: mysql connect name ?host?.", 0);
    return TCL_ERROR;
  }

  name = argv[2];

  if (argc > 3 && *argv[3] != '\0') host = argv[3];

  socket = dbConnect(host, getenv("EXPID"));
  if (socket == NULL)
  {
    printf("MYSQL_Cmd_Connect: error 'cause socket < 0\n");
    Tcl_AppendResult(interp, argv[0], ": ", mysql_error(socket), 0);
    return TCL_ERROR;
  }
  
  c = mysqlConnHead;
  
  while (c != NULL)
  {
    if (strcmp(c->name,name) == 0) 
      break;
    c = c->next;
  }

  if (c == NULL)
  {
    c = (mysqlConn *) malloc(sizeof(mysqlConn));
    memset((char *)c, 0, sizeof(mysqlConn));
  } 

  c->socket = socket;
  strcpy (c->name,name);
  if (host == NULL)
  {
    c->host = NULL;
  }
  else
  {
    c->host = (char *) malloc(strlen(host) + 1);
    strcpy (c->host,host);
  }

  c->next = mysqlConnHead;
  mysqlConnHead = c;
  
  if (c->database[0] != 0)
  {
    mysql_select_db(c->socket,c->database);
  }
  
  Tcl_CreateCommand(interp, name, MYSQL_Conn_Cmd, (ClientData) c, 0);
    
  return TCL_OK;
}

int
MYSQL_Cmd_ConnectPort(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  char *name = NULL;
  char *host = NULL;
  char *port = NULL;
  char tmp[40];
  FILE *fp;
  mysqlConn *c,*cl;

  Tcl_AppendResult(interp, argv[0], ": unsupported command.", 0);
  return TCL_ERROR;
}

int
MYSQL_Cmd_Connections(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  mysqlConn *c;
  char list[200];

  c = mysqlConnHead;
  
  while (c != NULL)
  {
    if (c->host != NULL) 
      sprintf(list,"%s %s",c->name,c->host);
    else
      sprintf(list,"%s \"\"",c->name);
    
    Tcl_AppendElement(interp, list);
    c = c->next;
  }
  return TCL_OK;
}

int
MYSQL_Conn_Cmd(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  if (argc < 2) {
    Tcl_AppendResult(interp, argv[0], ": missing command name.", 0);
    return TCL_ERROR;
  }

  if (strcmp(argv[1], "get") == 0)
    return MYSQL_Cmd_Get((mysqlConn *)clientData, interp, argc, argv);
  else if (strcmp(argv[1], "query") == 0)
    return MYSQL_Cmd_Query((mysqlConn *)clientData, interp, argc, argv); 
  else if (strcmp(argv[1], "register") == 0)
    return MYSQL_Cmd_Register((mysqlConn *)clientData, interp, argc, argv); 
  else if (strcmp(argv[1], "set") == 0)
    return MYSQL_Cmd_Set((mysqlConn *)clientData, interp, argc, argv);
  else if (strcmp(argv[1], "disconnect") == 0) 
    return MYSQL_Cmd_Disconnect((mysqlConn *)clientData, interp, argc, argv);
  

  Tcl_AppendResult(interp, argv[0], ": unsupported command.", 0);
  return TCL_ERROR;
}

int
MYSQL_Cmd_Disconnect(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  if (argc != 2) {
    Tcl_AppendResult(interp, "usage: ", argv[0], " disconnect.", 0);
    return TCL_ERROR;
  }

  dbDisconnect(c->socket);

  return Tcl_DeleteCommand(interp, argv[0]);
}

int
MYSQL_Cmd_Set(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  if (argc < 3) {
    Tcl_AppendResult(interp, "usage: ", argv[0], " set ???.", 0);
    return TCL_ERROR;
  }

  if (strcmp(argv[2], "database") == 0)
    return MYSQL_Set_DB(c, interp, argc, argv);
  else if (strcmp(argv[2], "row") == 0)
    return MYSQL_Set_Row(c, interp, argc, argv);

  Tcl_AppendResult(interp, argv[0], ": unknown set name ", argv[2], ".", 0);
  return TCL_ERROR;
}

int
MYSQL_Set_DB(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  int res;

  if (argc != 4) {
    Tcl_AppendResult(interp, argv[0], " ", argv[1], " ", argv[2],
		     ": missing database name", 0);
    return TCL_ERROR;
  }

  strcpy(c->database,argv[3]);
 retry:
  if ((res = mysql_select_db(c->socket, argv[3])) == -1) {
    Tcl_AppendResult(interp, argv[0], " ", argv[1], " ", argv[2],
		     ": ", mysql_error(c->socket), 0);
    return TCL_ERROR;
  }

  return TCL_OK;
}

int
MYSQL_Set_Row(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  int res;

  if (argc != 4) {
    Tcl_AppendResult(interp, argv[0], " ", argv[1], " ", argv[2],
		     ": missing row number", 0);
    return TCL_ERROR;
  }

  if (c->query_result == NULL) {
    Tcl_AppendResult(interp, argv[0], " ", argv[1], " ", argv[2],
		     ": no current query", 0);
    return TCL_ERROR;
  }

  mysql_data_seek(c->query_result, atoi(argv[3]));

  return TCL_OK;
}

int
MYSQL_Cmd_Get(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  if (argc < 3) {
    Tcl_AppendResult(interp, "usage: ", argv[0], " get ???.", 0);
    return TCL_ERROR;
  }

  if (strcmp(argv[2], "databases") == 0)
    return MYSQL_Get_DBs(c, interp, argc, argv);
  else if (strcmp(argv[2], "tables") == 0)
    return MYSQL_Get_Tables(c, interp, argc, argv);
  else if (strcmp(argv[2], "fields") == 0)
    return MYSQL_Get_Fields(c, interp, argc, argv);
  else if (strcmp(argv[2], "rest") == 0)
    return MYSQL_Get_Rest(c, interp, argc, argv);
  else if (strcmp(argv[2], "next") == 0)
    return MYSQL_Get_Next(c, interp, argc, argv);
  else if (strcmp(argv[2], "rows") == 0)
    return MYSQL_Get_Rows(c, interp, argc, argv);
  else if (strcmp(argv[2], "cols") == 0)
    return MYSQL_Get_Cols(c, interp, argc, argv);

  Tcl_AppendResult(interp, argv[0], ": unknown get name ", argv[2], ".", 0);
  return TCL_ERROR;
}

void
free_query_result(MYSQL_RES *query_result)
{
  if (query_result != NULL) {
    mysql_free_result(query_result);
    query_result = NULL;
  }
}

int
MYSQL_Get_DBs(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  Tcl_DString results;
  MYSQL_RES *result;
  MYSQL_ROW row;
  int num_cols;
  int pos;

  if (argc != 3) {
    Tcl_AppendResult(interp, "usage: ", argv[0], " get databases.", 0);
    return TCL_ERROR;
  }
 retry:
  result = mysql_list_dbs(c->socket, NULL);
  
  if (result == NULL) {
    if (mysql_error(c->socket)[0] != '\0') {
      Tcl_AppendResult(interp, argv[0], " ", argv[1], " ", argv[2],
		       ": ", mysql_error(c->socket), 0);
      return TCL_ERROR;
    }
    return TCL_ERROR;
  }


  Tcl_DStringInit(&results);
  
  num_cols = 1;
  while ((row = mysql_fetch_row(result)) != NULL) {
    Tcl_DStringStartSublist(&results);
    for (pos = 0; pos < num_cols; pos++) {
      Tcl_DStringAppendElement(&results, row[pos]);
    }
    Tcl_DStringEndSublist(&results);
  }
  
  mysql_free_result(result);
  
  Tcl_DStringResult(interp, &results);
  return TCL_OK;
}

int
MYSQL_Get_Tables(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  Tcl_DString results;
  MYSQL_RES *result;
  MYSQL_ROW row;
  int num_cols;
  int pos;

  if (argc != 3) {
    Tcl_AppendResult(interp, "usage: ", argv[0], " get tables.", 0);
    return TCL_ERROR;
  }

  result = mysql_list_tables(c->socket, NULL);

  if (result == NULL) {
    if (mysql_error(c->socket)[0] != '\0') {
      Tcl_AppendResult(interp, argv[0], " ", argv[1], " ", argv[2],
		       ": ", mysql_error(c->socket), 0);
      return TCL_ERROR;
    }
    return TCL_ERROR;
  }

  Tcl_DStringInit(&results);

  num_cols = 1;
  while ((row = mysql_fetch_row(result)) != NULL) {
    Tcl_DStringStartSublist(&results);

    for (pos = 0; pos < num_cols; pos++)
      Tcl_DStringAppendElement(&results, row[pos]);

    Tcl_DStringEndSublist(&results);
  }

  mysql_free_result(result);

  Tcl_DStringResult(interp, &results);
  return TCL_OK;
}

int
MYSQL_Get_Fields(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  Tcl_DString results;
  MYSQL_RES *result;
  MYSQL_FIELD *field;
  char buf[40];

  if (argc != 4) {
    Tcl_AppendResult(interp, "usage: ", argv[0], " get fields <table>.", 0);
    return TCL_ERROR;
  }

  result = mysql_list_fields(c->socket, argv[3], NULL);
  
  if (result == NULL) {
    if (mysql_error(c->socket)[0] != '\0') {
      Tcl_AppendResult(interp, argv[0], " ", argv[1], " ", argv[2],
		       ": ", mysql_error(c->socket), 0);
      return TCL_ERROR;
    }
    return TCL_ERROR;
  }

  Tcl_DStringInit(&results);
  
  while ((field = mysql_fetch_field(result)) != NULL) {
    Tcl_DStringStartSublist(&results);
    
    Tcl_DStringAppendElement(&results, field->name);

    switch (field->type) {
    case INT_TYPE:
      Tcl_DStringAppendElement(&results, "INT");
      break;
    case REAL_TYPE:
      Tcl_DStringAppendElement(&results, "REAL");
      break;
    case CHAR_TYPE:
      sprintf(buf, "CHAR(%d)", field->length);
      Tcl_DStringAppendElement(&results, buf);
      break;
    }

    sprintf(buf, "%d", field->flags);
    Tcl_DStringAppendElement(&results, buf);
    
    Tcl_DStringEndSublist(&results);
  }

  mysql_free_result(result);

  Tcl_DStringResult(interp, &results);
  return TCL_OK;
}

int
MYSQL_Get_Rest(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  Tcl_DString results;
  MYSQL_ROW row;
  int num_cols;
  int pos;

  if (c->query_result == NULL) {
    return TCL_OK;
  }

  Tcl_DStringInit(&results);

  num_cols = mysql_num_fields(c->query_result);
  while (row = mysql_fetch_row(c->query_result)) {
    Tcl_DStringStartSublist(&results);

    for (pos = 0; pos < num_cols; pos++)
      Tcl_DStringAppendElement(&results, row[pos]);

    Tcl_DStringEndSublist(&results);
  }

  Tcl_DStringResult(interp, &results);
  return TCL_OK;
}

int
MYSQL_Get_Next(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  Tcl_DString results;
  MYSQL_ROW row;
  int num_cols;
  int pos;

  if (c->query_result == NULL) {
    return TCL_OK;
  }

  num_cols = mysql_num_fields(c->query_result);
  row = mysql_fetch_row(c->query_result);
  
  if (row == NULL) {
    return TCL_OK;
  }

  Tcl_DStringInit(&results);

  for (pos = 0; pos < num_cols; pos++)
    Tcl_DStringAppendElement(&results, row[pos]);

  Tcl_DStringResult(interp, &results);
  return TCL_OK;
}

int
MYSQL_Get_Rows(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  char buf[80];

  if (argc != 3) {
    Tcl_AppendResult(interp, "usage: ", argv[0], " get rows.", 0);
    return TCL_ERROR;
  }

  if (c->query_result == NULL) {
    Tcl_AppendResult(interp, "0", 0);
    return TCL_OK;
  }

  sprintf(buf, "%d", mysql_num_rows(c->query_result));
  Tcl_AppendResult(interp, buf, 0);
  return TCL_OK;
}

int
MYSQL_Get_Cols(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  char buf[80];

  if (argc != 3) {
    Tcl_AppendResult(interp, "usage: ", argv[0], " get cols.", 0);
    return TCL_ERROR;
  }

  if (c->query_result == NULL) {
    Tcl_AppendResult(interp, "0", 0);
    return TCL_OK;
  }

  sprintf(buf, "%d", mysql_num_fields(c->query_result));
  Tcl_AppendResult(interp, buf, 0);
  return TCL_OK;
}

int
MYSQL_Cmd_Query(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  int res;

  if (argc != 3) {
    Tcl_AppendResult(interp, "usage: ", argv[0], " query <query>.", 0);
    return TCL_ERROR;
  }

  if ((res = mysql_query(c->socket, argv[2])) != 0) 
    {
      Tcl_AppendResult(interp, argv[0], " ", argv[1], ": ", mysql_error(c->socket), 0);
	return TCL_ERROR;
    }
  
  free_query_result(c->query_result);
  c->query_result = mysql_store_result(c->socket);

  return TCL_OK;
}

int
MYSQL_Cmd_Register(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv)
{
  int res;

  Tcl_AppendResult(interp, "Unsupported command: ", argv[0], 0);
  return TCL_ERROR;
}
