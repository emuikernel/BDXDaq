/*
// dbrouter.h
//
// parameters, prototypes, enums, etc.  for dbrouter(c++), dbrsql(c)
//
// ejw, 14-jan-97
// ejw, 13-jun-2006 upgrade to use mySQL
//
*/


#include <mysql/mysql.h>


/* max rows to return for select */
#define DBR_MAX_ROWS_TO_RETURN 1000

/* max number of char in a column name */
#define MAX_COLNAME_LEN 256

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
void count_sqltype(char *sqlstring);
void dbrouter_done(void);
#endif


/* dbrsql prototypes, called in dbrouter */
#ifdef __cplusplus
extern "C" {
#endif
void quit_callback(int sig);
void status_poll_data(T_IPC_MSG msg);
sql_code sql_connect(char *dbhost, char *dbuser, char *dbname);
sql_code sql_connect_check(void);
sql_code sql_process_sql(int maxrow, T_STR statement, T_IPC_MSG reply);
sql_code sql_prepare_sqlda_sql(void);
sql_code sql_clear_sqlda(void);
sql_code sql_process_data(T_IPC_MSG msg,T_IPC_MSG reply);
void sql_add_header_to_message(T_IPC_MSG reply, MYSQL_RES *res);
void sql_add_row_to_message(T_IPC_MSG reply, MYSQL_RES *res, MYSQL_ROW row);
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
void coutprintf(const char *fmt, ...);
void cerrprintf(const char *fmt, ...);
void logprintf(const char *fmt, ...);
void errprintf(const char *fmt, ...);
void msgerrprint(T_STR *fmt, ...);
int Exit_Error (const char *MsgErr);
#ifdef __cplusplus
}
#endif


