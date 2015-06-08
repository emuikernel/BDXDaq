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
