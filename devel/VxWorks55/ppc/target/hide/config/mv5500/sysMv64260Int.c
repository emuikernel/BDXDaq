/* sysMv64260Int.c - Interrupt controller support functions */

/* Copyright 2002 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01c,28feb03,scb GPP int handler checks "val" register instead of "cause" reg,
		this reduces "Main interrupt with no cause" under heavy load.
01b,25oct02,scb Minor tweeks to be consistent with MOTLoad.
01a,01oct02,scb Copied from hxeb100 base (ver 01d).
*/

/*
DESCRIPTION
This module implements the Mv64260 interrupt handling support functions.
*/

/* includes */

#include "mv64260.h"
#include "mv64260Int.h"

/* defines */

#define	SYSINT_TABLE_SZ		256

#define	LO			0
#define	HI			1

/* globals */

IMPORT STATUS	    (*_func_intConnectRtn) (VOIDFUNCPTR *, VOIDFUNCPTR, int);
IMPORT int	    (*_func_intEnableRtn)  (int);
IMPORT int	    (*_func_intDisableRtn) (int);
IMPORT STATUS	    excIntConnect (VOIDFUNCPTR *, VOIDFUNCPTR);

INT_HANDLER_DESC    *sysIntTbl [SYSINT_TABLE_SZ];  /* system interrupts */

/* locals */

/*
 * This table contains all of the static interrupt controller initialization
 * values for the MV64260 system controller chip.
 */

