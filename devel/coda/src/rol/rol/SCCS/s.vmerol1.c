h00080
s 00010/00003/01323
d D 1.7 11/04/08 12:36:59 boiarino 7 6
c *** empty log message ***
e
s 00056/00001/01270
d D 1.6 11/04/08 12:26:46 boiarino 6 5
c *** empty log message ***
e
s 00003/00000/01268
d D 1.5 10/09/28 20:02:40 boiarino 5 4
c *** empty log message ***
e
s 00003/00001/01265
d D 1.4 10/09/28 12:20:41 boiarino 4 3
c *** empty log message ***
e
s 00033/00000/01233
d D 1.3 10/09/27 09:58:30 boiarino 3 2
c *** empty log message ***
e
s 00357/00094/00876
d D 1.2 10/03/09 12:44:43 boiarino 2 1
c *** empty log message ***
e
s 00970/00000/00000
d D 1.1 06/05/26 16:10:48 boiarino 1 0
c date and time created 06/05/26 16:10:48 by boiarino
e
u
U
f e 0
t
T
I 6
/*
E 6
I 3
#define PRIMEX
I 6
*/
E 6

E 3
I 2
/* if event rate goes higher then 10kHz, with randon triggers we have wrong
slot number reported in GLOBAL HEADER and/or GLOBAL TRAILER words; to work
around that problem temporary patches were applied - until fixed (Sergey) */
#define SLOTWORKAROUND
E 2
I 1

/* vmerol1.c - first readout list for VME crates */

I 2
/* with BUS analyser

nboards = 0
tdcbaseadr = 0
tdcadr = malloc(128)
tdc1190ScanCLAS(0x00020400, &nboards, tdcadr)
tdc1190ScanCLAS(0x00020300, &nboards, tdcadr)
tdc1190InitCLAS(nboards, tdcadr, &tdcbaseadr)

*/


E 2
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list VMEROL1 */
#define ROL_NAME__ "VMEROL1"

/* ts control */
#define TRIG_SUPV

/* name used by loader */
#define INIT_NAME vmerol1__init

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


#undef DEBUG


/* main TI board */
#define TIRADR   0x0ed0

static char *rcname = "RC00";

/* user buffer pointers - all NULL at initialization */
extern int *userbuffers[NUSERBUFFERS];
/* pointer to TTS structures for current roc */
extern TTSPtr ttp;

/* time profiling: 0-OFF, 1-ON */
D 2
static int timeprofile = 1;
E 2
I 2
static int timeprofile = 0;
E 2

/* histogram area */
static UThisti histi[NHIST];
static int neventh = 0;


D 2
#define NBOARDS 21    /* maximum number of VME boards */
E 2
I 2
#define NBOARDS 22    /* maximum number of VME boards: we have 21 boards, but numbering starts from 1 */
E 2


/* v1190 start */

D 2
#define MY_MAX_EVENT_LENGTH 3000 /* max words per v1190 board */
E 2
I 2
#define MY_MAX_EVENT_LENGTH 3000/*3200*/ /* max words per v1190 board */
E 2
#define TDC_OFFSET 0
#define CH_OFFSET  0

static int nboards;
static unsigned int tdcadr[NBOARDS];
static unsigned int tdcbaseadr;
static int maxbytes = 0;
static int error_flag[NBOARDS];
I 2
#ifdef SLOTWORKAROUND
static int slotnums[NBOARDS];
#endif
static unsigned int tdcbuftmp[NBOARDS*MY_MAX_EVENT_LENGTH+16];
static unsigned int *tdcbuf;
E 2
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
  sysBusToLocalAdrs(0x29,0,&offset);
  printf("A16 offset = 0x%08x\n",offset);

  tir[1] = (struct vme_tir *)(offset+TIRADR);


  /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&offset);
  printf("A24 offset = 0x%08x\n",offset);


/* v1190 start */

I 2
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
E 2

I 2

E 2
  /* returns 'nboards' and 'tdcadr' */
D 2
  tdc1190ScanCLAS(&nboards, tdcadr);
