/* 
 * FORTRAN wrapper for: timeline_var_reg function to 
 * register variables in the online database
 * interfaces to mysql on claspc10
 */

#include <stdio.h>
#include <ctype.h>
#include "timeline.h"


void timeline_var_reg_(name, group, value, error, freq, unit, status, len_name, len_group)
char  *name;
char  *group;
float *value;
float *error;
int   *freq;
int   *unit;
int   *status;
int   len_name, len_group;
{
  int icheck, i, end_name, end_group;
  char *cname, *cgroup;

  end_name=0;
  for (i=1; i<=len_name && end_name==0; i++) {
    if ((isspace(name[i]))) {
      end_name=i;
    }
  }

  cname = (char *)malloc(end_name+1);
  strncpy(cname, name, end_name);
  cname[end_name]=0;

  end_group=0;
  for (i=1; i<=len_group && end_group==0; i++) {
    if ((isspace(group[i]))) {
      end_group=i;
    }
  }
  cgroup = (char *)malloc(end_group+1);
  strncpy(cgroup, group, end_group);
  cgroup[end_group]=0;
        

  icheck = timeline_var_reg(cname, cgroup, value, error, *freq, *unit, status); 

  free(cname);
  free(cgroup);
}

