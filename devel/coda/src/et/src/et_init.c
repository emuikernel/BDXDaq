/*----------------------------------------------------------------------------*
 *  Copyright (c) 1998        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Author:  Carl Timmer                                                    *
 *             timmer@jlab.org                   Jefferson Lab, MS-12H        *
 *             Phone: (757) 269-5130             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *      Routines for initializations & destructions.
 *
 *----------------------------------------------------------------------------*/
 

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "et_private.h"
#include "et_network.h"


/*****************************************************/
/*                INITIALIZATIONS                    */
/*****************************************************/

void et_init_process(et_system *sys, et_proc_id id)
{
  int i;
  
  sys->proc[id].num          = -1;
  sys->proc[id].nattachments = 0;
  sys->proc[id].et_status    = ET_PROC_ETDEAD;
  sys->proc[id].heartbeat    = 0;
  sys->proc[id].pid          = -1;
  for (i=0 ; i < ET_ATTACHMENTS_MAX; i++) {
    sys->proc[id].att[i]     = -1;
  }
  sys->proc[id].status       = ET_PROC_CLOSED;
}

void et_init_attachment(et_system *sys, et_att_id id)
{
  sys->attach[id].num         = -1;
  sys->attach[id].proc        = -1;
  sys->attach[id].stat        = -1;
  sys->attach[id].blocked     = ET_ATT_UNBLOCKED;
  sys->attach[id].quit        = ET_ATT_CONTINUE;
  sys->attach[id].sleep       = ET_ATT_NOSLEEP;
  et_bigint_init(&sys->attach[id].events_put);
  et_bigint_init(&sys->attach[id].events_get);
  et_bigint_init(&sys->attach[id].events_dump);
  et_bigint_init(&sys->attach[id].events_make);
  sys->attach[id].pid         = -1;
  strcpy(sys->attach[id].host, "CANNOT_FIND_HOST");
  sys->attach[id].status      = ET_ATT_UNUSED;
}

/*****************************************************/
void et_init_station(et_station *ps)
{
  int i;

  /* init all except mutex which is done in et_init_mem_station
   * as it should only be done once.
   */
  ps->num          = -1;
  ps->next         = -1;
  ps->prev         = -1;
  ps->nextparallel = -1;
  ps->prevparallel = -1;
  ps->waslast      =  0;
 *ps->name         = '\0';
  ps->conductor    = ET_THREAD_KEEP;

  ps->config.flow_mode    = ET_STATION_SERIAL;
  ps->config.user_mode    = ET_STATION_USER_MULTI;
  ps->config.restore_mode = ET_STATION_RESTORE_OUT;
  ps->config.block_mode   = ET_STATION_BLOCKING;
  ps->config.select_mode  = ET_STATION_SELECT_ALL;
  ps->config.cue          = ET_STATION_CUE;
  ps->config.prescale     = ET_STATION_PRESCALE;
 *ps->config.fname        = '\0';
 *ps->config.lib          = '\0';
 *ps->config.classs       = '\0';
  for (i=0 ; i< ET_STATION_SELECT_INTS ; i++) {
    ps->config.select[i]  = -1;
  }
  ps->config.init         = ET_STRUCT_OK;

  ps->data.nattachments =  0;
  ps->data.pid_create   = -1;
  ps->data.lib_handle   = NULL;
  for (i=0 ; i < ET_ATTACHMENTS_MAX; i++) {
    ps->data.att[i] = -1;
  }
  
  ps->fix.in.first = NULL;
  ps->fix.in.start = 0;
  ps->fix.in.cnt   = 0;
  ps->fix.in.num   = 0;
  ps->fix.in.call  = ET_FIX_READ;
  et_bigint_init(&ps->fix.in.eventsin);
  
  ps->fix.out.start = 0;
  ps->fix.out.cnt   = 0;
  ps->fix.out.num   = 0;
  
  et_init_llist(&ps->list_in);
  et_init_llist(&ps->list_out);
  
  /* This is done last so error recovery is easier.
   * We'll know settings are "unused" consistant.
   */
  ps->data.status = ET_STATION_UNUSED;
}

/*****************************************************/
void et_init_llist(et_list *pl)
{
  pl->cnt                = 0;
  pl->lasthigh           = 0;
  et_bigint_init(&pl->events_try);
  et_bigint_init(&pl->events_in);
  et_bigint_init(&pl->events_out);
  pl->firstevent         = NULL;
  pl->lastevent          = NULL;
}

/*****************************************************/
void et_init_event(et_event *pe)
{
  et_init_event_(pe);
  pe->owner = ET_SYS;
}

/*****************************************************/
/* doesn't change owner, only called directly by
 * et_event(s)_new. Called indirectly by et_init_mem_event,
 * etr_event(s)_new
 */

