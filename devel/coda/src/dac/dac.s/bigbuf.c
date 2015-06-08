
/* bigbuf.c - library for the memory allocation system */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef VXWORKS
#include <vxWorks.h>
#include <semLib.h>
#include <memLib.h>
#endif

#include "circbuf.h"

/*
#ifndef VXWORKS
*/
#define logMsg printf
/*
#endif
*/



/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/********** BIG BUFFERS MANAGEMENT PACKAGE **********************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#include "bigbuf.h"


/* returns pool id */

BIGBUF *
bb_new(int nbufs, int nbytes)
{
  BIGBUF *bbp;
  int i;

  /* check parameters */
  if(nbufs > NBIGBUFFERS)
  {
    logMsg("bb_new: ERROR: nbufs=%d (must be <= %d\n",
      nbufs,NBIGBUFFERS,3,4,5,6);
    return(0);
  }

  /* allocate structure */
  bbp = (BIGBUF *) malloc(sizeof(BIGBUF));
  if(bbp == NULL)
  {
    logMsg("bb_new: ERROR: cannot allocate memory for 'bbp'\n",
      1,2,3,4,5,6);
    return(0);
  }
  else
  {
    bzero((void *)bbp,sizeof(BIGBUF));
  }

  /* allocate buffers */
  bbp->nbufs = nbufs;
  bbp->nbytes = nbytes;
  for(i=0; i<bbp->nbufs; i++)
  {
    bbp->data[i] = (unsigned int *) malloc(bbp->nbytes);
    if(bbp->data[i] == NULL)
    {
      logMsg("bb_new: ERROR: buffer allocation FAILED\n",1,2,3,4,5,6);
      return(0);
    }
  }

  /* initialize semaphores */
#ifdef VXWORKS
  bbp->read_lock = semBCreate(SEM_Q_FIFO, SEM_FULL);
  bbp->write_lock = semBCreate(SEM_Q_FIFO, SEM_FULL);
  if(bbp->read_lock == NULL || bbp->write_lock == NULL)
  {
    logMsg("bb_new: ERROR: could not allocate a semaphore\n",1,2,3,4,5,6);
    return(0);
  }
#else
  pthread_mutex_init(&bbp->read_lock, NULL);
  pthread_mutex_init(&bbp->write_lock, NULL);
#endif

  /* initialize index */
  bbp->write = 1;
  bbp->read = 0;

  /* reset cleanup condition */
  bbp->cleanup = 0;

  logMsg("bb_new: 'big' buffer created (addr=0x%08x, %d bufs, %d size)\n",
    bbp,bbp->nbufs,bbp->nbytes,4,5,6);

  return(bbp);
}





/*************/
/* GEF staff */

#ifdef Linux_vme

#include "../../rol/jvme/jvme.h"
#include "gef/gefcmn_vme.h"

GEF_VME_BUS_HDL vmeHdl;

static int n_dma_hdl = 0;
GEF_VME_DMA_HDL dma_hdl[1024];

#define UINT32 unsigned int


/*similar to the usrVmeDmaInit() from usrvme.c*/
void
bb_InitChunk(BIGBUF *bbp, int n)
{
  unsigned int offset;
  GEF_STATUS status;
  GEF_MAP_PTR mapPtr;

  if(vmeHdl==NULL)
  {
    printf("bb_InitChunk ERROR: vmeHdl undefined.\n");
    return;
  }
 /* Parameters:
 *    <GEF_VME_BUS_HDL> vmeHdl   - (I) Handle allocated by <gefVmeOpen>.
 *    GEF_UINT32 size         - (I) size of the buffer.
 *    <GEF_VME_DMA_HDL> *dma_hdl - (O) DMA handle of type <GEF_VME_DMA_HDL>.
 *    void **addr                - (O) User process address to allocated DMA Buffer.
 */
  status = gefVmeAllocDmaBuf (vmeHdl, bbp->nbytes, &dma_hdl[n_dma_hdl], &mapPtr);
  if(status != GEF_STATUS_SUCCESS) 
  {
    printf("bb_InitChunk ERROR: gefVmeAllocDmaBuf returned 0x%x\n",status);
    return;
  }

  bbp->data[n] = (unsigned int *)mapPtr;
  memset((char *)bbp->data[n],0,bbp->nbytes);

  bbp->userMemBase[n] = (unsigned int)bbp->data[n];
  bbp->physMemBase[n] = (unsigned int)dma_hdl[n_dma_hdl]->phys_addr;

  offset = bbp->userMemBase[n] - bbp->physMemBase[n];

  printf("bb_InitChunk[%2d] (n_dma_hdl=%2d): userMem=0x%08x, physMem=0x%08x, size=%d bytes (offset=0x%08x)\n",
		 n, n_dma_hdl, bbp->userMemBase[n], bbp->physMemBase[n], bbp->nbytes, offset);

  n_dma_hdl ++;

  return;
}


