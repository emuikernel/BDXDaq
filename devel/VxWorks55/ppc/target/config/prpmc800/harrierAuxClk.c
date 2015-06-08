/* harrierAuxClk.c - Harrier Timer (Aux Clk) library */

/* Copyright 1999-2001 Wind River Systems, Inc. */
/* Copyright 1997-2001 Motorola, Inc. All Rights Reserved */
 
/*
modification history
--------------------
01b,19sep00,krp  Added support for Harrier Aux Clock
01a,31aug00,dmw  Written (from version 01b of mcpn765/hawkAuxClk.c).
*/
 
/*
DESCRIPTION
This timer contains routines to use MPIC timer 0 on the Harrier as the auxiliary
clock. The Harrier MPIC timers are driven by the PCI bus clock through a divide by
8 pre-scaler. There are a total of 4 identical MPIC timers each 31-bits wide.
Each timer is composed of the following registers:

Current Count Register - 
Bits 30-0 hold the current value of the timer which
counts down towards 0. each time the timer value
reaches zero, Bit 31 (MSB) toggles and an interrupt
is made pending.

Base Count Register - 
Bit 31 of this register contains a count inhibit bit.
When this bit is set to 0, the associated MPIC timer
counts down towards 0. When the count inhibit bit is
set to 1, the associated MPIC timer stops decrementing
and holds its current value. Bits 30-0 contain a timer
reload value. When a value is written to this field and
the count inhibit bit changes from 1 to 0, the base
count value is loaded into the current count register
and the toggle bit in the current count register is
cleared. Bits 30-0 of the Base Count register are also
transferred to the Current Count register each time the
current count register counts down to zero. This allows
the timer to create a periodic interrupt without
processor intervention.

Vector/Priority Register - 
Bit 31 (MSB) of this register contains the Mask
bit, which when set to 1, inhibits the delivery
of the associated MPIC timer's interrupt. Bit 30
contains the Active bit will be set to a 1 anytime
the associated MPIC timer is requesting an
interrupt or has an interrupt in service. Bits 19
through 16 hold a 4-bit interrupt priority value
where 15 is the highest priority and 0 effectively
disables the interrupt. Bits 7-0 hold an 8-bit
vector which will be provided during MPIC
interrupt acknowledge cycles when the interrupt
associated with this timer is acknowledged.

Destination Register - 
If bit 0 (LSB) of this register is set, the interrupt
associated with this MPIC timer is directed to CPU0. If
bit 1 is set, the interrupt is directed to CPU1.

In addition to the per-timer registers decribed above, there is a single Timer
Frequency Register which is used to hold the effective clock frequency of the
MPIC timer clock frequency. Start-up code must initialize this 32-bit register
to the PCI clock frequency divided by 8.
*/

/* includes */

/* defines */

/*
 * NOTE: For the MPIC_TIMER_CNT using the sysGetBusSpd function instead of
 *       sysGetPciSpd. Typically, for the harrier implementation this is
 *       is 100/8 MHz or 12.5 MHz.
 */

#define MPIC_TIMER_CNT	(sysGetBusSpd() / 8) 
#define MPIC_TIMER_DISABLE	0xFFFFFFFF

/* locals */

LOCAL void    sysAuxClkInt ();
LOCAL STATUS  sysAuxClkInit();
LOCAL FUNCPTR sysAuxClkRoutine        = NULL;
LOCAL int     sysAuxClkArg            = 0; 
LOCAL int     sysAuxClkTicksPerSecond = 60;
LOCAL BOOL    sysAuxClkConnected      = FALSE;
LOCAL int     sysAuxClkRunning        = FALSE;


/******************************************************************************
*
* sysAuxClkInt - handle an auxiliary clock interrupt from Harrier timer 0
*
* This routine handles a Harrier timer 0 interrupt.  It clears the interrupt
* and calls the routine  installed by sysAuxClkConnect().
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
* sysAuxClkInit - Harrier aux. clock initialization routine
*
* This routine should be called before calling any other routine in this
* module.
*
* RETURNS: OK, always.
*/
 
STATUS sysAuxClkInit (void)
    {
    /* disable counter */

    MPIC_WRITE(MPIC_TIMER0_BASE_CT_REG, MPIC_TIMER_DISABLE);
    EIEIO;					/* synchronize */
   
    /* setup timer frequency register */

    MPIC_WRITE(MPIC_TIMER_FREQ_REG, MPIC_TIMER_CNT);
   
    /* interrupt unmasked, priority level 15, vector TIMER0_INT_VEC. */

    MPIC_WRITE(MPIC_TIMER0_VEC_PRI_REG, 
                  (( PRIORITY_LVL15 | (TIMER0_INT_VEC) ) & ~TIMER_INHIBIT));
   
    /* interrupt directed at processor 0 */

    MPIC_WRITE(MPIC_TIMER0_DEST_REG, DESTINATION_CPU0);

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
* RETURNS: OK, always.
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

        MPIC_READ(MPIC_TIMER0_BASE_CT_REG, baseCount);
	MPIC_WRITE(MPIC_TIMER0_BASE_CT_REG, 
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

        MPIC_WRITE(MPIC_TIMER0_DEST_REG, 0);

	/* enable counter and write value to count from */

	MPIC_WRITE(MPIC_TIMER0_BASE_CT_REG,
	              ((MPIC_TIMER_CNT/sysAuxClkTicksPerSecond) & 
                      ~(MPIC_TIMER_BASE_CT_CI)));

        /* Direct interrupt at CPU 0 */

        MPIC_WRITE(MPIC_TIMER0_DEST_REG, DESTINATION_CPU0);
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