E 2
I 2

  /*mar 2, 2007 test (firmware1.2, 6 boards, about 800bytes event size, 1kHz):
mv5100, board-by-board: 77us, dmalist: 79us, cblt: 48us
mv6100, board-by-board: 106us, dmalist: 57us, cblt: 56us
  */





  /* v1190/v1290 TDC setup (see tdc890.h for possible options) */
  /* CBLT ROCs: 20-ec3, 21-ec4, 22-sc2, 24-lac2, 27-dvcs2 */
  if(rol->pid==20||rol->pid==21||rol->pid==22||rol->pid==24||rol->pid==27)
  {
	/*tdc1190ScanCLAS(CLAS_A32_MBLT_CBLT, &nboards, tdcadr);*/
	tdc1190ScanCLAS(CLAS_A32_MBLT_FIFO, &nboards, tdcadr);
    /*tdc1190ScanCLAS(CLAS_A32_2eSST_FIFO, &nboards, tdcadr);*/
    printf("-------========-------> tdcbaseadr=0x%08x\n",tdcbaseadr);
  }
D 6
  else if(rol->pid==28||rol->pid==29) /* 28-tage3, 29-tage2 */
E 6
I 6
  else if(rol->pid==28||rol->pid==29||rol->pid==17) /* 28-tage3, 29-tage2, 17-tage */
E 6
  {
    tdc1190ScanCLAS(CLAS_A32_2eSST_FIFO, &nboards, tdcadr);
  }
I 3
#ifdef PRIMEX
  else if(rol->pid==16||rol->pid==17||rol->pid==18) /* 16-tage2, 17-tage3, 18-primextdc1 */
  {
    tdc1190ScanCLAS(CLAS_A32_2eSST_FIFO, &nboards, tdcadr);
  }
