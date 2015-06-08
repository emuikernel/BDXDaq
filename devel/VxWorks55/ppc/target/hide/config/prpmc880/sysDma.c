/* sysDma.c - Device independent user callable DMA functions */

/* Copyright 2003 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01a,02apr03,simon  Ported. from ver 01d, mcpm905/sysDma.c.
*/

/*
DESCRIPTION

This file provides device independent top-level DMA routines for general
use. By using these routines, applications will not be affected by
DMA controller differences in future BSPs.

The DMA functions provided support initialization, configuration, 
start, pause, resume, abort and status retrieval of DMA operations on a 
per channel basis. Other device specific functions may be available in
the <device>Dma.c file, but may not always be available in the future.

The DMA initialization routine must be called once before any other DMA
routine is invoked. Failure to do so will result in status error returns
from all other DMA functions. This initialization is handled by sysHwInit2 
whenever INCLUDE_<device>_DMA is defined in config.h. 

To start DMA for a particular channel, invoke the start routine with
the desired addresses, byte count and transfer attributes. These 
parameters are specified via separate descriptor and attribute 
structures defined in sysDma.h. 

DMA is supported from DRAM to DRAM, DRAM to PCI, PCI to DRAM, and PCI to 
PCI by appropriate selection of the source and destination addresses 
supplied in the DMA descriptor. The starting address for either or both
source and destination may be optionally held (no increment) during the 
transfer.

The user interrupt handler, if provided, must conform to the rules for 
standard VxWorks interrupt service routines. It is called with a current
channel parameter during handling of both normal completion and error 
interrupts. Since this routine is called by the driver interrupt handlers,
the user routine is not responsible for clearing interrupts. Interrupts 
are cleared after the user routine returns to the driver's handler.

All parameter validation is performed in the actual driver functions.
*/

/* includes */

#include "vxWorks.h"
#include "config.h"
#include "sysDma.h"

/* defines */

/******************************************************************************
*
* sysDmaInit - Initialize DMA and attach DMA Interrupt Handler.
*
* This function configures the DMA driver for all available DMA channels.
* The driver tracks information on each channel independently.
* Register defaults are also initialized.
*
* RETURNS: OK if initialization succeeded or ERROR if an error occurred.
*/

STATUS sysDmaInit (void)
    {
    return (DMA_INIT ());
    }

/******************************************************************************
*
* sysDmaStart - Configure and start the DMA controller.
*
* This function sets up the DMA controller for a block mode DMA transfer.
* The user must fill out the descriptor and attribute parameters.  For now
* only single descriptor block mode is supported. Transfer options are set
* according to the attributes. Then interrupts for the given channel are 
* enabled and the transfer is initiated. 
*
* The user has three options for being notified of the completion of a DMA
* transaction depending on the contents of the userHandler field in the
* dmaAttrib parameter. The first is to have the DMA start routine wait for
* the done interrupt. The start routine then returns with DMA status
* (dmaDesc.userHandler == WAIT_FOREVER).  The second option is to provide a 
* user routine for the DMA interrupt handler to call upon completion 
* (dmaDesc.userHandler == user routine). The routine must follow the rules 
* for executing inside an ISR by not calling waits, prints, etc. The 
* sysDmaStart routine returns immediately so the user task can proceed with its
* execution. The third option is for no user interrupt handler, but still
* allows sysDmaStart to return immediately (dmaDesc.userHandler == 0). In
* the later two cases sysDmaStatus can be called later to get the DMA results.
*
* Status is returned by sysDmaStart when the dmaStatus parameter is not
* NULL and when using dmaDesc.userHandler == WAIT_FOREVER or by calling
* sysDmaStatus directly.  In any case, the status should be interpreted 
* using the appropriate device status structure defined in <device>Dma.h.
*
* RETURNS: 
* OK is returned if the channel started successfully.
* ERROR is returned if the driver is not initialized, or the 
* channel is invalid or busy, or a parameter is invalid.
*/

STATUS sysDmaStart
    (
    UINT32          chan,      /* DMA channel to start */
    DMA_DESCRIPTOR *dmaDesc,   /* Source, Destination, Byte count */
    DMA_ATTRIBUTES *dmaAttrib, /* Configuration options */
    void           *dmaStatus  /* DMA status output */
    )
    {
    return (DMA_START (chan, dmaDesc, dmaAttrib, dmaStatus));
    }

