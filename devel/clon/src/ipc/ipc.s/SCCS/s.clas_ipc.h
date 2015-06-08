h36598
s 00001/00005/00050
d D 1.6 98/01/05 13:28:56 wolin 7 6
c Removed smartsockets include
c 
e
s 00000/00000/00055
d D 1.5 96/09/11 13:11:12 wolin 6 5
c Bug in codemgr
e
s 00000/00000/00055
d D 1.4 96/09/11 13:09:24 wolin 5 4
c Bug in codemgr
e
s 00000/00000/00055
d D 1.3 96/09/11 13:05:16 wolin 4 3
c Bug in codemgr
e
s 00004/00005/00051
d D 1.2 96/09/05 11:44:43 wolin 3 1
c New scheme for init of standard message types
e
s 00000/00000/00000
d R 1.2 96/08/08 16:27:30 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 include/s/clas_ipc.h
c Name history : 1 0 s/clas_ipc.h
e
s 00056/00000/00000
d D 1.1 96/08/08 16:27:29 wolin 1 0
c CLAS IPC error codes, smartsockets headers, etc.
e
u
U
f e 0
t
T
I 1
/*
*  clas_ipc.h
*
D 7
*  defines mt numbers, ipc error codes, etc.
E 7
I 7
*  defines ipc error codes, etc.
E 7
*
*  ejw, 18-jun-96
*
*/

D 7
/* smartsockets parameter definitions */
#include <rtworks/ipc.h>


E 7
/* 
* define ipc error codes for "perror" functions 
*
* NOTE: codes <0 indicate fatal errors
*             >0 are warnings and can possibly be ignored
*
*/

typedef enum{

  IPC_ERR_UNIQUE_DATAGROUP           	= -1000,
  IPC_ERR_LICENSE_FILE               	=  1001,
  IPC_ERR_SERVER_NAMES               	=  1002,
  IPC_ERR_APPLICATION                	=  1003,
  IPC_ERR_SET_UNIQUE_DATAGROUP       	= -1004,
  IPC_ERR_SERVER_DISCONNECT_MODE     	=  1005,   
  IPC_ERR_SERVER_DELIVERY_TIMEOUT    	=  1006,
  IPC_ERR_SIGTERM                    	=  1007,
  IPC_ERR_SIGQUIT                    	=  1008,
  IPC_ERR_SIGINT                     	=  1009,
  IPC_ERR_SIGHUP                     	=  1010,
  IPC_ERR_IDENT_STRING               	=  1011, 
  IPC_ERR_ALARM_HANDLER              	=  1012,
  IPC_ERR_SERVER_CREATE              	= -1013,
  IPC_ERR_CONN_CALLBACK_DESTROY      	=  1014,
  IPC_ERR_SERVER_GMD_FAILURE         	=  1015,
  IPC_ERR_CONTROL_CALLBACK_DESTROY   	=  1016,
  IPC_ERR_CONTROL_CALLBACK_CREATE    	=  1017,
  IPC_ERR_STANDARD_DATAGROUPS        	=  1018,
D 3
  IPC_ERR_STATUS_POLL_RESULT_MT_CREATE  =  1019,
E 3
I 3
  IPC_ERR_MT_CREATE                     = -1019,
E 3
  IPC_ERR_NULL_APPLICATION           	=  1020,
  IPC_ERR_ATEXIT                     	=  1021,
  IPC_ERR_NULL_STATUS_POLL_GROUP     	=  1022,
D 3
  IPC_ERR_NULL_STATUS_POLL_RESULT_MT    =  1023,
  IPC_ERR_NULL_DISCONNECT_MODE          =  1024,
  IPC_ERR_DEFAULT_CALLBACK_CREATE       =  1025,
  IPC_ERR_LOG_MT                        =  1026
E 3
I 3
  IPC_ERR_NULL_DISCONNECT_MODE          =  1023,
  IPC_ERR_DEFAULT_CALLBACK_CREATE       =  1024,
  IPC_ERR_LOG_MT                        =  1025
E 3

} IPC_ERROR_CODE;




E 1
