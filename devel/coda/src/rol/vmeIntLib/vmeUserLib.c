/*
#define PRIMEX
*/

/* if event rate goes higher then 10kHz, with random triggers we have wrong
slot number reported in GLOBAL HEADER and/or GLOBAL TRAILER words; to work
around that problem temporary patches were applied - until fixed (Sergey) */
#define SLOTWORKAROUND

/*************************************************************************
 *
 *   vmeUserLib.c  - Library of routines for the user to write for 
 *                   Aysyncronous readout and buffering of events
 *                   from VME. Use Universe Chip DMA Engine 
 *
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "vmeLib.h"

#include "../sfiDmaLib/dmaPList.h"

#include "uthbook.h"
#include "coda.h"
#include "tt.h"
#include "scaler7201.h"
#include "adc792.h"
#include "tdc890.h"


#include "circbuf.h" /* just to get the number of bufs and buffer sizes */


/* maximum 131072,100 */
#define BUFSIZE_IN_BYTES1 MAX_EVENT_LENGTH/*65536*/
#define NUM_BUFFERS1 MAX_EVENT_POOL/*200*/

/* 0: External Mode   1:Trigger Supervisor Mode */
#define TS_MODE  1

/* Define Interrupt source and address */
#define TIR_ADDR 0x0ed0
#define IRQ_SOURCE TIR_SOURCE

/* from Bank...h */
#define BT_UI4_ty  0x01

/* from fputil.h */
#define SYNC 0x20000000


#undef DEBUG

/**********/
/* locals */

static int syncFlag, lateFail, type;
static int *jw, *dabufp1, *dabufp2, ind;

static int rocid = 0;
static char *rcname = "RC00";
/* time profiling: 0-OFF, 1-ON */
static int timeprofile = 1;
/* histogram area */
static UThisti histi[NHIST];
static int neventh = 0;

static int nevents;

#define NBOARDS 22    /* maximum number of VME boards: we have 21 boards, but numbering starts from 1 */


/* v1190 start */

#define MY_MAX_EVENT_LENGTH 3000 /* max words per v1190 board */
#define TDC_OFFSET 0
#define CH_OFFSET  0

static int nboards;
static unsigned int tdcadr[NBOARDS];
static unsigned int tdcbaseadr;
static int maxbytes = 0;
static int error_flag[NBOARDS]; /* element can be 21 */
#ifdef SLOTWORKAROUND
static int slotnums[NBOARDS]; /* used from element 0 */
#endif

static int rlenbuf[NBOARDS];
static unsigned int tdcbuftmp[NBOARDS*MY_MAX_EVENT_LENGTH+16];
static unsigned int *tdcbuf;

#define NTICKS 1000 /* the number of ticks per second */

void
tsleep(int n)
{
  taskDelay ((sysClkRateGet() / NTICKS) * n);
}

/* v1190 end */


/*************/
/* externals */

extern int rocId();

/* pointer to TTS structures for current roc */
extern TTSPtr ttp;

/* Input and Output Partions for VME Readout */
volatile ROL_MEM_ID vmeIN, vmeOUT;

/* system clock */
extern int vxTicks;




/* Initialize Memory partitions for managing events */
void
vmeUserInit()
{
  unsigned int res, laddr;

  /* Setup Buffer memory to store events */
  dmaPFreeAll();
  vmeIN  = dmaPCreate("vmeIN", BUFSIZE_IN_BYTES1, NUM_BUFFERS1, 0);
  vmeOUT = dmaPCreate("vmeOUT",0,0,0);

  dmaPStatsAll();
}


