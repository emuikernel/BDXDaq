/*****************************************************************************
*
*  tdc890.c  -  Driver library for readout of C.A.E.N. Model 890 TDC
*               using a VxWorks 5.2 or later based Single Board computer. 
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          August 2002
*
*  Revision  1.0 - Initial Revision
*                    - Supports up to 20 CAEN Model 890s in a Crate
*                    - Programmed I/O reads
*                    - Interrupts from a Single 890
*
*  adjusted for CLAS by Sergey Boyarinov - May 2003-present
*
*/

#ifdef VXWORKS

#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <logLib.h>
#include <taskLib.h>
#include <intLib.h>
#include <iv.h>
#include <vxLib.h>


#include "tdc890.h"

/* for time profiling*/ 
#include "coda.h"
#include "uthbook.h"
static UThisti histi[NHIST];



/* Define external Functions */
IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);
IMPORT  STATUS intDisconnect(int);
IMPORT  STATUS sysIntEnable(int);
IMPORT  STATUS sysIntDisable(int);

/* Define global variables */

/* window parameters */
static int window_offset = -4000;;
static int window_width = 4600;

/* readout options */
static int cblt_not, a24_a32, sngl_blt_mblt, berr_fifo;

/* variables filled by DmaStart and used by DmaDone */
static int ndata_save[21];
static int extra_save[21];
static int size_save[21];


#define EIEIO    __asm__ volatile ("eieio")
#define SynC     __asm__ volatile ("sync")

/*******************************************************************************
*
* tdc890Init - Initialize tdc890 Library. 
*
*
* RETURNS: OK, or ERROR if the address is invalid or board is not present.
*/

STATUS 
tdc890Init(UINT32 addr, UINT32 addr_inc, int nadc, UINT16 crateID)
{
  volatile TDC890 *tdc890;
  int ii, jj, res, rdata, errFlag = 0;
  int boardID = 0;

  /* Check if Board exists at that address */
  tdc890 = (TDC890 *) addr;
  /*res = vxMemProbe((char *) tdc890,0,4,(char *)&rdata);
  if(res < 0)
  {
    printf("tdc890Init: ERROR: No addressable board at addr=0x%x\n",
           (UINT32) tdc890);
    errFlag = 1;
  }
  else*/
  {
    /* Check if this is a Model V890 */
    boardID = tdc890->firmwareRev; 
    if((boardID != V890_BOARD_ID)&&(boardID != (V890_BOARD_ID+1)))
    {
      printf("WARN: Firmware does not match: 0x%08x (expected 0x%08x)\n",
        boardID,V890_BOARD_ID);
      /*return(ERROR);*/
    }
  }
  printf("Initialized v890 TDC at address 0x%08x \n",(UINT32)tdc890);

  /* Disable/Clear all TDCs */
  tdc890->moduleReset = 1; /* 'error' light here !!! */
  tdc890->clear = 1;

  if(errFlag > 0)
  {
    printf("tdc890Init: ERROR: Unable to initialize v890 TDC Module\n");
    return(ERROR);
  }
  else
  {
    return(OK);
  }
}

/*******************************************************************************
*
* tdc890Status - Gives Status info on specified TDC
*
*
* RETURNS: None
*/

void
tdc890Status(UINT32 addr)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  int lock;
  int drdy=0, afull=0, bfull=0;
  int berr=0, testmode=0;
  int trigMatch=0, headers=0, tdcerror[4];
  UINT16 statReg, cntlReg, afullLevel, bltEvents;
  UINT16 iLvl, iVec, evStored;
  UINT16 evCount, res;

  /* read various registers */
  lock = intLock();
  statReg = (tdc890->status)&V890_STATUS_MASK;
  cntlReg = (tdc890->control)&V890_CONTROL_MASK;
  afullLevel = tdc890->almostFullLevel;
  bltEvents = (tdc890->bltEventNumber)&V890_BLTEVNUM_MASK;
  
  iLvl = (tdc890->intLevel)&V890_INTLEVEL_MASK;
  iVec = (tdc890->intVector)&V890_INTVECTOR_MASK;
  evCount  = (tdc890->evCount)&V890_EVCOUNT_MASK;
  evStored = tdc890->evStored;
  intUnlock(lock);

  drdy  = statReg&V890_STATUS_DATA_READY;
  afull = statReg&V890_STATUS_ALMOST_FULL;
  bfull = statReg&V890_STATUS_FULL;

  trigMatch = statReg&V890_STATUS_TRIG_MATCH;
  headers  = statReg&V890_STATUS_HEADER_ENABLE;
  tdcerror[0] = (statReg&V890_STATUS_ERROR_0)>>6;
  tdcerror[1] = (statReg&V890_STATUS_ERROR_1)>>7;
  tdcerror[2] = (statReg&V890_STATUS_ERROR_2)>>8;
  tdcerror[3] = (statReg&V890_STATUS_ERROR_3)>>9;

  berr = cntlReg&V890_BUSERROR_ENABLE;
  testmode = cntlReg&V890_TESTMODE;

  /* print out status info */

  printf("STATUS for v890 TDC at base address 0x%x\n",(UINT32)tdc890);
  printf("---------------------------------------------- \n");

  if(iLvl>0) {
    printf(" Interrupts Enabled - Request level = %d words\n",afullLevel);
    printf(" VME Interrupt Level: %d   Vector: 0x%x \n",iLvl,iVec);
  } else {
    printf(" Interrupts Disabled\n");
  }
  printf("\n");

  printf("  Data Status \n");
  if(bfull) {
    printf("    Status        = 0x%04x (Buffer Full)\n",statReg);
  } else if(afull) {
    printf("    Status        = 0x%04x (Almost Full)\n",statReg);
  }else if(drdy) {
    printf("    Status        = 0x%04x (Data Ready )\n",statReg);
  }else {
    printf("    Status        = 0x%04x (No Data)\n",statReg);
  }
  
  printf("    Events Stored = %d\n",evStored);
  if(evCount == 0x00ffffff)
    printf("    Total Events  = (No Events taken)\n");
  else
    printf("    Total Events  = %d\n",evCount);
  printf("    Almost Full   = %d\n",afullLevel);

  
  printf("\n");

  printf("  TDC Mode/Status \n"); 

  tdc890GetEdgeResolution(addr,&res);
  printf("    Resolution      : %d ps\n",res);

  if(testmode)
    printf("    Test Mode       : On\n");

  if(trigMatch)
    printf("    Trigger Matching: On\n");
  else
    printf("    Trigger Matching: Off\n");
  
  if(headers)
    printf("    TDC Headers/EOB : On\n");
  else
    printf("    TDC Headers/EOB : Off\n");

  if(berr)
    printf("    Bus Errors      : On\n");
  else
    printf("    Bus Errors      : Off\n");

  printf("    TDC Errors      : 0:%d   1:%d   2:%d   3:%d\n",
	 tdcerror[0],tdcerror[1],tdcerror[2],tdcerror[3]);

  printf("\n");

}



/*******************************************************************************
*
*  tdc890ReadMicro - Read Microcontroller Register
*
*  returns  OK or ERROR
*/
STATUS
tdc890ReadMicro(UINT32 addr, UINT16 *data, int nwords)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  int ii, jj, kk=0;
  UINT16 mstatus;

retry:

  mstatus = (tdc890->microHandshake) & V890_MICRO_READOK;
  if(kk > 10)
  {
    printf("-> ReadMicro: mstatus=%d (0x%x)\n",mstatus,tdc890->microHandshake);
  }
  if(mstatus)
  {
    for(ii=0; ii<nwords; ii++)
    {
      jj=0;
      while(!((tdc890->microHandshake) & V890_MICRO_READOK))
      {
        jj++;
        if(jj>20)
        {
          logMsg("tdc890ReadMicro: ERROR1: Read Status not OK (read %d)\n",
            ii,0,0,0,0,0);
          return(ERROR);
        }
      }
      data[ii] = tdc890->microReg;
    }
  }
  else
  {
    kk++;
    if(kk>=20)
    {
      logMsg("tdc890ReadMicro: ERROR2: Read Status not OK\n",0,0,0,0,0,0);
      return(ERROR);
    }
    else
    {
      taskDelay(10);
      goto retry;
    }
  }

  if(kk > 10) printf("-> ReadMicro: kk=%d\n",kk);
  return(OK);
}

/* group read: read 'nwords' from 'nboards' boards into 'data' */
/* NOT FINISHED !!! */
STATUS
tdc890ReadMicroGroup(int nboards, UINT32 *addr, UINT16 *data[21], int nwords)
{
  volatile TDC890 *tdc890;
  int ii, jj, ll, kk;
  UINT16 *mstatus;

  for(ll=0; ll<nwords; ll++)
  {
    for(ii=0; ii<nboards; ii++)
    {
      tdc890 = (TDC890 *) addr[ii];
      mstatus[ii] = (tdc890->microHandshake) & V890_MICRO_READOK;
    }

    for(ii=0; ii<nboards; ii++)
    {
      tdc890 = (TDC890 *) addr[ii];
      kk = 0;

retry:

      if(mstatus[ii])
      {
        data[ii][ll] = tdc890->microReg;
      }
      else
      {
        kk++;
        mstatus[ii] = 0;
        if(kk>=20)
        {
          logMsg("tdc890ReadMicroGroup: ERROR: Read Status not OK\n",
                 0,0,0,0,0,0);
          return(ERROR);
        }
        else
        {
          /* wait and repeat again */
          taskDelay(10);
          mstatus[ii] = (tdc890->microHandshake) & V890_MICRO_READOK;
          goto retry;
        }
      }

      if(kk > 10) printf("-> ReadMicroGroup: ii=%d kk=%d\n",ii, kk);
	}

  }

  return(OK);
}


/*******************************************************************************
*
*  tdc890WriteMicro - Write to Microcontroller Register
*
*   returns  OK or ERROR
*/
STATUS
tdc890WriteMicro(UINT32 addr, UINT16 data)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  int kk=0;
  volatile UINT16 mstatus;

retry:

  mstatus = (tdc890->microHandshake) & V890_MICRO_WRITEOK;
  if(mstatus)
  {
    tdc890->microReg = data;
  }
  else
  {
    kk++;
    mstatus=0;
    if(kk>=20)
    {
      logMsg("tdc890WriteMicro: ERROR: Write Status not OK\n",0,0,0,0,0,0);
      return(ERROR);
    }
    else
    {
      taskDelay(10);
      goto retry;
    }
  }

  if(kk > 10) printf("-> WriteMicro: kk=%d\n",kk);
  return(OK);
}

/* group write: writes the same 'data' to 'nboards' boards */
/* NOT FINISHED !!! */
STATUS
tdc890WriteMicroGroup(int nboards, UINT32 *addr, UINT16 data)
{
  volatile TDC890 *tdc890;
  int ii, kk;
  volatile UINT16 *mstatus;

  for(ii=0; ii<nboards; ii++)
  {
    tdc890 = (TDC890 *) addr[ii];
    mstatus[ii] = (tdc890->microHandshake) & V890_MICRO_WRITEOK;
  }

  for(ii=0; ii<nboards; ii++)
  {
    tdc890 = (TDC890 *) addr[ii];
    kk = 0;

retry:

    if(mstatus[ii])
    {
      tdc890->microReg = data;
    }
    else
    {
      kk++;
      mstatus[ii] = 0;
      if(kk>=20)
      {
        logMsg("tdc890WriteMicro: ERROR: Write Status not OK\n",0,0,0,0,0,0);
        return(ERROR);
      }
      else
      {
        /* wait and repeat again */
        taskDelay(10);
        mstatus[ii] = (tdc890->microHandshake) & V890_MICRO_WRITEOK;
        goto retry;
      }
    }

    if(kk > 10) printf("-> WriteMicro: ii=%d kk=%d\n",ii, kk);
  }

  return(OK);
}








/*******************************************************************************
*
* tdc890PrintEvent - Print an event from TDC to standard out. 
*
*
* RETURNS: Number of Data words read from the TDC (including Header/Trailer).
*/

