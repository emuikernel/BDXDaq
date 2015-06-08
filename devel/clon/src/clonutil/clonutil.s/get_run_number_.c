/*
 *  get_run_number_
 * fortran wrapper for get_run_number
 *
 * ejw, 20-feb-98
 *
 */

/* prototypes */
char *strdupf(char *s, int len);
int get_run_number(char *msql_database, char *session);


int *get_run_number_(char *fmsql_database, char *fsession, int c1, int c2) {

  char *msql_database = strdupf(fmsql_database,c1);
  char *session       = strdupf(fsession,c2);
  int run;

  run=get_run_number(msql_database,session);

  free(msql_database);
  free(session);

  return((int *)run);

}


