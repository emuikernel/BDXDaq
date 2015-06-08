/* sysTempeInt.c - Tempe PCI-X to VME bridge chip interrupt support */

/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,25feb04,scb Fill in TBD stubs with real interrupt handling code.
01a,03sep03,scb Initial writing
*/

/*
DESCRIPTION

This file contains Tempe PCI-X to VME bridge chip interrupt support.
*/

/* includes */

/* defines */

/* typedefs */

/* extern declarations */

IMPORT INT_HANDLER_DESC * sysIntTbl [256];

/* globals */

/* locals */

LOCAL STATUS tempeIntLvlToBit (int intLevel, int *bit);
LOCAL STATUS tempeBitToIntLvl (int bit, int *intLevel);
LOCAL void sysTempeVmeIntr (void);
LOCAL void sysMailboxInt (void);
LOCAL void sysTempeVmeIrqx (int irqNo);

LOCAL FUNCPTR sysMailboxRoutine  = NULL;
LOCAL int sysMailboxArg          = 0;

LOCAL TEMPE_INTLVL_BIT tempeIntToBit [] =
    {
	{ TEMPE_INT_NUM_IRQ1, TEMPE_INT_NUM_IRQ1_BIT },
	{ TEMPE_INT_NUM_IRQ2, TEMPE_INT_NUM_IRQ2_BIT },
	{ TEMPE_INT_NUM_IRQ3, TEMPE_INT_NUM_IRQ3_BIT },
	{ TEMPE_INT_NUM_IRQ4, TEMPE_INT_NUM_IRQ4_BIT },
	{ TEMPE_INT_NUM_IRQ5, TEMPE_INT_NUM_IRQ5_BIT },
	{ TEMPE_INT_NUM_IRQ6, TEMPE_INT_NUM_IRQ6_BIT },
	{ TEMPE_INT_NUM_IRQ7, TEMPE_INT_NUM_IRQ7_BIT },
	{ TEMPE_INT_NUM_ACFL, TEMPE_INT_NUM_ACFL_BIT },
	{ TEMPE_INT_NUM_SYSFL, TEMPE_INT_NUM_SYSFL_BIT },
	{ TEMPE_INT_NUM_IACK, TEMPE_INT_NUM_IACK_BIT },
	{ TEMPE_INT_NUM_VIEEN, TEMPE_INT_NUM_VIEEN_BIT },
	{ TEMPE_INT_NUM_VERR, TEMPE_INT_NUM_VERR_BIT },
	{ TEMPE_INT_NUM_PERR, TEMPE_INT_NUM_PERR_BIT },
	{ TEMPE_INT_NUM_MB0, TEMPE_INT_NUM_MB0_BIT },
	{ TEMPE_INT_NUM_MB1, TEMPE_INT_NUM_MB1_BIT },
	{ TEMPE_INT_NUM_MB2, TEMPE_INT_NUM_MB2_BIT },
	{ TEMPE_INT_NUM_MB3, TEMPE_INT_NUM_MB3_BIT },
	{ TEMPE_INT_NUM_LM0, TEMPE_INT_NUM_LM0_BIT },
	{ TEMPE_INT_NUM_LM1, TEMPE_INT_NUM_LM1_BIT },
	{ TEMPE_INT_NUM_LM2, TEMPE_INT_NUM_LM2_BIT },
	{ TEMPE_INT_NUM_LM3, TEMPE_INT_NUM_LM3_BIT },
	{ TEMPE_INT_NUM_DMA0, TEMPE_INT_NUM_DMA0_BIT },
	{ TEMPE_INT_NUM_DMA1, TEMPE_INT_NUM_DMA1_BIT },
	{ 0xffffffff, 0xffffffff }
    }; 

/* forward declarations */

void sysTempeIntInit (void);
void sysTempeInit (void);
int sysBusIntAck (int intLevel);
STATUS sysBusIntGen (int level, int);

STATUS sysMailboxConnect (FUNCPTR routine, int arg);
STATUS sysMailboxEnable (char *mailboxAdrs);
STATUS sysMailboxDisable (char *mailboxAdrs);
STATUS sysTempeIntEnable (int intLevel);
STATUS sysTempeIntDisable (int intLevel);
STATUS sysIntEnable (int intLevel);
STATUS sysIntDisable (int intLevel);

