/*************************************************************************
 *
 *  tdc1190_list.c - Library of routines for the user to write for
 *                   readout and buffering of events from CAEN 1190/1290 
 *                   using a Linux VME controller.
 *
 */

/* Event Buffer definitions */
#define MAX_EVENT_POOL     400
#define MAX_EVENT_LENGTH   1024*10      /* Size in Bytes */

/* Define Interrupt source and address */
#define TIR_SOURCE
#define TIR_ADDR 0x0ed0
#define TIR_MODE TIR_EXT_INT

#include "linuxvme_list.c"
#include "c1190Lib.h"

/* CAEN 1190/1290 specific definitions */
#define NUM_V1190 2

void
rocDownload()
{
  /* Put one-time (boot) module initialization here */

  printf("rocDownload: User Download Executed\n");
}

void
rocPrestart()
{
  unsigned short iflag;
  int itdc, stat;

  /* Setup Address and data modes for DMA transfers
   *   
   *  vmeDmaConfig(addrType, dataType, sstMode);
   *
   *  addrType = 0 (A16)    1 (A24)    2 (A32)
   *  dataType = 0 (D16)    1 (D32)    2 (BLK32) 3 (MBLK) 4 (2eVME) 5 (2eSST)
   *  sstMode  = 0 (SST160) 1 (SST267) 2 (SST320)
   */
  vmeDmaConfig(2,5,2); 

  printf("Done configuring dma\n");

  /* Program/Init VME Modules Here */
  /* INIT C1190/C1290 - Must be A32 for 2eSST */
  UINT32 list[NUM_V1190] = {0x08270000,0x08280000};
  
  tdc1190InitList(list,NUM_V1190,1);
  /* Another way to do the same thing */
  /*   tdc1190Init(list[0],0x10000,NUM_V1190,1); */
  for(itdc=0; itdc<NUM_V1190; itdc++) 
    {
      tdc1190SetTriggerMatchingMode(itdc);
      tdc1190SetEdgeResolution(itdc,100);
      tdc1190EventFifo(itdc,1);
      tdc1190BusError(itdc,1);
      tdc1190Align64(itdc,1);
    }
  /* Disable some noisy channels */
/*   tdc1190DisableChannel(1,12); */
/*   tdc1190DisableChannel(1,13); */

  for(itdc=0; itdc<NUM_V1190; itdc++) 
    tdc1190Status(itdc);

  printf("rocPrestart: User Prestart Executed\n");

}

void
rocGo()
{
  /* Enable modules, if needed, here */

  /* Interrupts/Polling enabled after conclusion of rocGo() */
}

void
rocEnd()
{
  int status, count;
  int itdc;

  /* 1190/1290 Status  - FIXME: Reset too? */
  for(itdc=0; itdc<NUM_V1190; itdc++) 
    tdc1190Status(itdc);

  printf("rocEnd: Ended after %d events\n",tirGetIntCount());
  
}

void
rocTrigger(int arg)
{
  int ii, status, dma, count;
  int nwords;
  unsigned int datascan;

  tirIntOutput(2);

  *dma_dabufp++ = LSWAP(tirGetIntCount()); /* Insert Event Number */

  /* Check for valid data here */
  for(ii=0;ii<100;ii++) 
    {
      datascan = tdc1190Dready(0);
      if (datascan>0) 
	{
	  break;
	}
    }

  if(datascan>0) 
    {
      /* Get the TDC data from all modules... rflag=2 for Linked List DMA 
	 "64" is ignored in Linux */
      nwords = tdc1190ReadBlock(0,dma_dabufp,64,2);
    
      if(nwords < 0) 
	{
	  printf("ERROR: in transfer (event = %d), status = 0x%x\n", tirGetIntCount(),nwords);
	  *dma_dabufp++ = LSWAP(0xda000bad);
	} 
      else 
	{
	  dma_dabufp += nwords;
	}
    } 
  else 
    {
      printf("ERROR: Data not ready in event %d\n",tirGetIntCount());
      *dma_dabufp++ = LSWAP(0xda000bad);
    }

  *dma_dabufp++ = LSWAP(0xd00dd00d); /* Event EOB */

  tirIntOutput(0);
}
