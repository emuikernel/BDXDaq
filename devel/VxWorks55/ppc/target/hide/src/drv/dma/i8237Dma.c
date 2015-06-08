/* i8237Dma.c - i8237 dma device driver */

/* Copyright 1989-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,14jun95,hdn  removed function declarations defined in sysLib.h
01a,28sep93,hdn  written.
*/

/*
DESCRIPTION

SEE ALSO:
.pG "I/O System"
*/

#include "vxWorks.h"
#include "memLib.h"
#include "stdlib.h"
#include "errnoLib.h"
#include "stdio.h"
#include "sysLib.h"
#include "ioLib.h"
#include "cacheLib.h"
#include "sys/fcntlcom.h"
#include "drv/dma/i8237Dma.h"


/* global */

int dmaDebug = 0;


/* local */

LOCAL int dma1PageReg[] = {0x87, 0x83, 0x81, 0x82};
LOCAL int dma2PageReg[] = {0x8b, 0x89, 0x8a, 0x8f};


/* function prototypes */

int		dmaSetup (int direction, void *pBuf, UINT nBytes, UINT chan);


/* setup DMA controller */

STATUS dmaSetup
    (
    int direction,
    void *pBuf,
    UINT nBytes,
    UINT chan
    )
    {
    char modeReg;

    if ((chan > 7) || (nBytes > (1 << 16)))
	{
	printf ("dmaSetup: impossible request\n");
	(void) errnoSet (S_ioLib_DEVICE_ERROR);
	return (ERROR);
	}
    
    if (direction == O_RDONLY)
	modeReg = DMA_MODE_SINGLE | DMA_MODE_READ  | (chan & 0x03);
    else
	modeReg = DMA_MODE_SINGLE | DMA_MODE_WRITE | (chan & 0x03);

    if (dmaDebug)
	{
	printf ("dir=0x%x pBuf=0x%x nBytes=%d chan=%d mode=0x%x\n",
		direction, (int)pBuf, nBytes, chan, modeReg);
	}

    if ((chan & 4) == 0)
	{
	cacheInvalidate (DATA_CACHE, pBuf, nBytes);
	sysOutByte (DMA1_MASK_SINGLE, DMA_MASK_SET | (chan & 0x03));
	sysDelay ();
	sysOutByte (DMA1_MODE, modeReg);
	sysDelay ();
	sysOutByte (DMA1_CLEAR_FF, 0);
	sysDelay ();
	sysOutByte (DMA1_ADDR(chan), (int)pBuf & 0x000000ff);
	sysDelay ();
	sysOutByte (DMA1_ADDR(chan), ((int)pBuf & 0x0000ff00) >> 8);
	sysDelay ();
	sysOutByte (dma1PageReg[chan & 0x03], ((int)pBuf & 0x00ff0000) >> 16);
	sysDelay ();
	sysOutByte (DMA1_COUNT(chan), --nBytes & 0x000000ff);
	sysDelay ();
	sysOutByte (DMA1_COUNT(chan), (nBytes & 0x0000ff00) >> 8);
	sysDelay ();
	sysOutByte (DMA1_MASK_SINGLE, DMA_MASK_RESET | (chan & 0x03));
	sysDelay ();
	}
    else
	{
	cacheFlush (DATA_CACHE, pBuf, nBytes);
	sysOutByte (DMA2_MASK_SINGLE, DMA_MASK_SET | (chan & 0x03));
	sysDelay ();
	sysOutByte (DMA2_MODE, modeReg);
	sysDelay ();
	sysOutByte (DMA2_CLEAR_FF, 0);
	sysDelay ();
	sysOutByte (DMA2_ADDR(chan), ((int)pBuf & 0x000001fe) >> 1);
	sysDelay ();
	sysOutByte (DMA2_ADDR(chan), ((int)pBuf & 0x0001fe00) >> 9);
	sysDelay ();
	sysOutByte (dma2PageReg[chan & 0x03], ((int)pBuf & 0x00ff0000) >> 16);
	sysDelay ();
	nBytes <<= 1;
	sysOutByte (DMA2_COUNT(chan), --nBytes & 0x000000ff);
	sysDelay ();
	sysOutByte (DMA2_COUNT(chan), (nBytes & 0x0000ff00) >> 8);
	sysDelay ();
	sysOutByte (DMA2_MASK_SINGLE, DMA_MASK_RESET | (chan & 0x03));
	sysDelay ();
	}

    return (OK);
    }

