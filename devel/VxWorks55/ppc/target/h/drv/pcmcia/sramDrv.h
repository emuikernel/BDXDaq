/* sramDrv.h - PCMCIA memory driver header file */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,22feb96,hdn  cleaned up.
01a,28feb95,hdn  written
*/

#ifndef __INCsramDrvh
#define __INCsramDrvh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE


#define DEFAULT_DISK_SIZE	512000
#define DEFAULT_SEC_SIZE	512

#include "private/semLibP.h"
#include "blkIo.h"
#include "iosLib.h"

typedef struct sramCtrl	/* PCMCIA memory device controller */
    {
    SEMAPHORE	muteSem[MAX_SOCKETS];	/* mutex  sem for mutual-exclusion */
    SEMAPHORE	syncSem[MAX_SOCKETS];	/* binary sem for syncronization */
    } SRAM_CTRL;

typedef struct sramDev		/* PCMCIA memory block device descriptor */
    {
    BLK_DEV	blkDev;		/* generic block device structure */
    int		sock;		/* socket no. */
    int		blkOffset;	/* block offset of this device from ram_addr */
    } SRAM_DEV;


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS sramDrv		(int sock);
extern BLK_DEV *sramDevCreate	(int sock, int bytesPerBlk, int blksPerTrack,
			 	 int nBlocks, int blkOffset);
extern STATUS sramMap		(int sock, int type, int start, int stop, 
			 	 int offset, int extraws);
extern STATUS sramShow		(int sock);

#else	/* __STDC__ */

extern STATUS sramDrv		();
extern BLK_DEV *sramDevCreate	();
extern STATUS sramMap		();
extern STATUS sramShow		();

#endif	/* __STDC__ */


#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCsramDrvh */
