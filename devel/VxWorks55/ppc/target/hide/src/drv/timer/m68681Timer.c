/* m68681Timer.c - MC68681/SC2681 timer driver */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,10jun97,wlf  doc: cleanup
01g,03mar97,kkk  added timestamp support. Added macro M68681_PRESCALER.
01f,04feb97,dat	 changed timer to run in counter mode.
01e,04jan97,p_m  removed "," in param list that was introduced by doc cleanup.
01d,25oct96,wlf  doc: cleanup.
01c,27mar96,dat  new timer driver, to be used with new sio serial drivers
01b,08feb93,caf  fixed ansi warning (SPR 1851).
01a,21aug92,caf  created.
*/

/*
DESCRIPTION
This is the timer driver for the MC68681/SC2681 DUART.
It uses the DUART for the system clock only.

This driver relies on the serial driver m68681Serial.c for its operation.

The macros SYS_CLK_RATE_MIN and SYS_CLK_RATE_MAX must be defined to provide
parameter checking for sysClkRateSet().

The macro DUART_CLOCK_DEVICE must be defined.
If the driver controls only one DUART, DUART_CLOCK_DEVICE must be 0.

The macro DUART_HZ must be defined to reflect the clock frequency supplied
to the DUART hardware.  The typical value is 3686400.

If this driver is being used with the new bi-modal driver m68681Sio.c, the
a completely new interface is used.  This is expected to be a model for
a new class of timer devices altogether.  This new interface is initialized
by the m68681DevInit() function which links the m68681Duart to the new
m68681Clk structures.
*/

#ifdef INCLUDE_TYCODRV_5_2

/* TO BE USED WITH SINGLE-MODE SERIAL DRIVER */

/* locals */

LOCAL int	sysClkTicksPerSecond	= 60;
LOCAL int	sysClkRunning		= FALSE;
LOCAL BOOL	sysClkConnected		= FALSE;
LOCAL FUNCPTR 	sysClkRoutine		= NULL;
LOCAL int 	sysClkArg		= NULL;

IMPORT void tyImrSet ();
IMPORT void tyCoClkConnect ();

/*******************************************************************************
*
* sysClkInt - handle system clock interrupts
*
* This routine handles system clock interrupts.
*
* RETURNS: N/A
*/

LOCAL void sysClkInt (void)
    {
    if (*tyCoDv [2 * DUART_CLOCK_DEVICE].ctroff)      /* reset the interrupt */
	;

    if (sysClkRoutine != NULL)
	(*(FUNCPTR) sysClkRoutine) (sysClkArg);
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
    FUNCPTR routine,	/* routine called at each system clock interrupt */
    int     arg		/* argument with which to call routine           */
    )
    {
    sysHwInit2 ();	/* XXX for now -- needs to be in usrConfig.c */

    /* DUART serial driver handles the timer interrupt */

    tyCoClkConnect (DUART_CLOCK_DEVICE, sysClkInt);

    sysClkRoutine   = routine;
    sysClkArg       = arg;
    sysClkConnected = TRUE;

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
	tyImrSet (& tyCoDv [2 * DUART_CLOCK_DEVICE], 0, CTR_RDY_INT);
	*tyCoDv [2 * DUART_CLOCK_DEVICE].acr = BRG_SELECT;
	sysClkRunning = FALSE;
	}
    }

/*******************************************************************************
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
	int temp;

	/* preload the preload register */

	temp = (DUART_HZ / 2) / sysClkTicksPerSecond;

	*tyCoDv [2 * DUART_CLOCK_DEVICE].ctlr = LSB (temp);
	*tyCoDv [2 * DUART_CLOCK_DEVICE].ctur = MSB (temp);
	*tyCoDv [2 * DUART_CLOCK_DEVICE].acr = BRG_SELECT | TMR_EXT_CLK;

	tyImrSet (& tyCoDv [2 * DUART_CLOCK_DEVICE], CTR_RDY_INT, 0);

	if (* tyCoDv [2 * DUART_CLOCK_DEVICE].ctron)
	    ;				/* start timer */

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

