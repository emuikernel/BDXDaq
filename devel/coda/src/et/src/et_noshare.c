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
 *      Routines for operating systems like Linux that cannot share mutexes
 *	between different Unix processes.
 *
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "et_private.h"
#include "et_network.h"

/*
 * For a Linux local open, need to map the ET system memory AND
 * open up a network connection to the server which the ET system
 * is running. Thus, the server handles all the mutex and condition
 * variable stuff, and the application can read the data right
 * from the mapped memory. Voila, a local ET system on Linux.
 */
int etn_open(et_sys_id *id, const char *filename, et_openconfig openconfig)
{     
  et_open_config *config = (et_open_config *) openconfig;
  et_mem  *first_item;
  et_id *etid;
  struct timespec heartbeat;
  int fd, status, sockfd, length, bufsize, version, nselects;
  int err=ET_OK, transfer[4], incoming[8];
  char *buf, *pbuf;
  char  buffer[20];
  
  /* system id */
  etid = (et_id *) *id;
       
  /* First look to see if it's a Java ET system, in which case
   * a remote open must be made instead of a local one.
   */
  if ((fd = open(filename, O_RDONLY, S_IRWXU)) < 0) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open: cannot open ET system file\n");
    }
    return ET_ERROR;
  }
  if ((read(fd, (void *)buffer, 19)) == 19) {
    buffer[19] = '\0';
    if (strcmp(buffer, "JAVA ET SYSTEM FILE") == 0) {
      /* This is a Java ET system - use remote etr_open */
      close(fd);
      return etr_open(id, filename, openconfig);
    }
  }
  close(fd);

  /* attach to mapped memory */
  first_item = (et_mem *) et_mem_attach(filename);
  if (first_item == NULL) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open: cannot attach to ET system file\n");
    }
    return ET_ERROR;
  }
  
  /* find size of mapped memory */
  if (et_mem_size(filename, &etid->memsize, NULL) != ET_OK) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open: cannot find size of ET system file\n");
    }
    return ET_ERROR;
  }

  etid->pmap   = (void *)       (first_item);
  etid->sys    = (et_system *)  (first_item + 1);
  
  /* Stop here and check to see if the ET system version and our
   * version of the ET software is the same or not.
   */
  if (etid->version != etid->sys->version) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open: ET system & user's ET versions are different. User: %i, system: %i \n",etid->version,etid->sys->version);
    }
    munmap(etid->pmap, etid->memsize);
    return ET_ERROR;
  }
  if (etid->nselects != etid->sys->nselects) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open: ET system & user have incompatible values for ET_STATION_SELECT_INTS\n");
    }
    munmap(etid->pmap, etid->memsize);
    return ET_ERROR;
  }
  
  etid->stats     = (et_station *) (etid->sys    + 1);
  etid->histogram = (int *)        (etid->stats  + etid->sys->config.nstations);
  etid->events    = (et_event *)   (etid->histogram + (etid->sys->config.nevents + 1));
  etid->data      = (char *)       (etid->events + etid->sys->config.nevents);
  etid->grandcentral = etid->stats;
  etid->offset = (char *)etid->pmap - (char *)etid->sys->pmap;
  
  /* Add data to id - locality, port & host of ET system */
  etid->locality = ET_LOCAL_NOSHARE;
  etid->port     = etid->sys->port;
  strcpy(etid->ethost, etid->sys->host);
  
  if (etid->debug >= ET_DEBUG_INFO) {
    et_logmsg("INFO", "etn_open, offset   : val = %d\n", etid->offset);
    et_logmsg("INFO", "etn_open, ET map   : ptr = %p\n", etid->pmap);
    et_logmsg("INFO", "etn_open, ET sys   : ptr = %p\n", etid->sys);
    et_logmsg("INFO", "etn_open, ET stats : ptr = %p\n", etid->stats);
    et_logmsg("INFO", "etn_open, ET histo : ptr = %p\n", etid->histogram);
    et_logmsg("INFO", "etn_open, ET events: ptr = %p\n", etid->events);
    et_logmsg("INFO", "etn_open, ET data  : ptr = %p\n", etid->data);
  }
    
  heartbeat.tv_sec  = ET_BEAT_SEC;
  heartbeat.tv_nsec = ET_BEAT_NSEC;
  
  /* wait for ET system to start running */
  if (config->wait == ET_OPEN_WAIT) {
    status = et_wait_for_system(*id, &config->timeout, filename);
  }
  /* wait 1 heartbeat minimum, to see if ET system is running */
  else {
    status = et_wait_for_system(*id, &heartbeat, filename);
  }
  
  if (status != ET_OK) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open: ET system is not active\n");
    }
    munmap(etid->pmap, etid->memsize);
    return status;
  }
     
  /***************************/
  /* Now the network portion */
  /***************************/
  
  if (etid->sys->port <= 0) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open: bad value for port\n");
    }
    munmap(etid->pmap, etid->memsize);
    return ET_ERROR_REMOTE;
  }
  
  /* find client's iov_max value */
  if ( (etid->iov_max = sysconf(_SC_IOV_MAX)) == -1) {
    /* set it to POSIX minimum by default (it always bombs on Linux) */
    etid->iov_max = ET_IOV_MAX;
  }
  
  /* endian stuff */
  transfer[0] = htonl(etid->endian);
  
  /* length of ET system name */
  length = strlen(filename)+1;
  transfer[1] = htonl(length);
  /* not used */
  transfer[2] = 0;
  transfer[3] = 0;
  
  /* make the network connection */
  sockfd = tcp_connect(etid->sys->host, etid->sys->port);
  if (sockfd < 0) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open: cannot connect to server\n");
    }
    munmap(etid->pmap, etid->memsize);
    return ET_ERROR_REMOTE;
  }
  etid->sockfd = sockfd;

  /* put everything in one buffer */
  bufsize = sizeof(transfer) + length;
  if ( (pbuf = buf = (char *) malloc(bufsize)) == NULL) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open, cannot allocate memory\n");
    }
    err = ET_ERROR_REMOTE;
    goto error;
  }
  memcpy(pbuf, transfer, sizeof(transfer));
  pbuf += sizeof(transfer);
  memcpy(pbuf,filename, length);
  
  /* write it to server */
  if (tcp_write(sockfd, (void *) buf, bufsize) != bufsize) {
    free(buf);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open, write error\n");
    }
    err = ET_ERROR_WRITE;
    goto error;
  }
  free(buf);
  
  /* read the return */
  if (tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open, read error\n");
    }
    err = ET_ERROR_READ;
    goto error;
  }
  err = ntohl(err);
  if (err != ET_OK) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open: found the wrong ET system\n");
    }
    goto error;
  }
  
  if (tcp_read(sockfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open, read error\n");
    }
    err = ET_ERROR_READ;
    goto error;
  }
  /* ET's endian */
  etid->systemendian = ntohl(incoming[0]);
  /* ET's total number of events */
  etid->nevents = ntohl(incoming[1]);
  /* ET's max event size */
  etid->esize = ntohl(incoming[2]);
  /* ET's version number */
  version = ntohl(incoming[3]);
  /* ET's number of selection integers */
  nselects = ntohl(incoming[4]);
  /* ET's language */
  etid->lang = ntohl(incoming[5]);
  
  if (version != etid->version) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open: ET system & user's ET versions are different\n");
    }
    err = ET_ERROR_REMOTE;
    goto error;
  }
  
  if (nselects != etid->nselects) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_open: ET system & user have incompatible values for ET_STATION_SELECT_INTS\n");
    }
    err = ET_ERROR_REMOTE;
    goto error;
  }

  return ET_OK;
  
  error:
    close(sockfd);
    munmap(etid->pmap, etid->memsize);
    return err;
}

