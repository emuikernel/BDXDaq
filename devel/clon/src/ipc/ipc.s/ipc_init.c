/*
*  ipc_init.c
*    
*
*  Library for initializion of Smartsockets IPC for CLAS
*    sets CLAS standard IPC options
*    sets CLAS standard GMD handling options
*    sets TERM,HUP,QUIT,INT signals to call quit callback function
*    opens full connection to server
*    sets up control message handler which implements data message logging, gmd resend, 
*         STATUS_POLL handling, etc.
*         ..."quit" message routed to quit callback function
*    provides default message callback handler
*    alarm callback for message processing set up if alarm_poll_time set to positive integer
*    calls TipcSrvDestroy() at exit 
*
*
*  Users can:
*        override default quit callback
*        specify control callback to handle unknown control messages
*        provide custom STATUS_POLL handler
*        specify default callback to handle unexpected messages
*        enable SIGALRM for checking for incoming messages (use ipc_set_alarm_poll_time)
*        override default server disconnect mode
*
*
*  NOTES: default application is "clastest" (use ipc_get_application to retrieve this)
*         use ipc_set_application("clasprod") to join CLAS production application
*
*         default disconnect mode is "gmd_failure"; override with ipc_set_disconnect_mode("warm")
*
*
*  C and Fortran bindings provided...some functionality not available from Fortran
*
*
*  Feature/bug:
*      only the LAST non-fatal error code is returned 
*
*
*  Problems:
*      sigalarms may interrupt i/o, causing it to fail
*      can't place control msg in status catagory
*      handle server disconnect...need TipcServerCbCreateCb, etc.
*
*
*
*  EJW, 25-sep-96
*
*
*  usage from c
*  ------------
*
*    int ipc_init(
*        char *unique_datagroup,         !unique name for this process    
*        char *ident_string              !monitoring identification string
*        )
*        returns 0 if successful, <0 for fatal error, >0 for non-fatal warning
*        use ipc_perror(err) to print error message
*
*
*  usage from fortran
*  ------------------
*
*      integer ipc_init(unique_datagroup,ident_string)
*           character*(*) unique_datagroup
*           character*(*) ident_string
*        returns 0 if successful, <0 for fatal error, >0 for non-fatal warning
*        call ipc_perror(err) to print error message
*
*
*  Note:  There is a long list of c and fortran helper functions available
*
*/

#define _POSIX_SOURCE 1
#define __EXTENSIONS__

/* Smartsockets include file */
#include <rtworks/ipc.h>

#include <clas_ipc.h>
#include <clas_ipc_prototypes.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/times.h>
#include <stdlib.h>
#include <pwd.h>
#include <pthread.h>


/* 
*  Set default ipc initialization parameters, callbacks, etc.
*  These can be reset with ipc_set_xxx functions before ipc_init called
*/
static char *disconnect_mode                              = NULL;
static char *licensecm  	         	     	  = "/standard/license.cm";
static char *server_names  	         	     	  = NULL;
static char *application   	         	     	  = NULL;
static char *status_poll_group                            = NULL;
static int  alarm_poll_time    	         	     	  = 0;
static char *unique_datagroup            	     	  = NULL;
static char *ident_string                	     	  = NULL;
static int  num_status_poll_req                           = 0;
static char *username                                	  = NULL;
static char *nodename                                     = NULL;
static int  pid                                           = 0;
static int  uid                                           = 0;
static time_t start_time                                  = 0;
static time_t current_time                                = 0;
static T_REAL4 cpu_time                                   = 0.;
static void (*quit_callback_func)(int)   	     	  = ipc_default_quit_callback;
static void (*user_control_message_callback_func)(
		    T_IPC_CONN,	    
		    T_IPC_CONN_PROCESS_CB_DATA,	    
		    T_CB_ARG)                        	  = NULL;
static void (*user_control_string_callback_func)(
		    const char *cstr1, 
		    const char *cstr2)                    = NULL;
