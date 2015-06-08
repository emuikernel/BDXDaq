h02299
s 00348/00000/00000
d D 1.1 10/03/09 12:47:33 boiarino 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
/* if event rate goes higher then 10kHz, with randon triggers we have wrong
slot number reported in GLOBAL HEADER and/or GLOBAL TRAILER words; to work
around that problem temporary patches were applied - until fixed (Sergey) */
#define SLOTWORKAROUND

/* fanuc1.c - first readout list for VME crates */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list VMEROL1 */
#define ROL_NAME__ "FANUC1"

/* ts control */
#define TRIG_SUPV

/* polling mode if needed */
#define POLLING_MODE

/* name used by loader */
#define INIT_NAME fanuc1__init

/* main TI board */
#define TIR_ADDR   0x0ed0

#include "rol.h"

void usrtrig(unsigned int EVTYPE, unsigned int EVSOURCE);
void usrtrig_done();

/* vme readout */
#include "FANUC_source.h"


/* user code */

#include "uthbook.h"
#include "coda.h"
#include "tt.h"
#include "scaler7201.h"
#include "adc792.h"
#include "tdc890.h"

/* from fputil.h */
#define SYNC 0x20000000


#undef DEBUG


static char rcname[5];


#define NBOARDS 22    /* maximum number of VME boards: we have 21 boards, but numbering starts from 1 */



/**********************/
/* standart functions */

static void
__download()
{
  unsigned int offset, ii;

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  printf(">>>>>>>>>>>>>>>>>>>>>>>>>> ROCID = %d <<<<<<<<<<<<<<<<\n",rol->pid);

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");

  logMsg("INFO: User Download Executed\n",1,2,3,4,5,6);
}


