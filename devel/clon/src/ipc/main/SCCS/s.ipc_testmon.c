h44792
s 00001/00001/00360
d D 1.16 98/11/15 22:54:34 wolin 17 16
c No unique name
c 
e
s 00026/00003/00335
d D 1.15 98/11/15 21:09:09 clasrun 16 15
c Added -grief option
c 
e
s 00017/00020/00321
d D 1.14 98/06/18 13:45:52 wolin 15 14
c all not default for once
c 
e
s 00006/00008/00335
d D 1.13 98/05/18 15:48:19 wolin 14 13
c Typo
c 
e
s 00056/00049/00287
d D 1.12 98/05/18 15:31:24 wolin 13 12
c Added server node
c 
e
s 00001/00001/00335
d D 1.11 98/05/12 11:07:59 wolin 12 11
c Minor mods
c 
e
s 00008/00002/00328
d D 1.10 98/05/12 11:04:55 wolin 11 10
c Added proper exit, -w waittime
c 
e
s 00055/00032/00275
d D 1.9 98/03/20 17:47:14 wolin 10 9
c Added -once
c 
e
s 00009/00009/00298
d D 1.8 96/10/11 09:37:43 wolin 9 8
c Minor formatting mods
e
s 00000/00000/00307
d D 1.7 96/09/11 13:14:54 wolin 8 7
c Bug in codemgr
e
s 00056/00073/00251
d D 1.6 96/09/10 16:52:50 wolin 7 6
c Now works with double, strings, etc.
e
s 00005/00011/00319
d D 1.5 96/09/05 13:22:54 wolin 6 5
c New scheme for init of standard message types
e
s 00018/00013/00312
d D 1.4 96/08/22 11:10:17 aswin 5 4
c minor output changes
e
s 00002/00002/00323
d D 1.3 96/08/19 16:51:24 aswin 4 3
c fixed line format
e
s 00143/00028/00182
d D 1.2 96/08/09 13:53:34 aswin 3 1
c Change output format.  Added additional infomation option.
e
s 00000/00000/00000
d R 1.2 96/07/24 17:13:21 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 ipcutil/s/ipc_testmon.c
c Name history : 1 0 s/ipc_testmon.c
e
s 00210/00000/00000
d D 1.1 96/07/24 17:13:20 wolin 1 0
c Ascii version of ipc_monitor receives and displays STATUS_POLL_RESULT messages
e
u
U
f e 0
t
T
I 1
/*
*  ipc_testmon
* 
*  Ascii test version of ipc_monitor
*
D 7
*  ejw, W&M, 16-jul-96
E 7
I 7
*  ejw, W&M, 9-sep-96
E 7
*
*/

D 7
/*  stilltodo:
*
*       format output better
*
*/
E 7

#define _POSIX_SOURCE 1
#define __EXTENSIONS__

#include <rtworks/ipc.h>
D 6
#include <clas_ipc_mt.h>
E 6
#include <stdlib.h>
#include <signal.h>

I 3
D 7
typedef int bool;   const bool true = 1;    const bool false = 0;   
E 7

I 7
typedef int bool;
const bool true = 1;
const bool false = 0;   

E 7
E 3
D 10
char *application = "clastest";
int polltime=0;
I 3
int opt = 0;
E 10
I 10
static char *application = "clastest";
static char *destination = NULL;
static int once          = 0;
static int polltime      = 0;
I 11
static int waittime      = 5;
E 11
D 15
static int opt           = 0;
E 15
I 15
static int all           = 0;
I 16
static int brief         = 0;
E 16
E 15
E 10
E 3

I 10

E 10
void print_status_result(
     T_IPC_CONN conn,
     T_IPC_CONN_PROCESS_CB_DATA data,
     T_CB_ARG arg);
void mystatuspollcallback(T_IPC_MSG msg);
void decode_command_line(int argc, char **argv);
void alarm_handler(int i);
I 3
D 7
void opt_printout (T_IPC_MSG msg, int max);
E 7
E 3
D 10

E 10
I 7
void print_opt_info(T_IPC_MSG msg, int max);
I 16
void ipc_output_dummy(T_STR str, va_list arg);
E 16


E 7
D 3

