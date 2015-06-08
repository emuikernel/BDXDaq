
/* circbuf.c - circular buffer library */
/*   (used by UNIX programs only, no vxWorks !!!) */
#ifndef VXWORKS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "circbuf.h"

#define DEBUG1 1
/*
#define DEBUG 1
*/
#define MIN(x,y)    ( (x) < (y) ? (x) : (y) )
#define NFBUF(r,w)  ( (w) >= (r) ? ((w)-(r)) : ((w)-(r)+QSIZE) )

static CIRCBUF rocqueues[32];

CIRCBUF *
new_cb(int roc, char *name, char *parent)
{
  CIRCBUF *cbp;
  int i;
  char temp[100];

  /* use static array rocqueues[32]
  cbp = (CIRCBUF *) malloc(sizeof(CIRCBUF));
  if(cbp == NULL)
  {
    printf("new_cb(): ERROR: cannot allocate memory.\n");
    fflush(stdout);
    return(NULL);
  }*/
  cbp = (CIRCBUF *) &rocqueues[roc];
  memset((void *)cbp,0,sizeof(CIRCBUF));

  cbp->roc = roc;
  sprintf(temp,"%s[%02d]",name,cbp->roc);
  strncpy(cbp->name, temp, 99);
  strncpy(cbp->parent, parent, 99);

  pthread_mutex_init(&cbp->read_lock, NULL);
  pthread_cond_init(&cbp->read_cond, NULL);
  pthread_mutex_init(&cbp->write_lock, NULL);
  pthread_cond_init(&cbp->write_cond, NULL);

  cbp->write = cbp->read = 0;

  for(i=0; i<QSIZE; i++) cbp->nattach[i] = 0;
  for(i=0; i<NTHREADMAX; i++) cbp->nbuf[i] = 0;
  printf("new_cb(): circular buffer '%s' created by '%s' (addr=0x%08x)\n",
            cbp->name,cbp->parent,cbp);
  fflush(stdout);

  return(cbp);
}

/* cb_init must be called in the very beginning of Prestart
to cleanup everything in circular buffer package */
void
cb_init(int roc)
{
  CIRCBUF *cbp;
  int i;

  cbp = (CIRCBUF *) &rocqueues[roc];

  pthread_mutex_init(&cbp->read_lock, NULL);
  pthread_cond_init(&cbp->read_cond, NULL);
  pthread_mutex_init(&cbp->write_lock, NULL);
  pthread_cond_init(&cbp->write_cond, NULL);

  cbp->write = cbp->read = 0;

  for(i=0; i<QSIZE; i++) cbp->nattach[i] = 0;
  for(i=0; i<NTHREADMAX; i++) cbp->nbuf[i] = 0;
  printf("cb_init(): circular buffer for roc=%d initialized\n",roc);
  fflush(stdout);

  return;
}


void
cb_delete(int roc)
{
  CIRCBUF *cbp;
  int i;

  cbp = (CIRCBUF *) &rocqueues[roc];

  printf("cb_delete roc=%d\n",roc);fflush(stdout);

  /* set flag to initiate deleting process */
  cbp->deleting = 1;

  /* broadcast to PUTs and GETs which can be waiting on locks;
  after waking up they suppose to check 'deleting' flag before
  doing anything else, and behave apprpriately */
  pthread_cond_broadcast(&cbp->read_cond);
  pthread_cond_broadcast(&cbp->write_cond);

  return;
}



/*
 * put_cb_data() puts new data on the queue.
 * If the queue is full, it waits until there is room.
 */