static void (*user_default_callback_func)(     
		    T_IPC_CONN,	    
		    T_IPC_CONN_PROCESS_CB_DATA,	    
		    T_CB_ARG)                        	  = NULL;
static void (*user_status_poll_callback_func)(T_IPC_MSG)  = NULL;
void *ipc_reconnect_thread(void *param);
static int ipc_recon_time;
static char fname[256];


#define DEFAULT_APPLICATION        "clastest"
#define DEFAULT_STATUS_POLL_GROUP  "clas_monitor"
#define DEFAULT_DISCONNECT_MODE    "gmd_failure"


/* prototypes for misc functions */
char *strdupf(char *, int);



/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/


/*
*  Initialize Smartsockets server connection after setting CLAS standard options
*/
int ipc_init(const char *uniq_dgrp, const char *id_str) {

  T_IPC_MT mt;
  T_CB cb;
  T_OPTION opt;
  char *temp;
  char *login_name;
  char *setsrvnam="setopt server_names ";
  int err=0;
  int i;
  struct passwd *pwd;
  time_t now=time(NULL);


  /* set defaults if not set by user via helper functions */
  if(application       == NULL)application       = strdup(DEFAULT_APPLICATION);
  if(status_poll_group == NULL)status_poll_group = strdup(DEFAULT_STATUS_POLL_GROUP);
  if(disconnect_mode   == NULL)disconnect_mode   = strdup(DEFAULT_DISCONNECT_MODE);

  /* set default callback function if not specified by user */
  if(user_default_callback_func == NULL)user_default_callback_func=ipc_default_callback;


  /* store ident string */
  if(ident_string!=NULL)free(ident_string);
  ident_string=strdup(id_str);


  /* store pid */
  pid=getpid();


  /* store uid */
  uid=getuid();


  /* store user name */
  if(username!=NULL)free(username);
  pwd=getpwuid(uid);
  login_name=pwd->pw_name;
  if(login_name!=NULL){
    username=strdup(login_name);
  } else {
    username=strdup("UNKNOWN");
  }


  /* store nodename */
  if(nodename!=NULL)free(nodename);
  nodename= (char*) malloc(30);
  gethostname(nodename,30);


  /* store start time*/
  start_time=time(NULL);


  /* set quit callback to handle all termination-type signals */
  if(signal(SIGTERM,quit_callback_func)==SIG_ERR){if((err=IPC_ERR_SIGTERM)<0)return(err);}
  if(signal(SIGQUIT,quit_callback_func)==SIG_ERR){if((err=IPC_ERR_SIGQUIT)<0)return(err);}
  if(signal( SIGINT,quit_callback_func)==SIG_ERR){if((err=IPC_ERR_SIGINT)<0)return(err);}
  if(signal( SIGHUP,quit_callback_func)==SIG_ERR){if((err=IPC_ERR_SIGHUP)<0)return(err);}
  if(atexit(ipc_atexit)!=0){if((err=IPC_ERR_ATEXIT)<0)return(err);}


  /* set server names if specified...default is from $RTHOME/standard/license.cm */
  if(server_names!=NULL){
    temp=malloc(strlen(setsrvnam)+strlen(server_names));
    strcpy(temp,setsrvnam);
    strcat(temp,server_names);
    TutCommandParseStr(temp);
    free(temp);
  }


  /* read license.cm */
  strcpy(fname,getenv("RTHOME"));
  strcat(fname,licensecm);
  TutCommandParseFile(fname);


  /* set application */
  opt=TutOptionLookup("Application");
  if(!TutOptionSetEnum(opt,application)){if((err=IPC_ERR_APPLICATION)<0)return(err);}


  /*  check for non-null uniq_dgrp if disconnect mode is warm...fatal error if not specified */
  if(((uniq_dgrp==NULL)||(strlen(uniq_dgrp)==0)) && (strcmp(disconnect_mode,"warm")==0)){
    return(IPC_ERR_UNIQUE_DATAGROUP);}


  /* set unique datagroup if provided by user...otherwise get default */
  if((uniq_dgrp!=NULL) && (strlen(uniq_dgrp)>0)){
    opt=TutOptionLookup("Unique_Datagroup");
    if(!TutOptionSetEnum(opt,(T_STR)uniq_dgrp)){return(IPC_ERR_SET_UNIQUE_DATAGROUP);}
  }


  /* set server disconnect mode */
  opt=TutOptionLookup("Server_Disconnect_Mode");
  if(!TutOptionSetEnum(opt,disconnect_mode)){if((err=IPC_ERR_SERVER_DISCONNECT_MODE)<0)
					       return(err);}


  /* disable GMD delivery timeout */
  opt=TutOptionLookup("Server_Delivery_Timeout");
  if(!TutOptionSetNum(opt,0.0)){if((err=IPC_ERR_SERVER_DELIVERY_TIMEOUT)<0)return(err);}


  /* create clas standard message types */
  init_msg_types();


  /* remove control message from data and add to status catagory */
  mt=TipcMtLookupByNum(T_MT_CONTROL);
  if(!TipcSrvLogRemoveMt(T_IPC_SRV_LOG_DATA,mt)){if((err=IPC_ERR_LOG_MT)<0)return(err);}
  if(!TipcSrvLogAddMt(T_IPC_SRV_LOG_STATUS,mt)){if((err=IPC_ERR_LOG_MT)<0)return(err);}


  /* set monitoring identification string */
  if(!TipcMonSetIdentStr(ident_string)){if((err=IPC_ERR_IDENT_STRING)<0)return(err);}

  
  /* connect to server...fatal error if can't connect */
  if(!TipcSrvCreate(T_IPC_SRV_CONN_FULL)){if((err=IPC_ERR_SERVER_CREATE)<0)return(err);}


  /* get and save unique_datagroup */
  if(unique_datagroup!=NULL)free(unique_datagroup);
  opt=TutOptionLookup("Unique_Datagroup");
  TutOptionGetEnum(opt,&unique_datagroup);


  /* destroy default connection callback for GMD failure if warm connection specified */
  if(strcasecmp(disconnect_mode,"warm")==0){
    mt=TipcMtLookupByNum(T_MT_GMD_FAILURE);
    cb=TipcSrvProcessCbLookup(mt,TipcCbConnProcessGmdFailure, NULL);
    if(!TutCbDestroy(cb)){if((err=IPC_ERR_CONN_CALLBACK_DESTROY)<0)return(err);}
  }


  /* destroy default server callback for GMD failure if warm connection specified */
  if(strcasecmp(disconnect_mode,"warm")==0){
    cb=TipcSrvProcessCbLookup(mt,TipcCbSrvProcessGmdFailure, NULL);
    if(!TutCbDestroy(cb)){if((err=IPC_ERR_SERVER_GMD_FAILURE)<0)return(err);}
  }


  /* destroy default and create new control message callback */
  mt=TipcMtLookupByNum(T_MT_CONTROL);
  cb=TipcSrvProcessCbLookup(mt,TipcCbSrvProcessControl, NULL);
  if(!TutCbDestroy(cb)){if((err=IPC_ERR_CONTROL_CALLBACK_DESTROY)<0)return(err);}
  if(!TipcSrvProcessCbCreate(mt,ipc_control_callback,NULL)){
           if((err=IPC_ERR_CONTROL_CALLBACK_CREATE)<0)return(err);}


  /* create default message callback */
  if(!TipcSrvDefaultCbCreate(user_default_callback_func,NULL)){
           if((err=IPC_ERR_DEFAULT_CALLBACK_CREATE)<0)return(err);}


  /* receive all standard datagroups except "time" */
  if(!TipcSrvStdDgSetRecv(TRUE,FALSE)){if((err=IPC_ERR_STANDARD_DATAGROUPS)<0)return(err);}


  /* receive "application" datagroup */
  TipcSrvDgSetRecv(application, TRUE);


  /* enable command interp */
  TipcInitCommands();


  /* set alarm for processing incoming messages, if requested */
  if(alarm_poll_time>0){

    /* declare SIGALRM handler */
    if(signal(SIGALRM,ipc_alarm_handler)==SIG_ERR){
      if((err=IPC_ERR_ALARM_HANDLER)<0)return(err);}

    /* set alarm */
    alarm(alarm_poll_time);

  }    


  /* flush all pending messages */
  TipcSrvFlush();


  /* done */
  TutOut("\nipc_init called at %s\n",ctime(&now));
  return(err);

}


