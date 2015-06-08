/*
 *-----------------------OPYRIGHT NOTICE
 *
 *       Copyright (C) 1997 VME Microsystems International Corporation
 *       International copyright secured.  All rights reserved.
 *--------------------------------------------------------------------------
 * drv6016.c - 6016 board Device Driver
 *--------------------------------------------------------------------------
 */

/*--------------------------------------------------------------------------
 *
 *  CFT release per SAR 9806030256 - JHL 06/04/98
 *  Made modifications to release version 1.0 to correct problem described
 *  by PR number 980092 and SAR 9806030256 Problem Description section.
 *  Actual source code modifications are as follows:  Added declaration of
 *  global semaphore identifier semWrite, created semaphore with global
 *  semaphore identifier semWrite in function v6016Drv, inside function
 *  v6016Write, protected write function calls with semaphore semWrite.
 *
 */

#include <vxWorks.h>
#include <intLib.h>
#include <logLib.h>
#include <iosLib.h>
#include <semLib.h>
#include <sysLib.h>
#include <tyLib.h>
#include <stdlib.h>
#include <iv.h>
#include <stdio.h>
#include <string.h>
#include <taskLib.h>
#include <vxLib.h>
#include "dev6016.h"
#include "drv6016.h"


/*#define DEBUG 1*/ /* Define as 1 for debug */

#define ONE_SEC             (sysClkRateGet())
#define TEN_SECONDS         (ONE_SEC*10)
#define ALLOC_INC           10
#define ERROR_INT_MASK      0x10000L
#define BYTE_MASK           0xff
#define MAX_BUFFER_SIZE     256
#define BUFFER_INC          4096L
#define ROM_VER_SHIFT       8
#define SLAVE_BYTES_PER_GRN 128
#define INPUT_PTR_INDEX     0
#define OUTPUT_PTR_INDEX    2
#define HUNDRED_TIMES       100
#define ENDIAN              V_BIG_ENDIAN
#define BUS_TIMEOUT         1000000L
#define SLAVE_RING_START    4
#define BR_LEVEL_SHIFT      6
#define VALID_TESTS         0x3f
#define CHANNEL_REG_OFFSET  0x20
#define CHANNEL_REG_SIZE    0x0e
#define V_A32SD             0
#define V_A32UD             1
#define V_A24SD             2
#define V_A24UD             3


/* Type Definitions */

/* The following structure is an interrupt notification entry */
typedef struct NotifyEntry
{
  ULONG   NotifyConditions; /* Conditions to be notified */
  SEM_ID  NotifySem;        /* Semaphore to signal */
} NOTIFY_ENTRY;

/* Shape of the contents of an Interrupt Notification List */
typedef struct NotifyList
{
  unsigned int  uiListSize;       /* The size of the list */
  unsigned int  uiNumListEntries; /* Counts list entries */
  NOTIFY_ENTRY  *IntNotifyEntry;  /* Notification entries */
} NOTIFY_LIST;

struct CardData;
/* Shape of information structure for one channel on one card */
typedef struct ChannelData
{
  TY_DEV           DevHdr;          /* Device Header                */
  BOOL             IsOpen;          /* Board is Opened		        */
  BOOL             bIsTTY;          /* Indicates if TTY device      */
  V6016_STATUS     ErrCode;         /* The Device Error		        */
  UINT             Channel;         /* Number of Channels		    */
  SEM_ID           AccessSem;       /* Exclusive Access Semaphore   */
  UINT             InterruptLevel;  /* Interrupt Level		        */
  UINT             InterruptVector; /* Interrupt Vector		        */
  FUNCPTR          OldIntVector;    /* Old Interrupt Vector		    */
  NOTIFY_LIST      IntNotifyList;   /* Interrupt Notification List  */
  struct CardData *pCardData;       /* Back link to Card Data	    */
} CHANNEL_DATA;

/* Shape of the device structure for a 6016 card */
typedef struct CardData
{
  ULONG         BoardAddress;    /* Card Base Address	      */
  ULONG         BusRequestLevel; /* Bus Request Level		  */
  UINT          InterruptLevel;  /* Interrupt Level		      */
  UINT          InterruptVector; /* Interrupt Vector          */
  FUNCPTR       OldIntVector;    /* Old Int. Vector		      */
  UINT          NumChannels;     /* Number of Channels		  */
  SEM_ID        GlobalRegSem;    /* Semaphore for global regs */
  CHANNEL_DATA  ChannelData[CHANNELS_PER_CARD]; /* Per Channel */
} CARD_DATA;


/* Local Data */
LOCAL CARD_DATA **CardList = (CARD_DATA **) NULL;
LOCAL UINT        CardListSize = 0;
LOCAL int         v6016DrvNum = ERROR;


/* Local Function Prototypes */

LOCAL int           v6016Drv(void);
LOCAL int           v6016Open(CHANNEL_DATA *theChannelData,
                              char *remainder,
                              int mode);
LOCAL STATUS        v6016Close(CHANNEL_DATA *theChannelData);
LOCAL STATUS        v6016Ioctl(CHANNEL_DATA *theChannelData,
                               int iCommand,
                               void *pData);
LOCAL int           v6016Read(CHANNEL_DATA *theChannelData,
                              char *buffer,
                              size_t maxbytes);
LOCAL int           v6016Write(CHANNEL_DATA *theChannelData,
                               char *buffer,
                               size_t maxbytes);

LOCAL STATUS        RemoveCard(CARD_DATA * theCardData);
LOCAL int           Read6016Data(CARD_DATA * theCardData,
                                 CHANNEL_DATA *theChannelData,
			                     UINT Channel,
			                     UCHAR * puchBuffer,
			                     UINT BytesToRead);
LOCAL int           Write6016Data(CARD_DATA * theCardData,
                                  CHANNEL_DATA *theChannelData,
				                  UINT Channel,
				                  UCHAR * puchBuffer,
				                  UINT BytesToWrite);
LOCAL V6016_STATUS  Config6016Buffer(CARD_DATA * theCardData,
                                     CHANNEL_DATA *theChannelData,
				                     ULONG BufferAddress,
				                     RELEASE_MODE ReleaseMode,
				                     UINT FairnessTimeout,
				                     BOOL UseA32,
				                     BOOL UseSupervisor);
LOCAL V6016_STATUS  Send6016Break(CARD_DATA * theCardData,
                                  CHANNEL_DATA *theChannelData,
                                  UINT Channel);
LOCAL V6016_STATUS  Run6016Selftest(CARD_DATA * theCardData,
                                    CHANNEL_DATA *theChannelData,
				                    SELFTESTS Selftests);
LOCAL V6016_STATUS  Reset6016Board(CARD_DATA * theCardData,
                                   CHANNEL_DATA *theChannelData);
LOCAL V6016_STATUS  Config6016Controller(CARD_DATA * theCardData,
                                         CHANNEL_DATA *theChannelData,
				                         ARB_MODE ArbitrationMode,
				                         UINT BusErrorTimeout);
LOCAL V6016_STATUS  Set6016ChannelEnable(CARD_DATA * theCardData,
                                         CHANNEL_DATA *theChannelData,
					                     UINT Channel,
					                     BOOL Enable);
LOCAL V6016_STATUS  Config6016Errors(CARD_DATA * theCardData,
                                     CHANNEL_DATA *theChannelData,
				                     UINT OredErrors);
LOCAL V6016_STATUS  Read6016GlobalRegister(CARD_DATA * theCardData,
                                           CHANNEL_DATA *theChannelData,
					                       GLOBAL_REG GlobalReg,
					                       REGISTER_VALUE_TYPE * pValue);
LOCAL V6016_STATUS  Write6016GlobalRegister(CARD_DATA * theCardData,
                                            CHANNEL_DATA *theChannelData,
						                    GLOBAL_REG GlobalReg,
					                        REGISTER_VALUE_TYPE * pValue);
LOCAL V6016_STATUS  Request6016IntNotify(CARD_DATA * theCardData,
                                         CHANNEL_DATA *theChannelData,
                                         UINT Channel,
					                     ULONG NotifyConditions,
					                     SEM_ID SemID);
LOCAL V6016_STATUS  Config6016Channel(CARD_DATA * theCardData,
                                      CHANNEL_DATA *theChannelData,
				                      UINT Channel,
				                      FLOW_CTRL FlowControl,
				                      BAUD_RATE BaudRate,
				                      TIMEOUTS InputTimeout,
					                  STOP_BITS StopBits,
					                  PARITY_TYPE ParityType,
					                  PARITY_SENSE ParitySense,
					                  NUM_DATA_BITS NumBits);
LOCAL V6016_STATUS  Get6016DeviceError(CHANNEL_DATA *theChannelData,
				                       V6016_STATUS *pStatus);
LOCAL V6016_STATUS  Read6016ChannelRegister(CARD_DATA *theCardData,
                                            CHANNEL_DATA *theChannelData,
						                    UINT Channel,
					                        CHANNEL_REG ChannelReg,
				                            REGISTER_VALUE_TYPE *pValue);
LOCAL V6016_STATUS  Write6016ChannelRegister(CARD_DATA *theCardData,
                                             CHANNEL_DATA *theChannelData,
						                     UINT Channel,
						                     CHANNEL_REG ChannelReg,
					                         REGISTER_VALUE_TYPE *pValue);
LOCAL V6016_STATUS  Read6016Buffer(CARD_DATA *theCardData,
                                   CHANNEL_DATA *theChannelData,
			                       UINT Channel,
			                       BUFFER_TYPE WhichBuffer,
			                       ULONG Offset,
			                       UINT ByteCount,
				                   UCHAR *pBuffer);
LOCAL V6016_STATUS  Write6016Buffer(CARD_DATA * theCardData,
                                    CHANNEL_DATA *theChannelData,
					                UINT Channel,
					                BUFFER_TYPE WhichBuffer,
					                ULONG Offset,
					                UINT ByteCount,
				                    UCHAR *pBuffer);
LOCAL ULONG         ErrorIntHandler(ULONG VectorID);
LOCAL ULONG         ChannelIntHandler(ULONG VectorID);
LOCAL V6016_STATUS  WriteBusyReg(CARD_DATA *theCardData,
                                 CHANNEL_DATA *theChannelData,
				                 GLOBAL_REG Register,
				                 REGISTER_VALUE_TYPE *Value);
LOCAL V6016_STATUS  AllocateNotifySpace(NOTIFY_LIST *IntNotifyList);
LOCAL V6016_STATUS  StartChannel(CARD_DATA *theCardData,
                                 CHANNEL_DATA *theChannelData);
LOCAL V6016_STATUS  StopChannel(CARD_DATA *theCardData,
                                CHANNEL_DATA *theChannelData);
LOCAL ULONG         ReadAccessMode(CARD_DATA *theCardData,
                                   CHANNEL_DATA *theChannelData);
LOCAL UCHAR         GetChannelMask(CHANNEL_DATA *theChannelData);
LOCAL VOID          tyCoStartup(CHANNEL_DATA *theChannelData);
LOCAL VOID          InitializeTTY(CHANNEL_DATA *theChannelData);


/* ... */

SEM_ID semWrite;


/*********************************************************/
/* top-level functions: driver installation and removing */
/*********************************************************/

/* install one 6016 device
 *------------------------------------------------------------------------
 * This routine should be called for once for each 6016 card. It will
 * verify that it can communicate with the 6016 card, initialize the 6016
 * card and create and initialize the board device structure.
 *------------------------------------------------------------------------*/