/******************************************************/
int etn_alive(et_sys_id id)
{
  et_id *etid = (et_id *) id;
  int sockfd = etid->sockfd;
  struct timespec waittime;
  unsigned int newheartbt, oldheartbt;
  int status=1, alive, com;
  
  /* monitor ET system's heartbeat by socket first */  
  com = htonl(ET_NET_ALIVE);
  
  /* If ET system is NOT alive, or if ET system was killed and
   * restarted (breaking tcp connection), we'll get a write error.
   */
  et_tcp_lock(etid);
  if (tcp_write(sockfd, (void *) &com, sizeof(com)) != sizeof(com)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_alive, write error\n");
    }
    status = ET_ERROR_WRITE;
  }
  if (tcp_read(sockfd, &alive, sizeof(alive)) != sizeof(alive)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_alive, read error\n");
    }
    status = ET_ERROR_READ;
  }
  et_tcp_unlock(etid);

  /* if there's no communication error, return alive */
  if (status > 0) {
    return ntohl(alive);
  }
  /* Socket error, so now monitor hbeat thru shared memory.
   * Monitoring this way takes a couple seconds (1.5X the
   * time for system's heart to beat), so only do it when
   * socket communication fails
   */
  waittime.tv_sec  = 1.5*ET_BEAT_SEC;
  waittime.tv_nsec = 1.5*ET_BEAT_NSEC;
  if (waittime.tv_nsec > 1000000000L) {
    waittime.tv_nsec -= 1000000000L;
    waittime.tv_sec  += 1;
  }
    
  oldheartbt = etid->sys->heartbeat;
  nanosleep(&waittime, NULL);
  newheartbt = etid->sys->heartbeat;
  if (oldheartbt == newheartbt) {
    return 0;
  }  
  return 1;
}