/*-----------------------------------------------------------------------*/


/* destroys IPC server connection */

int ipc_close(void)
{
  time_t now=time(NULL);

  TutOut("\nipc_close called...closing connection at %s\n\n",ctime(&now));

  if(strcasecmp(disconnect_mode,"warm")==0) {
    TipcSrvDestroy(T_IPC_SRV_CONN_WARM);
  } 
  else {
    TipcSrvDestroy(T_IPC_SRV_CONN_NONE);
  }
  return(0);
}


/*-----------------------------------------------------------------------*/


/* processes messages in queue */

int ipc_check(float timeout)
{
  return(TipcSrvMainLoop((double)timeout));
}


/*-----------------------------------------------------------------------*/


/* destroys IPC server connection...called at exit...just in case */

void ipc_atexit(void)
{
  time_t now=time(NULL);
  TutOut("\nIPC atexit handler called at %s\n",ctime(&now));
}

/*-----------------------------------------------------------------------*/


/*  default quit callback just closes connection */

void ipc_default_quit_callback(int sig)
{
  if(sig>0){
    TutOut("\n*** Default quit callback called...received signal %d\n",sig);
  }
  else {
    TutOut("\n*** Default quit callback called...received QUIT control message\n");
  }    

  TutOut("     ...destroying connection to server ***\n\n");
  ipc_close();
  exit(0);
}

