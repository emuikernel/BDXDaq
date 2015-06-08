/*****************************************************************************
 *
 *  caen1190Lib.c - Driver library for readout of C.A.E.N. Model 1190
 *                  TDC using a VxWorks 5.2 or later based Single Board computer.
 *
 *  Author: David Abbott 
 *          Jefferson Lab Data Acquisition Group
 *          August 2002
 *
 *  Revision  1.0 - Initial Revision
 *                    - Supports up to 20 CAEN Model 1190s in a Crate
 *                    - Programmed I/O reads
 *                    - Interrupts from a Single 1190
 *
 *  adjusted for CLAS by Sergey Boyarinov - May 2003-present
 *
 *  Ported to Linux by Bryan Moffit - Jan 2010
 *
 *    - Linked List DMA supported
 *       vxworks: requires modified tsi148 DMA library 
 *
 * SVN: $Rev: 416 $
 *
 */


#include <stdio.h>
#include <string.h>
#include <pthread.h>
#ifdef VXWORKS
#include <vxWorks.h>
#include <logLib.h>
#include <taskLib.h>
#include <intLib.h>
#include <iv.h>
#include <vxLib.h>
#else
#include "jvme.h"
#endif

#include "c1190Lib.h"

/* Register Read/Write routines */
static unsigned short tdcRead16(volatile unsigned short *addr);
static void tdcWrite16(volatile unsigned short *addr, unsigned short val);
static unsigned int tdcRead32(volatile unsigned int *addr);
static void tdcWrite32(volatile unsigned int *addr, unsigned int val);

/* Prototype of Common Initialization of Modules */
STATUS tdc1190CommonInit(int itdc, UINT32 laddr);

/* Macro to check id and c1190p */
#define CHECKID(id) {							\
    if((id<0) || (c1190p[id] == NULL)) {				\
      logMsg("%s: ERROR : TDC id %d not initialized \n",		\
	     (int)__FUNCTION__,id,3,4,5,6);				\
      return ERROR;							\
    }									\
    if(use1190[id]==0)							\
      {									\
	logMsg("%s: TDC id %d flagged to NOT be used.\n",		\
	       (int)__FUNCTION__,id,3,4,5,6);				\
	return ERROR;							\
      }									\
  }

#ifdef VXWORKS
/* Define external Functions */
IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);
IMPORT  STATUS intDisconnect(int);
IMPORT  STATUS sysIntEnable(int);
IMPORT  STATUS sysIntDisable(int);
#endif

#ifdef VXWORKS
#define EIEIO    __asm__ volatile ("eieio")
#define SynC     __asm__ volatile ("sync")
#else
#define EIEIO    
#define SynC     
#endif

/* Mutex to hold of reads and writes from competing threads */
pthread_mutex_t c1190_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_1190 {				\
    if(pthread_mutex_lock(&c1190_mutex)<0)	\
      perror("pthread_mutex_lock");	\
  }
#define UNLOCK_1190 {				\
    if(pthread_mutex_unlock(&c1190_mutex)<0)	\
      perror("pthread_mutex_unlock");	\
  }

/* Define global variables */
unsigned int tdcAddrOffset = 0;                /* Difference in CPU (USERSPACE) Base */
int Nc1190 = 0;                              /* Number of TDCs in crate */
volatile struct v1190_struct *c1190p[V1190_MAX_MODULES];    /* pointers to TDC memory map */
int use1190[V1190_MAX_MODULES] = {           /* Switch to turn on (1) or off (0) specific TDCs */
  0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};

volatile struct v1190_struct *c1190MCSTp;    /* pointer to TDC MCST memory map */
int tdcMaxSlot=-1;                           /* Highest element in use1190 hold an TDC */
int tdcMinSlot=-1;                           /* Lowest element in use1190 holding an TDC */

/* Define Interrupts variables */
BOOL              c1190IntRunning = FALSE; /* running flag */
int               c1190IntID      = -1; /* id number of TDC generating interrupts */
LOCAL VOIDFUNCPTR c1190IntRoutine = NULL; /* user interrupt service routine */
LOCAL int         c1190IntArg     = 0; /* arg to user routine */
LOCAL int         c1190IntEvCount = 0; /* Number of Events to generate Interrupt */
LOCAL UINT32      c1190IntLevel   = V1190_VME_INT_LEVEL; /* default VME interrupt level */
LOCAL UINT32      c1190IntVec     = V1190_INT_VEC; /* default interrupt Vector */
int               c1190IntCount   = 0; /* Count of interrupts from TDC */



/*******************************************************************************
 *
 * tdc1190Init - Initialize tdc1190 Library. 
 *
 *
 * RETURNS: OK, or ERROR if the address is invalid or board is not present.
 */

STATUS 
tdc1190Init(UINT32 addr, UINT32 addr_inc, int ntdc, int iFlag)
{
  int ii, jj, res=0, errFlag = 0;
  unsigned short rdata=0;
  int boardID = 0;
  unsigned int laddr;
  volatile struct v1190_ROM_struct *rp;

  /* Check for valid address */
  if(addr==0) 
    {
      printf("tdc1190Init: ERROR: Must specify a Bus (VME-based A32/A24) address for TDC 0\n");
      return(ERROR);
    }
  else if(addr < 0x00ffffff) 
    {  /* A24 Addressing */
      if((addr_inc==0)||(ntdc==0))
	ntdc = 1; /* assume only one TDC to initialize */

      /* get the TDCs address */
#ifdef VXWORKS
      res = sysBusToLocalAdrs(0x39,(char *)addr,(char **)&laddr);
      if (res != 0) 
	{
	  printf("tdc1190Init: ERROR in sysBusToLocalAdrs(0x39,0x%x,&laddr) \n",addr);
	  return(ERROR);
	}
      tdcAddrOffset = (unsigned int)laddr-(unsigned int)addr;
#else
      res = vmeBusToLocalAdrs(0x39,(char *)addr,(char **)&laddr);
      if (res != 0) 
	{
	  printf("tdc1190Init: ERROR in vmeBusToLocalAdrs(0x39,0x%x,&laddr) \n",addr);
	  return(ERROR);
	}
      tdcAddrOffset = (unsigned int)laddr-(unsigned int)addr;
#endif
    } /* A24 Addressing */
  else
    { /* A32 Addressing */

      if((addr_inc==0)||(ntdc==0))
	ntdc = 1; /* assume only one TDC to initialize */

      /* get the TDC address */
#ifdef VXWORKS
      res = sysBusToLocalAdrs(0x09,(char *)addr,(char **)&laddr);
      if (res != 0) 
	{
	  printf("tdc1190Init: ERROR in sysBusToLocalAdrs(0x09,0x%x,&laddr) \n",addr);
	  return(ERROR);
	}
      tdcAddrOffset = (unsigned int)laddr-(unsigned int)addr;
#else
      res = vmeBusToLocalAdrs(0x09,(char *)addr,(char **)&laddr);
      if (res != 0) 
	{
	  printf("tdc1190Init: ERROR in vmeBusToLocalAdrs(0x09,0x%x,&laddr) \n",addr);
	  return(ERROR);
	}
      tdcAddrOffset = (unsigned int)laddr-(unsigned int)addr;
#endif
    } /* A32 Addressing */

  Nc1190 = 0;
  for (ii=0;ii<ntdc;ii++) 
    {
      tdc1190CommonInit(ii,laddr + ii*addr_inc);
    }
  
  /* Disable/Clear all TDCs */
  for (ii=0;ii<ntdc;ii++) 
    {
      if(!use1190[ii]) continue;
      
      /* Determine the first TDC for MCST or CBLT */
      if (tdcMinSlot==-1) 
	tdcMinSlot = ii;

      /* Determine the last TDC for MCST or CBLT */
      tdcMaxSlot = ii;

      tdcWrite16(&(c1190p[ii]->moduleReset),1);
      tdcWrite16(&(c1190p[ii]->clear),1);

    }      
  }


/*******************************************************************************
 *
 * tdc1190InitList - Initialize tdc1190 Library for a "list" of modules
 *                   All modules must follow the same addressing scheme 
 *                   (A24 or A32).  The first module, in the list, must
 *                   be valid (addressable).
 *
 * RETURNS: OK, or ERROR if the address is invalid or board is not present.
 */

STATUS 
tdc1190InitList(UINT32 *addr_list, int ntdc, int iFlag)
{
  int ii, jj, res=0, errFlag = 0;
  unsigned short rdata=0;
  int boardID = 0;
  UINT32 addr;
  unsigned int laddr;
  volatile struct v1190_ROM_struct *rp;

  if(addr_list == NULL)
    {
      printf("tdc1190InitList: ERROR: Invalid addr_list\n");
      return(ERROR);
    }

  if(ntdc<1 || ntdc>V1190_MAX_MODULES)
    {
      printf("tdc1190InitList: ERROR: Invalid ntdc specified %d\n",ntdc);
      return(ERROR);
    }

  /* Start with the first address in the list, determine A24/A32 */
  addr = addr_list[0];

  /* Check for valid address */
  if(addr==0) 
    {
      printf("tdc1190InitList: ERROR: Must specify a Bus (VME-based A32/A24) address for TDC 0\n");
      return(ERROR);
    }
  else if(addr < 0x00ffffff) 
    {  /* A24 Addressing */
      /* get the TDCs address */
#ifdef VXWORKS
      res = sysBusToLocalAdrs(0x39,(char *)addr,(char **)&laddr);
      if (res != 0) 
	{
	  printf("tdc1190InitList: ERROR in sysBusToLocalAdrs(0x39,0x%x,&laddr) \n",addr);
	  return(ERROR);
	}
      tdcAddrOffset = (unsigned int)laddr-(unsigned int)addr;
#else
      res = vmeBusToLocalAdrs(0x39,(char *)addr,(char **)&laddr);
      if (res != 0) 
	{
	  printf("tdc1190InitList: ERROR in vmeBusToLocalAdrs(0x39,0x%x,&laddr) \n",addr);
	  return(ERROR);
	}
      tdcAddrOffset = (unsigned int)laddr-(unsigned int)addr;
#endif
    } /* A24 Addressing */
  else
    { /* A32 Addressing */
      /* get the TDC address */
#ifdef VXWORKS
      res = sysBusToLocalAdrs(0x09,(char *)addr,(char **)&laddr);
      if (res != 0) 
	{
	  printf("tdc1190InitList: ERROR in sysBusToLocalAdrs(0x09,0x%x,&laddr) \n",addr);
	  return(ERROR);
	}
      tdcAddrOffset = (unsigned int)laddr-(unsigned int)addr;
#else
      res = vmeBusToLocalAdrs(0x09,(char *)addr,(char **)&laddr);
      if (res != 0) 
	{
	  printf("tdc1190InitList: ERROR in sysBusToLocalAdrs(0x09,0x%x,&laddr) \n",addr);
	  return(ERROR);
	}
      tdcAddrOffset = (unsigned int)laddr-(unsigned int)addr;
#endif
    } /* A32 Addressing */

  Nc1190 = 0;

  /* Loop through the provided list checking for valid addresses */
  for (ii=0;ii<ntdc;ii++) 
    {
      laddr = addr_list[ii]+tdcAddrOffset;
      tdc1190CommonInit(ii,laddr);
    }

  /* Disable/Clear all TDCs */
  for (ii=0;ii<ntdc;ii++) 
    {
      if(!use1190[ii]) continue;
      
      /* Determine the first TDC for MCST or CBLT */
      if (tdcMinSlot==-1) 
	tdcMinSlot = ii;

      /* Determine the last TDC for MCST or CBLT */
      tdcMaxSlot = ii;

      tdcWrite16(&(c1190p[ii]->moduleReset),1); /* 'error' light here !!! */
      tdcWrite16(&(c1190p[ii]->clear),1);

    }      
}

