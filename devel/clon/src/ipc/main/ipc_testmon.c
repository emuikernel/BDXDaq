/*
*  ipc_testmon
* 
*  Ascii test version of ipc_monitor
*
*  ejw, W&M, 9-sep-96
*
*/


#define _POSIX_SOURCE 1
#define __EXTENSIONS__

#include <rtworks/ipc.h>
#include <stdlib.h>
#include <signal.h>


typedef int bool;
const bool true = 1;
const bool false = 0;   

static char *application = "clastest";
static char *destination = NULL;
static int once          = 0;
static int polltime      = 0;
static int waittime      = 5;
static int all           = 0;
static int brief         = 0;


void print_status_result(
     T_IPC_CONN conn,
     T_IPC_CONN_PROCESS_CB_DATA data,
     T_CB_ARG arg);
void mystatuspollcallback(T_IPC_MSG msg);
void decode_command_line(int argc, char **argv);
void alarm_handler(int i);
void print_opt_info(T_IPC_MSG msg, int max);
void ipc_output_dummy(T_STR str, va_list arg);


/*------------------------------------------------------------------*/


int main(int argc, char **argv) {

  char *mon_dg;


  /* decode command line */
  decode_command_line(argc,argv);
  
  /* set application */
  ipc_set_application(application);
  
  /* set destination */
  if(destination==NULL)destination=strdup(application);
  
  /* inhibit boilerplate for brief mode */
  if(brief==1)TutSetOutputFunc(ipc_output_dummy);

  /*  connect to server */
  ipc_init("","ipc test monitor");
  
  /*  define callback to add user information to status_poll_result message */
  ipc_set_user_status_poll_callback(mystatuspollcallback);
  
  /* receive status_poll_result message group */
  ipc_get_status_poll_group(&mon_dg);
  TipcSrvDgSetRecv(mon_dg,TRUE);
  
  /*  define callback to handle (i.e. print) status_poll_result messages */
  TipcSrvProcessCbCreate(TipcMtLookup("status_poll_result"),print_status_result,NULL);
  
  /* set alarm if polltime > 0 */
  if(polltime>0) {
    
    /* declare SIGALRM handler */
    signal(SIGALRM,alarm_handler);
    
    /* set alarm */
    alarm(polltime);

  } else if (once==1) {
    T_IPC_MSG msg=TipcMsgCreate(TipcMtLookupByNum(T_MT_CONTROL));
    TipcMsgSetDest(msg,destination);
    TipcMsgAppendStr(msg,"status_poll");
    TipcSrvMsgSend(msg,TRUE);
    TipcSrvFlush();
    TipcMsgDestroy(msg);
  }
  
  /* process messages */
  if(brief==1)TutSetOutputFunc(NULL);
  for (;;) {
    TipcSrvMainLoop((double)waittime);
    if(once==1)break;
  }
  if(brief==1)TutSetOutputFunc(ipc_output_dummy);
  
  exit(EXIT_SUCCESS);
}


/*------------------------------------------------------------------*/


void print_status_result(
			 T_IPC_CONN conn,
     T_IPC_CONN_PROCESS_CB_DATA data,
			 T_CB_ARG arg) {
  
  T_IPC_MSG msg;
  int len;
  T_IPC_FT type;
  T_INT4 n;
  T_INT4 int4;
  T_REAL4 flt;
  T_REAL8 dbl;
  T_STR txt;
  char *time1, *time2;
  int done=0;
  

  msg = data->msg;
  
  TipcMsgGetNumFields(msg,&n);
  TipcMsgSetCurrent(msg, 0);

  /* unique id */
  TipcMsgNextStr(msg, &txt);
  TutOut("%-19s ",txt);

  /* just print name for brief option */
  if(brief==1) {
    TutOut("\n");
    return;
  }

  /* username */
  TipcMsgNextStr(msg, &txt);
  TutOut("%-7s ", txt);

  /* nodename */
  TipcMsgNextStr(msg, &txt);
  TutOut("%-7s ", txt);

  /* pid */
  TipcMsgNextInt4(msg,&int4);
  TutOut("%-10d ",int4);

  /* start time */
  TipcMsgNextInt4(msg,&int4);
  len = strlen(ctime(&int4)) - 1;
  time1 = strdup (ctime(&int4));

  /* current time */
  TipcMsgNextInt4(msg,&int4);
  len = strlen(ctime(&int4)) - 1;
  time2 = strdup (ctime(&int4));

  /* cputime */
  TipcMsgNextReal4(msg,&flt);
  TutOut("%-8.2f ",flt);

  /* number of responses so far*/
  TipcMsgNextInt4(msg,&int4);
  TutOut("%-8d ",int4);

  /* check for server node (new format) */
  TipcMsgNextStr(msg, &txt);
  if(strcmp (txt,"end_system_data")!=0) {
    TutOut("%-7s ",txt);
    TipcMsgNextStr(msg, &txt);
    TutOut(" %s %98s", time1, time2); 
    if(all==1)print_opt_info(msg, n);
  } else {
    TutOut("%-7s ","unknown");
    TutOut(" %s %98s", time1, time2); 
    if(all==1)print_opt_info(msg, n);
  }

  /* end key */



  TutOut ("\n----------------------------------------------------------------------------------------------------\n\n");

}


