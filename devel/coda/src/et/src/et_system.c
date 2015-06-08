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
 *      Routines for starting an ET system or called by the ET system
 *
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <dlfcn.h>
#ifdef sun
#include <thread.h>
#endif

#include "et_private.h"
#include "et_network.h"

/* Time intervals to wait in seconds for threads to start */
#define ET_WAIT_FOR_THREADS 10
#define DEBUG 1

/* prototypes */

/* init routines done once in ET system */
static void  et_init_mem_sys(et_id *id, et_sys_config *config);
static void  et_init_mem_station(et_id *id);
static void  et_init_mem_event(et_id *id);

/* routines running as threads */
static void *et_add_stations(void *arg);
static void *et_sys_heartbeat(void *arg);
static void *et_sys_heartmonitor(void *arg);
static void *et_conductor(void *arg);
static void et_sys_stopthread(pthread_t pt);

/* misc. routines */
static int   et_grandcentral_station_create(et_id *id);
static int   et_condition(et_sys_id id, et_stat_id stat_id, et_event *pe);
static int   et_system_config_check(et_id *id, et_sys_config *config);
static void  shellSort(int n, int a[], int b[]);

/* fix et system routines */
static void  et_fix_mutexes(et_id *id);
static void  et_fix_linkedlist(et_id *id);
static void  et_fix_stations(et_id *id);
static void  et_fix_attachments(et_id *id);
static void  et_fix_natts(et_id *id);
static void  et_fix_nprocs(et_id *id);
static void  et_fix_system(et_id *id);


int et_system_start (et_sys_id* id, et_sysconfig sconfig)
{
  size_t          size, size_old_used, size_events, size_data,
		  size_system, size_stations, size_histo;
  int             i, status, status_old, num_try, try_max,
		  creating=0;
  unsigned int    hbeat;
#ifdef sun
  int		  con, con_add;
#endif
  pthread_attr_t  attr;
  pthread_t       thd_id;
  struct timespec waitforme, monitor, beat;
  et_sys_config  *config = (et_sys_config *) sconfig;
  et_mem         *first_item;
  et_id          *etid;
  et_event       *pe;
  et_station     *ps;
  et_netthread	 threadarg;
  sigset_t       sigblock, oldset;

  /* set some useful timeout periods */
  waitforme.tv_sec  = 0;
  waitforme.tv_nsec = 10000000; /* 10 millisec */
  beat.tv_sec       = ET_BEAT_SEC + 2;
  beat.tv_nsec      = ET_BEAT_NSEC;
  monitor.tv_sec    = 2*ET_MON_SEC;
  monitor.tv_nsec   = 2*ET_MON_NSEC;
  if (monitor.tv_nsec >= 1000000000) {
    monitor.tv_sec += monitor.tv_nsec/1000000000 + 1;
    monitor.tv_nsec = 0;
  }

#ifdef sun
  /*
   * want one thread for each station's conductor + extra threads
   * for heartbeat production and detection + 1 for main thd +
   * 1 for add stations thd + 2 for udp & tcp threads. However,
   * if we exceed system resources, we'll need to try something
   * more conservative.
   */
  con = thr_getconcurrency();
  if (thr_setconcurrency(con + config->nstations + ET_EXTRA_THREADS + 4) != 0) {
    /* exceeded # of threads allowed so reduce and try again */
    if (thr_setconcurrency(config->nstations + ET_EXTRA_THREADS + 4) != 0) {
      /* exceeded # of threads allowed so try fixed number */
      if (thr_setconcurrency(20) != 0) {
        /* exceeded # of threads allowed so let system choose */
	thr_setconcurrency(0);
      }
    }
  }
  con_add = thr_getconcurrency() - con;
  if (con_add < 1) {
    con_add = 0;
  }
#endif

  /* get thread attribute ready */
  status = pthread_attr_init(&attr);
  if(status != 0) {
    err_abort(status, "Init thd attr");
  }
  status = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if(status != 0) {
    err_abort(status, "Set thd detach");
  }
  
  /*********************/
  /* create & init mem */
  /*********************/

  et_id_init(id);
  etid = (et_id *) *id;
  
  /* register total # of events & their size */
  etid->nevents = config->nevents;
  etid->esize   = config->event_size;  
  /* set debug level for this routine (default = ERROR) */
  /*etid->debug = ET_DEBUG_ERROR;*/
  etid->debug = 4;  

  if (et_system_config_check(etid, config) != ET_OK) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_start, bad system configuration parameter(s)\n");
    }
    pthread_attr_destroy(&attr);
    et_id_destroy(*id);
    return ET_ERROR;
  }

  /*
   * The mapped memory consists of 5 regions:
   *	The first holds the et_system structure with all system info.
   *	The second contains the et_station structures with all the
   *		station info.
   *	The third contains all the et_event structures which are
   *		essentially the event headers.
   *	The fourth contains a histogram of the number of events
   *		available in GrandCentral station's input list
   *		when producers ask for new events
   *	The last holds all data associated with the events.
   *    (Actually, the first item in the mapped mem is its size, but
   *	 that is automatically taken care of in et_mem_create).
   */
  size_system   = sizeof(et_system);
  size_stations = sizeof(et_station) * config->nstations;
  size_histo    = sizeof(int) * (config->nevents + 1);
  size_events   = sizeof(et_event)   * config->nevents;
  size_data     = config->event_size * config->nevents;
  size = size_system + size_stations + size_histo + size_events + size_data;
  
  /* create the ET system memory */
  status = et_mem_create(config->filename, size, (void **) &first_item);
  
  /* if the ET system file already exists ... */
  if (status == ET_ERROR_EXISTS) {
    /* First look at the existing ET system to make sure it's what we want. 
     * To do that, we need to map the existing file to this process to
     * access all the data in the system.
     */
    first_item = (et_mem *) et_mem_attach(config->filename);
    if (first_item == NULL) {
      if (etid->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "et_system_start, can't attach to existing ET system\n");
      }
      pthread_attr_destroy(&attr);
      et_id_destroy(*id);
      return ET_ERROR;
    }
    
    /* find size of mapped memory */
    if (et_mem_size(config->filename, &etid->memsize, NULL) != ET_OK) {
      if (etid->debug >= ET_DEBUG_ERROR) {
	et_logmsg("ERROR", "et_system_start: cannot find size of ET system file\n");
      }
      pthread_attr_destroy(&attr);
      et_id_destroy(*id);
      return ET_ERROR;
    }
  
    size_old_used = first_item->usedsize;
    etid->sys = (et_system *) (first_item + 1);
    
    /* Since we now have access to all the ET system information that
     * we need, the first order of business is to check and see if,
     * in fact, an ET system process has already taken up residence
     * here. If it has, only one ET system process per file. So
     * return an error.
     */
    hbeat = etid->sys->heartbeat;
    nanosleep(&beat,NULL);
    if (hbeat != etid->sys->heartbeat) {
      /* ET system process already exists */
      if (etid->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "et_system_start, ET system process already exists!\n");
      }
      pthread_attr_destroy(&attr);
      munmap((void *) first_item, etid->memsize);
      et_id_destroy(*id);
      return ET_ERROR;
    }
      
    /* if trying to create ET system of different size, shape, ... */
    if ((size                   != size_old_used)                 ||
        (config->nstations      != etid->sys->config.nstations)   ||
        (config->nevents        != etid->sys->config.nevents)     ||
        (config->event_size     != etid->sys->config.event_size)  ||
	(config->nprocesses      < etid->sys->config.nprocesses)  ||
	(config->nattachments    < etid->sys->config.nattachments)||
        (ET_STATION_SELECT_INTS != etid->sys->nselects))            {
	
      /* The existing ET system is not what we want! First find out 
       * whether there are active clients on this system.
       */
      int have_client=0;
      for (i=0; i < etid->sys->config.nprocesses ; i++) {
        if (etid->sys->proc[i].status == ET_PROC_OPEN) {
	  hbeat = etid->sys->proc[i].heartbeat;
	  nanosleep(&beat,NULL);
	  if (hbeat != etid->sys->proc[i].heartbeat) {
	    /* this process is alive */
	    have_client++;
	    break;
	  }
	}
      }
      
      if (have_client) {
        if (etid->debug >= ET_DEBUG_ERROR) {
          et_logmsg("ERROR", "et_system_start, existing ET system has a different configuration\n");
          et_logmsg("ERROR", "                 and still has active clients.\n");
       }
      }
      else {
        if (etid->debug >= ET_DEBUG_ERROR) {
          et_logmsg("ERROR", "et_system_start, existing ET system has a different configuration\n");
          et_logmsg("ERROR", "                 and no clients, delete the file and try again.\n");
        }
      }
      
      pthread_attr_destroy(&attr);
      munmap((void *) first_item, etid->memsize);
      et_id_destroy(*id);
      return ET_ERROR;
    }
    
    /* if trying to create an identical ET system, ... */
    else {
      if (etid->debug >= ET_DEBUG_WARN) {
        et_logmsg("WARN", "et_system_start, can't create ET system mem, so attaching to it\n");
      }
      /*
       * Wait over one monitor period to make sure all clients detect ET death
       * & adjust themselves before re-initializing the mapped memory.
       */
      nanosleep(&monitor, NULL);
      
      etid->offset    = 0;
      etid->pmap      = (void *)       (first_item);
      etid->sys       = (et_system *)  (first_item + 1);
      etid->stats     = (et_station *) (etid->sys + 1);
      etid->histogram = (int *)        (etid->stats + config->nstations);
      etid->events    = (et_event *)   (etid->histogram + (config->nevents + 1));
      etid->data      = (char *)       (etid->events + config->nevents);
      etid->grandcentral = etid->stats;
      
      if (etid->debug >= ET_DEBUG_INFO) {
        et_logmsg("INFO", "et_system_start, ET map   : ptr = %p\n", etid->pmap);
        et_logmsg("INFO", "et_system_start, ET sys   : ptr = %p, size = %d\n", etid->sys, size_system);
        et_logmsg("INFO", "et_system_start, ET stats : ptr = %p, size = %d\n", etid->stats, size_stations);
        et_logmsg("INFO", "et_system_start, ET histo : ptr = %p, size = %d\n", etid->histogram, size_histo);
        et_logmsg("INFO", "et_system_start, ET events: ptr = %p, size = %d\n", etid->events, size_events);
        et_logmsg("INFO", "et_system_start, ET data  : ptr = %p, size = %d\n", etid->data, size_data);
      }

      /* store system config info */
      etid->sys->config = *config;
      /* Fix any inconsistancies and unresolved problems from a possible
       * previous ET system crash (and perhaps client crashes as well).
       */
      et_fix_system(etid);
      /* This is used to translate ET pointers between processes.*/
      etid->sys->pmap = (void *) first_item;
#ifdef sun
      /* keep track of thread concurrency increase */
      etid->sys->con_add = con_add;
#endif

      /* Fill grand_central station input list with all events */
      pe = etid->events;
      for (i=0 ; i < etid->sys->config.nevents ; i++) {
        if (et_llist_write_gc(etid, &pe, 1) != ET_OK) {
          if (etid->debug >= ET_DEBUG_SEVERE) {
            et_logmsg("SEVERE", "et_system_start, error filling grandcentral station\n") ;
          }
 	  pthread_attr_destroy(&attr);
          munmap(etid->pmap, etid->memsize);
          et_id_destroy(*id);
          return ET_ERROR;
        }
        pe++;
      }
      /* Do not include this refilling of GrandCentral with events in
       * the general statistics keeping 
       */
      etid->grandcentral->list_in.events_in =
         et_bigint_sub(etid->grandcentral->list_in.events_in, etid->sys->config.nevents);

      /* wait up to ET_WAIT_FOR_THREADS seconds for a thread to start */
      try_max = (etid->sys->hz)*ET_WAIT_FOR_THREADS;
      waitforme.tv_sec  = 0;
      waitforme.tv_nsec = 1000000000/(etid->sys->hz);

      /* Add "conductor" threads for all active/idle stations */
      for (ps = etid->grandcentral + etid->sys->stat_head;; ps = etid->grandcentral + ps->next) {
        /* keep track of current station status */
        status_old = ps->data.status;
        /* fool system into thinking that we are creating each station */
        ps->data.status = ET_STATION_CREATING;
        status = pthread_create(&thd_id, &attr, et_conductor, (void *) etid);
        if (status != 0) {
          err_abort(status, "Create et_conductor thd");
        }
	/* wait until conductor thread has started */
	num_try = 0;
        while ((ps->data.status != ET_STATION_IDLE)   &&
	       (ps->data.status != ET_STATION_ACTIVE) &&
	       (num_try++ < try_max)) {
          nanosleep(&waitforme, NULL);
        }
        if (num_try > try_max) {
          if (etid->debug >= ET_DEBUG_SEVERE) {
            et_logmsg("SEVERE", "et_system_start, CANNOT start conductor\n");
          }
	  pthread_attr_destroy(&attr);
          munmap(etid->pmap, etid->memsize);
          et_id_destroy(*id);
          return ET_ERROR;
        }
	
	/* restore original value of station's status */
        ps->data.status = status_old;
	
        if ((ps->num == etid->sys->stat_tail) || (ps->next < 0)) {
          break;
        }
      }
    }
    
    if (etid->debug >= ET_DEBUG_INFO) {
      et_logmsg("INFO", "et_system_start, Restarting ET system ...\n");
    }
  }
  /* general error trying to create mapped memory */
  else if (status != ET_OK) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_start, can't create or attach to ET system\n");
    }
    pthread_attr_destroy(&attr);
    et_id_destroy(*id);
    return ET_ERROR;
  }
  /* everything's OK so start up a new ET system */
  else if (status == ET_OK) {
    creating = 1;

    /* find size of mapped memory */
    if (et_mem_size(config->filename, &etid->memsize, NULL) != ET_OK) {
      if (etid->debug >= ET_DEBUG_ERROR) {
	et_logmsg("ERROR", "et_system_start: cannot find size of ET system file\n");
      }
      pthread_attr_destroy(&attr);
      unlink(config->filename);
      et_id_destroy(*id);
      return ET_ERROR;
    }

    etid->offset    = 0;
    etid->pmap      = (void *)       (first_item);
    etid->sys       = (et_system *)  (first_item + 1);
    etid->stats     = (et_station *) (etid->sys + 1);
    etid->histogram = (int *)        (etid->stats + config->nstations);
    etid->events    = (et_event *)   (etid->histogram + (config->nevents + 1));
    etid->data      = (char *)       (etid->events + config->nevents);
    etid->grandcentral = etid->stats;

    if (etid->debug >= ET_DEBUG_INFO) {
      et_logmsg("INFO", "et_system_start, ET map   : ptr = %p\n", etid->pmap);
      et_logmsg("INFO", "et_system_start, ET sys   : ptr = %p, size = %d\n", etid->sys, size_system);
      et_logmsg("INFO", "et_system_start, ET stats : ptr = %p, size = %d\n", etid->stats, size_stations);
      et_logmsg("INFO", "et_system_start, ET histo : ptr = %p, size = %d\n", etid->histogram, size_histo);
      et_logmsg("INFO", "et_system_start, ET events: ptr = %p, size = %d\n", etid->events, size_events);
      et_logmsg("INFO", "et_system_start, ET data  : ptr = %p, size = %d\n", etid->data, size_data);
    }
 
    /* initialize et_system mem, mutexes. Do this first */
    et_init_mem_sys(etid, config);
    /* This is used to translate ET pointers between processes.*/
    etid->sys->pmap = (void *) first_item;
    
#ifdef sun
    /* keep track of thread concurrency increase */
    etid->sys->con_add = con_add;
#endif

    /* initialize station mem, llist & station mutexes */
    et_init_mem_station(etid);
 
    /* initialize histogram mem */
    et_init_histogram(etid);
    
    /* initialize event mem and data */
    et_init_mem_event(etid);

    /* make the station that stores all events */
    if (et_grandcentral_station_create(etid) < 0) {
      if (etid->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "et_system_start, cannot create grandcentral station\n");
      }
      pthread_attr_destroy(&attr);
      munmap(etid->pmap, etid->memsize);
      unlink(config->filename);
      et_id_destroy(*id);
      goto error;
    }
  }
  
  /************************************************
   *      Start system threads.      
   *      Same code from here on whether starting
   *      from scratch or restarting.
   ************************************************/
 
  /* setup signal handling for all threads, block all signals except cntrl-C */
  sigfillset(&sigblock);
  sigdelset(&sigblock, SIGINT);
  pthread_sigmask(SIG_BLOCK, &sigblock, &oldset);
  
  /* wait up to ET_WAIT_FOR_THREADS seconds for a thread to start */
  try_max = (etid->sys->hz)*ET_WAIT_FOR_THREADS;
  waitforme.tv_sec  = 0;
  waitforme.tv_nsec = 1000000000/(etid->sys->hz);
  num_try = 0;
  
  /* Thread to monitor heartbeats of attaching processes */
  status = pthread_create(&etid->sys->tid_hm, NULL, et_sys_heartmonitor, (void *) etid);
  if (status != 0) {
    err_abort(status, "Create Thread");
  }
  while((etid->race != -1) && (num_try++ < try_max)) {
    nanosleep(&waitforme, NULL);
  }
  if (num_try > try_max) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_start, cannot start heartmonitor thread\n");
    }
    goto error;
  }
  etid->race = 0;
  
  /* Thread to provide a heartbeat */
  status = pthread_create(&etid->sys->tid_hb, NULL, et_sys_heartbeat, (void *) etid);
  if (status != 0) {
    err_abort(status, "Create Thread");
  }
  num_try = 0;
  while((etid->race != -1) && (num_try++ < try_max)) {
    nanosleep(&waitforme, NULL);
  }
  if (num_try > try_max) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_start, cannot start heartbeat thread\n");
    }
    et_sys_stopthread(etid->sys->tid_hm);
    goto error;
  }
  etid->race = 0;
   
  /* thread to watch for addition of new stations */
  status = pthread_create(&etid->sys->tid_as, &attr, et_add_stations, (void *) etid);
  if (status != 0) {
    err_abort(status, "Create Thread");
  }
  num_try = 0;
  while ((etid->race != -1) && (num_try++ < try_max)) {
    nanosleep(&waitforme, NULL);
  }
  if (num_try > try_max) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_start, cannot start add_station thread\n");
    }
    et_sys_stopthread(etid->sys->tid_hm);
    et_sys_stopthread(etid->sys->tid_hb);
    pthread_attr_destroy(&attr);
    goto error;
  }
  etid->race = 0;

  /* set argument to pass to network threads */
  threadarg.id     = etid;
  threadarg.config = &etid->sys->config;
  
  /* run ET remote server as thread */
  status = pthread_create(&etid->sys->tid_srv, &attr, et_netserver, (void *) &threadarg);
  if (status != 0) {
    err_abort(status, "Create Thread");
  }
  num_try = 0;
  while ((etid->race != -1) && (num_try++ < try_max)) {
    nanosleep(&waitforme, NULL);
  }
  if (num_try > try_max) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_start, cannot start et_netserver thread\n");
    }
    et_sys_stopthread(etid->sys->tid_hm);
    et_sys_stopthread(etid->sys->tid_hb);
    et_sys_stopthread(etid->sys->tid_as);
    pthread_attr_destroy(&attr);
    goto error;
  }
  etid->race = 0;

  /* thread so clients can find server port thru broad/multicast */
  status = pthread_create(&etid->sys->tid_mul, &attr, et_cast_thread, (void *) &threadarg);
  if (status != 0) {
    err_abort(status, "Create Thread");
  }
  num_try = 0;
  while ((etid->race != -1) && (num_try++ < try_max)) {
    nanosleep(&waitforme, NULL);
  }
  if (num_try > try_max) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_start, cannot start et_cast_thread thread\n");
    }
    et_sys_stopthread(etid->sys->tid_hm);
    et_sys_stopthread(etid->sys->tid_hb);
    et_sys_stopthread(etid->sys->tid_as);
    et_sys_stopthread(etid->sys->tid_srv);
    pthread_attr_destroy(&attr);
    goto error;
  }
  etid->race = 0;
  
  /* Release some resources */
  status = pthread_attr_destroy(&attr);
  if(status != 0) {
    err_abort(status, "Thread attr destroy");
  }
  
  /* set signal handling back to the way it was in calling thread */
  pthread_sigmask(SIG_SETMASK, &oldset, NULL);
  
  /* set debug level to ERROR as a default for the system */
  etid->debug = ET_DEBUG_ERROR;
  /* we're officially an alive ET system! */
  etid->alive = 1;
  return ET_OK;
  
  error:
    pthread_attr_destroy(&attr);
    if (creating) {
      munmap(etid->pmap, etid->memsize);
      unlink(config->filename);
    }
    else {
      munmap(etid->pmap, etid->memsize);
    }
    et_id_destroy(*id);
    /* set signal handling back to the way it was in calling thread */
    pthread_sigmask(SIG_SETMASK, &oldset, NULL);
    return ET_ERROR;
}

