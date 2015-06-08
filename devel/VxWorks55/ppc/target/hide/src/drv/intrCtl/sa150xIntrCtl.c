/* sa150xIntrCtl.c - interrupt controller driver for ARM 1500/1501 */

/* Copyright 1998, Wind River Systems, Inc. */

/*
modification history
--------------------
01a,21sep98,cdp  created from 01a of ambaIntrCtl.c
*/

/*
This module implements the interrupt controller driver for the
Strong-ARM 1500/1501 interrupt controllers.

The interrupt controllers contained within the SA-1500 and SA-1501
chips are edge-triggered, flexible controllers for multiple interrupt
sources.  They have registers to configure the priority of interrupts
(IRQ or FIQ), an enable/disable register, a register to configure
polarity (inversion of the input signal before edge-capture), status,
request and latch set/clear registers.  This library provides the
routines to manage interrupts multiplexed by a variable number of these
controllers.

The following registers are defined in the header file for this library
(sa150xIntrCtl.h).

SA150X_INT_DESTINATION (read/write): this is the SA-1500 IPRI or the
SA-1501 INTDEST register.  When this register is written, each bit
determines whether the corresponding interrupt will be directed to the
IRQ or FIQ interrupt.  On the versions of these chips available at the
time of writing, the sense of the bits is different between the two
chips (0 configures IRQ on SA-1500 but FIQ on SA-1501).  To handle this
in a flexible way, the symbol "SA150X_INT_HANDLE_1501_DESTINATION" can
be #defined before including this source file (see below).

SA150X_INT_SPECIAL (read/write): this is the SA-1500 IDIR register
which is a test register on the SA-1501.  This is used during
configuration.  For details, see the BIU section of the SA-1500
documentation.

SA150X_INT_ENABLE (read/write): this is the SA-1500 IENAB or the
SA-1501 INTENABLE register.  When this register is written, each data
bit that is set (1) causes the corresponding interrupt to be enabled.
Each bit that is that is clear (0) causes the corresponding interrupt
to be disabled.  This register controls enabling of FIQs as well as
IRQs (the actual destination of the interrupt is controlled by
SA150X_INT_DESTINATION).

SA150X_INT_POLARITY (read/write): this is the SA-1500 IPOL or SA-1501
INTPOL register.  When this register is written, each data bit that is
set (1) causes the corresponding raw interrupt source to be inverted
before it is clocked by the edge-capture logic.  Each bit that is clear
(0) leaves the raw interrupt source unaltered before capture.  This
effectively controls whether rising or falling edges of the interrupt
source cause interrupts.  The values read from the status registers
(below) take into account what has been written to this register.

SA150X_INT_IRQ_REQUEST (read): this is the SA-1500 IRQ or SA-1501
INTRIRQ register.  When this register is read, each data bit that is
set (1) indicates an IRQ interrupt source that is both active and
enabled i.e. can interrupt the CPU.

SA150X_INT_FIQ_REQUEST (read): this is the SA-1500 FIQ or SA-1501
INTRFIQ register.  When this register is read, each data bit that is
set (1) indicates a FIQ interrupt source that is both active and
enabled i.e. can interrupt the CPU.  This register is not used by
VxWorks.

SA150X_INT_STATUS (read): this is the SA-1500 ISTAT or SA-1501
INTSTATCLR register.  When this register is read, each data bit that is
set (1) indicates that an interrupt for the corresponding source is
pending.  Each data bit that is clear (0) indicates that no such
interrupt is pending.  This is the same register as SA150X_INT_CLEAR.

SA150X_INT_CLEAR (write): this is the SA-1500 ICLR or SA-1501
INTSTATCLR register.  When this register is written, each data bit that
is set (1) clears the corresponding pending interrupt.  Each data bit
that is clear (0) has no effect.  This is the same register as
SA150X_INT_STATUS.

SA150X_INT_SOURCE (read): this is the SA-1500 ISRC or SA-1501 INTSRCSET
register.  When this register is read, each data bit that is set (1)
indicates that the corresponding interrupt is active.  Each bit that is
clear (0) indicates that the raw interrupt source is inactive.  The
values read are after the polarity configuration but before the
edge-capture logic i.e. they indicate the states of the raw sources.
This is the same register as SA150X_INT_SET.

SA150X_INT_SET (write): this is the SA-1500 ISET or SA-1501 INTSRCSET
register.  When this register is written, each data bit that is set (1)
sets the corresponding interrupt to be active.  Each bit that is clear
(0) has no effect.  This is the same register as SA150X_INT_SOURCE.

This driver uses an array of "sa150xIntrCtlDetails" to describe the
multiple controllers to be handled.  At its simplest, each element
gives the base address of the controller.  If various features are
enabled, other details are required (see below and sa150xIntrCtl.h).

The number of devices supported by this driver i.e. the number of
register blocks is specified by SA150X_INT_NUM_CONTROLLERS.  There must
be this number of entries in the sa150xIntrCtlDetails array.

The number of interrupts supported is specified by
SA150X_INT_NUM_LEVELS.  Thirty-two bits per device are assumed so
    SA150X_INT_NUM_LEVELS = 32 * SA150X_INT_NUM_CONTROLLERS.

This driver assumes that the chip is memory-mapped and does direct
memory accesses to the registers which are assumed to be 32 bits wide.
If a different access method is needed, the BSP can redefine the macros
SA150X_INT_REG_READ(addr,result) and SA150X_INT_REG_WRITE(addr,data).

This driver assumes that interrupt vector numbers are calculated and
not the result of a special cycle on the bus.  Vector numbers are
generated by adding the current interrupt bit number (bit number +
controller_number * 32) to SA150X_INT_VEC_BASE to generate a vector
number which the architecture level will use to invoke the proper
handling routine.  If a different mapping scheme, or a special hardware
routine is needed, then the BSP should redefine the macro
SA150X_INT_LVL_VEC_MAP(level,vector) to override the version defined in
the header file for this driver.

This driver was designed to support multiple devices.  For the SA-1500,
it expects one entry in the sa150xIntrCtlDetails array; for the SA-1501
(which has two sets of registers), it expects two entries.

Priorities
==========
The order of interrupt level priority is undefined at the architecture
level.  In this driver, level 0 is highest and and indicates that all
levels are disabled; level <SA150X_INT_NUM_LEVELS> is the lowest and
indicates that all levels are enabled.

This driver implements a BSP-configurable interrupt priority scheme.
The BSP should define an array of int called sa150xIntLvlPriMap, each
element of which is a bit number to be polled, where 0 is bit 0 in the
0th controller, 31 is bit 31 in the 0th controller, 32 is bit 0 in the
1st controller etc..  The list should be terminated with an entry
containing -1.  This list is used in the interrupt handler to check
bits in the requested order and is also used to generate a map of
interrupt source to new interrupt level such that whilst servicing an
interrupt, all interrupts defined by the BSP to be of lower priority
than that interrupt are disabled.  Interrupt sources not in the list
are serviced after all others with the least-significant bit being
highest priority.  (Note that the list is a list of ints rather than
bytes because it causes the current compiler to generate faster code.)

Note that in this priority system, intLevelSet(n) does not necessarily
disable interrupt bit n and all lower-priority ones but uses
sa150xIntLvlPriMap to determine which interrupts should be masked e.g.
if sa150xIntLvlPriMap[] contains { 9, 4, 8, 5, -1 }, intLevelSet(0)
disables all interrupt bits; intLevelSet(1) enables interrupt bit 9 but
disables interrupts 4, 8, 5 and all others not listed; intLevelSet(3)
enables interrupt bits 9, 4 and 8 but disables all others.  This
enabling of interrupts only occurs if the interrupt has been explicitly
enabled via a call to sa150xIntLvlEnable().

If the list is empty (contains just a terminator) or sa150xIntLvlPriMap
is declared as an int pointer of value 0 (this is more efficient),
interrupts are handled as least-significant bit is highest priority.

FIQs
====
Because the SA150X_INT_ENABLE register controls the enabling of FIQ as
well as IRQ (see above), individual FIQs *must* be enabled/disabled by
calling the routines in this file (sa150xIntFiqEnable and
sa150xIntFiqDisable) rather than by direct accesses to the register.
Otherwise, each IRQ will cause FIQs previously enabled to be disabled.
In other respects, this driver (and VxWorks) do not support FIQ (see
the ARM Architecture Supplement).

Special features
================
This driver can be compiled with certain features enabled by predefining
certain symbols (e.g. in bsp.h or config.h).  Some have been described above
but here is the complete list.

#define SA150X_INT_INITIALISE_SOURCES
Configures the driver so that the sa150xIntDevInit() call sets all interrupt
sources to IRQ, disables and clears them all and sets their polarity.

#define	SA150X_INT_HANDLE_1501_DESTINATION
Configures the driver to handle the different meaning of bits within
the DESTINATION register on different SA-150X chips.  If this symbol is
defined, each element of the sa150xIntrCtlDetails array must have an
extra field to modify the value written to the DESTINATION register
(the value specified in the structure is exclusive ORred with the value
to be written to the register).

#define	SA150X_INT_RETRY_READS
Configures the driver to retry reads from the controller, where safe,
to attempt to cope with corruption of values read.

#define	SA150X_INT_RETRY_WRITES
Configures the driver to retry writes to the controller, where safe, to
attempt to cope with corruption of values written.

#define SA150X_INT_CACHE_IRQ_REQUEST
Configures the driver to cache the interrupt request registers i.e. for
each interrupt request, the IRQ_REQUEST registers will only be read
once.  Depending on the speed of RAM versus I/O, it is possible that
this might improve performance but it was originally added to improve
reliability on some systems.


The BSP will initialize this driver in sysHwInit2(), after initializing
the main interrupt library, usually intLibInit().  The initialization
routine, sa150xIntDevInit() will setup the interrupt controller device,
it will mask off all individual interrupt sources and then set the
interrupt level to enable all interrupts.  See sa150xIntDevInit for
more information.

All of the functions in this library are global.  This allows them to
be used by the BSP if it is necessary to create wrapper routines or to
incorporate several drivers together as one.
*/