/*------------------------------------------------------------------*/


void print_opt_info(T_IPC_MSG msg, int max) {

  int i;
  T_IPC_FT type;
  T_INT4 int4;
  T_REAL4 flt;
  T_REAL8 dbl;
  T_STR txt;
  
  /*   for (i = 10; i < max; i++) { */
  for (i = 10; i < max; i+=2) {

    TipcMsgNextStr(msg, &txt);
    if (strcmp (txt,"end_user_data")==0) {
      TutOut("\n");
      break;
    } else {
      TutOut("\n%50s  ",txt);
    }  


    TipcMsgNextType(msg,&type);
    
    switch(type) {
      
    case(T_IPC_FT_INT4):
      TipcMsgNextInt4(msg,&int4);
      TutOut("%-7d     ",int4);
      break;

    case(T_IPC_FT_REAL4):
      TipcMsgNextReal4(msg,&flt);
	TutOut("%-7.2f     ",flt);
      break;
    
    case(T_IPC_FT_REAL8):
      TipcMsgNextReal8(msg,&dbl);
      TutOut("%-7.2f     ",dbl);
      break;
    
    case(T_IPC_FT_STR):
      TipcMsgNextStr(msg, &txt);
      if(strcmp (txt,"end_user_data")==0){
	TutOut("\n");
	break;
      } else {
	TutOut("%s",txt);
      }
    }
    
  }

  TutOut("\n");

}


/*------------------------------------------------------------------*/


void decode_command_line(int argc, char **argv) {

  char *help ="\nusage:\n\n   ipc_testmon [-a application] [-d destination] "
    "[-p polltime] [-w waittime] [-all] [-once] [-brief]\n\n\n";
  int i=1;

  while(i<argc) {

    if(strncasecmp(argv[i],"-h",2)==0){
      printf("%s", help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-all",4)==0){
      all=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-brief",6)==0){
      brief=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-once",5)==0){
      once=1;
      i=i+1;
    }
    else if (argc!=(i+1)) {
      if (strncasecmp(argv[i],"-a",2)==0){
	application=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-d",2)==0){
	destination=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-p",5)==0){
	sscanf(argv[i+1],"%d",&polltime);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-w",5)==0){
	sscanf(argv[i+1],"%d",&waittime);
	i=i+2;
      }
      else {
	printf("Unknown command line arg: %s\n",argv[i]);
	i=i+1;
      }
    }
    else {
      printf("%s", help);
      exit(EXIT_SUCCESS);
    }
  }
}


/*-------------------------------------------------------------------*/


/* 
*  SIGALARM handler sends out status_poll request every polltime seconds
*/

void alarm_handler(int i) {

  T_IPC_MSG msg;

  /* create and send out status_poll control message */
  msg=TipcMsgCreate(TipcMtLookupByNum(T_MT_CONTROL));
  TipcMsgSetDest(msg,destination);
  TipcMsgAppendStr(msg,"status_poll");
  TipcSrvMsgSend(msg,TRUE);
  TipcSrvFlush();
  TipcMsgDestroy(msg);

  /* redeclare alarm signal handler */
  signal(SIGALRM,alarm_handler);

  /* reset alarm */
  if(once==0)alarm(polltime);

}


/*-----------------------------------------------------------------------*/


void mystatuspollcallback(T_IPC_MSG msg) {

  TipcMsgAppendStr(msg,"Poll time");
  TipcMsgAppendInt4(msg,polltime);


  if(brief==1)return;


  TutOut("\n\n******************************************* STATUS *************************************************\n");
  TutOut("\n                                               CPU   No.Times    Server         Start / Current");
  TutOut("\nId                   User    Node    pid       Time   Polled                        Time\n");  
  TutOut("----------------------------------------------------------------------------------------------------\n");


  return;
}


/*------------------------------------------------------------------*/