/*******************************************************************************
*
* tempeIntLvlToBit - Convert a Tempe interrupt level to a bit position.
*
* This routine takes a Tempe PCI-VME bridge interrupt level (the kind used
* in an intConnect() call) and converts it into an associated bit position
* in the interrupt status regsiter.
*
* RETURNS: OK if conversion is possible, ERROR if invalid interrupt level.
*/

LOCAL STATUS tempeIntLvlToBit
    (
    int intLevel,
    int *bit
    )
    {
    int i = 0;
    STATUS retVal = ERROR;

    while (tempeIntToBit[i].intLvl != 0xffffffff)
	{
	if (tempeIntToBit[i].intLvl == intLevel)
	    {
	    *bit = tempeIntToBit[i].bit;
	    retVal = OK;
	    }
	i++;
	}
    return (retVal);
    }

/*******************************************************************************
*
* tempeBitToIntLvl - Convert a Tempe bit position to an interrupt level
*
* This routine takes a Tempe PCI-VME bridge bit position (inside the interrupt
* status regsiter) and converts it into an interrupt level (the kind that
* could be used in an intConnect() call).
*
* RETURNS: OK if conversion is possible, ERROR if invalid bit position.
*/

LOCAL STATUS tempeBitToIntLvl
    (
    int bit,
    int *intLvl
    )
    {
    int i = 0;
    STATUS retVal = ERROR;

    while (tempeIntToBit[i].bit != 0xffffffff)
	{
	if (tempeIntToBit[i].bit == bit)
	    {
	    *intLvl = tempeIntToBit[i].intLvl;
	    retVal = OK;
	    }
	i++;
	}
    return (retVal);
    }

/******************************************************************************
*
* sysTempeVmeIntr - Tempe Interrupt handler
*
* This routine is the the main Tempe interrupt handler.  It will dispatch
* subordinate interrupt handlers associated with the Tempe chip depending
* on which bits are set in the interrupt status register.  The order of
* query is configurable by the user via the #define TEMPE_INT_PRIORITIES.
*
* RETURNS: NA
*/

LOCAL void sysTempeVmeIntr (void)
    {
    INT_HANDLER_DESC * currentHandler;
    UINT32 statReg;
    UINT32 enableReg;
    LOCAL UINT8  tempeIntPriorities [] = { TEMPE_INT_PRIORITIES };
    UINT8 *p = tempeIntPriorities;
    BOOL dispatch = FALSE;
    int	intNum;

    /* Read the status and enable registers, AND the results */

    statReg = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_INTS);
    enableReg   = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_INTEN);
    statReg &= enableReg;
    enableReg   = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_INTEO);
    statReg &= enableReg;	/* Only unmasked int bits remain set */

    while (*p != 0xff)
	{
	if (statReg & (1 << *p))
	    {

	    /* We found a status register bit set */

	    dispatch = TRUE;

	    if (tempeBitToIntLvl (*p, &intNum) == ERROR)
		{
		logMsg ("Unknown Tempe Interrupt Bit: %d\n\r", *p,
			0,0,0,0,0);
		}
	    else
		{

		/* Call EACH respective chained interrupt handler */

	        if ((currentHandler = sysIntTbl [intNum]) == NULL)
		    logMsg ("uninitialized VME interrupt %d\n", intNum, 
                             0,0,0,0,0);

	 	else
		    {
		    while (currentHandler != NULL)
		        {
		        currentHandler->vec (currentHandler->arg);
		        currentHandler = currentHandler->next;
		        }
		    }
		}
	    
	    if (*p >= 8)
	        {

	        /*
	         * Clear the Tempe interrupt by writing a 0 to the appropriate
	         * bit in the Tempe's "Interrupt Clear" register.  Note that
		 * VME IRQ interrupts are not cleared in the Interrupt Clear
		 * register, hence the bit number for a valid clear must be
		 * equal to or greater than 8.
	         */

	        TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_INTC, (1 << (*p)));
	        SYNC;
		}

	    /* Continue processing any additional Tempe Interrupts */

	    }
	p++;	/* Advance to the next entry in priority table */
	}

    if (dispatch == FALSE)
	{

	/* We got into this handler with no unmasked Tempe int bits set */

	logMsg ("VME interrupt with no cause\r\r",0,0,0,0,0,0);
	}
    }

