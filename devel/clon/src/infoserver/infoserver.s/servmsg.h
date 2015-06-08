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