void
vmeUserDownload()
{
  unsigned long offset, ii;
  short tdata, ldata;
  char tname[100];

  /* Reinitialize the Buffer memory */
  dmaPReInitAll();

  /* Initialize VME Interrupt interface - use defaults */
  vmeIntInit(TIR_ADDR,0,0,0);

  /* Connect User Trigger Routine */
  vmeIntConnect(vmeUserTrigger,0/*Sergey: IRQ_SOURCE ???*/);



  /**************/
  /* CLAS stuff */

  utBootGetTargetName(tname,99);
  printf("target name >%s<\n",tname);

  rocid = rocId();
  printf("rocid = %d\n",rocid);

  printf("rol1: downloading DDL table (clonbanks_) ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");


  /* if coda_proc (which is ROL2) is running on PMC, we have to get translation tables ourself */
  if(getproconpmc())
  {
    printf("rol2: downloading DDL table (clonbanks) ...\n");
    clonbanks();
    printf("rol2: ... done.\n");

    printf("rol2: downloading translation table for roc=%d (ttp=0x%08x)\n",rocid,ttp);
    ttp = TT_LoadROCTT(rocid, ttp);
    printf("rol2: ... done.\n");
  }


  /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&offset);

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






  /* returns 'nboards' and 'tdcadr' */


  /* v1190/v1290 TDC setup (see tdc890.h for possible options) */
  /* CBLT ROCs: 20-ec3, 21-ec4, 22-sc2, 24-lac2, 27-dvcs2 */
  if(rocid==20||rocid==21||rocid==22||rocid==24||rocid==27)
  {
    /*tdc1190ScanCLAS(CLAS_A32_MBLT_CBLT, &nboards, tdcadr);*/
    tdc1190ScanCLAS(CLAS_A32_MBLT_FIFO, &nboards, tdcadr);    
    /*tdc1190ScanCLAS(CLAS_A32_2eSST_FIFO, &nboards, tdcadr);*/
    printf("-------========-------> tdcbaseadr=0x%08x\n",tdcbaseadr);
  }
  else if(rocid==28||rocid==29) /* 28-tage3, 29-tage2 */
  {
    tdc1190ScanCLAS(CLAS_A32_2eSST_FIFO, &nboards, tdcadr); /**/
  }
#ifdef PRIMEX
  else if(rocid==16||rocid==17||rocid==18||rocid==19) /* 16-tage2, 17-tage3, 18-primextdc1, 19-tage */
  {
    tdc1190ScanCLAS(CLAS_A32_2eSST_FIFO, &nboards, tdcadr);
  }
#endif
  else if(rocid==0) /*croctest2*/
  {

    /*tdc1190ScanCLAS(CLAS_A32_MBLT_CBLT, &nboards, tdcadr);*/

	/*firmware 1.2, numbers for 2 v1190 boards: 768 bytes data plus headers (in brackets - headers only)*/
	
    tdc1190ScanCLAS(CLAS_A32_MBLT_FIFO, &nboards, tdcadr); /*6100: board-by-board:46(30)us,list:32(16)us*/ /*50MB/s*/

	/*tdc1190ScanCLAS(CLAS_A32_2eVME_FIFO, &nboards, tdcadr);*/ /*6100: board-by-board:39(30)us,list:26(16)us*/ /*80MB/s*/
                                          /*firmware1.3->*/ /*6100: board-by-board:38(30)us,list:24(16)us*/ /*96MB/s*/

	/*tdc1190ScanCLAS(CLAS_A32_2eVME, &nboards, tdcadr);*/      /*6100: board-by-board:58(51)us,list:n/a*/ /*100MB/s??*/


/*firmware 0.6*/
    /*tdc1190ScanCLAS(CLAS_A32_2eSST_FIFO, &nboards, tdcadr);*/ /**/
    /*tdc1190ScanCLAS(CLAS_A32_2eSST, &nboards, tdcadr);*/ /**/


    printf("-------========-------> tdcbaseadr=0x%08x\n",tdcbaseadr);
  }
  else
  {
    printf("Use MBLT method for ROC # %d\n",rocid);
    tdc1190ScanCLAS(CLAS_A32_MBLT_FIFO, &nboards, tdcadr);
    printf("-------========-------> tdcbaseadr=0x%08x\n",tdcbaseadr);
  }

  printf("vmerol1: nboards=%d\n",nboards);
  for(ii=0; ii<nboards; ii++)
  {
    printf("vmerol1: tdcadr[%2d] = 0x%08x\n",ii,tdcadr[ii]);
  }





  /* L2 delayed against L1 on 3320ns, so offset=-3500ns and window=400ns */ 

  /* window parameters:   width  offset */
  if(rocid == 29)
  {
    /*tdc1190SetGlobalWindow(1350,  -4050 );*/
    tdc1190SetGlobalWindow(1450,  -4050 );
  }
  else if(rocid == 28)
  {
    /*tdc1190SetGlobalWindow(1350,  -4050 );*/
    tdc1190SetGlobalWindow(1450,  -4050 );
  }
  else if(rocid == 20)
  {
    tdc1190SetGlobalWindow(1200,  -3800 );
  }
  else if(rocid == 21)
  {
    tdc1190SetGlobalWindow(1000,  -3600 );
  }
  else if(rocid == 22)
  {
    tdc1190SetGlobalWindow(1100,  -3600 );
  }
  else if(rocid == 27)
  {
    tdc1190SetGlobalWindow( 1500,  -4500 );
  }