/*******************************************************************************
 *
 * tdc1190CommonInit - Called by tdc1190Init or tdc1190InitList.
 *                     Initialization of individual modules, checking
 *                     for a valid VME address and BOARD_ID and 
 *                     Firmware Version.
 *
 * RETURNS: OK, or ERROR if the address is invalid or board is not present.
 */

STATUS
tdc1190CommonInit(int itdc, UINT32 laddr)
{
  int ii, res=0, errFlag = 0;
  unsigned short rdata=0;
  int boardID = 0;
  volatile struct v1190_ROM_struct *rp;

  c1190p[itdc] = (struct v1190_struct *)(laddr);

  /* Check if Board exists at that address */
#ifdef VXWORKS
  res = vxMemProbe((char *) &(c1190p[itdc]->firmwareRev),0,2,(char *)&rdata);
#else
  res = vmeMemProbe((char *) &(c1190p[itdc]->firmwareRev),2,(char *)&rdata);
#endif
  if(res < 0 )
    /*       if(res < 0 || rdata==0xffff)  */
    {
      printf("tdc1190CommonInit: ERROR: No addressable board at addr=0x%x\n",(UINT32) c1190p[itdc]);
      c1190p[itdc] = NULL;
      use1190[itdc] = 0;
      errFlag = 1;
      return ERROR;
    } 
  else 
    {
      /* Check if this is a Model 1190/1290 */
      rp = (struct v1190_ROM_struct *)((UINT32)c1190p[itdc] + V1190_ROM_OFFSET);
      boardID = ((tdcRead16(&(rp->board2))&(0xff))<<16) + 
	((tdcRead16(&(rp->board1))&(0xff))<<8) + 
	(tdcRead16(&(rp->board0))&(0xff)); 
      if((boardID != V1190_BOARD_ID) && (boardID & 0xffff != V1290_BOARD_ID)) 
	{
	  printf("tdc1190CommonInit: ERROR: Board ID does not match: 0x%x \n",boardID);
	  use1190[itdc] = 0;
	  return ERROR;
	}

      /* Check if this is the firmware we expect 
	 V1190_FIRMWARE_REV or V1190_FIRMWARE_REV+1 */
      if( (rdata != V1190_FIRMWARE_REV) && (rdata != (V1190_FIRMWARE_REV+1)) )
	{
	  printf("WARN: Firmware does not match: 0x%08x (expected 0x%08x)\n",
		 rdata,V1190_FIRMWARE_REV);
	} 
    }
  use1190[itdc] = 1;
  Nc1190++;
  printf("Initialized TDC ID %2d at address 0x%08x \n",itdc,(UINT32) c1190p[itdc]);

}



/*******************************************************************************
 *
 * tdc1190Status - Gives Status info on specified TDC
 *
 *
 * RETURNS: None
 */

STATUS
tdc1190Status(int id)
{
  int lock;
  int drdy=0, afull=0, bfull=0;
  int berr=0, testmode=0;
  int trigMatch=0, headers=0, tdcerror[4];
  UINT16 statReg, cntlReg, afullLevel, bltEvents;
  UINT16 iLvl, iVec, evStored;
  UINT16 evCount, res=0;

  CHECKID(id);

  /* read various registers */
  LOCK_1190;
  statReg = tdcRead16(&(c1190p[id]->status))&V1190_STATUS_MASK;
  cntlReg = tdcRead16(&(c1190p[id]->control))&V1190_CONTROL_MASK;
  afullLevel = tdcRead16(&(c1190p[id]->almostFullLevel));
  bltEvents = tdcRead16(&(c1190p[id]->bltEventNumber))&V1190_BLTEVNUM_MASK;
  
  iLvl = tdcRead16(&(c1190p[id]->intLevel))&V1190_INTLEVEL_MASK;
  iVec = tdcRead16(&(c1190p[id]->intVector))&V1190_INTVECTOR_MASK;
  evCount  = tdcRead32(&(c1190p[id]->evCount))&V1190_EVCOUNT_MASK;
  evStored = tdcRead16(&(c1190p[id]->evStored));
  UNLOCK_1190;

  drdy  = statReg&V1190_STATUS_DATA_READY;
  afull = statReg&V1190_STATUS_ALMOST_FULL;
  bfull = statReg&V1190_STATUS_FULL;

  trigMatch = statReg&V1190_STATUS_TRIG_MATCH;
  headers  = statReg&V1190_STATUS_HEADER_ENABLE;
  tdcerror[0] = (statReg&V1190_STATUS_ERROR_0)>>6;
  tdcerror[1] = (statReg&V1190_STATUS_ERROR_1)>>7;
  tdcerror[2] = (statReg&V1190_STATUS_ERROR_2)>>8;
  tdcerror[3] = (statReg&V1190_STATUS_ERROR_3)>>9;

  berr = cntlReg&V1190_BUSERROR_ENABLE;
  testmode = cntlReg&V1190_TESTMODE;

  /* print out status info */

#ifdef VXWORKS
  printf("STATUS for v1190 TDC at base address 0x%x\n",(UINT32)c1190p[id]);
#else
  printf("STATUS for v1190 TDC at VME (USER) base address 0x%x (0x%x)\n",
	 (UINT32)c1190p[id] - tdcAddrOffset, (UINT32)c1190p[id]);
#endif
  printf("---------------------------------------------- \n");

  if(iLvl>0) {
    printf(" Interrupts Enabled - Request level = %d words\n",afullLevel);
    printf(" VME Interrupt Level: %d   Vector: 0x%x \n",iLvl,iVec);
  } else {
    printf(" Interrupts Disabled\n");
  }
  printf("\n");

  printf("  Data Status \n");
  printf("    Events to transfer via BLT  = 0x%04x\n",bltEvents);
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
  if(evCount == 0xffff)
    printf("    Total Events  = (No Events taken)\n");
  else
    printf("    Total Events  = %d\n",evCount);
  printf("    Almost Full   = %d\n",afullLevel);

  
  printf("\n");

  printf("  TDC Mode/Status \n"); 

  res = tdc1190GetEdgeResolution(id);
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

  return OK;
}



/*******************************************************************************
 *
 *  tdc1190ReadMicro - Read Microcontroller Register
 *
 *  returns  OK or ERROR
 */

STATUS
tdc1190ReadMicro(int id, UINT16 *data, int nwords)
{
  int ii, jj, kk=0;
  UINT16 mstatus;

  CHECKID(id);

  LOCK_1190;
 retry:

  mstatus = tdcRead16(&(c1190p[id]->microHandshake)) & V1190_MICRO_READOK;
  if(kk > 10)
    {
      printf("-> ReadMicro: mstatus=%d (0x%x)\n",mstatus,
	     tdcRead16(&(c1190p[id]->microHandshake)));
    }
  if(mstatus)
    {
      for(ii=0; ii<nwords; ii++)
	{
	  jj=0;
	  while(!(tdcRead16(&(c1190p[id]->microHandshake)) & V1190_MICRO_READOK))
	    {
	      jj++;
	      if(jj>20)
		{
		  logMsg("tdc1190ReadMicro: ERROR1: Read Status not OK (read %d)\n",
			 ii,0,0,0,0,0);
		  UNLOCK_1190;
		  return(ERROR);
		}
	    }
	  data[ii] = tdcRead16(&(c1190p[id]->microReg));
	}
    }
  else
    {
      kk++;
      if(kk>=20)
	{
	  logMsg("tdc1190ReadMicro: ERROR2: Read Status not OK\n",0,0,0,0,0,0);
	  UNLOCK_1190;
	  return(ERROR);
	}
      else
	{
	  taskDelay(10);
	  goto retry;
	}
    }

  if(kk > 10) printf("-> ReadMicro: kk=%d\n",kk);
  UNLOCK_1190;
  return(OK);
}



/*******************************************************************************
 *
 *  tdc1190WriteMicro - Write to Microcontroller Register
 *
 *   returns  OK or ERROR
 */
STATUS
tdc1190WriteMicro(int id, UINT16 data)
{
  int kk=0;
  volatile UINT16 mstatus;

  CHECKID(id);

  LOCK_1190;
 retry:

  mstatus = tdcRead16(&(c1190p[id]->microHandshake)) & V1190_MICRO_WRITEOK;

  if(mstatus)
    {
      tdcWrite16(&(c1190p[id]->microReg),data);
    }
  else
    {
      kk++;
      mstatus=0;
      if(kk>=20)
	{
	  logMsg("tdc1190WriteMicro: ERROR: Write Status not OK\n",0,0,0,0,0,0);
	  UNLOCK_1190;
	  return(ERROR);
	}
      else
	{
	  taskDelay(10);
	  goto retry;
	}
    }

  UNLOCK_1190;
  if(kk > 10) printf("-> WriteMicro: kk=%d\n",kk);
  return(OK);
}

/*******************************************************************************
 *
 * tdc1190PrintEvent - Print an event from TDC to standard out. 
 *
 *
 * RETURNS: Number of Data words read from the TDC (including Header/Trailer).
 */