/* big buffers for ROL1 will be in the special area for VME->MEM DMA */

BIGBUF *
bb_new_rol1(int nbufs, int nbytes)
{
  BIGBUF *bbp;
  int i, status;
  int i1, i2, i3;

  /* check parameters */
  if(nbufs > NBIGBUFFERS)
  {
    logMsg("bb_new_rol1: ERROR: nbufs=%d (must be <= %d\n",
      nbufs,NBIGBUFFERS,3,4,5,6);
    return(0);
  }

  /* allocate structure */
  bbp = (BIGBUF *) malloc(sizeof(BIGBUF));
  if(bbp == NULL)
  {
    logMsg("bb_new_rol1: ERROR: cannot allocate memory for 'bbp'\n",
      1,2,3,4,5,6);
    return(0);
  }
  else
  {
    bzero((void *)bbp,sizeof(BIGBUF));
  }

  /* allocate buffers */
  bbp->nbufs = nbufs;
  bbp->nbytes = nbytes;





  /* open dma memory driver */
  status = gefVmeOpen(&vmeHdl);
  if (status != GEF_STATUS_SUCCESS) 
  {
    printf("bb_new_rol1: gefVmeOpen failed: %x\n",status);
    return(0);
  }
  /* Turn off the Debug Flags, by default */
  gefVmeSetDebugFlags(vmeHdl,0x0);




  printf("bb_new_rol1: allocate %d chunks %d(0x%08x) bytes each\n",bbp->nbufs,bbp->nbytes,bbp->nbytes);
  for(i=0; i<bbp->nbufs; i++)
  {
    bbp->data[i] == NULL;
    bb_InitChunk(bbp, i);
    if(bbp->data[i] == NULL)
    {
      logMsg("bb_new_rol1: ERROR: buffer allocation FAILED\n",1,2,3,4,5,6);
      return(0);
    }
  }

  /* initialize semaphores */
  pthread_mutex_init(&bbp->read_lock, NULL);
  pthread_mutex_init(&bbp->write_lock, NULL);

  /* initialize index */
  bbp->write = 1;
  bbp->read = 0;

  /* reset cleanup condition */
  bbp->cleanup = 0;

  logMsg("bb_new_rol1: 'big' buffer created (addr=0x%08x, %d bufs, %d size)\n",
    bbp,bbp->nbufs,bbp->nbytes,4,5,6);

  return(bbp);
}


unsigned int *
bb_get_usermembase(BIGBUF **bbh)
{
  BIGBUF *bbp = *bbh;

  return(bbp->userMemBase[bbp->write]);
}

unsigned int *
bb_get_physmembase(BIGBUF **bbh)
{
  BIGBUF *bbp = *bbh;

  return(bbp->physMemBase[bbp->write]);
}

void
bb_dma_free()
{
  int i, status;

  for(i=0; i<n_dma_hdl; i++)
  {
    status = gefVmeFreeDmaBuf(dma_hdl[i]);
    if (status != GEF_SUCCESS)
    {
      printf("bb_delete1: ERROR: gefVmeFreeDmaBuf[%2d] returned %x\n",i,status);
      return;
    }
    else
	{
      printf("bb_delete1: gefVmeFreeDmaBuf[%2d] has been called\n",i);
	}
  }
}


void
bb_delete1(BIGBUF **bbh)
{
  BIGBUF *bbp = *bbh;
  int i, status;

printf("bb_delete1 0: 0x%08x\n",bbh);fflush(stdout);

  if((bbh == NULL)||(*bbh == NULL)) return;

  pthread_mutex_unlock(&bbp->read_lock);
  pthread_mutex_unlock(&bbp->write_lock);
  pthread_mutex_destroy(&bbp->read_lock);
  pthread_mutex_destroy(&bbp->write_lock);

printf("bb_delete1 5\n");fflush(stdout);

  /* free buffers 
  for(i=0; i<bbp->nbufs; i++)
  {
printf("bb_delete1 [%d]\n",i);fflush(stdout);
    free(bbp->data[i]);
printf("bb_delete1 (%d)\n",i);fflush(stdout);
  }
  */
#ifndef VXWORKS
  bb_dma_free();
#endif

  /* free 'bbp' structure */
printf("bb_delete1 6\n");fflush(stdout);
  free(bbp);
printf("bb_delete1 7\n");fflush(stdout);



  return;
}

