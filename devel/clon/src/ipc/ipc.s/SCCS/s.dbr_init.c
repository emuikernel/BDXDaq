h41803
s 00001/00001/00180
d D 1.14 02/09/23 16:52:17 wolin 15 14
c typo
e
s 00007/00000/00174
d D 1.13 02/08/16 10:19:59 wolin 14 13
c license.cm
e
s 00000/00004/00174
d D 1.12 02/08/13 13:35:13 wolin 13 12
c Removed TutCommandParseStdLicense
c 
e
s 00002/00002/00176
d D 1.11 00/10/26 13:10:23 wolin 12 11
c const
e
s 00001/00002/00177
d D 1.10 00/10/26 13:09:34 wolin 11 10
c const 
e
s 00012/00000/00167
d D 1.9 99/05/18 13:12:58 wolin 10 9
c Added dbr_output_dummy
c 
e
s 00018/00000/00149
d D 1.8 98/07/02 15:19:26 wolin 9 8
c Now destroys gmd failure callbacks
c 
e
s 00001/00001/00148
d D 1.7 98/01/05 13:33:04 wolin 8 7
c Need smartsockets include since changed clas_ipc.h
c 
e
s 00001/00001/00148
d D 1.6 97/06/06 12:56:16 wolin 7 6
c No _time datagroup
e
s 00003/00002/00146
d D 1.5 97/04/01 11:50:13 wolin 6 5
c Typo
e
s 00004/00000/00144
d D 1.4 97/04/01 11:44:25 wolin 5 4
c Added standard dg set receive
e
s 00004/00000/00140
d D 1.3 97/04/01 11:42:13 wolin 4 3
c Added extra flush for pending server messages
e
s 00005/00002/00135
d D 1.2 96/12/13 09:07:13 wolin 3 1
c Typo...ipc_atexit changed to dbr_atexit
e
s 00000/00000/00000
d R 1.2 96/12/11 09:46:24 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/s/dbr_init.c
c Name history : 1 0 s/dbr_init.c
e
s 00137/00000/00000
d D 1.1 96/12/11 09:46:23 wolin 1 0
c Stripped down version of ipc_init just for dbrouter access
e
u
U
f e 0
t
T
I 1
/*
*  dbr_init.c
*
*  stripped down version of ipc_init, does minimal initialization for 
*   communicating with the database router
*    
*
*  EJW, 11-dec-96
*
*
*  usage from c
*  ------------
*
*    int dbr_init(
*        char *unique_datagroup,         !unique name for this process    
*        char *application               !application name
*        char *ident_string              !monitoring identification string
*        )
*        returns 0 if successful, <0 for fatal error, >0 for non-fatal warning
*
*
*/

#define _POSIX_SOURCE 1
#define __EXTENSIONS__

I 8
#include <rtworks/ipc.h>
E 8
#include <clas_ipc.h>
#include <clas_ipc_mt.h>
#include <clas_ipc_prototypes.h>
I 6
D 8
#include <rtworks/ipc.h>
E 8
E 6


I 3
/* prototypes */
void dbr_atexit(void);
E 3

I 3

E 3
/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/