/*******************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.  It does not
* enable system clock interrupts.  It is called by usrRoot() in
* usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond      /* number of clock interrupts per second */
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

#else	/* INCLUDE_TYCODRV_5_2 */

/* TO BE USED WITH NEW DUAL-MODE SIO DRIVER */

/*
DESCRIPTION
This is the timer driver for the MC68681/SC2681 DUART.
Each MC68681 device provides one counter/timer device.  This driver
presumes counter mode operation.

This driver relies on the serial driver m68681Sio.c for its operation.
This driver should not be used if m68681Sio.o is not included in the system.

The default clock rate can be specified by setting the macro M68681_CLK_
TICKS to the desired value.  If not set, the value used will be 60 hertz.

To use this driver, the user creates and initializes the data in a 
M68681_CLK structure.  The address of that structure, and the address
of the associated M68681_DUART structure (which has already been created
and completely initialized) are passed to the m68681ClkInit routine.

i.e.

.CS
/@ data structures, serial and clock @/
M68681_DUART myDuart;
M68681_CLK myClock;

sysHwInit2 ()
    {
    ...

    /@ 68681 Duart must be initialized first @/
    m68681DevInit (&myDuart);

    ...

    /@ interrupt should be connected before m68681ClkInit @/
    intConnect (INUM_TO_IVEC(CLK_INUM), m68681ClkInt, &myClock);

    ...

    /@ initialize clock structure elements, as needed @/
    myClock->min   = SYS_CLK_RATE_MIN;
    myClock->max   = SYS_CLK_RATE_MAX;
    myClock->hertz = 3686400;
    myClock->rate  = 60;

    /@ call initialization procedure @/
    m68681ClkInit (&myClock, &myDuart);

    ...
    }
.CE
*/

#include "drv/sio/m68681Sio.h"

/* default clock rate */
#ifndef M68681_CLK_TICKS
#define M68681_CLK_TICKS	(60)
#endif

/* default prescaler value in counter mode */
#ifndef M68681_PRESCALER
#define M68681_PRESCALER        (16)
#endif

/*
 *  Allow user to redefine the register access procedure.
 * Default is to assume normal memory accesses.
 * The register pointer argument comes directly from the
 * data structures.
 */

#ifndef M68681_READ
#define M68681_READ(x)	(*(x))	/* arg is register pointer */
#endif

#ifndef M68681_WRITE
#define M68681_WRITE(x,y) (*(x) = y)	/* args are reg ptr, data to write */
#endif

/* Oscillator frequency input to m68681 */

#define M68681_OSC_HZ		(3686400)
#define MAX_CTR_VALUE		(0xffff)
#define MIN_CTR_VALUE		(0x2)

/*********************************************************************
*
* m68681ClkInit - initialize an MC68681 counter/timer
*
* This routine initializes a clock device.  It requires a pointer to
* the associated DUART structure.
*
* After initialization, the MC68681 CLK structure is disabled with no attached
* routines.
*
* If the structure elements <hertz>, <rate>, <min>, or <max> are uninitialized,
* this routine initializes them.  If these values are invalid, they
* are corrected.
*
* RETURNS: OK, or ERROR if either <pClk> or <pDuart> is invalid.
*/

STATUS m68681ClkInit 
    (
    M68681_CLK *pClk,	/* clock to be initialized */
    M68681_DUART *pDuart /* associated DUART for this clock */
    )
    {
    int temp;

    if (pClk == NULL || pDuart == NULL)
	return ERROR;

    /* initialize clock data fields */
    pClk->running	= FALSE;
    pClk->routine	= NULL;
    pClk->arg		= 0;

    /* normalize and fix up ticks, hertz, min, max */
    if (pClk->rate <= 0)
	pClk->rate = M68681_CLK_TICKS;

    if (pClk->hertz <= 0)
	pClk->hertz = M68681_OSC_HZ;

    temp = pClk->hertz / (M68681_PRESCALER * MAX_CTR_VALUE);
    if (pClk->min < temp)
	pClk->min = temp;

    temp = pClk->hertz / (M68681_PRESCALER * MIN_CTR_VALUE);
    if (pClk->max > temp || pClk->max <= 0)
	pClk->max = temp;

    /* connect DUART to clock */
    pClk->pDuart	= pDuart;

    /* connect clock to DUART */
    pDuart->tickArg	= pClk;
    pDuart->tickRtn	= m68681ClkInt;

    return OK;
    }