#include "vxWorks.h"
#include "config.h"
#include "intLib.h"
#include "drv/intrCtl/sa150xIntrCtl.h"


/* imports */

IMPORT int ffsLsb(UINT32);


/* Defines from config.h, or <bsp>.h */

#if !defined(SA150X_INT_NUM_LEVELS) || !defined(SA150X_INT_NUM_CONTROLLERS)
#error missing definitions for sa150xIntrCtl.c
#endif

#if (SA150X_INT_NUM_LEVELS != (SA150X_INT_NUM_CONTROLLERS * 32))
#error number of sa150x interrupt levels/controllers incompatible
#endif

#define SA150X_INT_VEC_BASE	(0x0)


/* driver constants */

#define SA150X_INT_ALL_ENABLED	(SA150X_INT_NUM_LEVELS)
#define SA150X_INT_ALL_DISABLED	0


/* Local data */

/* Current interrupt level setting (sa150xIntLvlChg). */

LOCAL UINT32 sa150xIntLvlCurrent = SA150X_INT_ALL_DISABLED;


/*
 * Controller masks: for each interrupt level, this provides a mask for
 * each controller (see IntLvlChg).
 * Mask is 32 bits/controller * (levels + 1)
 */

LOCAL UINT32 sa150xIntLvlMask[SA150X_INT_NUM_CONTROLLERS *
			      (1 + SA150X_INT_NUM_LEVELS) *
			      sizeof(UINT32)];


