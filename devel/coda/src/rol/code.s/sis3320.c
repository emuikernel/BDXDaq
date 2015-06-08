/******************************************************************************
 *
 *   sis3320.c  -  Driver library for readout of Struck 3320 Flash ADC
 *                 using a VxWorks 5.4 or later based Single Board computer.
 *
 *  Started      :  by Sergey Boyarinov December 2005
 *  Continued    :  by Serguei Pozdniakov June 2006
 *  Accomplished :  by
 *
 */

#ifdef VXWORKS

#include <stdio.h>
#include <string.h>
#include <vxWorks.h>

#include "circbuf.h"

/* Include ADC definitions */
#include "sis3320.h"

/* Define external Functions */
IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);
IMPORT  STATUS intDisconnect(int);
IMPORT  STATUS sysIntEnable(int);
IMPORT  STATUS sysIntDisable(int);


/* Define global variables */
static int ssNSamples = 200;   /* the number of samples in window */
static int ssWrapSize = 256;   /* the number of samples in WRAP */
static int ssMaxNEvt  = 1;     /* Max. N of Event for Multi Event Mode */
static int ssEvtCNT   = 0;     /* Read Counter for Multi Event Mode */

static int OperationMode = 1;  /* 0 = Common Start mode;  1 = Common Stop mode */

volatile SIS3320 *sis3320;       /* pointer to ADC memory map */

static int flyIndex[4] = {0,1,2,-1};

#define FlyIndexCorrector(Index) \
          ((Index) & SIS3320_ADC_ADDR_OFFSET_MASK) + \
  flyIndex[(Index) & SIS3320_ADC_SAMPLE_OFFSET_MASK]

#define SIS3320_SET_STRUCT(address) \
  sis3320->ctrl = (SIS3320CTRL *) (address); \
  sis3320->cnfgtot = (SIS3320CNFGTOT *)(address+SIS3320_EVENT_CONFIG_ALL_ADC); \
  sis3320->cnfg[0] = (SIS3320CNFG *)(address+SIS3320_ADC12); \
  sis3320->cnfg[1] = (SIS3320CNFG *)(address+SIS3320_ADC34); \
  sis3320->cnfg[2] = (SIS3320CNFG *)(address+SIS3320_ADC56); \
  sis3320->cnfg[3] = (SIS3320CNFG *)(address+SIS3320_ADC78); \
  sis3320->data[0] = (unsigned int *)(address+SIS3320_ADC1_OFFSET); \
  sis3320->data[1] = (unsigned int *)(address+SIS3320_ADC2_OFFSET); \
  sis3320->data[2] = (unsigned int *)(address+SIS3320_ADC3_OFFSET); \
  sis3320->data[3] = (unsigned int *)(address+SIS3320_ADC4_OFFSET); \
  sis3320->data[4] = (unsigned int *)(address+SIS3320_ADC5_OFFSET); \
  sis3320->data[5] = (unsigned int *)(address+SIS3320_ADC6_OFFSET); \
  sis3320->data[6] = (unsigned int *)(address+SIS3320_ADC7_OFFSET); \
  sis3320->data[7] = (unsigned int *)(address+SIS3320_ADC8_OFFSET)

#define VME_A32_MSTR_BUS 0x08000000 /* ERRORRRRRRRRRRRRRRRRRRRRRRRRRRRR */
#define SISADR           0x20000000


/*============================================================================
 *  local function Prototypes
 *===========================================================================*/


/* Key Address functions */
void sis3320ResetDDR2 ();

/* Acquisition/Mode Functions */
unsigned int sis3320AcqConfig              (unsigned int val);
unsigned int sis3320EnableAutostart        ();
unsigned int sis3320DisableAutostart       ();
int          sis3320AutostartStatus        ();
unsigned int sis3320EnableMultiEvent       ();
unsigned int sis3320DisableMultiEvent      ();
int          sis3320MultiEventStatus       ();
unsigned int sis3320EnableInternalTrigger  ();
unsigned int sis3320DisableInternalTrigger ();
int          sis3320InternalTriggerStatus  ();
unsigned int sis3320EnableLemoStartStop    ();
unsigned int sis3320DisableLemoStartStop   ();
int          sis3320LemoStartStopStatus    ();
int          sis3320SamplingArmedStatus    ();
int          sis3320SamplingBusyStatus     ();

int          sis3320DACBusyStatus  ();
void         sis3320ClearDACs      ();
int          sis3320SetDAC1        (UINT16  offset);
int          sis3320SetDACs        (UINT16 *offsets);
int          sis3320SetUniformDACs (UINT16  offset);
int          sis3320GetDACs        (int    *offsets);
void         sis3320DisplayDACs    ();

void         sis3320SetStartDelay  (unsigned int val);
unsigned int sis3320GetStartDelay  ();
void         sis3320SetStopDelay   (unsigned int val);
unsigned int sis3320GetStopDelay   ();
void         sis3320SetMaxNofEvent (unsigned int val);
unsigned int sis3320GetMaxNofEvent ();
void         sis3320ResetEvtCNT    ();
void         sis3320SetGain        (unsigned int gain);
unsigned int sis3320GetGain        ();

void         sis3320EventConfig        (unsigned int val);
unsigned int sis3320SetWrapPageSize    (unsigned int val);
unsigned int sis3320GetWrapPageSize    ();
void         sis3320SetSampleStartAddr (unsigned int val);
void         sis3320SetSampleLength    (unsigned int val);
unsigned int sis3320GetSampleLength    ();


/* Some useful check functions */
void sis3320ReadTest1  (int size, int prn);
void sis3320ReadTest2  (int size, int prn);
void sis3320RunTest1   (int ncycles, unsigned int pageWrapSize);
void sis3320ReadActualSampleValue   ();
int  Get_ADC_values_of_DAC_RampDown (int Dmax, int Dmin, int prn, int adc);
void sis3320PrintDataADC12          (int nsamples);
void sis3320PrintDataADC12_2        (int nsamples);
void sis3320PrintForMultiEvent      (int nsamples);
void sis3320Print2ForMultiEvent     (int nsamples);


/*****************************************************************************
*
*   sis3320Init - Initialize sis3320 Library
*
*   RETURNS: OK, or ERROR if the address is invalid or board is not present.
*
*/  