/********************************************************************
*
* m68681ClkInt - MC68681 clock interrupt handler
*
* This is the clock interrupt handler for the MC68681/SC2681 DUART.  The
* system connects this routine to the appropriate interrupt vector.
*
* Because the counter is actually stopped for a very small amount of
* time, there is some amount of clock skew in the system.  It should
* be less than the actual clock frequency error.
*
* RETURNS: N/A
*/

void m68681ClkInt 
    (
    M68681_CLK *pClk	/* clock to be serviced */
    )
    {
    char resetTimer;
    UINT16 temp;

    /* turn off the counter */

    resetTimer = M68681_READ (pClk->pDuart->ctroff);

    /* adjust for clock skew */

    temp = (M68681_READ (pClk->pDuart->ctur) << 8);
    temp |= M68681_READ (pClk->pDuart->ctlr);

    temp += pClk->preload; 

    M68681_WRITE (pClk->pDuart->ctlr, LSB (temp));
    M68681_WRITE (pClk->pDuart->ctur, MSB (temp));

    /* restart counter */

    resetTimer = M68681_READ (pClk->pDuart->ctron);

    if (pClk->routine != NULL)
	(*pClk->routine) (pClk->arg);
    }

/********************************************************************
*
* m68681ClkConnect - connect a routine to the clock interrupt
*
* This routine specifies the user routine to be called at each clock
* interrupt of a Motorola MC68681 timer.  It does not enable system 
* clock interrupts.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: m68681ClkEnable()
*/

STATUS m68681ClkConnect
    (
    M68681_CLK *pClk,	/* clock to connect to */
    VOIDFUNCPTR routine,/* routine called at each clock interrupt */
    void *  arg		/* argument with which to call routine */
    )
    {
    pClk->routine   = NULL;
    pClk->arg       = arg;
    pClk->routine   = routine;

    return OK;
    }

/********************************************************************
*
* m68681ClkDisable - turn off clock interrupts
*
* This routine disables clock interrupts generated by the Motorola MC68681
* timer.
*
* RETURNS: N/A
*
* SEE ALSO: m68681ClkEnable()
*/

void m68681ClkDisable 
    (
    M68681_CLK *pClk	/* clock to be disabled */
    )
    {
    if (pClk->running)
	{
	int level = intLock ();

	/* Disable the timer interrupt */

	m68681ImrSetClr (pClk->pDuart, 0, M68681_ISR_CTR_RDY_INT);

	/* stop clock */

	M68681_READ (pClk->pDuart->ctroff);

        pClk->running = FALSE;

	intUnlock (level);
	}
    }

/********************************************************************
*
* m68681ClkEnable - turn on clock interrupts
*
* This routine enables clock interrupts generated by the Motorola MC68681
* timer.
*
* RETURNS: N/A
*
* SEE ALSO: m68681ClkConnect(), m68681ClkDisable(), m68681ClkRateSet()
*/

