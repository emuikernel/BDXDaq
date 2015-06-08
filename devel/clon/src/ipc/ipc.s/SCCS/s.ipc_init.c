h22579
s 00009/00001/00927
d D 1.31 02/08/16 10:11:33 wolin 32 31
c license.cm
e
s 00000/00004/00928
d D 1.30 02/08/13 13:34:58 wolin 31 30
c Removed TutCommandParseStdLicense
c 
c 
c 
e
s 00003/00001/00929
d D 1.29 01/11/08 12:24:00 wolin 30 29
c Added time to ipc_close msg
e
s 00002/00000/00928
d D 1.28 01/11/08 12:21:35 wolin 29 28
c Added startup time msg
e
s 00002/00015/00926
d D 1.27 01/10/12 10:37:09 wolin 28 27
c Using init_msg_types
e
s 00009/00010/00932
d D 1.26 00/10/26 13:02:54 wolin 27 26
c const
e
s 00000/00011/00942
d D 1.25 99/11/15 14:12:22 wolin 26 25
c Removed ipc_output_dummy
e
s 00005/00007/00948
d D 1.24 98/07/02 15:18:04 wolin 25 24
c Typo, minor mods
c 
e
s 00005/00000/00950
d D 1.23 98/05/18 14:59:15 wolin 24 23
c Added server node
c 
e
s 00056/00001/00894
d D 1.22 98/05/18 14:02:55 wolin 23 22
c Added auto-reconnect, but it does not work!
c 
e
s 00001/00001/00894
d D 1.21 98/05/18 12:43:10 wolin 22 21
c Typo
c 
e
s 00007/00000/00888
d D 1.20 98/05/18 12:38:52 wolin 21 20
c Added output dummy
c 
e
s 00004/00000/00884
d D 1.19 98/05/14 14:25:08 wolin 20 19
c Added TipcInitCommands
c 
e
s 00008/00000/00876
d D 1.18 98/05/14 14:18:51 wolin 19 18
c Added reconnect control msg
c 
e
s 00002/00001/00874
d D 1.17 98/04/27 15:48:42 wolin 18 17
c Added time to ipc_atexit
c 
e
s 00001/00001/00874
d D 1.16 98/04/07 13:56:48 wolin 17 16
c Added double to ipc_check
c 
e
s 00001/00008/00874
d D 1.15 98/02/18 15:40:13 wolin 16 15
c No closing connection in exit handler in ss40
c 
e
s 00003/00000/00879
d D 1.14 98/01/05 13:33:19 wolin 15 14
c Need smartsockets include since changed clas_ipc.h
c 
e
s 00016/00008/00863
d D 1.13 97/09/30 11:06:56 wolin 14 13
c Now using getpwuid instead of getlogin
c 
e
s 00004/00000/00867
d D 1.12 97/04/01 11:38:40 wolin 13 12
c Added extra flush for pending server messages
e
s 00007/00001/00860
d D 1.11 96/12/11 09:22:53 wolin 12 11
c Bombproofed getlogin
e
s 00005/00004/00856
d D 1.10 96/10/25 16:21:26 wolin 11 10
c Add'l bombproofing
e
s 00016/00012/00844
d D 1.9 96/10/11 10:27:35 wolin 10 9
c Null uniq_dgrp now allowed if disconnect mode is not warm
e
s 00000/00000/00856
d D 1.8 96/10/08 11:02:05 wolin 9 8
c Tcl stuff now working
e
s 00010/00000/00846
d D 1.7 96/10/07 17:16:31 wolin 8 7
c Added ipc_get_unique_datagroup
e
s 00006/00001/00840
d D 1.6 96/10/02 16:30:55 wolin 7 6
c Bombproofed str2 when no string sent
e
s 00002/00002/00839
d D 1.5 96/09/26 15:41:07 wolin 6 5
c Typo in disconnect mode...default is gmd_failure
e
s 00047/00025/00794
d D 1.4 96/09/26 15:12:11 wolin 5 4
c Finished fortran interface, other minor mods
e
s 00002/00000/00817
d D 1.3 96/09/10 15:04:10 wolin 4 3
c Redid fortran api
e
s 00021/00027/00796
d D 1.2 96/09/05 12:40:05 wolin 3 1
c New scheme for init of standard message types
e
s 00000/00000/00000
d R 1.2 96/07/23 20:31:13 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/s/ipc_init.c
c Name history : 1 0 s/ipc_init.c
e
s 00823/00000/00000
d D 1.1 96/07/23 20:31:12 muguira 1 0
c 
e
u
U
f e 0
t
T
I 1
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
D 3
*         default disconnect mode is "warm"; override with ipc_set_disconnect_mode("none")
E 3
I 3
D 6
*         default disconnect mode is "none"; override with ipc_set_disconnect_mode("warm")
E 6
I 6
*         default disconnect mode is "gmd_failure"; override with ipc_set_disconnect_mode("warm")
E 6
E 3
*
*
D 5
*  C (and Fortran, soon) bindings provided
E 5
I 5
*  C and Fortran bindings provided...some functionality not available from Fortran
E 5
*
*
I 3
*  Feature/bug:
*      only the LAST non-fatal error code is returned 
*
*
E 3
*  Problems:
D 5
*      specifying fortran callbacks may not work (quit, control, etc.)
E 5
*      sigalarms may interrupt i/o, causing it to fail
*      can't place control msg in status catagory
*      handle server disconnect...need TipcServerCbCreateCb, etc.
*
*
*
D 3
*  EJW, 10-jul-96
E 3
I 3
D 5
*  EJW, 4-sep-96
E 5
I 5
*  EJW, 25-sep-96
E 5
E 3
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
I 5
*  Note:  There is a long list of c and fortran helper functions available
*
E 5
*/