#ifdef PRIMEX
  /*10000, -9000 - for compton, 3000, -2000 for production*/
  else if(rocid == 19) /*primex tage*/
  {
    tdc1190SetGlobalWindow( 3000,  -2000 );
  }
  else if(rocid == 16) /*primex tage2*/
  {
    tdc1190SetGlobalWindow( 3000,  -2000 );
  }
  else if(rocid == 17) /*primex tage3*/
  {
    tdc1190SetGlobalWindow( 3000,  -2000 );
  }
  else if(rocid == 18) /*primex primextdc1*/
  {
    /*tdc1190SetGlobalWindow( 2500,  -1500 );*/
    tdc1190SetGlobalWindow( 2000,  -1000 );
    /*tdc1190SetGlobalWindow( 100,  -200 ); good*/
    /*tdc1190SetGlobalWindow( 400,  -200 ); slot 19 tdc 00 occationally, err=0x1000 */
    /*tdc1190SetGlobalWindow( 600,  -300 ); slots 17(01),18(00),19(00), err=0x1000*/
  }
#endif
  else
  {
    tdc1190SetGlobalWindow(1500,  -3900 );
  }

  printf("ttp=0x%08x\n",ttp);

  tdc1190InitCLAS(nboards, tdcadr, &tdcbaseadr);



  /* trying to limit the number of stuff from sc2 - test 21-may-2007 */
  if(rocid==20||rocid==21||rocid==22||rocid == 0)
  {
    for(ii=0; ii<nboards; ii++)
    {
	  /*
      tdata = 128;
      tdc890SetMaxNumberOfHitsPerEvent(tdcadr[ii], tdata);
      tdc890GetMaxNumberOfHitsPerEvent(tdcadr[ii], &tdata);
	  */
      tdata = 128;
      tdc890SetEffectiveSizeOfReadoutFIFO(tdcadr[ii], tdata);
      tdc890GetEffectiveSizeOfReadoutFIFO(tdcadr[ii], &tdata);

    }
  }

#ifdef PRIMEX
  /*increase the number of hits per event for TAGE TDCs*/
  if(rocid==16||rocid==17||rocid==19)
  {
    for(ii=0; ii<nboards; ii++)
    {
      tdata = 1000/*means unlimited*//*128*/;
      tdc890SetMaxNumberOfHitsPerEvent(tdcadr[ii], tdata);
      tdc890GetMaxNumberOfHitsPerEvent(tdcadr[ii], &tdata);
	}
  }
#endif

  /* if VME crate is not 64X 
  for(ii=0; ii<nboards; ii++)
  {
    tdc890SetGeoAddress(tdcadr[ii], (ii+1));
  }
  */

/* v1190 end */


  /**************/
  /**************/

  printf("vmeUserDownload: User Download Executed\n");
}

void
vmeUserPrestart()
{
  char *env;
  char *histname = "ROL1 RC00";
  unsigned short value, value0, value1, array0[32], array1[32], data16;
  int i, ii, ifull;
  short buf;
  unsigned short slot, channel, pattern[8];



  /* repeat it here until problem fixed */
  dmaPReInitAll();



  /* PRogram/Init VME Modules Here */
  /**************/
  /* CLAS stuff */

  tttest("\npolar rol1:");

  sprintf((char *)&rcname[2],"%02d",rocid);
  printf("rcname >%4.4s<\n",rcname);


#ifdef SLOTWORKAROUND
  for(ii=0; ii<nboards; ii++)
  {
    tdc890GetGeoAddress(tdcadr[ii], &slot);
	slotnums[ii] = slot;
  }
#endif

  if((ttp != NULL) && (rocid != 28) && (rocid != 29) && (rocid != 0))
  {
#ifdef PRIMEX
if((rocid != 16) && (rocid != 17) && (rocid != 18) && (rocid != 19))
{
#endif

  /* disable v1190 channels undescribed in TT */
  for(ii=0; ii<nboards; ii++)
  {
    tdc890GetGeoAddress(tdcadr[ii], &slot);
    printf("v1190 address 0x%08x at slot %2d\n",tdcadr[ii],slot);
    tdc890EnableAllChannels(tdcadr[ii]);
    for(channel=1; channel<128; channel++) /* always keep channel 0 enabled */
    {
      if(ttp->idnr[slot][channel] <= 0)
      {
        /*printf("disable slot# %2d channel# %3d\n",slot,channel);*/
        tdc890DisableChannel(tdcadr[ii], channel);
      }
    }


	/*	
if(ii==3||ii==4)
{
  for(channel=4; channel<5; channel++)
  {
    printf("[%d] disable channel %d\n",ii,channel);
    tdc890DisableChannel(tdcadr[ii], channel);
  }
}
*/

    tdc890GetChannels(tdcadr[ii], pattern);

    printf("TDC connectors:    DL   DR   CL   CR   BL   BR   AL   AR\n");
    printf("channels pattern: %04x %04x %04x %04x %04x %04x %04x %04x\n",
      pattern[7],pattern[6],pattern[5],pattern[4],
      pattern[3],pattern[2],pattern[1],pattern[0]);
  }

#ifdef PRIMEX
}
#endif
  }



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
    sprintf((char *)&histname[7],"%02d",rocid);
    uthbook1(histi, 1000+rocid, histname, 200, 0, 200);
  }

  /**************/
  /**************/




  /* Initialize VME Interrupt variables */
  vmeIntClearCount();

  printf("vmeUserPrestart: User Prestart Executed\n");

}

