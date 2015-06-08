/*
#define PRIMEX
*/

#define USE_V1720

/* if event rate goes higher then 10kHz, with randon triggers we have wrong
slot number reported in GLOBAL HEADER and/or GLOBAL TRAILER words; to work
around that problem temporary patches were applied - until fixed (Sergey) */
#define SLOTWORKAROUND

/* fadc1.c - first readout list for FT */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

#define ROL_NAME__ "FADC1"

/* ts control */
/* Se non e' definito standalone, compila e definisci trig_supv */
/* E' definito nel makefile */
#ifndef STANDALONE 
#define TRIG_SUPV
#endif

/* polling mode if needed */
#define POLLING_MODE


/* name used by loader */
#ifdef STANDALONE
#define INIT_NAME fadc1_standalone__init
#else
#define INIT_NAME fadc1__init
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
#include "v1720.h"

/* from fputil.h */
#define SYNC 0x20000000


#undef DEBUG


/* main TI board */
/* INDIRIZZO DELLA TI */
#define TIRADR   0x0ed0

static char *rcname = "RC00";

/* user buffer pointers - all NULL at initialization */
extern int *userbuffers[NUSERBUFFERS];

/* pointer to TTS structures for current roc */
static TTSPtr ttp1;

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
  unsigned long offset, ii, jj;

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  printf(">>>>>>>>>>>>>>>>>>>>>>>>>> ROCID = %d <<<<<<<<<<<<<<<<\n",rol->pid);

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");

  printf("rol1: downloading translation table for roc=%d (ttp1=0x%08x)\n",rol->pid,ttp1);
  ttp1 = TT_LoadROCTT_(rol->pid, ttp1);
  printf("rol1: ... done, ttp1=0x%08x\n",ttp1);

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
  usrVmeDmaSetConfig(2,3,0); /* A32,MBLT */


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
  nboards = tdc1190Init(0x08210000,0x10000,20,0); /*tdc1190Init(bus addr first tdc,step,max number,flag=0)*/
  /*window_with+offset+extra <= 1000 ns!!!!!!!!!!*/
  if(nboards>0)
  {
    tdc1190SetDefaultWindowWidth(1000); /*1.5 us */ /*in ns*/
    tdc1190SetDefaultWindowOffset(-600); /*-3.9 us*/

    tdc1190Config(1); /*Trigger matching mode */


    for(jj=0; jj<nboards; jj++)
    {
      tdc1190SetExtraSearchMargin(jj,100);
      /*tdc1190TriggerTimeTag(jj,1);*/ /* Enable Trigger Time Tag */    
    }
  }
/* v1190 end */

  logMsg("INFO: User Download Executed\n",1,2,3,4,5,6);
}


