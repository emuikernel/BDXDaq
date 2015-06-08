/*
#define USE_1182
*/

/* ctoftest1.c - first readout list for central tof test setup */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list SFI */
#define ROL_NAME__ "CTOFTEST1"

/* ts control */
/*#define TRIG_SUPV*/

/* polling mode if needed */

#define POLLING_MODE


/* name used by loader */
#define INIT_NAME ctoftest1__init

#include "rol.h"

/* vme readout */
#include "VME_source.h" /* POLLING_MODE for tir_triglib.h undefined here */


/* user code */

#include "uthbook.h"
#include "coda.h"
#include "tt.h"
#include "adc792.h"



/* main TI board */
#define TIRADR   0x0ed0
/* CAEN v775 TDC */
#define TDCADR0  0x610000
/* CAEN v792 ADC */
#define ADCADR0  0x510000

#ifdef USE_1182
/* lecroy 1182 ADCs */
#define ADCADR1  0x110000
#define ADCADR2  0x120000
#define ADCADR3  0x130000
#define ADCADR4  0x140000
#endif

/* interrupt vector (f0) */
#define INTVECTOR  0xf0
#define INTVECTOR1  0xf4

/* interrupt level (TI board has level 5) */
/* everybody must use the same interrupt level */
#define INTLEVEL  6


/* DO NOT FORGET TO CHANGE THIS IF CHANGING 'maximum #####,###' ABOVE !!! */ 
#define BUFSIZE_IN_BYTES1 MAX_EVENT_LENGTH
#define NUM_BUFFERS1 MAX_EVENT_POOL


#undef DEBUG


static char *rcname = "RC00";

/* global addresses for boards */
static unsigned long tdcadr0, adcadr0;
static unsigned long adcadr[4];

/* user buffer pointers - all NULL at initialization */
extern int *userbuffers[NUSERBUFFERS];
/* pointer to TTS structures for current roc */
extern TTSPtr ttp;

static int printRingFull = 1;

/* time profiling: 0-OFF, 1-ON */
static int timeprofile = 1;

/* histogram area */
static UThisti histi[NHIST];
static neventh = 0;


#define NBOARDS 22    /* maximum number of VME boards */






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
  /*sysBusToLocalAdrs(0x29,0xFFFF0000,&offset);*/
  tir[1] = (struct vme_tir *)(offset+TIRADR);
  printf("rol1: TI board address = 0x%08x\n",tir[1]);


  /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&offset);
  /*sysBusToLocalAdrs(0x39,0xFF000000,&offset);*/
  tdcadr0 = offset + TDCADR0;
  printf("rol1: tdcadr0 = 0x%08x\n",tdcadr0);
  adcadr0 = offset + ADCADR0;
  printf("rol1: adcadr0 = 0x%08x\n",adcadr0);

#ifdef USE_1182
  adcadr[0] = offset + ADCADR1;
  adcadr[1] = offset + ADCADR2;
  adcadr[2] = offset + ADCADR3;
  adcadr[3] = offset + ADCADR4;
  printf("rol1: adcadr = 0x%08x\n",adcadr[3]);
#endif

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

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  /* init trig source VME */
  VME_INIT;

  /* Register an async trigger source, (up to 32 sources) */
  /* CTRIGRSA(source,code,handler,done)*/
  /* link async trig source VME 1 to usrtrig and usrtrig_done */
