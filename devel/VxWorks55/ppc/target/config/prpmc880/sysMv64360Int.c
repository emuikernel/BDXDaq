/* sysMv64360Int.c - Interrupt controller support functions */

/* Copyright 2003 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01a,23may03,simon  Ported (from version 01a of mcp905/sysMv64360Int.c).
*/

/*
DESCRIPTION
This module implements the Mv64360 interrupt handling support functions.
*/

/* includes */

#include "mv64360.h"
#include "mv64360Int.h"

/* defines */

#define	SYSINT_TABLE_SZ		256

#define	LOW			0
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
 * values for the MV64360 system controller chip.
 */

LOCAL UINT32 mv64360IntTbl [] =
    {
    NOT_INIT_MV64360_REG ( ICI_MAIN_INTERRUPT_CAUSE_LOW	  , readOnly )
    NOT_INIT_MV64360_REG ( ICI_MAIN_INTERRUPT_CAUSE_HI	  , readOnly )

    YES_INIT_MV64360_REG ( ICI_CPU_INT0_MASK_LOW	  , MASK_ALL )
    YES_INIT_MV64360_REG ( ICI_CPU_INT0_MASK_HI		  , MASK_ALL )
    NOT_INIT_MV64360_REG ( ICI_CPU_INT0_SELECT_CAUSE	  , readOnly )

    YES_INIT_MV64360_REG ( ICI_CPU_INT1_MASK_LOW	  , MASK_ALL )
    YES_INIT_MV64360_REG ( ICI_CPU_INT1_MASK_HI		  , MASK_ALL )
    NOT_INIT_MV64360_REG ( ICI_CPU_INT1_SELECT_CAUSE	  , readOnly )

    NOT_INIT_MV64360_REG ( ICI_INT0_MASK_LOW		  , MASK_ALL )
    NOT_INIT_MV64360_REG ( ICI_INT0_MASK_HI		  , MASK_ALL )
    NOT_INIT_MV64360_REG ( ICI_INT0_SELECT_CAUSE	  , readOnly )
    NOT_INIT_MV64360_REG ( ICI_INT1_MASK_LOW		  , MASK_ALL )
    NOT_INIT_MV64360_REG ( ICI_INT1_MASK_HI		  , MASK_ALL )
    NOT_INIT_MV64360_REG ( ICI_INT1_SELECT_CAUSE	  , readOnly )

    END_INIT_MV64360_REG
    };

/* forward declarations	*/

LOCAL STATUS  sysMv64360IntConnect (VOIDFUNCPTR	*vector,
				    VOIDFUNCPTR routine, int parameter);

void  sysMv64360IntInit	   (void);
LOCAL int     sysMv64360IntEnable  (int);
LOCAL int     sysMv64360IntDisable (int);
LOCAL int     iciIntEnable	   (int);
LOCAL int     iciIntDisable	   (int);
LOCAL int     gppIntEnable	   (int);
LOCAL int     gppIntDisable	   (int);

LOCAL void    sysMv64360IntHandler (void);
LOCAL void    gppIntHandler	   (int);

/* Local xxxintEnable()/xxxintDisable() functions */

LOCAL INT_UTIL_DESC intUtil [] = {
				     { INT_UTIL_ICI },
				     { INT_UTIL_GPP }
				       INT_UTIL_EXTRA
				 };

/******************************************************************************
*
* sysMv64360IntInit - connect an interrupt handler to the system vector table
*
* This function performs the necessary VxWorks infrastructure
* initialization to allow intConnect(), intEnable(), and intDisable()
* to function.
*
* RETURNS: NA
*/

