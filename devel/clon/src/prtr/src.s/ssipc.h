
/*
#define SSIPC
*/


#ifdef SSIPC

/* ssipc.h - header file for ssipc.c */

#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__
#include <rtworks/ipc.h>
#include <clas_ipc_prototypes.h>

typedef struct ssthread
{
  char  *project; /* "clasprod" from command line */
  char  *session;
  char  *unique_name;
  char  *data_flow;
  int    nev_rec;
  int    nev_proc;
  double rate_rec;
  double rate_proc;
  int    et_ok;
  int    force_exit;
  time_t last_time;

  int   Count[8];
  float Ratio[8];
} SSTHREAD;

/* prototypes */
void ssipc_put_args(SSTHREAD *arg);
void ssipc_get_args(SSTHREAD *arg);
void ssipc_quit_callback(int sig);
void ssipc_status_poll_callback(T_IPC_MSG msg);
void ssipc_control_message_callback(T_IPC_CONN conn,
			                        T_IPC_CONN_PROCESS_CB_DATA data,
			                        T_CB_ARG arg);

#endif


