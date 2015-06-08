h37091
s 00003/00000/00132
d D 1.3 98/01/05 13:33:15 wolin 4 3
c Need smartsockets include since changed clas_ipc.h
c 
e
s 00006/00006/00126
d D 1.2 96/09/05 12:40:06 wolin 3 1
c New scheme for init of standard message types
e
s 00000/00000/00000
d R 1.2 96/07/23 20:31:14 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/s/ipc_perror.c
c Name history : 1 0 s/ipc_perror.c
e
s 00132/00000/00000
d D 1.1 96/07/23 20:31:13 muguira 1 0
c 
e
u
U
f e 0
t
T
I 1
/* 
*  ipc_perror(error)
*
*  prints informative message given IPC error code
*
*/

I 4
/* Smartsockets include file */
#include <rtworks/ipc.h>

E 4
#include <clas_ipc.h>
#include <clas_ipc_prototypes.h>

void ipc_perror(int error)
{

  if(error==IPC_ERR_UNIQUE_DATAGROUP){
    TutWarning("unique datagroup is empty or NULL\n");
  }
  
  else if(error==IPC_ERR_LICENSE_FILE){
    TutWarning("unable to read license file\n");
  }
  
  else if(error==IPC_ERR_SERVER_NAMES){
    TutWarning("server name is NULL or empty\n");
  }
  
  else if(error==IPC_ERR_APPLICATION){
    TutWarning("unable to set application\n");
  }

  else if(error==IPC_ERR_SET_UNIQUE_DATAGROUP){
    TutWarning("unable to set unique datagroup\n");
  }

  else if(error==IPC_ERR_SERVER_DISCONNECT_MODE){
    TutWarning("unable to set server disconnect mode\n");
  }

  else if(error==IPC_ERR_SERVER_DELIVERY_TIMEOUT){
    TutWarning("unable to set server delivery timeout\n");
  }

  else if(error==IPC_ERR_SIGTERM){
    TutWarning("unable to set TERM handler\n");
  }

  else if(error==IPC_ERR_SIGQUIT){
    TutWarning("unable to set QUIT handler\n");
  }

  else if(error==IPC_ERR_SIGINT){
    TutWarning("unable to set INT handler\n");
  }

  else if(error==IPC_ERR_SIGHUP){
    TutWarning("unable to set HUP handler\n");
  }

  else if(error==IPC_ERR_IDENT_STRING){
    TutWarning("unable to set ident string\n");
  }

  else if(error==IPC_ERR_ALARM_HANDLER){
    TutWarning("unable to set alarm handler\n");
  }

  else if(error==IPC_ERR_SERVER_CREATE){
    TutWarning("unable to connect to server\n");
  }

  else if(error==IPC_ERR_CONN_CALLBACK_DESTROY){
    TutWarning("unable to destroy conn GMD failure callback\n");
  }

  else if(error==IPC_ERR_SERVER_GMD_FAILURE){
    TutWarning("unable to destroy srv GMD failure callback\n");;
  }

  else if(error==IPC_ERR_CONTROL_CALLBACK_DESTROY){
    TutWarning("unable to destroy default control callback\n");
  }

  else if(error==IPC_ERR_CONTROL_CALLBACK_CREATE){
    TutWarning("unable to create clas control callback\n");
  }

  else if(error==IPC_ERR_STANDARD_DATAGROUPS){
    TutWarning("unable to receive standard datagroups\n");
  }

D 3
  else if(error==IPC_ERR_STATUS_POLL_RESULT_MT_CREATE){
    TutWarning("unable to create status poll result message type\n");
E 3
I 3
  else if(error==IPC_ERR_MT_CREATE){
    TutWarning("unable to create result message type\n");
E 3
  }

  else if(error==IPC_ERR_NULL_APPLICATION){
    TutWarning("application is null\n");
  }

  else if(error==IPC_ERR_ATEXIT){
    TutWarning("unable to set atexit handler\n");
  }

  else if(error==IPC_ERR_NULL_STATUS_POLL_GROUP){
    TutWarning("status poll group is null\n");
  }

D 3
  else if(error==IPC_ERR_NULL_STATUS_POLL_RESULT_MT){
    TutWarning("status poll result msg type is null\n");
  }

E 3
  else if(error==IPC_ERR_NULL_DISCONNECT_MODE){
    TutWarning("disconnect mode is null\n");
  }

I 3
  else if(error==IPC_ERR_DEFAULT_CALLBACK_CREATE){
    TutWarning("unable to create default callback\n");
  }

E 3
  else {
    TutWarning("unknown IPC error code is %d\n",error);
  }

}


/*------------------------------------------------------------*/


/* fortran binding */

void ipc_perror_(int *error)
{
  ipc_perror(*error);
}


/*------------------------------------------------------------*/
E 1