/******************************************************/
int et_system_close(et_sys_id id)
{
  int             i;
  et_id           *etid = (et_id *) id;
  et_station      *ps  = etid->grandcentral;
  et_sys_config   config = etid->sys->config;
  et_list         *pl;
  struct timespec sometime;
  
  /* only the ET system process can call this routine */
  if (etid->proc != ET_SYS) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_close, not ET system process\n");
    }
    return ET_ERROR;
  }
  
#ifdef sun
    {
      int con, con_add = etid->sys->con_add;
    
      con = thr_getconcurrency();
      if (con < con_add + 1) {
        con = 1;
      }
      else {
        con -= con_add;
      }
      thr_setconcurrency(con);
    }
#endif
  
  /* As of Redhat 6.2, the LinuxThreads library is still not integrated
   * enough with the C library so that system calls that block (such as
   * read, write, accept, etc) are pthread cancellation points. Thus many
   * of the following threads which are probably stuck on such statements
   * may not be cancelled in Linux. In tests these threads do seem to be
   * canceled, however, the surviving threads seem to be "messed up".
   */
  
  /* stop tcp server thread */
  et_sys_stopthread(etid->sys->tid_srv);
  
  /* stop broad/multicast listening threads */
  for (i=0; i < config.ifaddrs.count; i++) {
    et_sys_stopthread(config.ifaddrs.tid[i]);
  }
  for (i=0; i < config.mcastaddrs.count; i++) {
    et_sys_stopthread(config.mcastaddrs.tid[i]);
  }
  for (i=0; i < config.subnets.count; i++) {
    et_sys_stopthread(config.subnets.tid[i]);
  }

  /* stop heartbeat & heartmonitor threads */
  et_sys_stopthread(etid->sys->tid_hb);
  et_sys_stopthread(etid->sys->tid_hm);
  
  /* stop add station thread */
  etid->sys->asthread = ET_THREAD_KILL;
  pthread_cond_signal(&etid->sys->statadd);
  
  /* wait for all event transfers to stop by waiting past 1 monitor period */
  sometime.tv_sec  = ET_MON_SEC + 2;
  sometime.tv_nsec = ET_MON_NSEC;
  nanosleep(&sometime, NULL);
  
  /* stop all those conductor threads */
  for (i=0; i < etid->sys->config.nstations; i++) {
    if (ps->data.status != ET_STATION_UNUSED) {
      ps->conductor = ET_THREAD_KILL;
      pl = &ps->list_out;
      pthread_cond_signal(&pl->cread);
    }
    ps++;
  }

  /* unmap ET memory */
  if (munmap(etid->pmap, etid->memsize) != 0) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_close, cannot unmap ET memory\n");
    }
  }
  et_id_destroy(id);
  
  return ET_OK;
}

/*****************************************************/
static void et_init_mem_sys(et_id *id, et_sys_config *config)
{
  int        i;
  et_system  *sys = id->sys;
  
#ifndef MUTEX_NOSHARE
  int status;
  pthread_mutexattr_t mattr;
  pthread_condattr_t  cattr;

  /* set attribute for mutex & condition variable multiprocess sharing */
  status = pthread_mutexattr_init(&mattr);
  if(status != 0) {
    err_abort(status, "et_init_mem_sys");
  }
  
  status = pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
  if(status != 0) {
    err_abort(status, "et_init_mem_sys");
  }
  
  status = pthread_condattr_init(&cattr);
  if(status != 0) {
    err_abort(status, "et_init_mem_sys");
  }
  
  status = pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
  if(status != 0) {
    err_abort(status, "et_init_mem_sys");
  }
  
  /* initialize mutex's & cv's in shared mem, ONLY ONCE! */
  pthread_mutex_init(&sys->mutex, &mattr);
  pthread_mutex_init(&sys->stat_mutex, &mattr);
  pthread_mutex_init(&sys->statadd_mutex, &mattr);
  pthread_cond_init (&sys->statadd, &cattr);
  pthread_cond_init (&sys->statdone,  &cattr);
#else
  pthread_mutex_init(&sys->mutex, NULL);
  pthread_mutex_init(&sys->stat_mutex, NULL);
  pthread_mutex_init(&sys->statadd_mutex, NULL);
  pthread_cond_init (&sys->statadd, NULL);
  pthread_cond_init (&sys->statdone,  NULL);
#endif

  sys->version      = ET_VERSION;
  sys->nselects     = ET_STATION_SELECT_INTS;
  sys->hz           = sysconf(_SC_CLK_TCK);
  sys->asthread     = ET_THREAD_KEEP;
#ifdef sun
  sys->con_add	    = 0;
#endif
  sys->pmap	    = NULL;
  sys->heartbeat    = 0;
  sys->mainpid      = getpid();
  sys->nstations    = 0;
  sys->ntemps       = 0;
  sys->nprocesses   = 0;
  sys->nattachments = 0;
  
  sys->port	    = 0;
 *sys->host	    = '\0';
  if (et_defaulthost(sys->host, ET_MAXHOSTNAMELEN) != ET_OK) {
    if (id->debug >= ET_DEBUG_SEVERE) {
      et_logmsg("SEVERE", "et_init_mem_sys: cannot find hostname\n");
    }
    exit(-1);
  }
  
  sys->stat_head    = ET_GRANDCENTRAL;
  sys->stat_tail    = ET_GRANDCENTRAL;
  
  for (i=0; i < ET_PROCESSES_MAX; i++) {
    et_init_process(sys, (et_proc_id) i);
  }
  for (i=0; i < ET_ATTACHMENTS_MAX; i++) {
    et_init_attachment(sys, (et_att_id) i);
  }
  
  /* store system config info */
  sys->config = *config;
  
#ifndef MUTEX_NOSHARE
  /* Release resources */
  status = pthread_condattr_destroy(&cattr);
  if(status != 0) {
    err_abort(status, "Condition attr destroy");
  }
  status = pthread_mutexattr_destroy(&mattr);
  if(status != 0) {
    err_abort(status, "Mutex attr destroy");
  }
#endif

}

