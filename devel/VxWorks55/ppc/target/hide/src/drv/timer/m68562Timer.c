/* m68562Timer.c - Motorola MC68562 DUSCC timer library */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,21jun96,wlf  doc: cleanup.
01b,10jul92,caf  preload timers in sys[Aux]ClkEnable() (SPR 937).
01a,08jul92,caf  created, based on version 01c of pccTimer.c.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
DUSCC chip with a board-independent interface.  This library handles both
the system clock and the auxiliary clock functions.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

The macro DUSCC_BASE_ADRS must be defined when including this driver.
*/

/* includes */

#include "drv/timer/timerDev.h"

/* defines */

/*
 * If hardware varies from the typical 3.6864 MHz frequency supplied
 * to the DUSCC chip, define DUSCC_FREQ when including this driver.
 */

#ifndef	DUSCC_FREQ
#define	DUSCC_FREQ	    3686400
#endif	/* DUSCC_FREQ */

/*
 * This macro expresses the lowest clock rate that can be attained by a
 * 16 bit timer without using a prescaler (0x0ffff is the max timer value).
 */

#define DUSCC_MIN_DIV_1	    (DUSCC_FREQ / 0x0ffff + 1)

/* locals */ 

LOCAL int     sysClkTicksPerSecond = 60; 
LOCAL BOOL    sysClkRunning        = FALSE; 
LOCAL FUNCPTR sysClkRoutine        = NULL; 
LOCAL int     sysClkArg            = NULL; 
LOCAL BOOL    sysClkConnected      = FALSE; 

LOCAL int     auxClkTicksPerSecond = 60; 
LOCAL BOOL    sysAuxClkRunning     = FALSE; 
LOCAL FUNCPTR sysAuxClkRoutine     = NULL; 
LOCAL int     sysAuxClkArg         = NULL;
LOCAL BOOL    auxClkConnected      = FALSE; 

/*******************************************************************************
*
* sysClkInt - handle system clock interrupts
*
* This routine handles system clock interrupts.
*/

LOCAL void sysClkInt (void)

    {
    /* acknowledge interrupt */

    *DUSCC_ICTSRA = DUSCC_ICTSR_CT_ZERO_COUNT;

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
* RETURN: OK, or ERROR if the routine cannot be connected to the interrupt.
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

/*******************************************************************************
*
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
 
        *DUSCC_CCRA = DUSCC_CCR_CT_STOP;        /* stop timer */
        sysClkRunning = FALSE;
        }
    }

/****************************************************************************** 
*
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
	FAST int	tc;
	FAST int	prescale;

        /* calculate the divide ratio, and write it to the timer chip */

	prescale = (sysClkTicksPerSecond < DUSCC_MIN_DIV_1) ? 64 : 1;
	tc = DUSCC_FREQ / prescale / sysClkTicksPerSecond;

        *DUSCC_CTPRHA = MSB (tc);
        *DUSCC_CTPRLA = LSB (tc);

        /* enable and start clock interrupts */

        if (prescale == 1)
            {
	    /* prescale is 1 */

            *DUSCC_CTCRA = DUSCC_CTCR_ZERO_DET_INTR_ENABLE |
                           DUSCC_CTCR_CLK_X1_CLK |
                           DUSCC_CTCR_PRESCALER_DIV_1;
            }
        else
            {
	    /* prescale is 64 */

            *DUSCC_CTCRA = DUSCC_CTCR_ZERO_DET_INTR_ENABLE |
                           DUSCC_CTCR_CLK_X1_CLK |
                           DUSCC_CTCR_PRESCALER_DIV_64;
            }

        *DUSCC_CCRA  = DUSCC_CCR_CT_PRE_CTPRHL;	/* preload */
        *DUSCC_CCRA  = DUSCC_CCR_CT_START;	/* start   */
 
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
 
/*******************************************************************************
*
* sysAuxClkInt - handle auxiliary clock interrupts
*/
 
LOCAL void sysAuxClkInt (void)
 
    {
    /* acknowledge interrupt */

    *DUSCC_ICTSRB = DUSCC_ICTSR_CT_ZERO_COUNT;

    if (sysAuxClkRoutine != NULL)
        (*sysAuxClkRoutine) (sysAuxClkArg);
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
    auxClkConnected  = TRUE;
 
    sysAuxClkRoutine = routine;
    sysAuxClkArg     = arg;
 
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
    if (sysAuxClkRunning)
        {
        *DUSCC_CCRB = DUSCC_CCR_CT_STOP;        /* stop timer */
 
        sysAuxClkRunning = FALSE;
        }
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
    if (!sysAuxClkRunning)
        {
	FAST int	tc;
	FAST int	prescale;

        /* calculate the divide ratio, and write it to the timer chip */

	prescale = (auxClkTicksPerSecond < DUSCC_MIN_DIV_1) ? 64 : 1;
	tc = DUSCC_FREQ / prescale / auxClkTicksPerSecond;

        *DUSCC_CTPRHB = MSB (tc);
        *DUSCC_CTPRLB = LSB (tc);

        /* enable and start clock interrupts */

        if (prescale == 1)
            {
	    /* prescale is 1 */

	    *DUSCC_CTCRB = DUSCC_CTCR_ZERO_DET_INTR_ENABLE |
			   DUSCC_CTCR_CLK_X1_CLK |
			   DUSCC_CTCR_PRESCALER_DIV_1;
            }
        else
            {
	    /* prescale is 64 */

            *DUSCC_CTCRB = DUSCC_CTCR_ZERO_DET_INTR_ENABLE |
                           DUSCC_CTCR_CLK_X1_CLK |
                           DUSCC_CTCR_PRESCALER_DIV_64;
            }

        *DUSCC_CCRB  = DUSCC_CCR_CT_PRE_CTPRHL;	/* preload */
        *DUSCC_CCRB  = DUSCC_CCR_CT_START;	/* start   */
 
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
