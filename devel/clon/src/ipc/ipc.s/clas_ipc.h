/*
*  clas_ipc.h
*
*  defines ipc error codes, etc.
*
*  ejw, 18-jun-96
*
*/

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
  IPC_ERR_MT_CREATE                     = -1019,
  IPC_ERR_NULL_APPLICATION           	=  1020,
  IPC_ERR_ATEXIT                     	=  1021,
  IPC_ERR_NULL_STATUS_POLL_GROUP     	=  1022,
  IPC_ERR_NULL_DISCONNECT_MODE          =  1023,
  IPC_ERR_DEFAULT_CALLBACK_CREATE       =  1024,
  IPC_ERR_LOG_MT                        =  1025

} IPC_ERROR_CODE;