STATUS 
sis3320Init(UINT32 addr, UINT32 addr_inc, int nadc, UINT16 crateID)
{
  int ii, jj, res, rdata, errFlag = 0;
  int boardID = 0;
  unsigned int data;
  /* unsigned int offset; */

  if(addr==0)  addr = SISADR; /* lazy trick, to avoid input anything */

  sis3320 = (SIS3320 *) malloc(sizeof(SIS3320));
  SIS3320_SET_STRUCT(addr);
  /*  sysBusToLocalAdrs(0x09,(char *)VME_A32_MSTR_BUS,(char **)&offset);
      printf("A32 offset=0x%08x offset(0x09)=0x%08x\n",offset,offset);
      printf("UUU\n"); fflush(stdout); printf("0: 0x%08x\n",sis3320->ctrl); */


  /* Check if Board exists at that address */
  res = vxMemProbe((char *) sis3320->ctrl,0,4,(char *)&rdata);
  if(res < 0)
  {
    printf("sis3320Init: ERROR: No addressable board at addr=0x%x\n",
	   (UINT32) sis3320->ctrl);
    errFlag = 1;
  }
  else
  {
    /* Check if this is a Model 3320 */
    boardID = sis3320->ctrl->firmware; 
    if(boardID != SIS3320_BOARD_ID)
    {
      printf("WARN: Firmware does not match: 0x%08x (expected 0x%08x)\n",
	     boardID,SIS3320_BOARD_ID);
      return(ERROR);
    }
  }
  printf("Initialized sis3320 at address 0x%08x \n",(UINT32)sis3320->ctrl);


  /*********/
  /* Reset */
  sis3320Reset();


  /******************************************************/
  /* set Clock, LEMO Start/Stop, Multi Event, Autostart,
         Wrap Page Mode & Size, Sample Length Stop,
         Start/Stop Delays, Event Length,
         Sample Start Address, max N of Events,
         Input Range (ADC Gain), DACs (signals offset) */
  sis3320DefaultSetup();


  /*******************/
  /* Input Test Mode */
  sis3320->cnfgtot->ADCInputModeAllADC = 0;


  /*****************/
  /* Trigger Setup */
  data = TRIGGER_PEAK & 0x1f ;
  data = data + ( (TRIGGER_GAP & 0x1f) << 8 ) ;
  data = data + ( (TRIGGER_PULSE_LEN & 0xff) << 16 ) ;
  sis3320->cnfg[0]->trigReg[0][0] = data; /*channel 1*/
  sis3320->cnfg[0]->trigReg[1][0] = data; /*channel 2*/
  sis3320->cnfg[1]->trigReg[0][0] = data; /*channel 3*/
  sis3320->cnfg[1]->trigReg[1][0] = data; /*channel 4*/
  sis3320->cnfg[2]->trigReg[0][0] = data; /*channel 5*/
  sis3320->cnfg[2]->trigReg[1][0] = data; /*channel 6*/
  sis3320->cnfg[3]->trigReg[0][0] = data; /*channel 7*/
  sis3320->cnfg[3]->trigReg[1][0] = data; /*channel 8*/


  /*********************/
  /* Trigger Threshold */
  data = TRIGGER_THRESHOLD & 0x1ffff ;
  data = data + ( (RING_TRIGGER & 0x3) << 24) ;
  sis3320->cnfg[0]->trigReg[0][1] = data; /*channel 1*/
  sis3320->cnfg[0]->trigReg[1][1] = data; /*channel 2*/
  sis3320->cnfg[1]->trigReg[0][1] = data; /*channel 3*/
  sis3320->cnfg[1]->trigReg[1][1] = data; /*channel 4*/
  sis3320->cnfg[2]->trigReg[0][1] = data; /*channel 5*/
  sis3320->cnfg[2]->trigReg[1][1] = data; /*channel 6*/
  sis3320->cnfg[3]->trigReg[0][1] = data; /*channel 7*/
  sis3320->cnfg[3]->trigReg[1][1] = data; /*channel 8*/


  if(errFlag > 0)
    {
      printf("sis3320Init: ERROR: Unable to initialize sis3320 Module\n");
      return(ERROR);
    }
  else
    {
      return(OK);
    }
}

int
sis3320Status (UINT32 addr)
{
  int ii;
  unsigned int *boardID;
  unsigned int sreg, iconfReg, clk, acqreg;
  int iLevel, iVec, iEnable, iMode;

  if(addr==0)  addr = SISADR; /* lazy trick, to avoid input anything */

  /* Check if this is a Model 3320 */
  boardID = (unsigned int *)(addr+0x4);
  if((*boardID) != SIS3320_BOARD_ID)
    {
      printf("sis3320Status: ERROR: Firmware does not match: 0x%08x (expected 0x%08x)\n\n",
	     *boardID, SIS3320_BOARD_ID);
      return(-1);
    }
  printf ("\nThere is sis3320 at address 0x%08x \n\n", addr);
  
  /* Check if sis3320 was initialized */
  if(sis3320->ctrl->firmware != SIS3320_BOARD_ID)
    {
      printf ("sis3320Status: WARN: sis3320 is not initialized at address 0x%08x \n\n", addr);
      return(-2);
    }

  sreg   = sis3320->ctrl->control;
  acqreg = sis3320->ctrl->acquistionControl;

  iconfReg = sis3320->ctrl->intConfig;
  iLevel   = (iconfReg & SIS3320_INT_LEVEL_MASK) >> 8;
  iVec     = (iconfReg & SIS3320_INT_VEC_MASK);
  iEnable  = (iconfReg & SIS3320_INT_ENABLE_MASK);
  iMode    = (iconfReg & SIS3320_INT_MODE_MASK) >> 12;

  clk = (acqreg & SIS3320_CLOCKSOURCE_MASK) >> 12;

  printf ("\nSTATUS for FADC sis3320 at base address 0x%x \n",
	  (UINT32) sis3320->ctrl);
  printf ("-------------------------------------------------- \n");

  if (iEnable)
    {
      printf (" Interrupts Enabled  - Mode = %d (0:RORA 1:ROAK) \n", iMode);
      printf (" VME Interrupt Level: %d   Vector: 0x%x \n", iLevel, iVec);
    }
  else
    {
      printf (" Interrupts Disabled - Mode = %d (0:RORA 1:ROAK) \n", iMode);
      printf (" VME Interrupt Level: %d   Vector: 0x%x \n", iLevel, iVec);
    }

  printf ("\n");
  printf ("  MODULE ID   register = 0x%08x \n", sis3320->ctrl->firmware);
  printf ("\n");
  printf ("  Clock Source         = %s \n",     sis3320_clksrc_string[clk]);
  printf ("  STATUS      register = 0x%08x \n", sreg);
  printf ("  INT CONTROL register = 0x%08x \n", sis3320->ctrl->intControl);
  printf ("  ACQ CONTROL register = 0x%08x \n", acqreg);
  printf ("\n");
  printf ("  External Start Delay = %d clocks\n", sis3320->ctrl->startDelay);
  printf ("  External Stop  Delay = %d clocks\n", sis3320->ctrl->stopDelay);

  printf ("\n  Event Configuration Information \n");
  printf ("  ------------------------------- \n");
  for (ii = 0; ii < 4; ii++) 
    {
      printf ("  Group %d  ADC %d, %d\n", ii, ii*2+1, ii*2+2);
      printf ("     Event Config   register       = 0x%08x\n",
	      sis3320->cnfg[ii]->eventConfig);
      printf ("     Sample Length register        = 0x%08x\n",
	      sis3320->cnfg[ii]->sampleLength);
      printf ("     Sample Start address register = 0x%08x\n",
	      sis3320->cnfg[ii]->sampleStart);
    }
  printf ("\n");

  return(0);
}

