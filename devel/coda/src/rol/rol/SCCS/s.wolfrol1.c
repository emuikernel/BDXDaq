h02640
s 00056/00008/00457
d D 1.8 10/09/27 09:58:43 boiarino 8 7
c *** empty log message ***
e
s 00005/00001/00460
d D 1.7 10/03/09 12:45:02 boiarino 7 6
c *** empty log message ***
e
s 00001/00001/00460
d D 1.6 08/10/26 22:10:06 boiarino 6 5
c *** empty log message ***
e
s 00001/00001/00460
d D 1.5 07/09/27 20:19:28 boiarino 5 4
c *** empty log message ***
e
s 00002/00002/00459
d D 1.4 07/09/27 20:17:03 boiarino 4 3
c *** empty log message ***
e
s 00022/00001/00439
d D 1.3 07/09/27 19:24:07 boiarino 3 2
c add wolfram histograms
c 
e
s 00055/00150/00385
d D 1.2 07/09/18 12:11:27 boiarino 2 1
c first version
c 
e
s 00535/00000/00000
d D 1.1 07/09/18 09:48:06 boiarino 1 0
c date and time created 07/09/18 09:48:06 by boiarino
e
u
U
f e 0
t
T
I 1

D 2
/* caenrol1.c - first readout list for CAEN boards testing in VME crates */
E 2
I 2
/* wolfrol1.c - first readout list for 'wolfram' test setup */
E 2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list SFI */
D 2
#define ROL_NAME__ "CAENROL1"
E 2
I 2
#define ROL_NAME__ "WOLFROL1"
E 2

/* ts control */
/*#define TRIG_SUPV*/

/* name used by loader */
D 2
#define INIT_NAME caenrol1__init
E 2
I 2
#define INIT_NAME wolfrol1__init
E 2

#include "rol.h"

/* vme readout */
#include "VME_source.h" /* POLLING_MODE for tir_triglib.h undefined here */


/* user code */

#include "uthbook.h"
#include "coda.h"
#include "tt.h"
D 2
#include "scaler7201.h"
#include "adc792.h"
#include "tdc890.h"
E 2



/* main TI board */
#define TIRADR   0x0ed0
I 8

#define NADCS 3/*4*/
E 8
D 2
/* CAEN v775 TDC */
#define TDCADR0  0x610000
/* CAEN v792 ADC */
#define ADCADR0  0x510000
E 2
I 2
/* lecroy 1182 ADCs */
#define ADCADR1  0x110000
#define ADCADR2  0x120000
#define ADCADR3  0x130000
#define ADCADR4  0x140000
E 2

I 8
#define SCALADR1  0x50000
E 8
I 2

E 2
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
D 2
static unsigned long tdcadr0, adcadr0;
E 2
I 2
static unsigned long adcadr[4];
E 2

I 8
static unsigned long scaladr;

E 8
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


D 6
#define NBOARDS 21    /* maximum number of VME boards */
E 6
I 6
#define NBOARDS 22    /* maximum number of VME boards */
E 6






/* standart functions */


static void
__download()
{
  unsigned long offset, ii;

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");


  /* Must check address in TI module */
  /* TI uses A16 (0x29, 4 digits), not A24 */

  /* get global offset for A16 */
  sysBusToLocalAdrs(0x29,0,&offset);
  /*sysBusToLocalAdrs(0x29,0xFFFF0000,&offset);*/
  tir[1] = (struct vme_tir *)(offset+TIRADR);


  /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&offset);
  /*sysBusToLocalAdrs(0x39,0xFF000000,&offset);*/
D 2
  tdcadr0 = offset + TDCADR0;
  printf("rol1: tdcadr0 = 0x%08x\n",tdcadr0);
  adcadr0 = offset + ADCADR0;
  printf("rol1: adcadr0 = 0x%08x\n",adcadr0);
E 2

I 2

  adcadr[0] = offset + ADCADR1;
  adcadr[1] = offset + ADCADR2;
  adcadr[2] = offset + ADCADR3;
  adcadr[3] = offset + ADCADR4;
  printf("rol1: adcadr = 0x%08x\n",adcadr[0],adcadr[1],adcadr[2],adcadr[3]);

I 8
  scaladr = offset + SCALADR1;
  printf("rol1: scaladr = 0x%08x\n",scaladr);

E 8
E 2
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

  /* init trig source VME */
  VME_INIT;

  /* Register an async trigger source, (up to 32 sources) */
  /* CTRIGRSA(source,code,handler,done)*/
  /* link async trig source VME 1 to usrtrig and usrtrig_done */
  CTRIGRSA(VME, 1, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */

  printf("===================> rol->pid=%d\n",rol->pid);
  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);