void sysMv64360IntInit (void)
    {

    UINT32 i;
    UINT32 *p;

    /* Initialize the interrupt related MV64360 registers */

    p = mv64360IntTbl;
    sysMv64360RegInit (p);

    /* Initialize the interrupt table */

    for (i = 0; i < SYSINT_TABLE_SZ; ++i)
	{
	sysIntTbl[i] = NULL;
	}

    /* Connect the interrupt demultiplexer to the PowerPC external interrupt */

    excIntConnect ((VOIDFUNCPTR *) _EXC_OFF_INTR, sysMv64360IntHandler);

    /* Attach the local routines to the vxWorks system calls */

    _func_intConnectRtn = sysMv64360IntConnect;
    _func_intEnableRtn  = sysMv64360IntEnable;
    _func_intDisableRtn = sysMv64360IntDisable;

    /* Connect the GPP int handler to each of the associated main cause bits */ 

    sysMv64360IntConnect (GPP_INT_VEC_0_7, gppIntHandler, GPP_VAL);
    sysMv64360IntConnect (GPP_INT_VEC_8_15, gppIntHandler, GPP_VAL);
    sysMv64360IntConnect (GPP_INT_VEC_16_23, gppIntHandler, GPP_VAL);
    sysMv64360IntConnect (GPP_INT_VEC_24_31, gppIntHandler, GPP_VAL);
    }

/******************************************************************************
*
* sysMv64360IntConnect - connect an interrupt handler to the system vector table
*
* This function connects an interrupt handler to the system vector table.
*
* RETURNS: OK if connection succeeded, ERROR if connection failed due to
* vector input parameter out of range, memory allocation error or
* intEnable/intDisable functions not being registered for this vector.
*/

LOCAL STATUS sysMv64360IntConnect
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
	logMsg ("sysMv64360IntConnect(0x%x,0x%x,0x%x) returned ERROR\r\n",
		(int)vector, (int)routine, parameter,0,0,0);
	}
    return status;
    }

/*******************************************************************************
*
* sysMv64360IntEnable - enable an interrupt level
*
* This is the top-level routine involved in enabling a specified
* interrupt level.  It dispatches the previously registered interrupt
* enabling function.
*
* RETURNS: OK or ERROR if interrupt level not supported
*/

LOCAL int sysMv64360IntEnable
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
* sysMv64360IntDisable - disable an interrupt level
*
* This is the top-level routine involved in disabling a specified
* interrupt level.  It dispatches the previously registered interrupt
* disabling function.
*
* RETURNS: OK or ERROR if interrupt level not supported
*/

LOCAL int sysMv64360IntDisable
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
* sysMv64360IntHandler - Field an interrupt from the Mv64360
*
* This routine will field any interrupt from the Mv64360 system controller
* chip.	 It will dispatch a specific device interrupt handler which has
* previously intConnect()ed.
*
* RETURNS: NA
*/
LOCAL void sysMv64360IntHandler (void)
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

    regCause [LOW] = MV64360_READ32 (MV64360_REG_BASE, 
                                     ICI_MAIN_INTERRUPT_CAUSE_LOW) &
                     MV64360_READ32 (MV64360_REG_BASE, ICI_CPU_INT0_MASK_LOW);

    regCause [HI] =  MV64360_READ32 (MV64360_REG_BASE, 
                                     ICI_MAIN_INTERRUPT_CAUSE_HI) &
                     MV64360_READ32 (MV64360_REG_BASE, ICI_CPU_INT0_MASK_HI);

    /*
     * Examine main cause values in priority order dictated by the priority 
     * table mainIntPriority.
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
* handler, sysMv64360IntHandler()).  It will dispatch an interrupt handler
* which has previously intConnect()ed to one of the GPP interrupts.
*
* RETURNS: NA
*/

