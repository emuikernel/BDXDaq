
#define MULTIEVENT
/*
#define COMMONSTART
*/

/* fadcrol1.c - first readout list for VME crates with FADCs */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list FADCROL1 */
#define ROL_NAME__ "FADCROL1"

/* ts control */
#define TRIG_SUPV

/* name used by loader */
#define INIT_NAME fadcrol1__init

#include "rol.h"

/* vme readout */
#include "VME_source.h" /* POLLING_MODE for tir_triglib.h undefined here */


/* user code */

#include "uthbook.h"
#include "coda.h"
#include "tt.h"
#include "scaler7201.h"
#include "adc792.h"
#include "tdc890.h"

/* from fputil.h */
#undef  SYNC
#define SYNC 0x20000000


#undef DEBUG


/* main TI board */
#define TIRADR   0x0ed0
/*test setup (mv6100??) 
#define SISADR   0x80000000
*/
/*EC3 (mv5100)*/
#define SISADR   0x20000000


static char *rcname = "RC00";

/* user buffer pointers - all NULL at initialization */
extern int *userbuffers[NUSERBUFFERS];
/* pointer to TTS structures for current roc */
extern TTSPtr ttp;

/* time profiling: 0-OFF, 1-ON */
static int timeprofile = 1;

/* histogram area */
static UThisti histi[NHIST];
static int neventh = 0;


#define NBOARDS 22    /* maximum number of VME boards */


/* v1190 start */

#define MY_MAX_EVENT_LENGTH 3000 /* max words per v1190 board */
#define TDC_OFFSET 0
#define CH_OFFSET  0

static int nboards;
static unsigned int tdcadr[NBOARDS];
static unsigned int tdcbaseadr;
static int maxbytes = 0;
static int error_flag[NBOARDS];
/*
static int tmpgood[MY_MAX_EVENT_LENGTH];
static int tmpbad[MY_MAX_EVENT_LENGTH];
*/
#define NTICKS 1000 /* the number of ticks per second */

void
tsleep(int n)
{
  taskDelay ((sysClkRateGet() / NTICKS) * n);
}

/* v1190 end */








/* standart functions */


static void
__download()
{
  unsigned long offset, ii;

  rol->poll = 0;

  printf(">>>>>>>>>>>>>>>>>>>>>>>>>> ROCID = %d <<<<<<<<<<<<<<<<\n",rol->pid);

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");

  /* Must check address in TI module */
  /* TI uses A16 (0x29, 4 digits), not A24 */

  /* get global offset for A16 */
  sysBusToLocalAdrs(0x29,0,(long **)&offset);
  printf("A16 offset = 0x%08x\n",offset);

  tir[1] = (struct vme_tir *)(offset+TIRADR);


  /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,(long **)&offset);
  printf("A24 offset = 0x%08x\n",offset);


  /* sis3320 download start */
  sis3320Init(SISADR,0,0,0);

#ifdef COMMONSTART
  /* common start */
  printf("sis3320: Common Start Mode\n");
  sis3320SetCommonStart();
  sis3320SetGlobalWindow(1000,0);

  /*sis3320SetGlobalWindow(4000,0);*/         /*pulse in ch.464*/
  /*sis3320SetGlobalWindow(4000,1000);*/      /*pulse in ch.264*/
  /*sis3320SetGlobalWindow(4000,2000);*/      /*pulse in ch.64*/
  /*sis3320SetGlobalWindow(500,2000);*/       /*pulse in ch.64*/
#else
  /* common stop */
  printf("sis3320: Common Stop Mode\n");
  sis3320SetCommonStop();
  sis3320SetGlobalWindow(1000,-500);
  /*  sis3320SetGlobalWindow(1000,-1000); */
#endif

#ifdef MULTIEVENT
  sis3320EnableMultiEvent();
  sis3320SetMaxNofEvent(8);
  printf("sis3320: Multi Event Mode is enabled, Max_N_of_Events = 8\n");
#ifndef COMMONSTART
  sis3320SetStartDelay(10);
#endif
#else
  printf("sis3320: Single Event Mode\n");