/*
 * Map of interrupt bit number to level: if bit n is set,
 * sa150xIntLvlMap[n] is the interrupt level to change to such that
 * interrupt n and all lower priority ones are disabled.
 */

LOCAL int sa150xIntLvlMap[SA150X_INT_NUM_LEVELS * sizeof(int)];


/* forward declarations */

STATUS	sa150xIntLvlVecChk (int*, int*);
STATUS  sa150xIntLvlVecAck (int, int);
int	sa150xIntLvlChg (int);
STATUS	sa150xIntLvlEnable (int);
STATUS	sa150xIntLvlDisable (int);

/*******************************************************************************
*
* sa150xIntDevInit - initialize the interrupt controller
*
* This routine will initialize the interrupt controller device,
* disabling all interrupt sources.  It will also connect the device
* driver specific routines into the architecture level hooks.  If the BSP
* needs to create a wrapper routine around any of the architecture level
* routines, it should install the pointer to the wrapper routine after
* calling this routine.
*
* Before this routine is called, sa150xIntLvlPriMap should be initialised
* as a list of interrupt bits to poll in order of decreasing priority and
* terminated by <-1>.  If sa150xIntLvlPriMap is a null pointer or an
* empty list, the priority scheme used will be least-significant bit
* first.
*
* The return value ERROR indicates that the contents of
* sa150xIntLvlPriMap were invalid.
*
* RETURNS: OK or ERROR if sa150xIntLvlPriMap invalid.
*/

