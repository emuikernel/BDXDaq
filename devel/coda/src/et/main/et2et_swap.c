
/* et2et_swap.c - run on little-endian machine, converts data to big-endian
   format and send them to big-endian machine

clondaq1:

et_start -f /tmp/et_sys_clastest -n 500 -s 100000
fpack2et /tmp/et_sys_clastest
et2et_swap /tmp/et_sys_clastest /tmp/et_sys_sertest ET2ET

clon10:

et_start -f /tmp/et_sys_sertest -n 500 -s 100000
et2bos_test /tmp/et_sys_sertest TEST

*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#ifdef sun
#include <thread.h>
#endif
#include "et.h"

#define NUMEVENTS 600
#define CHUNK 100

/* prototype */
static void * signal_thread (void *arg);

int main(int argc,char **argv) {  
  int             i, j, status, numread, chunk, totalread=0, loops=0;
  int		  con[ET_STATION_SELECT_INTS], ntransferred=0;
  pthread_t       tid;
  et_statconfig   sconfig;
  et_openconfig   openconfig;
  et_bridgeconfig bconfig;
  sigset_t        sigblock;
  struct timespec timeout;
  et_att_id       att_from, att_to;
  et_stat_id      stat_from, stat_to;
  et_sys_id       id_from, id_to;
  int             selections[] = {17,15,-1,-1}; /* 17,15 are arbitrary */

  char ch[100];

 
  if (argc != 4) {
    printf("Usage: %s <from etname> <to etname> <station_name>\n",
            argv[0]);
    exit(1);
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
  
  /* init ET configuration will be used for both ET systems */
  et_open_config_init(&openconfig);

  /* open local ET */
  /* clon10: 129.57.167.14 */
  /* clon10-daq1: 129.57.68.21 */
  /* clon10-daq2: 129.57.68.22 */
  /*et_open_config_sethost(openconfig, ET_HOST_LOCAL);*/
  /*et_open_config_sethost(openconfig, "clon10-daq1");*/
  et_open_config_sethost(openconfig, "clondaq1");
  et_open_config_gethost(openconfig, ch);
  printf("local host >%s<\n",ch);
  if (et_open(&id_from, argv[1], openconfig) != ET_OK) {
    printf("%s: et_open 'from' problems\n", argv[1]);
    exit(1);
  }

  /* open remote ET */
  /* clon00: 129.57.167.5 */
  /* clon00-daq1: 129.57.68.1 */
  /*et_open_config_sethost(openconfig, "clon00-daq1");*/
  /*et_open_config_sethost(openconfig, "clon00-daq1");*/

  et_open_config_sethost(openconfig, "clon10");


  et_open_config_setcast(openconfig, ET_DIRECT);
  et_open_config_setserverport(openconfig, 11111);








  /*et_open_config_sethost(openconfig, "clon10");*/
  /*et_open_config_sethost(openconfig, "129.57.167.14");*/
  et_open_config_gethost(openconfig, ch);
  printf("remote host >%s<\n",ch);
  if (et_open(&id_to, argv[2], openconfig) != ET_OK) {
    printf("%s: et_open 'to' problems\n", argv[2]);
    exit(1);
  }

  /* destroy configuration */
  et_open_config_destroy(openconfig);

  /* init station configuration */
  et_station_config_init(&sconfig);
  et_station_config_setuser(sconfig, ET_STATION_USER_MULTI);
  et_station_config_setrestore(sconfig, ET_STATION_RESTORE_OUT);
  et_station_config_setprescale(sconfig, 1);
  et_station_config_setcue(sconfig, 150);
 

  /* DD system "all" mode */
  et_station_config_setselect(sconfig, ET_STATION_SELECT_ALL);
  et_station_config_setblock(sconfig, ET_STATION_BLOCKING);

  /* DD system "on req" mode 
  et_station_config_setselect(sconfig, ET_STATION_SELECT_ALL);
  et_station_config_setblock(sconfig, ET_STATION_NONBLOCKING);
  */
  /* DD system "condition" mode 
  et_station_config_setselect(sconfig, ET_STATION_SELECT_MATCH);
  et_station_config_setblock(sconfig, ET_STATION_BLOCKING);
  et_station_config_setselectwords(sconfig, selections);
  */
  /* new non-blocking "condition" mode 
  et_station_config_setselect(sconfig, ET_STATION_SELECT_MATCH);
  et_station_config_setblock(sconfig, ET_STATION_NONBLOCKING);
  et_station_config_setselectwords(sconfig, selections);
  */
  /* user's condition, blocking  mode 
  et_station_config_setselect(sconfig, ET_STATION_SELECT_USER);
  et_station_config_setblock(sconfig, ET_STATION_BLOCKING);
  et_station_config_setselectwords(sconfig, selections);
  if (et_station_config_setfunction(sconfig, "et_carls_function") == ET_ERROR) {
	printf("%s: cannot set function\n", argv[0]);
	exit(1);
  }
  if (et_station_config_setlib(sconfig, "/home/timmer/cvs/coda/source/et/src/libet_user.so") == ET_ERROR) {
    printf("%s: cannot set library\n", argv[0]);
	exit(1);
  }
  */
  /* user's condition, nonblocking mode 
  et_station_config_setselect(sconfig, ET_STATION_SELECT_USER);
  et_station_config_setblock(sconfig, ET_STATION_NONBLOCKING);
  et_station_config_setselectwords(sconfig, selections);
  et_station_config_setfunction(sconfig, "et_carls_function");
  et_station_config_setlib(sconfig, "/home/timmer/cvs/coda/source/et/src/libet_user.so");
  */
  
  /* set debug level */
  et_system_setdebug(id_from, ET_DEBUG_INFO);
  et_system_setdebug(id_to,   ET_DEBUG_INFO);

  if ((status = et_station_create(id_from, &stat_from, argv[3], sconfig)) < ET_OK) {
    if (status == ET_ERROR_EXISTS) {
      /* my_stat contains pointer to existing station */;
      printf("%s: station already exists\n", argv[0]);
    }
    else {
      printf("%s: error in station creation\n", argv[0]);
      goto error;
    }
  }
  et_station_config_destroy(sconfig);

  if (et_station_attach(id_from, stat_from, &att_from) < 0) {
    printf("%s: error in station attach\n", argv[0]);
    goto error;
  }

  if (et_station_attach(id_to, ET_GRANDCENTRAL, &att_to) < 0) {
    printf("%s: error in station attach\n", argv[0]);
    goto error;
  }

  et_bridge_config_init(&bconfig);
  /*et_bridge_config_setfunc(bconfig, et_bridge_CODAswap);*/
  while (/*(loops++ < 1000000) &&*/ (status == ET_OK)) {
    status = et_events_bridge(id_from, id_to, att_from, att_to,
			      bconfig, NUMEVENTS, &ntransferred);
	/*
    printf("status after et_bridge = %d, ntransferred = %d\n", status, ntransferred);
	*/
  }
  et_bridge_config_destroy(bconfig);

  et_forcedclose(id_from);
  et_forcedclose(id_to);
  return 0;
error:
  return -1;
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
  printf("et_2_et: got a control-C, exiting\n");
  exit(1);
}


/************************************************************/
/************************************************************/
/************************************************************/