static void
__prestart()
{
  char *env;
  char *histname = "ROL1 RC00";
  unsigned short value, value0, value1, array0[32], array1[32], data16;
  int i, ii, ifull;
  short buf;
  unsigned short slot, channel, pattern[8];

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  /* initialize OS windows and TI board */
  CDOINIT(FANUC,TIR_SOURCE);

  /* renamed to CDOINIT
  FANUC_INIT;
  */

#ifdef POLLING_MODE
  CTRIGRSS(FANUC, TIR_SOURCE, usrtrig, usrtrig_done);
#else
  CTRIGRSA(FANUC, TIR_SOURCE, usrtrig, usrtrig_done);
#endif

  sprintf(rcname,"RC%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);



  /* USER code here */




  printf("INFO: User Prestart Executed\n");fflush(stdout);

  /* from parser */
  if(__the_event__) WRITE_EVENT_;

  printf("INFO: Write Event Executed\n");fflush(stdout);

  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}       

static void
__pause()
{
  CDODISABLE(FANUC,TIR_SOURCE,0);

  logMsg("INFO: User Pause Executed\n",1,2,3,4,5,6);

  /* from parser */
  if(__the_event__) WRITE_EVENT_;

  return;
}

static void
__go()
{  
  unsigned short value, value0, value1, array0[32], array1[32];
  int i, ii;
  int extra;

  logMsg("INFO: User Go ...\n",1,2,3,4,5,6);

  CDOENABLE(FANUC,TIR_SOURCE,0); /* bryan has (,1,1) ... */

  /* from parser */
  if(__the_event__) WRITE_EVENT_;

  return;
}

static void
__end()
{
  CDODISABLE(FANUC,TIR_SOURCE,0);

  logMsg("INFO: User End Executed\n",1,2,3,4,5,6);

  /* from parser */
  if(__the_event__) WRITE_EVENT_;

  return;
}


void
usrtrig(unsigned int EVTYPE, unsigned int EVSOURCE)
{
  int EVENT_LENGTH;
  int *jw;
  int *adrlen, *bufin, *bufout, i, ind, ind2, ncol, nrow, len, len1, len2;
  unsigned int tmp, *buf, *dabufp1, *dabufp2;
  unsigned short *buf2;
  unsigned short value;
  unsigned int nwords, nevent, nbcount, nbsubtract, buff[32];
  unsigned short level;

  int ii, iii, njjloops, blen, jj, nev, rlen, rlenbuf[NBOARDS], nevts, nwrds;
  unsigned int res, datascan, mymask=0xfff0;
  unsigned int tdcslot, tdcchan, tdcval, tdc14, tdcedge, tdceventcount;
  unsigned int tdceventid, tdcbunchid, tdcwordcount, tdcerrorflags;
  unsigned int *tdc, *tdchead, itdcbuf;
  int nheaders, ntrailers;
#ifdef SLOTWORKAROUND
  unsigned int tdcslot_h, tdcslot_t, remember_h;
#endif

  /*  
  printf("EVTYPE=%d syncFlag=%d\n",EVTYPE,syncFlag);
  */

  rol->dabufp = (int *) 0;

  /*open event type EVTYPE of BT_UI4*/
  CEOPEN(EVTYPE, BT_UI4);

  jw = rol->dabufp; 

  /***/

  /* at that moment only second CODA world defined  */
  /* first CODA world (length) undefined, so set it */
  /*jw[ILEN] = 1;*/ jw[-2] = 1;


  /* for sync events do nothing          */
  /* for physics events send HLS scalers */

/* if want physics_sync events in external mode
if( (*(rol->nevents))%1000)
{
  syncFlag=0;
}
else
{
  syncFlag=1;
  rol->dabufp += bosMmsg_(jw,"INFO",rol->pid,"physics_sync event");
}
*/

  if((syncFlag<0)||(syncFlag>1))         /* illegal */
  {
    printf("Illegal1: syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE);
  }
  else if((syncFlag==0)&&(EVTYPE==0))    /* illegal */
  {
    printf("Illegal2: syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE);
  }
  else if((syncFlag==1)&&(EVTYPE==0))    /* force_sync (scaler) events */
  {

/*
!!! we are geting here on End transition: syncFlag=1 EVTYPE=0 !!!
*/
  }
  else if((syncFlag==0)&&(EVTYPE==15)) /* helicity strob events */
  {
    ;
  }
  else           /* physics and physics_sync events */
  {

/*
!!! very first event in test setup: syncFlag=0 EVTYPE=14
even before pulser is started !!!
all following events are: syncFlag=0 EVTYPE=11
*/

	/*
    logMsg("physics or physics_sync: syncFlag=%d EVTYPE=%d\n",
      syncFlag,EVTYPE,3,4,5,6);
	*/

    /* open data bank */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      printf("bosMopen_ Error: %d\n",ind);
    }
    rol->dabufp += NHEAD;

    for(iii=0; iii<200; iii++) *rol->dabufp ++ = (0x1000+iii);

	/* close bank */
    blen = rol->dabufp - (int *)&jw[ind+1];
    if(blen == 0) /* no data - return pointer to the initial position */
    {
      rol->dabufp -= NHEAD;
	  /* NEED TO CLOSE BANK !!?? */
    }
    else if(blen >= (MAX_EVENT_LENGTH/4))
    {
      printf("ERROR: event too long, blen=%d, ind=%d\n",blen,ind);
      printf(": %d %d 0x%x 0x%x\n",blen,ind,dabufp1,dabufp2);
      sleep(1);
    }
    else if(bosMclose_(jw,ind,1,blen) <= 0)
    {
      printf("ERROR in bosMclose_ - space is not enough !!!\n");
    }



    /* for physics sync event, make sure all board buffers are empty */
    if(syncFlag==1)
    {
      int scan_flag = 0;
      unsigned short slot, nev16;

	  /*
	  if(scan_flag) logMsg("SYNC: scan_flag=0x%08x\n",scan_flag,2,3,4,5,6);
	  */
	  /*logMsg("PTRN ..\n",1,2,3,4,5,6);*/
      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = SYNC + scan_flag;
      rol->dabufp += bosMclose_(jw,ind2,1,1);
	  /*logMsg("PTRN done.\n",1,2,3,4,5,6);*/
    }

  }

  /* close event */
  CECLOSE;

  return;
}

void
usrtrig_done()
{
  return;
}


void
__done()
{
  /* from parser */
  poolEmpty = 0; /* global Done, Buffers have been freed */

  /* Acknowledge tir register */
  CDOACK(FANUC,TIR_SOURCE,0);

  return;
}  

static void
__status()
{
  return;
}  
E 1
