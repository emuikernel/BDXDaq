
/* roc_process.c - ROC processing service routines; can be run at the same
   CPU as coda_roc or on secondary CPU */


#include "da.h"

#ifdef VXWORKS

#include <stdio.h>
#include <sockLib.h>
#include <errno.h>
#include <errnoLib.h>

extern long     vxTicks;

#define MYCLOCK 25

#else

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <dlfcn.h>
#include <sys/mman.h>

#define MYCLOCK NANOMICRO

#ifdef Linux
#include <linux/prctl.h>
#endif

#endif

#include "circbuf.h"
#include "bigbuf.h"


/*
#define DEBUGPMC
*/


/***************************************************************************/
/******************************** user code ********************************/


#include "coda.h"
#include "tt.h"


#include "pmc.h"

#ifndef VXWORKS
#ifdef __cplusplus
typedef void 		(*VOIDFUNCPTR) (...); /* ptr to function returning void */
#else
typedef void 		(*VOIDFUNCPTR) (); /* ptr to function returning void */
#endif			/* _cplusplus */
#endif
#include "rolInt.h"
static ROLPARAMS rolP2;


extern TTSPtr ttp; /* pointer to TTS structures for current roc */

/* copy 'bufin' to 'bufout' */
/* input: bufout[0] - max length of output event */

int
proc_copy(int *bufin, int *bufout, int pid)
{
  int lenin;

  lenin = bufin[0] + 1; /* bufin[0] does not include itself */
  memcpy(bufout, bufin, (lenin<<2));
  /*
  printf("proc_copy: copy event from 0x%08x to 0x%08x, lenw=%d\n",
    bufin,bufout,lenin);
  */

  return(lenin);
}


int
proc_tt(int *bufin, int *bufout, int pid)
{
  ROLPARAMS *rolP = &rolP2;
  int len, ii;

  /* set input/output pointers and cleanup first
  word in output (will be CODA fragment length) */
  rolP->dabufpi = (long *) bufin;
  rolP->dabufp = (long *) bufout;
  bufout[0] = 0;

#ifdef DEBUGPMC
  printf("proc_tt reached\n");fflush(stdout);
#endif

  /*call ROL2 trigger routine */
  /*DA_POLL_PROC==__poll()==CDOPOLL==cdopolldispatch()*/
  rolP->daproc = DA_POLL_PROC;
  (*rolP->rol_code) (rolP);

  /* let see now what we've got from ROL2 */
  len = bufout[0];
  if(len > NWBOS)
  {
    printf("proc_tt ERROR: on output event length=%d (longwords) is too big\n",len);
    return(0);
  }
  else if(len < 1)
  {
    printf("proc_tt: ERROR: on output len = %d\n",len);
    return(0);
  }

#ifdef DEBUGPMC
  printf("proc_tt: return len+1=%d\n",len+1);fflush(stdout);
#endif

  return(len+1);
}


/**********************/
/* STANDARD FUNCTIONS */

int
proc_download(char *rolname, char *rolparams, int pid)
{
  ROLPARAMS *rolP = &rolP2;
  int res;

  /*******************************************/
  /* downloading rol2 we suppose to run here */

  res = codaLoadROL(rolP,rolname,rolparams);
  /*
          "/usr/clas/devel/coda/src/rol/rol/VXWORKS_ppc/obj/rol2_tt.o",
          "usr");
  */
  if(res)
  {
    return(-1);
  }

  rolP->daproc = DA_INIT_PROC;
  rolP->pid = pid;

  /* execute ROL init procedure (described in 'rol.h') */
  (*(rolP->rol_code)) (rolP);

  /* check if initialization was successful */
  if(rolP->inited != 1)
  {
    printf ("ERROR: ROL initialization failed\n");
    return(-2);
  }

  rolP->daproc = DA_DOWNLOAD_PROC;
  (*rolP->rol_code) (rolP);

  return(0);
}

int
proc_prestart(int pid)
{
  ROLPARAMS *rolP = &rolP2;
  /*
  rolP->runNumber = object->runNumber;
  rolP->runType = object->runType;
  rolP->recNb = 0;
  */
  rolP->daproc = DA_PRESTART_PROC;
  (*rolP->rol_code) (rolP);

  return(0);
}

int
proc_go(int pid)
{
  ROLPARAMS *rolP = &rolP2;

  printf("proc_go reached\n");fflush(stdout);

  rolP->daproc = DA_GO_PROC;
  (*rolP->rol_code) (rolP);

  printf("proc_go done\n");fflush(stdout);
  return(0);
}

int
proc_end()
{
  ROLPARAMS *rolP = &rolP2;
  return(0);
}

int
proc_poll(int *bufin, int *bufout, int pid)
{
  int len;

  if(pid == -1)
  {
    len = proc_copy(bufin, bufout, pid);
  }
  else
  {
    len = proc_tt(bufin, bufout, pid);
  }

  return(len);
}



