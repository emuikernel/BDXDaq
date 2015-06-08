/*  sends data to RTserever */

 /* main include file variable intialization */
#define INIT 1

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <laser.h>
#include <time.h>
#include <sys/times.h>
#include <sys/param.h>
#include <stdlib.h>
#include <laser.h>
#include <bit_defs.h>
#include <parse.h>

/* for smartsockets */
#include <ssinclude.h>


static void T_ENTRY ProcessInfo(T_IPC_CONN conn, 
				T_IPC_CONN_PROCESS_CB_DATA data, 
				T_CB_ARG arg);
				
static void T_ENTRY Processbos(T_IPC_CONN conn, 
				T_IPC_CONN_PROCESS_CB_DATA data, 
				T_CB_ARG arg);
				
static void T_ENTRY ProcessStatus(T_IPC_CONN conn, 
				T_IPC_CONN_PROCESS_CB_DATA data, 
				T_CB_ARG arg);
		

int
main (int argc, char *argv[])
{
 int returncode;
 T_IPC_MT mt;  /* this will contian the RT message type */
 T_IPC_MT mt2;
 
/* call the init routine which connects to servers */
 returncode=ss_init("clasprod",0);
 printf(" status returned from init= %d \n", returncode );

 

 
  /* Setup callback to catch INFO messages */
  mt = TipcMtLookupByNum(T_MT_INFO);
  if(TipcSrvSubjectCbCreate("/EClaser/progress",NULL, ProcessInfo, NULL))
    {printf(" CB for EClaser/progress \n");}

  if( (mt2=TipcMtLookup("evt_bosbank")) == NULL) {printf(" message type not found \n ");return(901);}
 

  if(TipcSrvSubjectCbCreate(ECevent_subject,mt2, Processbos, NULL))
    {printf(" CB for event write \n");}

  if(TipcSrvSubjectCbCreate("/EClaser/status",NULL, ProcessStatus, NULL))
    {printf(" CB fo EClaser/status \n");}

 
  /* Start subscribing to the "/EClaser/progress" subject */
  TipcSrvSubjectSetSubscribe("/EClaser/progress", TRUE);

/* Start subscribing to the event to event stream subject */
  TipcSrvSubjectSetSubscribe(ECevent_subject, TRUE);


  /* Start subscribing to the "/EClaser/progress" subject */
  TipcSrvSubjectSetSubscribe("/EClaser/status", TRUE);


  /* Read and Process all incoming messages */
  TipcSrvMainLoop(T_TIMEOUT_FOREVER);


 TipcSrvDestroy(T_IPC_SRV_CONN_NONE);  
   
 exit;

}


/* ========================================================================= */
/*..ProcessInfo --- callback for processing INFO messages */

static void T_ENTRY
ProcessInfo(conn, data, arg)
     T_IPC_CONN                  conn;
     T_IPC_CONN_PROCESS_CB_DATA  data;
     T_CB_ARG                    arg;
{
  T_IPC_MSG msg = data->msg;
  T_STR MsgText;
  T_IPC_MT mt;
  T_STR name;
  T_INT4 mt_num;

   TipcMsgGetType(msg, &mt);  

   TipcMtGetNum(mt, &mt_num);
/* print out the name of the type of the message */
    if((mt_num != T_MT_INFO)) {
    TipcMtGetName(mt, &name);
    TutOut("/Eclaser/progress non-info message: type is <%s>", name);
    return;
    }

  TipcMsgSetCurrent(msg, 0);
  TipcMsgNextStr(msg, &MsgText);
  TutOut("INFO message /EClaser/progress  ***************\n %s \n", MsgText);
 

} /* ProcessInfo */


/* ========================================================================= */
/* ========================================================================= */
/*..ProcessInfo --- callback for processing INFO messages */