/*****************************************************/
/* pass in ptr to first station to init all stations */

static void et_init_mem_station(et_id *id)
{
  int         i;
  et_system   *sys = id->sys;
  et_station  *ps  = id->grandcentral;
  
#ifndef MUTEX_NOSHARE
  int status;
  pthread_mutexattr_t mattr;
  pthread_condattr_t  cattr;

  /* set attribute for mutex & condition variable multiprocess sharing */
  status = pthread_mutexattr_init(&mattr);
  if(status != 0) {
    err_abort(status, "et_init_mem_station");
  }
  
  status = pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
  if(status != 0) {
    err_abort(status, "et_init_mem_station");
  }
  
  status = pthread_condattr_init(&cattr);
  if(status != 0) {
    err_abort(status, "et_init_mem_station");
  }
  
  status = pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
  if(status != 0) {
    err_abort(status, "et_init_mem_station");
  }
  
  /* initialize all mutexes & cv's in shared mem, ONLY ONCE! */
  for (i=0; i < sys->config.nstations; i++) {
    /* both lists in every station */
    pthread_mutex_init(&ps->list_in.mutex,  &mattr);
    pthread_mutex_init(&ps->list_out.mutex, &mattr);
    pthread_cond_init (&ps->list_in.cread,  &cattr);
    pthread_cond_init (&ps->list_out.cread, &cattr);
    et_init_station(ps);
    pthread_mutex_init(&ps->mutex, &mattr);
    ps++;
  }
  
  /* Release resources */
  status = pthread_condattr_destroy(&cattr);
  if(status != 0) {
    err_abort(status, "Condition attr destroy");
  }
  status = pthread_mutexattr_destroy(&mattr);
  if(status != 0) {
    err_abort(status, "Mutex attr destroy");
  }
#else
  for (i=0; i < sys->config.nstations; i++) {
    /* both lists in every station */
    pthread_mutex_init(&ps->list_in.mutex,  NULL);
    pthread_mutex_init(&ps->list_out.mutex, NULL);
    pthread_cond_init (&ps->list_in.cread,  NULL);
    pthread_cond_init (&ps->list_out.cread, NULL);
    et_init_station(ps);
    pthread_mutex_init(&ps->mutex, NULL);
    ps++;
  }
#endif
}

/******************************************************/
static void et_init_mem_event(et_id *id)
{
  int i;
  et_system *sys  = id->sys;
  et_event  *pe   = id->events;
  char      *pmem = id->data;
  
  memset((void *) pmem , 0,
	 (size_t) (sys->config.nevents * sys->config.event_size));

  /* init all event structures (headers) & set pointer to data */
  for (i=0; i < sys->config.nevents ; i++) {
    et_init_event(pe);
    sprintf(pe->filename, "%s%s%d", sys->config.filename, "_temp", i);
    pe->data = pmem;
    pmem += sys->config.event_size;
    pe++;
  }
}

/******************************************************/
static void et_sys_stopthread(pthread_t pt)
{
  int status;
  
  status = pthread_cancel(pt);
  if (status != 0) {
    err_abort(status, "Cancel thread");
  }
  return;
}

/******************************************************/
static void *et_sys_heartbeat(void *arg)
{
  et_id           *id = (et_id *) arg;
  et_system *sys = id->sys;
  struct timespec  timeout;
  const int        forever = 1;

  timeout.tv_sec  = ET_BEAT_SEC;
  timeout.tv_nsec = ET_BEAT_NSEC;

  /* send signal that thread started */
  id->race = -1;
  
  while (forever) {
    /* update my own heartbeat */
    sys->heartbeat = (sys->heartbeat + 1) % ET_HBMODULO;
    nanosleep(&timeout, NULL);
    pthread_testcancel();
  }
  return (NULL);
}

/******************************************************/
static void *et_sys_heartmonitor(void *arg)
{
  et_id *id = (et_id *) arg;
  et_system *sys = id->sys;
  et_stat_id stat;
  et_station *ps;
  int numprocs = sys->config.nprocesses;
  int i, j, status;
  unsigned int oldheartbt[ET_PROCESSES_MAX], newheartbt[ET_PROCESSES_MAX];
  struct timespec timeout;
  const int forever = 1;
 
  timeout.tv_sec  = ET_MON_SEC;
  timeout.tv_nsec = ET_MON_NSEC;
  
  /* send signal that thread started */
  id->race = -1;
  
  for (i=0; i < numprocs ; i++) {
    oldheartbt[i] = sys->proc[i].heartbeat;
  }
  
  while (forever) {
    /* read other process's heartbeats every ET_MON_SEC seconds */
    nanosleep(&timeout, NULL);
    pthread_testcancel();

    /* look at each process */
    for (i=0; i < numprocs ; i++) {
      if (sys->proc[i].status != ET_PROC_OPEN) {
        continue;
      }
      newheartbt[i] = sys->proc[i].heartbeat;
      if (oldheartbt[i] != newheartbt[i]) {
          oldheartbt[i]  = newheartbt[i];
      }
      /* else if no more heartbeat from process, eliminate it from ET system */
      else {
        /* tell all routines called that we're cleaning up */
        id->cleanup = 1;
	
        if (id->debug >= ET_DEBUG_INFO) {
          et_logmsg("INFO", "et_sys_heartmonitor, kill bad process (%d,%d)\n", i, sys->proc[i].pid);
        }
	/* kill process dead */
        kill(sys->proc[i].pid, SIGKILL);
	
        /* repair system, station, & transfer mutexes */
        et_fix_mutexes(id);
	
        /* grab station mutex for upcoming repairs */
        et_station_lock(sys);
	
	/* Grab transfer mutexes so no events get transfered and GC's input
	 * list is free from all activity.
	 */
        et_transfer_lock_all(id);
	
        /* repair linked list of active/idle stations */
        et_fix_linkedlist(id);
	
        /* Repair stations' inconsistancies from crashes in
	 * et_station_create and et_station_remove.
	 */
	et_fix_stations(id);
	
        /* Repair attachments' inconsistancies from crashes in
	 * et_station_attach (incomplete attachments)
	 */
        et_system_lock(sys);
	et_fix_attachments(id);
	
        /* If this process was doing an et_event(s)_dump, or possibly
	 * an et_event(s)_new, then it may have died with GC's input
	 * mutexes grabbed and its list messed up. In the case of the
	 * "dump", no attachment to GC is necessary so we need to look
	 * at this apart from any attachments this process had. However,
	 * we need to know if GC's input list was messed up by the "dump"
	 * or the "new" so it can be fixed accordingly.
	 */
	et_repair_gc(id);
	
	/* Deal with existing attachments of crashed process.
	 * This should take care of most crashes in the routine
	 * et_station_detach.
	 */
	for (j=0; j < sys->config.nattachments; j++) {
	  /* sys->proc[].att[] is the last value to be set (to -1)
	   * during et_station_detach, so it should be a good
	   * measure of the number of attachments a process still has.
	   * The routine et_fix_attachments has already eliminated
	   * any incomplete attachments from crashes in 
	   * et_station_attach.
	   */
	  if (sys->proc[i].att[j] == -1) {
	    continue;
	  }
	  stat = sys->attach[j].stat;
	  ps = id->grandcentral + stat;
          if (id->debug >= ET_DEBUG_INFO) {
            et_logmsg("INFO", "et_sys_heartmonitor, detach attachment %d from stat %d\n", j, stat);
          }
	  /* mutex problems are most likely in read/write lists */
	  status = et_repair_station(id, stat);
          if ((status != ET_OK) && (id->debug >= ET_DEBUG_SEVERE)) {
            et_logmsg("SEVERE", "et_sys_heartmonitor, CANNOT repair station\n");
          }
	  /* Get rid of process' attachment (this frees their events). This
	   * routine must be called with id->cleanup == 1 since we are cleaning
	   * up after a user process' crash. Note that et_station_detach calls
	   * et_restore_events which can lock GrandCentral's input list if that's
	   * where it supposed to send all the recovered events. It can also lock
	   * the input and output list mutexes of its own station during
	   * event recovery. In addition, et_flush_events is called which calls
	   * et_station_nread and et_station_nwrite thereby grabbing the input
	   * and output list mutexes of its own station.
	   */
          et_station_detach((et_sys_id) id, j);
	}
	
 	/* clean up process info */
        if (id->debug >= ET_DEBUG_INFO) {
          et_logmsg("INFO", "et_sys_heartmonitor, cleanup process %d\n", i);
        }
        et_init_process(sys, i);
        id->sys->nprocesses--;
	
	/* fix et process number */
        et_fix_nprocs(id);
	/* fix <x>.nattachments numbers */
        et_fix_natts(id);
	
        et_transfer_unlock_all(id);
        et_system_unlock(sys);
        et_station_unlock(sys);

        oldheartbt[i] = 0;
	
	/* we're done cleaning up */
        id->cleanup = 0;
      }
    }
  }
  return (NULL);
}

/************************************************
 * fix system, station and transfer mutexes 
 *
 * On Solaris & Linux, a thread which did not lock a pthread mutex
 * can still unlock it. This behavior is non-portable and is not guaranteed
 * to be so on a POSIX compliant operating system. In routines such as this
 * one, that free up mutexes locked by a crashed process, we may simply unlock
 * such mutexes, but it may be safer to re-initialize them. One of the difficulties
 * is that the programmer has no way of knowing whether the crashed process is
 * really the one which locked the mutex. So it's a bit of a guessing game. If
 * an active process has a mutex which another process or thread re-initializes,
 * what happens? I cannot find the answer to this in any book or man page.
 * In practice, Linux changes a locked mutex to an unlocked one when reinitialized.
 * In any case, we know that calls to pthread_mutex_unlock will never block and
 * so may be used whether or not they actually work. In order to be prudent,
 * program both methods of mutex recovery using a compile flag to chose between them.
 * Use the flag "-DMUTEX_INIT" in order to reinitialize mutexes locked by a dead thread.
 *
 ************************************************/
static void et_fix_mutexes(et_id *id)
{
  int i, status;
  et_station *ps;
#ifdef MUTEX_INIT
#ifndef MUTEX_NOSHARE
  pthread_mutexattr_t mattr;

  /* set attribute for mutex multiprocess sharing */
  status = pthread_mutexattr_init(&mattr);
  if(status != 0) {
    err_abort(status, "et_init_mem_sys");
  }
  
  status = pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
  if(status != 0) {
    err_abort(status, "et_init_mem_sys");
  }   
#endif
#endif
/*
   * looking at system & station mutexes is a bit of a gamble
   * as other folks might be using it for other things ... but
   * not likely for 2 seconds (see et_mutex_locked)
   */
  
  if (et_mutex_locked(&id->sys->mutex) == ET_MUTEX_LOCKED) {
    et_system_unlock(id->sys);
#ifdef MUTEX_INIT
#ifndef MUTEX_NOSHARE
    pthread_mutex_init(&id->sys->mutex, &mattr);
#else
    pthread_mutex_init(&id->sys->mutex, NULL);
#endif
#endif
  }
  
  if (et_mutex_locked(&id->sys->stat_mutex) == ET_MUTEX_LOCKED) {
    et_station_unlock(id->sys);
#ifdef MUTEX_INIT
#ifndef MUTEX_NOSHARE
    pthread_mutex_init(&id->sys->stat_mutex, &mattr);
#else
    pthread_mutex_init(&id->sys->stat_mutex, NULL);
#endif
#endif
  }
  
  if (et_mutex_locked(&id->sys->statadd_mutex) == ET_MUTEX_LOCKED) {
    status = pthread_mutex_unlock(&id->sys->statadd_mutex);
#ifdef MUTEX_INIT
#ifndef MUTEX_NOSHARE
    pthread_mutex_init(&id->sys->statadd_mutex, &mattr);
#else
    pthread_mutex_init(&id->sys->statadd_mutex, NULL);
#endif
#endif
    if (status != 0) {
      err_abort(status, "Failed add station mutex unlock");
    }
  }
 
  /*
   * Look at event transfer mutexes. The user locks all
   * such mutexes while creating, removing or detaching from
   * a station.
   */
  ps = id->grandcentral;
  for (i=0; i < id->sys->config.nstations; i++) {
    if (et_mutex_locked(&ps->mutex) == ET_MUTEX_LOCKED) {
      et_transfer_unlock(ps);
#ifdef MUTEX_INIT
#ifndef MUTEX_NOSHARE
      pthread_mutex_init(&ps->mutex, &mattr);
#else
      pthread_mutex_init(&ps->mutex, NULL);
#endif
#endif
    }
    ps++;
  }

#ifdef MUTEX_INIT
#ifndef MUTEX_NOSHARE
  /* Release resources */
  status = pthread_mutexattr_destroy(&mattr);
  if(status != 0) {
    err_abort(status, "Mutex attr destroy");
  }
#endif
#endif

  return;
}

