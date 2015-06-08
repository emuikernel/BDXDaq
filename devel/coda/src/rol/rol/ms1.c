
/* ms1.c - first readout list for microstrip crate */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list MS1 */
#define ROL_NAME__ "MS1"

/* maximum 131072,100 */
#define MAX_EVENT_LENGTH 65536
#define MAX_EVENT_POOL 200

/* ts control */
#define TRIG_SUPV


/* name used by loader */
#define INIT_NAME ms1__init

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



/* ms1 */
#define CRAMS_SEQ_ADDR 0xd00000
#define CRAMS_ADDR_INC 0x010000
#define CRAMS_NUMBER   2
#define CRAMS_DET_CHAN 384
#include "c-rams.h"
extern int nCRAMS;
extern volatile struct c_rams_s_struct *crams_s;
extern volatile struct c_rams_struct *crams[10];
#define NPEDEVENTS 100
#define NPEDBINS 20000
static int pedsready;
static int pedestals[NPEDBINS];
/* ms1 */



/* from fputil.h */
#define SYNC 0x20000000


#undef DEBUG

/* CAEN v792 ADC */
#define ADCADR0  0x110000

/* global addresses for boards */
static unsigned long adcadr0;

/* main TI board */
#define TIRADR   0x0ed0

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
static int neventh1 = 0;



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
  sysBusToLocalAdrs(0x29,0,&offset);
  /*sysBusToLocalAdrs(0x29,0xFFFF0000,&offset);*/
  tir[1] = (struct vme_tir *)(offset+TIRADR);


  /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&offset);




  /* ms1 */
  /* Initialize C-RAMS Sequencer and ADC Addresses */
  cramsInit(CRAMS_SEQ_ADDR,CRAMS_ADDR_INC,CRAMS_NUMBER);
  /* ms1 */


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
  int i, ii, jj, tchan, ifull;
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

  tttest("\npolar rol1:");

  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);



  /* ms1 */

  /* Make sure All C-RAMS Pedestal and Threshold settings are Zero */
  for(ii=0;ii<CRAMS_NUMBER*2;ii+=2)
  {
    printf("zero id=%d\n",ii);
	cramsPTZero(ii);
    printf("zero id=%d\n",ii+1);
    cramsPTZero(ii+1);
  }

  /* Setup C-RAMS Sequencer and ADCs for Readout */
  cramsClear(0);

  /* Program T registers 
   T1 (0-0xff)  Delay of Hold from Trigger     (t = 500ns + T1*10ns)
   T2 (2-0x1ff) Delay of Clock from Hold       (t = 130ns + T2*20ns)
   T3 (1-T4)    Width of Clock/Convert Pulse   (t = T3*20ns)
   T4 (1-0x1ff) Period of Clock/Convert Pulses (t = 20ns + T4*20ns)
   T5/T6 (2-0x1ff) 
                   Delay of Convert from Clock (t = 40ns + T5*20ns )
      or           Delay of Convert from Test  (t = 150ns + T5*20ns)
  */
  cramsTprog(CRAMS_DET_CHAN,70,18,6,18,10);

  /* Program Number of conversion pulses to send out per ADC */
  /* This number is in units of 32 Channels */
  tchan = (CRAMS_DET_CHAN>>5);
  for(ii=0; ii<nCRAMS; ii++)
  {
    crams[ii]->channels = tchan | (tchan<<6); 
    crams[ii]->status = 2;     /* Memory owner Internal */
  }

  /* Print out status info */
  cramsStatus();

  /* microstrip histos */
  uthbook1(histi, 101, "MS PLANE 1", 384, 0, 384);
  uthbook1(histi, 102, "MS PLANE 2", 384, 0, 384);
  uthbook1(histi, 103, "MS PLANE 3", 384, 0, 384);
  uthbook1(histi, 104, "MS PLANE 4", 384, 0, 384);
  uthbook1(histi, 105, "MS PLANE 1 (SUB)", 384, 0, 384);
  uthbook1(histi, 106, "MS PLANE 2 (SUB)", 384, 0, 384);
  uthbook1(histi, 107, "MS PLANE 3 (SUB)", 384, 0, 384);
  uthbook1(histi, 108, "MS PLANE 4 (SUB)", 384, 0, 384);

  /* microstrip histos */
  uthbook1(histi, 201, "ACOL 1", 500, 0, 1000);
  uthbook1(histi, 202, "ACOL 2", 500, 0, 1000);
  uthbook1(histi, 203, "ACOL 3", 500, 0, 1000);
  uthbook1(histi, 204, "ACOL 4", 500, 0, 1000);
  uthbook1(histi, 205, "ACOL 5", 500, 0, 1000);
  uthbook1(histi, 206, "ACOL 6", 500, 0, 1000);
  uthbook1(histi, 207, "ACOL 7", 500, 0, 1000);
  uthbook1(histi, 208, "ACOL 8", 500, 0, 1000);

  pedsready = 0;

  /* ms1 */





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



  CDOENABLE(VME,1,0);

  return;
}




