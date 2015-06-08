




/* etmacros.h - some useful macros */

/* MACROS */

#define ETVARS1 \
et_openconfig  openconfig; \
et_statconfig  sconfig; \
et_stat_id     my_stat; \
et_sys_id      id; \
int   et_init; \
int  nevents_max, event_size

#define ETVARS2 \
  ETSYS *sysptr; \
  et_event **pe; \
  et_att_id  attach; \
  struct timespec timeout


#define ETOPEN \
  if(!et_init) /* ET initialization */ \
  { \
    et_open_config_init(&openconfig); \
    et_open_config_setwait(openconfig, ET_OPEN_WAIT); \
    if(et_open(&id, et_file_name, openconfig) != ET_OK) \
    { \
      printf("pr_et_init: cannot open ET system\n"); \
      et_init = 0; \
      return(-1); \
    } \
    else \
    { \
      et_init = 1; \
    } \
    et_open_config_destroy(openconfig); \
  } \
  /* \
  Now that we have access to an ET system, find out how many \
  events it has and what size they are. Then allocate an array \
  of pointers to use for reading, writing, and modifying these events. \
  */ \
  if(et_system_getnumevents(id, &nevents_max) != ET_OK) \
  { \
    printf("pr_et_init: ET has died\n"); \
  } \
  printf("pr_et_init: nevents_max=%d\n",nevents_max); \
  if(et_system_geteventsize(id, &event_size) != ET_OK) \
  { \
    printf("pr_et_init: ET has died\n"); \
  } \
  printf("pr_et_init: event_size=%d\n",event_size); \
  et_station_config_init(&sconfig); \
  et_station_config_setuser(sconfig, ET_STATION_USER_MULTI); \
  /*et_station_config_setrestore(sconfig, ET_STATION_RESTORE_IN);*/ \
  et_station_config_setrestore(sconfig, ET_STATION_RESTORE_OUT); \
  et_station_config_setprescale(sconfig, 1); \
  /* old "all" mode (BLOCKING only ???) */ \
  et_station_config_setselect(sconfig, ET_STATION_SELECT_ALL); \
  /* if ET_STATION_BLOCKING  \
  et_station_config_setblock(sconfig, ET_STATION_BLOCKING);*/ \
  /* if ET_STATION_NONBLOCKING */ \
  et_station_config_setblock(sconfig, ET_STATION_NONBLOCKING); \
  et_station_config_setcue(sconfig, 100); \
  /* set debug level */ \
  et_system_setdebug(id, ET_DEBUG_INFO); \
  { \
    int status; \
    if((status = et_station_create(id, &my_stat, et_station_name, sconfig)) < 0) \
    { \
      if(status == ET_ERROR_EXISTS) \
      { \
        /* my_stat contains pointer to existing station */ \
        printf("pr_et_init: station exists, will attach\n"); \
      } \
      else if (status == ET_ERROR_TOOMANY) \
      { \
        printf("pr_et_init: too many stations created\n"); \
        return(-2); \
      } \
      else \
      { \
        printf("pr_et_init: error in station creation\n"); \
        return(-3); \
      } \
    } \
  } \
  printf("pr_et_init: station ready\n")