/******************************************************************************
*
* sysDmaStatus - Read and return DMA status.
*
* For the given channel, provide the following DMA status information:
*
* The status information is placed into a user provided DMA status
* structure reference by dmaStatus. This structure should match the 
* device status structure defined in <device>Dma.h. and be interpreted as
* such. Status information can provide an indication of successful DMA
* completion, in-progress, or error conditions along with supporting
* diagnostic information. See the device specific function call for
* details in <device>Dma.c.
*
* RETURNS: 
* OK is returned if the dmaStatus is valid.
* ERROR is returned if the driver is not initialized, or the 
* channel is invalid or the dmaStatus reference is null. The 
* dmaStatus contents will not be valid.
*/

STATUS sysDmaStatus
    (
    UINT32  chan,          /* Channel to get status */
    void   *dmaStatus      /* Current status of channel */
    )
    {
    return (DMA_STATUS_GET (chan, dmaStatus));
    }

/******************************************************************************
*
* sysDmaAbort - Initiate an abort of the current DMA operation.
*
* For the given valid channel, (the channel must actually be busy), the 
* transfer is aborted after the current minimum transfer segment completes.
* The function waits for the abort to complete before returning. 
*
* RETURNS: 
* OK is returned if abort succeeded.
* ERROR is returned if the driver is not initialized or the 
* channel number is invalid or the channel is already stopped.
*/

STATUS sysDmaAbort
    (
    UINT32   chan		/* The active DMA channel to abort */
    )
    {
    return (DMA_ABORT (chan));
    }

/******************************************************************************
*
* sysDmaPause - Initiate a pause of the current DMA operation.
*
* For the given valid channel, if the channel is busy, pause the channel. 
* The channel will pause after completing any current transfer segment in
* progress.
*
* RETURNS: 
* OK is returned if pause succeeded.
* ERROR is returned if the driver is not initialized, or the channel
* is invalid, or the channel was already paused.
*/

STATUS sysDmaPause
    (
    UINT32 chan		/* Channel to be paused */
    )
    {
    return (DMA_PAUSE (chan));
    }

/******************************************************************************
*
* sysDmaResume - Resume a previously paused DMA operation.
*
* For a given valid channel, verify that the channel is not active and that
* there are bytes remaining to transfer (a non-zero byte count). If so
* activate the channel to continue with the transfer.
*
* RETURNS: 
* OK is returned if the resume succeeded.
* ERROR is returned if the driver is not initialized, or the channel
* is invalid, or the channel is not paused (paused meaning inactive with a 
* a non-zero byte count descriptor register).
*/

STATUS sysDmaResume
    (
    UINT32 chan          /* Channel to be resumed */
    )
    {
    return (DMA_RESUME (chan));
    }





#define DEBUG

/******************************************************************************
*
* user functions
*
* for example: usrMem2MemDmaCopy(0,0x01000000,0x02000000,0x100)
*              usrMem2MemDmaCopy(0,0x11000000,0x01000000,0x100)
*/


LOCAL void
setMem2Mem(UINT32 chan)
{
  UINT32 tmp;
  tmp = *(unsigned int *)(0xf1000840+chan*4);
  *(unsigned int *)(0xf1000840+chan*4) = (tmp & 0xffff1ffe) + 0x0000a000;
  return;
}

LOCAL void
setMem2Pci(UINT32 chan)
{
  UINT32 tmp;
  tmp = *(unsigned int *)(0xf1000840+chan*4);
  *(unsigned int *)(0xf1000840+chan*4) = (tmp & 0xffff1ffe) + 0x00002001;
  return;
}

LOCAL void
setPci2Mem(UINT32 chan)
{
  UINT32 tmp;
  tmp = *(unsigned int *)(0xf1000840+chan*4);
  *(unsigned int *)(0xf1000840+chan*4) = (tmp & 0xffff1ffe) + 0x0000c000;
  return;
}

LOCAL void
setPci2Pci(UINT32 chan)
{
  UINT32 tmp;
  tmp = *(unsigned int *)(0xf1000840+chan*4);
  *(unsigned int *)(0xf1000840+chan*4) = (tmp & 0xffff1ffe) + 0x00004001;
  return;
}