void
usrtrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  int *adrlen, *bufin, *bufout, i, ind, ind2, ncol, nrow, len, len1, len2;
  unsigned long tmp, *secondword, *jw, *buf, *dabufp1, *dabufp2, fifodata;
  TIMER_VAR;
  unsigned short *buf2;
  unsigned short value, value1;
  unsigned int nwords, nevent, nbcount, nbsubtract, buff[32], buff1[32];

  int id, nData, nData1, nData2, drdy, active, ind1, trigbits;
  int ii, njjloops, blen, jj, nev, rlen, nevts, nwrds;
  unsigned long res, datascan, mymask=0xfff0;
  unsigned long tdcslot, tdcchan, tdcval, tdc14, tdcedge, tdceventcount;
  unsigned long tdceventid, tdcbunchid, tdcwordcount, tdcerrorflags;
  unsigned int *tdc, *tdchead, itdcbuf;


  
  /*trigbits = tir[1]->tir_dat & 0xFFF;*/
  /*
logMsg("usrtrig reached, TI inputs = 0x%08x\n",tir[1]->tir_dat,2,3,4,5,6);
  */

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
  else if((syncFlag==1)&&(EVTYPE==0))    /* force_sync events */
  {

/*
!!! we are geting here on End transition: syncFlag=1 EVTYPE=0 !!!
*/


    /* report histogram
    if(timeprofile)
    {
      rol->dabufp += uth2bos(histi, 1000+rol->pid, jw);
    }
	*/


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
	

goto skipadc;

  /********************/
  /* v792 ADC readout */
  /********************/
  if(trigbits & 0x2)
  {
	/*logMsg("Active Colimator readout\n",1,2,3,4,5,6);*/
    /* without interrupts */
    adc792getBufferStatus(adcadr0,&value);
    /*adc792getBufferStatus(adcadr0,&value1);*/
    if( (!(value & BUFFER_EMPTY)) /*&& (!(value1 & BUFFER_EMPTY))*/ )
    {
      adc792readEvent(adcadr0, buff, &nwords, &nevent);
      /*adc792readEvent(adcadr0, buff1, &nwords1, &nevent);*/

      /*
      logMsg("nevent=%d data0=%d (UN=%1d OV=%1d V=%1d) nwords=%d\n",
        nevent,buff[0]&0xFFF,(buff[0]>>13)&1,(buff[0]>>12)&1,(buff[0]>>14)&1,nwords);
      for(i=0; i<nwords; i++)
	  {
        logMsg("[%2d] -> 0x%08x -> slot=%d ch=%d data=%d\n",
          i,buff[i],(buff[i]>>27)&0x1F,(buff[i]>>16)&0x3F,buff[i]&0xFFF,6);
	  }
	  */


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
          *buf2++ = 0; /* tdc */
          *buf2++ = tmp&0xFFF; /* adc */

          if(i>=0&&i<=7)
          {
            uthfill(histi, 200+i+1,tmp&0xFFF,0,1);
		  }
        }
        rol->dabufp += bosMclose_(jw,ind2,3,nwords);
      }
    }
  }