/*-----------------------------------------------------------------------*/


/*
*  ipc_control_callback
*
*
*  callback to process control messages
*
*  unrecognized control messages passed to user_control_callback, if defined
*  otherwise sent to smartsockets parser
*
*  can specify a callback to process unknown messages (via ipc_set_control_message_callback
*    or via ipc_set_control_string_callback)
*    
*
*  clas standard commands:
*
*             QUIT    		 call quit callback handler
*      LOG_IN_DATA [ON|OFF]	 start logging incoming data messages
*     LOG_OUT_DATA [ON|OFF]	 start logging outgoing data messages
*    LOG_IN_STATUS [ON|OFF]	 start logging incoming status messages
*   LOG_OUT_STATUS [ON|OFF]	 start logging outgoing status messages
*       GMD_RESEND    		 resend all gmd messages (not sure if this works...)
*      STATUS_POLL    		 send status_poll_result to monitoring group
*        RECONNECT               disconnect and reconnect to server
*
*
*/

void ipc_control_callback(
			     T_IPC_CONN conn,
   	     T_IPC_CONN_PROCESS_CB_DATA data,
			       T_CB_ARG arg)
{
  T_STR str1,str2;
  T_OPTION opt;
  char *temp;
  T_INT4 nfield;
  T_IPC_FT type;
  char *dummy="";


  /* get first string */
  TipcMsgSetCurrent(data->msg,0);
  TipcMsgRead(data->msg,T_IPC_FT_STR,&str1,NULL);

  /* get second string, if it exists */
  str2=NULL;
  TipcMsgGetNumFields(data->msg,&nfield);
  if(nfield>1){
    TipcMsgNextType(data->msg,&type);
    if(type==T_IPC_FT_STR)TipcMsgRead(data->msg,T_IPC_FT_STR,&str2,NULL);
  }



  /* now figure out what to do... */


  /* QUIT...call quit_callback_func */
  if(strncasecmp(str1,"quit",4)==0) {
      (quit_callback_func)(0);
  }


  /* LOG_IN_DATA...enable/disable logging of incoming data messages */
  else if(!strncasecmp(str1,"log_in_data",11)) {
    if((str2==NULL) || (strncasecmp(str2,"OFF",3)!=0) ){
      temp=(char *) malloc(strlen(unique_datagroup)+10);
      strcpy(temp,unique_datagroup);
      strcat(temp,".data.in");
      opt=TutOptionLookup("Log_In_Data");
      if(!TutOptionSetStr(opt,temp))TutWarning("?unable to set log in data\n");
      free(temp);
    }
    else {
      TutOut("Stopped logging data in\n");
      TutCommandParseStr("unsetopt log_in_data");
    }
  }
    
    
  /* LOG_OUT_DATA...enable logging of outgoing data messages */
  else if(!strncasecmp(str1,"log_out_data",12)){
    if((str2==NULL) || (strncasecmp(str2,"OFF",3)!=0) ){
      temp=(char *) malloc(strlen(unique_datagroup)+10);
      strcpy(temp,unique_datagroup);
      strcat(temp,".data.out");
      opt=TutOptionLookup("Log_Out_Data");
      if(!TutOptionSetStr(opt,temp))TutWarning("?unable to set log out data\n");
      free(temp);
    }
    else {
      TutOut("Stopped logging data out\n");
      TutCommandParseStr("unsetopt log_out_data");
    }
  }
    

  /* LOG_IN_STATUS...enable/disable logging of incoming status messages */
  else if(!strncasecmp(str1,"log_in_status",13)) {
    if((str2==NULL) || (strncasecmp(str2,"OFF",3)!=0) ){
      temp=(char *) malloc(strlen(unique_datagroup)+10);
      strcpy(temp,unique_datagroup);
      strcat(temp,".stat.in");
      opt=TutOptionLookup("Log_In_Status");
      if(!TutOptionSetStr(opt,temp))TutWarning("?unable to set log in status\n");
      free(temp);
    }
    else {
      TutOut("Stopped logging status in\n");
      TutCommandParseStr("unsetopt log_in_status");
    }
  }
    
    
  /* LOG_OUT_STATUS...enable logging of outgoing status messages */
  else if(!strncasecmp(str1,"log_out_status",14)){
    if((str2==NULL) || (strncasecmp(str2,"OFF",3)!=0) ){
      temp=(char *) malloc(strlen(unique_datagroup)+10);
      strcpy(temp,unique_datagroup);
      strcat(temp,".stat.out");
      opt=TutOptionLookup("Log_Out_Status");
      if(!TutOptionSetStr(opt,temp))TutWarning("?unable to set log out status\n");
      free(temp);
    }
    else {
      TutOut("Stopped logging status out\n");
      TutCommandParseStr("unsetopt log_out_status");
    }
  }
    

  /* GMD_RESEND...resend messages in gmd file ??? will this work ??? */
  else if(strncasecmp(str1,"gmd_resend",10)==0) {
    TipcSrvGmdResend();
    TipcSrvFlush();
  }


  /* STATUS_POLL...send status_poll_result to "monitor" group */
  else if(strncasecmp(str1,"status_poll",11)==0) {
    ipc_send_status_poll_result();
  }


  /* RECONNECT */
  else if(strncasecmp(str1,"reconnect",9)==0) {
    TutCommandParseStr("disconnect");
    TutCommandParseStr("connect");
  }


  /* UNKNOWN...pass control to user message callback, if defined */
  else if(user_control_message_callback_func!=NULL){
      (user_control_message_callback_func)(conn,data,arg);
  }
  

  /* UNKNOWN...but no message callback...pass first 2 strings to callback */
  else if(user_control_string_callback_func!=NULL){
    if(str2==NULL){
      (user_control_string_callback_func)(str1,dummy);
    } else {
      (user_control_string_callback_func)(str1,str2);
    }
  }
  

  /* UNKNOWN...but no user callbacks...pass first string to Smartsockets command string parser */
  else {
      TutCommandParseStr(str1);
  }
  
}