/******************************************************/
int etn_wait_for_alive(et_sys_id id)
{
  struct timespec sleeptime;
  
  sleeptime.tv_sec  = 0;
  sleeptime.tv_nsec = 10000000;

  while (!etn_alive(id)) {
    nanosleep(&sleeptime, NULL);
  }
  return ET_OK;
} 

/******************************************************/
int etn_close(et_sys_id id)
{
  et_id *etid = (et_id *) id;
  
  /* unmap the shared memory */
  if (munmap(etid->pmap, etid->memsize) != 0) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_close, cannot unmap ET memory\n");
    }
    return ET_ERROR_REMOTE;
  }
  
  /* close network connection & free mem in "id" */
  return etr_close(id);
}

/******************************************************/
int etn_forcedclose(et_sys_id id)
{
  et_id *etid = (et_id *) id;
  
  /* unmap the shared memory */
  if (munmap(etid->pmap, etid->memsize) != 0) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_close, cannot unmap ET memory\n");
    }
    return ET_ERROR_REMOTE;
  }
  
  /* close network connection & free mem in "id" */
  return etr_forcedclose(id);
}

/******************************************************/
/*                   ET EVENT STUFF                   */
/******************************************************/
int etn_event_new(et_sys_id id, et_att_id att, et_event **ev,
		 int mode, struct timespec *deltatime, int size)
{
  et_id *etid = (et_id *) id;
  int sockfd = etid->sockfd;
  int err, transfer[6], incoming[2];
  void *pdata;
  
  /*
   * The command to the server is swapped so make sure it's sent in the
   * right byte order. The rest of the items are not swapped by the code.
   */
  transfer[0] = htonl(ET_NET_EV_NEW_L);
  transfer[1] = att;
  transfer[2] = mode;
  transfer[3] = size;
  transfer[4] = 0;
  transfer[5] = 0;
  if (deltatime) {
    transfer[4] = deltatime->tv_sec;
    transfer[5] = deltatime->tv_nsec;
  }
 
  et_tcp_lock(etid);
  if (tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_event_new, write error\n");
    }
    return ET_ERROR_WRITE;
  }
 
  if (tcp_read(sockfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_event_new, read error\n");
    }
    return ET_ERROR_READ;
  }
  et_tcp_unlock(etid);

  if ( (err = incoming[0]) != ET_OK) {
    return err;
  }
  
  /* pointers are in ET system space and must be translated */
  *ev = ET_PEVENT2USR(incoming[1], etid->offset);
  
  /* if not a temp event, data is in mem already mmapped */
  if ((*ev)->temp != ET_EVENT_TEMP) {
    /*(*ev)->pdata = ET_PDATA2USR((*ev)->pdata, etid->offset);*/
    (*ev)->pdata = ET_PDATA2USR((*ev)->data, etid->offset);
  }
  /* else, mmap file so we can get at data */
  else {
    /* store ET system's temp data pointer so it doesn't get lost */
    (*ev)->tempdata = (*ev)->pdata;
    /* attach to temp event mem */
    if ((pdata = et_temp_attach((*ev)->filename, (*ev)->memsize)) == NULL) {
      if (etid->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "etn_event_new, cannot attach to temp event\n");
      }
      return ET_ERROR_REMOTE;
    }
    (*ev)->pdata = pdata;
  }

  return ET_OK;
}