LOCAL UINT32 mv64260IntTbl [] =
    {
    NOT_INIT_MV64260_REG ( ICI_MAIN_INT_CAUSE_LO	  , readOnly )
    NOT_INIT_MV64260_REG ( ICI_MAIN_INT_CAUSE_HI	  , readOnly )

    YES_INIT_MV64260_REG ( ICI_CPU_INT_MASK_LO		  , MASK_ALL )
    YES_INIT_MV64260_REG ( ICI_CPU_INT_MASK_HI		  , MASK_ALL )
    NOT_INIT_MV64260_REG ( ICI_CPU_SEL_CAUSE	  	  , readOnly )

    /* The following are not used */

    YES_INIT_MV64260_REG ( ICI_PCI0_INT_MASK_LO		  , MASK_ALL )
    YES_INIT_MV64260_REG ( ICI_PCI0_INT_MASK_HI		  , MASK_ALL )
    NOT_INIT_MV64260_REG ( ICI_PCI0_SEL_CAUSE		  , readOnly )

    /* The following are not used */

    YES_INIT_MV64260_REG ( ICI_PCI1_INT_MASK_LO		  , MASK_ALL )
    YES_INIT_MV64260_REG ( ICI_PCI1_INT_MASK_HI		  , MASK_ALL )
    NOT_INIT_MV64260_REG ( ICI_PCI1_SEL_CAUSE		  , readOnly )

    /* The following are not used */

    YES_INIT_MV64260_REG ( ICI_CPU_INT0_MASK		  , MASK_ALL )
    YES_INIT_MV64260_REG ( ICI_CPU_INT1_MASK		  , MASK_ALL )
    YES_INIT_MV64260_REG ( ICI_CPU_INT2_MASK		  , MASK_ALL )
    YES_INIT_MV64260_REG ( ICI_CPU_INT3_MASK		  , MASK_ALL )

    /*
     * Each of the 32 4-bit nibbles in the MPP_CTRLx registers represents
     * a multi-purpose pin selection value.  Every pin needs to be programmed
     * as a GPP[x] (where x is pin number).  With the exception of MPP_CTRL3
     * pint 24, 25, and 26, GPP[x] nibble programming value
     * is zero so all of these registers will be programmed to zero.
     */

    YES_INIT_MV64260_REG ( MPP_CTRL0, 0 )
    YES_INIT_MV64260_REG ( MPP_CTRL1, 0 )
    YES_INIT_MV64260_REG ( MPP_CTRL2, 0 )

    /*
     * MPP_CTRL3: pin 24, 9 = WDNMI#
     *            pin 25, 9 = WDE#
     *		  pin 26, 5 = IntAct (SROM init active)
     */

    YES_INIT_MV64260_REG ( MPP_CTRL3, 0x00000599 )

    /*
     * The following serial ports multiplex register will be programmed
     * to the default value except for bits 3:0 which are set to '1' to
     * indicate "Connected to Ethernet controller 0 MII interface."
     */

    YES_INIT_MV64260_REG ( MPP_SERIAL_PORT_MPLEX, 1)

    YES_INIT_MV64260_REG ( GPP_INT_MASK 		  , MASK_ALL )
    YES_INIT_MV64260_REG ( GPP_IO_CTRL, (GPP_IO0 << 0)   | (GPP_IO1  << 1)  | \
					(GPP_IO2 << 2)   | (GPP_IO3  << 3)  | \
					(GPP_IO4 << 4)   | (GPP_IO5  << 5)  | \
					(GPP_IO6  << 6)  | (GPP_IO7  << 7)  | \
					(GPP_IO8  << 8)  | (GPP_IO9  << 9)  | \
					(GPP_IO10 << 10) | (GPP_IO11 << 11) | \
					(GPP_IO12 << 12) | (GPP_IO13 << 13) | \
					(GPP_IO14 << 14) | (GPP_IO15 << 15) | \
					(GPP_IO16 << 16) | (GPP_IO17 << 17) | \
					(GPP_IO18 << 18) | (GPP_IO19 << 19) | \
					(GPP_IO20 << 20) | (GPP_IO21 << 21) | \
					(GPP_IO22 << 22) | (GPP_IO23 << 23) | \
					(GPP_IO24 << 24) | (GPP_IO25 << 25) | \
					(GPP_IO26 << 26) | (GPP_IO27 << 27) | \
					(GPP_IO28 << 28) | (GPP_IO29 << 29) | \
					(GPP_IO30 << 30) | (GPP_IO31 << 31) )

    YES_INIT_MV64260_REG ( GPP_LVL_CTRL, (GPP_LEVEL0 << 0) | \
					 (GPP_LEVEL1 << 1) | \
					 (GPP_LEVEL2 << 2) | \
					 (GPP_LEVEL3 << 3) | \
					 (GPP_LEVEL4 << 4) | \
					 (GPP_LEVEL5 << 5) | \
					 (GPP_LEVEL6 << 6) | \
					 (GPP_LEVEL7 << 7) | \
					 (GPP_LEVEL8 << 8) | \
					 (GPP_LEVEL9 << 9) | \
					 (GPP_LEVEL10 << 10) | \
					 (GPP_LEVEL11 << 11) | \
					 (GPP_LEVEL12 << 12) | \
					 (GPP_LEVEL13 << 13) | \
					 (GPP_LEVEL14 << 14) | \
					 (GPP_LEVEL15 << 15) | \
					 (GPP_LEVEL16 << 16) | \
					 (GPP_LEVEL17 << 17) | \
					 (GPP_LEVEL18 << 18) | \
					 (GPP_LEVEL19 << 19) | \
					 (GPP_LEVEL20 << 20) | \
					 (GPP_LEVEL21 << 21) | \
					 (GPP_LEVEL22 << 22) | \
					 (GPP_LEVEL23 << 23) | \
					 (GPP_LEVEL24 << 24) | \
					 (GPP_LEVEL25 << 25) | \
					 (GPP_LEVEL26 << 26) | \
					 (GPP_LEVEL27 << 27) | \
					 (GPP_LEVEL28 << 28) | \
					 (GPP_LEVEL29 << 29) | \
					 (GPP_LEVEL30 << 30) | \
					 (GPP_LEVEL31 << 31) )

    YES_INIT_MV64260_REG ( GPP_VAL			  , 0 )
    YES_INIT_MV64260_REG ( GPP_INT_CAUSE		  , 0 )

    END_INIT_MV64260_REG
    };

/* forward declarations	*/

LOCAL STATUS  sysMv64260IntConnect (VOIDFUNCPTR	*vector,
				    VOIDFUNCPTR routine, int parameter);

void  sysMv64260IntInit	   (void);
LOCAL int     sysMv64260IntEnable  (int);
LOCAL int     sysMv64260IntDisable (int);
LOCAL int     iciIntEnable	   (int);
LOCAL int     iciIntDisable	   (int);
LOCAL int     gppIntEnable	   (int);
LOCAL int     gppIntDisable	   (int);

LOCAL void    sysMv64260IntHandler (void);
LOCAL void    gppIntHandler	   (int);

/* Local xxxintEnable()/xxxintDisable() functions */

LOCAL INT_UTIL_DESC intUtil [] = {
				     { INT_UTIL_ICI },
				     { INT_UTIL_GPP }
				       INT_UTIL_EXTRA
				 };

