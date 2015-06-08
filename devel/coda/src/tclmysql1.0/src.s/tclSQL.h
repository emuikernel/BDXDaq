/*
 *	tclSQL.h	- Definitions
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
 * $Id: tclSQL.h,v 1.1.1.1 1996/09/05 14:29:47 heyes Exp $
 *
 * $Log: tclSQL.h,v $
 * Revision 1.1.1.1  1996/09/05 14:29:47  heyes
 * Initial Import
 *
 * Revision 1.5  1994/08/25  00:28:10  pepers
 * Changed for mSQL version 0.2
 *
 * Revision 1.4  1994/07/28  22:44:45  pepers
 * Changed around how commands work
 *
 * Revision 1.3  1994/07/28  02:04:58  pepers
 * Changed close command to disconnect
 *
 * Revision 1.2  1994/07/27  22:43:24  pepers
 * Fixed log files
 *
 * Revision 1.1  1994/07/27  22:34:58  pepers
 * First version of mSQL to Tcl/Tk interface
 *
*/


/* from msql.h: not sure if it corresponds to mysql or not ... */
#define INT_TYPE	1
#define CHAR_TYPE	2
#define REAL_TYPE	3

typedef struct mysqlConnS mysqlConn;

typedef struct mysqlConnS {
  mysqlConn *next;
  char name[80];
  char *host;
  MYSQL *socket;
  char database[100];
  MYSQL_RES *query_result;
} mysqlConns;


/* function prototypes */

int MYSQL_Init(Tcl_Interp *interp);
int MYSQL_Cmd(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Cmd_Connect(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Cmd_ConnectPort(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Cmd_Connections(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Conn_Cmd(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Cmd_Disconnect(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Cmd_Set(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Set_DB(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Set_Row(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Cmd_Get(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Get_DBs(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Get_Tables(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Get_Fields(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Get_Rest(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Get_Next(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Get_Rows(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Get_Cols(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Cmd_Query(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);
int MYSQL_Cmd_Register(mysqlConn *c, Tcl_Interp *interp, int argc, char **argv);

