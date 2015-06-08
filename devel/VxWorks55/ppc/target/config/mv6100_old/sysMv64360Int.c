/* sysMv64360Int.c - Interrupt controller support functions */

/* Copyright 2002-2004 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01p,20apr04,scb Protect against calling NULL function (intEnable, intDisable).
01o,25feb04,scb Add intConnectShow() routine, bug fix in intUtil table scan.
01n,09feb04,cak Created MPP_CTRLx_VAL definitions in mv6100A.h to 
		replace the "hard-coding" in this file.
01m,17dec03,cak BSP update.
01l,04mar03,cak Added "break" to sysMv64360IntHandler() to fix the
		"GPP interrupt with no cause" problem. 
01k,08oct01,scb Fix sysMv64360IntHandler() to prevent "interrupt with no cause"
01j,26aug02,cak Modifications made to fix interrupt handling during board
		bring-up, including register initialization and clearing of
		GPP interrupts.
01i,21aug02,scb Propagate "interrupt" code review fixes from hxeb100.
01h,20aug02,scb Fix botched clearing of GPP interrupt.
01g,20aug02,scb Fix botched MPP_CTRL3 programming.
01f,20aug02,scb Mv64620 testing on hxeb100 prompted fixes to this file.
01e,15aug02,scb Fix botched MV64360_READ32 macro call.
01d,09jul02,scb Clear GPP interrupt cause bit on handling GPP interrupts.
01c,26jun02,scb Register mnemonic name changes.
01b,26jun02,scb Fix register mnemonic faux pas.
01a,10apr02,scb Original writing.
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

    YES_INIT_MV64360_REG ( MPP_CTRL0, MPP_CTRL0_VAL) 

    YES_INIT_MV64360_REG ( MPP_CTRL1, MPP_CTRL1_VAL) 

    YES_INIT_MV64360_REG ( MPP_CTRL2, MPP_CTRL2_VAL) 

    YES_INIT_MV64360_REG ( MPP_CTRL3, MPP_CTRL3_VAL) 

    YES_INIT_MV64360_REG ( GPP_IO_CTRL, (GPP_IO0 << 0)   | (GPP_IO1  << 1)  | \
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

    YES_INIT_MV64360_REG ( GPP_LEVEL_CTRL, (GPP_LEVEL0 << 0) | \
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

    YES_INIT_MV64360_REG ( CUI_ARB_CTRL			  , 0x11FF0400 )
    NOT_INIT_MV64360_REG ( GPP_VAL			  , readOnly )
    YES_INIT_MV64360_REG ( GPP_INTERRUPT_CAUSE		  , 0 )
    YES_INIT_MV64360_REG ( GPP_INTERRUPT_MASK0		  , MASK_ALL )
    YES_INIT_MV64360_REG ( GPP_INTERRUPT_MASK1		  , MASK_ALL )
    NOT_INIT_MV64360_REG ( GPP_VAL_SET			  , readOnly )
    NOT_INIT_MV64360_REG ( GPP_VAL_CLEAR		  , readOnly )

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
void  intConnectShow (void);

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
	if (i >= (sizeof(intUtil))/(sizeof(INT_UTIL_DESC)))
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
	if (i >= (sizeof(intUtil))/(sizeof(INT_UTIL_DESC)))
	    return (ERROR);	/* No registered intEnable() function */
	}

    if (intUtil[i].intEnableFunc != NULL)
        return (intUtil[i].intEnableFunc (intNum));
    else
	return (ERROR);
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
	if (i >= (sizeof(intUtil))/(sizeof(INT_UTIL_DESC)))
	    return (ERROR);	/* No registered intDisable() function */
	}

    if (intUtil[i].intDisableFunc != NULL)
        return (intUtil[i].intDisableFunc (intNum));
    else
	return (ERROR);
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

#ifdef INCLUDE_SHOW_ROUTINES

/******************************************************************************
*
* intConnectShow - Show all intConnect()ions
*
* This routine scans the 256-entry sysIntTbl[] vector table.  For each
* vector which has been intConnect()ed, a line is displayed showing the
* vector number and handler(arg) associated with the previously executed
* intConnect().  If there is an interrupt chain then each item is the
* chain is shown on that line as well.
* 
* RETURNS: NA
*/

void intConnectShow (void)
    {
    IMPORT INT_HANDLER_DESC * sysIntTbl [256];
    INT_HANDLER_DESC * handler;
    int i;
     
    printf ("\n   Vector :  Handler  (Arg)\n\n");
    for (i=0; i<=255; i++)
        {
	if ((handler = sysIntTbl[i]) != NULL)
	    {
	    printf ("%3d (0x%02x):  0x%x (0x%x)\n", 
		     i,i, (UINT32)handler->vec, handler->arg);
	    handler = handler->next;
	    while (handler != NULL)
		{
		printf("             0x%x (0x%x) - chained\n",
			(UINT32)handler->vec, handler->arg);
		handler = handler->next; 
		}
	    }
        }
    printf("\n");
    }
#endif /* INCLUDE_SHOW_ROUTINES */