/*-----------------------------------------------------------------------*/

/*
*  ipc_default_callback
*
*
*  callback to process unexpected messages
*
*  does nothing
*
*  can specify user callback to process unknown messages (via ipc_set_user_default_callback()) 
*
*/

void ipc_default_callback(
			     T_IPC_CONN conn,
   	     T_IPC_CONN_PROCESS_CB_DATA data,
			       T_CB_ARG arg)
{
  return;
}


/*-----------------------------------------------------------------------*/

/*
*  ipc_send_status_poll_result
*
*
*  Sends status poll result to status_poll_group
*
*  First sends CLAS standard information, then
*   appends user-defined info via user-supplied function
*    (set with ipc_set_user_status_poll_callback)
*
*  uses key "end_system_data" to end system part of message

*/
int ipc_send_status_poll_result()
{
  int err=0;
  int cputime;
  T_IPC_MSG msg;
  struct tms tbuf;
  T_STR server;


  /* increment request counter */
  num_status_poll_req++;

  /* create  message */
  msg=TipcMsgCreate(TipcMtLookup("status_poll_result"));

  /* set destination */
  TipcMsgSetDest(msg,status_poll_group);

  /* append unique_datagroup */
  TipcMsgAppendStr(msg,unique_datagroup);

  /* append username */
  TipcMsgAppendStr(msg,username);

  /* append nodename */
  TipcMsgAppendStr(msg,nodename);

  /* append pid */
  TipcMsgAppendInt4(msg,pid);

  /* append start time */
  TipcMsgAppendInt4(msg,start_time);

  /* append current time */
  current_time=time(NULL);
  TipcMsgAppendInt4(msg,current_time);

  /* append system + user cputime for process and children */
  times(&tbuf);
  cpu_time=((float)(tbuf.tms_utime+tbuf.tms_stime+tbuf.tms_cutime+tbuf.tms_cstime)) / 
    (float) CLK_TCK;
  TipcMsgAppendReal4(msg,cpu_time);

  /* append number of responses so far*/
  TipcMsgAppendInt4(msg,num_status_poll_req);

  /* append server node */
  TipcSrvGetNode(&server);
  TipcMsgAppendStr(msg,server);

  /* append system end key */
  TipcMsgAppendStr(msg,"end_system_data");

  /* add extra user-defined information */
  if(user_status_poll_callback_func!=NULL){
    (user_status_poll_callback_func)(msg);
  }

  /* append user end key */
  TipcMsgAppendStr(msg,"end_user_data");

     
  /* ship the message off */
  TipcSrvMsgSend(msg,TRUE);
  TipcSrvFlush();
  TipcMsgDestroy(msg);

  return(err);
}