void m68681ClkEnable 
    (
    M68681_CLK * pClk 	/* clock to be enabled */
    )
    {
    if (!pClk->running)
	{
	int level;
	int temp;
	char startTimer;
	M68681_DUART *pDuart = pClk->pDuart;

	/* calculate the preload value */

	temp = (pClk->hertz) / M68681_PRESCALER / pClk->rate; 

	/* min is 2, max is 0xffff, per m68681 users manual */

	if (temp < MIN_CTR_VALUE)
	    temp = MIN_CTR_VALUE;
	else if (temp > MAX_CTR_VALUE)
	    temp = MAX_CTR_VALUE;

	pClk->preload = temp;

	level = intLock ();

	M68681_WRITE (pDuart->ctlr, LSB (temp));
	M68681_WRITE (pDuart->ctur, MSB (temp));

	/* Counter mode */

	m68681AcrSetClr (pDuart, M68681_ACR_CTR_EXT_CLK_16, 0);

	/* Enable the timer interrupt */

	m68681ImrSetClr (pDuart, M68681_ISR_CTR_RDY_INT, 0);

	/* Initialize the timer */

	startTimer = M68681_READ (pDuart->ctron);

	intUnlock (level);

        pClk->running = TRUE;
	}
    }

/*****************************************************************
*
* m68681ClkRateGet - get the clock rate
*
* This routine returns the interrupt rate of the MC68681 clock.
*
* RETURNS: The number of ticks per second of the MC68681 clock.
*
* SEE ALSO: m68681ClkEnable(), m68681ClkRateSet()
*/

int m68681ClkRateGet
    (
    M68681_CLK * pClk	/* clock to be read */
    )
    {
    return (pClk->rate);
    }

/*****************************************************************
*
* m68681ClkRateSet - set the clock rate
*
* This routine sets the interrupt rate of the MC68681 clock.  It does not
* enable clock interrupts.
*
* In addition to checking against the user-specified min/max rates, this
* routine also checks against the hardware limits for min/max rates.  These 
* limits cannot be exceeded.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO:
* m68681ClkEnable(), m68681ClkRateGet()
*/

STATUS m68681ClkRateSet
    (
    M68681_CLK *pClk,	/* clock to be set */
    int rate      /* number of clock interrupts per second */
    )
    {
    if (rate < pClk->min
     || rate > pClk->max)
        return (ERROR);

    pClk->rate = rate;

    if (pClk->running)
        {
        m68681ClkDisable (pClk);
        m68681ClkEnable  (pClk);
        }

    return (OK);
    }

/*****************************************************************
*
* m68681ClkReadOnFly - read the clock internal counter
*
* This routine reads the counter/timer value on the fly when receiving
* normal oscillator input.  The lowest 8 bits of the counter roll over every
* 140 microseconds.  A standard hi/low/hi read sequence ensures a consistent
* value from the pair of registers, assuming that the two 8-bit values are
* individually latched and there is no problem with ripple in either
* register.
*
* RETURNS: The current countdown counter value, or ERROR if
* there is a hardware error.
*/

int m68681ClkReadOnFly
    (
    M68681_CLK * pClk	/* clock to be read */
    )
    {
    volatile UCHAR *pLow = pClk->pDuart->ctlr;
    volatile UCHAR *pHi = pClk->pDuart->ctur;
    UCHAR low;
    UCHAR hi1;
    UCHAR hi2;
    int level;

    level = intLock ();

    /* usual hi/low/hi sequence */
    hi1 = M68681_READ (pHi);
    low = M68681_READ (pLow);
    hi2 = M68681_READ (pHi);
    if (hi1 != hi2)
	low = M68681_READ (pLow);

    intUnlock (level);

    return (int)((hi2 << 8) + low);
    }

IMPORT M68681_CLK * pSysClk;

/*******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  Normally, it is called from usrRoot() in usrConfig.c to
* connect usrClock() to the system clock interrupt.  
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR routine,	/* routine called at each system clock interrupt */
    int     arg		/* argument with which to call routine           */
    )
    {
    sysHwInit2 ();

    return (m68681ClkConnect (pSysClk, (VOIDFUNCPTR) routine, (void *) arg)); 
    }

/*******************************************************************************
*
* sysClkEnable - turn on system clock interrupts
*
* This routine enables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkConnect(), sysClkDisable(), sysClkRateSet()
*/