LOCAL void gppIntHandler
    (
    int gppVal	/* GPP value register */
    )
    {

    /* GPP local interrupt priorities */

    INT_HANDLER_DESC * currentHandler;
    LOCAL UINT8	     gppLocalIntPriorities [] = { GPP_LOCAL_INT_PRIORITIES };
    UINT32	     gppCause;
    UINT32	     gppIntCause;
    UINT8	     * p = gppLocalIntPriorities;
    UINT32	     intNum;
    BOOL   	     dispatch = FALSE;

    /* 
     * Construct a "gppCause" word with a bit set for each unmasked
     * GPP interrupt.
     */

    gppCause = MV64360_READ32 (MV64360_REG_BASE, gppVal) &
               MV64360_READ32 (MV64360_REG_BASE, GPP_INTERRUPT_MASK0);

    while (*p != 0xff)
	{
	if (gppCause & (1 << *p))
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
	     * Clear the GPP interrupt by writing a 0 to the appropriate
	     * bit in the GPP interrupt cause register.
	     */

	    gppIntCause = MV64360_READ32(MV64360_REG_BASE, GPP_INTERRUPT_CAUSE);
	    MV64360_WRITE32_PUSH(MV64360_REG_BASE, GPP_INTERRUPT_CAUSE, 
				 (gppIntCause & (~(1 << *p))));
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
* iciIntEnable - Enable an MV64360 "main cause" interrupt
*
* Based on the input parameter representing "interrupt level", this
* routine turns on (enables) the appropriate bit in the low or high
* "main cause cpu int0 mask register".  Note that a separate interrupt
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

    /* turn on the appropriate bit in the low or high CPU INT0 MASK reg */

    if (regNum == LOW)
	{
	reg = MV64360_READ32 (MV64360_REG_BASE, ICI_CPU_INT0_MASK_LOW);
	reg |= (1 << bitNum);			/* turn on bit */
	MV64360_WRITE32_PUSH (MV64360_REG_BASE, ICI_CPU_INT0_MASK_LOW, reg);
	}
    else
	{
	reg = MV64360_READ32 (MV64360_REG_BASE, ICI_CPU_INT0_MASK_HI);
	reg |= (1 << bitNum);			/* turn on bit */
	MV64360_WRITE32_PUSH (MV64360_REG_BASE, ICI_CPU_INT0_MASK_HI,reg);
	}

    return (OK);
    }

/******************************************************************************
*
* iciIntDisable - Disable an MV64360 "main cause" interrupt
*
* Based on the input parameter representing "interrupt level", this
* routine turns off (disables) the appropriate bit in the low or high
* main cause cpu int0 mask register.  Note that a separate interrupt
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

    /* Turn off the appropriate bit in the low or high CPU INT0 MASK reg */

    if (regNum == LOW)
	{
	reg = MV64360_READ32 (MV64360_REG_BASE, ICI_CPU_INT0_MASK_LOW);
	reg &= ~(1 << bitNum);			/* turn off bit */
	MV64360_WRITE32_PUSH (MV64360_REG_BASE, ICI_CPU_INT0_MASK_LOW, reg);
	SYNC;
	}
    else
	{
	reg = MV64360_READ32 (MV64360_REG_BASE, ICI_CPU_INT0_MASK_HI);
	reg &= ~(1 << bitNum);			/* turn off bit */
	MV64360_WRITE32_PUSH (MV64360_REG_BASE, ICI_CPU_INT0_MASK_HI, reg);
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
* Mask 0 register.  Note that a separate interrupt level (numerically
* equivalent to the corresponding interrupt vector) has been reserved
* for each bit in the GPP interrupt Mask 0 register.
*/

LOCAL int gppIntEnable
    (
    int intLevel	/* Interrupt level (number) to enable */
    )
    {
    UINT32 bitNum;
    UINT32 reg;

    reg = MV64360_READ32 (MV64360_REG_BASE, GPP_INTERRUPT_MASK0);
    bitNum = GPP_LOCAL_BIT_NUM(intLevel);

    /* Turn on the appropriate bit in the mask register */

    reg |= (1 << bitNum);
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, GPP_INTERRUPT_MASK0, reg);

    return (OK);
    }

/******************************************************************************
*
* gppIntDisable - Disable General Purpose Port Interrupt
*
* Based on the input parameter representing "interrupt level", this
* routine turns off (disables) the appropriate bit in the GPP
* interrupt Mask 0 register.  Note that a separate interrupt level
* (numerically equivalent to the corresponding interrupt vector) has
* been reserved for each bit in the GPP interrupt Mask 0 register.
*/

LOCAL int gppIntDisable
    (
    int intLevel	/* Interrupt level (number) to disable */
    )
    {
    UINT32 bitNum;
    UINT32 reg;

    reg = MV64360_READ32 (MV64360_REG_BASE, GPP_INTERRUPT_MASK0);
    bitNum = GPP_LOCAL_BIT_NUM(intLevel);

    /* Turn off the appropriate bit in the mask register */

    reg &= ~(1 << bitNum);
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, GPP_INTERRUPT_MASK0,reg);

    return (OK);
    }
