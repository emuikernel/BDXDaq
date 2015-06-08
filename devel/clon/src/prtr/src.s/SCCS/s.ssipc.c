h26212
s 00190/00000/00000
d D 1.1 07/06/28 23:25:06 boiarino 1 0
c date and time created 07/06/28 23:25:06 by boiarino
e
u
U
f e 0
t
T
I 1


/* ssipc.c - Smart Socket based ipc functions */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/utsname.h>
#include "ssipc.h"


#ifdef SSIPC

/* try getpwuid() etc !!! */

/* security info: list of users permitted to stop current program */ 
static char *userlist[] = {"boiarino"};

/* security info: list of nodes permitted to stop current program */ 
static char *nodelist[] = {"clon00"};

static SSTHREAD local;

void
ssipc_put_args(SSTHREAD *arg)
{
  memcpy((char *)&local, (char *)arg, sizeof(SSTHREAD));

  return;
}

void
ssipc_get_args(SSTHREAD *arg)
{
  memcpy((char *)arg, (char *)&local, sizeof(SSTHREAD));

  return;
}

void
ssipc_quit_callback(int sig)
{
  uid_t uid, euid;
  gid_t gid, egid;
  struct utsname nam;

  /* received signal or quit control command */
  printf("\nssipc: received signal: %d\n\n",sig); fflush(stdout);

  if(sig == 0)
  {
    printf("ssipc: signal 0 was ignored\n\n"); fflush(stdout);
    return;
  }

  if(uname(&nam) == -1)
  {
    printf("ssipc: ERROR in uname()"); fflush(stdout);
    return;
  }
  uid  = getuid();
  euid = geteuid();
  gid  = getgid();
  egid = getegid();

  printf("ssipc: information about myself:\n");
  printf("            username >%s<\n",getenv("USER"));
  printf("            hostname >%s<\n",getenv("HOST"));
  printf("            userid   >%d< >%d<\n",uid,euid);
  printf("            groupid  >%d< >%d<\n",gid,egid);
  printf("ssipc: information about my computer:\n");
  printf("            sysname  >%s<\n",nam.sysname);
  printf("            nodename >%s<\n",nam.nodename);
  printf("            release  >%s<\n",nam.release);
  printf("            version  >%s<\n",nam.version);
  printf("            machine  >%s<\n\n",nam.machine);

  local.force_exit = 1;
  printf("ssipc: local.force_exit set to 1\n"); fflush(stdout);

  return;
}

/* do not change this routine */

void
ssipc_control_message_callback(T_IPC_CONN conn,
                               T_IPC_CONN_PROCESS_CB_DATA data,
                               T_CB_ARG arg)
{
  T_STR string;
  T_STR srvnode;
  T_IPC_MSG msg;
  /*
  printf("ssipc_control_message_callback: %d %d\n",
  local.nev_rec,local.nev_proc);
  */
  /*  get first string */
  TipcMsgSetCurrent(data->msg,0);
  TipcMsgNextStr(data->msg,&string);

  /*  evt status request */
  if(strcasecmp(string,"evt_status_poll")==0)
  {
    TipcSrvGetNode(&srvnode);

    msg=TipcMsgCreate(TipcMtLookup("evt_status"));
    TipcMsgSetDest(msg,(T_STR)"/evt_system/status");

    /* must send 'unique_name' here from main program */
    TipcMsgSetSender(msg,local.unique_name);
    TipcMsgAppendStr(msg,local.unique_name);

    TipcMsgAppendStr(msg,getenv("HOST"));
    TipcMsgAppendStr(msg,local.session);
    TipcMsgAppendStr(msg,srvnode);
    TipcMsgAppendInt4(msg,local.nev_rec);
    TipcMsgAppendReal8(msg,local.rate_rec);
    TipcMsgAppendInt4(msg,local.nev_proc);
    TipcMsgAppendReal8(msg,local.rate_proc);
    TipcMsgAppendInt4(msg,local.et_ok);
    TipcMsgAppendStr(msg,local.data_flow);

    TipcSrvMsgSend(msg,TRUE);
    TipcSrvFlush();
  }
  else  /* don't understand message ... ship to smartsockets interpreter */
  {
    TutCommandParseStr(string);
  }

  return;
}


/* include whatever you wish here... */

void
ssipc_status_poll_callback(T_IPC_MSG msg)
{
  TipcMsgAppendStr(msg,(T_STR)"session");
  TipcMsgAppendStr(msg,local.session);
  
  TipcMsgAppendStr(msg,(T_STR)"Total rate");
  TipcMsgAppendReal8(msg,local.Ratio[0]);
  
  TipcMsgAppendStr(msg,(T_STR)"Level 3 rate");
  TipcMsgAppendReal8(msg,local.Ratio[1]);
  
  TipcMsgAppendStr(msg,(T_STR)"Pattern Recognition rate");
  TipcMsgAppendReal8(msg,local.Ratio[2]);

  TipcMsgAppendStr(msg,(T_STR)"Hit-Based Tracking rate");
  TipcMsgAppendReal8(msg,local.Ratio[3]);

  TipcMsgAppendStr(msg,(T_STR)"Particle Identification rate");
  TipcMsgAppendReal8(msg,local.Ratio[4]);

  TipcMsgAppendStr(msg,(T_STR)"DC Time-To-Distance rate");
  TipcMsgAppendReal8(msg,local.Ratio[5]);

  TipcMsgAppendStr(msg,(T_STR)"Time-based Tracking rate");
  TipcMsgAppendReal8(msg,local.Ratio[6]);

  TipcMsgAppendStr(msg,(T_STR)"Final Analysis rate");
  TipcMsgAppendReal8(msg,local.Ratio[7]);

  return;
}

#else

void
ssipc_dummy()
{
  return;
}

#endif










E 1
