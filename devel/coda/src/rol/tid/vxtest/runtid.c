// runtid.c
//  Program to test tidLib
//
//  -- Bryan Moffit - Nov 2010.
//

#include <stdioLib.h>
#include <usrLib.h>
#include <stdlib.h>
#include <taskLib.h>
#include <semLib.h>
#include <intLib.h>
#include <cacheLib.h>
#include <iv.h>
#include <math.h>
#include "../tidLib.h"

extern volatile struct TID_A24RegStruct *TIDp; /* pointer to TID memory map */
extern volatile        unsigned int     *TIDpd; /* pointer to TID data FIFO */
extern unsigned int tidRead(volatile unsigned int *addr);
extern void tidWrite(volatile unsigned int *addr, unsigned int val);


int
runTID()
{
  unsigned int data[150];
  int dCnt=0;
  int idata=0;

  printf("\nTID Library tests\n");
  sysVmeDmaInit(1);
  sysVmeDmaSet (4,1);      /* Set for 64bit PCI transfers */
  sysVmeDmaSet (11,2);     /* Access to A32 VME Slave module */
  sysVmeDmaSet (12,4);     /* Setup for BLK32 VME Bus transfers */



    /* Set the TIR structure pointer */
    tidInit(20,0,0,0);
    tidReset();
    tidDisableA32();
/*     printf("adr32: 0x%08x\n",tidRead(&TIDp->adr32)); */
    tidSetupA32(0x09000000,0,0);
    tidClearDataBuffer();
/*     printf("adr32: 0x%08x\n",tidRead(&TIDp->adr32)); */
/*     printf(" data: 0x%08x\n",tidRead(&TIDpd[0])); */
/*     tidDisableA32(); */

    tidSoftTrig(10,100);
    sleep(1);

    dCnt = tidReadBlock(&data,6*10,1);
    
    if(dCnt<=0)
      {
	printf("ERROR: dCnt = %d",dCnt);
      }
    else
      {
	for(idata=0;idata<dCnt;idata++)
	  {
	    if((idata%5)==0) printf("\n\t");
	    printf("  0x%.8x ",data[idata]);
	  }
	printf("\n\n");
      }

}