#ifdef POLLING_MODE
  CTRIGRSS(VME, 1, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */
#else
  CTRIGRSA(VME, 1, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */
#endif

#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  printf("===================> rol->pid=%d\n",rol->pid);
  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);





  /* CAEN v775 TDC program start */

  /* board initialization */
  adc792boardInit(tdcadr0);


  /*********************************/
  /* for non-PAUX version only !!! *
  adc792setGEOAddress(tdcadr0, 4);
  adc792setLogic(tdcadr0, SOFT_RESET);
  adc792resetLogic(tdcadr0, SOFT_RESET);
  *********************************/
  /*********************************/


  adc792resetOperationMode(tdcadr0, START_STOP); /* Common Start mode */
  /*adc792setOperationMode(tdcadr0, START_STOP);*/ /* Common Stop mode */

  /* set resolution (0xFF for 35 ps, 0x1E for 300 ps) */
  value = 0xFF;
  tdc775setResolution(tdcadr0, value);
  tdc775getResolution(tdcadr0, &value);
  printf("Resolution per channel is 0x%x\n",value);

  /* set thresholds */
  for(i=0; i<32; i++) array0[i] = 0;
  adc792setThresholds(tdcadr0, array0);
  adc792getThresholds(tdcadr0, array1);
  printf("TDC thresholds:\n");
  for(i=0; i<32; i++) printf("ch[%2d] set %d  get %d\n",i,array0[i],array1[i]);

  /* disable overflow suppression */
  adc792setOperationMode(tdcadr0, OVER_RANGE);

  /* disable valid suppression */
  adc792setOperationMode(tdcadr0, VALID_CONTROL);

  adc792getOperationMode(tdcadr0, &value);
  if(value&START_STOP) printf("Common Stop mode\n");
  else                 printf("Common Start mode\n");
  if(value&OVER_RANGE) printf("NO Overflow suppression\n");
  else                 printf("Overflow suppression\n");
  if(value&VALID_CONTROL) printf("NO Valid suppression\n");
  else                    printf("Valid suppression\n");

  /* enable interrupt on every event SERGEY !!!
  adc792intenable(tdcadr0, 1, INTLEVEL,INTVECTOR1);
  */

  /* CAEN v775 TDC program end */



#ifdef USE_1182

  printf("Using ADC 1182\n");

#else

  /* CAEN v792 ADC start */

  printf("Using ADC 792\n");

  /* board initialization */
  adc792boardInit(adcadr0);

  /* set thresholds */
  for(i=0; i<32; i++) array0[i] = 0;
  adc792setThresholds(adcadr0, array0);
  adc792getThresholds(adcadr0, array1);
  for(i=0; i<32; i++) printf("ch[%2d] set %d  get %d\n",i,array0[i],array1[i]);

  /* set pedestal */
  value0 = 255;
  adc792setPedestal(adcadr0, value0);
  adc792getPedestal(adcadr0, &value);
  printf("ped: set %d  get %d\n",value0,value);

  /* disable overflow suppression */
  adc792setOperationMode(adcadr0, OVER_RANGE);

  /* disable valid suppression */
  adc792setOperationMode(adcadr0, VALID_CONTROL);

  adc792getOperationMode(adcadr0, &value);
  if(value&START_STOP) printf("Common Stop mode\n");
  else                 printf("Common Start mode\n");
  if(value&OVER_RANGE) printf("NO Overflow suppression\n");
  else                 printf("Overflow suppression\n");
  if(value&VALID_CONTROL) printf("NO Valid suppression\n");
  else                    printf("Valid suppression\n");

  /* CAEN ADC end */

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


  /*ctoftest1-related histograms*/
  for(i=1; i<=24; i++) uthbook1(histi, 130+i, "TDC", 4000, 0, 4000);
  for(i=1; i<=16; i++) uthbook1(histi, 154+i, "ADC", 2000, 0, 2000);
  uthbook1(histi, 171, "ctof-res", 2000, 0, 2000);

  printf("INFO: User Prestart Executed\n");

  /* from parser */
  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}       

static void
__pause()
{
  /*scaler7201intdisable(scaler0);*/

  CDODISABLE(VME,1,0);

  logMsg("INFO: User Pause Executed\n",1,2,3,4,5,6);

  return;
}

