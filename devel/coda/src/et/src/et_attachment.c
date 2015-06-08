/*----------------------------------------------------------------------------*
 *  Copyright (c) 2001        Southeastern Universities Research Association, *
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
 *      Attachment routines
 *
 *----------------------------------------------------------------------------*/

#include "et_private.h"


/********************************************************/
/* Wake up a specific attachment waiting to read events.*/
int et_wakeup_attachment(et_sys_id id, et_att_id att)
{
  int status;
  et_id      *etid = (et_id *) id;
  et_stat_id  stat_id = etid->sys->attach[att].stat;
  et_station *ps = etid->grandcentral + stat_id;
  et_list    *pl = &ps->list_in, *pl_gc = &etid->grandcentral->list_in;

  if (att < 0) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_wakeup_attachment, bad argument\n");
    }
    return ET_ERROR;
  }
  
  if (etid->locality != ET_LOCAL) {
    return etr_wakeup_attachment(id, att);
  }
  
  if (att >= etid->sys->config.nattachments) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_wakeup_attachment, bad argument\n");
    }
    return ET_ERROR;
  }
  
  /* Only tell things to wake up if they are sleeping, otherwise
   * after the NEXT read, it will quit as if being woken up!
   */
  if ((etid->sys->attach[att].blocked == ET_ATT_UNBLOCKED) &&
      (etid->sys->attach[att].sleep   == ET_ATT_NOSLEEP)) {
    if (etid->debug >= ET_DEBUG_WARN) {
      et_logmsg("WARN", "et_wakeup_attachment, attachment is NOT blocked so not sending wakeup signal\n");
    }
    return ET_OK;
  }
  
  if (etid->debug >= ET_DEBUG_INFO) {
    et_logmsg("INFO", "et_wakeup_attachment, waking up attachment %d\n", att);
  }
  /* attachment may be waiting on a "get", so wake own station in list */
  etid->sys->attach[att].quit = ET_ATT_QUIT;
  status = pthread_cond_broadcast(&pl->cread);
  if (status != 0) {
    err_abort(status, "Wakeup readers");
  }
  /* attachment may be waiting on a "new", so wake GrandCentral's in list */
  status = pthread_cond_broadcast(&pl_gc->cread);
  if (status != 0) {
    err_abort(status, "Wakeup readers");
  }
  
  return ET_OK;
}
 
/******************************************************/
/*   Wake up all attachments waiting to read events   */
int et_wakeup_all(et_sys_id id, et_stat_id stat_id)
{
  int i, status;
  et_att_id att;
  et_id *etid = (et_id *) id;
  et_station *ps = etid->grandcentral + stat_id;
  et_list    *pl = &ps->list_in, *pl_gc = &etid->grandcentral->list_in;

  if (stat_id < 0) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_wakeup_all, bad argument\n");
    }
    return ET_ERROR;
  }
  
  if (etid->locality != ET_LOCAL) {
    return etr_wakeup_all(id, stat_id);
  }
  
  if (stat_id >= etid->sys->config.nstations) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_wakeup_all, bad argument\n");
    }
    return ET_ERROR;
  }
  
  for (i=0; i < etid->sys->config.nattachments ; i++) {
    att = ps->data.att[i];
    if (att > -1) {
      if ((etid->sys->attach[att].blocked == ET_ATT_BLOCKED) ||
	  (etid->sys->attach[att].sleep   == ET_ATT_SLEEP)) {
        etid->sys->attach[att].quit = ET_ATT_QUIT;
        if (etid->debug >= ET_DEBUG_INFO) {
          et_logmsg("INFO", "et_wakeup_all, waking up attachment %d\n", att);
        }
      } 
    }
  }
  /* attachment may be waiting on a "get", so wake own station in list */
  status = pthread_cond_broadcast(&pl->cread);
  if (status != 0) {
    err_abort(status, "Wakeup all readers");
  }
  /* attachment may be waiting on a "new", so wake GrandCentral's in list */
  status = pthread_cond_broadcast(&pl_gc->cread);
  if (status != 0) {
    err_abort(status, "Wakeup all readers");
  }
  
  return ET_OK;
}

/******************************************************/
/*       Get number of events put by attachment       */
int et_attach_geteventsput(et_sys_id id, et_att_id att_id,
                           int *highint, int *lowint)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality == ET_REMOTE) {
    return etr_attach_geteventsput(id, att_id, highint, lowint);
  }
  
  if ((att_id < 0) || (att_id >= etid->sys->config.nattachments)) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_attach_geteventsput, bad attachment id\n");
    }
    return ET_ERROR;
  }
  
  if (!et_alive(id)) {
    return ET_ERROR_DEAD;
  }
  
  if ((highint != NULL) && (lowint != NULL)) {
    et_bigint val;
    val = etid->sys->attach[att_id].events_put;
    *highint = val.highint;
    *lowint  = val.lowint;
  }
  
  return ET_OK;
}

/******************************************************/
/*      Get number of events gotten by attachment     */
int et_attach_geteventsget(et_sys_id id, et_att_id att_id,
                           int *highint, int *lowint)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality == ET_REMOTE) {
    return etr_attach_geteventsget(id, att_id, highint, lowint);
  }
  
  if ((att_id < 0) || (att_id >= etid->sys->config.nattachments)) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_attach_geteventsget, bad attachment id\n");
    }
    return ET_ERROR;
  }
  
  if (!et_alive(id)) {
    return ET_ERROR_DEAD;
  }
  
  if ((highint != NULL) && (lowint != NULL)) {
    et_bigint val;
    val = etid->sys->attach[att_id].events_get;
    *highint = val.highint;
    *lowint  = val.lowint;
  }
  
  return ET_OK;
}

/******************************************************/
/*     Get number of events dumped by attachment      */
int et_attach_geteventsdump(et_sys_id id, et_att_id att_id,
                            int *highint, int *lowint)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality == ET_REMOTE) {
    return etr_attach_geteventsdump(id, att_id, highint, lowint);
  }
  
  if ((att_id < 0) || (att_id >= etid->sys->config.nattachments)) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_attach_geteventsdump, bad attachment id\n");
    }
    return ET_ERROR;
  }
  
  if (!et_alive(id)) {
    return ET_ERROR_DEAD;
  }
  
  if ((highint != NULL) && (lowint != NULL)) {
    et_bigint val;
    val = etid->sys->attach[att_id].events_dump;
    *highint = val.highint;
    *lowint  = val.lowint;
  }
  
  return ET_OK;
}

/******************************************************/
/*      Get number of events made by attachment       */
int et_attach_geteventsmake(et_sys_id id, et_att_id att_id,
                            int *highint, int *lowint)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality == ET_REMOTE) {
    return etr_attach_geteventsmake(id, att_id, highint, lowint);
  }
  
  if ((att_id < 0) || (att_id >= etid->sys->config.nattachments)) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_attach_geteventsmake, bad attachment id\n");
    }
    return ET_ERROR;
  }
  
  if (!et_alive(id)) {
    return ET_ERROR_DEAD;
  }
  
  if ((highint != NULL) && (lowint != NULL)) {
    et_bigint val;
    val = etid->sys->attach[att_id].events_make;
    *highint = val.highint;
    *lowint  = val.lowint;
  }
  
  return ET_OK;
}

