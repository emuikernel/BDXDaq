
/* sysMv64360Wdog.c - MV64360 Watchdog Timer Library */

/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,03jun04,cak  Initial writing.
*/

/*
DESCRIPTION

The MV64360's watchdog timer is a 32-bit count down counter that counts
down at the same rate as the sysclk.  It can be used to generate a
non-maskable interrupt, or reset the system.  After it is enabled, it is
a free running counter that needs to be serviced periodically in order
to prevent expiration.

The following routines are provided for the watchdog timer:

STATUS sysMv64360WdogStart (seconds, mode)

This routine has two input parameters: the first is the number of seconds
until watchdog expiration, and the second is the watchdog timer mode.  The
mode can be either interrupt (0) or reset (1).

STATUS sysMv64360WdogStop ()

This routine will stop the watchdog timer by disabling it.  The only way to
restart it is to call sysMv64360WdogStart() again.

STATUS sysMv64360WdogService ()

This routine will service the watchdog timer.  Servicing the watchdog
periodically will prevent it from expiring.
*/

/* includes */

#include "mv64360.h"
#include "mv6100A.h"

/* defines */

#define WDOG_MODE_INTERRUPT	0
#define WDOG_MODE_RESET		1
#undef	WDOG_DEBUG

/* typedefs */

/* globals */

/* locals */

LOCAL BOOL isWdogInitialized = FALSE;
LOCAL BOOL isWdogInterruptEnabled = FALSE;
LOCAL FUNCPTR sysMv64360WdogRoutine = NULL;
LOCAL UINT32 watchdogFrequency = 0x0;

/* forward declarations */

STATUS sysMv64360WdogConnectRoutine (FUNCPTR);
LOCAL void sysMv64360WdogEnableDisable ();
STATUS sysMv64360WdogService ();
LOCAL BOOL sysMv64360WdogIsEnabled ();
LOCAL void sysMv64360WdogInt ();
LOCAL void sysMv64360WdogIntConnect ();
LOCAL void sysMv64360WdogIntEnable ();
LOCAL void sysMv64360WdogIntDisable ();
LOCAL STATUS sysMv64360WdogInit ();
LOCAL STATUS sysMv64360WdogPrgmVal (UINT32, UINT32); 
STATUS sysMv64360WdogStart (UINT32, UINT32);
STATUS sysMv64360WdogStop ();

/*******************************************************************************
*
* sysMv64360WdogConnectRoutine - connect user specified interrupt handler 
*
* This routine connects a user specifed interrupt handler routine to be
* called by the watchdog timer's interrupt handler.
*
* RETURNS: OK if routine not already connected, ERROR otherwise.
*/

STATUS sysMv64360WdogConnectRoutine 
    (
    FUNCPTR routine	/* user-defined routine */
    )
    {
    if (sysMv64360WdogRoutine == NULL)
	{
	sysMv64360WdogRoutine = routine;
	return (OK);
	}
    else
	{
	printf("Routine already connected\n");
	return (ERROR);
	}
    }
 
/*******************************************************************************
*
* sysMv64360WdogEnableDisable - enable/disable the mv64360 watchdog timer
*
* This routine will enable or disable the mv64360 watchdog timer, dependent
* upon the current state of the watchdog timer.  A write sequence of `01'
* followed by `10' into CTL1 disables/enables the watchdog timer.
*
* RETURNS: N/A
*/

LOCAL void sysMv64360WdogEnableDisable (void)
    {
    UINT32 wdogCfgReg = 0x0;

    wdogCfgReg = MV64360_READ32 (MV64360_REG_BASE, WDOG_CFG_REG);

    /* clear CTL1 bits */

    wdogCfgReg &= ~WDOG_CFG_CTL1_MASK;

    /* enable/disable watchdog timer */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, WDOG_CFG_REG,
			  (wdogCfgReg | 0x01000000));
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, WDOG_CFG_REG,
                          (wdogCfgReg | 0x02000000));
    }

