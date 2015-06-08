h59442
s 00000/00000/00000
d R 1.2 98/02/21 00:48:09 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 libutil/s/get_run_number_.c
e
s 00029/00000/00000
d D 1.1 98/02/21 00:48:08 clasrun 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
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


E 1
