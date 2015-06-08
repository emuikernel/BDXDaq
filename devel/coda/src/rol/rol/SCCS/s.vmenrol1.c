h30774
s 01468/00000/00000
d D 1.1 11/09/28 16:29:18 boiarino 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
/*
#define USE_V1720
*/
/*
#define USE_FADC250
*/

/* if event rate goes higher then 10kHz, with randon triggers we have wrong
slot number reported in GLOBAL HEADER and/or GLOBAL TRAILER words; to work
around that problem temporary patches were applied - until fixed (Sergey) */
#define SLOTWORKAROUND

/* vmenrol1.c - first readout list for VME crates using new Bryan Mossit's library (Feb 2011) */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list VMENROL1 */
#define ROL_NAME__ "VMENROL1"

/* ts control */
#ifndef STANDALONE
#define TRIG_SUPV
#endif

/* polling mode if needed */

#define POLLING_MODE


/* name used by loader */
#ifdef STANDALONE
#define INIT_NAME vmenrol1_standalone__init
#else
#define INIT_NAME vmenrol1__init
#endif

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
#include "tdc1190.h"

#ifdef USE_V1720
#include "v1720.h"
#endif
#ifdef USE_FADC250
#include "fadcLib.h"
int FA_SLOT;
int fadc_offset;
extern int fadcID[20];
#define FADC_ADDR 0x110000
#endif

/* from fputil.h */
#define SYNC 0x20000000


#undef DEBUG


/* main TI board */
#define TIRADR   0x0ed0

static char *rcname = "RC00";

/* user buffer pointers - all NULL at initialization */
extern int *userbuffers[NUSERBUFFERS];
/* pointer to TTS structures for current roc */
extern TTSPtr ttp;

/* time profiling: 0-OFF, 1-ON */
static int timeprofile = 0;

/* histogram area */
static UThisti histi[NHIST];
static int neventh = 0;


#define NBOARDS 22    /* maximum number of VME boards: we have 21 boards, but numbering starts from 1 */


/* v1190 start */

#define MY_MAX_EVENT_LENGTH 3000/*3200*/ /* max words per v1190 board */
#define TDC_OFFSET 0
#define CH_OFFSET  0

static int nboards, nadcs;
static int maxbytes = 0;
static int error_flag[NBOARDS];
#ifdef SLOTWORKAROUND
static int slotnums[NBOARDS];
#endif
static unsigned int tdcbuftmp[NBOARDS*MY_MAX_EVENT_LENGTH+16];
static unsigned int *tdcbuf;
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

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  printf(">>>>>>>>>>>>>>>>>>>>>>>>>> ROCID = %d <<<<<<<<<<<<<<<<\n",rol->pid);

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");

  /* Must check address in TI module */
  /* TI uses A16 (0x29, 4 digits), not A24 */

  /* get global offset for A16 */
  sysBusToLocalAdrs(0x29,0,&offset);
  printf("A16 offset = 0x%08x\n",offset);

  tir[1] = (struct vme_tir *)(offset+TIRADR);


  /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&offset);
  printf("A24 offset = 0x%08x\n",offset);


  /* DMA setup */
  usrVmeDmaInit();
#ifdef USE_FADC250
  usrVmeDmaSetConfig(2,5,1); /* A32,2eSST,267MB/s - fadc250 */
#else
  usrVmeDmaSetConfig(2,5,0); /* A32,2eSST,160MB/s - v1190 */
#endif
  /*usrVmeDmaSetConfig(2,3,0);*/ /* A32,MBLT */