void
sis3320DefaultSetup()
{
  /****************************/
  /* Acquisition/Event Config */
  if (OperationMode == 1)        /* Common Stop mode */
    {
      sis3320SetCommonStop();
    }
  else
    {
      sis3320SetCommonStart();
    }

  /*************************/
  /* Set Start/Stop Delays */
  sis3320SetStartDelay(0);
  sis3320SetStopDelay(0);

  /************************************/
  /* Set Event Length, Wrap Page Size */
  /* and correct Start/Stop Delay     */
  sis3320SetGlobalWindow(1000,-4000);

  /************************/
  /* sample start Address */
  sis3320SetSampleStartAddr(0);

  /***************************/
  /* Program 1 event maximum */
  sis3320SetMaxNofEvent(1);

  /*****************/
  /* Set Full Gain */
  sis3320SetGain(0);

  /*********************************/
  /* Give an offset to the signals */
  sis3320SetUniformDACs(45000);

}

void
sis3320SetCommonStart()
{
  OperationMode = 0;  /* 0 = Common Start mode;  1 = Common Stop mode */

  sis3320AcqConfig   (
		      SIS3320_ACQ_SET_CLOCK_TO_200MHZ |
		      SIS3320_ACQ_ENABLE_LEMO_START_STOP |
		      SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER |
		      SIS3320_ACQ_DISABLE_MULTIEVENT |
		      SIS3320_ACQ_DISABLE_AUTOSTART );
  sis3320EventConfig (
		      SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE |
		      SIS3320_EVENT_CONF_PAGE_SIZE_256_WRAP |
		      SIS3320_EVENT_CONF_ENABLE_SAMPLE_LENGTH_STOP );
}

void
sis3320SetCommonStop()
{
  OperationMode = 1;  /* 0 = Common Start mode;  1 = Common Stop mode */

  sis3320AcqConfig   (
		      SIS3320_ACQ_SET_CLOCK_TO_200MHZ |
		      SIS3320_ACQ_ENABLE_LEMO_START_STOP |
		      SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER |
		      SIS3320_ACQ_DISABLE_MULTIEVENT |
		      SIS3320_ACQ_ENABLE_AUTOSTART );
  sis3320EventConfig (
		      SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE |
		      SIS3320_EVENT_CONF_PAGE_SIZE_256_WRAP );
}

int
sis3320ReadEvent(UINT32 addr, INT32 *lenbuf, INT32 *adcbuf)
{
  int nsample, nwrap, lempty;
  int ii, adc, grp, tmp;
  int geo, header;
  int tdata, tlen;
  unsigned int *boardID;
  unsigned short *pshdata, *ptr;
  int startIndex, stopIndex;
  unsigned int data[8][MAX_EVENT_LENGTH/4]; /* should never be more then maximum event buffer size */

  /* Check if this is a Model 3320 */
  boardID = (unsigned int *)(addr+0x4);
  if((*boardID) != SIS3320_BOARD_ID)
  {
    printf("sis3320ReadEvent: ERROR: Firmware does not match: 0x%08x (expected 0x%08x)\n",
       *boardID, SIS3320_BOARD_ID);
    return(-1);
  }

  /* NOTE: 2 samples = 1 32-bit word !   */
  nwrap   = sis3320GetWrapPageSize();   /* the number of samples in WRAP */
  nsample = sis3320GetSampleLength();   /* the number of samples in window */
  lempty  = nwrap - nsample;            /* the length of area we do not want to read */

  geo     = addr >> 27;                 /* make geo.addr. from ADC base addr. */
  header  = (geo<<27) | (nsample+1);    /* make HEADER WORD as for 1881M */

  /* read data from ADC into memory buffer (will be replaced by DMA) */
  for(adc=0; adc<8; adc++)
    for(ii=0; ii<(nwrap/2); ii++)
      data[adc][ii] = sis3320->data[adc][ii];

  tlen = 0;
  for(adc=0; adc<8; adc++)              /***** Process Data *****/
  {
    adcbuf[tlen++] = header;            /* place header */

    if(OperationMode == 0)              /*** Common Start mode ***/
    {
      for(ii=0; ii<(nsample/2); ii++)
      {
        tdata = data[adc][ii];

        /* make First and Second DATA WORD as for 1881M */
        adcbuf[tlen++] = (geo<<27) | (adc<<17) |  (tdata & SIS3320_ADC_LODATA_MASK);
        adcbuf[tlen++] = (geo<<27) | (adc<<17) | ((tdata & SIS3320_ADC_HIDATA_MASK)>>16);
      }

    }
    else if(OperationMode == 1)         /*** Common Stop mode ***/
    {
      /* swap data: we'll use 'short' pointer, and we have big endian machine */
      for(ii=0; ii<(nwrap/2); ii++)
      {
        unsigned short tt, *pp;
        pp    = (unsigned short *)&data[adc][ii];
        tt    = pp[0];
        pp[0] = pp[1];
        pp[1] = tt;
      }

      pshdata = (unsigned short *)&data[adc][0];

      /* adc - channel number 0..7 */
      grp = adc/2;   /* group 0..3 */

      /* pointer to the last sample (2 fields inside !) */
      tmp = sis3320->cnfg[grp]->nextSampAdr[adc%2];

      /* Upper part of tmp - Stop Index, pointer to quartet;
            example: if nwrap=256 then Stop Index can be 0,4,8,..,248,252;
	 Low part of tmp (bits 0 and 1) - Index Corrector  {0,1,2,3} -> {0,1,2,-1} */
      stopIndex = FlyIndexCorrector(tmp);

      if(stopIndex < 0) /* if Stop Index was 0 and Corrector was -1 */
	{
	  stopIndex += nwrap;
	}

      startIndex = stopIndex + lempty + 1;  /* Start Index */


      if(startIndex >= nwrap)     /* one piece */
      {
        startIndex -= nwrap;

        ptr = &pshdata[startIndex];
        for(ii=0; ii<nsample; ii++)
          adcbuf[tlen++] = (geo<<27) | (adc<<17) | ((*ptr++)&SIS3320_ADC_LODATA_MASK);

      }
      else                        /* two pieces */
      {
        ptr = &pshdata[startIndex];
        for(ii=0; ii<(nwrap-startIndex); ii++)
	  adcbuf[tlen++] = (geo<<27) | (adc<<17) | ((*ptr++)&SIS3320_ADC_LODATA_MASK);

        ptr = &pshdata[0];
        for(ii=0; ii<(stopIndex+1); ii++)
	  adcbuf[tlen++] = (geo<<27) | (adc<<17) | ((*ptr++)&SIS3320_ADC_LODATA_MASK);
      }
    }
  }

  *lenbuf = tlen;

  return (0);
}