/******************************************************************************
*
* sysMailboxInt - mailbox interrupt handler
*
* This routine calls the installed mailbox routine, if it exists.
*/

LOCAL void sysMailboxInt (void)
    {
    if (sysMailboxRoutine != NULL)
        sysMailboxRoutine (sysMailboxArg);
    }

/******************************************************************************
*
* sysTempeVmeIrqx - Handle IRQ type VME bus interrupt
*
* This routine is invoked by sysTempeVmeIntr because it was intConnected
* previously to each one of the seven IRQxEN interrupts.  It will
* read the appropriate VIACKx register in the Tempe chip to obtain the bus
* vector.  If no IACK error occurs during the read, this vector will be
* used to invoke a user-written handler which previously used intConnect
* to connect to this vector.  If an IACK error occurs, the error is cleared
* and no user handler is called.
*
* RETURNS: NA
*/

LOCAL void sysTempeVmeIrqx 
    (
    int irqNo
    )
    {
    UINT32 viackAddr;
    UINT32 veatReg;
    int	   VMEvector;
    INT_HANDLER_DESC *currHandler;
    
    /* Depending on which IRQ it was, build the VIACKx address to read */

    switch (irqNo)
	{
	case 7:
	    viackAddr = TEMPE_VIACK7 + 3;
	    break;
	case 6:
	    viackAddr = TEMPE_VIACK6 + 3;
	    break;
	case 5:
	    viackAddr = TEMPE_VIACK5 + 3;
	    break;
	case 4:
	    viackAddr = TEMPE_VIACK4 + 3;
	    break;
	case 3:
	    viackAddr = TEMPE_VIACK3 + 3;
	    break;
	case 2:
	    viackAddr = TEMPE_VIACK2 + 3;
	    break;
	case 1:
	    viackAddr = TEMPE_VIACK1 + 3;
	    break;
	default:
	    return;	/* Should never get here */
        }

    /* 
     * The VMEbus exception attributes register can log only one exception
     * at a time.  If a 2nd exception comes before the first is cleared,
     * the 2nd is lost and an overflow indicator is set in this exception
     * register.  To properly detect an IACK exception, we need to clear
     * any pending error which is being held in the VMEbus exception
     * attributes register before attempting to read the IACK register.
     */

    veatReg = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_VEAT);
    if ((veatReg & TEMPE_VEAT_VES_MASK) != 0) /* Did any kind of error occur? */
	sysTempeVeatClear (TRUE);	/* Yes, clear and log the error */

    /* Now do 8-bit read of the appropriate IACK register. */

    VMEvector = *(UINT8 *)(TEMPE_REG_BASE + viackAddr);

    /* Did we raise a VME exception doing the IACK read? */

    if ((veatReg & TEMPE_VEAT_VES_MASK) != 0)
	{

	/* Yes we did have some kind of VME exception, was it IACK ? */

	if ((veatReg & TEMPE_VEAT_IACK_MASK) != 0)
	    VMEvector = 0;	/* It was IACK so make vector 0 (bogus value) */
	sysTempeVeatClear (TRUE);	/* Yes clear the exception */
	}

    if (VMEvector != 0)
	{

	/* We have a valid VME vector, dispatch the appropriate handler */

        if ((currHandler = sysIntTbl[VMEvector]) == NULL)
            logMsg ("uninitialized VME IRQ interrupt %d (0x%x)\n", VMEvector, 
		     VMEvector,0,0,0,0);
        else
            {
            while (currHandler != NULL)
                {
                currHandler->vec (currHandler->arg);
                currHandler = currHandler->next;
                }
            }
	}
    }

/******************************************************************************
*
* sysTempeIntInit - Initialize Tempe interrupt support
*
* This routine is the initialization function called from sysHwInit2().  
* It simply connects the main Tempe interrupt handler as well as the
* VMEbus interrupt handlers.
*
* RETURN: NA
*/

