/* 
 * timeline functions to 
 * register variables in the online database
 * interfaces to mysql on claspc10.
 *
 * everything in this files NEEDS access to the timeline variable stack
 */

#include <stdio.h>
#include <time.h>
#include <mysql/mysql.h>
#include "timeline.h"


static struct timeline_variable *table_ptr[MAX_VAR];
static int n_variables=0;

/*
 * add the names and pointers to the timeline stack
 */

int
add_to_stack (name, group, value, error, freq, unit, status)
char  name[100];
char  group[100];
float *value;
float *error;
int   freq;
int   unit;
int   *status;
{
  int local_status;
  static struct timeline_variable all_variables[MAX_VAR];
  
  if (n_variables+1 < MAX_VAR) {
    strcpy(all_variables[n_variables].name,name);
    strcpy(all_variables[n_variables].group,group);
    all_variables[n_variables].value_ptr  = value;
    all_variables[n_variables].error_ptr  = error;
    all_variables[n_variables].freq   = freq;
    all_variables[n_variables].unit   = unit;
    all_variables[n_variables].status_ptr = status;
    all_variables[n_variables].last_update = 0;
    
    table_ptr[n_variables] = &all_variables[0];

    n_variables++;
    local_status = 1;
  } else {
    local_status = 0;
  }
  return(local_status);
}

/* 
 * simple function to make sure that the name and group are unique
 * for each variable
 */

int
check_if_registered(char name[100], char group[100], float *value, float *error, int *status) 
{
  int n, local_status;
  local_status = 1;
  for (n=0; n < n_variables; n++) {

    if (strcmp((*table_ptr)[n].name,name)+strcmp((*table_ptr)[n].group,group) == 0) {
      local_status = 0;
      printf("check_if_registered W: A variable with name:%s and group: %s is already registered\n",name,group);

    } else if (value == (*table_ptr)[n].value_ptr) {
      local_status = 0;
      printf("check_if_registered W: Variable with name:%s and group: %s has identical VALUE pointer to previous registered variable\n",name,group);

    } else if (status == (*table_ptr)[n].status_ptr) {
      local_status = 0;
      printf("check_if_registered W: Variable with name:%s and group: %s has identical STATUS pointer to previous registered variable\n",name,group);

    } else if (error == (*table_ptr)[n].error_ptr) {
      local_status = 0;
      printf("check_if_registered W: Variable with name:%s and group: %s has identical ERROR pointer to previous registered variable\n",name,group);

    }
  }
  return(local_status);
  
}

/*
 * this function is to placed in the event loop.  It automatically updates
 * the database based on the time or number of events that have passed
 */

int
timeline_updatedb(int run_number, int event_number, int n_events_processed)
{
  char  name[100];
  char  group[100];
  float value;
  float error;
  int   freq;
  int   unit;
  int   status, last_update, time_now, update, local_status;

  MYSQL online_mysql;
  int n;

  time_now = time(NULL);

  for (n=0; n< n_variables; n++) {
    update = 0;

    strcpy(name, (*table_ptr)[n].name);
    strcpy(group, (*table_ptr)[n].group);
    status = *((*table_ptr)[n].status_ptr);
    value = *((*table_ptr)[n].value_ptr);
    error = *((*table_ptr)[n].error_ptr);
    unit = (*table_ptr)[n].unit;
    freq = (*table_ptr)[n].freq;
    last_update = (*table_ptr)[n].last_update;

    if (unit & 1 && (n_events_processed-last_update) >= freq) { /* update based on number of events processed */
      update = 1;
    }
    if (unit & 2 && (time_now-last_update) >= freq) { /* update based on time */
      update = 1;
    }
    if (unit & 4 && n_events_processed == -1) { /* update on beginning of run */
      update = 1;
    }
    if (unit & 8 && n_events_processed == -2) { /* update on end of run */
      update = 1;
    }

    if (update) {
      
      local_status = update_tables(run_number, event_number, name, group, value, error);
      *((*table_ptr)[n].status_ptr)=local_status;
      if (local_status) {
	if (unit & 1) {
	  (*table_ptr)[n].last_update = n_events_processed;
	}
	if (unit & 2) {
	  (*table_ptr)[n].last_update = time_now;
	}
      }
      
    } else {
      *((*table_ptr)[n].status_ptr)=1;
    }
    
  }
  return;
}


