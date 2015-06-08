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

#include <rtworks/ipc.h>
#include <clas_ipc.h>
#include <clas_ipc_mt.h>
#include <clas_ipc_prototypes.h>


/* prototypes */
void dbr_atexit(void);


/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/


/*
*  Initialize Smartsockets server connection after setting CLAS standard options
*/
int
dbr_init(const char *uniq_dgrp, const char *app, const char *ident_string)
{
  char *application;
  T_CB cb;
  T_IPC_MT mt;
  T_OPTION opt;
  int err=0;
  int i;
  char fname[256];
  IPC_MSG_TYPE_LOCAL(1);


  /* set application...default is CLASTEST if none specified */
  if((app!=NULL) && (strlen(app)>0)){
    application=strdup(app);
  } else {
    application=strdup("CLASTEST");
  }
    

  /* read Smartsockets license file */
  strcpy(fname,getenv("RTHOME"));
  strcat(fname,"/standard/license.cm");
  if(!TutCommandParseFile(fname)){if((err=IPC_ERR_LICENSE_FILE)<0)return(err);}


  /* set application */
  opt=TutOptionLookup("Application");
  if(!TutOptionSetEnum(opt,application)){if((err=IPC_ERR_APPLICATION)<0)return(err);}


  /* set unique datagroup if supplied */
  if((uniq_dgrp!=NULL) && (strlen(uniq_dgrp)>0)){
    opt=TutOptionLookup("Unique_Datagroup");
    if(!TutOptionSetEnum(opt,(T_STR)uniq_dgrp)){return(IPC_ERR_SET_UNIQUE_DATAGROUP);}
  }


  /* create clas standard message types */
  for(i=0; i<sizeof(ipc_msg_type1)/sizeof(struct clas_standard_msg_type); i++){
    mt=TipcMtCreate(ipc_msg_type1[i].mtname,ipc_msg_type1[i].mtnum,"verbose");
    if((mt==NULL)&&(TutErrNumGet()!=T_ERR_ALREADY_EXISTS))if((err=IPC_ERR_MT_CREATE)<0)return(err);
  }


  /* set monitoring identification string if supplied */
  if((ident_string!=NULL) && (strlen(ident_string)>0)){
    if(!TipcMonSetIdentStr((T_STR)ident_string)){if((err=IPC_ERR_IDENT_STRING)<0)return(err);}
  }
  

  /* disable GMD delivery timeout */
  opt=TutOptionLookup("Server_Delivery_Timeout");
  if(!TutOptionSetNum(opt,0.0)){if((err=IPC_ERR_SERVER_DELIVERY_TIMEOUT)<0)return(err);}


  /* connect to server...fatal error if can't connect */
  if(!TipcSrvCreate(T_IPC_SRV_CONN_FULL)){if((err=IPC_ERR_SERVER_CREATE)<0)return(err);}


  /* destroy default connection callback for GMD failure */
  mt=TipcMtLookupByNum(T_MT_GMD_FAILURE);
  cb=TipcSrvProcessCbLookup(mt,TipcCbConnProcessGmdFailure, NULL);
  if(!TutCbDestroy(cb)){if((err=IPC_ERR_CONN_CALLBACK_DESTROY)<0)return(err);}


  /* destroy default server callback for GMD failure */
  cb=TipcSrvProcessCbLookup(mt,TipcCbSrvProcessGmdFailure, NULL);
  if(!TutCbDestroy(cb)){if((err=IPC_ERR_SERVER_GMD_FAILURE)<0)return(err);}


  /* set atexit handler */
  if(atexit(dbr_atexit)!=0){if((err=IPC_ERR_ATEXIT)<0)return(err);}


  /* receive standard datagroups */
  TipcSrvStdDgSetRecv(TRUE,FALSE);


  /* receive application datagroup */
  TipcSrvDgSetRecv(application, TRUE);


  /* flush pending messages */
  TipcSrvFlush();


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

void dbr_atexit(void)
{
  TutOut("\nDBR atexit handler called...closing connection\n\n");
  TipcSrvDestroy(T_IPC_SRV_CONN_NONE);
}

/*-----------------------------------------------------------------------*/


void dbr_output_dummy(T_STR str, va_list arg) {

  return;

}


/*-----------------------------------------------------------------------*/