int
tdc890PrintEvent(UINT32 addr, int pflag)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  int ii, jj, nWords, evID, bunchID, evCount, headFlag, trigMatch;
  UINT32 gheader, gtrailer, theader, ttrailer, tmpData, dCnt;
  int tdcID, chanID, dataVal, tEdge;

  /* Check if there is a valid event */
  if((tdc890->status)&V890_STATUS_DATA_READY) {
    dCnt = 0;
    headFlag  = tdc890->status&V890_STATUS_HEADER_ENABLE;
    trigMatch = tdc890->status&V890_STATUS_TRIG_MATCH;

    if(trigMatch) {  /* If trigger match mode then print individual event */

      /* Read Global Header - Get event count */
      gheader = tdc890->data[0];
      if((gheader&V890_DATA_TYPE_MASK) != V890_GLOBAL_HEADER_DATA)
      {
        printf("tdc890PrintEvent: ERROR: Invalid Global Header Word 0x%08x\n",
               gheader);
        return(ERROR);
      }
      else
      {
        printf("  TDC DATA for Module at address 0x%08x\n",addr);
        evCount = (gheader&V890_GHEAD_EVCOUNT_MASK)>>5;
        dCnt++;
        printf("  Global Header: 0x%08x   Event Count = %d \n",gheader,evCount);
      }

      /* Loop over four TDC chips and get data for each */
      for(ii=0; ii<4; ii++)
      {
        /* Read TDC Header - Get event ID, Bunch ID */
        theader = tdc890->data[0];
        if((theader&V890_DATA_TYPE_MASK) != V890_TDC_HEADER_DATA)
        {
          printf("ERROR: Invalid TDC Header Word 0x%08x for TDC %d\n",
                 theader,ii);
          return(ERROR);
        }
        else
        {
          evID = (theader&V890_TDCHEAD_EVID_MASK)>>12;
          bunchID = (theader&V890_TDCHEAD_BUNCHID_MASK);
          dCnt++;
          printf("    TDC %d Header: 0x%08x   EventID = %d  Bunch ID = %d ",
                 ii,theader,evID,bunchID);
        }
        jj=0;
        tmpData = tdc890->data[0];
        dCnt++;
        while((tmpData&V890_DATA_TYPE_MASK) != V890_TDC_EOB_DATA)
        {
          if((jj % 5) == 0) printf("\n     ");
          printf("       0x%08x",tmpData);
          jj++;
          tmpData = tdc890->data[jj];	
        }
        /* reached EOB for TDC */
        printf("\n");
        ttrailer = tmpData;
        if((ttrailer&V890_DATA_TYPE_MASK) != V890_TDC_EOB_DATA)
        {
          printf("ERROR: Invalid TDC EOB Word 0x%08x for TDC %d\n",
                 ttrailer,ii);
          return(ERROR);
        }
        else
        {
          nWords = (ttrailer&V890_TDCEOB_WORDCOUNT_MASK);
          dCnt++;
          printf("    TDC %d EOB   : 0x%08x   Word Count = %d \n",
          ii,ttrailer,nWords);
        }
      }

      /* next data word should be Global EOB */
      gtrailer = tdc890->data[dCnt];
      if((gtrailer&V890_DATA_TYPE_MASK) != V890_GLOBAL_EOB_DATA)
      {
        printf("tdc890PrintEvent: ERROR: Invalid Global EOB Word 0x%08x\n",
               gtrailer);
        return(ERROR);
      }
      else
      {
        nWords = (gtrailer&V890_GEOB_WORDCOUNT_MASK)>>5;
        dCnt++;
        printf("  Global EOB   : 0x%08x   Total Word Count = %d \n",
               gtrailer,nWords);
      }
    }
    else /* Continuous Storage mode */
    {
      tmpData = tdc890->data[dCnt];
      printf("  TDC Continuous Storage DATA\n");
      while((tmpData&V890_DATA_TYPE_MASK) != V890_FILLER_DATA)
      {
        tdcID  = (tmpData&V890_TDC_MASK)>>24;
        chanID = (tmpData&V890_CHANNEL_MASK)>>19;
        tEdge = (tmpData&V890_EDGE_MASK)>>19;
        dataVal = (tmpData&V890_DATA_MASK);
        /* if((dCnt % 5) == 0) printf("\n    ");
        printf("     0x%08x",tmpData); */
        printf("    %d   %d   %d    %d\n",tdcID, chanID, tEdge, dataVal);
        dCnt++;
        tmpData = tdc890->data[dCnt];	
      }
      printf("\n");
    }
    return(dCnt);
  }
  else
  {
    printf("tdc890PrintEvent: No data available for readout!\n");
    return(0);
  }
}


/* ... */
int
tdc890StatusFull(UINT32 addr)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  int res;

  res = (tdc890->status) & V890_STATUS_FULL;

  return(res);
}
int
tdc890StatusAlmostFull(UINT32 addr)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  int res;

  res = (tdc890->status) & V890_STATUS_ALMOST_FULL;

  return(res);
}


/******************************************************************************
*
* tdc890Dready - data readyness
*
*
* RETURNS: 0(No Data) or the number of events
*/

int
tdc890Dready(UINT32 addr)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  UINT16 stat=0, nevents;
  
  stat = (tdc890->status) & V890_STATUS_DATA_READY;
  if(stat)
  {
    tdc890GetEventStored(addr, &nevents);
    return(nevents);
  }
  else
  {
    logMsg("tdc890Dready: board at 0x%08x is not ready\n",addr,2,3,4,5,6);
    return(0);
  }
}


int
tdc890EventFifo(UINT32 addr, UINT32 flag)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;

  printf("tdc890EventFifo befor: 0x%04x\n",tdc890->control);

  if(flag == 1)
    tdc890->control |= 0x100;
  else if(flag == 0)
    tdc890->control ^= 0x100; /* DOES IT WORK ? */
  else
    printf("tdc890EventFifo: unknown flag=%d\n",flag);

  printf("tdc890EventFifo after: 0x%04x\n",tdc890->control);

  return(0);
}

int
tdc890ResetMCST(UINT32 addr)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;

  tdc890->mcstCtrl = 0;

  return(0);
}

int
tdc890BusError(UINT32 addr, UINT32 flag)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;

  printf("tdc890BusError befor: 0x%04x\n",tdc890->control);

  if(flag == 1)
  {
    printf("set BUSerror\n");
    tdc890->control |= 0x1;
  }
  else if(flag == 0)
  {
    printf("reset BUSerror\n");
    tdc890->control ^= 0x1; /* DOES NOT WORK !!! */
  }
  else
    printf("tdc890BusError: unknown flag=%d\n",flag);

  printf("tdc890BusError after: 0x%04x\n",tdc890->control);

  return(0);
}

int
tdc890Align64(UINT32 addr, UINT32 flag)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;

  printf("tdc890Align64 befor: 0x%04x\n",tdc890->control);

  if(flag == 1)
    tdc890->control |= 0x10;
  else if(flag == 0)
    tdc890->control ^= 0x10; /* DOES IT WORK ? */
  else
    printf("tdc890Align64: unknown flag=%d\n",flag);

  printf("tdc890Align64 after: 0x%04x\n",tdc890->control);


/* turn time tag on to make header size multiple to 4 - was used to debug 2eVME firmware
tdc890->control |= 0x200;
 printf("TRIGGER TIME TAG enabled\n");
*/


  return(0);
}

/******************************************************************************
*
* tdc890ReadEventDma,
* tdc890ReadEvent - reads data in trigger matching mode; before this call
*                   user must make sure that trigger matching mode was set
*                   and data is ready; data will be read out until Global
*                   Trailer (Global EOB) is met
*
*
* RETURNS: Number of Data words read from the TDC (including Header/Trailer).
*/

/* Dma functions prototypes (need header !) */
STATUS usrVme2MemDmaStart(UCHAR *localVmeAddr, UCHAR *localAddr, UINT32 nbytes);

/* we'll DMA more words then actual event length, enforcing
  the same alignment for source and destination;
  mv5100 etc want it this way (see
  function  usrVme2MemDmaStart() return(-2) (universe.c) )*/

/* Sergey: mv5100: if trying to DMA odd number of 32-bit words, last word
usually lost; BUT: sometimes (on high rate 1-2kHz) it transfered !!!
crap ...
we'll check if number of words odd and add one extra ONLY if it's odd !
*/

/* I've seen another interpretation:
      'ndata' must be more then actual length to force 'VME bus error'
      return, otherwise next event readout will return it instead of data

   ... need to understand it better what all this about ...

*/

int
tdc890ReadEventDma(UINT32 addr, UINT32 *tdata)
{
  volatile UINT32 *data = (UINT32 *) addr;
  volatile UINT32 *fifo = (UINT32 *) (addr+0x1038);
  UINT32 *output = tdata;
  int ndata, mdata, fifodata, res, extra;
  int i;
  /*
TIMER_VAR;
  */

/* whole: 18usec */

/* part1: 1 usec */

  if(berr_fifo == 0x01)
  {
    /* get event length in words */
    fifodata = *fifo;
    ndata = fifodata&0xffff;
    /*
    logMsg("tdc890ReadEventDma: INFO: event fifo reports %d words\n",
           ndata,0,0,0,0,0);
	*/
  }
  else
  {
    ndata = V890_MAX_WORDS_PER_BOARD;
    mdata = 0;
    /*
    logMsg("tdc890ReadEventDma: INFO: trying to DMA %d words\n",
           ndata,0,0,0,0,0);
    */
  }

  /*need it?
  EIEIO;
  SynC;
  */

  /*
usrVmeDmaReset();
  */

  /* part2: 2 usec */

  /*
{
  TIMER_NORMALIZE;
  TIMER_START;
}
*/
  if(berr_fifo == 0x01)
  {
    if(sngl_blt_mblt >= 0x04) /* 128 bit alignment */
	{
      extra = (4-(ndata%4));
      if(extra==4) extra=0;
/*logMsg("tdc890ReadDataDma: 1: ndata=%d extra=%d\n",ndata,extra,3,4,5,6);*/
	}
	else /* 64 bit alignment */
	{
      if( (ndata%2) != 0 ) extra = 1;
      else                 extra = 0;
	  
/*logMsg("tdc890ReadDataDma: 2: ndata=%d extra=%d\n",ndata,extra,3,4,5,6);*/
	}
	
    res = usrVme2MemDmaStart( (UINT32 *)data, (UINT32 *)tdata,
                                                       ((ndata+extra)<<2) );
  }
  else
  {
    res = usrVme2MemDmaStart( (UINT32 *)data, (UINT32 *)tdata, (ndata<<2) );
  }
  if(res < 0)
  {
    logMsg("tdc890ReadEventDma: ERROR: usrVme2MemDmaStart returned %d\n",
           res,0,0,0,0,0);
    return(0);
  }
  /*else
  {
    logMsg("tdc890ReadEventDma: usrVme2MemDmaStart returned %d\n",
      res,0,0,0,0,0);
  }
  */
  /*
{
  TIMER_STOP(100000,-1);
}
*/
  /* part3: 14 usec */

  if(berr_fifo == 0x01)
  {
    /* check if transfer is completed; returns zero or ERROR  */
    if((res = usrVme2MemDmaDone()) < 0)
    {
      logMsg("tdc890ReadEventDma: ERROR: usrVme2MemDmaDone returned = %d\n",
             res,2,3,4,5,6);
      return(0);
    }
    else
    {
      mdata = (ndata+extra) - (res>>2);
	  /*
logMsg("tdc890ReadEventDma: res=%d ndata=%d extra=%d -> mdata=%d\n",
res,ndata,extra,mdata,5,6);
	  */


if( (res>0) || ((mdata%2)!=0) )
{
  logMsg("WRONG alignment 1: res=%d (ndata=%d, extra=%d => was %d), mdata=%d\n",
    res,ndata,extra,((ndata+extra)<<2),mdata,6);
}

	  /*
      logMsg("tdc890ReadEventDma: INFO: usrVme2MemDmaDone returned = %d (%d)\n",
             res,mdata,3,4,5,6);
	  */
    }
  }
  else
  {
    /* check if transfer is completed; returns zero or ERROR  */
    if((res = usrVme2MemDmaDone()) < 0)
    {
      logMsg("tdc890ReadEventDma: ERROR: usrVme2MemDmaDone returned = %d\n",
             res,2,3,4,5,6);
      return(0);
    }
    else if(res == 0)
    {
      logMsg("tdc890ReadEventDma: ERROR: usrVme2MemDmaDone returned = %d\n",
             res,2,3,4,5,6);
      logMsg("tdc890ReadEventDma: ERROR: we are expecting VME BUS error !\n",
             res,2,3,4,5,6);
      return(0);
    }
    else
    {
      mdata = ndata - (res>>2);
	  if(mdata>999)
        logMsg("tdc890ReadEventDma: INFO: usrVme2MemDmaDone returned = %d (%d)\n",
               res,mdata,3,4,5,6);
    }
  }


  return(mdata);
}


/* start only */
int
tdc890ReadEventDmaStart(int ib, UINT32 addr, UINT32 *tdata)
{
  volatile UINT32 *data = (UINT32 *) addr;
  volatile UINT32 *fifo = (UINT32 *) (addr+0x1038);
  int mdata, fifodata, res;
  int i;

  if(berr_fifo == 0x01)
  {
    /* get event length in words */
    fifodata = *fifo;
    ndata_save[ib] = fifodata&0xffff;
    /*
    logMsg("tdc890ReadEventDmaStart: INFO: event fifo reports %d words\n",
           ndata_save[ib],0,0,0,0,0);
	*/
  }
  else
  {
    ndata_save[ib] = V890_MAX_WORDS_PER_BOARD;
    mdata = 0;
    /*
    logMsg("tdc890ReadEventDmaStart: INFO: trying to DMA %d words\n",
           ndata_save[ib],0,0,0,0,0);
    */
  }

  /*usrVmeDmaReset();*/

  if(berr_fifo == 0x01)
  {
    if(sngl_blt_mblt >= 0x04) /* 128 bit alignment */
	{
      extra_save[ib] = (4-(ndata_save[ib]%4));
      if(extra_save[ib]==4) extra_save[ib]=0;
	}
	else /* 64 bit alignment */
	{
	  /*
      extra_save[ib] = ndata_save[ib]%2;
	  */
      if( (ndata_save[ib]%2) != 0 ) extra_save[ib] = 1;
      else                          extra_save[ib] = 0;
	}
	
    logMsg("tdc890ReadEventDmaStart: ndata_save=%d extra_save=%d\n",
      ndata_save[ib],extra_save[ib],3,4,5,6);
	

    res = usrVme2MemDmaStart( (UINT32 *)data, (UINT32 *)tdata,
                              ((ndata_save[ib]+extra_save[ib])<<2) );
  }
  else
  {
    res = usrVme2MemDmaStart( (UINT32 *)data, (UINT32 *)tdata, (ndata_save[ib]<<2) );
  }

  if(res < 0)
  {
    logMsg("tdc890ReadEventDmaStart: ERROR: usrVme2MemDmaStart returned %d\n",
           res,0,0,0,0,0);
  }
  /*else
  {
    logMsg("tdc890ReadEventDmaStart: usrVme2MemDmaStart returned %d\n",
      res,0,0,0,0,0);
  }
  */

  return(res);
}