int
put_cb_data(CIRCBUF **cbh, void *data)
{
  int icb;
  CIRCBUF *cbp = *cbh;
  unsigned int *buf;

#ifdef DEBUG
  printf("put_cb_data(reached): cbp=0x%08x\n",cbp);
  fflush(stdout);
#endif
  if((cbh == NULL)||(*cbh == NULL)) return(-1);
#ifdef DEBUG
  printf("put_cb_data() 2\n");
  fflush(stdout);
#endif

  if(cbp->deleting)
  {
    printf("PUT: fifo is being emptied !\n");
    fflush(stdout);
    return(-1);
  }

  /* take buffer */
  icb = cbp->write;
#ifdef DEBUG
  printf("put_cb_data(): icb etc  -> %d %d %d\n",
      icb,cbp->read,cbp->write);
  fflush(stdout);
#endif

if(cbp->data[icb] == (void *)-1)
{
  printf("FFFFFFFFFFFFFFFFFF\n");
  fflush(stdout);
}

#ifndef NOALLOC
  /* release previously used buffer */
  if((cbp->data[icb] != NULL) && (cbp->data[icb] != (void *)-1))
  {
#ifdef DEBUG
    printf("put_cb_data(): release [%1d] 0x%08x\n",icb,cbp->data[icb]);
    fflush(stdout);
#endif
    cfree(cbp->data[icb]);
  }
#endif

  cbp->data[icb] = data;
#ifdef DEBUG
  printf("put_cb_data(): got icb=%2d at 0x%08x\n",icb,cbp->data[icb]);
  fflush(stdout);
#endif

#ifdef Linux
  if((int)data != -1) /* can be -1 which means no buffer !!! */
#else
  if((int)data > 0) /* can be -1 which means no buffer !!! */
#endif
  {
int lll;
    buf = (unsigned int *)data;
lll=buf[BBIWORDS]<<2;
    cbp->rocid = buf[BBIROCID];         /* ROC id */
    cbp->nevents[icb] = buf[BBIEVENTS]; /* the number of events in buffer */
    /* &buf[BBHEAD] - start of first event, contains event length in words */
    buf += BBHEAD;
    cbp->evptr1[icb] = buf; /* pointer to the first event in buffer */




#ifdef DEBUG
  printf("PUT[%2d]: nevents=%d, buf=0x%08x, 1st 0x%08x, len=0x%08x (end=0x%08x)\n",
  icb,cbp->nevents[icb],data,cbp->evptr1[icb],lll,(int)buf+lll);
  {
    int i;
    unsigned int *buff = (unsigned int *)data;
    for(i=0; i<32; i+=8)
      printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
        buff[i],buff[i+1],buff[i+2],buff[i+3],buff[i+4],buff[i+5],buff[i+6],buff[i+7]);
  }
  fflush(stdout);
#endif

#ifdef DEBUG
    printf("put_cb_data(): icb=%2d contains %d events\n",icb,cbp->nevents[icb]);
    fflush(stdout);
#endif










    /* check if next buffer has been read already; if not - wait */
    icb = (icb + 1) % QSIZE;
    pthread_mutex_lock(&cbp->read_lock);
    while(icb == cbp->read)
    {
      printf("PUT: wait while icb=%d == cbp->read=%d -------\n",icb,cbp->read);
      /*sleep(1);*/pthread_cond_wait(&cbp->read_cond, &cbp->read_lock);
    }
    pthread_mutex_unlock(&cbp->read_lock);
#ifdef DEBUG
  printf("PUT: locking .. (icb=%d)\n",icb);fflush(stdout);
#endif
    /* set 'write' pointer to the next buffer */
    pthread_mutex_lock(&cbp->write_lock);
#ifdef DEBUG
  printf("PUT: locked (icb=%d)\n",icb);fflush(stdout);
#endif
	cbp->write = icb;
    /* let a waiting reader know there's data */
    pthread_cond_signal(&cbp->write_cond);
    pthread_mutex_unlock(&cbp->write_lock);
#ifdef DEBUG
  printf("PUT: unlock (icb=%d)\n",icb);fflush(stdout);
#endif
  }
  else
  {
#ifdef DEBUG
    printf("put_cb_data(): %s data=%d -> set cbp->nevents[%d]=-1\n",
                cbp->name,(int)data,icb,cbp->nevents[icb]);
    fflush(stdout);
#endif
    cbp->nevents[icb] = -1; /* need it in cb_events_get() */
  }

#ifdef DEBUG
  printf("put_cb_data(done): cbp=0x%08x\n",cbp);
  fflush(stdout);
#endif

  return(0);
}


/*
 * get_cb_count() gets the number of filled circular buffers.
 */

/* USED TO FLASH INPUT STREAMS ONLY NOW !!! */

int
get_cb_count(CIRCBUF **cbh)
{
  CIRCBUF *cbp = *cbh;
  int count;
  
  if((cbh == NULL)||(*cbh == NULL)) return(-1);

  pthread_mutex_lock(&cbp->read_lock);
  pthread_mutex_lock(&cbp->write_lock);

  count = NFBUF(cbp->read,cbp->write);
  printf("CCCCCCCCCC: %d\n",count);
  fflush(stdout);

  pthread_mutex_unlock(&cbp->read_lock);
  pthread_mutex_unlock(&cbp->write_lock);

  return(count);
}






/********************/
/* group operations */


/*
   Gets a chunk of event fragments from all circular buffers

   input:  cba[32] - array of circular buffers
           id - 'id' of the calling process
           nrocs - the number of active rocs (ebp->nrocs from deb_component.c)
           chunk - the number of events requested
   output: **buf - pointers to the events returned
           evsize - average event size for every ROC (temporary ?)
        (was in deb_component.c:
           roc_queue_evsize[roc] = (desc->lenbuf)/(desc->nevbuf);
         must do something similar here !!!)
		 *nphys - the number of physics events returned (excluding scaler
                  events)
   return: the number of events returned
*/

#define NPROF1 100

