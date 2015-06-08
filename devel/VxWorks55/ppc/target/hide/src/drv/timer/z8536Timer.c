/* z8536Timer.c - Z8536 timer library */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01i,20dec96,dat  fixed SPR 4274, sysTimestampLock, didn't do a lock.
01h,23sep96,dat  merged with timestamp driver.
01g,11jul96,wlf  doc: cleanup.
01f,06jun96,wlf  doc: cleanup.
01e,09aug95,kkk  fixed I960 warnings. (spr# 4177)
01d,06jan93,dzb  removed sysClk1Int().
01c,07dec92,ccc  added sysCioReset() routine to reset specified device.
01b,20oct92,ccc  added volatile to register pointer.
01a,28aug92,caf  moved clock routines from ver.01p of hkv30/sysLib.c, ansified.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
Zilog Z8536.  This library handles both the system clock and the auxiliary
clock functions.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

The macros ZCIO_CNTRL_ADRS and ZCIO_HZ must also be defined to indicate the
control register address and clock frequency of the Z8536, respectively.

The system clock is the ZCIO counter/timer 3.
The auxiliary clock is the ZCIO counter/timer 2.
The timestamp clock is the ZCIO counter/timer 1.

SEE ALSO
.I "ZCIO Counter/Timer and Parallel I/O Unit Technical Manual"
*/

/* includes */

#include "drv/timer/timestampDev.h"

/* locals */

LOCAL FUNCPTR sysClkRoutine           = NULL;
LOCAL int     sysClkArg               = NULL;
LOCAL int     sysClkTicksPerSecond    = 60;
LOCAL BOOL    sysClkConnected         = FALSE;
LOCAL int     sysClkRunning           = FALSE;
LOCAL FUNCPTR sysAuxClkRoutine        = NULL;
LOCAL int     sysAuxClkArg            = NULL;
LOCAL int     sysAuxClkTicksPerSecond = 60;
LOCAL BOOL    sysAuxClkConnected      = FALSE;
LOCAL int     sysAuxClkRunning        = FALSE;

#ifdef INCLUDE_TIMESTAMP

#define ZCIO_MAX_TIME_CNST		0x10000

LOCAL	void	sysTimestampInt (void);
LOCAL	FUNCPTR	sysTimestampRoutine     = NULL;
LOCAL	int	sysTimestampArg         = NULL;
LOCAL	BOOL	sysTimestampConnected   = FALSE;
LOCAL	int	sysTimestampRunning     = FALSE;
#endif /*INCLUDE_TIMESTAMP*/
     
/******************************************************************************
*
* sysCioReset - z8536 reset
*
* This routine resets the specified z8536.
*
* NOMANUAL
*/

void sysCioReset
    (
    volatile UINT8 *pCio		/* cio to reset */
    )
    {
    volatile UINT8 temp;
    int delay;

    temp  = *pCio;			/* see CIO manual sec 2.3 */
    *pCio = ZERO;
    temp  = *pCio;
    *pCio = ZCIO_MCC;			/* master config control */
    *pCio = ZERO;
    *pCio = ZCIO_MIC;			/* master interrupt control */
    *pCio = ZCIO_MIC_RESET;		/* device RESET */

    for (delay = 0; delay < 1000; delay++)
	;				/* allow reset to complete */

    *pCio = ZCIO_MIC;
    *pCio = ZERO;
    }

/*******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  Normally, it is called from usrRoot() in usrConfig.c to
* connect usrClock() to the system clock interrupt.  It also connects the clock
* error interrupt service routine.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR routine,    /* routine called at each system clock interrupt */
    int arg             /* argument with which to call routine           */
    )
    {
    sysHwInit2 ();      /* XXX for now -- needs to be in usrConfig.c */

    sysClkRoutine	= routine;
    sysClkArg		= arg;
    sysClkConnected	= TRUE;

    return (OK);
    }

/*******************************************************************************
*
* sysClkInt - handle a system clock interrupt
*
* This routine handles a system clock interrupt.  It acknowledges the
* interrupt and calls the routine installed by sysClkConnect().
*/