int
tdc890ReadEventDmaDone(int ib)
{
  int mdata, res;
  int i;

  if(berr_fifo == 0x01) /* using fifo */
  {
    /* check if transfer is completed; returns zero or ERROR  */
    if((res = usrVme2MemDmaDone()) < 0)
    {
      logMsg("tdc890ReadEventDmaDone: ERROR: usrVme2MemDmaDone returned = %d\n",
             res,2,3,4,5,6);
      return(0);
    }
    else
    {
      mdata = (ndata_save[ib]+extra_save[ib]) - (res>>2);
	  /*
logMsg("tdc890ReadEventDmaDone: res=%d ndata_save=%d extra_save=%d -> mdata=%d\n",
res,ndata_save[ib],extra_save[ib],mdata,5,6);
	  */

      if( (res>4) || ((mdata%2)!=0) )
      {
        logMsg("WRONG alignment 2: res=%d (ndata_save=%d, extra_save=%d => was %d), mdata=%d\n",
          res,ndata_save[ib],extra_save[ib],((ndata_save[ib]+extra_save[ib])<<2),mdata,6);
      }

	  /*
      logMsg("tdc890ReadDataDma: INFO: usrVme2MemDmaDone returned = %d (%d)\n",
             res,mdata,3,4,5,6);
	  */
    }
  }
  else /* using bus error */
  {
    /* check if transfer is completed; returns zero or ERROR  */
    if((res = usrVme2MemDmaDone()) < 0)
    {
      logMsg("tdc890ReadEventDmaDone: ERROR: usrVme2MemDmaDone returned = %d\n",
             res,2,3,4,5,6);
      return(0);
    }
    else if(res == 0)
    {
      logMsg("tdc890ReadEventDmaDone: ERROR: usrVme2MemDmaDone returned = %d\n",
             res,2,3,4,5,6);
      logMsg("tdc890ReadDataDma: ERROR: we are expecting VME BUS error !\n",
             res,2,3,4,5,6);
      return(0);
    }
    else
    {
      mdata = ndata_save[ib] - (res>>2);
	  if(mdata>999)
        logMsg("tdc890ReadEventDmaDone: INFO: usrVme2MemDmaDone returned = %d (%d)\n",
               res,mdata,3,4,5,6);
    }
  }

  return(mdata);
}



int
tdc890ReadEvent(UINT32 addr, UINT32 *tdata)
{
  volatile UINT32 *data = (UINT32 *) addr;
  volatile UINT32 *fifo = (UINT32 *) (addr+0x1038);
  UINT32 *output = tdata - 1;
  int fifodata, ndata;

  if(berr_fifo == 0x01)
  {
    /* get event length in words */
    /* do not need it here but must read fifo as soon as it is enabled,
       otherwise 'full' condition will happens */
    fifodata = *fifo;
  }

  do
  {
    *(++output) = *data;
  } while( ((*output)&V890_DATA_TYPE_MASK) != V890_GLOBAL_EOB_DATA );

  return(((int)(output-tdata))+1);
}


/******************************************************************************
*
* tdc890ReadData - available data into a buffer. 
*
*
* RETURNS: Number of Data words read from the TDC (including Header/Trailer).
*/

int
tdc890ReadData(UINT32 addr, UINT32 *tdata, int maxWords)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  int ii, jj, nWords, evID, bunchID, evCount, headFlag, trigMatch;
  UINT32 gheader, gtrailer, theader, ttrailer, tmpData, dCnt;

  if(maxWords==0) maxWords = 1024;

  /* Check if there is a valid event */

  if((tdc890->status) & V890_STATUS_DATA_READY)
  {
    dCnt = 0;
    headFlag  = tdc890->status & V890_STATUS_HEADER_ENABLE;
    trigMatch = tdc890->status & V890_STATUS_TRIG_MATCH;

    if(trigMatch) /* If trigger match mode then read individual event */
    {
      /* Read Global Header - Get event count */
      gheader = tdc890->data[0];
      if((gheader & V890_DATA_TYPE_MASK) != V890_GLOBAL_HEADER_DATA)
      {
        printf("tdc890ReadData: ERROR: Invalid Global Header Word 0x%08x\n",
          gheader);
        return(ERROR);
      }
      else
      {
        tdata[dCnt] = gheader;
        evCount = (gheader & V890_GHEAD_EVCOUNT_MASK) >> 5;
        dCnt++;
      }

      /* Loop over four TDC chips and get data for each */
      for(ii=0; ii<4; ii++)
      {
        /* Read TDC Header - Get event ID, Bunch ID */
        theader = tdc890->data[0];
        if((theader&V890_DATA_TYPE_MASK) != V890_TDC_HEADER_DATA)
        {
          printf("ERROR: Invalid TDC Header Word 0x%08x for TDC %d\n",
                 theader,ii);
          return(ERROR);
        }
        else
        {
          tdata[dCnt] = theader;
          evID = (theader&V890_TDCHEAD_EVID_MASK)>>12;
          bunchID = (theader&V890_TDCHEAD_BUNCHID_MASK);
          dCnt++;
        }
        jj=0;
        tmpData = tdc890->data[0];
        while((tmpData&V890_DATA_TYPE_MASK) != V890_TDC_EOB_DATA)
        {
          tdata[dCnt] = tmpData;
          jj++;
          dCnt++;
          tmpData = tdc890->data[jj];	
        }

        /* reached EOB for TDC */
        ttrailer = tmpData;
        if((ttrailer&V890_DATA_TYPE_MASK) != V890_TDC_EOB_DATA)
        {
          printf("ERROR: Invalid TDC EOB Word 0x%08x for TDC %d\n",
                 ttrailer,ii);
          return(ERROR);
        }
        else
        {
          tdata[dCnt] = ttrailer;
          nWords = (ttrailer&V890_TDCEOB_WORDCOUNT_MASK);
          dCnt++;
        }
      }

      /* next data word should be Global EOB */
      gtrailer = tdc890->data[dCnt];
      if((gtrailer&V890_DATA_TYPE_MASK) != V890_GLOBAL_EOB_DATA)
      {
        printf("tdc890ReadData: ERROR: Invalid Global EOB Word 0x%08x\n",
          gtrailer);
        return(ERROR);
      }
      else
      {
        tdata[dCnt] = gtrailer;
        nWords = (gtrailer&V890_GEOB_WORDCOUNT_MASK)>>5;
        dCnt++;
      }
    }
    else /* Continuous Storage mode */
    {
      tmpData = tdc890->data[dCnt];
      while(((tmpData&V890_DATA_TYPE_MASK) != V890_FILLER_DATA) &&
            (dCnt<maxWords))
      {
        tdata[dCnt]=tmpData;
        dCnt++;
        tmpData = tdc890->data[dCnt];	
      }
    }

    return(dCnt);
  }
  else
  {
    printf("tdc890ReadData: No data available for readout!\n");
    return(0);
  }
}




/*********** OPCODES **************/






/****************************/
/* ACQUISITION MODE OPCODES */


void
tdc890SetTriggerMatchingMode(UINT32 addr)
{
  tdc890WriteMicro(addr,0x0000);
  tdc890Clear(addr);
  return;
}

void
tdc890SetContinuousStorageMode(UINT32 addr)
{
  tdc890WriteMicro(addr,0x0100);
  tdc890Clear(addr);
  return;
}

void
tdc890ReadAcquisitionMode(UINT32 addr, UINT16 *data)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x0200);
  tdc890ReadMicro(addr,&tdata,1);

  tdata &= 0x1;
  if(tdata==0)
    printf("Acquisition Mode: Continuous Storage\n");
  else
    printf("Acquisition Mode: Trigger Matching\n");

  return;
}


void
tdc890SetKeepToken(UINT32 addr)
{
  tdc890WriteMicro(addr,0x0300);
  return;
}

void
tdc890ClearKeepToken(UINT32 addr)
{
  tdc890WriteMicro(addr,0x0400);
  return;
}

void
tdc890LoadDefaultConfiguration(UINT32 addr)
{
  tdc890WriteMicro(addr,0x0500);
  return;
}

void
tdc890SaveUserConfiguration(UINT32 addr)
{
  tdc890WriteMicro(addr,0x0600);
  return;
}

void
tdc890LoadUserConfiguration(UINT32 addr)
{
  tdc890WriteMicro(addr,0x0700);
  return;
}

void
tdc890SetAutoLoadUserConfiguration(UINT32 addr)
{
  tdc890WriteMicro(addr,0x0800);
  return;
}

void
tdc890SetAutoLoadDefaultConfiguration(UINT32 addr)
{
  tdc890WriteMicro(addr,0x0900);
  return;
}




/************************/
/* TRIGGER MODE OPCODES */



/* set the width of the trigger window: 25ns < ww < 51175ns */
/* (25ns step) */
STATUS
tdc890SetWindowWidth(UINT32 addr, UINT32 ww)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x1000);

  if(ww < 25)         tdata = 1;
  else if(ww > 51175) tdata = 0x7FF;
  else                tdata = ww/25;
  printf("Set Window Width to %d ns\n",tdata*25);

  tdc890WriteMicro(addr,tdata);

  return(OK);
}

/* set the offset of the trigger window with respect
to the trigger itself: w0 > -800000ns (25ns step) */
STATUS
tdc890SetWindowOffset(UINT32 addr, INT32 wo)
{
  INT16 tdata;

  tdc890WriteMicro(addr,0x1100);

  if(wo < -800000) tdata = -32000;
  else             tdata = wo/25;
  printf("Set Window Offset to %d ns\n",tdata*25);

  tdc890WriteMicro(addr,tdata);

  return(OK);
}

/* 0 < wm < 1250ns (25ns step) */
STATUS
tdc890SetExtraSearchMargin(UINT32 addr, UINT32 wm)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x1200);

  if(wm < 0)         tdata = 0;
  else if(wm > 1250) tdata = 50;
  else               tdata = wm/25;
  printf("Set Extra Search Margin to %d ns\n",tdata*25);

  tdc890WriteMicro(addr,tdata);

  return(OK);
}

/* 0 < wr < ..ns (25ns step) */
STATUS
tdc890SetRejectMargin(UINT32 addr, UINT32 wr)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x1300);

  if(wr < 0)         tdata = 0;
  else if(wr > 1250) tdata = 50;
  else               tdata = wr/25;
  printf("Set Reject Margin to %d ns\n",tdata*25);

  tdc890WriteMicro(addr,tdata);

  return(OK);
}

/* enable subtruction of trigger time */
STATUS
tdc890EnableTriggerTimeSubtruction(UINT32 addr)
{
  tdc890WriteMicro(addr,0x1400);
  return(OK);
}

/* disable subtruction of trigger time */
STATUS
tdc890DisableTriggerTimeSubtruction(UINT32 addr)
{
  tdc890WriteMicro(addr,0x1500);
  return(OK);
}





/* Read back and print TDC Trigger Matching Configuration */
STATUS
tdc890ReadTriggerConfiguration(UINT32 addr)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  INT16 tmp[5] = {0,0,0,0,0};

  EIEIO;
  SynC;
  tdc890WriteMicro(addr,0x1600);
  EIEIO;
  SynC;

  /*tdc890ReadMicro(addr,tmp,5);*/
  tdc890ReadMicro(addr,&tmp[0],1);
  tdc890ReadMicro(addr,&tmp[1],1);
  tdc890ReadMicro(addr,&tmp[2],1);
  tdc890ReadMicro(addr,&tmp[3],1);
  tdc890ReadMicro(addr,&tmp[4],1);

  EIEIO;
  SynC;

  printf("  Window Width              = %6d ns\n",tmp[0]*25);
  printf("  Window Offset             = %6d ns\n",tmp[1]*25);
  printf("  Extra Seach Margin        = %6d ns\n",tmp[2]*25);
  printf("  Reject Margin             = %6d ns\n",tmp[3]*25);
  printf("  Trigger Time Subtruction = %6d\n",tmp[4]);

  return(OK);
}







/*****************************************/
/* EDGE DETECTION AND RESOLUTION OPCODES */


