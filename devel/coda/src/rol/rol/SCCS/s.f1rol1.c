h28320
s 00001/00001/00858
d D 1.5 10/03/09 12:41:22 boiarino 5 4
c *** empty log message ***
e
s 00004/00000/00855
d D 1.4 10/03/09 12:29:12 boiarino 4 3
c *** empty log message ***
e
s 00002/00002/00853
d D 1.3 08/10/26 22:14:47 boiarino 3 2
c *** empty log message ***
e
s 00001/00001/00854
d D 1.2 08/10/26 22:09:02 boiarino 2 1
c *** empty log message ***
e
s 00855/00000/00000
d D 1.1 06/05/26 16:10:46 boiarino 1 0
c date and time created 06/05/26 16:10:46 by boiarino
e
u
U
f e 0
t
T
I 1

/* f1rol1.c - first readout list for F1TDC readout */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

#define F1TDC

/*****************************/
/* former 'crl' control keys */

/* readout list VMEROL1 */
#define ROL_NAME__ "F1ROL1"

/* ts control */
#define TRIG_SUPV

/* name used by loader */
#define INIT_NAME f1rol1__init

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
#define SYNC 0x20000000

#ifdef F1TDC

#include "f1tdcLib.h"

#define F1_SLOT 4
#define MAX_F1_DATA  (16*64*12)

#endif


/* main TI board */
#define TIRADR   0x0ed0
/* CAEN v775 TDC */
#define TDCADR0  0xE00000

/* CAEN v1190/v1290 TDCs base address and step */
/* first board has address TDCADR, second TDCADR+TDCSTEP etc */
/*
#define TDCADR  0x210000
#define TDCSTEP  0x10000
*/

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

#define SCALER0 0x508000
#define SCALER1 0x303000
#define MASK    0xffff0000   /* unmask 16 lowest channels */
#define NCHN            16
#define NBUFHLS      72000


static char *rcname = "RC00";

/* global addresses for boards */
static unsigned long scaler0, scaler1, tdcadr0;

/* user buffer pointers - all NULL at initialization */
extern int *userbuffers[NUSERBUFFERS];
/* pointer to TTS structures for current roc */
extern TTSPtr ttp;

static int *ring0, *ring1, *ring2;
static int nHLS;
static int lenHLS0 = ((((BUFSIZE_IN_BYTES1/4) - 1024) / 2) / NCHN) * NCHN;
static int lenHLS1 = ((((BUFSIZE_IN_BYTES1/4) - 1024) / 2) / NCHN) * NCHN;
static int printRingFull = 1;

/* time profiling: 0-OFF, 1-ON */
static int timeprofile = 1;

/* histogram area */
static UThisti histi[NHIST];
static neventh = 0;



static int stoppp = 0;
static int word_count_termination = 0;

D 2
#define NBOARDS 21    /* maximum number of VME boards */
E 2
I 2
#define NBOARDS 22    /* maximum number of VME boards */
E 2


/* v1190 start */

#define MY_MAX_EVENT_LENGTH 3000 /* max words per v1190 board */
#define TDC_OFFSET 0
#define CH_OFFSET  0
/*
#define NDMASTAT 3
*/
static int nboards;
static unsigned long tdcadr[21];
static unsigned long tdcbaseadr;
static int maxbytes = 0;

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
  scaler0 = offset + SCALER0;
  scaler1 = offset + SCALER1;
  tdcadr0 = offset + TDCADR0;
  printf("\nrol1: lenHLS0 = %d,   lenHLS1 = %d\n",lenHLS0,lenHLS1);
  printf("rol1: scaler0 = 0x%08x  scaler1 = 0x%08x\n",scaler0,scaler1);
  printf("rol1: tdcadr0 = 0x%08x\n",tdcadr0);

  if(userbuffers[0] == NULL)
  {
    printf("allocate %d bytes for userbuffers[0]\n",NBUFHLS);
    userbuffers[0] = utRingAlloc(NBUFHLS);
  }
  ring0 = userbuffers[0];

  if(userbuffers[1] == NULL)
  {
    printf("allocate %d bytes for userbuffers[1]\n",NBUFHLS);
    userbuffers[1] = utRingAlloc(NBUFHLS);
  }
  ring1 = userbuffers[1];

  if(userbuffers[2] == NULL)
  {
    printf("allocate %d bytes for userbuffers[2]\n",NBUFHLS);
    userbuffers[2] = utRingAlloc(NBUFHLS);
  }
  ring2 = userbuffers[2];



