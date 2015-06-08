h33852
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 include/s/clas_libipc.h
e
s 00078/00000/00000
d D 1.1 00/07/26 15:37:40 wolin 1 0
c date and time created 00/07/26 15:37:40 by wolin
e
u
U
f e 0
t
T
I 1
#ifndef LIBIPC_H
#define LIBIPC_H

#include <rtworks/ipc.h>

//  Contains:
//  ipc_PackInit
//  ipc_PackForget
//  ipc_PackError
//  ipc_VaPack
//  dbrt_VaSendRow
//  dbrt_VaSendSql
//  dbrt_VaSendWRSql
//
//  This code currently uses a smartsockets define NUMERIC data
//  messsage type to transfer data in.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// JAM - initial version.
// JAM - add in dbrt_VaPackCmd
// JAM - change to Elliot's message types. I ONLY lookup
//       by name. The ipc_init call created the needed structures
//       in the server.
//

extern "C" {

T_IPC_MSG dbrt_VaSendWRSql (int, ...);
int dbrt_VaSendSql (int, ...);
/* Pack a variable number of pairs,
 * where each pair is made up of:
 *    1. an integer numberRows/Number2Return.
 *    2. the sql command to execute.
 * inputs:  a double of int, string
 * return the number of message fragments "packed"
 * if any error return -1 and set global error info.
*/

int dbrt_VaSendRow (char *, ...);
/* Pack a table row - var arguments
 * takes a triple of column, dataType, data
 * returns: 0  = failed - use ipc_packerror, >0 = success.
*/

T_IPC_MSG ipc_PackInit ();

void ipc_PackForget (T_IPC_MSG);

void ipc_PackError (void);
/* provide verbose feedback about a packing error.
 * makes use of the static error struct
 * carried in this file.
*/

int ipc_VaPack (T_IPC_MSG);
/* Pack a variable number of items into a smartsockets msg buff.
 * take a double of dataType, data
 * return the number of message fragments "packed"
*/


void dbrt_answer_error ();
void dbrt_SetDest (char *);
void dbrt_SetAppl (char *);

}

struct ipc_ERR {
  char  *ms, *ts;
  int   number_packed;
  int   num_errors ;
  int   error_index [16] ;
};

extern char theAppl[];
extern char theDest[];

#endif

E 1