#define ETOPENB \
  if(!et_init) /* ET initialization */ \
  { \
    et_open_config_init(&openconfig); \
    et_open_config_setwait(openconfig, ET_OPEN_WAIT); \
    if(et_open(&id, et_file_name, openconfig) != ET_OK) \
    { \
      printf("pr_et_init: cannot open ET system\n"); \
      et_init = 0; \
      return(-1); \
    } \
    else \
    { \
      et_init = 1; \
    } \
    et_open_config_destroy(openconfig); \
  } \
  /* \
  Now that we have access to an ET system, find out how many \
  events it has and what size they are. Then allocate an array \
  of pointers to use for reading, writing, and modifying these events. \
  */ \
  if(et_system_getnumevents(id, &nevents_max) != ET_OK) \
  { \
    printf("pr_et_init: ET has died\n"); \
  } \
  printf("pr_et_init: nevents_max=%d\n",nevents_max); \
  if(et_system_geteventsize(id, &event_size) != ET_OK) \
  { \
    printf("pr_et_init: ET has died\n"); \
  } \
  printf("pr_et_init: event_size=%d\n",event_size); \
  et_station_config_init(&sconfig); \
  et_station_config_setuser(sconfig, ET_STATION_USER_MULTI); \
  /*et_station_config_setrestore(sconfig, ET_STATION_RESTORE_IN);*/ \
  et_station_config_setrestore(sconfig, ET_STATION_RESTORE_OUT); \
  et_station_config_setprescale(sconfig, 1); \
  /* old "all" mode (BLOCKING only ???) */ \
  et_station_config_setselect(sconfig, ET_STATION_SELECT_ALL); \
  /* if ET_STATION_BLOCKING */ \
  et_station_config_setblock(sconfig, ET_STATION_BLOCKING); \
et_station_config_setcue(sconfig, 100); /* ignore it for blocking ??? */ \
  /* if ET_STATION_NONBLOCKING  \
  et_station_config_setblock(sconfig, ET_STATION_NONBLOCKING); \
  et_station_config_setcue(sconfig, 100);*/ \
  /* set debug level */ \
  et_system_setdebug(id, ET_DEBUG_INFO); \
  { \
    int status; \
    if((status = et_station_create(id, &my_stat, et_station_name, sconfig)) < 0) \
    { \
      if(status == ET_ERROR_EXISTS) \
      { \
        /* my_stat contains pointer to existing station */ \
        printf("pr_et_init: station exists, will attach\n"); \
      } \
      else if (status == ET_ERROR_TOOMANY) \
      { \
        printf("pr_et_init: too many stations created\n"); \
        return(-2); \
      } \
      else \
      { \
        printf("pr_et_init: error in station creation\n"); \
        return(-3); \
      } \
    } \
  } \
  printf("pr_et_init: station ready\n")

#define ETCLOSE \
    et_init = 0; \
    if(et_station_remove(id, my_stat) != ET_OK) \
    { \
      printf("l3_main: error in station remove\n"); \
      return(-2); \
    } \
    if(et_close(id) != ET_OK) \
    { \
      printf("l3_main: cannot close ET system\n"); \
      return(-3); \
    }

#define ETATTACH(attach__) \
  timeout.tv_sec  = 0; \
  timeout.tv_nsec = 20000000; /* 20 ms */ \
  if(et_station_attach(id, my_stat, &(attach__)) < 0) \
  { \
    printf("etmacros: error in station attach\n"); \
    return; \
  } \
  /* allocate a system area for the etbosio */ \
  etSysalloc(&sysptr)

#define ETDETACH \
  /* release a system area for the etbosio */ \
  etSysfree(sysptr); \
  /* detach ET */ \
  if(et_station_detach(id, attach) != ET_OK) \
  { \
    printf("l3_thread: error in station detach\n"); \
    return; \
  }