void
vmeUserGo()
{
  unsigned short value, value0, value1, array0[32], array1[32];
  int i, ii;



  /* v1190/v1290 */
  if(tdcbaseadr)
  {
    logMsg("Use MCST_CBLT method\n",1,2,3,4,5,6);

	/*temporary until mv6100 mapping understood
    tdc890Clear(tdcbaseadr);
	IT IS UNDERSTOOD ...*/
    for(ii=0; ii<nboards; ii++)
    {
      tdc890Clear(tdcadr[ii]);
    }
  }
  else
  {
    logMsg("Do NOT use MCST_CBLT method\n",1,2,3,4,5,6);
    for(ii=0; ii<nboards; ii++)
    {
      tdc890Clear(tdcadr[ii]);
    }
  }


  /* print disabled v1190 channels 
  {
    unsigned short slot, pattern[8];
  for(ii=0; ii<nboards; ii++)
  {
    tdc890GetGeoAddress(tdcadr[ii], &slot);
    printf("v1190 address 0x%08x at slot %2d\n",tdcadr[ii],slot);

    tdc890GetChannels(tdcadr[ii], pattern);

    printf("channels enable pattern: 0x%04x 0x%04x 0x%04x 0x%04x\n",
       pattern[7],pattern[6],pattern[5],pattern[4]);
    printf("                         0x%04x 0x%04x 0x%04x 0x%04x\n",
       pattern[3],pattern[2],pattern[1],pattern[0]);
  }
  }
  */

  for(ii=0; ii<NBOARDS; ii++)
  {
    error_flag[ii] = 0;
  }



  /* Enable Interrupts */
  vmeIntEnable(IRQ_SOURCE,TS_MODE);
}





#define DATA_VAR \
  int njjloops, ii, jj, rlen; \
  unsigned long tdcslot, tdcchan, tdcval, tdc14, tdcedge, tdceventcount; \
  unsigned long tdceventid, tdcbunchid, tdcwordcount, tdcerrorflags; \
  unsigned int *tdc, *tdchead, itdcbuf, nbcount, nbsubtract; \
  int nheaders, ntrailers; \
/*#ifdef SLOTWORKAROUND*/ \
  unsigned long tdcslot_h, tdcslot_t, remember_h; \
/*#endif*/