/******************************************************************************
*
* sysMv64260IntInit - connect an interrupt handler to the system vector table
*
* This function performs the necessary VxWorks infrastructure
* initialization to allow intConnect(), intEnable(), and intDisable()
* to function.
*
* RETURNS: NA
*/

void sysMv64260IntInit (void)
    {

    UINT32 i;
    UINT32 *p;

    /*
     * Set bit 10 in the Comm Unit Arbiter Control register so that we
     * configure GPP interrupts as level sensitive rather than edge
     * sensitive.
     */

    i = MV64260_READ32 (MV64260_REG_BASE, CU_COMM_UNIT_ARB_CTRL);
    i |= (1<<10);
    MV64260_WRITE32_PUSH (MV64260_REG_BASE, CU_COMM_UNIT_ARB_CTRL, i);

    /* Initialize the interrupt related MV64260 registers */

    p = mv64260IntTbl;
    sysMv64260RegInit (p);

    /* Initialize the interrupt table */

    for (i = 0; i < SYSINT_TABLE_SZ; ++i)
	{
	sysIntTbl[i] = NULL;
	}

    /* Connect the interrupt demultiplexer to the PowerPC external interrupt */

    excIntConnect ((VOIDFUNCPTR *) _EXC_OFF_INTR, sysMv64260IntHandler);

    /* Attach the local routines to the vxWorks system calls */

    _func_intConnectRtn = sysMv64260IntConnect;
    _func_intEnableRtn  = sysMv64260IntEnable;
    _func_intDisableRtn = sysMv64260IntDisable;

    /* Connect the GPP int handler to each of the associated main cause bits */ 

    sysMv64260IntConnect (GPP_INT_VEC_0_7, gppIntHandler, GPP_VAL);
    sysMv64260IntConnect (GPP_INT_VEC_8_15, gppIntHandler, GPP_VAL);
    sysMv64260IntConnect (GPP_INT_VEC_16_23, gppIntHandler, GPP_VAL);
    sysMv64260IntConnect (GPP_INT_VEC_24_31, gppIntHandler, GPP_VAL);

    }

/******************************************************************************
*
* sysMv64260IntConnect - connect an interrupt handler to the system vector table
*
* This function connects an interrupt handler to the system vector table.
*
* RETURNS: OK if connection succeeded, ERROR if connection failed due to
* vector input parameter out of range, memory allocation error or
* intEnable/intDisable functions not being registered for this vector.
*/

LOCAL STATUS sysMv64260IntConnect
    (
    VOIDFUNCPTR	      *vector,	       /* interrupt vector to attach */
    VOIDFUNCPTR	      routine,	       /* routine to be called */
    int		      parameter	       /* parameter to be passed to routine */
    )
    {
    INT_HANDLER_DESC * newHandler;
    INT_HANDLER_DESC * currentHandler;
    UINT32	     theVector =  (UINT32)vector;
    STATUS	     status =  OK;
    UINT32	     i;

    /* Check to see if intEnable() registered for this vector number */

    for (i = 0;;)
	{

	if ( (theVector >= intUtil[i].loIntNum) &&
	     (theVector <= intUtil[i].hiIntNum) )
	    break;		/* Registered - so continue */
	i++;
	if (i >= sizeof(intUtil))
	    return (ERROR); 	/* No registered intEnable() function */
	}

    /*
     * We have intEnable/intDisable registered so we can safely connect
     * this interrupt.
     */

    if ( (theVector > 0) && (theVector < SYSINT_TABLE_SZ) )
	{

	/* Allocate and initialize the new handler */

	newHandler = (INT_HANDLER_DESC *)calloc (1, sizeof (INT_HANDLER_DESC));

	if (newHandler != NULL)
	    {
	    newHandler->vec  = routine;
	    newHandler->arg  = parameter;
	    newHandler->next = NULL;

	    /* Install the handler in the system interrupt table */

	    if (sysIntTbl[theVector] == NULL)
		{

		/* Add first or single handler */

		sysIntTbl[theVector] = newHandler;
		}
	    else
		{

		/* Add subsequent multiple handlers */

		currentHandler = sysIntTbl[theVector];

		while (currentHandler->next != NULL)
		    {
		    currentHandler = currentHandler->next;
		    }

		currentHandler->next = newHandler;
		}
	    }
	else
	    {
	    status = ERROR;
	    }
	}
    else
	{
	status = ERROR;
	}

    if (status == ERROR)
	{
	logMsg ("sysMv64260IntConnect(0x%x,0x%x,0x%x) returned ERROR\r\n",
		(int)vector, (int)routine, parameter,0,0,0);
	}
    return status;
    }