/* v1190 start */

  /* check 'tdcbuftmp' alignment and set 'tdcbuf' to 16-byte boundary */
  tdcbuf = &tdcbuftmp[0];
  printf("alignment0: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);
  if( (((int)tdcbuf)&7) == 0xc )
  {
    tdcbuf += 1;
    printf("alignment1: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);
  }
  else if( (((int)tdcbuf)&7) == 0x8 )
  {
    tdcbuf += 2;
    printf("alignment2: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);
  }
  else if( (((int)tdcbuf)&7) == 0x4 )
  {
    tdcbuf += 3;
    printf("alignment3: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);
  }
  printf("alignment: tdcbuftmp=0x%08x, tdcbuf=0x%08x\n",tdcbuftmp,tdcbuf);


  /* have to call tdc1190Init() before any other calls */
  nboards = tdc1190Init(0x08210000,0x10000,20,0);
  if(nboards>0)
  {
#ifdef STANDALONE
    tdc1190SetDefaultWindowWidth(1500);
    tdc1190SetDefaultWindowOffset(-600);
#else
    tdc1190SetDefaultWindowWidth(1500);
	tdc1190SetDefaultWindowOffset(-3900);
#endif

    tdc1190Config(1); /*0-BERR, 1-FIFO*/
  }

/* v1190 end */

  logMsg("INFO: User Download Executed\n",1,2,3,4,5,6);
}


static void
__prestart()
{
  char *env;
  char *histname = "ROL1 RC00";
  unsigned short value, value0, value1, array0[32], array1[32], data16, iflag;
  int i, ii, jj, ifull, istat;
  short buf;
  unsigned short slot, channel, pattern[8];
#ifdef USE_V1720
  int ADCmask[2]; 
  int ADCoffset[2][V1720_MAX_CHANNELS];
  int PostTriggerValue[2];
#endif

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  VME_INIT; /* init trig source VME */

  /* Register an async trigger source, (up to 32 sources) */
  /* CTRIGRSA(source,code,handler,done)*/
  /* link async trig source VME 1 to usrtrig and usrtrig_done */
#ifdef POLLING_MODE
  CTRIGRSS(VME, TIR_SOURCE, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */
#else
  CTRIGRSA(VME, TIR_SOURCE, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */
#endif

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  tttest("\npolar rol1:");

  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);


#ifdef SLOTWORKAROUND
  for(ii=0; ii<nboards; ii++)
  {
    slot = tdc1190GetGeoAddress(ii);
	slotnums[ii] = slot;
    printf("[%d] slot %d\n",ii,slotnums[ii]);
  }
#endif



  /* disable v1190 channels undescribed in TT */
  /*
  if(ttp!=NULL)
  {
    for(ii=0; ii<nboards; ii++)
    {
      slot = tdc1190GetGeoAddress(ii);

      tdc1190EnableAllChannels(ii);
      for(channel=1; channel<128; channel++)
      {
        if(ttp->idnr[slot][channel] <= 0)
        {
          tdc1190DisableChannel(ii, channel);
        }
      }

      tdc1190GetChannels(ii, pattern);
      printf("TDC connectors:    DL   DR   CL   CR   BL   BR   AL   AR\n");
      printf("channels pattern: %04x %04x %04x %04x %04x %04x %04x %04x\n",
        pattern[7],pattern[6],pattern[5],pattern[4],
        pattern[3],pattern[2],pattern[1],pattern[0]);
    }
  }
  */



/*TODO !!! (was default inside library, now must be here)*/
/*1-trailing edge, 2-leading, 3-both
tdc890SetEdgeDetectionConfig(0x90260000, 0x3);
*/
if(rol->pid == 0)
{
  /*1-trailing edge, 2-leading, 3-both*/

  for(ii=0; ii<nboards; ii++)
  {
    /*if(ii<4)*/ tdc1190SetEdgeDetectionConfig(ii, 0x3);
  }
}



#ifdef USE_V1720
  printf("v1720 init\n");
  printf("v1720 init\n");
  printf("v1720 init\n");
  printf("v1720 init\n");
  printf("v1720 init\n");

  ADCmask[0]=0xff; /*First board, 0xff=11111111*/
  ADCmask[1]=0x01; /*Second board */

  ADCoffset[0][0]=0; /*First Board */
  ADCoffset[0][1]=0;
  ADCoffset[0][2]=0;
  ADCoffset[0][3]=0;
  ADCoffset[0][4]=0;
  ADCoffset[0][5]=0;
  ADCoffset[0][6]=0;
  ADCoffset[0][7]=0;

  ADCoffset[1][0]=0; /*Second Board */
  ADCoffset[1][1]=0;
  ADCoffset[1][2]=0;
  ADCoffset[1][3]=0;
  ADCoffset[1][4]=0;
  ADCoffset[1][5]=0;
  ADCoffset[1][6]=0;
  ADCoffset[1][7]=0;

  PostTriggerValue[0]=16; /*First Board*/
  PostTriggerValue[1]=16; /*Second Board*/

  nadcs = v1720Init(0x09110000,0x10000,20,0);
  v1720Config(1);
  /*
  for(ii=0; ii<nadcs; ii++)
  {
    v1720SetChannelEnableMask(ii, 0x1);
 	v1720SetBLTRange(ii,0);
  }
  */

  for(ii=0; ii<nadcs; ii++)/*Put here V1720Settings Andrea*/ 
  {
    v1720SetBLTRange(ii,0);
    if(nadcs==2)
    {
	  printf("I have 2 ADCs\n");
	  v1720SetChannelEnableMask(ii,ADCmask[ii]); 
	  v1720SetPostTrigger(ii, PostTriggerValue[ii]);
	  v1720SetBuffer(ii, 0x0A); /* 1024 buffers 1K samples max size 0X0A*/
	  /* v1720SetBuffer(ii, 0x06);*//* 64 buffers 1K samples max size 0X0A*/
	  for(jj=0; jj<V1720_MAX_CHANNELS; jj++)
      {
	    v1720SetChannelDACOffset(ii,jj,ADCoffset[ii][jj]);
	  }
    }
    else
    {
      printf("I DON'T have 2 ADCs\n");
      v1720SetChannelEnableMask(ii,/*0xff*/0x01); 
      v1720SetPostTrigger(ii,0);    
      for(jj=0; jj<V1720_MAX_CHANNELS; jj++)
      {
	    v1720SetChannelDACOffset(ii,jj,0);
      }
    }
  }

  printf("v1720 done\n");
  printf("v1720 done\n");
  printf("v1720 done\n");
  printf("v1720 done\n");
  printf("v1720 done\n");
#endif


#ifdef USE_FADC250

  /* Program/Init FADC Modules Here */
  iflag = 0/*xea00*/; /* SDC Board address */
  /*iflag |= 1<<0;*/  /* Front panel sync-reset */
  iflag |= 1<<1;  /* Front Panel Input trigger source */
  iflag |= 0<<4;  /* Internal 250MHz Clock source */
  /*   iflag |= 1<<4;  /\* Front Panel 250MHz Clock source *\/ */
  printf("iflag = 0x%x\n",iflag);

/*   fadcA32Base = 0x08000000; */

  faInit(FADC_ADDR,0x0,1,iflag);
  FA_SLOT = fadcID[0];

  /*      Setup FADC Programming */
  faSetBlockLevel(FA_SLOT,1);
  /*  for Block Reads */
  faEnableBusError(FA_SLOT);
  /*  for Single Cycle Reads */
  /*       faDisableBusError(FA_SLOT); */
  
  /*  Set All channel thresholds to 10 */
  faSetThreshold(FA_SLOT,10,0xffff);

  /*3000 gives about 650, 3200 gives 170-400, 3300 gives 50-250, 4000 kills all */
  fadc_offset = 3200/*0*/;
  faSetDAC(FA_SLOT,fadc_offset,0xffff);
    
  /*  Setup option 1 processing - RAW Window Data     <-- */
  /*        option 2            - RAW Pulse Data */
  /*        option 3            - Integral Pulse Data */
  /*  Setup 200 nsec latency (PL  = 50)  */
  /*  Setup  80 nsec Window  (PTW = 20) */
  /*  Setup Pulse widths of 36ns (NSB(3)+NSA(6) = 9)  */
  /*  Setup up to 1 pulse processed */
  /*  Setup for both ADC banks(0 - all channels 0-15) */
  /*faSetProcMode(FA_SLOT,1,50,20,3,6,1,0);*/

  /* window: position 800=3200ns, size 50=200ns*/
  /*faSetProcMode(FA_SLOT,1,800,50,3,6,1,0);*/
  faSetProcMode(FA_SLOT,1,850,100,3,6,1,0);

  /*faClear(FA_SLOT);*/
  faStatus(FA_SLOT,0);
  faPrintThreshold(FA_SLOT);
  faPrintDAC(FA_SLOT);

  /*for FANOUT
  faSDC_Config(1,0);
  faSDC_Status(0);
  */


  /* software trigger test (slot=4)

  faInit(0x110000,0x0,1,0)
  faSetBlockLevel(4,1)
  faEnableBusError(4)
  faSetThreshold(4,0,0xffff)
  faSetProcMode(4,1,50,20,3,6,1,0)
    faClear(4)
  faStatus

  faEnable
  faTrig
  faStatus

   */


#endif



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
  int extra;

  logMsg("INFO: User Go ...\n",1,2,3,4,5,6);

  for(ii=0; ii<nboards; ii++) tdc1190Clear(ii);
  for(ii=0; ii<NBOARDS; ii++) error_flag[ii] = 0;


#ifdef USE_V1720
  for(ii=0; ii<nadcs; ii++)
  {
    v1720SetAcquisition(ii, 0, 1, 0);
 	v1720SetBLTRange(ii,1);
  }
#endif

#ifdef USE_FADC250
  /*  Enable FADC */
  faEnable(FA_SLOT,0,0);

  taskDelay(1);
  
  /*  Send Sync Reset to FADC */
  faSync(FA_SLOT);
	/*faSDC_Sync();*/
#endif

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

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
  unsigned short level;
  int previousIndex, kk;

  int ii, iii, njjloops, blen, jj, nev, rlen, rlenbuf[NBOARDS], nevts, nwrds;
  unsigned long res, datascan, mymask=0xfff0;
  unsigned long tdcslot, tdcchan, tdcval, tdc14, tdcedge, tdceventcount;
  unsigned long tdceventid, tdcbunchid, tdcwordcount, tdcerrorflags;
  unsigned int *tdc, *tdchead, itdcbuf;
  int nheaders, ntrailers;
  int zerochan[22];
#ifdef SLOTWORKAROUND
  unsigned long tdcslot_h, tdcslot_t, remember_h;
#endif

  rol->dabufp = (long *) 0;

  /*open event type EVTYPE of BT_UI4*/
  CEOPEN(EVTYPE, BT_UI4);

  jw = rol->dabufp;
  /*
logMsg("000\n",1,2,3,4,5,6);
  */

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


#ifdef USE_V1720

    /* open data bank */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0) /* bosMopen_(int *jw, char *name, int nr, int ncol, int nrow);*/
    {
      logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
    }
    rol->dabufp += NHEAD;

	if(nboards>0)
	{
      tdc1190ReadStart(tdcbuf, rlenbuf);
      for(ii=0; ii<rlenbuf[0]; ii++) *rol->dabufp++ = tdcbuf[ii];
	}

    if(timeprofile)
    {
      TIMER_NORMALIZE;
	  TIMER_START;
    }



    v1720ReadStart(tdcbuf, rlenbuf);
    /*logMsg("adc=%d\n",rlenbuf[0],2,3,4,5,6);*/

    if(timeprofile)
    {
      TIMER_STOP(100000,1000+rol->pid);
    }

    
    previousIndex=0;
    for(kk=0; kk<nadcs; kk++)
    {
      /*printf("This is ADC %i of %i total adcs \n",kk,nadcs); */
      for(ii=0; ii<rlenbuf[kk]; ii++)
      {
	    *rol->dabufp++ = tdcbuf[ii+previousIndex];
      }
      previousIndex=rlenbuf[kk]+previousIndex;
      /*printf("previous index is %i \n and rlenbuf is %i \n",previousIndex,rlenbuf[kk]);*/ 
    }

    blen = rol->dabufp - (int *)&jw[ind+1];

    /* printf("blen is: %i\n",blen); */

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
    else if(bosMclose_(jw,ind,1,blen) <= 0)
    {
      logMsg("2ERROR in bosMclose_ - space is not enough !!!\n",1,2,3,4,5,6);
    }

    goto skiptdcs;


#endif


#ifdef USE_FADC250


	if(nboards>0)
	{
      tdc1190ReadStart(tdcbuf, rlenbuf);
      for(ii=0; ii<rlenbuf[0]; ii++) *rol->dabufp++ = tdcbuf[ii];
	}

    if(timeprofile)
    {
      TIMER_NORMALIZE;
	  TIMER_START;
    }



    /* Check for valid data here */
    for(ii=0; ii<100; ii++) 
    {
      datascan = faBready(FA_SLOT);
      if(datascan>0) 
	  {
	    break;
	  }
    }

	
    if(datascan>0) 
    {
      nwords = faReadBlock(FA_SLOT,tdcbuf,60000,1);    

      if(nwords < 0) 
	  {
	    printf("ERROR: in transfer (event = %d), nwords = 0x%x\n",*(rol->nevents),nwords);
	  } 
      else 
	  {
		/*
        logMsg("[%d] nwords=%d (ii=%d datascan=0x%08x)\n",FA_SLOT,nwords,ii,datascan,5,6);
		*/
        if(nwords>2000) nwords=2000;
/*
logMsg("data: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",tdcbuf[0],tdcbuf[1],tdcbuf[2],tdcbuf[3],tdcbuf[4],tdcbuf[5]);
*/
/*
	    for(ii=0; ii<nwords; ii++)
        {
          *rol->dabufp++ = tdcbuf[ii];
		}
*/
	  }
    } 
    else 
    {
      printf("ERROR: Data not ready in event %d\n",*(rol->nevents));
      nwords = 0;
    }







    /* open data bank */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0) /* bosMopen_(int *jw, char *name, int nr, int ncol, int nrow);*/
    {
      logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
    }
    rol->dabufp += NHEAD;



    /* FADC data analysis */
    itdcbuf = 0;
    nheaders = ntrailers = 0;
    /*njjloops = nboards;*/njjloops = 1;
    for(jj=0; jj<njjloops; jj++)
    {
      /*rlen = rlenbuf[jj];*/rlen = nwords;
	  
	  /*
#ifdef DEBUG	  
      level = tdc1190GetAlmostFullLevel(jj);
      ii = tdc1190StatusAlmostFull(jj);
      logMsg("jj=%d, rlen=%d, almostfull=%d level=%d\n",jj,rlen,ii,level,5,6);
#endif
	  */

      if(rlen <= 0) continue;

      tdc = &tdcbuf[itdcbuf];
      itdcbuf += rlen;
      ii=0;
      while(ii<rlen)
      {
        int a_channel, a_nevents, a_blocknumber, a_triggernumber, a_windowwidth;
		int a_adc1, a_adc2, a_valid1, a_valid2, a_nwords, a_slot1, a_slot2;
        int a_trigtime[4], a_id;

        if( ((tdc[ii]>>27)&0x1F) == 0x10)
        {
          a_slot1 = ((tdc[ii]>>22)&0x1F);
          a_nevents = ((tdc[ii]>>11)&0x3FF);
          a_blocknumber = (tdc[ii]&0x3FF);
#ifdef DEBUG
		  logMsg("[%3d] BLOCK HEADER: slot %d, nevents %d, block number %d\n",ii,
				   a_slot1,a_nevents,a_blocknumber,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x11)
        {
          a_slot2 = ((tdc[ii]>>22)&0x1F);
          a_nwords = (tdc[ii]&0x3FFFFF);
#ifdef DEBUG
		  logMsg("[%3d] BLOCK TRAILER: slot %d, nwords %d\n",ii,
				   a_slot2,a_nwords,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x12)
        {
          a_triggernumber = (tdc[ii]&0x7FFFFFF);
#ifdef DEBUG
		  logMsg("[%3d] EVENT HEADER: trigger number %d\n",ii,
				   a_triggernumber,3,4,5,6);
#endif
		  ii++;
          while( ((tdc[ii]>>31)&0x1) == 0 )
		  {
            a_triggernumber = (tdc[ii]&0x7FFFFFF);
#ifdef DEBUG
            logMsg("   [%3d] EVENT HEADER 2: trigger number %d -> SERIOUS ERROR !!!\n",ii,
              a_triggernumber,3,4,5,6);
#endif
            ii++;
		  }
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x13)
        {
          a_trigtime[0] = (tdc[ii]&0xFFFFFF);
#ifdef DEBUG
		  logMsg("[%3d] TRIGGER TIME: 0x%06x\n",ii,
				   a_trigtime[0],3,4,5,6);
#endif
		  ii++;
          iii=1;
          while( ((tdc[ii]>>31)&0x1) == 0 )
		  {
            a_trigtime[iii] = (tdc[ii]&0xFFFFFF);
#ifdef DEBUG
            logMsg("   [%3d] TRIGGER TIME: 0x%06x\n",ii,
              a_trigtime[iii],3,4,5,6);
#endif
            iii++;
            ii++;
		  }
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x14)
        {
          a_channel = ((tdc[ii]>>23)&0xF);
          a_windowwidth = (tdc[ii]&0xFFF);
          a_id = (a_slot1<<24)+(a_channel<<16)+a_windowwidth;
#ifdef DEBUG
		  logMsg("[%3d] WINDOW RAW DATA: channel %d, window width %d (a_id=0x%08x)\n",ii,
				   a_channel,a_windowwidth,a_id,5,6);
#endif
          *rol->dabufp ++ = a_id;
		  ii++;
          while( ((tdc[ii]>>31)&0x1) == 0 )
		  {
            a_valid1 = ((tdc[ii]>>29)&0x1);
            a_adc1 = ((tdc[ii]>>16)&0xFFF);
            a_valid2 = ((tdc[ii]>>13)&0x1);
            a_adc2 = (tdc[ii]&0xFFF);
#ifdef DEBUG
            logMsg("   [%3d] WINDOW RAW DATA: valid1 %d, adc1 0x%04x, valid2 %d, adc2 0x%04x\n",ii,
              a_valid1,a_adc1,a_valid2,a_adc2,6);
#endif
            *rol->dabufp ++ = tdc[ii];
            ii++;
		  }
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x15)
        {
#ifdef DEBUG
		  logMsg("[%3d] WINDOW SUM: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x16)
        {
#ifdef DEBUG
		  logMsg("[%3d] PULSE RAW DATA: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x17)
        {
#ifdef DEBUG
		  logMsg("[%3d] PULSE INTEGRAL: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x18)
        {
#ifdef DEBUG
		  logMsg("[%3d] PULSE TIME: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x19)
        {
#ifdef DEBUG
		  logMsg("[%3d] STREAMING RAW DATA: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x1D)
        {
#ifdef DEBUG
		  logMsg("[%3d] EVENT TRAILER: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x1E)
        {
		  logMsg("[%3d] : DATA NOT VALID\n",ii,
				   2,3,4,5,6);
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x1F)
        {
#ifdef DEBUG
		  logMsg("[%3d] FILLER WORD: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else
        {
          logMsg("[%3d] ERROR: in TDC data format 0x%08x\n",ii,
            (int)tdc[ii],3,4,5,6);
          ii++;
        }

      } /* loop over 'rlen' words */

    }
















	/* add offset to the end of bank */
	*rol->dabufp ++ = fadc_offset;











    if(timeprofile)
    {
      TIMER_STOP(100000,1000+rol->pid);
    }

    blen = rol->dabufp - (int *)&jw[ind+1];

    /* printf("blen is: %i\n",blen); */

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
    else if(bosMclose_(jw,ind,1,blen) <= 0)
    {
      logMsg("2ERROR in bosMclose_ - space is not enough !!!\n",1,2,3,4,5,6);
    }

    goto skiptdcs;

#endif


	/*tdc1190*/

    for(jj=0; jj<22; jj++) zerochan[jj] = -1;



    /*board-by-board DMA readout*/
	tdc1190ReadStart(tdcbuf, rlenbuf);
	/*
	logMsg("rlenbuf = %d %d\n",rlenbuf[0],rlenbuf[1],3,4,5,6);
	*/



    /*linked list DMA readout*/
    /*res = tdc1190ReadListStart(tdcbuf, rlenbuf);
    if(res != OK) logMsg("START BAD %d\n",res,2,3,4,5,6);
    res = tdc1190ReadDone();
    if(res <= 0)
    {
      logMsg("DONE BAD BAD %d\n",res,2,3,4,5,6);
      for(jj=0; jj<nboards; jj++) rlenbuf[jj] = 0;
    }
	*/





    /************************/
    /************************/
    /************************/
	/* start of v1190/v1290 */

    /* open data bank */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
    }
    rol->dabufp += NHEAD;

    /* analysis */
    itdcbuf = 0;
    nheaders = ntrailers = 0;

    njjloops = nboards;
	/*
for(i=0; i<200; i++) tmpgood[i] = tdcbuf[i];
	*/
    for(jj=0; jj<njjloops; jj++)
    {
      rlen = rlenbuf[jj];
	  
#ifdef DEBUG
	  
      level = tdc1190GetAlmostFullLevel(jj);
      ii = tdc1190StatusAlmostFull(jj);
      logMsg("jj=%d, rlen=%d, almostfull=%d level=%d\n",jj,rlen,ii,level,5,6);
	  
#endif
	  
      if(rlen <= 0) continue;

      tdc = &tdcbuf[itdcbuf];
      itdcbuf += rlen;

      for(ii=0; ii<rlen; ii++)
      {
		
        if( ((tdc[ii]>>27)&0x1F) == 0)
        {
          nbcount ++;
          *rol->dabufp ++ = (tdc[ii] | (tdcslot<<27));

#ifdef PRIMEXXX
		  /*PRIMEX: zero chan check*/
          if(rol->pid==18)
		  {
            tdcchan = ((tdc[ii]>>19)&0x7F);
            tdcval = (tdc[ii]&0x7FFFF);
		  }
		  else
		  {
            tdcchan = ((tdc[ii]>>21)&0x1F);
            tdcval = (tdc[ii]&0x1FFFFF);
			/*logMsg("[%2d] chan %2d tdc %6d\n",tdcslot,tdcchan,tdcval);*/
		  }
          if(tdcchan==0)
		  {
            if(zerochan[tdcslot]>0)
            {
              logMsg("ERROR at slot %2d: multiple references\n",tdcslot,2,3,4,5,6);
		    }
            else
		    {
              zerochan[tdcslot] = tdcval;
			  /*logMsg("set zerochan[%2d]=%6d\n",tdcslot,tdcval);*/
		    }
		  }
          /*end of zero chan check*/
#endif


#ifdef DEBUG
          tdcedge = ((tdc[ii]>>26)&0x1);
          tdcchan = ((tdc[ii]>>19)&0x7F);
          tdcval = (tdc[ii]&0x7FFFF);
		  if(1/*tdcchan<2*/)
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
          logMsg(" FILLER\n",1,2,3,4,5,6);
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x11)
        {
          nbsubtract ++;
#ifdef DEBUG
          logMsg(" TIME TAG: %08x\n",(int)(tdc[ii]&0x7ffffff),2,3,4,5,6);
#endif
        }
        else if( ((tdc[ii]>>27)&0x1F) == 8)
        {
          nbcount = 1; /* counter for the number of output words from board */
          nbsubtract = 9; /* # words to subtract including errors */
          tdcslot = tdc[ii]&0x1F;
          tdceventcount = (tdc[ii]>>5)&0x3FFFFF;
          tdchead = (unsigned int *) rol->dabufp; /* remember pointer */
#ifdef SLOTWORKAROUND
          tdcslot_h = tdcslot;
	      remember_h = tdc[ii];
          if(slotnums[nheaders] != tdcslot) /* correct slot number */
		  {
            logMsg("WARN: [%2d] slotnums=%d, tdcslot=%d -> use slotnums\n",
				   nheaders,slotnums[nheaders],tdcslot,4,5,6);
            tdcslot = slotnums[nheaders];
		  }
#endif
          *rol->dabufp ++ = tdcslot;
          nheaders++;

		  
#ifdef DEBUG
		  
          logMsg("GLOBAL HEADER: %d words, 0x%08x (slot=%d nevent=%d) -> header=0x%08x\n",
            rlen,tdc[ii],tdc[ii]&0x1F,(tdc[ii]>>5)&0xFFFFFF,*tdchead,6);
		  
#endif
		  
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x10)
        {
#ifdef SLOTWORKAROUND
          /* double check for slot number */
          tdcslot_t = tdc[ii]&0x1F;
          if(tdcslot_h>21||tdcslot_t>21||tdcslot_h!=tdcslot_t)
          {
            /*logMsg("WARN: slot from header=%d (0x%08x), from trailer=%d (0x%08x), must be %d\n",
			  tdcslot_h,remember_h,tdcslot_t,tdc[ii],tdcslot,6)*/;
          }
#endif
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
          ntrailers ++;

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

      } /* loop over 'rlen' words */

    }


#ifdef PRIMEXXX
    /*PRIMEX: zero chan check*/
    for(ii=0; ii<nboards; ii++)
    {
	  if(zerochan[slotnums[ii]]<=0)  logMsg("ERROR at slot %2d: no reference signal\n",slotnums[ii],2,3,4,5,6);
    }
	/*end of zero chan check*/
#endif


	/*
    if(nheaders!=ntrailers || nboards!=nheaders)
      logMsg("ERROR: nboards=%d, nheaders=%d, ntrailers=%d\n",nboards,nheaders,ntrailers,4,5,6);
	*/





	/*TEST: add some junk to increase event size
    rol->dabufp += 2000;
	TEST*/



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
    else if(bosMclose_(jw,ind,1,blen) <= 0)
    {
      logMsg("2ERROR in bosMclose_ - space is not enough !!!\n",1,2,3,4,5,6);
    }

	/* end of v1190/v1290 */
    /**********************/
    /**********************/
    /**********************/

skiptdcs:


    /* for physics sync event, make sure all board buffers are empty */
    if(syncFlag==1)
    {
      int scan_flag;
      unsigned short slot, nev16;
	  /*
logMsg("SYNC reached\n",1,2,3,4,5,6);
	  */

	  /*
#ifdef USE_FADC250
if(fadc_offset < 3500) fadc_offset += 50;
faSetDAC(FA_SLOT,fadc_offset,0xffff);
#endif
	  */

      scan_flag = 0;
      for(ii=0; ii<nboards; ii++)
      {
        if(nboards>0)
		{
          nev = nev16 = tdc1190GetEventStored(ii);
          slot = tdc1190GetGeoAddress(ii);
          if(nev != 0) /* clear board if extra event */
          {
            logMsg("SYNC: ERROR: [%2d] slot=%2d nev=%d - clear\n",
              ii,slot,nev,4,5,6);
            tdc1190Clear(ii);
            scan_flag |= (1<<slot);
          }
          if(error_flag[slot] == 1) /* clear board if error flag was set */
          {
		    /*
            logMsg("SYNC: ERROR: [%2d] slot=%2d error_flag=%d - clear\n",
              ii,slot,error_flag[slot],4,5,6);
		    */
            tdc1190Clear(ii);
            error_flag[slot] = 0;
            scan_flag |= (1<<slot);
          }
		}

      }
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

static void
__end()
{
  int ii;

  CDODISABLE(VME,1,0);

  logMsg("INFO: User End Executed\n",1,2,3,4,5,6);

#ifdef USE_V1720
  for(ii=0; ii<nadcs; ii++)
  {
    v1720SetAcquisition(ii, 0, 0, 0);
 	v1720SetBLTRange(ii,0);
 }
#endif

#ifdef USE_FADC250
  /* FADC Disable */
  faDisable(FA_SLOT,0);

  /* FADC Event status - Is all data read out */
  faStatus(FA_SLOT,0);

  faReset(FA_SLOT,0);
#endif

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
