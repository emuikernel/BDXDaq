/*
 * File:
 *    tid_sendSync.c 
 *
 * Description:
 *    Just send a sync... nothing else
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
    tidInit(21<<19,0,0,1);

    printf("Press <Enter> to send the SYNC\n");
    getchar();
    tidSyncReset();
    printf("SENT\n");


    goto CLOSE;

 CLOSE:

    vmeCloseDefaultWindows();

    exit(0);
}

