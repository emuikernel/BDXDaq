/*
*  clas_ipc_prototypes.h
*
*  defines prototypes for clas ipc functions
*
*  ejw, 26-oct-00
*
*/


#ifdef __cplusplus
extern "C" {
#endif

int init_msg_types(void);

int ipc_init(const char *uniq_dgrp, const char *ident_string);

int ipc_check(float timeout);

/* int tclipc_init(void); */

int ipc_send_status_poll_result(void);

int ipc_close(void);

void ipc_atexit(void);

int ipc_set_server_names(const char *name);

int ipc_set_disconnect_mode(const char *mode);

int ipc_get_disconnect_mode(char **mode);

int ipc_set_application(const char *application);

int ipc_get_application(char **application);

int ipc_get_unique_datagroup(char **application);

int ipc_set_status_poll_group(const char *group_name);

int ipc_get_status_poll_group(char **group_name);

int ipc_set_alarm_poll_time(int time);

int ipc_set_quit_callback(void (*func)(int stat));

int ipc_set_control_message_callback(
	     void (*func)( T_IPC_CONN conn,
			   T_IPC_CONN_PROCESS_CB_DATA data,
			   T_CB_ARG arg)
			    );

int ipc_set_control_string_callback(
	     void (*func)(const char *str1, const char *str2)
	                    );


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

void ipc_perror(int);

int dbr_init(const char *uniq_dg, const char *application, const char *id_string);

int dbr_check(float time);

int dbr_close(void);

void dbr_output_dummy(T_STR str, va_list arg);

void ipc_output_dummy(T_STR str, va_list arg);

int ipc_init_reconnect(int reconnect_time);



/* fortran binding prototypes */

int *ipc_init_(const char *uniq_dgrp, const char *id_str, int len1, int len2);

int *ipc_close_(void);

int *ipc_check_(float *timeout);

/* int *tclipc_init_(void); */

int *tclinterp_init_(void);

int *tcl_evalfile_(const char *fname, int flen);

int *ipc_set_server_names_(const char *namesf, int len1);

int *ipc_set_disconnect_mode_(const char *namef, int len1);

int *ipc_get_disconnect_mode_(char *namef, int len1);

int *ipc_set_application_(const char *namef, int len1);

int *ipc_get_application_(char *namef, int len1);

int *ipc_get_unique_datagrup_(char *namef, int len1);

int *ipc_set_status_poll_group_(const char *namef, int len1);

int *ipc_get_status_poll_group_(char *namef, int len1);

int *ipc_set_alarm_poll_time_(int *time);

int *ipc_set_quit_callback_(void (*func)(int));

int *ipc_set_status_callback_(void (*func)(T_IPC_MSG));

int *ipc_set_control_string_callback_(
	     void (*func)(const char *cstr1, const char *cstr2));

int *ipc_status_append_str_(T_IPC_MSG msg, const char *namef, int len);

int *ipc_status_append_int4_(T_IPC_MSG msg, int *int4);

int *ipc_status_append_real4_(T_IPC_MSG msg, float *real4);

int *ipc_status_append_real8_(T_IPC_MSG msg, double *real8);

void ipc_command_parse_str_(const char *fstr, int flen);

void ipc_subject_subscribe_(const char *fsub, int flen); 

void ipc_perror_(int *);


#ifdef __cplusplus
}
#endif