skipadc:



  /**********************/
  /* microstrip readout */
  /**********************/
  /*if(trigbits & 0x1) - for standalone only !!! */
  {
	/*logMsg("MS1 readout\n",1,2,3,4,5,6);*/
    /* Wait for Sequencer to finish conversions */
    ii=0;
    active = (crams_s->status)&CRAMS_S_STATUS_ACTIVE;
#ifdef DDD
logMsg("ms1: active=%d\n",active,0,0,0,0,0);
#endif
    while(active && (ii<1000))
    {
      active = (crams_s->status)&CRAMS_S_STATUS_ACTIVE;
      ii++;
    }
#ifdef DDD
logMsg("ms1: active=%d ii=%d\n",active,ii,0,0,0,0);
#endif

    if(ii>=1000)
    {
      logMsg("ms1: timeout ERROR\n",0,0,0,0,0,0);
    }

    /* If data is ready then read out all CRAMS */
    if((crams_s->status)&(CRAMS_S_STATUS_DRDY))
    {
      tir[1]->tir_oport = 0x2;

	  /* RC bank 
      nData = 0;
      for(ii=0; ii<nCRAMS; ii++)
      {
        nData1 = (crams[ii]->wc[0])&CRAMS_WC_MASK;
        nData2 = (crams[ii]->wc[1])&CRAMS_WC_MASK;
        nData += (nData1+nData2+2);
	  }

      if((ind1 = bosMopen_(jw, rcname, 0, 1, nData)) > 0)
      {
        buf = (unsigned long *)&jw[ind1+1];

        for(ii=0; ii<nCRAMS; ii++)
        {
          id = 2*ii;

          nData1 = (crams[ii]->wc[0])&CRAMS_WC_MASK;
          *buf++ = (CRAMS_DATA_HEADER | (id<<16) | nData1);
          for(jj=0; jj<nData1; jj++)
          {
            *buf++ = crams[ii]->fifo[0];
          }

          nData2 = (crams[ii]->wc[1])&CRAMS_WC_MASK;
          *buf++ = (CRAMS_DATA_HEADER | ((id+1)<<16) | nData2);
          for(jj=0; jj<nData2; jj++)
          {
            *buf++ = crams[ii]->fifo[1];
          }
        }

        rol->dabufp += bosMclose_(jw,ind1,1,nData);
	  }
      */


      /* MS bank */
      nData = 0;
      for(ii=0; ii<nCRAMS; ii++)
      {
        nData1 = (crams[ii]->wc[0])&CRAMS_WC_MASK;
        nData2 = (crams[ii]->wc[1])&CRAMS_WC_MASK;
        nData += (nData1+nData2);
#ifdef DDD
logMsg("ms1: [%1d]-> %d %d %d\n",ii,nData,nData1,nData2,0,0);
#endif
	  }

      if((ind1 = bosMopen_(jw, "MS1 ", 0, 2, nData)) > 0)
      {
        buf2 = (unsigned short *)&jw[ind1+1];

        id=-1;
        for(ii=0; ii<nCRAMS; ii++)
        {

          id ++;
		  /***/

          nData1 = (crams[ii]->wc[0])&CRAMS_WC_MASK;
#ifdef DDD
logMsg("ms1: nData1=%d\n",nData1,0,0,0,0,0);
#endif
          for(jj=0; jj<nData1; jj++)
          {
            fifodata = crams[ii]->fifo[0];
            *buf2++ = ((fifodata>>12)&0x7FF) + (id<<12);
            *buf2++ = fifodata&0xFFF;

            uthfill(histi, 100+id+1,((fifodata>>12)&0x7FF),0,fifodata&0xFFF);


            if(pedsready) /* subtract pedestals */
            {
              uthfill(histi, 100+id+5,((fifodata>>12)&0x7FF),0,
                      (fifodata&0xFFF) - pedestals[*(buf2-2)] + 100);
			}
            else /* accumulate pedestals */
            {
              pedestals[*(buf2-2)] += *(buf2-1);
            }

          }

		  /***/
          id ++;
		  /***/


		  /***/


          nData2 = (crams[ii]->wc[1])&CRAMS_WC_MASK;
#ifdef DDD
logMsg("ms1: nData2=%d\n",nData2,0,0,0,0,0);
#endif
          for(jj=0; jj<nData2; jj++)
          {
            fifodata = crams[ii]->fifo[1];
            *buf2++ = ((fifodata>>12)&0x7FF) + ((id)<<12);
            *buf2++ = fifodata&0xFFF;

            uthfill(histi, 100+id+1, ((fifodata>>12)&0x7FF),0,fifodata&0xFFF);

            if(pedsready) /* subtract pedestals */
            {
              uthfill(histi, 100+id+5,((fifodata>>12)&0x7FF),0,
                      (fifodata&0xFFF) - pedestals[*(buf2-2)] + 100);
			}
            else /* accumulate pedestals */
            {
              pedestals[*(buf2-2)] += *(buf2-1);
            }
          }

		  /***/


        }

        rol->dabufp += bosMclose_(jw,ind1,2,nData);
	  }


	  /* pedestals */
      if( (*(rol->nevents)) == NPEDEVENTS)
      {
        for(i=0; i<NPEDBINS; i++)
        {
          pedestals[i] /= NPEDEVENTS;
		}
        logMsg("pedestals are ready ! %d %d %d %d %d %d\n",
pedestals[0],pedestals[1],pedestals[2],pedestals[3],pedestals[4],pedestals[5]);
        pedsready = 1;
      }



      tir[1]->tir_oport = 0x0;
    }
    else /* Output an error word */
    {
      logMsg("ms1: ERROR: data is not ready !\n",0,0,0,0,0,0);
    }
  }