E 3
/*------------------------------------------------------------------*/


D 15
int main(argc, argv)
D 10
int argc;
char **argv;
E 10
I 10
     int argc;
     char **argv;
E 10
{
E 15
I 15
int main(int argc, char **argv) {

E 15
  char *mon_dg;
D 6
  T_IPC_MT mt;
E 6

D 10
/* decode command line */
  decode_command_line(argc,argv);
E 10

D 10
/* set application */
E 10
I 10
  /* decode command line */
  decode_command_line(argc,argv);
D 15
  if(once==1) opt=1;
E 15
  
  /* set application */
E 10
  ipc_set_application(application);
D 10

/*  connect to server */
E 10
I 10
  
  /* set destination */
  if(destination==NULL)destination=strdup(application);
  
I 16
  /* inhibit boilerplate for brief mode */
  if(brief==1)TutSetOutputFunc(ipc_output_dummy);

E 16
  /*  connect to server */
E 10
D 17
  ipc_init("ipc_testmon","test ipc monitor");
E 17
I 17
  ipc_init("","ipc test monitor");
E 17
D 10

D 6
/*  define callback to add user information to status_poll_result message, etc. */
E 6
I 6
/*  define callback to add user information to status_poll_result message */
E 10
I 10
  
  /*  define callback to add user information to status_poll_result message */
E 10
E 6
  ipc_set_user_status_poll_callback(mystatuspollcallback);
D 10

D 6
/*  define callback to print status_poll_result messages */
  mt=TipcMtLookupByNum(STATUS_POLL_RESULT_MTNUM);
  TipcSrvProcessCbCreate(mt,print_status_result,NULL);

/* receive status_poll_result messages */
E 6
I 6
/* receive status_poll_result message group */
E 10
I 10
  
  /* receive status_poll_result message group */
E 10
E 6
  ipc_get_status_poll_group(&mon_dg);
  TipcSrvDgSetRecv(mon_dg,TRUE);
D 10

I 6
/*  define callback to handle (i.e. print) status_poll_result messages */
E 10
I 10
  
  /*  define callback to handle (i.e. print) status_poll_result messages */
E 10
  TipcSrvProcessCbCreate(TipcMtLookup("status_poll_result"),print_status_result,NULL);
E 6
D 10

/* set alarm if polltime > 0 */
  if(polltime>0){

E 10
I 10
  
  /* set alarm if polltime > 0 */
  if(polltime>0) {
    
E 10
    /* declare SIGALRM handler */
    signal(SIGALRM,alarm_handler);
D 10
      
E 10
I 10
    
E 10
    /* set alarm */
    alarm(polltime);
D 10
  }
E 10

D 10
/* process messages */
  for (;;){
    TipcSrvMainLoop(T_TIMEOUT_FOREVER);
I 3

E 10
I 10
  } else if (once==1) {
    T_IPC_MSG msg=TipcMsgCreate(TipcMtLookupByNum(T_MT_CONTROL));
    TipcMsgSetDest(msg,destination);
    TipcMsgAppendStr(msg,"status_poll");
    TipcSrvMsgSend(msg,TRUE);
    TipcSrvFlush();
    TipcMsgDestroy(msg);
E 10
E 3
  }
D 10

E 10
I 10
  
  /* process messages */
I 16
  if(brief==1)TutSetOutputFunc(NULL);
E 16
  for (;;) {
D 11
    TipcSrvMainLoop(5.0);
E 11
I 11
    TipcSrvMainLoop((double)waittime);
E 11
    if(once==1)break;
  }
I 16
  if(brief==1)TutSetOutputFunc(ipc_output_dummy);
E 16
  
I 11
  exit(EXIT_SUCCESS);
E 11
E 10
}


/*------------------------------------------------------------------*/


void print_status_result(
D 3
     T_IPC_CONN conn,
E 3
I 3
			 T_IPC_CONN conn,
E 3
     T_IPC_CONN_PROCESS_CB_DATA data,
D 3
     T_CB_ARG arg)
E 3
I 3
D 15
			 T_CB_ARG arg)
E 3
{
E 15
I 15
			 T_CB_ARG arg) {
E 15
D 3

E 3
I 3
  
E 3
  T_IPC_MSG msg;
D 3
  int i;
E 3
I 3
D 13
  int i, len;
E 13
I 13
  int len;
E 13
E 3
  T_IPC_FT type;
  T_INT4 n;
  T_INT4 int4;
  T_REAL4 flt;
I 7
  T_REAL8 dbl;
E 7
  T_STR txt;
D 3


E 3
I 3
  char *time1, *time2;
I 13
  int done=0;
E 13
  
D 7
  
E 7
I 7

E 7
E 3
  msg = data->msg;
D 3

E 3
I 3
  
E 3
  TipcMsgGetNumFields(msg,&n);
  TipcMsgSetCurrent(msg, 0);
I 14

  /* unique id */
E 14
D 3
  
  for (i=0; i<n; i++){
E 3
I 3
  TipcMsgNextStr(msg, &txt);
D 5
  TutOut("%-15s",txt);
E 5
I 5
D 9
  TutOut("%-12s ",txt);
E 9
I 9
  TutOut("%-19s ",txt);
E 9
E 5

I 16
  /* just print name for brief option */
  if(brief==1) {
    TutOut("\n");
    return;
  }

E 16
D 13
  for (i=1; i<9; i++){
E 3
    
    TipcMsgNextType(msg,&type);
    
    switch(type) {
      
    case(T_IPC_FT_INT4):
      TipcMsgNextInt4(msg,&int4);
D 3
      TutOut("%10d  ",int4);
E 3
I 3
      if (i==4) {
	len = strlen(ctime(&int4)) - 1;
D 7
       	/*strncpy (time, ctime(&int4), len);*/
E 7
	time1 = strdup (ctime(&int4));
      }
      else if (i==5) {
	len = strlen(ctime(&int4)) - 1;
D 7
       	/*strncpy (time2, ctime(&int4), len);*/
E 7
	time2 = strdup (ctime(&int4));
      }	
I 5
      else if (i==7)
	TutOut("%-8d ", int4);
E 5
      else
D 5
	TutOut("%10d     ",int4);
E 5
I 5
	TutOut("%-10d ",int4);
E 5
E 3
      break;
      
    case(T_IPC_FT_REAL4):
      TipcMsgNextReal4(msg,&flt);
D 3
      TutOut("%8.2f  ",flt);
E 3
I 3
D 5
      TutOut("%10.2f     ",flt);
E 5
I 5
      TutOut("%-8.2f ",flt);
E 5
E 3
      break;
      
I 7
    case(T_IPC_FT_REAL8):
      TipcMsgNextReal8(msg,&dbl);
      TutOut("%-8.2f ",dbl);
      break;
      
E 7
    case(T_IPC_FT_STR):
      TipcMsgNextStr(msg, &txt);
D 3
      TutOut("%-15s  ",txt);
E 3
I 3
D 7
      if (strcmp (txt,"end_system_data")==0) {
E 7
I 7
      if(strcmp (txt,"end_system_data")==0) {
E 7
D 5
	TutOut("     %-109s %s", time1, time2); 
E 5
I 5
D 9
	TutOut(" %s %83s", time1, time2); 
E 9
I 9
	TutOut(" %s %90s", time1, time2); 
E 9
E 5
D 7
	if (opt == 1) 
	  opt_printout (msg, n);
E 7
I 7
D 10
	if(opt == 1)print_opt_info(msg, n);
E 10
I 10
	if(opt==1)print_opt_info(msg, n);
E 10
E 7
      }
D 5
      else  
	TutOut("%-10s",txt);
E 5
I 5
      else
	TutOut("%-7s ", txt);
E 5
E 3
      break;
    }
D 3
    
E 3
I 3
D 7

E 7
I 7
    
E 13
I 13
D 14

  /* unique_datagroup */
  TipcMsgNextStr(msg, &txt);
  TutOut("%-7s ", txt);

E 14
  /* username */
  TipcMsgNextStr(msg, &txt);
  TutOut("%-7s ", txt);

  /* nodename */
  TipcMsgNextStr(msg, &txt);
  TutOut("%-7s ", txt);

  /* pid */
  TipcMsgNextInt4(msg,&int4);
I 14
  TutOut("%-10d ",int4);
E 14

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
D 14
  TutOut("%-8d ", int4);
E 14
I 14
  TutOut("%-8.2f ",flt);
E 14

  /* number of responses so far*/
  TipcMsgNextInt4(msg,&int4);
D 14
  TutOut("%-10d ",int4);
E 14
I 14
  TutOut("%-8d ",int4);
E 14

  /* check for server node (new format) */
  TipcMsgNextStr(msg, &txt);
  if(strcmp (txt,"end_system_data")!=0) {
    TutOut("%-7s ",txt);
    TipcMsgNextStr(msg, &txt);
    TutOut(" %s %98s", time1, time2); 
D 15
    if(opt==1)print_opt_info(msg, n);
E 15
I 15
    if(all==1)print_opt_info(msg, n);
E 15
  } else {
    TutOut("%-7s ","unknown");
    TutOut(" %s %98s", time1, time2); 
D 15
    if(opt==1)print_opt_info(msg, n);
E 15
I 15
    if(all==1)print_opt_info(msg, n);
E 15
E 13
E 7
E 3
  }
I 7

E 7
I 3
D 5
  TutOut ("-----------------------------------------------------------------------------------------------------------------\n");
E 5
I 5
D 9
  TutOut ("-------------------------------------------------------------------------------------\n");
E 9
I 9
D 12
  TutOut ("--------------------------------------------------------------------------------------------\n");
E 12
I 12
D 13
  TutOut ("\n--------------------------------------------------------------------------------------------\n\n");
E 13
I 13
  /* end key */
E 13
E 12
E 9
E 5
D 7
  
  
E 7
E 3

I 13


  TutOut ("\n----------------------------------------------------------------------------------------------------\n\n");

E 13
D 3
  TutOut("\n");

E 3
}


/*------------------------------------------------------------------*/


D 7
void mystatuspollcallback(T_IPC_MSG msg)
E 7
I 7
D 15
void print_opt_info(T_IPC_MSG msg, int max)
E 7
{
E 15
I 15
void print_opt_info(T_IPC_MSG msg, int max) {

E 15
D 3
  TutOut("\n\n****************** status_poll **********************\n");

E 3
I 3
D 5
  TutOut("\n\n************************************************ STATUS ********************************************************\n");
  TutOut("\n                                                                      No.Times           Start / Current");
  TutOut("\nId              User      Node            pid         CPU Time         Polled                Time\n");  
  TutOut ("-----------------------------------------------------------------------------------------------------------------\n");
E 5
I 5
D 7
  TutOut("\n\n*********************************** STATUS ******************************************\n");
  TutOut("\n                                        CPU   No.Times           Start / Current");
  TutOut("\nId            User    Node    pid       Time   Polled                Time\n");  
  TutOut ("-------------------------------------------------------------------------------------\n");
E 5
E 3
  TipcMsgAppendStr(msg,"Poll time");
  TipcMsgAppendInt4(msg,polltime);

}

I 3
/*------------------------------------------------------------------
void opt1_printout (T_IPC_MSG msg, int max)
{
  T_IPC_FT type;
E 7
  int i;
D 7
  T_INT4 int4;
  T_REAL4 flt;
  T_STR txt;
E 3

I 3
  TutOut("\n");

  for (i = 1; i < max-9; i ++) {
       TipcMsgNextType(msg,&type);
    
    switch(type) {
      
    case(T_IPC_FT_INT4):
      TipcMsgNextInt4(msg,&int4);
      TutOut("%4d     ",int4);
      if ((i % 6) == 0)   TutOut("\n");
      break;
    case(T_IPC_FT_STR):
      TipcMsgNextStr(msg, &txt);
      if (i==33) TutOut("\n");
      if ((i==10)||(i==12)||(i==9)||(i==13)||(i==14)||(i==30)||(i==29)) break; 
      else TutOut("%-28s  ",txt);
      break;
    }
  }
  TutOut("\n");
}
*/

E 3
/*------------------------------------------------------------------*/
I 3
void opt_printout (T_IPC_MSG msg, int max)
{
E 7
  T_IPC_FT type;
D 7
  int i;
E 7
  T_INT4 int4;
  T_REAL4 flt;
I 7
  T_REAL8 dbl;
E 7
  T_STR txt;
  
D 7
  for (i = 10; i < max; i ++) {
E 7
I 7
  /*   for (i = 10; i < max; i++) { */
  for (i = 10; i < max; i+=2) {

E 7
    TipcMsgNextStr(msg, &txt);
    if (strcmp (txt,"end_user_data")==0) {
      TutOut("\n");
      break;
D 7
    }
    else  TutOut("\n%50s   ",txt);
  
E 7
I 7
    } else {
      TutOut("\n%50s  ",txt);
    }  


E 7
    TipcMsgNextType(msg,&type);
    
    switch(type) {
      
    case(T_IPC_FT_INT4):
      TipcMsgNextInt4(msg,&int4);
D 4
      TutOut("%-50d     ",int4);
E 4
I 4
      TutOut("%-7d     ",int4);
E 4
      break;
E 3

I 3
    case(T_IPC_FT_REAL4):
      TipcMsgNextReal4(msg,&flt);
D 4
      TutOut("%47.2f     ",flt);
E 4
I 4
D 5
      TutOut("%7.2f     ",flt);
E 5
I 5
	TutOut("%-7.2f     ",flt);
E 5
E 4
      break;
    
I 7
    case(T_IPC_FT_REAL8):
      TipcMsgNextReal8(msg,&dbl);
      TutOut("%-7.2f     ",dbl);
      break;
    
E 7
    case(T_IPC_FT_STR):
      TipcMsgNextStr(msg, &txt);
D 7
      if (strcmp (txt,"end_user_data")==0) {
E 7
I 7
      if(strcmp (txt,"end_user_data")==0){
E 7
	TutOut("\n");
	break;
I 7
      } else {
	TutOut("%s",txt);
E 7
      }
D 5
      else  TutOut("%50s  ",txt);
E 5
I 5
D 7
      else if ((i%2)==0) 
	TutOut ("%-50s  ",txt);
      else  
	TutOut("%50s  ",txt);
E 5
      break;
E 7
    }
I 7
    
E 7
  }
I 7

E 7
  TutOut("\n");
I 7

E 7
}
E 3

I 3

/*------------------------------------------------------------------*/


E 3
D 15
void decode_command_line(int argc, char **argv)
{
D 3

E 3
I 3
D 9
  char *help ="\nusage:\n\n   ipc_testmon [-a application] [-poll time] [-all]\n\n\n";
E 9
I 9
D 10
  char *help ="\nusage:\n\n   ipc_testmon [-a application] [-p time] [-all]\n\n\n";
E 10
I 10
D 11
  char *help ="\nusage:\n\n   ipc_testmon [-a application] [-d destination] [-p time] [-all] [-once]\n\n\n";
E 11
I 11
  char *help ="\nusage:\n\n   ipc_testmon [-a application] [-d destination] [-p polltime] [-w waittime] [-all] [-once]\n\n\n";
E 15
I 15
void decode_command_line(int argc, char **argv) {

  char *help ="\nusage:\n\n   ipc_testmon [-a application] [-d destination] "
D 16
    "[-p polltime] [-w waittime] [-all] [-once]\n\n\n";
E 16
I 16
    "[-p polltime] [-w waittime] [-all] [-once] [-brief]\n\n\n";
E 16
E 15
E 11
E 10
E 9
E 3
  int i=1;
I 3

E 3
  while(i<argc) {

    if(strncasecmp(argv[i],"-h",2)==0){
D 3
      printf("%s","\nusage:\n\n   ipc_testmon [-a application] [-poll time]\n\n\n");
E 3
I 3
      printf("%s", help);
E 3
      exit(EXIT_SUCCESS);
    }
D 3
    else if (strncasecmp(argv[i],"-a",2)==0){
      application=strdup(argv[i+1]);
      i=i+2;
E 3
I 3
    else if (strncasecmp(argv[i],"-all",4)==0){
D 15
      opt=1;
E 15
I 15
      all=1;
E 15
      i=i+1;
E 3
    }
I 16
    else if (strncasecmp(argv[i],"-brief",6)==0){
      brief=1;
      i=i+1;
    }
E 16
I 10
    else if (strncasecmp(argv[i],"-once",5)==0){
      once=1;
      i=i+1;
    }
E 10
D 3
    else if (strncasecmp(argv[i],"-poll",5)==0){
      sscanf(argv[i+1],"%d",&polltime);
      i=i+2;
E 3
I 3
    else if (argc!=(i+1)) {
      if (strncasecmp(argv[i],"-a",2)==0){
	application=strdup(argv[i+1]);
	i=i+2;
      }
I 10
      else if (strncasecmp(argv[i],"-d",2)==0){
	destination=strdup(argv[i+1]);
	i=i+2;
      }
E 10
D 9
      else if (strncasecmp(argv[i],"-poll",5)==0){
E 9
I 9
      else if (strncasecmp(argv[i],"-p",5)==0){
E 9
	sscanf(argv[i+1],"%d",&polltime);
	i=i+2;
      }
I 11
      else if (strncasecmp(argv[i],"-w",5)==0){
	sscanf(argv[i+1],"%d",&waittime);
	i=i+2;
      }
E 11
      else {
	printf("Unknown command line arg: %s\n",argv[i]);
	i=i+1;
      }
E 3
    }
    else {
D 3
      printf("Unknown command line arg: %s\n",argv[i]);
      i=i+1;
E 3
I 3
      printf("%s", help);
      exit(EXIT_SUCCESS);
E 3
    }
  }
}

I 7

E 7
/*-------------------------------------------------------------------*/

I 7

E 7
/* 
*  SIGALARM handler sends out status_poll request every polltime seconds
*/

D 15
void alarm_handler(int i)
{
E 15
I 15
void alarm_handler(int i) {
E 15

D 6
  T_IPC_MT mt;
E 6
  T_IPC_MSG msg;

  /* create and send out status_poll control message */
D 6
  mt=TipcMtLookupByNum(T_MT_CONTROL);
  msg=TipcMsgCreate(mt);
E 6
I 6
  msg=TipcMsgCreate(TipcMtLookupByNum(T_MT_CONTROL));
E 6
D 10
  TipcMsgSetDest(msg,application);
E 10
I 10
  TipcMsgSetDest(msg,destination);
E 10
  TipcMsgAppendStr(msg,"status_poll");
  TipcSrvMsgSend(msg,TRUE);
  TipcSrvFlush();
  TipcMsgDestroy(msg);

  /* redeclare alarm signal handler */
  signal(SIGALRM,alarm_handler);

  /* reset alarm */
D 10
  alarm(polltime);
E 10
I 10
  if(once==0)alarm(polltime);
E 10

}


/*-----------------------------------------------------------------------*/

I 7

D 15
void mystatuspollcallback(T_IPC_MSG msg)
{
E 15
I 15
void mystatuspollcallback(T_IPC_MSG msg) {

I 16
  TipcMsgAppendStr(msg,"Poll time");
  TipcMsgAppendInt4(msg,polltime);
E 16

I 16

  if(brief==1)return;


E 16
E 15
D 9
  TutOut("\n\n*********************************** STATUS ******************************************\n");
  TutOut("\n                                        CPU   No.Times           Start / Current");
  TutOut("\nId            User    Node    pid       Time   Polled                Time\n");  
  TutOut ("-------------------------------------------------------------------------------------\n");
E 9
I 9
D 13
  TutOut("\n\n*************************************** STATUS *********************************************\n");
  TutOut("\n                                               CPU   No.Times           Start / Current");
  TutOut("\nId                   User    Node    pid       Time   Polled                Time\n");  
  TutOut("--------------------------------------------------------------------------------------------\n");
E 13
I 13
  TutOut("\n\n******************************************* STATUS *************************************************\n");
D 14
  TutOut("\n                                               CPU   No.Times     Server        Start / Current");
E 14
I 14
  TutOut("\n                                               CPU   No.Times    Server         Start / Current");
E 14
  TutOut("\nId                   User    Node    pid       Time   Polled                        Time\n");  
  TutOut("----------------------------------------------------------------------------------------------------\n");
E 13
E 9
D 16
  TipcMsgAppendStr(msg,"Poll time");
  TipcMsgAppendInt4(msg,polltime);
E 16

I 16

  return;
E 16
}


/*------------------------------------------------------------------*/
E 7

E 1