void sysTempeIntInit (void)
    {
    (void) intConnect (INUM_TO_IVEC(TEMPE_INT_LVL), sysTempeVmeIntr, 0);
    (void) intConnect (INUM_TO_IVEC(TEMPE_INT_NUM_IRQ1), sysTempeVmeIrqx, 1);
    (void) intConnect (INUM_TO_IVEC(TEMPE_INT_NUM_IRQ2), sysTempeVmeIrqx, 2);
    (void) intConnect (INUM_TO_IVEC(TEMPE_INT_NUM_IRQ3), sysTempeVmeIrqx, 3);
    (void) intConnect (INUM_TO_IVEC(TEMPE_INT_NUM_IRQ4), sysTempeVmeIrqx, 4);
    (void) intConnect (INUM_TO_IVEC(TEMPE_INT_NUM_IRQ5), sysTempeVmeIrqx, 5);
    (void) intConnect (INUM_TO_IVEC(TEMPE_INT_NUM_IRQ6), sysTempeVmeIrqx, 6);
    (void) intConnect (INUM_TO_IVEC(TEMPE_INT_NUM_IRQ7), sysTempeVmeIrqx, 7);
    }

/*******************************************************************************
*
* sysBusIntAck - acknowledge a bus interrupt
*
* This routine acknowledges a specified VMEbus interrupt level.
*
* NOTE: This routine is included for BSP compliance only.  Since VMEbus
* interrupts are re-enabled in the interrupt handler, and acknowledged
* automatically by hardware, this routine is a no-op.
*
* RETURNS: NULL.
*
* SEE ALSO: sysBusIntGen()
*/

int sysBusIntAck
    (
    int intLevel ARG_UNUSED	/* interrupt level to acknowledge */
    )
    {
    return (0);
    }

/*******************************************************************************
*
* sysBusIntGen - generate a bus interrupt
*
* This routine generates a VMEbus interrupt for a specified level with a
* specified vector.  Only one VME interrupt can be generated at a time and
* none can be generated if a previously generated VME interrupt has not been
* acknowledged, i.e., if no VME bus IACK cycle has completed.
*
* RETURNS: OK, or ERROR if <level> or <vector> are out of range or if an
* interrupt is already in progress.
*
* SEE ALSO: sysBusIntAck()
*/

STATUS sysBusIntGen
    (
    int level,          /* interrupt level to generate (1 - 7) */
    int vector          /* interrupt vector for interrupt (0 - 255) */
    )
    {
    UINT32 vicrReg;
    int    lockKey;

    /* Validate interrupt level */

    if ( ((level < 1) || (level > 7)) || ((vector < 0) || (vector > 255)) )
        return (ERROR);

    /* lock interrupts so there will be no interference */

    lockKey = intLock ();

    /* 
     * We cannot generate an interrupt if one is pending.  We will check
     * for this condition, returning ERROR if an interrupt is pending.
     */

    vicrReg = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_VICR);
    if ((vicrReg & TEMPE_VICR_IRQS_MASK) != 0)
        {
        intUnlock (lockKey);
        return (ERROR);
        }

    /*
     * It's safe now to generate the interrupt.  It is a two step process.
     * First we will store the vector number and then we will write the
     * VMEbus IRQ level to actually generate the interrupt.
     */

    vicrReg &= ~TEMPE_VICR_STID_MASK;
    vicrReg |= vector;
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_VICR, vicrReg);
    vicrReg |= (level << TEMPE_VICR_IRQL_BIT);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_VICR, vicrReg);

    /* unlock the interrupt */

    intUnlock (lockKey);

    return (OK);
    }

/*******************************************************************************
*
* sysMailboxConnect - connect a routine to the mailbox interrupt
*
* This routine specifies the interrupt service routine to be called at each
* mailbox interrupt.
*
* NOTE: The mailbox interrupt is the mailbox 0 interrupt on the
* Tempe chip.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysMailboxEnable()
*/

STATUS sysMailboxConnect
    (
    FUNCPTR routine,    /* routine called at each mailbox interrupt */
    int arg             /* argument with which to call routine      */
    )
    {
    static BOOL sysMailboxConnected = FALSE;

    if (!sysMailboxConnected &&
        intConnect (INUM_TO_IVEC (TEMPE_INT_NUM_MB0),
                    sysMailboxInt, 0) == ERROR)
        {
        return (ERROR);
        }

    sysMailboxConnected = TRUE;
    sysMailboxRoutine   = routine;
    sysMailboxArg       = arg;

    return (OK);
    }

/*******************************************************************************
*
* sysMailboxEnable - enable the mailbox interrupt
*
* This routine enables the mailbox interrupt.
*
* NOTE: The mailbox interrupt is the mailbox 0 interrupt on the
* Tempe chip.
*
* RETURNS: OK, if mailbox interrupt becomes enabled, else ERROR.
*
* SEE ALSO: sysMailboxConnect(), sysMailboxDisable()
*/