goto skipms1;

    /* for physics sync event, make sure all board buffers are empty */
    if(syncFlag==1)
    {
int nboards = 1;
int error_flag[21];

      int scan_flag;
      unsigned short slot, nev16;

      scan_flag = 0;
      for(ii=0; ii<nboards; ii++)
      {
        nev = nev16;

        if(nev != 0) /* clear board if extra event */
        {
          logMsg("SYNC: ERROR: [%2d] slot=%2d nev=%d - clear\n",
            ii,slot,nev,4,5,6);
          scan_flag |= (1<<slot);
        }

        if(error_flag[slot] == 1) /* clear board if error flag was set */
        {
		  /*
          logMsg("SYNC: ERROR: [%2d] slot=%2d error_flag=%d - clear\n",
            ii,slot,error_flag[slot],4,5,6);
		  */
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

skipms1:




	
    if(timeprofile)
    {
      TIMER_STOP(100000,1000+rol->pid);
    }
	

  }


/* ms1 */
  neventh1 ++;

/* comment following line out if want single event histograms */
/*if(neventh1 >= 100)*/
  {
    neventh1 = 0;
    rol->dabufp += uth2bos(histi, 101, jw);
    rol->dabufp += uth2bos(histi, 102, jw);
    rol->dabufp += uth2bos(histi, 103, jw);
    rol->dabufp += uth2bos(histi, 104, jw);

    uthreset(histi, 101, " ");
    uthreset(histi, 102, " ");
    uthreset(histi, 103, " ");
    uthreset(histi, 104, " ");

    rol->dabufp += uth2bos(histi, 105, jw);
    rol->dabufp += uth2bos(histi, 106, jw);
    rol->dabufp += uth2bos(histi, 107, jw);
    rol->dabufp += uth2bos(histi, 108, jw);

    uthreset(histi, 105, " ");
    uthreset(histi, 106, " ");
    uthreset(histi, 107, " ");
    uthreset(histi, 108, " ");

    rol->dabufp += uth2bos(histi, 201, jw);
    rol->dabufp += uth2bos(histi, 202, jw);
    rol->dabufp += uth2bos(histi, 203, jw);
    rol->dabufp += uth2bos(histi, 204, jw);
    rol->dabufp += uth2bos(histi, 205, jw);
    rol->dabufp += uth2bos(histi, 206, jw);
    rol->dabufp += uth2bos(histi, 207, jw);
    rol->dabufp += uth2bos(histi, 208, jw);

    uthreset(histi, 201, " ");
    uthreset(histi, 202, " ");
    uthreset(histi, 203, " ");
    uthreset(histi, 204, " ");
    uthreset(histi, 205, " ");
    uthreset(histi, 206, " ");
    uthreset(histi, 207, " ");
    uthreset(histi, 208, " ");

  }
/* ms1 */

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
  tirtack(0);
  /*CDOACK(VME,1,0);*/

  return;
}  

static void
__status()
{
  return;
}  
