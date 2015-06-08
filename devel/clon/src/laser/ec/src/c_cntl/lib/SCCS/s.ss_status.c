h51675
s 00000/00000/00000
d R 1.2 00/09/21 14:12:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/lib/ss_status.c
e
s 00057/00000/00000
d D 1.1 00/09/21 14:12:41 wolin 1 0
c date and time created 00/09/21 14:12:41 by wolin
e
u
U
f e 0
t
T
I 1

/* for smartsockets */
#include <ssinclude.h>

int ss_status(void){

  T_INT4 n;
  T_IPC_MSG ECmsg;
  T_STR     msg_name;
  T_IPC_MT  msg_type;

 

/* TipcMsg msg("evt_bosbank"); */


  printf(" starting status message with %d pairs \n", nopairs);
  

  n=0;
 
  while(n<nopairs){
    printf(" id=%s, val= %d \n",status_id[n][0],status_val[n]); ++n;}
  
  /*  msg_type = TipcMtLookup("evt_bosbank"); */

  msg_type = TipcMtLookupByNum(T_MT_INFO);
  if(msg_type == NULL) 
    {printf(" message type not found \n "); return(901);}
  else
    { /* printf(" message type found\n ");*/ }

 
  ECmsg=TipcMsgCreate(msg_type);
  if(ECmsg == NULL) 
    {printf(" message not tcreated \n "); return(902);}
  else
    { /* printf(" message created \n ");*/ }

 if( !TipcMsgSetDest(ECmsg,"/EClaser/status"))
   {printf("destination (subject) not set \n "); return(902);}
  else
    {/* printf("destination= %s \n ", "/EClaser/status");*/ }

 n=0;
 while(n<nopairs){ 
   if( !TipcMsgAppendStr(ECmsg,status_id[n][0])     )  {printf(" append string fail ");}
   if( !TipcMsgAppendInt4(ECmsg,status_val[n])      )  {printf(" append int4 fail ");}
   ++n;
 }

 TipcSrvMsgSend(ECmsg, TRUE);
 TipcMsgDestroy(ECmsg);

 return(1);
}

E 1