#endif
E 3
  else if(rol->pid == 0) /*croctest2*/
  {
    /*tdc1190ScanCLAS(CLAS_A32_MBLT_CBLT, &nboards, tdcadr);*/

	/*firmware 1.2, numbers for 2 v1190 boards: 768 bytes data plus headers (in brackets - headers only)*/
	
    /*tdc1190ScanCLAS(CLAS_A32_MBLT_FIFO, &nboards, tdcadr);*/ /*6100: board-by-board:46(30)us,list:32(16)us*/ /*50MB/s*/

	/*tdc1190ScanCLAS(CLAS_A32_2eVME_FIFO, &nboards, tdcadr);*/ /*6100: board-by-board:39(30)us,list:26(16)us*/ /*80MB/s*/
                                          /*firmware1.3->*/ /*6100: board-by-board:38(30)us,list:24(16)us*/ /*96MB/s*/

	/*tdc1190ScanCLAS(CLAS_A32_2eVME, &nboards, tdcadr);*/      /*6100: board-by-board:58(51)us,list:n/a*/ /*100MB/s??*/



    /* 1.2, board-by-board, event size about 600(160) bytes) */
    /*tdc1190ScanCLAS(CLAS_A32_MBLT_FIFO, &nboards, tdcadr);*/ /* 102(93)us */
    /*tdc1190ScanCLAS(CLAS_A32_MBLT, &nboards, tdcadr);*/ /* 165(157)us */

    /* 1.5, board-by-board, event size about 600(160) bytes) */
    /*tdc1190ScanCLAS(CLAS_A32_MBLT_FIFO, &nboards, tdcadr);*/ /* 104(94)us */
    /*tdc1190ScanCLAS(CLAS_A32_MBLT, &nboards, tdcadr);*/ /* 167(158)us */
    /*tdc1190ScanCLAS(CLAS_A32_2eVME_FIFO, &nboards, tdcadr);*/ /* 96(92)us [DMA list: 47(41)us]*/
    /*tdc1190ScanCLAS(CLAS_A32_2eVME, &nboards, tdcadr);*/ /* 159(156)us */
    /*tdc1190ScanCLAS(CLAS_A32_2eSST_FIFO, &nboards, tdcadr);*/ /* 114(111)us [DMA list: does not work]*/
    /*tdc1190ScanCLAS(CLAS_A32_2eSST, &nboards, tdcadr);*/ /* 156(154)us */


    /* BERR and RETRY connected on board; 2 v1190s, DMA linked list only */
    /* empty about 50 bytes, full about 550 bytes (512bytes data)*/

    tdc1190ScanCLAS(CLAS_A32_MBLT_FIFO, &nboards, tdcadr); /*27(16)us*/
    /*tdc1190ScanCLAS(CLAS_A32_MBLT, &nboards, tdcadr);*/ /*reads first board only !!!*/

    /*tdc1190ScanCLAS(CLAS_A32_2eVME_FIFO, &nboards, tdcadr);*/ /*22(16)us*/
    /*tdc1190ScanCLAS(CLAS_A32_2eVME, &nboards, tdcadr);*/ /**/

    /*tdc1190ScanCLAS(CLAS_A32_2eSST_FIFO, &nboards, tdcadr);*/ /*20(16)us*/
    /*tdc1190ScanCLAS(CLAS_A32_2eSST, &nboards, tdcadr);*/ /*reads first board only !!!*/


/*
FIFO:
-> usrTempeDmaStatus(0,1)
DMA Chan 0 Status = 0x02000000
  Source(VME) address: 0x082600e0 
  Destination address: 0x0126d670 
  Byte Count         : 0x00000000 
  Execption Status   : 0x0008c900 
  Bus Error   address: 0x08271026 
  VME Address Mode : 2 
  VME Data Width   : 1 
  VME Transfer Mode: 4 
  STATUS of Last Transfer:
   Done Bit Set
value = 33554432 = 0x2000000
-> 

BERR:
-> usrTempeDmaStatus(0,1)
DMA Chan 0 Status = 0x10020000
  Source(VME) address: 0x08210800 
  Destination address: 0x0126dba0 
  Byte Count         : 0x00000000 
  Execption Status   : 0x0014e011 
  Bus Error   address: 0x08210030 
  VME Address Mode : 2 
  VME Data Width   : 1 
  VME Transfer Mode: 4 
  STATUS of Last Transfer:
   Error Bit Set
     Source: VME Bus
value = 268566528 = 0x10020000
-> 
*/


	/*2eSST boards, firmware 0.6*/
    /*4 boards - 32(28)us */
    /*3 boards - 26(22)us */
    /*2 boards - 20(16)us */

    printf("-------========-------> tdcbaseadr=0x%08x\n",tdcbaseadr);
  }
  else
  {
    tdc1190ScanCLAS(CLAS_A32_MBLT_FIFO, &nboards, tdcadr);
    printf("-------========-------> tdcbaseadr=0x%08x\n",tdcbaseadr);
  }


  /*
ON:
bcnaf croctest4 0 2 3 0 16 0x0
bcnaf croctest4 0 2 4 0 16 0x0
bcnaf croctest4 0 2 5 0 16 0x0
bcnaf croctest4 0 2 6 0 16 0x0

OFF:
bcnaf croctest4 0 2 3 0 16 0xffff
bcnaf croctest4 0 2 4 0 16 0xffff
bcnaf croctest4 0 2 5 0 16 0xffff
bcnaf croctest4 0 2 6 0 16 0xffff


   */

  /*bus error disable
-> usrTempeDmaStatus(0,1)
DMA Chan 0 Status = 0x10020000
  Source(VME) address: 0x08210800 
  Destination address: 0x01236990 
  Byte Count         : 0x00002a00 
  Execption Status   : 0x0014e011 
  Bus Error   address: 0x08210030 
  VME Address Mode : 2 
  VME Data Width   : 1 
  VME Transfer Mode: 4 
  STATUS of Last Transfer:
   Error Bit Set
     Source: VME Bus
value = 268566528 = 0x10020000
-> 

-> usrTempeDmaStatus(0,1)
DMA Chan 0 Status = 0x10020000
  Source(VME) address: 0x08210800 
  Destination address: 0x01236990 
  Byte Count         : 0x00002a00 
  Execption Status   : 0x0014e011 
  Bus Error   address: 0x08210030 
  VME Address Mode : 2 
  VME Data Width   : 1 
  VME Transfer Mode: 4 
  STATUS of Last Transfer:
   Error Bit Set
     Source: VME Bus
value = 268566528 = 0x10020000
->

-> usrTempeDmaStatus(0,1)
DMA Chan 0 Status = 0x10020000
  Source(VME) address: 0x08210800 
  Destination address: 0x0126db90 
  Byte Count         : 0x00000000 
  Execption Status   : 0x0014e011 
  Bus Error   address: 0x08210030 
  VME Address Mode : 2 
  VME Data Width   : 1 
  VME Transfer Mode: 4 
  STATUS of Last Transfer:
   Error Bit Set
     Source: VME Bus
value = 268566528 = 0x10020000
->
  */