int
cb_events_get(CIRCBUF *cba[32], int id, int nrocs, int chunk,
              unsigned int *evptrr[32][NCHUNKMAX], int evsize[32], int *nphys)
{
  int i, j, nev1, nev2, nevtot, nevbuf, lenev, icb, icb_next, lenbuf, nev;
  int nevphys, itmp, icb_next_available;
  CIRCBUF *cbp;
  unsigned int *buf;
  unsigned int **evptr;
/*
static hrtime_t start1[NTHREADMAX], end1[NTHREADMAX], time1[NTHREADMAX];
static hrtime_t start2[NTHREADMAX], end2[NTHREADMAX], time2[NTHREADMAX];
static hrtime_t nevtime1[NTHREADMAX], nevchun[NTHREADMAX];
start1[id] = gethrtime();
*/

#ifdef DEBUG
  printf("cb_events_get[%1d] reached, nrocs=%d, chunk=%d\n",id,nrocs,chunk);
  fflush(stdout);
#endif

  if(nrocs<=0)
  {
    printf("cb_events_get[%1d]: ERROR: nrocs=%d, return no events\n",id,nrocs);
    fflush(stdout);
    return(0);
  }

  /* first loop - found the minimum number of events
  and compare it with 'chunk'; use the minimum one;
  buffers will be locked and unlocked one by one to get 'nevtot' */
  nevphys = 0;
  nev = chunk;
  for(i=0; i<nrocs; i++)
  {
    cbp = cba[i]; /* set pointer to the next fifo */
	/*
printf("------------------> i=%d nbuf=%d\n",i,cbp->nbuf[id]);fflush(stdout);
	*/
    /* get first buffer number with valid data
    and the number of the next buffer */
    icb = cbp->read;
#ifdef DEBUG
  printf("GET1: icb=%d\n",icb);
#endif
    pthread_mutex_lock(&cbp->write_lock);
    while(icb == cbp->write)
    {
#ifdef DEBUG
      printf("[%1d] GET: wait (cbp->write=%d) ..............\n",id,cbp->write);
#endif  
      /*sleep(1);*/pthread_cond_wait(&cbp->write_cond, &cbp->write_lock);
#ifdef DEBUG
      printf("[%1d] GET: proceed ..\n",id);
#endif  
      if(cbp->deleting)
      {
        printf("[%1d] deleting 1 .. %8.8s\n",id,cbp->name);fflush(stdout);
        pthread_mutex_unlock(&cbp->write_lock);
        return(-1);
      }
    }
    pthread_mutex_unlock(&cbp->write_lock);


    icb_next = (icb + 1) % QSIZE;
#ifdef DEBUG
  printf("  GET1: icb_next=%d cbp->write=%d\n",icb_next,cbp->write);
#endif
    if(icb_next == cbp->write)
    {
      /*printf("  GET1: next is NOT available (cbp->write=%d)\n",cbp->write);*/
      icb_next_available = 0;
    }
    else
    {
      /*printf("  GET1: next is available (cbp->write=%d)\n",cbp->write);*/
      icb_next_available = 1;
    }

/*start2[id] = gethrtime();*/

    /* wait while there's nothing in the buffer */
    while(cbp->nevents[icb] == 0)
    {
printf("[%1d] >%8.8s< NEVER COME HERE - 1\n",id,cbp->name);fflush(stdout);
      if(cbp->deleting && (NFBUF(cbp->read,cbp->write) == 0))
      {
        printf("[%1d] deleting 2 .. %8.8s\n",id,cbp->name);fflush(stdout);
        return(-1);
      }

#ifdef DEBUG
      printf("cb_events_get[%1d] empty, wait for %s (rocid=%d) (adr=0x%08x) %d\n",
                   id,cbp->name,cbp->rocid,cbp,cbp->nevents[icb]);fflush(stdout);
      printf("cb_events_get[%1d] #roc=%d\n",id,cbp->rocid);
      fflush(stdout);
#endif
      sleep(1);
#ifdef DEBUG
      printf("cb_events_get[%1d] itmp=%d\n",id,itmp);fflush(stdout);
#endif
    }

/*
end2[id] = gethrtime();
time2[id] += ((end2[id]-start2[id])/1000);
*/

/*printf("[%1d] 2\n",id);fflush(stdout);*/

#ifdef Linux
    if((int)cbp->data[icb] != -1)
#else
    if((int)cbp->data[icb] > 0)
#endif
    {
      if(icb_next_available)
      {
        if(cbp->nevents[icb_next]<=0)
        {
          printf("cb_events_get[%1d] ERROR <<<%d>>>\n",
            id,cbp->nevents[icb_next]);
          fflush(stdout);
        }
        nevtot = MIN(chunk,(cbp->nevents[icb]+cbp->nevents[icb_next]));
/*printf("cb_events_get[%1d] 2: %d -> %d\n",
id,(cbp->nevents[icb]+cbp->nevents[icb_next]),nevtot);
fflush(stdout);*/
      }
      else
      {
        nevtot = MIN(chunk,cbp->nevents[icb]);
      }
    }
    nev = MIN(nev,nevtot);

    /* return 0 if there's nothing at least in one of the fifo's */
    /* will NEVER happens - we have 'wait' above !!! */
    if(nev == 0)
    {
printf("NEVER COME HERE 2\n");fflush(stdout);
      return(0);
	}
  }

#ifdef DEBUG
  printf("cb_events_get[%1d] nev=%d\n",id,nev);fflush(stdout);
#endif

  /* loop again over array of fifo's taking events;
  unlock every fifo after processing !!! */
  lenbuf = 0;
  for(i=0; i<nrocs; i++)
  {
    cbp = cba[i]; /* set pointer to the next roc's fifo */
    /*if(cbp != NULL) printf("[%2d] 2 %8.8s\n",i,cbp->name);*/

    /* release buffers kept by 'id' if any */
    for(j=0; j<cbp->nbuf[id]; j++)
    {
      icb = (cbp->buf1[id]+j) % QSIZE;
      /*printf("      GET1: release icb[%1d]=%d\n",j,icb);*/
      cbp->nattach[icb] --;
      if(cbp->nattach[icb] < 0)
      {
        printf("cb_events_get[%1d] ERROR: cbp->nattach[%d]=%d\n",
                      id,icb,cbp->nattach[icb]); fflush(stdout);
      }

#ifdef NOALLOC
      /* mark buffer as 'free' if .. */
      if(cbp->nattach[icb]==0) /* .. nobody holds that buffer, and .. */
      {
        if(cbp->nevents[icb]==0) /* .. all events are used from that buffer */
        {
          buf = (unsigned int *)cbp->data[icb];
#ifdef DEBUG
          printf("cb_events_get[%1d]: free buffer %d (0x%08x)\n",id,icb,buf);
          fflush(stdout);
#endif
          buf[0] = 0; /* mark buffer as free */
        }
      }
#endif

    }
    cbp->nbuf[id] = 0;

    /* get first buffer number with valid data
    and the number of the next buffer */
    icb = cbp->read;
    icb_next = (icb + 1) % QSIZE;
    /*printf("GET2(second pass): icb=%d icb_next=%d\n",icb,icb_next);*/
/*
    icb = cbp->start_idx;
    icb_next = (cbp->start_idx + 1) % QSIZE;
printf("    GET2(second pass): icb=%d icb_next=%d cbp->write=%d\n",icb,icb_next,cbp->write);
*/

    evptr = evptrr[i];


  /* if buffer pointer is greater then zero then process the buffer */ 
  /* -1 which means no buffer !!! */ 
#ifdef Linux
  if((int)cbp->data[icb] != -1) 
#else
  if((int)cbp->data[icb] > 0) 
#endif
  { 
    nevbuf = nev; 
if(cbp->nevents[icb] < 0) 
{ 
printf("%s ????? \n",cbp->name);fflush(stdout); 
printf("%s ????? %d > %d+%d\n",cbp->name,nev,cbp->nevents[icb], 
cbp->nevents[icb_next]); fflush(stdout); 
} 
/*printf("[%1d] get_cb_data():nev=%d (chunk=%d nevents[%1d]=%d\n", 
  id,nev,chunk,icb,cbp->nevents[icb]); fflush(stdout);*/ 
    if(cbp->nevents[icb] >= nev) /* use first buffer only - enough events */ 
    { 
/*printf("[%1d] get_cb_data(): 1: buf %d has %d events (chunk=%d)\n", 
  id,icb,cbp->nevents[icb],nev); fflush(stdout);*/ 
      /* attach buffer */ 
      cbp->nattach[icb] ++; 
      cbp->buf1[id] = icb; 
      cbp->nbuf[id] = 1; 
      /* get the pointer to the first valid event in the buffer */ 
      buf =  cbp->evptr1[icb]; 
/*printf("[%1d] GET: first event 0x%08x from icb=%d (0x%08x)\n", 
id,cbp->evptr1[icb],icb,cbp); fflush(stdout);*/ 
      /* fill array of pointers to events */ 
      lenbuf = 0; 
      for(j=0; j<nev; j++) 
      { 
        evptr[j] = buf; 
        lenev = buf[0]+1; 
        lenbuf += lenev; /* returned buffer length in words */ 
        nevphys += ( (((buf[1]>>16)&0xff)==0) ? 0 : 1 ); 
        buf += lenev; 
      } 
/*printf("[%1d] GET: last event 0x%08x from icb=%d (0x%08x)\n", 
id,evptr[nev-1],icb,cbp); fflush(stdout);*/ 
      cbp->evptr1[icb] = buf; /* new first valid event */ 
      cbp->nevents[icb] -= nev; /* update the number of events in buffer */ 
      /* if buffer is empty now switch to the next one */ 
      if(cbp->nevents[icb]==0) 
      { 
/*printf("[%1d] get_cb_data(): cbp->nevents[%d]==0\n",id,icb); fflush(stdout);*/ 



        
        pthread_mutex_lock(&cbp->read_lock); 
        cbp->read = (icb + 1) % QSIZE; 
        /*printf("GET3: set cbp->read=%d\n",cbp->read);*/ 
        
       /* let a waiting writer know there's room */ 
        pthread_cond_signal(&cbp->read_cond); 
        pthread_mutex_unlock(&cbp->read_lock); 



      } 
    } 
    else /* use both current and next buffer */ 
    { 
/*printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"); 
printf("[%1d] get_cb_data(): 2: buf %d has %d events (chunk=%d)\n", 
id,icb,cbp->nevents[icb],nev); fflush(stdout);*/ 
      /* attach first buffer */ 
      cbp->nattach[icb] ++; 
      cbp->buf1[id] = icb; 
      cbp->nbuf[id] = 1; 
      /* get the pointer to the first valid event in the buffer */ 
      buf =  cbp->evptr1[icb]; 
/*printf("[%1d] GET: first event 0x%08x from icb=%d (0x%08x)\n", 
id,cbp->evptr1[icb],icb,cbp); fflush(stdout);*/ 
      /* get the number of events in the buffer */ 
      nev1 = cbp->nevents[icb]; 
/*printf("[%1d] nev1=%d icb=%d (0x%08x)\n",id,nev1,icb,cbp); fflush(stdout);*/ 
      /* fill array of pointers to events from current buffer */ 
      lenbuf = 0; 
      for(j=0; j<nev1; j++) 
      { 
        evptr[j] = buf; 
        lenev = buf[0]+1; 
        lenbuf += lenev; /* returned buffer length in words */ 
        nevphys += ( (((buf[1]>>16)&0xff)==0) ? 0 : 1 ); 
        buf += lenev; 
      } 
      cbp->nevents[icb] -= nev1; /* update the number of events in buffer */ 
      /* switch to the next buffer */ 



      
      pthread_mutex_lock(&cbp->read_lock); 
      cbp->read = (icb + 1) % QSIZE; 
      /* let a waiting writer know there's room */ 
      pthread_cond_signal(&cbp->read_cond); 
      pthread_mutex_unlock(&cbp->read_lock); 
      /*printf("GET4: set cbp->read=%d\n",cbp->read);*/ 
      
      icb = (icb + 1) % QSIZE; 



    if(cbp->read == cbp->write) printf("  GET41: NEVER COME HERE !!!\n"); 



      /* attach second buffer */ 
      cbp->nattach[icb] ++; 
      cbp->nbuf[id] ++; 
      /* get the pointer to the first valid event in the buffer */ 
      buf =  cbp->evptr1[icb]; 
      /* get the number of events in the buffer */ 
      nev2 = nev - nev1; 
/*printf("[%1d] get_cb_data(): nev2=%d icb=%d (0x%08x)\n",id,nev2,icb,cbp); fflush(stdout);*/ 
if(cbp->nevents[icb] < nev2) 
{ 
printf("%s cbp->nevents[%d]=%d < nev2=%d\n",cbp->name,cbp->nevents[icb],nev2); 
 fflush(stdout); 
printf("SHOULD NEVER COME HERE !!!\n"); fflush(stdout); 
exit(0); 
} 
      /* fill array of pointers to events from current buffer */ 
      for(j=nev1; j<nev; j++) 
      { 
        evptr[j] = buf; 
        lenev = buf[0]+1; 
        lenbuf += lenev; /* returned buffer length in words */ 
        nevphys += ( (((buf[1]>>16)&0xff)==0) ? 0 : 1 ); 
        buf += lenev; 
      } 
/*printf("[%1d] GET: last event 0x%08x from icb=%d (0x%08x)\n", 
id,evptr[nev-1],icb,cbp); fflush(stdout);*/ 
      cbp->evptr1[icb] = buf; /* new first valid event */ 
      cbp->nevents[icb] -= nev2; /* update the number of events in buffer */ 
      /* if buffer is empty now switch to the next one */ 
      if(cbp->nevents[icb]==0) 
      { 
/*printf("[%1d] get_cb_data(): second chunk: cbp->nevents[%d]==0\n",id,icb); 
fflush(stdout);*/ 



        
        pthread_mutex_lock(&cbp->read_lock); 
        cbp->read = (icb + 1) % QSIZE; 
        /* let a waiting writer know there's room */ 
        pthread_cond_signal(&cbp->read_cond); 
        pthread_mutex_unlock(&cbp->read_lock); 
        /*printf("GET5: set cbp->read=%d\n",cbp->read);*/ 
        



      } 
    } 
  } 
  else /* do we need it ??? */ 
  { 
    nevbuf = -1; 



    
    pthread_mutex_lock(&cbp->read_lock); 
    cbp->read = (cbp->read + 1) % QSIZE; 
    /* let a waiting writer know there's room */ 
    pthread_cond_signal(&cbp->read_cond); 
    pthread_mutex_unlock(&cbp->read_lock); 
    printf("GET6: set cbp->read=%d (WHY WE ARE HERE ???\n",cbp->read); 
    



    printf("get_cb_data(): return -1 ?? (cbp=0x%08x)\n",cbp); fflush(stdout); 
  }























    evsize[i] = lenbuf/nevbuf;
/*
printf("cb_events_get[%1d] evsize[%2d]=%d\n",id,i,evsize[i]);fflush(stdout);
*/
  }

/*printf("cb_events_get[%1d] return nev=%d\n",id,nev);fflush(stdout);*/


/*
end1[id] = gethrtime();
time1[id] += ((end1[id]-start1[id])/1000);
nevchun[id] = nevchun[id] + ((hrtime_t)nev);
if(++nevtime1[id] == NPROF1)
{
  printf("[%1d] average chunk size = %4lld,",
    id,nevchun[id]/nevtime1[id]);
  if(nevchun[id] > 0)
  {
    printf("    tot=%7lld wait=%7lld get=%7lld\n",
      time1[id]/nevchun[id],time2[id]/nevchun[id],
      (time1[id]-time2[id])/nevchun[id]);
  }
  else
  {
    printf("\n");
  }
  nevtime1[id] = 0;
  time1[id] = 0;
  time2[id] = 0;
  nevchun[id] = 0;
}
*/

  *nphys = nevphys / nrocs; /* we are summing 'nevphys' for every roc */

  return(nev);
}