int
sis3320ReadEventMultiMode (UINT32 addr, INT32 *lenbuf, INT32 *adcbuf)
{
  int ii, iiw, adcCH, grp, grpCH, tmp;
  int geo, header;
  int actEvtCNT;
  int tdata, tlen;
  unsigned short *pshdata, *ptr;
  int startIndex, stopIndex;
  unsigned int data[8][MAX_EVENT_LENGTH/4];


  actEvtCNT = sis3320->ctrl->eventCount;
  if((actEvtCNT == ssMaxNEvt) && !sis3320SamplingBusyStatus())
    sis3320Arm();                       /* reactivate board to accept next events */

  iiw = ssWrapSize*ssEvtCNT/2;
  for(adcCH=0; adcCH<8; adcCH++)        /* read data from ADC into memory buffer */
    for(ii=0; ii<(ssWrapSize/2); ii++)
      data[adcCH][ii] = sis3320->data[adcCH][iiw+ii];

  geo    = addr >> 27;                  /* make geo.addr. from ADC base addr. */
  header = (geo<<27) | (ssNSamples+1);  /* make HEADER WORD as for 1881M */

  tlen   = 0;
  for(adcCH=0; adcCH<8; adcCH++)        /***** Process Data *****/
  {
    adcbuf[tlen++] = header;            /* place header */

    if(OperationMode == 0)              /*** Common Start mode ***/
    {
      for(ii=0; ii<(ssNSamples/2); ii++)
      {
        tdata = data[adcCH][ii];

        /* make First and Second DATA WORD as for 1881M */
        adcbuf[tlen++] = (geo<<27) | (adcCH<<17) |  (tdata & SIS3320_ADC_LODATA_MASK);
        adcbuf[tlen++] = (geo<<27) | (adcCH<<17) | ((tdata & SIS3320_ADC_HIDATA_MASK)>>16);
      }

    }
    else if(OperationMode == 1)         /*** Common Stop mode ***/
    {
      /* swap data: we'll use 'short' pointer, and we have big endian machine */
      for(ii=0; ii<(ssWrapSize/2); ii++)
      {
        unsigned short tt, *pp;
        pp    = (unsigned short *)&data[adcCH][ii];
        tt    = pp[0];
        pp[0] = pp[1];
        pp[1] = tt;
      }

      pshdata    = (unsigned short *)&data[adcCH][0];
      grp        = adcCH/2;
      grpCH      = adcCH%2;
      tmp        = sis3320->cnfg[grp]->eventDir[grpCH][ssEvtCNT];
      stopIndex  = FlyIndexCorrector(tmp) - iiw*2;
      if(stopIndex < 0)
	stopIndex += ssWrapSize;
      startIndex = stopIndex + ssWrapSize - ssNSamples + 1;

      if(startIndex >= ssWrapSize)      /* one piece */
      {
	startIndex -= ssWrapSize;

	ptr = &pshdata[startIndex];
	for(ii=0; ii<ssNSamples; ii++)
	  adcbuf[tlen++] = (geo<<27) | (adcCH<<17) | ((*ptr++)&SIS3320_ADC_LODATA_MASK);

      }
      else                              /* two pieces */
      {
	ptr = &pshdata[startIndex];
	for(ii=0; ii<(ssWrapSize-startIndex); ii++)
	  adcbuf[tlen++] = (geo<<27) | (adcCH<<17) | ((*ptr++)&SIS3320_ADC_LODATA_MASK);

	ptr = &pshdata[0];
	for(ii=0; ii<(stopIndex+1); ii++)
	  adcbuf[tlen++] = (geo<<27) | (adcCH<<17) | ((*ptr++)&SIS3320_ADC_LODATA_MASK);
      }
    }
  }

  ssEvtCNT++;
  if(ssEvtCNT >= ssMaxNEvt)  ssEvtCNT = 0;

  *lenbuf = tlen;

  return (0);
}



/*******************************************************************************/
/*   Declare Key Address functions                                             */
/*******************************************************************************/

void
sis3320Reset()
{
  sis3320->ctrl->keyReset = 1;
}

void
sis3320Arm()
{
  sis3320->ctrl->keyArm =1;
}

void
sis3320Disarm()
{
  sis3320->ctrl->keyDisarm =1;
}

void
sis3320Start()
{
  sis3320->ctrl->keyStart =1;
}

void
sis3320Stop()
{
  sis3320->ctrl->keyStop =1;
}