E 2
  printf("vmerol1: nboards=%d\n",nboards);
  for(ii=0; ii<nboards; ii++)
  {
    printf("vmerol1: tdcadr[%2d] = 0x%08x\n",ii,tdcadr[ii]);
  }



  /* L2 delayed against L1 on 3320ns, so offset=-3500ns and window=400ns */ 

  /* window parameters:   width  offset */
  if(rol->pid == 29)
  {
    /*tdc1190SetGlobalWindow( 750,  -4050 );*/
D 2
    tdc1190SetGlobalWindow( 1350,  -4050 );
E 2
I 2
    /*tdc1190SetGlobalWindow( 1350,  -4050 );*/
    tdc1190SetGlobalWindow( 1450,  -4050 );
E 2
  }
  else if(rol->pid == 28)
  {
    /*tdc1190SetGlobalWindow( 750,  -4050 );*/
D 2
    tdc1190SetGlobalWindow( 1350,  -4050 );
E 2
I 2
    /*tdc1190SetGlobalWindow( 1350,  -4050 );*/
    tdc1190SetGlobalWindow( 1450,  -4050 );
E 2
  }
I 6
  else if(rol->pid == 17)
  {
    /*tdc1190SetGlobalWindow( 750,  -4050 );*/
    /*tdc1190SetGlobalWindow( 1350,  -4050 );*/
    tdc1190SetGlobalWindow( 1450,  -4550 );
  }
E 6
  else if(rol->pid == 20)
  {
    /*tdc1190SetGlobalWindow( 600,  -3800 );*/
    tdc1190SetGlobalWindow( 1200,  -3800 );
  }
  else if(rol->pid == 21)
  {
    /*tdc1190SetGlobalWindow( 400,  -3600 );*/
    tdc1190SetGlobalWindow( 1000,  -3600 );
  }
  else if(rol->pid == 22)
  {
    /*tdc1190SetGlobalWindow( 500,  -3600 );*/
    tdc1190SetGlobalWindow( 1100,  -3600 );
  }
D 2
  else
E 2
I 2
  else if(rol->pid == 27)
E 2
  {
D 2
    /*tdc1190SetGlobalWindow( 900,  -3900 );*/
    tdc1190SetGlobalWindow( 1500,  -3900 );
E 2
I 2
    tdc1190SetGlobalWindow( 1500,  -4500 );
E 2
  }
D 2

  printf("ttp=0x%08x\n",ttp);
  /* v1190/v1290 TDC setup (see tdc890.h for possible options) */
  if(/*(ttp != NULL) &&*/ (/*(rol->pid == 22) ||*/ (rol->pid == 29)) )
E 2
I 2
  else if(rol->pid == 0)
E 2
  {
D 2
    printf("Use CBLT method for ROC # %d\n",rol->pid);
    tdc1190InitCLAS(CLAS_A32_MBLT_BERR, nboards, tdcadr, &tdcbaseadr);
    printf("-------========-------> tdcbaseadr=0x%08x\n",tdcbaseadr);
E 2
I 2
    tdc1190SetGlobalWindow( 1500,  -3600 );
E 2
  }
I 3
#ifdef PRIMEX
  else if(rol->pid == 16) /*primex tage2*/
  {
    tdc1190SetGlobalWindow( 2500,  -1500 );
  }
  else if(rol->pid == 17) /*primex tage3*/
  {
    tdc1190SetGlobalWindow( 2500,  -1500 );
  }
  else if(rol->pid == 18) /*primex primextdc1*/
  {
    tdc1190SetGlobalWindow( 2500,  -1500 );
I 6
    /*tdc1190SetGlobalWindow( 100,  -200 ); good*/
    /*tdc1190SetGlobalWindow( 400,  -200 ); slot 19 tdc 00 occationally, err=0x1000 */
    /*tdc1190SetGlobalWindow( 600,  -300 ); slots 17(01),18(00),19(00), err=0x1000*/
E 6
  }