/*-----------------------------------------------------------------------*/


/* 
*  SIGALARM handler processes IPC messages every alarm_poll_time seconds
*/

void ipc_alarm_handler(int i)
{

  /* process all messages in queue */
  TipcSrvMainLoop(0.);

  /* redeclare alarm signal handler */
  signal(SIGALRM,ipc_alarm_handler);

  /* reset alarm */
  alarm(alarm_poll_time);

}


/*-----------------------------------------------------------------------*/


/* 
*  helper functions reset initialization parameters, etc.
*/

int ipc_set_server_names(const char *names)
{
  if(server_names!=NULL)free(server_names);
  server_names=strdup(names);
  return(0);
}


/*-----------------------------------------------------------------*/


int ipc_set_disconnect_mode(const char *name)
{
  if(disconnect_mode!=NULL)free(disconnect_mode);
  disconnect_mode=strdup(name);
  return(0);
}


/*-----------------------------------------------------------------*/


int ipc_get_disconnect_mode(char **name)
{
  if(disconnect_mode!=NULL){
    *name=strdup(disconnect_mode);
    return(0);
  }
  else {
    *name=NULL;
    return(IPC_ERR_NULL_DISCONNECT_MODE);
  }
}


/*-----------------------------------------------------------------*/


int ipc_set_application(const char *name)
{
  if(application!=NULL)free(application);
  application=strdup(name);
  return(0);
}