STATUS
usrMem2MemDmaCopy(UINT32 chan, UINT32 *sourceAddr, UINT32 *destAddr,
                  UINT32 nbytes)
{
  DMA_DESCRIPTOR dmaDesc;     /* Byte count, Source, Destination, Next Desc */
  DMA_ATTRIBUTES dmaAttrib;   /* Configuration options */
  IDMA_STATUS     dmaStatus;   /* DMA status output */

  setMem2Mem(chan);

  dmaDesc.byteCount = nbytes;      /* Number of bytes to transfer */
  dmaDesc.sourceAddr = (UINT32)sourceAddr; /* Starting address of source data */
  dmaDesc.destAddr = (UINT32)destAddr;     /* Starting address of destination */

  /* 1: do not increment the source/destination address             */
  /* Sergey: if both=1, DRAM->DRAM copies only one burst (32 bytes) */
  /*         if src=0, dest=1 - DRAM->DRAM copies whole source,     */
  /*                            but to the first 32 bytes of dest   */
  /*         if src=1, dest=0 - DRAM->DRAM copies only first burst, */
  /*                            but fills up whole destination      */
  /*         if src=0, dest=0 - DRAM->DRAM works fine               */
  dmaAttrib.holdSourceAddr = 0;
  dmaAttrib.holdDestAddr = 0;

  /* User defined interrupt completion routine */
  dmaAttrib.userHandler = (HANDLER)WAIT_FOREVER;
#ifdef DEBUG
  printf("dma params: from: 0x%08x, to: 0x%08x, nbytes: %d\n",
    dmaDesc.sourceAddr, dmaDesc.destAddr, dmaDesc.byteCount);
#endif
  sysDmaStart(chan, &dmaDesc, &dmaAttrib, &dmaStatus);
#ifdef DEBUG
  printf("============ dmaStatus ============\n");
  printf("OK when DMA completes successfully: 0x%08x\n",dmaStatus.dmaStatus);
  printf("Error code when dmaStatus is ERROR: 0x%08x\n",dmaStatus.dmaErrorCode);
  printf("Address causing the error code: 0x%08x\n",dmaStatus.dmaErrorAddr);
  printf("Current address of source data: 0x%08x\n",dmaStatus.curSourceAddr);
  printf("Current address of destination: 0x%08x\n",dmaStatus.curDestinAddr);
  printf("Address of next descriptor: 0x%08x\n",dmaStatus.curNextDesc);
  printf("Current channel control settings: 0x%08x\n",dmaStatus.chanCntlLow);
#endif
  return(dmaStatus.dmaStatus);
}

STATUS
usrMem2MemDmaStart(UINT32 chan, UINT32 *sourceAddr, UINT32 *destAddr,
                   UINT32 nbytes)
{
  DMA_DESCRIPTOR dmaDesc;     /* Byte count, Source, Destination, Next Desc */
  DMA_ATTRIBUTES dmaAttrib;   /* Configuration options */
  IDMA_STATUS     dmaStatus;   /* DMA status output */

  setMem2Mem(chan);

  dmaDesc.byteCount = nbytes;      /* Number of bytes to transfer */
  dmaDesc.sourceAddr = (UINT32)sourceAddr; /* Starting address of source data */
  dmaDesc.destAddr = (UINT32)destAddr;     /* Starting address of destination */

  dmaAttrib.holdSourceAddr = 0; /* increment the source address */
  dmaAttrib.holdDestAddr = 0;   /* increment the destination address */

  /* User defined interrupt completion routine */
  dmaAttrib.userHandler = (HANDLER)NULL;
  return(sysDmaStart(chan, &dmaDesc, &dmaAttrib, &dmaStatus));
}