/************************************************
 * If a user process is killed during the creation or removal
 * of a station, it's possible to mess up the linked list of
 * active or idle stations which define where events can go.
 *
 * When a station is added on the end of the main linked list,
 * the symptom of crashing during station creation's linked list
 * manipulation is that the tail of this list is not pointing to
 * nothing, but is pointing to the newly added station.
 *
 * When a station is inserted between 2 other stations in the middle
 * of the main linked list, the symptom of crashing is that the first
 * of the 2 existing stations points forward to the 2nd existing
 * station, but the 2nd station does NOT point back to the first. It
 * points to the station being inserted.
 *
 * When a station is being added to the parallel linked list of a 
 * station, there are similar considerations.
 *
 * The symptom of crashing during a station's removal from the main
 * linked list is that the tail isn't set and/or the next station doesn't
 * point back properly. For the parallel linked list, the next parallel
 * station doesn't point back properly. In addition, if it's the first
 * parallel station being removed, the main linked list is also involved.
 * 
 * Restore the lists as close to the original condition as possible by
 * starting with GC and gradually going down the lists making sure
 * all links are self-consistent.
 *
 */

static void et_fix_linkedlist(et_id *id)
{
  et_station *ps, *tail, *last, *pnext, *pstat, *nextparallel;
  et_system  *sys = id->sys;
  
  ps   = id->grandcentral + sys->stat_head;
  tail = id->grandcentral + sys->stat_tail;
  
  while (ps->next > -1) {
    /* Since "next" is always the last quantity to be
     * set when adding or removing a station, we can
     * always depend on its value.
     */
    pnext = id->grandcentral + ps->next;
    pnext->prev = ps->num;
    
    /* If it's a parallel station, check out those links too. */
    if (pnext->config.flow_mode == ET_STATION_PARALLEL) {
      pnext->prevparallel = -1;
      pstat = pnext;
      while (pstat->nextparallel > -1) {
        nextparallel = id->grandcentral + pstat->nextparallel;
	nextparallel->prevparallel = pstat->num;
	nextparallel->prev = -1;
	nextparallel->next = -1;
	pstat = nextparallel;
      }
    }
    else {
      pnext->prevparallel = -1;
      pnext->nextparallel = -1;
    }
    
    ps = pnext;
    
  }
  sys->stat_tail = ps->num;
    
  return;
}


/************************************************
 * This is called immediately after et_fix_linkedlist
 * is called so the linked list should be in a self-
 * consistent state. This routine "removes" stations
 * that are not in the linked lists.
 */
static void et_fix_stations(et_id *id)
{
  int         i, inlist, nstats=1;
  et_station *ps  = id->grandcentral + 1, *stat;
  et_station *pnext, *pstat, *pstat2, *nextparallel;
  et_system  *sys = id->sys;
  et_list    *pl;  

  /* station mutex is already locked */
  
  /* check first for stations of "creating" status */
  for (i=1; i < sys->config.nstations; i++) {
    if (ps->data.status == ET_STATION_CREATING) {
      /* Being here means conductor wasn't started.
       * The station can be in any state of partial initialization.
       * The count of stations may or may not include this one.
       * Since it isn't in llist, get rid of it.
       */
      if (id->debug >= ET_DEBUG_INFO) {
        et_logmsg("INFO", "et_fix_stations, remove station %s (half-created)\n", ps->name);
      }
      et_init_station(ps);
    }
    else if (ps->data.status != ET_STATION_UNUSED) {
      /* Check to see if this is in linked list like it should be.
       * If it isn't, this problem could have originated either during
       * creation or removal. In both cases, just remove it.
       */
      nstats++;
      inlist = 0;
      pstat  = id->grandcentral;
      
      while (pstat->next > -1) {
	pnext = id->grandcentral + pstat->next;
	if (pnext->num == ps->num) {
	  inlist = 1;
	  goto foundIt;
	}
	
	if (pnext->config.flow_mode == ET_STATION_PARALLEL) {
	  pstat2 = pnext;
	  while (pstat2->nextparallel > -1) {
            nextparallel = id->grandcentral + pstat2->nextparallel;
	    if (nextparallel->num == ps->num) {
	      inlist = 1;
	      goto foundIt;
	    }
	    pstat2 = nextparallel;
	  }
	}
	pstat = pnext;
      }
      
      foundIt:

      if (!inlist) {
        /* it's not in the list, let's dump it */
        if (id->debug >= ET_DEBUG_INFO) {
          et_logmsg("INFO", "et_fix_stations, remove station %s (not in llist)\n", ps->name);
        }
        ps->conductor = ET_THREAD_KILL;
        pl = &ps->list_out;
        pthread_cond_signal(&pl->cread);
        et_init_station(ps);
      }
    }
    ps++;
  }
     
  /* set count of stations right */
  if (nstats != sys->nstations) {
    if (id->debug >= ET_DEBUG_INFO) {
      et_logmsg("INFO", "et_fix_stations, reset nstations from %d to %d\n", sys->nstations, nstats);
    }
    sys->nstations = nstats;
  }

  return;
}

/************************************************/
/* This routine fixes any discrepancies in the
 * ET system's data about attachments. Such
 * discrepancies are caused by a user's process
 * crashing during et_station_attach (et_station_detach
 * is designed so that a crash in it will not cause this
 * routine to do anything). Any partially formed
 * attachment will be eradicated from existence.
 */
static void et_fix_attachments(et_id *id)
{
  int         i, j;
  et_system  *sys = id->sys;
  et_station *ps;
  et_stat_id  stat;

  /* station and system mutexes are already locked */
  
  /* go attachment by attachment */
  for (i=0; i < sys->config.nattachments; i++) {
    if (sys->attach[i].num != -1) {
      /* check to see if this attachment is part of a station */
      stat = -1;
      ps = id->grandcentral;
      for (j=0; j < sys->config.nstations; j++) {
        if ((ps->data.att[i] == i) && (ps->data.status == ET_STATION_ACTIVE)) {
	  /* got registered with station j */
	  stat = j;
	  break;
	}
	ps++;
      }
      /* if no station knows of attachment i, wipe it out */
      if (stat == -1) {
        /* wipe out all process' knowledge of it */
	for (j=0; j < sys->config.nprocesses; j++) {
	  sys->proc[j].att[i] = -1;
 	}
	/* wipe out system knowledge of it */
        et_init_attachment(sys, i);
	sys->nattachments--;
	
        if (id->debug >= ET_DEBUG_INFO) {
          et_logmsg("INFO", "set_fix_attachments, remove attachment %d\n", i);
        }
      }
    }
  }
  
  return;
}
  
/************************************************/
/* make sure all the <x>.nattachment values are correct */
static void et_fix_natts(et_id *id)
{
  int         i, j, natts=0;
  et_station *ps  = id->grandcentral;
  et_system  *sys = id->sys;

  /* system and station mutexes already locked */

  for (i=0; i < sys->config.nstations; i++) {
    if (ps->data.status != ET_STATION_ACTIVE) {
      ps++;
      continue;
    }
    ps->data.nattachments = 0;
    for (j=0; j < sys->config.nattachments; j++) {
      if (ps->data.att[j] == j) {
        ps->data.nattachments++;
      }
    }
    
    if (id->debug >= ET_DEBUG_INFO) {
      et_logmsg("INFO", "set_fix_natts, station %s has %d attachments\n", ps->name, ps->data.nattachments);
    }
    
    natts += ps->data.nattachments;
    ps++;
  }
  
  if (id->debug >= ET_DEBUG_INFO) {
    et_logmsg("INFO", "set_fix_natts, # total attachments %d -> %d\n", sys->nattachments, natts);
  }
  sys->nattachments = natts;
  
  for (i=0; i < sys->config.nprocesses; i++) {
    if (sys->proc[i].status != ET_PROC_OPEN) {
      continue;
    }
    sys->proc[i].nattachments = 0;
    for (j=0; j < sys->config.nattachments; j++) {
      if (sys->proc[i].att[j] == j) {
        sys->proc[i].nattachments++;
      }
    }
    if (id->debug >= ET_DEBUG_INFO) {
      et_logmsg("INFO", "set_fix_natts, proc %d has %d attachments\n", i, sys->proc[i].nattachments);
    }
  } 
  
  return;
}

/************************************************/
/*       make sure process count is correct     */
static void et_fix_nprocs(et_id *id)
{
  int         i, nprocs=0;
  et_system  *sys = id->sys;

  /* system mutex already locked */
    
  for (i=0; i < sys->config.nprocesses; i++) {
    if (sys->proc[i].status == ET_PROC_OPEN) {
      nprocs++;
    }
  } 
  
  if (id->debug >= ET_DEBUG_INFO) {
    et_logmsg("INFO", "set_fix_nprocs, change # of ET processes from %d to %d\n", sys->nprocesses, nprocs);
  }
  
  sys->nprocesses = nprocs;
  return;
}

/************************************************/
/* used when restarting an ET system to put
 * things into a consistent state.
 */
static void et_fix_system(et_id *id)
{
  et_system  *sys = id->sys;
  et_stat_id stat;
  et_station *ps, *pstat, *pstat2, *pnext, *nextparallel;
  et_list    *pl;
  int numprocs = sys->config.nprocesses;
  int numstats = sys->config.nstations;
  int             i, j, inlist;
  unsigned int    oldheartbt[ET_PROCESSES_MAX];
  struct timespec timeout;
 
  timeout.tv_sec  = ET_BEAT_SEC + 1;
  timeout.tv_nsec = ET_BEAT_NSEC;
  
  /* record heartbeat values */ 
  for (i=0; i < numprocs ; i++) {
    oldheartbt[i] = sys->proc[i].heartbeat;
  }
  
  /* wait to make sure we give all hearts a chance to beat once */
  nanosleep(&timeout, NULL);
  
  /* unlock system, station, & transfer mutexes */
  et_fix_mutexes(id);
  et_transfer_unlock_all(id);
  /* unlock station lists' mutexes */
  ps = id->grandcentral;
  for (i=0; i < numstats; i++) {
    pl = &ps->list_in;
    et_llist_unlock(pl);
    pl = &ps->list_out;
    et_llist_unlock(pl);
    ps++;
  }
  
  /* reset some elements in sys structure */
  sys->asthread     = ET_THREAD_KEEP;
  sys->heartbeat    = 0;
  sys->mainpid      = getpid();
  sys->ntemps       = 0;
  
  /* initialize event structures and data */
  et_init_mem_event(id);
  
  /* fix linked list of active/idle stations */  
  et_fix_linkedlist(id);
  
  /* check out the stations */
  ps = id->grandcentral;
  for (i=0; i < numstats; i++) {
    /* initialize if station not in use */
    if ((ps->data.status != ET_STATION_ACTIVE) &&
        (ps->data.status != ET_STATION_IDLE))    {
      et_init_station(ps);
      continue;
    }

    /* if a "used" station is not in active/idle linked list, remove it */
    inlist = 0;
    pstat  = id->grandcentral + sys->stat_head;

    while (pstat->next > -1) {
      pnext = id->grandcentral + pstat->next;
      if (pnext->num == ps->num) {
	inlist = 1;
	goto foundIt;
      }

      if (pnext->config.flow_mode == ET_STATION_PARALLEL) {
	pstat2 = pnext;
	while (pstat2->nextparallel > -1) {
          nextparallel = id->grandcentral + pstat2->nextparallel;
	  if (nextparallel->num == ps->num) {
	    inlist = 1;
	    goto foundIt;
	  }
	  pstat2 = nextparallel;
	}
      }
      pstat = pnext;
    }

    foundIt:

    if (!inlist) {
      et_init_station(ps);
      continue;
    }
    
    /* idle stations have no attachments by definition */
    if (ps->data.status == ET_STATION_IDLE) {
      ps->data.nattachments = 0;
      for (j=0 ; j < ET_ATTACHMENTS_MAX; j++) {
        ps->data.att[j] = -1;
      }
    }
    
    /* reinitialize various parts of a station */
    ps->conductor = ET_THREAD_KEEP;
    /* fix struct */
    ps->fix.in.first  = NULL;
    ps->fix.in.start  = 0;
    ps->fix.in.cnt    = 0;
    ps->fix.in.num    = 0;
    ps->fix.in.call   = ET_FIX_READ;
    et_bigint_init(&ps->fix.in.eventsin);
    ps->fix.out.start = 0;
    ps->fix.out.cnt   = 0;
    ps->fix.out.num   = 0;
    /* input list */
    pl = &ps->list_in;
    pl->cnt        = 0;
    pl->lasthigh   = 0;
    pl->firstevent = NULL;
    pl->lastevent  = NULL;
    /* output list */
    pl = &ps->list_out;
    pl->cnt        = 0;
    pl->lasthigh   = 0;
    pl->firstevent = NULL;
    pl->lastevent  = NULL;

    ps++;
  }
  
  /* reinit dead or unused processes */
  for (i=0; i < numprocs ; i++) {
    if (sys->proc[i].status != ET_PROC_OPEN) {
      et_init_process(sys, i);
      continue;
    }
    /* if no more heartbeat, it's dead */
    if (sys->proc[i].heartbeat == oldheartbt[i]) {
      if (id->debug >= ET_DEBUG_INFO) {
        et_logmsg("INFO", "set_fix_system, removed process %d\n", i);
      }
      /* If we remove a process, make sure we first remove any of its
       * attachments, then check to see if removing those attachments
       * made any stations idle (we removed the last attachment to a
       * station).
       */
       
      for (j=0; j < sys->config.nattachments; j++) {
	if (sys->proc[i].att[j] == -1) {
	  continue;
	}
	stat = sys->attach[j].stat;
	ps = id->grandcentral + stat;
        if ((ps->data.nattachments == 1) && (stat != ET_GRANDCENTRAL)) {
          ps->data.status = ET_STATION_IDLE;
	  ps->data.nattachments--;
          ps->data.att[j] = -1;
          sys->attach[j].num = -1;
          sys->attach[j].status = ET_ATT_UNUSED;
          sys->nattachments--;
	}
      }
	
      et_init_process(sys, i);
      continue;
    }
  }
  
  /* fix attachments - this will remove attachments destroyed 
   * by any removal of stations that we did above.
   */
  et_fix_attachments(id);
  /* fix et process number */
  et_fix_nprocs(id);
  /* fix <x>.nattachments numbers */
  et_fix_natts(id);
  
 return;
}