STATUS sysMailboxEnable
    (
    char *mailboxAdrs ARG_UNUSED	/* address of mailbox (ignored) */
    )
    {

    /* enable the mailbox 0 interrupt */

    if (sysTempeIntEnable (TEMPE_INT_NUM_MB0) == OK)
        return (OK);
    else
	return (ERROR);
    }

/*******************************************************************************
*
* sysMailboxDisable - disable the mailbox interrupt
*
* This routine disables the mailbox interrupt.
*
* NOTE: The mailbox interrupt is the mailbox 0 interrupt on the
* Tempe chip.
*
* RETURNS: OK, if the mailbox interrupt gets disabled, else ERROR
*
* SEE ALSO: sysMailboxConnect(), sysMailboxEnable()
*/

STATUS sysMailboxDisable
    (
    char *mailboxAdrs ARG_UNUSED	/* address of mailbox (ignored) */
    )
    {

    /* disable the mailbox interrupt */

    if (sysTempeIntDisable (TEMPE_INT_NUM_MB0) == OK)
        return (OK);
    else
        return (ERROR);
    }

/******************************************************************************
*
* sysTempeIntEnable - Enable a specific Tempe interrupt
*
* This routine will enable a specific Tempe interrupt associated with
* an intLevel input parameter.  This input parameter is one which would
* be used in an intConnect() call.
*
* RETURNS: OK if enabling is possible, ERROR if invalid input parameter. 
*/

STATUS sysTempeIntEnable 
    (
    int intLevel
    )
    {
    int bit;
    UINT32 reg;

    /* Convert interrupt level to "bit" position in "enable register" */

    if (tempeIntLvlToBit (intLevel, &bit) == ERROR)
	return (ERROR);

    reg = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_INTEO); 
    reg |= (1<<bit);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_INTEO, reg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_INTEN, reg);

    return (OK);
    }

/******************************************************************************
*
* sysTempeIntDisable - Disable a specific Tempe interrupt
*
* This routine will disable a specific Tempe interrupt associated with
* an intLevel input parameter.  This input parameter is one which would
* be used in an intConnect() call.
*
* RETURNS: OK if enabling is possible, ERROR if invalid input parameter. 
*/

STATUS sysTempeIntDisable 
    (
    int intLevel
    )
    {
    int bit;
    UINT32 reg;

    /* Convert interrupt level to "bit" position in "enable register" */

    if (tempeIntLvlToBit (intLevel, &bit) == ERROR)
	return (ERROR);

    reg = TEMPE_READ32 (TEMPE_REG_BASE, TEMPE_INTEO); 
    reg &= ~(1<<bit);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_INTEO, reg);
    TEMPE_WRITE32_PUSH (TEMPE_REG_BASE, TEMPE_INTEN, reg);

    return (OK);
    }

/*******************************************************************************
*
* sysIntEnable - enable a bus interrupt level
*
* This routine enables reception of a specified VMEbus interrupt level.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range 1 - 7.
*
* SEE ALSO: sysIntDisable()
*/

STATUS sysIntEnable
    (
    int intLevel        /* interrupt level to enable (1-7) */
    )
    {
    int level;

    if (intLevel < 1 || intLevel > 7)
        return (ERROR);

    /*
     * We will rely on the quirky fact that the interrupt Level is
     * numerically identical to the bit position on the interrupt
     * related registers of the Tempe chip.
     */

    (void)tempeBitToIntLvl (intLevel, &level);
    sysTempeIntEnable (level);

    return (OK);
    }

/*******************************************************************************
*
* sysIntDisable - disable a bus interrupt level
*
* This routine disables reception of a specified VMEbus interrupt level.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range 1 - 7.
*
* SEE ALSO: sysIntEnable()
*/

STATUS sysIntDisable
    (
    int intLevel        /* interrupt level to disable (1-7) */
    )
    {
    int level;

    if (intLevel < 1 || intLevel > 7)
        return (ERROR);

    /*
     * We will rely on the quirky fact that the interrupt Level is
     * numerically identical to the bit position on the interrupt
     * related registers of the Tempe chip.
     */

    (void)tempeBitToIntLvl (intLevel, &level);

    sysTempeIntDisable (level);

    return (OK);
    }
