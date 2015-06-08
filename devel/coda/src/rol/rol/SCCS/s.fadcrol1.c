h08694
s 00001/00001/00569
d D 1.20 10/03/09 12:46:05 boiarino 20 19
c *** empty log message ***
e
s 00004/00000/00566
d D 1.19 10/03/09 12:30:02 boiarino 19 18
c *** empty log message ***
e
s 00001/00001/00565
d D 1.18 08/10/26 22:09:31 boiarino 18 17
c *** empty log message ***
e
s 00004/00003/00562
d D 1.17 07/02/20 14:58:11 sergpozd 17 16
c into Common Stop (window 1000,-500)
c 
e
s 00008/00005/00557
d D 1.16 07/02/20 14:52:08 boiarino 16 15
c *** empty log message ***
e
s 00003/00000/00559
d D 1.15 06/11/28 10:08:08 sergpozd 15 14
c *** empty log message ***
e
s 00035/00013/00524
d D 1.14 06/10/24 15:39:22 sergpozd 14 13
c *** empty log message ***
e
s 00007/00010/00530
d D 1.13 06/06/28 15:05:08 sergpozd 13 12
c sis3320EnableAutostart() removed
c all necessary in sis3320SetCommonStart()
c 
e
s 00003/00004/00537
d D 1.12 06/06/26 16:09:39 boiarino 12 11
c *** empty log message ***
e
s 00015/00004/00526
d D 1.11 06/06/26 16:01:52 boiarino 11 10
c start/stop 
e
s 00000/00007/00530
d D 1.10 06/06/24 23:57:45 boiarino 10 9
c *** empty log message ***
e
s 00004/00001/00533
d D 1.9 06/06/24 17:12:02 boiarino 9 8
c *** empty log message ***
e
s 00001/00001/00533
d D 1.8 06/06/23 18:57:12 boiarino 8 7
c *** empty log message ***
e
s 00020/00003/00514
d D 1.7 06/06/19 16:34:39 boiarino 7 6
c *** empty log message ***
e
s 00002/00002/00515
d D 1.6 06/06/09 12:44:12 boiarino 6 5
c *** empty log message ***
e
s 00001/00001/00516
d D 1.5 06/06/09 12:28:32 boiarino 5 4
c *** empty log message ***
e
s 00014/00136/00503
d D 1.4 06/06/08 17:38:51 boiarino 4 3
c *** empty log message ***
e
s 00007/00001/00632
d D 1.3 06/06/02 15:40:36 boiarino 3 2
c *** empty log message ***
e
s 00002/00000/00631
d D 1.2 06/05/26 16:14:31 sergpozd 2 1
c *** empty log message ***
e
s 00631/00000/00000
d D 1.1 06/05/26 16:10:46 boiarino 1 0
c date and time created 06/05/26 16:10:46 by boiarino
e
u
U
f e 0
t
T
I 11
D 16
/*
E 16
I 16

E 16
D 14
#define COMMONSTART
E 14
I 14
#define MULTIEVENT
E 14
D 16
*/
E 16
I 16
D 17

E 17
I 17
/*
E 17
E 16
I 14
#define COMMONSTART
I 17
*/
E 17
E 14
E 11
I 1

I 14
D 17

E 17
E 14
D 4

I 2


E 4
E 2
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
I 14
#undef  SYNC
E 14
#define SYNC 0x20000000


#undef DEBUG


/* main TI board */
#define TIRADR   0x0ed0
I 16
/*test setup (mv6100??) 
E 16
I 15
#define SISADR   0x80000000
D 16
/*
E 15
D 14
#define SISADR   0x10000000
E 14
I 14
#define SISADR   0x20000000
E 16
I 15
*/
I 16
/*EC3 (mv5100)*/
#define SISADR   0x20000000
E 16
E 15
E 14

I 16

E 16
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


D 18
#define NBOARDS 21    /* maximum number of VME boards */
E 18
I 18
#define NBOARDS 22    /* maximum number of VME boards */
E 18


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
D 14
  sysBusToLocalAdrs(0x29,0,&offset);
E 14
I 14
  sysBusToLocalAdrs(0x29,0,(long **)&offset);
E 14
  printf("A16 offset = 0x%08x\n",offset);

  tir[1] = (struct vme_tir *)(offset+TIRADR);


  /* get global offset for A24 */
