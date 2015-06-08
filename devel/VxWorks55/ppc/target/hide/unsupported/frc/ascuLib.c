/* ascuLib.c - Force SYS68K/CPU-21/29/32 ASCU2 library */

static char *copyright = "Copyright 1988-1989, Wind River Systems, Inc.";

/*
modification history
--------------------
02m,27feb89,tja  released bus in ascuIntGen if all interrupt channels
		   are unavailable to avoid bus locking
02l,24jan89,tja	 upgraded support and changed naming convention 
		   to be like WRS
02k,11jul88,gae  written based on Terry Arden's frc21/9 version.
	   +tja  untested at WRS.
*/

/*
DESCRIPTION
This library contains a set of routines to manipulate the Force ASCU2 board.

BUGS
Unknown.  Unused and untested at Wind River Systems.

AUTHOR
Written by Terry Arden of MacMillan-Bloedel Research, B.C. Canada.
*/

/* LINTLIBRARY */

#include "vxWorks.h"
#include "iv68k.h"
#include "ascu.h"

LOCAL BOOL sysAscu2 = FALSE;
LOCAL FUNCPTR usrFunc [N_USR_INTS];
LOCAL int usrArg [N_USR_INTS];
LOCAL int ascu_clk_vecnum;
LOCAL FUNCPTR ascuClkRoutine     = NULL; 	/* clock interrupt routine */
LOCAL int ascuClkArg             = NULL;	/* its argument */
LOCAL int ascuClkTicksPerSecond  = 60;		/* ascu clock rate */
LOCAL BOOL ascuClkIsConnected    = FALSE;	/* hooked up to interrupt yet?*/
LOCAL BOOL ascuClkRunning        = FALSE;	/* ascu clock enabled? */


LOCAL char *ascuErrMsg = "%s: not available\n";


/* This is a control register table for the BIM controllers
 * which reside on the ASCU2 board.
 */

LOCAL char *ascuBimCtr [] =
    { BIM3_CR1, BIM3_CR2, BIM3_CR3, BIM3_CR4,
      BIM4_CR1, BIM4_CR2, BIM4_CR3, BIM4_CR4
    };

/* This is a vector register table for the BIM controllers
 * which reside on the ASCU2 board.
 */

LOCAL char *ascuBimVec [] =
    { BIM3_VR1, BIM3_VR2, BIM3_VR3, BIM3_VR4,
      BIM4_VR1, BIM4_VR2, BIM4_VR3, BIM4_VR4
    };

/* forward declarations */

LOCAL VOID ascuUsrInt ();
LOCAL VOID ascuClkInt ();


/*******************************************************************************
*
* ascuReset - generate a sysReset on VMEbus
*/

VOID ascuReset ()

    {
    if (!sysAscu2)
	logMsg (ascuErrMsg, "ascuReset");
    else
	*PIT1_PBDR &= 0x7f;
    }
/*******************************************************************************
*
* ascuInit - initialize ASCU2
*/