/*******************************************************************************
*
* sysMv64260IntEnable - enable an interrupt level
*
* This is the top-level routine involved in enabling a specified
* interrupt level.  It dispatches the previously registered interrupt
* enabling function.
*
* RETURNS: OK or ERROR if interrupt level not supported
*/

LOCAL int sysMv64260IntEnable
    (
    int intLevel 	/* interrupt level to enable */
    )
    {
    UINT32 i;
    UINT32 intNum = intLevel;

    for (i = 0; ;)
	{
	if ( (intNum >= intUtil[i].loIntNum) &&
	     (intNum <= intUtil[i].hiIntNum) )
	    break;

	i++;
	if (i >= sizeof(intUtil))
	    return (ERROR);	/* No registered intEnable() function */
	}

    return (intUtil[i].intEnableFunc (intNum));
    }

/*******************************************************************************
*
* sysMv64260IntDisable - disable an interrupt level
*
* This is the top-level routine involved in disabling a specified
* interrupt level.  It dispatches the previously registered interrupt
* disabling function.
*
* RETURNS: OK or ERROR if interrupt level not supported
*/

LOCAL int sysMv64260IntDisable
    (
    int intLevel	/* interrupt level to disable */
    )
    {
    UINT32 i;
    UINT32 intNum = intLevel;

    for (i = 0;;)
	{
	if ( (intNum >= intUtil[i].loIntNum) &&
	     (intNum <= intUtil[i].hiIntNum) )
	    break;

	i++;
	if (i >= sizeof(intUtil))
	    return (ERROR);	/* No registered intDisable() function */
	}

    return (intUtil[i].intDisableFunc (intNum));
    }


/******************************************************************************
*
* sysMv64260IntHandler - Field an interrupt from the Mv64260
*
* This routine will field any interrupt from the Mv64260 system controller
* chip.	 It will dispatch a specific device interrupt handler which has
* previously intConnect()ed.
*
* RETURNS: NA
*/

LOCAL void sysMv64260IntHandler (void)
    {

   /* Main cause checking priority order table */

    LOCAL UINT8 mainIntPriority [] = { ICI_MAIN_INT_PRIORITIES };

    UINT8 * p = mainIntPriority;
    UINT32 regCause [2];
    UINT32 intNum;
    UINT32 bitNum;
    UINT32 regNum;
    BOOL   dispatch = FALSE;
    INT_HANDLER_DESC *currentHandler;

    /* 
     * Construct a Low and High cause value by ANDing the low main interrupt
     * cause register with its mask and doing the same with the "high"
     * counterpart.  The two 32-bit values so constructed will have bits
     * set only for active unmasked active interrupts.
     */

    regCause [LO] = MV64260_READ32 (MV64260_REG_BASE, ICI_MAIN_INT_CAUSE_LO) &
		    MV64260_READ32 (MV64260_REG_BASE, ICI_CPU_INT_MASK_LO);

    regCause [HI] = MV64260_READ32 (MV64260_REG_BASE, ICI_MAIN_INT_CAUSE_HI) &
		    MV64260_READ32 (MV64260_REG_BASE, ICI_CPU_INT_MASK_HI);

    /*
     * Examine main cause values in priority order dictated by the priority 
     * table "mainIntPriority".
     */

    while (*p != 0xff)
	{

	/* 
	 * Calculate register number (0 for "low" or 1 for "high") 
	 * and bit number associated with the next interrupt priority .
	 */

	regNum = MAIN_CAUSE_REG_NUM(*p);
	bitNum = MAIN_CAUSE_BIT_NUM(*p);

	/*
	 * Now let's see if the bit in the "cause value" (masked cause
	 * low or high register) is set - meaning that we need to dispatch
	 * an interrupt handler.
         */
 
	if (regCause[regNum] & (1 << bitNum))
	    {

	    /*
	     * We found an unmasked main cause bit set, calculate the
	     * interrupt number (numerically identical to "interrupt level"
	     * and "interrupt vector").
	     */

	    intNum = MAIN_CAUSE_INT_NUM(regNum,bitNum);

#ifdef INCLUDE_WINDVIEW
    	    WV_EVT_INT_ENT(intNum)
#endif

	    /*
	     * Using the calculated interrupt number as an index into our
	     * handler table, call the necessary interrupt handler(s)
	     */

	    currentHandler = sysIntTbl [intNum];
	    dispatch = TRUE;	/* Remember that we have attempted dispatch */

	    if (currentHandler == NULL)
		{

		/* 
		 * We encountered an interrupt for which no intConnect()
		 * has ever been recorded.  There's nothing we can do except
		 * log an error message and move on.
		 */

		logMsg ("Unconnected main interrupt %d\r\n", intNum,
			0,0,0,0,0);
		}
	    else
		{

                /*
                 * We have found a registered intConnect()ed routine for
                 * the interrupt we are analyzing.  Call the connected
                 * interrupt handler associated with this interrupt
                 * vector (level) and any additional handlers which may
                 * be chained together at the same intConnect()ed vector
                 * number.
		 */

		while (currentHandler != NULL)
		    {
		    currentHandler->vec (currentHandler->arg);
		    currentHandler = currentHandler->next;
		    }

		break;

		}

	    }
	p++;		/* Advance to next entry in priority table */
	}

    if (dispatch == FALSE)
	{

	/* We got into this handler with no unmasked main cause bits set */

	logMsg ("Main interrupt with no cause\r\n",0,0,0,0,0,0);

	}
    }

