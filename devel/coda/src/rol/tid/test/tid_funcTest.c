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

extern int ctpSetPayloadEnableMask(int enableMask);

int 
main(int argc, char *argv[]) {

/*     int stat; */

    printf("\nJLAB TID Library Tests\n");
    printf("----------------------------\n");

    vmeOpenDefaultWindows();

    /* Set the TIR structure pointer */
    tidInit((21<<19),TID_READOUT_EXT_POLL,0);

    tidSetOutputPort(1,0,0,0);

    getchar();

    tidSetOutputPort(0,1,0,0);

    getchar();

    tidSetOutputPort(0,0,1,0);

    getchar();

    tidSetOutputPort(0,0,0,1);

    getchar();
    tidSetOutputPort(0,0,0,0);

    goto CLOSE;
/*     sdInit(); */
    tidReset();
/*     tidEnableI2CDevHack(); */
/*     tidDisableI2CDevHack(); */
    ctpInit();

/*     int mask =  (1<<0) | (1<<(2)) | (1<<(7)) | (1<<(10));// | (1<<16); */
    int mask =  0xffff;

    printf("mask = 0x%x\n",mask);
    ctpSetPayloadEnableMask(mask);
    


    goto CLOSE;

 CLOSE:

    vmeCloseDefaultWindows();

    exit(0);
}