#endif /* Linux_vme */

/* end of GEF staff */
/********************/
















/* */

void
bb_delete(BIGBUF **bbh)
{
  BIGBUF *bbp = *bbh;
  int i;

printf("bb_delete 0: 0x%08x\n",bbh);fflush(stdout);

  if((bbh == NULL)||(*bbh == NULL)) return;

#ifdef VXWORKS
printf("bb_delete 1\n");fflush(stdout);
  semGive(bbp->read_lock);
printf("bb_delete 2\n");fflush(stdout);
  semDelete(bbp->read_lock);
printf("bb_delete 3\n");fflush(stdout);
  semGive(bbp->write_lock);
printf("bb_delete 4\n");fflush(stdout);
  semDelete(bbp->write_lock);
#else
  pthread_mutex_unlock(&bbp->read_lock);
  pthread_mutex_unlock(&bbp->write_lock);
  pthread_mutex_destroy(&bbp->read_lock);
  pthread_mutex_destroy(&bbp->write_lock);
#endif

printf("bb_delete 5\n");fflush(stdout);

  /* free buffers */
  for(i=0; i<bbp->nbufs; i++)
  {
printf("bb_delete [%d]\n",i);fflush(stdout);
    free(bbp->data[i]);
printf("bb_delete (%d)\n",i);fflush(stdout);
  }

  /* free 'bbp' structure */
printf("bb_delete 6\n");fflush(stdout);
  free(bbp);
printf("bb_delete 7\n");fflush(stdout);
}


/* */

void
bb_cleanup(BIGBUF **bbh)
{
  BIGBUF *bbp = *bbh;
  int i;

printf("bb_cleanup 0: 0x%08x\n",bbh);fflush(stdout);

  if((bbh == NULL)||(*bbh == NULL)) return;

printf("bb_cleanup 1: 0x%08x\n",bbp);fflush(stdout);

  bbp->cleanup = 1;
printf("bb_cleanup 2\n");fflush(stdout);

  return;
}

void
bb_init(BIGBUF **bbh)
{
  BIGBUF *bbp = *bbh;
  int i;

  if((bbh == NULL)||(*bbh == NULL))
  {
    logMsg("bb_init: ERROR: bbh=0x%08x *bbh=0x%08x\n",bbh,*bbh,3,4,5,6);
    return;
  }

  /* initialize index */
  bbp->write = 1;
  bbp->read = 0;

  /* reset cleanup condition */
  bbp->cleanup = 0;

  /* need that ? */
  for(i=0; i<bbp->nbufs; i++)
  {
    memset(bbp->data[i],0,bbp->nbytes);
  }

  return;
}


/* write method: gets free buffer from the 'pool' for writing */
/* waits for available buffer and returns buffer pointer */