void
sis3320ResetDDR2()
{
  sis3320->ctrl->keyResetDDR2 =1;
}


/*******************************************************************************/
/*   Acquisition/Mode Functions                                                */
/*******************************************************************************/

unsigned int
sis3320AcqConfig(unsigned int val)
{
  if(val)    /* writing a 0 does nothing, so if val=0 return current value */
    sis3320->ctrl->acquistionControl = val;
  return(sis3320->ctrl->acquistionControl);
}

unsigned int
sis3320EnableAutostart()
{
  unsigned int data;
  data = sis3320->ctrl->acquistionControl | SIS3320_ACQ_ENABLE_AUTOSTART;
  sis3320->ctrl->acquistionControl = data;
  return(sis3320->ctrl->acquistionControl);
}

unsigned int
sis3320DisableAutostart()
{
  unsigned int data;
  data = sis3320->ctrl->acquistionControl & ~(SIS3320_ACQ_ENABLE_AUTOSTART);
  data |= SIS3320_ACQ_DISABLE_AUTOSTART;
  sis3320->ctrl->acquistionControl = data;
  return(sis3320->ctrl->acquistionControl);
}

int
sis3320AutostartStatus()
{
  return((sis3320->ctrl->acquistionControl & SIS3320_ACQ_ENABLE_AUTOSTART) >> 4);
}

unsigned int
sis3320EnableMultiEvent()
{
  unsigned int data;
  data = sis3320->ctrl->acquistionControl | SIS3320_ACQ_ENABLE_MULTIEVENT;
  sis3320->ctrl->acquistionControl = data;
  return(sis3320->ctrl->acquistionControl);
}

unsigned int
sis3320DisableMultiEvent()
{
  unsigned int data;
  data = sis3320->ctrl->acquistionControl & ~(SIS3320_ACQ_ENABLE_MULTIEVENT);
  data |= SIS3320_ACQ_DISABLE_MULTIEVENT;
  sis3320->ctrl->acquistionControl = data;
  return(sis3320->ctrl->acquistionControl);
}

int
sis3320MultiEventStatus()
{
  return((sis3320->ctrl->acquistionControl & SIS3320_ACQ_ENABLE_MULTIEVENT) >> 5);
}

unsigned int
sis3320EnableInternalTrigger()
{
  unsigned int data;
  data = sis3320->ctrl->acquistionControl | SIS3320_ACQ_ENABLE_INTERNAL_TRIGGER;
  sis3320->ctrl->acquistionControl = data;
  return(sis3320->ctrl->acquistionControl);
}

unsigned int
sis3320DisableInternalTrigger()
{
  unsigned int data;
  data = sis3320->ctrl->acquistionControl & ~(SIS3320_ACQ_ENABLE_INTERNAL_TRIGGER);
  data |= SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER;
  sis3320->ctrl->acquistionControl = data;
  return(sis3320->ctrl->acquistionControl);
}

int
sis3320InternalTriggerStatus()
{
  return((sis3320->ctrl->acquistionControl & SIS3320_ACQ_ENABLE_INTERNAL_TRIGGER) >> 6);
}

unsigned int
sis3320EnableLemoStartStop()
{
  unsigned int data;
  data = sis3320->ctrl->acquistionControl | SIS3320_ACQ_ENABLE_LEMO_START_STOP;
  sis3320->ctrl->acquistionControl = data;
  return(sis3320->ctrl->acquistionControl);
}

unsigned int
sis3320DisableLemoStartStop()
{
  unsigned int data;
  data = sis3320->ctrl->acquistionControl & ~(SIS3320_ACQ_ENABLE_LEMO_START_STOP);
  data |= SIS3320_ACQ_DISABLE_LEMO_START_STOP;
  sis3320->ctrl->acquistionControl = data;
  return(sis3320->ctrl->acquistionControl);
}

int
sis3320LemoStartStopStatus()
{
  return((sis3320->ctrl->acquistionControl & SIS3320_ACQ_ENABLE_LEMO_START_STOP) >> 8);
}

int
sis3320SamplingArmedStatus()
{
  return((sis3320->ctrl->acquistionControl & SIS3320_ADC_SAMPLING_ARMED) >> 16);
}

int
sis3320SamplingBusyStatus()
{
  return((sis3320->ctrl->acquistionControl & SIS3320_ADC_SAMPLING_BUSY) >> 17);
}

int
sis3320DACBusyStatus()
{
  return((sis3320->ctrl->dacCSR & SIS3320_DAC_CYCLE_BUSY) >> 15);
}

void
sis3320ClearDACs()
{
  sis3320->ctrl->dacCSR = 3;   /* Should set DACs to 32768 (0x8000) */
  /* !!!!!!!!!!!!!!!!!!!!!!!!   but not for our board, DACs values  */
  /*                            stay the same as before clear !!!!! */
}

int
sis3320SetDAC1(UINT16 offset)
{
  int timeCnt, timeOut = 5000;

  sis3320->ctrl->dacData = offset;
  sis3320->ctrl->dacCSR  = 1;   /* write to DAC Register */

  timeCnt = 0;
  do { timeCnt++; }
  while ( sis3320DACBusyStatus() && (timeCnt < timeOut) );
  if(timeCnt >= timeOut) return(-1);

  sis3320->ctrl->dacCSR  = 2;   /* Load DACs */

  timeCnt = 0;
  do { timeCnt++; }
  while ( sis3320DACBusyStatus() && (timeCnt < timeOut) );
  if(timeCnt >= timeOut) return(-2);

  return(0);
}

int
sis3320SetDACs(UINT16 *offsets)
{
  int i, return_code = 0;

  for(i=0; i<8; i++)
    if ( (return_code = sis3320SetDAC1(offsets[7-i])) != 0)
      break;

  return(return_code);
}

int
sis3320SetUniformDACs(UINT16 offset)
{
  int i, return_code = 0;
  UINT16 dacOffset[8];

  for(i=0; i<8; i++)
    dacOffset[i] = offset;

  return_code = sis3320SetDACs(dacOffset);

  return(return_code);
}