int sa150xIntDevInit (void)
    {
    int i, j;
    int level, ctl;
    UINT32 bit;
    UINT32 val;
    sa150xIntrCtlDetails *p;


    /* if priorities are supplied, validate the supplied list */

    if (sa150xIntLvlPriMap != 0)
	{
	/* first check the list is terminated (VecChk requires this) */

	for (i = 0; i < SA150X_INT_NUM_LEVELS; ++i)
	    if (sa150xIntLvlPriMap[i] == -1)
		break;

	if (!(i < SA150X_INT_NUM_LEVELS))
	    return ERROR;	/* no terminator */


	/* now check that all are in range and that there are no duplicates */

	for (i = 0; sa150xIntLvlPriMap[i] != -1; ++i)
	    if (sa150xIntLvlPriMap[i] < 0 ||
		sa150xIntLvlPriMap[i] >= SA150X_INT_NUM_LEVELS)
		{
		return ERROR;	/* out of range */
		}
	    else
		for (j = i + 1; sa150xIntLvlPriMap[j] != -1; ++j)
		    if (sa150xIntLvlPriMap[j] == sa150xIntLvlPriMap[i])
			{
			return ERROR;	/* duplicate */
			}
	}


    /*
     * Now initialise the mask array.
     * For each level (in ascending order), the mask is the mask of the
     * previous level with the bit for the current level set to enable it.
     */

    for (ctl = 0; ctl < SA150X_INT_NUM_CONTROLLERS; ++ctl)
	sa150xIntLvlMask[ctl] = 0;	/* mask for level 0 = all disabled */


    /* do the levels for which priority has been specified */

    level = 1;
    if (sa150xIntLvlPriMap != 0)
	{
	for ( ; level <= SA150X_INT_NUM_LEVELS &&
		(i = sa150xIntLvlPriMap[level - 1], i >= 0); ++level)
	    {
	    /* copy previous level's mask to this one's */

	    for (ctl = 0; ctl < SA150X_INT_NUM_CONTROLLERS; ++ctl)
		sa150xIntLvlMask[
		    level * SA150X_INT_NUM_CONTROLLERS + ctl] =
			sa150xIntLvlMask[
			    (level - 1) * SA150X_INT_NUM_CONTROLLERS + ctl];


	    /* OR in the bit indicated by the next entry in PriMap[] */

	    ctl = i / 32;
	    bit = 1 << (i % 32);
	    sa150xIntLvlMask[level * SA150X_INT_NUM_CONTROLLERS + ctl] |= bit;

	    /*
	     * set index in level map: to disable this interrupt and all
	     * lower-priority ones, select the level one less than this
	     */

	    sa150xIntLvlMap[i] = level - 1;
	    }
	}


    /* do the rest of the levels */

    i = 0;		/* lowest-numbered interrupt bit */

    for ( ; level <= SA150X_INT_NUM_LEVELS; ++level)
	{
	/* copy previous level's mask to this one's */

	for (ctl = 0; ctl < SA150X_INT_NUM_CONTROLLERS; ++ctl)
	    sa150xIntLvlMask[
		level * SA150X_INT_NUM_CONTROLLERS + ctl] =
		    sa150xIntLvlMask[
			(level - 1) * SA150X_INT_NUM_CONTROLLERS + ctl];


	/* try to find a bit that has not yet been set */

	for ( ; ; ++i)
	    {
	    ctl = i / 32;
	    bit = 1 << (i % 32);

	    if ((sa150xIntLvlMask[level * SA150X_INT_NUM_CONTROLLERS + ctl] &
		 bit) == 0)
		{
		/* this bit not set so put it in the mask */

		sa150xIntLvlMask[level * SA150X_INT_NUM_CONTROLLERS + ctl] |=
									    bit;

		/*
		 * set index in level map: to disable this interrupt and all
		 * lower-priority ones, select the level one less than this
		 */

		sa150xIntLvlMap[i] = level - 1;
		break;
		}
	    }
	}


    /* install the driver routines in the architecture hooks */

    sysIntLvlVecChkRtn	= sa150xIntLvlVecChk;
    sysIntLvlVecAckRtn	= sa150xIntLvlVecAck;
    sysIntLvlChgRtn	= sa150xIntLvlChg;
    sysIntLvlEnableRtn	= sa150xIntLvlEnable;
    sysIntLvlDisableRtn	= sa150xIntLvlDisable;


    /* set up the controllers */

    for (i = 0; i < SA150X_INT_NUM_CONTROLLERS; ++i)
	{
	p = &sa150xIntrCtl[i];

#ifdef SA150X_INT_INITIALISE_SOURCES
	/* disable all sources */

#ifdef SA150X_INT_RETRY_WRITES
	SA150X_INT_REG_WRITE_RETRY (SA150X_INT_ENABLE (p->base), 0);
#else
	SA150X_INT_REG_WRITE (SA150X_INT_ENABLE (p->base), 0);
#endif


	/* set all sources to IRQ - note: must not retry write (locks up) */

	val = 0;
#ifdef SA150X_INT_HANDLE_1501_DESTINATION
	val ^= p->destModifier;
#endif
	SA150X_INT_REG_WRITE (SA150X_INT_DESTINATION (p->base), val);


	/* set polarity of edge-triggered interrupt to positive */

#ifdef SA150X_INT_RETRY_WRITES
	SA150X_INT_REG_WRITE_RETRY (SA150X_INT_POLARITY (p->base), 0);
#else
	SA150X_INT_REG_WRITE (SA150X_INT_POLARITY (p->base), 0);
#endif


	/* clear all latches */

	SA150X_INT_REG_WRITE (SA150X_INT_CLEAR (p->base), ~0);
#endif	/* SA150X_INT_INITIALISE_SOURCES */

	p->enabledIrqs = 0;			/* all sources disabled */
	}

    sa150xIntLvlChg (SA150X_INT_ALL_ENABLED);	/* enable all levels */

    return OK;
    }