#define DATA_PROCESS \
  { \
    itdcbuf = 0; \
    nheaders = ntrailers = 0; \
    if(tdcbaseadr) njjloops = 1; \
    else           njjloops = nboards; \
    for(jj=0; jj<njjloops; jj++) \
    { \
      rlen = rlenbuf[jj]; \
      if(rlen <= 0) continue; \
      tdc = &tdcbuf[itdcbuf]; \
      itdcbuf += rlen; \
      for(ii=0; ii<rlen; ii++) \
      { \
        if( ((tdc[ii]>>27)&0x1F) == 0) \
        { \
          nbcount ++; \
          *dma_dabufp ++ = (tdc[ii] | (tdcslot<<27)); \
/*#ifdef DEBUG \
          tdcedge = ((tdc[ii]>>26)&0x1); \
          tdcchan = ((tdc[ii]>>19)&0x7F); \
          tdcval = (tdc[ii]&0x7FFFF); \
		  if(tdcchan<20) \
          { \
            logMsg(" DATA: tdc=%02d ch=%03d edge=%01d value=%08d\n", \
              (int)tdc14,(int)tdcchan,(int)tdcedge,(int)tdcval,5,6); \
		  } \
#endif*/ \
        } \
        else if( ((tdc[ii]>>27)&0x1F) == 1) \
        { \
          tdc14 = ((tdc[ii]>>24)&0x3); \
          tdceventid = ((tdc[ii]>12)&0xFFF); \
          tdcbunchid = (tdc[ii]&0xFFF); \
/*#ifdef DEBUG \
          logMsg(" HEAD: tdc=%02d event_id=%05d bunch_id=%05d\n", \
            (int)tdc14,(int)tdceventid,(int)tdcbunchid,4,5,6); \
#endif*/ \
        } \
        else if( ((tdc[ii]>>27)&0x1F) == 3) \
        { \
          tdc14 = ((tdc[ii]>>24)&0x3); \
          tdceventid = ((tdc[ii]>12)&0xFFF); \
          tdcwordcount = (tdc[ii]&0xFFF); \
/*#ifdef DEBUG \
          logMsg(" EOB:  tdc=%02d event_id=%05d word_count=%05d\n", \
            (int)tdc14,(int)tdceventid,(int)tdcwordcount,4,5,6); \
#endif*/ \
        } \
        else if( ((tdc[ii]>>27)&0x1F) == 0x18) \
        { \
/*#ifdef DEBUG \
          logMsg(" FILLER:  tdc=%02d event_id=%05d word_count=%05d\n", \
            (int)tdc14,(int)tdceventid,(int)tdcwordcount,4,5,6); \
#endif*/ \
        } \
        else if( ((tdc[ii]>>27)&0x1F) == 8) \
        { \
          nbcount = 1; /* counter for the number of output words from board */ \
          nbsubtract = 9; /* # words to subtract including errors */ \
          tdcslot = tdc[ii]&0x1F; \
          tdceventcount = (tdc[ii]>>5)&0x3FFFFF; \
          tdchead = (unsigned int *) dma_dabufp; /* remember pointer */ \
/*#ifdef SLOTWORKAROUND*/ \
          tdcslot_h = tdcslot; \
	      remember_h = tdc[ii]; \
          if(slotnums[nheaders] != tdcslot) \
		  { \
            logMsg("WARN: [%2d] slotnums=%d, tdcslot=%d -> use slotnums\n", \
				   nheaders,slotnums[nheaders],tdcslot,4,5,6); \
            tdcslot = slotnums[nheaders]; \
		  } \
/*#endif*/ \
          *dma_dabufp ++ = tdcslot; \
          nheaders++; \
/*#ifdef DEBUG \
          logMsg("GLOBAL HEADER: %d words, 0x%08x (slot=%d nevent=%d) -> header=0x%08x\n", \
            rlen,tdc[ii],tdc[ii]&0x1F,(tdc[ii]>>5)&0xFFFFFF,*tdchead,6); \
#endif*/ \
        } \
        else if( ((tdc[ii]>>27)&0x1F) == 0x10) \
        { \
/*#ifdef SLOTWORKAROUND*/ \
          /* double check for slot number */ \
          tdcslot_t = tdc[ii]&0x1F; \
          if(tdcslot_h>21||tdcslot_t>21||tdcslot_h!=tdcslot_t) \
          { \
            /*logMsg("WARN: slot from header=%d (0x%08x), from trailer=%d (0x%08x), must be %d\n", \
			  tdcslot_h,remember_h,tdcslot_t,tdc[ii],tdcslot,6)*/; \
		  } \
/*#endif*/ \
          /* put nwords in header; substract 4 TDC headers, 4 TDC EOBs, \
          global trailer and error words if any */ \
          /**tdchead = (*tdchead) | (((tdc[ii]>>5)&0x3FFF) - nbsubtract);*/ \
          *tdchead |= ((((tdc[ii]>>5)&0x3FFF) - nbsubtract)<<5); \
          if((((tdc[ii]>>5)&0x3FFF) - nbsubtract) != nbcount) \
          { \
            logMsg("ERROR: word counters: %d != %d (subtract=%d)\n", \
			  (((tdc[ii]>>5)&0x3FFF) - nbsubtract),nbcount,nbsubtract,4,5,6); \
          } \
          ntrailers ++; \
/*#ifdef DEBUG \
          logMsg("GLOBAL TRAILER: 0x%08x (slot=%d nw=%d stat=%d) -> header=0x%08x\n", \
            tdc[ii],tdc[ii]&0x1F,(tdc[ii]>>5)&0x3FFF, \
            (tdc[ii]>>23)&0xF,*tdchead,6); \
#endif*/ \
        } \
        else if( ((tdc[ii]>>27)&0x1F) == 4) \
        { \
          nbsubtract ++; \
          tdc14 = ((tdc[ii]>>24)&0x3); \
          tdcerrorflags = (tdc[ii]&0x7FFF); \
		  /* \
#ifdef DEBUG \
		  */ \
		  if(error_flag[tdcslot] == 0) /* print only once */ \
          { \
            unsigned int ddd, lock, ntdc; \
            /* \
            logMsg(" ERR: event# %7d, slot# %2d, tdc# %02d, error_flags=0x%08x, err=0x%04x, lock=0x%04x\n", \
              tdceventcount,tdcslot,(int)tdc14,(int)tdcerrorflags,ddd,lock); \
            */ \
		  } \
          error_flag[tdcslot] = 1; \
		  /* \
#endif \
		  */ \
        } \
        else \
        { \
/*#ifdef DEBUG \
          logMsg("ERROR: in TDC data format 0x%08x\n", \
            (int)tdc[ii],2,3,4,5,6); \
#endif*/ \
        } \
      } /* loop over 'rlen' words */ \
    } \
  } \
  /*if(nheaders!=ntrailers || nboards!=nheaders) \
    logMsg("ERROR: nboards=%d, nheaders=%d, ntrailers=%d\n",nboards,nheaders,ntrailers,4,5,6);*/ \