int
tdc1190PrintEvent(int id, int pflag)
{
  int ii, jj, nWords, evID, bunchID, evCount, headFlag, trigMatch;
  UINT32 gheader, gtrailer, theader, ttrailer, tmpData, dCnt;
  int tdcID, chanID, dataVal, tEdge;

  CHECKID(id);


  LOCK_1190;
  /* Check if there is a valid event */
  if(tdcRead16(&(c1190p[id]->status))&V1190_STATUS_DATA_READY) {
    dCnt = 0;
    headFlag  = tdcRead16(&(c1190p[id]->status))&V1190_STATUS_HEADER_ENABLE;
    trigMatch = tdcRead16(&(c1190p[id]->status))&V1190_STATUS_TRIG_MATCH;

    if(trigMatch) {  /* If trigger match mode then print individual event */

      /* Read Global Header - Get event count */
      gheader = tdcRead32(&(c1190p[id]->data[0]));
      if((gheader&V1190_DATA_TYPE_MASK) != V1190_GLOBAL_HEADER_DATA)
	{
	  logMsg("tdc1190PrintEvent: ERROR: Invalid Global Header Word 0x%08x\n",
		 gheader,2,3,4,5,6);
	  UNLOCK_1190;
	  return(ERROR);
	}
      else
	{
	  logMsg("  TDC DATA for Module at address 0x%08x\n",
		 (UINT32)c1190p[id],2,3,4,5,6);
	  evCount = (gheader&V1190_GHEAD_EVCOUNT_MASK)>>5;
	  dCnt++;
	  logMsg("  Global Header: 0x%08x   Event Count = %d \n",
		 gheader,evCount,3,4,5,6);
	}

      /* Loop over four TDC chips and get data for each */
      for(ii=0; ii<4; ii++)
	{
	  /* Read TDC Header - Get event ID, Bunch ID */
	  theader = tdcRead32(&(c1190p[id]->data[0]));
	  if((theader&V1190_DATA_TYPE_MASK) != V1190_TDC_HEADER_DATA)
	    {
	      logMsg("ERROR: Invalid TDC Header Word 0x%08x for TDC %d\n",
		     theader,ii,3,4,5,6);
	      UNLOCK_1190;
	      return(ERROR);
	    }
	  else
	    {
	      evID = (theader&V1190_TDCHEAD_EVID_MASK)>>12;
	      bunchID = (theader&V1190_TDCHEAD_BUNCHID_MASK);
	      dCnt++;
	      logMsg("    TDC %d Header: 0x%08x   EventID = %d  Bunch ID = %d ",
		     ii,theader,evID,bunchID,5,6);
	    }
	  jj=0;
	  tmpData = tdcRead32(&(c1190p[id]->data[0]));
	  dCnt++;
	  while((tmpData&V1190_DATA_TYPE_MASK) != V1190_TDC_EOB_DATA)
	    {
/* 	      if((jj % 5) == 0) printf("\n     "); */
	      logMsg("ch %d:   0x%08x (0x%08x)\n",
		     (tmpData&0x3e00000)>>21,(tmpData&0x1ffff),
		     (tmpData),4,5,6);
	      jj++;
	      tmpData = tdcRead32(&(c1190p[id]->data[jj]));
	    }
	  /* reached EOB for TDC */
	  logMsg("\n",1,2,3,4,5,6);
	  ttrailer = tmpData;
	  if((ttrailer&V1190_DATA_TYPE_MASK) != V1190_TDC_EOB_DATA)
	    {
	      logMsg("ERROR: Invalid TDC EOB Word 0x%08x for TDC %d\n",
		     ttrailer,ii,3,4,5,6);
	      UNLOCK_1190;
	      return(ERROR);
	    }
	  else
	    {
	      nWords = (ttrailer&V1190_TDCEOB_WORDCOUNT_MASK);
	      dCnt++;
	      logMsg("    TDC %d EOB   : 0x%08x   Word Count = %d \n",
		     ii,ttrailer,nWords,4,5,6);
	    }
	}

      /* next data word should be Global EOB */
      gtrailer = tdcRead32(&(c1190p[id]->data[dCnt]));
      if((gtrailer&V1190_DATA_TYPE_MASK) != V1190_GLOBAL_EOB_DATA)
	{
	  logMsg("tdc1190PrintEvent: ERROR: Invalid Global EOB Word 0x%08x\n",
		 gtrailer,2,3,4,5,6);
	  UNLOCK_1190;
	  return(ERROR);
	}
      else
	{
	  nWords = (gtrailer&V1190_GEOB_WORDCOUNT_MASK)>>5;
	  dCnt++;
	  logMsg("  Global EOB   : 0x%08x   Total Word Count = %d \n",
		 gtrailer,nWords,3,4,5,6);
	}
    }
    else /* Continuous Storage mode */
      {
	tmpData = tdcRead32(&(c1190p[id]->data[dCnt]));
	logMsg("  TDC Continuous Storage DATA\n",1,2,3,4,5,6);
	while((tmpData&V1190_DATA_TYPE_MASK) != V1190_FILLER_DATA)
	  {
	    tdcID  = (tmpData&V1190_TDC_MASK)>>24;
	    chanID = (tmpData&V1190_CHANNEL_MASK)>>19;
	    tEdge = (tmpData&V1190_EDGE_MASK)>>19;
	    dataVal = (tmpData&V1190_DATA_MASK);
	    logMsg("    %d   %d   %d    %d\n",tdcID, chanID, tEdge, dataVal,5,6);
	    dCnt++;
	    tmpData = tdcRead32(&(c1190p[id]->data[dCnt]));
	  }
	printf("\n");
      }
    UNLOCK_1190;
    return(dCnt);
  }
  else
    {
      logMsg("tdc1190PrintEvent: No data available for readout!\n",1,2,3,4,5,6);
      UNLOCK_1190;
      return(0);
    }
  UNLOCK_1190;
}

/*******************************************************************************
 *
 * tdc1190StatusFull - Return the status of the Output Buffer Full register
 *
 * RETURNS: 1 if Full, 0 if Not Full, -1 if TDC is disabled
 *
 */

int
tdc1190StatusFull(int id)
{
  int res;

  CHECKID(id);

  if(use1190[id]==0) 
    return -1; 

  LOCK_1190;
  res = tdcRead16(&(c1190p[id]->status)) & V1190_STATUS_FULL;
  UNLOCK_1190;

  return(res);
}

/*******************************************************************************
 *
 * tdc1190StatusAlmostFull - Return the status of the Output Buffer 
 *                           Almost Full register
 *
 * RETURNS: 1 if Almost Full level is met, 0 if not, -1 if TDC is disabled
 *
 */

int
tdc1190StatusAlmostFull(int id)
{
  int res;

  CHECKID(id);

  if(use1190[id]==0) 
    return -1; 

  LOCK_1190;
  res = tdcRead16(&(c1190p[id]->status)) & V1190_STATUS_ALMOST_FULL;
  UNLOCK_1190;

  return(res);
}


/******************************************************************************
 *
 * tdc1190Dready - data readyness
 *
 *
 * RETURNS: 0(No Data) or the number of events
 */

int
tdc1190Dready(int id)
{
  UINT16 stat=0, nevents;
  
  CHECKID(id);

  if(use1190[id]==0) 
    return ERROR; 

  LOCK_1190;
  stat = tdcRead16(&(c1190p[id]->status)) & V1190_STATUS_DATA_READY;
  if(stat)
    {
      nevents = tdcRead16(&(c1190p[id]->evStored));
      UNLOCK_1190;
      return(nevents);
    }
  else
    {
      UNLOCK_1190;
      return(0);
    }
}

/******************************************************************************
 *
 * tdc1190EventFifo - Set the status of the event fifo
 *                flag = 1 : enable event fifo
 *                       0 : disable event fifo
 *
 *
 * RETURNS: 0 if successful, ERROR otherwise.
 */

int
tdc1190EventFifo(int id, UINT32 flag)
{

  CHECKID(id);

  if(use1190[id]==0) 
    return ERROR; 

  if(flag>1)
    {
      printf("tdc1190EventFifo: ERROR: Invalid flag = %d",flag);
      return ERROR;
    }

  LOCK_1190;
  if(flag == 1)
    tdcWrite16(&(c1190p[id]->control),
	       tdcRead16(&(c1190p[id]->control)) | V1190_EVENT_FIFO_ENABLE);
  else if(flag == 0)
    tdcWrite16(&(c1190p[id]->control),
	       tdcRead16(&(c1190p[id]->control)) & ~V1190_EVENT_FIFO_ENABLE);

  UNLOCK_1190;

  return(0);
}

/******************************************************************************
 *
 * tdc1190ResetMSCT - Reset the MSCT/CBLT register
 *                    This effectively disables MSCT/CBLT
 *
 *
 * RETURNS: 0 if successful, ERROR otherwise.
 */

int
tdc1190ResetMCST(int id)
{
  CHECKID(id);

  if(use1190[id]==0) 
    return ERROR; 

  LOCK_1190;
  tdcWrite16(&(c1190p[id]->mcstCtrl),0);
  UNLOCK_1190;

  return(0);
}

/******************************************************************************
 *
 * tdc1190BusError - Enable/Disable Bus Errors (to finish a block transfer,
 *                   or on an empty buffer read)
 *                    flag = 0 : Disable
 *                           1 : Enable
 *
 * RETURNS: 0 if successful, ERROR otherwise.
 */

int
tdc1190BusError(int id, UINT32 flag)
{
  CHECKID(id);

  if(use1190[id]==0) 
    return ERROR; 

  if(flag>1)
    {
      printf("tdc1190EventFifo: ERROR: Invalid flag = %d",flag);
      return ERROR;
    }

  LOCK_1190;
  if(flag == 1)
    {
      printf("set BUSerror\n");
      tdcWrite16(&(c1190p[id]->control),
		 tdcRead16(&(c1190p[id]->control)) | V1190_BUSERROR_ENABLE);
    }
  else if(flag == 0)
    {
      printf("reset BUSerror\n");
      tdcWrite16(&(c1190p[id]->control),
		 tdcRead16(&(c1190p[id]->control)) & ~V1190_BUSERROR_ENABLE);
    }
  UNLOCK_1190;

  return(0);
}

/******************************************************************************
 *
 * tdc1190Align64 - Enable/Disable 64 bit alignment for block transfers
 *                   flag = 0 : Disable
 *                          1 : Enable
 *
 * RETURNS: 0 if successful, ERROR otherwise.
 */

int
tdc1190Align64(int id, UINT32 flag)
{

  CHECKID(id);

  if(use1190[id]==0) 
    return ERROR; 

  if(flag>1)
    {
      printf("tdc1190EventFifo: ERROR: Invalid flag = %d",flag);
      return ERROR;
    }

  LOCK_1190;
  printf("tdc1190Align64 before: 0x%04x\n",
	 tdcRead16(&(c1190p[id]->control)));

  if(flag == 1)
    tdcWrite16(&(c1190p[id]->control),
	       tdcRead16(&(c1190p[id]->control)) | V1190_ALIGN64);

  else if(flag == 0)
    tdcWrite16(&(c1190p[id]->control),
	       tdcRead16(&(c1190p[id]->control)) & ~V1190_ALIGN64);

  else
    printf("tdc1190Align64: unknown flag=%d\n",flag);

  printf("tdc1190Align64 after: 0x%04x\n",
	 tdcRead16(&(c1190p[id]->control)));
  UNLOCK_1190;


  return(0);
}

/**************************************************************************************
 *
 *  tdc1190ReadBlock - General Data readout routine
 *
 *    id    - Slot number of module to read
 *    data  - local memory address to place data
 *    nwrds - Max number of words to transfer
 *    rflag - Readout Flag
 *              0 - programmed I/O from the specified board
 *              1 - DMA transfer using Universe/Tempe DMA Engine 
 *                    (DMA VME transfer Mode must be setup prior)
 *              2 - Linked-List DMA transfer (Linux: Linked-List system memory
 *                     must be setup prior)
 *                - Must be compiled with WITHLL
 */