/******************************************************/
int etn_events_new(et_sys_id id, et_att_id att, et_event *evs[],
		 int mode, struct timespec *deltatime,
		 int size, int num, int *nread)
{
  et_id *etid = (et_id *) id;
  int sockfd = etid->sockfd;
  int i, nevents, err, transfer[7];
  void *pdata;
  
  transfer[0] = htonl(ET_NET_EVS_NEW_L);
  transfer[1] = att;
  transfer[2] = mode;
  transfer[3] = size;
  transfer[4] = num;
  transfer[5] = 0;
  transfer[6] = 0;
  
  if (deltatime) {
    transfer[5] = deltatime->tv_sec;
    transfer[6] = deltatime->tv_nsec;
  }
 
  et_tcp_lock(etid);
  if (tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_events_new, write error\n");
    }
    return ET_ERROR_WRITE;
  }
 
  if (tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_events_new, read error\n");
    }
    return ET_ERROR_READ;
  }

  if (err < 0) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_events_new, error in server\n");
    }
    return err;
  }
  
  nevents = err;
  size    = nevents*sizeof(et_event *);
  
  if (tcp_read(sockfd, (void *) evs, size) != size) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_events_new, read error\n");
    }
    return ET_ERROR_READ;
  }
  et_tcp_unlock(etid);
  
  /* pointers in ET system space and must be translated */
  for (i=0; i < nevents; i++) {
    evs[i] = ET_PEVENT2USR(evs[i], etid->offset);
    
    /* if not a temp event, data is in mem already mmapped */
    if (evs[i]->temp != ET_EVENT_TEMP) {
      /*evs[i]->pdata = ET_PDATA2USR(evs[i]->pdata, etid->offset);*/
      evs[i]->pdata = ET_PDATA2USR(evs[i]->data, etid->offset);
    }
    /* else, mmap file so we can get at data */
    else {
      /* store ET system's temp data pointer so it doesn't get lost */
      evs[i]->tempdata = evs[i]->pdata;
      /* attach to temp event mem */
      if ((pdata = et_temp_attach(evs[i]->filename, evs[i]->memsize)) == NULL) {
	if (etid->debug >= ET_DEBUG_ERROR) {
          et_logmsg("ERROR", "etn_events_new, cannot attach to temp event\n");
	}
	return ET_ERROR_REMOTE;
      }
      evs[i]->pdata = pdata;
    }
  }
  
  *nread = nevents;
  return ET_OK;
}

/******************************************************
 * NO SWAPPING IS NECESSARY 
 ******************************************************/