STATUS
usrPci2MemDmaStart(UINT32 chan, UINT32 *sourceAddr, UINT32 *destAddr,
                   UINT32 nbytes)
{
  DMA_DESCRIPTOR dmaDesc;     /* Byte count, Source, Destination, Next Desc */
  DMA_ATTRIBUTES dmaAttrib;   /* Configuration options */
  IDMA_STATUS     dmaStatus;   /* DMA status output */

  setPci2Mem(chan);

  dmaDesc.byteCount = nbytes;      /* Number of bytes to transfer */
  dmaDesc.sourceAddr = (UINT32)sourceAddr; /* Starting address of source data */
  dmaDesc.destAddr = (UINT32)destAddr;     /* Starting address of destination */

  dmaAttrib.holdSourceAddr = 0; /* increment the source address */
  dmaAttrib.holdDestAddr = 0;   /* increment the destination address */

  /* User defined interrupt completion routine */
  
  dmaAttrib.userHandler = (HANDLER)NULL/*WAIT_FOREVER*/;
  return(sysDmaStart(chan, &dmaDesc, &dmaAttrib, &dmaStatus));
}

STATUS
usrMem2PciDmaStart(UINT32 chan, UINT32 *sourceAddr, UINT32 *destAddr,
                   UINT32 nbytes)
{
  DMA_DESCRIPTOR dmaDesc;     /* Byte count, Source, Destination, Next Desc */
  DMA_ATTRIBUTES dmaAttrib;   /* Configuration options */
  IDMA_STATUS     dmaStatus;   /* DMA status output */

  setMem2Pci(chan);

  dmaDesc.byteCount = nbytes;      /* Number of bytes to transfer */
  dmaDesc.sourceAddr = (UINT32)sourceAddr; /* Starting address of source data */
  dmaDesc.destAddr = (UINT32)destAddr;     /* Starting address of destination */

  dmaAttrib.holdSourceAddr = 0; /* increment the source address */
  dmaAttrib.holdDestAddr = 0;   /* increment the destination address */

  /* User defined interrupt completion routine */
  dmaAttrib.userHandler = (HANDLER)NULL;
  return(sysDmaStart(chan, &dmaDesc, &dmaAttrib, &dmaStatus));
}

STATUS
usrPci2PciDmaStart(UINT32 chan, UINT32 *sourceAddr, UINT32 *destAddr,
                   UINT32 nbytes)
{
  DMA_DESCRIPTOR dmaDesc;     /* Byte count, Source, Destination, Next Desc */
  DMA_ATTRIBUTES dmaAttrib;   /* Configuration options */
  IDMA_STATUS     dmaStatus;   /* DMA status output */

  setPci2Pci(chan);

  dmaDesc.byteCount = nbytes;      /* Number of bytes to transfer */
  dmaDesc.sourceAddr = (UINT32)sourceAddr; /* Starting address of source data */
  dmaDesc.destAddr = (UINT32)destAddr;     /* Starting address of destination */

  dmaAttrib.holdSourceAddr = 0; /* increment the source address */
  dmaAttrib.holdDestAddr = 0;   /* increment the destination address */

  /* User defined interrupt completion routine */
  dmaAttrib.userHandler = (HANDLER)NULL;
  return(sysDmaStart(chan, &dmaDesc, &dmaAttrib, &dmaStatus));
}



/* in control reg: bit 12 (from 0) is enable, bit 14 is active */

STATUS
usrDmaDone(UINT32 chan)
{
  int status, res;
  int dmaStatus[7];

  res = sysDmaStatus(chan, dmaStatus);
  if(res != OK)
  {
    printf("usrDmaDone: ERROR: sysDmaStatus() returns %d\n",res);
    return(res);
  }
  /*printf("dmaStatus[0,1]=0x%08x 0x%08x\n",dmaStatus[0],dmaStatus[1]);*/
  if(dmaStatus[0] == ERROR && dmaStatus[1] == 0) status = 1; /* in progress */
  else                        status = dmaStatus[0];

  return(status);
}


/* temporary: simple syncronization mechanizm */

/* PMC bridge general purpose register access */
/* always swap !!! */

int
usrReadGPR()
{
  UINT32 data, offset;

  offset = 0x24;
  pciConfigInLong(0,0,0,offset,&data);

  /*printf("usrReadGPR: data=0x%08x\n",data);*/

  return(data);
}

int
usrWriteGPR(UINT32 data)
{
  UINT32 offset;

  offset = 0x24;
  pciConfigOutLong(0,0,0,offset,data);
  /*
  printf("usrWriteGPR: data=0x%08x\n",data);
  */
  return(0);
}


