/* if event rate goes higher then 10kHz, with randon triggers we have wrong
slot number reported in GLOBAL HEADER and/or GLOBAL TRAILER words; to work
around that problem temporary patches were applied - until fixed (Sergey) */
#define SLOTWORKAROUND

/* vmenrol1.c - first readout list for VME crates using new Bryan Mossit's library (Feb 2011) */

#define BT_UI4_ty  0x01
#define BT_BANKS_ty  0x0e

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
/* Se non e' definito standalone, compila e definisci trig_supv */
/* E' definito nel makefile */
#ifndef STANDALONE 
#define TRIG_SUPV
#endif

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
#include "c1190Lib.h"
#include "v895.h"
#include "v1495.h"
#include "fadcLib.h"

/* from fputil.h */
#define SYNC 0x20000000


#undef DEBUG

/* polling mode if needed */
/*#define POLLING_MODE*/


/* main TI board */
/* INDIRIZZO DELLA TI */
#define TIRADR   0x0ed0
/*Address of the Signal Distribution Module (A16)*/
#define SDM_ADDR 0x0f00 

static char *rcname = "RC00";

/* user buffer pointers - all NULL at initialization */
extern int *userbuffers[NUSERBUFFERS];
/* pointer to TTS structures for current roc */
extern TTSPtr;

/* time profiling: 0-OFF, 1-ON */ 
static int timeprofile = 0;

/* histogram area */
static UThisti histi[NHIST]; 
static int neventh = 0;

unsigned int fadcSlotMask   = 0;    /* bit=slot (starting from 0) */

/*These are defined in fadcLib.c, and so we use the keyword extern*/
extern   int fadcA32Base;           /* This will need to be reset from it's default so that it does not overlap with the TI */
extern   int nfadc;                 /* Number of FADC250s verified with the library */
extern   int fadcID[FA_MAX_BOARDS]; /* Array of slot numbers, discovered by the library */ /*FA_MAX_BOARDS: 20 in fadcLib.h*/
static int NFADC;                   /* The Maximum number of tries the library will
                                     * use before giving up finding FADC250s */
static int FA_SLOT;                 /* We'll use this over and over again to provide
				                     * us access to the current FADC slot number */ 

static int FADC_ROFLAG = 1;  /* 0-noDMA, 1-board-by-board DMA, 2-chainedDMA */
static int fadc_mode = 1; /*1-raw,2-pulse,3-pulse integral*/
#define FIRST_FADC 0x010000
#define INCREMENT_FADC 0x001000
#define FADC_WINDOW_LAT      500  /* Trigger Window Latency (counts, 1 count=4ns)*/
#define FADC_WINDOW_WIDTH    500  /* Trigger Window Width (counts, 1 count=4ns)*/
#define FADC_DAC_LEVEL       3300 /* Internal DAC Level */
#define FADC_THRESHOLD       0    /* Threshold for data readout (NON IN RAW MODE) */

int fadc_window_lat = FADC_WINDOW_LAT;
int block_level = 1; /*NUMBER OF EVENTS IN BLOCK_TRANSFER*/


#define NTICKS 1000 /* the number of ticks per second */
#define NBOARDS 22    /* maximum number of VME boards: we have 21 boards, but numbering starts from 1 */
#define MY_MAX_EVENT_LENGTH FADC_WINDOW_WIDTH*16+16+5   /* max 32-bit words per board: assume raw-mode readout */

#define V1495_ADDR 0xa00000

static unsigned int tdcbuftmp[NBOARDS*MY_MAX_EVENT_LENGTH+16];
static unsigned int *tdcbuf;

/*TDC*/
static int ntdcs;
static int slotnums[NBOARDS];

static unsigned int Dma_words=MY_MAX_EVENT_LENGTH;
static unsigned int Dma_mode=3;

void
tsleep(int n)
{
  taskDelay ((sysClkRateGet() / NTICKS) * n);
}

 
/* standart functions */