#endif
E 3
D 2
  else if(rol->pid == 0) /*croctest2 (timing for mv6100 - 3 empty boards)*/
  {
    /*tdc1190InitCLAS(CLAS_A24_D32_FIFO, nboards, tdcadr, &tdcbaseadr);34us*/
    /*tdc1190InitCLAS(CLAS_A24_D32_BERR, nboards, tdcadr, &tdcbaseadr);32us*/

    /*tdc1190InitCLAS(CLAS_A24_BLT_FIFO, nboards, tdcadr, &tdcbaseadr);71us*/
    /*tdc1190InitCLAS(CLAS_A24_BLT_BERR, nboards, tdcadr, &tdcbaseadr);105*/

    /*tdc1190InitCLAS(CLAS_A24_MBLT_FIFO, nboards, tdcadr, &tdcbaseadr);71us*/
    /*tdc1190InitCLAS(CLAS_A24_MBLT_BERR, nboards, tdcadr, &tdcbaseadr);91us*/

    /*tdc1190InitCLAS(CLAS_A32_BLT_BERR, nboards, tdcadr, &tdcbaseadr);46us*/
	tdc1190InitCLAS(CLAS_A32_MBLT_BERR, nboards, tdcadr, &tdcbaseadr);/*42us*/

/*tdcbaseadr = 0x08000000;-works somehow !!*/
    printf("-------========-------> tdcbaseadr=0x%08x\n",tdcbaseadr);
  }
E 2
  else
  {
D 2
    printf("Use MBLT method for ROC # %d\n",rol->pid);

    /* 202us - good: 10kHz pulse -> 4kHz rate (mv5500: 371us)
    tdc1190InitCLAS(CLAS_A24_D32_FIFO, nboards, tdcadr, &tdcbaseadr);
	*/

    /* 194us - good: 10kHz pulse -> 4kHz rate (mv5500:357us)
    tdc1190InitCLAS(CLAS_A24_D32_BERR, nboards, tdcadr, &tdcbaseadr);
    */

    /* 270 ??? (mv5500: 172us)
    tdc1190InitCLAS(CLAS_A24_BLT_FIFO, nboards, tdcadr, &tdcbaseadr);
	*/

	/*  GOOD   (mv5500: 161us)
    tdc1190InitCLAS(CLAS_A24_MBLT_FIFO, nboards, tdcadr, &tdcbaseadr);
	*/

    /* BAD (mv5500: 115us)
    tdc1190InitCLAS(CLAS_A32_BLT_BERR, nboards, tdcadr, &tdcbaseadr);
	*/

    /* ??? (mv5500: 92us) */
    tdc1190InitCLAS(CLAS_A32_MBLT_BERR, nboards, tdcadr, &tdcbaseadr);




    printf("-------========-------> tdcbaseadr=0x%08x\n",tdcbaseadr);
E 2
I 2
    /*tdc1190SetGlobalWindow( 900,  -3900 );*/
    tdc1190SetGlobalWindow( 1500,  -3900 );
E 2
  }

I 2
  printf("ttp=0x%08x\n",ttp);
E 2

I 2
  tdc1190InitCLAS(nboards, tdcadr, &tdcbaseadr);
E 2

I 2

E 2
  /* if VME crate is not 64X 
  for(ii=0; ii<nboards; ii++)
  {
    tdc890SetGeoAddress(tdcadr[ii], (ii+1));
  }
  */

/* v1190 end */

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
D 2
  CTRIGRSA(VME, 1, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */
E 2
I 2
  CTRIGRSA(VME, TIR_SOURCE, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */
E 2

  rol->poll = 0;

  tttest("\npolar rol1:");

  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);


I 2
#ifdef SLOTWORKAROUND
  for(ii=0; ii<nboards; ii++)
  {
    tdc890GetGeoAddress(tdcadr[ii], &slot);
	slotnums[ii] = slot;
  }
#endif
E 2

D 2
if((ttp != NULL) && (rol->pid != 24) && (rol->pid != 28) && (rol->pid != 29)
 && (rol->pid != 0))
E 2
I 2
if((ttp != NULL) && (rol->pid != 28) && (rol->pid != 29) && (rol->pid != 0))
E 2
{
I 3
#ifdef PRIMEX
if((rol->pid != 16) && (rol->pid != 17) && (rol->pid != 18))
{
#endif
E 3

  /* disable v1190 channels undescribed in TT */
  for(ii=0; ii<nboards; ii++)
  {
    tdc890GetGeoAddress(tdcadr[ii], &slot);
    printf("v1190 address 0x%08x at slot %2d\n",tdcadr[ii],slot);
I 2

E 2
    tdc890EnableAllChannels(tdcadr[ii]);
    for(channel=1; channel<128; channel++) /* always keep channel 0 enabled */
    {
      if(ttp->idnr[slot][channel] <= 0)
      {
        /*printf("disable slot# %2d channel# %3d\n",slot,channel);*/
        tdc890DisableChannel(tdcadr[ii], channel);
      }
    }


D 2
	/*	
if(ii==3||ii==4)
E 2
I 2


	/*disable some channels manually if needed*/
	
/*if(ii==2)
E 2
{
D 2
  for(channel=4; channel<5; channel++)
E 2
I 2
  for(channel=0; channel<128; channel++)
E 2
  {
D 2
    printf("[%d] disable channel %d\n",ii,channel);
E 2
    tdc890DisableChannel(tdcadr[ii], channel);
  }
D 2
}
*/
E 2
I 2
}*/
E 2

I 2



E 2
    tdc890GetChannels(tdcadr[ii], pattern);

    printf("TDC connectors:    DL   DR   CL   CR   BL   BR   AL   AR\n");
    printf("channels pattern: %04x %04x %04x %04x %04x %04x %04x %04x\n",
      pattern[7],pattern[6],pattern[5],pattern[4],
      pattern[3],pattern[2],pattern[1],pattern[0]);
  }

I 3
#ifdef PRIMEX
E 3
}
I 3
#endif
}
E 3