int etn_event_get(et_sys_id id, et_att_id att, et_event **ev,
		 int mode, struct timespec *deltatime)
{
  et_id *etid = (et_id *) id;
  int sockfd = etid->sockfd;
  int err, transfer[5], incoming[2];
  void *pdata;
 
  /* value of mode is checked and modified in et_event_get */
  
  transfer[0] = htonl(ET_NET_EV_GET_L);
  transfer[1] = att;
  transfer[2] = mode;
  transfer[3] = 0;
  transfer[4] = 0;
  if (deltatime) {
    transfer[3] = deltatime->tv_sec;
    transfer[4] = deltatime->tv_nsec;
  }
 
  et_tcp_lock(etid);
  if (tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_event_get, write error\n");
    }
    return ET_ERROR_WRITE;
  }
 
  if (tcp_read(sockfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_event_get, read error\n");
    }
    return ET_ERROR_READ;
  }
  et_tcp_unlock(etid);

  if ( (err = incoming[0]) != ET_OK) {
    return err;
  }
  
  /* pointers in ET system space and must be translated */
  *ev = ET_PEVENT2USR(incoming[1], etid->offset);
  
  /* if not a temp event, data is in mem already mmapped */
  if ((*ev)->temp != ET_EVENT_TEMP) {
    (*ev)->pdata = ET_PDATA2USR((*ev)->data, etid->offset);
  }
  /* else, mmap file so we can get at data */
  else {
    /* store ET system's temp data pointer so it doesn't get lost */
    (*ev)->tempdata = (*ev)->pdata;
    /* attach to temp event mem */
    if ((pdata = et_temp_attach((*ev)->filename, (*ev)->memsize)) == NULL) {
      if (etid->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "etn_event_get, cannot attach to temp event\n");
      }
      return ET_ERROR_REMOTE;
    }
    (*ev)->pdata = pdata;
  }
   
  return ET_OK;
}

/******************************************************/
int etn_events_get(et_sys_id id, et_att_id att, et_event *evs[],
		  int mode, struct timespec *deltatime, int num, int *nread)
{
  et_id *etid = (et_id *) id;
  int size;
  int sockfd = etid->sockfd;
  int i, nevents, err, transfer[6];
  void *pdata;
  
  /* value of mode is checked and modified in et_events_get */
  
  transfer[0] = htonl(ET_NET_EVS_GET_L);
  transfer[1] = att;
  transfer[2] = mode;
  transfer[3] = num;
  transfer[4] = 0;
  transfer[5] = 0;
  
  if (deltatime) {
    transfer[4] = deltatime->tv_sec;
    transfer[5] = deltatime->tv_nsec;
  }
 
  et_tcp_lock(etid);
  if (tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_events_get, write error\n");
    }
    return ET_ERROR_WRITE;
  }
 
  if (tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_events_get, read error\n");
    }
    return ET_ERROR_READ;
  }

  if (err < 0) {
    et_tcp_unlock(etid);
    return err;
  }
  
  nevents = err;
  size    = nevents*sizeof(et_event *);
  
  if (tcp_read(sockfd, (void *) evs, size) != size) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_events_get, read error\n");
    }
    return ET_ERROR_READ;
  }
  et_tcp_unlock(etid);
  
  /* these pointers are in ET system space and must be translated */
  for (i=0; i < nevents; i++) {
    evs[i] = ET_PEVENT2USR(evs[i], etid->offset);
    
    /* if not a temp event, data is in mem already mmapped */
    if (evs[i]->temp != ET_EVENT_TEMP) {
      evs[i]->pdata = ET_PDATA2USR(evs[i]->data, etid->offset);
    }
    /* else, mmap file so we can get at data */
    else {
      /* store ET system's temp data pointer so it doesn't get lost */
      evs[i]->tempdata = evs[i]->pdata;
      /* attach to temp event mem */
      if ((pdata = et_temp_attach(evs[i]->filename, evs[i]->memsize)) == NULL) {
	if (etid->debug >= ET_DEBUG_ERROR) {
          et_logmsg("ERROR", "etn_events_get, cannot attach to temp event\n");
	}
	return ET_ERROR_REMOTE;
      }
      evs[i]->pdata = pdata;
    }
  }
  
  *nread = nevents;
  return ET_OK;
}

