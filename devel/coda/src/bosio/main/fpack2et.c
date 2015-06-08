
#include <stdio.h>
#include <stdlib.h>
#include "et.h"

#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>

#include <errno.h>
#include <sys/times.h>
#include <math.h>

#include "bosio.h"


#define NUMLOOPS 100
#define CHUNK 100
int control[4];


main(int argc,char **argv)
{
  int *jw;
  BOSIO *fd;
  int status, i, j, size, nevents_max, event_size, 
      freq, freq_tot=0, freq_avg, iterations=1, count;
  int con[ET_STATION_SELECT_INTS];
  et_statconfig   sconfig;
  et_openconfig   openconfig;
  et_event      **pe;
  et_att_id	      attach;
  et_sys_id       id;
  struct timespec timeout, t1, t2;
  double          time, avgsize, totsize;
  sigset_t        sigblock;
  pthread_t       tid;
  
  if (argc != 2) {
    printf("Usage: fpack2et <et_filename>\n");
    exit(1);
  }

  timeout.tv_sec  = 0;
  timeout.tv_nsec = 1;

  printf("fpack2et: will try to attach\n");
  et_open_config_init(&openconfig);
  if (et_open(&id, argv[1], openconfig) != ET_OK) {
    printf("fpack2et: et_attach problems\n");
    exit(1);
  }
  et_open_config_destroy(openconfig);
  
  et_system_setdebug(id, ET_DEBUG_INFO);
  /*
   * Now that we have access to an ET system, find out have many
   * events it has and what size they are. Then allocate an array
   * of pointers to use for reading, writing, and modifying these events.
   */
  
  if (et_system_getnumevents(id, &nevents_max) != ET_OK) {
    printf("et_client: ET has died");
    exit(1);
  }
  if (et_system_geteventsize(id, &event_size) != ET_OK) {
    printf("et_client: ET has died");
    exit(1);
  }

  printf("event_size=%d\n",event_size);
  if ( (pe = (et_event **) calloc(nevents_max, sizeof(et_event *))) == NULL) {
    printf("et_client: cannot allocate memory");
    exit(1);
  }

  if (et_station_attach(id, ET_GRANDCENTRAL, &attach) < 0) {
    printf("et_client: error in station attach\n");
    exit(1);
  }
  printf("fpack2et: attached to gc, att = %d, pid = %d\n", attach, getpid());


/* open FPACK file */

  if ( (status = bosOpen("./bigfile","r",&fd)) !=0)
  {
	printf("bosOpen status %d \n",status);
    fflush(stdout);
    goto error;
  }


  while (et_alive(id))
  {
#if defined(linux) || defined(Darwin)
    gettimeofday(&t1, NULL);
#else
    clock_gettime(CLOCK_REALTIME, &t1);
#endif
    avgsize = 0.;
    totsize = 0.;
    for (j=0; j < NUMLOOPS ; j++)
    {
      count = 0;
      for (i=0; i < CHUNK ; i++)
      {
        /*status = et_event_new(id, attach, &pe[i], ET_SLEEP, NULL, size);*/
        status = et_event_new(id, attach, &pe[i], ET_SLEEP, NULL, (event_size-100));
        if (status == ET_OK)
        {
          count++;
        }
        else if (status == ET_ERROR)
        {
          printf("fpack2et: et_event_new() returns ET_ERROR\n");
          break;
        }
        else if (status == ET_ERROR_TIMEOUT)
        {
          printf("fpack2et: got timeout\n");
          break;
        }
        else if (status == ET_ERROR_EMPTY)
        {
          printf("fpack2et: no events\n");
          break;
        }
        else if (status == ET_ERROR_BUSY)
        {
          printf("fpack2et: grandcentral is busy\n");
          break;
        }
        else if (status != ET_OK)
        {
          printf("fpack2et: request error\n");
          goto error;
        }

/************************************************/
/* write data, set priority, set control values */
/************************************************/

/* read one event from FPACK file */

        et_event_getdata(pe[i], (void **)&jw);
        if((status = etRead(fd,jw,event_size,&size,control) ) == EBIO_EOF)
        {
          /*goto endoffile;*/

          bosRewind(fd);
          status = etRead(fd,jw,event_size,&size,control);
        }

        avgsize += (double)size;
        totsize += (double)size;

        if ( status != 0)
        {
          printf("etRead error = %d\n",status);	fflush(stdout);
          goto error;
        }

		control[0] = 1; /* to make MON happy */
		
        et_event_setcontrol(pe[i], control, 4);
        et_event_setlength(pe[i], size);
		
        /*if(i==5) et_event_setpriority(pe[i], ET_HIGH);*/
		/*
status = et_event_put(id, attach, pe[i]);
		*/
      } /* for CHUNK */
    
      /* write data */
	  /*
goto a456;
*/

      status = et_events_put(id, attach, pe, count);
/*exit(0);*/

      if (status == ET_OK)
      {
        ;
      }
      else if (status == ET_ERROR_DEAD)
      {
        printf("fpack2et: et_events_put() returns ET_ERROR_DEAD\n");
        break;
      }
      else if (status != ET_OK)
      {
        printf("fpack2et: put error\n");
        goto error;
      }
a456:
;
    } /* for NUMLOOPS */


    /* statistics */
    avgsize = avgsize / ((double)(CHUNK*NUMLOOPS));
#if defined(linux) || defined(Darwin)
    gettimeofday(&t2, NULL);
#else
    clock_gettime(CLOCK_REALTIME, &t2);
#endif
    time = (double)(t2.tv_sec - t1.tv_sec) + 1.e-9*(t2.tv_nsec - t1.tv_nsec);
    freq = (CHUNK*NUMLOOPS)/time;
    if ((INT_MAX - freq_tot) < freq)
    {
      freq_tot   = 0;
	  iterations = 1;
    }
    freq_tot += freq;
    freq_avg = freq_tot/iterations;
    totsize = totsize/time/1024./1024.;
    iterations++;
    printf("%4d fpack2et: %7d Hz, %7d Hz Avg;  %7.1f bytes size, %6.1f MBytes/sec\n",
      (int)totsize,freq,freq_avg,avgsize,totsize);
    if (!et_alive(id))
    {
      et_wait_for_alive(id);
    }

  } /* while(alive) */
    
  
error:
  printf("fpack2et: ERROR\n");
  exit(0);


endoffile:

/* close FPACK file */
  bosClose(fd);
  exit(0);
}

