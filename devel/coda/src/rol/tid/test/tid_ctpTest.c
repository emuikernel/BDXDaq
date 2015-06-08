/*
 * File:
 *    tid_ctpTest.c 
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

extern int ctpSetPayloadEnableMask(int enableMask);

int 
main(int argc, char *argv[]) {

/*     int stat; */

    printf("\nJLAB TID Library Tests\n");
    printf("----------------------------\n");

    vmeOpenDefaultWindows();

    /* Set the TIR structure pointer */
    tidInit(21<<19,0,0);
    sdInit();
    ctpInit();

    goto CLOSE;
/*     tidReset(); */

    printf(" First Try\n");
    tidWrite(&TIDp->ctp.sum_threshold_lsb, 0x1111);
    printf("READIT CTP 0x%08x\n",tidRead(&TIDp->ctp.sum_threshold_lsb));

    tidWrite(&TIDp->sd.payloadPorts, 0x1234);
    printf("READIT  SD 0x%08x\n",tidRead(&TIDp->sd.payloadPorts));

    printf(" Second try\n");
    tidWrite(&TIDp->ctp.sum_threshold_lsb, 0x2222);
    printf("READIT CTP 0x%08x\n",tidRead(&TIDp->ctp.sum_threshold_lsb));

    tidRead(&TIDp->sd.system);
    tidWrite(&TIDp->sd.payloadPorts, 0x5678);
    printf("READIT  SD 0x%08x\n",tidRead(&TIDp->sd.payloadPorts));

    tidStatus();

    goto CLOSE;

 CLOSE:

    vmeCloseDefaultWindows();

    exit(0);
}

