/* sysMv64360AuxClk.c - Motorola MV64360 AuxClock library */

/* Copyright 2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,02apr03,simon  Ported. from ver 01c, mcpm905/sysMv64360AuxClk.c.
*/

/*
DESCRIPTION
There are a total of four identical timer/counters on the MV64360,
each 32-bits wide.  Each timer/counter can be selected to operate as
a timer or a counter.  Each timer/counter increments with every Tclk
rising edge.  The MV64360 timer/counters are driven by the CPU bus clock.  

This file contains routines to use timer/counter 0 as the auxiliary 
clock.  The other three timer/counters (1 through 3) are not affected 
by the AuxClock routines.  

In Counter mode, the counter counts down to zero, stops, and issues 
an interrupt.

In Timer mode, the timer counts down, issues an interrupt when it
reaches zero, reloads itself to the programmed value, and continues 
to count.

Besides four timer/counter registers, there are three other timer
related registers:

Timer/Counter Control Register -
Bit 0 is the Timer/Counter Enable bit of timer/counter 0.  Writing
a value of "0" disables timer/counter 0, while writing a value of
"1" enables timer/counter 0.  When in timer mode, the count continues 
as long as this bit is set to '1'.  Bit 1 is the Timer/Counter Mode bit, 
the timer/counter 0 is in timer mode if this bit is set to '1'.

Timer/Counter Interrupt Cause Register -
Bit 0 is set to 1 upon timer 0 terminal count.  It's cleared by writing 
a value of "0".  Writing a value of "1" has no effect.

Timer/Counter Interrupt Mask Register -
Writing a value of "0" to bit 0 disables timer/counter 0 interrupt.
Writing a value of "1" to bit 0 enables timer/counter 0 interrupt.
*/

/* includes */

#include "mv64360.h"
#include "config.h"

/* defines */

#define MV64360_TMR_RATE  	(CPU_BUS_SPD)

/* typedefs */

/* globals */

/* locals */

LOCAL FUNCPTR 	sysAuxClkRoutine	= NULL;
LOCAL int 	sysAuxClkArg		= 0; 
LOCAL BOOL	sysAuxClkRunning	= FALSE;
LOCAL int	sysAuxClkTicksPerSecond = 60;
LOCAL BOOL	sysAuxClkConnected	= FALSE;

/* forward declarations */

LOCAL void 	sysAuxClkInt     (void);
LOCAL STATUS 	sysAuxClkInit    (void);
STATUS 		sysAuxClkConnect (FUNCPTR routine, int arg);
void 		sysAuxClkDisable (void);
void 		sysAuxClkEnable  (void);
int 		sysAuxClkRateGet (void);
STATUS 		sysAuxClkRateSet (int ticksPerSecond);

/*******************************************************************************
*
* sysAuxClkInt - handle an auxiliary clock interrupt from Timer/Counter 0
*
* This routine handles an MV64360 Timer/Counter 0 interrupt.  It clears
* the interrupt and calls the routine installed by sysAuxClkConnect().
*
* RETURNS: N/A
*/

LOCAL void sysAuxClkInt (void)
    {
    /* clear timer/counter 0 cause bit */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_INT_CAUSE,
			  ((MV64360_READ32 
                          (MV64360_REG_BASE, TMR_CNTR_INT_CAUSE)) 
                          & ~TMR_CNTR_INT_CAUSE_TC0_MASK)); 

    if (sysAuxClkRoutine != NULL)
	{
	(*sysAuxClkRoutine) (sysAuxClkArg);
	}
    }

/*******************************************************************************
*
* sysAuxClkInit - MV64360 auxiliary clock initialization routine
*
* This routine should be called before calling any other routine in this
* module.
*
* RETURNS: OK, always.
*/

LOCAL STATUS sysAuxClkInit (void)
    {

    /* disable timer/counter 0 */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_CTRL, 
			  ((MV64360_READ32 (MV64360_REG_BASE, TMR_CNTR_CTRL)) &
			  ~TMR_CNTR_CTRL_TC0EN_MASK));
	
    /* set timer/counter in timer mode */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_CTRL, 
			  ((MV64360_READ32 (MV64360_REG_BASE, TMR_CNTR_CTRL)) |
			   TMR_CNTR_CTRL_TC0MOD_MASK));

    /* connect and enable the associated main cause interrupt */

    intConnect (TMR_CNTR0_INT_VEC, (VOIDFUNCPTR)sysAuxClkInt, 0);

    intEnable (TMR_CNTR0_INT_LVL);

    /* enable timer/counter 0 interrupt */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_INT_MASK,
			  ((MV64360_READ32 (MV64360_REG_BASE, 
			    TMR_CNTR_INT_MASK)) |
			  TMR_CNTR_INT_MASK_TC0_MASK));

    sysAuxClkRunning = FALSE;

    return (OK);
    }

/********************************************************************************
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.
*
* RETURN: OK, always.
*
* SEE ALSO: intConnect(), sysAuxClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR routine,	/* routine called at each aux clock interrupt */
    int arg		/* argument with which to call routine */
    )
    {
    sysAuxClkRoutine	= routine;
    sysAuxClkArg	= arg;
    sysAuxClkConnected  = TRUE;

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

        /* stop counting */
        
        MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_CTRL,
			      ((MV64360_READ32 (MV64360_REG_BASE, 
				TMR_CNTR_CTRL)) &
			      ~TMR_CNTR_CTRL_TC0EN_MASK));

	/* disable timer/counter 0 interrupt */
	
	MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_INT_MASK,
			      ((MV64360_READ32 (MV64360_REG_BASE, 
				TMR_CNTR_INT_MASK)) &
			      ~TMR_CNTR_INT_MASK_TC0_MASK));

	/* disable associated main cause interrupt */

    	intDisable (TMR_CNTR0_INT_LVL);

	sysAuxClkRunning = FALSE;
	}
    }

/********************************************************************************
* sysAuxClkEnable - turn on auxiliary clock interrupts
*
* This routine enables auxiliary clock interrupts.
*
* RETURN: N/A
*
* SEE ALSO: sysAuxClkConnect(), sysAuxClkDisable()
*/

void sysAuxClkEnable (void)
    {
    if (!sysAuxClkRunning)
        {

        /* write value to count from */

        MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR0, 
 			      (MV64360_TMR_RATE/sysAuxClkTicksPerSecond));

        /* enable timer/counter 0 */
       
        MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_CTRL,
			      ((MV64360_READ32 (MV64360_REG_BASE, 
				TMR_CNTR_CTRL)) |
			      TMR_CNTR_CTRL_TC0EN_MASK));

        sysAuxClkRunning = TRUE;

	/* enable the associated main cause interrupt in case it was disabled */

    	intEnable (TMR_CNTR0_INT_LVL);

	/* enable timer/counter 0 interrupt */
	
        MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_INT_MASK,
			      ((MV64360_READ32 (MV64360_REG_BASE, 
				TMR_CNTR_INT_MASK)) |
			      TMR_CNTR_INT_MASK_TC0_MASK));
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
    return (sysAuxClkTicksPerSecond);
    }

/*******************************************************************************
*
* sysAuxClkRateSet - set the auxiliary clock rate
*
* This routine sets the interrupt rate of the auxiliary clock. It does not
* enable auxiliary clock interrupts.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/

STATUS sysAuxClkRateSet
    (
    int ticksPerSecond	/* number of clock interrupts per second */
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