void  sysClkEnable (void)
    {
    m68681ClkEnable (pSysClk); 
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
    m68681ClkDisable (pSysClk); 
    }

/*******************************************************************************
*
* sysClkRateGet - get the system clock rate
*
* This routine returns the interrupt rate of the system clock.
*
* RETURNS: The number of ticks per second of the system clock.
*
* SEE ALSO: sysClkRateSet()
*/

int sysClkRateGet ()
    {
    return m68681ClkRateGet (pSysClk); 
    }

/*******************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.  It does not
* enable system clock interrupts.  It is called by usrRoot() in
* usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/

STATUS sysClkRateSet (int ticksPerSecond)
    {
    return m68681ClkRateSet (pSysClk, ticksPerSecond);
    }

#ifdef INCLUDE_TIMESTAMP

/*******************************************************************************
*
* sysTimestampConnect - connect a user routine to a timestamp timer interrupt
*
* This routine specifies the user interrupt routine to be called at each
* timestamp timer interrupt.
*
* RETURNS: ERROR, always.
*/

STATUS sysTimestampConnect
    (
    FUNCPTR routine,    /* routine called at each timestamp timer interrupt */
    int arg             /* argument with which to call routine */
    )
    {
    return (ERROR);
    }

/*******************************************************************************
*
* sysTimestampEnable - enable a timestamp timer interrupt
*
* This routine enables timestamp timer interrupts and resets the counter.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysTimestampDisable()
*/

STATUS sysTimestampEnable (void)
   {
   return (OK);
   }

/*******************************************************************************
*
* sysTimestampDisable - disable a timestamp timer interrupt
*
* This routine disables timestamp timer interrupts.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysTimestampEnable()
*/

STATUS sysTimestampDisable (void)
    {
    return (OK);
    }

/*******************************************************************************
*
* sysTimestampPeriod - get the period of a timestamp timer
*
* This routine gets the period of the timestamp timer, in ticks.  The
* period, or terminal count, is the number of ticks to which the timestamp
* timer counts before rolling over and restarting the counting process.
*
* RETURNS: The period of the timestamp timer in counter ticks.
*/

UINT32 sysTimestampPeriod (void)
    {
    return (pSysClk->preload);
    }

/*******************************************************************************
*
* sysTimestampFreq - get a timestamp timer clock frequency
*
* This routine gets the frequency of the timer clock, in ticks per
* second.  The rate of the timestamp timer is set explicitly by the
* hardware and typically cannot be altered.
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/

UINT32 sysTimestampFreq (void)
    {
    return (pSysClk->hertz / M68681_PRESCALER);
    }

/*******************************************************************************
*
* sysTimestamp - get a timestamp timer tick count
*
* This routine returns the current value of the timestamp timer tick counter.
* The tick count can be converted to seconds by dividing it by the return of
* sysTimestampFreq().
*
* This routine should be called with interrupts locked.  If interrupts are
* not locked, sysTimestampLock() should be used instead.
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestampFreq(), sysTimestampLock()
*/

UINT32 sysTimestamp (void)
    {
    /* timer is decrementing */

    return (pSysClk->preload - m68681ClkReadOnFly(pSysClk));
    }

/*******************************************************************************
*
* sysTimestampLock - lock interrupts and get the timestamp timer tick count
*
* This routine locks interrupts when the tick counter must be stopped
* in order to read it or when two independent counters must be read.
* It then returns the current value of the timestamp timer tick
* counter.
*
* The tick count can be converted to seconds by dividing it by the return of
* sysTimestampFreq().
*
* If interrupts are already locked, sysTimestamp() should be
* used instead.
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestampFreq(), sysTimestamp()
*/

UINT32 sysTimestampLock (void)
    {
    /* same as sysTimestamp() */
    return (pSysClk->preload - m68681ClkReadOnFly(pSysClk));
    }

#endif  /* INCLUDE_TIMESTAMP */
#endif	/* INCLUDE_TYCODRV_5_2 */

