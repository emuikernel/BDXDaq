h33441
s 00002/00002/00061
d D 1.5 96/10/11 10:27:05 wolin 6 5
c Testing null uniq_dgrp
e
s 00005/00000/00058
d D 1.4 96/10/11 09:59:03 wolin 5 4
c Minor mods
e
s 00004/00000/00054
d D 1.3 96/10/07 17:27:26 wolin 4 3
c Added test of tclipc_init
e
s 00002/00004/00052
d D 1.2 96/09/26 15:42:27 wolin 3 1
c Updated to conform to latest ipc lib
e
s 00000/00000/00000
d R 1.2 96/07/23 21:05:14 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/test/ipctest.c
c Name history : 1 0 test/ipctest.c
e
s 00056/00000/00000
d D 1.1 96/07/23 21:05:13 muguira 1 0
c 
e
u
U
f e 0
t
T
I 1
/***  for testing only ***/

#include <clas_ipc.h>
#include <stdlib.h>

void myquitcallback(int sig)
{
  TutOut("myquitcallback called with signal %d\n",sig);
  ipc_close();
  exit(0);
}

void mystatuspollcallback(T_IPC_MSG msg)
{
  TipcMsgAppendStr(msg,"Current");
  TipcMsgAppendReal4(msg,50.0*((float)rand())/((float)RAND_MAX));
  TipcMsgAppendStr(msg,"Voltage");
  TipcMsgAppendReal4(msg,100.0*((float)rand())/((float)RAND_MAX));
  TipcMsgAppendStr(msg,"Calibration file");
  TipcMsgAppendStr(msg,"/clas/u2/data/calib.dat");
}


main()
{
  int err;
  char *name;
  T_IPC_MT mt;
  char *monitor_group;
I 5
  char *uniq_dgrp;
  T_OPTION opt;
E 5


D 3
/*    err=ipc_set_quit_callback(myquitcallback); 
  if(err!=0)ipc_perror(err);   */
E 3
I 3
  err=ipc_set_quit_callback(myquitcallback); 
  if(err!=0)ipc_perror(err);
E 3

  err=ipc_set_user_status_poll_callback(mystatuspollcallback);

  err=ipc_set_alarm_poll_time(1);
  if(err!=0)ipc_perror(err);

D 6
  err=ipc_init("ipctest","ipc system test");
E 6
I 6
  err=ipc_init("","ipc system test");
E 6
  if(err!=0)ipc_perror(err);

I 5
  opt=TutOptionLookup("Unique_Datagroup");
D 6
  err=TutOptionGetEnum(opt,&uniq_dgrp));
E 6
I 6
  err=TutOptionGetEnum(opt,&uniq_dgrp);
E 6

E 5
I 4
  err=tclipc_init(NULL);

E 4
  err=ipc_get_application(&name);

I 4
  err=ipc_get_unique_datagroup(&name);

E 4
  err=ipc_get_status_poll_group(&monitor_group);

D 3
  err=ipc_get_status_poll_result_mt(&mt);
E 3

D 3

E 3
  for(;;){
    TutSleep(1000.0);
  }
}



E 1
