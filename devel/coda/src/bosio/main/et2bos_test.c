/*----------------------------------------------------------------------------*
 *  Copyright (c) 1998        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 * TJNAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606 *
 *      heyes@cebaf.gov   Tel: (757) 269-7030    Fax: (757) 269-5800          *
 *----------------------------------------------------------------------------*
 * Description:
 *      ET system sample event client
 *
 * Author:
 *	Carl Timmer
 *	TJNAF Data Acquisition Group
 *
 * Revision History:
 *
 *  Serguei Boiarinov : implement test for et2bos()
 *
 *----------------------------------------------------------------------------*/
 
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#if defined(Linux) || defined(Darwin)
#include <pthread.h>
#else
#include <thread.h>
#endif
#include <et.h>

#include "bosio.h"

#define NUMEVENTS 20000

/* prototype */
static void * signal_thread (void *arg);
static int fd;

#define NBCS 200000
static int jw[NBCS];

main(int argc,char **argv)
{  
  int *ev, len, i, j, status, numread, totalread=0, nevents_max, event_size, chunk;
  int con[ET_STATION_SELECT_INTS];
  et_event      **pe;
  et_openconfig   openconfig;
  et_statconfig   sconfig;
  et_att_id       attach;
  et_stat_id      my_stat;
  et_sys_id       id;
  sigset_t        sigblock;
  struct timespec timeout;
  pthread_t       tid;

  BOSIOptr BIOstream;
  char *ch;

  char *str1 =
"OPEN UNIT=11 FILE='/scratch/boiarino/test.A00' WRITE RECL=32768 SPLITMB=2047 RAW SEQ NEW BINARY";
/*"OPEN UNIT=11 FILE='/work/clas/disk1/boiarino/test.A00' WRITE RECL=32768 SPLITMB=2047 RAW SEQ NEW BINARY";*/
  
  if(argc != 3) {
    printf("Usage: et2bos_test <et_filename> <station_name>\n");
    exit(1);
  }

  timeout.tv_sec  = 0;
  timeout.tv_nsec = 1;
  
  /* setup signal handling */
  sigfillset(&sigblock);
  /* block all signals */
  status = pthread_sigmask(SIG_BLOCK, &sigblock, NULL);
  if (status != 0) {
    printf("et2bos: pthread_sigmask failure\n");
    exit(1);
  }
  #ifdef sun
    thr_setconcurrency(thr_getconcurrency() + 1);
  #endif
  /* spawn signal handling thread */
  pthread_create(&tid, NULL, signal_thread, (void *)NULL);
  

restartLinux:

  /* open ET system */
  et_open_config_init(&openconfig);
  et_open_config_setwait(openconfig, ET_OPEN_WAIT);
  if (et_open(&id, argv[1], openconfig) != ET_OK) {
    printf("et2bos: et_open problems\n");
    exit(1);
  }
  et_open_config_destroy(openconfig);


  /*
   * Now that we have access to an ET system, find out how many
   * events it has and what size they are. Then allocate an array
   * of pointers to use for reading, writing, and modifying these events.
   */
  
  if (et_system_getnumevents(id, &nevents_max) != ET_OK)
  {
    printf("et2bos: ET has died\n");
    exit(1);
  }
  if (et_system_geteventsize(id, &event_size) != ET_OK)
  {
    printf("et2bos: ET has died\n");
    exit(1);
  }
  
  if ( (pe = (et_event **) calloc(nevents_max, sizeof(et_event *))) == NULL)
  {
    printf("et2bos: cannot allocate memory\n");
    exit(1);
  }

  et_station_config_init(&sconfig);
  et_station_config_setuser(sconfig, ET_STATION_USER_MULTI);
  et_station_config_setrestore(sconfig, ET_STATION_RESTORE_OUT);
  et_station_config_setprescale(sconfig, 1);
 
  /* old "all" mode */
  et_station_config_setselect(sconfig, ET_STATION_SELECT_ALL);
  et_station_config_setblock(sconfig, ET_STATION_BLOCKING);
  et_station_config_setcue(sconfig, 150);  /* if ET_STATION_NONBLOCKING */

  /* set debug level */
  et_system_setdebug(id, ET_DEBUG_INFO);


  /* create station */
  if ((status = et_station_create(id, &my_stat, argv[2], sconfig)) < 0)
  {
    if (status == ET_ERROR_EXISTS)
    {
      /* my_stat contains pointer to existing station */;
      printf("et2bos: set ptr to the existing station\n");
    }
    else if (status == ET_ERROR_TOOMANY)
    {
      printf("et2bos: too many stations created\n");
      goto error;
    }
    else
    {
      printf("et2bos: error in station creation\n");
      goto error;
    }
  }

  printf("et2bos: station ready\n");




  if (et_station_attach(id, my_stat, &attach) < 0)
  {
    printf("et2bos: error in station attach\n");
    goto error;
  }

  /* open FPACK file for writing */
  /*if( (status = FParm(str1,&fd)) !=0)
  {
	printf("FParm status %d \n",status);
	printf("command was >%s<\n",str1);
	exit(1);
  }*/

  /* set file descriptor pointer */
  BIOstream = (BOSIOptr)fd;

  /* initialize BOS array */
  bosInit(jw,NBCS);


  while (et_alive(id))
  {
    /**************/
    /* read data  */
    /**************/

    /* example of single, timeout read */
    /* status = et_event_get(&id, attach, &pe[0], ET_TIMED, &timeout); */

    /* example of single, asynchronous read */
    /* status = et_event_get(&id, attach, &pe[0], ET_ASYNC, NULL);*/

    /* example of reading array of up to "chunk" events */
    /* chunk = 500; */
    /* numread = status = et_events_get(&id, attach, pe, ET_SLEEP, NULL, chunk, &numread);*/

    chunk   = 500;
    status = et_events_get(id, attach, pe, ET_SLEEP, NULL, chunk, &numread);
    if (status == ET_OK) {
      ;
    }
    else if (status == ET_ERROR_DEAD) {
      printf("et2bos: ET system is dead\n");
      goto end;
    }
    else if (status == ET_ERROR_TIMEOUT) {
      printf("et2bos: got timeout\n");
      goto end;
    }
    else if (status == ET_ERROR_EMPTY) {
      printf("et2bos: no events\n");
      goto end;
    }
    else if (status == ET_ERROR_BUSY) {
      printf("et2bos: station is busy\n");
      goto end;
    }
    else if (status == ET_ERROR_WAKEUP) {
      printf("et2bos: someone told me to wake up\n");
      goto end;
    }
    else if ((status == ET_ERROR_WRITE) || (status == ET_ERROR_READ)) {
      printf("et2bos: socket communication error\n");
      goto end;
    }
    else if (status != ET_OK) {
      printf("et2bos: get error\n");
      goto error;
    }

    /****************************************/
    /* print data, write data to FPACK file */
    /****************************************/

    for (j=0; j<numread; j++)
    {
      et_event_getdata(pe[j], (void **) &ev);

/*
      et_event_getlength(pe[j], &len);
      printf("et2bos: recname=>%4.4s%4.4s<, run#=%d, event#=%d, reclen=%d\n",
           (char *)&ev[3],(char *)&ev[4],ev[5],ev[6],ev[10]);
      printf(" temp = %d, pri = %d, len = %d\n",
           pe[j]->temp,et_event_getpriority(pe[j]),len);
      et_event_getcontrol(pe[j], con);
      for (i=0; i < ET_STATION_SELECT_INTS; i++)
      {
        printf("control[%d] = %d\n",i,con[i]);
      }
*/

      /* drop banks from previous event */

      bosLdrop(jw, "E");
      bosNgarbage(jw);
      bosWgarbage(jw);

      /* create banks in BOS array */

      status = et2bos(ev, jw, "E");
      if (status != 0)
      {
        printf ("et2bos_test: error %d in et2bos()\n",status);
        exit(1);
      }

      /* call next if want to use ET record header; otherwise
      record name will be "RUNEVENT" and other info from HEAD bank */

      /*bosWriteRecord(fd,&ev[3],ev[5],ev[6],ev[8]);*/

      /* write down BOS banks to file */
/*
      status = bosWrite(fd, jw, "E");
      if (status != 0)
      {
        printf ("et2bos_test: error %d in bosWrite()\n",status);
        exit(1);
      }
*/

    }

    /*********************/
    /* return data to ET */
    /*********************/

    /* example of writing single event */
    /* status = et_event_put(id, attach, pe[0]);*/

    /* example of writing array of events */
    status = et_events_put(id, attach, pe, numread);

    if (status == ET_ERROR_DEAD) {
      printf("et2bos: ET is dead\n");
      goto end;
    }
    else if ((status == ET_ERROR_WRITE) || (status == ET_ERROR_READ)) {
      printf("et2bos: socket communication error\n");
      goto end;
    }
    else if (status != ET_OK) {
      printf("et2bos: put error\n");
      goto error;
    }
    totalread += numread;

end:
    /* print something out after having read NUMEVENTS events */
    if (totalread >= NUMEVENTS)
    {
      totalread = 0;
      printf(" et2bos: %d events\n", NUMEVENTS);
    }

    /* if ET system is dead, wait here until it comes back */
	while (!et_alive(id)) {
	  status = et_wait_for_alive(id);
	  if (status == ET_OK) {
	    int locality;
	    et_system_getlocality(id, &locality);
	    /* if Linux, re-establish connection to ET system since socket broken */
	    if (locality == ET_LOCAL_NOSHARE) {
              printf("et2bos: try to reconnect Linux client\n");
	      et_forcedclose(id);
	      goto restartLinux;
	    }
	  }
	}



  } /* while(alive) */
    
error:
  free(pe);
  printf("et2bos: ERROR\n");
  exit(0);
}

/************************************************************/
/*              separate thread to handle signals           */
static void * signal_thread (void *arg)
{
  int status;
  sigset_t   signal_set;
  int        sig_number;
  char *str1 = "CLOSE UNIT=11";

  sigemptyset(&signal_set);
  sigaddset(&signal_set, SIGINT);
  
  /* Not necessary to clean up as ET system will do it */
  sigwait(&signal_set, &sig_number);
  printf("et2bos: got a control-C, exiting\n");

  /* flush output buffer and close FPACK file */
  if( (status = etFlush(fd)) != 0)
  {
    printf("et2bos: ERROR in etFlush, status = %d\n",status);
    exit(0);
  }
  if( (status = FParm(str1,&fd)) !=0)
  {
	printf("et2bos: FParm status %d \n",status);
	printf("et2bos: command was >%s<\n",str1);
	exit(1);
  }

  exit(1);
}
