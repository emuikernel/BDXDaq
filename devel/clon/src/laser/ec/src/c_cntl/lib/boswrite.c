
/* for smartsockets */
#include <ssinclude.h>

int boswrite( T_INT4 buf[], T_INT4 ecrows){

/* insert bos bank */
  int n;
  T_IPC_MSG ECmsg;
  T_STR     msg_name;
  T_IPC_MT  msg_type;
 

/* TipcMsg msg("evt_bosbank"); */

  int m,l;
  printf(" starting boswrite \n");
  

  n=0;
  while(n<ecrows*4){printf(" buf[%d] = %d \n",n,buf[n]);++n;}

  msg_type = TipcMtLookup("evt_bosbank");

  if(msg_type == NULL) 
    {printf(" message type not found \n "); return(901);}
  else
    {printf(" message type found\n ");}

 
  ECmsg=TipcMsgCreate(msg_type);
  if(ECmsg == NULL) 
    {printf(" message not tcreated \n "); return(902);}
  else
    {printf(" message created \n ");}

 if( !TipcMsgSetDest(ECmsg,ECevent_subject))
   {printf("destination (subject) not set \n "); return(902);}
  else
    {printf("destination= %s \n ", ECevent_subject);}
 
 if( !TipcMsgAppendStr(ECmsg,"LASR")     )  {printf(" 1st append string fail ");}
 if( !TipcMsgAppendInt4(ECmsg, 0)        )  {printf(" 2nd append int4 fail ");}
 if( !TipcMsgAppendStr(ECmsg,"(4I)")     )  {printf(" 3rd append string fail ");}
 if( !TipcMsgAppendInt4(ECmsg, 4)        )  {printf(" 4th append int4 fail ");}
 if( !TipcMsgAppendInt4(ECmsg, ecrows)  )   {printf(" 5th append int4 fail ");}
 if( !TipcMsgAppendInt4(ECmsg, 4*ecrows))   {printf(" 6th append int4 fail ");}
 if( !TipcMsgAppendInt4Array(ECmsg,buf,4*ecrows)){printf(" 7th append size of array fail ");}
 

 TipcSrvMsgSend(ECmsg, TRUE);
 TipcMsgDestroy(ECmsg);
 TipcSrvFlush(); 
 return(1);
}