/* v1190 start */

  /* returns 'nboards' and 'tdcadr' */
D 3
  tdc1190ScanCLAS(&nboards, tdcadr);
E 3
I 3
  tdc1190ScanCLAS(CLAS_A32_MBLT_FIFO, &nboards, tdcadr);
E 3
  printf("vmerol1: nboards=%d\n",nboards);
  for(ii=0; ii<nboards; ii++)
  {
    printf("vmerol1: tdcadr[%2d] = 0x%08x\n",ii,tdcadr[ii]);
  }

  /* v1190/v1290 TDC setup (see tdc890.h for possible options) */
D 3
  tdc1190InitCLAS(CLAS_A24_MBLT_FIFO, nboards, tdcadr, &tdcbaseadr);
E 3
I 3
  tdc1190InitCLAS(nboards, tdcadr, &tdcbaseadr);
E 3
  printf("-------========-------> tdcbaseadr=0x%08x\n",tdcbaseadr);
  /* if VME crate is not 64X 
  for(ii=0; ii<nboards; ii++)
  {
    tdc890SetGeoAddress(tdcadr[ii], (ii+1));
  }
  */

/* v1190 end */


#ifdef F1TDC

/*
   Initialize F1 library and Modules
     params: first board address, address increase (address
             difference between boards), the number of boards,
             flag (0 - high res, 2 - low res)
*/
  /*f1Init(0xed0000,0,1,0x0ee0);*/
  f1Init(0xed0000,0x1000,12,/*0x8*/0xa); /* last parameter: 0x8 + res: 0x8 - high res, 0xa - low res */

  usrVmeDmaInit();
  usrVmeDmaConfig(2,2); /* A32, D32 BLK */

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

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  /* init trig source VME */
  VME_INIT;

  /* Register an async trigger source, (up to 32 sources) */
  /* CTRIGRSA(source,code,handler,done)*/
  /* link async trig source VME 1 to usrtrig and usrtrig_done */
  CTRIGRSA(VME, 1, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */


  printf("\npolar rol1: lenHLS0 = %d,   lenHLS1 = %d\n",lenHLS0,lenHLS1);
  tttest("\npolar rol1:");

  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);



if(rol->pid != 24)
{

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

}



#ifdef F1TDC

/* Setup F1TDC */

/*
  f1Clear(F1_SLOT);
  f1SetWindow(F1_SLOT,900,1000,0xff);
  f1EnableData(F1_SLOT,0xff);
  f1SetBlockLevel(F1_SLOT,1);
  f1EnableBusError(F1_SLOT);
  f1Status(F1_SLOT,0);
*/

/* for multiblock read */
  f1GClear();
/*  for(...) f1SetWindow(F1_SLOT,900,1000,0xff);*/
  f1GEnableData(0xff);
  f1GSetBlockLevel(1);
  f1EnableMultiBlock();
  f1GStatus(0);

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


  /* v1190/v1290 */
  if(tdcbaseadr)
  {
    logMsg("Use MCST_CBLT method\n",1,2,3,4,5,6);
    tdc890Clear(tdcbaseadr);
  }
  else
  {
    logMsg("Do NOT use MCST_CBLT method\n",1,2,3,4,5,6);
    for(ii=0; ii<nboards; ii++)
    {
      tdc890Clear(tdcadr[ii]);
    }
  }


stoppp=0;



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

/* Send Sync Reset to ALL F1 TDCS */

/*if Using SDC Board
  f1SDC_Sync();*/