#endif


  /* sis3320 download end */

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

  rol->poll = 0;

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  VME_INIT; /* init trig source VME */

  /* Register an async trigger source, (up to 32 sources) */
  /* CTRIGRSA(source,code,handler,done)*/
  /* link async trig source VME 1 to usrtrig and usrtrig_done */
  CTRIGRSA(VME, 1, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */

  rol->poll = 0;

  tttest("\npolar rol1:");

  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);


/* sis3320 start */

#ifdef MULTIEVENT
  sis3320ResetEvtCNT();
#endif

/* sis3320 end */


  if( (env=getenv("PROFILE")) != NULL )
  {
    if(*env == 'T')
    {
      timeprofile = 1;
      logMsg("rol1: time profiling is ON\n",1,2,3,4,5,6);
    }
    else
    {
      timeprofile = 0;
      logMsg("rol1: time profiling is OFF\n",1,2,3,4,5,6);
    }
  }

  if(timeprofile)
  {
    sprintf((char *)&histname[7],"%02d",rol->pid);
    uthbook1(histi, 1000+rol->pid, histname, 200, 0, 200);
  }
/*
  printf("tmpgood=0x%08x tmpbad=0x%08x\n",tmpgood,tmpbad);
*/
  printf("INFO: User Prestart Executed\n");

  /* from parser */
  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}       

static void
__pause()
{
  CDODISABLE(VME,1,0);

  logMsg("INFO: User Pause Executed\n",1,2,3,4,5,6);

  return;
}

static void
__go()
{  
  unsigned short value, value0, value1, array0[32], array1[32];
  int i, ii;

  logMsg("INFO: User Go ...\n",1,2,3,4,5,6);


/* sis3320 go start */

  sis3320Arm();
  /*sis3320Start();*/

/* sis3320 go end */

  rol->poll = 0;

  CDOENABLE(VME,1,0);

  return;
}