VOID ascuInit ()

    {
    unsigned int wdog;
    char zero = 0;
    char probe;

    sysAscu2 = vxMemProbe (AMPCC_RSR, READ, 1, &probe) == OK;

    if (sysAscu2 && sysProcNumGet () == 0)
	{
	/* if proc 0, initialize ASCU2 board */

	/* init PI/T-1 to bit I/O mode */

	*PIT1_PGCR &= 0x3f; 	/* Set mode=0, uni-8-bit */
	*PIT1_PSRR  = 0x18;  	/* PC5 = PIRQ, PC6 = PIACK */
	*PIT1_PACR |= 0x80;	/* submode=1X, port A */
	*PIT1_PBCR |= 0x86;	/* submode=1X, port B */
				/* h3,h4 interrupt enable */

	*PIT1_PBDR  = 0xee;	/* BUS TIMER set to 32us DATA */
	*PIT1_PBDDR = 0xdf;	/* and IACK-cycle */

	wdog = 0x1000;		/* setup WATCHDOG counter */
	*PIT2_CPRL  = wdog;	/* for BCLR* Timer */
	*PIT2_CPRM  = wdog >> 8;
	*PIT2_CPRH  = wdog >> 16;

	*PIT2_PCDR  = 0xfd;	/* BCLR-mode selection */
	*PIT2_PCDDR = 0x5b;

	/* init PI/T-2 to bit I/O mode for programmable IRQ-generation 
	 * and user-defined interrupts.
	 */

	*PIT2_PGCR &= 0x3f;	/* Set mode=0, uni-8-bit */
	*PIT2_PSRR  = 0x08;	/* PC5 = PIRQ */
	*PIT2_PACR |= 0x86;	/* submode=1X, port A */
				/* h1,h2 interrupt enable */
	*PIT2_PBCR |= 0x86;	/* submode=1X, port B */
				/* h3,h4 interrupt enable */
	*PIT2_PADR  = zero;	/* port A = output */
	*PIT2_PADDR = 0xff;	/* port A = low */

	/* enable user-defined interrupts */

	*PIT1_PGCR |= 0x20;	/* enable h3,h4 */
	*PIT2_PGCR |= 0x30;	/* enable h1,h2,h3,h4 */
	}
    }
/*******************************************************************************
*
* ascuClkInit - initialize ascu clock
*/

VOID ascuClkInit(vecnum, level)
    int vecnum;
    int level;
    {
    if (!sysAscu2)
	logMsg (ascuErrMsg, "ascuClkInit");
    else
	{
	ascu_clk_vecnum = vecnum;	/* save vector */
	*PIT1_TVIR 	= vecnum;	/* interrupt vector */
	*BIM1_CR2 	= 0x30 | level;	/* ext. vec. IRQ level */

	ascuClkRateSet (ascuClkTicksPerSecond);
	}
    }
/*******************************************************************************
*
* ascuClkConnect - connect routine to ascu clock interrupt
*
* This routine connects the given function to the ascu clock interrupt.
* Ascu clock interrupts are not enabled.
*
* RETURNS: OK or ERROR if unable to connect to interrupt
*/

STATUS ascuClkConnect (routine, arg)
    FUNCPTR routine;
    int arg;

    {
    if (!sysAscu2)
	{
	logMsg (ascuErrMsg, "ascuClkConnect");
	return (ERROR);
	}
    else if (!ascuClkIsConnected &&
	    intConnect (INUM_TO_IVEC(ascu_clk_vecnum), ascuClkInt, 0) == ERROR)
	{
	return (ERROR);
	}

    ascuClkRoutine = routine;
    ascuClkArg     = arg;

    ascuClkIsConnected = TRUE;

    return (OK);
    }
/*******************************************************************************
*
* ascuClkDisable - turn off ascu clock interrupts
*/

VOID ascuClkDisable ()

    {
    if (ascuClkRunning)
	{
	/* disable the PI/T Timer */

	*PIT1_TCR = 0xa0;

	ascuClkRunning = FALSE;
	}
    }
/*******************************************************************************
*
* ascuClkEnable - turn ascu clock interrupts on
*/

VOID ascuClkEnable ()

    {
    unsigned int tc;

    if (!sysAscu2)
	logMsg (ascuErrMsg, "ascuClkEnable");
    else
	{
	/* A 5-bit prescaler (divide-by-32) is assumed to be used
	* on the PI/T clock input.
	*/

	tc = ASCU_CLK_FREQ / (ascuClkTicksPerSecond * 32);

	/* write the timer value */

	*PIT1_CPRL = tc;
	*PIT1_CPRM = tc >> 8;
	*PIT1_CPRH = tc >> 16;

	/* enable the PI/T Timer */

	*PIT1_TCR = 0xa1;
	*PIT1_TSR = 0x1;

	ascuClkRunning = TRUE;
	}
    }