/************************************************/
/* thread to watch for addition of new stations */

static void *et_add_stations(void *arg)
{
  pthread_t      thread_id;
  pthread_attr_t attr;
  int            status;
  et_id         *id = (et_id *) arg;
  et_system     *sys = id->sys;
  const int      forever = 1;
  
  /* make conductor thread detached */
  status = pthread_attr_init(&attr);
  if(status != 0) {
    err_abort(status, "Init thd attr");
  }
  status = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if(status != 0) {
    err_abort(status, "Set thd detach");
  }
  
  /* send signal that thread started */
  id->race = -1;
  
  /* grab ET add station mutex */
  status = pthread_mutex_lock(&sys->statadd_mutex);
  if (status != 0) {
    err_abort(status, "Failed add station lock");
  }

  while (forever) {
    do {
      /* wait on condition var for adding stations */
      status = pthread_cond_wait(&sys->statadd, &sys->statadd_mutex);
      if (status != 0) {
        err_abort(status, "Wait et_add_stations thread");
      }
      if (sys->asthread == ET_THREAD_KILL) {
        pthread_exit(NULL);
      }
    } while (sys->nstations > sys->config.nstations);
    
    if (id->debug >= ET_DEBUG_INFO) {
      et_logmsg("INFO", "et_add_stations, nstations = %d, stats_max = %d\n",
		sys->nstations,sys->config.nstations);
    }
    
    /* Now that another station is added, add a "conductor"
     * thread to move events from that station to the next.
     */
    status = pthread_create(&thread_id, &attr, et_conductor, arg);
    if(status != 0) {
      err_abort(status, "Create et_conductor thd");
    }
  }
  return (NULL);
}

/************************************************
 * Thread to conduct events between stations.
 * This conductor is identical to et_conductorSerial
 * with the exception that it handles the new
 * parallel stations.
 ***********************************************/
