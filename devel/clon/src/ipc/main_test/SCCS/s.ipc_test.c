h43955
s 00000/00000/00000
d R 1.2 96/07/23 21:05:13 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/test/ipc_test.c
c Name history : 1 0 test/ipc_test.c
e
s 00056/00000/00000
d D 1.1 96/07/23 21:05:12 muguira 1 0
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


/*    err=ipc_set_quit_callback(myquitcallback); 
  if(err!=0)ipc_perror(err);   */

  err=ipc_set_user_status_poll_callback(mystatuspollcallback);

  err=ipc_set_alarm_poll_time(1);
  if(err!=0)ipc_perror(err);

  err=ipc_init("ipc_test_hp","ipc system test");
  if(err!=0)ipc_perror(err);

  err=ipc_get_application(&name);

  err=ipc_get_status_poll_group(&monitor_group);

  err=ipc_get_status_poll_result_mt(&mt);


  for(;;){
    TutSleep(1000.0);
  }
}



E 1
