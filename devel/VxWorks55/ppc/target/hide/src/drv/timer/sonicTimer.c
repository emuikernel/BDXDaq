/* sonicTimer.c - National Semiconductor DP83932BVF (SONIC) timer library */

/* Copyright 1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,29Apr97,sal  Change INCLUDE_SN to INCLUDE_NETWORK
01a,03mar97,kkk  created.
*/

/*
DESCRIPTION
This library provides auxiliary clock timer routines, which are based 
on the the SONIC ethernet driver. Thus, the auxiliary clock is enabled 
only if the SONIC ethernet driver is enabled.

The macros AUX_CLK_RATE_MIN and AUX_CLK_RATE_MAX must be defined to 
provide parameter checking for the sysAuxClkRateSet() routines.

SEE ALSO:
.pG "Configuration"
*/

/* extern declarations */

#ifdef INCLUDE_NETWORK			/* enable aux. clock only if SONIC */
IMPORT void     snClkDisable();		/* is enabled 			   */
IMPORT void     snClkEnable();
#endif /* INCLUDE_NETWORK */

/* Locals */

LOCAL int   sysAuxClkTicksPerSecond = 100;      /* default aux timer rate     */
LOCAL int   sysAuxClkArg            = NULL;     /* aux clock int routine arg  */
LOCAL BOOL  sysAuxClkConnected      = FALSE;    /* sys aux clock connect flag */
LOCAL BOOL  sysAuxClkRunning        = FALSE;    /* sys aux clock enabled flag */
LOCAL FUNCPTR   sysAuxClkRoutine    = NULL;     /* aux clock interpt routine  */

/*******************************************************************************
*
* sysAuxClkInt - interrupt level processing for auxiliary clock
*
* This routine handles the auxiliary clock interrupt.  It is attached to the
* clock interrupt vector by the routine sysAuxClkConnect().
* The appropriate routine is called and the interrupt is acknowleged.
*
* RETURNS: N/A.
*/

LOCAL void sysAuxClkInt (void)
    {
    if (sysAuxClkRoutine != NULL)
        (*sysAuxClkRoutine) (sysAuxClkArg);     /* call system clock routine */
    }

/*******************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.  It does not enable auxiliary clock
* interrupts.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysAuxClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR routine,    /* routine called at each aux clock interrupt    */
    int arg             /* argument to auxiliary clock interrupt routine */
    )
    {
    if (!sysAuxClkConnected)
        {
        sysAuxClkConnected = TRUE;
        }

    sysAuxClkRoutine = routine;
    sysAuxClkArg = arg;

    return (OK);
    }

/*******************************************************************************
*
* sysAuxClkDisable - turn off auxiliary clock interrupts
*
* This routine disables auxiliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkEnable()
*/

void sysAuxClkDisable (void)
    {
#ifdef  INCLUDE_NETWORK
    /* disable clock in the SONIC */

    snClkDisable (0);
    sysAuxClkRunning = FALSE;
#endif  /* INCLUDE_NETWORK */
    }

/*******************************************************************************
*
* sysAuxClkEnable - turn on auxiliary clock interrupts
*
* This routine enables auxiliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkConnect(), sysAuxClkDisable(), sysAuxClkRateSet()
*/

void sysAuxClkEnable (void)
    {
#ifdef  INCLUDE_NETWORK
    /* enable clock in the SONIC */

    snClkEnable (0, sysAuxClkTicksPerSecond, sysAuxClkInt);
    sysAuxClkRunning = TRUE;
#endif  /* INCLUDE_NETWORK */
    }

/*******************************************************************************
*
* sysAuxClkRateGet - get the auxiliary clock rate
*
* This routine returns the interrupt rate of the auxiliary clock.
*
* RETURNS: The number of ticks per second of the auxiliary clock.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateSet()
*/

int sysAuxClkRateGet (void)
    {
    return (sysAuxClkTicksPerSecond);
    }

/*******************************************************************************
*
* sysAuxClkRateSet - set the auxiliary clock rate
*
* This routine sets the interrupt rate of the auxiliary clock.
* It does not enable auxiliary clock interrupts.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/

STATUS sysAuxClkRateSet
    (
    int ticksPerSecond     /* number of clock interrupts per second */
    )
    {
#ifdef  INCLUDE_NETWORK
    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
        {
        return (ERROR);
        }

    sysAuxClkTicksPerSecond = ticksPerSecond;

    if (sysAuxClkRunning)
        {
        sysAuxClkDisable ();
        sysAuxClkEnable ();
        }

    return (OK);
#else
    return (ERROR);
#endif  /* INCLUDE_NETWORK */
    }