int
tdc1190ReadBlock(int id, volatile UINT32 *data, int nwrds, int rflag)
{
  int ii, jj, blknum, evnum1, evnum2, chan;
  int stat, retVal, xferCount;
  int dCnt, berr=0;
  int dummy=0;
  volatile unsigned int *laddr;
  unsigned int status, bhead, btail, ehead, val;
  unsigned int vmeAdr, csr;
  unsigned int vmeAdrLL[V1190_MAX_MODULES],dmaSizeLL[V1190_MAX_MODULES],
    destAdrLL[V1190_MAX_MODULES], numLL;
  unsigned int itdcbuf=0;
  unsigned int fifowords=0;

  CHECKID(id);

  if(nwrds <= 0) 
    nwrds= (V1190_MAX_WORDS_PER_BOARD*V1190_MAX_MODULES) + 10;

  LOCK_1190;
  if(rflag >= 1) /* Block Transfers */
    { 
      
      /*Assume that the DMA programming is already setup. */
      /* Don't Bother checking if there is valid data - that should be done prior
	 to calling the read routine */

      /* Check for 8 byte boundary for address - insert dummy word (Slot 0 FADC Dummy DATA)*/
      if((unsigned long) (data)&0x7) 
	{
#ifdef VXWORKS
	  *data = V1190_FILLER_DATA;
#else
	  *data = LSWAP(V1190_FILLER_DATA);
#endif
	  dummy = 1;
	  laddr = (data + 1);
	} 
      else 
	{
	  dummy = 0;
	  laddr = data;
	}

      if(rflag == 2) /* Linked-list Mode DMA */
	{
#ifdef WITHLL
	  /* Setup the LL arrays */
	  jj=0;
	  for(ii=0; ii<V1190_MAX_MODULES;ii++)
	    {
	      if(use1190[ii])
		{
#ifdef VXWORKS
		  vmeAdrLL[jj] = (unsigned int)c1190p[ii];
#else
		  vmeAdrLL[jj] = (unsigned int)c1190p[ii] - 
		    (unsigned int)(tdcAddrOffset);
#endif
		  /* Get the number of words in the FIFO from the module */
		  fifowords = (tdcRead32(&(c1190p[ii]->fifo))&0xffff);
		  /* 128 bit alignment for 2eVME/2eSST */
		  if(fifowords%4)
		    dmaSizeLL[jj] = (fifowords+(4-(fifowords%4)))<<2;
		  else
		    dmaSizeLL[jj] = fifowords<<2;

		  destAdrLL[jj] = (unsigned int)&laddr[itdcbuf];
		  itdcbuf += dmaSizeLL[jj];
		  jj++;
		}
	    }
#ifdef DEBUGLL
	  for(ii=0;ii<jj;ii++)
	    {
	      logMsg("%d   0x%08x   0x%08x   0x%08x\n",ii,vmeAdrLL[ii],destAdrLL[ii],dmaSizeLL[ii],5,6);
	    }
#endif
	  numLL=jj;
#ifdef VXWORKS
	  usrVme2MemDmaListSet(vmeAdrLL, destAdrLL, dmaSizeLL, numLL);
#else 
	  vmeDmaSetupLL((UINT32)laddr,vmeAdrLL,dmaSizeLL,numLL);
#endif
#else /* WITHLL */
	  logMsg("tdc1190ReadBlock: ERROR: Linked List DMA not enabled through compilation WITHLL\n",
		 1,2,3,4,5,6);
	  return ERROR;
#endif /* WITHLL */
	}
      
      else if (rflag == 1) /* Single Module DMA Mode */
	{ 
#ifdef VXWORKS
	  vmeAdr = (unsigned int)(c1190p[id])- (unsigned int)(tdcAddrOffset);
#else
	  vmeAdr = (unsigned int)(c1190p[id])- (unsigned int)(tdcAddrOffset);
#endif
	}
      if (rflag == 1) 
	{
#ifdef VXWORKS
	  retVal = sysVmeDmaSend((UINT32)laddr, vmeAdr, (nwrds<<2), 0);
#else
	  retVal = vmeDmaSend((UINT32)laddr, vmeAdr, (nwrds<<2));
#endif
	}
#ifdef WITHLL
      else if (rflag == 2) 
	{
#ifdef VXWORKS
	  retVal = usrVmeDmaListStart();
	  retVal = 0;
#else
	  vmeDmaSendLL();
	  retVal = 0;
#endif
	}
#endif /* WITHLL */
      if(retVal |= 0) 
	{
	  logMsg("tdc1190ReadBlock: ERROR in DMA transfer Initialization 0x%x\n",retVal,0,0,0,0,0);
	  UNLOCK_1190;
	  return(retVal);
	}
      /* Wait until Done or Error */
#ifdef VXWORKS
      if (rflag == 2)
	{
#ifdef WITHLL
	  retVal = usrVme2MemDmaDone();
#else
	  logMsg("tdc1190ReadBlock: ERROR: Linked List DMA not enabled through compilation WITHLL\n",
		 1,2,3,4,5,6);
	  return ERROR;
#endif /* WITHLL */
	}
      else if (rflag == 1)
	retVal = sysVmeDmaDone(10000,1);
#else
      retVal = vmeDmaDone();
#endif

      if(retVal > 0) 
	{
	  /* Check to see that Bus error was generated by TDC */
	  if(rflag == 2) 
	    {
	      csr = tdcRead16(&(c1190p[tdcMaxSlot]->status));  /* from Last TDC */
	      stat = (csr)&V1190_STATUS_BERR_FLAG;  /* from Last TDC */
	    }
	  else if (rflag == 1)
	    {
	      csr = tdcRead16(&(c1190p[id]->status));
	      stat = (csr)&V1190_STATUS_BERR_FLAG;
	    }
	  if((retVal>0) && (stat)) 
	    {
#ifdef VXWORKS
	      xferCount = (nwrds - (retVal>>2) + dummy);  /* Number of Longwords transfered */
#else
	      xferCount = (retVal>>2) + dummy;  /* Number of Longwords transfered */
	      /* 	xferCount = (retVal + dummy);  /\* Number of Longwords transfered *\/ */
#endif
	      UNLOCK_1190;
	      return(xferCount); /* Return number of data words transfered */
	    }
	  else
	    {
#ifdef VXWORKS
	      xferCount = (nwrds - (retVal>>2) + dummy);  /* Number of Longwords transfered */
#else
	      xferCount = (retVal>>2) + dummy;  /* Number of Longwords transfered */
#endif
	      logMsg("tdc1190ReadBlock: DMA transfer terminated by unknown BUS Error (csr=0x%x xferCount=%d)\n",csr,xferCount,0,0,0,0);
	      UNLOCK_1190;
	      return(xferCount);
	      /* 	return(ERROR); */
	    }
	} 
      else if (retVal == 0)
	{ /* Block Error finished without Bus Error */
#ifdef VXWORKS
	  logMsg("tdc1190ReadBlock: WARN: DMA transfer terminated by word count 0x%x\n",nwrds,0,0,0,0,0);
#else
	  logMsg("tdc1190ReadBlock: WARN: DMA transfer returned zero word count 0x%x\n",nwrds,0,0,0,0,0);
#endif
	  UNLOCK_1190;
	  return(nwrds);
	} 
      else 
	{  /* Error in DMA */
#ifdef VXWORKS
	  logMsg("tdc1190ReadBlock: ERROR: sysVmeDmaDone returned an Error\n",0,0,0,0,0,0);
#else
	  logMsg("tdc1190ReadBlock: ERROR: vmeDmaDone returned an Error\n",0,0,0,0,0,0);
#endif
	  UNLOCK_1190;
	  return(retVal);
	}

    } 
  else 
    {  /*Programmed IO */

      /* Check if Bus Errors are enabled. If so then disable for Prog I/O reading */
// FIXME: Get rid of this?
#ifdef BERR
      berr = tdcRead16(&(c1190p[id]->control))&V1190_BUSERROR_ENABLE;
      if(berr)
	tdcWrite16(&(c1190p[id]->control),
		   tdcRead16(&(c1190p[id]->control)) & ~V1190_BUSERROR_ENABLE);
#endif
      UNLOCK_1190; /* will be locked again within tdc1190ReadData */
      dCnt = tdc1190ReadData(id,(UINT32 *)data,nwrds);
      LOCK_1190;
      

// FIXME: Get rid of this?
#ifdef BERR
      if(berr)
	tdcWrite16(&(c1190p[id]->control),
		   tdcRead16(&(c1190p[id]->control)) | V1190_BUSERROR_ENABLE);
#endif
      UNLOCK_1190;
      return(dCnt);
    }

  UNLOCK_1190;
  return(OK);

}

/*******************************************************************************
 *
 * tdc1190ReadEvent - Read event from TDC to specified address. 
 *
 * // FIXME: May just remove this routine..
 *
 * RETURNS: Number of Data words read from the TDC (including Header/Trailer).
 */
// FIXME: Make sure endian-ness is correct

int
tdc1190ReadEvent(int id, UINT32 *tdata)
{
/*   int ii, nWords, evID; */
  UINT32 header, trailer, dCnt, tmpData;
  UINT16 contReg, statReg;
  int fifodata;

  CHECKID(id);

  LOCK_1190;
  /* Check if there is a valid event */
  if(tdcRead16(&(c1190p[id]->evStored))==0) 
    {
      logMsg("tdc1190ReadEvent: Data Buffer is EMPTY!\n",0,0,0,0,0,0);
      UNLOCK_1190;
      return(0);
    }

  statReg = tdcRead16(&(c1190p[id]->status));
  contReg = tdcRead16(&(c1190p[id]->control));
  if(statReg & V1190_STATUS_DATA_READY) 
    {
      dCnt = 0;

      /* Check to see if the Event FIFO is enabled, 
	 If so, read its register */
      if(contReg & V1190_EVENT_FIFO_ENABLE)
	fifodata = tdcRead32(&(c1190p[id]->fifo));

	/* Read Header  */
	header = tdcRead32(&(c1190p[id]->data[dCnt]));
      if((header&V1190_DATA_TYPE_MASK) != V1190_TDC_HEADER_DATA) 
	{
	  logMsg("tdc1190ReadEvent: ERROR: Invalid Header Word 0x%08x\n",header,0,0,0,0,0);
	  UNLOCK_1190;
	  return(-1);
	}
      else
	{
	  tdata[dCnt] = header;
	  dCnt++;
	  tmpData = tdcRead32(&(c1190p[id]->data[0]));
	  while((tmpData&V1190_DATA_TYPE_MASK) != V1190_TDC_EOB_DATA )
	    {
	      tdata[dCnt] = tmpData;
	      dCnt++;
	      tmpData = tdcRead32(&(c1190p[id]->data[0]));
	    }
	}

      /* reached EOB for TDC */
      trailer = tmpData;
      if((trailer&V1190_DATA_TYPE_MASK) != V1190_TDC_EOB_DATA) {
	logMsg("tdc1190ReadEvent: ERROR: Invalid Trailer Word 0x%08x\n",trailer,0,0,0,0,0);
	UNLOCK_1190;
	return(-1);
      }else{
	tdata[dCnt] = trailer;
	dCnt++;
      }
      UNLOCK_1190;
      return (dCnt);
      
    }else{
    logMsg("tdc1190ReadEvent: Data Not ready for readout!\n",0,0,0,0,0,0);
    UNLOCK_1190;
    return(0);
  }

}


/******************************************************************************
 *
 * tdc1190ReadData - available data into a buffer. 
 *
 *
 * RETURNS: Number of Data words read from the TDC (including Header/Trailer).
 */
// FIXME: Make sure endian-ness is correct