/******************************************************************************
*
* gppIntHandler - Handle GPP interrupt.
*
* This routine is a secondary interrupt handler (called from the primary
* handler, sysMv64260IntHandler()).  It will dispatch an interrupt handler
* which has previously intConnect()ed to one of the GPP interrupts.
*
* RETURNS: NA
*/

LOCAL void gppIntHandler
    (
    int gppIntCause	/* GPP interrupt cause register */
    )
    {

    /* GPP local interrupt priorities */

    INT_HANDLER_DESC * currentHandler;
    LOCAL UINT8	     gppLocalIntPriorities [] = { GPP_LOCAL_INT_PRIORITIES };
    UINT32	     gppCause;
    UINT8	     * p = gppLocalIntPriorities;
    UINT32	     intNum;
    UINT32	     reg;
    BOOL   	     dispatch = FALSE;

    /* 
     * Construct a "gppCause" word with a bit set for each unmasked
     * GPP interrupt.
     */

    gppCause = MV64260_READ32 (MV64260_REG_BASE, gppIntCause) &
	       MV64260_READ32 (MV64260_REG_BASE, GPP_INT_MASK);

    while (*p != 0xff)
	{
	if (gppCause & (1 << *p) )
	    {

	    /* We found a local "value" bit set */

	    dispatch = TRUE;	/* Remember that we have attempted dispatch */

	    intNum = GPP_LOCAL_INT_NUM (*p);

	    /* call the necessary interrupt handlers */

	    currentHandler = sysIntTbl [intNum];

	    if (currentHandler == NULL)
		{
		logMsg ("Unconnected GPP interrupt %d\r\n", intNum,
			0,0,0,0,0);
		}
	    else
		{

		/* Call EACH respective chained interrupt handler */

		while (currentHandler != NULL)
		    {
		    currentHandler->vec (currentHandler->arg);
		    currentHandler = currentHandler->next;
		    }
		}

	    /*
	     * Clear the GPP interrupt cause bit by writing a 0 to 
	     * to the appropriate bit in the GPP interrupt cause 
	     * register.  Make sure not to perturb any of the other
	     * bits.
	     */

	    reg = MV64260_READ32 (MV64260_REG_BASE, gppIntCause);
	    reg &= (~(1 << *p));
	    MV64260_WRITE32_PUSH (MV64260_REG_BASE, gppIntCause, reg);
	    SYNC;

	    /* Continue processing any additional GPP interrupts */

	    }
	p++;		/* Advance to next entry in priority table */
	}

    if (dispatch == FALSE)
	{

	/* We got into this handler with no unmasked main cause bits set */

	logMsg ("GPP interrupt with no cause\r\n",0,0,0,0,0,0);

	}
    }

/******************************************************************************
*
* iciIntEnable - Enable an MV64260 "main cause" interrupt
*
* Based on the input parameter representing "interrupt level", this
* routine turns on (enables) the appropriate bit in the low or high
* "main cause cpu int mask register".  Note that a separate interrupt
* level (numerically equivalent to the corresponding interrupt vector)
* has been reserved for each bit in each of the "interrupt main cause"
* registers.
*
* RETURNS: OK
*/