int
cb_events_init(CIRCBUF *cba[32])
{
  int i;
  CIRCBUF *f;

  printf("cb_events_init() reached\n"); fflush(stdout);

  for(i=0; i<32; i++)
  {
    f = cba[i];
    /*if(f != NULL) printf("[%2d] 2 %8.8s\n",i,f->name);*/
  }

  return(0);
}




/******************************************************************/
/******************************************************************/
/******************************************************************/




/*
 * get_cb_data() gets the oldest data in the circular buffer.
 * If there is none, it waits until new data appears.
 *   returns the number of events in buffer or -1 if no buffer
 */

/* USED TO FLASH INPUT STREAMS ONLY NOW !!! */

int
get_cb_data(CIRCBUF **cbh, int id, int chunk, unsigned int *evptr[NCHUNKMAX],
            int *lenbuffer, int *rocid)
{
  CIRCBUF *cbp = *cbh;
  int nevbuf, nevphys, itmp, icb_next_available;
  int i, j, icb, icb_next, nev, nev1, nev2, numev, lenev, lenbuf;
  unsigned int *buf;

#ifdef DEBUG
  printf("get_cb_data reached\n");fflush(stdout);
#endif

  /* entry conditions check */
  if((cbh == NULL)||(*cbh == NULL)) return(-1);
  if(cbp->deleting && (NFBUF(cbp->read,cbp->write) == 0))
  {
    printf("get_cb_data(): deleting\n");
    fflush(stdout);
    return(-1);
  }

  /* release buffers kept by 'id' */
  for(j=0; j<cbp->nbuf[id]; j++)
  {
    icb = (cbp->buf1[id]+j) % QSIZE;
#ifdef DEBUG
    printf("get_cb_data(): thread[%1d] releases buf %3d\n",id,icb);
    fflush(stdout);
#endif
    cbp->nattach[icb] --;
    if(cbp->nattach[icb] < 0)
    {
      printf("get_cb_data(): ERROR: thread[%d]: cbp->nattach[%d]=%d\n",
                    id,icb,cbp->nattach[icb]); fflush(stdout);
    }

#ifdef NOALLOC
    /* mark buffer as 'free' if .. */
    if(cbp->nattach[icb]==0) /* .. nobody holds that buffer, and .. */
    {
      if(cbp->nevents[icb]==0) /* .. all events are used from that buffer */
      {
        buf = (unsigned int *)cbp->data[icb];
#ifdef DEBUG
        printf("get_cb_data(): free buffer %d (0x%08x)\n",icb,buf);
        fflush(stdout);
#endif
        buf[0] = 0; /* mark buffer as free */
      }
    }
#endif

  }
  cbp->nbuf[id] = 0;


  /* get first buffer number with valid data
  and the number of the next buffer */
  /*
  icb = cbp->start_idx;
  icb_next = (cbp->start_idx + 1) % QSIZE;
  */
  icb = cbp->read;
  icb_next = (icb + 1) % QSIZE;

  if(NFBUF(cbp->read,cbp->write) > 1) icb_next_available = 1;
  else                                icb_next_available = 0;

  /* wait while there's nothing in the buffer */
  while(cbp->nevents[icb] == 0)
  {
#ifdef DEBUG
    printf("get_cb_data(): empty, wait for %s (rocid=%d) (adr=0x%08x)\n",
                   cbp->name,cbp->rocid,cbp); fflush(stdout);
#endif
  }

  /* get ROC id */
  *rocid  = cbp->rocid;

#ifdef Linux
  if((int)cbp->data[icb] != -1) 
#else
  if((int)cbp->data[icb] > 0)
#endif
  {
    /* get the number of events will be returned; it cannot be more then
    we have total AND in 2 buffers */
    if(icb_next_available)
    {
      if(cbp->nevents[icb_next]<=0)
      {
        printf("ERRR <<<%d>>>\n",cbp->nevents[icb_next]);
        fflush(stdout);
      }
      nev=MIN(chunk,(cbp->nevents[icb]+cbp->nevents[icb_next]));
    }
    else
    {
      nev=MIN(chunk,cbp->nevents[icb]);
    }
  }
  lenbuf = 0;


  /*GET_ONE_ROC_DATA*/
  /* if buffer pointer is greater then zero then process the buffer */
  /* -1 which means no buffer !!! */
#ifdef Linux
  if((int)cbp->data[icb] != -1)
#else
  if((int)cbp->data[icb] > 0)
#endif
  {
    nevbuf = nev;
if(cbp->nevents[icb] < 0)
{
printf("%s ????? \n",cbp->name);fflush(stdout);
printf("%s ????? %d > %d+%d\n",cbp->name,nev,cbp->nevents[icb],
cbp->nevents[icb_next]); fflush(stdout);
}
/*printf("[%1d] get_cb_data():nev=%d (chunk=%d nevents[%1d]=%d\n",
  id,nev,chunk,icb,cbp->nevents[icb]); fflush(stdout);*/
    if(cbp->nevents[icb] >= nev) /* use first buffer only - enough events */
    {
/*printf("[%1d] get_cb_data(): 1: buf %d has %d events (chunk=%d)\n",
  id,icb,cbp->nevents[icb],nev); fflush(stdout);*/
      /* attach buffer */
      cbp->nattach[icb] ++;
      cbp->buf1[id] = icb;
      cbp->nbuf[id] = 1;
      /* get the pointer to the first valid event in the buffer */ \
      buf =  cbp->evptr1[icb]; \
/*printf("[%1d] GET: first event 0x%08x from icb=%d (0x%08x)\n", \
id,cbp->evptr1[icb],icb,cbp); fflush(stdout);*/ \
      /* fill array of pointers to events */ \
      lenbuf = 0; \
      for(j=0; j<nev; j++) \
      { \
        evptr[j] = buf; \
        lenev = buf[0]+1; \
        lenbuf += lenev; /* returned buffer length in words */ \
        nevphys += ( (((buf[1]>>16)&0xff)==0) ? 0 : 1 ); \
        buf += lenev; \
      } \
/*printf("[%1d] GET: last event 0x%08x from icb=%d (0x%08x)\n", \
id,evptr[nev-1],icb,cbp); fflush(stdout);*/ \
      cbp->evptr1[icb] = buf; /* new first valid event */ \
      cbp->nevents[icb] -= nev; /* update the number of events in buffer */ \
      /* if buffer is empty now switch to the next one */ \
      if(cbp->nevents[icb]==0) \
      { \
/*printf("[%1d] get_cb_data(): cbp->nevents[%d]==0\n",id,icb); fflush(stdout);*/ \
\
\
\
        \
        pthread_mutex_lock(&cbp->read_lock); \
        cbp->read = (icb + 1) % QSIZE; \
        /*printf("GET3: set cbp->read=%d\n",cbp->read);*/ \
        \
       /* let a waiting writer know there's room */ \
        pthread_cond_signal(&cbp->read_cond); \
        pthread_mutex_unlock(&cbp->read_lock); \
\
\
\
      } \
    } \
    else /* use both current and next buffer */ \
    { \
/*printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"); \
printf("[%1d] get_cb_data(): 2: buf %d has %d events (chunk=%d)\n", \
id,icb,cbp->nevents[icb],nev); fflush(stdout);*/ \
      /* attach first buffer */ \
      cbp->nattach[icb] ++; \
      cbp->buf1[id] = icb; \
      cbp->nbuf[id] = 1; \
      /* get the pointer to the first valid event in the buffer */ \
      buf =  cbp->evptr1[icb]; \
/*printf("[%1d] GET: first event 0x%08x from icb=%d (0x%08x)\n", \
id,cbp->evptr1[icb],icb,cbp); fflush(stdout);*/ \
      /* get the number of events in the buffer */ \
      nev1 = cbp->nevents[icb]; \
/*printf("[%1d] nev1=%d icb=%d (0x%08x)\n",id,nev1,icb,cbp); fflush(stdout);*/ \
      /* fill array of pointers to events from current buffer */ \
      lenbuf = 0; \
      for(j=0; j<nev1; j++) \
      { \
        evptr[j] = buf; \
        lenev = buf[0]+1; \
        lenbuf += lenev; /* returned buffer length in words */ \
        nevphys += ( (((buf[1]>>16)&0xff)==0) ? 0 : 1 ); \
        buf += lenev; \
      } \
      cbp->nevents[icb] -= nev1; /* update the number of events in buffer */ \
      /* switch to the next buffer */ \
\
\
\
      \
      pthread_mutex_lock(&cbp->read_lock); \
      cbp->read = (icb + 1) % QSIZE; \
      /* let a waiting writer know there's room */ \
      pthread_cond_signal(&cbp->read_cond); \
      pthread_mutex_unlock(&cbp->read_lock); \
      /*printf("GET4: set cbp->read=%d\n",cbp->read);*/ \
      \
      icb = (icb + 1) % QSIZE; \
\
\
\
    if(cbp->read == cbp->write) printf("  GET41: NEVER COME HERE !!!\n"); \
\
\
\
      /* attach second buffer */ \
      cbp->nattach[icb] ++; \
      cbp->nbuf[id] ++; \
      /* get the pointer to the first valid event in the buffer */ \
      buf =  cbp->evptr1[icb]; \
      /* get the number of events in the buffer */ \
      nev2 = nev - nev1; \
/*printf("[%1d] get_cb_data(): nev2=%d icb=%d (0x%08x)\n",id,nev2,icb,cbp); fflush(stdout);*/ \
if(cbp->nevents[icb] < nev2) \
{ \
printf("%s cbp->nevents[%d]=%d < nev2=%d\n",cbp->name,cbp->nevents[icb],nev2); \
 fflush(stdout); \
printf("SHOULD NEVER COME HERE !!!\n"); fflush(stdout); \
exit(0); \
} \
      /* fill array of pointers to events from current buffer */ \
      for(j=nev1; j<nev; j++) \
      { \
        evptr[j] = buf; \
        lenev = buf[0]+1; \
        lenbuf += lenev; /* returned buffer length in words */ \
        nevphys += ( (((buf[1]>>16)&0xff)==0) ? 0 : 1 ); \
        buf += lenev; \
      } \
/*printf("[%1d] GET: last event 0x%08x from icb=%d (0x%08x)\n", \
id,evptr[nev-1],icb,cbp); fflush(stdout);*/ \
      cbp->evptr1[icb] = buf; /* new first valid event */ \
      cbp->nevents[icb] -= nev2; /* update the number of events in buffer */ \
      /* if buffer is empty now switch to the next one */ \
      if(cbp->nevents[icb]==0) \
      { \
/*printf("[%1d] get_cb_data(): second chunk: cbp->nevents[%d]==0\n",id,icb); \
fflush(stdout);*/ \
\
\
\
        \
        pthread_mutex_lock(&cbp->read_lock); \
        cbp->read = (icb + 1) % QSIZE; \
        /* let a waiting writer know there's room */ \
        pthread_cond_signal(&cbp->read_cond); \
        pthread_mutex_unlock(&cbp->read_lock); \
        /*printf("GET5: set cbp->read=%d\n",cbp->read);*/ \
        \
\
\
\
      } \
    } \
  } \
  else /* do we need it ??? */ \
  { \
    nevbuf = -1; \
\
\
\
    \
    pthread_mutex_lock(&cbp->read_lock); \
    cbp->read = (cbp->read + 1) % QSIZE; \
    /* let a waiting writer know there's room */ \
    pthread_cond_signal(&cbp->read_cond); \
    pthread_mutex_unlock(&cbp->read_lock); \
    printf("GET6: set cbp->read=%d (WHY WE ARE HERE ???\n",cbp->read); \
    \
\
\
\
    printf("get_cb_data(): return -1 ?? (cbp=0x%08x)\n",cbp); fflush(stdout); \
  }


  
  *lenbuffer = lenbuf;

  return(nevbuf);
}


