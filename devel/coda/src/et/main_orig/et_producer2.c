/*----------------------------------------------------------------------------*
 *  Copyright (c) 1998        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Author:  Randy MacLeod, Carl Timmer                                                    *
 *             timmer@jlab.org                   Jefferson Lab, MS-12H        *
 *             Phone: (757) 269-5130             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *      ET system sample event producer
 *
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include <assert.h>
#include <signal.h>
#include <time.h>
#include <math.h>
/* recent versions of linux put float.h (and DBL_MAX) in a strange place */
#define DOUBLE_MAX   1.7976931348623157E+308

/* for pthread_sigmask */
#define __EXTENSIONS__
#include <stdlib.h>   /* __EXTENSIONS__ for getopt. RWM */
#include <pthread.h>  /* __EXTENSIONS__ for pthread_sigmask. RWM */
#include <unistd.h>   /* __EXTENSIONS__ for usleep. RWM */
#undef __EXTENSIONS__

#include <sys/time.h>
#include <sys/types.h>
#include <sys/errno.h>


#ifdef sun
#include <thread.h>
#endif

#include "et.h"
#include "et_producer.h"
#define DBL_MAX   1.e+100

/************************************/
/*           Globals.               */
/************************************/
volatile int    statistician_is_sleeping = FALSE;
etp_stats_t     stats;
double		secondsPerLoop = 5.e-7;
int		*eventData;

/******************************************************************/
void timeDelay(int usec) {
  int i, loops;
  double u;
  
  loops = usec*1.e-6/secondsPerLoop;
  /* printf("timeDelay: %d loops for %d usec\n",loops, usec); */
    
  for (i=0; i < loops; i++) {
    u = sqrt(pow((double)i, 2.));
  }
  return;
}

/******************************************************************/
int fill_buf(long *buf, int length)
{
  int i;
  
  if (length < 4)
    return ET_ERROR;
  
  buf[0] = length - 1;
  buf[1] = 0x000210cc;
  buf[2] = length - 3;
  buf[3] = 0xc0000100;
  
  for (i=4; i < length; i++) {
    buf[i] = i+1;
  }
  
  return ET_OK;
}

/******************************************************************/
void calibrateDelay() {
  int i, loops = 5000000;
  double u, time;
  struct timeval t1, t2;
  
  gettimeofday(&t1, NULL);
  for (i=0; i < loops; i++) {
    u = sqrt(pow((double)i, 2.));
  }
  gettimeofday(&t2, NULL);
  time = (double) (t2.tv_sec - t1.tv_sec) + 1.e-6*(t2.tv_usec - t1.tv_usec);
  secondsPerLoop = time/loops;
  printf("calibrateDelay: %e seconds/loop, time of measurement = %4.2f sec\n",
		secondsPerLoop, time);
  return;
}

/******************************************************************/
int main(int argc,char **argv) {  
  opt_t           options;
  prod_conf_t     conf;
  int             status = 0;
  
  options = parse_command_line_et_pro(argc, argv);
  conf    = conf_et_producer(&options);
  status  = et_prod_run_et(options, conf);

  return (status);
}

