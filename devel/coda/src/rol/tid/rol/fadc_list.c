/*************************************************************************
 *
 *  fadc_list.c - Library of routines for readout and buffering of 
 *                events using a JLAB Trigger Interface and
 *                Distribution Module (TID) AND one or more FADC250 
 *                with a Linux VME controller.
 *
 */

/* Event Buffer definitions */
#define MAX_EVENT_POOL     400
#define MAX_EVENT_LENGTH   1024*10      /* Size in Bytes */

/* Define Interrupt source and address */
#define TID_MASTER   /* Master accepts triggers and distributes them, if needed */
#define TID_READOUT TID_READOUT_EXT_POLL  /* Poll for available data, external triggers */
#define TID_ADDR    (21<<19)              /* GEO slot 21 */

#include "tidprimary_list.c" /* source required for CODA */
#include "fadcLib.h"         /* Header for FADC250 library */

/* FADC250 Global Definitions */
#define FADC_WINDOW_LAT      725  /* Trigger Window Latency */
#define FADC_WINDOW_WIDTH     40  /* Trigger Window Width */
#define FADC_DAC_LEVEL      3300  /* Internal DAC Level */
#define FADC_THRESHOLD       100  /* Threshold for data readout */

unsigned int fadcSlotMask   = 0;    /* bit=slot (starting from 0) */
extern   int fadcA32Base;           /* This will need to be reset from it's default
                                     * so that it does not overlap with the TID */
extern   int nfadc;                 /* Number of FADC250s verified with the library */
extern   int fadcID[FA_MAX_BOARDS]; /* Array of slot numbers, discovered by the library */
int NFADC;                          /* The Maximum number of tries the library will
                                     * use before giving up finding FADC250s */
int FA_SLOT;                        /* We'll use this over and over again to provide
				     * us access to the current FADC slot number */ 

/* for the calculation of maximum data words in the block transfer */
unsigned int MAXFADCWORDS = 0;
unsigned int MAXTIDWORDS  = 0;

/* Global Blocklevel (Number of events per block) */
#define BLOCKLEVEL             1

/* function prototype */
void rocTrigger(int arg);

/****************************************
 *  DOWNLOAD
 ****************************************/
void
rocDownload()
{
  int islot;

  /* Setup Address and data modes for DMA transfers
   *   
   *  vmeDmaConfig(addrType, dataType, sstMode);
   *
   *  addrType = 0 (A16)    1 (A24)    2 (A32)
   *  dataType = 0 (D16)    1 (D32)    2 (BLK32) 3 (MBLK) 4 (2eVME) 5 (2eSST)
   *  sstMode  = 0 (SST160) 1 (SST267) 2 (SST320)
   */
  vmeDmaConfig(2,5,1); 

  /***************************************
   * TID Setup 
   ***************************************/
  /* Set the trigger source to the TS Inputs */
  tidSetTriggerSource(TID_TRIGGER_TSINPUTS);

  /* Set number of events per block */
  tidSetBlockLevel(BLOCKLEVEL);

  /* Enable TS Input #3 and #4 */
  tidEnableTSInput( TID_TSINPUT_3 | TID_TSINPUT_4 );

  /* Load the trigger table that associates 
   *     TS#1 | TS#2 | TS#3 : trigger1
   *     TS#4 | TS#5 | TS#6 : trigger2 (playback trigger)
  */
  tidLoadTriggerTable();

  /* Set the Block Acknowledge threshold 
   *               0:  No threshold  -  Pipeline mode
   *               1:  One Acknowdge for each Block - "ROC LOCK" mode
   *           2-255:  "Buffered" mode.
   */
  tidSetBlockAckThreshold(0);

  /***************************************
   * FADC Setup 
   ***************************************/
  /* Here, we assume that the addresses of each board were set according to their
   * geographical address (slot number):
   * Slot  3:  (3<<19) = 0x180000
   * Slot  4:  (4<<19) = 0x200000
   * ...
   * Slot 20: (20<<19) = 0xA00000
   */

  NFADC = 16+2;   /* 16 slots + 2 (for the switch slots) */
  fadcA32Base=0x09000000;

  /* Setup the iFlag.. flags for FADC initialization */
  int iFlag;
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

  
  if(NFADC>1)
    faEnableMultiBlock(1);

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
      faSetProcMode(FA_SLOT,3,FADC_WINDOW_LAT,FADC_WINDOW_WIDTH,3,6,1,0);
	
      /* Bus errors to terminate block transfers (preferred) */
      faEnableBusError(FA_SLOT);
      /* Set the Block level */
      faSetBlockLevel(FA_SLOT,BLOCKLEVEL);

      /* Set the individual channel pedestals for the data that is sent
       * to the CTP
       */
      int ichan;
      for(ichan=0; ichan<16; ichan++)
	{
	  faSetChannelPedestal(FA_SLOT,ichan,0);
	}

    }

  /***************************************
   *   SD SETUP
   ***************************************/
  sdInit();   /* Initialize the SD library */
  sdSetActiveVmeSlots(fadcSlotMask); /* Use the fadcSlotMask to configure the SD */
  sdStatus();

  printf("rocDownload: User Download Executed\n");

}

