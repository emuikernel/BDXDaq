h11220
s 00001/00001/00117
d D 1.11 00/12/05 18:22:20 wolin 12 11
c New CC
e
s 00002/00002/00116
d D 1.10 00/12/05 18:15:15 wolin 11 10
c New CC
e
s 00004/00004/00114
d D 1.9 00/10/26 14:13:56 wolin 10 9
c const
e
s 00001/00001/00117
d D 1.8 00/10/26 13:59:28 wolin 9 8
c const
e
s 00001/00001/00117
d D 1.7 98/03/11 16:50:59 wolin 8 7
c Minor mods
c 
e
s 00000/00000/00118
d D 1.6 97/02/18 09:35:22 wolin 7 6
c Added start/stop message to clasmsg database
e
s 00001/00002/00117
d D 1.5 97/01/14 15:40:29 wolin 6 5
c Added tcl capability
e
s 00001/00000/00118
d D 1.4 96/08/09 15:34:25 aswin 5 4
c Bombproofing.  Added new Exit_Error function for failed calls.
e
s 00001/00001/00117
d D 1.3 96/08/09 11:08:31 aswin 4 3
c Added sort for backlog files, added packed binary message stuff,
c other minor mods and bombproofing
e
s 00000/00000/00118
d D 1.2 96/07/30 14:23:23 wolin 3 1
c Now working on SUN with new Ingres installation
e
s 00000/00000/00000
d R 1.2 96/07/26 17:10:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 dbrouter/s/dbrouter.h
c Name history : 1 0 s/dbrouter.h
e
s 00118/00000/00000
d D 1.1 96/07/26 17:10:19 wolin 1 0
c Header for dbrouter
e
u
U
f e 0
t
T
I 1
/*
// dbrouter.h
//
// parameters, prototypes, enums, etc.  for dbrouter(c++), dbrsql(c)
//
D 6
// ejw, 28-jun-96
E 6
I 6
// ejw, 14-jan-97
E 6
//
*/


/* max rows to return for select */
#define DBR_MAX_ROWS_TO_RETURN 300

/* max number of char in a column name */
#define MAX_COLNAME_LEN 100

/* maximum size of long varchar */
#define MAX_VARCHAR_LEN 32000


/* assorted counters */
extern int msg_count;
extern int proc_count;
extern int err_count;
extern int sql_count;
extern int sql_trans_count;
extern int data_count;
extern int insert_count;
extern int select_count;
extern int update_count;
extern int delete_count;
extern int other_count;
extern int commit_count;
extern int rollback_count;
extern int row_mod_count;
extern int row_return_count;


/* enums for internal status codes, types, formats, etc. */
typedef enum {
  SQL_COMMIT=0,
  SQL_ROLLBACK,
  SQL_BAD_FORMAT,
  SQL_ERROR,
  SQL_OK
} sql_code;

typedef enum {
  DBR_SQL=0,
  DBR_DATA,
  DBR_INSERT,
  DBR_SELECT,
  DBR_UPDATE,
  DBR_DELETE,
  DBR_OTHER
} dbr_type;

typedef enum {
  DBR_OK=0,
  DBR_FAIL
} dbr_code;



/* dbrouter prototypes...not called in dbrsql */
#ifdef __cplusplus
void receive_database_request(
			      T_IPC_CONN                 conn,
			      T_IPC_CONN_DEFAULT_CB_DATA data,
			      T_CB_ARG                   arg);
void process_backlog(void);
dbr_code process_message(T_IPC_MSG msg);
void check_status(int status, T_IPC_MSG msg);
void decode_command_line(int argc, char **argv);
D 8
void dbrouter_quit_callback(int sig);
E 8
I 8
D 11
void quit_callback(int sig);
E 8
void status_poll_data(T_IPC_MSG msg);
E 11
void count_sqltype(char *sqlstring);
void dbrouter_done(void);
#endif


/* dbrsql prototypes, called in dbrouter */
#ifdef __cplusplus
extern "C" {
#endif
I 11
void quit_callback(int sig);
void status_poll_data(T_IPC_MSG msg);
E 11
sql_code sql_connect(char *dbname);
sql_code sql_connect_check(void);
sql_code sql_process_sql(int maxrow, T_STR statement, T_IPC_MSG reply);
sql_code sql_prepare_sqlda_sql(void);
I 4
D 12
sql_code sql_Error (T_IPC_MSG reply, char *errmsg);
E 12
I 12
sql_code sql_Error (T_IPC_MSG reply, const char *errmsg);
E 12
E 4
sql_code sql_clear_sqlda(void);
D 4
sql_code sql_prepare_sqlda_data(T_IPC_MSG msg);
E 4
sql_code sql_process_data(T_IPC_MSG msg,T_IPC_MSG reply);
void sql_add_header_to_message(T_IPC_MSG reply);
void sql_add_row_to_message(T_IPC_MSG reply);
void sql_commit(void);
void sql_rollback(void);
sql_code sql_disconnect(void);
#ifdef __cplusplus
}
#endif



/* dbrouter utility prototypes called in dbrsql */
#ifdef __cplusplus
extern "C" {
#endif
D 10
void coutprintf(char *fmt, ...);
void cerrprintf(char *fmt, ...);
void logprintf(char *fmt, ...);
void errprintf(char *fmt, ...);
E 10
I 10
void coutprintf(const char *fmt, ...);
void cerrprintf(const char *fmt, ...);
void logprintf(const char *fmt, ...);
void errprintf(const char *fmt, ...);
E 10
void msgerrprint(T_STR *fmt, ...);
I 5
D 9
int Exit_Error (char *MsgErr);
E 9
I 9
int Exit_Error (const char *MsgErr);
E 9
E 5
#ifdef __cplusplus
}
#endif


D 6

E 6
E 1