void
vmeUserEnd()
{
  int status, count, blen;
  DATA_VAR;

  /* Disable Interrupts */
  vmeIntDisable(IRQ_SOURCE,TS_MODE);
  dmaPStatsAll();
  printf("ENDINGGGGGG: vmeIntCount=%d syncFlag=%d lateFail=%d type=%d\n",
    vmeIntCount,syncFlag,lateFail,type);

  /* Check on last Transfer */
  if(vmeIntCount > 0)
  {
    if((syncFlag==1)&&(type==0))  /* force_sync (scaler) events */
    {
      logMsg("ending by force_sync (scaler) event\n",1,2,3,4,5,6);
    }
    else if((syncFlag==0)&&(type==15))  /* helicity event */
    {
      logMsg("ending by helicity event\n",1,2,3,4,5,6);
    }
    else /* physics and physics (scheduled) sync events */
    {


      if(tdcbaseadr)
      {
        status = CLAStdc890ReadEventCBLTStop(rlenbuf);
      }
      else
      {
        status = tdc1190ReadEventDone();
        if(status == -1)
        {
          logMsg("BAD%d\n",status,2,3,4,5,6);
          for(jj=0; jj<nboards; jj++) rlenbuf[jj] = 0;
        }
	  }


logMsg("vmeUserEnd: status=%d\n",status,2,3,4,5,6);
      /*status = vmeDmaStatus();*/
      if(status < 0)
      {
        logMsg("vmeUserEnd: ERROR: Last Transfer Event NUMBER %d, status = 0x%x (0x%x 0x%x 0x%x 0x%x)\n",
          vmeIntCount,status,*(dma_dabufp-4),
          *(dma_dabufp-3),*(dma_dabufp-2),*(dma_dabufp-1));
        /* remove BOS header created at DMA start time */
        dma_dabufp -= NHEAD;
      }
      else
      {
        logMsg("vmeUserEnd: INFO: Last Event %d, status=%d (0x%08x 0x%08x)\n",
          vmeIntCount,status,dma_dabufp,jw,4,5,6);
        logMsg("data: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
          *(dma_dabufp-4),*(dma_dabufp-3),*(dma_dabufp-2),
          *(dma_dabufp-1),*(dma_dabufp),*(dma_dabufp+1));
        logMsg("jw1 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
          *(jw-4),*(jw-3),*(jw-2),
          *(jw-1),*(jw),*(jw+1));

        DATA_PROCESS;

        dabufp2 = dma_dabufp;
        blen = (int *)dma_dabufp - (int *)&jw[ind+1];
        logMsg("Last DMA status = 0x%x count=%d blen=%d\n",status,count,blen,4,5,6);
        if(blen >= (BUFSIZE_IN_BYTES1/4))
        {
          logMsg("1ERROR vmeUserLib: event too long, blen=%d, ind=%d\n",blen,ind,0,0,0,0);
          logMsg(": %d %d 0x%x 0x%x\n",blen,ind,dabufp1,dabufp2,0,0);
        }
        if(bosMclose_(jw,ind,1,blen) == 0)
        {
          logMsg("2ERROR in bosMclose_ - space is not enough !!!\n");
        }
      }
	}
    PUTEVENT(vmeOUT);
  }

  dmaPStatsAll();

  printf("vmeUserEnd: Ended after %d events\n",vmeIntCount);
  
}