char *
getFadcPedsFilename(int runNumber)
{
  char *dir = NULL;
  char *expid = NULL;
  static char fname[1024];

  if((dir=getenv("CLON_PARMS")) == NULL)
  {
    printf("ERROR: environment variable CLON_PARMS is not defined - exit\n");
    return(NULL);
  }
  sprintf(fname,"%s/pedestals/run%02d.ped",dir,runNumber);

  return fname;
}




  
static void
__download()
{
  unsigned long offset16,offset24,offset32, ii, jj;
  unsigned short iflag;
  int iFlag = 0;
  int ich,islot, isl, ichan, ipattern;	

  char *clonparms;
  char *expid;
  static char fname[1024];
  FILE   *fd;	
  rol->poll = 0; /*Rol: read out list. Look in rolInt.h */

  
  printf(">>>>>>>>>>>>>>>>>>>>>>>>>> ROCID = %d <<<<<<<<<<<<<<<<\n",rol->pid);

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");

  /* Must check address in TI module */
  /* TI uses A16 (0x29, 4 digits), not A24 */

  /* get global offset for A16 */
  sysBusToLocalAdrs(0x29,0,&offset16);
  printf("A16 offset = 0x%08x\n",offset16);

  tir[1] = (struct vme_tir *)(offset16+TIRADR);

  /* check 'tdcbuftmp' alignment and set 'tdcbuf' to 16-byte boundary */
  tdcbuf = &tdcbuftmp[0];/*Instruction to init the tdcbuf to a "real" memory location*/
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






  /* get global offset for A24 */
  sysBusToLocalAdrs(0x39,0,&offset24);
  printf("A24 offset = 0x%08x\n",offset24);

  /* get global offset for A32 */
  sysBusToLocalAdrs(0x09,0,&offset32);
  printf("A32 offset = 0x%08x\n",offset32);


  /*Configure TDCs*/
  ntdcs = tdc1190Init(0x10300000,0x00000,1,0); /*addr is the bus addr*/



  tdc1190SetDefaultWindowWidth(2000);  /*in ns*/  /*Minimo 25 ns, 1 colpo di clock */
  tdc1190SetDefaultWindowOffset(-2000);  /*From 0 to -800000 ns. It is the start of the window where searching for hits.*/
  tdc1190SetDefaultWindowExtraMargin(100); /*100 ns*/
  tdc1190Config(TDC1190_MBLT_FIFO); /*Trigger matching mode */

  for (jj=0;jj<ntdcs;jj++){
     tdc1190TriggerTimeTag(jj,1); /* Enable Trigger Time Tag */    
  }



  /*Configure FADCs*/
  iFlag=0;
  iFlag=0x13; /*0x13  CLK (FP)   TRIG (FP)     SYNC (FP)      (VME SDC Mode)*/
  iFlag=iFlag|SDM_ADDR;
  NFADC=7; /*No more than 7 with the SDC module*/ /*A.C. while fixing FA-TI*/
  fadcA32Base = 0x09000000; 	
  
  faInit(FIRST_FADC,INCREMENT_FADC,NFADC,iFlag); /*This also touches (trough extern) nfadc and fadcID[FA_MAX_BOARDS]*/
	
  NFADC=nfadc; /*Update what we got with the library results*/
  printf("I found %i FADC boards \n",NFADC);
  for (ii=0;ii<NFADC;ii++){
	printf("FADC %i: ID-> %i \n",ii,fadcID[ii]);
  }
	


  if(NFADC==1) FADC_ROFLAG = 1; /*no chainedDMA if one board only*/

  if(FADC_ROFLAG==2) faEnableMultiBlock(1);/*This also sets properly the BERR*/
  else faDisableMultiBlock();

  fadc250Config("fadc.cnf"); /*Load settings from the file, in $CLON_PARMS/fadc250*/


   /* Additional Configuration for each module */
  fadcSlotMask=0;
  for(islot=0; islot<NFADC; islot++) 
  {
    FA_SLOT = fadcID[islot];      /* Grab the current module's slot number */
    fadcSlotMask |= (1<<FA_SLOT); /* Add it to the mask */
    printf("=======================> fadcSlotMask=0x%08x\n",fadcSlotMask);
 /*   faSetDAC(FA_SLOT,FADC_DAC_LEVEL,0);*/
    faPrintDAC(FA_SLOT); 
   /*
   printf("fadc_mode=%d\n",fadc_mode);
    if(fadc_mode==1) 
    {
    
      faSetThreshold(FA_SLOT,FADC_THRESHOLD,0);
	
      faSetProcMode(FA_SLOT,1,fadc_window_lat,FADC_WNDOW_WIDTH,3,6,1,0);
    }
    else
    {
     
      faSetProcMode(FA_SLOT,fadc_mode,fadc_window_lat,FADC_WINDOW_WIDTH,5,30,3,0);
    }
    if(FADC_ROFLAG!=2) faEnableBusError(FA_SLOT);
    */

    faEnableBusError(FA_SLOT);


    /* Set the Block level */
    faSetBlockLevel(FA_SLOT, block_level);/*Sets the number of events in block-transfer*/


   }
 
  


  /*  Enable FADC channels - new place */
  /*Disable at 0, means enable all*/
  for(islot=0; islot<NFADC; islot++) 
  {
    FA_SLOT = fadcID[islot];
    faChanDisable(FA_SLOT,0x0);
  }

 faSDC_Enable(0);

logMsg("BEFORE DMA \n",1,2,3,4,5,6);
/*************************************/
  /* VME controller DMA initialization */
  usrVmeDmaInit();
  /* usrVmeDmaConfig params:
          first: 0 - A16,
                 1 - A24,
                 2 - A32
          second: 0 - D16 / SINGLE
                  1 - D32 / SINGLE
                  2 - D32 / BLOCK (BLT)  <- use this for DMA !
                  3 - D64 / BLOCK (MBLT) <- use this for DMA !
		  4 - D64 / 2eVME DMA
		  5 - D64 / 2eSST DMA  */
    usrVmeDmaConfig(2,Dma_mode);/* A32,  D64 / BLOCK (MBLT).  */
    logMsg("AFTER DMA \n",1,2,3,4,5,6);


    Dma_words=MY_MAX_EVENT_LENGTH;
    printf("Number of requested DMA words: %i \n",Dma_words);

    /*Andrea: my work-around for alignment*/
    if ((Dma_words%2)!=0) Dma_words+=1; /* 64 bits alignment */
    if (Dma_mode >= 0x04) {
	if ((Dma_words%4)!=0) Dma_words+=2; /* 128 bits alignment */
    } 	

 if (FADC_ROFLAG) Dma_words+=4; /*Andrea: to force BERR?*/ 
 printf("Number of aligned DMA words (32-bits each!): %i \n",Dma_words);
 logMsg("INFO: User Download Executed\n",1,2,3,4,5,6);


 /*Set the trigger*/
  v1495BDXTriggerConfig("trigger.cnf");
 
 
}


