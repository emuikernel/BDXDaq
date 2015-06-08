
/* caenrol1.c - first readout list for CAEN boards testing in VME crates */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list SFI */
#define ROL_NAME__ "CAENROL1"

/* ts control */
/*#define TRIG_SUPV*/

/* name used by loader */
#define INIT_NAME caenrol1__init

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



/* main TI board */
#define TIRADR   0x0ed0
/* CAEN v775 TDC */
#define TDCADR0  0x610000
/* CAEN v792 ADC */
#define ADCADR0  0x510000

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
  tdcadr0 = offset + TDCADR0;
  printf("rol1: tdcadr0 = 0x%08x\n",tdcadr0);
  adcadr0 = offset + ADCADR0;
  printf("rol1: adcadr0 = 0x%08x\n",adcadr0);

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
  unsigned short value, value1;
  unsigned int nwords, nwords1, nevent, nbcount, nbsubtract, buff[32], buff1[32];

  int ii, njjloops, blen, jj, nev, rlen, rlenbuf[NBOARDS];
  unsigned long res;
  unsigned long tdcslot, tdcchan, tdcval, tdc14, tdcedge, tdceventcount;
  unsigned long tdceventid, tdcbunchid, tdcwordcount, tdcerrorflags;

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




    /*************************/
    /* v775 and v792 readout */
    /*************************/

    /* without interrupts */
    adc792getBufferStatus(tdcadr0,&value);
    /*adc792getBufferStatus(adcadr0,&value1);*/
    if( (!(value & BUFFER_EMPTY)) && (!(value1 & BUFFER_EMPTY)) )
    {
      adc792readEvent(tdcadr0, buff, &nwords, &nevent);
      /*adc792readEvent(adcadr0, buff1, &nwords1, &nevent);*/

      
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

      if((ind2 = bosMopen_(jw,"CC  ",5,3,nwords)) > 0)
      {
        buf2 = (unsigned short *)&jw[ind2+1];
        for(i=0; i<nwords; i++)
        {
          tmp = buff[i];
          *buf2++ = i/*(tmp>>16)&0x3F*/;
          *buf2++ = tmp&0xFFF;
          *buf2++ = buff1[i]&0xFFF;
        }
        rol->dabufp += bosMclose_(jw,ind2,3,nwords);
      }
    }



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

  /*adc792reset(tdcadr0);*/
  /*adc792clear(tdcadr0);*/


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