/*if Using a BDC system*/
  f1BDC_Sync();


  CDOENABLE(VME,1,0);

  /* from parser */
  if(__the_event__) WRITE_EVENT_;

  return;
}



/* 

SN 0037 (slot 6) TDC00
error_flags=0x00000800 -> hit error in group 3
err=0x0008 -> L1 buffer parity error
lock=0x0001

SN 0045 (slot 7) TDC00
error_flags=0x00000100
err=0x0008 -> L1 buffer parity error
lock=0x0001

SN 0032 (slot 8) TDC02
error_flags=0x00004000
error_flags=0x00000004
err=0x0008
lock=0x0001

SN 0064 (slot 9) TDC02
error_flags=0x00000100
err=0x0008
lock=0x0001

SN 0059 (slot 10) TDC03
error_flags=0x00000800 -> hit error in group 3
err=0x0008 -> L1 buffer parity error
lock=0x0001



 */

void
mytest0(unsigned int addr)
{
  unsigned short err, loc, ntdc;

  tdc890GetTDCError(addr, 0, &err);
  tdc890GetTDCDLLLock(addr, 0, &loc);
  printf("err=0x%04x lock=0x%04x\n",err,loc);
}
void
mytest1(unsigned int addr)
{
  unsigned short err, loc, ntdc;

  tdc890GetTDCError(addr, 1, &err);
  tdc890GetTDCDLLLock(addr, 1, &loc);
  printf("err=0x%04x lock=0x%04x\n",err,loc);
}
void
mytest2(unsigned int addr)
{
  unsigned short err, loc, ntdc;

  tdc890GetTDCError(addr, 2, &err);
  tdc890GetTDCDLLLock(addr, 2, &loc);
  printf("err=0x%04x lock=0x%04x\n",err,loc);
}
void
mytest3(unsigned int addr)
{
  unsigned short err, loc, ntdc;

  tdc890GetTDCError(addr, 3, &err);
  tdc890GetTDCDLLLock(addr, 3, &loc);
  printf("err=0x%04x lock=0x%04x\n",err,loc);
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

  /*
logMsg("======================================\n",1,2,3,4,5,6);
logMsg("======================================\n",1,2,3,4,5,6);
logMsg("======================================\n",1,2,3,4,5,6);
logMsg("usrtrig: %d %d\n",syncFlag,EVTYPE,3,4,5,6);
logMsg("======================================\n",1,2,3,4,5,6);
logMsg("======================================\n",1,2,3,4,5,6);
logMsg("======================================\n",1,2,3,4,5,6);
  */

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

    /* force_sync (scaler) event */
    if(timeprofile)
    {
      /*logMsg("report histogram ...\n");*/
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
I 4
  else if((syncFlag==0)&&(EVTYPE==15)) /* helicity strob events */
  {
    ;
  }
E 4
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



#ifdef F1TDC
    /* multiblock readout */
    datascan = f1DataScan(0); /*1 for print*/
  /*
logMsg("f1tdc: datascan=0x%08x mymask=0x%08x\n",datascan,mymask,3,4,5,6);
  */
    if(datascan == mymask)
    {

      /* open data bank */
      if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
      {
        logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
      }
      rol->dabufp += NHEAD;


      /* first parameter: left slot number */
      nwrds = f1ReadEvent(4,rol->dabufp,MAX_F1_DATA,2);
	/*
logMsg("f1tdc: nwrds=%d\n",nwrds,2,3,4,5,6);
	*/
      if(nwrds<=0)
      {
        logMsg("ERROR: Block Read Failed, nwrds=0x%08x\n",nwrds,0,0,0,0,0);
        /**rol->dabufp++ = 0xda000bad;*/
      }
      else
      {
	    /*
        int iii;
        for(iii=0; iii<nwrds; iii++)
	    {
          logMsg("0x%08x\n",rol->dabufp[iii],2,3,4,5,6);
	    }
	    */

	    /*
        logMsg("INFO: Read %d words: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
          nwrds,rol->dabufp[0],rol->dabufp[1],rol->dabufp[2],rol->dabufp[3],
          rol->dabufp[4]);

        logMsg("INFO: first word:  0x%01x 0x%06x 0x%09x 0x%01x 0x%03x 0x%03x\n",
          (rol->dabufp[0]>>22)&0x1,
          (rol->dabufp[0]>>16)&0x3f,
          (rol->dabufp[0]>>7)&0x1ff,
          (rol->dabufp[0]>>6)&0x1,
          (rol->dabufp[0]>>3)&0x7,
          rol->dabufp[0]&0x7);

        logMsg("INFO: second word: 0x%01x 0x%06x 0x%09x 0x%01x 0x%03x 0x%03x\n",
          (rol->dabufp[1]>>22)&0x1,
          (rol->dabufp[1]>>16)&0x3f,
          (rol->dabufp[1]>>7)&0x1ff,
          (rol->dabufp[1]>>6)&0x1,
          (rol->dabufp[1]>>3)&0x7,
          rol->dabufp[1]&0x7);
	    */
        rol->dabufp += nwrds;

        /* if f1ReadEvent() returned 'nwrds' equal to MAX_F1_DATA then
        we had readout problem; we'll clear all boards and raise a flag
        which will be used in next sync event to mark whole interval as 'bad' */
        if(nwrds == MAX_F1_DATA)
        {
          f1GClear();
          word_count_termination = 1;
          logMsg("WARN: word_count_termination set to %d\n",
            word_count_termination,2,3,4,5,6);
        }
      }




D 5
      blen = rol->dabufp - (long *)&jw[ind+1];
E 5
I 5
      blen = rol->dabufp - (int *)&jw[ind+1];
E 5
      if(blen == 0) /* no data - return pointer to the initial position */
      {
        rol->dabufp -= NHEAD;
	    /* NEED TO CLOSE BANK !!?? */
      }
      else if(blen >= (BUFSIZE_IN_BYTES1/4))
      {
        logMsg("1ERROR: event too long, blen=%d, ind=%d\n",blen,ind,0,0,0,0);
        logMsg(": %d %d 0x%x 0x%x\n",blen,ind,dabufp1,dabufp2,0,0);
        tsleep(1); /* 1 = 0.01 sec */
      }
      else if(bosMclose_(jw,ind,1,blen) == 0)
      {
        logMsg("2ERROR in bosMclose_ - space is not enough !!!\n",1,2,3,4,5,6);
      }






    }
    else
    {
      logMsg("ERROR: NO data in TDC: datascan=0x%08x mymask=0x%08x\n",
        datascan,mymask,0,0,0,0);
      f1GClear();
    }


    /* one slot readout (not tested example !)
    nevts = f1Dready(F1_SLOT);
    if(nevts > 0)
    {
      nwrds = f1ReadEvent(F1_SLOT,rol->dabufp,MAX_F1_DATA,0);
      if(nwrds<=0)
      {
        logMsg("ERROR: Block Read Failed\n",0,0,0,0,0,0);
      }
      else
      {
        rol->dabufp += nwrds;
      }
    }
    else
    {
      logMsg("ERROR: NO data in TDC\n",0,0,0,0,0,0);
    }
    */


    /* for physics sync event, make sure all board buffers are empty */
    if(syncFlag==1)
    {
      int scan_flag;

      scan_flag = f1DataScan(0);
      f1GClear();

      if(word_count_termination)
      {
        scan_flag = scan_flag | 0x1;
      }

	  if(scan_flag)
      {
        logMsg("SYNC: scan_flag=0x%08x\n",scan_flag,2,3,4,5,6);
	  }

	  /*logMsg("PTRN ..\n",1,2,3,4,5,6);*/
      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = SYNC + scan_flag;
      rol->dabufp += bosMclose_(jw,ind2,1,1);
	  /*logMsg("PTRN done.\n",1,2,3,4,5,6);*/

    }
#endif

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
