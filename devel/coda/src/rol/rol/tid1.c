/* tid1.c - first readout list for VME crates with new TID */

#define USE_FADC250
#undef DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/* from fputil.h */
#define SYNC_FLAG 0x20000000

/* readout list name */
#define ROL_NAME__ "TID1"

/* polling mode if needed */
#define POLLING_MODE

/* name used by loader */
#define INIT_NAME tid1__init

/* main TI board */
#define TID_ADDR   (21<<19)  /* if 0 - default will be used, assuming slot 21*/

/* Poll for available data, front panel triggers */
#define TID_READOUT TID_READOUT_EXT_POLL

/*???*/
#define TID_MASTER

#include "rol.h"

void usrtrig(unsigned int EVTYPE, unsigned int EVSOURCE);
void usrtrig_done();

#include "TIDPRIMARY_source.h"

/* user code */

#include "uthbook.h"
#include "coda.h"
#include "tt.h"
#include "scaler7201.h"
#include "adc792.h"
#include "tdc1190.h"

static char rcname[5];

#define NBOARDS 22    /* maximum number of VME boards: we have 21 boards, but numbering starts from 1 */
#define MY_MAX_EVENT_LENGTH 3000/*3200*/ /* max words per board */
static unsigned int tdcbuftmp[NBOARDS*MY_MAX_EVENT_LENGTH+16];
static unsigned int *tdcbuf;

#ifdef USE_FADC250

#include "fadcLib.h"

/* FADC250 Global Definitions */
#define FADC_WINDOW_LAT      200/*850*/  /* Trigger Window Latency */
#define FADC_WINDOW_WIDTH    100  /* Trigger Window Width */
#define FADC_DAC_LEVEL      3200  /* Internal DAC Level */
#define FADC_THRESHOLD        10  /* Threshold for data readout */

unsigned int fadcSlotMask   = 0;    /* bit=slot (starting from 0) */
extern   int fadcA32Base;           /* This will need to be reset from it's default
                                     * so that it does not overlap with the TID */
extern   int nfadc;                 /* Number of FADC250s verified with the library */
extern   int fadcID[FA_MAX_BOARDS]; /* Array of slot numbers, discovered by the library */
static int NFADC;                   /* The Maximum number of tries the library will
                                     * use before giving up finding FADC250s */
static int FA_SLOT;                 /* We'll use this over and over again to provide
				                     * us access to the current FADC slot number */ 

static int FADC_ROFLAG           = 2;  /* 0-noDMA, 1-DMA, 2-chainedDMA */

/* for the calculation of maximum data words in the block transfer */
static unsigned int MAXFADCWORDS = 0;
static unsigned int MAXTIDWORDS  = 0;

/* Global Blocklevel (Number of events per block) */
#define BLOCKLEVEL             1

#endif

static void
__download()
{
#ifdef POLLING_MODE
  rol->poll = 1;
#else
  rol->poll = 0;
#endif

  printf(">>>>>>>>>>>>>>>>>>>>>>>>>> ROCID = %d <<<<<<<<<<<<<<<<\n",rol->pid);

  printf("rol1: downloading DDL table ...\n");
  clonbanks_();
  printf("rol1: ... done.\n");

  /* TEMPORARY HERE from TIDPRIMARY_source.h */
  vmeOpenDefaultWindows();

  logMsg("INFO: User Download Executed\n",1,2,3,4,5,6);
}