/*******************************************************************************
*
* sa150xIntLvlConfigure - configure polarity and type of interrupt
*
* This routine configures the polarity and type (edge or level) or an
* interrupt.  It should be called after initialisation of the driver and
* before enabling the level.
*
* The return value ERROR indicates that the parameters were invalid.
*
* RETURNS: OK or ERROR if parameters invalid.
*/

STATUS sa150xIntLvlConfigure
    (
    int level,		/* level to configure */
    int features	/* edge/level/positive/negative */
    )
    {
    int key;
    int bit;
    UINT32 val;
    sa150xIntrCtlDetails *p;


    if (level < 0 ||
	level >= SA150X_INT_NUM_LEVELS)
	return ERROR;

    bit = 1 << (level % 32);
    p = &sa150xIntrCtl[level / 32];

    key = intIFLock ();		/* disable IRQ and FIQ */


    /* configure destination - note: must not retry write (locks up) */

    SA150X_INT_REG_READ (SA150X_INT_DESTINATION (p->base), val);
    val &= ~bit;		/* clear bit for this interrupt */

#ifdef SA150X_INT_HANDLE_1501_DESTINATION
    if (features & SA150X_INT_DESTINATION_FIQ)
	val |= bit & (bit ^ p->destModifier);
    else
	val |= bit & (0 ^ p->destModifier);
#else
    if (features & SA150X_INT_DESTINATION_FIQ)
	val |= bit;
#endif

    SA150X_INT_REG_WRITE (SA150X_INT_DESTINATION (p->base), val);


    /* configure polarity */

    SA150X_INT_REG_READ (SA150X_INT_POLARITY (p->base), val);
    if (features & SA150X_INT_POLARITY_POSITIVE)
	val &= ~bit;
    else
	val |= bit;
#ifdef SA150X_INT_RETRY_WRITES
    SA150X_INT_REG_WRITE_RETRY (SA150X_INT_POLARITY (p->base), val);
#else
    SA150X_INT_REG_WRITE (SA150X_INT_POLARITY (p->base), val);
#endif

    intIFUnlock (key);		/* restore IRQ and FIQ */


    /* clear any pending interrupt */

    SA150X_INT_REG_WRITE (SA150X_INT_CLEAR (p->base), bit);


    /*
     * if level interrupt, and source register (which takes account of
     * polarity) indicates that interrupt is still active, set request
     */

    if (features & SA150X_INT_TYPE_LEVEL)
	{
	SA150X_INT_REG_READ (SA150X_INT_SOURCE (p->base), val);
	if (val & bit)
	    SA150X_INT_REG_WRITE (SA150X_INT_SET (p->base), bit);
	}


    return OK;
    }