unsigned int *
bb_write_(BIGBUF **bbh, int flag)
{
  BIGBUF *bbp = *bbh;
  int icb;
  /*
printf("bb_write: bbh=0x%08x 0x%08x\n",bbh,*bbh);fflush(stdout);
printf("bb_write(in): bbp->write=%d\n",bbp->write);fflush(stdout);
  */
  if((bbh == NULL)||(*bbh == NULL))
  {
    logMsg("bb_write ERROR 1\n",1,2,3,4,5,6); 
    return(NULL);
  }

  if(bbp->cleanup)
  {
    logMsg("bb_write 1: return(NULL) on bbp->cleanup=%d condition\n",
      bbp->cleanup,2,3,4,5,6); 
    return(NULL);
  }

  /* try to take next (empty) buffer; if not available - sleep and try again */
  icb = (bbp->write + 1) % bbp->nbufs;
  /*
printf("bb_write: icb=%d, bbp->read=%d, bbp->write=%d bbp->nbufs=%d\n",
icb,bbp->read,bbp->write,bbp->nbufs);
  */
  
  while(icb == bbp->read)
  {
    if(bbp->cleanup)
    {
      logMsg("bb_write 2: return(NULL) on bbp->cleanup=%d condition\n",
        bbp->cleanup,2,3,4,5,6); 
      return(NULL);
    }
	/*
    logMsg("bb_write: icb=%d == bbp->read=%d (no bufs)\n",icb,bbp->read,3,4,5,6);
	*/
	logMsg("[0x%08x] bb_write: no bufs, back end too slow ..read=%d write=%d\n",
		   bbp,bbp->read,bbp->write);
    if(flag) return(NULL);
#ifdef VXWORKS
    taskDelay(60);
#else
    /*sleep(1);*/

#ifdef Linux
	usleep(1000); /*since bb_read (see comment there) had problem in
					similar place, put it here as well*/
#endif

#endif
  }
  

  /*logMsg("bb_write: lock (icb=%d)\n",icb,2,3,4,5,6);*/
  /* set 'write' pointer to the next buffer */
#ifdef VXWORKS
  semTake(bbp->write_lock, WAIT_FOREVER);
#else
  pthread_mutex_lock(&bbp->write_lock);
#endif
  bbp->write = icb;
#ifdef VXWORKS
  semGive(bbp->write_lock);
#else
  pthread_mutex_unlock(&bbp->write_lock);
#endif
  /*logMsg("bb_write: unlock (icb=%d)\n",icb,2,3,4,5,6);*/
/*
printf("bb_write(out): bbp->write=%d\n",bbp->write);fflush(stdout);
*/

  return(bbp->data[icb]);
}

unsigned int *
bb_write(BIGBUF **bbh)
{
  return(bb_write_(bbh,0));
}

unsigned int *
bb_write_nodelay(BIGBUF **bbh)
{
  return(bb_write_(bbh,1));
}





unsigned int *
bb_write_current(BIGBUF **bbh)
{
  BIGBUF *bbp = *bbh;
  int icb;
/*
printf("bb_write_current: bbh=0x%08x 0x%08x\n",bbh,*bbh);fflush(stdout);
*/
  if((bbh == NULL)||(*bbh == NULL)) return(NULL);

  icb = bbp->write;

/*
printf("bb_write_current(out): bbp->write=%d\n",bbp->write);fflush(stdout);
*/

  return(bbp->data[icb]);
}








/* read method: gets full buffer from the 'pool' for reading */
/* waits for available buffer and returns buffer pointer */

unsigned int *
bb_read(BIGBUF **bbh)
{
  BIGBUF *bbp = *bbh;
  int icb;
  /*
logMsg("bb_read(in): bbp->read=%d bbp->cleanup=%d\n",
bbp->read,bbp->cleanup,3,4,5,6);
  */
  
  /*
taskDelay(100*20);
printf("bb_read(in): bbp->read=%d bbp->cleanup=%d\n",bbp->read,bbp->cleanup);
printf("bb_read(in): bbp->read=%d bbp->cleanup=%d\n",bbp->read,bbp->cleanup);
printf("bb_read(in): bbp->read=%d bbp->cleanup=%d\n",bbp->read,bbp->cleanup);
printf("bb_read(in): bbp->read=%d bbp->cleanup=%d\n",bbp->read,bbp->cleanup);
printf("bb_read(in): bbp->read=%d bbp->cleanup=%d\n",bbp->read,bbp->cleanup);
  */

  if((bbh == NULL)||(*bbh == NULL)) return(NULL);
  if(bbp->cleanup)
  {
    logMsg("bb_read: return(NULL) on bbp->cleanup=%d condition\n",
      bbp->cleanup,2,3,4,5,6); 
    return(NULL);
  }

  /* try to get next (full) buffer; if not available - sleep */
  icb = (bbp->read + 1) % bbp->nbufs;
  
  while(icb == bbp->write)
  {
    if(bbp->cleanup)
    {
      logMsg("bb_read: return(NULL) on bbp->cleanup=%d condition\n",
        bbp->cleanup,2,3,4,5,6); 
      return(NULL);
	}
	/*
    logMsg("bb_read: icb=%d == bbp->write=%d (no bufs)\n",
      icb,bbp->write,3,4,5,6);
	*/
#ifdef VXWORKS
    taskDelay(60);
#else
    /*sleep(1);*/

#ifdef Linux
    /*sergey: does not work without some sleep if '-O2' use in compiler;
      even usleep(1) helps, although not clear how much system really
      sleeps; we'll set it to 1 millisec */
	usleep(1000);
#endif


#endif
  }

  /*logMsg("bb_read: lock (icb=%d)\n",icb,2,3,4,5,6);*/
  /* set 'read' pointer to the next buffer */
#ifdef VXWORKS
  semTake(bbp->read_lock, WAIT_FOREVER);
#else
  pthread_mutex_lock(&bbp->read_lock);
#endif
  bbp->read = icb;
#ifdef VXWORKS
  semGive(bbp->read_lock);
#else
  pthread_mutex_unlock(&bbp->read_lock);
#endif
  /*logMsg("bb_read: unlock (icb=%d)\n",icb,2,3,4,5,6);*/
/*
printf("bb_read(out): bbp->read=%d\n",bbp->read);fflush(stdout);
*/
  return(bbp->data[icb]);
}



