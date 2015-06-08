h29635
s 00006/00008/00104
d D 1.8 96/10/28 12:54:15 wolin 9 8
c No more ipc_tcl_interp...using T_CB_ARG
e
s 00024/00011/00088
d D 1.7 96/10/23 14:37:36 wolin 8 7
c Now handles multiple messages in one tcl_request
e
s 00004/00006/00095
d D 1.6 96/10/21 13:59:22 wolin 7 6
c Fixed passing fortran interp
e
s 00011/00002/00090
d D 1.5 96/10/16 09:38:39 wolin 6 5
c Added tclinterp_init
e
s 00006/00009/00086
d D 1.4 96/10/15 11:55:02 wolin 5 4
c Removed startup script from tclipc_init call
e
s 00001/00001/00094
d D 1.3 96/10/08 11:02:04 wolin 4 3
c Tcl stuff now working
e
s 00001/00001/00094
d D 1.2 96/10/07 17:27:09 wolin 3 1
c Typo
e
s 00000/00000/00000
d R 1.2 96/10/07 17:17:35 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/s/tclipc_init.c
c Name history : 1 0 s/tclipc_init.c
e
s 00095/00000/00000
d D 1.1 96/10/07 17:17:34 wolin 1 0
c Initializes tcl interp, creates callback for tcl_request message, calls tcl_eval, sends back tcl_reply message
e
u
U
f e 0
t
T
I 1
/*
D 5
 *  tclipc_init(tcl_startup_script)
E 5
I 5
 *  tclipc_init
E 5
 *
 *  initialize ipc support of remote tcl command execution
I 5
D 6
 *     creates Tcl interpreter
E 6
 *     creates callback for tcl_request messages
E 5
 *
 *  note:  (tcl_request,tcl_reply) can be used with Smartsockets RPC
 *
D 5
 *  ejw, 7-oct-96
E 5
I 5
D 7
 *  ejw, 15-oct-96
E 7
I 7
 *  ejw, 21-oct-96
E 7
E 5
 */

 
#define _POSIX_SOURCE 1
#define __EXTENSIONS__

#include <rtworks/ipc.h>
#include <tcl.h>

D 9
/* tcl interpreter is external variable */
Tcl_Interp *ipc_tcl_interp;

E 9
void tcl_callback(
     T_IPC_CONN conn,
     T_IPC_CONN_PROCESS_CB_DATA data,
     T_CB_ARG arg);



/*----------------------------------------------------------------*/


D 5
int tclipc_init(char *tcl_startup_script){
E 5
I 5
D 6
int tclipc_init(void){
E 6
I 6
D 7
int tclinterp_init(void){
E 7
I 7
Tcl_Interp *tclinterp_init(void){
E 7
E 6
E 5

D 5
  int code=TCL_OK;

E 5
D 7
  /* create interpreter */
  ipc_tcl_interp=Tcl_CreateInterp();
E 7
I 7
  /* create interpreter and store in global variable */
D 9
  return(ipc_tcl_interp=Tcl_CreateInterp());
E 9
I 9
  return(Tcl_CreateInterp());
E 9
E 7

I 6
D 7
  return(TCL_OK);

E 7
}


/*----------------------------------------------------------------*/


D 9
int tclipc_init(void){
E 9
I 9
int tclipc_init(Tcl_Interp *interp){
E 9

E 6
  /* create callback for tcl request message */
D 9
  TipcSrvProcessCbCreate(TipcMtLookup("tcl_request"),tcl_callback,NULL);
E 9
I 9
  TipcSrvProcessCbCreate(TipcMtLookup("tcl_request"),tcl_callback,(T_CB_ARG)interp);
E 9

D 5
  /* process tcl startup file */
  if(tcl_startup_script!=NULL)code=Tcl_EvalFile(ipc_tcl_interp,tcl_startup_script);
E 5
I 5
  return(TCL_OK);
E 5

D 5
  return(code);

E 5
}


/*----------------------------------------------------------------*/


void tcl_callback(
     T_IPC_CONN conn,
     T_IPC_CONN_PROCESS_CB_DATA data,
     T_CB_ARG arg){


  T_STR tcl_command;
  T_IPC_MSG reply;
D 8
  int code;
E 8
I 8
  int code,i;
E 8
  T_STR sender;
  char *uniq_dgrp;
I 8
  T_INT4 ncmd;
I 9
  Tcl_Interp *interp = (Tcl_Interp *)arg;
E 9
E 8


D 8
  /* extract tcl command from message */
  TipcMsgNextStr(data->msg,&tcl_command);
E 8
I 8
  /* get number of commands in the message */
  TipcMsgGetNumFields(data->msg,&ncmd);
E 8

D 8
  /* execute command */
  code=Tcl_Eval(ipc_tcl_interp,tcl_command);

E 8
  /* create reply message */
  reply=TipcMsgCreate(TipcMtLookup("tcl_reply"));
D 8

E 8
I 8
    
E 8
  /* set sender */
D 4
  ipc_get_unique_datagroup(uniq_dgrp);
E 4
I 4
  ipc_get_unique_datagroup(&uniq_dgrp);
E 4
  TipcMsgSetSender(reply,uniq_dgrp);
  free(uniq_dgrp);
D 8

E 8
I 8
  
E 8
  /* set dest */
  TipcMsgGetSender(data->msg,&sender);
  TipcMsgSetDest(reply,sender);
I 8
    
  /* reset to first field */
  TipcMsgSetCurrent(data->msg,0);
E 8

D 8
  /* fill in data fields */
  TipcMsgAppendInt4(reply,code);
  TipcMsgAppendStr(reply,ipc_tcl_interp->result);
E 8
I 8
  /* extract and process each command */
  /* note...empty message yields empty reply */
  for(i=1; i<=ncmd; i++){
E 8

I 8
    /* extract tcl command from message */
    TipcMsgNextStr(data->msg,&tcl_command);
    
    /* execute command */
D 9
    code=Tcl_Eval(ipc_tcl_interp,tcl_command);
E 9
I 9
    code=Tcl_Eval(interp,tcl_command);
E 9
    
    /* append data fields */
    TipcMsgAppendInt4(reply,code);
D 9
    TipcMsgAppendStr(reply,ipc_tcl_interp->result);
E 9
I 9
    TipcMsgAppendStr(reply,interp->result);
E 9
  }

E 8
  /* send, flush, and destroy message */
D 3
  TipcSrvSend(reply);
E 3
I 3
  TipcSrvMsgSend(reply,TRUE);
E 3
  TipcSrvFlush();
  TipcMsgDestroy(reply);

I 8
  return;
E 8
}


/*----------------------------------------------------------------*/
E 1