/* 0-pair, 1-trailing, 2-leading, 3-both leading and trailing */
STATUS
tdc890SetEdgeDetectionConfig(UINT32 addr, UINT16 data)
{
  UINT16 tdata;

  tdata = data & 0x3;

  printf("SetEdgeDetectionConfig: ");
  if(tdata==0x0)
    printf("set pair mode\n");
  else if(tdata==0x1)
    printf("set trailing edge only\n");
  else if(tdata==0x2)
    printf("set leading edge only\n");
  else
    printf("set both leading and trailing edges\n");


  tdc890WriteMicro(addr,0x2200);
  tdc890WriteMicro(addr,tdata);

  return(OK);
}


STATUS
tdc890ReadEdgeDetectionConfig(UINT32 addr, UINT16 *data)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x2300);
  tdc890ReadMicro(addr,&tdata,1);

  *data = 0;
  tdata &= 0x3;
  printf("ReadEdgeDetectionConfig: tdata=%d (0x%x)\n",tdata,tdata);
  if(tdata==0)
    printf("ReadEdgeDetectionConfig: paired measurement\n");
  else if(tdata==1)
    printf("ReadEdgeDetectionConfig: trailing edge\n");
  else if(tdata==2)
    printf("ReadEdgeDetectionConfig: leading edge\n");
  else if(tdata==3)
    printf("ReadEdgeDetectionConfig: both leading and trailing\n");
  else
  {
    printf("ReadEdgeDetectionConfig: ERROR, tdata=%d (0x%x)\n",tdata,tdata);
    return(ERROR);
  }
  *data = tdata;

  return(OK);
}


/* set leading/trailing edge resolution: can be 100, 200 or 800 */
STATUS
tdc890SetEdgeResolution(UINT32 addr, UINT16 data)
{
  UINT16 tdata;

  if(data==800)      tdata = 0;
  else if(data==200) tdata = 1;
  else if(data==100) tdata = 2;
  else
  {
    printf("ERROR: SetEdgeResolution: data=%d, must be 100, 200 or 800 ps\n",data);
    return(ERROR);
  }

  tdc890WriteMicro(addr,0x2400);
  tdc890WriteMicro(addr,tdata);
  printf("Set Edge Resolution to %d ps\n",data);

  return(OK);
}


/* get leading/trailing edge resolution */
STATUS
tdc890GetEdgeResolution(UINT32 addr, UINT16 *data)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x2600);
  tdc890ReadMicro(addr,&tdata,1);

  tdata &= 0x3;
  if(tdata==0)      *data = 800;
  else if(tdata==1) *data = 200;
  else if(tdata==2) *data = 100;
  else
  {
    printf("ERROR: GetEdgeResolution: tdata=%d (0x%x)\n",tdata,tdata);
    return(ERROR);
  }
  printf("Edge Resolution is %d ps\n",*data);

  return(OK);
}


/* set pair resolution:
    leading time resolution (data1): 100,200,400,800,1600,3120,6250,12500 ps
    width resolution (data2): 100,200,400,800,1600,3120,6250,12500,
                              25000,50000,100000,200000,400000,800000 ps
*/
STATUS
tdc890SetPairResolution(UINT32 addr, UINT32 data1, UINT32 data2)
{
  UINT16 tdata;

  if(data1==100)        tdata = 0x0;
  else if(data1==200)   tdata = 0x1;
  else if(data1==400)   tdata = 0x2;
  else if(data1==800)   tdata = 0x3;
  else if(data1==1600)  tdata = 0x4;
  else if(data1==3120)  tdata = 0x5;
  else if(data1==6250)  tdata = 0x6;
  else if(data1==12500) tdata = 0x7;
  else
  {
    printf("ERROR: SetPairResolution: data1=%d, see source file\n",data1);
    return(ERROR);
  }

  if(data2==100)         tdata += 0x0;
  else if(data2==200)    tdata += 0x100;
  else if(data2==400)    tdata += 0x200;
  else if(data2==800)    tdata += 0x300;
  else if(data2==1600)   tdata += 0x400;
  else if(data2==3200)   tdata += 0x500;
  else if(data2==6250)   tdata += 0x600;
  else if(data2==12500)  tdata += 0x700;
  else if(data2==25000)  tdata += 0x800;
  else if(data2==50000)  tdata += 0x900;
  else if(data2==100000) tdata += 0xA00;
  else if(data2==200000) tdata += 0xB00;
  else if(data2==400000) tdata += 0xC00;
  else if(data2==800000) tdata += 0xD00;
  else
  {
    printf("ERROR: SetPairResolution: data2=%d, see source file\n",data2);
    return(ERROR);
  }

  tdc890WriteMicro(addr,0x2500);
  tdc890WriteMicro(addr,tdata);
  printf("Set Pair Resolution to %d(leading time), %d(width) ps\n",data1,data2);

  return(OK);
}


/* get pair resolution */
STATUS
tdc890GetPairResolution(UINT32 addr, UINT32 *data1, UINT32 *data2)
{
  UINT16 tdata, tdata1, tdata2;

  tdc890WriteMicro(addr,0x2600);
  tdc890ReadMicro(addr,&tdata,1);

  tdata1 = tdata & 0x7;
  if(tdata1 == 0x0)      *data1 = 100;
  else if(tdata1 == 0x1) *data1 = 200;
  else if(tdata1 == 0x2) *data1 = 400;
  else if(tdata1 == 0x3) *data1 = 800;
  else if(tdata1 == 0x4) *data1 = 1600;
  else if(tdata1 == 0x5) *data1 = 3120;
  else if(tdata1 == 0x6) *data1 = 6250;
  else if(tdata1 == 0x7) *data1 = 12500;
  else
  {
    printf("ERROR: GetPairResolution: tdata1=%d\n",tdata1);
    return(ERROR);
  }

  tdata2 = tdata & 0xF00;
  if(tdata2 == 0x0)        *data2 = 100;
  else if(tdata2 == 0x100) *data2 = 200;
  else if(tdata2 == 0x200) *data2 = 400;
  else if(tdata2 == 0x300) *data2 = 800;
  else if(tdata2 == 0x400) *data2 = 1600;
  else if(tdata2 == 0x500) *data2 = 3200;
  else if(tdata2 == 0x600) *data2 = 6250;
  else if(tdata2 == 0x700) *data2 = 12500;
  else if(tdata2 == 0x800) *data2 = 25000;
  else if(tdata2 == 0x900) *data2 = 50000;
  else if(tdata2 == 0xA00) *data2 = 100000;
  else if(tdata2 == 0xB00) *data2 = 200000;
  else if(tdata2 == 0xC00) *data2 = 400000;
  else if(tdata2 == 0xD00) *data2 = 800000;
  else
  {
    printf("ERROR: GetPairResolution: tdata2=%d\n",tdata2);
    return(ERROR);
  }
  printf("Pair Resolution is %d(leading time), %d(width) ps\n",*data1,*data2);

  return(OK);
}

/* set double hit resolution: can be 5, 10, 30 or 100 ns */
STATUS
tdc890SetDoubleHitResolution(UINT32 addr, UINT16 data)
{
  UINT16 tdata;

  if(data==5)        tdata = 0;
  else if(data==10)  tdata = 1;
  else if(data==30)  tdata = 2;
  else if(data==100) tdata = 3;
  else
  {
    printf("ERROR: SetDoubleHitResolution: data=%d, must be 5, 10, 30 or 100 ns\n",data);
    return(ERROR);
  }

  tdc890WriteMicro(addr,0x2800);
  tdc890WriteMicro(addr,tdata);
  printf("Set Double Hit Resolution to %d ns\n",data);

  return(OK);
}

/* get double hit resolution */
STATUS
tdc890GetDoubleHitResolution(UINT32 addr, UINT16 *data)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x2900);
  tdc890ReadMicro(addr,&tdata,1);

  tdata &= 0x3;
  if(tdata==0)      *data = 5;
  else if(tdata==1) *data = 10;
  else if(tdata==2) *data = 30;
  else if(tdata==3) *data = 100;
  else
  {
    printf("ERROR: GetDoubleHitResolution: tdata=%d (0x%x)\n",tdata,tdata);
    return(ERROR);
  }
  printf("Double Hit Resolution is %d ns\n",*data);

  return(OK);
}






/***********************/
/* TDC READOUT OPCODES */


STATUS
tdc890EnableTDCHeaderAndTrailer(UINT32 addr)
{
  tdc890WriteMicro(addr,0x3000);
  return(OK);
}

STATUS
tdc890DisableTDCHeaderAndTrailer(UINT32 addr)
{
  tdc890WriteMicro(addr,0x3100);
  return(OK);
}


STATUS
tdc890GetTDCHeaderAndTrailer(UINT32 addr, UINT16 *data)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x3200);
  tdc890ReadMicro(addr,&tdata,1);

  tdata &= 0x1;
  if(tdata==0)
    printf("GetTDCHeaderAndTrailer: TDC Header/Trailer disabled\n");
  else if(tdata==1)
    printf("GetTDCHeaderAndTrailer: TDC Header/Trailer enabled\n");
  else
  {
    printf("ERROR: GetTDCHeaderAndTrailer: tdata=%d (0x%x)\n",tdata,tdata);
    return(ERROR);
  }
  *data = tdata;

  return(OK);
}


STATUS
tdc890SetMaxNumberOfHitsPerEvent(UINT32 addr, UINT32 nhits)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x3300);

  if(nhits==0)        tdata = 0;
  else if(nhits==1)   tdata = 1;
  else if(nhits==2)   tdata = 2;
  else if(nhits==4)   tdata = 3;
  else if(nhits==8)   tdata = 4;
  else if(nhits==16)  tdata = 5;
  else if(nhits==32)  tdata = 6;
  else if(nhits==64)  tdata = 7;
  else if(nhits==128) tdata = 8;
  else                tdata = 9;

  if(tdata==9)
    printf("Set Unlimited Number Of Hits Per Event\n");
  else
    printf("Set Maximum Number Of Hits Per Event to %d\n",nhits);


  tdc890WriteMicro(addr,tdata);

  return(OK);
}

STATUS
tdc890GetMaxNumberOfHitsPerEvent(UINT32 addr, UINT16 *data)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x3400);
  tdc890ReadMicro(addr,&tdata,1);

  tdata &= 0xF;
  if(tdata==0)
    printf("GetMaxNumberOfHitsPerEvent: 0 hits\n");
  else if(tdata==1)
    printf("GetMaxNumberOfHitsPerEvent: 1 hit\n");
  else if(tdata==2)
    printf("GetMaxNumberOfHitsPerEvent: 2 hits\n");
  else if(tdata==3)
    printf("GetMaxNumberOfHitsPerEvent: 4 hits\n");
  else if(tdata==4)
    printf("GetMaxNumberOfHitsPerEvent: 8 hits\n");
  else if(tdata==5)
    printf("GetMaxNumberOfHitsPerEvent: 16 hits\n");
  else if(tdata==6)
    printf("GetMaxNumberOfHitsPerEvent: 32 hits\n");
  else if(tdata==7)
    printf("GetMaxNumberOfHitsPerEvent: 64 hits\n");
  else if(tdata==8)
    printf("GetMaxNumberOfHitsPerEvent: 128 hits\n");
  else if(tdata==9)
    printf("GetMaxNumberOfHitsPerEvent: unlimited\n");
  else
  {
    printf("ERROR: GetMaxNumberOfHitsPerEvent: tdata=%d (0x%x)\n",tdata,tdata);
    return(ERROR);
  }
  *data = tdata;

  return(OK);
}

STATUS
tdc890EnableTDCErrorMark(UINT32 addr)
{
  tdc890WriteMicro(addr,0x3500);
  return(OK);
}

STATUS
tdc890DisableTDCErrorMark(UINT32 addr)
{
  tdc890WriteMicro(addr,0x3600);
  return(OK);
}

STATUS
tdc890EnableTDCErrorBypass(UINT32 addr)
{
  tdc890WriteMicro(addr,0x3700);
  return(OK);
}

STATUS
tdc890DisableTDCErrorBypass(UINT32 addr)
{
  tdc890WriteMicro(addr,0x3800);
  return(OK);
}

STATUS
tdc890SetTDCErrorType(UINT32 addr, UINT32 mask)
{
  UINT16 tdata;

  tdata = mask & 0x7FF;
  tdc890WriteMicro(addr,0x3900);
  tdc890WriteMicro(addr,tdata);

  return(OK);
}