/******************************************************************/
int et_prod_run_et(opt_t options, prod_conf_t conf) {

  /* array of pointers to events. */
  et_event      **pe;
  char           *pev_data = NULL;
  /*
  int             priority = ET_HIGH;
  int             control[] = {17,8,-1,-1};
  char            tmp;
  */

  struct timespec sometime;
  int             nap_count;
  int             numread, status;
  int             i;
  void           *pdata;

  /* Allocate array of pointers to et events. */
  if ( (pe = (et_event **) calloc(options.ev_in_chunk, sizeof(et_event *))) == NULL) {
    printf("et_producer2: cannot allocate memory");
    return(-1);
  }

  /* Allocate a buffer for local storage of events. */
  if ( (pev_data = (char *) calloc(options.ev_size, 1)) == NULL) {
    printf("et_producer2: cannot allocate memory");
    return(-1);
  }

/* stats.mutex = PTHREAD_MUTEX_INITIALIZER; */

  while (et_alive(conf.id)) {
    nap_count = 0;
    while ( ! statistician_is_sleeping) {
      nap_count++;
      sometime.tv_sec  = ET_PROD_NAP_SEC;
      sometime.tv_nsec = ET_PROD_NAP_NSEC;
      nanosleep(&sometime, NULL);
    }
    pthread_mutex_lock(&stats.mutex);

#ifdef DEBUG
    printf("et_producer2: Napped %d times for a total of %d nanoseconds.\n",
	   nap_count, nap_count * (1000000* ET_PROD_NAP_SEC +  ET_PROD_NAP_NSEC));
#endif

    /* start_snapshot() */
    stats.nChunks = 0;
#if defined linux || defined __APPLE__
    gettimeofday(&stats.start, NULL);
#else
    clock_gettime(CLOCK_REALTIME, &stats.start);
#endif
    while (statistician_is_sleeping) {
      stats.evCount = 0;
      status = et_events_new(conf.id, conf.att_id, pe, ET_SLEEP, NULL,
		 options.ev_size, options.ev_in_chunk, &numread);
      if (status == ET_OK) {
	stats.evCount += numread;
      } else if (status == ET_ERROR_DEAD) {
	printf("et_producer2: ET system is dead\n");
	break;
      } else if (status == ET_ERROR_TIMEOUT) {
	printf("et_producer2: got timeout\n");
	break;
      } else if (status == ET_ERROR_EMPTY) {
	printf("et_producer2: no events\n");
	break;
      } else if (status == ET_ERROR_BUSY) {
	printf("et_producer2: grandcentral is busy\n");
	break;
      } else if (status == ET_ERROR_WAKEUP) {
	printf("et_producer2: someone told me to wake up\n");
	break;
      } else if (status != ET_OK) {
	printf("et_producer2: request error\n");
	return(-2);
      }
      
      for (i=0; i < numread ; i++) {
	/* set control values,  write data, set priority, */
        /*
	if (i%5 == 0) {
          et_event_setpriority(pe[i], priority);
        }
	
	et_event_setcontrol(pe[i], control, 4);
	*/
	if (options.ev_size) {
	  et_event_getdata(pe[i], &pdata);
	  /* memcpy(pdata, pev_data, options.ev_size); */
	  memcpy(pdata, (void *)eventData, options.ev_size);
	}
	et_event_setlength(pe[i], options.ev_size);

	/* if (options.incr_word && options.ev_size) { */
	  /* increment each word by one to test transport. */
	  /* tmp = pev_data[0];
	  tmp++;
	  memset(pev_data, tmp, options.ev_size/sizeof(int));
	} */
	
	if (options.udelay != 0) {
	  timeDelay(options.udelay);
	  /*
	  sometime.tv_sec  = 0;
	  sometime.tv_nsec = 1000 * options.udelay;
	  if (nanosleep(&sometime, NULL) != 0) {
	    puts("et_producer2 WARNING: nanosleep failed! ignoring.");
	    fflush(stdout);
	  }
	  */
	}
      } /* for ev_in_chunk */

/*       printf("evC = %d, o.ev_in_chunk = %d.\n", stats.evCount, options.ev_in_chunk); */

      /* assert (stats.evCount == options.ev_in_chunk); */
      status = et_events_put(conf.id, conf.att_id, pe, numread);
      
      if (status == ET_OK) {
	;
      } else if (status == ET_ERROR_DEAD) {
	printf("et_producer2: ET is dead\n");
	break;
      } else if (status != ET_OK) {
	printf("et_producer2: ERROR\n");
	return(-2);
      } else {
	printf("et_producer2: Unknown error from et_events_put. Exiting.\n");
	exit(-1);
      }

      stats.nChunks++;
    } /* while (statistician_is_sleeping) */
  
#if defined linux || defined __APPLE__
    gettimeofday(&stats.end, NULL);
#else
    clock_gettime(CLOCK_REALTIME, &stats.end);
#endif
    pthread_mutex_unlock(&stats.mutex);
      
    if (!et_alive(conf.id)) {
      et_wait_for_alive(conf.id);
    }
  } /* while (et_alive(...)) */
  return(0);
}


/************************************************************/
/*             Statistician thread                          */
/************************************************************/
static void * statistician(void *arg)
{
  const int forever = 1;
  double    eventsProduced;
  double    timeDelta;
  double    freq=0.0, freq_tot=0.0, freq_avg=0.0;
  int       iterations=1;
  time_t    start_time;
  etp_stats_t  *pStats = (etp_stats_t  *) arg;


  start_time = time(NULL);
  while (forever) {

    statistician_is_sleeping = TRUE;
    sleep(pStats->report_period);

    statistician_is_sleeping = FALSE;
    pthread_mutex_lock(&pStats->mutex);

    /* Calculate statistics. */
#if defined linux || defined __APPLE__
    timeDelta = (double) (pStats->end.tv_sec  - pStats->start.tv_sec) + 
                1.0e-6 * (pStats->end.tv_usec - pStats->start.tv_usec);
#else
    timeDelta = (double) (pStats->end.tv_sec  - pStats->start.tv_sec) + 
                1.0e-9 * (pStats->end.tv_nsec - pStats->start.tv_nsec);
#endif
    eventsProduced = (double) (pStats->ev_in_chunk * pStats->nChunks + pStats->evCount);
    if (timeDelta > 0.0) {
      freq = eventsProduced/timeDelta;
    }

    if ((DOUBLE_MAX - freq_tot) < freq) {
      freq_tot   = 0.0;
      iterations = 1;
      puts("Reset counters - huh?");
    }

    freq_tot += freq;
    freq_avg = freq_tot/((double)iterations);
    iterations++;

/* res_nsec */

    /*    printf("et_producer2: time = %d sec, %.0f Hz,  %.0f Hz Avg.\n",  */
    printf(" %10.2f   %10.2f  %10.2f\n", 
           timeDelta, freq, freq_avg);
    /*     printf(" eventsProduced = %.0f, timeDelta = %f.\n", eventsProduced, timeDelta); */
    pthread_mutex_unlock(&pStats->mutex);

  }
  return (NULL);
}