/*******************************************************************************
*
* sysMv64360WdogService - service the mv64360 watchdog timer
*
* This routine services the mv64360 watchdog timer.  A write sequence of `01'
* followed by `10' to CTL2 services the watchdog timer.
*
* RETURNS: OK always. 
*/

STATUS sysMv64360WdogService (void)
    {
    UINT32 wdogCfgReg = 0x0;

    wdogCfgReg = MV64360_READ32 (MV64360_REG_BASE, WDOG_CFG_REG);

    /* clear CTL2 bits */

    wdogCfgReg &= ~WDOG_CFG_CTL2_MASK;

    /* service the watchdog timer */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, WDOG_CFG_REG,
			  (wdogCfgReg | 0x04000000));
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, WDOG_CFG_REG,
                          (wdogCfgReg | 0x08000000));
    return (OK);
    }

/*******************************************************************************
*
* sysMv64360WdogIsEnabled - capture state of watchdog timer
*
* This routine will return the current state of the watchdog timer,
* whether it is enabled or disabled.
*
* RETURNS: True if enabled, FALSE if disabled.
*/

LOCAL BOOL sysMv64360WdogIsEnabled (void)
    {
    if (MV64360_READ32 (MV64360_REG_BASE, WDOG_CFG_REG) &
	WDOG_CFG_EN_MASK)
	return (TRUE);
    else
	return (FALSE);
    }

/*******************************************************************************
*
* sysMv64360WdogInt - watchdog timer interrupt handler
*
* This routine is the interrupt handler for the mv64360 watchdog timer.
*
* RETURNS: N/A
*/

LOCAL void sysMv64360WdogInt (void)
    {

    /*
     * Disable the watchdog timer
     *
     * This is necessary in order to stop the watchdog counter from
     * expiring and generating a reset of the entire system.
     */

    sysMv64360WdogEnableDisable();

#ifdef WDOG_DEBUG
    printf ("DEBUG: Received watchdog timer interrupt\n");
#endif /* WDOG_DEBUG */

    /* call user-specified routine */

    if (sysMv64360WdogRoutine != NULL)
        {
        (*sysMv64360WdogRoutine)();
        }
    }

/*******************************************************************************
*
* sysMv64360WdogIntConnect - connect watchdog timer interrupt
*
* This routine connects the watchdog timer interrupt handler to the
* system interrupt vector table.
*
* RETURNS: N/A
*/

LOCAL void sysMv64360WdogIntConnect (void)
    {
    intConnect (WDOG_INT_VEC, (VOIDFUNCPTR)sysMv64360WdogInt, 0);
    }

/*******************************************************************************
*
* sysMv64360WdogIntEnable - enable watchdog timer interrupt
*
* This routine enables the watchdog timer interrupt.
*
* RETURNS: N/A
*/

LOCAL void sysMv64360WdogIntEnable (void)
    {
    intEnable (WDOG_INT_LVL);
    isWdogInterruptEnabled = TRUE;
    }

/*******************************************************************************
*
* sysMv64360WdogIntDisable - disable watchdog timer interrupt
*
* This routine disables the watchdog timer interrupt.
*
* RETURNS: N/A
*/

LOCAL void sysMv64360WdogIntDisable (void)
    {
    intDisable (WDOG_INT_LVL);
    isWdogInterruptEnabled = FALSE;
    }

/*******************************************************************************
*
* sysMv64360WdogInit - watchdog timer initialization routine
*
* This routine initializes the mv64360 watchdog timer.
*
* RETURNS: OK if initialization is successful, ERROR otherwise. 
*/

LOCAL STATUS sysMv64360WdogInit (void)
    {

    /* Verify that the watchdog timer is disabled */

    if (sysMv64360WdogIsEnabled() == TRUE)
	{
	printf("Cannot initialize watchdog, timer is currently enabled\n");
	return (ERROR);
	}

    /* set MV64360 watchdog frequency to the CPU bus frequency */

    watchdogFrequency = CPU_BUS_SPD;

    /* connect watchdog timer interrupt */

    sysMv64360WdogIntConnect();
    isWdogInitialized = TRUE;
    return (OK);
    }