I 2
D 7
/*1-trailing edge, 2-leading, 3-both
tdc890SetEdgeDetectionConfig(0x90260000, 0x3);
*/
E 7
I 7
if(rol->pid == 0)
{
  /*1-trailing edge, 2-leading, 3-both*/
E 7
E 2

I 7
tdc890SetEdgeDetectionConfig(0x90210000, 0x3);
tdc890SetEdgeDetectionConfig(0x90220000, 0x3);
tdc890SetEdgeDetectionConfig(0x90230000, 0x3);
tdc890SetEdgeDetectionConfig(0x90240000, 0x3);

}

E 7
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
I 2
  int extra;
E 2

  logMsg("INFO: User Go ...\n",1,2,3,4,5,6);


  /* v1190/v1290 */
  if(tdcbaseadr)
  {
    logMsg("Use MCST_CBLT method\n",1,2,3,4,5,6);


	/*temporary until mv6100 mapping understood
    tdc890Clear(tdcbaseadr);
	*/
    for(ii=0; ii<nboards; ii++)
    {
      tdc890Clear(tdcadr[ii]);
    }

I 2
/*workaround
workaround1();
*/
E 2

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

  rol->poll = 0;

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



/* test setup: 11 boards from slot 5 to slot 15
================================================


SN 24, slot#  6, tdc# 02, error_flags=0x00004000
-> mytest2(0xf0120000)
err=0x0002 -> coarse error (parity error on coarse count)
lock=0x0001

SN 30, slot# 11, tdc# 03, error_flags=0x00004000
err=0x0008
lock=0x0001

SN 34, slot# 12, tdc# 00, error_flags=0x00004000
err=0x0008
lock=0x0001

SN 29, slot# 13, tdc# 01, error_flags=0x00004000
err=0x0008
lock=0x0001
*/

/*
TAGE2 crate, real beam
======================

SN 25, slot#11, tdc# 02
-> mytest2(0xfa230000)
err=0x0008 lock=0x0001
value = 23 = 0x17

*/



/* during TDC_CALIB run 16-dec-2005: ROC21 (ec4)
interrupt:  ERR: event#  152962, slot# 19, tdc# 02, error_flags=0x00004000, err=0x109528, lock=0x0000
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
I 2
  unsigned short level;
E 2

  int ii, njjloops, blen, jj, nev, rlen, rlenbuf[NBOARDS], nevts, nwrds;
  unsigned long res, datascan, mymask=0xfff0;
  unsigned long tdcslot, tdcchan, tdcval, tdc14, tdcedge, tdceventcount;
  unsigned long tdceventid, tdcbunchid, tdcwordcount, tdcerrorflags;
D 2
  unsigned int *tdc, *tdchead, itdcbuf, tdcbuf[NBOARDS*MY_MAX_EVENT_LENGTH];
E 2
I 2
  unsigned int *tdc, *tdchead, itdcbuf;
  int nheaders, ntrailers;
I 6
  int zerochan[22];
E 6
#ifdef SLOTWORKAROUND
  unsigned long tdcslot_h, tdcslot_t, remember_h;
#endif
E 2

  rol->dabufp = (long *) 0;

  /*open event type EVTYPE of BT_UI4*/
  CEOPEN(EVTYPE, BT_UI4);

  jw = rol->dabufp;
I 2
  /*
logMsg("000\n",1,2,3,4,5,6);
  */
E 2

D 2

E 2
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
I 2
  else if((syncFlag==0)&&(EVTYPE==15)) /* helicity strob events */
  {
    ;
  }
E 2
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
D 2
      TIMER_START;
E 2
I 2
	  TIMER_START;
E 2
    }
	



