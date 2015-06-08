h16462
s 00000/00000/00000
d R 1.2 98/01/22 12:44:48 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 info_server/includes/servmsg.h
c Name history : 1 0 includes/servmsg.h
e
s 00033/00000/00000
d D 1.1 98/01/22 12:44:47 jhardie 1 0
c setup
e
u
U
f e 0
t
T
I 1
/***********************************************************************
 * servmsg.h
 *
 * Defines the input and output event types of the information server.
 *
 ***********************************************************************/

#define INFO_SCA_EVT 13
#define INFO_SCA_MSG 1001
#define SCA_EVT_GR "verbose"
#define SCA_MSG_GR "verbose"
#define SCA_EVT_NAME "info_server"
#define SCA_MSG_NAME "scaler_msg"


void create_new_types(char *name, int etype, char *grammar)
{
  T_IPC_MT mt;

  mt = TipcMtCreate(name, etype, grammar);
  if (mt == NULL)
  {
      fprintf(stderr,"Error: Could not create message type %s\n",name);
      exit(T_EXIT_FAILURE);
  }

  if (!TipcSrvLogAddMt(T_IPC_SRV_LOG_DATA,mt))
  {
      fprintf(stderr,"Error: Could not add message type %s to DATA\n",name);
      exit(T_EXIT_FAILURE);
  }

}
E 1