int
sis3320GetDACs(int *offsets)
{
  int i, return_code = 0;
  unsigned int tmpOffset[8] = { 0,0,0,0, 0,0,0,0 };

  for(i=0; i<8; i++) {
    if ( (return_code = sis3320SetDAC1(tmpOffset[7-i])) != 0)
      break;
    offsets[7-i] = tmpOffset[7-i] = (sis3320->ctrl->dacData & 0xffff0000 ) >> 16;
  }

  if ( return_code == 0)
    for(i=0; i<8; i++)
      if ( (return_code = sis3320SetDAC1(tmpOffset[7-i])) != 0)
	break;

  return(return_code);
}

void
sis3320DisplayDACs()
{
  unsigned int i, dacOffset[8];

  if(sis3320GetDACs(dacOffset) != 0)
    printf("sis3320DisplayDACs: ERROR: Unable to read/write DACs\n");

  printf("\n");
  for(i=0; i<8; i++)
    printf(" DAC%d = %d\n", i+1, dacOffset[i]);
  printf("\n");
}

void
sis3320SetStartDelay(unsigned int val)
{
  sis3320->ctrl->startDelay = val;
}

unsigned int
sis3320GetStartDelay()
{
  return(sis3320->ctrl->startDelay);
}

void
sis3320SetStopDelay(unsigned int val)
{
  sis3320->ctrl->stopDelay = val;
}

unsigned int
sis3320GetStopDelay()
{
  return(sis3320->ctrl->stopDelay);
}

void
sis3320SetMaxNofEvent(unsigned int val)
{
  ssMaxNEvt = val;
  sis3320->ctrl->maxNevents = ssMaxNEvt;
}

unsigned int
sis3320GetMaxNofEvent()
{
  return(sis3320->ctrl->maxNevents);
}

void
sis3320ResetEvtCNT()
{
  ssEvtCNT = 0;
}

void
sis3320SetGain(unsigned int gain)
{
  sis3320->ctrl->ADCGainControl = gain;
}

unsigned int
sis3320GetGain()
{
  return(sis3320->ctrl->ADCGainControl);
}

void
sis3320EventConfig(unsigned int val)
{
  sis3320->cnfgtot->eventConfigAllADC = val;
}

unsigned int
sis3320SetWrapPageSize(unsigned int val)
{
  unsigned int ii, conf_save;

  ii = 12;
  do {
    ii--;
    if (val <= sis3320_pagesize[ii])  break;
  } while (ii > 0);

  conf_save = sis3320->cnfg[0]->eventConfig & ~(SIS3320_PAGESIZE_MASK);
  sis3320EventConfig(conf_save | ii);
  ssWrapSize = sis3320_pagesize[sis3320->cnfg[0]->eventConfig & SIS3320_PAGESIZE_MASK];

  return(ssWrapSize);
}

unsigned int
sis3320GetWrapPageSize()
{
  return(sis3320_pagesize[sis3320->cnfg[0]->eventConfig & SIS3320_PAGESIZE_MASK]);
}

void
sis3320SetSampleStartAddr(unsigned int val)
{
  sis3320->cnfgtot->sampleStartAddressAllADC = val & SIS3320_ADC_ADDR_OFFSET_MASK;
}

void
sis3320SetSampleLength(unsigned int val)
{
  ssNSamples = val & SIS3320_ADC_SAMPLE_LENGTH_MASK;
  sis3320->cnfgtot->sampleLengthAllADC = ssNSamples;
}

unsigned int
sis3320GetSampleLength()
{
  return(sis3320->cnfg[0]->sampleLength & SIS3320_ADC_SAMPLE_LENGTH_MASK);
}

void
sis3320SetGlobalWindow(int width, int delay) /* width & delay in ns */
{
  unsigned int data;
  unsigned int clk, acqreg;
  int frequency, period;


  /********************/
  /* Set Window Width */
  if (width < 0) {
    printf("sis3320SetGlobalWindow: ERROR:   window_width < 0 \n");
    return;
  }

  acqreg = sis3320AcqConfig(0);
  clk = (acqreg & SIS3320_CLOCKSOURCE_MASK) >> 12;
  frequency = sis3320_clocksource[clk];

  if (frequency == -1) {
    printf("sis3320SetGlobalWindow: ERROR: Unknown Clock Frequency\n");
    return;
  }
  period = 1000000000 / frequency;
  data   =      width / period;     /* at 200MHz:  1000 ns = 200 clocks */

  sis3320SetSampleLength(data);     /* set Sample Length */
  sis3320SetWrapPageSize(data);     /* set Wrap Page Size */


  /*********************/
  /* Set Window Offset */
  if (delay <= 0)
    sis3320SetStopDelay( -delay/period ); /* set Stop Delay */
  else
    sis3320SetStartDelay( delay/period ); /* set Start Delay */

}

void
sis3320GetGlobalWindow()
{
  unsigned int clk, acqreg;
  unsigned int wlen, wstop, wstart, wrap;
  int frequency, period;

  acqreg = sis3320AcqConfig(0);
  clk = (acqreg & SIS3320_CLOCKSOURCE_MASK) >> 12;
  frequency = sis3320_clocksource[clk];
  if (frequency == -1) {
    printf("sis3320GetGlobalWindow: ERROR: Unknown Clock Frequency\n");
    return; }
  period = 1000000000 / frequency;

  wlen   = sis3320GetSampleLength();
  wstart = sis3320GetStartDelay();
  wstop  = sis3320GetStopDelay();
  wrap   = sis3320GetWrapPageSize();

  printf ("\n External Start Delay   = %8d clocks,  %8d ns \n", wstart, wstart*period);
  printf (" External Stop  Delay   = %8d clocks,  %8d ns \n", wstop, wstop*period);
  printf (" Width (Sample Length)  = %8d clocks,  %8d ns \n", wlen, wlen*period);
  printf (" Wrap Page Size         = %8d (0x%x) Samples \n\n", wrap, wrap);

}


/*******************************************************************************/
/*******************************************************************************/
/*     Some useful check functions                                             */
/*******************************************************************************/
/*******************************************************************************/


void
sis3320ReadTest1(int size, int prn)  /* to check sis3320ReadEvent() */
{
  int ii, lenbuf;
  unsigned int adcbuf[10000];

  bzero((char *)adcbuf,0x9c40);  /* Zero 40000 bytes */
  
  sis3320ReadEvent(SISADR, &lenbuf, adcbuf);
  printf("\nlenbuf = %d\n\n", lenbuf);

  for (ii=0; ii<size; ii++) {
    if ((ii%prn) == 0) {
      printf("adcbuf [%5d] = 0x%08x\n", ii, adcbuf[ii]);
    }
  }
}