static void __prestart()
{
  char *env;
  char *histname = "ROL1 RC00";
  unsigned short value, value0, value1, array0[32], array1[32], data16,slot;
  int i, ii, ifull, jj,islot;
 

  printf("vmenrol1:: prestart start\n");fflush(stdout);
  rol->poll = 0;

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  VME_INIT; /* init trig source VME */

  /* Register an async trigger source, (up to 32 sources) */
  /* CTRIGRSA(source,code,handler,done)*/
  /* link async trig source VME 1 to usrtrig and usrtrig_done */
  CTRIGRSA(VME, TIR_SOURCE, usrtrig, usrtrig_done); /* second arg=1 - what is that ? */
  /*VME= */
  /*TIR_SOURCE= trigger interface board, e NON il trigger supervisor */
  /*usrtrig -> handler */
  /*usrtrig_done -> done */
  printf("vmenrol1:: set discriminator\n");fflush(stdout);
  v895Init();
  v895SetConfig("$CLON_PARMS/pretrigger/conf/discriminator.conf");	/*put here to force re-setting every prestart*/	


  rol->poll = 0;

  tttest("\npolar rol1:");

  sprintf((char *)&rcname[2],"%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);fflush(stdout);

#ifdef SLOTWORKAROUND
  for(ii=0; ii<ntdcs; ii++)
  {
    slot = tdc1190GetGeoAddress(ii);
    slotnums[ii] = slot;
    printf("[%d] slot %d\n",ii,slotnums[ii]);
  }
#endif

   fadc250Config("fadc.cnf");	
   /*For each run, write a pedestal file*/
   fadc250peds(getFadcPedsFilename(rol->runNumber)); /* at the end, this triggers fadc250Config("fadc.conf")*/
   
  /* 1) Load FADC pedestals from file for trigger path. (NOT USED IN THIS SETUP)
     2) Offset FADC threshold for each channel based on pedestal for both readout and trigger */
   faGLoadChannelPedestals(getFadcPedsFilename(rol->runNumber), 1);

  

   /* Program/Init VME Modules Here */
  for(islot=0; islot<NFADC; islot++)
  {
    FA_SLOT = fadcID[islot];
    faSetClockSource(FA_SLOT,1);/*0 1  Front Panel */
  }

  tsleep(1);



  for(islot=0; islot<NFADC; islot++)
  {

    FA_SLOT = fadcID[islot];
    faSoftReset(FA_SLOT,0);
    faResetToken(FA_SLOT);
    faResetTriggerCount(FA_SLOT);
    faPrintThreshold(FA_SLOT);
  }

  


  /*  Enable FADC */
  for(islot=0; islot<NFADC; islot++) 
  {
    FA_SLOT = fadcID[islot];
    /*faSetMGTTestMode(FA_SLOT,0);*/
   /* faSetBlockLevel(FA_SLOT,block_level); *//* done above !!??*/ 
    /*faChanDisable(FA_SLOT,0xffff);enabled in download*/
    faClear(FA_SLOT);
    faStatus(FA_SLOT,0);
    faEnable(FA_SLOT,0,0);
/*   faEnable(id,int eflag,int bank). bank is ignored. eflag=enable internal trigger processes (0/1)*/
  }
  
 

 
  /*  faSDC_Config(0,0x3); *//*Default Configuration. 0x3: busy for FADC 1 and 2*/
 
  faSDC_Enable(0);
  faSDC_Sync(); 
  faSDC_Status(0); /*0: meaningless*/
  
  /*Set the trigger*/
  v1495BDXTriggerConfig("trigger.cnf");


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

  printf("AAAAAAAAAAAAAAAAAAAa %i \n",rol->runNumber);
  

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
  int i, ii,islot;
  int extra;

  logMsg("INFO: User now go ...\n",1,2,3,4,5,6);
/*
  for(islot=0; islot<NFADC; islot++) 
  {
    FA_SLOT = fadcID[islot];  
    faEnable(FA_SLOT,0,0);
  }
  tsleep(1);

  faSDC_Sync();	
*/
  rol->poll = 0;

  for(ii=0; ii<ntdcs; ii++)
  {
    tdc1190Clear(ii);
  }	

  CDOENABLE(VME,1,0);

  /* from parser */
 

  return;
}


void
usrtrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{


  int *jw, ind, ind2, i, ii, jj, jjj, iii, blen, len, rlen, itdcbuf, nbytes;
  int rlenbuf[NBOARDS];
  unsigned int *tdcbuf_save, *tdc;
  unsigned int *dabufp1, *dabufp2;
  int vmeadr;
  unsigned int datascan, mask;
  int nwords, njjloops;
  unsigned short *dabufp16, *dabufp16_save;
  int islot;
  int dCnt, idata , val;
  int stat, itime, gbready;



 
#ifdef SLOTWORKAROUND
  unsigned long tdcslot_h, tdcslot_t, remember_h;
#endif

    rol->dabufp = (long *) 0;

  /*open event type EVTYPE of BT_UI4*/
 /* CEOPEN(EVTYPE, BT_UI4);*/
    CEOPEN(EVTYPE,BT_BANKS); 
/*
#define BT_UI4_ty  0x01
#define BT_BANKS_ty  0x0e
*/

  jw = rol->dabufp; /*output write pointer */
  /*
logMsg("000\n",1,2,3,4,5,6);
  */

  /***/

  /* at that moment only second CODA world defined  */
  /* first CODA world (length) undefined, so set it */
  /*jw[ILEN] = 1;*/ jw[-2] = 1; /*jw[-2] e jw[-1]: coda header */

 

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


    /* open data bank */
    /* bosMopen_(int *jw, char *name, int nr, int ncol, int nrow);*/
   /* if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0) 
    {
      logMsg("bosMopen_ Error: %d\n",ind,0,0,0,0,0);
    }
    rol->dabufp += NHEAD;*/




   
   tdc1190ReadStart(tdcbuf,rlenbuf); 
   BANKOPEN(0xe10B,1,rol->pid); /*Tag type num*/
   for(ii=0; ii<rlenbuf[0]; ii++) *rol->dabufp++ = tdcbuf[ii];
   BANKCLOSE;
 /*  printf("TDC: %x %x %x %x %x -- %x %x\n",tdcbuf[0],tdcbuf[1],tdcbuf[2],tdcbuf[3],tdcbuf[4],tdcbuf[rlenbuf[0]-2],tdcbuf[rlenbuf[0]-1]);*/
  /**************************************************************************************
 *	HERE GOES READOUT 
 *  faReadBlock - General Data readout routine
 *
 *    id    - Slot number of module to read
 *    data  - local memory address to place data
 *    nwrds - Max number of words to transfer
 *    rflag - Readout Flag
 *              0 - programmed I/O from the specified board
 *              1 - DMA transfer using Universe/Tempe DMA Engine 
 *                    (DMA VME transfer Mode must be setup prior)
 *              2 - Multiblock DMA transfer (Multiblock must be enabled
 *                     and daisychain in place or SD being used)
 */
     
/*COMMENT OUT FOLLOWING 'FOR' LOOP FOR SPEED UP !!!*/
    for(itime=0; itime<100000; itime++) {
	    gbready = faGBready();
	    stat = (gbready == fadcSlotMask);
	    if (stat>0) { 
	      break;
	    } 
	}
	dCnt=0;
        if(stat>0){

        FA_SLOT = fadcID[0];
        if(FADC_ROFLAG==2) /*Chained DMA*/
        { 
	   dCnt = faReadBlock(FA_SLOT,&tdcbuf[rlenbuf[0]],Dma_words*NBOARDS,FADC_ROFLAG);
        }
        else {/*0: noDma, 1: board by board DMA */
          for(jj=0; jj<NFADC; jj++){
	    len = faReadBlock(fadcID[jj],&tdcbuf[dCnt+rlenbuf[0]],Dma_words,FADC_ROFLAG);          
	    dCnt += len;
	   }
	}
		
	/*printf("Here: %i %x %x %x %x %x %x\n",dCnt,tdcbuf[rlenbuf[0]],tdcbuf[rlenbuf[0]+1],tdcbuf[rlenbuf[0]+2],tdcbuf[rlenbuf[0]+3],tdcbuf[rlenbuf[0]+4],tdcbuf[rlenbuf[0]+5]);fflush(stdout);*/

	BANKOPEN(0xe109,1,rol->pid); 
 	for(jj=0; jj<dCnt; jj++){
		 *rol->dabufp++ = tdcbuf[jj+rlenbuf[0]];			
		}
   	BANKCLOSE;
     /*   printf("%x %x %x %x %x %x %x\n",tdcbuf[rlenbuf[0]+0],tdcbuf[rlenbuf[0]+1],tdcbuf[rlenbuf[0]+2],tdcbuf[rlenbuf[0]+3],tdcbuf[rlenbuf[0]+4],tdcbuf[rlenbuf[0]+5],tdcbuf[rlenbuf[0]+6]);*/
	} /*END OF READOUT FROM THE BOARD*/
	
 

	 else   {
	    printf ("FADCs: no events   stat=%d  gbready = 0x%08x  fadcSlotMask = 0x%08x\n",stat,gbready,fadcSlotMask);
            printf("Missing slots:");
        for(jj=1; jj<21; jj++)	{
          	mask = 1<<jj;
          	if((fadcSlotMask&mask) && !(gbready&mask)) printf("%3d",jj);
		}
        printf("\n");
	}

     /* Reset the Token */
      if(FADC_ROFLAG==2) 
	  {
/*2us->*/
	    for(islot=0; islot<NFADC; islot++)
	    {
	      FA_SLOT = fadcID[islot];
	      faResetToken(FA_SLOT);
	    }
/*->2us*/
	  }


    blen = rol->dabufp - (int *)&jw[ind+1];

    


    if(blen == 0) /* no data - return pointer to the initial position */
    {
      rol->dabufp -= NHEAD;
	  /* NEED TO CLOSE BANK !!?? */
    }
    else if(blen >= (NBOARDS*MY_MAX_EVENT_LENGTH/4))
    {
      logMsg("1ERROR here vmenrol1: event too long, buffer length=%d, ind=%d, max_length=%d \n",blen,ind,NBOARDS*MY_MAX_EVENT_LENGTH/4,0,0,0);
      logMsg(": %d %d 0x%x 0x%x\n",blen,ind,dabufp1,dabufp2,0,0);
      tsleep(1); /* 1 = 0.01 sec */
    }
  /*  else if(bosMclose_(jw,ind,1,blen) <= 0)
    {
      logMsg("2ERROR in bosMclose_ - space is not enough !!!\n",1,2,3,4,5,6);
    }*/


 
    /* for physics sync event, make sure all board buffers are empty */
    /* NOTE: does not work without TS crate !! MUST do you own check*/
    if(syncFlag==1)
    {
      int scan_flag;
      unsigned short slot, nev16;
	  /*
logMsg("SYNC reached\n",1,2,3,4,5,6);
	  */
  
  
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
  int islot;

  CDODISABLE(VME,1,0);

  logMsg("INFO: User End Executed\n",1,2,3,4,5,6);

 /* FADC Disable */
  for(islot=0;islot<NFADC;islot++) 
  {
    FA_SLOT = fadcID[islot];
    faDisable(FA_SLOT,0);
    faStatus(FA_SLOT,0);
  }
  /* from parser */
 

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
