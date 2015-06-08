
/* wolfrol1.c - first readout list for 'wolfram' test setup */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list SFI */
#define ROL_NAME__ "WOLFROL1"

/* ts control */
/*#define TRIG_SUPV*/

/* name used by loader */
#define INIT_NAME wolfrol1__init

#include "rol.h"

/* vme readout */
#include "VME_source.h" /* POLLING_MODE for tir_triglib.h undefined here */


/* user code */

#include "uthbook.h"
#include "coda.h"
#include "tt.h"



/* main TI board */
#define TIRADR   0x0ed0

#define NADCS 3/*4*/
/* lecroy 1182 ADCs */
#define ADCADR1  0x110000
#define ADCADR2  0x120000
#define ADCADR3  0x130000
#define ADCADR4  0x140000

#define SCALADR1  0x50000

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
static unsigned long adcadr[4];

static unsigned long scaladr;

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


  adcadr[0] = offset + ADCADR1;
  adcadr[1] = offset + ADCADR2;
  adcadr[2] = offset + ADCADR3;
  adcadr[3] = offset + ADCADR4;
  printf("rol1: adcadr = 0x%08x\n",adcadr[0],adcadr[1],adcadr[2],adcadr[3]);

  scaladr = offset + SCALADR1;
  printf("rol1: scaladr = 0x%08x\n",scaladr);

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


  /*wolfram-related histograms*/
  for(i=0; i<8; i++) uthbook1(histi, 132+i, "wolfram", 1000, 0, 4000);


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


  /* adc1182 reset */
  for(i=0; i<NADCS; i++) adc1182reset(adcadr[i]);

  /* reset scaler */
  scaler560clear(scaladr);
  scaler560resetVETO(scaladr);


  CDOENABLE(VME,1,0);

  return;
}




void
usrtrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  int i, j, k, id1, id2, ind, ind2, ncol, nrow, len, len1, len2;
  unsigned long tmp, *secondword, *jw, *buf, *dabufp1, *dabufp2;
  TIMER_VAR;
  unsigned short *buf2, tmp2;
  unsigned short value, value1;
  unsigned int nwords, nwords1, nevent, buff[32];
  unsigned int scalbuf[16];

  int ii, njjloops, jj, nev, rlen, rlenbuf[NBOARDS];
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


/*
logMsg("wolfrol1 trigger reached\n",1,2,3,4,5,6);
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



    if( !((*(rol->nevents)) % 1000) )
    {
      /* read scaler */
      scaler560read(scaladr, scalbuf);

      logMsg("reading scaler 0x%08x -> %d %d %d %d %d\n",
        scaladr,scalbuf[0],scalbuf[1],scalbuf[2],scalbuf[3],scalbuf[4]);

      /* reset scaler */
      scaler560clear(scaladr);
      scaler560resetVETO(scaladr);
    }








	/*
goto skipall;
	*/

    /********************/
    /* 1182 adc readout */
    /********************/

    /* wait for all ADCs to finish conversion */
    for(i=0; i<NADCS; i++) while( adc1182cip(adcadr[i]) );

    nwords = 32;
    if((ind2 = bosMopen_(jw,"STN1",0,2,nwords)) > 0)
    {
      buf2 = (unsigned short *)&jw[ind2+1];
      id1 = 0;
      k = 0;
      for(i=0; i<NADCS; i++)
      {
        for(j=0; j<8; j++)
        {

/* id: 257, 258, 259, 260, 513, 514, ... */
/*
          id2 = (j % 4) + 1;
          if(id2 == 1) id1 += 256;
          *buf2++ = id1 + id2; 
*/
          *buf2++ = i*8 + j + 1;
          tmp2 = adc1182read(adcadr[i], j);
          *buf2++ = tmp2 & 0xFFF;

          /*wolfram-related histograms*/
          buff[k++] = tmp2 & 0xFFF;

        }
	  }
      rol->dabufp += bosMclose_(jw,ind2,2,nwords);

      /*wolfram-related histograms*/
      for(k=0; k<8; k++)
      {
        if(buff[k]>0&&buff[k]<4000) uthfill(histi, 132+k, buff[k], 0, 1);
      }

    }

    /* reset all ADC boards */
    for(i=0; i<NADCS; i++) adc1182reset(adcadr[i]);




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


  skipall:
	;

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

/*
goto nohist;
*/

  neventh ++;
  if(timeprofile)
  {
    if(neventh >= 100000)
    {
      neventh = 0;
      rol->dabufp += uth2bos(histi, 1000+rol->pid, jw);
    }
  }

  /*wolfram-related histograms*/
  i = neventh % 200;
  if(i>=0 && i<8) rol->dabufp += uth2bos(histi, 132+i, jw);

nohist:

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
