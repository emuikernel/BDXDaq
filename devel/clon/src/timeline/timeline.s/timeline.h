/*
 * parameters, prototypes and structures for timeline API
 */

#define MAX_VAR 1000
#define HOST    "clondb3.jlab.org"
#define NAME    "clasrun"
#define DB_NAME "clas_online"

/*
 * function prototypes
 */

int timeline_var_reg(char name[100], char group[100], float *value, float *error, int freq, int unit, int *status);

int add_to_stack(char name[100], char group[100], float *value, float *error, int freq, int unit, int *status);

int make_db_tables(char name[100], char group[100]);

int check_if_registered(char name[100], char group[100], float *value, float *error, int *status);

int timeline_updatedb(int run_number, int event_number, int n_events_processed);

int update_tables(int run_number, int event_number, char name[100], char group[100], float value, float error);

int timeline_connect();
int timeline_disconnect();
int timeline_put(int run, int event, char name[100], char group[100], float value, float error);

int timeline_put_group(int run, int event, char table_name[100], char *col_names[MAX_VAR], float col_values[MAX_VAR], int n_columns);

int update_epics(char name[100], char group[100], float value);

/*
 * structures
 */
struct timeline_variable {
  char name[100], group[100];
  float *value_ptr, *error_ptr;
  int freq, unit, last_update;
  int *status_ptr;
}; 