/*******************************************************************************
*
* sa150xIntLvlVecChk - check for and return any pending interrupts
*
* This routine interrogates the hardware to determine the highest priority
* interrupt pending.  It returns the vector associated with that interrupt, and
* also the interrupt priority level prior to the interrupt (not the
* level of the interrupt).  The current interrupt priority level is then
* raised to the level of the current interrupt so that only higher priority
* interrupts will be accepted until this interrupt is finished.
*
* This routine must be called with CPU interrupts disabled.
*
* The return value ERROR indicates that no pending interrupt was found and
* that the level and vector values were not returned.
*
* RETURNS: OK or ERROR if no interrupt is pending.
*/

STATUS  sa150xIntLvlVecChk
    (
    int* pLevel,  /* ptr to receive old interrupt level */
    int* pVector  /* ptr to receive current interrupt vector */
    )
    {
    int newLevel;
    sa150xIntrCtlDetails *p;
    UINT32 isr, bit;
    UINT32 *priPtr;
    int ctl;
    int bitNum;
#ifdef SA150X_INT_RETRY_READS
    int ok;
#define TRIES 2
#endif

    /* initialise vars to stop compiler warnings */

    p = 0;
    bit = 0;


    SA150X_INT_DEBUG(0x00, 0);

#ifdef SA150X_INT_CACHE_IRQ_REQUEST
    /* mark all cached requests as invalid */

    for (ctl = 0; ctl < SA150X_INT_NUM_CONTROLLERS; ++ctl)
	sa150xIntrCtl[ctl].requestsValid = FALSE;
#endif


    if (priPtr = sa150xIntLvlPriMap, priPtr == 0)
	bitNum = -1;
    else
	{
	/* service interrupts according to priority specified in map */

	while (bitNum = *priPtr++, bitNum != -1)
	    {
	    /*
	     * bitNum = interrupt bit from priority map - convert it to
	     * a bit position in a specific controller
	     * Note: UINT32 cast of bitNum is an optimisation(!)
	     */

	    ctl = (UINT32)bitNum / 32;
	    bit = 1 << ((UINT32)bitNum % 32);
	    p = &sa150xIntrCtl[ctl];

#ifdef SA150X_INT_CACHE_IRQ_REQUEST
	    if (p->requestsValid)
		{
		isr = p->irqRequests;
		SA150X_INT_DEBUG(0x81, ctl);
		}
	    else
		{
#endif
		/* read pending interrupt register for this controller */

		SA150X_INT_DEBUG(0x80, ctl);
		SA150X_INT_REQ_REG_READ (SA150X_INT_IRQ_REQUEST (p->base), isr,
					 ~p->enabledIrqs, TRIES, ok);

#ifdef SA150X_INT_RETRY_READS
		if (ok)
#endif
#ifdef SA150X_INT_CACHE_IRQ_REQUEST
		    {
		    p->requestsValid = TRUE;
		    p->irqRequests = isr;
		    }
#ifdef SA150X_INT_RETRY_READS
		else
		    {
		    isr = 0;
		    SA150X_INT_DEBUG(0x83, bitNum);
		    }
#endif
		}
#endif	/* SA150X_INT_CACHE_IRQ_REQUEST */

	    if (isr & bit)
		{
		SA150X_INT_DEBUG(0x82, bitNum);
		break;
		}
	    }
	}


    /*
     * If priority scan didn't find anything, look for any bit set in any
     * controller, starting with the lowest-numbered bit
     */

    if (bitNum == -1)
	for (ctl = 0; ctl < SA150X_INT_NUM_CONTROLLERS; ++ctl)
	    {
	    p = &sa150xIntrCtl[ctl];

#ifdef SA150X_INT_CACHE_IRQ_REQUEST
	    if (p->requestsValid)
		{
		isr = p->irqRequests;
		SA150X_INT_DEBUG(0x91, ctl);
		}
	    else
		{
#endif
		/* read pending interrupt register for this controller */

		SA150X_INT_DEBUG(0x90, ctl);
		SA150X_INT_REQ_REG_READ (SA150X_INT_IRQ_REQUEST (p->base), isr,
					 ~p->enabledIrqs, TRIES, ok);

#ifdef SA150X_INT_RETRY_READS
		if (ok)
#endif
#ifdef SA150X_INT_CACHE_IRQ_REQUEST
		    {
		    p->requestsValid = TRUE;
		    p->irqRequests = isr;
		    }
#ifdef SA150X_INT_RETRY_READS
		else
		    {
		    isr = 0;
		    SA150X_INT_DEBUG(0x93, ctl);
		    }
#endif
		}
#endif	/* SA150X_INT_CACHE_IRQ_REQUEST */

	    /* find lowest bit set */

	    bitNum = ffsLsb (isr) - 1;	/* ffsLsb returns 1..n, not 0..n-1 */
	    if (bitNum != -1)
		{
		bit = 1 << bitNum;
		bitNum += ctl * 32;
		SA150X_INT_DEBUG(0x92, bitNum);
		break;
		}
	    }


    /* If no interrupt is pending, return ERROR */

    if (bitNum == -1)
	{
	SA150X_INT_DEBUG(0xEE, 0xEE);
	return ERROR;
	}

    /*
     * bitNum is the bit that is interrupting (0..SA150X_INT_NUM_LEVELS-1)
     * ctl is the controller number (0..SA150X_INT_NUM_CONTROLLERS-1)
     * bit is the bit within that controller
     * p-> sa150xIntrCtl structure for this controller
     *
     * clear the interrupt
     */

    SA150X_INT_REG_WRITE (SA150X_INT_CLEAR (p->base), bit);


    /* map the interrupting device to an interrupt level number */

    newLevel = sa150xIntLvlMap[bitNum];


    /* change to new interrupt level, returning previous level to caller */

    *pLevel = sa150xIntLvlChg (newLevel);


    /* fetch, or compute the interrupt vector number */

    SA150X_INT_LVL_VEC_MAP (bitNum, *pVector);

    SA150X_INT_DEBUG(0xEE, 0x00);
    return OK;
    }