static void
__go()
{  
  unsigned short value, value0, value1, array0[32], array1[32];
  int i, ii;
  struct vme_tir
  {
    unsigned short tir_csr;
    unsigned short tir_vec;
    unsigned short tir_dat;
    unsigned short tir_oport;
    unsigned short tir_iport;
  };

  logMsg("INFO: User Go ...\n",1,2,3,4,5,6);

  /* reset output register in TI board - will be used for tdc test */
  tir[1]->tir_oport = 0x0;

#ifdef USE_1182
  /* adc1182 reset */
  adc1182reset(adcadr[3]);
#endif

  /*
{
  unsigned short *buf2, tmp2;
  unsigned short value, value1, tdc, adc, ttt[9];
  unsigned int nwords, nwords1, nevent, nevent1, buff[32], buff1[32];
      adc792getBufferStatus(tdcadr0,&value);
      while(!(value & BUFFER_EMPTY))
	  {
        adc792readEvent(tdcadr0, buff, &nwords, &nevent);
        logMsg("GO: TDC ERROR: extra event # %d - recovering\n",nevent,2,3,4,5,6);
        adc792getBufferStatus(tdcadr0,&value);
	  }
      logMsg("GO: TDC ERROR: extra event - recovered\n",1,2,3,4,5,6);

      adc792getBufferStatus(adcadr0,&value1);
	  while(!(value1 & BUFFER_EMPTY))
      {
        adc792readEvent(adcadr0, buff1, &nwords1, &nevent);
        logMsg("GO: ADC ERROR: extra event # %d - recovering\n",nevent,2,3,4,5,6);
        adc792getBufferStatus(adcadr0,&value1);
      }
      logMsg("GO: ADC ERROR: extra event - recovered\n",1,2,3,4,5,6);
}
*/

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
  int i, j, k, id1, id2, ind, ind2, ncol, nrow, len, len1, len2, tdcrec, adcrec;
  unsigned long tmp, *secondword, *jw, *buf, *dabufp1, *dabufp2;
  TIMER_VAR;
  unsigned short *buf2, tmp2;
  unsigned short value, value1, tdc, adc, ttt[9];
  unsigned int nwords, nwords1, nevent, nevent1, buff[32], buff1[32];

  int ii, jj, nev, timeres, goodtoread;
  unsigned long res;

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
    ;
  }
  else if((syncFlag==0)&&(EVTYPE==0))    /* illegal */
  {
    ;
  }
  else if((syncFlag==1)&&(EVTYPE==0))    /* force_sync events */
  {
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


    if(timeprofile)
    {
      TIMER_NORMALIZE;
      TIMER_START;
    }

    /*logMsg("trigger\n",1,2,3,4,5,6);*/

    goodtoread=0;
    tdcrec=0;
    adcrec=0;
    for(j=0; j<32; j++)
    {
      buff[j] = 0;
      buff1[j] = 0;
    }

    /*************************/
    /* v775 and v792 readout */
    /*************************/
	
    adc792getStatus(tdcadr0, &value);
    while(!(value&DREADY))
    {
      /*logMsg("waiting for tdc...\n",1,2,3,4,5,6);*/
      adc792getStatus(tdcadr0, &value);
    }
#ifdef USE_1182
	
    while( adc1182cip(adcadr[3]) );
	
	/*    if( adc1182cip(adcadr[3]) ) goodtoread = 1;*/

#else
    adc792getStatus(adcadr0, &value1);
    while(!(value1&DREADY))
    {
      /*logMsg("waiting for adc...\n",1,2,3,4,5,6);*/
      adc792getStatus(adcadr0, &value1);
    }
#endif



#ifdef USE_1182
    /* read TDC */
    adc792getBufferStatus(tdcadr0,&value);
    if( !(value & BUFFER_EMPTY) )
    {
      adc792readEvent(tdcadr0, buff, &nwords, &nevent);
	}

    /* read ADC */
    for(j=0; j<8; j++) buff1[j*2] = adc1182read(adcadr[3], j);
    nwords1 = 8;

    /* check TDC */
    adc792getBufferStatus(tdcadr0,&value);
    while(!(value & BUFFER_EMPTY))
	{
      tdcrec=1;
      adc792readEvent(tdcadr0, buff, &nwords, &nevent);
      logMsg("TDC ERROR: extra event # %d - recovering\n",nevent,2,3,4,5,6);
      adc792getBufferStatus(tdcadr0,&value);
	}
    if(tdcrec) logMsg("TDC ERROR: extra event - recovered\n",1,2,3,4,5,6);

    if((ind2 = bosMopen_(jw,"CT  ",0,3,nwords)) > 0)
    {
      buf2 = (unsigned short *)&jw[ind2+1];
      for(i=0; i<nwords; i++)
      {
        tmp = buff[i];
        *buf2++ = /*i*/((tmp>>16)&0x3F)+1;
        *buf2++ = tmp&0xFFF;
        *buf2++ = buff1[i]&0xFFF;
      }
      rol->dabufp += bosMclose_(jw,ind2,3,nwords);
    }


#else
    /* without interrupts */
    adc792getBufferStatus(tdcadr0,&value);
    adc792getBufferStatus(adcadr0,&value1);

    if( (!(value & BUFFER_EMPTY)) && (!(value1 & BUFFER_EMPTY)) )
    {
      adc792readEvent(tdcadr0, buff, &nwords, &nevent);
      adc792readEvent(adcadr0, buff1, &nwords1, &nevent1);
      if(!(nevent%10000))
      {
        logMsg("Got TDC event # %d, ADC event # %d, nw(tdc)=%d nw(adc)=%d\n",nevent,nevent1,nwords,nwords1,5,6);
        logMsg(" slot=%2d channel=%2d data=%4d (un=%1d ov=%1d)\n",
          (buff1[0]>>27)&0x1F, (buff1[0]>>16)&0x3F, buff1[0]&0xFFF, (buff1[0]>>13)&0x1,(buff1[0]>>12)&0x1);
        logMsg(" slot=%2d channel=%2d data=%4d (un=%1d ov=%1d)\n",
          (buff1[1]>>27)&0x1F, (buff1[1]>>16)&0x3F, buff1[1]&0xFFF, (buff1[1]>>13)&0x1,(buff1[1]>>12)&0x1);
      }
      /* check if buffers are empty; if not, read everything out and
      do not store anything */
      goodtoread = 1;

      adc792getBufferStatus(tdcadr0,&value);
      while(!(value & BUFFER_EMPTY))
	  {
        goodtoread = 0;
        tdcrec=1;
        adc792readEvent(tdcadr0, buff, &nwords, &nevent);
        logMsg("TDC ERROR: extra event # %d - recovering\n",nevent,2,3,4,5,6);
        adc792getBufferStatus(tdcadr0,&value);
	  }
      if(tdcrec) logMsg("TDC ERROR: extra event - recovered\n",1,2,3,4,5,6);

      adc792getBufferStatus(adcadr0,&value1);
	  while(!(value1 & BUFFER_EMPTY))
      {
        goodtoread = 0;
        adcrec=1;
        adc792readEvent(adcadr0, buff1, &nwords1, &nevent);
        logMsg("ADC ERROR: extra event # %d - recovering\n",nevent,2,3,4,5,6);
        adc792getBufferStatus(adcadr0,&value1);
      }
      if(adcrec) logMsg("ADC ERROR: extra event - recovered\n",1,2,3,4,5,6);

      if(goodtoread)
	  {
		/* 
        logMsg("TDC: nevent=%d data0=%d (UN=%1d OV=%1d V=%1d) nwords=%d\n",
          nevent,buff[0]&0xFFF,(buff[0]>>13)&1,(buff[0]>>12)&1,(buff[0]>>14)&1,nwords);
        for(i=0; i<nwords; i++)
	    {
          logMsg("[%2d] -> 0x%08x -> slot=%d ch=%d data=%d\n",
            i,buff[i],(buff[i]>>27)&0x1F,(buff[i]>>16)&0x3F,buff[i]&0xFFF,6);
	    }

        logMsg("ADC: nevent=%d data0=%d (UN=%1d OV=%1d V=%1d) nwords=%d\n",
          nevent,buff1[0]&0xFFF,(buff1[0]>>13)&1,(buff1[0]>>12)&1,(buff1[0]>>14)&1,
           nwords1);
        for(i=0; i<nwords; i++)
	    {
          logMsg("data -> slot=%d ch=%d data=%d\n",
            (buff1[i]>>27)&0x1F,(buff1[i]>>16)&0x3F,buff1[i]&0xFFF,4,5,6);
        }
		*/
        if((ind2 = bosMopen_(jw,"CT  ",0,3,nwords)) > 0)
        {
          buf2 = (unsigned short *)&jw[ind2+1];
          for(i=0; i<nwords; i++)
          {
            tmp = buff[i];
            *buf2++ = /*i*/((tmp>>16)&0x3F)+1;
            *buf2++ = tmp&0xFFF;
            *buf2++ = buff1[i]&0xFFF; /* we assume here that we always have 32 TDC and 32 ADC channels */
          }
          rol->dabufp += bosMclose_(jw,ind2,3,nwords);
        }
      }
    }
#endif


    /*ctoftest1-related histograms*/
    if((ind2 = bosMlink_(jw,"CT  ",0)) > 0)
    {
      buf2 = (unsigned short *)&jw[ind2+1];
      for(i=0; i<nwords; i++)
      {
        k = *buf2++;
        tdc = *buf2++;
        adc = *buf2++;
        if(k <= 24)
		{
          if(tdc>0 && tdc<4000) uthfill(histi, 130+k, tdc, 0, 1);
          ttt[k] = tdc;
		}
        if(k <= 16)
		{
          if(adc>0 && adc<4000) uthfill(histi, 130+24+k, adc, 0, 1);
		}
      }
      timeres = ttt[1]+ttt[2]+ttt[5]+ttt[6]-2*(ttt[3]+ttt[4]) + 1000;
      if(timeres>0 && timeres<2000) uthfill(histi, 171, timeres, 0, 1);
    }

#ifdef USE_1182
    /* reset ADC 1182 */
    adc1182reset(adcadr[3]);
#endif

    if(timeprofile)
    {
      TIMER_STOP(100000,1000+rol->pid);
    }

    if(syncFlag==1)
    {
      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = 0x20000000;
      rol->dabufp += bosMclose_(jw,ind2,1,1);
    }
    else
    {
      ;
    }

  }


  /***********************/
  /* test signals to tdc */
  /***********************/
  /* start */
  tir[1]->tir_oport = 0x1;
  tir[1]->tir_oport = 0x0;
  /* stop */
  tir[1]->tir_oport = 0x2;
  tir[1]->tir_oport = 0x0;


