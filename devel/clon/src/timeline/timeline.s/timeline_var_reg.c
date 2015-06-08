/* 
 * timeline_var_reg function to 
 * register variables in the online database
 * interfaces to mysql on claspc10
 */

#include <stdio.h>

int timeline_var_reg(name, group, value, error, freq, unit, status)
char  name[100];
char  group[100];
float *value;
float *error;
int   freq;
int   unit;
int   *status;
{
  int local_status;

  if (unit != 0) {
    local_status = check_if_registered(name,group,value,error,status);
    if (local_status > 0 ) {
      local_status = add_to_stack(name, group, value, error, freq, unit, status);
    }
  } else {
    local_status=1;
  }
  
  if (local_status > 0) {
    local_status = make_db_tables(name,group);
  }

  *status = local_status;
  return(local_status);
}