goto a321;
{
  /* delay */
  int i, j, k, aa, bb, cc;
  for(i=0; i<100; i++)
  {
    for(j=0; j<100; j++)
    {
	  /* mv6100: 100->2416us (400Hz), 10->263us (3.5kHz) */
      for(k=0; k<10; k++)
      {
        aa=123;
        bb=aa;
        cc=bb;
      }
	}
  }
}
a321:




/*
goto a123;
*/

I 6
    for(jj=0; jj<22; jj++) zerochan[jj] = -1;

E 6
    if(tdcbaseadr)
    {
D 2
      /*CLAStdc890ReadEventCBLT(nboards, tdcbaseadr, tdcadr, tdcbuf, rlenbuf);*/

E 2
I 2
	  /*
      CLAStdc890ReadEventCBLT(nboards, tdcbaseadr, tdcadr, tdcbuf, rlenbuf);
	  */
E 2
      CLAStdc890ReadEventCBLTStart(nboards, tdcbaseadr, tdcadr, tdcbuf, rlenbuf);
      CLAStdc890ReadEventCBLTStop(rlenbuf);
D 2

E 2
I 2
	  
E 2
    }
    else
    {
D 2
	  
E 2
I 2



	  /*DMA board-by-board readout*/
E 2
      tdc1190ReadEventCLAS(nboards, tdcadr, tdcbuf, rlenbuf);
D 2
	  /*
E 2
I 2
  
    
      

	  /*does not work for BERR !!!*/
	  /*DMA list readout
E 2
      tdc1190ReadEventStart(nboards, tdcadr, tdcbuf, rlenbuf);
I 2
*/
E 2
      res = tdc1190ReadEventDone();
      if(res == -1)
      {
I 2
        logMsg("BAD%d\n",res,2,3,4,5,6);
E 2
        for(jj=0; jj<nboards; jj++) rlenbuf[jj] = 0;
      }
D 2
	  */
E 2
I 2
      /*else logMsg("GOOD%d\n",res,2,3,4,5,6);*/
	  

E 2
    }

I 2
    if(timeprofile)
    {
      TIMER_STOP(100000,1000+rol->pid);
    }
E 2

I 2

