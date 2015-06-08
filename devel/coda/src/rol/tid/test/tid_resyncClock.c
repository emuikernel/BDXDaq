/*
 * File:
 *    tid_resyncClock.c
 *
 * Description:
 *    Resync the 250MHz clock
 *
 *
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "jvme.h"
#include "tidLib.h"

int 
main(int argc, char *argv[]) {

    int stat;

    printf("\nReload FPGA Firmware on the TID\n");
    printf("----------------------------\n");

    vmeOpenDefaultWindows();

    stat = tidInit((21<<19),TID_READOUT_POLL,TID_SOURCE_SOFT,0);
    if(stat<0)
      goto CLOSE;


    tidClockResync();

    sleep(1);

    tidStatus();
 CLOSE:


    vmeCloseDefaultWindows();

    exit(0);
}