/*******************************************************************************
*
* sysMv64360WdogPrgmVal - configure Preset_VAL and NMI_VAL fields 
*
* This routine programs the Preset_VAL and NMI_VAL fields of the watchdog
* configuration and value registers for the given number of cycles.
*
* RETURNS: OK if able to successfully program Preset_VAL and NIM_VAL,
* ERROR otherwise.
*/

LOCAL STATUS sysMv64360WdogPrgmVal
    (
    UINT32 preset,	/* number of cycles for Preset_VAL */
    UINT32 nmi		/* number of cycles for NMI_VAL */
    ) 
    {
    UINT32 wdogCfgReg = 0x0;
    UINT32 wdogValReg = 0x0;

    /* verify arguments */

    if (preset & ~WDOG_CFG_PRESET_MASK)
	{
	printf ("preset value too large\n");
	return (ERROR);
	}

    if (nmi & ~WDOG_VAL_NMI_MASK)
	{
	printf ("nmi value too large\n");
	return (ERROR);
	}

    /* program Preset_VAL in Watchdog Configuration Register */

    wdogCfgReg = MV64360_READ32(MV64360_REG_BASE, WDOG_CFG_REG);
    wdogCfgReg &= ~(WDOG_CFG_PRESET_MASK);
    wdogCfgReg |= preset;
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, WDOG_CFG_REG, preset);

    /* Confirm preset was written to register */

    wdogCfgReg = MV64360_READ32(MV64360_REG_BASE, WDOG_CFG_REG);
    wdogCfgReg &= WDOG_CFG_PRESET_MASK;

    if (wdogCfgReg != preset)
	{
	printf("error writing Preset_VAL\n");
	return (ERROR);
	}

    /* program NMI_VAL in Watchdog Value Register */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, WDOG_VAL_REG, nmi);

    /* Confirm nmi was written to register */

    wdogValReg = MV64360_READ32(MV64360_REG_BASE, WDOG_VAL_REG);
    wdogValReg &= WDOG_VAL_NMI_MASK;

    if (wdogValReg != nmi)
	{
	printf("error writing NMI_VAL\n");
	return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* sysMv64360WdogStart - start the mv64360 watchdog timer
* 
* This routine configures (based on user input), and starts, the mv64360
* watchdog timer.
*
* RETURNS: OK if watchdog timer successfully started, ERROR otherwise. 
*/

STATUS sysMv64360WdogStart
    (
    UINT32 seconds,	/* number of seconds to watchdog expiration */
    UINT32 mode		/* watchdog timer mode, either interrupt or reset */
    )
    {
    UINT32 preset = 0x0;
    UINT32 nmi = 0x0;
    int maxSeconds = 0x0;
    UINT32 numCycles = 0x0;

    if (!isWdogInitialized)
	{
	if (sysMv64360WdogInit() == ERROR)
	    {
	    printf("Error initializing watchdog timer\n");
	    return (ERROR);
	    }
	}
    else
	{

	/* If it is already initialized, make sure that it is disabled */

	if (sysMv64360WdogIsEnabled() == TRUE)
	    {
	    printf("Error, watchdog is enabled\n");
	    return (ERROR);
	    }
	}

    /* If initialized and disabled, then it is OK to proceed */

    if (seconds == 0x0)
	{
	printf("Watchdog timer duration must be greater than 0\n");
	return (ERROR);
	}

    /* 
     * Calculate the maximum duration of the watchdog timer (in seconds)
     * based on the current watchdog timer frequency, which happens to
     * be the same as the CPU bus frequency.  Use this to validate the
     * duration requested by the user.
     */

    maxSeconds = 0xFFFFFFFF/watchdogFrequency;

    if (seconds > maxSeconds)
	{
	printf("Watchdog cannot be configured for %d seconds - duration too long for the current bus frequency of %d!\n", seconds, watchdogFrequency);
	return (ERROR);
	}
    else
	{
	numCycles = (seconds * watchdogFrequency);

	/*
	 * We can only specify 24 bits, of the 32 bits the watchdog
	 * timer is programmed to count down from - the upper 24 bits.
	 * This is specified with Preset_VAL in the watchdog configuration
	 * register.  0xFF is appended to the bottom 8 bits of the 24 bit
	 * value we specify.  Given this restriction, we want the watchdog
	 * to start counting down from a value as close to `numCycles' as
	 * possible.  In order to do this we subtract 0xFF from the 
	 * total number of cycles and then shift right by 8 bits.
	 */

	preset = ((numCycles - 0xFF) >> 8);

	/* 
	 * nmi = 0x0, ie. we will always count down to 0, regardless 
	 * of mode.  This is done because nmi is not used when using the
	 * reset mode of the watchdog timer, and because nmi must be 0
	 * when in interrupt mode in order to program the watchdog
	 * timer for the longest duration possible.
	 */

	nmi = 0x00000000;
	}

    /*
     * The watchdog timer is essentially setup the same way whether
     * we are using it in interrupt mode or reset mode.  When the 
     * watchdog timer expires both the WDNMI and WDE bits are asserted.
     * This means that the NMI# pin and WDE# pin will both be asserted.
     * So, an interrupt and a system reset are signalled at the same time.
     * This is alright when running in interrupt mode, as the interrupt
     * is handled first, and the interrupt handler can disable the 
     * watchdog timer before it has a chance to generate a system reset.
     * However, if we wish to generate a reset, we do not want to cause,
     * and then handle an interrupt first.  So when using the watchdog
     * timer in reset mode, we disable the watchdog timer interrupt.
     */

    if (mode == WDOG_MODE_INTERRUPT)
	{

	/* enable interrupts if interrupts are currently disabled */

	if (isWdogInterruptEnabled != TRUE)
	    sysMv64360WdogIntEnable();
	}
    else if (mode == WDOG_MODE_RESET)
	{

	/* disable interrupts if interrupts are currently enabled */

	if (isWdogInterruptEnabled == TRUE)
	    sysMv64360WdogIntDisable();
	}
    else
	{
	printf("Valid modes are:\n");
	printf(" 0 - interrupt mode\n");
	printf(" 1 - reset mode\n");
	printf("sysMv64360WdogStart(<seconds>,<mode>)\n");
	return (ERROR);
	}

    if (sysMv64360WdogPrgmVal(preset, nmi) == ERROR)
	{
	printf("Error starting watchdog timer\n");
	return (ERROR);
	}	

    /* Service the watchdog timer in order to load Preset_VAL */

    sysMv64360WdogService();

    /* Start the watchdog timer */

    sysMv64360WdogEnableDisable();	 

    return (OK); 
    }

/*******************************************************************************
*
* sysMv64360WdogStop - Stop the mv64360 Watchdog Timer
*
* This routine will stop the mv64360 watchdog timer.
*
* RETURNS: OK if successfully stopped, ERROR otherwise.
*/

STATUS sysMv64360WdogStop (void)
    {
    if (sysMv64360WdogIsEnabled() != TRUE)
	{
	printf("Error, watchdog not enabled\n");
	return (ERROR);
	}

    /* Disable the watchdog timer */

    sysMv64360WdogEnableDisable();	 

    /* Verify that the timer is indeed disabled */

    if (sysMv64360WdogIsEnabled() == TRUE)
	{
	printf("Error, unable to disable watchdog timer\n");
	return (ERROR);
	}
   
    /* if interrupts are enabled, disable them */

    if (isWdogInterruptEnabled == TRUE)
	sysMv64360WdogIntDisable();
 
    return (OK);
    }
