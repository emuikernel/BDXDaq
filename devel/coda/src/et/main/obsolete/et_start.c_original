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
 *      An example program to show how to start up an Event Transfer system.
 *
 *----------------------------------------------------------------------------*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include "et.h"

int main(int argc, char **argv)
{  
  int           c;
  extern char  *optarg;
  extern int    optind;
  int           errflg = 0;
  int           i_tmp;
  
  int           status, sig_num;
  int           et_verbose = ET_DEBUG_NONE;
  int           deleteFile = 0;
  sigset_t      sigblockset, sigwaitset;
  et_statconfig sconfig;
  et_sysconfig  config;
  et_stat_id    statid;
  et_sys_id     id;

  /************************************/
  /* default configuration parameters */
  /************************************/
  int nevents = 2000;               /* total number of events */
  int event_size = 3000;            /* size of event in bytes */
  char *et_filename = NULL;
  char  et_name[ET_FILENAME_LENGTH];

  
  while ((c = getopt(argc, argv, "vdn:s:f:")) != EOF) {
    switch (c) {
    case 'n':
      i_tmp = atoi(optarg);
      if (i_tmp > 0) {
	nevents = i_tmp;
      } else {
	printf("Invalid argument to -n. Must be a positive integer.\n");
	exit(-1);
      }
      break;
      
    case 's':
      i_tmp = atoi(optarg);
      if (i_tmp > 0) {
	event_size = i_tmp;
      } else {
	printf("Invalid argument to -s. Must be a positive integer.\n");
	exit(-1);
      }
      break;
      
    case 'f':
      if (strlen(optarg) >= ET_FILENAME_LENGTH) {
        fprintf(stderr, "ET file name is too long\n");
        exit(-1);
      }
      strcpy(et_name, optarg);
      et_filename = et_name;
      break;

    /* Remove an existing memory-mapped file first, then recreate it. */
    case 'd':
      deleteFile = 1;
      break;

    case 'v':
      et_verbose = ET_DEBUG_INFO;
      break;
      
    case ':':
    case 'h':
    case '?':
    default:
      errflg++;
    }
  }
    
  if (optind < argc || errflg){
    fprintf(stderr, "usage: %s -v -r [-n events] [-s event_size] [-f file]\n", argv[0]);
    fprintf(stderr, "          -v for verbose output\n");
    fprintf(stderr, "          -d deletes an existing file first\n");
    fprintf(stderr, "          -n sets number of events\n");
    fprintf(stderr, "          -s sets event size in bytes\n");
    fprintf(stderr, "          -f sets memory-mapped file name\n");
    exit(2);
  }

  /* Check et_filename */
  if (et_filename == NULL) {
    /* see if env variable SESSION is defined */
    if ( (et_filename = getenv("SESSION")) == NULL ) {
      fprintf(stderr, "No ET file name given and SESSION env variable not defined\n");
      exit(-1);
    }
    /* check length of name */
    if ( (strlen(et_filename) + 12) >=  ET_FILENAME_LENGTH) {
      fprintf(stderr, "ET file name is too long\n");
      exit(-1);
    }
    sprintf(et_name, "%s%s", "/tmp/et_sys_", et_filename);
  }
  
  for ( ; optind < argc; optind++) {
    printf("%s\n", argv[optind]);
  }

  if (et_verbose) {
    printf("et_start: asking for %d byte events.\n", event_size);
    printf("et_start: asking for %d events.\n", nevents);
  }

  if (deleteFile) {
    remove(et_filename);
  }
  
  /********************************/
  /* set configuration parameters */
  /********************************/
  
  if (et_system_config_init(&config) == ET_ERROR) {
    printf("et_start: no more memory\n");
    exit(1);
  }
  /* total number of events */
  et_system_config_setevents(config, nevents);

  /* size of event in bytes */
  et_system_config_setsize(config, event_size);

  /* max number of temporary (specially allocated mem) events */
  /* This cannot exceed total # of events                     */
  et_system_config_settemps(config, nevents);

  /* limit on # of stations */
  et_system_config_setstations(config, 10);
  
  /* soft limit on # of attachments (hard limit = ET_ATTACHMENTS_MAX) */
  et_system_config_setattachments(config, 20);
  
  /* soft limit on # of processes (hard limit = ET_PROCESSES_MAX) */
  et_system_config_setprocs(config, 20);
    
  /* set TCP server port */
  /* et_system_config_setserverport(config, 11222); */
  
  /* add multicast address to listen to  */
  /* et_system_config_addmulticast(config, ET_MULTICAST_ADDR); */
  
  /* Make sure filename is null-terminated string */
  if (et_system_config_setfile(config, et_name) == ET_ERROR) {
    printf("et_start: bad filename argument\n");
    exit(1);
  }
  
  /*************************/
  /* setup signal handling */
  /*************************/
  sigfillset(&sigblockset);
  status = pthread_sigmask(SIG_BLOCK, &sigblockset, NULL);
  if (status != 0) {
    printf("et_start: pthread_sigmask failure\n");
    exit(1);
  }
  sigemptyset(&sigwaitset);
  sigaddset(&sigwaitset, SIGINT);
  
  /*************************/
  /*    start ET system    */
  /*************************/
  if (et_verbose) {
    printf("et_start: starting ET system %s\n", et_name);
  }
  if (et_system_start(&id, config) != ET_OK) {
    printf("et_start: error in starting ET system\n");
    exit(1);
  }
  
  /* in CODA usage, most want the TAPE station to be first
   * as that is the station used by the event recorder to
   * send events to tape.
   */
  /*
  et_station_config_init(&sconfig);
  et_station_config_setselect(sconfig,  ET_STATION_SELECT_ALL);
  et_station_config_setblock(sconfig,   ET_STATION_BLOCKING);
  et_station_config_setuser(sconfig,    ET_STATION_USER_MULTI);
  et_station_config_setrestore(sconfig, ET_STATION_RESTORE_OUT);
  et_station_config_setprescale(sconfig,1);

  if ((status = et_station_create(id, &statid, "TAPE", sconfig)) < 0) {
    if (status == ET_ERROR_EXISTS) {
      printf("et_start: \"TAPE\" station exists\n");
    }
    else {
      printf("et_start: cannot create \"TAPE\" station, error = %d\n", status);
      exit(1);
    }
  }
  et_station_config_destroy(sconfig);
  */
  et_system_setdebug(id, et_verbose);
 
  /* turn this thread into a signal handler */
  sigwait(&sigwaitset, &sig_num);
  

  printf("Interrupted by CONTROL-C\n");
  printf("ET is exiting\n");
  et_system_close(id);

  exit(0);  
}