void
usrtrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  int *adrlen, *bufin, *bufout, i, ind, ind2, ncol, nrow, len, len1, len2;
  unsigned long tmp, *secondword, *jw, *buf, *dabufp1, *dabufp2;
  TIMER_VAR;
  unsigned short *buf2;
  unsigned short value;
  unsigned int nwords, nevent, nbcount, nbsubtract, buff[32];

  int ii, njjloops, blen, jj, nev, rlen, rlenbuf[NBOARDS], nevts, nwrds;
  unsigned long res, datascan, mymask=0xfff0;
  unsigned long tdcslot, tdcchan, tdcval, tdc14, tdcedge, tdceventcount;
  unsigned long tdceventid, tdcbunchid, tdcwordcount, tdcerrorflags;
  unsigned int *tdc, *tdchead, itdcbuf, tdcbuf[NBOARDS*MY_MAX_EVENT_LENGTH];

  rol->dabufp = (long *) 0;

  /*open event type EVTYPE of BT_UI4*/
  CEOPEN(EVTYPE, BT_UI4);

  jw = rol->dabufp;



  /***/

  /* at that moment only second CODA world defined  */
  /* first CODA world (length) undefined, so set it */
  /*jw[ILEN] = 1;*/ jw[-2] = 1;

  secondword = rol->dabufp - 1; /* pointer to the second CODA header word */

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
    logMsg("Illegal1: syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE,3,4,5,6);
  }
  else if((syncFlag==0)&&(EVTYPE==0))    /* illegal */
  {
    logMsg("Illegal2: syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE,3,4,5,6);
  }
  else if((syncFlag==1)&&(EVTYPE==0))    /* force_sync (scaler) events */
  {

/*
!!! we are geting here on End transition: syncFlag=1 EVTYPE=0 !!!
*/


    /* report histograms */
    if(timeprofile)
    {
      rol->dabufp += uth2bos(histi, 1000+rol->pid, jw);
    }



    /*logMsg("Force Sync: syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE,3,4,5,6);*/
    /*
    ncol=32;
    nrow=1;
    if( (ind = bosMopen_(jw,"HLS+",0,ncol,nrow)) > 0)
    {
      rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
    }
    if( (ind = bosMopen_(jw,"HLS+",1,ncol,nrow)) > 0)
    {
      rol->dabufp += bosMclose_(jw,ind,ncol,nrow);
    }
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

	
    if(timeprofile)
    {
      TIMER_NORMALIZE;
      TIMER_START;
    }
	

    /* sis3320 readout start */

#ifdef MULTIEVENT
    sis3320ReadEventMultiMode(SISADR,&itdcbuf,tdcbuf);
#else
    sis3320ReadEvent(SISADR,&itdcbuf,tdcbuf);
#endif

	/*		
logMsg("sis3320: len=%d -> 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
itdcbuf,tdcbuf[0],tdcbuf[1],tdcbuf[2],tdcbuf[3],tdcbuf[4]);
	*/

#ifndef MULTIEVENT
    sis3320Arm();
#endif
    /*sis3320Start();*/

    /* sis3320 readout end */


    /************************/
    /************************/
    /************************/

    /* open data bank */
    if((ind = bosMopen_((int *)jw, rcname, 0, 1, 0)) <=0)
    {
      logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
    }
    rol->dabufp += NHEAD;




    /*first event only: for(i=0; i<(itdcbuf/8); i++) *rol->dabufp++ = tdcbuf[i];*/
    for(i=0; i<itdcbuf; i++) *rol->dabufp++ = tdcbuf[i];


    blen = rol->dabufp - (int *)&jw[ind+1];
    if(blen == 0) /* no data - return pointer to the initial position */
    {
      rol->dabufp -= NHEAD;
	  /* NEED TO CLOSE BANK !!?? */
    }
    else if(blen >= (MAX_EVENT_LENGTH/4))
    {
      logMsg("1ERROR: event too long, blen=%d, ind=%d\n",blen,ind,0,0,0,0);
      logMsg(": %d %d 0x%x 0x%x\n",blen,ind,dabufp1,dabufp2,0,0);
      tsleep(1); /* 1 = 0.01 sec */
    }
    else if(bosMclose_((int *)jw,ind,1,blen) <= 0)
    {
      logMsg("2ERROR in bosMclose_ - space is not enough !!!\n",1,2,3,4,5,6);
    }


    /**********************/
    /**********************/
    /**********************/


goto a54321;
    /* for physics sync event, make sure all board buffers are empty */
    if(syncFlag==1)
    {
      int scan_flag;
      unsigned short slot, nev16;

      scan_flag = 0;
      for(ii=0; ii<nboards; ii++)
      {
        tdc890GetEventStored(tdcadr[ii], &nev16);
        nev = nev16;
        tdc890GetGeoAddress(tdcadr[ii], &slot);

        if(nev != 0) /* clear board if extra event */
        {
          logMsg("SYNC: ERROR: [%2d] slot=%2d nev=%d - clear\n",
            ii,slot,nev,4,5,6);
          tdc890Clear(tdcadr[ii]);
          scan_flag |= (1<<slot);
        }

        if(error_flag[slot] == 1) /* clear board if error flag was set */
        {
		  /*
          logMsg("SYNC: ERROR: [%2d] slot=%2d error_flag=%d - clear\n",
            ii,slot,error_flag[slot],4,5,6);
		  */
          tdc890Clear(tdcadr[ii]);
          error_flag[slot] = 0;
          scan_flag |= (1<<slot);
        }
      }
	  /*
	  if(scan_flag) logMsg("SYNC: scan_flag=0x%08x\n",scan_flag,2,3,4,5,6);
	  */
	  /*logMsg("PTRN ..\n",1,2,3,4,5,6);*/
      ind2 = bosMopen_((int *)jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = SYNC + scan_flag;
      rol->dabufp += bosMclose_((int *)jw,ind2,1,1);
	  /*logMsg("PTRN done.\n",1,2,3,4,5,6);*/
    }

a54321:
	
    if(timeprofile)
    {
      TIMER_STOP(100000,1000+rol->pid);
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

static void
__end()
{
  CDODISABLE(VME,1,0);


  /* sis3320 end start */

  sis3320Disarm();

  /* sis3320 end end */


  logMsg("INFO: User End Executed\n",1,2,3,4,5,6);

  return;
}

void
__done()
{
  /* from parser */
  poolEmpty = 0; /* global Done, Buffers have been freed */

  /* Acknowledge tir register */
  CDOACK(VME,1,0);

  return;
}  

static void
__status()
{
  return;
}  