static void
__prestart()
{
  int i1, i2, i3;
#ifdef USE_FADC250
  int islot, ichan;
  unsigned short iflag;
  int iFlag = 0;
#endif

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  /* initialize OS windows and TI board */
  CDOINIT(TIDPRIMARY,TIR_SOURCE);
  /* renamed to CDOINIT
  TIDPRIMARY_INIT;
  */



/*************/
/* TID Setup */
  /* Set the trigger source to the TS Inputs */
  /*tidSetTriggerSource(TID_TRIGGER_TSINPUTS);*/

  /* Set number of events per block */
  /*tidSetBlockLevel(1);*/

  /* Enable TS Input #3 and #4 */
  /*tidEnableTSInput( TID_TSINPUT_3 | TID_TSINPUT_4 );*/

  /* Load the trigger table that associates 
   *     TS#1 | TS#2 | TS#3 : trigger1
   *     TS#4 | TS#5 | TS#6 : trigger2 (playback trigger)
  */
  /*tidLoadTriggerTable();*/

  /* Set the Block Acknowledge threshold 
   *               0:  No threshold  -  Pipeline mode
   *               1:  One Acknowdge for each Block - "ROC LOCK" mode
   *           2-255:  "Buffered" mode.
   */
  /*tidSetBlockAckThreshold(0);*/
/* TID Setup */
/*************/



#ifdef POLLING_MODE
  CTRIGRSS(TIDPRIMARY, TIR_SOURCE, usrtrig, usrtrig_done);
#else
  CTRIGRSA(TIDPRIMARY, TIR_SOURCE, usrtrig, usrtrig_done);
#endif


  /* If the TI Master, send a Clock and Trig Link Reset */
#ifdef TID_MASTER
  tidClockReset();
  taskDelay(2);
  tidTrigLinkReset();
#endif


  sprintf(rcname,"RC%02d",rol->pid);
  printf("rcname >%4.4s<\n",rcname);


  /******************/
  /* USER code here */

  /* DMA setup */
  usrVmeDmaInit();
  usrVmeDmaSetConfig(2,5,1); /*A32,2eSST,267MB/s*/
  /*usrVmeDmaSetConfig(2,3,0);*/ /*A32,MBLT*/
  usrVmeDmaMemory(&i1, &i2, &i3);

  /* check 'tdcbuftmp' alignment and set 'tdcbuf' to 16-byte boundary */
#ifdef VXWORKS
  tdcbuf = &tdcbuftmp[0];
#else
  tdcbuf = (unsigned int *)i2;
#endif
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


#ifdef USE_FADC250

  /* Here, we assume that the addresses of each board were set according
   to their geographical address (slot number):
   * Slot  3:  (3<<19) = 0x180000
   * Slot  4:  (4<<19) = 0x200000
   * ...
   * Slot 20: (20<<19) = 0xA00000
   */

  NFADC = 16+2;   /* 16 slots + 2 (for the switch slots) */
  fadcA32Base=0x09000000;

  /* Setup the iFlag.. flags for FADC initialization */
  iFlag = 0;
  /* Sync Source */
  iFlag |= (1<<0);    /* VXS */
  /* Trigger Source */
  iFlag |= (1<<2);    /* VXS */
  /* Clock Source */
  iFlag |= (1<<5);    /* VXS */

  vmeSetQuietFlag(1); /* skip the errors associated with BUS Errors */
  faInit((unsigned int)(3<<19),(1<<19),NFADC,iFlag);
  NFADC=nfadc;        /* Redefine our NFADC with what was found from the driver */
  vmeSetQuietFlag(0); /* Turn the error statements back on */
  
  /* Calculate the maximum number of words per block transfer (assuming Pulse mode)
   *   MAX = NFADC * BLOCKLEVEL * (EvHeader + TrigTime*2 + Pulse*2*chan) 
   *         + 2*32 (words for byte alignment) 
   */
  MAXFADCWORDS = NFADC * BLOCKLEVEL * (1+2+32) + 2*32;
  /* Maximum TID words is easier to calculate, but we can be conservative, since
   * it's first in the readout
   */
  MAXTIDWORDS = 8+(3*BLOCKLEVEL);
  
  printf("**************************************************\n");
  printf("* Calculated MAX FADC words per block = %d\n",MAXFADCWORDS);
  printf("* Calculated MAX TID  words per block = %d\n",MAXTIDWORDS);
  printf("**************************************************\n");
  /* Check these numbers, compared to our buffer size.. */
  if( (MAXFADCWORDS+MAXTIDWORDS)*4 > MAX_EVENT_LENGTH )
  {
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf(" WARNING.  Event buffer size is smaller than the expected data size\n");
    printf("     Increase the size of MAX_EVENT_LENGTH and recompile!\n");
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  }


  if(FADC_ROFLAG==2 && NFADC>1) faEnableMultiBlock(1);
  else faDisableMultiBlock();

  /* Additional Configuration for each module */
  fadcSlotMask=0;
  for(islot=0;islot<NFADC;islot++) 
  {
    FA_SLOT = fadcID[islot];      /* Grab the current module's slot number */
    fadcSlotMask |= (1<<FA_SLOT); /* Add it to the mask */

    /* Set the internal DAC level */
    faSetDAC(FA_SLOT,FADC_DAC_LEVEL,0);
    /* Set the threshold for data readout */
    faSetThreshold(FA_SLOT,FADC_THRESHOLD,0);
	
    /*  Setup option 1 processing - RAW Window Data     <-- */
    /*        option 2            - RAW Pulse Data */
    /*        option 3            - Integral Pulse Data */
    /*  Setup 200 nsec latency (PL  = 50)  */
    /*  Setup  80 nsec Window  (PTW = 20) */
    /*  Setup Pulse widths of 36ns (NSB(3)+NSA(6) = 9)  */
    /*  Setup up to 1 pulse processed */
    /*  Setup for both ADC banks(0 - all channels 0-15) */
    /* Integral Pulse Data */
    faSetProcMode(FA_SLOT,1,FADC_WINDOW_LAT,FADC_WINDOW_WIDTH,3,6,1,0);
	
    /* Bus errors to terminate block transfers (preferred) */
    faEnableBusError(FA_SLOT);
    /* Set the Block level */
    faSetBlockLevel(FA_SLOT,BLOCKLEVEL);

    /* Set the individual channel pedestals for the data that is sent to the CTP */
	/*
    for(ichan=0; ichan<16; ichan++)
	{
	  faSetChannelPedestal(FA_SLOT,ichan,0);
	}
	*/
  }

  /***************************************
   *   SD SETUP
   ***************************************/
  sdInit();   /* Initialize the SD library */
  sdSetActiveVmeSlots(fadcSlotMask); /* Use the fadcSlotMask to configure the SD */
  sdStatus();




  /* FADC Perform some resets, status */
  for(islot=0;islot<NFADC;islot++) 
  {
    FA_SLOT = fadcID[islot];
    faResetToken(FA_SLOT);
    faResetTriggerCount(FA_SLOT);
    faStatus(FA_SLOT,0);
  }

  /*  Enable FADC */
  for(islot=0;islot<NFADC;islot++) 
  {
    FA_SLOT = fadcID[islot];
    /*faSetMGTTestMode(FA_SLOT,1); */
    faEnable(FA_SLOT,0,0);
  }




#endif

  /* USER code here */
  /******************/



  /* If the TI Master, send a Sync Reset 
     - required by FADC250 after it is enabled */
#ifdef TID_MASTER
  tidSyncReset();
#endif


  tidStatus();


  printf("INFO: User Prestart Executed\n");fflush(stdout);

  printf("INFO: Write Event Executed\n");fflush(stdout);

  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}       