#define MYGETEVENT \
    /* grab a buffer from the queue */ \
    GETEVENT(vmeIN,vmeIntCount); /* vmeIntCount == my nevents */ \
    /* move 'dma_dabufp' pointer to reserve a room for CODA header */ \
    dma_dabufp += 2; \
    /* set the 'jw' pointer and initialize BOS format */ \
    jw = dma_dabufp; \
    dabufp1 = jw; \
    jw[ILEN] = 1; \
    /* fill second CODA header word */ \
    nevents ++; \
    jw[-1] = (syncFlag<<24)|(type<<16)|(BT_UI4_ty<<8)|(0xff&nevents)

#define MYPUTEVENT \
      if(tdcbaseadr) \
      { \
        status = CLAStdc890ReadEventCBLTStop(rlenbuf); \
      } \
      else \
      { \
        status = tdc1190ReadEventDone(); \
        if(status == -1) \
        { \
          logMsg("BAD%d\n",status,2,3,4,5,6); \
          for(jj=0; jj<nboards; jj++) rlenbuf[jj] = 0; \
        } \
	  } \
      if(status < 0) \
      { \
        logMsg("Trig: ERROR: Last Transfer Event NUMBER %d, status=0x%08x\n", \
          vmeIntCount,status,0,0,0,0); \
        /* remove BOS header created at DMA start time */ \
        dma_dabufp -= NHEAD; \
      } \
      else \
      { \
        DATA_PROCESS; \
        dabufp2 = dma_dabufp; \
        blen = ((int *)dma_dabufp) - ((int *)&jw[ind+1]); \
        if(blen == 0) /* no data - return pointer to the initial position */ \
        { \
          dma_dabufp -= NHEAD; \
	      /* NEED TO CLOSE BANK !!?? */ \
        } \
        else if(blen >= (BUFSIZE_IN_BYTES1/4)) \
        { \
          logMsg("1ERROR vmeUserLib2: event too long, blen=%d, ind=%d\n",blen,ind,0,0,0,0); \
          logMsg(": %d %d 0x%x 0x%x\n",blen,ind,dabufp1,dabufp2,0,0); \
          /*tsleep(1);*/ /* 1 = 0.01 sec */ \
        } \
        if(bosMclose_(jw,ind,1,blen) == 0) \
        { \
          logMsg("2ERROR in bosMclose_ - space is not enough !!!\n"); \
        } \
      }

