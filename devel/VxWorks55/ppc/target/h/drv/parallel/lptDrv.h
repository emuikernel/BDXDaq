/* lptDrv.h - IBM-PC LPT */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,23mar01,sru  add register spacing field to LPT_RESOURCE SPR# 65937
01f,23aug00,jgn  move _ASMLANGUAGE guard to prevent header file problems with
                 assembler builds
01e,04jan00,dat  SPR 29875, chg'd SEMAPHOREs to SEM_IDs
01d,22nov99,stv  added missing header files (SPR #24014).
01c,10dec97,ms   added lptResources import
01b,18oct96,hdn	 re-written.
01a,13oct94,hdn	 written.
*/

#ifndef	__INClptDrvh
#define	__INClptDrvh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

/* includes */

#include "iosLib.h"
#include "semLib.h"

typedef struct lptDev		/* LPT_DEV */
    {
    DEV_HDR	devHdr;
    BOOL	created;	/* TRUE if this device has been created */
    BOOL	autofeed;	/* TRUE if enable autofeed */
    BOOL	inservice;	/* TRUE if interrupt in service */
    ULONG	data;		/* data register */
    ULONG	stat;		/* status register */
    ULONG	ctrl;		/* control register */
    int 	intCnt;		/* interrupt count */
    int 	retryCnt;	/* retry count */
    int		busyWait;	/* loop count for BUSY wait */
    int 	strobeWait;	/* loop count for STROBE wait */
    int 	timeout;	/* timeout second for syncSem */
    int 	intLevel;	/* interrupt level */
    SEM_ID	muteSem;	/* mutex semaphore */
    SEM_ID	syncSem;	/* sync semaphore */
    } LPT_DEV;

typedef struct lptResource	/* LPT_RESOURCE */
    {
    int		ioBase;		/* IO base address */
    int		intVector;	/* interrupt vector */
    int		intLevel;	/* interrupt level */
    BOOL	autofeed;       /* TRUE if enable autofeed */
    int		busyWait;       /* loop count for BUSY wait */
    int		strobeWait;     /* loop count for STROBE wait */
    int		retryCnt;       /* retry count */
    int		timeout;        /* timeout second for syncSem */
    int		regDelta;	/* register address spacing */
    } LPT_RESOURCE;

/* IO address (LPT) */

#define N_LPT_CHANNELS	3	/* max LPT channels */

/* register definitions */

#define CAST
#define LPT_REG_ADDR_INTERVAL   1       /* address diff of adjacent regs. */
#define LPT_ADRS(base,reg)   (CAST (base+(reg*LPT_REG_ADDR_INTERVAL)))

/* register spacing based on header file definition */

#define LPT_DATA(base)	LPT_ADRS(base,0x00)	/* data reg. */
#define LPT_STAT(base)	LPT_ADRS(base,0x01)	/* status reg. */
#define LPT_CTRL(base)	LPT_ADRS(base,0x02)	/* control reg. */

/* register spacing based on resource structure */

#define LPT_DATA_RES(resource)  (resource->ioBase + (0 * resource->regDelta))
#define LPT_STAT_RES(resource)  (resource->ioBase + (1 * resource->regDelta))
#define LPT_CTRL_RES(resource)  (resource->ioBase + (2 * resource->regDelta))

/* control register */

#define C_STROBE	0x01	/* STROBE signal */
#define C_AUTOFEED	0x02	/* AUTOFEED signal */
#define C_NOINIT	0x04	/* INITIALIZE signal */
#define C_SELECT	0x08	/* SELECT INPUT signal */
#define C_ENABLE	0x10	/* ENABLE interrupt */
#define C_INPUT		0x20	/* DATA DIRECTION control bit */

/* status register */

#define S_NODERR	0x08	/* Device error */
#define S_SELECT	0x10	/* Device selected */
#define S_PERR		0x20	/* Device out of paper */
#define S_NOACK		0x40	/* Printer ready */
#define S_NOBUSY	0x80	/* Device busy */
#define S_MASK		0xb8	/* mask bits */

/* ioctl functions */

#define LPT_SETCONTROL	0
#define LPT_GETSTATUS	1

IMPORT LPT_RESOURCE lptResources[];

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

STATUS	lptDrv (int channels, LPT_RESOURCE *pResource);
STATUS	lptDevCreate (char *name, int channel);

#else

STATUS	lptDrv ();
STATUS	lptDevCreate ();

#endif  /* __STDC__ */


#endif	/* _ASMLANGUAGE */


#ifdef __cplusplus
}
#endif

#endif	/* __INClptDrvh */