/*******************************************************************************
*
* ascuClkInt - clock interrupt handler
*
* This routine handles the clock interrupt.  It is attached to the clock
* interrupt vector by the routine ascuClkConnect (2).
* The appropriate routine is called and the interrupts are acknowleged.
*/

LOCAL VOID ascuClkInt ()

    {
    if (ascuClkRoutine != NULL)
	{
	*PIT1_TCR = 0xa1;
	*PIT1_TSR = 0x1;		/* acknowledge timer interrupt */
	(* ascuClkRoutine) (ascuClkArg);	/* call ascu clock routine */
	}
    }
/*******************************************************************************
*
* ascuClkRateGet - get rate of ascu clock
*
* This routine is used to find out the ascu clock speed.
*
* RETURNS: number of ticks per second of the ascu clock
*
* SEE ALSO: ascuClkRateSet (2)
*/

int ascuClkRateGet ()
    {
    if (!sysAscu2)
	logMsg (ascuErrMsg, "ascuClkRateGet");
    else
	return (ascuClkTicksPerSecond);
    }
/*******************************************************************************
*
* ascuClkRateSet - set rate of ascu clock
*
* This routine sets the clock rate of the ascu clock.
* Ascu clock interrupts are not enabled.
*/

VOID ascuClkRateSet (ticksPerSecond)
    int ticksPerSecond;	    /* number of clock interrupts per second */
    
    {
    if (!sysAscu2)
	logMsg (ascuErrMsg, "ascuClkRateSet");
    else 
	{
	if (ticksPerSecond > 0)
	    ascuClkTicksPerSecond = ticksPerSecond;

	if (ascuClkRunning)
	    {
	    ascuClkDisable ();
	    ascuClkEnable ();
	    }
	}
    }
/*******************************************************************************
*
* ascuIntGen - generate VME bus interrupt
*
* This routine generates a VME interrupt request at the given
* IRQ level and vector.
*
* RETURNS: OK or ERROR if ASCU2 not present
*/

STATUS ascuIntGen (level, vector)
    FAST int level;
    FAST int vector;

    {
    FAST char **csr;
    FAST char **vec;
    FAST int i;
    FAST int found = FALSE;

    if (!sysAscu2) 
	{
	logMsg (ascuErrMsg, "ascuIntGen");
	return (ERROR);
	}

    /* find a free channel to use */

    while (!found)
	{
	csr = ascuBimCtr;
	vec = ascuBimVec;

	for (i = 0; i < N_PROG_CHANNELS; i++, csr++, vec++)
	    {
	    /* test-and-set flat bit of channel */
	    if (!vxTas (**csr))
		{
		found = TRUE;
		break;
		}
	    }
	/* release bus for a bit */
	taskDelay(1);
	}

    **vec = vector;		/* set vector up */
    **csr = 0xd8 | level;	/* set channel in use and generate IRQ */

    return (OK);
    }
/*******************************************************************************
*
* ascuRTCSet - set real-time clock
*
* RETURNS: OK if ASCU2 present, otherwise ERROR
*/

STATUS ascuRTCSet (pRtc)
    struct rtc *pRtc;

    {
    if (!sysAscu2)
	{
	logMsg (ascuErrMsg, "ascuSetRTC");
	return (ERROR);
	}

    *RTC_CTR_RESET	= 0xff;
    taskDelay (1);

    *RTC_CTR_1		= itobcd (pRtc->sec);
    taskDelay (1);
    *RTC_CTR_MIN	= itobcd (pRtc->mins);
    taskDelay (1);
    *RTC_CTR_HOUR	= itobcd (pRtc->hour);
    taskDelay (1);
    *RTC_CTR_DATE	= itobcd (pRtc->date);
    taskDelay (1);
    *RTC_CTR_DAY	= itobcd (pRtc->day);
    taskDelay (1);
    *RTC_CTR_MONTH	= itobcd (pRtc->month);
    taskDelay (1);
    *RTC_GO;

    return (OK);
    }