static void T_ENTRY 
Processbos(conn, data, arg)
     T_IPC_CONN                  conn;
     T_IPC_CONN_PROCESS_CB_DATA  data;
     T_CB_ARG                    arg;
{
  T_IPC_MSG msg = data->msg;
  T_STR MsgText1,MsgText2;
  T_INT4 int1,int2,int3,int4,buf[100];
  int iii;
  T_INT4 *pnt1,**pnt2;

  T_IPC_MT mt1, mt2;
  T_STR name;
  T_INT4 mt_num1, mt_num2;

   mt1 = TipcMtLookup("evt_bosbank");
   TipcMsgGetType(msg, &mt2);  
   TipcMtGetNum(mt1, &mt_num1);
   TipcMtGetNum(mt2, &mt_num2);
/* print out the name of the type of the message */
    if(mt_num1 != mt_num2) {
    TipcMtGetName(mt2, &name);
    TutOut("event received non-evt_bosbank message: type is <%s>\n", name);
    return;
  }

  TipcMsgSetCurrent(msg, 0);
 

  /* define the pointer for the array data */
  pnt2=&pnt1;


  /*
  TipcMsgNextStr(msg, &MsgText);
  TutOut("Text from INFO message = %s\n", MsgText);
  */

  if( !TipcMsgNextStr(msg,&MsgText1)     ){printf(" 1st read string fail "); return;}
  if( strstr(MsgText1,"LASR") ==  NULL) {printf(" non laser event=%s\n ",MsgText1); return;}
  TutOut(" event to tape message *******\n %s\n", MsgText1);
  /*
 if( !TipcMsgNextInt4(msg,&int1)        )   {printf(" 2nd read int4 fail ");}
  TutOut(" 1st int = %d\n",int1);
 if( !TipcMsgNextStr(msg,&MsgText2)     )   {printf(" 3rd read string fail ");}
  TutOut(" 2nd str = %s\n",MsgText2);
 if( !TipcMsgNextInt4(msg, &int2)        )   {printf(" 4th read int4 fail ");}
  TutOut(" 2nd int = %d\n",int2);
 if( !TipcMsgNextInt4(msg, &int3)  )   {printf(" 5th read int4 fail ");}
  TutOut(" 3rd int = %d\n",int3);
 if( !TipcMsgNextInt4(msg, &int4))   {printf(" 6th read int4 fail ");}
  TutOut(" 4th int = %d\n",int4);
 if( !TipcMsgNextInt4Array(msg, pnt2, &int4))   {printf(" 7th get array fail ");}


 iii=0;
  while(iii < int4){
    buf[iii]=*(pnt1+iii);
    printf(" buf[%d] = %d \n",iii,buf[iii]);
    ++iii;}
 */

} /* ProcessInfo */


/* ========================================================================= */
/* ========================================================================= */
/* ========================================================================= */
/*..ProcessInfo --- callback for processing INFO messages */

static void T_ENTRY 
ProcessStatus(conn, data, arg)
     T_IPC_CONN                  conn;
     T_IPC_CONN_PROCESS_CB_DATA  data;
     T_CB_ARG                    arg;
{
  T_IPC_MSG msg = data->msg;
  T_STR MsgText1,MsgText2;
  T_INT4 int1,int2,int3,int4,buf[100];
  int iii;



  T_IPC_MT mt1, mt2;
  T_STR name;

  T_INT4 mt_num1, mt_num2;

  /* mt1 = TipcMtLookup("evt_bosbank"); */
   mt1 = TipcMtLookupByNum(T_MT_INFO);
   TipcMsgGetType(msg, &mt2);  
   TipcMtGetNum(mt1, &mt_num1);
   TipcMtGetNum(mt2, &mt_num2);
/* print out the name of the type of the message */
    if((mt_num1 != mt_num2)) {
    TipcMtGetName(mt2, &name);
    TutOut("EClaser/status non-info message: type is <%s>\n", name);
    return;
  }


  TipcMsgSetCurrent(msg, 0);
 


  /*
  TipcMsgNextStr(msg, &MsgText);
  TutOut("Text from INFO message = %s\n", MsgText);
  */
 TutOut(" status message received ");
  /* 
 iii=0;
  while(iii < 1000){
    if( !TipcMsgNextStr(msg,&MsgText1)     ){printf(" read string fail \n "); return;}
    TutOut("Text %d string = %s\n",iii, MsgText1);
    if( !TipcMsgNextInt4(msg,&int1)        )   {printf(" read int4 fail \n ");return;}
    TutOut(" int %d = %d\n",iii,int1);
    ++iii;
  }
  */

} /* ProcessInfo */

