/*
  bos2et_test.c - test program for bos2et function
*/


#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/times.h>
#include <limits.h>
#include <math.h>
#include <signal.h>

#include "bosio.h"
#include "et.h"


#define NUMLOOPS 10
#define CHUNK 500
int control[4];

/* prototype */
#define NBCS 200000
static int iw[NBCS];

main(int argc,char **argv)
{
  int *jw;
  int fd, status, i, j, size, nevents_max, event_size, 
      freq, freq_tot=0, freq_avg, iterations=1, count;
  int con[ET_STATION_SELECT_INTS];
  et_statconfig   sconfig;
  et_openconfig   openconfig;
  et_att_id	  attach;
  et_sys_id       id;
  et_event       **pe;
  struct timespec timeout, t1, t2;
  double          time, avgsize;
  
  if (argc != 2) {
    printf("Usage: bos2et_test <et_filename>\n");
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
  printf("bos2et_test: attached to gc, att = %d, pid = %d\n", attach, getpid());


  /* open FPACK file */
  if ( (status = bosOpen("./bigfile","r",&fd)) !=0)
  {
	printf("bosOpen status %d \n",status);
    fflush(stdout);
    goto error;
  }

  /* init BOS array */
  bosInit(iw,NBCS);

  while (et_alive(id))
  {
#if defined(Linux) || defined(Darwin)
    gettimeofday(&t1, NULL);
#else
    clock_gettime(CLOCK_REALTIME, &t1);
#endif
    avgsize = 0.;
    for (j=0; j < NUMLOOPS ; j++)
    {
      count = 0;
      for (i=0; i < CHUNK ; i++)
      {
        status = et_event_new(id, attach, &pe[i], ET_SLEEP, NULL, event_size);

        if (status == ET_OK)
        {
          count++;
        }
        else if (status == ET_ERROR_DEAD)
        {
          printf("bos2et_test: request detach\n");
          break;
        }
        else if (status == ET_ERROR_TIMEOUT)
        {
          printf("bos2et_test: got timeout\n");
          break;
        }
        else if (status == ET_ERROR_EMPTY)
        {
          printf("bos2et_test: no events\n");
          break;
        }
        else if (status == ET_ERROR_BUSY)
        {
          printf("bos2et_test: grandcentral is busy\n");
          break;
        }
        else if (status != ET_OK)
        {
          printf("bos2et_test: request error\n");
          goto error;
        }


/************************************************/
/* write data, set priority, set control values */
/************************************************/



        /* drop banks from previous event */
        bosLdrop(iw, "E");
        bosNgarbage(iw);
        bosWgarbage(iw);

        /* read one event from FPACK file */
        if((status = bosRead(fd,iw,"E") ) == EBIO_EOF)
        {
          /*goto endoffile;*/

          bosRewind(fd);
          status = bosRead(fd,iw,"E");
        }

        /* fill ET buffer using all banks from BOS array */

        et_event_getdata(pe[i], (void **)&jw);

        status = bos2et(iw, "0", jw, event_size, &size, control);

        /* change ET record header if want - only those 5 elements !!! */
        /*
        jw[irNAME1] = *((unsigned long *)"RUNE");
        jw[irNAME2] = *((unsigned long *)"VENT");
        jw[irNRUN]  = 111111;
        jw[irNEVNT] = 1;
        jw[irTRIG]  = 222;
        */

        avgsize += (double)size;
        if ( status != 0)
        {
          printf("etRead error = %d\n",status);	fflush(stdout);
          goto error;
        }

        et_event_setcontrol(pe[i], control, 4);
        et_event_setlength(pe[i], size);

        /*if(i==5) et_event_setpriority(pe[i], ET_HIGH);*/

      } /* for CHUNK */
    
      /* write data */

      status = et_events_put(id, attach, pe, count);
      if (status == ET_OK)
      {
        ;
      }
      else if (status == ET_ERROR_DEAD)
      {
        printf("bos2et_test: et_events_put() returns ET_ERROR_DEAD\n");
        break;
      }
      else if (status != ET_OK)
      {
        printf("bos2et_test: put error\n");
        goto error;
      }
    } /* for NUMLOOPS */

    /* statistics */

    avgsize = avgsize / ((double)(CHUNK*NUMLOOPS));
#if defined(Linux) || defined(Darwin)
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
    iterations++;
    printf("bos2et_test: %7d Hz, %7d Hz Avg;  %7.1f bytes size\n",freq,freq_avg,avgsize);

    if (!et_alive(id))
    {
      et_wait_for_alive(id);
    }
  } /* while(alive) */
    
  
error:
  printf("bos2et_test: ERROR\n");
  exit(0);


endoffile:

/* close FPACK file */
  bosClose(fd);
  exit(0);
}

