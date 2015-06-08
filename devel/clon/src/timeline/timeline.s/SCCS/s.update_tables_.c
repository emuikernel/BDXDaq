h29259
s 00000/00000/00000
d R 1.2 03/12/22 17:19:45 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 timeline/update_tables_.c
e
s 00041/00000/00000
d D 1.1 03/12/22 17:19:44 boiarino 1 0
c date and time created 03/12/22 17:19:44 by boiarino
e
u
U
f e 0
t
T
I 1
/*
 * FORTRAN wrapper function to update the database tables if they do not exist
 */

#include <stdio.h>
#include "timeline.h"

void update_tables_(run, event, name, group, value, error, status, len_name, len_group)
int *run, *event, len_name, len_group;
char *name, *group;
float *value, *error;
int *status;
{
  int i, end_name, end_group, istatus;
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

  *status = update_tables(*run, *event, cname, cgroup, *value, *error);
  return;
}

E 1