void et_init_event_(et_event *pe)
{
  int i;
  
  pe->next       = NULL;
  pe->priority   = ET_LOW;
  pe->length     = -1;
  pe->memsize    = -1;
  pe->temp       = ET_EVENT_NORMAL;
  pe->age        = ET_EVENT_NEW;
  pe->datastatus = ET_DATA_OK;
  pe->byteorder  = 0x04030201;
  pe->pdata      = NULL;
  pe->modify     = 0;
  pe->pointer    = 0;
  pe->tempdata   = 0;
  for (i=0 ; i < ET_STATION_SELECT_INTS ; i++) {
    pe->control[i] = 0;
  }
}

/*****************************************************/
void et_init_histogram(et_id *id)
{
  int  i, *p = id->histogram;
  
  for (i=0; i < id->nevents + 1; i++,p++) {
    *p = 0;
  }
}

/*****************************************************/
void et_init_stats_att(et_system *sys, et_att_id id)
{  
  et_bigint_init(&sys->attach[id].events_put);
  et_bigint_init(&sys->attach[id].events_get);
  et_bigint_init(&sys->attach[id].events_dump);
  et_bigint_init(&sys->attach[id].events_make);
}

/*****************************************************/
void et_init_stats_allatts(et_system *sys)
{  
  int i;
  
  for (i=0; i < ET_ATTACHMENTS_MAX; i++) {
    et_init_stats_att(sys , (et_att_id) i);
  }
}

/*****************************************************/
void et_init_stats_station(et_station *ps)
{  
  et_bigint_init(&ps->list_in.events_try);
  et_bigint_init(&ps->list_in.events_out);
  et_bigint_init(&ps->list_in.events_in);
  et_bigint_init(&ps->list_out.events_try);
  et_bigint_init(&ps->list_out.events_out);
  et_bigint_init(&ps->list_out.events_in);
}

/*****************************************************/
void et_init_stats_allstations(et_id *id)
{  
  int i;
  et_station *ps = id->grandcentral;
  
  for (i=0; i < id->sys->config.nstations; i++) {
    et_init_stats_station(ps);
    ps++;
  }
}

/*****************************************************/
void et_init_stats_all(et_id *id)
{
  et_init_stats_allatts(id->sys);
  et_init_stats_allstations(id);
  et_init_histogram(id);
}

/*****************************************************/
/*             ET SYSTEM ID                          */
/*****************************************************/

int et_id_init(et_sys_id *id)
{
  et_id *etid;
  
  etid = (et_id *) malloc(sizeof(et_id));
  if (etid == NULL) {
    et_logmsg("ERROR", "et_id_init, cannot allocate memory\n");
    return ET_ERROR;
  }
  
  etid->alive        = 0;
  etid->lang         = ET_LANG_C;
  etid->proc         = ET_SYS;
  etid->offset       = 0;
  etid->race         = 0;
  etid->cleanup      = 0;
  etid->debug        = ET_DEBUG_ERROR;
  etid->nevents      = 0;
  etid->esize        = 0;
  etid->version      = ET_VERSION;
  etid->nselects     = ET_STATION_SELECT_INTS;
  /* can operating system share pthread mutexes between processes? */
#ifdef VXWORKS
  etid->share = ET_MUTEX_SHARE;
#else
  if (sysconf(_SC_THREAD_PROCESS_SHARED) == 1) {
    etid->share = ET_MUTEX_SHARE;
  }
  else {
    etid->share = ET_MUTEX_NOSHARE;
  }
#endif
  etid->memsize      = 0;
  
  /* pointers */
  etid->pmap         = NULL;
  etid->sys          = NULL;
  etid->stats        = NULL;
  etid->events       = NULL;
  etid->data         = NULL;
  etid->grandcentral = NULL;
  
  /* remote stuff */
  etid->locality     = ET_LOCAL;
  etid->sockfd       = -1;
  etid->iov_max      = ET_IOV_MAX;
  etid->port         = 0;
 *etid->ethost       = '\0';
  /* find endian value */
  if ( (etid->endian = et_byteorder()) == ET_ERROR) {
    et_logmsg("SEVERE", "et_id_init, strange byteorder\n");
    exit(1);
  }
  /* default is user is on same host as system */
  etid->systemendian = etid->endian;
  /* initialize mutex for thread-safe TCP */
#ifdef VXWORKS
  etid->mutex  = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if (etid->mutex == NULL) {
    et_logmsg("SEVERE","et_id_init could not allocate a semaphore.\n");
    exit(1);
  }
#else
  pthread_mutex_init(&etid->mutex, NULL);
#endif
  
  /* sign that we're done with init */
  etid->init         = ET_STRUCT_OK;
  
  *id = (et_sys_id) etid;
  return ET_OK;
}

/*****************************************************/
void et_id_destroy(et_sys_id id)
{
  et_id *etid = (et_id *) id;
  if (etid != NULL) {
    free(etid);
  }
  etid = NULL;
  return;
}