#define ETGETEVENTS(attach__, pe__, numread__) \
    /* if ET system is dead, wait here until it comes back */ \
	while(!et_alive(id)) \
    { \
	  status = et_wait_for_alive(id); \
	  if(status == ET_OK) \
      { \
	    int locality; \
	    et_system_getlocality(id, &locality); \
	    /* if Linux, re-establish connection to ET system since socket broken */ \
	    if(locality == ET_LOCAL_NOSHARE) \
        { \
          printf("etmacros: try to reconnect Linux client\n"); \
	      et_forcedclose(id); \
	      /*goto restartLinux; means goto et_open_config_init() - what to do in our case ??? */ \
          printf("etmacros: ERROR 123 !!!\n"); \
	    } \
	  } \
	} \
    lock = 1; \
    do \
    { \
      /*status = et_events_get(id, (attach__), (pe__), ET_SLEEP, NULL, chunk, &(numread__));*/ \
      status = et_events_get(id, (attach__), (pe__), ET_TIMED, &timeout, chunk, &(numread__)); \
      /*printf("l3_thread: geting %d events from ET, status=%d\n",(numread__),status);*/ \
      if (status == ET_OK) \
      { \
       ntimeouts = 0; \
       if(lock==0) pthread_mutex_lock(&mm->mutex_fifo); \
       /*printf("etmacros: numread1=%d\n",(numread__)); \
       for(j=0; j<(numread__); j++) \
       { \
         len = (pe__)[j]->length>>2; \
         printf("etmacros: event type %d length %d (bytes)\n", \
                               (pe__)[j]->control[0], (pe__)[j]->length); \
        } \
        printf("\n"); \
        exit(0);*/ \
      } \
      else if (status == ET_ERROR_BUSY) \
      { \
        if(lock==1) pthread_mutex_unlock(&mm->mutex_fifo); \
        lock = 0; \
        printf("etmacros: Trying to get event from ET but station is busy - try again\n"); \
        status = ET_OK; \
      } \
      else if (status == ET_ERROR_TIMEOUT) \
      { \
        if(ntimeouts < 100000) \
        { \
          /*printf("ntimeouts = %d (0x%08x)\n",ntimeouts,&ntimeouts);*/ \
          ntimeouts ++; \
        } \
        if(lock==1) pthread_mutex_unlock(&mm->mutex_fifo); \
        /*if(lock==1) printf("[%1d] etmacros: got timeout, no events - try again\n",threadid);*/ \
        /*if(lock==1) printf("[%1d] fifo-> %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d\n",threadid, \
        fifo[0],fifo[1],fifo[2],fifo[3],fifo[4], \
        fifo[5],fifo[6],fifo[7],fifo[8],fifo[9],fifo[10],fifo[11]);*/ \
        lock = 0; \
      } \
      else if (status == ET_ERROR_EMPTY) \
      { \
        if(lock==1) pthread_mutex_unlock(&mm->mutex_fifo); \
        lock = 0; \
        printf("etmacros: No events in ET\n"); \
      } \
      else if (status == ET_ERROR_WAKEUP) \
      { \
        if(lock==1) pthread_mutex_unlock(&mm->mutex_fifo); \
        lock = 0; \
        printf("etmacros: someone told me to wake up\n"); \
      } \
      else if (status == ET_ERROR_DEAD) \
      { \
        if(lock==1) pthread_mutex_unlock(&mm->mutex_fifo); \
        lock = 0; \
        printf("etmacros: ET is dead\n"); \
        exit(0); \
      } \
      else if ((status == ET_ERROR_WRITE) || (status == ET_ERROR_READ)) { \
        printf("etmacros: socket communication error\n"); \
        exit(0); \
      } \
      else \
      { \
        printf("etmacros: error %d in et_events_get()\n",status); \
        exit(0); \
      } \
    } while(status < ET_OK && force_exit == 0)

#define ETPUTEVENTS(attach__, pe__, numread__) \
    /*printf("l3_thread: puting %d events back to ET\n",numread);*/ \
    status = et_events_put(id, (attach__), (pe__), (numread__)); \
    if (status == ET_ERROR_DEAD) \
    { \
      printf("l3_thread: detach error\n"); \
      return; \
    } \
    else if ((status == ET_ERROR_WRITE) || (status == ET_ERROR_READ)) { \
      printf("l3_thread: socket communication error\n"); \
      return; \
    } \
    else if (status != ET_OK) \
    { \
      printf("l3_thread: put error\n"); \
      return; \
    } \
    /*printf("l3_thread: put events back to ET, status=%d\n",status);*/

#define ETDUMPEVENTS(attach__, pe__, numread__) \
    /*printf("l3_thread: dumping %d events back to ET\n",numread);*/ \
    status = et_events_dump(id, (attach__), (pe__), (numread__)); \
    if (status == ET_ERROR_DEAD) \
    { \
      printf("l3_thread: detach error\n"); \
      return; \
    } \
    else if ((status == ET_ERROR_WRITE) || (status == ET_ERROR_READ)) { \
      printf("l3_thread: socket communication error\n"); \
      return; \
    } \
    else if (status != ET_OK) \
    { \
      printf("l3_thread: put error\n"); \
      return; \
    } \
    /*printf("l3_thread: dump events back to ET, status=%d\n",status);*/