/*******************************************************************************
*
* sa150xIntLvlVecAck - acknowledge the current interrupt
*
* Acknowledge the current interrupt cycle.  The level and vector values are
* those generated during the sa150xIntLvlVecChk() routine for this interrupt
* cycle.  The basic action is to return the interrupt level to its previous
* setting.  Note that the SA150X interrupt controller does not need an
* acknowledge cycle and that the interrupt has already been cleared in
* sa150xIntLvlVecChk().
*
* RETURNS: OK or ERROR if a hardware fault is detected.
* ARGSUSED
*/

STATUS  sa150xIntLvlVecAck
    (
    int level,	/* old interrupt level to be restored */
    int vector	/* current interrupt vector, if needed */
    )
    {
    SA150X_INT_DEBUG(0xFF, vector);

    /* restore the previous interrupt level */

    sa150xIntLvlChg (level);

    return OK;
    }

/*******************************************************************************
*
* sa150xIntLvlChg - change the interrupt level value
*
* This routine implements the overall interrupt setting.  All levels
* up to and including the specified level are disabled.  All levels above
* the specified level will be enabled, but only if they were specifically
* enabled by the sa150xIntLvlEnable() routine.
*
* The specific priority level SA150X_INT_NUM_LEVELS is valid and represents
* all levels enabled.
*
* RETURNS: Previous interrupt level.
*/

int  sa150xIntLvlChg
    (
    int level	/* new interrupt level */
    )
    {
    int oldLevel;
    int i, m;
    sa150xIntrCtlDetails *p;
    int key;

    oldLevel = sa150xIntLvlCurrent;

    if (level >= 0 &&
        level <= SA150X_INT_NUM_LEVELS)
	{
	/* change current interrupt level */

	sa150xIntLvlCurrent = level;
	}


    /* Activate the enabled interrupts */

    m = sa150xIntLvlCurrent * SA150X_INT_NUM_CONTROLLERS;
    for (i = 0; i < SA150X_INT_NUM_CONTROLLERS; ++i)
	{
	p = &sa150xIntrCtl[i];
	key = intIFLock ();		/* disable IRQ and FIQ */
#ifdef SA150X_INT_RETRY_WRITES
	SA150X_INT_REG_WRITE_RETRY (SA150X_INT_ENABLE (p->base),
				    (sa150xIntLvlMask[m + i] & p->enabledIrqs) |
				    p->enabledFiqs);
#else
	SA150X_INT_REG_WRITE (SA150X_INT_ENABLE (p->base),
			      (sa150xIntLvlMask[m + i] & p->enabledIrqs) |
			      p->enabledFiqs);
#endif
	intIFUnlock (key);
	}

    return oldLevel;
    }