D 2


  /* CAEN v775 TDC program start */

  /* board initialization */
  adc792boardInit(tdcadr0);


  /*********************************/
  /* for non-PAUX version only !!! */
  adc792setGEOAddress(tdcadr0, 4);
  adc792setLogic(tdcadr0, SOFT_RESET);
  adc792resetLogic(tdcadr0, SOFT_RESET);
  /*********************************/
  /*********************************/


  /*adc792resetOperationMode(tdcadr0, START_STOP);*/ /* Common Start mode */
  adc792setOperationMode(tdcadr0, START_STOP); /* Common Stop mode */

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




  /* CAEN v792 ADC start */

  printf("ADC===\n");

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




E 2
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

I 3

  /*wolfram-related histograms*/
D 8
  for(i=0; i<32; i++) uthbook1(histi, 132+i, "wolfram", 400, 0, 4000);
E 8
I 8
  for(i=0; i<8; i++) uthbook1(histi, 132+i, "wolfram", 1000, 0, 4000);
E 8


E 3
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
  /*scaler7201intdisable(scaler0);*/

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

I 8

E 8
I 2
  /* adc1182 reset */
D 8
  for(i=0; i<4; i++) adc1182reset(adcadr[i]);
E 8
I 8
  for(i=0; i<NADCS; i++) adc1182reset(adcadr[i]);
E 8

I 8
  /* reset scaler */
  scaler560clear(scaladr);
  scaler560resetVETO(scaladr);
E 8

I 8

E 8
E 2
  CDOENABLE(VME,1,0);

  /* from parser */
  if(__the_event__) WRITE_EVENT_;

  return;
}




void
usrtrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
D 2
  int *adrlen, *bufin, *bufout, i, ind, ind2, ncol, nrow, len, len1, len2;
E 2
I 2
D 3
  int i, j, id1, id2, ind, ind2, ncol, nrow, len, len1, len2;
E 3
I 3
  int i, j, k, id1, id2, ind, ind2, ncol, nrow, len, len1, len2;
E 3
E 2
  unsigned long tmp, *secondword, *jw, *buf, *dabufp1, *dabufp2;
  TIMER_VAR;
D 2
  unsigned short *buf2;
E 2
I 2
  unsigned short *buf2, tmp2;
E 2
  unsigned short value, value1;
D 2
  unsigned int nwords, nwords1, nevent, nbcount, nbsubtract, buff[32], buff1[32];
E 2
I 2
  unsigned int nwords, nwords1, nevent, buff[32];
I 8
  unsigned int scalbuf[16];
E 8
E 2

D 7
  int ii, njjloops, blen, jj, nev, rlen, rlenbuf[NBOARDS];
E 7
I 7
  int ii, njjloops, jj, nev, rlen, rlenbuf[NBOARDS];
E 7
  unsigned long res;
D 2
  unsigned long tdcslot, tdcchan, tdcval, tdc14, tdcedge, tdceventcount;
  unsigned long tdceventid, tdcbunchid, tdcwordcount, tdcerrorflags;
E 2

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

I 8

/*
logMsg("wolfrol1 trigger reached\n",1,2,3,4,5,6);
*/

E 8
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
I 7
  else if((syncFlag==0)&&(EVTYPE==15)) /* helicity strob events */
  {
    ;
  }