int
tdc1190ReadData(int id, UINT32 *tdata, int maxWords)
{
  int ii, jj, nWords, evID, bunchID, evCount, headFlag, trigMatch;
  UINT32 gheader, gtrailer, theader, ttrailer, tmpData, dCnt;

  CHECKID(id);

  if(maxWords==0) maxWords = 1024;

  /* Check if there is a valid event */
  LOCK_1190;

  if(tdcRead16(&(c1190p[id]->status)) & V1190_STATUS_DATA_READY)
    {
      dCnt = 0;
      headFlag  = tdcRead16(&(c1190p[id]->status)) & V1190_STATUS_HEADER_ENABLE;
      trigMatch = tdcRead16(&(c1190p[id]->status)) & V1190_STATUS_TRIG_MATCH;

      if(trigMatch) /* If trigger match mode then read individual event */
	{
	  /* Read Global Header - Get event count */
	  gheader = tdcRead32(&(c1190p[id]->data[0]));
	  if((gheader & V1190_DATA_TYPE_MASK) != V1190_GLOBAL_HEADER_DATA)
	    {
	      logMsg("tdc1190ReadData: ERROR: Invalid Global Header Word 0x%08x\n",
		     gheader,2,3,4,5,6);
	      UNLOCK_1190;
	      return(ERROR);
	    }
	  else
	    {
	      tdata[dCnt] = gheader;
	      evCount = (gheader & V1190_GHEAD_EVCOUNT_MASK) >> 5;
	      dCnt++;
	    }

	  /* Loop over four TDC chips and get data for each */
	  for(ii=0; ii<4; ii++)
	    {
	      /* Read TDC Header - Get event ID, Bunch ID */
	      theader = tdcRead32(&(c1190p[id]->data[0]));
	      if((theader&V1190_DATA_TYPE_MASK) != V1190_TDC_HEADER_DATA)
		{
		  logMsg("ERROR: Invalid TDC Header Word 0x%08x for TDC %d\n",
			 theader,ii,3,4,5,6);
		  UNLOCK_1190;
		  return(ERROR);
		}
	      else
		{
		  tdata[dCnt] = theader;
		  evID = (theader&V1190_TDCHEAD_EVID_MASK)>>12;
		  bunchID = (theader&V1190_TDCHEAD_BUNCHID_MASK);
		  dCnt++;
		}
	      jj=0;
	      tmpData = tdcRead32(&(c1190p[id]->data[0]));
	      while((tmpData&V1190_DATA_TYPE_MASK) != V1190_TDC_EOB_DATA)
		{
		  tdata[dCnt] = tmpData;
		  jj++;
		  dCnt++;
		  tmpData = tdcRead32(&(c1190p[id]->data[jj]));
		}

	      /* reached EOB for TDC */
	      ttrailer = tmpData;
	      if((ttrailer&V1190_DATA_TYPE_MASK) != V1190_TDC_EOB_DATA)
		{
		  logMsg("ERROR: Invalid TDC EOB Word 0x%08x for TDC %d\n",
			 ttrailer,ii,3,4,5,6);
		  UNLOCK_1190;
		  return(ERROR);
		}
	      else
		{
		  tdata[dCnt] = ttrailer;
		  nWords = (ttrailer&V1190_TDCEOB_WORDCOUNT_MASK);
		  dCnt++;
		}
	    }

	  /* next data word should be Global EOB */
	  gtrailer = tdcRead32(&(c1190p[id]->data[dCnt]));
	  if((gtrailer&V1190_DATA_TYPE_MASK) != V1190_GLOBAL_EOB_DATA)
	    {
	      logMsg("tdc1190ReadData: ERROR: Invalid Global EOB Word 0x%08x\n",
		     gtrailer,2,3,4,5,6);
	      UNLOCK_1190;
	      return(ERROR);
	    }
	  else
	    {
	      tdata[dCnt] = gtrailer;
	      nWords = (gtrailer&V1190_GEOB_WORDCOUNT_MASK)>>5;
	      dCnt++;
	    }
	}
      else /* Continuous Storage mode */
	{
	  tmpData = tdcRead32(&(c1190p[id]->data[dCnt]));
	  while(((tmpData&V1190_DATA_TYPE_MASK) != V1190_FILLER_DATA) &&
		(dCnt<maxWords))
	    {
	      tdata[dCnt]=tmpData;
	      dCnt++;
	      tmpData = tdcRead32(&(c1190p[id]->data[dCnt]));
	    }
	}
      UNLOCK_1190;
      return(dCnt);
    }
  else
    {
      logMsg("tdc1190ReadData: No data available for readout!\n",1,2,3,4,5,6);
      UNLOCK_1190;
      return(0);
    }
}

/******************************************************************************
 * ACQUISITION MODE OPCODES Routines
 *
 *  All routines return OK or ERROR,
 *   unless otherwise specified.
 * 
 * tdc1190SetTriggerMatchingMode
 * tdc1190SetContinuousStorageMode
 * tdc1190ReadAcquisitionMode         - Returns 0: Continuous storage mode
 *                                              1: Trigger matching mode
 * tdc1190SetKeepToken
 * tdc1190ClearKeepToken
 * tdc1190LoadDefaultConfiguration
 * tdc1190SaveUserConfiguration
 * tdc1190LoadUserConfiguration
 * tdc1190SetAutoLoadUserConfiguration
 * tdc1190SetAutoLoadDefaultConfiguration
 *
*/

STATUS
tdc1190SetTriggerMatchingMode(int id)
{
  STATUS status;
  CHECKID(id);
  status = tdc1190WriteMicro(id,0x0000);
  tdc1190Clear(id);
  return status;
}

STATUS
tdc1190SetContinuousStorageMode(int id)
{
  STATUS status;
  CHECKID(id);
  status = tdc1190WriteMicro(id,0x0100);
  tdc1190Clear(id);
  return status;
}

int
tdc1190ReadAcquisitionMode(int id)
{
  UINT16 tdata;
  int status;
  int rval;

  CHECKID(id);
  tdc1190WriteMicro(id,0x0200);
  status = tdc1190ReadMicro(id,&tdata,1);
  if(status==ERROR)
    {
      rval = ERROR;
    }
  else 
    {
      rval = tdata&0x1;
    }

  return rval;
}

STATUS
tdc1190SetKeepToken(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x0300);
}

STATUS
tdc1190ClearKeepToken(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x0400);
}

STATUS
tdc1190LoadDefaultConfiguration(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x0500);
}

STATUS
tdc1190SaveUserConfiguration(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x0600);
}

STATUS 
tdc1190LoadUserConfiguration(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x0700);
}

STATUS
tdc1190SetAutoLoadUserConfiguration(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x0800);
}

STATUS
tdc1190SetAutoLoadDefaultConfiguration(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x0900);
}




/******************************************************************************
 * TRIGGER MODE OPCODES Routines
 *
 *  All routines return OK or ERROR,
 *   unless otherwise specified.
 *  All parameters in tdc1190Set* are in steps of 25ns
 * 
 * tdc1190SetWindowWidth                - Set window width [25ns,51175ns]
 * tdc1190SetWindowOffset               - Set window offset > -800000ns
 * tdc1190SetExtraSearchMargin          - Set window extra search margin [0,1250ns]
 * tdc1190SetRejectMargin               - Set reject margin [0,1250ns]
 * tdc1190EnableTriggerTimeSubtraction
 * tdc1190DisableTriggerTimeSubtraction
 * tdc1190ReadTriggerConfiguration
 *
 */

STATUS
tdc1190SetWindowWidth(int id, UINT32 window_width)
{
  UINT16 tdata;

  CHECKID(id);
  tdc1190WriteMicro(id,0x1000);

  if(window_width < 25)         
    tdata = 1;
  else if (window_width > 51175) 
    tdata = 0x7FF;
  else
    tdata = window_width/25;

  printf("%s(%d): Set Window Width to %d ns\n",__FUNCTION__,id,tdata*25);

  return tdc1190WriteMicro(id,tdata);
}

STATUS
tdc1190SetWindowOffset(int id, INT32 window_offset)
{
  INT16 tdata;

  CHECKID(id);
  tdc1190WriteMicro(id,0x1100);

  if(window_offset < -800000) 
    tdata = -32000;
  else             
    tdata = window_offset/25;

  printf("%s(%d): Set Window Offset to %d ns\n",__FUNCTION__,id,tdata*25);

  return tdc1190WriteMicro(id,tdata);

}

STATUS
tdc1190SetExtraSearchMargin(int id, UINT32 window_extra)
{
  UINT16 tdata;

  CHECKID(id);
  tdc1190WriteMicro(id,0x1200);

  if(window_extra < 0)         
    tdata = 0;
  else if (window_extra > 1250) 
    tdata = 50;
  else               
    tdata = window_extra/25;
  printf("%s(%d): Set Extra Search Margin to %d ns\n",__FUNCTION__,id,tdata*25);

  return tdc1190WriteMicro(id,tdata);

}

STATUS
tdc1190SetRejectMargin(int id, UINT32 window_reject)
{
  UINT16 tdata;

  CHECKID(id);
  tdc1190WriteMicro(id,0x1300);

  if(window_reject < 0)
    tdata = 0;
  else if (window_reject > 1250) 
    tdata = 50;
  else               
    tdata = window_reject/25;

  printf("%s(%d): Set Reject Margin to %d ns\n",__FUNCTION__,id,tdata*25);

  return tdc1190WriteMicro(id,tdata);

}

STATUS
tdc1190EnableTriggerTimeSubtraction(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x1400);
}

STATUS
tdc1190DisableTriggerTimeSubtraction(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x1500);
}

STATUS
tdc1190ReadTriggerConfiguration(int id)
{
  INT16 tmp[5] = {0,0,0,0,0};
  CHECKID(id);

  EIEIO;
  SynC;
  tdc1190WriteMicro(id,0x1600);
  EIEIO;
  SynC;

  /*tdc1190ReadMicro(id,tmp,5);*/
  tdc1190ReadMicro(id,&tmp[0],1);
  tdc1190ReadMicro(id,&tmp[1],1);
  tdc1190ReadMicro(id,&tmp[2],1);
  tdc1190ReadMicro(id,&tmp[3],1);
  tdc1190ReadMicro(id,&tmp[4],1);

  EIEIO;
  SynC;

  printf("%s(%d):\n",__FUNCTION__,id);
  printf("  Window Width              = %6d ns\n",tmp[0]*25);
  printf("  Window Offset             = %6d ns\n",tmp[1]*25);
  printf("  Extra Seach Margin        = %6d ns\n",tmp[2]*25);
  printf("  Reject Margin             = %6d ns\n",tmp[3]*25);
  printf("  Trigger Time Subtraction = %6d\n",tmp[4]);

  return(OK);
}