/*-----------------------------------------------------------------*/


int ipc_get_application(char **name)
{
  if(application!=NULL){
    *name=strdup(application);
    return(0);
  }
  else {
    *name=NULL;
    return(IPC_ERR_NULL_APPLICATION);
  }
}


/*-----------------------------------------------------------------*/


int ipc_get_unique_datagroup(char **name)
{
  *name=strdup(unique_datagroup);
  return(0);
}


/*-----------------------------------------------------------------*/


int ipc_set_status_poll_group(const char *name)
{
  if(status_poll_group!=NULL)free(status_poll_group);
  status_poll_group=strdup(name);
  return(0);
}


/*-----------------------------------------------------------------*/


int ipc_get_status_poll_group(char **name)
{
  if(status_poll_group!=NULL){
    *name=strdup(status_poll_group);
    return(0);
  }
  else {
    *name=NULL;
    return(IPC_ERR_NULL_STATUS_POLL_GROUP);
  }
}


/*-----------------------------------------------------------------*/


int ipc_set_alarm_poll_time(int time)
{
  alarm_poll_time=time;
  return(0);
}


/*-----------------------------------------------------------------*/


int ipc_set_quit_callback(void (*func)(int))
{
  quit_callback_func=func;
  return(0);
}


/*-----------------------------------------------------------------*/


int ipc_set_control_message_callback(
                    void (*func)(T_IPC_CONN,
		                 T_IPC_CONN_PROCESS_CB_DATA,
 		                 T_CB_ARG))
{
  user_control_message_callback_func=func;
  return(0);
}


/*-----------------------------------------------------------------*/


int ipc_set_control_string_callback(
                    void (*func)(const char *cstr1, const char *cstr2))
{
  user_control_string_callback_func=func;
  return(0);
}


/*-----------------------------------------------------------------*/


int ipc_set_user_default_callback_func(
                    void (*func)(T_IPC_CONN,
		                 T_IPC_CONN_PROCESS_CB_DATA,
 		                 T_CB_ARG))
{
  user_default_callback_func=func;
  return(0);
}


/*-----------------------------------------------------------------*/


int ipc_set_user_status_poll_callback(void (*func)(T_IPC_MSG))
{
  user_status_poll_callback_func=func;
  return(0);
}


/*-----------------------------------------------------------------------*/


int ipc_init_reconnect(int rtime) {

  pthread_t reconnect;
  int r=0;
  

  /* save reconnect time */
  ipc_recon_time=rtime;


  /* enable commands */
  TipcInitCommands();

#ifdef SunOS
  /* bump thread count */
  thr_setconcurrency(thr_getconcurrency()+1);
#endif

  /* launch reconnect thread */
  return(pthread_create(&reconnect,NULL,ipc_reconnect_thread,(void *)&r));

}


/*-----------------------------------------------------------------------*/


void *ipc_reconnect_thread(void *param) {

  T_STR srv_node;

  while(0==0) {

    sleep(ipc_recon_time);

    /* reconnect only if node and local node not the same */
    TipcSrvGetNode(&srv_node);
    if(strcasecmp(getenv("HOST"),srv_node)!=0) {
      TutCommandParseStr("disconnect");
      TutCommandParseStr("connect");
    }
  }

}


/*-----------------------------------------------------------------------*/
