h41092
s 00000/00000/00000
d R 1.2 03/12/22 17:19:44 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 timeline/timeline_var_reg_.c
e
s 00052/00000/00000
d D 1.1 03/12/22 17:19:43 boiarino 1 0
c date and time created 03/12/22 17:19:43 by boiarino
e
u
U
f e 0
t
T
I 1
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

E 1
