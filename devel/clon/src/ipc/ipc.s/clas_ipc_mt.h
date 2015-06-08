/* 
*  clas_ipc_mt.h 
*
*  defines array of structures holding CLAS standard message types and numbers
*
*  ipc_init initializes all of them by default
*
*  NOTE:  java ipc_init must be modified if this file is modified
*
*  ejw, 4-sep-96
*
*/

typedef struct clas_standard_msg_type {
  int mtnum;                      /* must be positive                 */
  char mtname[32];                /* limit of 32 char for each mtname */
} IPC_MSG_TYPE;

#define IPC_MSG_TYPE_LOCAL(nnn) \
     IPC_MSG_TYPE ipc_msg_type##nnn[] = { \
		  1,"status_poll_result", \
		  2,"dbr_request", \
		  3,"dbr_reply", \
		  4,"dbr_request_rpc", \
		  5,"dbr_reply_rpc", \
		  6,"tcl_request", \
		  7,"tcl_reply", \
		  8,"cmlog", \
		  9,"dd_event", \
		 10,"dd_bosbank", \
		 11,"dd_status", \
		 12,"run_control", \
		 13,"info_server", \
		 14,"evt_event", \
		 15,"evt_bosbank", \
		 16,"evt_status", \
		 17,"ipc2timeline" \
};
