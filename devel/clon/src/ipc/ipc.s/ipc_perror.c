/* 
*  ipc_perror(error)
*
*  prints informative message given IPC error code
*
*/

/* Smartsockets include file */
#include <rtworks/ipc.h>

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

  else if(error==IPC_ERR_MT_CREATE){
    TutWarning("unable to create result message type\n");
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

  else if(error==IPC_ERR_NULL_DISCONNECT_MODE){
    TutWarning("disconnect mode is null\n");
  }

  else if(error==IPC_ERR_DEFAULT_CALLBACK_CREATE){
    TutWarning("unable to create default callback\n");
  }

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