D 14
  sysBusToLocalAdrs(0x39,0,&offset);
E 14
I 14
  sysBusToLocalAdrs(0x39,0,(long **)&offset);
E 14
  printf("A24 offset = 0x%08x\n",offset);


D 13
  /* sis3320 start */
E 13
I 13
  /* sis3320 download start */
E 13
D 11

E 11
  sis3320Init(SISADR,0,0,0);
I 3
D 4
  sis3320SetSampleLength(200);
E 4
E 3

I 11
#ifdef COMMONSTART
D 14

E 14
I 12
  /* common start */
I 14
  printf("sis3320: Common Start Mode\n");
E 14
E 12
  sis3320SetCommonStart();
I 12
  sis3320SetGlobalWindow(1000,0);
E 12

E 11
I 4
D 12
  /* common start */
E 12
  /*sis3320SetGlobalWindow(4000,0);*/         /*pulse in ch.464*/
  /*sis3320SetGlobalWindow(4000,1000);*/      /*pulse in ch.264*/
  /*sis3320SetGlobalWindow(4000,2000);*/      /*pulse in ch.64*/
  /*sis3320SetGlobalWindow(500,2000);*/       /*pulse in ch.64*/
D 14

E 14
I 7
D 11
  /*sis3320SetGlobalWindow(500,0);*/
E 11
I 11
D 12
  sis3320SetGlobalWindow(1000,0);
E 12
#else
D 12
  sis3320SetCommonStop();
E 11

E 12
E 7
  /* common stop */
I 14
  printf("sis3320: Common Stop Mode\n");
E 14
I 12
  sis3320SetCommonStop();
E 12
D 7
  sis3320SetGlobalWindow(2000,0);
E 7
I 7
D 8
  sis3320SetGlobalWindow(1000,-80);
E 8
I 8
D 11
  sis3320SetGlobalWindow(1000,-300);
E 11
I 11
D 17
  sis3320SetGlobalWindow(1000,-1000);
E 17
I 17
  sis3320SetGlobalWindow(1000,-500);
  /*  sis3320SetGlobalWindow(1000,-1000); */
E 17
#endif
E 11
E 8
E 7

I 14
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


E 14
D 11

E 11
E 4
D 13
  /* sis3320 end */
E 13
I 13
  /* sis3320 download end */
E 13

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

I 14
#ifdef MULTIEVENT
  sis3320ResetEvtCNT();
#endif
E 14

D 14

E 14
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
  if(__the_event__) WRITE_EVENT_;
  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}       