/*************************************************************************
 remote access
*/

#ifdef VXWORKS

#ifdef PMCOFFSET




void
bb_cleanup_pci(BIGBUF **bbh)
{
  BIGBUF *bbp = *bbh;
  int i;

printf("bb_cleanup_pci 0: 0x%08x\n",bbh);fflush(stdout);

  bbp = ((int)(*bbh)) + PMCOFFSET;
  if((bbh == NULL)||(*bbh == NULL)) return;

printf("bb_cleanup_pci 1: 0x%08x\n",bbp);fflush(stdout);

  bbp->cleanup = 1;
printf("bb_cleanup_pci 2\n");fflush(stdout);

  return;
}



unsigned int *
bb_read_pci(BIGBUF **bbh)
{
  BIGBUF *bbp = *bbh;
  int icb, icbdma, status;
  static int lastword, lastind, dmainprogress;
  static unsigned int *buffer1 = NULL, *buffer2 = NULL;
  static unsigned int *buffer, *bufferdma;
  unsigned int *buftmp;

  /* first entry operations */
  if(buffer1 == NULL)
  {
    buffer1 = (unsigned int *) malloc(SEND_BUF_SIZE);
    if(buffer1 == NULL)
    {
      printf("bb_read_pci: ERROR: buffer1 allocation FAILED\n");
      return(0);
    }
    else
    {
      printf("bb_read_pci: buffer1 allocated at 0x%08x\n",buffer1);
    }

    buffer2 = (unsigned int *) malloc(SEND_BUF_SIZE);
    if(buffer2 == NULL)
    {
      printf("bb_read_pci: ERROR: buffer2 allocation FAILED\n");
      return(0);
    }
    else
    {
      printf("bb_read_pci: buffer2 allocated at 0x%08x\n",buffer2);
    }

    dmainprogress = 0;
    buffer = buffer1;
    bufferdma = buffer2;
  }

  /* set stricture pointer and do some checks */
  bbp = ((int)(*bbh)) + PMCOFFSET;
  if((bbh == NULL)||(*bbh == NULL))
  {
    printf("bb_read_pci: ERROR1: 0x%08x 0x%08x\n",bbh,*bbh);
    return(NULL);
  }
  if(bbp->cleanup)
  {
    printf("bb_read_pci: cleanup1: %d\n",bbp->cleanup);fflush(stdout);
    return(NULL);
  }

/*
{
unsigned *ptr;
ptr = ((int)(bbp->data[0])) + PMCOFFSET;
printf("1> 0x%08x 0x%08x\n",ptr,PMCOFFSET);
printf("0> 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7]);
printf("9> 0x%08x\n",ptr[786431]);
}
*/

  /* if DMA was not started last time, start it now; we will get buffer
  and start DMA for it */
  if(dmainprogress == 0)
  {
	/*	
printf("111: DMA was not started last time\n");
	*/
    /* geting next (full) buffer; if not available - sleep */
    icb = (bbp->read + 1) % bbp->nbufs;
    while(icb == bbp->write)
    {
	  /*
printf("111: sleep .. %d %d\n",icb,bbp->write);
	  */
      if(bbp->cleanup)
      {
        printf("bb_read_pci: cleanup2: %d\n",bbp->cleanup);fflush(stdout);
        return(NULL);
	  }
      taskDelay(100);
    }
    bbp->read = icb;

    /* start DMA */
    lastind = (bbp->nbytes>>2)-1;
{
unsigned int *ptr;
ptr = ((int)(bbp->data[icb])) + PMCOFFSET;
lastword = ptr[lastind];
/*lastword = bbp->data[icb][lastind];-wrong!!!*/
}
    if(lastword == 0) buffer[lastind] = 1;
    else              buffer[lastind] = 0;
	/*
printf("111: lastword=0x%08x, buffer[0x%08x]=0x%08x\n",
lastword,lastind,buffer[lastind]);
	*/
    status = usrPci2MemDmaStart(0,bbp->data[icb],buffer,bbp->nbytes);
    while(status != 0)
    {
      printf("DMA status = %d - try again\n",status);
      sysUsDelay(1000);
      status = usrPci2MemDmaStart(0,bbp->data[icb],buffer,bbp->nbytes);
    }
	/*
printf("111: DMA started from 0x%08x to 0x%08x, nbytes=%d\n",
bbp->data[icb],buffer,bbp->nbytes);
	*/
  }
  else /* swap buffer pointers so 'buffer' will be pointing to full one */
  {
	/*
printf("222: swap buffer pointer\n");
	*/
    buftmp = buffer;
    buffer = bufferdma;
    bufferdma = buftmp;
  }
/*
printf("222: usrDmaDone=%d\n",usrDmaDone(0));
*/
  /* now wait for DMA completion; we have to do it for DMA just started
  and for DMA started last time */
  if(buffer[lastind] != lastword)
  {
	/*
printf("bb_read_pci: wait for DMA completion ..\n");
	*/
    ;
  }
  while(buffer[lastind] != lastword)
  {
	
    sysUsDelay(5);
	/*
taskDelay(100);
printf("333: usrDmaDone=%d\n",usrDmaDone(0));
printf("333 -> lastword=0x%08x, buffer[0x%08x]=0x%08x (buffer=0x%08x)\n",
lastword,lastind,buffer[lastind],buffer);
	*/
  }
/*
printf("bb_read_pci: DMA completed\n");
*/

  /* we have one buffer now and that buffer will be returned; now lets
  try to get another buffer; if not available - do not wait, otherwise
  start DMA */
  icbdma = (bbp->read + 1) % bbp->nbufs;
  if(icbdma != bbp->write) /* buffer available, will start DMA */
  {
	/*
printf("444\n");
	*/
    bbp->read = icbdma;

    /* start DMA */
    lastind = (bbp->nbytes>>2)-1;
{
unsigned int *ptr;
ptr = ((int)(bbp->data[icbdma])) + PMCOFFSET;
lastword = ptr[lastind];
/*lastword = bbp->data[icbdma][lastind];-wrong!!!*/
}
    if(lastword == 0) bufferdma[lastind] = 1;
    else              bufferdma[lastind] = 0;
	/*
printf("444: lastword=0x%08x, bufferdma[0x%08x]=0x%08x\n",
lastword,lastind,bufferdma[lastind]);
	*/
    status = usrPci2MemDmaStart(0,bbp->data[icbdma],bufferdma,bbp->nbytes);
    while(status != 0)
    {
      printf("DMA status = %d - try again\n",status);
      sysUsDelay(1000);
      status = usrPci2MemDmaStart(0,bbp->data[icbdma],bufferdma,bbp->nbytes);
    }
	/*
printf("444: DMA started from 0x%08x to 0x%08x, nbytes=%d\n",
bbp->data[icbdma],bufferdma,bbp->nbytes);
	*/
    dmainprogress = 1;
  }
  else /* cannot get second buffer, so just return first one */
  {
	/*
printf("555\n");
	*/
    dmainprogress = 0;
  }




  return(buffer);




  /* copy 
  memcpy(buffer, (((int)bbp->data[icb])+PMCOFFSET), bbp->nbytes);
  return(buffer);
  */
}

#endif

#else

/* UNIX - to recognize reference */
void
bb_cleanup_pci(BIGBUF **bbh)
{
  return;
}

unsigned int *
bb_read_pci(BIGBUF **bbh)
{
  return(0);
}

#endif


#ifndef VXWORKS

/* UNIX-only: VXWORKS has those functions in BSPs */

#define UINT32 unsigned int

int
usrMem2MemDmaCopy(UINT32 chan, UINT32 *sourceAddr, UINT32 *destAddr,
                  UINT32 nbytes)
{
  memcpy((char *)destAddr, (char *)sourceAddr,nbytes);

  return(0);
}

int
usrMem2MemDmaStart(UINT32 chan, UINT32 *sourceAddr, UINT32 *destAddr,
                  UINT32 nbytes)
{
  memcpy((char *)destAddr, (char *)sourceAddr,nbytes);

  return(0);
}

int
usrDmaDone(UINT32 chan)
{
  return(0);
}

#endif