#define ETGETDATA(pe__) \
      et_event_getdata((pe__)[jj], (void **) &jw); \
      /*printf("jw=%08x\n",(int)jw);*/ \
      et_event_getlength((pe__)[jj], &size); \
      len = etOpen(jw,(event_size/4-10),sysptr); \
      if(size != len) \
      { \
        /*printf("ERROR: etOpen returns %d but ET size is %d\n",len,size);*/ \
      }

#define ETGETDATALAST(pe__) \
      /*printf("1: jw=%08x ->%d\n",(int)jw,mm->numread2[myid]-1);*/ \
      et_event_getdata((pe__)[mm->numread2[myid]-1], (void **) &jw); \
      /*printf("jw=%08x\n",(int)jw);*/ \
      et_event_getlength((pe__)[mm->numread2[myid]-1], &size); \
      len = etOpen(jw,(event_size/4-10),sysptr); \
      if(size != len) \
      { \
        /*printf("ERROR: etOpen returns %d but ET size is %d\n",len,size);*/ \
      }

#define ETPUTDATA(pe__) \
      et_event_getlength((pe__)[jj], &size); \
      len = etClose(jw); \
      if(size != len) \
      { \
        et_event_setlength((pe__)[jj], len); \
        et_event_getlength((pe__)[jj], &size); \
      } \
      if(size != len) \
      { \
        printf("ERROR: etClose returns %d but ET size is %d\n",len,size); \
      }

#define ETPUTDATALAST(pe__) \
      et_event_getlength((pe__)[mm->numread2[myid]-1], &size); \
      len = etClose(jw); \
      if(size != len) \
      { \
        et_event_setlength((pe__)[mm->numread2[myid]-1], len); \
        et_event_getlength((pe__)[mm->numread2[myid]-1], &size); \
      } \
      if(size != len) \
      { \
        printf("ERROR: etClose returns %d but ET size is %d\n",len,size); \
      } \
      /* set control[2] to non-zero value to pay attention from ROOT presenter etc */ \
      /*printf("inject histos !!!\n");*/ \
      (pe__)[mm->numread2[myid]-1]->control[2] = 1


/* end of MACROS */




#define NTHREADMAX 7
#define NFIFOMAX   1000
#define NIDMAX     (NFIFOMAX+NTHREADMAX)
#define NCHUNKMAX  200


typedef struct
{
  int totalsize;
  int usedsize;

  /* initialization variables */
  int runnum;

  /* mutexes and cond. variables */
  pthread_mutex_t newmap;
  pthread_mutex_t mutex_fifo;

  /* attributes for every thread */
  int nthread;

  /* id stack */
  int nid;
  int idstack[NIDMAX];

  /* fifo */
  int nfifo;
  int fifo[NFIFOMAX]; /* protected by mutex_fifo */

  /* attributes for every fifo element */
  et_att_id attach1[NIDMAX];
  int ready[NIDMAX];

  et_event *pe1[NIDMAX][NCHUNKMAX]; /* input event list */
  int numread1[NIDMAX];
  et_event *pe2[NIDMAX][NCHUNKMAX]; /* accept event list */
  int numread2[NIDMAX];
  et_event *pe3[NIDMAX][NCHUNKMAX]; /* reject event list */
  int numread3[NIDMAX];

  /* event rate counters */
  int   time0;
  int   nevents;
  int   nevents1;
  int   nevents2;
  int   Nevents;
  int   Nevents1;
  int   Nevents2;
  float Rate;
  float Rate1;
  float Rate2;

  int   Count[8];
  float Ratio[8];

#ifdef TRLIB
  TRstat trstat;
#endif

} MM;