/******************************************************/
int etn_event_put(et_sys_id id, et_att_id att, et_event *ev)
{
  et_id *etid = (et_id *) id;
  int sockfd = etid->sockfd;
  int err=ET_OK, transfer[3];

  /* If temp buffer, unmap it from this process' mem.
   * Restore old values for data pointer.
   */
  if (ev->temp != ET_EVENT_TEMP) {
    ev->pdata = ET_PDATA2ET(ev->pdata, etid->offset);
  }
  else {
    if (munmap(ev->pdata, ev->memsize) != 0) {
      if (etid->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "etn_event_put, error in munmap\n");
      }
      return ET_ERROR_REMOTE;
    }
    ev->pdata = ev->tempdata;
  }
  
  transfer[0] = htonl(ET_NET_EV_PUT_L);
  transfer[1] = att;
  /* translate event pointer back into ET system's space */
  transfer[2] = (int) ET_PEVENT2ET(ev, etid->offset);
  

  /* write event */
  et_tcp_lock(etid);
  if (tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_event_put, write error\n");
    }
    /* undo what was done (not possible for temps as mem already unmapped) */
    if (ev->temp != ET_EVENT_TEMP) {
      ev->pdata = ET_PDATA2USR(ev->pdata, etid->offset);
    }
    return ET_ERROR_WRITE;
  }
  
  if (tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_event_put: read error\n");
    }
    /* undo what was done (not possible for temps as mem already unmapped) */
    if (ev->temp != ET_EVENT_TEMP) {
      ev->pdata = ET_PDATA2USR(ev->pdata, etid->offset);
    }
    return ET_ERROR_READ;
  }
  et_tcp_unlock(etid);
    
  return err;
}

/******************************************************/
int etn_events_put(et_sys_id id, et_att_id att, et_event *evs[], int num)
{
  et_id *etid = (et_id *) id;
  int sockfd = etid->sockfd;
  int i, err=ET_OK, transfer[3], *events;
  struct iovec iov[2];

  /* for translating event pointers back into ET system's space */
  if ( (events = (int *) malloc(num*sizeof(int))) == NULL) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_events_put, cannot allocate memory\n");
    }
    return ET_ERROR_REMOTE;
  }
  
  /* (1) If temp buffer, unmap it from this process' mem.
   * (2) Restore old values for data pointer.
   * (3) Translate event pointers back into ET system's space.
   */
  for (i=0; i < num ; i++) {
    events[i] = (int) ET_PEVENT2ET(evs[i], etid->offset);
    
    if (evs[i]->temp != ET_EVENT_TEMP) {
      evs[i]->pdata = ET_PDATA2ET(evs[i]->pdata, etid->offset);
    }
    else {
      if (munmap(evs[i]->pdata, evs[i]->memsize) != 0) {
        if (etid->debug >= ET_DEBUG_ERROR) {
          et_logmsg("ERROR", "etn_events_put, error in munmap\n");
	}
	free(events);
        return ET_ERROR_REMOTE;
      }
      evs[i]->pdata = evs[i]->tempdata;
    }
  }
  
  
  transfer[0] = htonl(ET_NET_EVS_PUT_L);
  transfer[1] = att;
  transfer[2] = num;
  
  iov[0].iov_base = (void *) transfer;
  iov[0].iov_len  = sizeof(transfer);

  iov[1].iov_base = (void *) events;
  iov[1].iov_len  = num*sizeof(int);

  et_tcp_lock(etid);
  if (tcp_writev(sockfd, iov, 2, 16) == -1) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_events_put, write error\n");
    }
    
    /* undo what was done (not possible for temps as mem already unmapped) */
    for (i=0; i < num; i++) {
      if (evs[i]->temp != ET_EVENT_TEMP) {
        evs[i]->pdata = ET_PDATA2USR(evs[i]->pdata, etid->offset);
      }
    }
    
    free(events);
    return ET_ERROR_WRITE;
  }

  if (tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_events_put, read error\n");
    }
    
    /* undo what was done (not possible for temps as mem already unmapped) */
    for (i=0; i < num; i++) {
      if (evs[i]->temp != ET_EVENT_TEMP) {
        evs[i]->pdata = ET_PDATA2USR(evs[i]->pdata, etid->offset);
      }
    }
    
    free(events);
    return ET_ERROR_READ;
  }
  et_tcp_unlock(etid);

  free(events);
  return err;
}

