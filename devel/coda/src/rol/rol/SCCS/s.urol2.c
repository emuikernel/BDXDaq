h29829
s 00242/00000/00000
d D 1.1 06/05/26 16:10:48 boiarino 1 0
c date and time created 06/05/26 16:10:48 by boiarino
e
u
U
f e 0
t
T
I 1

/* urol2.c - UNIX dummy second readout list, just copies data */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifndef VXWORKS
#include <sys/types.h>
#include <time.h>
#endif

#include "circbuf.h"


/*****************************/
/* former 'crl' control keys */

/* readout list UROL2 */
#define ROL_NAME__ "UROL2"

/* polling */
#define POLLING_MODE


/* need in one place in trigger_dispatch.h; must be undefined in rol1 */
#define EVENT_MODE



#define INIT_NAME urol2__init

#include "rol.h"

/* event readout */
#include "EVENT_source.h"

/************************/
/************************/


#include "coda.h"
#include "tt.h"

/*int TT_nev;*/

long decrement;     /* local decrement counter */
/*long nevent;*/ /* local event counter */
long mynev;



/* user routines */



void
rol2trig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  int ii;

  /*nevent++;*/

  if(rol->dabufp != NULL)
  {
    if(input_event__)
    {
      int i,len,tmp;

      /* trying step back ... */

      if( syncFlag==0 && ((rol->dabufpi[1] >> 16 ) & 0xffff) == 0 )
      {
        /*TT_nev = *(rol->nevents);*/
        printf("illegal event number %d !!! second coda header word = %8x\n",
          *(rol->nevents),rol->dabufpi[1]);
        decrement++;
        if(decrement > 1) printf("ILLEGAL EVENT NUMBER %d - trying to recover ...\n",decrement);
        rol->dabufp[0] = 0;
        (*(rol->nevents))--;
      }
      else
      {

mynev ++;
if(mynev == 256) mynev = 0;


        len = rol->dabufpi[0]+1;
		/*
printf("rol2: len=%d\n",rol->dabufpi[0]);
printf("rol2: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
rol->dabufpi[0],rol->dabufpi[1],rol->dabufpi[2],
rol->dabufpi[3],rol->dabufpi[4],rol->dabufpi[5]);
		*/
        for(ii=0; ii<len; ii++)
        {
          *rol->dabufp++ = rol->dabufpi[ii];
        }
		
		/*
        memcpy(rol->dabufp,rol->dabufpi,len<<2);
        rol->dabufp += len;
		*/
      }
    }
  }

  return;
}
  
void
rol2trig_done()
{
  return;
}  





static void
__download()
{

#ifdef POLLING_MODE
   rol->poll = 1;
#endif

  printf("INFO: User Download 2 Executed\n");

  return;
}

static void
__prestart()
{
  unsigned long jj, adc_id, sl;
  char *env;


  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  printf("INFO: Entering User Prestart 2\n");

  /*
  tttest("\ntrans_rol2:");
  */
  decrement = 0;
  /*
  TT_Prestart(rol->pid);
  */
  printf("MAX_EVENT_LENGTH = %d NWBOS = %d\n",MAX_EVENT_LENGTH,NWBOS);

  /* init trig source EVENT */
  EVENT_INIT;

  /* Register a sync trigger source (up to 32 sources) */
  CTRIGRSS(EVENT, 1, rol2trig, rol2trig_done); /* second arg=1 - what is that ? */

  /* Register a physics event type (up to 16 types) */
  CRTTYPE(1, EVENT, 1);
  CRTTYPE(2, EVENT, 1);
  CRTTYPE(3, EVENT, 1);
  CRTTYPE(4, EVENT, 1);
  CRTTYPE(5, EVENT, 1);
  CRTTYPE(6, EVENT, 1);
  CRTTYPE(7, EVENT, 1);
  CRTTYPE(8, EVENT, 1);
  CRTTYPE(9, EVENT, 1);

  rol->poll = 1;

  printf("INFO: User Prestart 2 executed\n");

  /* from parser (do we need that in rol2 ???) */
  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}

static void
__end()
{
  printf("INFO: User End 2 Executed\n");

  return;
}

static void
__pause()
{
  printf("INFO: User Pause 2 Executed\n");

  return;
}

static void
__go()
{
  char *env;

  /*
  if(rol->pid == 25 || rol->pid == 18 || rol->pid == 19 || rol->pid == 23 ||
     rol->pid == 30 || rol->pid == 24 || rol->pid == 27 || rol->pid == 0)
  {
    TT_VmeGo();
    printf("vme ----------------------------\n");
  }
  else
  {
    TT_Go();
    printf("regular -------------------------\n");
  }
  */

  mynev = 0;

  printf("INFO: User Go 2 Executed\n");

  return;
}

void
__done()
{
  /* from parser */
  poolEmpty = 0; /* global Done, Buffers have been freed */

  return;
}
  
static void
__status()
{
  return;
}  
E 1