STATUS
tdc890GetTDCErrorType(UINT32 addr, UINT16 *mask)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x3A00);
  tdc890ReadMicro(addr,&tdata,1);

  tdata &= 0x7FF;
  if(tdata & 0x1)
    printf("GetTDCErrorType: Vernier error\n");
  else if(tdata & 0x2)
    printf("GetTDCErrorType: Coarse error\n");
  else if(tdata & 0x4)
    printf("GetTDCErrorType: Channel select error\n");
  else if(tdata & 0x8)
    printf("GetTDCErrorType: L1 buffer parity error\n");
  else if(tdata & 0x10)
    printf("GetTDCErrorType: Trigger fifo parity error\n");
  else if(tdata & 0x20)
    printf("GetTDCErrorType: Trigger matching error\n");
  else if(tdata & 0x40)
    printf("GetTDCErrorType: Readout fifo parity error\n");
  else if(tdata & 0x80)
    printf("GetTDCErrorType: Readout state error\n");
  else if(tdata & 0x100)
    printf("GetTDCErrorType: Set up parity error\n");
  else if(tdata & 0x200)
    printf("GetTDCErrorType: Control parity error\n");
  else if(tdata & 0x400)
    printf("GetTDCErrorType: Jtag instruction parity error\n");

  *mask = tdata;

  return(OK);
}

STATUS
tdc890SetEffectiveSizeOfReadoutFIFO(UINT32 addr, UINT32 nwords)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x3B00);

  if(nwords==2)        tdata = 0;
  else if(nwords==4)   tdata = 1;
  else if(nwords==8)   tdata = 2;
  else if(nwords==16)  tdata = 3;
  else if(nwords==32)  tdata = 4;
  else if(nwords==64)  tdata = 5;
  else if(nwords==128) tdata = 6;
  else if(nwords==256) tdata = 7;
  else                 tdata = 7;

  printf("Set Effective Size Of Readout FIFO to %d\n",nwords);

  tdc890WriteMicro(addr,tdata);

  return(OK);
}

STATUS
tdc890GetEffectiveSizeOfReadoutFIFO(UINT32 addr, UINT16 *data)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x3C00);
  tdc890ReadMicro(addr,&tdata,1);

  tdata &= 0x7;
  if(tdata==0)
    printf("GetEffectiveSizeOfReadoutFIFO: 2 words\n");
  else if(tdata==1)
    printf("GetEffectiveSizeOfReadoutFIFO: 4 words\n");
  else if(tdata==2)
    printf("GetEffectiveSizeOfReadoutFIFO: 8 words\n");
  else if(tdata==3)
    printf("GetEffectiveSizeOfReadoutFIFO: 16 words\n");
  else if(tdata==4)
    printf("GetEffectiveSizeOfReadoutFIFO: 32 words\n");
  else if(tdata==5)
    printf("GetEffectiveSizeOfReadoutFIFO: 64 words\n");
  else if(tdata==6)
    printf("GetEffectiveSizeOfReadoutFIFO: 128 words\n");
  else if(tdata==7)
    printf("GetEffectiveSizeOfReadoutFIFO: 256 words\n");
  else
  {
    printf("ERROR: GetEffectiveSizeOfReadoutFIFO: tdata=%d (0x%x)\n",tdata,tdata);
    return(ERROR);
  }
  *data = tdata;

  return(OK);
}


/**************************/
/* CHANNEL ENABLE OPCODES */


STATUS
tdc890EnableChannel(UINT32 addr, UINT16 channel)
{
  int opcode = 0x4000 + (channel & 0x7F);

  tdc890WriteMicro(addr,opcode);
  return(OK);
}

STATUS
tdc890DisableChannel(UINT32 addr, UINT16 channel)
{
  int opcode = 0x4100 + (channel & 0x7F);

  tdc890WriteMicro(addr,opcode);
  return(OK);
}

STATUS
tdc890EnableAllChannels(UINT32 addr)
{
  tdc890WriteMicro(addr,0x4200);
  return(OK);
}

STATUS
tdc890DisableAllChannels(UINT32 addr)
{
  tdc890WriteMicro(addr,0x4300);
  return(OK);
}

STATUS
tdc890EnableChannels(UINT32 addr, UINT16 channels[8])
{
  int i;

  tdc890WriteMicro(addr,0x4400);
  for(i=0; i<8; i++) tdc890WriteMicro(addr,channels[i]);
  return(OK);
}

STATUS
tdc890GetChannels(UINT32 addr, UINT16 channels[8])
{
  int i;

  tdc890WriteMicro(addr,0x4500);
  for(i=0; i<8; i++) tdc890ReadMicro(addr,&channels[i],1);
  return(OK);
}


/********************/
/* ADVANCED OPCODES */


STATUS
tdc890GetTDCError(UINT32 addr, UINT16 ntdc, UINT16 *error)
{
  int opcode = 0x7400 + (ntdc & 0x3);
  unsigned short tmp;

  tdc890WriteMicro(addr,opcode);
  tdc890ReadMicro(addr,&tmp,1);

  *error = tmp & 0x7FF;

  return(OK);
}

STATUS
tdc890GetTDCDLLLock(UINT32 addr, UINT16 ntdc, UINT16 *lock)
{
  int opcode = 0x7500 + (ntdc & 0x3);
  unsigned short tmp;

  tdc890WriteMicro(addr,opcode);
  tdc890ReadMicro(addr,&tmp,1);

  *lock = tmp & 0x1;

  return(OK);
}



















/****************************************************/
/************** MCST/CBLT functions *****************/


STATUS
tdc890InitMCST(int nboards, UINT32 *addr, UINT32 *vmeaddress)
{
  volatile TDC890 *tdc890;
  unsigned int baseadrs;
  int ii;

  /* will be downloaded to every board's register */
  baseadrs = (V890_A32_ADDR >> 24) & 0xFF;
  baseadrs += 0x01; /*do not coincide with the address installed by switches*/

  /* we have to return address in 'local' space, because usrVme2MemDmaStart()
  will convert it back to 'bus' space */
  sysBusToLocalAdrs(0x09,(baseadrs<<24),vmeaddress);

  printf("tdc base address = 0x%02x, global =0x%08x\n",baseadrs,*vmeaddress);



  printf("tdc890InitMCST: set base address 0x%02x for following boards:\n",
    (baseadrs & V890_MCSTBASEADDR_MASK));
  for(ii=0; ii<nboards; ii++)
  {
    tdc890 = (TDC890 *) addr[ii];

    tdc890->mcstBaseAddr = (baseadrs & V890_MCSTBASEADDR_MASK);

    if(ii==0)
    {
      tdc890->mcstCtrl = 2; /* active first */
      printf("tdc890InitMCST:     first  board at 0x%08x\n",addr[ii]);
    }
    else if(ii==(nboards-1))
    {
      tdc890->mcstCtrl = 1; /* active last */
      printf("tdc890InitMCST:     last   board at 0x%08x\n",addr[ii]);
    }
    else
    {
      tdc890->mcstCtrl = 3; /* active intermediate */
      printf("tdc890InitMCST:     middle board at 0x%08x\n",addr[ii]);
    }
  }

  return(OK);
}





UINT32 tdcbaseadr11, offset11;

test33()
{
  UINT32 tdcadr[4] = {0xfa210000,0xfa220000,0xfa230000,0xfa240000};

  sysBusToLocalAdrs(0x39,0,&offset11);
  printf("offset(0x39)=0x%08x\n",offset11);
  sysBusToLocalAdrs(0x0A,V890_A32_ADDR,&offset11);
  printf("offset(0x0A)=0x%08x\n",offset11);
  sysBusToLocalAdrs(0x09,V890_A32_ADDR,&offset11);
  printf("offset(0x09)=0x%08x\n",offset11);


  tdcbaseadr11 = (/*V890_A32_ADDR*/offset11 >> 24) & 0xFF;
  printf("fbrol1: tdcbaseadr11 = 0x%02x\n",tdcbaseadr11);

  /*tdc890InitMCST(4,tdcadr,tdcbaseadr11);*/
  tdc890InitMCST(1,tdcadr,tdcbaseadr11);

}

test44()
{
  volatile TDC890 *tdc890 = (TDC890 *) offset11;
  printf("test44: offset11=0x%08x\n",offset11);
  tdc890->clear = 1;
}

test333()
{
  UINT32 tdcadr[8] = {0xf0110000,0xf0120000,0xf0130000,0xf0140000,
                      0xf0150000,0xf0160000,0xf0170000,0xf0180000};

  sysBusToLocalAdrs(0x39,0,&offset11);
  printf("offset(0x39)=0x%08x\n",offset11);
  sysBusToLocalAdrs(0x0A,V890_A32_ADDR,&offset11);
  printf("offset(0x0A)=0x%08x\n",offset11);
  sysBusToLocalAdrs(0x09,V890_A32_ADDR,&offset11);
  printf("offset(0x09)=0x%08x\n",offset11);


  tdcbaseadr11 = (V890_A32_ADDR/*offset11*/ >> 24) & 0xFF;
  printf("fbrol1: tdcbaseadr11 = 0x%02x\n",tdcbaseadr11);

  tdc890InitMCST(8,tdcadr,tdcbaseadr11);

}

test444()
{
  volatile TDC890 *tdc890 = (TDC890 *) offset11;
  printf("test444: offset11=0x%08x\n",offset11);
  tdc890->clear = 1;
}


/******************************************************************************
*
* tdc890Trig         - Issue Software trigger to TDC
* tdc890Enable       - Bring TDC Online (Enable Gates)
* tdc890Disable      - Bring TDC Offline (Disable Gates)
* tdc890Clear        - Clear TDC
* tdc890Reset        - Clear/Reset TDC
*
*
* RETURNS: None.
*/

void
tdc890Trig(UINT32 addr)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  tdc890->trigger = 1;
  return;
}

void
tdc890Enable(UINT32 addr)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  return;
}

void
tdc890Disable(UINT32 addr)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  return;
}


void
tdc890Clear(UINT32 addr)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  tdc890->clear = 1;
  return;
}

void
tdc890Reset(UINT32 addr)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  tdc890->moduleReset = 1;
  return;
}

void
tdc890GetEventCounter(UINT32 addr, UINT32 *nevents)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  *nevents = tdc890->evCount;
  return;
}

void
tdc890GetEventStored(UINT32 addr, UINT16 *nevents)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  *nevents = tdc890->evStored;
  return;
}

/* */
void
tdc890SetAlmostFullLevel(UINT32 addr, UINT16 nwords)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  tdc890->almostFullLevel = (nwords & 0x7FFF);
  return;
}

void
tdc890GetAlmostFullLevel(UINT32 addr, UINT16 *nwords)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  *nwords = tdc890->almostFullLevel;
  printf("Almost Full Level set to %d (0x%04x) words\n",*nwords,*nwords);
  return;
}


/* 0-data ready, 1-full, 2-almost full, 3-error*/
void
tdc890SetOutProg(UINT32 addr, UINT16 code)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  tdc890->outProgControl = code & 0x3;
  return;
}

void
tdc890GetOutProg(UINT32 addr, UINT16 *code)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  *code = tdc890->outProgControl;
  printf("OutProg set to %d\n",*code);
  return;
}





/*******************************************/
/*******************************************/



void
tdc890Output(UINT32 addr, int delay)
{
  if(delay<=0) delay=2;

  while(1)
  {
    tdc890PrintEvent(addr,0);
    taskDelay(60*delay);
  }

  return;
}


void
tdc890TestMode(UINT32 addr, int off)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;

  if(off) 
    tdc890->control &= (~V890_TESTMODE&0xf);
  else
    tdc890->control |= V890_TESTMODE;

  return;
}

void
tdc890Test(UINT32 addr, UINT16 tval)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  UINT16 testMode=0;

  testMode = tdc890->control&V890_TESTMODE;
  if(testMode)
  {
    if(tval==0)
      tdc890->testReg = 0x11223344;
    else
      (UINT16 *)(tdc890->testReg) = tval;
  }
  else
  {
    printf("tdc890Test: ERROR: TestMode not enabled.\n");
  }
     
  return;
}


void
tdc890SetGeoAddress(UINT32 addr, UINT16 geoaddr)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  tdc890->geoAddr = geoaddr & 0x1F;
  return;
}

void
tdc890GetGeoAddress(UINT32 addr, UINT16 *geoaddr)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  *geoaddr = tdc890->geoAddr & 0x1F;
  return;
}


void
tdc890SetBLTEventNumber(UINT32 addr, UINT16 nevents)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  tdc890->bltEventNumber = nevents & 0xFF;
  return;
}

void
tdc890GetBLTEventNumber(UINT32 addr, UINT16 *nevents)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  *nevents = tdc890->bltEventNumber;
  return;
}




/***********************/
/* INTERRUPT FUNCTIONS */
/***********************/





/* Define Interrupts variables */

BOOL              v890IntRunning = FALSE; /* running flag */
int               v890IntID      = -1; /* id number of TDC generating interrupts */
LOCAL VOIDFUNCPTR v890IntRoutine = NULL; /* user interrupt service routine */
LOCAL int         v890IntArg     = 0; /* arg to user routine */
LOCAL int         v890IntEvCount = 0; /* Number of Events to generate Interrupt */
LOCAL UINT32      v890IntLevel   = V890_VME_INT_LEVEL; /* default VME interrupt level */
LOCAL UINT32      v890IntVec     = V890_INT_VEC; /* default interrupt Vector */

int Nv890 = 0;
volatile struct v890_struct *v890p[20];
int v890IntCount = 0;                   /* Count of interrupts from TDC */



/*******************************************************************************
*
* v890Int - default interrupt handler
*
* This rountine handles the v890 TDC interrupt.  A user routine is
* called, if one was connected by v890IntConnect().
*
* RETURNS: N/A
*
*/