static void
__pause()
{
  CDODISABLE(VME,1,0);

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

  logMsg("INFO: User Go ...\n",1,2,3,4,5,6);


D 13
/* sis3320 start */
E 13
I 13
/* sis3320 go start */
E 13

I 11
D 13
#ifndef COMMONSTART
E 11
I 3
D 4
  sis3320Arm();
E 4
I 4
  sis3320EnableAutostart();
I 11
#endif
E 13
E 11
D 5
  /*sis3320Arm();*/
E 5
I 5
  sis3320Arm();
I 7
D 13
  /*sis3320Start();*/
E 13
I 13
D 14
  /* sis3320Start(); */
E 14
I 14
  /*sis3320Start();*/
E 14
E 13
E 7
E 5
E 4
E 3

D 3

E 3
D 13
/* sis3320 end */
E 13
I 13
/* sis3320 go end */
E 13

  rol->poll = 0;

  CDOENABLE(VME,1,0);

  /* from parser */
  if(__the_event__) WRITE_EVENT_;

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
I 19
  else if((syncFlag==0)&&(EVTYPE==15)) /* helicity strob events */
  {
    ;
  }
E 19
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
	

I 9
D 10
/*temporary*/
for(i=0; i<NBOARDS*MY_MAX_EVENT_LENGTH; i++) tdcbuf[i] = 0;
E 9

I 9

E 10
E 9
    /* sis3320 readout start */

I 11
D 14

E 14
I 14
#ifdef MULTIEVENT
    sis3320ReadEventMultiMode(SISADR,&itdcbuf,tdcbuf);
#else
E 14
E 11
    sis3320ReadEvent(SISADR,&itdcbuf,tdcbuf);
I 14
#endif
E 14
I 7
D 10
/*temporary !!!*/
D 9
tdcbuf[0] = *(unsigned int *)0x12000010;
E 9
I 9
/*tdcbuf[0] = *(unsigned int *)0x12000010;*/
E 9
/*temporary !!!*/
E 10
E 7
I 3

D 6
	/*	
E 6
I 6
D 7
		
E 7
I 7
	/*		
E 7
E 6
E 3
logMsg("sis3320: len=%d -> 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
itdcbuf,tdcbuf[0],tdcbuf[1],tdcbuf[2],tdcbuf[3],tdcbuf[4]);
I 3
D 6
	*/
E 6
I 6
D 7
	
E 7
I 7
	*/
E 7
E 6
E 3

I 14
#ifndef MULTIEVENT
E 14
I 3
    sis3320Arm();
I 14
#endif
E 14
I 7
    /*sis3320Start();*/
E 7

E 3
    /* sis3320 readout end */


D 4
goto a12345;
E 4
    /************************/
    /************************/
    /************************/
D 4
	/* start of v1190/v1290 */
E 4

    /* open data bank */
D 14
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
E 14
I 14
    if((ind = bosMopen_((int *)jw, rcname, 0, 1, 0)) <=0)
E 14
    {
      logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
    }
    rol->dabufp += NHEAD;

D 4
    /* analysis */
    itdcbuf = 0;
    if(tdcbaseadr) njjloops = 1;
    else           njjloops = nboards;
	/*
for(i=0; i<200; i++) tmpgood[i] = tdcbuf[i];
	*/
    for(jj=0; jj<njjloops; jj++)
    {
      rlen = rlenbuf[jj];
      if(rlen <= 0) continue;
E 4

I 7


E 7
D 4
      tdc = &tdcbuf[itdcbuf];
      itdcbuf += rlen;
E 4
I 4
D 16
    for(i=0; i<(itdcbuf/8); i++) *rol->dabufp++ = tdcbuf[i];
E 16
I 16
    /*first event only: for(i=0; i<(itdcbuf/8); i++) *rol->dabufp++ = tdcbuf[i];*/
    for(i=0; i<itdcbuf; i++) *rol->dabufp++ = tdcbuf[i];
E 16
E 4

D 4
      for(ii=0; ii<rlen; ii++)
      {
		
        if( ((tdc[ii]>>27)&0x1F) == 0)
        {
          nbcount ++;
          *rol->dabufp ++ = (tdc[ii] | (tdcslot<<27));
#ifdef DEBUG
          tdcedge = ((tdc[ii]>>26)&0x1);
          tdcchan = ((tdc[ii]>>19)&0x7F);
          tdcval = (tdc[ii]&0x7FFFF);
		  if(tdcchan<20)
          {
            logMsg(" DATA: tdc=%02d ch=%03d edge=%01d value=%08d\n",
              (int)tdc14,(int)tdcchan,(int)tdcedge,(int)tdcval,5,6);
		  }
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 1)
        {
          tdc14 = ((tdc[ii]>>24)&0x3);
          tdceventid = ((tdc[ii]>12)&0xFFF);
          tdcbunchid = (tdc[ii]&0xFFF);
#ifdef DEBUG
          logMsg(" HEAD: tdc=%02d event_id=%05d bunch_id=%05d\n",
            (int)tdc14,(int)tdceventid,(int)tdcbunchid,4,5,6);
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 3)
        {
          tdc14 = ((tdc[ii]>>24)&0x3);
          tdceventid = ((tdc[ii]>12)&0xFFF);
          tdcwordcount = (tdc[ii]&0xFFF);
#ifdef DEBUG
          logMsg(" EOB:  tdc=%02d event_id=%05d word_count=%05d\n",
            (int)tdc14,(int)tdceventid,(int)tdcwordcount,4,5,6);
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x18)
        {
#ifdef DEBUG
          logMsg(" FILLER:  tdc=%02d event_id=%05d word_count=%05d\n",
            (int)tdc14,(int)tdceventid,(int)tdcwordcount,4,5,6);
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 8)
        {
          nbcount = 1; /* counter for the number of output words from board */
          nbsubtract = 9; /* # words to subtract including errors */
          tdcslot = tdc[ii]&0x1F;
          tdceventcount = (tdc[ii]>>5)&0x3FFFFF;
          tdchead = (unsigned int *) rol->dabufp; /* remember pointer */
          *rol->dabufp ++ = tdcslot/*(tdcslot<<27)*/;
#ifdef DEBUG
          logMsg("GLOBAL HEADER: %d words, 0x%08x (slot=%d nevent=%d) -> header=0x%08x\n",
            rlen,tdc[ii],tdc[ii]&0x1F,(tdc[ii]>>5)&0xFFFFFF,*tdchead,6);
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x10)
        {
          /* put nwords in header; substract 4 TDC headers, 4 TDC EOBs,
          global trailer and error words if any */
          /**tdchead = (*tdchead) | (((tdc[ii]>>5)&0x3FFF) - nbsubtract);*/
          *tdchead |= ((((tdc[ii]>>5)&0x3FFF) - nbsubtract)<<5);
          if((((tdc[ii]>>5)&0x3FFF) - nbsubtract) != nbcount)
          {
            logMsg("ERROR: word counters: %d != %d (subtract=%d)\n",
			  (((tdc[ii]>>5)&0x3FFF) - nbsubtract),nbcount,nbsubtract,4,5,6);
			/*
for(i=0; i<200; i++) tmpbad[i] = tdcbuf[i];
logMsg("tmpgood=0x%08x tmpbad=0x%08x\n",tmpgood,tmpbad,3,4,5,6);
			*/
          }
	  
#ifdef DEBUG
          logMsg("GLOBAL TRAILER: 0x%08x (slot=%d nw=%d stat=%d) -> header=0x%08x\n",
            tdc[ii],tdc[ii]&0x1F,(tdc[ii]>>5)&0x3FFF,
            (tdc[ii]>>23)&0xF,*tdchead,6);
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 4)
        {
          nbsubtract ++;
          tdc14 = ((tdc[ii]>>24)&0x3);
          tdcerrorflags = (tdc[ii]&0x7FFF);
		  /*
#ifdef DEBUG
		  */
		  if(error_flag[tdcslot] == 0) /* print only once */
          {
            unsigned int ddd, lock, ntdc;
			
            logMsg(" ERR: event# %7d, slot# %2d, tdc# %02d, error_flags=0x%08x, err=0x%04x, lock=0x%04x\n",
              tdceventcount,tdcslot,(int)tdc14,(int)tdcerrorflags,ddd,lock);
			
		  }
          error_flag[tdcslot] = 1;
		  /*
#endif
		  */
        }
        else
        {
#ifdef DEBUG
          logMsg("ERROR: in TDC data format 0x%08x\n",
            (int)tdc[ii],2,3,4,5,6);
#endif
        }
E 4

D 4
      } /* loop over 'rlen' words */


    }


E 4
D 20
    blen = rol->dabufp - (long *)&jw[ind+1];
E 20
I 20
    blen = rol->dabufp - (int *)&jw[ind+1];
E 20
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
D 14
    else if(bosMclose_(jw,ind,1,blen) <= 0)
E 14
I 14
    else if(bosMclose_((int *)jw,ind,1,blen) <= 0)
E 14
    {
      logMsg("2ERROR in bosMclose_ - space is not enough !!!\n",1,2,3,4,5,6);
    }

D 4
	/* end of v1190/v1290 */
E 4
I 4

E 4
    /**********************/
    /**********************/
    /**********************/

D 4
a12345:
E 4

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
D 14
      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
E 14
I 14
      ind2 = bosMopen_((int *)jw,"PTRN",rol->pid,1,1);
E 14
      jw[ind2+1] = SYNC + scan_flag;
D 14
      rol->dabufp += bosMclose_(jw,ind2,1,1);
E 14
I 14
      rol->dabufp += bosMclose_((int *)jw,ind2,1,1);
E 14
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

I 7

D 13
  /* sis3320 readout start */
E 13
I 13
  /* sis3320 end start */
E 13

  sis3320Disarm();

D 13
  /* sis3320 readout end */
E 13
I 13
  /* sis3320 end end */
E 13


E 7
  logMsg("INFO: User End Executed\n",1,2,3,4,5,6);

  /* from parser */
  if(__the_event__) WRITE_EVENT_;

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
E 1