/***/

  neventh ++;
  if(timeprofile)
  {
    if(neventh >= 100000)
    {
      neventh = 0;
      rol->dabufp += uth2bos(histi, 1000+rol->pid, jw);
    }
  }

  /*ctoftest1-related histograms*/
  i = neventh % 42;
  if(i==1) rol->dabufp += uth2bos(histi, 131, jw);
  if(i==2) rol->dabufp += uth2bos(histi, 132, jw);
  if(i==3) rol->dabufp += uth2bos(histi, 133, jw);
  if(i==4) rol->dabufp += uth2bos(histi, 134, jw);
  if(i==5) rol->dabufp += uth2bos(histi, 135, jw);
  if(i==6) rol->dabufp += uth2bos(histi, 136, jw);
  if(i==7) rol->dabufp += uth2bos(histi, 137, jw);
  if(i==8) rol->dabufp += uth2bos(histi, 138, jw);
  if(i==9) rol->dabufp += uth2bos(histi, 139, jw);
  if(i==10) rol->dabufp += uth2bos(histi, 140, jw);
  if(i==11) rol->dabufp += uth2bos(histi, 141, jw);
  if(i==12) rol->dabufp += uth2bos(histi, 142, jw);
  if(i==13) rol->dabufp += uth2bos(histi, 143, jw);
  if(i==14) rol->dabufp += uth2bos(histi, 144, jw);
  if(i==15) rol->dabufp += uth2bos(histi, 145, jw);
  if(i==16) rol->dabufp += uth2bos(histi, 146, jw);
  if(i==17) rol->dabufp += uth2bos(histi, 147, jw);
  if(i==18) rol->dabufp += uth2bos(histi, 148, jw);
  if(i==19) rol->dabufp += uth2bos(histi, 149, jw);
  if(i==20) rol->dabufp += uth2bos(histi, 150, jw);
  if(i==21) rol->dabufp += uth2bos(histi, 151, jw);
  if(i==22) rol->dabufp += uth2bos(histi, 152, jw);
  if(i==23) rol->dabufp += uth2bos(histi, 153, jw);
  if(i==24) rol->dabufp += uth2bos(histi, 154, jw);
  if(i==25) rol->dabufp += uth2bos(histi, 155, jw);
  if(i==26) rol->dabufp += uth2bos(histi, 156, jw);
  if(i==27) rol->dabufp += uth2bos(histi, 157, jw);
  if(i==28) rol->dabufp += uth2bos(histi, 158, jw);
  if(i==29) rol->dabufp += uth2bos(histi, 159, jw);
  if(i==30) rol->dabufp += uth2bos(histi, 160, jw);
  if(i==31) rol->dabufp += uth2bos(histi, 161, jw);
  if(i==32) rol->dabufp += uth2bos(histi, 162, jw);
  if(i==33) rol->dabufp += uth2bos(histi, 163, jw);
  if(i==34) rol->dabufp += uth2bos(histi, 164, jw);
  if(i==35) rol->dabufp += uth2bos(histi, 165, jw);
  if(i==36) rol->dabufp += uth2bos(histi, 166, jw);
  if(i==37) rol->dabufp += uth2bos(histi, 167, jw);
  if(i==38) rol->dabufp += uth2bos(histi, 168, jw);
  if(i==39) rol->dabufp += uth2bos(histi, 169, jw);
  if(i==40) rol->dabufp += uth2bos(histi, 170, jw);
  if(i==41) rol->dabufp += uth2bos(histi, 171, jw);

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
