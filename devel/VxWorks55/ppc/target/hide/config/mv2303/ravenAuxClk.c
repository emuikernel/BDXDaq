/* ravenAuxClk.c - Raven Timer (Aux Clk) library */

/* Copyright 1997 Wind River Systems, Inc. */
/* Copyright 1997 Motorola, Inc. All Rights Reserved */
 
/*
modification history
--------------------
01d,01nov01,mil  Cleaned up compiler warnings.
01c,05feb99,dmw	 Fix for timer hardware bug.
01b,09jun98,mas  prototype for sysAuxClkInit() now returns STATUS (SPR 21482).
01a,08oct97,srr	 created from external source.
*/
 
/*
DESCRIPTION
This timer contains routines to use timer 0 on the Raven as the auxiliary
clock.
*/


/* includes */

#include "ravenMpic.h"

/* defines */

#define MPIC_TIMER_CNT	(DEC_CLOCK_FREQ / 8)
#define MPIC_TIMER_DISABLE	0xFFFFFFFF

/* forward declarations and prototypes */

STATUS        sysAuxClkInit();
LOCAL void    sysAuxClkInt ();

/* locals */

LOCAL FUNCPTR sysAuxClkRoutine        = NULL;
LOCAL int     sysAuxClkArg            = 0;
LOCAL int     sysAuxClkTicksPerSecond = 60;
LOCAL BOOL    sysAuxClkConnected      = FALSE;
LOCAL int     sysAuxClkRunning        = FALSE;


/******************************************************************************
*
* sysAuxClkInt - handle an auxiliary clock interrupt from Raven timer 0
*
* This routine handles a Raven timer 0 interrupt.  It clears the interrupt
* and calls the routine installed by sysAuxClkConnect().
*
* RETURNS: N/A
*/
 
LOCAL void sysAuxClkInt (void)
    {
 
    if (sysAuxClkRoutine != NULL)
 	(*sysAuxClkRoutine) (sysAuxClkArg);
    }
 

/*****************************************************************************
*
* sysAuxClkInit - raven aux. clock initialization routine
*
* This routine should be called before calling any other routine in this
* module.
*
* RETURNS: OK, or ERROR.
*/
 
STATUS sysAuxClkInit (void)
    {
    /* disable counter */

    sysPciWrite32(MPIC_ADDR(MPIC_TIMER0_BASE_CT_REG), MPIC_TIMER_DISABLE);
    EIEIO;					/* synchronize */
   
    /* setup timer frequency register */

    sysPciWrite32(MPIC_ADDR(MPIC_TIMER_FREQ_REG), MPIC_TIMER_CNT);
   
    /* interrupt unmasked, priority level 15, vector TIMER0_INT_VEC. */

    sysPciWrite32(MPIC_ADDR(MPIC_TIMER0_VEC_PRI_REG), 
                  (( PRIORITY_LVL15 | (TIMER0_INT_VEC) ) & ~TIMER_INHIBIT));
   
    /* interrupt directed at processor 0 */

    sysPciWrite32(MPIC_ADDR(MPIC_TIMER0_DEST_REG), DESTINATION_CPU0);

    EIEIO;					/* synchronize */
   
    sysAuxClkRunning = FALSE;
   
    return (OK);
    }

/******************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysAuxClkEnable()
*/
 
STATUS sysAuxClkConnect
    (
    FUNCPTR routine,    /* routine called at each aux clock interrupt */
    int arg             /* argument with which to call routine        */
    )
    {
    sysAuxClkRoutine	= routine;
    sysAuxClkArg	= arg;
    sysAuxClkConnected	= TRUE;
 
    return (OK);
    }
 

/******************************************************************************
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
    UINT32 baseCount;

    if (sysAuxClkRunning)
        {
        /* Set Count Inhibit in the Base Count register. */

        sysPciRead32(MPIC_ADDR(MPIC_TIMER0_BASE_CT_REG), &baseCount);
	sysPciWrite32(MPIC_ADDR(MPIC_TIMER0_BASE_CT_REG), 
                      (baseCount | MPIC_TIMER_BASE_CT_CI));
	EIEIO;					/* synchronize */
        
        sysAuxClkRunning = FALSE;
        }
    }
 
/******************************************************************************
*
* sysAuxClkEnable - turn on auxiliary clock interrupts
*
* This routine enables auxiliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkDisable()
*/
 
void sysAuxClkEnable (void)
    {
    if (!sysAuxClkRunning)
	{
        /* Direct interrupt at no one */

        sysPciWrite32(MPIC_ADDR(MPIC_TIMER0_DEST_REG), 0);

	/* enable counter and write value to count from */

	sysPciWrite32(MPIC_ADDR(MPIC_TIMER0_BASE_CT_REG),
	              ((MPIC_TIMER_CNT/sysAuxClkTicksPerSecond) & 
                      ~(MPIC_TIMER_BASE_CT_CI)));

        /* Direct interrupt at CPU 0 */

        sysPciWrite32(MPIC_ADDR(MPIC_TIMER0_DEST_REG), DESTINATION_CPU0);
	EIEIO;					/* synchronize */
	  
	sysAuxClkRunning = TRUE;
	}
    }
 
/******************************************************************************
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
 
/******************************************************************************
*
* sysAuxClkRateSet - set the auxiliary clock rate
*
* This routine sets the interrupt rate of the auxiliary clock.  It is not
* supported, since the auxiliary clock always runs at the same rate as the
* system clock.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/
 
STATUS sysAuxClkRateSet
    (
    int ticksPerSecond  /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < AUX_CLK_RATE_MIN ||
        ticksPerSecond > AUX_CLK_RATE_MAX)
	return (ERROR);
 
    sysAuxClkTicksPerSecond = ticksPerSecond;

    if (sysAuxClkRunning)
	{
 	sysAuxClkDisable ();
 	sysAuxClkEnable ();
 	}
 
    return (OK);
    }