E 7
  else           /* physics and physics_sync events */
  {


    if(timeprofile)
    {
      TIMER_NORMALIZE;
      TIMER_START;
    }



I 8
    if( !((*(rol->nevents)) % 1000) )
    {
      /* read scaler */
      scaler560read(scaladr, scalbuf);
E 8

I 8
      logMsg("reading scaler 0x%08x -> %d %d %d %d %d\n",
        scaladr,scalbuf[0],scalbuf[1],scalbuf[2],scalbuf[3],scalbuf[4]);
E 8
D 2
    /*************************/
    /* v775 and v792 readout */
    /*************************/
E 2

I 8
      /* reset scaler */
      scaler560clear(scaladr);
      scaler560resetVETO(scaladr);
    }
E 8
D 2
    /* without interrupts */
    adc792getBufferStatus(tdcadr0,&value);
    /*adc792getBufferStatus(adcadr0,&value1);*/
    if( (!(value & BUFFER_EMPTY)) && (!(value1 & BUFFER_EMPTY)) )
    {
      adc792readEvent(tdcadr0, buff, &nwords, &nevent);
      /*adc792readEvent(adcadr0, buff1, &nwords1, &nevent);*/
E 2

D 2
      
      logMsg("nevent=%d data0=%d (UN=%1d OV=%1d V=%1d) nwords=%d\n",
        nevent,buff[0]&0xFFF,(buff[0]>>13)&1,(buff[0]>>12)&1,(buff[0]>>14)&1,nwords);
      for(i=0; i<nwords; i++)
	  {
        logMsg("[%2d] -> 0x%08x -> slot=%d ch=%d data=%d\n",
          i,buff[i],(buff[i]>>27)&0x1F,(buff[i]>>16)&0x3F,buff[i]&0xFFF,6);
	  }
	  /*
      logMsg("nevent=%d data0=%d (UN=%1d OV=%1d V=%1d) nwords=%d\n",
        nevent,buff1[0]&0xFFF,(buff1[0]>>13)&1,(buff1[0]>>12)&1,(buff1[0]>>14)&1,
         nwords1);
      logMsg("data -> slot=%d ch=%d data=%d\n",
        (buff1[2]>>27)&0x1F,(buff1[2]>>16)&0x3F,buff1[2]&0xFFF,4,5,6);
      */
E 2

D 2
      if((ind2 = bosMopen_(jw,"CC  ",5,3,nwords)) > 0)
E 2
I 2






I 8
	/*
goto skipall;
	*/

E 8
    /********************/
    /* 1182 adc readout */
    /********************/

    /* wait for all ADCs to finish conversion */
D 8
    for(i=0; i<4; i++) while( adc1182cip(adcadr[i]) );
E 8
I 8
    for(i=0; i<NADCS; i++) while( adc1182cip(adcadr[i]) );
E 8

    nwords = 32;
    if((ind2 = bosMopen_(jw,"STN1",0,2,nwords)) > 0)
    {
      buf2 = (unsigned short *)&jw[ind2+1];
      id1 = 0;
I 3
      k = 0;
E 3
D 8
      for(i=0; i<4; i++)
E 8
I 8
      for(i=0; i<NADCS; i++)
E 8
E 2
      {
D 2
        buf2 = (unsigned short *)&jw[ind2+1];
        for(i=0; i<nwords; i++)
E 2
I 2
        for(j=0; j<8; j++)
E 2
        {
I 8

/* id: 257, 258, 259, 260, 513, 514, ... */
/*
E 8
D 2
          tmp = buff[i];
          *buf2++ = i/*(tmp>>16)&0x3F*/;
          *buf2++ = tmp&0xFFF;
          *buf2++ = buff1[i]&0xFFF;
E 2
I 2
          id2 = (j % 4) + 1;
          if(id2 == 1) id1 += 256;
D 8
          *buf2++ = id1 + id2; /* id: 257, 258, 259, 260, 513, 514, ... */
E 8
I 8
          *buf2++ = id1 + id2; 
*/
          *buf2++ = i*8 + j + 1;
E 8
          tmp2 = adc1182read(adcadr[i], j);
          *buf2++ = tmp2 & 0xFFF;
I 3

          /*wolfram-related histograms*/
          buff[k++] = tmp2 & 0xFFF;

E 3
E 2
        }
D 2
        rol->dabufp += bosMclose_(jw,ind2,3,nwords);
      }
E 2
I 2
	  }
      rol->dabufp += bosMclose_(jw,ind2,2,nwords);
I 3

      /*wolfram-related histograms*/
D 8
      for(k=0; k<32; k++)
E 8
I 8
      for(k=0; k<8; k++)
E 8
      {
        if(buff[k]>0&&buff[k]<4000) uthfill(histi, 132+k, buff[k], 0, 1);
      }

E 3
E 2
    }

I 2
    /* reset all ADC boards */
D 8
    for(i=0; i<4; i++) adc1182reset(adcadr[i]);
E 8
I 8
    for(i=0; i<NADCS; i++) adc1182reset(adcadr[i]);
E 8
E 2


I 2


E 2
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

I 8

  skipall:
	;

E 8
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

I 8
/*
goto nohist;
*/

E 8
  neventh ++;
  if(timeprofile)
  {
    if(neventh >= 100000)
    {
      neventh = 0;
      rol->dabufp += uth2bos(histi, 1000+rol->pid, jw);
    }
  }

I 3
  /*wolfram-related histograms*/
D 4
  i = neventh % 100;
  if(i>=0 && i<32) rol->dabufp += uth2bos(histi, 132+i, jw);
E 4
I 4
D 5
  i = neventh % 1000;
E 5
I 5
  i = neventh % 200;
E 5
D 8
  if(i>=0 && i<8/*32*/) rol->dabufp += uth2bos(histi, 132+i, jw);
E 8
I 8
  if(i>=0 && i<8) rol->dabufp += uth2bos(histi, 132+i, jw);
E 8
E 4

I 8
nohist:

E 8
E 3
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

D 2
  /*adc792reset(tdcadr0);*/
  /*adc792clear(tdcadr0);*/
E 2


  CDODISABLE(VME,1,0);

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