/******************************************************/
int etn_event_dump(et_sys_id id, et_att_id att, et_event *ev)
{
  et_id *etid = (et_id *) id;
  int sockfd = etid->sockfd;
  int err=ET_OK, transfer[3];

  /* If temp buffer, unmap it from this process' mem.
   * Restore old values for data pointer.
   */
  if (ev->temp != ET_EVENT_TEMP) {
    ev->pdata = ET_PDATA2ET(ev->pdata, etid->offset);
  }
  else {
    if (munmap(ev->pdata, ev->memsize) != 0) {
      if (etid->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "etn_event_dump, error in munmap\n");
      }
      return ET_ERROR_REMOTE;
    }
    ev->pdata = ev->tempdata;
  }

  transfer[0] = htonl(ET_NET_EV_DUMP_L);
  transfer[1] = att;
  /* translate pointers back into ET system's space */
  transfer[2] = (int) ET_PEVENT2ET(ev, etid->offset);
  

  /* write event */
  et_tcp_lock(etid);
  if (tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_event_dump, write error\n");
    }
    /* undo what was done (not possible for temps as mem already unmapped) */
    if (ev->temp != ET_EVENT_TEMP) {
      ev->pdata = ET_PDATA2USR(ev->pdata, etid->offset);
    }
    return ET_ERROR_WRITE;
  }
  
  if (tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_event_dump: read error\n");
    }
    /* undo what was done (not possible for temps as mem already unmapped) */
    if (ev->temp != ET_EVENT_TEMP) {
      ev->pdata = ET_PDATA2USR(ev->pdata, etid->offset);
    }
    return ET_ERROR_READ;
  }
  et_tcp_unlock(etid);
    
  return err;
}

/******************************************************/
int etn_events_dump(et_sys_id id, et_att_id att, et_event *evs[], int num)
{
  et_id *etid = (et_id *) id;
  int sockfd = etid->sockfd;
  int i, err=ET_OK, transfer[3], *events;
  struct iovec iov[2];

  /* translate pointers back into ET system's space */
  if ( (events = (int *) malloc(num*sizeof(int))) == NULL) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_events_dump, cannot allocate memory\n");
    }
    return ET_ERROR_REMOTE;
  }
  
  /* (1) If temp buffer, unmap it from this process' mem.
   * (2) Restore old values for data pointer.
   * (3) Translate event pointers back into ET system's space.
   */
  for (i=0; i < num ; i++) {
    events[i] = (int) ET_PEVENT2ET(evs[i], etid->offset);
    
    if (evs[i]->temp != ET_EVENT_TEMP) {
      evs[i]->pdata = ET_PDATA2ET(evs[i]->pdata, etid->offset);
    }
    else {
      if (munmap(evs[i]->pdata, evs[i]->memsize) != 0) {
        if (etid->debug >= ET_DEBUG_ERROR) {
          et_logmsg("ERROR", "etn_events_dump, error in munmap\n");
	}
	free(events);
        return ET_ERROR_REMOTE;
      }
      evs[i]->pdata = evs[i]->tempdata;
    }
  }
  
  
  transfer[0] = htonl(ET_NET_EVS_DUMP_L);
  transfer[1] = att;
  transfer[2] = num;
  
  iov[0].iov_base = (void *) transfer;
  iov[0].iov_len  = sizeof(transfer);

  iov[1].iov_base = (void *) events;
  iov[1].iov_len  = num*sizeof(int);

  et_tcp_lock(etid);
  if (tcp_writev(sockfd, iov, 2, 16) == -1) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_events_dump, write error\n");
    }
    
    /* undo what was done (not possible for temps as mem already unmapped) */
    for (i=0; i < num; i++) {
      if (evs[i]->temp != ET_EVENT_TEMP) {
        evs[i]->pdata = ET_PDATA2USR(evs[i]->pdata, etid->offset);
      }
    }
    
    free(events);
    return ET_ERROR_WRITE;
  }

  if (tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
    et_tcp_unlock(etid);
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "etn_events_dump, read error\n");
    }
    
    /* undo what was done (not possible for temps as mem already unmapped) */
    for (i=0; i < num; i++) {
      if (evs[i]->temp != ET_EVENT_TEMP) {
        evs[i]->pdata = ET_PDATA2USR(evs[i]->pdata, etid->offset);
      }
    }
    
    free(events);
    return ET_ERROR_READ;
  }
  et_tcp_unlock(etid);

  free(events);
  return err;
}