/***************************************************************************/
/******************************* proc_thread *******************************/


#ifdef VXWORKS

#define PROC_TIMER_START timeout = vxTicks + token_interval

#else

#define PROC_TIMER_START timeout = time(0) + token_interval/60

#endif


/* special events must be in separate buffer, buffer number must be -1 */
/* from BIGNET structure it uses following:
  BIGBUF *gbigBuffer - input data buffer
  BIGBUF *gbigBuffer1 - output data buffer
  token_interval - for time profiling
  failure - set it to 0 just in case
  doclose - just for printing
 */
#ifdef VXWORKS
void 
proc_thread(BIGNET *bigprocptrin, unsigned int offsetin)
{
#else
void 
proc_thread(BIGNET *bigprocptrin)
{
  unsigned int offsetin = 0;
#endif
  ROLPARAMS *rolP = &rolP2;
  static int length, status, ifend;
  int i, jj, llen, llenw, evsz, res, nevent, iev, lenin, lenout, pid, nbuffer;
  unsigned int maxoutbuflen;
  unsigned int *bigbufin, *bigbufout, *bufin, *bufout;
  unsigned int timeout;
  static BIGNET *bigprocptr;
  static unsigned int offset;
  static int token_interval;

/* timing */
#ifdef VXWORKS
  unsigned long start, end, time1, time2, icycle, cycle = 20;
#else
  hrtime_t start, end, time1, time2, icycle, cycle = 20;
  static int nev;
  static hrtime_t sum;
#endif

#ifdef Linux
  prctl(PR_SET_NAME,"coda_proc");
#endif

  /*printf("input: bigprocptrin=0x%08x offsetin=0x%08x\n",
    bigprocptrin,offsetin);*/
  bigprocptr = bigprocptrin;
  offset = offsetin;
  token_interval = bigprocptr->token_interval;

#ifdef VXWORKS
  printf("bigprocptr=0x%08x offset=0x%08x\n",bigprocptr,offset);
  taskDelay(100);
  printf("bigprocptr=0x%08x offset=0x%08x\n",bigprocptr,offset);
  
  printf("bigproc at 0x%08x, bigproc.gbigBuffer at 0x%08x -> 0x%08x\n",
          bigprocptr, &(bigprocptr->gbigBuffer),
          (&(bigprocptr->gbigBuffer))+offset);
#endif





  printf("proc_thread reached\n");fflush(stdout);
  printf("proc_thread reached\n");fflush(stdout);
  printf("proc_thread reached\n");fflush(stdout);
  printf("proc_thread reached\n");fflush(stdout);
  printf("proc_thread reached\n");fflush(stdout);
  printf("proc_thread reached\n");fflush(stdout);
#ifndef VXWORKS
  sleep(1);
#endif
  printf("proc_thread reached\n");fflush(stdout);
  printf("proc_thread reached\n");fflush(stdout);
  printf("proc_thread reached\n");fflush(stdout);
  printf("proc_thread reached\n");fflush(stdout);
  printf("proc_thread reached\n");fflush(stdout);
  printf("proc_thread reached\n");fflush(stdout);

  nevent = icycle = time1 = time2 = 0;
  timeout = 0;
  bigbufout = NULL;
  nbuffer = 1;

  /* get output buffer maximum length in bytes */
  maxoutbuflen = bigprocptr->gbigBuffer1->nbytes;
  /*printf("proc_thread: maxoutbuflen=%u bytes\n",maxoutbuflen);*/

  do
  {
    icycle ++;
#ifdef VXWORKS
    start = sysTimeBaseLGet();
#else
    start = gethrtime();
#endif
#ifdef DEBUGPMC
    printf("proc: next itteration, icycle=%d\n",icycle);fflush(stdout);
#endif

    /* wait for input buffer */
#ifdef PMCOFFSET
    if(offset == 0) /* we are on host board - get buffer from local memory */
    {
      bigbufin = bb_read(&(bigprocptr->gbigBuffer));
    }
    else            /* we are on pmc board - get buffer over pci bus */
    {
      bigbufin = bb_read_pci(&(bigprocptr->gbigBuffer));
    }
#else
    bigbufin = bb_read(&(bigprocptr->gbigBuffer));
#endif
    if(bigbufin == NULL)
    {
      printf("proc_thread: ERROR: bigbufin==NULL\n");fflush(stdout);
      break;
    }

#ifdef VXWORKS
    end = sysTimeBaseLGet();
    time1 += (end-start)/MYCLOCK;
#else
    end = gethrtime();
    time1 += (end-start)/MYCLOCK;
#endif


#ifdef VXWORKS
    start = sysTimeBaseLGet();
#else
    start = gethrtime();
#endif

    bigprocptr->failure = 0;


    /* remember some values (do not need all of them ..) */
    llenw = bigbufin[BBIWORDS];
    llen = bigbufin[BBIWORDS] << 2;
	/*printf("proc_thread: llen=%d\n",llen);*/
    nevent += bigbufin[BBIEVENTS];
    ifend = bigbufin[BBIEND];
    pid = bigbufin[BBIROCID];

    bufin = bigbufin + BBHEAD;

#ifdef DEBUGPMC
    printf("proc: got input buffer 0x%08x: num=%d nev=%d lenw=%d ifend=%d pid=%d\n",
      bigbufin,bigbufin[BBIBUFNUM],bigbufin[BBIEVENTS],llenw,ifend,pid);
#endif

    /* loop over input buffer */
    for(iev=0; iev<bigbufin[BBIEVENTS]; iev++)
    {

      /* if input buffer contains special event, release output buffer if it exist */
      if((bigbufin[BBIBUFNUM] == -1) && (bigbufout != NULL))
      {
#ifdef DEBUGPMC
    printf("proc: releasing output buffer - 0 0x%08x: num=%d nev=%d lenw=%d ifend=%d pid=%d\n",
      bigbufout,bigbufout[BBIBUFNUM],bigbufout[BBIEVENTS],bigbufout[BBIWORDS],
      bigbufout[BBIEND],bigbufout[BBIROCID]);
#endif
/*
printf("=0=> %d\n",bigbufout[BBIFD]);
*/
        bigbufout = bb_write(&(bigprocptr->gbigBuffer1));
        if(bigbufout == NULL)
        {
          printf("proc bb_write ERROR 0\n");
          break;
        }
        else /* cleanup pointer so new buffer will be obtained on next loop */
        {
          bigbufout = NULL;
        }
      }

      /* get new output buffer if needed */
      if(bigbufout == NULL)
      {
        bigbufout = bb_write_current(&(bigprocptr->gbigBuffer1));
        if(bigbufout == NULL)
        {
          printf("proc bb_write_current ERROR 1\n");
          break;
        }
        else /* initialize output buffer header */
        {
          bigbufout[BBIWORDS]  = BBHEAD;
          if(bigbufin[BBIBUFNUM] == -1)
          {
            bigbufout[BBIBUFNUM] = -1;
          }
          else
          {
            bigbufout[BBIBUFNUM] = nbuffer ++;
          }
          bigbufout[BBIROCID]  = bigbufin[BBIROCID];
          bigbufout[BBIEVENTS] = 0;
          bigbufout[BBIFD]     = bigbufin[BBIFD];
/*
printf("<--- 0x%08x 0x%08x - fd=%d\n",bigprocptr->gbigBuffer1,bigbufout,bigbufin[BBIFD]);
*/
          bigbufout[BBIEND]    = bigbufin[BBIEND];
          bufout = bigbufout + BBHEAD;
          PROC_TIMER_START;
#ifdef DEBUGPMC
    printf("proc: got output buffer 0x%08x: num=%d nev=%d lenw=%d ifend=%d pid=%d\n",
      bigbufout,bigbufout[BBIBUFNUM],bigbufout[BBIEVENTS],bigbufout[BBIWORDS],
      bigbufout[BBIEND],bigbufout[BBIROCID]);
#endif
        }
      }


      /* process one event */
      lenin = bufin[0] + 1; /* input event length */

      /* force data coping instead of translation for special events */
      if(bigbufin[BBIBUFNUM] == -1)
      {
        printf(">>>>>>>>>>>>>>>> use pid=-1 <<<<<<<<<<<<<<<<<\n");
        lenout = proc_poll(bufin, bufout, -1);
	  }
      else
      {
#ifdef DEBUGPMC
        printf(">>>>>>>>>>>>>>>> use pid=%d <<<<<<<<<<<<<<<<<\n",pid);fflush(stdout);
#endif
        lenout = proc_poll(bufin, bufout, pid);
      }
      bufout += lenout;

      /* update output buffer header */
      bigbufout[BBIEVENTS] ++;
      bigbufout[BBIWORDS] += lenout;

#ifdef DEBUGPMC
      printf("proc: after proc_poll lenout=%d\n",lenout);fflush(stdout);
      printf("proc: time(0)=%u timeout=%u\n",time(0),timeout);fflush(stdout);
      printf("proc: bufout=%u bigbufout=%u BBHEAD_BYTES=%d\n",bufout,bigbufout,BBHEAD_BYTES);fflush(stdout);
#endif

      /* release output buffer on full condition (use actual big buffer size, not SEND_BUF_SIZE !!!) or on timer */
      if( ((bigbufout[BBIWORDS]<<2) > (maxoutbuflen/*SEND_BUF_SIZE*/ - SEND_BUF_MARGIN)) ||
#ifdef VXWORKS
          ((vxTicks > timeout) && ((bufout - bigbufout) > BBHEAD_BYTES)) )
#else
          ((time(0) > timeout) && ((bufout - bigbufout) > BBHEAD_BYTES)) )
#endif
      {

#ifdef DEBUGPMC
        if( ((bufout - bigbufout) > (maxoutbuflen/*SEND_BUF_SIZE*/ - SEND_BUF_MARGIN)) )
        {
          printf("proc: releasing output buffer - 1 on 'buffer full' condition: %d %d %d %d\n",
				 bufout,bigbufout,maxoutbuflen/*SEND_BUF_SIZE*/,SEND_BUF_MARGIN);
        }

#ifdef VXWORKS
        if( ((vxTicks > timeout) && ((bufout - bigbufout) > BBHEAD_BYTES)) )
#else
        if( ((time(0) > timeout) && ((bufout - bigbufout) > BBHEAD_BYTES)) )
#endif
        {
          printf("proc: releasing output buffer - 1 on 'timeout' condition\n");
        }

        printf("proc: releasing output buffer - 1 0x%08x: num=%d nev=%d lenw=%d ifend=%d pid=%d\n",
          bigbufout,bigbufout[BBIBUFNUM],bigbufout[BBIEVENTS],bigbufout[BBIWORDS],
          bigbufout[BBIEND],bigbufout[BBIROCID]);
#endif
/*
printf("=1=> %d\n",bigbufout[BBIFD]);
*/
/*printf("!!!!!!!!!!!! maxoutbuflen=%u datalength=%u\n",maxoutbuflen,(bigbufout[BBIWORDS]<<2));*/
        bigbufout = bb_write(&(bigprocptr->gbigBuffer1));
        if(bigbufout == NULL)
        {
          printf("proc bb_write ERROR 2\n");
          break;
        }
        else /* cleanup pointer so new buffer will be obtained on next loop */
        {
          bigbufout = NULL;
        }
      }

      bufin += lenin;

    } /* finish processing input buffer*/




    /* always release output buffer if input buffer number equal to -1 (special events) */
  /* SERGEY: ALWAYS release output buffer after it processed ??? */
    if((bigbufin[BBIBUFNUM] == -1) && (bigbufout != NULL))
    {
#ifdef DEBUGPMC
    printf("proc: releasing output buffer - 2 0x%08x: num=%d nev=%d lenw=%d ifend=%d pid=%d\n",
      bigbufout,bigbufout[BBIBUFNUM],bigbufout[BBIEVENTS],bigbufout[BBIWORDS],
      bigbufout[BBIEND],bigbufout[BBIROCID]);
#endif
/*
printf("=2=> %d\n",bigbufout[BBIFD]);
*/
      bigbufout = bb_write(&(bigprocptr->gbigBuffer1));
      if(bigbufout == NULL)
      {
        printf("proc bb_write ERROR 3\n");
        break;
      }
      else /* cleanup pointer so new buffer will be obtained on next loop */
      {
        bigbufout = NULL;
      }
	}

/*timing */
#ifdef VXWORKS
    end = sysTimeBaseLGet();
    time2 += (end-start)/MYCLOCK;
#else
    end = gethrtime();
    time2 += (end-start)/MYCLOCK;
#endif
if(nevent != 0 && icycle >= cycle)
{

#ifdef VXWORKS
printf("proc_thread: waiting=%7lu processing=%7lu microsec per event (nev=%d)\n",
	   time1/nevent,time2/nevent,nevent/icycle);
#else
printf("proc_thread: waiting=%7llu processing=%7llu microsec per event (nev=%d)\n",
	   time1/nevent,time2/nevent,nevent/icycle);
#endif
nevent = icycle = time1 = time2 = 0;
}


    if(ifend == 1)
    {
      printf("proc_thread: ifend==1 (%d), ending ..\n",
        bigprocptr->doclose);fflush(stdout);
#ifdef VXWORKS
      taskDelay(60);
#else
      sleep(1);
#endif
	}

	/* ????? why 'bigprocptr->doclose == 1' ??? */
    /* exit the loop if 'End' condition was received */
    if(ifend == 1/* && bigprocptr->doclose == 1*/)
    {
      printf("proc_thread: END condition received\n");fflush(stdout);
      break;
    }

  } while(1);

  /* force 'big' buffer read/write methods to exit */
#ifdef PMCOFFSET
    if(offset == 0) /* we are on host board */
    {
      bb_cleanup(&(bigprocptr->gbigBuffer));
    }
    else            /* we are on pmc board */
    {
      bb_cleanup_pci(&(bigprocptr->gbigBuffer));
    }
#else
    bb_cleanup(&(bigprocptr->gbigBuffer));
#endif

  printf("PROC THREAD EXIT\n");
}