/*******************************************************************************
*
* sa150xIntLvlEnable - enable a single interrupt level
*
* Enable a specific interrupt level.  The enabled level will be allowed
* to generate an interrupt when the overall interrupt level is set to
* enable interrupts of this priority (as configured by
* sa150xIntLvlPriMap).  Without being enabled, the interrupt is blocked
* regardless of the overall interrupt level setting.
*
* RETURNS: OK or ERROR if the specified level cannot be enabled.
*/

STATUS  sa150xIntLvlEnable
    (
    int level  /* level to be enabled */
    )
    {
    int key;

    if (level < 0 ||
	level >= SA150X_INT_NUM_LEVELS)
	return ERROR;


    /* set bit in enable mask */

    key = intIFLock ();		/* disable IRQ and FIQ */
    sa150xIntrCtl[level / 32].enabledIrqs |= (1 << (level % 32));
    intIFUnlock (key);

    sa150xIntLvlChg (-1);	/* reset current mask */

    return OK;
    }

/*******************************************************************************
*
* sa150xIntLvlDisable - disable a single interrupt level
*
* Disable a specific interrupt level.  The disabled level is prevented
* from generating an interrupt even if the overall interrupt level is set
* to enable interrupts of this priority (as configured by
* sa150xIntLvlPriMap).
*
* RETURNS: OK or ERROR, if the specified interrupt level cannot be disabled.
*/

STATUS  sa150xIntLvlDisable
    (
    int level  /* level to be disabled */
    )
    {
    int key;

    if (level < 0 ||
	level >= SA150X_INT_NUM_LEVELS)
	return ERROR;


    /* clear bit in enable mask */

    key = intIFLock ();		/* disable IRQ and FIQ */
    sa150xIntrCtl[level / 32].enabledIrqs &= ~(1 << (level % 32));
    intIFUnlock (key);

    sa150xIntLvlChg (-1);	/* reset current mask */

    return OK;
    }

/*******************************************************************************
*
* sa150xIntFiqEnable - enable a single FIQ interrupt
*
* Enable a specific FIQ interrupt. Unlike an enable IRQ interrupt, an
* enabled FIQ interrupt will be always be allowed to interrupt,
* irrespective of the overall interrupt level.
*
* RETURNS: OK or ERROR if the specified level cannot be enabled.
*/

STATUS  sa150xIntFiqEnable
    (
    int level  /* level to be enabled */
    )
    {
    int key;

    if (level < 0 ||
	level >= SA150X_INT_NUM_LEVELS)
	return ERROR;


    /* set bit in enable mask */

    key = intIFLock ();		/* disable IRQ and FIQ */
    sa150xIntrCtl[level / 32].enabledFiqs |= (1 << (level % 32));
    intIFUnlock (key);

    sa150xIntLvlChg (-1);	/* reset current mask */

    return OK;
    }

/*******************************************************************************
*
* sa150xIntFiqDisable - disable a single FIQ interrupt
*
* Disable a specific FIQ interrupt.  The disabled level is prevented
* from generating an interrupt.
*
* RETURNS: OK or ERROR, if the specified interrupt level cannot be disabled.
*/

STATUS  sa150xIntFiqDisable
    (
    int level  /* level to be disabled */
    )
    {
    int key;

    if (level < 0 ||
	level >= SA150X_INT_NUM_LEVELS)
	return ERROR;


    /* clear bit in enable mask */

    key = intIFLock ();		/* disable IRQ and FIQ */
    sa150xIntrCtl[level / 32].enabledFiqs &= ~(1 << (level % 32));
    intIFUnlock (key);

    sa150xIntLvlChg (-1);	/* reset current mask */

    return OK;
    }