I 4
#define _POSIX_SOURCE 1
#define __EXTENSIONS__
E 4

I 15
/* Smartsockets include file */
#include <rtworks/ipc.h>

E 15
#include <clas_ipc.h>
D 28
#include <clas_ipc_mt.h>
E 28
#include <clas_ipc_prototypes.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/times.h>
#include <stdlib.h>
I 14
#include <pwd.h>
I 23
#include <pthread.h>
E 23
E 14


/* 
*  Set default ipc initialization parameters, callbacks, etc.
*  These can be reset with ipc_set_xxx functions before ipc_init called
*/
static char *disconnect_mode                              = NULL;
I 32
static char *licensecm  	         	     	  = "/standard/license.cm";
E 32
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
I 14
static int  uid                                           = 0;
E 14
static time_t start_time                                  = 0;
static time_t current_time                                = 0;
static T_REAL4 cpu_time                                   = 0.;
static void (*quit_callback_func)(int)   	     	  = ipc_default_quit_callback;
D 5
static void (*user_control_callback_func)(     
E 5
I 5
static void (*user_control_message_callback_func)(
E 5
		    T_IPC_CONN,	    
		    T_IPC_CONN_PROCESS_CB_DATA,	    
		    T_CB_ARG)                        	  = NULL;
I 5
static void (*user_control_string_callback_func)(
D 27
		    char *cstr1, 
		    char *cstr2)                          = NULL;
E 27
I 27
		    const char *cstr1, 
		    const char *cstr2)                    = NULL;
E 27
E 5
static void (*user_default_callback_func)(     
		    T_IPC_CONN,	    
		    T_IPC_CONN_PROCESS_CB_DATA,	    
		    T_CB_ARG)                        	  = NULL;
D 3
static T_IPC_MT status_poll_result_mt                     = NULL;
E 3
static void (*user_status_poll_callback_func)(T_IPC_MSG)  = NULL;
I 23
void *ipc_reconnect_thread(void *param);
static int ipc_recon_time;
I 32
static char fname[256];
E 32
E 23


