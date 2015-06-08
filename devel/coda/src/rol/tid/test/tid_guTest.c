/*
 * File:
 *    tid_guTest.c
 *
 * Description:
 *    Test Vme TID interrupts with GEFANUC Linux Driver
 *    and TID library
 *
 *
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "jvme.h"
#include "tidLib.h"

/* access tirLib global variables */
extern unsigned int tidIntCount;
extern unsigned int tidDaqCount;
DMA_MEM_ID vmeIN,vmeOUT;

extern void tidStartPollingThread(void);

/* Interrupt Service routine */
void
mytidISR(int arg)
{
  volatile unsigned short reg;
  int dCnt, len=0,idata;
  DMANODE *outEvent;

  GETEVENT(vmeIN,tidIntCount);
/*   *dma_dabufp++ = LSWAP(0xdecea5ed); */

  dCnt = tidReadBlock(dma_dabufp,900>>2,1);
  if(dCnt<=0)
    {
      printf("No data or error.  dCnt = %d\n",dCnt);
    }
  else
    {
      dma_dabufp += dCnt;
      printf("dCnt = %d\n",dCnt);
    
    }
  PUTEVENT(vmeOUT);
  
  outEvent = dmaPGetItem(vmeOUT);
/*   if(tidIntCount%100==0) */
    {
      printf("Received %d triggers... tidDaqCount = %d\n",
	     tidIntCount,tidDaqCount);

      len = outEvent->length;
      
      for(idata=0;idata<len;idata++)
	{
	  if((idata%5)==0) printf("\n\t");
	  printf("  0x%.8x ",LSWAP(outEvent->data[idata]));
	}
      printf("\n\n");
    }
  dmaPFreeItem(outEvent);
}


int 
main(int argc, char *argv[]) {

    int stat;

    printf("\nJLAB TID Tests\n");
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

    /* INIT dmaPList */
    dmaPFreeAll();
    vmeIN  = dmaPCreate("vmeIN",1024,100,0);
    vmeOUT = dmaPCreate("vmeOUT",0,0,0);
    
    dmaPStatsAll();

    dmaPReInitAll();


/*     gefVmeSetDebugFlags(vmeHdl,0x0); */
    /* Set the TID structure pointer */
    tidInit(21,TID_READOUT_POLL,TID_SOURCE_SOFT,0);
/*     FiberMeas(); */

    stat = tidIntConnect(TID_INT_VEC, mytidISR, 0);
    if (stat != OK) {
      printf("ERROR: tidIntConnect failed \n");
      goto CLOSE;
    } else {
      printf("INFO: Attached TID Interrupt\n");
    }

    tidDisableA32();
    /*     printf("adr32: 0x%08x\n",tidRead(&TIDp->adr32)); */
    tidSetupA32(0x09000000,0,0);
#ifdef MYSTUFF
    tidSetEventFormat(2);
    tidSetBlockLevel(25);
    tidClearDataBuffer();
    /*     printf("adr32: 0x%08x\n",tidRead(&TIDp->adr32)); */
   /*     printf(" data: 0x%08x\n",tidRead(&TIDpd[0])); */
    /*     tidDisableA32(); */
    tidSetupFiberLoopback();

/*     tidSetTriggerSource(4); */
#endif
    TrgDAQSet(0x19,0x5a);
    TrgLinkReset();
    TrgSrcEnable(0x12);

    tidStatus();

/*     tidStartPollingThread(); */
    tidIntEnable(0);
    printf("Hit enter to start triggers\n");
    getchar();

    tidSoftTrig(0xffff,0x8888);
#ifdef OTHERWAY
    int inputchar=10;
    while(1 && (inputchar==10))
      {
	printf("Enter 1 to quit\n");
	inputchar = getchar();
	mytidISR(0);
      }
#endif
    /* Enable the TIR and clear the trigger counter */
/*     tidSetIntCount();     */

    printf("Hit any key to Disable TID and exit.\n");
    getchar();

    tidSoftTrig(0x0,0x8888);

    tidIntDisable();

    tidIntDisconnect();

 CLOSE:

#ifdef MYSTUFF
    tidClearDataBuffer();
#endif
/*     tidIntStatus(1); */
/*     tidReload(); */

    vmeCloseDefaultWindows();

    exit(0);
}