void
vmeUserTrigger(int arg)
{
  int status, dma, count;
  unsigned int tirval, vme_addr;
  long EVENT_LENGTH;
  int *adrlen, *bufin, *bufout, i, ind2, ncol, nrow, len, len1, len2;
  unsigned long tmp, *secondword, *buf;
  TIMER_VAR;
  unsigned short *buf2;
  unsigned short value;
  unsigned int nwords, nevent, buff[32];

  int blen, nev, nevts, nwrds;
  unsigned long res, datascan;

  DATA_VAR;
  /*
logMsg("vmeUserTrigger reached: vmeIntCount=%d\n",vmeIntCount,2,3,4,5,6);
  */
  if(timeprofile)
  {
    TIMER_NORMALIZE;
    TIMER_START;
  }

  /***************************************************/
  /* complete previous event processing if necessary */
  /***************************************************/

  if(vmeIntCount > 1)
  {
    if(syncFlag==1)
    {
      if(type!=0) /* physics sync event */
	  {
        /*logMsg("previous event was physics sync\n",1,2,3,4,5,6)*/;
	  }
      if(type==0) /* force_sync (scaler) events */
      {
        /*logMsg("previous event was force_sync (scaler)\n",1,2,3,4,5,6)*/;
      }
    }
    else if(type==15)
	{
      /*logMsg("previous event was helicity event\n",1,2,3,4,5,6)*/;
	}
    else
	{
      MYPUTEVENT;
	}
    PUTEVENT(vmeOUT);
  }




  /**********************************/
  /* start current event processing */
  /**********************************/


  /* gets 'type', 'lateFail' and 'syncFlag' */
  tirval = vmeTrigType(IRQ_SOURCE, TS_MODE);
  type = tirval/*(((tirval)&0x3c)>>2)*/;
  lateFail = vmeLateFail/*((tirval)&0x02)>>1*/;
  syncFlag = vmeSyncFlag/*(tirval)&0x01*/;

  /* check event type and sync flag consistency */
  if((syncFlag<0)||(syncFlag>1))         /* illegal */
  {
    logMsg("Illegal1: syncFlag=%d EVTYPE=%d\n",syncFlag,type,3,4,5,6);
  }
  else if((syncFlag==0)&&(type==0))    /* illegal */
  {
    logMsg("Illegal2: syncFlag=%d EVTYPE=%d\n",syncFlag,type,3,4,5,6);
  }
  else if((syncFlag==1)&&(type==0))    /* force_sync (scaler) events */
  {
    MYGETEVENT;

    if(timeprofile)
    {
      /*logMsg("report timing histogram\n",1,2,3,4,5,6);*/
      dma_dabufp += uth2bos(histi, 1000+rocid, jw);
    }
  }
  else if((syncFlag==0)&&(type==15)) /* helicity strob events */
  {
    MYGETEVENT;
  }
  else /* physics and physics (scheduled) sync events */
  {

    /* perform VME datascan */
    datascan = 1;

    /* 'open' event */
    MYGETEVENT;

    /* put BOS bank header */
    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
    }
    dma_dabufp += NHEAD;

    /* start transfer if all boards have at least one event */ 
    if(datascan)
    {
      /* start readout */
      if(tdcbaseadr)
      {
        /*logMsg("CBLT does not work in 'parallel' readout yet\n",1,2,3,4,5,6);*/
        CLAStdc890ReadEventCBLTStart(nboards, tdcbaseadr, tdcadr, tdcbuf, rlenbuf);
      }
      else
      {
        res = tdc1190ReadEventStart(nboards, tdcadr, tdcbuf, rlenbuf);
        /* MUST check 'res' here ! */
      }
    }
    else  /* 'missing gate' situation */
    {
      logMsg("missing gate !!!\n",1,2,3,4,5,6);
    }

    /* for physics sync event, wait for DMA completion
    and make sure all boards buffers are empty */
    if(syncFlag==1)
    {
      MYPUTEVENT;
      /*logMsg("physics sync event\n",1,2,3,4,5,6)*/;

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
            logMsg("SYNC: ERROR: [%2d] slot=%2d error_flag=%d - clear\n",
              ii,slot,error_flag[slot],4,5,6);
            tdc890Clear(tdcadr[ii]);
            error_flag[slot] = 0;
            scan_flag |= (1<<slot);
          }
        }
	    if(scan_flag) logMsg("SYNC: scan_flag=0x%08x\n",scan_flag,2,3,4,5,6);

	    /*logMsg("PTRN ..\n",1,2,3,4,5,6);*/
        ind2 = bosMopen_(jw,"PTRN",rocid,1,1);
        jw[ind2+1] = SYNC + scan_flag;
        dma_dabufp += bosMclose_(jw,ind2,1,1);
	    /*logMsg("PTRN done.\n",1,2,3,4,5,6);*/
      }



    }

  }


  if(timeprofile) TIMER_STOP(100000,1000+rocid);

  /* if the is at least one input buffer - acknowledge interrupt */
  /* otherwise set 'vmeNeedAck' flag (will be acknowledged in 'done' */
  if(dmaPEmpty(vmeIN))
  {
    vmeNeedAck = 1;
  }
  else
  {
    vmeIntAck(IRQ_SOURCE,TS_MODE);
  }

}

void
vmeUserStatus()
{

  printf("VME DMA Status:\n\n");

  dmaPStatsAll();

  printf("\n");
  if(vmeIntStatus())
    printf(" DAQ: Active\n");
  else
    printf(" DAQ: Idle\n");
    
  printf(" Total Event Count = %d\n",vmeIntCount);

}

int
vmeUserGetCount()
{
  if(vmeOUT)
  {
    return(vmeOUT->list.c);
  }
  else
  {
    return(0);
  }      
}

void
vmeUserTest()
{
  int lock_key;
  DANODE *outEvent;
  
  while(1)
  {
    if(dmaPEmpty(vmeOUT)) continue;
    
    /* get event - then free it */
    lock_key = intLock();
    outEvent = dmaPGetItem(vmeOUT);
    dmaPFreeItem(outEvent);
    intUnlock(lock_key);
  }

}