/************************************************************/
/*              thread to handle signals                    */
/************************************************************/
static void * signal_thread (void *arg)
{
  sigset_t   signal_set;
  int        sig_number;
 
  sigemptyset(&signal_set);
  sigaddset(&signal_set, SIGINT);

  /* Wait for the INTerrupt signal - Control-C. */
  if (sigwait(&signal_set, &sig_number) != -1) {
    /* Not necessary to clean up as ET system will do it */
    exit(-1);
  } else {
    perror("et_producer2: ");
  }
  return (NULL);
}

opt_t default_options(void) {

  opt_t opt;

  opt.ev_in_chunk = EV_IN_CHUNK;
  opt.ev_size     = EV_SIZE;
  opt.incr_word   = FALSE;
  opt.et_filename = NULL;
  opt.verbose     = FALSE;
  opt.udelay      = EV_UDELAY;

  return (opt);
}

/******************************************************************/
/*            Parse command line arguments                        */
/*            and fill options structure.                         */
/******************************************************************/

opt_t parse_command_line_et_pro(int my_argc, char* my_argv[]) {


  extern char    *optarg;
  extern int      optind;

  opt_t           opt;

  int             delay = 0;
  int             c;
  int             errflg = 0;


  opt = default_options();

  while ((c = getopt(my_argc, my_argv, "vihc:d:s:")) != EOF) {

    switch (c) {

    case 'v':
      opt.verbose = 1;
      break;
    case 'i':
      opt.incr_word = 1;
      break;
    case 'c':
      /* Event chunk size. */
      if (atoi(optarg) > 0) {
	opt.ev_in_chunk = atoi(optarg);
      } else {
	printf("Invalid value for -c, must be a positive integer.\n");
	errflg++;
      }
      break;
    case 'd':
      delay = strtol(optarg, (char **)NULL, 10);
      if (delay >= 0) {
        if (delay < (1000 * 1000)) {
	  opt.udelay = delay; /* convert to useconds. */
	} else {
          printf("Invalid value for -d, must be less than 1,000,000 useconds.\n");
          errflg++;
        }
      } else {
	printf("Invalid value for -d, must be a positive integer or zero.\n");
	errflg++;
      }
      break;
    case 's':
      if (atoi(optarg) >= 0) {
	opt.ev_size = atoi(optarg);
      } else {
	printf("Invalid value for -s, must be a positive integer or zero.\n");
	errflg++;
      }
      break;
    case '?':
    case 'h':
      errflg++;
    }
  }

  if (errflg || (my_argc < 2)) {
    (void)fprintf(stderr,
		  "usage: %s [-v] [-s event_size] [-c chunk_size] [-d delay(us)] [-i] <et_filename>\n", 
		  my_argv[0]);
    exit (2);
  }

  /* Save pointer to et_filename. */
  opt.et_filename = my_argv[optind];

  if (opt.verbose) {
    printf("\net_producer2:\n");
    printf("  Running in verbose mode.\n");
    printf("  Putting events into ET in chunks of %d events.\n", opt.ev_in_chunk);
    printf("  Event size is %d bytes.\n", opt.ev_size);
    /* Print parsed values */
    printf("  Event size = %d,\n", opt.ev_size);
    printf("  Events in a chunk = %d,\n", opt.ev_in_chunk);
    if (opt.incr_word) {
      printf("  Will increment value of each word put into ET system.\n");
    }
    printf("  et_filename = %s.\n", opt.et_filename);
    printf("  Delay between events is %d microseconds.\n", opt.udelay);
    printf("\n");
  }

  return (opt);
}

