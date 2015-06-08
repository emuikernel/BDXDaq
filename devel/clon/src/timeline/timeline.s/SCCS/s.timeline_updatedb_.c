h48262
s 00000/00000/00000
d R 1.2 03/12/22 17:19:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 timeline/timeline_updatedb_.c
e
s 00015/00000/00000
d D 1.1 03/12/22 17:19:41 boiarino 1 0
c date and time created 03/12/22 17:19:41 by boiarino
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

#include "timeline.h"

void timeline_updatedb_(run, event, n_events_processed)
int *run, *event, *n_events_processed;
{
  int istatus;

  istatus = timeline_updatedb(*run, *event, *n_events_processed);

  return;
}
E 1