static void
__end()
{
  int iwait=0;
  int blocksLeft=0;
  int islot;

  CDODISABLE(TIDPRIMARY,TIR_SOURCE,0);

  /* Before disconnecting... wait for blocks to be emptied */
  blocksLeft = tidBReady();
  if(blocksLeft)
  {
    printf("... end:  Blocks left on the TID (%d)\n",blocksLeft);
    while(iwait < 100)
	{
	  if(blocksLeft <= 0) break;
	  blocksLeft = tidBReady();
	  iwait++;
	}
    printf("... end:  Blocks left on the TID (%d)\n",blocksLeft);
  }


#ifdef USE_FADC250
  /* FADC Disable */
  for(islot=0;islot<NFADC;islot++) 
  {
    FA_SLOT = fadcID[islot];
    faDisable(FA_SLOT,0);
    faStatus(FA_SLOT,0);
  }
  sdStatus();
#endif

  tidStatus();

  logMsg("INFO: User End Executed\n",1,2,3,4,5,6);

  return;
}

static void
__pause()
{
  CDODISABLE(TIDPRIMARY,TIR_SOURCE,0);
  logMsg("INFO: Pause Executed\n",1,2,3,4,5,6);
  
} /*end pause */

static void
__go()
{
  logMsg("INFO: Entering Go 1\n",1,2,3,4,5,6);

#ifdef USE_FADC250
  /*  Enable FADC */
  faEnable(FA_SLOT,0,0);

  taskDelay(1);
  
  /*  Send Sync Reset to FADC */
  /*faSDC_Sync();*/
#endif

  CDOENABLE(TIDPRIMARY,TIR_SOURCE,0); /* bryan has (,1,1) ... */

  logMsg("INFO: Go 1 Executed\n",1,2,3,4,5,6);
}