prod_conf_t  conf_et_producer(opt_t *pOpts) {

  prod_conf_t    s;
  struct timespec res;
  sigset_t        sigblock;
  int             total_et_events;
  pthread_t       tid, stat_tid;
  int             max_event_size; 
  et_openconfig   openconfig;

  res.tv_sec =  res.tv_nsec = 0;
#if defined linux || defined __APPLE__
    s.res_nsec = 10000000;
    if (pOpts->verbose) {
      perror("et_producer2: the resolution of the real-time clock could not be read.\n");
      fprintf(stderr, "Will assume it is %d ns.\n", s.res_nsec);
    }
#else
  if (clock_getres(CLOCK_REALTIME, &res)) {
    s.res_nsec = 10000000;
    if (pOpts->verbose) {
      perror("et_producer2: the resolution of the real-time clock could not be read.\n");
      fprintf(stderr, "Will assume it is %d ns.\n", s.res_nsec);
    }
  }
#endif
  assert(res.tv_sec == 0);
  s.res_nsec = res.tv_nsec;

  if (pOpts->verbose) {
    printf("et_producer2: Resolution of the real-time clock is %d nanoseconds.\n", s.res_nsec);
  }
  
  /* calibrate delay routine for setting event rate */
  calibrateDelay();
  
  /* fill buffer with CODA format data */
  if (pOpts->ev_size) {
    eventData = (int *) malloc(pOpts->ev_size);
    fill_buf((long *)eventData, pOpts->ev_size/sizeof(long));
    if (pOpts->verbose) {
      printf("et_producer2: fill buffer (%d bytes, %d longs) with CODA data\n",
		  pOpts->ev_size,  pOpts->ev_size/sizeof(long));
    }
  }
  
  /*************/
  /* ET conf  */
  /*************/
  if (pOpts->verbose) {
    printf("et_producer2: will try to attach\n");
  }

  if (et_open_config_init(&openconfig) != ET_OK) {
    printf("et_producer2: et_attach problems. Exiting.\n");
    exit(-1);
  }
  et_open_config_setwait(openconfig, ET_OPEN_WAIT);
  if (et_open(&s.id, pOpts->et_filename, openconfig) != ET_OK) {
    printf("et_producer2: et_attach problems. Exiting.\n");
    exit(-1);
  }
  et_open_config_destroy(openconfig);
  et_system_setdebug(s.id, ET_DEBUG_INFO);

  /*
   * Now that we have access to an ET system, find out have many
   * events it has and what size they are. Then allocate an array
   * of pointers to use for handling these events.
   */
  if (et_system_getnumevents(s.id, &total_et_events) != ET_OK) {
    printf("et_producer2: ET has died");
    exit(1);
  }

  if (pOpts->ev_in_chunk > total_et_events) {
    pOpts->ev_in_chunk = total_et_events;
    printf("et_producer2 Warning: Set event chunk size to %d.\n", pOpts->ev_in_chunk);
    printf(" Chunk size cannot be greater than the number of events in the system.\n");
  }

  if (et_system_geteventsize(s.id, &max_event_size) != ET_OK) {
    printf("et_producer2: ET has died");
    exit(-1);
  }
  if (pOpts->ev_size > max_event_size) {
    puts("et_producer2: Warning, the requested event size is larger");
    puts(" than the maximum ET event size.");
    puts(" The system will run slowly.");
  }
  
  if (et_station_attach(s.id, ET_GRANDCENTRAL, &s.att_id) < 0) {
    printf("et_producer2: error in station \n");
    exit(1);
  }

  if (pOpts->verbose) {
    printf("et_producer2: attached to gc, att = %d, pid = %d\n", 
           (int) s.att_id, (int)getpid());
  }

  /**********************************************************************/
  /*                    Conf signal and stats threads.                 */
  /**********************************************************************/

#ifdef sun
  thr_setconcurrency(thr_getconcurrency() + 2);
#endif

  /****************************************/
  /*           Catch control-C .          */
  /****************************************/

  sigfillset(&sigblock);
  /* block signals in this thread. */
  if ( pthread_sigmask(SIG_BLOCK, &sigblock, NULL) != 0) {
    printf("et_producer2: pthread_sigmask failure. Exiting.\n");
    exit(-1);
  }

  /* spawn signal handling thread */
  if (pthread_create(&tid, NULL, signal_thread, (void *)NULL) != 0) {
    printf("et_producer2: pthread_create failure. Exiting.\n");
    exit(-1);
  }
  
  /****************************************/
  /*             Statistics               */
  /****************************************/
  stats.report_period = 5;
  stats.ev_in_chunk = pOpts->ev_in_chunk;

#ifdef DEBUG
  printf("et_producer2: Address of stats = %X.\n", (unsigned int)&stats);
#endif
  if (pthread_create(&stat_tid, NULL, statistician, (void *)&stats) != 0) {
    printf("et_producer2: pthread_create failure. Exiting.\n");
    exit(-1);
  }
  return(s);
}