static void
__prestart()
{
  char *env;
  char *histname = "ROL1 RC00";
  unsigned short value, value0, value1, array0[32], array1[32], data16;
  int i, ii, ifull, jj;
  int ADCmask[2]; 
  int ADCoffset[2][V1720_MAX_CHANNELS];
  int PostTriggerValue[2];
  int BuffSize[2];
  short buf;
  unsigned short slot, channel, pattern[8];

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
  /*VME= */
  /*TIR_SOURCE= trigger interface board, e NON il trigger supervisor */
  /*usrtrig -> handler */
  /*usrtrig_done -> done */

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
      slot = tdc890GetGeoAddress(ii);

      tdc890EnableAllChannels(ii);
      for(channel=1; channel<128; channel++)
      {
        if(ttp->idnr[slot][channel] <= 0)
        {
          tdc890DisableChannel(ii, channel);
        }
      }

      pattern = tdc890GetChannels(ii);
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



#ifdef USE_V1720
  printf("v1720 init\n");
  printf("v1720 init\n");
  printf("v1720 init\n");
  printf("v1720 init\n");
  printf("v1720 init\n");
  printf("v1720 init\n");
  
  ADCmask[0]=0x0ff; /*First board, 0xff=11111111*/
  ADCmask[1]=0x0ff; /*Second board 0x3f=11111111*/
  
  /*number of mem loc for each event/2, i.e. value of samples/4*/
  
/*80 samples, 320 ns*/
/*
  BuffSize[0]= 20; 
  BuffSize[1]= 20;
*/
  BuffSize[0]=50;
  BuffSize[1]=50;
  

  /*Number of post trigger samples=4*PostTriggerValue, see manual*/
  PostTriggerValue[0]=10;
  PostTriggerValue[1]=10; 



 /*First Board (our board) */ /*0: Negative unipolar signal */
/*0xffff: Positive unipolar signal */
   /*1 dac=3.051E-5 volt */


  /*
  ADCoffset[0][0]=5900; 
  ADCoffset[0][1]=6450; 
  ADCoffset[0][2]=6400;
  ADCoffset[0][3]=6080;
  ADCoffset[0][4]=6430;
  ADCoffset[0][5]=6500;
  ADCoffset[0][6]=5850;
  ADCoffset[0][7]=6740;

  ADCoffset[1][0]=6340;
  ADCoffset[1][1]=6450;
  ADCoffset[1][2]=5750;
  ADCoffset[1][3]=6460;
  ADCoffset[1][4]=6010;
  ADCoffset[1][5]=6300;
  ADCoffset[1][6]=6500;
  ADCoffset[1][7]=6880; 
  */
  
  ADCoffset[0][0]=11900; 
  ADCoffset[0][1]=11450; 
  ADCoffset[0][2]=11400;
  ADCoffset[0][3]=11080;
  ADCoffset[0][4]=11430;
  ADCoffset[0][5]=11500;
  ADCoffset[0][6]=11850;
  ADCoffset[0][7]=11740;

  ADCoffset[1][0]=11340;
  ADCoffset[1][1]=6450;
  ADCoffset[1][2]=5750;
  ADCoffset[1][3]=6460;
  ADCoffset[1][4]=6010;
  ADCoffset[1][5]=6300;
  ADCoffset[1][6]=6500;
  ADCoffset[1][7]=6880; 




  nadcs = v1720Init(0x09110000,0x1000000,20,0); /*First addr VME bus, step, max, opt */
  printf("====> nadcs=%d\n",nadcs);
  v1720Config(1);

  for(ii=0; ii<nadcs; ii++)/*Put here V1720Settings Andrea*/ 
  {
    
    v1720SetBLTRange(ii,0);
    if (nadcs==2)
      {
	printf("I have 2 ADCs\n");
	v1720SetBufferSizePerEvent(ii,BuffSize[ii]);
	v1720SetChannelEnableMask(ii,ADCmask[ii]); 
	v1720SetPostTrigger(ii,PostTriggerValue[ii]);
	v1720SetBuffer(ii, 0x0A); /* 1024 buffers 1K samples max size 0X0A*/
	/* v1720SetBuffer(ii, 0x06);*//* 64 buffers 1K samples max size 0X0A*/
	for (jj=0;jj<V1720_MAX_CHANNELS;jj++){
	  printf("Will set ADC offset %i for ch %i \n",ADCoffset[ii][jj],jj);
	  v1720SetChannelDACOffset(ii,jj,ADCoffset[ii][jj]);
	}
      }
    else { 
      printf("I DON'T have 2 ADCs\n");
      v1720SetChannelEnableMask(ii,0xff); 
      v1720SetPostTrigger(ii,0);    
      for (jj=0;jj<V1720_MAX_CHANNELS;jj++){
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
  int extra;

  logMsg("INFO: User Go ...\n",1,2,3,4,5,6);


  for(ii=0; ii<nboards; ii++)
  {
    tdc1190Clear(ii);
  }

  for(ii=0; ii<NBOARDS; ii++)
  {
    error_flag[ii] = 0;
  }


#ifdef USE_V1720
  printf("Setting V1720\n");
  for(ii=0; ii<nadcs; ii++)
  {
   

    v1720SetAcquisition(ii, 0, 1, 0);
    v1720SetBLTRange(ii,1);
  }
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

  int ii, kk, njjloops, blen, jj, nev, rlen, rlenbuf[NBOARDS], nevts, nwrds;
  int previousIndex;
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

  jw = rol->dabufp; /*output write pointer */
  /*
logMsg("000\n",1,2,3,4,5,6);
  */

  /***/

  /* at that moment only second CODA world defined  */
  /* first CODA world (length) undefined, so set it */
  /*jw[ILEN] = 1;*/ jw[-2] = 1; /*jw[-2] e jw[-1]: coda header */

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


	/*
goto skip1;
	*/


    /* open data bank */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0) /* bosMopen_(int *jw, char *name, int nr, int ncol, int nrow);*/
    {
      logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
    }
    rol->dabufp += NHEAD;


		 
    tdc1190ReadStart(tdcbuf, rlenbuf); /*tdc1190ReadStart(data *,int ndata[nboards])*. In tdcbuf dati in serie per diversi tdc/ 
    /*logMsg("tdc=%d\n",rlenbuf[0],2,3,4,5,6);*/

    for(ii=0; ii<rlenbuf[0]; ii++) *rol->dabufp++ = tdcbuf[ii];


	/*
goto skip1;
	*/

    v1720ReadStart(tdcbuf, rlenbuf); /*Come sopra */
    /*logMsg("adc=%d\n",rlenbuf[0],2,3,4,5,6);*/


    /*for(ii=0; ii<rlenbuf[0]; ii++) *rol->dabufp++ = tdcbuf[ii]; /*Is it writing only the first ADC?? Andrea */
    

    previousIndex=0;
    for(kk=0;kk<nadcs;kk++)
    {
      /*printf("This is ADC %i of %i total adcs \n",kk,nadcs); */
      for(ii=0; ii<rlenbuf[kk]; ii++)
      {
	    *rol->dabufp++ = tdcbuf[ii+previousIndex];
      }
      previousIndex=rlenbuf[kk]+previousIndex;
      /*printf("previous index is %i \n and rlenbuf is %i \n",previousIndex,rlenbuf[kk]);*/ 
    }

skip1:



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


    /* for physics sync event, make sure all board buffers are empty */
    /* NOTE: does not work without TS crate !! MUST do you own check*/
    if(syncFlag==1)
    {
      int scan_flag;
      unsigned short slot, nev16;
	  /*
logMsg("SYNC reached\n",1,2,3,4,5,6);
	  */
      scan_flag = 0;
      for(ii=0; ii<nboards; ii++)
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