E 2
	/*
goto a123;
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
I 2
    nheaders = ntrailers = 0;

E 2
    if(tdcbaseadr) njjloops = 1;
    else           njjloops = nboards;
	/*
for(i=0; i<200; i++) tmpgood[i] = tdcbuf[i];
	*/
    for(jj=0; jj<njjloops; jj++)
    {
      rlen = rlenbuf[jj];
I 2
	  
#ifdef DEBUG
	  
      tdc890GetAlmostFullLevel(tdcadr[jj], &level);
      ii=tdc890StatusAlmostFull(tdcadr[jj]);
      logMsg("jj=%d, rlen=%d, almostfull=%d level=%d\n",jj,rlen,ii,level,5,6);
	  
#endif
	  
E 2
      if(rlen <= 0) continue;

      tdc = &tdcbuf[itdcbuf];
      itdcbuf += rlen;

      for(ii=0; ii<rlen; ii++)
      {
		
        if( ((tdc[ii]>>27)&0x1F) == 0)
        {
          nbcount ++;
          *rol->dabufp ++ = (tdc[ii] | (tdcslot<<27));
I 6

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


E 6
#ifdef DEBUG
          tdcedge = ((tdc[ii]>>26)&0x1);
          tdcchan = ((tdc[ii]>>19)&0x7F);
          tdcval = (tdc[ii]&0x7FFFF);
D 2
		  if(tdcchan<20)
E 2
I 2
		  if(1/*tdcchan<2*/)
E 2
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
D 2
          logMsg(" FILLER:  tdc=%02d event_id=%05d word_count=%05d\n",
            (int)tdc14,(int)tdceventid,(int)tdcwordcount,4,5,6);
E 2
I 2
          logMsg(" FILLER\n",1,2,3,4,5,6);
E 2
#endif
        }
I 2
        else if( ((tdc[ii]>>27)&0x1F) == 0x11)
        {
          nbsubtract ++;
#ifdef DEBUG
          logMsg(" TIME TAG: %08x\n",(int)(tdc[ii]&0x7ffffff),2,3,4,5,6);
#endif
        }
E 2
        else if( ((tdc[ii]>>27)&0x1F) == 8)
        {
          nbcount = 1; /* counter for the number of output words from board */
          nbsubtract = 9; /* # words to subtract including errors */
          tdcslot = tdc[ii]&0x1F;
          tdceventcount = (tdc[ii]>>5)&0x3FFFFF;
          tdchead = (unsigned int *) rol->dabufp; /* remember pointer */
D 2
          *rol->dabufp ++ = tdcslot/*(tdcslot<<27)*/;
E 2
I 2
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

I 3
		  
E 3
E 2
#ifdef DEBUG
I 3
		  
E 3
          logMsg("GLOBAL HEADER: %d words, 0x%08x (slot=%d nevent=%d) -> header=0x%08x\n",
            rlen,tdc[ii],tdc[ii]&0x1F,(tdc[ii]>>5)&0xFFFFFF,*tdchead,6);
I 3
		  
E 3
#endif
I 3
		  
E 3
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x10)
        {
I 2
#ifdef SLOTWORKAROUND
          /* double check for slot number */
          tdcslot_t = tdc[ii]&0x1F;
          if(tdcslot_h>21||tdcslot_t>21||tdcslot_h!=tdcslot_t)
          {
            /*logMsg("WARN: slot from header=%d (0x%08x), from trailer=%d (0x%08x), must be %d\n",
			  tdcslot_h,remember_h,tdcslot_t,tdc[ii],tdcslot,6)*/;
          }
#endif
E 2
          /* put nwords in header; substract 4 TDC headers, 4 TDC EOBs,
          global trailer and error words if any */
          /**tdchead = (*tdchead) | (((tdc[ii]>>5)&0x3FFF) - nbsubtract);*/
          *tdchead |= ((((tdc[ii]>>5)&0x3FFF) - nbsubtract)<<5);
          if((((tdc[ii]>>5)&0x3FFF) - nbsubtract) != nbcount)
D 2
          {
E 2
I 2
          {			
			
E 2
            logMsg("ERROR: word counters: %d != %d (subtract=%d)\n",
			  (((tdc[ii]>>5)&0x3FFF) - nbsubtract),nbcount,nbsubtract,4,5,6);
I 2
			
E 2
			/*
for(i=0; i<200; i++) tmpbad[i] = tdcbuf[i];
logMsg("tmpgood=0x%08x tmpbad=0x%08x\n",tmpgood,tmpbad,3,4,5,6);
			*/
          }
D 2
	  
E 2
I 2
          ntrailers ++;

E 2
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

D 2

E 2
    }

I 6

#ifdef PRIMEXXX
    /*PRIMEX: zero chan check*/
    for(ii=0; ii<nboards; ii++)
    {
	  if(zerochan[slotnums[ii]]<=0)  logMsg("ERROR at slot %2d: no reference signal\n",slotnums[ii],2,3,4,5,6);
    }
	/*end of zero chan check*/
#endif


E 6
I 2
	/*
    if(nheaders!=ntrailers || nboards!=nheaders)
      logMsg("ERROR: nboards=%d, nheaders=%d, ntrailers=%d\n",nboards,nheaders,ntrailers,4,5,6);
	*/
E 2

D 2
    blen = rol->dabufp - (long *)&jw[ind+1];
E 2
I 2
    blen = rol->dabufp - (int *)&jw[ind+1];
E 2
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


    /* for physics sync event, make sure all board buffers are empty */
    if(syncFlag==1)
    {
      int scan_flag;
      unsigned short slot, nev16;
D 4

E 4
I 4
	  /*
logMsg("SYNC reached\n",1,2,3,4,5,6);
	  */
I 5
	  /*
goto a123;
	  */
E 5
E 4
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
      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = SYNC + scan_flag;
      rol->dabufp += bosMclose_(jw,ind2,1,1);
	  /*logMsg("PTRN done.\n",1,2,3,4,5,6);*/
    }

a123:
D 2
	
    if(timeprofile)
    {
      TIMER_STOP(100000,1000+rol->pid);
    }
	
E 2
I 2
	;	
E 2

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
