/*************************************************************************
 *
 *  vme_list.c - Library of routines for readout and buffering of 
 *                events using a JLAB Trigger Interface and
 *                Distribution Module (TID) with a Linux VME controller.
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

/* function prototype */
void rocTrigger(int arg);

void
rocDownload()
{

  /* Setup Address and data modes for DMA transfers
   *   
   *  vmeDmaConfig(addrType, dataType, sstMode);
   *
   *  addrType = 0 (A16)    1 (A24)    2 (A32)
   *  dataType = 0 (D16)    1 (D32)    2 (BLK32) 3 (MBLK) 4 (2eVME) 5 (2eSST)
   *  sstMode  = 0 (SST160) 1 (SST267) 2 (SST320)
   */
  vmeDmaConfig(2,5,1); 

  /* TID Setup */
  /* Set the trigger source to the TS Inputs */
  tidSetTriggerSource(TID_TRIGGER_TSINPUTS);

  /* Set number of events per block */
  tidSetBlockLevel(1);

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

  printf("rocDownload: User Download Executed\n");

}

void
rocPrestart()
{
  unsigned short iflag;
  int stat;

  /* Program/Init VME Modules Here */

  printf("rocPrestart: User Prestart Executed\n");
  /* SYNC is issued after this routine is executed */
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

  printf("rocEnd: Ended after %d blocks\n",tidGetIntCount());
  
}

void
rocTrigger(int arg)
{
  int ii;
  int dCnt, len=0, idata;

  /* Set high, the first output port */
  tidSetOutputPort(1,0,0,0);


  /* Grab the data from the TID */
  dCnt = tidReadBlock(dma_dabufp,900>>2,1);
  if(dCnt<=0)
    {
      printf("No data or error.  dCnt = %d\n",dCnt);
    }
  else
    {
      dma_dabufp += dCnt;
    }

  /* Add some addition data here, as necessary 
   * Use LSWAP here to be sure they are added with the correct byte-ordering 
   */
  *dma_dabufp++ = LSWAP(0x1234);

  /* Turn off all output ports */
  tidSetOutputPort(0,0,0,0);

}
