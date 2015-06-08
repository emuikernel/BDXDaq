/*
 *  tclipc_init
 *
 *  initialize ipc support of remote tcl command execution
 *     creates callback for tcl_request messages
 *
 *  note:  (tcl_request,tcl_reply) can be used with Smartsockets RPC
 *
 *  ejw, 21-oct-96
 */

 
#define _POSIX_SOURCE 1
#define __EXTENSIONS__

#include <rtworks/ipc.h>
#include <tcl.h>

void tcl_callback(
     T_IPC_CONN conn,
     T_IPC_CONN_PROCESS_CB_DATA data,
     T_CB_ARG arg);



/*----------------------------------------------------------------*/


Tcl_Interp *tclinterp_init(void){

  /* create interpreter and store in global variable */
  return(Tcl_CreateInterp());

}


/*----------------------------------------------------------------*/


int tclipc_init(Tcl_Interp *interp){

  /* create callback for tcl request message */
  TipcSrvProcessCbCreate(TipcMtLookup("tcl_request"),tcl_callback,(T_CB_ARG)interp);

  return(TCL_OK);

}


/*----------------------------------------------------------------*/


void tcl_callback(
     T_IPC_CONN conn,
     T_IPC_CONN_PROCESS_CB_DATA data,
     T_CB_ARG arg){


  T_STR tcl_command;
  T_IPC_MSG reply;
  int code,i;
  T_STR sender;
  char *uniq_dgrp;
  T_INT4 ncmd;
  Tcl_Interp *interp = (Tcl_Interp *)arg;


  /* get number of commands in the message */
  TipcMsgGetNumFields(data->msg,&ncmd);

  /* create reply message */
  reply=TipcMsgCreate(TipcMtLookup("tcl_reply"));
    
  /* set sender */
  ipc_get_unique_datagroup(&uniq_dgrp);
  TipcMsgSetSender(reply,uniq_dgrp);
  free(uniq_dgrp);
  
  /* set dest */
  TipcMsgGetSender(data->msg,&sender);
  TipcMsgSetDest(reply,sender);
    
  /* reset to first field */
  TipcMsgSetCurrent(data->msg,0);

  /* extract and process each command */
  /* note...empty message yields empty reply */
  for(i=1; i<=ncmd; i++){

    /* extract tcl command from message */
    TipcMsgNextStr(data->msg,&tcl_command);
    
    /* execute command */
    code=Tcl_Eval(interp,tcl_command);
    
    /* append data fields */
    TipcMsgAppendInt4(reply,code);
    TipcMsgAppendStr(reply,interp->result);
  }

  /* send, flush, and destroy message */
  TipcSrvMsgSend(reply,TRUE);
  TipcSrvFlush();
  TipcMsgDestroy(reply);

  return;
}


/*----------------------------------------------------------------*/