void
usrtrig(unsigned int EVTYPE, unsigned int EVSOURCE)
{
  int *jw, ind, ind2, ii, jj, iii, blen, len;
#ifdef USE_FADC250
  unsigned int datascan, mask;
  int nwords, itdcbuf, njjloops, rlen;
  unsigned int *tdc;
  int islot;
  int dCnt, idata;
  int stat, itime, gbready;
#endif

  /*printf("EVTYPE=%d syncFlag=%d\n",EVTYPE,syncFlag);*/

  rol->dabufp = (int *) 0;

  /*open event type EVTYPE of BT_UI4*/
  CEOPEN(EVTYPE, BT_UI4);

  jw = rol->dabufp; 

  /***/

  /* at that moment only second CODA world defined  */
  /* first CODA world (length) undefined, so set it */
  /*jw[ILEN] = 1;*/ jw[-2] = 1;

  if((syncFlag<0)||(syncFlag>1))         /* illegal */
  {
    printf("Illegal1: syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE);
  }
  else if((syncFlag==0)&&(EVTYPE==0))    /* illegal */
  {
    printf("Illegal2: syncFlag=%d EVTYPE=%d\n",syncFlag,EVTYPE);
  }
  else if((syncFlag==1)&&(EVTYPE==0))    /* force_sync (scaler) events */
  {

/*
!!! we are geting here on End transition: syncFlag=1 EVTYPE=0 !!!
*/
  }
  else if((syncFlag==0)&&(EVTYPE==15)) /* helicity strob events */
  {
    ;
  }
  else           /* physics and physics_sync events */
  {

    if((ind = bosMopen_(jw, rcname, 0, 1, 0)) <=0)
    {
      printf("bosMopen_ Error: %d\n",ind);
    }
    rol->dabufp += NHEAD;



    /*************/
	/* TID stuff */

    /* Set high, the first output port */
    tidSetOutputPort(1,0,0,0);

    /* Grab the data from the TID */
    len = tidReadBlock(tdcbuf,900>>2,1);
    if(len<=0)
    {
      printf("TIDreadout : No data or error, len = %d\n",len);
    }
    else
    {
	  ;
      /*printf("len=%d\n",len);*/
      /*for(jj=0; jj<len; jj++) *rol->dabufp++ = LSWAP(tdcbuf[jj]);*/
    }

    /* Turn off all output ports */
    tidSetOutputPort(0,0,0,0);

	/* TID stuff */
    /*************/


#ifdef USE_FADC250

    /* Configure Block Type... temp fix for 2eSST trouble with token passing */
    dCnt=0;
    if(NFADC != 0)
    {
      for(itime=0; itime<1000; itime++) 
	  {
	    gbready = faGBready();
	    stat = (gbready == fadcSlotMask);
	    if (stat>0) 
	    {
	      break;
	    }
	  }
      if(stat>0) 
	  {
        FA_SLOT = fadcID[0];
        if(FADC_ROFLAG==2)
        {
	      dCnt = faReadBlock(FA_SLOT,tdcbuf,14000/*9100*//*MAXFADCWORDS*/,FADC_ROFLAG);
        }
        else
		{
          for(jj=0; jj<NFADC; jj++)
		  {
	        len = faReadBlock(fadcID[jj],&tdcbuf[dCnt],1000/*9100*//*MAXFADCWORDS*/,FADC_ROFLAG);
            dCnt += len;
#ifdef DEBUG
            printf("[%d] len=%d dCnt=%d\n",jj,len,dCnt);
#endif
		  }
	    }

	    if(dCnt<=0)
	    {
	      printf("FADCs: No data or error.  dCnt = %d (slots from %d)\n",dCnt,FA_SLOT);
          dCnt=0;
	    }
	    else
	    {
          /*printf("FADC: dCnt=%d\n",dCnt);*/
          /*for(jj=0; jj<dCnt; jj++) printf("  data[%3d] 0x%08x\n",jj,tdcbuf[jj]);*/
#ifndef VXWORKS
          /*for(jj=0; jj<dCnt; jj++) tdcbuf[jj] = LSWAP(tdcbuf[jj]);*/
          for(jj=0; jj<dCnt; jj++) *rol->dabufp++ = LSWAP(tdcbuf[jj]);
#endif
          /*for(jj=0; jj<dCnt; jj++) printf("  swap[%3d] 0x%08x\n",jj,tdcbuf[jj]);*/
	      /*tdcbuf += dCnt*/;
	    }
	  } 
      else 
	  {
	    printf ("FADCs: no events   stat=%d  intcount = %d   gbready = 0x%08x  fadcSlotMask = 0x%08x\n",
		  stat,tidGetIntCount(),gbready,fadcSlotMask);
        printf("Missing slots:");
        for(jj=1; jj<21; jj++)
		{
          mask = 1<<jj;
          if((fadcSlotMask&mask) && !(gbready&mask)) printf("%3d",jj);
		}
        printf("\n");
	  }

      /* Reset the Token */
      if(FADC_ROFLAG==2)
	  {
	    for(islot=0; islot<NFADC; islot++)
	    {
	      FA_SLOT = fadcID[islot];
	      faResetToken(FA_SLOT);
	    }
	  }
    }



	
goto skipfadctranslating;
	

    itdcbuf = 0;
    /*njjloops = nboards;*/njjloops = 1;
    for(jj=0; jj<njjloops; jj++)
    {
      /*rlen = rlenbuf[jj];*/rlen = dCnt;

      if(rlen <= 0) continue;

      tdc = &tdcbuf[itdcbuf];
      itdcbuf += rlen;
      ii=0;
      while(ii<rlen)
      {
        int a_channel, a_nevents, a_blocknumber, a_triggernumber, a_windowwidth;
		int a_adc1, a_adc2, a_valid1, a_valid2, a_nwords, a_slot1, a_slot2;
        int a_trigtime[4], a_id;

        if( ((tdc[ii]>>27)&0x1F) == 0x10)
        {
          a_slot1 = ((tdc[ii]>>22)&0x1F);
          a_nevents = ((tdc[ii]>>11)&0x3FF);
          a_blocknumber = (tdc[ii]&0x3FF);
#ifdef DEBUG
		  logMsg("[%3d] BLOCK HEADER: slot %d, nevents %d, block number %d\n",ii,
				   a_slot1,a_nevents,a_blocknumber,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x11)
        {
          a_slot2 = ((tdc[ii]>>22)&0x1F);
          a_nwords = (tdc[ii]&0x3FFFFF);
#ifdef DEBUG
		  logMsg("[%3d] BLOCK TRAILER: slot %d, nwords %d\n",ii,
				   a_slot2,a_nwords,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x12)
        {
          a_triggernumber = (tdc[ii]&0x7FFFFFF);
#ifdef DEBUG
		  logMsg("[%3d] EVENT HEADER: trigger number %d\n",ii,
				   a_triggernumber,3,4,5,6);
#endif
		  ii++;
          while( ((tdc[ii]>>31)&0x1) == 0 )
		  {
            a_triggernumber = (tdc[ii]&0x7FFFFFF);
#ifdef DEBUG
            logMsg("   [%3d] EVENT HEADER 2: trigger number %d -> SERIOUS ERROR !!!\n",ii,
              a_triggernumber,3,4,5,6);
#endif
            ii++;
		  }
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x13)
        {
          a_trigtime[0] = (tdc[ii]&0xFFFFFF);
#ifdef DEBUG
		  logMsg("[%3d] TRIGGER TIME: 0x%06x\n",ii,
				   a_trigtime[0],3,4,5,6);
#endif
		  ii++;
          iii=1;
          while( ((tdc[ii]>>31)&0x1) == 0 && ii<rlen )
		  {
            a_trigtime[iii] = (tdc[ii]&0xFFFFFF);
#ifdef DEBUG
            logMsg("   [%3d] TRIGGER TIME: 0x%06x\n",ii,
              a_trigtime[iii],3,4,5,6);
#endif
            iii++;
            ii++;
		  }
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x14)
        {
          a_channel = ((tdc[ii]>>23)&0xF);
          a_windowwidth = (tdc[ii]&0xFFF);
          a_id = (a_slot1<<24)+(a_channel<<16)+a_windowwidth;
#ifdef DEBUG
		  logMsg("[%3d] WINDOW RAW DATA: channel %d, window width %d (a_id=0x%08x)\n",ii,
				   a_channel,a_windowwidth,a_id,5,6);
#endif
          *rol->dabufp ++ = a_id;
		  ii++;
          while( ((tdc[ii]>>31)&0x1) == 0 && ii<rlen )
		  {
            a_valid1 = ((tdc[ii]>>29)&0x1);
            a_adc1 = ((tdc[ii]>>16)&0xFFF);
            a_valid2 = ((tdc[ii]>>13)&0x1);
            a_adc2 = (tdc[ii]&0xFFF);
#ifdef DEBUG
            logMsg("   [%3d] WINDOW RAW DATA: valid1 %d, adc1 0x%04x, valid2 %d, adc2 0x%04x\n",ii,
              a_valid1,a_adc1,a_valid2,a_adc2,6);
#endif
            *rol->dabufp ++ = tdc[ii];
            ii++;
		  }
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x15)
        {
#ifdef DEBUG
		  logMsg("[%3d] WINDOW SUM: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x16)
        {
#ifdef DEBUG
		  logMsg("[%3d] PULSE RAW DATA: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x17)
        {
#ifdef DEBUG
		  logMsg("[%3d] PULSE INTEGRAL: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x18)
        {
#ifdef DEBUG
		  logMsg("[%3d] PULSE TIME: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x19)
        {
#ifdef DEBUG
		  logMsg("[%3d] STREAMING RAW DATA: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x1D)
        {
#ifdef DEBUG
		  logMsg("[%3d] EVENT TRAILER: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x1E)
        {
		  logMsg("[%3d] : DATA NOT VALID\n",ii,
				   2,3,4,5,6);
		  ii++;
        }
        else if( ((tdc[ii]>>27)&0x1F) == 0x1F)
        {
#ifdef DEBUG
		  logMsg("[%3d] FILLER WORD: \n",ii,
				   2,3,4,5,6);
#endif
		  ii++;
        }
        else
        {
          logMsg("[%3d] ERROR: in FADC data format 0x%08x\n",ii,
            (int)tdc[ii],3,4,5,6);
          ii++;
        }

      } /* loop over 'rlen' words */

    }

#endif


  skipfadctranslating:


	/* close bank */
    blen = rol->dabufp - (int *)&jw[ind+1];
    if(blen == 0) /* no data - return pointer to the initial position */
    {
      rol->dabufp -= NHEAD;
	  /* NEED TO CLOSE BANK !!?? */
    }
    else if(blen >= (MAX_EVENT_LENGTH/4))
    {
      printf("ERROR: event too long, blen=%d, ind=%d\n",blen,ind);
      printf(": %d %d 0x%x 0x%x\n",blen,ind,rol->dabufp);
      sleep(1);
    }
    else if(bosMclose_(jw,ind,1,blen) <= 0)
    {
      printf("ERROR in bosMclose_ - space is not enough !!!\n");
    }

    /* for physics sync event, make sure all board buffers are empty */
    if(syncFlag==1)
    {
      int scan_flag = 0;
      unsigned short slot, nev16;

	  /*
	  if(scan_flag) logMsg("SYNC: scan_flag=0x%08x\n",scan_flag,2,3,4,5,6);
	  */
      ind2 = bosMopen_(jw,"PTRN",rol->pid,1,1);
      jw[ind2+1] = SYNC_FLAG + scan_flag;
      rol->dabufp += bosMclose_(jw,ind2,1,1);
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

void
__done()
{
  /* from parser */
  poolEmpty = 0; /* global Done, Buffers have been freed */

  /* Acknowledge tir register */
  CDOACK(TIDPRIMARY,TIR_SOURCE,0);

  return;
}

static void
__status()
{
  return;
}  