void
sis3320ReadTest2(int size, int prn)
{
  int ii, lenbuf;
  unsigned int *adcbuf = 0;

  if(adcbuf == 0) {
    adcbuf = (int *)malloc(0x10000);
    printf("sis3320ReadTest2: Allocated 64K for sample data at 0x%x\n",(UINT32) adcbuf);
  } else {
    printf("sis3320ReadTest2: Using 64K memory for sample data at 0x%x\n",(UINT32) adcbuf);
  }

  bzero((char *)adcbuf,0x10000);  /* Zero 64K bytes */
  
  sis3320ReadEvent(SISADR, &lenbuf, adcbuf);
  printf("\nlenbuf = %d\n\n", lenbuf);

  for (ii=0; ii<size; ii++) {
    if ((ii%prn) == 0) {
      printf("adcbuf [%5d] = 0x%08x\n", ii, adcbuf[ii]);
    }
  }
} /* end of check of sis3320ReadEvent() */


void
sis3320RunTest1(int ncycles, unsigned int pageWrapSize) /* 0x0 < pageWrapSize < 0xB */
{
  unsigned int event_sample_length;
  unsigned int data;
  int ii;

  if(ncycles == 0)
    ncycles = 1;

 
  /* sis3320Reset(); */           /* Reset */

  data  = SIS3320_ACQ_SET_CLOCK_TO_200MHZ;
  data |= SIS3320_ACQ_DISABLE_LEMO_START_STOP;
  data |= SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER;
  data |= SIS3320_ACQ_DISABLE_MULTIEVENT;
  data |= SIS3320_ACQ_DISABLE_AUTOSTART;
  sis3320AcqConfig(data);   /* Program for Single Event, VME Stop, 200 MHz */

  data  = SIS3320_EVENT_CONF_ENABLE_SAMPLE_LENGTH_STOP;
  data |= SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE;
  data |= pageWrapSize;
  /*  data |= SIS3320_EVENT_CONF_PAGE_SIZE_128_WRAP; */  /* 128 Sample pagesize */
  sis3320EventConfig(data); /* Program for Wrap mode */


  for (ii=0;ii<ncycles;ii++) {

    sis3320Arm();          /* Armed Sampling Logic */
    sis3320Start();        /* Start Sampling */

    printf("Before while  %d: 0x%08x 0x%08x\n", ii,
	   &sis3320->ctrl->acquistionControl,sis3320->ctrl->acquistionControl);
    printf("Before while  %d: 0x%08x 0x%08x", ii,
	   &sis3320->cnfg[0]->actualSampleValue,sis3320->cnfg[0]->actualSampleValue);
    printf("  ADC1: %5d   ADC2: %5d\n",
	   ((sis3320->cnfg[0]->actualSampleValue)&0xfff0000)>>16,
	   (sis3320->cnfg[0]->actualSampleValue)&0xfff);

    while((sis3320->ctrl->acquistionControl)&0x20000) {
      printf("%d: 0x%08x 0x%08x\n", ii,
	     &sis3320->ctrl->acquistionControl,sis3320->ctrl->acquistionControl);
    }

    taskDelay(30);
  }


  event_sample_length = sis3320->cnfg[0]->sampleLength;
  data = (event_sample_length & 0xfffffffc) + 4;
  printf(" Printout DATA near by Sample Length Size = %d (0x%x) :\n",
	 event_sample_length, event_sample_length);
  ii = data/2 - 1;
  printf("ADC1_%04d: 0x%08x   0x%08x",ii,&sis3320->data[0][ii],sis3320->data[0][ii]);
  printf("   N+1: %5d   N: %5d\n",
	 ((sis3320->data[0][ii])&0xffff0000)>>16,(sis3320->data[0][ii])&0xffff);
  printf("ADC2_%04d: 0x%08x   0x%08x",ii,&sis3320->data[1][ii],sis3320->data[1][ii]);
  printf("   N+1: %5d   N: %5d\n",
	 ((sis3320->data[1][ii])&0xffff0000)>>16,(sis3320->data[1][ii])&0xffff);
  ii = data/2;
  printf("ADC1_%04d: 0x%08x   0x%08x",ii,&sis3320->data[0][ii],sis3320->data[0][ii]);
  printf("   N+1: %5d   N: %5d\n",
	 ((sis3320->data[0][ii])&0xffff0000)>>16,(sis3320->data[0][ii])&0xffff);
  printf("ADC2_%04d: 0x%08x   0x%08x",ii,&sis3320->data[1][ii],sis3320->data[1][ii]);
  printf("   N+1: %5d   N: %5d\n",
	 ((sis3320->data[1][ii])&0xffff0000)>>16,(sis3320->data[1][ii])&0xffff);


  printf(" Printout DATA near by Wrap Page Size = %d (0x%x) :\n",
	 sis3320_pagesize[pageWrapSize], sis3320_pagesize[pageWrapSize]);
  ii = sis3320_pagesize[pageWrapSize]/2 - 1;
  printf("ADC1_%04d: 0x%08x   0x%08x",ii,&sis3320->data[0][ii],sis3320->data[0][ii]);
  printf("   N+1: %5d   N: %5d\n",
	 ((sis3320->data[0][ii])&0xffff0000)>>16,(sis3320->data[0][ii])&0xffff);
  printf("ADC2_%04d: 0x%08x   0x%08x",ii,&sis3320->data[1][ii],sis3320->data[1][ii]);
  printf("   N+1: %5d   N: %5d\n",
	 ((sis3320->data[1][ii])&0xffff0000)>>16,(sis3320->data[1][ii])&0xffff);
  ii = sis3320_pagesize[pageWrapSize]/2;
  printf("ADC1_%04d: 0x%08x   0x%08x",ii,&sis3320->data[0][ii],sis3320->data[0][ii]);
  printf("   N+1: %5d   N: %5d\n",
	 ((sis3320->data[0][ii])&0xffff0000)>>16,(sis3320->data[0][ii])&0xffff);
  printf("ADC2_%04d: 0x%08x   0x%08x",ii,&sis3320->data[1][ii],sis3320->data[1][ii]);
  printf("   N+1: %5d   N: %5d\n",
	 ((sis3320->data[1][ii])&0xffff0000)>>16,(sis3320->data[1][ii])&0xffff);

}