#define DEFAULT_APPLICATION        "clastest"
#define DEFAULT_STATUS_POLL_GROUP  "clas_monitor"
D 3
#define DEFAULT_DISCONNECT_MODE    "warm"
E 3
I 3
D 6
#define DEFAULT_DISCONNECT_MODE    "none"
E 6
I 6
#define DEFAULT_DISCONNECT_MODE    "gmd_failure"
E 6
E 3

I 23

E 23
/* prototypes for misc functions */
char *strdupf(char *, int);



/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/


/*
*  Initialize Smartsockets server connection after setting CLAS standard options
*/
D 27
int ipc_init(char *uniq_dgrp, char *id_str)
{
E 27
I 27
int ipc_init(const char *uniq_dgrp, const char *id_str) {
E 27

  T_IPC_MT mt;
  T_CB cb;
  T_OPTION opt;
  char *temp;
I 12
D 14
  char *login;
E 14
I 14
  char *login_name;
E 14
E 12
  char *setsrvnam="setopt server_names ";
  int err=0;
I 3
  int i;
I 14
  struct passwd *pwd;
I 29
  time_t now=time(NULL);
E 29
E 14
E 3


  /* set defaults if not set by user via helper functions */
  if(application       == NULL)application       = strdup(DEFAULT_APPLICATION);
  if(status_poll_group == NULL)status_poll_group = strdup(DEFAULT_STATUS_POLL_GROUP);
  if(disconnect_mode   == NULL)disconnect_mode   = strdup(DEFAULT_DISCONNECT_MODE);

  /* set default callback function if not specified by user */
  if(user_default_callback_func == NULL)user_default_callback_func=ipc_default_callback;


D 10
  /*  check for valid unique datagroup...fatal error if not specified */
  if((uniq_dgrp==NULL)||(strlen(uniq_dgrp)==0)){return(IPC_ERR_UNIQUE_DATAGROUP);}


  /* store unique datagroup name */
  if(unique_datagroup!=NULL)free(unique_datagroup);
  unique_datagroup=strdup(uniq_dgrp);


E 10
  /* store ident string */
  if(ident_string!=NULL)free(ident_string);
  ident_string=strdup(id_str);


I 14
  /* store pid */
  pid=getpid();


  /* store uid */
  uid=getuid();


E 14
  /* store user name */
  if(username!=NULL)free(username);
D 12
  username=strdup(getlogin());
E 12
I 12
D 14
  login=getlogin();
  if(login!=NULL){
    username=strdup(login);
E 14
I 14
  pwd=getpwuid(uid);
  login_name=pwd->pw_name;
  if(login_name!=NULL){
    username=strdup(login_name);
E 14
  } else {
    username=strdup("UNKNOWN");
  }
E 12


D 14
  /* store pid */
  pid=getpid();


E 14
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


D 31
  /* read Smartsockets license file */
  if(!TutCommandParseStdLicense()){if((err=IPC_ERR_LICENSE_FILE)<0)return(err);}


E 31
D 32
  /* set server names if specified...default is from $RTHOME/standard/license.cmd */
E 32
I 32
  /* set server names if specified...default is from $RTHOME/standard/license.cm */
E 32
  if(server_names!=NULL){
    temp=malloc(strlen(setsrvnam)+strlen(server_names));
    strcpy(temp,setsrvnam);
    strcat(temp,server_names);
    TutCommandParseStr(temp);
    free(temp);
  }


I 32
  /* read license.cm */
  strcpy(fname,getenv("RTHOME"));
  strcat(fname,licensecm);
  TutCommandParseFile(fname);


E 32
  /* set application */
  opt=TutOptionLookup("Application");
  if(!TutOptionSetEnum(opt,application)){if((err=IPC_ERR_APPLICATION)<0)return(err);}


D 10
  /* set unique datagroup...fatal error if unable to set */
  opt=TutOptionLookup("Unique_Datagroup");
  if(!TutOptionSetEnum(opt,unique_datagroup)){return(IPC_ERR_SET_UNIQUE_DATAGROUP);}
E 10
I 10
  /*  check for non-null uniq_dgrp if disconnect mode is warm...fatal error if not specified */
  if(((uniq_dgrp==NULL)||(strlen(uniq_dgrp)==0)) && (strcmp(disconnect_mode,"warm")==0)){
    return(IPC_ERR_UNIQUE_DATAGROUP);}
E 10


I 10
  /* set unique datagroup if provided by user...otherwise get default */
  if((uniq_dgrp!=NULL) && (strlen(uniq_dgrp)>0)){
    opt=TutOptionLookup("Unique_Datagroup");
D 27
    if(!TutOptionSetEnum(opt,uniq_dgrp)){return(IPC_ERR_SET_UNIQUE_DATAGROUP);}
E 27
I 27
    if(!TutOptionSetEnum(opt,(T_STR)uniq_dgrp)){return(IPC_ERR_SET_UNIQUE_DATAGROUP);}
E 27
  }


E 10
  /* set server disconnect mode */
  opt=TutOptionLookup("Server_Disconnect_Mode");
  if(!TutOptionSetEnum(opt,disconnect_mode)){if((err=IPC_ERR_SERVER_DISCONNECT_MODE)<0)
					       return(err);}


D 25
  /* disable GMD delivery timeout if warm connection specified */
  if(strcasecmp(disconnect_mode,"warm")==0){
    opt=TutOptionLookup("Server_Delivery_Timeout");
    if(!TutOptionSetNum(opt,0.0)){if((err=IPC_ERR_SERVER_DELIVERY_TIMEOUT)<0)return(err);}
  }
E 25
I 25
  /* disable GMD delivery timeout */
  opt=TutOptionLookup("Server_Delivery_Timeout");
  if(!TutOptionSetNum(opt,0.0)){if((err=IPC_ERR_SERVER_DELIVERY_TIMEOUT)<0)return(err);}
E 25


D 3
  /* create status_poll_result message type */
  status_poll_result_mt=TipcMtCreate(STATUS_POLL_RESULT_MTNAME,STATUS_POLL_RESULT_MTNUM,"verbose");
  if(status_poll_result_mt==NULL){if((err=IPC_ERR_STATUS_POLL_RESULT_MT_CREATE)<0)
				    return(err);}
E 3
I 3
D 28
  /* create clas standard message types and register in status or data catagory */
  for(i=0; i<sizeof(ipc_msg_type)/sizeof(struct clas_standard_msg_type); i++){
E 3

D 3
  /* add status_poll_result message type to "status" catagory */
  if(!TipcSrvLogAddMt(T_IPC_SRV_LOG_STATUS,status_poll_result_mt)){
                                if((err=IPC_ERR_LOG_MT)<0)return(err);}
E 3
I 3
    mt=TipcMtCreate(ipc_msg_type[i].mtname,ipc_msg_type[i].mtnum,"verbose");
D 11
    if(mt==NULL){if((err=IPC_ERR_MT_CREATE)<0)return(err);}
E 11
I 11
    if((mt==NULL)&&(TutErrNumGet()!=T_ERR_ALREADY_EXISTS))if((err=IPC_ERR_MT_CREATE)<0)return(err);
E 11
E 3

I 3
    if(strcmp(ipc_msg_type[i].mtname,"status_poll_result")==0){
D 11
      if(!TipcSrvLogAddMt(T_IPC_SRV_LOG_STATUS,mt)){if((err=IPC_ERR_LOG_MT)<0)return(err);}
E 11
I 11
      if((!TipcSrvLogAddMt(T_IPC_SRV_LOG_STATUS,mt))&&(TutErrNumGet()!=T_ERR_ALREADY_EXISTS))
	if((err=IPC_ERR_LOG_MT)<0)return(err);
E 11
    } else {
D 11
      if(!TipcSrvLogAddMt(T_IPC_SRV_LOG_DATA,mt)){if((err=IPC_ERR_LOG_MT)<0)return(err);}
E 11
I 11
      if((!TipcSrvLogAddMt(T_IPC_SRV_LOG_DATA,mt))&&(TutErrNumGet()!=T_ERR_ALREADY_EXISTS))
	if((err=IPC_ERR_LOG_MT)<0)return(err);
E 11
    }
E 3
D 11

E 11
I 3
  }
E 28
I 28
  /* create clas standard message types */
  init_msg_types();
E 28


E 3
D 25
  /* remove control message from data and add to status catagory /*
E 25
I 25
  /* remove control message from data and add to status catagory */
E 25
  mt=TipcMtLookupByNum(T_MT_CONTROL);
  if(!TipcSrvLogRemoveMt(T_IPC_SRV_LOG_DATA,mt)){if((err=IPC_ERR_LOG_MT)<0)return(err);}
  if(!TipcSrvLogAddMt(T_IPC_SRV_LOG_STATUS,mt)){if((err=IPC_ERR_LOG_MT)<0)return(err);}


  /* set monitoring identification string */
  if(!TipcMonSetIdentStr(ident_string)){if((err=IPC_ERR_IDENT_STRING)<0)return(err);}

  
  /* connect to server...fatal error if can't connect */
  if(!TipcSrvCreate(T_IPC_SRV_CONN_FULL)){if((err=IPC_ERR_SERVER_CREATE)<0)return(err);}


I 10
  /* get and save unique_datagroup */
  if(unique_datagroup!=NULL)free(unique_datagroup);
  opt=TutOptionLookup("Unique_Datagroup");
  TutOptionGetEnum(opt,&unique_datagroup);


E 10
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

I 25

E 25
  /* destroy default and create new control message callback */
  mt=TipcMtLookupByNum(T_MT_CONTROL);
  cb=TipcSrvProcessCbLookup(mt,TipcCbSrvProcessControl, NULL);
  if(!TutCbDestroy(cb)){if((err=IPC_ERR_CONTROL_CALLBACK_DESTROY)<0)return(err);}
  if(!TipcSrvProcessCbCreate(mt,ipc_control_callback,NULL)){
           if((err=IPC_ERR_CONTROL_CALLBACK_CREATE)<0)return(err);}

I 5

E 5
  /* create default message callback */
  if(!TipcSrvDefaultCbCreate(user_default_callback_func,NULL)){
           if((err=IPC_ERR_DEFAULT_CALLBACK_CREATE)<0)return(err);}


  /* receive all standard datagroups except "time" */
  if(!TipcSrvStdDgSetRecv(TRUE,FALSE)){if((err=IPC_ERR_STANDARD_DATAGROUPS)<0)return(err);}


  /* receive "application" datagroup */
  TipcSrvDgSetRecv(application, TRUE);


I 20
  /* enable command interp */
  TipcInitCommands();


E 20
D 5
  /* set alarm and callback for processing incoming messages */
E 5
I 5
  /* set alarm for processing incoming messages, if requested */
E 5
  if(alarm_poll_time>0){

    /* declare SIGALRM handler */
    if(signal(SIGALRM,ipc_alarm_handler)==SIG_ERR){
      if((err=IPC_ERR_ALARM_HANDLER)<0)return(err);}

    /* set alarm */
    alarm(alarm_poll_time);

  }    


I 13
  /* flush all pending messages */
  TipcSrvFlush();


E 13
  /* done */
I 29
  TutOut("\nipc_init called at %s\n",ctime(&now));
E 29
  return(err);

}


/*-----------------------------------------------------------------------*/


/* destroys IPC server connection */

int ipc_close(void)
{
D 30
  TutOut("\nipc_close called...closing connection\n\n");
E 30
I 30
  time_t now=time(NULL);

  TutOut("\nipc_close called...closing connection at %s\n\n",ctime(&now));
E 30

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
D 17
  return(TipcSrvMainLoop(timeout));
E 17
I 17
  return(TipcSrvMainLoop((double)timeout));
E 17
}


/*-----------------------------------------------------------------------*/


/* destroys IPC server connection...called at exit...just in case */

void ipc_atexit(void)
{
D 16
  TutOut("\nIPC atexit handler called...closing connection\n\n");

  if(strcasecmp(disconnect_mode,"warm")==0) {
    TipcSrvDestroy(T_IPC_SRV_CONN_WARM);
  }
  else {
    TipcSrvDestroy(T_IPC_SRV_CONN_NONE);
  }
E 16
I 16
D 18
  TutOut("\nIPC atexit handler called...connection should be closed automatically by ss40 \n\n");
E 18
I 18
  time_t now=time(NULL);
  TutOut("\nIPC atexit handler called at %s\n",ctime(&now));
E 18
E 16
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
D 5
*  can specify a callback to process unknown messages (via ipc_set_user_control_callback()) 
E 5
I 5
*  can specify a callback to process unknown messages (via ipc_set_control_message_callback
*    or via ipc_set_control_string_callback)
*    
E 5
*
D 5
*
E 5
*  clas standard commands:
*
*             QUIT    		 call quit callback handler
*      LOG_IN_DATA [ON|OFF]	 start logging incoming data messages
*     LOG_OUT_DATA [ON|OFF]	 start logging outgoing data messages
*    LOG_IN_STATUS [ON|OFF]	 start logging incoming status messages
*   LOG_OUT_STATUS [ON|OFF]	 start logging outgoing status messages
*       GMD_RESEND    		 resend all gmd messages (not sure if this works...)
*      STATUS_POLL    		 send status_poll_result to monitoring group
I 19
*        RECONNECT               disconnect and reconnect to server
E 19
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
I 5
  T_IPC_FT type;
I 7
  char *dummy="";
E 7
E 5


D 5
  /* get number of fields and 1st 2 strings */
  TipcMsgGetNumFields(data->msg,&nfield);
E 5
I 5
  /* get first string */
E 5
  TipcMsgSetCurrent(data->msg,0);
D 5
  TipcMsgRead(data->msg,
	      T_IPC_FT_STR, &str1,
	      NULL);
E 5
I 5
  TipcMsgRead(data->msg,T_IPC_FT_STR,&str1,NULL);

  /* get second string, if it exists */
  str2=NULL;
  TipcMsgGetNumFields(data->msg,&nfield);
E 5
  if(nfield>1){
D 5
    TipcMsgRead(data->msg,
		T_IPC_FT_STR, &str2,
		NULL);
E 5
I 5
    TipcMsgNextType(data->msg,&type);
    if(type==T_IPC_FT_STR)TipcMsgRead(data->msg,T_IPC_FT_STR,&str2,NULL);
E 5
  }
D 5
  else {
    str2=NULL;
  }
E 5


I 5

E 5
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

I 19

  /* RECONNECT */
  else if(strncasecmp(str1,"reconnect",9)==0) {
    TutCommandParseStr("disconnect");
D 23
    TutCommandParseStr("reconnect");
E 23
I 23
    TutCommandParseStr("connect");
E 23
  }

E 19

D 5
  /* UNKNOWN...pass to user callback, if defined */
  else if(user_control_callback_func!=NULL){
      (user_control_callback_func)(conn,data,arg);
E 5
I 5
  /* UNKNOWN...pass control to user message callback, if defined */
  else if(user_control_message_callback_func!=NULL){
      (user_control_message_callback_func)(conn,data,arg);
E 5
  }
  

D 5
  /* no user callback...pass to Smartsockets command string parser */
E 5
I 5
D 7
  /* UNKNOWN...but no message callback...pass first 2 strings to user string callback, if defined */
E 7
I 7
  /* UNKNOWN...but no message callback...pass first 2 strings to callback */
E 7
  else if(user_control_string_callback_func!=NULL){
I 7
    if(str2==NULL){
      (user_control_string_callback_func)(str1,dummy);
    } else {
E 7
      (user_control_string_callback_func)(str1,str2);
I 7
    }
E 7
  }
  

  /* UNKNOWN...but no user callbacks...pass first string to Smartsockets command string parser */
E 5
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
I 24
  T_STR server;
E 24


  /* increment request counter */
  num_status_poll_req++;

  /* create  message */
D 3
  msg=TipcMsgCreate(status_poll_result_mt);
E 3
I 3
  msg=TipcMsgCreate(TipcMtLookup("status_poll_result"));
E 3

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

I 24
  /* append server node */
  TipcSrvGetNode(&server);
  TipcMsgAppendStr(msg,server);

E 24
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

D 27
int ipc_set_server_names(char *names)
E 27
I 27
int ipc_set_server_names(const char *names)
E 27
{
  if(server_names!=NULL)free(server_names);
  server_names=strdup(names);
  return(0);
}


/*-----------------------------------------------------------------*/


D 27
int ipc_set_disconnect_mode(char *name)
E 27
I 27
int ipc_set_disconnect_mode(const char *name)
E 27
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


D 27
int ipc_set_application(char *name)
E 27
I 27
int ipc_set_application(const char *name)
E 27
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


I 8
int ipc_get_unique_datagroup(char **name)
{
  *name=strdup(unique_datagroup);
  return(0);
}


/*-----------------------------------------------------------------*/


E 8
D 27
int ipc_set_status_poll_group(char *name)
E 27
I 27
int ipc_set_status_poll_group(const char *name)
E 27
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
D 3


/*-----------------------------------------------------------------*/


int ipc_get_status_poll_result_mt(T_IPC_MT *mt)
{
  if(status_poll_result_mt!=NULL){
    *mt=status_poll_result_mt;
    return(0);
  }
  else {
    return(IPC_ERR_NULL_STATUS_POLL_RESULT_MT);
  }
}
E 3


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


D 5
int ipc_set_user_control_callback(
E 5
I 5
int ipc_set_control_message_callback(
E 5
                    void (*func)(T_IPC_CONN,
		                 T_IPC_CONN_PROCESS_CB_DATA,
 		                 T_CB_ARG))
{
D 5
  user_control_callback_func=func;
E 5
I 5
  user_control_message_callback_func=func;
E 5
  return(0);
}


/*-----------------------------------------------------------------*/


D 5
int ipc_set_user_default_callback(
E 5
I 5
int ipc_set_control_string_callback(
D 27
                    void (*func)(char *cstr1, char *cstr2))
E 27
I 27
                    void (*func)(const char *cstr1, const char *cstr2))
E 27
{
  user_control_string_callback_func=func;
  return(0);
}


/*-----------------------------------------------------------------*/


int ipc_set_user_default_callback_func(
E 5
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


I 21
D 22
void output_dummy(T_STR str, va_list arg) {
E 22
I 22
D 26
void ipc_output_dummy(T_STR str, va_list arg) {
E 22
E 21

I 21
  return;

}

I 23

/*-----------------------------------------------------------------------*/



E 26
int ipc_init_reconnect(int rtime) {

  pthread_t reconnect;
  int r=0;
  

  /* save reconnect time */
  ipc_recon_time=rtime;


  /* enable commands */
  TipcInitCommands();


  /* bump thread count */
  thr_setconcurrency(thr_getconcurrency()+1);


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

D 25
  return (void *)0;
E 25
}

E 23

/*-----------------------------------------------------------------------*/
E 21
E 1