LOCAL void sysClkInt (void)
    {
    /* acknowledge the interrupt */

    *ZCIO_CNTRL_ADRS = ZCIO_CT3CS;		/* C/T 3 Command and Status */
    *ZCIO_CNTRL_ADRS = ZCIO_CS_CLIPIUS |	/* Clear IP and IUS */
		       ZCIO_CS_GCB;		/* Gate Command Bit */

    if (sysClkRoutine != NULL)
	(*sysClkRoutine) (sysClkArg);	/* call system clock service routine */
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

	*ZCIO_CNTRL_ADRS = ZCIO_CT3CS;	/* C/T 3 Command and Status */
	*ZCIO_CNTRL_ADRS = ZCIO_CS_CLIE;	/* Clear Interrupt Enable */

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
    volatile UINT8 *cioCtl;
    ULONG  tc;    /* time constant */

    if (!sysClkRunning)
	{
	cioCtl = ZCIO_CNTRL_ADRS;

	/* set time constant */

	tc = ZCIO_HZ / sysClkTicksPerSecond;
	*cioCtl = ZCIO_CT3TCMSB;             /* C/T 3 Time Const (MS Byte) */
	*cioCtl = (UINT8) MSB(tc);
	*cioCtl = ZCIO_CT3TCLSB;             /* C/T 3 Time Const (LS Byte) */
	*cioCtl = (UINT8) LSB(tc);

	/* clear and start timer */

	*cioCtl = ZCIO_CT3CS;                /* C/T 3 Command and Status */
	*cioCtl = ZCIO_CS_CLIPIUS;           /* Clear IP and IUS */
	*cioCtl = ZCIO_CT3CS;                /* C/T 3 Command and Status */
	*cioCtl = ZCIO_CS_SIE                /* Set Interrupt Enable */
		| ZCIO_CS_GCB                /* Gate Command Bit */
		| ZCIO_CS_TCB;               /* Trigger Command Bit */

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
* This routine sets the interrupt rate of the system clock.
* It is called by usrRoot() in usrConfig.c.
*
* NOTE: The valid range for the system clock is 38 to 10000
* ticks per second.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond  /* number of clock interrupts per second */
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
    FUNCPTR routine,    /* routine called at each aux clock interrupt */
    int arg             /* argument with which to call routine        */
    )
    {
    sysAuxClkRoutine	= routine;
    sysAuxClkArg	= arg;
    sysAuxClkConnected	= TRUE;

    return (OK);
    }

/*******************************************************************************
*
* sysAuxClkInt - handle an auxiliary clock interrupt
*
* This routine handles an auxiliary clock interrupt.  It acknowledges the
* interrupt and calls the routine installed by sysAuxClkConnect().
*
* RETURNS: N/A
*/

LOCAL void sysAuxClkInt (void)
    {
    /* acknowledge the interrupt */

    *ZCIO_CNTRL_ADRS = ZCIO_CT2CS;          /* C/T 2 Command and Status */
    *ZCIO_CNTRL_ADRS = ZCIO_CS_CLIPIUS      /* Clear IP and IUS */
			  | ZCIO_CS_GCB;         /* Gate Command Bit */

    /* call auxiliary clock service routine */

    if (sysAuxClkRoutine != NULL)
	(*sysAuxClkRoutine) (sysAuxClkArg);
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
	/* disable interrupts */

	*ZCIO_CNTRL_ADRS = ZCIO_CT2CS;	/* C/T 2 Command and Status */
	*ZCIO_CNTRL_ADRS = ZCIO_CS_CLIE;   /* Clear Interrupt Enable */

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
* SEE ALSO: sysAuxClkDisable()
*/

void sysAuxClkEnable (void)
    {
    volatile UINT8 *cioCtl;
    ULONG  tc;             /* time constant */

    if (!sysAuxClkRunning)
        {
	cioCtl = ZCIO_CNTRL_ADRS;

	/* set time constant */

	tc = ZCIO_HZ / sysAuxClkTicksPerSecond;
	*cioCtl = ZCIO_CT2TCMSB;             /* C/T 2 Time Const (MS Byte) */
        *cioCtl = (UINT8) MSB(tc);
	*cioCtl = ZCIO_CT2TCLSB;             /* C/T 2 Time Const (LS Byte) */
        *cioCtl = (UINT8) LSB(tc);

	/* clear and start timer */

	*cioCtl = ZCIO_CT2CS;                /* C/T 2 Command and Status */
	*cioCtl = ZCIO_CS_CLIPIUS;           /* Clear IP and IUS */
	*cioCtl = ZCIO_CT2CS;                /* C/T 2 Command and Status */
	*cioCtl = ZCIO_CS_SIE                /* Set Interrupt Enable */
		| ZCIO_CS_GCB                /* Gate Command Bit */
		| ZCIO_CS_TCB;               /* Trigger Command Bit */

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
    int ticksPerSecond  /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
        return (ERROR);

    sysAuxClkTicksPerSecond = ticksPerSecond;

    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}

    return (OK);
    }

/*******************************************************************************
*
* sysClkErrInt - handle a clock error interrupt
*
* An error interrupt occurs because a timer interrupt
* was not serviced before the next interrupt from the same timer
* occurred.  One or more clock ticks have been lost.
* An error interrupt is treated as a regular timer interrupt.
*
* NOTE:
* The system clock is the ZCIO counter/timer 3.
* The auxiliary clock is the ZCIO counter/timer 2.
* The timestamp clock is the ZCIO counter/timer 1.
*/

LOCAL void sysClkErrInt (void)
    {
    FAST UINT8  ctcs;           /* C/T Command and Status */
    FAST volatile UINT8 *cioCtl = ZCIO_CNTRL_ADRS;

    /* check if counter/timer 3 error set */

    *cioCtl = ZCIO_CT3CS;
    ctcs    = *cioCtl;

    if (ctcs & (ZCIO_CS_IP | ZCIO_CS_ERR))
	sysClkInt ();

    /* check if counter/timer 2 error set */

    *cioCtl = ZCIO_CT2CS;
    ctcs    = *cioCtl;

    if (ctcs & (ZCIO_CS_IP | ZCIO_CS_ERR))
	sysAuxClkInt ();

    /* check if counter/timer 1 error set */

    *cioCtl = ZCIO_CT1CS;
    ctcs    = *cioCtl;

    if (ctcs & (ZCIO_CS_IP | ZCIO_CS_ERR))
	{
#ifdef INCLUDE_TIMESTAMP
	sysTimestampInt ();
#else
	/* acknowledge the clock interrupt */

	*cioCtl = ZCIO_CT1CS;             /* C/T 1 Command and Status */
	*cioCtl = ZCIO_CS_CLIPIUS         /* Clear IP and IUS */
		| ZCIO_CS_GCB;            /* Gate Command Bit */

	logMsg ("counter/timer 1 interrupt\n", 0, 0, 0, 0, 0, 0);
#endif
	}
    }

#ifdef INCLUDE_TIMESTAMP
/*******************************************************************************
*
* sysTimestampConnect - connect a routine to the stamp clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* timestamp clock rollover interrupt.  It is also called from clock error
* interrupt service routine.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysTimestampEnable()
*/

STATUS sysTimestampConnect
    (
    FUNCPTR routine,    /* routine called at each system clock interrupt */
    int arg             /* argument with which to call routine           */
    )
    {

    sysTimestampRoutine	= routine;
    sysTimestampArg	= arg;
    sysTimestampConnected = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* sysTimestampInt - handle a timestamp clock interrupt
*
* This routine handles a rollover timer interrupt.  It acknowledges the
* interrupt and calls the routine installed by sysTimestampConnect().
*/

LOCAL void sysTimestampInt (void)
    {
    /* acknowledge the interrupt */

    *ZCIO_CNTRL_ADRS = ZCIO_CT1CS;		/* C/T 3 Command and Status */
    *ZCIO_CNTRL_ADRS = ZCIO_CS_CLIPIUS |	/* Clear IP and IUS */
		       ZCIO_CS_GCB ;		/* Gate Command Bit */

    /* call system clock service routine */

    if (sysTimestampRoutine != NULL)
	(*sysTimestampRoutine) (sysTimestampArg);
    }

/*******************************************************************************
*
* sysTimestampDisable - turn off timestamp clock without disabling interrupts
*
* This routine disables timestamp clock(it actually stops the counting
* sequence).
*
* RETURNS: N/A
*
* SEE ALSO: sysTimestampEnable()
*/

STATUS sysTimestampDisable (void)
    {
    volatile UINT8 *cioCtl = ZCIO_CNTRL_ADRS;
    FAST UINT8 temp;

    if (sysTimestampRunning)
        {

	/* Stop counting  sequence */

        *cioCtl = ZCIO_MCC;              /* Master Configuration Cntrl     */
        temp = *cioCtl;                  /* get the configuration          */
        *cioCtl = ZCIO_MCC;              /* Master Configuration Cntrl     */
        *cioCtl = temp&(~ZCIO_MCC_CT1E); /* Disable the timer/count 1      */

	*cioCtl = ZCIO_CT1CS;   	 /* C/T 1 Command and Status */
	*cioCtl = ZCIO_CS_CLIPIUS;       /* CLear IP and IUS,  */


	sysTimestampRunning = FALSE;
        }
    return (OK);
    }

/*******************************************************************************
*
* sysTimestampEnable - turn on Timestamp clock
*
* This routine enables timer/count1 which used as Timestamp clock. It also
* enables the interrupt for this timer/count.
*
* RETURNS: OK
*
* SEE ALSO: sysTimestampConnect(), sysTimestampDisable(),sysTimestampRateSet()
*/

STATUS sysTimestampEnable (void)
    {
    volatile UINT8 *cioCtl = ZCIO_CNTRL_ADRS;
    UINT8 temp;

    if (!sysTimestampRunning)
	{
        *cioCtl = ZCIO_MCC;              /* Master Configuration Cntrl     */
        temp = *cioCtl;                  /* read the master configuration  */
        *cioCtl = ZCIO_MCC;              /* Master Configuration Cntrl     */
        *cioCtl = temp|ZCIO_MCC_CT1E;    /* enables the timer/count 1      */

	/* set time constant, zero constant equal to 0x10000 */

        *cioCtl = ZCIO_CT1TCMSB;             /* C/T 1 Time Const (MS Byte) */
	*cioCtl = (UINT8) 0;
	*cioCtl = ZCIO_CT1TCLSB;             /* C/T 1 Time Const (LS Byte) */
	*cioCtl = (UINT8) 0;

	/* clear and start timer */

	*cioCtl = ZCIO_CT1CS;                /* C/T 1 Command and Status */
	*cioCtl = ZCIO_CS_CLIPIUS;           /* Clear IP and IUS */
	*cioCtl = ZCIO_CT1CS;                /* C/T 1 Command and Status */
	*cioCtl = ZCIO_CS_SIE                /* Set Interrupt Enable */
		| ZCIO_CS_GCB                /* Gate Command Bit */
		| ZCIO_CS_TCB;               /* Trigger Command Bit */

	sysTimestampRunning = TRUE;
	}
    return (OK);
    }
/*******************************************************************************
*
* sysTimestampPeriod - get the period of the timer
*
* This routine returns the period of the timer in timestamp ticks
*
* RETURNS: The period of the timer.
*
* SEE ALSO: sysTimestampEnable(), sysTimestampFreq()
*/

UINT32 sysTimestampPeriod (void)
    {
    return (ZCIO_MAX_TIME_CNST);
    }

/*******************************************************************************
*
* sysTimestampFreq - get the Timestamp clock output frequence
*
* This routine returns the output frequency of the timer, in timestamp ticks
*
* RETURNS: The number of ticks per second of the timestamp clock.
*
* SEE ALSO: sysTimestampEnable(), sysTimestampFreq()
*/

UINT32 sysTimestampFreq (void)
    {
    return (ZCIO_HZ);  /* the frequency */
    } 

/*******************************************************************************
*
* sysTimestamp - get the current value of the timestamp counter
*
* This routine returns the current value of the timestamp counter.
*
* RETURNS: The current timestamp counter value.
*
* SEE ALSO: sysTimestampEnable(), sysTimestampFreq()
*/

UINT32 sysTimestamp (void)
    {
    volatile UINT8 *cioCtl;
    FAST UINT16 temp=0;

    /* freeze and read current counter value */

    cioCtl = ZCIO_CNTRL_ADRS;
    *cioCtl = ZCIO_CT1CS;                /* C/T 1 Command and Status */
    *cioCtl = ZCIO_CS_RCC |              /* freeze the counter value */
              ZCIO_CS_GCB;               /* do not reset the Gate bit */
 
    *cioCtl = ZCIO_CT1CCMSB;             /* C/T 1 Current Count (MS Byte) */
    temp = *cioCtl;
    temp <<= 8;                   
    *cioCtl = ZCIO_CT1CCLSB;            /* C/T 1 Current Count (LS Byte) */
    temp |= *cioCtl;                   /* it also reset the ZCIO_CS_RCC bit*/

    return (ZCIO_MAX_TIME_CNST-temp);  /* This is a count down timer */
    }

/*******************************************************************************
*
* sysTimestampLock - get the current value of the timestamp counter
*
* This routine returns the current value of the timestamp counter.
*
* RETURNS: The current timestamp counter value.
*
* SEE ALSO: sysTimestampEnable(), sysTimestampFreq()
*/

UINT32 sysTimestampLock (void)
    {
    int lvl;
    UINT32 result;

    lvl = intLock ();

    result = sysTimestamp ();

    intUnlock (lvl);

    return result;
    }
#endif