/*******************************************************************************
*
* ascuRTCGet - get time from real-time clock
*
* RETURNS: OK if ASCU2 present, otherwise ERROR
*/

STATUS ascuRTCGet (pRtc)
    struct rtc *pRtc;

    {
    if (!sysAscu2)
	{
	logMsg (ascuErrMsg, "ascuRTCGet");
	return (ERROR);
	}

    pRtc->s10000	= bcdtoi(*RTC_CTR_10000);
    taskDelay (1);
    pRtc->s100		= bcdtoi(*RTC_CTR_100);
    taskDelay (1);
    pRtc->sec		= bcdtoi(*RTC_CTR_1);
    taskDelay (1);
    pRtc->mins		= bcdtoi(*RTC_CTR_MIN);
    taskDelay (1);
    pRtc->hour		= bcdtoi(*RTC_CTR_HOUR);
    taskDelay (1);
    pRtc->date		= bcdtoi(*RTC_CTR_DATE);
    taskDelay (1);
    pRtc->day		= bcdtoi(*RTC_CTR_DAY);
    taskDelay (1);
    pRtc->month		= bcdtoi(*RTC_CTR_MONTH);

    return (OK);
    }

/* test routines */

/*******************************************************************************
*
* ascuTestConnect - enable and connect 'routine' for interrupt handling
*
*/

STATUS ascuTestConnect (vecnum, level, routine, arg)
    int vecnum,
    level;
    FUNCPTR routine;	/* routine to be called */
    int arg;		/* argument to be passed to routine */

    {
    if (!sysAscu2)
	{
	logMsg (ascuErrMsg, "ascuTestConnect");
	return (ERROR);
	}

    *BIM2_CR1  = BIM_FAC|BIM_IRE|level;
    *BIM2_VR1  = vecnum;

    return (intConnect (INUM_TO_IVEC (vecnum), routine, arg));
    }
/*******************************************************************************
*
* ascuTL - returns the state of the ASCU2 T/L switch (up = 0, down = 1)
*/

STATUS ascuTL()
    {
    return ((*PIT2_PCDR & 0x80) ? 1 : 0);
    }
/*******************************************************************************
*
* ascuUsrInit - initializes user interrupts for ASCU2
*
* This routine initializes the user interrupts to  be connected 
* by ascuUsrConnect().
*
* RETURNS:
*   OK if ASCU2 present and able to initialize, otherwise ERROR
*
* SEE ALSO: ascuUsrConnect (2)
*/

STATUS ascuUsrInit (vecnum, level)
    int vecnum;		/* vector number */
    int level;		/* interrupt level */

    {
    if (!sysAscu2)
	{
        logMsg (ascuErrMsg, "ascuUsrInit");
        return (ERROR);
	}

    /* disable user interrupts 0-5 */
    *PIT1_PBCR &= ~0x2;
    *PIT2_PACR &= ~0x6;
    *PIT2_PBCR &= ~0x6;

    *BIM1_CR1  = BIM_FAC|BIM_IRE|level;
    *BIM1_VR1  = vecnum;

    return (intConnect (INUM_TO_IVEC (vecnum), ascuUsrInt, 0));
    }
/*******************************************************************************
*
* ascuUsrConnect - connect interrupt routine to ASCU2 given user interrupt 
*
* RETURNS: OK or ERROR
*
* SEE ALSO: ascuUsrInit ()
*/

STATUS ascuUsrConnect (intNum, routine, arg)
    int intNum;		/* user interrupt number (0 thru N_USR_INTS) */
    FUNCPTR routine;	/* routine to be called */
    int arg;		/* argument to be passed to routine */

    {
    if (!sysAscu2)
	{
	logMsg (ascuErrMsg, "ascuUsrConnect");
	return (ERROR);
	}

    if (intNum < 0 || intNum >= N_USR_INTS)
	{
	logMsg ("ascuUsrConnect: invalid user interrupt %d\n", intNum);
	return (ERROR);
	}

    usrFunc [intNum] = routine;
    usrArg [intNum]  = arg;

    return (OK);
    }
