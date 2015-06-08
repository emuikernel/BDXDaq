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
 *      ET system sample event producer
 *
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "et.h"
/* recent versions of linux put float.h (and DBL_MAX) in a strange place */
#define DOUBLE_MAX   1.7976931348623157E+308

#define NUMLOOPS 2000
#define CHUNK 100
#define DBL_MAX   1.e+100

/* prototype */
static void * signal_thread (void *arg);

int main(int argc,char **argv)
{  
  int             i, j, size, status;
  double          freq=0.0, freq_tot=0.0, freq_avg=0.0;
  int             iterations=1, count;
  et_att_id	  attach1;
  et_sys_id       id;
  et_openconfig   openconfig;
  et_event       *pe[CHUNK];
  struct timespec timeout;
#if defined linux || defined __APPLE__
  struct timeval  t1, t2;
#else
  struct timespec t1, t2;
#endif
  double          time;
  sigset_t        sigblock;
  pthread_t       tid;
  
  /* handy data for testing */
  int   numbers[] = {0,1,2,3,4,5,6,7,8,9};
  char   *stuff[] = {"One","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten"};
  int   control[] = {17,8,-1,-1}; /* 17,8 are arbitrary */
  
  if ((argc != 2) && (argc != 3)) {
    printf("Usage: %s <et_filename> [<eventsize>]\n", argv[0]);
    exit(1);
  }
  size = 10;
  if (argc == 3) {
    size = atoi(argv[2]);
  }
  
  timeout.tv_sec  = 0;
  timeout.tv_nsec = 1;

  /*************************/
  /* setup signal handling */
  /*************************/
  /* block all signals */
  sigfillset(&sigblock);
  status = pthread_sigmask(SIG_BLOCK, &sigblock, NULL);
  if (status != 0) {
    printf("%s: pthread_sigmask failure\n", argv[0]);
    exit(1);
  }

#ifdef sun
  /* prepare to run signal handling thread concurrently */
  thr_setconcurrency(thr_getconcurrency() + 1);
#endif

  /* spawn signal handling thread */
  pthread_create(&tid, NULL, signal_thread, (void *)NULL);
  
restartLinux:
  /* open ET system */
  et_open_config_init(&openconfig);
  if (et_open(&id, argv[1], openconfig) != ET_OK) {
    printf("%s: et_open problems\n", argv[0]);
    exit(1);
  }
  et_open_config_destroy(openconfig);
 
  /* set level of debug output (everything) */
  et_system_setdebug(id, ET_DEBUG_INFO);
  
  /* attach to grandcentral station */
  if (et_station_attach(id, ET_GRANDCENTRAL, &attach1) < 0) {
    printf("%s: error in et_station_attach\n", argv[0]);
    exit(1);
  }

  while (et_alive(id)) {
      /* read time for future statistics calculations */
#if defined linux || defined __APPLE__
      gettimeofday(&t1, NULL);
#else
      clock_gettime(CLOCK_REALTIME, &t1);
#endif
      /* loop NUMLOOPS times before printing out statistics */
      for (j=0; j < NUMLOOPS ; j++) {
        status = et_events_new(id, attach1, pe, ET_SLEEP, NULL, size, CHUNK, &count);
        if (status == ET_OK) {
          ;
        }
        else if (status == ET_ERROR_DEAD) {
          printf("%s: ET system is dead\n", argv[0]);
          break;
        }
        else if (status == ET_ERROR_TIMEOUT) {
          printf("%s: got timeout\n", argv[0]);
          break;
        }
        else if (status == ET_ERROR_EMPTY) {
          printf("%s: no events\n", argv[0]);
          break;
        }
        else if (status == ET_ERROR_BUSY) {
          printf("%s: grandcentral is busy\n", argv[0]);
          break;
        }
        else if (status == ET_ERROR_WAKEUP) {
          printf("%s: someone told me to wake up\n", argv[0]);
          break;
        }
        else if ((status == ET_ERROR_WRITE) || (status == ET_ERROR_READ)) {
          printf("%s: socket communication error\n", argv[0]);
          break;
        }
        else if (status != ET_OK) {
          printf("%s: request error\n", argv[0]);
          goto error;
        }

        /* write data, set priority, set control values here */
        if (0) {
	  char *pdata;
    	  for (i=0; i < count; i++) {
            /* the following line will allow et_client modes 3 & 4 to work */
            /* et_event_setcontrol(pe[i], control, 4); */
	    et_event_getdata(pe[i], (void **) &pdata);
            /*
	    strcpy(pdata, stuff[i]);
            et_event_setlength(pe[i], strlen(stuff[i])+1);
	    */
	    
	    memcpy((void *)pdata, (const void *) &numbers[i], sizeof(int));
            et_event_setlength(pe[i], sizeof(int));
	    
	  }
        }
	  
        /* put events back into the ET system */
        status = et_events_put(id, attach1, pe, count);
        if (status == ET_OK) {
          ;
        }
        else if (status == ET_ERROR_DEAD) {
          printf("%s: ET is dead\n", argv[0]);
          break;
        }
        else if ((status == ET_ERROR_WRITE) || (status == ET_ERROR_READ)) {
          printf("%s: socket communication error\n", argv[0]);
          break;
        }
        else if (status != ET_OK) {
          printf("%s: put error, status = %d\n", argv[0], status);
          goto error;
        }
      } /* for NUMLOOPS */
  
      /* statistics */
#if defined linux || defined __APPLE__
      gettimeofday(&t2, NULL);
      time = (double)(t2.tv_sec - t1.tv_sec) + 1.e-6*(t2.tv_usec - t1.tv_usec);
#else
      clock_gettime(CLOCK_REALTIME, &t2);
      time = (double)(t2.tv_sec - t1.tv_sec) + 1.e-9*(t2.tv_nsec - t1.tv_nsec);
#endif
      freq = (count*NUMLOOPS)/time;
      if ((DOUBLE_MAX - freq_tot) < freq) {
        freq_tot   = 0.0;
	iterations = 1;
      }
      freq_tot += freq;
      freq_avg = freq_tot/(double)iterations;
      iterations++;
      printf("%s: %9.1f Hz,  %10.2f Hz Avg.\n", argv[0], freq, freq_avg);

      /* if ET system is dead, wait here until it comes back */
      if (!et_alive(id)) {
        status = et_wait_for_alive(id);
	if (status == ET_OK) {
	  int locality;
	  et_system_getlocality(id, &locality);
	  /* if Linux, re-establish connection to ET system since socket broken */
	  if (locality == ET_LOCAL_NOSHARE) {
            printf("%s: try to reconnect Linux client\n", argv[0]);
	    et_forcedclose(id);
	    goto restartLinux;
	  }
	}
      }
      
  } /* while(alive) */
    
  
  error:
    printf("%s: ERROR\n", argv[0]);
    exit(0);
}

/************************************************************/
/*              separate thread to handle signals           */
static void * signal_thread (void *arg)
{
  sigset_t   signal_set;
  int        sig_number;
 
  sigemptyset(&signal_set);
  sigaddset(&signal_set, SIGINT);
  
  /* Not necessary to clean up as ET system will do it */
  sigwait(&signal_set, &sig_number);
  printf("Got a control-C, exiting\n");
  exit(1);
}