/******************************************************************************
 * EDGE DETECTION AND RESOLUTION OPCODES Routines
 *
 *  All routines return OK or ERROR,
 *   unless otherwise specified.
 *
 * tdc1190SetEdgeDetectionConfig   
 * tdc1190ReadEdgeDetectionConfig - Returns:
 *                                    0 : Pair mode
 *                                    1 : Trailing Edge
 *                                    2 : Leading Edge
 *                                    3 : Both leading and trailing edge
 * tdc1190SetEdgeResolution
 * tdc1190GetEdgeResolution       - Returns edge resolution (ns)
 * tdc1190SetPairResolution 
 * tdc1190GetPairResolution       - Returns 2nd and 3rd argument (ns)
 * tdc1190SetDoubleHitResolution
 * tdc1190GetDoubleHitResolution  - Returns double hit resolution (ns)
 *
*/

STATUS
tdc1190SetEdgeDetectionConfig(int id, UINT16 setedge)
{
  UINT16 tdata=0;

  CHECKID(id);

  printf("%s(%d): ",__FUNCTION__,id);
  if(setedge==0)
    {
      tdata=0x0;
      printf("set pair mode\n");
    }
  else if(setedge==1)
    {
      tdata=0x1;
      printf("set trailing edge only\n");
    }
  else if(setedge==2)
    {
      tdata=0x2;
      printf("set leading edge only\n");
    }
  else
    {
      printf("set both leading and trailing edges\n");
      tdata=0x3;
    }

  tdc1190WriteMicro(id,0x2200);
  return tdc1190WriteMicro(id,tdata);

}

int
tdc1190ReadEdgeDetectionConfig(int id)
{
  UINT16 tdata;

  CHECKID(id);

  tdc1190WriteMicro(id,0x2300);
  tdc1190ReadMicro(id,&tdata,1);

  tdata &= 0x3;
  if(tdata<3)
    {
      printf("%s(%d): Mode =",__FUNCTION__,id);
      if(tdata==0)
	printf(" pair mode\n");
      else if(tdata==1)
	printf(" trailing edge\n");
      else if(tdata==2)
	printf(" leading edge\n");
      else if(tdata==3)
	printf(" both leading and trailing\n");
    }
  else
    {
      printf("%s(%d) ERROR: tdata=%d (0x%x)\n",__FUNCTION__,id,tdata,tdata);
      return(ERROR);
    }

  return(tdata);
}

STATUS
tdc1190SetEdgeResolution(int id, UINT16 edge_res)
{
  UINT16 tdata;

  CHECKID(id);

  if(edge_res==800)      
    tdata = 0;
  else if(edge_res==200) 
    tdata = 1;
  else if(edge_res==100) 
    tdata = 2;
  else
    {
      printf("%s(%d): ERROR: Invalid edge_res=%d, must be 100, 200 or 800 ps\n",
	     __FUNCTION__,id,edge_res);
      return(ERROR);
    }

  tdc1190WriteMicro(id,0x2400);
  tdc1190WriteMicro(id,tdata);
  printf("%s(%d): Set Edge Resolution to %d ps\n",__FUNCTION__,id,edge_res);

  return(OK);
}

int
tdc1190GetEdgeResolution(int id)
{
  UINT16 tdata=0;
  int rval;

  CHECKID(id);

  tdc1190WriteMicro(id,0x2600);
  tdc1190ReadMicro(id,&tdata,1);

  tdata &= 0x3;
  if(tdata==0)
    rval = 800;
  else if(tdata==1)
    rval = 200;
  else if(tdata==2)
    rval = 100;
  else
    {
      printf("%s(%d): ERROR: tdata=%d (0x%x)\n",__FUNCTION__,id,tdata,tdata);
      return(ERROR);
    }
  printf("%s(%d): Edge Resolution is %d ps\n",__FUNCTION__,id,rval);

  return(rval);
}

STATUS
tdc1190SetPairResolution(int id, UINT32 leading_res, UINT32 pulse_width_res)
{
  UINT16 tdata;

  CHECKID(id);

  if(leading_res==100)            tdata = 0x0;
  else if(leading_res==200)       tdata = 0x1;
  else if(leading_res==400)       tdata = 0x2;
  else if(leading_res==800)       tdata = 0x3;
  else if(leading_res==1600)      tdata = 0x4;
  else if(leading_res==3120)      tdata = 0x5;
  else if(leading_res==6250)      tdata = 0x6;
  else if(leading_res==12500)     tdata = 0x7;
  else
    {
      printf("%s(%d): ERROR: Invalid leading_res=%d\n",__FUNCTION__,id,
	     leading_res);
      return(ERROR);
    }

  if(pulse_width_res==100)         tdata += 0x0;
  else if(pulse_width_res==200)    tdata += 0x100;
  else if(pulse_width_res==400)    tdata += 0x200;
  else if(pulse_width_res==800)    tdata += 0x300;
  else if(pulse_width_res==1600)   tdata += 0x400;
  else if(pulse_width_res==3200)   tdata += 0x500;
  else if(pulse_width_res==6250)   tdata += 0x600;
  else if(pulse_width_res==12500)  tdata += 0x700;
  else if(pulse_width_res==25000)  tdata += 0x800;
  else if(pulse_width_res==50000)  tdata += 0x900;
  else if(pulse_width_res==100000) tdata += 0xA00;
  else if(pulse_width_res==200000) tdata += 0xB00;
  else if(pulse_width_res==400000) tdata += 0xC00;
  else if(pulse_width_res==800000) tdata += 0xD00;
  else
    {
      printf("%s(%d): ERROR: Invalid pulse_width_res=%d\n",__FUNCTION__,id,
	     pulse_width_res);
      return(ERROR);
    }

  tdc1190WriteMicro(id,0x2500);
  tdc1190WriteMicro(id,tdata);
  printf("%s(%d): Set Pair Resolution to %d(leading time), %d(width) ps\n",
	 __FUNCTION__,id,leading_res,pulse_width_res);

  return(OK);
}

STATUS
tdc1190GetPairResolution(int id, UINT32 *leading_res, UINT32 *pulse_width_res)
{
  UINT16 tdata, tdata1, tdata2;

  CHECKID(id);

  tdc1190WriteMicro(id,0x2600);
  tdc1190ReadMicro(id,&tdata,1);

  tdata1 = tdata & 0x7;
  if(tdata1 == 0x0)      *leading_res = 100;
  else if(tdata1 == 0x1) *leading_res = 200;
  else if(tdata1 == 0x2) *leading_res = 400;
  else if(tdata1 == 0x3) *leading_res = 800;
  else if(tdata1 == 0x4) *leading_res = 1600;
  else if(tdata1 == 0x5) *leading_res = 3120;
  else if(tdata1 == 0x6) *leading_res = 6250;
  else if(tdata1 == 0x7) *leading_res = 12500;
  else
    {
      printf("ERROR: GetPairResolution: tdata1=%d\n",tdata1);
      return(ERROR);
    }

  tdata2 = tdata & 0xF00;
  if(tdata2 == 0x0)        *pulse_width_res = 100;
  else if(tdata2 == 0x100) *pulse_width_res = 200;
  else if(tdata2 == 0x200) *pulse_width_res = 400;
  else if(tdata2 == 0x300) *pulse_width_res = 800;
  else if(tdata2 == 0x400) *pulse_width_res = 1600;
  else if(tdata2 == 0x500) *pulse_width_res = 3200;
  else if(tdata2 == 0x600) *pulse_width_res = 6250;
  else if(tdata2 == 0x700) *pulse_width_res = 12500;
  else if(tdata2 == 0x800) *pulse_width_res = 25000;
  else if(tdata2 == 0x900) *pulse_width_res = 50000;
  else if(tdata2 == 0xA00) *pulse_width_res = 100000;
  else if(tdata2 == 0xB00) *pulse_width_res = 200000;
  else if(tdata2 == 0xC00) *pulse_width_res = 400000;
  else if(tdata2 == 0xD00) *pulse_width_res = 800000;
  else
    {
      printf("ERROR: GetPairResolution: tdata2=%d\n",tdata2);
      return(ERROR);
    }
  printf("Pair Resolution is %d(leading time), %d(width) ps\n",*leading_res,*pulse_width_res);

  return(OK);
}

STATUS
tdc1190SetDoubleHitResolution(int id, UINT16 doublehit_res)
{
  UINT16 tdata;

  CHECKID(id);

  if(doublehit_res==5)        tdata = 0;
  else if(doublehit_res==10)  tdata = 1;
  else if(doublehit_res==30)  tdata = 2;
  else if(doublehit_res==100) tdata = 3;
  else
    {
      printf("%s(%d): ERROR: Invalid doublehit_res=%d, must be 5, 10, 30 or 100 ns\n",
	     __FUNCTION__,id,doublehit_res);
      return(ERROR);
    }

  tdc1190WriteMicro(id,0x2800);
  tdc1190WriteMicro(id,tdata);
  printf("%s(%d): Set Double Hit Resolution to %d ns\n",__FUNCTION__,id,doublehit_res);

  return(OK);
}

int
tdc1190GetDoubleHitResolution(int id)
{
  UINT16 tdata;
  int rval;

  CHECKID(id);

  tdc1190WriteMicro(id,0x2900);
  tdc1190ReadMicro(id,&tdata,1);

  tdata &= 0x3;
  if(tdata==0)      rval = 5;
  else if(tdata==1) rval = 10;
  else if(tdata==2) rval = 30;
  else if(tdata==3) rval = 100;
  else
    {
      printf("%s(%d): ERROR: tdata=%d (0x%x)\n",__FUNCTION__,id,tdata,tdata);
      return(ERROR);
    }
  printf("w%s(%d): Double Hit Resolution is %d ns\n",__FUNCTION__,id,rval);

  return(rval);
}

/******************************************************************************
 * TDC READOUT OPCODES Routines
 *
 *  All routines return OK or ERROR,
 *   unless otherwise specified.
 *
 * tdc1190EnableTDCHeaderAndTrailer
 * tdc1190DisableTDCHeaderAndTrailer
 * tdc1190GetTDCHeaderAndTrailer        - Returns 
 *                                         0 : header/trailer disabled
 *                                         1 : header/trailer enabled
 * tdc1190SetMaxNumberOfHitsPerEvent
 * tdc1190GetMaxNumberOfHitsPerEvent    - Returns maximum number of hits/event
 * tdc1190EnableTDCErrorMark
 * tdc1190DisableTDCErrorMark
 * tdc1190EnableTDCErrorBypass
 * tdc1190DisableTDCErrorBypass
 * tdc1190SetTDCErrorType
 * tdc1190GetTDCErrorType               - Returns mask of enabled error types
 * tdc1190SetEffectiveSizeOfReadoutFIFO
 * tdc1190GetEffectiveSizeOfReadoutFIFO - Returns number of words in readout FIFO
 *
 */

STATUS
tdc1190EnableTDCHeaderAndTrailer(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x3000);
}

STATUS
tdc1190DisableTDCHeaderAndTrailer(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x3100);
}

int
tdc1190GetTDCHeaderAndTrailer(int id)
{
  UINT16 tdata;

  CHECKID(id);

  tdc1190WriteMicro(id,0x3200);
  tdc1190ReadMicro(id,&tdata,1);

  tdata &= 0x1;
  if(tdata==0)
    printf("%s(%d): TDC Header/Trailer disabled\n",__FUNCTION__,id);
  else if(tdata==1)
    printf("%s(%d): TDC Header/Trailer enabled\n",__FUNCTION__,id);

  return(tdata);
}

