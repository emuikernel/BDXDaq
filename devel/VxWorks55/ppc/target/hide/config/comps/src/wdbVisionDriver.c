/* wdbVisionDriver.c - Wind River Vision Driver WDB_COMM initialization library */

/* Copyright 1986-2002 Wind River Systems Inc. */

/*
modification history
--------------------
01j,13jul02,tcr  Added support for the 440.
02c,30nov01,g_h  Cleaning for T2.2
02b,12nov01,g_h  Add support for ColdFire
02a,04jun01,g_h  Changed to wdbVisionDriver based version of for the TMD mode
01c,09apr01,rnr  Changed to vDriver based version of for the TMD mode
01b,07feb01,g_h  renaming module name and cleaning
01a,24dec99,est	 adapted from wdbNETROM.c
*/

/*
DESCRIPTION
Initializes the Wind River Vision Tools connection for the WDB agent.

NOMANUAL
*/

/* Include main packet driver source module */

#include "wdb/vision/wdbVisionDrv.c"
#include "drv/wdb/vision/wdbVisionPktDrv.h"
#include "wdb/vision/wdbVisionPktDrv.c"

#if defined (INCLUDE_WDB_COMM_VTMD)
#include "wdb/vision/visionTmdDrv.c"
#if   ((CPU == PPC603) || (CPU == PPC860) || (CPU == PPC604) || (CPU == PPC405) || (CPU == PPC440))
#include "wdb/vision/visionTmdAsmPpc.c"
#elif (CPU == MIPS32)
#include "wdb/vision/visionTmdAsmMips.c"
#elif (CPU == ARMARCH4)
#include "wdb/vision/visionTmdAsmArm.c"
#elif (CPU == XSCALE)
#include "wdb/vision/visionTmdAsmXscale.c"
#elif ((CPU == SH7700) || (CPU == SH7750) || (CPU == SH7600))
#include "wdb/vision/visionTmdAsmSh.c"
#elif (CPU == MCF5200)
#include "wdb/vision/visionTmdAsmCfire.c"
#else
#error "This CPU is not supported !!!"
#endif
#endif /* (INCLUDE_WDB_COMM_VTMD) */

/* Check MTU */

#if     WDB_MTU > WDB_VISION_PKT_MTU
#undef  WDB_MTU
#define WDB_VISION_PKT_MTU
#endif  /* WDB_MTU > WDB_VISION_PKT_MTU */

/***************************************************************************
*
* wdbCommDevInit - initialize the vision connection
*
* This routine initialize the vision connection
*
* RETURN: OK or ERROR
*/

STATUS wdbCommDevInit
    (
    WDB_COMM_IF  * pCommIf,
    char        ** ppWdbInBuf,
    char        ** ppWdbOutBuf
    )
    {
    LOCAL WDB_VISION_PKT_DEV wdbVisionPktDev;  
    LOCAL uint_t             wdbInBuf [WDB_MTU/4];
    LOCAL uint_t             wdbOutBuf [WDB_MTU/4];

    /* update input & output buffer pointers */

    *ppWdbInBuf  = (char *) wdbInBuf;
    *ppWdbOutBuf = (char *) wdbOutBuf;

    /* update communication interface mtu */

    wdbCommMtu = WDB_MTU;

    wdbVisionPktDevInit (&wdbVisionPktDev, udpRcv);

    if (udpCommIfInit (pCommIf, &wdbVisionPktDev.wdbDrvIf) == ERROR)
	return (ERROR);

    return (OK);
    }