static void *et_conductor(void *arg)
{
  et_id      *id  = (et_id *) arg;
  et_sys_id  etid = (et_sys_id) arg;
  et_system  *sys = id->sys;
  int        event_depth = sys->config.nevents;
  int        i, j, status, me=0, num_temp=0, numwritten, none=1;
  int	     firstimethruloop, writeall;
  int        events_total, events_left, events_toput;
  int        *numEvents, *inListCount, *place;
  short      *event_put;
  et_event   **allevents, **putevents, **temps;
  et_station *ps, *pmystation, *pstat=NULL;
  et_station *startStation, *firstActive, *firstParallel;
  et_list    *pl, *pmylist;  
  void       *sym;
  const int  forever = 1;
  int        parallelIsActive, rrobin_equalcue, numActiveStations;
  int        num, extra, startLooking, lastEventIndex=0, eventsAlreadyPut;
  int        index, count, skip, numOfEvents, min;
  int        eventsPerStation, nextHigherCue, eventsDoledOut, stationsWithSameCue;
	   
  /* station mutex has already been locked by et_station_create */
  
  /* find the station in act of being created */   
  ps = id->grandcentral;
  for (i=0 ; i < sys->config.nstations ; i++){
    if (ps->data.status == ET_STATION_CREATING) {
        none = 0;
        break;
    }
    ps++;
    me++;
  }
  
  if (none) {
    if (id->debug >= ET_DEBUG_WARN) {
      et_logmsg("WARN", "et_conductor, all stations have their conductors already\n");
    }
    pthread_exit(NULL);
  }
  
  /* found station, label as conductor (this thread) attached */
  pmystation = ps;
  if (id->debug >= ET_DEBUG_INFO) {
    et_logmsg("INFO", "et_conductor %d, found station %p\n", me, ps);
  }
  /* my station's event list_out */
  pmylist = &pmystation->list_out;
  
  /* if using default event selection routine */
  if (ps->config.select_mode == ET_STATION_SELECT_MATCH) {
    ps->data.func = et_condition;
  }
  /* if using user-defined event selection routine, load it */
  else if (ps->config.select_mode == ET_STATION_SELECT_USER) {
    ps->data.lib_handle = dlopen(ps->config.lib, RTLD_NOW);
    if (ps->data.lib_handle == NULL) {
      /* tell et_station_create that we are done */
      pthread_cond_signal(&sys->statdone);
      if (id->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "et_conductor %d, %s\n", me, dlerror());
      }
      pthread_exit(NULL);
    }
    if (id->debug >= ET_DEBUG_INFO) {
      et_logmsg("INFO", "et_conductor %d, loaded shared lib %s\n", me, ps->config.lib);
    }
    sym = dlsym(ps->data.lib_handle, ps->config.fname);
    if (sym == NULL) {
      dlclose(ps->data.lib_handle);
      /* tell et_station_create that we are done */
      pthread_cond_signal(&sys->statdone);
      if (id->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "et_conductor %d, %s\n", me, dlerror()) ;
      }
      pthread_exit(NULL);
    }
    if (id->debug >= ET_DEBUG_INFO) {
      et_logmsg("INFO", "et_conductor %d, loaded function %s\n", me, ps->config.fname);
    }
    ps->data.func = (ET_SELECT_FUNCPTR) sym;
  }
  
  /* calloc arrays since we don't know how big they are at compile time */
  
  /* all events initially read in from the station's output list */
  if ( (allevents = (et_event **) calloc(event_depth, sizeof(et_event *))) == NULL) {
   /* tell et_station_create that we are done */
    pthread_cond_signal(&sys->statdone);
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_conductor %d, no mem left\n", me) ;
    }
    pthread_exit(NULL);
  }
  
  /* events to be "put" into the next station's input list */
  if ( (putevents = (et_event **) calloc(event_depth, sizeof(et_event *))) == NULL) {
    pthread_cond_signal(&sys->statdone);
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_conductor %d, no mem left\n", me) ;
    }
    pthread_exit(NULL);
  }
  
  /* temporary (large, specially allocated) events */
  if ( (temps = (et_event **) calloc(event_depth, sizeof(et_event *))) == NULL) {
    pthread_cond_signal(&sys->statdone);
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_conductor %d, no mem left\n", me) ;
    }
    pthread_exit(NULL);
  }
  
  /* events which have already been put into a previous station */
  if ( (event_put = (short *) calloc(event_depth, sizeof(short))) == NULL) {
    pthread_cond_signal(&sys->statdone);
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_conductor %d, no mem left\n", me) ;
    }
    pthread_exit(NULL);
  }

  /* For parallel stations, an array containing the numbers of events to be
   * placed in each of these stations.
   */
  if ( (numEvents = (int *) calloc(sys->config.nstations, sizeof(int))) == NULL) {
    pthread_cond_signal(&sys->statdone);
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_conductor %d, no mem left\n", me) ;
    }
    pthread_exit(NULL);
  }

  /* For parallel stations, an array containing the numbers of events currently
   * in each of these stations' input lists.
   */
  if ( (inListCount = (int *) calloc(sys->config.nstations, sizeof(int))) == NULL) {
    pthread_cond_signal(&sys->statdone);
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_conductor %d, no mem left\n", me) ;
    }
    pthread_exit(NULL);
  }

  /* For parallel stations, an array containing the original place of an active
   * station in the parallel linked list before they were ordered by the amount
   * of events in their input lists.
   */
  if ( (place = (int *) calloc(sys->config.nstations, sizeof(int))) == NULL) {
    pthread_cond_signal(&sys->statdone);
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_conductor %d, no mem left\n", me) ;
    }
    pthread_exit(NULL);
  }

  /* tell et_station_create that we have successfully started */
  if (ps == id->grandcentral) {
    ps->data.status = ET_STATION_ACTIVE;
  } else {
    ps->data.status = ET_STATION_IDLE;
  }
  pthread_cond_signal(&sys->statdone);
  
  
  while (forever) {

    /* wait on condition var for these events */
    et_llist_lock(pmylist);
    while (pmylist->cnt < 1) {
      status = pthread_cond_wait(&pmylist->cread, &pmylist->mutex);
      if (status != 0) {
        err_abort(status, "Wait list_out cond var");
      }
      /* kill self if station removed */  
      if (pmystation->conductor == ET_THREAD_KILL) {
	et_llist_unlock(pmylist);
	if (pmystation->config.select_mode == ET_STATION_SELECT_USER) {
          dlclose(pmystation->data.lib_handle);
        }
	free(allevents); free(putevents); free(temps); free(event_put);
	free(numEvents); free(inListCount); free(place);
	pthread_exit(NULL);
      }
    }
    
    /* grab all events in station's list_out */
    if ((events_total = et_llist_read(pmylist, allevents)) < 0) {
      et_llist_unlock(pmylist);
      goto error;
    }
    et_llist_unlock(pmylist);
    events_left = events_total;

    /* reinit items to track events being put */
    for (i=0; i < events_total ; i++) {
      event_put[i] = 0;
    }
    firstimethruloop = 1;
    writeall = 0;
    
    /* grabbing mutex allows no change to linked list of created stations */
    et_transfer_lock(pmystation);
    
    /* Send events downstream by going to the next active station
     * in the main linked list & putting events its list. */
    
    /* If this is a parallel station, find the first parallel station
     * in this group. The next station in the main linked list after that
     * will our "next" station.
     */
    if (pmystation->config.flow_mode == ET_STATION_PARALLEL) {
      firstParallel = pmystation;
      while (firstParallel->prevparallel > -1) {
        firstParallel = id->grandcentral + firstParallel->prevparallel;
      }
      /* Now we're back to the first parallel station. The one we want is
       * the next one in the main list from the first parallel.
       */
      if (firstParallel->next < 0) {
	ps = id->grandcentral;
      }
      else {
	ps = id->grandcentral + firstParallel->next;
      }
    }
    else {
      if (pmystation->next < 0) {
	ps = id->grandcentral;
      }
      else {
	ps = id->grandcentral + pmystation->next;
      }
    }
    /* get its input list */
    pl = &ps->list_in;    
    
    /* while there are events still left to distribute ... */
    while (events_left > 0) {
      /* Is there at least one active parallel station? */
      parallelIsActive = 0;
      /* Total number of active parallel stations. */
      numActiveStations = 0;
      /* Direct to different event dispersing algorithms. */
      rrobin_equalcue = 0;
      /* Station to receive first round-robin event. */
      startStation = NULL;
      /* First active station in parallel linked list. */
      firstActive  = NULL;
      
      /* If this is a parallel station ... */
      if (ps->config.flow_mode == ET_STATION_PARALLEL) {
	/* Are any of the parallel stations active or can we skip the bunch? */
	pstat = ps;
	while (1) {
	  if (pstat->data.status == ET_STATION_ACTIVE) {
	    parallelIsActive = 1;
	    firstActive = pstat;
	    break;
	  }
	  /* Find next station in the parallel linked list. */
	  if (pstat->nextparallel < 0) {
	    break;
	  }
	  pstat = id->grandcentral + pstat->nextparallel;
	}
	
	/* Which algorithm are we using? */
	if (parallelIsActive &&
	    ((ps->config.select_mode == ET_STATION_SELECT_RROBIN) ||
	     (ps->config.select_mode == ET_STATION_SELECT_EQUALCUE))) {
	  rrobin_equalcue = 1;
	}
      }
      
      if (!rrobin_equalcue &&
          (parallelIsActive || (ps->data.status == ET_STATION_ACTIVE))) {
	  
	  if (ps->config.flow_mode == ET_STATION_PARALLEL) {
            /* Save pointer to first parallel station (even if inactive). */
	    pstat = ps;
	    /* Skip to first active parallel station. */
	    ps = firstActive;
            pl = &ps->list_in;
	  }
	  
	  /* Loop through all the active parallel stations if necessary. */
	  do {
            /* init here */
            events_toput = 0;
	    
            /* OPTIMIZATION:
             * If this is the first time thru the while loop, no events have yet
             * been divided up between the stations downstream. If the next station
             * is GrandCentral or a blocking station with prescale=1, then save
             * time by dumping everything directly into it.
             */

            if (ps == id->grandcentral) {	  
              num_temp = 0;
              /* if first time thru while loop, dump everything into GC */
	      if (firstimethruloop) {
		events_toput = events_total;
        	for (i=0; i < events_total ; i++) {
        	  /* find temp events for elimination */
        	  if (allevents[i]->temp == ET_EVENT_TEMP) {
	            temps[num_temp++] = allevents[i];
		  }
        	}
	      }
	      else {
        	for (i=0; i < events_total ; i++) {
        	  /* do next event if already put in another station */
        	  if (event_put[i]) {
                    continue;
		  }
        	  /* find temp events for elimination */
        	  if (allevents[i]->temp == ET_EVENT_TEMP) {
	            temps[num_temp++] = allevents[i];
		  }
		  putevents[events_toput++] = allevents[i];
        	}
	      }
            }

            /* all events, blocking */
            else if ((ps->config.select_mode == ET_STATION_SELECT_ALL) &&
	             (ps->config.block_mode  == ET_STATION_BLOCKING  ))  {

              /* if first time thru while loop, dump everything into station */
              if ((firstimethruloop) && (ps->config.prescale == 1))  {
		writeall = 1;
		events_toput = events_total;
        	et_llist_lock(pl);
		pl->events_try = et_bigint_add(pl->events_try, events_total);
		/* pl->events_try += events_total; */
	      }
	      else {
        	et_llist_lock(pl);
        	for (i=0; i < events_total ; i++) {
        	  if (event_put[i]) {
        	    continue;
		  }
        	  /* apply prescale */ 
		  /* if (((pl->events_try++)%(ps->config.prescale)) == 0) { */
		  if (et_bigint_mod(pl->events_try, ps->config.prescale) == 0) {
		    putevents[events_toput++] = allevents[i];
		    event_put[i] = 1;
		  }
		  pl->events_try = et_bigint_add(pl->events_try, 1);
        	}
		if (events_toput == 0) {
		  et_llist_unlock(pl);
		}
	      }
            }

            /* all events, nonblocking */
            /* note: pl->cnt IS mutex protected here !! */
            else if ((ps->config.select_mode == ET_STATION_SELECT_ALL)  &&
	             (ps->config.block_mode  == ET_STATION_NONBLOCKING))  {
              et_llist_lock(pl);
              if (pl->cnt < ps->config.cue) {
        	j = ps->config.cue - pl->cnt;
        	for (i=0; i < events_total ; i++) {
        	  if (event_put[i]) {
                    continue;
		  }
		  putevents[events_toput++] = allevents[i];
		  event_put[i] = 1;
        	  if (--j == 0) {
		    break;
		  }
        	}
              }
	      if (events_toput == 0) {
		et_llist_unlock(pl);
	      }
            }

            /*  condition (user or match), blocking */
            else if (ps->config.block_mode == ET_STATION_BLOCKING) {
              et_llist_lock(pl);
              for (i=0; i < events_total ; i++) {
        	if (event_put[i]) {
        	  continue;
		}
		if ((*ps->data.func)(etid, ps->num, allevents[i])) {
		  /* if (((pl->events_try++)%(ps->config.prescale)) == 0) { */
		  if (et_bigint_mod(pl->events_try, ps->config.prescale) == 0) {
	            putevents[events_toput++] = allevents[i];
	            event_put[i] = 1;
		  }
		  pl->events_try = et_bigint_add(pl->events_try, 1);
		}
	      }
	      if (events_toput == 0) {
		et_llist_unlock(pl);
	      }
            }

	    /* condition (user or match) + nonblocking */
            else if (ps->config.block_mode == ET_STATION_NONBLOCKING) {
              et_llist_lock(pl);
              if (pl->cnt < ps->config.cue) {
        	j = ps->config.cue - pl->cnt;
        	for(i=0; i < events_total ; i++) {
        	  if (event_put[i]) {
                    continue;
		  }
		  if ((*ps->data.func)(etid, ps->num, allevents[i])) {
	            putevents[events_toput++] = allevents[i];
	            event_put[i] = 1;
		    if (--j == 0) {
		      break;
		    }
		  }
        	}
              }
	      if (events_toput == 0) {
		et_llist_unlock(pl);
	      }
	    }

 	    /* If items go in this station ... This condition is necessary
	     * as "et_llist_write" does not take 0 as a good argument. It's
	     * also a waste of time to do the mutex manipulation if no
	     * events are changing hands.
	     */
            if (events_toput > 0) {

	      if (ps == id->grandcentral) {
        	/* if tmp events exist, remove */
        	if (temps[0] != NULL) {
        	  et_system_lock(sys);
        	  for (i=0; i < num_temp ; i++) {
                    /*
		     * temp events are not mapped in ET system space
		     * but their files need to be removed
		     */
		    unlink(temps[i]->filename);
                    sys->ntemps--;
    /*printf("conductor %d: unlink tmp event\n", me);*/
        	  }
		  et_system_unlock(sys);
		}
        	/* lock mutex of gc station's list_in */
        	et_llist_lock(pl);
		if (firstimethruloop) {
        	  status = et_llist_write_gc(id, allevents, events_total);
		}
		else { 
        	  status = et_llist_write_gc(id, putevents, events_toput);
        	}
		et_llist_unlock(pl);
		if (status == ET_ERROR) {
		  goto error;
		}
        	events_left = 0;	
	      }

	      else {
		/* pl is already locked */
 		if (writeall) {
        	  numwritten = et_llist_write(id, pl, allevents, events_total);
		  et_llist_unlock(pl);
        	  events_left = 0;
		  writeall = 0;
        	}
		else {
		  numwritten = et_llist_write(id, pl, putevents, events_toput);
 		  et_llist_unlock(pl);
        	  events_left -= events_toput;
        	}
        	if (numwritten == ET_ERROR) {
        	  goto error;
        	}
		else if (numwritten < events_toput) {
		  /* this one should never happen */
        	  if (id->debug >= ET_DEBUG_SEVERE) {
                    et_logmsg("SEVERE", "et_conductor %d, cannot write all events to %s input\n", me, ps->name);
        	  }
        	  goto error;
		}
              }

              /* signal reader that new events are here */
              status = pthread_cond_broadcast(&pl->cread);
              if(status != 0) {
        	err_abort(status, "events here");
	      }

            } /* if items go in this station */
	  
            /* Find the next active parallel station. */
	    if (parallelIsActive) {
	      do {	
		if (ps->nextparallel > -1) {
		  ps = id->grandcentral + ps->nextparallel;
        	  pl = &ps->list_in;
		  firstimethruloop=0;
		}
		else {
		  goto outOfLoop;
		}
	      } while (ps->data.status != ET_STATION_ACTIVE);
	    }
	    /* loop over all parallel stations if necessary. */
	  } while (parallelIsActive && (events_left > 0));
	  
	  outOfLoop:
	  
	  /* Restore ps back to head of parallel linked list (which is also
	   * part of the main linked list.
	   */
	  if (ps->config.flow_mode == ET_STATION_PARALLEL) {
	    ps = pstat;
	  }
      } /* if (!rrobin_equalcue && station(s) active) */
      
      /* Implement the round-robin & equal-cue algorithms for dispensing
       * events to a single group of parallel stations.
       */
      else if (rrobin_equalcue && parallelIsActive) {
	
        if (ps->config.select_mode == ET_STATION_SELECT_RROBIN) {
	  
	  /* Flag to start looking for station that receives first round-robin event. */
	  startLooking = 0;
	  pstat = ps;
	  
	  while (forever) {
	    /* for each active station ... */
	    if (pstat->data.status == ET_STATION_ACTIVE) {
	      if (startLooking) {
		/* This is the first active station after
		 * the last station to receive an event.
		 */
		startStation = pstat;
		startLooking = 0;
	      }
	      numActiveStations++;
	    }

	    /* Find last station to receive a round-robin event and start looking
	     * for the next active station to receive the first one.
	     */
	    if (pstat->waslast == 1) {
		pstat->waslast = 0;
		startLooking = 1;
	    }

	    /* find next station in the parallel linked list */
	    if (pstat->nextparallel > -1) {
	      pstat = id->grandcentral + pstat->nextparallel;
	    }
	    /* else if we're at the end of the list ... */
	    else {
	      /* If we still haven't found a place to start the round-robin
	       * event dealing, make it the first active station.
	       */
	      if (startStation == NULL) {
		startStation = firstActive;
	      }
	      break;
	    }
	  }
	  
	  /* Find the number of events going into each station. */
	  num = events_left / numActiveStations;
	  /* Find the number of events left over (not enough for another round). */
	  extra = events_left % numActiveStations;
	  eventsAlreadyPut = count = 0;

	  /* Rearrange "allevents" so all those destined for a particular
	   * station are grouped together in the new array.
	   */
	  for (i=0; i < numActiveStations; i++) {
	    if (i < extra) {
	      numEvents[i] = num + 1;
	      if (i == (extra - 1)) {
	        lastEventIndex = i;
	      }
	    }
	    else {
	      numEvents[i] = num;
	    }
	    
	    if (extra == 0) {
	      lastEventIndex = numActiveStations - 1;
	    }
	    
	    numOfEvents = numEvents[i];
	    
	    /* If all events go into these parallel stations ... */
	    if (firstimethruloop) {
	      index = i;
	      for (j=0; j < numOfEvents; j++) {
		putevents[count++] = allevents[index];
		index += numActiveStations;
              }
	    }
	    else {
	      skip = 0;
              for (j=i; j < events_total; j++) {
        	/* Skip over events already put into other stations. */
		if (event_put[j]) {
		  continue;
		}
		/* Skip over events in round-robin algorithm. */
		if (skip-- > 0) {
		  continue;
		}

		putevents[count++] = allevents[j];
		skip = numActiveStations - 1;
		numOfEvents--;
		if (numOfEvents == 0) {
	          break;
		}
              }
	    }
	  }

	  /* Place the first event with the station after the one which
	   * received the last event in the previous round.
	   */
	  pstat = startStation;
	  count = 0;

	  while (forever) {
	    /* For each active parallel station ... */
	    if (pstat->data.status == ET_STATION_ACTIVE) {
              /* Mark station that got the last event. */
	      if (count == lastEventIndex) {
		pstat->waslast = 1;
	      }
	      
	      /* Put "events_toput" number of events in the next active station. */
	      events_toput = numEvents[count++];
	      
	      if (events_toput > 0) {
		pl = &pstat->list_in;    
        	et_llist_lock(pl);
        	pl->events_try = et_bigint_add(pl->events_try, events_toput);
	        numwritten = et_llist_write(id, pl, &putevents[eventsAlreadyPut], events_toput);
 		et_llist_unlock(pl);
        	events_left -= events_toput;

        	if (numwritten == ET_ERROR) {
        	  goto error;
        	}
		else if (numwritten < events_toput) {
		  /* This should never happen. */
        	  if (id->debug >= ET_DEBUG_SEVERE) {
                    et_logmsg("SEVERE", "et_conductor %d, cannot write all events to %s input\n", me, pstat->name);
        	  }
        	  goto error;
		}

        	/* Signal reader that new events are here. */
        	status = pthread_cond_broadcast(&pl->cread);
        	if(status != 0) {
        	  err_abort(status, "events here");
		}

		eventsAlreadyPut += events_toput;
	      } /* if (events_toput > 0) */
	    } /* if station is active */
	    
	    /* Find next active station. */
	    if (count >= numActiveStations) {
	      break;
	    }
	    else if (pstat->nextparallel > -1) {
	      pstat = id->grandcentral + pstat->nextparallel;
	    }
	    else {
	      /* Go back to the first active parallel station. */
	      pstat = firstActive;
	    }
	  } /* while (forever) */
	} /* if round-robin */
	
	/* else if equal-cue algorithm. */
        else {
	  eventsDoledOut = 0;
	  eventsAlreadyPut = 0;
	  stationsWithSameCue = 0;
	  
	  /* Array that keeps track of original station order. */
          for (i=0; i < sys->config.nstations; i++){
	    place[i] = i+1;
	  }
	 
	  pstat = firstActive;
	  while (forever) {
	    /* For each active station ... */
	    if (pstat->data.status == ET_STATION_ACTIVE) {
	      /* Find total # of events in stations' input lists (cues).
	       * Store this information as it will change and we don't
	       * really want to grab all the input mutexes to make
	       * sure these values don't change.
	       */
	      inListCount[numActiveStations++] = pstat->list_in.cnt;
	    }

	    /* find next station in the parallel linked list */
	    if (pstat->nextparallel > -1) {
	      pstat = id->grandcentral + pstat->nextparallel;
	    }
	    else {
	      break;
	    }
	  }
	  
	  /* Sort the input lists (cues) according to number of events. The "place" 
	   * array remembers the place in the presorted array of input lists.
	   * Since arrays to be sorted are assumed to have indexes from 1 to n,
	   * the -1 trick allows us to use arrays indexed from 0 to n-1.
	   */
	  shellSort(numActiveStations, inListCount-1, place-1);

	  /* To determine which stations get how many events:
	   * Take the lowest cues, add enough to make them equal
	   * to the next higher cue. Continue doing this until all
	   * are equal. Evenly divide any remaining events.
	   */
	  nextHigherCue = 0;
	  min = inListCount[0];
	  for (i=0; i < numActiveStations; i++) {
	    numEvents[i] = 0;
	  }
	  
	  while(eventsDoledOut < events_left) {
	    /* Find how many cues have the lowest # of events in them. */
	    stationsWithSameCue = 0;
	    for (i=0; i < numActiveStations; i++) {
	      /* Does events in cue + events we've just given it = min? */
	      if (min == inListCount[i] + numEvents[place[i]-1]) {
	        stationsWithSameCue++;
	      }
	      else {
	        nextHigherCue = inListCount[i];
		break;
	      }
	    }
	     
	    /* If all stations have same # of events, or if there are not enough
	     * events to fill each lowest cue to level of the next higher cue,
	     * we spread available events between them all ... 
	     */
	    if ((stationsWithSameCue == numActiveStations) ||
	        ((events_left - eventsDoledOut) < ((nextHigherCue - min)*stationsWithSameCue))) {
	      events_toput = events_left - eventsDoledOut;
	      eventsPerStation = events_toput / stationsWithSameCue;
	      extra = events_toput % stationsWithSameCue;
	      count = 0;
	      for (i=0; i < stationsWithSameCue; i++) {
		if (count++ < extra) {
		  numEvents[place[i]-1] += eventsPerStation + 1;
		}
		else {
		  numEvents[place[i]-1] += eventsPerStation;
		}
	      }
	      break;
	    }
	    /* Else, fill the lowest cues to the level of the next higher cue
	     * and repeat the cycle.
	     */
	    else {
	      eventsPerStation = nextHigherCue - min;
	      for (i=0; i < stationsWithSameCue; i++) {
		numEvents[place[i]-1] += eventsPerStation;
	      }
	      min = nextHigherCue;
	    }
	    eventsDoledOut += eventsPerStation*stationsWithSameCue;
	  }
	  	  
	  pstat = firstActive;
	  count = 0;
	  	  
	  while (forever) {
	    /* For each active parallel station ... */
	    if (pstat->data.status == ET_STATION_ACTIVE) {

	      if ((events_toput = numEvents[count++]) < 1) {
		/* Find next station in the parallel linked list. */
		if (pstat->nextparallel > -1) {
		  pstat = id->grandcentral + pstat->nextparallel;
		  continue;
		}
		else {
		  break;
		}
	      }
	      
              /* Put "events_toput" number of events in the next active station. */
	      if (firstimethruloop) {
		pl = &pstat->list_in;    
        	et_llist_lock(pl);
		pl->events_try = et_bigint_add(pl->events_try, events_toput);
		numwritten = et_llist_write(id, pl, &allevents[eventsAlreadyPut], events_toput);
 		et_llist_unlock(pl);
              }
	      else {
        	index = 0;
		for (i=0; i < events_total; i++) {
        	  if (event_put[i]) {
        	    continue;
		  }
		  putevents[index++] = allevents[i];
		  event_put[i] = 1;
		  if (index >= events_toput) {
		    break;
		  }
        	}
		pl = &pstat->list_in;    
        	et_llist_lock(pl);
		pl->events_try = et_bigint_add(pl->events_try, events_toput);
		numwritten = et_llist_write(id, pl, putevents, events_toput);
 		et_llist_unlock(pl);
	      }
	      	      
              if (numwritten == ET_ERROR) {
        	goto error;
              }
	      else if (numwritten < events_toput) {
		/* This should never happen. */
        	if (id->debug >= ET_DEBUG_SEVERE) {
                  et_logmsg("SEVERE", "et_conductor %d, cannot write all events to %s input\n", me, pstat->name);
        	}
        	goto error;
	      }

              /* Signal reader that new events are here. */
              status = pthread_cond_broadcast(&pl->cread);
              if(status != 0) {
        	err_abort(status, "events here");
	      }
	      
              events_left -= events_toput;
	      eventsAlreadyPut += events_toput;
	    }
	    
	    /* Find next station in the parallel linked list. */
	    if (pstat->nextparallel > -1) {
	      pstat = id->grandcentral + pstat->nextparallel;
	    }
	    else {
	      break;
	    }
	  } /* while(forever) */
	  
	  
	} /* else if equal-cue algorithm */
      } /* if (rrobin_equalcue && parallelIsActive) */


      /* Find the next serial station in the linked list. */
      if (ps == id->grandcentral) {
        break;
      }
      if (ps->next < 0) {
	ps = id->grandcentral;
      }
      else {
	ps = id->grandcentral + ps->next;
      }
      
      pl = &ps->list_in;
      
      firstimethruloop=0;
      
    } /* while(events_left > 0) */
    
    /* can now allow changes to station list */
    et_transfer_unlock(pmystation);
    
    /* no events left to put, start from beginning */
    ps = pmystation;
    
  } /* while(1) */
  
  error:
  et_transfer_unlock(pmystation);
  if (id->debug >= ET_DEBUG_SEVERE) {
    et_logmsg("SEVERE", "et_conductor %d, EXIT THREAD\n", me);
  }
  
  free(allevents); free(putevents); free(temps); free(event_put);
  free(numEvents); free(inListCount); free(place);
  
  pthread_exit(NULL);

  return (NULL);
}