STATUS
tdc1190SetMaxNumberOfHitsPerEvent(int id, UINT32 nhits)
{
  UINT16 tdata;

  CHECKID(id);

  tdc1190WriteMicro(id,0x3300);

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
    printf("%s(%d): Set Unlimited Number Of Hits Per Event\n",__FUNCTION__,id);
  else
    printf("%s(%d): Set Maximum Number Of Hits Per Event to %d\n",__FUNCTION__,id,nhits);

  return tdc1190WriteMicro(id,tdata);

}

int
tdc1190GetMaxNumberOfHitsPerEvent(int id)
{
  UINT16 tdata;
  int rval;

  CHECKID(id);

  tdc1190WriteMicro(id,0x3400);
  tdc1190ReadMicro(id,&tdata,1);

  tdata &= 0xF;
  if(tdata==0)      rval = 0;
  else if(tdata==1) rval = 1;
  else if(tdata==2) rval = 2;
  else if(tdata==3) rval = 4;
  else if(tdata==4) rval = 8;
  else if(tdata==5) rval = 16;
  else if(tdata==6) rval = 32;
  else if(tdata==7) rval = 64;
  else if(tdata==8) rval = 128;
  else if(tdata==9) rval = 9999;
  else
    {
      printf("%s(%d): ERROR: tdata=%d (0x%x)\n",__FUNCTION__,id,tdata,tdata);
      return(ERROR);
    }

  return(rval);
}

STATUS
tdc1190EnableTDCErrorMark(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x3500);
}

STATUS
tdc1190DisableTDCErrorMark(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x3600);
}

STATUS
tdc1190EnableTDCErrorBypass(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x3700);
}

STATUS
tdc1190DisableTDCErrorBypass(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x3800);
}

STATUS
tdc1190SetTDCErrorType(int id, UINT32 mask)
{
  UINT16 tdata;

  CHECKID(id);

  tdata = mask & 0x7FF;
  tdc1190WriteMicro(id,0x3900);
  return tdc1190WriteMicro(id,tdata);

}

int
tdc1190GetTDCErrorType(int id)
{
  UINT16 tdata;

  CHECKID(id);

  tdc1190WriteMicro(id,0x3A00);
  tdc1190ReadMicro(id,&tdata,1);

  tdata &= 0x7FF;
  if(tdata & 0x1)
    printf("%s(%d): Vernier error\n",__FUNCTION__,id);
  else if(tdata & 0x2)
    printf("%s(%d): Coarse error\n",__FUNCTION__,id);
  else if(tdata & 0x4)
    printf("%s(%d): Channel select error\n",__FUNCTION__,id);
  else if(tdata & 0x8)
    printf("%s(%d): L1 buffer parity error\n",__FUNCTION__,id);
  else if(tdata & 0x10)
    printf("%s(%d): Trigger fifo parity error\n",__FUNCTION__,id);
  else if(tdata & 0x20)
    printf("%s(%d): Trigger matching error\n",__FUNCTION__,id);
  else if(tdata & 0x40)
    printf("%s(%d): Readout fifo parity error\n",__FUNCTION__,id);
  else if(tdata & 0x80)
    printf("%s(%d): Readout state error\n",__FUNCTION__,id);
  else if(tdata & 0x100)
    printf("%s(%d): Set up parity error\n",__FUNCTION__,id);
  else if(tdata & 0x200)
    printf("%s(%d): Control parity error\n",__FUNCTION__,id);
  else if(tdata & 0x400)
    printf("%s(%d): Jtag instruction parity error\n",__FUNCTION__,id);

  return(tdata);
}

STATUS
tdc1190SetEffectiveSizeOfReadoutFIFO(int id, UINT32 nwords)
{
  UINT16 tdata;

  CHECKID(id);

  tdc1190WriteMicro(id,0x3B00);

  if(nwords==2)        tdata = 0;
  else if(nwords==4)   tdata = 1;
  else if(nwords==8)   tdata = 2;
  else if(nwords==16)  tdata = 3;
  else if(nwords==32)  tdata = 4;
  else if(nwords==64)  tdata = 5;
  else if(nwords==128) tdata = 6;
  else if(nwords==256) tdata = 7;
  else                 tdata = 7;

  printf("%s(%d): Set Effective Size Of Readout FIFO to %d\n",__FUNCTION__,id,nwords);

  return tdc1190WriteMicro(id,tdata);

}

int
tdc1190GetEffectiveSizeOfReadoutFIFO(int id)
{
  UINT16 tdata;

  CHECKID(id);

  tdc1190WriteMicro(id,0x3C00);
  tdc1190ReadMicro(id,&tdata,1);

  tdata &= 0x7;
  if(tdata==0)
    printf("%s(%d): 2 words\n",__FUNCTION__,id);
  else if(tdata==1)
    printf("%s(%d): 4 words\n",__FUNCTION__,id);
  else if(tdata==2)
    printf("%s(%d): 8 words\n",__FUNCTION__,id);
  else if(tdata==3)
    printf("%s(%d): 16 words\n",__FUNCTION__,id);
  else if(tdata==4)
    printf("%s(%d): 32 words\n",__FUNCTION__,id);
  else if(tdata==5)
    printf("%s(%d): 64 words\n",__FUNCTION__,id);
  else if(tdata==6)
    printf("%s(%d): 128 words\n",__FUNCTION__,id);
  else if(tdata==7)
    printf("%s(%d): 256 words\n",__FUNCTION__,id);

  return(tdata);
}

/******************************************************************************
 * CHANNEL ENABLE OPCODES Routines
 *
 *  All routines return OK or ERROR,
 *   unless otherwise specified.
 *
 * tdc1190EnableChannel
 * tdc1190DisableChannel
 * tdc1190EnableAllChannels
 * tdc1190DisableAllChannels
 * tdc1190EnableChannels
 * tdc1190GetChannels        - Returns mask of enabled channels
 *
*/


STATUS
tdc1190EnableChannel(int id, UINT16 channel)
{
  int opcode = 0x4000 + (channel & 0x7F);

  CHECKID(id);

  return tdc1190WriteMicro(id,opcode);
}

STATUS
tdc1190DisableChannel(int id, UINT16 channel)
{
  int opcode = 0x4100 + (channel & 0x7F);

  CHECKID(id);
  return tdc1190WriteMicro(id,opcode);
}

STATUS
tdc1190EnableAllChannels(int id)
{
  CHECKID(id);
  return tdc1190WriteMicro(id,0x4200);
}

STATUS
tdc1190DisableAllChannels(int id)
{
  CHECKID(id);
  tdc1190WriteMicro(id,0x4300);
  return(OK);
}

STATUS
tdc1190EnableChannels(int id, UINT16 channel_mask)
{
  CHECKID(id);

  tdc1190WriteMicro(id,0x4400);
  tdc1190WriteMicro(id,channel_mask&0xffff);
  tdc1190WriteMicro(id,(channel_mask&0xffff0000)>>16);
  return(OK);
}

int
tdc1190GetChannels(int id)
{
  int rval;
  UINT16 read1, read2;

  CHECKID(id);

  tdc1190WriteMicro(id,0x4500);
  tdc1190ReadMicro(id,&read1,1);
  tdc1190ReadMicro(id,&read2,1);

  rval = (read1) | (read2<<16);

  return(rval);
}


/******************************************************************************
 * ADVANCED OPCODES Routines
 *
 *  All routines return OK or ERROR,
 *   unless otherwise specified.
 *
 * tdc1190GetTDCError   - Returns internal error from selected TDC
 * tdc1190GetTDCDLLLock - Returns 
 *                         0 : DLL not in LOCK
 *                         1 : DLL in LOCK
 * 
*/


int
tdc1190GetTDCError(int id, UINT16 ntdc)
{
  int opcode = 0x7400 + (ntdc & 0x3);
  unsigned short tmp;
  int rval;

  CHECKID(id);

  tdc1190WriteMicro(id,opcode);
  tdc1190ReadMicro(id,&tmp,1);

  rval = tmp & 0x7FF;

  return(rval);
}

int
tdc1190GetTDCDLLLock(int id, UINT16 ntdc)
{
  int opcode = 0x7500 + (ntdc & 0x3);
  unsigned short tmp;
  int rval;

  CHECKID(id);

  tdc1190WriteMicro(id,opcode);
  tdc1190ReadMicro(id,&tmp,1);

  rval = tmp & 0x1;

  return(rval);
}

/******************************************************************************
 *
 * tdc1190InitMSCT     - Initialize MSCT for all initialized boards
 *                       vmeaddress is set to the MSCT address
 *                       Returns OK, if successful.  ERROR, otherwise.
 *
 */

STATUS
tdc1190InitMCST(UINT32 *vmeaddress)
{
  unsigned int baseadrs;
  int ii;

  baseadrs = (V1190_A32_ADDR >> 24) & 0xFF;
  baseadrs += (V1190_A32_MCST_OFFSET >> 24);

#ifdef VXWORKS
  sysBusToLocalAdrs(0x09,(char *)(baseadrs<<24),(char **)vmeaddress);
#else
  vmeBusToLocalAdrs(0x09,(char *)(baseadrs<<24),(char **)vmeaddress);
#endif
  c1190MCSTp = (struct v1190_struct *)(vmeaddress);

  printf("tdc1190InitMCST: MCST VME (USER) base address 0x%x (0x%x):\n",
	 (baseadrs<<24), (UINT32)c1190MCSTp);

  /* Loop through use1190/c1190p array to set mcstBaseAddr
     and set First/intermediate/Last boards */
  for(ii=0; ii<V1190_MAX_MODULES; ii++)
    {
      if(use1190[ii] == 0)
	continue;

      tdcWrite16(&(c1190p[ii]->mcstBaseAddr),(baseadrs & V1190_MCSTBASEADDR_MASK));

      if(ii==tdcMinSlot)
	{
	  tdcWrite16(&(c1190p[ii]->mcstCtrl),2); /* active first */
	  printf("\tFirst  board at 0x%08x\n",(UINT32)c1190p[ii]);
	}
      else if(ii==tdcMaxSlot)
	{
	  tdcWrite16(&(c1190p[ii]->mcstCtrl),1); /* active last */
	  printf("\tLast   board at 0x%08x\n",(UINT32)c1190p[ii]);
	}
      else
	{
	  tdcWrite16(&(c1190p[ii]->mcstCtrl),3); /* active intermediate */
	  printf("\tMiddle board at 0x%08x\n",(UINT32)c1190p[ii]);
	}
    }

  return(OK);
}


/******************************************************************************
 *
 * tdc1190Trig         - Issue Software trigger to TDC
 * tdc1190Clear        - Clear TDC
 * tdc1190Reset        - Clear/Reset TDC
 * tdc1190GetEventCounter - Return number of triggers since last clear/reset.
 * tdc1190GetEventStored  - Return number of events currently stored in 
 *                          output buffer
 * tdc1190SetAlmostFullLevel - Set value of number of words in output buffer
 *                             when an IRQ is generated (if enabled) and
 *                             correspond bit in Status reg is set.
 * tdc1190SetOutProg - Set the function for the output on the control connector
 *                     0 : Data Ready
 *                     1 : Full
 *                     2 : Almost Full
 *                     3 : Error
 * tdc1190GetOutProg - Get value set for the output on the control connector.
 * tdc1190TestMode   - Enable (off=0) or disable (off=1) Test mode
 * tdc1190Test       - Set a value (tval) into the Testreg register
 * tdc1190SetGeoAddress - Set the GEO Address (contained in header/trailer words)
 * tdc1190GetGeoAddress - Get the GEO Address (contained in header/trailer words)
 *
 */

