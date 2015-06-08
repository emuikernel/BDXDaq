/*
 * File:
 *    tid_i2cTest.c 
 *
 * Description:
 *    Test the Vme TI library with jvme API
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "jvme.h"
#include "tidLib.h"
#include "sdLib.h"
#include "ctpLib.h"

unsigned int blockCount=0;
extern volatile struct TID_A24RegStruct *TIDp; /* pointer to TID memory map */
extern volatile        unsigned int     *TIDpd; /* pointer to TID data FIFO */
extern unsigned int tidRead(volatile unsigned int *addr);
extern void tidWrite(volatile unsigned int *addr, unsigned int val);

int 
main(int argc, char *argv[]) {

/*     int stat; */

    printf("\nJLAB TID Library Tests\n");
    printf("----------------------------\n");

    vmeOpenDefaultWindows();

    /* Set the TIR structure pointer */
    tidInit(21<<19,0,0);
    sdInit();
/*     sdSetActivePayloadSlots(0x1234); */
    sdStatus();

    goto CLOSE;
/*     tidReset(); */
/*     tidEnableI2CDevHack(); */
/*     tidDisableI2CDevHack(); */

    /* calculate the difference in ctp.base and dataSetup to check offset */
/*     unsigned int ctpbase = (unsigned int) &(TIDp->ctp.base[ctpreg>>2]); */

/* #define SD */
#ifdef CTP
    unsigned int ctpbase = (unsigned int) &(TIDp->ctp.fpga1.status1);

    unsigned int resetv2j = (unsigned int) &(TIDp->resetVMEtoJTAG);
    unsigned int jtagfpgabase = (unsigned int) &(TIDp->JTAGFPGABase[0]);

    unsigned int tidbase = (unsigned int) &(TIDp->dataSetup);

    printf("ctpbase offset = 0x%08x\n",ctpbase-tidbase);
    printf("resetv2j offset = 0x%08x\n",resetv2j-tidbase);
    printf("jtagfpgabase offset = 0x%08x\n",jtagfpgabase-tidbase);

/*     unsigned int chip1_read = (tidRead(&TIDp->ctp.base[ctpreg>>2])); */
    tidWrite(&TIDp->ctp.fpga1.config0,0x0);
    printf("chip1_read =%c[0;31;40m 0x%08x %c[0;37;40m\n",0x1b,
	   (tidRead(&TIDp->ctp.fpga1.config0)),0x1b);
    tidWrite(&TIDp->ctp.fpga1.config0,0x4321);

    printf("chip1_read =%c[0;31;40m 0x%08x %c[0;37;40m\n",0x1b,
	   (tidRead(&TIDp->ctp.fpga1.config0)),0x1b);

    printf("Chip 1 status0: 0x%08x   status1: 0x%08x\n",
	   tidRead(&TIDp->ctp.fpga1.status0), tidRead(&TIDp->ctp.fpga1.status1));
    printf("Chip 2 status0: 0x%08x   status1: 0x%08x\n",
	   tidRead(&TIDp->ctp.fpga2.status0), tidRead(&TIDp->ctp.fpga2.status1));
    printf("Chip 3 status0: 0x%08x   status1: 0x%08x\n",
	   tidRead(&TIDp->ctp.fpga3.status0), tidRead(&TIDp->ctp.fpga3.status1));

    printf("user_code = 0x%08x\n",tidReadFPGAUserCode());

    ctpInit();
    ctpStatus();
#endif
/* #define SD */
#ifdef SD
    unsigned int tidbase = (unsigned int) &(TIDp->dataSetup);
    unsigned int sdbase = (unsigned int) &(TIDp->sd.status1);

    printf("sdbase offset = 0x%08x\n",sdbase-tidbase);

    tidWrite(&TIDp->sd.payloadSlots,0x1234);
    getchar();
/*     printf("status1       = 0x%08x\n",tidRead(&TIDp->sd.status1)); */
    printf("payloadSlots  = 0x%08x\n",tidRead(&TIDp->sd.payloadSlots));
    getchar();

/*     printf("tokenSlots    = 0x%08x\n",tidRead(&TIDp->sd.tokenSlots)); */
    printf("busyoutSlots  = 0x%08x\n",tidRead(&TIDp->sd.busyoutSlots));
/*     printf("trigoutSlots  = 0x%08x\n",tidRead(&TIDp->sd.trigoutSlots)); */
    printf("Next one\n");

    tidWrite(&TIDp->sd.payloadSlots,0x1234);
    printf("payloadSlots  = 0x%08x\n",tidRead(&TIDp->sd.payloadSlots));
/*     tidReadFPGAUserCode(); */
    tidWrite(&TIDp->sd.payloadSlots,0x1234);
    printf("payloadSlots  = 0x%08x\n",tidRead(&TIDp->sd.payloadSlots));
    tidWrite(&TIDp->sd.payloadSlots,0x1234);
    printf("payloadSlots  = 0x%08x\n",tidRead(&TIDp->sd.payloadSlots));

#endif

    goto CLOSE;

 CLOSE:
    dmaPFreeAll();

    vmeCloseDefaultWindows();

    exit(0);
}

