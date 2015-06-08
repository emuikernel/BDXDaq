/* pcc2Timer.c - Peripheral Channel Controller 2 (PCC2) timer library */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,20dec96,dat  fixed SPR 2672, resetting devices before calling funcs.
01d,18jun96,wlf  doc: cleanup.
01c,07jul92,ccc  changed genericTimer.h to timerDev.h.
01b,25jun92,ccc  changed include to genericTimer.h.
01a,10jun92,ccc  created by moving routines from sysLib.c of mv167, ANSIfied.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
PCC2 chip with a board-independent interface.  This library handles both
the system clock and the auxiliary clock functions.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.
*/

#include "drv/timer/timerDev.h"

/* Locals */ 

#define TICK_FREQ  160000 

LOCAL int     sysClkTicksPerSecond = 60; 
LOCAL BOOL    sysClkRunning        = FALSE; 
LOCAL FUNCPTR sysClkRoutine        = NULL; 
LOCAL int     sysClkArg            = NULL; 
LOCAL BOOL    sysClkConnected      = FALSE; 

LOCAL int     auxClkTicksPerSecond = 100; 
LOCAL BOOL    sysAuxClkRunning     = FALSE; 
LOCAL FUNCPTR sysAuxClkRoutine     = NULL; 
LOCAL int     sysAuxClkArg         = NULL;
LOCAL BOOL    auxClkConnected      = FALSE; 

/********************************************************************************
* sysClkInt - handle system clock interrupts
*
* This routine handles system clock interrupts.
*/

LOCAL void sysClkInt (void)

    {
    /* reset clock interrupt */
    *PCC2_T1_IRQ_CR |=  T1_IRQ_CR_ICLR;

    if (sysClkRoutine != NULL)
        (*sysClkRoutine) (sysClkArg);
    }

/*******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  Normally, it is called from usrRoot() in usrConfig.c to
* connect usrClock() to the system clock interrupt.
*
* RETURN: OK, always.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/
 
STATUS sysClkConnect
    (
    FUNCPTR routine,    /* routine called at each system clock interrupt */
    int arg             /* argument with which to call routine           */
    )
    {
    sysHwInit2 ();	/* XXX for now -- needs to be in usrConfig.c */
 
    sysClkConnected = TRUE;
 
    sysClkRoutine   = routine;
    sysClkArg       = arg;
 
    return (OK);
    }

/********************************************************************************
* sysClkDisable - turn off system clock interrupts
*
* This routine disables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkEnable()
*/
 
void sysClkDisable (void)
 
    {
    if (sysClkRunning)
        {
        /* disable interrupts */
 
        *PCC2_T1_IRQ_CR = 0;
        *PCC2_TIMER1_CR = 0;    /* and disable counter */
 
        sysClkRunning = FALSE;
        }
    }
/********************************************************************************    
* sysClkEnable - turn on system clock interrupts
*
* This routine enables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkConnect(), sysClkDisable(), sysClkRateSet()
*/
 
void sysClkEnable (void)
 
    {
    if (!sysClkRunning)
        {
        /* load compare register with the number of micro seconds */

        *PCC2_TIMER1_CMP = 1000000 / sysClkTicksPerSecond;

        /* enable the clock interrupt. */

        *PCC2_TIMER1_CNT = 0;           /* clear counter */

        /* enable interrupt, clear any pending, and set IRQ level */

        *PCC2_T1_IRQ_CR = T1_IRQ_CR_IEN         |
                          T1_IRQ_CR_ICLR        |
                          SYS_CLK_LEVEL;

        /* now enable timer */

        *PCC2_TIMER1_CR = TIMER1_CR_CEN         |
                          TIMER1_CR_COC;
     
        sysClkRunning = TRUE;
        }
    }
/*******************************************************************************
*
* sysClkRateGet - get the system clock rate
*
* This routine returns the system clock rate.
*
* RETURNS: The number of ticks per second of the system clock.
*
* SEE ALSO: sysClkEnable(), sysClkRateSet()
*/
 
int sysClkRateGet (void)
 
    {
    return (sysClkTicksPerSecond);
    }
/********************************************************************************
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.
* It is called by usrRoot() in usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be
* set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/
 
STATUS sysClkRateSet
    (
    int ticksPerSecond     /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < SYS_CLK_RATE_MIN || ticksPerSecond > SYS_CLK_RATE_MAX)
        return (ERROR);
 
    sysClkTicksPerSecond = ticksPerSecond;
 
    if (sysClkRunning)
        {
        sysClkDisable ();
        sysClkEnable ();
        }
 
    return (OK);
    }
/********************************************************************************
* sysAuxClkInt - handle auxiliary clock interrupts
*/

LOCAL void sysAuxClkInt (void)

    {
    /* reset clock interrupt */
    *PCC2_T2_IRQ_CR |=  T2_IRQ_CR_ICLR;

    if (sysAuxClkRoutine != NULL)
        (*sysAuxClkRoutine) (sysAuxClkArg);
    }
/*******************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.  It does not enable auxiliary clock interrupts.
*
* RETURNS: OK, always.
*
* SEE ALSO: intConnect(), sysAuxClkEnable()
*/
 
STATUS sysAuxClkConnect
    (
    FUNCPTR routine,    /* routine called at each aux clock interrupt    */
    int arg             /* argument to auxiliary clock interrupt routine */
    )
    {
    auxClkConnected  = TRUE;
 
    sysAuxClkRoutine = routine;
    sysAuxClkArg     = arg;
 
    return (OK);
    }

/********************************************************************************
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
    if (sysAuxClkRunning)
        {
        /* disable interrupts */
 
        *PCC2_T2_IRQ_CR = 0;
        *PCC2_TIMER2_CR = 0;
 
        sysAuxClkRunning = FALSE;
        }
    }
/********************************************************************************
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
    if (!sysAuxClkRunning)
        {
        /* load compare register with the number of micro seconds per tick */
 
        *PCC2_TIMER2_CMP = 1000000 / auxClkTicksPerSecond;
 
        /* enable the clock interrupt */
 
        *PCC2_TIMER2_CNT = 0;   /* clear counter */
 
        /* enable interrupt, clear any pending, and set IRQ level */
 
        *PCC2_T2_IRQ_CR = T2_IRQ_CR_IEN         |
                          T2_IRQ_CR_ICLR        |
                          AUX_CLK_LEVEL;
 
        /* now enable timer */
 
        *PCC2_TIMER2_CR = TIMER2_CR_CEN         |
                          TIMER2_CR_COC;
 
        sysAuxClkRunning = TRUE;
        }
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
    return (auxClkTicksPerSecond);
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
    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
        return (ERROR);
 
    auxClkTicksPerSecond = ticksPerSecond;
 
    if (sysAuxClkRunning)
        {
        sysAuxClkDisable ();
        sysAuxClkEnable ();
        }
 
    return (OK);
    } 