void
sis3320ReadActualSampleValue()
{
  unsigned int i, data;

  for(i=0; i<4; i++) {
    data = sis3320->cnfg[i]->actualSampleValue;
    printf("0x%08x  ", &sis3320->cnfg[i]->actualSampleValue);
    printf("ActualSampleVal.=0x%08x;", data);
    printf("  ADC%d: %5d  ADC%d: %5d\n", i*2+1, (data&0xfff0000)>>16, i*2+2, data&0xfff);
  }
}


int
Get_ADC_values_of_DAC_RampDown (int Dmax, int Dmin, int prn, int adc)
{
  unsigned int i, j, i_dac_offset, error;
  unsigned int data;

  for (i_dac_offset=Dmax; i_dac_offset>Dmin; i_dac_offset--)
    {
      if((error = sis3320SetUniformDACs(i_dac_offset)) != 0)
	return(-1);

      if ((i_dac_offset%prn) == 0) {
	for(j=0; j<adc; j++) {
	  data = sis3320->cnfg[j]->actualSampleValue;
	  printf("DACOffset = %5d (0x%04x), ", i_dac_offset, i_dac_offset);
	  printf("AcSmplVal = 0x%08x at ", data);
	  printf("0x%08x;  ", &sis3320->cnfg[j]->actualSampleValue);
	  printf("ADC%d=%4d ADC%d=%4d\n", j*2+1, (data&0xfff0000)>>16, j*2+2, data&0xfff);
	}
      }
    }
  return(0);
}


void
sis3320PrintDataADC12(int nsamples)
{
  int ii;

  for (ii=0;ii<nsamples;ii++) {
    printf("ADC1_%04d: 0x%08x   0x%08x",ii,&sis3320->data[0][ii],sis3320->data[0][ii]);
    printf("   N+1: %5d   N: %5d\n",
	   ((sis3320->data[0][ii])&0xffff0000)>>16,(sis3320->data[0][ii])&0xffff);
    printf("ADC2_%04d: 0x%08x   0x%08x",ii,&sis3320->data[1][ii],sis3320->data[1][ii]);
    printf("   N+1: %5d   N: %5d\n",
	   ((sis3320->data[1][ii])&0xffff0000)>>16,(sis3320->data[1][ii])&0xffff);
  }
}


void
sis3320PrintDataADC12_2(int nsamples)
{
  int ii;

  for (ii=0;ii<nsamples;ii++) {

    printf("0x%08x  0x%08x  ADC1_%03d: %5d   ADC2_%03d: %5d \n",
	   &sis3320->data[0][ii], sis3320->data[0][ii],
	   ii*2, (sis3320->data[0][ii])&0xffff,
	   ii*2, (sis3320->data[1][ii])&0xffff);
  }
}


void
sis3320PrintForMultiEvent(int nsamples)
{
  int ii, iiw, adcCH, grp, grpCH, NN;
  int nwrap, maxEvntN, eventCNT, stopIndexReg, stopIndexDir;

  adcCH = 0;
  grp   = adcCH/2;
  grpCH = adcCH%2;
  NN    = nsamples;
  nwrap = sis3320GetWrapPageSize() / 2;  /* the number of 32-bit words in WRAP */
  /*                                        each word contains 2 samples       */

  printf("---------------------------                              MaxN EvCNT StopIndReg StopIndDir\n");
  for (ii=0;ii<NN;ii++) {

    maxEvntN     = sis3320->ctrl->maxNevents;
    eventCNT     = sis3320->ctrl->eventCount;
    stopIndexReg = sis3320->cnfg[grp]->nextSampAdr[grpCH]  & SIS3320_ADC_ADDR_OFFSET_MASK;
    stopIndexDir = sis3320->cnfg[grp]->eventDir[grpCH][ii] & SIS3320_EDIR_END_ADDR_MASK;

    iiw =  nwrap*ii;
    printf("0x%08x: 0x%08x 0x%08x  0x%08x 0x%08x %3d  %3d   0x%08x 0x%08x\n",
	   &sis3320->data[adcCH][iiw],
	   sis3320->data[adcCH][iiw],         sis3320->data[adcCH][iiw+1],
	   sis3320->data[adcCH][iiw+nwrap-2], sis3320->data[adcCH][iiw+nwrap-1],
	   maxEvntN, eventCNT, stopIndexReg, stopIndexDir);
  }
  printf("===========================\n");
}


void
sis3320Print2ForMultiEvent(int nsamples)
{
  int ii, iiw, adcCH, grp, grpCH, NN;
  int nwrap, maxEvntN, eventCNT, stopIndexReg, stopIndexDir;

  adcCH = 0;
  grp   = adcCH/2;
  grpCH = adcCH%2;
  NN    = nsamples;
  nwrap = sis3320GetWrapPageSize() / 2;  /* the number of 32-bit words in WRAP */
                                         /* each word contains 2 samples       */
  printf("*******      maxNevents (0x%08x) = %d\n",
	 &sis3320->ctrl->maxNevents, sis3320->ctrl->maxNevents);

  for (ii=0; ii<NN; ii++) {
    eventCNT     = sis3320->ctrl->eventCount;
    stopIndexReg = sis3320->cnfg[grp]->nextSampAdr[grpCH]  & SIS3320_ADC_ADDR_OFFSET_MASK;
    stopIndexDir = sis3320->cnfg[grp]->eventDir[grpCH][ii] & SIS3320_EDIR_END_ADDR_MASK;
    iiw          = nwrap*ii;
    printf("0x%08x: 0x%08x   0x%08x:   %d     0x%08x: 0x%08x   0x%08x: 0x%08x\n",
	   &sis3320->data[adcCH][iiw],               sis3320->data[adcCH][iiw],
	   &sis3320->ctrl->eventCount,               eventCNT,
	   &sis3320->cnfg[grp]->nextSampAdr[grpCH],  stopIndexReg,
	   &sis3320->cnfg[grp]->eventDir[grpCH][ii], stopIndexDir);
  }
  printf("===========================\n");
}

#else /* no UNIX version */

void
sis3320_dummy()
{
  return;
}

#endif