/*******************************************************************************
*
* ascuUsrEnable - enable specified user interrupt
*
*/

STATUS ascuUsrEnable (intNum)
    int intNum;

    {
    int status = OK;

    if (!sysAscu2)
	{
	logMsg (ascuErrMsg, "ascuUsrEnable");
	return (ERROR);
	}

    switch (intNum)
	{
	case 0: *PIT1_PBCR |= 0x2; break;
	case 1: *PIT2_PACR |= 0x2; break;
	case 2: *PIT2_PACR |= 0x4; break;
	case 3: *PIT2_PBCR |= 0x2; break;
	case 4: *PIT2_PBCR |= 0x4; break;
	default:
		logMsg ("ascuUsrEnable: invalid user interrupt %d\n", intNum);
		status = ERROR;
		break;
	}

    return (status);
    }
/*******************************************************************************
*
* ascuUsrDisable - disable specified user interrupt
*
*/

STATUS ascuUsrDisable (intNum)
    int intNum;

    {
    int status = OK;

    if (!sysAscu2)
	{
	logMsg (ascuErrMsg, "ascuUsrDisable");
	return (ERROR);
	}

    switch(intNum)
	{
	case 0: *PIT1_PBCR &= ~0x2; break;
	case 1: *PIT2_PACR &= ~0x2; break;
	case 2: *PIT2_PACR &= ~0x4; break;
	case 3: *PIT2_PBCR &= ~0x2; break;
	case 4: *PIT2_PBCR &= ~0x4; break;
	default:
		logMsg ("ascuUsrDisable: invalid user interrupt %d\n", intNum);
		status = ERROR;
		break;
	}

    return (status);
    }
/*******************************************************************************
*
* ascuUsrInt - route user interrupt to user-defined interrupt handler
*/

LOCAL VOID ascuUsrInt ()

    {
    FAST int ix;
    FAST int mask;
    FAST int *arg = usrArg;

    /* check user interrupt handshake status to determine interrupt source */

    if (*PIT1_PSR & 0x80)
	{
	*PIT1_PSR |= 0x8;
	/* XXX huh? */
	(*usrFunc[0])(*arg++);
	}

    for (ix = 1, mask = 1; ix < N_USR_INTS; ix++, mask <<= 1)
	{
	if (*PIT2_PSR & mask)
	    {
	    *PIT2_PSR |= mask;
	    /* XXX huh? */
	    (*usrFunc[ix])(*arg++);
	    }
	}
    }

/* miscellaneous support routines */

/*******************************************************************************
*
* bcdtoi - convert Binary Coded Decimal byte to an integer
*
* This routine converts the specified BCD byte to an integer.
* Result is unspecified if bcd contains other than a two digit BCD number.
* Thus:
*
*	bcdtoi (0x12) == 12
*	bcdtoi (0x1a) == something weird
*
* RETURNS: integer equivalent of specified bcd
*/

int bcdtoi (bcd)
    UTINY bcd;		/* two BCD digits to be converted */

    {
    return ((bcd & 0x0f) + (10 * (int)((bcd >> 4) & 0x0f)));
    }
/*******************************************************************************
*
* itobcd - convert integer to Binary Coded Decimal
*
* This routine returns the BCD equivalent of the specified number.
* Result is undetermined if number not in range 0..9999.
*
* RETURNS: four digit BCD equivalent of number
*/

int itobcd (number)
    FAST short number;	/* integer number to convert */

    {
    FAST short i = 0;
    FAST short bcd = 0;

    while (number != 0)
	{
	bcd |= (number % 10) << i;

	number /= 10;
	i += 4;
	}

    return (bcd);
    }