V6016_STATUS
v6016DevCreate(char *pszName, ULONG CardAddress, ULONG BusRequestLevel,
               UINT InterruptLevel1, UINT InterruptVector1,
               UINT InterruptLevel2, UINT InterruptVector2,
              BOOL *pbTTYDeviceArray)
{
  CARD_DATA             *theCardData;
  CHANNEL_DATA          *theChannelData;
  UINT                   Channel;
  UINT                   TimeoutCount;
  REGISTER_VALUE_TYPE    Value;
  UCHAR                  BoardID;
  char                   pszDevName[80];

  VOIDFUNCPTR *vector1, *vector2;
  FUNCPTR oldfunc1, newfunc1, oldfunc2, newfunc2;

  /* Check all the parameters */
  if((BusRequestLevel  >    3) ||
     (InterruptLevel1  <    1) ||
     (InterruptLevel1  >    7) ||
     (InterruptVector1 > 0xFF) ||
     (InterruptLevel2  <    1) ||
     (InterruptLevel2  >    7) ||
     (InterruptVector2 > 0xFF))
  {
    return(INVALID_PARAMETER_ERR);
  }

  /* See if we can read the board ID register */
  if(vxMemProbe((char *)CardAddress, VX_READ,
     1, (char *) &BoardID) == ERROR)
  {
#if	DEBUG
    logMsg("ERROR: vxMemProbe Failed",0,0,0,0,0,0);
#endif
    return(INVALID_BOARD_ID_ERR);
  }

  /* See if we got one of the right values */
  if(BoardID != 0x3A)
  {
#if	DEBUG
    logMsg("ERROR: Invalid Board ID = %x", (int)BoardID,0,0,0,0,0);
#endif
    return(INVALID_BOARD_ID_ERR);
  }

  /*
   * If we have not successfully obtained a driver number, then try
   * to get it.
   */
  if(v6016DrvNum == ERROR)
  {
    v6016DrvNum = v6016Drv();
  }

  /* If we failed to get a driver number, then leave */
  if(v6016DrvNum == ERROR)
  {
    return(INVALID_BOARD_ID_ERR);
  }

  /* Create the device structure for this card */
  theCardData = (CARD_DATA *) malloc( sizeof(CARD_DATA) );
  if(theCardData == ((CARD_DATA *) NULL))
  {
#if	DEBUG
    logMsg("ERROR: Cannot allocate memory for device structure\n",
           0,0,0,0,0,0);
#endif
    return(MEMORY_ALLOCATION_ERROR);
  }

  /* Store the Card Table into the list */
  if((CardList == ((CARD_DATA **) NULL)) || (!CardListSize))
  {
    CardListSize = 0;
    CardList = (CARD_DATA **) malloc(sizeof(CARD_DATA *));
  }
  else
  {
    CardList = (CARD_DATA **) realloc( CardList,
				sizeof(CARD_DATA *) * (CardListSize + 1) );
  }

  /* Verify we have a card list */
  if(CardList == ((CARD_DATA **) NULL))
  {
    CardListSize = 0;
#if	DEBUG
    logMsg("ERROR: Cannot allocate memory for card list\n",0,0,0,0,0,0);
#endif
    free(theCardData);
    return(MEMORY_ALLOCATION_ERROR);
  }

  /* Store this card in the card list */
  CardList[CardListSize] = theCardData;
  CardListSize++;

  /* Initialize the per card data */
  theCardData->BoardAddress = CardAddress;
  theCardData->BusRequestLevel = BusRequestLevel;
  theCardData->InterruptLevel = InterruptLevel1;
  theCardData->InterruptVector = InterruptVector1;
  theCardData->NumChannels = 16;
  theCardData->GlobalRegSem = semBCreate(SEM_Q_FIFO, SEM_FULL);

  /* Wait for the POR selftest status flag register to clear */
  TimeoutCount = 0;
  do
  {
    if(Read6016GlobalRegister(theCardData, &(theCardData->ChannelData[0]),
         STFLAG_REG, &Value) != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Read in Init6016Card\n",0,0,0,0,0,0);
#endif
      free(theCardData);
      return(BOARD_ACCESS_ERR);
    }

    /* Increment timeout count */
    if(TimeoutCount > 0)
    {
      taskDelay(ONE_SEC/10);
    }
    TimeoutCount++;
  } while((Value.ByteValue != 0) && (TimeoutCount < HUNDRED_TIMES));

  /* See if we timed out */
  if(Value.ByteValue != 0)
  {
#if	DEBUG
    logMsg("ERROR: 6016 Card Selftest Not Complete\n",0,0,0,0,0,0);
#endif
    free(theCardData);
    return(CARD_SELFTEST_ERR);
  }

  /* Find out how many channels there really is */
  if(Read6016GlobalRegister(theCardData, &(theCardData->ChannelData[0]),
       ROM_VER_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in Init6016Card\n",0,0,0,0,0,0);
#endif
    free(theCardData);
    return(BOARD_ACCESS_ERR);
  }
  theCardData->NumChannels = (Value.WordValue & CHAN_OPT) ? 8 : 16;

  /* Write the bus request level */
  Value.ByteValue = (UCHAR) ((BusRequestLevel << BR_LEVEL_SHIFT)|RELROR);
  if(Write6016GlobalRegister(theCardData, &(theCardData->ChannelData[0]),
       CR2_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write in Init6016Card\n",0,0,0,0,0,0);
#endif
    free(theCardData);
    return(BOARD_ACCESS_ERR);
  }


  /******************/
  /* set interrupts */
  /******************/

  /* Initialize the error interrupt control register */
  Value.ByteValue = HDWE_ERR_MSK | BERR_MASK | InterruptLevel1;
  if(Write6016GlobalRegister(theCardData, &(theCardData->ChannelData[0]),
     ER_MSK_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write in Init6016Card\n",0,0,0,0,0,0);
#endif
    free(theCardData);
    return(BOARD_ACCESS_ERR);
  }

  /* Initialize the error interrupt vector register */
  Value.ByteValue = InterruptVector1;
  if(Write6016GlobalRegister(theCardData, &(theCardData->ChannelData[0]),
     ER_VEC_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write in Init6016Card\n",0,0,0,0,0,0);
#endif
    free(theCardData);
    return(BOARD_ACCESS_ERR);
  }

  /* convert vectors */
  vector1 = INUM_TO_IVEC(InterruptVector1);
#ifdef DEBUG
  logMsg("interrupt number %x(%d) -> interrupt vector %x(%d)\n",
       InterruptVector1,InterruptVector1,(int)vector1,(int)vector1,5,6);
#endif
  vector2 = INUM_TO_IVEC(InterruptVector2);
#ifdef DEBUG
  logMsg("interrupt number %x(%d) -> interrupt vector %x(%d)\n",
       InterruptVector2,InterruptVector2,(int)vector2,(int)vector2,5,6);
#endif

#ifdef VXWORKSPPC
  if((intDisconnect((int)vector1) != 0))
  {
    logMsg("error disconnecting interrupt 1\n",1,2,3,4,5,6);
    return(-1);
  }
  else
  {
#ifdef DEBUG
    logMsg("interrupt handler 1 disconnected\n",1,2,3,4,5,6);
#endif
  }
  if((intDisconnect((int)vector2) != 0))
  {
    logMsg("error disconnecting interrupt 2\n",1,2,3,4,5,6);
    return(-1);
  }
  else
  {
#ifdef DEBUG
    logMsg("interrupt handler 2 disconnected\n",1,2,3,4,5,6);
#endif
  }
#endif

  /* .. */
  oldfunc1 = intVecGet((FUNCPTR *)vector1);
#ifdef DEBUG
  logMsg("oldfunc1 = %08x\n",(int)oldfunc1,2,3,4,5,6);
#endif
  oldfunc2 = intVecGet((FUNCPTR *)vector2);
#ifdef DEBUG
  logMsg("oldfunc2 = %08x\n",(int)oldfunc2,2,3,4,5,6);
#endif

  /* Save the current interrupt functions */
  theCardData->OldIntVector = intVecGet((FUNCPTR *)vector1);
  theCardData->ChannelData[0].OldIntVector = intVecGet((FUNCPTR *)vector2);

  /* Connect to the error and channel interrupts */
  if((intConnect(vector1,
        (VOIDFUNCPTR) ErrorIntHandler, (int)InterruptVector1) != OK) ||
     (intConnect(vector2,
        (VOIDFUNCPTR) ChannelIntHandler, (int)InterruptVector2) != OK) )
  {
#if	DEBUG
    logMsg("ERROR: Failed to Install Interrupt Handlers in Init6016Card\n",
           0,0,0,0,0,0);
#endif
    free(theCardData);
    return(RESOURCE_ERR);
  }

  /* .. */
  newfunc1 = intVecGet((FUNCPTR *)vector1);
#ifdef DEBUG
  logMsg("newfunc1 = %08x\n",(int)newfunc1,2,3,4,5,6);
#endif
  newfunc2 = intVecGet((FUNCPTR *)vector2);
#ifdef DEBUG
  logMsg("newfunc2 = %08x\n",(int)newfunc2,2,3,4,5,6);
#endif

#ifdef VXWORKSPPC
#ifdef DEBUG
  logMsg("enable MPIC interrupts for vector1 = 0x%x\n",(int)vector1,2,3,4,5,6);
#endif
  intEnable(InterruptVector1);
#ifdef DEBUG
  logMsg("enable MPIC interrupts for vector2 = 0x%x\n",(int)vector2,2,3,4,5,6);
#endif
  intEnable(InterruptVector2);
#endif


  /* Run through each channel and initialize
     the per channel data and set up the interrupts */
  for(Channel = 0; Channel < theCardData->NumChannels; Channel++)
  {
#if 0
printf("chan=%2d: befor 0x%08x\n",Channel,theChannelData->InterruptVector);
#endif
    /* Get pointer to the channel data */
    theChannelData = (CHANNEL_DATA *)
  		&(theCardData->ChannelData[Channel]);
  	/* Initialize the per channel data */
    theChannelData->ErrCode = NO_ERR;
	theChannelData->IsOpen = FALSE;
  	theChannelData->Channel = Channel;
   	theChannelData->AccessSem = semBCreate(SEM_Q_FIFO, SEM_FULL);
   	theChannelData->InterruptLevel = InterruptLevel2;
   	theChannelData->InterruptVector = InterruptVector2;
   	theChannelData->IntNotifyList.uiListSize = 0;
   	theChannelData->IntNotifyList.uiNumListEntries = 0;
   	theChannelData->IntNotifyList.IntNotifyEntry = (NOTIFY_ENTRY *) NULL;
   	theChannelData->pCardData = theCardData;
#if 0
printf("chan=%2d: after 0x%08x\n",Channel,theChannelData->InterruptVector);
#endif
    /* Make sure we got a semaphore */
    if( theChannelData->AccessSem == (SEM_ID) NULL )
    {
#if	DEBUG
      logMsg("ERROR: Cannot allocate semaphore in Init6016Card\n",
			 0,0,0,0,0,0);
#endif
	  theChannelData->ErrCode = RESOURCE_ERR;
	  free(theCardData);
	  return(RESOURCE_ERR);
	}

	/* Initialize the error interrupt control register */
   	Value.ByteValue = InterruptLevel2;
   	if(Write6016ChannelRegister(theCardData, theChannelData, Channel,
           CH_MSK_REG, &Value) != NO_ERR)
    {
#if	DEBUG
	  logMsg("ERROR: Failed to Write Channel Register in Init6016Card\n",
             0,0,0,0,0,0);
#endif
   	  theChannelData->ErrCode = BOARD_ACCESS_ERR;
	  semDelete(theChannelData->AccessSem);
	  free(theCardData);
	  return(BOARD_ACCESS_ERR);
	}

	/* Initialize the error interrupt vector register */
	Value.ByteValue = InterruptVector2;
	if(Write6016ChannelRegister(theCardData, theChannelData, Channel,
            CH_VEC_REG, &Value) != NO_ERR)
    {
#if	DEBUG
	  logMsg("ERROR: Failed to Write Channel Register in Init6016Card\n",
             0,0,0,0,0,0);
#endif
	  theChannelData->ErrCode = BOARD_ACCESS_ERR;
	  semDelete(theChannelData->AccessSem);
	  free(theCardData);
	  return(BOARD_ACCESS_ERR);
	}

    if((pbTTYDeviceArray) && (pbTTYDeviceArray[Channel]))
    {
      if(tyDevInit((TY_DEV_ID) theChannelData, 1024, 1024,
                           (FUNCPTR) tyCoStartup) != OK)
      {
        logMsg("tyDevInit() failed !!!\n",0,0,0,0,0,0);
	    return(DEVICE_ADD_ERROR);
      }
	  /* Indicate this IS a TTY Device */
      theChannelData->bIsTTY = TRUE;
#ifdef DEBUG
      logMsg("This IS a TTY Device\n",0,0,0,0,0,0);
#endif
    }
    else
    {
	  /* Indicate this is NOT a TTY Device */
      theChannelData->bIsTTY = FALSE;
#ifdef DEBUG
      logMsg("This IS NOT a TTY Device: %d %d\n",
        (int)pbTTYDeviceArray,pbTTYDeviceArray[Channel],0,0,0,0);
#endif
    }

    /* Add device to operating system */
    sprintf(pszDevName, "%s%d", pszName, Channel);
#ifdef DEBUG
    logMsg("Adding device to the operating system .. ",0,0,0,0,0,0);
#endif
	if(iosDevAdd((DEV_HDR *)theChannelData, pszDevName, v6016DrvNum) == ERROR)
    {
      /* Return an error */
#ifdef DEBUG
      logMsg("failed !!!\n",0,0,0,0,0,0);
#endif
      return(DEVICE_ADD_ERROR);
    }
    else
    {
#ifdef DEBUG
      logMsg("done.\n",0,0,0,0,0,0);
#endif
    }
  }

#if 0
#warning checking Race Conditions problems
  /* Write the Command Register 1 Value */
  Value.ByteValue = 0x17;
  if(WriteBusyReg(theCardData, theChannelData, CTR1_REG, &Value) != NO_ERR)
  {
#if 0	
    logMsg("ERROR: Failed to Write Register in Config6016Controller\n",
  	     0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
#endif

  /* Enable the VME Bus interrupt levels for the two interrupts	*/
  sysIntEnable(InterruptLevel1);
  sysIntEnable(InterruptLevel2);
#ifdef DEBUG
  logMsg("INFO: interrupt enabled\n",0,0,0,0,0,0);
#endif

  return(NO_ERR); /* Return success */
}

/* remove the driver (closes all devices) */
STATUS
v6016DrvRemove(void)
{
  STATUS status = OK;

  /* Remove driver, forcing the closing of all devices */
  if(v6016DrvNum != ERROR)
  {
    status = iosDrvRemove(v6016DrvNum, TRUE);
    v6016DrvNum = ERROR;
  }

  /* Free memory for board table */
  if(CardList != ((CARD_DATA **) NULL))
  {
    /* Free resources for each board */
    while(CardListSize)
    {
      /* Make sure we have a board structure */
      if(CardList[0])
      {
        RemoveCard(CardList[0]);
      }
    }
    free( CardList );
    CardList = (CARD_DATA **) NULL;
  }
  return(status); /* Return the status */
}


/****************************/
/* driver-related functions */
/****************************/

/* install the device driver (call only once) */
LOCAL int 
v6016Drv(void)
{
  int drvNum;

  /* Install the driver entry points and return the driver number or ERROR */
  drvNum = iosDrvInstall(0, 0, (FUNCPTR) v6016Open, (FUNCPTR) v6016Close,
           (FUNCPTR) v6016Read, (FUNCPTR) v6016Write, (FUNCPTR) v6016Ioctl);
#if	DEBUG
  if(drvNum == ERROR)
  {
    logMsg("v6016Drv: ERROR installing driver\n",0,0,0,0,0,0);
  }
  else
  {
    logMsg("v6016Drv: drvNum=%d",drvNum,0,0,0,0,0);
  }
#endif

  semWrite = semBCreate(SEM_Q_FIFO, SEM_FULL);
#if	DEBUG
  if(semWrite == NULL)
  {
    logMsg("v6016Drv: ERROR creating binary semaphore\n",0,0,0,0,0,0);
  }
  else
  {
    logMsg("v6016Drv: semWrite=%d (0x%08x)",
           (int)semWrite,(int)semWrite,0,0,0,0);
  }
#endif

  return(drvNum);
}

/* open a device */
LOCAL int
v6016Open(CHANNEL_DATA *theChannelData, char *remainder, int mode)
{
 /*
  * Make sure we have a valid device structure. There is nothing we
  * can do to store an error here since we don't know what channel
  * this was for.
  */
  if(theChannelData == ((CHANNEL_DATA *) NULL)) return(ERROR);

  /* Make sure the device was not already open */
  if(theChannelData->IsOpen)
  {
    theChannelData->ErrCode = ALREADY_OPEN_ERROR;
    return(ERROR);
  }
  theChannelData->IsOpen = TRUE; /* Set device as opened */

  /* Initialize card if this is a TTY Channel. */
  if(theChannelData->bIsTTY) InitializeTTY(theChannelData);

  return((int)theChannelData); /* Return the device structure */
}

/* close a device */
LOCAL STATUS 
v6016Close(CHANNEL_DATA *theChannelData)
{
  /*
   * Make sure we got a valid device structure. There is nothing we
   * can do to store an error here since we don't know what channel
   * this was for.
   */
  if(theChannelData == ((CHANNEL_DATA *) NULL))
  {
    return(ERROR);
  }

  /* Make sure the device was already open */
  if( !theChannelData->IsOpen )
  {
    theChannelData->ErrCode = NOT_OPEN_ERROR;
    return( ERROR );
  }
  theChannelData->IsOpen = FALSE;	/* Set device as not opened	 */
  return(OK);
}

/* */
LOCAL int
v6016Read(CHANNEL_DATA *theChannelData, char *buffer, size_t maxbytes)
{
  if(theChannelData->bIsTTY)
  {
    return(tyRead ((TY_DEV_ID) theChannelData, buffer, maxbytes));
  }
  else
  {
    return(Read6016Data(theChannelData->pCardData, theChannelData,
                        theChannelData->Channel, (UCHAR *) buffer, maxbytes));
  }
}

/* */
LOCAL int
v6016Write(CHANNEL_DATA *theChannelData, char *buffer, size_t maxbytes)
{
  int numBytes;

  semTake(semWrite, WAIT_FOREVER);

  if(theChannelData->bIsTTY)
  {
    numBytes = tyWrite ((TY_DEV_ID) theChannelData, buffer, maxbytes);
  }
  else
  {
    numBytes = Write6016Data(theChannelData->pCardData, theChannelData,
                             theChannelData->Channel, (UCHAR *) buffer,
                             maxbytes);
  }

  semGive(semWrite);

  return(numBytes);
}

/* service ioctl(3) functions */
LOCAL STATUS
v6016Ioctl(CHANNEL_DATA *theChannelData, int iCommand, void *pData)
{
  CARD_DATA           *theCardData = (CARD_DATA *) theChannelData->pCardData;
  STATUS              Status = ERROR;
  int                 arg = (int) pData;
  BAUD_RATE           NewBaudRate;
  int                 oldLevel;
  REGISTER_VALUE_TYPE Value;

  /* Make sure we got a valid device structure. There is nothing we
     can do to store an error here since we don't know what channel
     this was for */
  if(theCardData == ((CARD_DATA *) NULL)) return( ERROR );

  /* Make sure the device was already open */
  if(!theChannelData->IsOpen)
  {
    theChannelData->ErrCode = NOT_OPEN_ERROR;
    return(ERROR);
  }

  /* Figure out which command is requested */
  switch(iCommand)
  {
    case FIOBAUDRATE:
      if(!theChannelData->bIsTTY)
      {
        theChannelData->ErrCode = NOT_OPEN_ERROR;
        return(ERROR);
      }
      switch(arg)
      {
        case 50:
          NewBaudRate = BAUDS_50;
          break;
        case 75:
          NewBaudRate = BAUDS_75;
          break;
        case 110:
          NewBaudRate = BAUDS_110;
          break;
        case 134:
          NewBaudRate = BAUDS_134p5;
          break;
        case 150:
          NewBaudRate = BAUDS_150;
          break;
        case 200:
          NewBaudRate = BAUDS_200;
          break;
        case 300:
          NewBaudRate = BAUDS_300;
          break;
        case 600:
          NewBaudRate = BAUDS_600;
          break;
        case 1050:
          NewBaudRate = BAUDS_1050;
          break;
        case 1200:
          NewBaudRate = BAUDS_1200;
          break;
        case 1800:
          NewBaudRate = BAUDS_1800;
          break;
        case 2000:
          NewBaudRate = BAUDS_2000;
          break;
        case 2400:
          NewBaudRate = BAUDS_2400;
          break;
        case 4800:
          NewBaudRate = BAUDS_4800;
          break;
        case 7200:
          NewBaudRate = BAUDS_7200;
          break;
        case 9600:
          NewBaudRate = BAUDS_9600;
          break;
        case 19200:
          NewBaudRate = BAUDS_19200;
          break;
        case 38400:
          NewBaudRate = BAUDS_38400;
          break;
        default:
          theChannelData->ErrCode = INVALID_PARAMETER_ERR;
          return(ERROR);
          break;
      }
      oldLevel = intLock();
      if(Read6016ChannelRegister(theChannelData->pCardData, theChannelData,
                                 theChannelData->Channel, CH_CON1_REG,
                                 &Value) != NO_ERR)
      {
        Status = ERROR;
      }
      else
      {
        StopChannel(theChannelData->pCardData, theChannelData);
        Value.ByteValue = (Value.ByteValue & 0xE0) | NewBaudRate;
        if(Write6016ChannelRegister(theChannelData->pCardData, theChannelData,
                                    theChannelData->Channel, CH_CON1_REG,
                                    &Value) != NO_ERR)
        {
          Status = ERROR;
        }
        StartChannel(theChannelData->pCardData, theChannelData);
      }
      intUnlock (oldLevel);
      Status = OK;
      break;

    default:
      if(!theChannelData->bIsTTY)
      {
        theChannelData->ErrCode = INVALID_PARAMETER_ERR;
        Status = ERROR;
      }
      else
      {
        Status = tyIoctl(&theChannelData->DevHdr, iCommand, arg);
        if(Status != OK)
        {
          theChannelData->ErrCode = INVALID_PARAMETER_ERR;
        }
      }
      break;

	case NO_COMMAND:
      Status = OK;
      theChannelData->ErrCode = NO_ERR;
      break;

    case CONFIG_6016_BUFFER:
      {
        CONFIG_BUFFER *pConfigBuffer = (CONFIG_BUFFER *) pData;
        if(pData == ((void *)NULL))
        {
          theChannelData->ErrCode = INVALID_PARAMETER_ERR;
          break;
        }
        if(Config6016Buffer(theCardData, theChannelData,
                            pConfigBuffer->BufferAddress,
                            pConfigBuffer->ReleaseMode,
                            pConfigBuffer->FairnessTimeout,
                            pConfigBuffer->UseA32,
                            pConfigBuffer->UseSupervisor) == NO_ERR)
        {
          Status = OK;
        }
      }
      break;

    case SEND_6016_BREAK:
      if(Send6016Break(theCardData,theChannelData,theChannelData->Channel)
         == NO_ERR)
      {
        Status = OK;
      }
      break;

    case RUN_6016_SELFTEST:
      if(Run6016Selftest(theCardData, theChannelData, (SELFTESTS) pData)
         == NO_ERR)
      {
        Status = OK;
      }
      break;

    case RESET_6016_BOARD:
      if(Reset6016Board(theCardData, theChannelData) == NO_ERR)
      {
        Status = OK;
      }
      break;

    case CONFIG_6016_CONTROLLER:
      {
        CONFIG_CONTROLLER *pConfigController = (CONFIG_CONTROLLER *) pData;
        if(pData == ((void *)NULL))
        {
          theChannelData->ErrCode = INVALID_PARAMETER_ERR;
          break;
        }
        if(Config6016Controller(theCardData, theChannelData,
                                pConfigController->ArbitrationMode,
                                pConfigController->BusErrorTimeout) == NO_ERR)
        {
          Status = OK;
        }
      }
      break;

    case SET_6016_CHANNEL_ENABLE:
      {
        SET_CHANNEL_ENABLE *pChannelEnable = (SET_CHANNEL_ENABLE *) pData;
        if(Set6016ChannelEnable(theCardData, theChannelData,
                                theChannelData->Channel,
                                pChannelEnable->Enable) == NO_ERR)
        {
          Status = OK;
        }
      }
      break;

    case CONFIG_6016_ERRORS:
      if(Config6016Errors(theCardData, theChannelData, (UINT) pData) == NO_ERR)
      {
        Status = OK;
      }
      break;

    case READ_6016_GLOBAL_REGISTER:
      {
        GLOBAL_REGISTER *pGlobalRegister = (GLOBAL_REGISTER *) pData;
        if(pData == ((void *)NULL))
        {
          theChannelData->ErrCode = INVALID_PARAMETER_ERR;
          break;
        }
        if(Read6016GlobalRegister(theCardData, theChannelData,
                                  pGlobalRegister->GlobalReg,
                                  &(pGlobalRegister->Value)) == NO_ERR)
        {
          Status = OK;
        }
      }
      break;

    case WRITE_6016_GLOBAL_REGISTER:
      {
        GLOBAL_REGISTER *pGlobalRegister = (GLOBAL_REGISTER *) pData;
        if(pData == ((void *)NULL))
        {
          theChannelData->ErrCode = INVALID_PARAMETER_ERR;
          break;
        }
        if(Write6016GlobalRegister(theCardData, theChannelData,
                                   pGlobalRegister->GlobalReg,
                                   &(pGlobalRegister->Value)) == NO_ERR)
        {
          Status = OK;
        }
      }
      break;

    case REQUEST_6016_INT_NOTIFY:
      {
        REQUEST_INT_NOTIFY *pRequestIntNotify = (REQUEST_INT_NOTIFY *) pData;
        if(pData == ((void *)NULL))
        {
          theChannelData->ErrCode = INVALID_PARAMETER_ERR;
          break;
        }
        if(Request6016IntNotify(theCardData, theChannelData,
                                theChannelData->Channel,
                                pRequestIntNotify->NotifyConditions,
                                pRequestIntNotify->SemID) == NO_ERR)
        {
          Status = OK;
        }
      }
      break;

    case CONFIG_6016_CHANNEL:
      {
        CONFIG_CHANNEL *pConfigChannel = (CONFIG_CHANNEL *) pData;
        if(pData == ((void *)NULL))
        {
          theChannelData->ErrCode = INVALID_PARAMETER_ERR;
          break;
        }
        if(Config6016Channel(theCardData, theChannelData,
                             theChannelData->Channel,
                             pConfigChannel->FlowControl,
                             pConfigChannel->BaudRate,
                             pConfigChannel->InputTimeout,
                             pConfigChannel->StopBits,
                             pConfigChannel->ParityType,
                             pConfigChannel->ParitySense,
                             pConfigChannel->NumBits) == NO_ERR)
        {
          Status = OK;
        }
      }
      break;

    case GET_6016_DEVICE_ERROR:
      if(Get6016DeviceError(theChannelData, (V6016_STATUS *) pData) == NO_ERR)
      {
        Status = OK;
      }
      break;

    case READ_6016_CHANNEL_REGISTER:
      {
        CHANNEL_REGISTER *pChannelRegister = (CHANNEL_REGISTER *) pData;
        if(pData == ((void *)NULL))
        {
          theChannelData->ErrCode = INVALID_PARAMETER_ERR;
          break;
        }
        if(Read6016ChannelRegister(theCardData, theChannelData,
                                   theChannelData->Channel,
                                   pChannelRegister->ChannelReg,
                                   &(pChannelRegister->Value)) == NO_ERR)
        {
          Status = OK;
        }
      }
      break;

    case WRITE_6016_CHANNEL_REGISTER:
      {
        CHANNEL_REGISTER *pChannelRegister = (CHANNEL_REGISTER *) pData;
        if(pData == ((void *)NULL))
        {
          theChannelData->ErrCode = INVALID_PARAMETER_ERR;
          break;
        }
        if(Write6016ChannelRegister(theCardData, theChannelData,
                                    theChannelData->Channel,
                                    pChannelRegister->ChannelReg,
                                    &(pChannelRegister->Value)) == NO_ERR)
        {
          Status = OK;
        }
      }
      break;

    case READ_6016_BUFFER:
      {
        BUFFER_STRUCT *pBufferStruct = (BUFFER_STRUCT *) pData;
        if(pData == ((void *)NULL))
        {
          theChannelData->ErrCode = INVALID_PARAMETER_ERR;
          break;
        }
        if(Read6016Buffer(theCardData, theChannelData,
                          theChannelData->Channel,
                          pBufferStruct->WhichBuffer,
                          pBufferStruct->Offset,
                          pBufferStruct->ByteCount,
                          pBufferStruct->pBuffer) == NO_ERR)
        {
          Status = OK;
        }
      }
      break;

    case WRITE_6016_BUFFER:
      {
        BUFFER_STRUCT *pBufferStruct = (BUFFER_STRUCT *) pData;
        if(pData == ((void *)NULL))
        {
          theChannelData->ErrCode = INVALID_PARAMETER_ERR;
          break;
        }
        if(Write6016Buffer(theCardData, theChannelData,
                           theChannelData->Channel,
                           pBufferStruct->WhichBuffer,
                           pBufferStruct->Offset,
                           pBufferStruct->ByteCount,
                           pBufferStruct->pBuffer) == NO_ERR)
        {
          Status = OK;
        }
      }
      break;
  }

  if(Status == OK) theChannelData->ErrCode = NO_ERR;

  return(Status); /* Return the status */
}














/*******************************************************************************
*
* tyCoStartup - transmitter startup routine
*
* Call interrupt level character output routine.
*/

LOCAL VOID
tyCoStartup(CHANNEL_DATA *theChannelData)
{
  char  outChar;

  if(!theChannelData->IsOpen) return;

  if(theChannelData->bIsTTY)
  {
    if(tyITx(&(theChannelData->DevHdr), &outChar) == OK)
    {
      Write6016Data(theChannelData->pCardData, theChannelData,
                    theChannelData->Channel, (UCHAR *) &outChar, 1);
    }
  }
}


/*
 *------------------------------------------------------------------------
 * InitializeTTY: Initialize channel as TTY.
 *------------------------------------------------------------------------
 */

LOCAL VOID
InitializeTTY(CHANNEL_DATA *theChannelData)
{
  REGISTER_VALUE_TYPE Value;

  /* Set the channel interrupt mask to interrupt on any character received
     or transmitted */
  if(Read6016ChannelRegister(theChannelData->pCardData, theChannelData,
                             theChannelData->Channel, CH_MSK_REG,
                             &Value) == NO_ERR)
  {
    Value.ByteValue = Value.ByteValue | RCV_MSK | TX_MSK;
    Write6016ChannelRegister(theChannelData->pCardData, theChannelData,
                             theChannelData->Channel, CH_MSK_REG, &Value);
  }

  return;
}



/*
 *------------------------------------------------------------------------
 * RemoveCard: driver uninitalize entry point
 *------------------------------------------------------------------------
 * This routine will free the memory associated with the card and remove
 * it from the card table. It will also disconnect this card from its
 * interrupts.
 *------------------------------------------------------------------------
 */

LOCAL STATUS
RemoveCard(CARD_DATA *theCardData)
{
  BOOL FoundIt;
  UINT Card;
  UINT NextCard;
  UINT Channel;
  CHANNEL_DATA *theChannelData;

  /* Make sure the parameter is valid. */
  if(theCardData == (CARD_DATA *) NULL)
  {
    return(INVALID_PARAMETER_ERR);
  }

  /* Remove the card from the card list */
  FoundIt = FALSE;
  for(Card = 0; Card<CardListSize; Card++)
  {
    /* See if this is the card */
    if(CardList[Card] == theCardData)
    {
      FoundIt = TRUE; /* Indicate we found the card	*/
      /* Move the rest of the cards up in the list	 */
      for(NextCard = Card + 1; NextCard < CardListSize; NextCard++)
      {
        /* Move the next one up */
        CardList[NextCard - 1] = CardList[NextCard];
      }
      CardListSize--; /* Change the list size */
      break;
    }
  }

  /* Make sure we found the card */
  if(!FoundIt)
  {
#if	DEBUG
    logMsg("ERROR: Attempt to uninit invalid card\n",0,0,0,0,0,0);
#endif
    return(INVALID_BOARD_ID_ERR);
  }

  /* Ff there was an interrupt attached to this channel, delete it */
  if( (theCardData->OldIntVector != ((FUNCPTR) NULL)) &&
      (theCardData->InterruptVector != 0) )
  {
    /* Restore the old interrupt vector */
    intVecSet( (FUNCPTR *) INUM_TO_IVEC(theCardData->InterruptVector),
               theCardData->OldIntVector );
  }

  /* If there was an interrupt attached to this channel, delete it. */
  if( (theCardData->ChannelData[0].OldIntVector != ((FUNCPTR) NULL)) &&
      (theCardData->ChannelData[0].InterruptVector != 0) )
  {
    /* Restore the old interrupt vector */
    intVecSet( (FUNCPTR *) INUM_TO_IVEC(
               theCardData->ChannelData[0].InterruptVector),
               theCardData->ChannelData[0].OldIntVector);
  }

  /* Go through each channel and uninitialize */
  for(Channel = 0; Channel < theCardData->NumChannels; Channel++)
  {
    /* Get channel pointer */
    theChannelData = &(theCardData->ChannelData[Channel]);
    /* Wait for access to this channel */
    if( semTake(theChannelData->AccessSem, TEN_SECONDS) == ERROR )
    {
#if	DEBUG
      logMsg("ERROR: Cannot Access Channel Semaphore in RemoveCard\n",
             0,0,0,0,0,0);
#endif
      return(RESOURCE_ERR);
    }

    /* Stop the serial channel from operation if necessary */
    StopChannel(theCardData, theChannelData);

    /* Free the memory associated with the Interrupt Notification List
       for this channel. */
    if( (theChannelData->IntNotifyList.uiListSize) &&
        (theChannelData->IntNotifyList.IntNotifyEntry !=
        ((NOTIFY_ENTRY *) NULL)) )
    {
      free( theChannelData->IntNotifyList.IntNotifyEntry );
    }

    /* Destroy the semaphore for this card. */
    semDelete(theChannelData->AccessSem);
  }
  free(theCardData); /* Free memory for card */

  return(NO_ERR); /* Return success */
}



/*
 *------------------------------------------------------------------------
 * Read6016Data: retrieve serial intput data
 *------------------------------------------------------------------------
 * The ring buffer input pointer will be updated in the buffer
 * accordingly. This routine will return the number of bytes read.
 *------------------------------------------------------------------------
 */

LOCAL int
Read6016Data(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
             UINT Channel, UCHAR *puchBuffer, UINT BytesToRead)
{
  ULONG                MemoryOption;
  ULONG                MaxBufferSize;
  USHORT               BufferSize;
  ULONG                Offset;
  ULONG                BytesAvailable;
  ULONG                BytesAvailable2;
  USHORT               InputPtr;
  USHORT               OutputPtr;
  ULONG                Address;
  UINT                 NumBytes;
  REGISTER_VALUE_TYPE  Value;
  ULONG                AccessMode;
  USHORT              *puhVmePtr;
  UCHAR               *puchVmePtr;
  int                  iByte;
  UINT                 NumBytesRead;

#if 0
  logMsg("Read6016Data:.\n",0,0,0,0,0,0);
#endif
  NumBytesRead = 0;

  /* Make sure card id is valid */
  if(theCardData == ((CARD_DATA *) NULL))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Card ID in Read6016Data\n",0,0,0,0,0,0);
#endif
    return(ERROR);
  }

  /* Make sure parameters are valid */
  if((Channel >= theCardData->NumChannels) ||
     (puchBuffer == ((UCHAR *) NULL))      ||
     (!BytesToRead))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameter to Read6016Data\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(ERROR);
  }

  /* Wait for access to this channel */
  if((!theChannelData->bIsTTY) &&
     (semTake(theChannelData->AccessSem, TEN_SECONDS) == ERROR))
  {
#if	DEBUG
    logMsg("ERROR: Cannot Access Channel Semaphore in Read6016Data\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = RESOURCE_ERR;
    return(ERROR);
  }

  /* Read the buffer information registers */
  if(Read6016ChannelRegister(theCardData, theChannelData, Channel,
                             SZ_UBUF_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Cannot Read Channel Register in Read6016Data\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    if(!theChannelData->bIsTTY)
    {
      semGive(theChannelData->AccessSem);
    }
    return(ERROR);
  }

  BufferSize = ((unsigned short)Value.ByteValue) & BYTE_MASK;
  if(BufferSize == 0)
  {
    BufferSize = MAX_BUFFER_SIZE;
  }

  /*
   * The buffer size in Slave Mode is always 4K times the memory
   * option in the ROM version register.
   *
   * Read the rom version register.
   */
  if(Read6016GlobalRegister(theCardData, theChannelData, ROM_VER_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in Read6016Data\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    if(!theChannelData->bIsTTY)
    {
      semGive(theChannelData->AccessSem);
    }
    return(ERROR);
  }

  MemoryOption = 1 << ((Value.WordValue & MEM_OPT) >> ROM_VER_SHIFT);

  /* Calculate the buffer size */
  MaxBufferSize = BUFFER_INC * MemoryOption;

  /* Calculate the real buffer size */
  BufferSize = BufferSize * SLAVE_BYTES_PER_GRN;

  /*
   *================================================================
   * SLAVE MODE
   *================================================================
   */
  /* Read the Buffer Address					 */
  if(Read6016GlobalRegister(theCardData, theChannelData, BUFBASE_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in Read6016Data\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    if(!theChannelData->bIsTTY)
    {
      semGive(theChannelData->AccessSem);
    }
    return(ERROR);
  }

  /* Calculate the buffer address */
  Address = Value.LongValue + (MaxBufferSize * 2L * Channel) + MaxBufferSize;

  /* Determine the VME Access Mode */
  AccessMode = ReadAccessMode(theCardData, theChannelData);

  /* Read the Input and Output Pointers */
  puhVmePtr = (USHORT *) Address;
  InputPtr = *puhVmePtr;
  puhVmePtr = (USHORT *) (Address + OUTPUT_PTR_INDEX);
  OutputPtr = *puhVmePtr;

  /* See if the buffer has wrapped around */
  if(OutputPtr > InputPtr)
  {
    /* Determine the number of bytes available */
    BytesAvailable = (BufferSize - (OutputPtr + SLAVE_RING_START));
    BytesAvailable2 = InputPtr;
  }
  else
  {
    /* The number of bytes available is the difference between
     * the two values */
    BytesAvailable = InputPtr - OutputPtr;
    BytesAvailable2 = 0;
  }

  /*
   * The Offset into the Buffer to read in a ring buffer is the
   * output pointer.
   */
  Offset = (unsigned long)OutputPtr + SLAVE_RING_START;

  /*
   * Determine how many bytes to read, which will be the least of
   * the number asked for and the number available.
   */
  if(BytesToRead >= (BytesAvailable + BytesAvailable2))
  {
    /* Modify the byte count to read all bytes available from the buffer */
    NumBytesRead = (int) (BytesAvailable + BytesAvailable2);
  }
  else
  {
    /* Otherwise, we will read the number of bytes the user wants */
    NumBytesRead = BytesToRead;
  }

  /* Make sure there are some bytes to read */
  if(NumBytesRead > 0)
  {
    /* Point to location to be read */
    puchVmePtr = (UCHAR *) (Address + Offset);

    /* Read the bytes from the buffer */
    if(BytesAvailable >= NumBytesRead)
    {
      /* Read all the bytes the user requested */
      for(iByte = 0; iByte < NumBytesRead; iByte++)
      {
        puchBuffer[iByte] = puchVmePtr[iByte];
      }
    }
    else
    {
      /* Read the number of bytes available */
      for(iByte = 0; iByte < BytesAvailable; iByte++)
      {
        puchBuffer[iByte] = puchVmePtr[iByte];
      }

      /* See if there are more bytes to be read */
      if(BytesAvailable2)
      {
        /* Determine whether the user requested more than is available */
        if((BytesAvailable + BytesAvailable2) >= ((unsigned long)NumBytesRead))
        {
          /* Read the number of bytes available */
          NumBytes = NumBytesRead - ((unsigned int)BytesAvailable);
        }
        else
        {
          /* Read the number of bytes available */
          NumBytes = (UINT) BytesAvailable2;
        }

        /* Read the Bytes */
        puchVmePtr = (UCHAR *) (Address + SLAVE_RING_START);
        for(iByte = 0; iByte < NumBytes; iByte++)
        {
          (puchBuffer + BytesAvailable)[iByte] = puchVmePtr[iByte];
        }
#if 1	
        logMsg("Read %d bytes from %lx\n",
               NumBytes,Address + SLAVE_RING_START,0,0,0,0);
        logMsg("To Address = %lx\n",(((ULONG) puchBuffer) + BytesAvailable),
			   0,0,0,0,0);
#endif
      }
    }

    /* Determine the new output pointer value;
    see if just incrementing the output pointer value by
    the number of bytes read will go past the end of the buffer */
    if( (((unsigned int)OutputPtr) + SLAVE_RING_START +
        NumBytesRead) >= ((unsigned int)BufferSize) )
    {
      /* Take into account the wraparound of the buffer. */
      OutputPtr = (((unsigned int) OutputPtr) + SLAVE_RING_START
                 + NumBytesRead) - ((unsigned int) BufferSize);
    }
    else
    {
      /* Just increment the output pointer by the number of bytes read */
      OutputPtr = OutputPtr + ((unsigned short) NumBytesRead);
    }

    /* Set the output pointer */
    puhVmePtr = (USHORT *) (Address + OUTPUT_PTR_INDEX);
    *puhVmePtr = OutputPtr;
  }

  /* Reset the receive buffer has data flag */
  if(Read6016ChannelRegister(theCardData, theChannelData, Channel,
     CST_REG, &Value) != NO_ERR )
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read Channel Register in Read6016Data\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    if(!theChannelData->bIsTTY)
    {
      semGive(theChannelData->AccessSem);
    }
    return(ERROR);
  }
  Value.WordValue = Value.WordValue & (~URCV_RDY) & (~RXRDY);
  if(Write6016ChannelRegister(theCardData, theChannelData, Channel,
     CST_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write Channel Register in Read6016Data\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    if(!theChannelData->bIsTTY)
    {
      semGive(theChannelData->AccessSem);
    }
    return(ERROR);
  }
  if((theChannelData->bIsTTY) ||
     (semTake(theCardData->GlobalRegSem,TEN_SECONDS) == OK))
  {
    /* Restart the Receive */
    if(Read6016GlobalRegister(theCardData, theChannelData, RX_REG, &Value)
       != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Read in Read6016Data\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      if(!theChannelData->bIsTTY)
      {
        semGive(theCardData->GlobalRegSem);
        semGive(theChannelData->AccessSem);
      }
      return(ERROR);
    }
    Value.WordValue = Value.WordValue | (1 << Channel);
    if(WriteBusyReg(theCardData, theChannelData, RX_REG, &Value) != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Write Register in Read6016Data\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      if(!theChannelData->bIsTTY)
      {
        semGive(theCardData->GlobalRegSem);
        semGive(theChannelData->AccessSem);
      }
      return(ERROR);
    }
    if(!theChannelData->bIsTTY)
    {
      semGive(theCardData->GlobalRegSem);
    }
  }
  else
  {
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    if(!theChannelData->bIsTTY)
    {
      semGive(theChannelData->AccessSem);
    }
    return (ERROR);
  }
  theChannelData->ErrCode = NO_ERR; /* Indicate success */
  if(!theChannelData->bIsTTY)
  {
    semGive(theChannelData->AccessSem); /* Allow other access */
  }

  return(NumBytesRead); /* Return success */
}


/*
 *------------------------------------------------------------------------
 * Write6016Data: write serial output data
 *------------------------------------------------------------------------
 * The ring buffer output pointer will be updated in the buffer
 * accordingly. This routine will return the number of bytes written.
 *------------------------------------------------------------------------
 */

LOCAL int
Write6016Data(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
              UINT Channel, UCHAR *puchBuffer, UINT BytesToWrite)
{
  ULONG                MemoryOption;
  ULONG                MaxBufferSize;
  USHORT               BufferSize;
  ULONG                Offset;
  ULONG                BytesAvailable;
  ULONG                BytesAvailable2;
  ULONG                Address;
  UINT                 NumBytes;
  USHORT               InputPtr;
  USHORT               OutputPtr;
  REGISTER_VALUE_TYPE  Value;
  ULONG                AccessMode;
  int                  iByte;
  USHORT              *puhVmePtr;
  UCHAR               *puchVmePtr;
  UINT                 NumBytesWritten;

  NumBytesWritten = 0;

  /* Make sure card id is valid */
  if(theCardData == ((CARD_DATA *) NULL))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Card ID in Write6016Data\n",0,0,0,0,0,0);
#endif
    return(ERROR);
  }

  /* Make sure parameters are valid */
  if((Channel >= theCardData->NumChannels) ||
     (puchBuffer == ((UCHAR *) NULL))      ||
     (!BytesToWrite))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameter to Write6016Data\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(ERROR);
  }

  /* Wait for access to this channel */
  if((!theChannelData->bIsTTY) &&
     (semTake(theChannelData->AccessSem, TEN_SECONDS) == ERROR))
  {
#if	DEBUG
    logMsg("ERROR: Cannot Access Channel Semaphore in Write6016Data\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = RESOURCE_ERR;
    return(ERROR);
  }

  /* Read the buffer information registers */
  if(Read6016ChannelRegister(theCardData, theChannelData, Channel,
     SZ_UBUF_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Cannot Read Channel Register in Write6016Data\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    if(!theChannelData->bIsTTY)
    {
      semGive(theChannelData->AccessSem);
    }
    return(ERROR);
  }

  BufferSize = ((unsigned short)Value.ByteValue) & BYTE_MASK;
  if(BufferSize == 0)
  {
    BufferSize = MAX_BUFFER_SIZE;
  }

  /*
   * The buffer size in Slave Mode is always 4K times the memory
   * option in the ROM version register.
   *
   * Read the ROM version register.
   */
  if(Read6016GlobalRegister(theCardData, theChannelData, ROM_VER_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in Write6016Data\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    if(!theChannelData->bIsTTY) semGive(theChannelData->AccessSem);
    return(ERROR);
  }
  MemoryOption = 1 << ((Value.WordValue & MEM_OPT) >> ROM_VER_SHIFT);
  /* Calculate the buffer size */
  MaxBufferSize = BUFFER_INC * MemoryOption;
  /* Calculate the real buffer size */
  BufferSize = BufferSize * SLAVE_BYTES_PER_GRN;
  /* Read the Buffer Address */
  if(Read6016GlobalRegister(theCardData, theChannelData, BUFBASE_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in Write6016Data\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    if(!theChannelData->bIsTTY) semGive(theChannelData->AccessSem);
    return(ERROR);
  }
  /* Calculate the buffer address */
  Address = Value.LongValue + (MaxBufferSize * 2L * Channel);
  /* Determine the VME Access Mode */
  AccessMode = ReadAccessMode(theCardData, theChannelData);
  /* Read the Input and Output Pointers */
  puhVmePtr = (USHORT *) Address;
  InputPtr = *puhVmePtr;
  puhVmePtr = (USHORT *) (Address + OUTPUT_PTR_INDEX);
  OutputPtr = *puhVmePtr;
  /* See if the buffer has wrapped around */
  if(OutputPtr <= InputPtr)
  {
    /* Determine the number of bytes available */
    BytesAvailable = (BufferSize - (InputPtr + SLAVE_RING_START));
    BytesAvailable2 = OutputPtr;
  }
  else
  {
    /* The number of bytes available is the difference between the two values*/
    BytesAvailable = OutputPtr - InputPtr;
    BytesAvailable2 = 0;
  }
  /*
   * The Offset into the Buffer to write in a ring buffer is the
   * input pointer.
   */
  Offset = (unsigned long)(InputPtr + SLAVE_RING_START);
  /*
   * Determine how many bytes to write, which will be the least of
   * the number asked for and the number available.
   */
  if(BytesToWrite >= (BytesAvailable + BytesAvailable2))
  {
    /* Modify the byte count to write all bytes available from the buffer */
    NumBytesWritten = (int) (BytesAvailable + BytesAvailable2);
  }
  else
  {
    /* Otherwise, we will write the number of bytes the user wants */
    NumBytesWritten = BytesToWrite;
  }

  /* Make sure there are some bytes to write */
  if(NumBytesWritten > 0)
  {
    /* Write the bytes to the buffer */
    if(BytesAvailable >= NumBytesWritten)
    {
      /* Write all the bytes the user requested	*/
      puchVmePtr = (UCHAR *) (Address + Offset);
      for(iByte = 0; iByte < NumBytesWritten; iByte++)
      {
        puchVmePtr[iByte] = puchBuffer[iByte];
      }
    }
    else
    {
      /* Write the number of bytes available */
      puchVmePtr = (UCHAR *) (Address + Offset);
      for(iByte = 0; iByte < BytesAvailable; iByte++)
      {
        puchVmePtr[iByte] = puchBuffer[iByte];
      }
      /* See if there are more bytes to be written */
      if((BytesAvailable2) && (NumBytesWritten > BytesAvailable))
      {
        /* Determine whether the user requested more than is available */
        if((BytesAvailable + BytesAvailable2) >=
           ((unsigned long)NumBytesWritten))
        {
          /* Write the number of bytes available */
          NumBytes = NumBytesWritten - ((unsigned int) BytesAvailable);
        }
        else
        {
          /* Write the number of bytes available */
          NumBytes = (UINT) BytesAvailable2;
        }
        /* Write the Bytes */
        puchVmePtr = (UCHAR *) (Address + SLAVE_RING_START);
        for(iByte = 0; iByte < NumBytes; iByte++)
        {
          puchVmePtr[iByte] = (puchBuffer + BytesAvailable)[iByte];
        }
      }
    }
    /* Determine the new output pointer value.
     *
     * See if just incrementing the input pointer value by the
     * number of bytes written will go past the end of the
     * buffer */
    if((((unsigned int)InputPtr) + SLAVE_RING_START +
       NumBytesWritten) >= ((unsigned int)BufferSize))
    {
      /* Take into account the wraparound of the buffer */
      InputPtr = (((unsigned int) InputPtr) +
                SLAVE_RING_START + NumBytesWritten) -
                ((unsigned int) BufferSize);
    }
    else
    {
      /* Just increment the output pointer by the number of bytes written */
      InputPtr = InputPtr + ((unsigned short)NumBytesWritten);
    }
    /* Set the input pointer */
    puhVmePtr = (USHORT *) (Address + INPUT_PTR_INDEX);
    *puhVmePtr = InputPtr;
  }

  /* Reset the Transmit Buffer has data flag */
  if(Read6016ChannelRegister(theCardData, theChannelData, Channel,
     CST_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read Channel Register in Write6016Data\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    if(!theChannelData->bIsTTY) semGive(theChannelData->AccessSem);
    return(ERROR);
  }
  Value.WordValue = Value.WordValue & (~UTX_RDY) & (~TXRDY);
  if(Write6016ChannelRegister(theCardData, theChannelData, Channel,
     CST_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write Channel Register in Write6016Data\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    if(!theChannelData->bIsTTY) semGive(theChannelData->AccessSem);
    return(ERROR);
  }
  if((theChannelData->bIsTTY) ||
     (semTake(theCardData->GlobalRegSem,TEN_SECONDS) == OK))
  {
    /* Restart the transmit */
    if(Read6016GlobalRegister(theCardData, theChannelData, TX_REG, &Value)
       != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Read in Write6016Data\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      if(!theChannelData->bIsTTY)
      {
        semGive(theCardData->GlobalRegSem);
        semGive(theChannelData->AccessSem);
      }
      return(ERROR);
    }
    Value.WordValue = Value.WordValue | (1 << Channel);
    if(WriteBusyReg(theCardData, theChannelData, TX_REG, &Value)
       != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Write Register in Write6016Data\n",
             0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      if(!theChannelData->bIsTTY)
      {
        semGive(theCardData->GlobalRegSem);
        semGive(theChannelData->AccessSem);
      }
      return(ERROR);
    }
    if(!theChannelData->bIsTTY) semGive(theCardData->GlobalRegSem);
  }
  else
  {
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    if(!theChannelData->bIsTTY) semGive(theChannelData->AccessSem);
    return(ERROR);
  }
  theChannelData->ErrCode = NO_ERR; /* Indicate success */
  if(!theChannelData->bIsTTY)
  {
    semGive(theChannelData->AccessSem); /* Allow other access */
  }

  return(NumBytesWritten); /* Return success */
}

/* configure the slave-mode xmit and rcv user buffers */
LOCAL V6016_STATUS
Config6016Buffer(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
                 ULONG BufferAddress, RELEASE_MODE ReleaseMode,
                 UINT FairnessTimeout, BOOL UseA32, BOOL UseSupervisor)
{
  REGISTER_VALUE_TYPE Value;
  UCHAR               CR;

  /* Verify that the Parameters are within range */
  if(( theCardData == ((CARD_DATA *) NULL) ) ||
     ( theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Config6016Buffer\n",0,0,0,0,0,0);
#endif
		return( INVALID_PARAMETER_ERR );
  }

  /* Check all the parameters. */
  if(((ReleaseMode != RELROR) && (ReleaseMode != RELRWD) &&
      (ReleaseMode != RELROC) && (ReleaseMode != RELBCAP)) ||
      (FairnessTimeout > FAIR_TO))
  {
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /*
   * Write the bus request level, release mode and fairness timeout.
   */
  Value.ByteValue = (UCHAR) ((theCardData->BusRequestLevel << BR_LEVEL_SHIFT)
                   | ReleaseMode | (FairnessTimeout & FAIR_TO));
  if(Write6016GlobalRegister(theCardData, theChannelData, CR2_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write in Config6016Buffer\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  /* Write the Buffer Address */
  Value.LongValue = BufferAddress;
  if(Write6016GlobalRegister(theCardData, theChannelData, BUFBASE_REG,
     &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write in Config6016Buffer\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  /* Read Command Register 0 */
  if(Read6016GlobalRegister(theCardData, theChannelData, CTR0_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in Config6016Buffer\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  /* Set the Command Register 0 Bits */
  CR = Value.ByteValue & ( (~MASTER_MODE) & (~SLV_32)
      & (~SUPONLY) & (~SLV_OK_MASK) );
  if(UseA32) CR |= SLV_32;
  if(UseSupervisor) CR |= SUPONLY;

  /* Write the Slave flags and not the buffer mode or the Slave OK Bit */
  Value.ByteValue = CR;
  if(WriteBusyReg(theCardData, theChannelData, CTR0_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write Register in Config6016Buffer\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  CR |= SLV_OK_MASK; /* Set the slave OK Bit */

  /* Write the Slave OK Bits. NOTE: this bit must be set after the
  other setup is performed */
  Value.ByteValue = CR;
  if(WriteBusyReg(theCardData, theChannelData, CTR0_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write Register in Config6016Buffer\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  theChannelData->ErrCode = NO_ERR;	/* Indicate success */

  return(NO_ERR); /* Return success */
}

/* transmit a break sequence on one channel */
LOCAL V6016_STATUS
Send6016Break(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
              UINT Channel)
{
  UCHAR                cr0;
  REGISTER_VALUE_TYPE  Value;
  UINT                 TimeoutCount;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL) ) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Send6016Break\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Make sure parameters are valid */
  if(Channel >= theCardData->NumChannels)
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameter in Send6016Break\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /* Wait for access to this channel */
  if(semTake(theChannelData->AccessSem, TEN_SECONDS) == ERROR)
  {
#if	DEBUG
    logMsg("ERROR: Cannot Access Channel Semaphore in Send6016Break\n",
      0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = RESOURCE_ERR;
    return(RESOURCE_ERR);
  }
  if(semTake(theCardData->GlobalRegSem,TEN_SECONDS) == OK)
  {
    /* Write the Break Register with this channel number */
    Value.WordValue = (1 << Channel);
    if(Write6016GlobalRegister(theCardData, theChannelData, BREAK_REG, &Value)
       != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Write in Send6016Break\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      semGive(theCardData->GlobalRegSem);
      semGive(theChannelData->AccessSem);
      return( BOARD_ACCESS_ERR );
    }
    /* Read the contents of Command Register 0 */
    if(Read6016GlobalRegister(theCardData, theChannelData, CTR0_REG, &Value)
       != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Read in Send6016Break\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      semGive(theCardData->GlobalRegSem);
      semGive(theChannelData->AccessSem);
      return(BOARD_ACCESS_ERR);
    }
    /* Write Command Register 0 with transmit break bit set */
    cr0 = Value.ByteValue | TXBRK_MASK;
    Value.ByteValue = cr0;
    if(WriteBusyReg(theCardData, theChannelData, CTR0_REG, &Value) != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Write Register in Send6016Break\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      semGive(theCardData->GlobalRegSem);
      semGive(theChannelData->AccessSem);
      return(BOARD_ACCESS_ERR);
    }
    /* Wait for the Transmit Break Bit to clear */
    TimeoutCount = 0;
    do
    {
      if(Read6016GlobalRegister(theCardData, theChannelData, CTR0_REG, &Value)
         != NO_ERR)
      {
#if	DEBUG
        logMsg("ERROR: Failed to Read in Send6016Break\n",0,0,0,0,0,0);
#endif
        theChannelData->ErrCode = BOARD_ACCESS_ERR;
        semGive(theCardData->GlobalRegSem);
        semGive(theChannelData->AccessSem);
        return(BOARD_ACCESS_ERR);
      }
      /* Increment timeout count */
      if(TimeoutCount > 0)
      {
        taskDelay(ONE_SEC / 10);
      }
      TimeoutCount++;
    } while((Value.ByteValue & TXBRK_MASK) && (TimeoutCount < HUNDRED_TIMES));
    semGive(theCardData->GlobalRegSem);
  }
  else
  {
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    semGive(theChannelData->AccessSem);
    return(BOARD_ACCESS_ERR);
  }

  /* See if we timed out */
  if(Value.ByteValue & TXBRK_MASK)
  {
#if	DEBUG
    logMsg("ERROR: Break Never Completed\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  semGive(theChannelData->AccessSem); /* Allow other access */
  theChannelData->ErrCode = NO_ERR; /* Indicate success */

  return(NO_ERR); /* Return success */
}

/* initiate a selftest on the 6016 device */
LOCAL V6016_STATUS
Run6016Selftest(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
                SELFTESTS Selftests)
{
  REGISTER_VALUE_TYPE  Value;
  UINT                 TimeoutCount;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL) ) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Run6016Selftest\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Check all the parameters */
  if(Selftests & (~VALID_TESTS))
  {
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /* Set the Selftest Procedure Register. Note we are making sure
  the download bit and the unused bit are not set */
  Value.ByteValue = (UCHAR) (Selftests & VALID_TESTS);
  if(Write6016GlobalRegister(theCardData, theChannelData, ST_PROC_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write in Run6016Selftest\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  /* Set the Selftest Busy Bit in the GST to avoid any race conditions */
  if(Read6016GlobalRegister(theCardData, theChannelData, GST_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in Run6016Selftest\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  Value.WordValue = Value.WordValue | ST_BUSY;
  if(Write6016GlobalRegister(theCardData, theChannelData, GST_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write in Run6016Selftest\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  /* Write Command Register 0 with the run selftest bit set */
  if(Read6016GlobalRegister(theCardData, theChannelData, CTR0_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in Run6016Selftest\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  Value.ByteValue = Value.ByteValue | SLFTST_MASK;
  if(WriteBusyReg(theCardData, theChannelData, CTR0_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write Register in Run6016Selftest\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  /* Wait for the selftest busy bit to clear in the GST */
  TimeoutCount = 0;
  do
  {
    if(Read6016GlobalRegister(theCardData, theChannelData, GST_REG, &Value)
       != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Read in Run6016Selftest\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      return(BOARD_ACCESS_ERR);
    }
    /* Increment timeout count */
    if(TimeoutCount > 0)
    {
      taskDelay(ONE_SEC / 10);
    }
    TimeoutCount++;
  } while((Value.WordValue & ST_BUSY) && (TimeoutCount < HUNDRED_TIMES));

  /* See if we timed out */
  if(Value.WordValue & ST_BUSY)
  {
#if	DEBUG
    logMsg("ERROR: Selftest Never Completed\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  theChannelData->ErrCode = NO_ERR;	/* Indicate success */

  return(NO_ERR); /* Return success */
}

/* perform a board reset */
LOCAL V6016_STATUS
Reset6016Board(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData)
{
  UINT                 Channel;
  UINT                 TimeoutCount;
  REGISTER_VALUE_TYPE  Value;
  CHANNEL_DATA        *theNewChannelData;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL) ) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Reset6016Board\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Write Command Register 0 with the reset board bit set */
  if(Read6016GlobalRegister(theCardData, theChannelData, CTR0_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in Reset6016Board\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  Value.ByteValue = Value.ByteValue | RST_BRD_MASK;
  if(Write6016GlobalRegister(theCardData, theChannelData, CTR0_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write in Reset6016Board\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  taskDelay(40); /* Wait for timeout */

  /* Wait for the POR selftest status flag register to clear */
  TimeoutCount = 0;
  do
  {
    if(Read6016GlobalRegister(theCardData, theChannelData, STFLAG_REG,
       &Value) != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Read in Reset6016Board\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      return(BOARD_ACCESS_ERR);
    }

    /* Increment timeout count */
    if(TimeoutCount > 0)
    {
      taskDelay(ONE_SEC / 10);
    }
    TimeoutCount++;
  } while( (Value.ByteValue) && (TimeoutCount < HUNDRED_TIMES) );

  /* See if we timed out */
  if(Value.ByteValue)
  {
#if	DEBUG
    logMsg("ERROR: Reset Never Completed\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  /* Write the bus request level */
  Value.ByteValue =
    (UCHAR)(theCardData->BusRequestLevel << BR_LEVEL_SHIFT) | RELROR;
  if(Write6016GlobalRegister(theCardData, theChannelData, CR2_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg( "ERROR: Failed to Write in Reset6016Board\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  /* Initialize the error interrupt control register */
  Value.ByteValue = 
    ( HDWE_ERR_MSK | BERR_MASK | (UCHAR) theCardData->InterruptLevel );
  if(Write6016GlobalRegister(theCardData, theChannelData, ER_MSK_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write in Reset6016Board\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  /* Initialize the error interrupt vector register */
  Value.ByteValue = (UCHAR) theCardData->InterruptVector;
  if(Write6016GlobalRegister(theCardData, theChannelData, ER_VEC_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write in Reset6016Board\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  /* Go through each channel */
  for(Channel = 0; Channel < theCardData->NumChannels; Channel++)
  {
    /* Get pointer to the channel data for this channel */
    theNewChannelData = &(theCardData->ChannelData[Channel]);
    /* Initialize the channel interrupt mask register */
    Value.ByteValue = GetChannelMask(theNewChannelData) |
                     ((UCHAR) theNewChannelData->InterruptLevel);
    if(Write6016ChannelRegister(theCardData, theNewChannelData, Channel,
       CH_MSK_REG, &Value) != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Write Channel Register in Reset6016Board\n",
             0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      return(BOARD_ACCESS_ERR);
    }

    /* Initialize the channel interrupt vector register */
    Value.ByteValue = (UCHAR) theNewChannelData->InterruptVector;
    if(Write6016ChannelRegister(theCardData, theNewChannelData, Channel,
       CH_VEC_REG, &Value) != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Write Channel Register in Reset6016Board\n",
             0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      return(BOARD_ACCESS_ERR);
    }
  }
  theChannelData->ErrCode = NO_ERR;	/* Indicate success */

  return(NO_ERR); /* Return success */
}

/* enable board as VMEbus system controller */
LOCAL V6016_STATUS 
Config6016Controller(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
                     ARB_MODE ArbitrationMode, UINT BusErrorTimeout)
{
  REGISTER_VALUE_TYPE  Value;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL) ) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Config6016Controller\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Make sure the Parameters are valid */
  if(((ArbitrationMode != ROUND_ROBIN) &&
      (ArbitrationMode != PRIORITY)) || (BusErrorTimeout > 7))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Config6016Controller\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /* Write the Command Register 1 Value */
  Value.ByteValue = BusErrorTimeout | ArbitrationMode;
  if(WriteBusyReg(theCardData, theChannelData, CTR1_REG, &Value) != NO_ERR )
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write Register in Config6016Controller\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  theChannelData->ErrCode = NO_ERR; /* Indicate success */

  return(NO_ERR); /* Return success */
}

/* enable/disable xmit or rcv on a channel */
LOCAL V6016_STATUS
Set6016ChannelEnable(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
                     UINT Channel, BOOL Enable)
{
  V6016_STATUS  Status;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL) ) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Set6016ChannelEnable\n",
           0,0,0,0,0,0);
#endif
    return( INVALID_PARAMETER_ERR );
  }

  /* Check to make sure the parameters are valid */
  if(Channel >= theCardData->NumChannels)
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Set6016ChannelEnable\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /* Wait for access to this channel */
  if(semTake(theChannelData->AccessSem, TEN_SECONDS) == ERROR)
  {
#if	DEBUG
    logMsg("ERROR: Cannot Access Channel Semaphore in Set6016ChannelEnable\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = RESOURCE_ERR;
    return(RESOURCE_ERR);
  }

  /* See which direction the user wants */
  if(Enable)
  {
    Status = StartChannel(theCardData, theChannelData);
  }
  else
  {
    Status = StopChannel(theCardData, theChannelData);
  }
  semGive(theChannelData->AccessSem); /* Allow other access */
  theChannelData->ErrCode = Status;	/* Indicate success */

  return(Status); /* Return success */
}

/* control what the card considers to be an error */
LOCAL V6016_STATUS 
Config6016Errors(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
                 UINT OredErrors)
{
  REGISTER_VALUE_TYPE  Value;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL) ) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Config6016Errors\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Make sure the errors parameter is valid */
  if((OredErrors > 255) || (OredErrors & CH_LEVEL))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Config6016Errors\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /* Read the Error Interrupt Control Register */
  if(Read6016GlobalRegister(theCardData, theChannelData, ER_MSK_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read Register in Config6016Errors\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  /* Set the Error Interrupt Control Register */
  Value.ByteValue = (UCHAR) ((Value.ByteValue & CH_LEVEL) | OredErrors);
  if(Write6016GlobalRegister(theCardData, theChannelData, ER_MSK_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write Register in Config6016Errors\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  theChannelData->ErrCode = NO_ERR;	/* Indicate success */

  return(NO_ERR); /* Return success */
}

/* read a global board register */
LOCAL V6016_STATUS
Read6016GlobalRegister(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
                       GLOBAL_REG GlobalReg, REGISTER_VALUE_TYPE *pValue)
{
  UCHAR   *puchVmePtr;
  USHORT  *puhVmePtr;
  ULONG   *pulVmePtr;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL) ) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Read6016GlobalRegister\n",
      0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Make sure we got a register value */
  if( pValue == ((REGISTER_VALUE_TYPE *) NULL) )
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Read6016GlobalRegister\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /* Determine the register */
  switch(GlobalReg)
  {
    default:
#if	DEBUG
      logMsg("ERROR: Invalid Parameters in Read6016GlobalRegister\n",
             0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = INVALID_PARAMETER_ERR;
      return(INVALID_PARAMETER_ERR);
    case BRD_ID_REG:
      /* FALLTHROUGH*/
    case STFLAG_REG:
      /* FALLTHROUGH*/
    case CTR0_REG:
      /* FALLTHROUGH*/
    case CTR1_REG:
      /* FALLTHROUGH*/
    case CR2_REG:
      /* FALLTHROUGH*/
    case SZ_AM_REG:
      /* FALLTHROUGH*/
    case ST_PROC_REG:
      /* FALLTHROUGH*/
    case ER_MSK_REG:
      /* FALLTHROUGH*/
    case ER_VEC_REG:
      /* FALLTHROUGH*/
    case RES_REG:
      /* FALLTHROUGH */
    case MAS_GRN_REG:
      puchVmePtr = (UCHAR *) (theCardData->BoardAddress + GlobalReg);
      pValue->ByteValue = *puchVmePtr;
      break;
    case ROM_VER_REG:
      /* FALLTHROUGH*/
    case GO_REG:
      /* FALLTHROUGH*/
    case TX_REG:
      /* FALLTHROUGH*/
    case RX_REG:
      /* FALLTHROUGH*/
    case BREAK_REG:
      /* FALLTHROUGH*/
    case GST_REG:
      puhVmePtr = (USHORT *) (theCardData->BoardAddress + GlobalReg);
      pValue->WordValue = *puhVmePtr;
      break;
    case BUFBASE_REG:
      pulVmePtr = (ULONG *) (theCardData->BoardAddress + GlobalReg);
      pValue->LongValue = *pulVmePtr;
      break;
  }
  theChannelData->ErrCode = NO_ERR; /* Indicate success */

  return(NO_ERR); /* Return success */
}

/* write a board global register */
LOCAL V6016_STATUS
Write6016GlobalRegister(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
                        GLOBAL_REG GlobalReg, REGISTER_VALUE_TYPE *pValue)
{
  UCHAR   *puchVmePtr;
  USHORT  *puhVmePtr;
  ULONG   *pulVmePtr;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL) ) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Write6016GlobalRegister\n",
           0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Make sure we got a register value */
  if(pValue == ((REGISTER_VALUE_TYPE *) NULL))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Write6016GlobalRegister\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /* Determine the register */
  switch(GlobalReg)
  {
    default:
#if	DEBUG
      logMsg("ERROR: Invalid Parameters in Write6016GlobalRegister\n",
             0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = INVALID_PARAMETER_ERR;
      return(INVALID_PARAMETER_ERR);
    case CTR0_REG:
      /* FALLTHROUGH */
    case CTR1_REG:
      /* FALLTHROUGH */
    case GO_REG:
      /* FALLTHROUGH */
    case TX_REG:
      /* FALLTHROUGH */
    case RX_REG:
      if( (GlobalReg == CTR0_REG) && (pValue->ByteValue & RST_BRD_MASK))
      {
        puchVmePtr = (UCHAR *) (theCardData->BoardAddress + GlobalReg);
        *puchVmePtr = pValue->ByteValue;
      }
      else
      {
        if(WriteBusyReg(theCardData, theChannelData, GlobalReg, pValue) !=
           NO_ERR)
        {
#if	DEBUG
         logMsg("ERROR: Failed to Write Register in Write6016GlobalRegister\n",
                0,0,0,0,0,0);
#endif
          theChannelData->ErrCode = BOARD_ACCESS_ERR;
          return(BOARD_ACCESS_ERR);
        }
        theChannelData->ErrCode = NO_ERR;
        return(NO_ERR);
      }
      break;
    case BRD_ID_REG:
      /* FALLTHROUGH */
    case STFLAG_REG:
      /* FALLTHROUGH */
    case CR2_REG:
      /* FALLTHROUGH */
    case SZ_AM_REG:
      /* FALLTHROUGH */
    case ST_PROC_REG:
      /* FALLTHROUGH */
    case ER_MSK_REG:
      /* FALLTHROUGH */
    case ER_VEC_REG:
      /* FALLTHROUGH */
    case RES_REG:
      /* FALLTHROUGH */
    case MAS_GRN_REG:
      puchVmePtr = (UCHAR *) (theCardData->BoardAddress + GlobalReg);
      *puchVmePtr = pValue->ByteValue;
      break;
    case ROM_VER_REG:
      /* FALLTHROUGH */
    case BREAK_REG:
      /* FALLTHROUGH */
    case GST_REG:
      puhVmePtr = (USHORT *) (theCardData->BoardAddress + GlobalReg);
      *puhVmePtr = pValue->WordValue;
      break;
    case BUFBASE_REG:
      pulVmePtr = (ULONG *) (theCardData->BoardAddress + GlobalReg);
      *pulVmePtr = pValue->LongValue; /* boy:  GlobalReg is BUFBASE_REG */
      break;
  }
  theChannelData->ErrCode = NO_ERR; /* Indicate success */

  return(NO_ERR); /* Return success */
}

/* set program's interrupt notification request */
LOCAL V6016_STATUS
Request6016IntNotify(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
                     UINT Channel, ULONG NotifyConditions, SEM_ID SemID)
{
  REGISTER_VALUE_TYPE  Value;
  NOTIFY_LIST         *IntNotifyList;
  ULONG                Conditions;
  UINT                 AddIndex;
  UINT                 DeleteIndex;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL) ) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Request6016IntNotify\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Make sure we have valid parameters */
  if( (Channel >= theCardData->NumChannels) || (SemID == ((SEM_ID) NULL)) )
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Request6016IntNotify\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /* Wait for access to this channel */
  if(semTake(theChannelData->AccessSem, TEN_SECONDS) == ERROR)
  {
#if	DEBUG
    logMsg("ERROR: Cannot Access Channel Semaphore in Request6016IntNotify\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = RESOURCE_ERR;
    return(RESOURCE_ERR);
  }

  /* Get pointer to the notify list */
  IntNotifyList = &(theChannelData->IntNotifyList);

  /* Determine if we need to add an entry or delete an entry */
  if(NotifyConditions != NO_NOTIFY)
  {
    /*
     * Add a new interrupt notification.
     *--------------------------------------------------------
     * Determine if we need to allocate more space for this entry.
     */
    if(IntNotifyList->uiNumListEntries >= IntNotifyList->uiListSize)
    {
      /* Call routine to allocate more notify space */
      if(AllocateNotifySpace(IntNotifyList) != NO_ERR)
      {
#if	DEBUG
        logMsg("ERROR: Cannot get memory in Request6016IntNotify\n",
               0,0,0,0,0,0);
#endif
        theChannelData->ErrCode = RESOURCE_ERR;
        return(RESOURCE_ERR);
      }
    }
    /* Calculate the conditions. The conditions stored will be
     * stored as a Channel Status Register Mask instead of a
     * Error Interrupt Mask so the interrupt handler doesn't
     * have to figure this out.
     */
    Conditions = 0L;
    if(NotifyConditions & COS_MSK)   Conditions |= (DCD_COS | CTS_COS);
    if(NotifyConditions & ERR_MSK)   Conditions |= (FERR | PERR | OVERRUN);
    if(NotifyConditions & BREAK_MSK) Conditions |= (BRK_RCVD);
    if(NotifyConditions & RCV_MSK)   Conditions |= (URCV_RDY);
    if(NotifyConditions & TX_MSK)    Conditions |= (UTX_RDY);
    if(NotifyConditions & ERR_INT)   Conditions |= (ERROR_INT_MASK);
    /*
     * Add new Entry. Note that we don't save the interrupt
     * level stuff for interrupt conditions.
     */
    AddIndex = IntNotifyList->uiNumListEntries;
    IntNotifyList->IntNotifyEntry[AddIndex].NotifyConditions = Conditions;
    theChannelData->IntNotifyList.IntNotifyEntry[AddIndex].NotifySem = SemID;
    /* Increment the number of entries */
    IntNotifyList->uiNumListEntries = AddIndex + 1;
/*
logMsg("Add entry # %d\n",IntNotifyList->uiNumListEntries,0,0,0,0,0);
for(AddIndex=0; AddIndex<IntNotifyList->uiNumListEntries; AddIndex++)
{
logMsg("[%2d] SemID=0x%08x\n",AddIndex,
(int)theChannelData->IntNotifyList.IntNotifyEntry[AddIndex].NotifySem,0,0,0,0);
}
*/
  }
  else
  {
    /*
     * Delete an existing interrupt notification.
     *--------------------------------------------------------
     * Loop through the entries and find all the semaphores
     * that match the one sent as a parameter.
     */
    for(DeleteIndex = 0; DeleteIndex < IntNotifyList->uiNumListEntries; )
    {
      /* See if this signal matches */
      if(SemID == IntNotifyList->IntNotifyEntry[DeleteIndex].NotifySem)
      {
        /* Make sure there are entries to move down */
        if((IntNotifyList->uiNumListEntries - DeleteIndex - 1) > 0)
        {
          /* Move the entries down to delete the entry */
          memcpy( &(IntNotifyList->IntNotifyEntry[DeleteIndex]),
                  &(IntNotifyList->IntNotifyEntry[DeleteIndex + 1]),
                     (IntNotifyList->uiNumListEntries - DeleteIndex - 1)
                       * sizeof(NOTIFY_ENTRY) );
        }
        /* Decrease the number of entries */
        IntNotifyList->uiNumListEntries--;
      }
      else
      {
        /* Go the next entry */
        DeleteIndex++;
      }
    }
  }

  /* Initialize the channel interrupt mask register */
  Value.ByteValue = GetChannelMask(theChannelData) |
                     ((UCHAR) theChannelData->InterruptLevel);
  if( Write6016ChannelRegister(theCardData, theChannelData, Channel,
      CH_MSK_REG, &Value) != NO_ERR )
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write Channel Register in Request6016IntNotify\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  semGive(theChannelData->AccessSem); /* Allow other access */
  theChannelData->ErrCode = NO_ERR;	/* Indicate success */

  return(NO_ERR); /* Return success */
}

/* configure one channel */
LOCAL V6016_STATUS
Config6016Channel(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
                  UINT Channel, FLOW_CTRL FlowControl, BAUD_RATE BaudRate,
                  TIMEOUTS InputTimeout, STOP_BITS StopBits,
                  PARITY_TYPE ParityType, PARITY_SENSE ParitySense,
                  NUM_DATA_BITS NumBits)
{
  REGISTER_VALUE_TYPE  Value;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL)) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Config6016Channel\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Check all the parameters */
  if((Channel >= theCardData->NumChannels) ||
    ((FlowControl  != NO_FLOW)            && (FlowControl  != XON_XOFF_FLOW) &&
     (FlowControl  != ANY_XOFF_FLOW)      && (FlowControl  != RTS_CTS_FLOW)) ||
    ((BaudRate     != BAUDS_50)           && (BaudRate     != BAUDS_110)  &&
     (BaudRate     != BAUDS_134p5)        && (BaudRate     != BAUDS_200)  &&
     (BaudRate     != BAUDS_300)          && (BaudRate     != BAUDS_600)  &&
     (BaudRate     != BAUDS_1200)         && (BaudRate     != BAUDS_1050) &&
     (BaudRate     != BAUDS_2400)         && (BaudRate     != BAUDS_4800) &&
     (BaudRate     != BAUDS_7200)         && (BaudRate     != BAUDS_9600) &&
     (BaudRate     != BAUDS_38400)        && (BaudRate     != BAUDS_75)   &&
     (BaudRate     != BAUDS_150)          && (BaudRate     != BAUDS_2000) &&
     (BaudRate     != BAUDS_1800)         && (BaudRate     != BAUDS_19200)) ||
((InputTimeout != NO_TIMEOUT)        && (InputTimeout != THREE_CHAR_TIMEOUT) &&
 (InputTimeout != HALF_SEC_TIMEOUT)   && (InputTimeout != ONE_SEC_TIMEOUT)) ||
   ((StopBits     != ONE_STOP_BIT)       && (StopBits     != TWO_STOP_BITS)) ||
    ((ParityType   != ODD_OR_EVEN_PARITY) && (ParityType   != FORCE_PARITY) &&
     (ParityType   != NO_PARITY)) ||
    ((ParitySense  != EVEN_PARITY)        && (ParitySense  != ODD_PARITY)) ||
    ((NumBits      != FIVE_BITS)          && (NumBits      != SIX_BITS) &&
     (NumBits      != SEVEN_BITS)         && (NumBits      != EIGHT_BITS)))
  {
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /* Wait for access to this channel				 */
  if(semTake(theChannelData->AccessSem, TEN_SECONDS) == ERROR)
  {
#if	DEBUG
    logMsg("ERROR: Cannot Access Channel Semaphore in Config6016Channel\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = RESOURCE_ERR;
    return(RESOURCE_ERR);
  }

  /* Write the new value of the Channel Control Byte 1 Register */
  Value.ByteValue = BaudRate | FlowControl | RING_BUFFER;
  if(Write6016ChannelRegister(theCardData, theChannelData, Channel,
     CH_CON1_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write Channel Register in Config6016Channel\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  /* Set the value requested for the Channel Control Byte 2 Register */
  Value.ByteValue =
    (InputTimeout | StopBits | ParityType | ParitySense | NumBits);
  if(Write6016ChannelRegister(theCardData, theChannelData, Channel,
     CH_CON2_REG, &Value) != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Write Channel Register in Config6016Channel\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  semGive(theChannelData->AccessSem); /* Allow other access */
  theChannelData->ErrCode = NO_ERR; /* Indicate success */

  return(NO_ERR); /* Return success */
}

/* return error code from channel's last access */
LOCAL V6016_STATUS
Get6016DeviceError(CHANNEL_DATA *theChannelData, V6016_STATUS *pStatus)
{
  /* Verify that the Parameters are within range */
  if(theChannelData == ((CHANNEL_DATA *) NULL))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Get6016DeviceError\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Verify that there is a return status pointer */
  if(pStatus == ((V6016_STATUS *) NULL))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Get6016DeviceError\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }
  *pStatus = theChannelData->ErrCode; /* Store the error code */
  theChannelData->ErrCode = NO_ERR; /* Indicate success */

  return(NO_ERR); /* Return success */
}

/* read channel control block register */
LOCAL V6016_STATUS
Read6016ChannelRegister(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
                        UINT Channel, CHANNEL_REG ChannelReg,
                        REGISTER_VALUE_TYPE *pValue)
{
  ULONG   Address;
  UCHAR  *puchVmePtr;
  USHORT *puhVmePtr;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL)) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Read6016ChannelRegister\n",
           0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Verify the parameters */
  if((Channel >= theCardData->NumChannels) ||
     (pValue == ((REGISTER_VALUE_TYPE *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Read6016ChannelRegister\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /* Calculate the address to read */
  Address = theCardData->BoardAddress + CHANNEL_REG_OFFSET
           + (CHANNEL_REG_SIZE * Channel) + ChannelReg;
  /* Determine the register */
  switch(ChannelReg)
  {
    default:
#if	DEBUG
      logMsg("ERROR: Invalid Parameters in Read6016ChannelRegister\n",
             0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = INVALID_PARAMETER_ERR;
      return(INVALID_PARAMETER_ERR);
    case CH_MSK_REG:
      /* FALLTHROUGH */
    case CH_VEC_REG:
      /* FALLTHROUGH */
    case EOB_REG:
      /* FALLTHROUGH */
    case XOFF_REG:
      /* FALLTHROUGH */
    case XON_REG:
      /* FALLTHROUGH */
    case BRK_DUR_REG:
      /* FALLTHROUGH */
    case SZ_RING_REG:
      /* FALLTHROUGH */
    case LO_RING_REG:
      /* FALLTHROUGH */
    case HI_RING_REG:
      /* FALLTHROUGH */
    case CH_CON1_REG:
      /* FALLTHROUGH */
    case CH_CON2_REG:
      /* FALLTHROUGH */
    case SZ_UBUF_REG:
      puchVmePtr = (UCHAR *) Address;
      pValue->ByteValue = *puchVmePtr;
      break;
    case CST_REG:
      puhVmePtr = (USHORT *) Address;
      pValue->WordValue = *puhVmePtr;
      break;
  }
  theChannelData->ErrCode = NO_ERR;	/* Indicate success */

  return(NO_ERR); /* Return success */
}

/* write a channel control block register */
LOCAL V6016_STATUS 
Write6016ChannelRegister(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
                         UINT Channel, CHANNEL_REG ChannelReg,
                         REGISTER_VALUE_TYPE *pValue)
{
  UCHAR  *puchVmePtr;
  USHORT *puhVmePtr;
  ULONG   Address;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL) ) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: NULL Card Data in Write6016ChannelRegister\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Verify the parameters */
  if((Channel >= theCardData->NumChannels) ||
     (pValue == ((REGISTER_VALUE_TYPE *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Write6016ChannelRegister\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /* Calculate the address to write */
  Address = theCardData->BoardAddress + CHANNEL_REG_OFFSET
           + (CHANNEL_REG_SIZE * Channel) + ChannelReg;
  /* Determine the register */
  switch(ChannelReg)
  {
    default:
#if	DEBUG
      logMsg("ERROR: Invalid Register Value in Write6016ChannelRegister\n",
             0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = INVALID_PARAMETER_ERR;
      return(INVALID_PARAMETER_ERR);
    case CH_MSK_REG:
      /* FALLTHROUGH */
    case CH_VEC_REG:
      /* FALLTHROUGH */
    case EOB_REG:
      /* FALLTHROUGH */
    case XOFF_REG:
      /* FALLTHROUGH */
    case XON_REG:
      /* FALLTHROUGH */
    case BRK_DUR_REG:
      /* FALLTHROUGH */
    case SZ_RING_REG:
      /* FALLTHROUGH */
    case LO_RING_REG:
      /* FALLTHROUGH */
    case HI_RING_REG:
      /* FALLTHROUGH */
    case CH_CON1_REG:
      /* FALLTHROUGH */
    case CH_CON2_REG:
      /* FALLTHROUGH */
    case SZ_UBUF_REG:
      puchVmePtr = (UCHAR *) Address;
      *puchVmePtr = pValue->ByteValue;
      break;
    case CST_REG:
      puhVmePtr = (USHORT *) Address;
      *puhVmePtr = pValue->WordValue;
      break;
  }
  theChannelData->ErrCode = NO_ERR; /* Indicate success */

  return(NO_ERR); /* Return success */
}

/* read channel's user buffer */
LOCAL V6016_STATUS
Read6016Buffer(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
               UINT Channel, BUFFER_TYPE WhichBuffer, ULONG Offset,
               UINT ByteCount, UCHAR *pBuffer)
{
  ULONG                BufferBase;
  ULONG                MemoryOption;
  ULONG                BufferSize;
  ULONG                VMEAddress;
  UINT                 ActualByteCount;
  REGISTER_VALUE_TYPE  Value;
  ULONG                AccessMode;
  UCHAR               *puchVmePtr;
  int                  iByte;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL)) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Read6016Buffer\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Make sure the parameters are correct */
  if((Channel >= theCardData->NumChannels) ||
     (ByteCount <= 0) ||
     ((WhichBuffer != TX_BUFFER) && (WhichBuffer != RX_BUFFER)) ||
     (pBuffer == ((UCHAR *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Read6016Buffer\n",0,0,0,0,0,0);
    logMsg("Channel   = %d\n",Channel,0,0,0,0,0);
    logMsg("ByteCount = %d\n",ByteCount,0,0,0,0,0);
    logMsg("pBuffer   = %d\n",(int)pBuffer,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /* Wait for access to this channel */
  if(semTake(theChannelData->AccessSem, TEN_SECONDS) == ERROR)
  {
#if	DEBUG
    logMsg("ERROR: Cannot Access Channel Semaphore in Read6016Buffer\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = RESOURCE_ERR;
    return(RESOURCE_ERR);
  }

  /* Read the Buffer Address */
  if(Read6016GlobalRegister(theCardData, theChannelData, BUFBASE_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in Read6016Buffer\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  BufferBase = Value.LongValue;

  /*
   * The buffer size in Slave Mode is always 4K times the memory
   * option in the ROM version register.
   *
   * Read the ROM version register.
   */
  if(Read6016GlobalRegister(theCardData, theChannelData, ROM_VER_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in Read6016Buffer\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  MemoryOption = 1 << ((Value.WordValue & MEM_OPT) >> ROM_VER_SHIFT);

  /* Calculate the buffer size */
  BufferSize = BUFFER_INC * MemoryOption;
  /* Calculate the VME Bus Address to read the buffer */
  VMEAddress = BufferBase + (BufferSize * 2 * Channel) + Offset;
  /* Offset this if this is a read receive buffer command */
  if(WhichBuffer == RX_BUFFER) VMEAddress = VMEAddress + BufferSize;

  /*
   * Make sure the user didn't ask for more than there is. We will
   * allow him to read past the actual used portion of the buffer,
   * but not into another channels portion of the buffer or into the
   * receive buffer from the transmit buffer.
   */
  if((Offset + ByteCount) >= BufferSize)
  {
    /* Modify the byte count to read all bytes available from given offset */
    ActualByteCount = (UINT) (BufferSize - Offset);
  }
  else
  {
    /* Otherwise, we will read the number of bytes the user wants */
    ActualByteCount = ByteCount;
  }
  /* Determine the VME Access Mode */
  AccessMode = ReadAccessMode(theCardData, theChannelData);
#if	0
  logMsg("Reading %d Bytes From %lx\n",ActualByteCount,VMEAddress,0,0,0,0);
  logMsg("Access Mode = %d, Bus Request Level = %d\n",AccessMode,
         theCardData->BusRequestLevel,0,0,0,0);
#endif
  /* Read the buffer bytes */
  puchVmePtr = (UCHAR *) VMEAddress;
  for(iByte = 0; iByte < ActualByteCount; iByte++)
  {
    pBuffer[iByte] = puchVmePtr[iByte];
  }
  semGive(theChannelData->AccessSem); /* Allow other access */
  theChannelData->ErrCode = NO_ERR; /* Indicate success */
  return(NO_ERR); /* Return success */
}

/* write channel's user buffer */
LOCAL V6016_STATUS
Write6016Buffer(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
                UINT Channel, BUFFER_TYPE WhichBuffer, ULONG Offset,
                UINT ByteCount, UCHAR *pBuffer)
{
  ULONG                BufferBase;
  ULONG                MemoryOption;
  ULONG                BufferSize;
  ULONG                VMEAddress;
  UINT                 ActualByteCount;
  REGISTER_VALUE_TYPE  Value;
  ULONG                AccessMode;
  int                  iByte;
  UCHAR               *puchVmePtr;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL) ) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Write6016Buffer\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Make sure the parameters are correct */
  if((Channel >= theCardData->NumChannels) ||
     (ByteCount <= 0) ||
     ((WhichBuffer != TX_BUFFER) && (WhichBuffer != RX_BUFFER)) ||
     (pBuffer == ((UCHAR *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in Write6016Buffer\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = INVALID_PARAMETER_ERR;
    return(INVALID_PARAMETER_ERR);
  }

  /* Wait for access to this channel */
  if(semTake(theChannelData->AccessSem, TEN_SECONDS) == ERROR)
  {
#if	DEBUG
    logMsg("ERROR: Cannot Access Channel Semaphore in Write6016Buffer\n",
           0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = RESOURCE_ERR;
    return(RESOURCE_ERR);
  }

  /* Read the Buffer Address */
  if(Read6016GlobalRegister(theCardData, theChannelData, BUFBASE_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in Write6016Buffer\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  BufferBase = Value.LongValue;

  /*
   * The buffer size in Slave Mode is always 4K times the memory
   * option in the ROM version register.
   *
   * Read the ROM version register.
   */
  if(Read6016GlobalRegister(theCardData, theChannelData, ROM_VER_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in Write6016Buffer\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }
  MemoryOption = 1 << ((Value.WordValue & MEM_OPT) >> ROM_VER_SHIFT);

  /* Calculate the buffer size */
  BufferSize = BUFFER_INC * MemoryOption;
  /* Calculate the VME Bus Address to read the buffer */
  VMEAddress = BufferBase + (BufferSize * 2 * Channel) + Offset;
  /* Offset this if this is a read receive buffer command */
  if(WhichBuffer == RX_BUFFER) VMEAddress = VMEAddress + BufferSize;

  /*
   * Make sure the user didn't ask for more than there is. We will
   * allow him to read past the actual used portion of the buffer,
   * but not into another channels portion of the buffer or into the
   * receive buffer from the transmit buffer.
   */
  if((Offset + ByteCount) >= BufferSize)
  {
    /* Modify the byte count to read all bytes available from given offset */
    ActualByteCount = (UINT) (BufferSize - Offset);
  }
  else
  {
    /* Otherwise, we will read the number of bytes the user wants */
    ActualByteCount = ByteCount;
  }

  /* Determine the VME Access Mode */
  AccessMode = ReadAccessMode(theCardData, theChannelData);
#if	0
  logMsg("Writing %d Bytes To %lx\n",ActualByteCount,VMEAddress,0,0,0,0);
  logMsg("Access Mode = %d, Bus Request Level = %d\n",AccessMode,
         theCardData->BusRequestLevel,0,0,0,0);
#endif
  /* Write the buffer bytes */
  puchVmePtr = (UCHAR *) VMEAddress;
  for(iByte = 0; iByte < ActualByteCount; iByte++)
  {
    puchVmePtr[iByte] = pBuffer[iByte];
  }
  semGive(theChannelData->AccessSem); /* Allow other access */
  theChannelData->ErrCode = NO_ERR; /* Indicate success */

  return(NO_ERR); /* Return success */
}

/*
 *------------------------------------------------------------------------
 * ErrorIntHandler: error interrupt entry point
 *------------------------------------------------------------------------
 * This routine is tied to any errors occurring at the V6016 card level.
 * This routine searches through the card list trying to find out which
 * card interrupted, by matching the interrupt vector sent as a paramter
 * from the VME Access program to the vector used by a particular V6016
 * card in the card list. Next this routine cycles through the interrupt
 * notification list and notifies any routine that wanted to be signaled
 * of the occurrence of an error.
 *------------------------------------------------------------------------
 */

LOCAL ULONG 
ErrorIntHandler(ULONG VectorID)
{
  UINT          Channel;
  UINT          Entry;
  UINT          Card;
  CHANNEL_DATA *theChannelData;
  NOTIFY_ENTRY *theNotifyEntry;

#if	DEBUG
  logMsg("INFO: error int handler reached\n",0,0,0,0,0,0);
#endif

  /* Remove the card from the card list */
  for(Card = 0; Card < CardListSize; Card++)
  {
    /* See if this is the card */
    if(CardList[Card]->InterruptVector == VectorID)
    {
      /* Go through all the channels and see if anybody wanted to know */
      for(Channel = 0; Channel < CardList[Card]->NumChannels; Channel++)
      {
        /* Get pointer to channel data */
		theChannelData = &(CardList[Card]->ChannelData[Channel]);

        /* Make sure there is a notify list, just in case */
        if(theChannelData->IntNotifyList.IntNotifyEntry
           != ((NOTIFY_ENTRY *) NULL))
        {
          /* Go through all the notify list entries */
          for(Entry = 0; Entry<theChannelData->IntNotifyList.uiNumListEntries;
			  Entry++)
          {
            /* Get the notify entry */
            theNotifyEntry =
              &(theChannelData->IntNotifyList.IntNotifyEntry[Entry]);

            /* Make sure there is a signal, just in case */
            if(theNotifyEntry->NotifySem != ((SEM_ID) NULL))
            {
              /* See if the error interrupt was selected */
              if(theNotifyEntry->NotifyConditions & ERROR_INT_MASK)
              {
                /* Notify the user via semaphore */
                semGive(theNotifyEntry->NotifySem);
              }
            }
          }
        }
      }
    }
  }

  return(0); /* Always return zero */
}

/*
 *------------------------------------------------------------------------
 * ChannelIntHandler: channel interrupt handler
 *------------------------------------------------------------------------
 * This routine is tied to any conditions occurring at the V6016 channel
 * level. This routine searches through the card list trying to find out
 * which card/channel interrupted, by matching the interrupt vector sent
 * as a paramter from the VME Access program to the vector used by a
 * particular V6016 card/channel in the card list. Next this routine
 * cycles through the interrupt notification list and notifies any routine
 * that wanted to be signaled of the occurrence of this interrupt.
 * *______________________________________________________________________
 */

LOCAL ULONG
ChannelIntHandler(ULONG VectorID)
{
  UINT                  Card;
  UINT                  Channel;
  UINT                  Entry;
  NOTIFY_ENTRY         *theNotifyEntry;
  CHANNEL_DATA         *theChannelData;
  REGISTER_VALUE_TYPE   Value;
  char                  ch;

#if	DEBUG
  logMsg("INFO: int handler reached, vector 0x%02x\n",VectorID,0,0,0,0,0);
#endif

  /* Search through all the cards */
  for(Card=0; Card<CardListSize; Card++)
  {
    /* Go through all the channels and see if anybody wanted to know */
    for(Channel=0; (Channel < CardList[Card]->NumChannels); Channel++)
    {
      /* Get pointer to channel data */
      theChannelData = &(CardList[Card]->ChannelData[Channel]);

      /* See if this is the card */
      if(theChannelData->InterruptVector == VectorID)
      {
#if 0
           #warning remove
           *(int *)(0xfffc0400)&=0x00080000;
           while(*(int*)(0xfffc0400)&0x00040000);
#endif
        /* Read the channel status register and see if this
         channel provided the interrupt, and what we have to do */
        if((Read6016ChannelRegister(CardList[Card], theChannelData,
          Channel, CST_REG, &Value) == NO_ERR) &&
		  (Value.WordValue & INTBSY) )
        {
#if 0
logMsg("interrupt from channel %d\n",Channel,0,0,0,0,0);
#endif
          /* Determine if this is a TTY device */
          if(theChannelData->bIsTTY)
          {
            /* Determine if this is a receive interrupt */
            if(Value.WordValue & URCV_RDY)
            {
logMsg("receive interrupt\n",0,0,0,0,0,0);
#undef BUFFERED
#ifdef BUFFERED
              char buffer[8];
              int i, x;
              int nBytesRead=0;
              int loop = 1;
              while(loop == 1)
              {
                /* Read character from the card */
                if((nBytesRead = Read6016Data(theChannelData->pCardData,
                                         theChannelData,
                                         theChannelData->Channel,
                                         (UCHAR *) buffer,
                                         sizeof(buffer))) > 0)
                {
                  /* Send it to the TTY buffer */
                  for(i=0; i<nBytesRead; i++)
                  {
                    ch = buffer[i];
                    if(tyIRd (&(theChannelData->DevHdr), ch) != OK)
                    {
                      logMsg("Error sending to the ty ring buffer.\n",
                        0,0,0,0,0,0);
                    }
                  }
                }
                else
                {
                  if(nBytesRead<sizeof(buffer)) loop = 0; 
                }
              } /* While */
#else
              while(1)
              {
                /* Read character from the card. */
                if(Read6016Data(theChannelData->pCardData,
                                         theChannelData,
                                         theChannelData->Channel,
                                         (UCHAR *) &ch, 1) == 1)
                {
                  /* Send it to the TTY buffer. */
                  if(tyIRd(&(theChannelData->DevHdr), ch) != OK)
                  {
                    logMsg("Error sending to the ty ring buffer.\n",
                      0,0,0,0,0,0);
                  }
                }
                else
			    {
                  break;
				}
              } /* While */
#endif
            }

            /* Determine if this is a transmitter interrupt */
            if(Value.WordValue & UTX_RDY)
            {
logMsg("transmitter interrupt\n",0,0,0,0,0,0);
              /* See if there is another character to send out in
              the TTY buffer. */
    	      if(tyITx(&(theChannelData->DevHdr), &ch) == OK)
              {
                /* Send character to the card to send. */
                Write6016Data(theChannelData->pCardData,
                              theChannelData,
                              theChannelData->Channel,
                              (UCHAR *) &ch, 1);
              }
            }
          }
          else
          {
#if 0
logMsg("not TTY interrupt\n",0,0,0,0,0,0);
#endif
            /* Make sure there is a notify list, just in case */
            if(theChannelData->IntNotifyList.IntNotifyEntry !=
               ((NOTIFY_ENTRY *) NULL) )
            {
              /* Go through all the notify list entries */
              for(Entry=0;
                  Entry<theChannelData->IntNotifyList.uiNumListEntries;
                  Entry++)
              {
#if 0
logMsg("Entry # %d\n",Entry,0,0,0,0,0);
#endif
                /* Get the notify entry */
                theNotifyEntry =
                  &(theChannelData->IntNotifyList.IntNotifyEntry[Entry]);
                /* Make sure there is a semaphore, just in case */
                if(theNotifyEntry->NotifySem != ((SEM_ID) NULL))
                {
                  /* See if any of the conditions match the Channel Status
                  Register */
#if 0
logMsg("[%2d] NotifyConditions=0x%08x, WordValue=0x%08x\n",
Entry,(USHORT)theNotifyEntry->NotifyConditions,Value.WordValue,0,0,0);
#endif
                  if( ((USHORT) theNotifyEntry->NotifyConditions) &
                     Value.WordValue)
                  {
					/* Notify the user via semaphore */
logMsg("match: 0x%08x-0x%08x, theNotifyEntry=0x%08x, semGive(0x%08x)\n",
(USHORT)theNotifyEntry->NotifyConditions,Value.WordValue,
(int)theNotifyEntry,(int)theNotifyEntry->NotifySem,
0,0);
					semGive(theNotifyEntry->NotifySem);
                  }
                }
              }
            }
          }

          /* Clear the interrupt busy bit. No reason to
          check the return status of this call because we
          won't do anything anyway */
          Value.WordValue = Value.WordValue & (~INTBSY);
          Write6016ChannelRegister(CardList[Card], theChannelData,
                                   Channel, CST_REG, &Value);
        }

      }
    }
  }

#if 0
#warning remove
  *(int *)(0xfffc0400)&=(~0x00080000);
#endif

  return(0); /* Always return zero */
}

/* write a register after checking for "register busy" */
LOCAL V6016_STATUS
WriteBusyReg(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData,
             GLOBAL_REG Register, REGISTER_VALUE_TYPE *Value)
{
  UINT                 TimeoutCount;
  REGISTER_VALUE_TYPE  GSTValue;
  UCHAR               *puchVmePtr;
  USHORT              *puhVmePtr;
  BOOL                 Wait = FALSE;

  /* Determine which register it is */
  switch(Register)
  {
    default:
      break;
    case CTR0_REG:
      /* FALLTHROUGH */
    case CTR1_REG:
      /* FALLTHROUGH */
    case GO_REG:
      /* FALLTHROUGH */
    case TX_REG:
      /* FALLTHROUGH */
    case RX_REG:
      TimeoutCount = 0;
      do
      {
        /* Wait for the busy bit to clear */
        if(Read6016GlobalRegister(theCardData, theChannelData, GST_REG,
           &GSTValue) != NO_ERR)
        {
#if	DEBUG
          logMsg("ERROR: Failed to Read in WriteBusyReg\n",0,0,0,0,0,0);
#endif
          theChannelData->ErrCode = BOARD_ACCESS_ERR;
          return(BOARD_ACCESS_ERR);
        }
        /* Increment timeout count */
        if(TimeoutCount > 0) taskDelay(ONE_SEC / 10);
		TimeoutCount++;
      } while((GSTValue.WordValue & REGBUSY) && (TimeoutCount<HUNDRED_TIMES));
      /* See if we timed out */
      if(GSTValue.WordValue & REGBUSY)
      {
#if	DEBUG
        logMsg("ERROR: Register Busy Never Completed\n",0,0,0,0,0,0);
#endif
        theChannelData->ErrCode = BOARD_ACCESS_ERR;
        return(BOARD_ACCESS_ERR);
      }
      /* Preset the Busy Bit to avoid race conditions */
      if(Read6016GlobalRegister(theCardData, theChannelData, GST_REG,
         &GSTValue) != NO_ERR)
      {
#if	DEBUG
        logMsg("ERROR: Failed to Read in WriteBusyReg\n",0,0,0,0,0,0);
#endif
        theChannelData->ErrCode = BOARD_ACCESS_ERR;
        return(BOARD_ACCESS_ERR);
      }
      GSTValue.WordValue = GSTValue.WordValue | REGBUSY;
      if(Write6016GlobalRegister(theCardData, theChannelData, GST_REG,
         &GSTValue) != NO_ERR)
      {
#if	DEBUG
        logMsg("ERROR: Failed to Write in WriteBusyReg\n",0,0,0,0,0,0);
#endif
        theChannelData->ErrCode = BOARD_ACCESS_ERR;
        return(BOARD_ACCESS_ERR);
      }
      /* Write the Register Value */
      if((Register == CTR0_REG) || (Register == CTR1_REG))
      {
        puchVmePtr = (UCHAR *) (theCardData->BoardAddress + Register);
        *puchVmePtr = Value->ByteValue;
      }
      else
      {
        puhVmePtr = (USHORT *) (theCardData->BoardAddress + Register);
        *puhVmePtr = Value->WordValue;
      }
      Wait = TRUE; /* Indicate we must wait */
      break;
    case CR2_REG:
      /* FALLTHROUGH */
    case RES_REG:
      /* FALLTHROUGH */
    case SZ_AM_REG:
      /* FALLTHROUGH */
    case ST_PROC_REG:
      /* FALLTHROUGH */
    case ER_MSK_REG:
      /* FALLTHROUGH */
    case ER_VEC_REG:
      /* FALLTHROUGH */
    case MAS_GRN_REG:
      /* FALLTHROUGH */
    case BREAK_REG:
      /* FALLTHROUGH */
    case GST_REG:
      /* FALLTHROUGH */
    case BUFBASE_REG:
      /* Write the Register Value				 */
      if(Write6016GlobalRegister(theCardData, theChannelData, Register, Value)
         != NO_ERR)
      {
#if	DEBUG
        logMsg("ERROR: Failed to Write in WriteBusyReg\n",0,0,0,0,0,0);
#endif
        theChannelData->ErrCode = BOARD_ACCESS_ERR;
        return(BOARD_ACCESS_ERR);
      }
      break;
  }

  /* Wait for the busy bit to clear */
  if(Wait)
  {
    TimeoutCount = 0;
    do
    {
      if(Read6016GlobalRegister(theCardData, theChannelData, GST_REG,&GSTValue)
         != NO_ERR)
      {
#if	DEBUG
        logMsg("ERROR: Failed to Read in WriteBusyReg\n",0,0,0,0,0,0);
#endif
        theChannelData->ErrCode = BOARD_ACCESS_ERR;
        return(BOARD_ACCESS_ERR);
      }

      /* Increment timeout count */
      if(TimeoutCount > 0) taskDelay(ONE_SEC / 10);
      TimeoutCount++;
    } while((GSTValue.WordValue & REGBUSY) && (TimeoutCount < HUNDRED_TIMES));

    /* See if we timed out */
    if(GSTValue.WordValue & REGBUSY)
    {
#if	DEBUG
      logMsg("ERROR: Register Busy Never Completed\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      return(BOARD_ACCESS_ERR);
    }
  }

  return(NO_ERR); /* Return success */
}

/* allocate more space to notification list */
LOCAL V6016_STATUS 
AllocateNotifySpace(NOTIFY_LIST *IntNotifyList)
{
  UINT  FirstIndex;
  UINT  LastIndex;
  UINT  InitIndex;

  /* Determine if we have ever allocated space for this table */
  if(IntNotifyList->IntNotifyEntry == ((NOTIFY_ENTRY *) NULL))
  {
    /* Allocate new space */
    IntNotifyList->IntNotifyEntry =
      (NOTIFY_ENTRY *) malloc( sizeof(NOTIFY_ENTRY) * ALLOC_INC );
    /* Set the new size */
    IntNotifyList->uiListSize = ALLOC_INC;
    /* Initialize indices for initialization of new area */
    FirstIndex = 0;
    LastIndex = ALLOC_INC - 1;
  }
  else
  {
    /* Reallocate space */
    IntNotifyList->IntNotifyEntry =
      (NOTIFY_ENTRY *) realloc( IntNotifyList->IntNotifyEntry,
        (sizeof(NOTIFY_ENTRY) * (ALLOC_INC + IntNotifyList->uiListSize)) );
    /* Set indices of new entries */
    FirstIndex = IntNotifyList->uiListSize;
    LastIndex = FirstIndex + ALLOC_INC - 1;
    /* Set the new size */
    IntNotifyList->uiListSize += ALLOC_INC;
  }

  /* Make sure we got memory */
  if(IntNotifyList->IntNotifyEntry == ((NOTIFY_ENTRY *) NULL))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Ioctl Parameters in AllocateNotifySpace\n",
           0,0,0,0,0,0);
#endif
    return(RESOURCE_ERR);
  }

  /* Initialize new members */
  for(InitIndex = FirstIndex; InitIndex <= LastIndex; InitIndex++)
  {
    IntNotifyList->IntNotifyEntry[InitIndex].NotifyConditions = 0L;
    IntNotifyList->IntNotifyEntry[InitIndex].NotifySem = (SEM_ID) NULL;
  }

  return(NO_ERR); /* Return success */
}

/* "start serial channel from running" (sic) */
LOCAL V6016_STATUS
StartChannel(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData)
{
  REGISTER_VALUE_TYPE  Value;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL)) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in StartChannel\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }
  if(semTake(theCardData->GlobalRegSem,TEN_SECONDS) == OK)
  {
    /* Write the GO register value to turn on this channel */
    if(Read6016GlobalRegister(theCardData, theChannelData, GO_REG, &Value)
       != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Read in StartChannel\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      semGive(theCardData->GlobalRegSem);
      return(BOARD_ACCESS_ERR);
    }
    Value.WordValue = Value.WordValue | (1 << theChannelData->Channel);
    if(WriteBusyReg(theCardData, theChannelData, GO_REG, &Value) != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Write Register in StartChannel\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      semGive(theCardData->GlobalRegSem);
      return(BOARD_ACCESS_ERR);
    }
    /* Write the RX register value to turn on this channel */
    if(Read6016GlobalRegister(theCardData, theChannelData, RX_REG, &Value)
       != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Read in StartChannel\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      semGive(theCardData->GlobalRegSem);
      return(BOARD_ACCESS_ERR);
    }
    Value.WordValue = Value.WordValue | (1 << theChannelData->Channel);
    if(WriteBusyReg(theCardData, theChannelData, RX_REG, &Value) != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Write Register in StartChannel\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      semGive(theCardData->GlobalRegSem);
      return( BOARD_ACCESS_ERR );
    }
    semGive(theCardData->GlobalRegSem);
  }
  else
  {
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  return(NO_ERR); /* Return success */
}

/* stop the serial channel from running */
LOCAL V6016_STATUS
StopChannel(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData)
{
  REGISTER_VALUE_TYPE Value;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL)) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in StopChannel\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  if(semTake(theCardData->GlobalRegSem,TEN_SECONDS) == OK)
  {
    /* Write the GO register value to turn off this channel */
    if(Read6016GlobalRegister(theCardData, theChannelData, GO_REG, &Value)
       != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Read in StartChannel\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      semGive(theCardData->GlobalRegSem);
      return(BOARD_ACCESS_ERR);
    }
    Value.WordValue = Value.WordValue & (~(1 << theChannelData->Channel));
    if(WriteBusyReg(theCardData, theChannelData, GO_REG, &Value) != NO_ERR)
    {
#if	DEBUG
      logMsg("ERROR: Failed to Write Register in StopChannel\n",0,0,0,0,0,0);
#endif
      theChannelData->ErrCode = BOARD_ACCESS_ERR;
      semGive(theCardData->GlobalRegSem);
      return(BOARD_ACCESS_ERR);
    }
    semGive(theCardData->GlobalRegSem);
  }
  else
  {
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return (BOARD_ACCESS_ERR);
  }

  return(NO_ERR); /* Return success */
}

/* read and display required access mode for card */
LOCAL ULONG
ReadAccessMode(CARD_DATA *theCardData, CHANNEL_DATA *theChannelData)
{
  ULONG                AccessMode;
  REGISTER_VALUE_TYPE  Value;

  /* Verify that the Parameters are within range */
  if((theCardData == ((CARD_DATA *) NULL)) ||
     (theChannelData == ((CHANNEL_DATA *) NULL)))
  {
#if	DEBUG
    logMsg("ERROR: Invalid Parameters in ReadAccessMode\n",0,0,0,0,0,0);
#endif
    return(INVALID_PARAMETER_ERR);
  }

  /* Read the Control Register 0 */
  if(Read6016GlobalRegister(theCardData, theChannelData, CTR0_REG, &Value)
     != NO_ERR)
  {
#if	DEBUG
    logMsg("ERROR: Failed to Read in ReadAccessMode\n",0,0,0,0,0,0);
#endif
    theChannelData->ErrCode = BOARD_ACCESS_ERR;
    return(BOARD_ACCESS_ERR);
  }

  /* Determine if the buffer is in A32 space */
  if(Value.ByteValue & SLV_32)
  {
    /* Determine if the buffer is to be accessed in supervisor mode */
    if(Value.ByteValue & SUPONLY)
    {
      AccessMode = V_A32SD;
    }
    else
    {
      AccessMode = V_A32UD;
    }
  }
  else
  {
    /* Determine if the buffer is to be accessed in supervisor mode */
    if(Value.ByteValue & SUPONLY)
    {
      AccessMode = V_A24SD;
    }
    else
    {
      AccessMode = V_A24UD;
    }
  }

  return(AccessMode); /* Return access mode */
}

/* obtain channel interrupt notification mask */
LOCAL UCHAR
GetChannelMask(CHANNEL_DATA *theChannelData)
{
  UINT          Entry;
  UCHAR         Conditions;
  NOTIFY_ENTRY *theNotifyEntry;

  /* Go through all the notify list entries */
  Conditions = 0;
  for(Entry = 0; Entry<theChannelData->IntNotifyList.uiNumListEntries; Entry++)
  {
    /* Get the notify entry */
    theNotifyEntry = &(theChannelData->IntNotifyList.IntNotifyEntry[Entry]);

    /* Add in conditions registered for */
    if(theNotifyEntry->NotifyConditions & (DCD_COS | CTS_COS))
    {
      Conditions |= COS_MSK;
    }
    if(theNotifyEntry->NotifyConditions & (FERR|PERR|OVERRUN))
    {
      Conditions |= ERR_MSK;
    }
    if(theNotifyEntry->NotifyConditions & BRK_RCVD)
    {
      Conditions |= BREAK_MSK;
    }
    if(theNotifyEntry->NotifyConditions & URCV_RDY)
    {
      Conditions |= RCV_MSK;
    }
    if(theNotifyEntry->NotifyConditions & UTX_RDY)
    {
      Conditions |= TX_MSK;
    }
  }
  if(theChannelData->bIsTTY)
  {
    Conditions |= RCV_MSK;
    Conditions |= TX_MSK;
  }

  return(Conditions); /* Return them */
}
