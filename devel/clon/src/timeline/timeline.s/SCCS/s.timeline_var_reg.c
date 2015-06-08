h15407
s 00000/00000/00000
d R 1.2 03/12/22 17:19:43 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 timeline/timeline_var_reg.c
e
s 00036/00000/00000
d D 1.1 03/12/22 17:19:42 boiarino 1 0
c date and time created 03/12/22 17:19:42 by boiarino
e
u
U
f e 0
t
T
I 1
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

E 1
