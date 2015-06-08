h27955
s 00000/00005/00157
d D 1.28 00/10/26 13:51:33 wolin 29 28
c Remove insert_msg!
e
s 00005/00000/00157
d D 1.27 00/10/26 13:48:47 wolin 28 27
c Added insert_msg
e
s 00018/00018/00139
d D 1.26 00/10/26 12:36:08 wolin 27 26
c Added const
e
s 00002/00000/00155
d D 1.25 99/05/18 13:09:29 wolin 26 25
c Added dbr_output_dummy
c 
e
s 00002/00000/00153
d D 1.24 98/05/18 13:40:00 wolin 25 24
c Added ipc_init_reconnect
c 
e
s 00001/00001/00152
d D 1.23 98/05/18 12:42:48 wolin 24 23
c Typo
c 
e
s 00002/00000/00151
d D 1.22 98/05/18 12:39:38 wolin 23 22
c Added output_dummy
c 
e
s 00002/00000/00149
d D 1.21 98/03/13 15:03:46 wolin 22 21
c Added ipc_subject_subscribe
c 
e
s 00002/00000/00147
d D 1.20 98/03/13 14:32:24 wolin 21 20
c Added ipc_command_parse_str_
c 
e
s 00002/00000/00145
d D 1.19 97/03/25 12:17:49 wolin 20 19
c Added init_msg_types
e
s 00005/00000/00140
d D 1.18 97/03/19 10:44:59 wolin 19 18
c Added dbr prototypes
e
s 00002/00002/00138
d D 1.17 96/11/08 11:50:19 wolin 18 17
c Removed tclipc_init
e
s 00003/00001/00137
d D 1.16 96/10/24 11:28:48 wolin 17 16
c Added some, not all, ipc tcl prototypes
e
s 00000/00004/00138
d D 1.15 96/10/21 13:24:51 wolin 16 15
c Redid some fortran prototypes, others too
e
s 00004/00000/00138
d D 1.14 96/10/16 09:37:03 wolin 15 14
c Added tclinterp_init
e
s 00002/00000/00136
d D 1.13 96/10/15 11:51:05 wolin 14 13
c Added tcl_evalfile_
e
s 00002/00002/00134
d D 1.12 96/10/15 11:40:56 wolin 13 12
c Removed startup script from tclipc_init
e
s 00001/00001/00135
d D 1.11 96/10/07 17:15:26 wolin 12 11
c Type
e
s 00004/00000/00132
d D 1.10 96/10/07 17:08:29 wolin 11 10
c Added ipc_get_unique_datagroup
e
s 00004/00000/00128
d D 1.9 96/10/07 17:02:24 wolin 10 9
c Added tclipc_init
e
s 00001/00001/00127
d D 1.8 96/09/26 15:11:16 wolin 9 8
c Typo
e
s 00011/00003/00117
d D 1.7 96/09/26 15:05:37 wolin 8 7
c Finished fortran interface, other minor mods
e
s 00000/00000/00120
d D 1.6 96/09/11 13:11:13 wolin 7 6
c Bug in codemgr
e
s 00000/00000/00120
d D 1.5 96/09/11 13:09:25 wolin 6 5
c Bug in codemgr
e
s 00000/00000/00120
d D 1.4 96/09/11 13:05:18 wolin 5 4
c Bug in codemgr
e
s 00026/00013/00094
d D 1.3 96/09/10 15:03:17 wolin 4 3
c Redid fortran wrappers
e
s 00000/00002/00107
d D 1.2 96/09/05 11:44:44 wolin 3 1
c New scheme for init of standard message types
e
s 00000/00000/00000
d R 1.2 96/08/08 16:27:07 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 include/s/clas_ipc_prototypes.h
c Name history : 1 0 s/clas_ipc_prototypes.h
e
s 00109/00000/00000
d D 1.1 96/08/08 16:27:06 wolin 1 0
c CLAS IPC function prototypes
e
u
U
f e 0
t
T
I 1
/*
*  clas_ipc_prototypes.h
*
*  defines prototypes for clas ipc functions
*
D 4
*  ejw, 18-jun-96
E 4
I 4
D 8
*  ejw, 10-sep-96
E 8
I 8
D 27
*  ejw, 25-sep-96
E 27
I 27
*  ejw, 26-oct-00
E 27
E 8
E 4
*
*/


