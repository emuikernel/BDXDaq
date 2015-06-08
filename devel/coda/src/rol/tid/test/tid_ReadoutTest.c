/*
 * File:
 *    tid_readoutTest.c 
 *
 * Description:
 *    Test the Vme TID library with jvme API
 *
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "jvme.h"
#include "tidLib.h"

unsigned int blockCount=0;
extern volatile struct TID_A24RegStruct *TIDp; /* pointer to TID memory map */
extern volatile        unsigned int     *TIDpd; /* pointer to TID data FIFO */
extern unsigned int tidRead(volatile unsigned int *addr);
extern void tidWrite(volatile unsigned int *addr, unsigned int val);
extern unsigned int tidDaqCount;

int 
main(int argc, char *argv[]) {

    int stat;
    DMA_MEM_ID vmeIN,vmeOUT;
    DMANODE *outEvent;

    printf("\nJLAB TID Library Tests\n");
    printf("----------------------------\n");

    vmeOpenDefaultWindows();

  /* Setup Address and data modes for DMA transfers
   *   
   *  vmeDmaConfig(addrType, dataType, sstMode);
   *
   *  addrType = 0 (A16)    1 (A24)    2 (A32)
   *  dataType = 0 (D16)    1 (D32)    2 (BLK32) 3 (MBLK) 4 (2eVME) 5 (2eSST)
   *  sstMode  = 0 (SST160) 1 (SST267) 2 (SST320)
   */
    vmeDmaConfig(2,5,1);
/*     vmeDmaAllocLLBuffer(); */

    /* INIT dmaPList */
    dmaPFreeAll();
    vmeIN  = dmaPCreate("vmeIN",1024,100,0);
    vmeOUT = dmaPCreate("vmeOUT",0,0,0);
    
    dmaPStatsAll();

    dmaPReInitAll();

    /* Set the TIR structure pointer */
    tidInit(21,0,0,0);
    tidReset();

    tidDisableA32();
/*     printf("adr32: 0x%08x\n",tidRead(&TIDp->adr32)); */
    tidSetupA32(0x09000000,0,0);
    tidSetBlockLevel(5);
    tidDisableBusError();
    tidClearDataBuffer();
    tidEnableBusError();
/*     printf("adr32: 0x%08x\n",tidRead(&TIDp->adr32)); */
/*     printf(" data: 0x%08x\n",tidRead(&TIDpd[0])); */
/*     tidDisableA32(); */

    tidSetupFiberLoopback();
    tidSetTriggerSource(4);
/*     tidSetRandomTrigger(); */
    tidSoftTrig(1,1);
    int step=0;
    sleep(1);
    while(tidIntPoll()==0 && step<10000)
      {
	step++;
      }
    tidDisableRandomTrigger();
    printf("step = %d    tidDaqCount=%d\n ",step,tidDaqCount);
/*     unsigned int data[50]; */
    int idata=0, dCnt;
    int len=0;

    GETEVENT(vmeIN,0);
    *dma_dabufp++ = LSWAP(0xdecea5ed);
  
    dCnt = tidReadBlock(dma_dabufp,6*10+2,1);
    if(dCnt<=0)
      {
	printf("No data or error.  dCnt = %d\n",dCnt);
      }
    else
      {
	dma_dabufp += dCnt;
	PUTEVENT(vmeOUT);

	outEvent = dmaPGetItem(vmeOUT);
	len = outEvent->length;

	for(idata=0;idata<len;idata++)
	  {
	    if((idata%5)==0) printf("\n\t");
	    printf("  0x%.8x ",LSWAP(outEvent->data[idata]));
	  }
	printf("\n\n");
	dmaPFreeItem(outEvent);
      }

    tidClearDataBuffer();
    goto CLOSE;


 CLOSE:
    dmaPFreeAll();

    vmeCloseDefaultWindows();

    exit(0);
}