LOCAL int iciIntEnable
    (
    int intLevel	/* Interrupt level (number) to enable */
    )
    {
    int bitNum;
    int regNum;
    int reg;

    regNum = MAIN_CAUSE_REG_NUM (intLevel);
    bitNum = MAIN_CAUSE_BIT_NUM (intLevel);

    /* turn on the appropriate bit in the low or high CPU INT MASK reg */

    if (regNum == LO)
	{
	reg = MV64260_READ32 (MV64260_REG_BASE, ICI_CPU_INT_MASK_LO);
	reg |= (1 << bitNum);			/* turn on bit */
	MV64260_WRITE32_PUSH (MV64260_REG_BASE, ICI_CPU_INT_MASK_LO, reg);
	}
    else
	{
	reg = MV64260_READ32 (MV64260_REG_BASE, ICI_CPU_INT_MASK_HI);
	reg |= (1 << bitNum);			/* turn on bit */
	MV64260_WRITE32_PUSH (MV64260_REG_BASE, ICI_CPU_INT_MASK_HI,reg);
	}

    return (OK);
    }

/******************************************************************************
*
* iciIntDisable - Disable an MV64260 "main cause" interrupt
*
* Based on the input parameter representing "interrupt level", this
* routine turns off (disables) the appropriate bit in the low or high
* main cause cpu int mask register.  Note that a separate interrupt
* level (numerically equivalent to the corresponding interrupt vector)
* has been reserved for	each bit in each of the	"interrupt main cause"
* registers.
*
*
* RETURNS: OK
*/

LOCAL int iciIntDisable
    (
    int intLevel	/* Interrupt level (number) to enable */
    )
    {
    int bitNum;
    int regNum;
    int reg;

    regNum = MAIN_CAUSE_REG_NUM(intLevel);
    bitNum = MAIN_CAUSE_BIT_NUM(intLevel);

    /* Turn off the appropriate bit in the low or high CPU INT MASK reg */

    if (regNum == LO)
	{
	reg = MV64260_READ32 (MV64260_REG_BASE, ICI_CPU_INT_MASK_LO);
	reg &= ~(1 << bitNum);			/* turn off bit */
	MV64260_WRITE32_PUSH (MV64260_REG_BASE, ICI_CPU_INT_MASK_LO, reg);
	SYNC;
	}
    else
	{
	reg = MV64260_READ32 (MV64260_REG_BASE, ICI_CPU_INT_MASK_HI);
	reg &= ~(1 << bitNum);			/* turn off bit */
	MV64260_WRITE32_PUSH (MV64260_REG_BASE, ICI_CPU_INT_MASK_HI, reg);
	SYNC;
	}
    SYNC;

    return (OK);
    }

/******************************************************************************
*
* gppIntEnable - Enable General Purpose Port Interrupt
*
* Based on the input parameter representing "interrupt level", this
* routine turns on (enables) the appropriate bit in the GPP interrupt
* Mask register.  Note that a separate interrupt level (numerically
* equivalent to the corresponding interrupt vector) has been reserved
* for each bit in the GPP interrupt Mask register.
*/

LOCAL int gppIntEnable
    (
    int intLevel	/* Interrupt level (number) to enable */
    )
    {
    UINT32 bitNum;
    UINT32 reg;

    reg = MV64260_READ32 (MV64260_REG_BASE, GPP_INT_MASK);
    bitNum = GPP_LOCAL_BIT_NUM(intLevel);

    /* Turn on the appropriate bit in the mask register */

    reg |= (1 << bitNum);
    MV64260_WRITE32_PUSH (MV64260_REG_BASE, GPP_INT_MASK, reg);

    return (OK);
    }

/******************************************************************************
*
* gppIntDisable - Disable General Purpose Port Interrupt
*
* Based on the input parameter representing "interrupt level", this
* routine turns off (disables) the appropriate bit in the GPP
* interrupt Mask register.  Note that a separate interrupt level
* (numerically equivalent to the corresponding interrupt vector) has
* been reserved for each bit in the GPP interrupt Mask register.
*/

LOCAL int gppIntDisable
    (
    int intLevel	/* Interrupt level (number) to disable */
    )
    {
    UINT32 bitNum;
    UINT32 reg;

    reg = MV64260_READ32 (MV64260_REG_BASE, GPP_INT_MASK);
    bitNum = GPP_LOCAL_BIT_NUM(intLevel);

    /* Turn off the appropriate bit in the mask register */

    reg &= ~(1 << bitNum);
    MV64260_WRITE32_PUSH (MV64260_REG_BASE, GPP_INT_MASK,reg);

    return (OK);
    }