/*
*  Initialize Smartsockets server connection after setting CLAS standard options
*/
D 11
int dbr_init(char *uniq_dgrp, char *app, char *ident_string)
{
E 11
I 11
int dbr_init(const char *uniq_dgrp, const char *app, const char *ident_string) {
E 11

  char *application;
I 9
  T_CB cb;
E 9
  T_IPC_MT mt;
  T_OPTION opt;
  int err=0;
  int i;
I 14
  char fname[256];
E 14


  /* set application...default is CLASTEST if none specified */
  if((app!=NULL) && (strlen(app)>0)){
    application=strdup(app);
  } else {
    application=strdup("CLASTEST");
  }
    

I 14
  /* read Smartsockets license file */
  strcpy(fname,getenv("RTHOME"));
D 15
  strcpy(fname,"/standard/license.cm");
E 15
I 15
  strcat(fname,"/standard/license.cm");
E 15
  if(!TutCommandParseFile(fname)){if((err=IPC_ERR_LICENSE_FILE)<0)return(err);}


E 14
D 13
  /* read Smartsockets license file */
  if(!TutCommandParseStdLicense()){if((err=IPC_ERR_LICENSE_FILE)<0)return(err);}


E 13
  /* set application */
  opt=TutOptionLookup("Application");
  if(!TutOptionSetEnum(opt,application)){if((err=IPC_ERR_APPLICATION)<0)return(err);}


  /* set unique datagroup if supplied */
  if((uniq_dgrp!=NULL) && (strlen(uniq_dgrp)>0)){
    opt=TutOptionLookup("Unique_Datagroup");
D 12
    if(!TutOptionSetEnum(opt,uniq_dgrp)){return(IPC_ERR_SET_UNIQUE_DATAGROUP);}
E 12
I 12
    if(!TutOptionSetEnum(opt,(T_STR)uniq_dgrp)){return(IPC_ERR_SET_UNIQUE_DATAGROUP);}
E 12
  }


  /* create clas standard message types */
  for(i=0; i<sizeof(ipc_msg_type)/sizeof(struct clas_standard_msg_type); i++){
    mt=TipcMtCreate(ipc_msg_type[i].mtname,ipc_msg_type[i].mtnum,"verbose");
    if((mt==NULL)&&(TutErrNumGet()!=T_ERR_ALREADY_EXISTS))if((err=IPC_ERR_MT_CREATE)<0)return(err);
  }


  /* set monitoring identification string if supplied */
  if((ident_string!=NULL) && (strlen(ident_string)>0)){
D 12
    if(!TipcMonSetIdentStr(ident_string)){if((err=IPC_ERR_IDENT_STRING)<0)return(err);}
E 12
I 12
    if(!TipcMonSetIdentStr((T_STR)ident_string)){if((err=IPC_ERR_IDENT_STRING)<0)return(err);}
E 12
  }
  
I 9

  /* disable GMD delivery timeout */
  opt=TutOptionLookup("Server_Delivery_Timeout");
  if(!TutOptionSetNum(opt,0.0)){if((err=IPC_ERR_SERVER_DELIVERY_TIMEOUT)<0)return(err);}


E 9
  /* connect to server...fatal error if can't connect */
  if(!TipcSrvCreate(T_IPC_SRV_CONN_FULL)){if((err=IPC_ERR_SERVER_CREATE)<0)return(err);}


I 9
  /* destroy default connection callback for GMD failure */
  mt=TipcMtLookupByNum(T_MT_GMD_FAILURE);
  cb=TipcSrvProcessCbLookup(mt,TipcCbConnProcessGmdFailure, NULL);
  if(!TutCbDestroy(cb)){if((err=IPC_ERR_CONN_CALLBACK_DESTROY)<0)return(err);}


  /* destroy default server callback for GMD failure */
  cb=TipcSrvProcessCbLookup(mt,TipcCbSrvProcessGmdFailure, NULL);
  if(!TutCbDestroy(cb)){if((err=IPC_ERR_SERVER_GMD_FAILURE)<0)return(err);}


E 9
  /* set atexit handler */
D 3
  if(atexit(ipc_atexit)!=0){if((err=IPC_ERR_ATEXIT)<0)return(err);}
E 3
I 3
  if(atexit(dbr_atexit)!=0){if((err=IPC_ERR_ATEXIT)<0)return(err);}
E 3


I 5
  /* receive standard datagroups */
D 6
  TipcSrvSTdDgSetRecv(TRUE, TRUE);
E 6
I 6
D 7
  TipcSrvStdDgSetRecv(TRUE,TRUE);
E 7
I 7
  TipcSrvStdDgSetRecv(TRUE,FALSE);
E 7
E 6


E 5
D 6
  /* receive "application" datagroup */
E 6
I 6
  /* receive application datagroup */
E 6
  TipcSrvDgSetRecv(application, TRUE);


I 4
  /* flush pending messages */
  TipcSrvFlush();


E 4
  /* done */
  return(err);

}


/*-----------------------------------------------------------------------*/


/* destroys IPC server connection */

int dbr_close(void)
{
  TipcSrvDestroy(T_IPC_SRV_CONN_NONE);
  return(0);
}


/*-----------------------------------------------------------------------*/


int dbr_check(float timeout)
{
  return(TipcSrvMainLoop(timeout));
}


/*-----------------------------------------------------------------------*/


/* destroys IPC server connection...called at exit...just in case */

D 3
void ipc_atexit(void)
E 3
I 3
void dbr_atexit(void)
E 3
{
  TutOut("\nDBR atexit handler called...closing connection\n\n");
  TipcSrvDestroy(T_IPC_SRV_CONN_NONE);
}

/*-----------------------------------------------------------------------*/

I 10

void dbr_output_dummy(T_STR str, va_list arg) {

  return;

}


/*-----------------------------------------------------------------------*/



E 10
E 1