I 28
D 29
extern "C" {
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *status, int code, const char *text);
}	

E 29
E 28
#ifdef __cplusplus
extern "C" {
#endif

I 20
int init_msg_types(void);

E 20
D 27
int ipc_init(char *uniq_dgrp, char *ident_string);
E 27
I 27
int ipc_init(const char *uniq_dgrp, const char *ident_string);
E 27

int ipc_check(float timeout);

I 15
D 16
int tclinterp_int(void);

E 16
E 15
I 10
D 13
int tclipc_int(char *tcl_startup_script);
E 13
I 13
D 17
int tclipc_int(void);
E 17
I 17
D 18
int tclipc_init(void);
E 18
I 18
/* int tclipc_init(void); */
E 18
E 17
E 13

E 10
int ipc_send_status_poll_result(void);

int ipc_close(void);

void ipc_atexit(void);

D 27
int ipc_set_server_names(char *name);
E 27
I 27
int ipc_set_server_names(const char *name);
E 27

D 27
int ipc_set_disconnect_mode(char *mode);
E 27
I 27
int ipc_set_disconnect_mode(const char *mode);
E 27

int ipc_get_disconnect_mode(char **mode);

D 27
int ipc_set_application(char *application);
E 27
I 27
int ipc_set_application(const char *application);
E 27

int ipc_get_application(char **application);

I 11
int ipc_get_unique_datagroup(char **application);

E 11
D 27
int ipc_set_status_poll_group(char *group_name);
E 27
I 27
int ipc_set_status_poll_group(const char *group_name);
E 27

int ipc_get_status_poll_group(char **group_name);

D 3
int ipc_get_status_poll_result_mt(T_IPC_MT *mt);

E 3
int ipc_set_alarm_poll_time(int time);

int ipc_set_quit_callback(void (*func)(int stat));

D 8
int ipc_set_user_control_callback(
E 8
I 8
int ipc_set_control_message_callback(
E 8
	     void (*func)( T_IPC_CONN conn,
			   T_IPC_CONN_PROCESS_CB_DATA data,
			   T_CB_ARG arg)
			    );

I 8
int ipc_set_control_string_callback(
D 27
	     void (*func)(char *str1, char *str2)
E 27
I 27
	     void (*func)(const char *str1, const char *str2)
E 27
	                    );


E 8
int ipc_set_user_default_callback(
	     void (*func)( T_IPC_CONN conn,
			   T_IPC_CONN_PROCESS_CB_DATA data,
			   T_CB_ARG arg)
			    );

int ipc_set_user_status_poll_callback(void (*func)(T_IPC_MSG msg));

void ipc_control_callback(
     T_IPC_CONN conn,
     T_IPC_CONN_PROCESS_CB_DATA data,
     T_CB_ARG arg);

void ipc_default_callback(
     T_IPC_CONN conn,
     T_IPC_CONN_PROCESS_CB_DATA data,
     T_CB_ARG arg);

void ipc_alarm_handler(int stat);

void ipc_default_quit_callback(int stat);

I 4
void ipc_perror(int);
E 4

I 19
D 27
int dbr_init(char *uniq_dg, char *application, char *id_string);
E 27
I 27
int dbr_init(const char *uniq_dg, const char *application, const char *id_string);
E 27
E 19

I 19
int dbr_check(float time);
E 19
I 4

I 19
int dbr_close(void);

I 26
void dbr_output_dummy(T_STR str, va_list arg);

E 26
I 23
D 24
void output_dummy(T_STR str, va_list arg);
E 24
I 24
void ipc_output_dummy(T_STR str, va_list arg);
E 24
E 23

I 25
int ipc_init_reconnect(int reconnect_time);
E 25
E 19

I 23

I 25

E 25
E 23
E 4
/* fortran binding prototypes */

D 4
int *ipc_init_(char *, char *, int, int);
E 4
I 4
D 27
int *ipc_init_(char *uniq_dgrp, char *id_str, int len1, int len2);
E 27
I 27
int *ipc_init_(const char *uniq_dgrp, const char *id_str, int len1, int len2);
E 27
E 4

int *ipc_close_(void);

D 4
int *ipc_set_server_names_(char *, int);
E 4
I 4
int *ipc_check_(float *timeout);
E 4

I 15
D 16
int *tclinterp_init_(void);

E 16
E 15
I 10
D 12
int *tclipc_init_(char *tcl_startup_script, len1);
E 12
I 12
D 13
int *tclipc_init_(char *tcl_startup_script, int len1);
E 13
I 13
D 18
int *tclipc_init_(void);
E 18
I 18
/* int *tclipc_init_(void); */
E 18
E 13
E 12

I 17
int *tclinterp_init_(void);

E 17
I 14
D 27
int *tcl_evalfile_(char *fname, int flen);
E 27
I 27
int *tcl_evalfile_(const char *fname, int flen);
E 27

E 14
E 10
D 4
int *ipc_set_application_(char *, int);
E 4
I 4
D 27
int *ipc_set_server_names_(char *namesf, int len1);
E 27
I 27
int *ipc_set_server_names_(const char *namesf, int len1);
E 27
E 4

D 4
int *ipc_set_status_poll_group_(char *, int);
E 4
I 4
D 27
int *ipc_set_disconnect_mode_(char *namef, int len1);
E 27
I 27
int *ipc_set_disconnect_mode_(const char *namef, int len1);
E 27
E 4

D 4
int *ipc_set_alarm_poll_time_(int *);
E 4
I 4
int *ipc_get_disconnect_mode_(char *namef, int len1);
E 4

I 4
D 27
int *ipc_set_application_(char *namef, int len1);
E 27
I 27
int *ipc_set_application_(const char *namef, int len1);
E 27

int *ipc_get_application_(char *namef, int len1);

I 11
int *ipc_get_unique_datagrup_(char *namef, int len1);

E 11
D 27
int *ipc_set_status_poll_group_(char *namef, int len1);
E 27
I 27
int *ipc_set_status_poll_group_(const char *namef, int len1);
E 27

int *ipc_get_status_poll_group_(char *namef, int len1);

int *ipc_set_alarm_poll_time_(int *time);

E 4
int *ipc_set_quit_callback_(void (*func)(int));

D 4
int *ipc_set_user_control_callback_(
		      void (*func)( T_IPC_CONN,
				    T_IPC_CONN_PROCESS_CB_DATA,
				    T_CB_ARG)
			      );
E 4
I 4
D 8
int *ipc_set__status_callback_(void (*func)(T_IPC_MSG));
E 8
I 8
int *ipc_set_status_callback_(void (*func)(T_IPC_MSG));
E 8
E 4

I 8
D 9
int ipc_set_control_string_callback_(
E 9
I 9
int *ipc_set_control_string_callback_(
E 9
D 27
	     void (*func)(char *cstr1, char *cstr2));
E 27
I 27
	     void (*func)(const char *cstr1, const char *cstr2));
E 27

E 8
D 4
int *ipc_set_user_status_poll_callback_(void (*func)(T_IPC_MSG));
E 4
I 4
D 27
int *ipc_status_append_str_(T_IPC_MSG msg, char *namef, int len);
E 27
I 27
int *ipc_status_append_str_(T_IPC_MSG msg, const char *namef, int len);
E 27
E 4

D 4
void ipc_perror(int);
E 4
I 4
int *ipc_status_append_int4_(T_IPC_MSG msg, int *int4);
E 4

I 4
int *ipc_status_append_real4_(T_IPC_MSG msg, float *real4);

int *ipc_status_append_real8_(T_IPC_MSG msg, double *real8);

I 21
D 27
void ipc_command_parse_str_(char *fstr, int flen);
E 27
I 27
void ipc_command_parse_str_(const char *fstr, int flen);
E 27

I 22
D 27
void ipc_subject_subscribe_(char *fsub, int flen); 
E 27
I 27
void ipc_subject_subscribe_(const char *fsub, int flen); 
E 27

E 22
E 21
E 4
void ipc_perror_(int *);


#ifdef __cplusplus
}
#endif



E 1