char *
get_cb_name(CIRCBUF *cbp)
{
  if(cbp == NULL) return("UNKNOWN");
  else            return(cbp->name);
}


/*
 * delete_cb() frees a circular buffer.
 */

/* NOT IN USE !!! */

void
delete_cb(CIRCBUF **cbh)
{
  CIRCBUF *cbp = *cbh;

  if((cbh == NULL)||(*cbh == NULL)) return;

  printf("delete_cb 1\n");fflush(stdout);
  pthread_mutex_trylock(&cbp->read_lock);
  pthread_mutex_trylock(&cbp->write_lock);
  if(cbp->deleting)
  {
    pthread_mutex_unlock(&cbp->read_lock);
    pthread_mutex_unlock(&cbp->write_lock);
    return;
  }
  
  cbp->deleting = 1;

  printf("delete_cb 2\n");fflush(stdout);
  pthread_cond_broadcast(&cbp->read_cond);
  pthread_cond_broadcast(&cbp->write_cond);

  pthread_mutex_unlock(&cbp->read_lock);
  pthread_mutex_unlock(&cbp->write_lock);

  pthread_mutex_destroy(&cbp->read_lock);
  pthread_mutex_destroy(&cbp->write_lock);

  pthread_cond_destroy(&cbp->read_cond);
  pthread_cond_destroy(&cbp->write_cond);

  printf("delete_cb 3\n");fflush(stdout);
  free(cbp->name);
  free(cbp);

  return;
}


#else /* ifndef VXWORKS */

void
circbuf_vxworks_dummy()
{
  return;
}

#endif