/*
 * Shell's method of sorting from "Numerical Recipes" slightly modified.
 * It is assumed that a and b have indexes from 1 to n. Since the input
 * arrays will start at 0 index, call this routine with something like:
 * shellSort(n, a-1, b-1). Then a[1] as seen in the routine is actually
 * a[0] in the program.
 */
static void shellSort(int n, int a[], int b[]) {
  unsigned long i, j, inc;
  int v, w;
  inc = 1;
  do {
    inc *= 3;
    inc++;
  } while (inc <= n);
  
  do {
    inc /= 3;
    for (i=inc+1; i<=n; i++) {
      v = a[i];
      w = b[i];
      j = i;
      while (a[j-inc] > v) {
        a[j] = a[j-inc];
        b[j] = b[j-inc];
	j -= inc;
	if (j <= inc) break;
      }
      a[j] = v;
      b[j] = w;
    }
  } while (inc > 1);
}
  


 #if 0
/************************************************
 * Thread to conduct events between stations.
 * This conductor places all events that go into a single station
 * into one array then writes it. It looks downstream, one
 * station at a time, and repeats the process. However, unlike
 * et_conductorOld, it optimizes for cases in which the next station
 * is GrandCentral or one which takes all events. In that case, it
 * dumps everything in that station's input list without bothering
 * to sort it. It gives a performance almost identical to the 
 * et_conductorNew for blocking stations, and better than it (but
 * not quite as good as et_conductorOld) for nonblocking stations.
 ***********************************************/