STATUS
tdc1190Trig(int id)
{
  CHECKID(id);
  LOCK_1190;
  tdcWrite16(&(c1190p[id]->trigger),1);
  UNLOCK_1190;
  return OK;
}

STATUS
tdc1190Clear(int id)
{
  CHECKID(id);
  LOCK_1190;
  tdcWrite16(&(c1190p[id]->clear),1);
  UNLOCK_1190;
  return OK;
}

STATUS
tdc1190Reset(int id)
{
  CHECKID(id);
  LOCK_1190;
  tdcWrite16(&(c1190p[id]->moduleReset),1);
  UNLOCK_1190;
  return OK;
}

int
tdc1190GetEventCounter(int id)
{
  int rval;

  CHECKID(id);
  LOCK_1190;
  rval = tdcRead32(&(c1190p[id]->evCount));
  UNLOCK_1190;
  return rval;
}

int
tdc1190GetEventStored(int id)
{
  int rval;

  CHECKID(id);
  LOCK_1190;
  rval = tdcRead16(&(c1190p[id]->evStored));
  UNLOCK_1190;
  return rval;
}

STATUS
tdc1190SetAlmostFullLevel(int id, UINT16 nwords)
{
  CHECKID(id);
  LOCK_1190;
  tdcWrite16(&(c1190p[id]->almostFullLevel),(nwords & 0x7FFF));
  UNLOCK_1190;
  return OK;
}

int
tdc1190GetAlmostFullLevel(int id)
{
  int rval;

  CHECKID(id);
  LOCK_1190;
  rval = tdcRead16(&(c1190p[id]->almostFullLevel));
  UNLOCK_1190;
  return rval;
}


STATUS
tdc1190SetOutProg(int id, UINT16 code)
{
  CHECKID(id);
  LOCK_1190;
  tdcWrite16(&(c1190p[id]->outProgControl),code & 0x3);
  UNLOCK_1190;
  return OK;
}

int
tdc1190GetOutProg(int id)
{
  int rval;

  CHECKID(id);
  LOCK_1190;
  rval = tdcRead16(&(c1190p[id]->outProgControl));
  UNLOCK_1190;
  return rval;
}

STATUS
tdc1190TestMode(int id, int off)
{
  CHECKID(id);
  /* FIXME: Test mode enabled/disabled through OPCODE */

  if(off) 
    tdcWrite16(&(c1190p[id]->control),
	       tdcRead16(&(c1190p[id]->control)) & (~V1190_TESTMODE&0xf));
  else
    tdcWrite16(&(c1190p[id]->control),
	       tdcRead16(&(c1190p[id]->control)) | (V1190_TESTMODE));

  return OK;
}

STATUS
tdc1190Test(int id, UINT16 tval)
{
  /* FIXME: Test mode enabled/disabled through OPCODE */
  UINT16 testMode=0;

  CHECKID(id);

  testMode = tdcRead16(&(c1190p[id]->control))&V1190_TESTMODE;
  if(testMode)
    {
      if(tval==0)
	tdcWrite32(&(c1190p[id]->testReg),0x11223344);
      else
	tdcWrite32(&(c1190p[id]->testReg),tval);
      /* FIXME: Maybe the below is better than above... I dont know... fix it later */
      /* 	(UINT16 *)(c1190p[id]->testReg) = tval; */
    }
  else
    {
      printf("tdc1190Test: ERROR: TestMode not enabled.\n");
    }
     
  return OK;
}


STATUS
tdc1190SetGeoAddress(int id, UINT16 geoaddr)
{
  CHECKID(id);
  LOCK_1190;
  tdcWrite16(&(c1190p[id]->geoAddr),geoaddr & 0x1F);
  UNLOCK_1190;
  return OK;
}

int
tdc1190GetGeoAddress(int id)
{
  int rval;

  CHECKID(id);
  LOCK_1190;
  rval = tdcRead16(&(c1190p[id]->geoAddr)) & 0x1F;
  UNLOCK_1190;
  return rval;
}


STATUS
tdc1190SetBLTEventNumber(int id, UINT16 nevents)
{
  CHECKID(id);
  LOCK_1190;
  tdcWrite16(&(c1190p[id]->bltEventNumber), nevents & 0xFF);
  UNLOCK_1190;
  return OK;
}

int
tdc1190GetBLTEventNumber(int id)
{
  int rval;

  CHECKID(id);
  LOCK_1190;
  rval = tdcRead16(&(c1190p[id]->bltEventNumber));
  UNLOCK_1190;
  return rval;
}




/***********************/
/* INTERRUPT FUNCTIONS */
/***********************/


/*******************************************************************************
 *
 * tdc1190Int - default interrupt handler
 *
 * This rountine handles the v1190 TDC interrupt.  A user routine is
 * called, if one was connected by tdc1190IntConnect().
 *
 * RETURNS: N/A
 *
 */

LOCAL void 
tdc1190Int (void)
{

  UINT32 nevt=0;
  
  /* Disable interrupts */
#ifdef VXWORKS
  sysIntDisable(c1190IntLevel);
#else
  vmeBusLock();
#endif

  c1190IntCount++;
 
  if (c1190IntRoutine != NULL)  {     /* call user routine */
    (*c1190IntRoutine) (c1190IntArg);
  }else{
    if((c1190IntID<0) || (c1190p[c1190IntID] == NULL)) {
      logMsg("c1190Int: ERROR : TDC id %d not initialized \n",c1190IntID,0,0,0,0,0);
      return;
    }

    logMsg("c1190Int: Processed %d events\n",nevt,0,0,0,0,0);

  }

  /* Enable interrupts */
#ifdef VXWORKS
  sysIntEnable(c1190IntLevel);
#else
  vmeBusUnlock();
#endif

}


/*******************************************************************************
 *
 * c1190IntConnect - connect a user routine to the v1190 TDC interrupt
 *
 * This routine specifies the user interrupt routine to be called at each
 * interrupt. 
 *
 * RETURNS: OK, or ERROR if Interrupts are enabled
 */

STATUS 
tdc1190IntConnect (VOIDFUNCPTR routine, int arg, UINT16 level, UINT16 vector)
{

  if(c1190IntRunning) {
    printf("c1190IntConnect: ERROR : Interrupts already Initialized for TDC id %d\n",
	   c1190IntID);
    return(ERROR);
  }
  
  c1190IntRoutine = routine;
  c1190IntArg = arg;

  /* Check for user defined VME interrupt level and vector */
  if(level == 0) {
    c1190IntLevel = V1190_VME_INT_LEVEL; /* use default */
  }else if (level > 7) {
    printf("c1190IntConnect: ERROR: Invalid VME interrupt level (%d). Must be (1-7)\n",level);
    return(ERROR);
  } else {
    c1190IntLevel = level;
  }

  if(vector == 0) {
    c1190IntVec = V1190_INT_VEC;  /* use default */
  }else if ((vector < 32)||(vector>255)) {
    printf("c1190IntConnect: ERROR: Invalid interrupt vector (%d). Must be (32<vector<255)\n",vector);
    return(ERROR);
  }else{
    c1190IntVec = vector;
  }
      
  /* Connect the ISR */
#ifdef VXWORKSPPC
  if((intDisconnect((int)INUM_TO_IVEC(c1190IntVec)) != 0)) {
    printf("c1190IntConnect: ERROR disconnecting Interrupt\n");
    return(ERROR);
  }
  if((intConnect(INUM_TO_IVEC(c1190IntVec),tdc1190Int,0)) != 0) {
    printf("c1190IntConnect: ERROR in intConnect()\n");
    return(ERROR);
  }
#else
  if(vmeIntConnect(c1190IntVec,c1190IntLevel,tdc1190Int,c1190IntArg) != OK)
    {
      printf("%s: ERROR: vmeIntConnect failed\n");
      return ERROR;
    }
#endif

  return (OK);
}


/*******************************************************************************
 *
 * c1190IntEnable - Enable interrupts from specified TDC
 *
 * Enables interrupts for a specified TDC.
 * 
 * RETURNS OK or ERROR if TDC is not available or parameter is out of range
 */

STATUS 
tdc1190IntEnable (int id, UINT16 evCnt)
{

  if(c1190IntRunning) {
    printf("c1190IntEnable: ERROR : Interrupts already initialized for TDC id %d\n",
	   c1190IntID);
    return(ERROR);
  }

  if((id<0) || (c1190p[id] == NULL)) {
    printf("c1190IntEnable: ERROR : TDC id %d not initialized \n",id);
    return(ERROR);
  }else{
    c1190IntID = id;
  }
  
  
#ifdef VXWORKS
  sysIntEnable(c1190IntLevel);   /* Enable VME interrupts */
#endif  

  /* Zero Counter and set Running Flag */
  c1190IntEvCount = evCnt;
  c1190IntCount = 0;
  c1190IntRunning = TRUE;
  
  return(OK);
}


/*******************************************************************************
 *
 * c1190IntDisable - disable the TDC interrupts
 *
 * RETURNS: OK, or ERROR if not initialized
 */

STATUS 
tdc1190IntDisable (int iflag)
{

  if((c1190IntID<0) || (c1190p[c1190IntID] == NULL)) {
    logMsg("c1190IntDisable: ERROR : TDC id %d not initialized \n",c1190IntID,0,0,0,0,0);
    return(ERROR);
  }

#ifdef VXWORKS
  sysIntDisable(c1190IntLevel);   /* Disable VME interrupts */
#endif

  /* Tell tasks that Interrupts have been disabled */
  if(iflag > 0) {
    c1190IntRunning = FALSE;
  }
  
  return (OK);
}

/*******************************************************************************
 *
 * tdcRead16         - Read a 16bit register
 * tdcWrite16        - Write to a 16bit register
 * tdcRead32         - Read a 32bit register
 * tdcWrite32        - Write to a 16bit register
 *
 */

static unsigned short
tdcRead16(volatile unsigned short *addr)
{
  unsigned short rval;
  rval = *addr;
#ifndef VXWORKS
  rval = SSWAP(rval);
#endif
  return rval;
}

static void
tdcWrite16(volatile unsigned short *addr, unsigned short val)
{
#ifndef VXWORKS
  val = SSWAP(val);
#endif
  *addr = val;
  return;
}

static unsigned int
tdcRead32(volatile unsigned int *addr)
{
  unsigned int rval;
  rval = *addr;
#ifndef VXWORKS
  rval = LSWAP(rval);
#endif
  return rval;
}

static void
tdcWrite32(volatile unsigned int *addr, unsigned int val)
{
#ifndef VXWORKS
  val = LSWAP(val);
#endif
  *addr = val;
  return;
}