/****************************************
 *  PRESTART
 ****************************************/
void
rocPrestart()
{
  unsigned short iflag;
  int stat,islot;

  /* FADC Perform some resets, status */
  for(islot=0;islot<NFADC;islot++) 
    {
      FA_SLOT = fadcID[islot];
      faResetToken(FA_SLOT);
      faResetTriggerCount(FA_SLOT);
      faStatus(FA_SLOT,0);
    }

  /* TID Status */
  tidStatus();

  /*  Enable FADC */
  for(islot=0;islot<NFADC;islot++) 
    {
      FA_SLOT = fadcID[islot];
/*       faSetMGTTestMode(FA_SLOT,1); */
      faEnable(FA_SLOT,0,0);
    }

  printf("rocPrestart: User Prestart Executed\n");
  /* SYNC is issued after this routine is executed */
}

/****************************************
 *  GO
 ****************************************/
void
rocGo()
{
  /* Enable modules, if needed, here */

  /* Interrupts/Polling enabled after conclusion of rocGo() */
}

/****************************************
 *  END
 ****************************************/
void
rocEnd()
{
  int islot;

  /* FADC Disable */
  for(islot=0;islot<NFADC;islot++) 
    {
      FA_SLOT = fadcID[islot];
      faDisable(FA_SLOT,0);
      faStatus(FA_SLOT,0);
    }

  tidStatus();
  sdStatus();

  printf("rocEnd: Ended after %d blocks\n",tidGetIntCount());
  
}

/****************************************
 *  READOUT TRIGGER
 ****************************************/
void
rocTrigger(int arg)
{
  int islot;
  int dCnt, len=0, idata;
  int stat, itime, gbready;
  int roflag=1;

  /* Set high, the first output port */
  tidSetOutputPort(1,0,0,0);

  /* Readout the TID */
  dCnt = tidReadBlock(dma_dabufp,MAXTIDWORDS,1);
  if(dCnt<=0)
    {
      printf("No data or error.  dCnt = %d\n",dCnt);
    }
  else
    {
      dma_dabufp += dCnt;
    }

    /* Readout FADC */
  /* Configure Block Type... temp fix for 2eSST trouble with token passing */
  if(NFADC!=0)
    {
      FA_SLOT = fadcID[0];
      for(itime=0;itime<100;itime++) 
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
	  if(NFADC>1) roflag=2; /* Use token passing scheme to readout all modules */
	  dCnt = faReadBlock(FA_SLOT,dma_dabufp,MAXFADCWORDS,roflag);
	  if(dCnt<=0)
	    {
	      printf("FADC%d: No data or error.  dCnt = %d\n",FA_SLOT,dCnt);
	    }
	  else
	    {
	      dma_dabufp += dCnt;
	    }
	} 
      else 
	{
	  printf ("FADC%d: no events   stat=%d  intcount = %d   gbready = 0x%08x  fadcSlotMask = 0x%08x\n",
		  FA_SLOT,stat,tidGetIntCount(),gbready,fadcSlotMask);
	}

      /* Reset the Token */
      if(roflag==2)
	{
	  for(islot=0; islot<NFADC; islot++)
	    {
	      FA_SLOT = fadcID[islot];
	      faResetToken(FA_SLOT);
	    }
	}
    }

  /* Add some addition data here, as necessary 
   * Use LSWAP here to be sure they are added with the correct byte-ordering 
   */
  *dma_dabufp++ = LSWAP(0x1234);

  /* Turn off all output ports */
  tidSetOutputPort(0,0,0,0);

}
