h28074
s 00002/00000/00033
d D 1.25 01/10/12 11:08:56 wolin 26 25
c Added doc
e
s 00001/00001/00032
d D 1.24 01/10/12 10:32:36 wolin 25 24
c minor
e
s 00001/00000/00032
d D 1.23 01/10/11 14:31:12 wolin 24 23
c Added ipc2timeline
e
s 00000/00001/00032
d D 1.22 99/03/19 12:42:44 wolin 23 22
c Had 2 cmlog message types
c 
e
s 00001/00000/00032
d D 1.21 99/03/18 15:38:11 wolin 22 21
c Added cmlog message type
c 
e
s 00002/00002/00030
d D 1.20 99/02/16 16:17:46 wolin 21 20
c typo
c 
e
s 00000/00000/00032
d D 1.19 99/02/16 16:16:06 wolin 20 19
c New scheme
c 
e
s 00003/00000/00029
d D 1.18 99/02/16 13:57:49 wolin 19 18
c Added evt_xxx message types
c 
e
s 00001/00001/00028
d D 1.17 98/11/11 14:50:08 wolin 18 17
c Added cmlog msg type
c 
e
s 00002/00002/00027
d D 1.16 98/05/22 09:40:53 wolin 17 16
c Added dd_bosbank, removed clas_ipc2dd
c 
e
s 00001/00001/00028
d D 1.15 98/03/20 09:55:23 wolin 16 15
c ipc2dd now clas_ipc2dd
c 
e
s 00001/00001/00028
d D 1.14 98/03/19 12:05:35 wolin 15 14
c Added dd_event
c 
e
s 00003/00003/00026
d D 1.13 98/03/17 13:36:17 wolin 14 13
c Added ipc2dd, renamed unused msg types
c 
e
s 00002/00001/00027
d D 1.12 97/03/21 16:05:16 wolin 13 12
c Added info_server message type
e
s 00002/00001/00026
d D 1.11 97/02/10 14:46:12 wolin 12 11
c Added run_control message type
e
s 00000/00000/00027
d D 1.10 97/01/21 16:02:42 wolin 11 10
c Typo
e
s 00000/00000/00027
d D 1.9 97/01/21 16:01:56 wolin 10 9
c Final info_server message type
e
s 00002/00001/00025
d D 1.8 96/11/01 14:24:59 wolin 9 8
c Added dd_status
e
s 00004/00001/00022
d D 1.7 96/10/23 13:41:27 wolin 8 7
c Added info server mtypes
e
s 00003/00001/00020
d D 1.6 96/10/07 12:58:53 wolin 7 6
c Added tcl_request,tcl_reply
e
s 00000/00000/00021
d D 1.5 96/09/11 13:11:12 wolin 6 5
c Bug in codemgr
e
s 00000/00000/00021
d D 1.4 96/09/11 13:09:25 wolin 5 4
c Bug in codemgr
e
s 00000/00000/00021
d D 1.3 96/09/11 13:05:17 wolin 4 3
c Bug in codemgr
e
s 00015/00018/00006
d D 1.2 96/09/05 11:44:44 wolin 3 1
c New scheme for init of standard message types
e
s 00000/00000/00000
d R 1.2 96/08/08 16:28:28 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 include/s/clas_ipc_mt.h
c Name history : 1 0 s/clas_ipc_mt.h
e
s 00024/00000/00000
d D 1.1 96/08/08 16:28:27 wolin 1 0
c CLAS IPC message type definitions
e
u
U
f e 0
t
T
I 1
/* 
*  clas_ipc_mt.h 
*
D 3
*  defines clas ipc message type numbers and names
E 3
I 3
*  defines array of structures holding CLAS standard message types and numbers
E 3
*
D 3
*  ejw, 26-jun-96
E 3
I 3
*  ipc_init initializes all of them by default
*
I 26
*  NOTE:  java ipc_init must be modified if this file is modified
*
E 26
*  ejw, 4-sep-96
*
E 3
*/

D 3
#define DBR_REQUEST_MTNUM               1
#define DBR_REQUEST_MTNAME              "dbr_request"

#define DBR_REPLY_MTNUM                 2
#define DBR_REPLY_MTNAME                "dbr_reply"

#define DBR_REQUEST_MTNUM_RPC           3
#define DBR_REQUEST_MTNAME_RPC          "dbr_request_rpc"

#define DBR_REPLY_MTNUM_RPC             4
#define DBR_REPLY_MTNAME_RPC            "dbr_reply_rpc"

#define STATUS_POLL_RESULT_MTNUM        5
#define STATUS_POLL_RESULT_MTNAME       "status_poll_result"


E 3
I 3
struct clas_standard_msg_type {
  int mtnum;                      /* must be positive                 */
  char mtname[32];                /* limit of 32 char for each mtname */
} ipc_msg_type[] = {
		  1,"status_poll_result",
		  2,"dbr_request",
		  3,"dbr_reply",
		  4,"dbr_request_rpc",
D 7
		  5,"dbr_reply_rpc"
E 7
I 7
		  5,"dbr_reply_rpc",
		  6,"tcl_request",
D 8
		  7,"tcl_reply"
E 8
I 8
		  7,"tcl_reply",
D 14
		  8,"info_scal_msg",
		  9,"info_scal_evt",
D 9
		 10,"info_scal_cmd"
E 9
I 9
		 10,"info_scal_cmd",
E 14
I 14
D 16
		  8,"ipc2dd",
E 16
I 16
D 17
		  8,"clas_ipc2dd",
E 17
I 17
D 18
		  8,"dummy1",
E 18
I 18
		  8,"cmlog",
E 18
E 17
E 16
D 15
		  9,"dummy1",
E 15
I 15
		  9,"dd_event",
E 15
D 17
		 10,"dummy2",
E 17
I 17
		 10,"dd_bosbank",
E 17
E 14
D 12
		 11,"dd_status"
E 12
I 12
		 11,"dd_status",
D 13
		 12,"run_control"
E 13
I 13
		 12,"run_control",
D 21
		 13,"info_server"
E 21
I 21
		 13,"info_server",
E 21
I 19
		 14,"evt_event",
		 15,"evt_bosbank",
		 16,"evt_status",
I 24
D 25
		 17,"ipc2timeline",
E 25
I 25
		 17,"ipc2timeline"
E 25
E 24
I 22
D 23
		 17,"cmlog",
E 23
E 22
E 19
E 13
E 12
E 9
E 8
E 7
D 21
                 };
E 21
I 21
};
E 21
E 3
E 1
