h21652
s 00000/00016/00219
d D 1.2 10/03/09 12:45:38 boiarino 2 1
c *** empty log message ***
e
s 00235/00000/00000
d D 1.1 06/05/26 16:10:48 boiarino 1 0
c date and time created 06/05/26 16:10:48 by boiarino
e
u
U
f e 0
t
T
I 1

/* urol1.c - UNIX first readout list (polling mode) */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifndef VXWORKS
#include <sys/types.h>
#include <time.h>
#endif

#include "circbuf.h"

void davetrig(unsigned long, unsigned long);
void davetrig_done();

/*****************************/
/* former 'crl' control keys */

/* readout list UROL1 */
#define ROL_NAME__ "UROL1"

/* polling */
#define POLLING_MODE

D 2

/* need in one place in trigger_dispatch.h; must be undefined in rol1 */
#undef EVENT_MODE


E 2
/*???*/
/* 0: External Mode   1: Trigger Supervisor Mode */
#define TS_MODE  0
#define IRQ_SOURCE  0x10


#define INIT_NAME urol1__init

#include "rol.h"

/* test readout */
#include "TEST_source.h"

/************************/
/************************/

#include "coda.h"
#include "tt.h"

static char *rcname = "RC00";

long decrement;     /* local decrement counter */
extern long nevent; /* local event counter */
long mynev;


static void
__download()
{
  int poolsize;

#ifdef POLLING_MODE
  rol->poll = 1;
#endif



  printf(">>>>>>>>>>>>>>>>>>>>>>>>>> ROCID = %d <<<<<<<<<<<<<<<<\n",rol->pid);

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");






  printf("INFO: User Download 1 Executed\n");

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

  /* init trig source TEST */
  TEST_INIT;

  /* Register a sync trigger source (up to 32 sources) */
  CTRIGRSS(TEST, 1, davetrig, davetrig_done); /* second arg=1 - what is that ? */

D 2
  /* Register a physics event type (up to 16 types) */
  CRTTYPE(1, TEST, 1);
  CRTTYPE(2, TEST, 1);
  CRTTYPE(3, TEST, 1);
  CRTTYPE(4, TEST, 1);
  CRTTYPE(5, TEST, 1);
  CRTTYPE(6, TEST, 1);
  CRTTYPE(7, TEST, 1);
  CRTTYPE(8, TEST, 1);
  CRTTYPE(9, TEST, 1);

E 2
  rol->poll = 1;


  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);


  printf("INFO: User Prestart 1 executed\n");

  /* from parser (do we need that in rol2 ???) */
  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}

static void
__pause()
{
  CDODISABLE(TEST,1,0);

  printf("INFO: User Pause 1 Executed\n");

  return;
}

static void
__go()
{
  char *env;

  CDOENABLE(TEST,1,1);

  mynev = 0;

  printf("INFO: User Go 1 Executed\n");

  return;
}

static void
__end()
{
  int ii, total_count, rem_count;

  CDODISABLE(TEST,1,0);

  printf("INFO: User End 1 Executed\n");

  return;
}

void
davetrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  int ii, len, len1, type, lock_key, *tmp;
  int *adrlen, *bufin, *bufout, i, ind, ind2, ncol, nrow, len2;
  unsigned long *secondword, *jw, *buf, *dabufp1, *dabufp2;
  DANODE *outEvent;

  
sleep(1);
  

  rol->dabufp = (long *) 0;
  /*open event type EVTYPE of BT_UI4*/
  CEOPEN(EVTYPE, BT_UI4);

  jw = rol->dabufp;
  jw[-2] = 1;
  secondword = rol->dabufp - 1; /* pointer to the second CODA header word */




    /* open data bank */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      printf("bosMopen_ Error: %d\n",ind);
    }
    rol->dabufp += NHEAD;



    len = 4;
    for(ii=0; ii<len; ii++)
    {
      *rol->dabufp++ = ii;
    }


    if(bosMclose_(jw,ind,1,len) == 0)
    {
      printf("ERROR in bosMclose_ - space is not enough !!!\n");
    }


  CECLOSE;

  return;
}
  
void
davetrig_done()
{
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