static void *et_conductorSerial(void *arg)
{
  et_id      *id  = (et_id *) arg;
  et_sys_id  etid = (et_sys_id) arg;
  et_system  *sys = id->sys;
  int        event_depth = sys->config.nevents;
  int        i, j, status, me=0, num_temp=0, numwritten, none=1;
  int	     firstimethruloop, writeall;
  int        events_total, events_left, events_toput;
  short      *event_put;
  et_event   **allevents, **putevents, **temps;
  et_station *ps, *pmystation;
  et_list    *pl, *pmylist;  
  void       *sym;
  const int   forever = 1;
	   
  /* station mutex has already been locked by et_station_create */
  
  /* find the station in act of being created */   
  ps = id->grandcentral;
  for (i=0 ; i < sys->config.nstations ; i++){
    if (ps->data.status == ET_STATION_CREATING) {
        none = 0;
        break;
    }
    ps++;
    me++;
  }
  
  if (none) {
    if (id->debug >= ET_DEBUG_WARN) {
      et_logmsg("WARN", "et_conductor, all stations have their conductors already\n");
    }
    pthread_exit(NULL);
  }
  
  /* found station, label as conductor (this thread) attached */
  pmystation = ps;
  if (id->debug >= ET_DEBUG_INFO) {
    et_logmsg("INFO", "et_conductor %d, found station %p\n", me, ps);
  }
  /* my station's event list_out */
  pmylist = &pmystation->list_out;
  
  /* if using default event selection routine */
  if (ps->config.select_mode == ET_STATION_SELECT_MATCH) {
    ps->data.func = et_condition;
  }
  /* if using user-defined event selection routine, load it */
  else if (ps->config.select_mode == ET_STATION_SELECT_USER) {
    ps->data.lib_handle = dlopen(ps->config.lib, RTLD_NOW);
    if (ps->data.lib_handle == NULL) {
      /* tell et_station_create that we are done */
      pthread_cond_signal(&sys->statdone);
      if (id->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "et_conductor %d, %s\n", me, dlerror());
      }
      pthread_exit(NULL);
    }
    if (id->debug >= ET_DEBUG_INFO) {
      et_logmsg("INFO", "et_conductor %d, loaded shared lib %s\n", me, ps->config.lib);
    }
    sym = dlsym(ps->data.lib_handle, ps->config.fname);
    if (sym == NULL) {
      dlclose(ps->data.lib_handle);
      /* tell et_station_create that we are done */
      pthread_cond_signal(&sys->statdone);
      if (id->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "et_conductor %d, %s\n", me, dlerror()) ;
      }
      pthread_exit(NULL);
    }
    if (id->debug >= ET_DEBUG_INFO) {
      et_logmsg("INFO", "et_conductor %d, loaded function %s\n", me, ps->config.fname);
    }
    ps->data.func = (ET_SELECT_FUNCPTR) sym;
  }
  
  /* calloc arrays since we don't know how big they are at compile time */
  
  if ( (allevents = (et_event **) calloc(event_depth, sizeof(et_event *))) == NULL) {
   /* tell et_station_create that we are done */
    pthread_cond_signal(&sys->statdone);
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_conductor %d, no mem left\n", me) ;
    }
    pthread_exit(NULL);
  }
  if ( (putevents = (et_event **) calloc(event_depth, sizeof(et_event *))) == NULL) {
    pthread_cond_signal(&sys->statdone);
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_conductor %d, no mem left\n", me) ;
    }
    pthread_exit(NULL);
  }
  if ( (temps = (et_event **) calloc(event_depth, sizeof(et_event *))) == NULL) {
    pthread_cond_signal(&sys->statdone);
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_conductor %d, no mem left\n", me) ;
    }
    pthread_exit(NULL);
  }
  if ( (event_put = (short *) calloc(event_depth, sizeof(short))) == NULL) {
    pthread_cond_signal(&sys->statdone);
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_conductor %d, no mem left\n", me) ;
    }
    pthread_exit(NULL);
  }

  /* tell et_station_create that we have successfully started */
  if (ps == id->grandcentral) {
    ps->data.status = ET_STATION_ACTIVE;
  } else {
    ps->data.status = ET_STATION_IDLE;
  }
  pthread_cond_signal(&sys->statdone);
  
  
  while (forever) {

    /* wait on condition var for these events */
    et_llist_lock(pmylist);
    while (pmylist->cnt < 1) {
      status = pthread_cond_wait(&pmylist->cread, &pmylist->mutex);
      if (status != 0) {
        err_abort(status, "Wait list_out cond var");
      }
      /* kill self if station removed */  
      if (pmystation->conductor == ET_THREAD_KILL) {
	et_llist_unlock(pmylist);
	if (pmystation->config.select_mode == ET_STATION_SELECT_USER) {
          dlclose(pmystation->data.lib_handle);
        }
	free(allevents); free(putevents); free(temps); free(event_put);
	pthread_exit(NULL);
      }
    }
    
    /* grab all events in station's list_out */
    if ((events_total = et_llist_read(pmylist, allevents)) < 0) {
      et_llist_unlock(pmylist);
      goto error;
    }
    et_llist_unlock(pmylist);
    events_left = events_total;
    /*printf(" %d: events total = %d\n", me, events_total);*/

    /* reinit items to track events being put */
    for (i=0; i < events_total ; i++) {
      event_put[i] = 0;
    }
    firstimethruloop = 1;
    writeall = 0;
    
    /* grabbing mutex allows no change to linked list of created stations */
    et_transfer_lock(pmystation);
    
    /* send events downstream by going to next */
    /* active station & putting in list        */
    if (pmystation->next < 0) {
      ps = id->grandcentral;
    }
    else {
      ps = id->grandcentral + pmystation->next;
    }
    pl = &ps->list_in;    

    while (events_left > 0) {
    
      /* only look at active stations ... */
      if (ps->data.status == ET_STATION_ACTIVE) {
        /* init here */
        events_toput = 0;
        
        /* OPTIMIZATION:
         * If this is the first time thru the while loop, no events have yet
         * been divided up between the stations downstream. If the next station
         * is GrandCentral or a blocking station with prescale=1, then save
         * time by dumping everything directly into it.
         */
      
        if (ps == id->grandcentral) {	  
          num_temp = 0;
          /* if first time thru while loop, dump everything into GC */
	  if (firstimethruloop) {
	    events_toput = events_total;
            for (i=0; i < events_total ; i++) {
              /* find temp events for elimination */
              if (allevents[i]->temp == ET_EVENT_TEMP) {
	        temps[num_temp++] = allevents[i];
	      }
            }
	  }
	  else {
            for (i=0; i < events_total ; i++) {
              /* do next event if already put in another station */
              if (event_put[i]) {
                continue;
	      }
              /* find temp events for elimination */
              if (allevents[i]->temp == ET_EVENT_TEMP) {
	        temps[num_temp++] = allevents[i];
	      }
	      putevents[events_toput++] = allevents[i];
            }
	  }
        }
          
        /* all events, blocking */
        else if ((ps->config.select_mode == ET_STATION_SELECT_ALL) &&
	         (ps->config.block_mode  == ET_STATION_BLOCKING  ))  {
       
          /* if first time thru while loop, dump everything into station */
          if ((firstimethruloop) && (ps->config.prescale == 1))  {
	    writeall = 1;
	    events_toput = events_total;
            et_llist_lock(pl);
	    pl->events_try = et_bigint_add(pl->events_try, events_total);
	    /* pl->events_try += events_total; */
	  }
	  else {
            et_llist_lock(pl);
            for (i=0; i < events_total ; i++) {
              if (event_put[i]) {
        	continue;
	      }
              /* apply prescale */ 
	      /* if (((pl->events_try++)%(ps->config.prescale)) == 0) { */
	      if (et_bigint_mod(pl->events_try, ps->config.prescale) == 0) {
		putevents[events_toput++] = allevents[i];
		event_put[i] = 1;
	      }
	      pl->events_try = et_bigint_add(pl->events_try, 1);
            }
	    if (events_toput == 0) {
	      et_llist_unlock(pl);
	    }
	  }
        }
        
        /* all events, nonblocking */
        /* note: pl->cnt IS mutex protected here !! */
        else if ((ps->config.select_mode == ET_STATION_SELECT_ALL)  &&
	         (ps->config.block_mode  == ET_STATION_NONBLOCKING))  {
          et_llist_lock(pl);
          if (pl->cnt < ps->config.cue) {
            j = ps->config.cue - pl->cnt;
            for (i=0; i < events_total ; i++) {
              if (event_put[i]) {
                continue;
	      }
	      putevents[events_toput++] = allevents[i];
	      event_put[i] = 1;
              if (--j == 0) {
		break;
	      }
            }
          }
	  if (events_toput == 0) {
	    et_llist_unlock(pl);
	  }
        }
        
        /*  condition (user or match), blocking */
        else if (ps->config.block_mode == ET_STATION_BLOCKING) {
          et_llist_lock(pl);
          for (i=0; i < events_total ; i++) {
            if (event_put[i]) {
              continue;
	    }
	    if ((*ps->data.func)(etid, ps->num, allevents[i])) {
	      /* if (((pl->events_try++)%(ps->config.prescale)) == 0) { */
	      if (et_bigint_mod(pl->events_try, ps->config.prescale) == 0) {
	        putevents[events_toput++] = allevents[i];
	        event_put[i] = 1;
	      }
	      pl->events_try = et_bigint_add(pl->events_try, 1);
	    }
	  }
	  if (events_toput == 0) {
	    et_llist_unlock(pl);
	  }
        }
        
	/* condition (user or match) + nonblocking */
        else if (ps->config.block_mode == ET_STATION_NONBLOCKING) {
          et_llist_lock(pl);
          if (pl->cnt < ps->config.cue) {
            j = ps->config.cue - pl->cnt;
            for(i=0; i < events_total ; i++) {
              if (event_put[i]) {
                continue;
	      }
	      if ((*ps->data.func)(etid, ps->num, allevents[i])) {
	        putevents[events_toput++] = allevents[i];
	        event_put[i] = 1;
		if (--j == 0) {
		  break;
		}
	      }
            }
          }
	  if (events_toput == 0) {
	    et_llist_unlock(pl);
	  }
	}

 	/* if items go in this station ... */
        if (events_toput) {
	  
	  if (ps == id->grandcentral) {
            /* if tmp events exist, remove */
            if (temps[0] != NULL) {
              et_system_lock(sys);
              for (i=0; i < num_temp ; i++) {
                /*
		 * temp events are not mapped in ET system space
		 * but their files need to be removed
		 */
		unlink(temps[i]->filename);
                sys->ntemps--;
/*printf("conductor %d: unlink tmp event\n", me);*/
              }
	      et_system_unlock(sys);
	    }
            /* lock mutex of gc station's list_in */
            et_llist_lock(pl);
	    if (firstimethruloop) {
              status = et_llist_write_gc(id, allevents, events_total);
	    }
	    else { 
              status = et_llist_write_gc(id, putevents, events_toput);
            }
	    et_llist_unlock(pl);
	    if (status == ET_ERROR) {
	      goto error;
	    }
            events_left = 0;	
	  }
	  
	  else {
	    /* pl is already locked */
 	    if (writeall) {
              numwritten = et_llist_write(id, pl, allevents, events_total);
	      et_llist_unlock(pl);
              events_left = 0;
	      writeall = 0;
            }
	    else {
	      numwritten = et_llist_write(id, pl, putevents, events_toput);
 	      et_llist_unlock(pl);
              events_left -= events_toput;
            }
            if (numwritten == ET_ERROR) {
              goto error;
            }
	    else if (numwritten < events_toput) {
	      /* this one should never happen */
              if (id->debug >= ET_DEBUG_SEVERE) {
                et_logmsg("SEVERE", "et_conductor %d, cannot write all events to %s input\n", me, ps->name);
              }
              goto error;
	    }
          }
	  
          /* signal reader that new events are here */
          status = pthread_cond_broadcast(&pl->cread);
          if(status != 0) {
            err_abort(status, "events here");
	  }
	   	  
        } /* if items go in this station */
      } /* if station active */
      
      if (ps == id->grandcentral) {
        break;
      }
      if (ps->next < 0) {
	ps = id->grandcentral;
      }
      else {
	ps = id->grandcentral + ps->next;
      }
      pl = &ps->list_in;
      
      firstimethruloop=0;
      
    } /* while(events_left > 0) */
    
    /* can now allow changes to station list */
    et_transfer_unlock(pmystation);
    
    /* no events left to put, start from beginning */
    ps = pmystation;
    
  } /* while(1) */
  
  error:
  et_transfer_unlock(pmystation);
  if (id->debug >= ET_DEBUG_SEVERE) {
    et_logmsg("SEVERE", "et_conductor %d, EXIT THREAD\n", me);
  }
  free(allevents); free(putevents); free(temps); free(event_put);
  pthread_exit(NULL);

  return (NULL);
}
#endif

/******************************************************/
/*
 * Creates grandcentral station & fills input list with the
 * pointers to all events. Starts its conducting thread.
 */

static int et_grandcentral_station_create(et_id *id)
{
  int            i, status;
  et_statconfig  sconfig;
  et_event       *pe = id->events;
  pthread_t      thd_id;
  pthread_attr_t attr;
  et_stat_id     stat_id;
  
  /* set conductor thd to detach */  
  status = pthread_attr_init(&attr);
  if(status != 0) {
    err_abort(status, "Init thd attr");
  }
  status = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if(status != 0) {
    err_abort(status, "Set thd detach");
  }

  /* set GRAND_CENTRAL station parameters */  
  et_station_config_init(&sconfig);
  et_station_config_setuser(sconfig, ET_STATION_USER_MULTI);
  et_station_config_setrestore(sconfig, ET_STATION_RESTORE_GC);
  et_station_config_setselect(sconfig, ET_STATION_SELECT_ALL);
  et_station_config_setblock(sconfig, ET_STATION_BLOCKING);
  et_station_config_setprescale(sconfig, 1);

  if ((status = et_station_create((et_sys_id) id, &stat_id, "GRAND_CENTRAL", sconfig)) != ET_OK) {
    if (id->debug >= ET_DEBUG_SEVERE) {
      et_logmsg("SEVERE", "et_grandcentral_station_create, cannot create, status = %d\n", status) ;
    }
    et_station_config_destroy(sconfig);
    return ET_ERROR;
  }
  et_station_config_destroy(sconfig);

  /* Fill grand_central station input list with all events */
  for (i=0 ; i < id->sys->config.nevents ; i++) {
    if (et_llist_write_gc(id, &pe, 1) != ET_OK) {
      if (id->debug >= ET_DEBUG_SEVERE) {
        et_logmsg("SEVERE", "et_grandcentral_station_create, error writing to input list\n") ;
      }
      return ET_ERROR;
    }
    pe++;
  }
  /* Do not include this initial filling of GrandCentral with events in
   * the general statistics keeping - undo et_llist_write_gc's stats
   */
  et_init_stats_station(id->grandcentral);
  
  /* Add "conductor" thread to move events to next station. */
  status = pthread_create(&thd_id, &attr, et_conductor, (void *) id);
  if(status != 0) {
    err_abort(status, "Create et_conductor thd");
  }

  status = pthread_attr_destroy(&attr);
  if(status != 0) {
    err_abort(status, "Thread attr destroy");
  }
  
  if (id->debug >= ET_DEBUG_INFO) {
    et_logmsg("INFO", "et_grandcentral_station_create, finished\n");
  }
  return ET_OK;
}

/******************************************************/
static int et_condition(et_sys_id id, et_stat_id stat_id, et_event *pe)
{
  int i, result=0;
  et_station *ps = ((et_id *)id)->stats + stat_id;

  for (i=0; i < ET_STATION_SELECT_INTS ; i++) {
    if (i%2 == 0) {
      result = result || ((ps->config.select[i] != -1) &&
                          (ps->config.select[i] == pe->control[i]));
    }
    else {
      result = result || ((ps->config.select[i] != -1) &&
                          (ps->config.select[i] & pe->control[i]));
    }
  }
  return result;
}


/*****************************************************/
static int et_system_config_check(et_id *id, et_sys_config *config)
{
  /* check all configuration settings and return changes */
  
  if (config->nevents < 1) {
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_config_check, bad value for number of events\n") ;
    }
    return ET_ERROR;
  }
  
  if (config->event_size < 1) {
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_config_check, bad value for event size\n") ;
    }
    return ET_ERROR;
  }
  
  if (config->ntemps < 1) {
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_config_check, bad value for number of temp events\n") ;
    }
    return ET_ERROR;
  }
  else if (config->ntemps > config->nevents) {
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_config_check, ntemps cannot be larger than nevents\n") ;
    }
    return ET_ERROR;
  }
  
  if (config->nstations < 1) {
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_config_check, bad value for number of stations\n") ;
    }
    return ET_ERROR;
  }
  
  if (config->nattachments < 1) {
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_config_check, bad value for number of attachments\n") ;
    }
    return ET_ERROR;
  }
  else if (config->nattachments > ET_ATTACHMENTS_MAX) {
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_config_check, # of attachments is too large, increase ET_ATTACHMENTS_MAX and recompile\n") ;
    }
    return ET_ERROR;
  }
  
  if (config->nprocesses < 1) {
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_config_check, bad value for number of processes\n") ;
    }
    return ET_ERROR;
  }
  else if (config->nprocesses > ET_PROCESSES_MAX) {
    if (id->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_system_config_check, # of processes is too large, increase ET_ATTACHMENTS_MAX and recompile\n") ;
    }
    return ET_ERROR;
  }
    
  /* make sure filename is null-terminated string */
  config->filename[ET_FILENAME_LENGTH - 1] = '\0';
  
  return ET_OK;
}