LOCAL void 
v890Int (void)
{

  UINT32 nevt=0;
  
  /* Disable interrupts */
  sysIntDisable(v890IntLevel);

  v890IntCount++;
 
  if (v890IntRoutine != NULL)  {     /* call user routine */
    (*v890IntRoutine) (v890IntArg);
  }else{
    if((v890IntID<0) || (v890p[v890IntID] == NULL)) {
      logMsg("v890Int: ERROR : TDC id %d not initialized \n",v890IntID,0,0,0,0,0);
      return;
    }

    logMsg("v890Int: Processed %d events\n",nevt,0,0,0,0,0);

  }

  /* Enable interrupts */
  sysIntEnable(v890IntLevel);

}


/*******************************************************************************
*
* v890IntConnect - connect a user routine to the v890 TDC interrupt
*
* This routine specifies the user interrupt routine to be called at each
* interrupt. 
*
* RETURNS: OK, or ERROR if Interrupts are enabled
*/

STATUS 
v890IntConnect (VOIDFUNCPTR routine, int arg, UINT16 level, UINT16 vector)
{

  if(v890IntRunning) {
    printf("v890IntConnect: ERROR : Interrupts already Initialized for TDC id %d\n",
	   v890IntID);
    return(ERROR);
  }
  
  v890IntRoutine = routine;
  v890IntArg = arg;

  /* Check for user defined VME interrupt level and vector */
  if(level == 0) {
    v890IntLevel = V890_VME_INT_LEVEL; /* use default */
  }else if (level > 7) {
    printf("v890IntConnect: ERROR: Invalid VME interrupt level (%d). Must be (1-7)\n",level);
    return(ERROR);
  } else {
    v890IntLevel = level;
  }

  if(vector == 0) {
    v890IntVec = V890_INT_VEC;  /* use default */
  }else if ((vector < 32)||(vector>255)) {
    printf("v890IntConnect: ERROR: Invalid interrupt vector (%d). Must be (32<vector<255)\n",vector);
    return(ERROR);
  }else{
    v890IntVec = vector;
  }
      
  /* Connect the ISR */
#ifdef VXWORKSPPC
  if((intDisconnect((int)INUM_TO_IVEC(v890IntVec)) != 0)) {
    printf("v890IntConnect: ERROR disconnecting Interrupt\n");
    return(ERROR);
  }
#endif
  if((intConnect(INUM_TO_IVEC(v890IntVec),v890Int,0)) != 0) {
    printf("v890IntConnect: ERROR in intConnect()\n");
    return(ERROR);
  }

  return (OK);
}


/*******************************************************************************
*
* v890IntEnable - Enable interrupts from specified TDC
*
* Enables interrupts for a specified TDC.
* 
* RETURNS OK or ERROR if TDC is not available or parameter is out of range
*/

STATUS 
v890IntEnable (int id, UINT16 evCnt)
{

  if(v890IntRunning) {
    printf("v890IntEnable: ERROR : Interrupts already initialized for TDC id %d\n",
	   v890IntID);
    return(ERROR);
  }

  if((id<0) || (v890p[id] == NULL)) {
    printf("v890IntEnable: ERROR : TDC id %d not initialized \n",id);
    return(ERROR);
  }else{
    v890IntID = id;
  }
  
  
  sysIntEnable(v890IntLevel);   /* Enable VME interrupts */
  
  /* Zero Counter and set Running Flag */
  v890IntEvCount = evCnt;
  v890IntCount = 0;
  v890IntRunning = TRUE;
  /* Enable interrupts on TDC */
  
  return(OK);
}


/*******************************************************************************
*
* v890IntDisable - disable the TDC interrupts
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS 
v890IntDisable (int iflag)
{

  if((v890IntID<0) || (v890p[v890IntID] == NULL)) {
    logMsg("v890IntDisable: ERROR : TDC id %d not initialized \n",v890IntID,0,0,0,0,0);
    return(ERROR);
  }

  sysIntDisable(v890IntLevel);   /* Disable VME interrupts */


  /* Tell tasks that Interrupts have been disabled */
  if(iflag > 0) {
    v890IntRunning = FALSE;
  }else{
    /*boy semGive(v890Sem);*/
  }
  
  return (OK);
}




/***************************/
/* CLAS-SPECIFIC FUNCTIONS */
/***************************/

/* CAEN v1190/v1290 TDCs base address and step */
/* first board has address TDCADR, second TDCADR+TDCSTEP etc */
/* NBOARDS - max # of boards */

#define TDCADR  0x210000
#define TDCSTEP 0x010000
#define NBOARDS 22 /* we have 21 boards, but numbering starts from 1 */


void
tdc1190ScanCLAS(int options, int *nboards, unsigned int *tdcadr)
{
  volatile TDC890 *tdc890;
  int ii, res, rdata, errFlag = 0;
  int iboards, boardID = 0;
  unsigned int offset;

  /* unpack options */

  cblt_not = (options>>24)&0xFF;
  a24_a32 = (options>>16)&0xFF;
  sngl_blt_mblt = (options>>8)&0xFF;
  berr_fifo = (options)&0xFF;

  if(cblt_not==0)
  {
    printf("  board-by-board readout\n");
  }
  else if(cblt_not==0x01)
  {
    printf("  chained readout\n");
  }
  else
  {
    printf("  unknown cblt_not mode, use board-by-board readout\n");
    cblt_not = 0x0;
  }

  if(a24_a32==0x01)
  {
    printf("  A24 addressing mode\n");
  }
  else if(a24_a32==0x02)
  {
    printf("  A32 addressing mode\n");
  }
  else
  {
    printf("  unknown addressing mode, use A24 addressing mode\n");
    a24_a32 = 0x01;
  }

  if(sngl_blt_mblt==0x01)
  {
    printf("  D32 single word readout\n");
  }
  else if(sngl_blt_mblt==0x02)
  {
    printf("  D32 DMA (BLT) readout\n");
  }
  else if(sngl_blt_mblt==0x03)
  {
    printf("  D64 DMA (MBLT) readout\n");
  }
  else if(sngl_blt_mblt==0x04)
  {
    printf("  D64 DMA (2eVME) readout\n");
  }
  else if(sngl_blt_mblt==0x05)
  {
    printf("  D64 DMA (2eSST) readout\n");
  }
  else
  {
    printf("  unknown readout mode, use D32 single word readout\n");
    sngl_blt_mblt = 0x01;
  }

  if(berr_fifo==0x00)
  {
    printf("  Nwords method: use VME BUS error\n");
  }
  else if(berr_fifo==0x01)
  {
    printf("  Nwords method: use event fifo\n");
  }
  else
  {
    printf("  unknown Nwords method, use VME BUS error\n");
    berr_fifo = 0x00;
  }


  /* get offset: use address modifier 0x09 for A32, 0x39 for A24 and anything else */
  if(a24_a32==0x02)
  {
    sysBusToLocalAdrs(0x09,V890_A32_ADDR,&offset);
  }
  else
  {
    sysBusToLocalAdrs(0x39,0,&offset);
  }
  printf("tdc1190ScanCLAS: offset = 0x%08x\n",offset);


  iboards = 0;
  for(ii=0; ii<(NBOARDS-1); ii++)
  {
    /* Check if Board exists at that address */
    tdc890 = (TDC890 *) (offset + TDCADR + ii*TDCSTEP);
    /*res = vxMemProbe((char *) tdc890,0,4,(char *)&rdata);
    if(res < 0)
    {
      printf("tdc1190ScanCLAS: No addressable board at addr=0x%x\n",
             (UINT32) tdc890);
      *nboards = iboards;
      return(OK);
    }
    else*/
    {
      boardID = tdc890->firmwareRev; 
      printf("tdc1190ScanCLAS: found board at addr=0x%x (offset=0x%08x) rev 0x%02x\n",
             (UINT32) tdc890, offset, boardID);
      /* Check if this is a Model V890 */
      if((boardID != V890_BOARD_ID)&&(boardID != (V890_BOARD_ID+1)))
      {
        printf("tdc1190ScanCLAS: Firmware does not match: 0x%02x (expected 0x%02x or 0x%02x)\n",
          boardID,V890_BOARD_ID,(V890_BOARD_ID+1));
        *nboards = iboards;
        return;
      }
    }
    iboards ++;
    tdcadr[ii] = (unsigned int *) tdc890;
    printf("tdc1190ScanCLAS: init v890 TDC number %d at address 0x%08x\n",
      iboards,tdcadr[ii]);
  }

  return;
}


/* set global variables which will be used to program all boards in crate */

void
tdc1190SetGlobalWindow(int width, int offset)
{
  window_width = width;
  window_offset = offset;

  return;
}

void
tdc1190InitCLAS(int nboards, unsigned int *tdcadr,
                unsigned int *tdcbaseadr)
{
  int ii, ifull;
  unsigned short value, value0, value1, array0[32], array1[32], data16;
  unsigned short dat16[21][10];
  short tmp[5], tdata;
  unsigned int ww, wm, wr;
  int wo;

  printf("start v1190/v1290 initialization\n");

  /* general initialization */
  for(ii=0; ii<nboards; ii++) tdc890Init(tdcadr[ii],0,1,0);
  for(ii=0; ii<nboards; ii++) tdc890Clear(tdcadr[ii]);
  for(ii=0; ii<nboards; ii++) tdc890Reset(tdcadr[ii]);

  /* Program TDC for trigger matching mode */
  for(ii=0; ii<nboards; ii++)
  {
    tdc890SetTriggerMatchingMode(tdcadr[ii]);
  }
  /*tdc890ReadAcquisitionMode(tdcadr[ii],&data16);*/
  for(ii=0; ii<nboards; ii++)
  {
    tdc890WriteMicro(tdcadr[ii],0x0200);
  }
  for(ii=0; ii<nboards; ii++)
  {
    tdc890ReadMicro(tdcadr[ii],&value,1);
    value &= 0x1;
    if(value==0)
      printf("Acquisition Mode: Continuous Storage\n");
    else
      printf("Acquisition Mode: Trigger Matching\n");
  }





  /* Set Trigger Window Width (ns) */
  ww = window_width;
  for(ii=0; ii<nboards; ii++)
  {
    /*tdc890SetWindowWidth(tdcadr[ii], ww);*/
    tdc890WriteMicro(tdcadr[ii],0x1000);
  }
  for(ii=0; ii<nboards; ii++)
  {
    if(ww < 25)         value = 1;
    else if(ww > 51175) value = 0x7FF;
    else                value = ww/25;
    printf("Set Window Width to %d ns\n",value*25);
    tdc890WriteMicro(tdcadr[ii],value);
  }





  /* Set Trigger Window Offset (ns) */
  wo = window_offset;;
  for(ii=0; ii<nboards; ii++)
  {
    /*tdc890SetWindowOffset(tdcadr[ii], wo);*/
    tdc890WriteMicro(tdcadr[ii],0x1100);
  }
  for(ii=0; ii<nboards; ii++)
  {
    if(wo < -800000) tdata = -32000;
    else             tdata = wo/25;
    printf("Set Window Offset to %d ns\n",tdata*25);
    tdc890WriteMicro(tdcadr[ii],tdata);
  }



  /* extra search margin (after window) (ns) */
  wm = 25;
  for(ii=0; ii<nboards; ii++)
  {
    /*tdc890SetExtraSearchMargin(tdcadr[ii], wm);*/
    tdc890WriteMicro(tdcadr[ii],0x1200);
  }
  for(ii=0; ii<nboards; ii++)
  {
    if(wm < 0)         value = 0;
    else if(wm > 1250) value = 50;
    else               value = wm/25;
    printf("Set Extra Search Margin to %d ns\n",value*25);
    tdc890WriteMicro(tdcadr[ii],value);
  }



  /* reject margin (before window) (ns) */
  wr = 50;
  for(ii=0; ii<nboards; ii++)
  {
    /*tdc890SetRejectMargin(tdcadr[ii], wr);*/
    tdc890WriteMicro(tdcadr[ii],0x1300);
  }
  for(ii=0; ii<nboards; ii++)
  {
    if(wr < 0)         value = 0;
    else if(wr > 1250) value = 50;
    else               value = wr/25;
    printf("Set Reject Margin to %d ns\n",value*25);
    tdc890WriteMicro(tdcadr[ii],value);
  }



  /* Enable subtraction of trigger time */
  for(ii=0; ii<nboards; ii++)
  {
    tdc890EnableTriggerTimeSubtruction(tdcadr[ii]);
  }




  /*tdc890ReadTriggerConfiguration(tdcadr[ii]);*/
  for(ii=0; ii<nboards; ii++)
  {
    tdc890WriteMicro(tdcadr[ii],0x1600);
  }
  for(ii=0; ii<nboards; ii++)
  {
    /*tdc890ReadMicro(tdcadr[ii],tmp,5);*/
    tdc890ReadMicro(tdcadr[ii],&tmp[0],1);
    tdc890ReadMicro(tdcadr[ii],&tmp[1],1);
    tdc890ReadMicro(tdcadr[ii],&tmp[2],1);
    tdc890ReadMicro(tdcadr[ii],&tmp[3],1);
    tdc890ReadMicro(tdcadr[ii],&tmp[4],1);
    printf("  Window Width              = %6d ns\n",tmp[0]*25);
    printf("  Window Offset             = %6d ns\n",tmp[1]*25);
    printf("  Extra Seach Margin        = %6d ns\n",tmp[2]*25);
    printf("  Reject Margin             = %6d ns\n",tmp[3]*25);
    printf("  Trigger Time Subtruction = %6d\n",tmp[4]);
  }




  /* edge detection: */
  /* 0-pair, 1-trailing, 2-leading, 3-both leading and trailing */
  value1=2;
  for(ii=0; ii<nboards; ii++)
  {
    /*tdc890SetEdgeDetectionConfig(tdcadr[ii], value1);*/
    value = value1 & 0x3;
    printf("SetEdgeDetectionConfig: ");
    if(value==0x0)
      printf("set pair mode\n");
    else if(value==0x1)
      printf("set trailing edge only\n");
    else if(value==0x2)
      printf("set leading edge only\n");
    else
      printf("set both leading and trailing edges\n");
    tdc890WriteMicro(tdcadr[ii],0x2200);
  }
  for(ii=0; ii<nboards; ii++)
  {
    tdc890WriteMicro(tdcadr[ii],value);
  }




    /* get edge detection configuration */
  for(ii=0; ii<nboards; ii++)
  {
    /*tdc890ReadEdgeDetectionConfig(tdcadr[ii],&data16);*/
    tdc890WriteMicro(tdcadr[ii],0x2300);
  }
  for(ii=0; ii<nboards; ii++)
  {
    tdc890ReadMicro(tdcadr[ii],&value,1);
    value &= 0x3;
    printf("ReadEdgeDetectionConfig: value=%d (0x%x)\n",value,value);
    if(value==0)
      printf("ReadEdgeDetectionConfig: paired measurement\n");
    else if(value==1)
      printf("ReadEdgeDetectionConfig: trailing edge\n");
    else if(value==2)
      printf("ReadEdgeDetectionConfig: leading edge\n");
    else if(value==3)
      printf("ReadEdgeDetectionConfig: both leading and trailing\n");
    else
    {
      printf("ReadEdgeDetectionConfig: ERROR, value=%d (0x%x)\n",value,value);
    }
  }



  /********************/
  /* readout settings */

  if(berr_fifo == 0x01)
  {
    /* enable event fifo */
    for(ii=0; ii<nboards; ii++)
    {
      tdc890EventFifo(tdcadr[ii],1);
    }
  }

  for(ii=0; ii<nboards; ii++)
  {
    ifull = tdc890StatusFull(tdcadr[ii]);
    printf("ifull=%d\n",ifull);

    printf("end v890\n");
  }


  /* if using event fifo, disable bus error on BLT finish */
  if(berr_fifo == 0x01)
  {
    printf("disable VME BUS error for event fifo readout\n");
    /*for(ii=0; ii<nboards; ii++)
    {
      tdc890BusError(tdcadr[ii],0);
	  }*/
  }
  else
  {
    printf("enable VME BUS error\n");
    for(ii=0; ii<nboards; ii++)
    {
      tdc890BusError(tdcadr[ii],1);
    }
  }

  /* enable 64-bit alignment */
  /* NOTE: 64-bit alignment must be enabled for any DMA readout,
  not only for MBLT(D64) but for BLT(D32) as well; some DMA engines
  (for example the one on mv5100) will cut off last 32-bit word if
  it is not even, for example if event size is equal to 137 D32 words,
  only 136 D32 words will be transfered */
  /* in addition universe library contains check for 64-bit alignment
	 in dmastart procedure, it will return error if ... */
  for(ii=0; ii<nboards; ii++)
  {
    tdc890Align64(tdcadr[ii],1);
  }

  /* set BLT Event Number Register */
  for(ii=0; ii<nboards; ii++)
  {
    tdata = 1;
    tdc890SetBLTEventNumber(tdcadr[ii], tdata);
    tdc890GetBLTEventNumber(tdcadr[ii], &tdata);
    printf("BLT Event Number set to %d\n",tdata);
  }


  /* set maximum number of hits per event */
  for(ii=0; ii<nboards; ii++)
  {
    tdata = 64;
    tdc890SetMaxNumberOfHitsPerEvent(tdcadr[ii], tdata);
    tdc890GetMaxNumberOfHitsPerEvent(tdcadr[ii], &tdata);
  }
  

  if(a24_a32==0x02 && cblt_not==0x01)
  {
    tdc890InitMCST(nboards, tdcadr, tdcbaseadr);
    printf("Chained readout: tdcbaseadr=0x%08x\n",*tdcbaseadr);
  }
  else
  {
    /* reset MCST flag in every board in case if it was set before */
    for(ii=0; ii<nboards; ii++)
    {
      tdc890ResetMCST(tdcadr[ii]);
    }
    *tdcbaseadr = 0;
    printf("Board-by-board readout: tdcbaseadr=0x%08x\n",*tdcbaseadr);
  }


  /*********************************************************/
  /* VME controller DMA initialization (universe or tempe) */
  usrVmeDmaInit();

  /* usrVmeDmaConfig params:
          first: 0 - A16,
                 1 - A24,
                 2 - A32
          second: 0 - D16 / SINGLE
                  1 - D32 / SINGLE
                  2 - D32 / BLOCK (BLT)  <- use this for DMA !
                  3 - D64 / BLOCK (MBLT) <- use this for DMA !

  */
  if(a24_a32==0x02) /* A32 */
  {
    usrVmeDmaConfig(2, sngl_blt_mblt);
  }
  else /* A24 */
  {
    usrVmeDmaConfig(1, sngl_blt_mblt);
  }



  /* set 'almost full' level and program output connector to signal on it */
  for(ii=0; ii<nboards; ii++)
  {
    tdc890SetAlmostFullLevel(tdcadr[ii], 16384/*8*//*32735*/); /* default is 64 words, max is 32735 words */
    tdc890GetAlmostFullLevel(tdcadr[ii],&tdata);

    tdc890SetOutProg(tdcadr[ii],2);/*0-data ready,1-full,2-almost full,3-err*/
    tdc890GetOutProg(tdcadr[ii],&tdata);
  }

  /*
  fflush(stdout);
  taskDelay(10);
  */

  return;
}






/* generic readout for v1190/v1290 TDC boards */
/* time profiling data for 2 boards 550 bytes event size */

int
tdc1190ReadEventCLAS(int nboards, UINT32 *tdcadr, INT32 *tdcbuf,
                     INT32 *rlenbuf)
{
  int jj, nev, itdcbuf, itmp1;
  int nn[21];
  unsigned short tdata;


/* whole routine: 42 usec */

/* part1: 6 usec */

  if(nboards==0)
  {
    logMsg("tdc1190ReadEventCLAS: ERROR: nboards=%d\n",nboards,2,3,4,5,6);
  }


  for(jj=0; jj<nboards; jj++)
  {
    /* check if board is full */
    if(tdc890StatusFull(tdcadr[jj]))
    {
      logMsg("ERROR: [%2d] board is full - clear (nboards=%d)\n",
        jj,nboards,3,4,5,6);
      /* clear board, otherwise cannot use it any more ! */
      tdc890Clear(tdcadr[jj]);
    }

    /* check the number of events */
    nn[jj] = nev = tdc890Dready(tdcadr[jj]);
    if(nev == 0)
    {
      logMsg("tdc1190ReadEventCLAS: [%2d] not ready !\n",jj,2,3,4,5,6);
    }

    /* Trigger Supervisor has 6 event buffer, but we can get 7
	   if 'parallel' readout is in use */
    if(nev > 7)      
    {
	  logMsg("tdc1190ReadEventCLAS: ERROR: [%2d] nev=%d\n",jj,nev,3,4,5,6);
	}
  }




  /* part2: 36 usec */

  /* readout */
  itdcbuf = 0;
  for(jj=0; jj<nboards; jj++)
  {

    if(sngl_blt_mblt == 0x01)
    {
      rlenbuf[jj] = tdc890ReadEvent(tdcadr[jj],&tdcbuf[itdcbuf]);
    }
    else
    {
	  /*	  
logMsg("tdc1190ReadEventCLAS 1: jj=%d itdcbuf=%d\n",jj,itdcbuf,3,4,5,6);
	  */
      /* 18usec x 2boards = 36Usec */
      rlenbuf[jj] = tdc890ReadEventDma(tdcadr[jj],&tdcbuf[itdcbuf]);
	  /*
logMsg("tdc1190ReadEventCLAS 2: rlen=%d\n",rlenbuf[jj],2,3,4,5,6);
	  */
    }

    if(rlenbuf[jj] <= 0)
    {
      logMsg("[%2d] ERROR: tdc890ReadEvent[Dma] returns %d\n",
        jj,rlenbuf[jj],3,4,5,6);
    }
    else
    {
	  /*
      logMsg("[%2d] INFO: tdc890ReadEvent[Dma] returns %d\n",
        jj,rlenbuf[jj],3,4,5,6);
	  */
      itdcbuf += rlenbuf[jj];
    }

  }

  return(OK);
}


/* use DMA list */
int
tdc1190ReadEventStart(int nboards, UINT32 *tdcadr, INT32 *tdcbuf,
                      INT32 *rlenbuf)
{
  volatile UINT32 *fifo;
  int fifodata;
  int jj, nev;
  int itdcbuf;
  static int *destination[NBOARDS];
  static int tdcadr1[NBOARDS];
  /*
TIMER_VAR;
  */
  if(nboards<2)
  {
    logMsg("tdc1190ReadEventStart: ERROR: nboards=%d\n",nboards,2,3,4,5,6);
  }

  for(jj=0; jj<nboards; jj++)
  {
    /* check if board is full */
    if(tdc890StatusFull(tdcadr[jj]))
    {
      logMsg("ERROR: [%2d] board is full - clear (nboards=%d)\n",
        jj,nboards,3,4,5,6);
      /* clear board, otherwise cannot use it any more ! */
      tdc890Clear(tdcadr[jj]);
    }

    /* check the number of events */
    nev = tdc890Dready(tdcadr[jj]);
    if(nev == 0)
    {
      logMsg("tdc1190ReadEventStart: [%2d] not ready !\n",jj,2,3,4,5,6);
    }

    /* Trigger Supervisor has 6 event buffer, but we can get 7
	   if 'parallel' readout is in use */
    if(nev > 7)
    {
	  logMsg("tdc1190ReadEventStart: ERROR: [%2d] nev=%d\n",jj,nev,3,4,5,6);
	}
  }


  if(sngl_blt_mblt == 0x01)
  {
    logMsg("ERROR: DMA method must be used: sngl_blt_mblt=%d\n",
        sngl_blt_mblt,2,3,4,5,6);
    return(ERROR);
  }


  /* readout settings */
  if(berr_fifo == 0x01) /* use FIFO reaout */
  {
    itdcbuf = 0;
    for(jj=0; jj<nboards; jj++)
    {
      fifo = (UINT32 *) (tdcadr[jj]+0x1038);

      /* get event length in words */
      fifodata = *fifo;
      ndata_save[jj] = fifodata & 0xffff;

      if(sngl_blt_mblt >= 0x04) /* 128 bit alignment */
	  {
        extra_save[jj] = (4-(ndata_save[jj]%4));
        if(extra_save[jj]==4) extra_save[jj]=0;
	  }
	  else /* 64 bit alignment */
	  {
        if( (ndata_save[jj]%2) != 0 ) extra_save[jj] = 1;
        else                          extra_save[jj] = 0;
	  }

      size_save[jj] = (ndata_save[jj]+extra_save[jj])<<2;
      rlenbuf[jj] = ndata_save[jj]+extra_save[jj];

      destination[jj] = &tdcbuf[itdcbuf];

      itdcbuf += rlenbuf[jj];

	/*
logMsg("[%d] ask=%d (%d bytes), got=%d (0x%08x to 0x%08x)\n",
 jj,ndata_save[jj]+extra_save[jj],size_save[jj],rlenbuf[jj],tdcadr[jj],destination[jj]);
	*/

    }
  }
  else /* use BERR readout */
  {
    itdcbuf = 0;
    for(jj=0; jj<nboards; jj++)
    {
      ndata_save[jj] = V890_MAX_WORDS_PER_BOARD;
      extra_save[jj] = 0;

      size_save[jj] = (ndata_save[jj])<<2;
      rlenbuf[jj] = ndata_save[jj];

      destination[jj] = &tdcbuf[itdcbuf];

      itdcbuf += rlenbuf[jj];
	} 
  }



  /*
{
  TIMER_NORMALIZE;
  TIMER_START;
}
*/

  /* start DMA list */
	
  usrVme2MemDmaListSet(tdcadr, destination, size_save, nboards);
	
  /* start DMA list */
  usrVmeDmaListStart();

  /*
{
  TIMER_STOP(100000,-1);
}
*/
  return(OK);
}

int
tdc1190ReadEventDone()
{
  int res;

  
  res = usrVme2MemDmaDone();
  

  /* for BERR need something like following:
  if(berr_fifo == 0x0)
  {
mdata = ndata_save[ib] - (res>>2);
  }
  */

  return(OK);
}













STATUS
CLAStdc890ReadEventCBLT(int nboards, UINT32 tdcbaseadr, UINT32 *tdcadr, 
                        INT32 *tdcbuf, INT32 *rlenbuf)
{
  int rlen, jj, nev, res;
  int ndata;
  UINT32 mdata;

  if(nboards==0)
  {
    logMsg("CLAStdc890ReadEventCBLT: ERROR: nboards=%d\n",nboards,2,3,4,5,6);
  }

  /*****************************/
  /* check if boards are ready */
  for(jj=0; jj<nboards; jj++)
  {
    if(tdc890StatusFull(tdcadr[jj]))
    {
      logMsg("[%2d] board is full - clear\n",jj,2,3,4,5,6);
      /* clear board, otherwise cannot use it any more ! */
      tdc890Clear(tdcadr[jj]);
    }
    else if((nev = tdc890Dready(tdcadr[jj])) > 0)
    {
	  
      /* Trigger Supervisor has 6 event buffer, but we can get 7
	     if 'parallel' readout is in use */
      if(nev > 7)
      {
        logMsg("[%2d] ERROR: nev=%d\n",jj,nev,3,4,5,6);
      }

    }
    else
    {
      logMsg("[%2d] next time ..\n",jj,2,3,4,5,6);
    }

  }



  /* should use following (?)
  rlenbuf[0] = tdc890ReadEventDma(tdcbaseadr, tdcbuf);
  */





  ndata = V890_MAX_WORDS_PER_BOARD;
  mdata = 0;
  res = usrVme2MemDmaStart((UINT32 *)tdcbaseadr,(UINT32 *)tdcbuf,(ndata<<2));
  if(res < 0)
  {
    logMsg("CLAStdc890ReadEventCBLT: ERROR: usrVme2MemDmaStart returned %d\n",
           res,0,0,0,0,0);
    return(0);
  }


  if((res = usrVme2MemDmaDone()) < 0)
  {
    logMsg("CLAStdc890ReadEventCBLT: ERROR: usrVme2MemDmaDone returned = %d\n",
      res,2,3,4,5,6);
    return(0);
  }
  else
  {
    mdata = ndata - (res>>2);
  }
  rlenbuf[0] = mdata;



  return(OK);
}




/* test !!!!!!!!!!!! */
STATUS
CLAStdc890ReadEventCBLTStart(int nboards, UINT32 tdcbaseadr, UINT32 *tdcadr, 
                        INT32 *tdcbuf, INT32 *rlenbuf)
{
  int rlen, jj, nev, res;
  int ndata;
  UINT32 mdata;

  if(nboards==0)
  {
    logMsg("CLAStdc890ReadEventCBLT: ERROR: nboards=%d\n",nboards,2,3,4,5,6);
  }

  /*****************************/
  /* check if boards are ready */
  for(jj=0; jj<nboards; jj++)
  {
    if(tdc890StatusFull(tdcadr[jj]))
    {
      logMsg("[%2d] board is full - clear\n",jj,2,3,4,5,6);
      /* clear board, otherwise cannot use it any more ! */
      tdc890Clear(tdcadr[jj]);
    }
    else if((nev = tdc890Dready(tdcadr[jj])) > 0)
    {
	  
      /* Trigger Supervisor has 6 event buffer, but we can get 7
	     if 'parallel' readout is in use */
      if(nev > 7)
      {
        logMsg("[%2d] ERROR: nev=%d\n",jj,nev,3,4,5,6);
      }

    }
    else
    {
      logMsg("[%2d] next time ..\n",jj,2,3,4,5,6);
    }

  }


  ndata = V890_MAX_WORDS_PER_BOARD;
  mdata = 0;


/*workaround
tdcbaseadr=0x81000000;
*/

  res = usrVme2MemDmaStart((UINT32 *)tdcbaseadr,(UINT32 *)tdcbuf,(ndata<<2));
  if(res < 0)
  {
    logMsg("CLAStdc890ReadEventCBLT: ERROR: usrVme2MemDmaStart returned %d\n",
           res,0,0,0,0,0);
    return(0);
  }

  return(OK);
}

STATUS
CLAStdc890ReadEventCBLTStop(INT32 *rlenbuf)
{
  int res;
  int ndata;
  UINT32 mdata;

  if((res = usrVme2MemDmaDone()) < 0)
  {
    logMsg("CLAStdc890ReadEventCBLT: ERROR: usrVme2MemDmaDone returned = %d\n",
      res,2,3,4,5,6);
    return(0);
  }
  else
  {
    ndata = V890_MAX_WORDS_PER_BOARD;
    mdata = ndata - (res>>2);
  }
  rlenbuf[0] = mdata;

  return(OK);
}





















/***********************/
/* SOME TEST FUNCTIONS */
/***********************/





STATUS
tdc890Enable1(UINT32 addr)
{
  printf("Enable ...\n");
  tdc890WriteMicro(addr,0x2100);
  taskDelay(100);
  printf("... done.\n");
  return(OK);
}

STATUS
tdc890ReadMicro1(UINT32 addr, UINT16 *data, int nwords)
{
  volatile TDC890 *tdc890 = (TDC890 *) addr;
  int ii, jj, kk=0;
  UINT16 mstatus;

  taskDelay(100);

retry:

  taskDelay(100);
  mstatus = (tdc890->microHandshake) & V890_MICRO_READOK;
printf("--> ReadMicro1: mstatus=%d (0x%x) data=0x%x\n",
mstatus,tdc890->microHandshake,tdc890->microReg);
  if(mstatus)
  {
    for(ii=0; ii<nwords; ii++)
    {
      jj=0;
      while(!((tdc890->microHandshake) & V890_MICRO_READOK))
      {
        jj++;
        if(jj>1000000)
        {
          logMsg("ReadMicro1: ERROR1: Read Status not OK (read %d)\n",
            ii,0,0,0,0,0);
          return(ERROR);
        }
      }
      data[ii] = tdc890->microReg;
    }
  }
  else
  {
    kk++;
    if(kk>=1000000)
    {
      logMsg("ReadMicro1: ERROR2: Read Status not OK (%d, 0x%x)\n",
        mstatus,mstatus,3,4,5,6);
      return(ERROR);
    }
    else
    {
      goto retry;
    }
  }

  if(kk > 0) printf("kk1(read)=%d\n",kk);
  return(OK);
}


STATUS
tdc890ReadEdge1(UINT32 addr, UINT16 *data)
{
  UINT16 tdata;

  tdc890WriteMicro(addr,0x2300);
  tdc890ReadMicro1(addr,&tdata,1);

  *data = 0;
  tdata &= 0x3;
  printf("Edge: tdata=%d (0x%x)\n",tdata,tdata);
  if(tdata==0)      printf("Edge: meaningless data\n");
  else if(tdata==1) printf("Edge: trailing edge\n");
  else if(tdata==2) printf("Edge: leading edge\n");
  else if(tdata==3) printf(": paired measurement\n");
  else
  {
    printf("Edge: ERROR, tdata=%d (0x%x)\n",tdata,tdata);
    return(ERROR);
  }
  *data = tdata;

  return(OK);
}


#define TDCADDRESS 0x210000

void
tdc890_testopcode()
{
  unsigned int tdcadr1;
  int status;
  unsigned short data16;
  unsigned int data321, data322;


  sysBusToLocalAdrs(0x39,TDCADDRESS,&tdcadr1);
  printf("tdcadr1=0x%08x\n",tdcadr1);


  /* Init */
  tdc890Init(tdcadr1,0,1,0);
  tdc890Reset(tdcadr1);
  tdc890Clear(tdcadr1);

printf("\n\n ..... edge test .............\n\n");

/*
  tdc890SetPairResolution(tdcadr1, 100, 400);
  tdc890GetPairResolution(tdcadr1, &data321, &data322);
*/

  tdc890Enable1(tdcadr1);
  taskDelay(100);
  tdc890ReadEdge1(tdcadr1,&data16);
  taskDelay(100);








return;




  /* Program TDC for trigger matching mode */
  tdc890SetTriggerMatchingMode(tdcadr1);
  tdc890ReadAcquisitionMode(tdcadr1,&data16);

  /* Set Trigger Window Width (ns) */
  tdc890SetWindowWidth(tdcadr1, 1000);

  /* Set Trigger Window Offset (ns) */
  tdc890SetWindowOffset(tdcadr1, -1025);

  /* (ns) */
  tdc890SetExtraSearchMargin(tdcadr1, 25);

  /* (ns) */
  tdc890SetRejectMargin(tdcadr1, 50);

  /* Enable subtraction of trigger time */
  tdc890EnableTriggerTimeSubtruction(tdcadr1);

  /* Enable leading edge */
  tdc890SetEdgeDetectionConfig(tdcadr1, 2);

  /* get trigger configuration */
  tdc890ReadTriggerConfiguration(tdcadr1);

  /* get edge detection configuration */
  tdc890ReadEdgeDetectionConfig(tdcadr1,&data16);

  /* set dead time (double peak resolution) */
  tdc890SetDoubleHitResolution(tdcadr1, 10);
  tdc890GetDoubleHitResolution(tdcadr1, &data16);

}



void
tdc890test2()
{
  unsigned int tdcadr1;
  unsigned short data16;

  sysBusToLocalAdrs(0x39,TDCADDRESS,&tdcadr1);
  printf("tdcadr1=0x%08x\n",tdcadr1);

  tdc890ReadEdgeDetectionConfig(tdcadr1,&data16);
}



void
tdc890testopcodes()
{
  unsigned short data16;
  unsigned int data32, data32_1, data32_2;
  unsigned int tdcadr1;

  sysBusToLocalAdrs(0x39,TDCADDRESS,&tdcadr1);
  printf("tdcadr1=0x%08x\n",tdcadr1);

  printf("start v890\n");

  tdc890Init(tdcadr1,0,1,0);
  tdc890Clear(tdcadr1);
  tdc890Reset(tdcadr1);

  /* Program TDC for continuous storage mode */
  tdc890SetContinuousStorageMode(tdcadr1);
  tdc890ReadAcquisitionMode(tdcadr1,&data16);

  /* Program TDC for trigger matching mode */
  tdc890SetTriggerMatchingMode(tdcadr1);
  tdc890ReadAcquisitionMode(tdcadr1,&data16);

  /* Set Trigger Window Width (ns) (step 25ns) */
  tdc890SetWindowWidth(tdcadr1, 1000);

  /* Set Trigger Window Offset (ns) (step 25ns) */
  tdc890SetWindowOffset(tdcadr1, -900);

  /* (ns) (step 25ns) */
  /*tdc890SetExtraSearchMargin(tdcadr1, 225);*/

  /* (ns) (step 25ns) */
  /*tdc890SetRejectMargin(tdcadr1, 125);*/

  /* Enable subtraction of trigger time */
  tdc890EnableTriggerTimeSubtruction(tdcadr1);
  /*tdc890DisableTriggerTimeSubtruction(tdcadr1);*/

  /* get trigger configuration */
  tdc890ReadTriggerConfiguration(tdcadr1);

  /* edge detection */
  tdc890SetEdgeDetectionConfig(tdcadr1, 0);
  tdc890ReadEdgeDetectionConfig(tdcadr1,&data16);
  tdc890SetEdgeDetectionConfig(tdcadr1, 1);
  tdc890ReadEdgeDetectionConfig(tdcadr1,&data16);
  tdc890SetEdgeDetectionConfig(tdcadr1, 2);
  tdc890ReadEdgeDetectionConfig(tdcadr1,&data16);
  tdc890SetEdgeDetectionConfig(tdcadr1, 3);
  tdc890ReadEdgeDetectionConfig(tdcadr1,&data16);


  /* edge resolution */
  tdc890SetEdgeResolution(tdcadr1, 800);
  tdc890GetEdgeResolution(tdcadr1, &data16);
  tdc890SetEdgeResolution(tdcadr1, 200);
  tdc890GetEdgeResolution(tdcadr1, &data16);
  tdc890SetEdgeResolution(tdcadr1, 100);
  tdc890GetEdgeResolution(tdcadr1, &data16);


  /* pair resolution */
  tdc890SetPairResolution(tdcadr1, 200, 800);
  tdc890GetPairResolution(tdcadr1, &data32_1, &data32_2);


  /* double hit resolution */
  tdc890SetDoubleHitResolution(tdcadr1, 100);
  tdc890GetDoubleHitResolution(tdcadr1, &data16);
  tdc890SetDoubleHitResolution(tdcadr1, 30);
  tdc890GetDoubleHitResolution(tdcadr1, &data16);
  tdc890SetDoubleHitResolution(tdcadr1, 10);
  tdc890GetDoubleHitResolution(tdcadr1, &data16);
  tdc890SetDoubleHitResolution(tdcadr1, 5);
  tdc890GetDoubleHitResolution(tdcadr1, &data16);

  printf("end v890\n");


}

#else /* no UNIX version */

void
tdc890_dummy()
{
  return;
}

#endif
